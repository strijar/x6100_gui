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
#include "pannel.h"
#include "meter.h"

typedef struct {
    uint16_t    n;
    float       db;
} fft_item_t;

#define FFT             1024
#define OVER            8
#define FFT_OVER        (FFT / OVER)
#define FFT_ALL         (FFT + FFT_OVER)

static bool             ready = false;

static fft_item_t       fft_items[FFT / 2];

static cbuffercf        audio_buf;
static spgramcf         audio_sg;
static float            *audio_psd[OVER];
static float            *audio_psd_sum;
static uint8_t          audio_psd_index = 0;

static float            peak_filtered;
static float            noise_filtered;
static bool             peak_on = false;

void cw_init() {
    audio_buf = cbuffercf_create(FFT_ALL * 10);

    audio_sg = spgramcf_create(FFT, LIQUID_WINDOW_BLACKMANHARRIS, FFT, FFT);

    for (uint8_t i = 0; i < OVER; i++)
        audio_psd[i] = (float *) malloc(FFT * sizeof(float));

    audio_psd_sum = (float *) malloc(FFT * sizeof(float));

    peak_filtered = S_MIN;
    noise_filtered = S_MIN;

    ready = true;
}

static int compare_fft_items(const void *p1, const void *p2) {
    fft_item_t *i1 = (fft_item_t *) p1;
    fft_item_t *i2 = (fft_item_t *) p2;

    return (i1->db > i2->db) ? -1 : 1;
}

static bool cw_get_peak() {
    uint32_t    start = FFT / 2 + FFT * params_mode.filter_low / AUDIO_CAPTURE_RATE;
    uint32_t    stop = FFT / 2 + FFT * params_mode.filter_high / AUDIO_CAPTURE_RATE;
    uint16_t    num = stop - start;

    float       peak_db = 0;
    uint16_t    peak_n = 0;
    float       noise_db = 0;
    bool        find = true;
    uint16_t    item = 0;

    for (uint16_t n = start; n < stop; n++) {
        fft_items[item].n = n;
        fft_items[item].db = audio_psd_sum[n];
        
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

    if (peak_db > -3.0f)
        peak_db = -3.0f;

    if (noise_db > -3.0f)
        noise_db = -3.0f;

    lpf(&peak_filtered, peak_db, params.cw_decoder_peak_beta);
    lpf(&noise_filtered, noise_db, params.cw_decoder_noise_beta);

    float snr = peak_filtered - noise_filtered;
    float snr_max = params.cw_decoder_snr * OVER;
    float snr_min = (params.cw_decoder_snr - params.cw_decoder_snr_gist) * OVER;

    if (peak_on) {
        if (snr < snr_min) {
            peak_on = false;
        }
    } else {
        if (snr > snr_max) {
            peak_on = true;
        }
    }

#if 0
    if (peak_on) {
        peak_n = fft_items[0].n;
    } else {
        peak_n = 0;
    }

    char    str[128];
    uint8_t i = 0;
    
    for (uint16_t n = start; n < stop; n++) {
        char c;
        
        if (n == peak_n) {
            c = '#';
        } else if (audio_psd_sum[n] > noise_filtered) {
            c = '.';
        } else {
            c = ' ';
        }
        
        str[i++] = c;
    }
    
    str[i] = '\0';
    LV_LOG_INFO("[ %s ]", str);
#endif

    return peak_on;
}

void cw_put_audio_samples(unsigned int n, float complex *samples) {
    if (!ready) {
        return;
    }

    cbuffercf_write(audio_buf, samples, n);
    
    while (cbuffercf_size(audio_buf) > FFT_ALL) {
        unsigned int n;
        float complex *buf;
        
        cbuffercf_read(audio_buf, FFT, &buf, &n);
        
        spgramcf_write(audio_sg, buf, n);
        cbuffercf_release(audio_buf, FFT_OVER);
        
        spgramcf_get_psd(audio_sg, audio_psd[audio_psd_index]);
        spgramcf_reset(audio_sg);
        
        audio_psd_index++;
        
        if (audio_psd_index >= OVER) {
            audio_psd_index = 0;
        }

        for (uint16_t i = 0; i < FFT; i++) {
            float sum = 0;
            
            for (uint8_t n = 0; n < OVER; n++) {
                float *psd = audio_psd[n];
                
                sum += psd[i];
            }
            
            audio_psd_sum[i] = sum;
        }

        if (params.cw_decoder) {
            cw_decoder_signal(cw_get_peak(), FFT_OVER * 1000.0f / AUDIO_CAPTURE_RATE);
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

    if (x < 7.0f) {
        x = 7.0f;
    } else if (x > 30.0f) {
        x = 30.0f;
    }

    params_lock();
    params.cw_decoder_snr = x;
    params_unlock(&params.durty.cw_decoder_snr);

    return params.cw_decoder_snr;
}

float cw_change_peak_beta(int16_t df) {
    if (df == 0) {
        return params.cw_decoder_peak_beta;
    }

    float x = params.cw_decoder_peak_beta + df * 0.01f;

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

    float x = params.cw_decoder_noise_beta + df * 0.01f;

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
