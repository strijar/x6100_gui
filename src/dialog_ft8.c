/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#include "lvgl/lvgl.h"
#include "dialog.h"
#include "dialog_ft8.h"
#include "styles.h"
#include "params.h"
#include "radio.h"
#include "audio.h"

#include "ft8/unpack.h"
#include "ft8/ldpc.h"
#include "ft8/decode.h"
#include "ft8/constants.h"
#include "ft8/encode.h"
#include "ft8/crc.h"

#define DECIM           4
#define SAMPLE_RATE     (AUDIO_CAPTURE_RATE / DECIM)

#define MIN_SCORE       10
#define MAX_CANDIDATES  120
#define LDPC_ITER       20
#define MAX_DECODED     50
#define FREQ_OSR        2
#define TIME_OSR        2

typedef enum {
    RX_IDLE = 0,
    RX_PROCESS,
} rx_state_t;

static lv_obj_t             *dialog;
static ft8_state_t          state = FT8_OFF;
static rx_state_t           rx_state = RX_IDLE;

static pthread_cond_t       audio_cond;
static pthread_mutex_t      audio_mutex;
static cbuffercf            audio_buf;

static firdecim_crcf        decim;
static float complex        *decim_buf;
static complex float        *rx_window = NULL;
static complex float        *time_buf;
static complex float        *freq_buf;
static complex float        *last_frame;
static fftplan              fft;

static ftx_protocol_t       protocol = PROTO_FT8;
static float                symbol_period;
static uint32_t             block_size;
static uint32_t             subblock_size;
static uint16_t             nfft;
static float                fft_norm;
static waterfall_t          wf;

static candidate_t          candidate_list[MAX_CANDIDATES];
static message_t            decoded[MAX_DECODED];
static message_t*           decoded_hashtable[MAX_DECODED];

static void init() {
    float   slot_time;
    
    switch (protocol) {
        case PROTO_FT4:
            slot_time = FT4_SLOT_TIME;
            symbol_period = FT4_SYMBOL_PERIOD;
            break;
            
        case PROTO_FT8:
            slot_time = FT8_SLOT_TIME;
            symbol_period = FT8_SYMBOL_PERIOD;
            break;
    }
    
    block_size = SAMPLE_RATE * symbol_period;
    subblock_size = block_size / TIME_OSR;
    nfft = block_size * FREQ_OSR;
    fft_norm = 2.0f / nfft;
    
    last_frame = (complex float *) malloc(nfft * sizeof(complex float));

    const uint32_t max_blocks = slot_time / symbol_period;
    const uint32_t num_bins = SAMPLE_RATE * symbol_period / 2;

    size_t mag_size = max_blocks * TIME_OSR * FREQ_OSR * num_bins * sizeof(uint8_t);
    
    wf.max_blocks = max_blocks;
    wf.num_bins = num_bins;
    wf.time_osr = TIME_OSR;
    wf.freq_osr = FREQ_OSR;
    wf.block_stride = TIME_OSR * FREQ_OSR * num_bins;
    wf.mag = (uint8_t *) malloc(mag_size);
    wf.protocol = protocol;
}

static void reset() {
    wf.num_blocks = 0;
    rx_state = RX_IDLE;
}

static void done() {
    state = FT8_OFF;
}

static void decode() {
    uint16_t    num_candidates = ft8_find_sync(&wf, MAX_CANDIDATES, candidate_list, MIN_SCORE);
    uint16_t    num_decoded = 0;
    
    for (uint16_t i = 0; i < MAX_DECODED; i++)
        decoded_hashtable[i] = NULL;
        
    for (uint16_t idx = 0; idx < num_candidates; idx++) {
        const candidate_t *cand = &candidate_list[idx];
        
        if (cand->score < MIN_SCORE)
            continue;
            
        float freq_hz = (cand->freq_offset + (float) cand->freq_sub / wf.freq_osr) / symbol_period;
        float time_sec = (cand->time_offset + (float) cand->time_sub / wf.time_osr) * symbol_period;
        
        message_t       message;
        decode_status_t status;
        
        if (!ft8_decode(&wf, cand, &message, LDPC_ITER, &status)) {
            continue;
        }
        
        uint16_t    idx_hash = message.hash % MAX_DECODED;
        bool        found_empty_slot = false;
        bool        found_duplicate = false;
        
        do {
            if (decoded_hashtable[idx_hash] == NULL) {
                found_empty_slot = true;
            } else if (decoded_hashtable[idx_hash]->hash == message.hash && strcmp(decoded_hashtable[idx_hash]->text, message.text) == 0) {
                found_duplicate = true;
            } else {
                idx_hash = (idx_hash + 1) % MAX_DECODED;
            }
        } while (!found_empty_slot && !found_duplicate);
        
        if (found_empty_slot) {
            memcpy(&decoded[idx_hash], &message, sizeof(message));
            decoded_hashtable[idx_hash] = &decoded[idx_hash];
            
            num_decoded++;
            LV_LOG_INFO("%3d %+4.2f %4.0f | %s", cand->score, time_sec, freq_hz, message.text);
        }
    }
}

void static process(float complex *frame) {
    int offset = wf.num_blocks * wf.block_stride;
    int frame_pos = 0;
    
    for (int time_sub = 0; time_sub < wf.time_osr; time_sub++) {
        for (int pos = 0; pos < nfft - subblock_size; pos++)
            last_frame[pos] = last_frame[pos + subblock_size];
            
        for (int pos = nfft - subblock_size; pos < nfft; pos++) {
            last_frame[pos] = frame[frame_pos];
            frame_pos++;
        }
        
        for (uint32_t pos = 0; pos < nfft; pos++)
            time_buf[pos] = rx_window[pos] * last_frame[pos];

        fft_execute(fft);
                
        for (int freq_sub = 0; freq_sub < wf.freq_osr; freq_sub++)
            for (int bin = 0; bin < wf.num_bins; bin++) {
                int             src_bin = (bin * wf.freq_osr) + freq_sub;
                complex float   freq = freq_buf[src_bin];
                float           v = crealf(freq * conjf(freq));
                float           db = 10.0f * log10f(v);
                int             scaled = (int16_t) ((db + 5.0f) * 10);
                
                if (scaled < 0) {
                    scaled = 0;
                } else if (scaled > 255) {
                    scaled = 255;
                }

                wf.mag[offset] = scaled;
                offset++;
            }
    }
    
    wf.num_blocks++;
}

static void * decode_thread(void *arg) {
    unsigned int    n;
    float complex   *buf;
    const size_t    size = block_size * DECIM;
    struct tm       *t;
    time_t          now;

    while (state != FT8_OFF) {
        pthread_mutex_lock(&audio_mutex);

        while (cbuffercf_size(audio_buf) < size) {
            pthread_cond_wait(&audio_cond, &audio_mutex);
        }
        
        pthread_mutex_unlock(&audio_mutex);

        now = time(NULL);
        t = localtime(&now);
            
        while (cbuffercf_size(audio_buf) > size) {
            cbuffercf_read(audio_buf, size, &buf, &n);
    
            firdecim_crcf_execute_block(decim, buf, block_size, decim_buf);
            cbuffercf_release(audio_buf, size);

            if (rx_state == RX_IDLE) {
                if (t->tm_sec % 15 == 0) {
                    rx_state = RX_PROCESS;
                }
            }
        
            if (rx_state == RX_PROCESS) {
                process(decim_buf);
        
                if (wf.num_blocks >= wf.max_blocks) {
                    decode();
                    reset();

                    rx_state = RX_IDLE;
               }
            }
        }
    }
}

static void deleted_cb(lv_event_t * e) {
    done();
}

lv_obj_t * dialog_ft8(lv_obj_t *parent) {
    dialog = dialog_init(parent);

    lv_obj_add_event_cb(dialog, deleted_cb, LV_EVENT_DELETE, NULL);

    decim = firdecim_crcf_create_kaiser(DECIM, 16, 40.0f);

    init();
    reset();

    audio_buf = cbuffercf_create(AUDIO_CAPTURE_RATE);
    decim_buf = (float complex *) malloc(block_size * sizeof(float complex));
    time_buf = (float complex*) malloc(nfft * sizeof(float complex));
    freq_buf = (float complex*) malloc(nfft * sizeof(float complex));
    fft = fft_create_plan(nfft, time_buf, freq_buf, LIQUID_FFT_FORWARD, 0);

    rx_window = malloc(nfft * sizeof(complex float));

    for (uint16_t i = 0; i < nfft; i++)
        rx_window[i] = liquid_hann(i, nfft);

    float gain = 0.0f;

    for (uint16_t i = 0; i < nfft; i++)
        gain += rx_window[i] * rx_window[i];
        
    gain = 1.0f / sqrtf(gain);

    for (uint16_t i = 0; i < nfft; i++)
        rx_window[i] *= gain;
        
    pthread_mutex_init(&audio_mutex, NULL);
    pthread_cond_init(&audio_cond, NULL);

    pthread_t thread;

    pthread_create(&thread, NULL, decode_thread, NULL);
    pthread_detach(thread);
    
    state = FT8_RX;

    return dialog;
}

ft8_state_t dialog_ft8_get_state() {
    return state;
}

void dialog_ft8_put_audio_samples(unsigned int n, float complex *samples) {
    pthread_mutex_lock(&audio_mutex);
    cbuffercf_write(audio_buf, samples, n);
    pthread_cond_broadcast(&audio_cond);
    pthread_mutex_unlock(&audio_mutex);
}
