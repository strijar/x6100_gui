/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <math.h>
#include "lvgl/lvgl.h"

#include "cw.h"
#include "audio.h"
#include "util.h"
#include "params.h"
#include "cw_decoder.h"

typedef struct {
    uint16_t    n;
    float       db;
} fft_item_t;

const static uint16_t   fft = 1024;
const static uint16_t   fft_over = fft / 8;
const static uint16_t   fft_all = fft + fft_over;

static bool             ready = false;

static fft_item_t       fft_items[512];
static float complex    *fft_window;
static float complex    *window_buf;

static cbuffercf        audio_buf;
static spgramcf         audio_sg;
static float            *audio_psd;
static float            *audio_psd_filtered;

static float            peak_filtered;
static float            noise_filtered;

void cw_init() {
    audio_buf = cbuffercf_create(fft_all * 10);

    audio_sg = spgramcf_create(fft, LIQUID_WINDOW_HANN, fft, fft);
    audio_psd = (float *) malloc(fft * sizeof(float));
    audio_psd_filtered = (float *) malloc(fft * sizeof(float));

    for (uint16_t i = 0; i < fft; i++)
        audio_psd_filtered[i] = -130.0f;

    peak_filtered = -130.0f;
    noise_filtered = -100.0f;

    fft_window = malloc(fft * sizeof(float complex));
    window_buf = malloc(fft * sizeof(float complex));

    for (uint16_t i = 0; i < fft; i++)
        fft_window[i] = liquid_hamming(i, fft);

    float g = 0.0f;

    for (uint16_t i = 0; i < fft; i++)
        g += fft_window[i] * fft_window[i];

    g = 1.0f / sqrtf(g);

    for (uint16_t i = 0; i < fft; i++)
        fft_window[i] = g * fft_window[i];

    ready = true;
}

static int compare_fft_items(const void *p1, const void *p2) {
    fft_item_t *i1 = (fft_item_t *) p1;
    fft_item_t *i2 = (fft_item_t *) p2;

    return (i1->db > i2->db) ? -1 : 1;
}

static bool cw_get_peak() {
    uint32_t    start = fft / 2 + fft * params_mode.filter_low / AUDIO_CAPTURE_RATE;
    uint32_t    stop = fft / 2 + fft * params_mode.filter_high / AUDIO_CAPTURE_RATE;
    uint16_t    num = stop - start;

    float       peak_db = 0;
    uint16_t    peak_n = 0;
    float       noise_db = 0;
    bool        find = true;
    uint16_t    item = 0;

    for (uint16_t n = start; n < stop; n++) {
        fft_items[item].n = n;
        fft_items[item].db = audio_psd_filtered[n];
        
        item++;
    }

    qsort(&fft_items, num, sizeof(fft_item_t), compare_fft_items);

    uint16_t peak_width = 2;

    for (uint16_t i = 0; i < num; i++) {
        if (i < peak_width) {
            peak_db += fft_items[i].db;
        } else {
            noise_db += fft_items[i].db;
        }
    }

    peak_db /= peak_width;
    noise_db /= num - peak_width;

    lpf(&peak_filtered, peak_db, params.cw_decoder_peak_beta);
    lpf(&noise_filtered, noise_db, params.cw_decoder_noise_beta);

    float snr = peak_filtered - noise_filtered;

    if (snr > params.cw_decoder_snr) {
        peak_n = fft_items[0].n;
    } else {
        peak_n = 0;
    }

#if 1
    char    str[128];
    uint8_t i = 0;
    
    for (uint16_t n = start; n < stop; n++) {
        char c;
        
        if (n == peak_n) {
            c = '#';
        } else if (audio_psd_filtered[n] > noise_filtered) {
            c = '.';
        } else {
            c = ' ';
        }
        
        str[i++] = c;
    }
    
    str[i] = '\0';
    LV_LOG_INFO("%s [ %i %i ]", str, (int16_t) peak_db, (int16_t) snr);
#endif

    return (peak_n != 0);
}

void cw_put_audio_samples(unsigned int n, float complex *samples) {
    if (!ready) {
        return;
    }

    cbuffercf_write(audio_buf, samples, n);
    
    while (cbuffercf_size(audio_buf) > fft_all) {
        unsigned int n;
        float complex *buf;
        
        cbuffercf_read(audio_buf, fft, &buf, &n);
        
        for (uint16_t i = 0; i < fft; i++)
            window_buf[i] = buf[i] * fft_window[i];
        
        spgramcf_write(audio_sg, window_buf, n);
        cbuffercf_release(audio_buf, fft_over);
        
        spgramcf_get_psd(audio_sg, audio_psd);
        spgramcf_reset(audio_sg);

        for (uint16_t i = 0; i < fft; i++) {
            float psd = audio_psd[i];

            lpf(&audio_psd_filtered[i], psd, params.cw_decoder_beta);
        }

        if (params.cw_decoder) {
            cw_decoder_signal(cw_get_peak(), fft_over * 1000.0f / AUDIO_CAPTURE_RATE);
        }
    }
}

bool cw_change_decoder(int16_t df) {
    if (df == 0) {
        return params.cw_decoder;
    }

    params_lock();
    params.cw_decoder = !params.cw_decoder;
    params_unlock(&params.durty.cw_decoder);

    pannel_visible();
    
    return params.cw_decoder;
}

float cw_change_snr(int16_t df) {
    if (df == 0) {
        return params.cw_decoder_snr;
    }
    
    float x = params.cw_decoder_snr + df * 0.1f;

    if (x < 10.0f) {
        x = 10.0f;
    } else if (x > 30.0f) {
        x = 30.0f;
    }

    params_lock();
    params.cw_decoder_snr = x;
    params_unlock(&params.durty.cw_decoder_snr);

    return params.cw_decoder_snr;
}

float cw_change_beta(int16_t df) {
    if (df == 0) {
        return params.cw_decoder_beta;
    }

    float x = params.cw_decoder_beta + df * 0.05f;

    if (x < 0.1f) {
        x = 0.1f;
    } else if (x > 0.95f) {
        x = 0.95f;
    }

    params_lock();
    params.cw_decoder_beta = x;
    params_unlock(&params.durty.cw_decoder_beta);
    
    return params.cw_decoder_beta;
}

float cw_change_peak_beta(int16_t df) {
    if (df == 0) {
        return params.cw_decoder_peak_beta;
    }

    float x = params.cw_decoder_peak_beta + df * 0.05f;

    if (x < 0.1f) {
        x = 0.1f;
    } else if (x > 0.95f) {
        x = 0.95f;
    }

    params_lock();
    params.cw_decoder_peak_beta = x;
    params_unlock(&params.durty.cw_decoder_peak_beta);
    
    return params.cw_decoder_peak_beta;
}

float cw_change_noise_beta(int16_t df) {
    if (df == 0) {
        return params.cw_decoder_noise_beta;
    }

    float x = params.cw_decoder_noise_beta + df * 0.05f;

    if (x < 0.1f) {
        x = 0.1f;
    } else if (x > 0.95f) {
        x = 0.95f;
    }

    params_lock();
    params.cw_decoder_noise_beta = x;
    params_unlock(&params.durty.cw_decoder_noise_beta);
    
    return params.cw_decoder_noise_beta;
}
