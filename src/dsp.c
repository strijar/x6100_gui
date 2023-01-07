/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <stdlib.h>
#include <pthread.h>

#include "dsp.h"
#include "spectrum.h"
#include "waterfall.h"
#include "util.h"
#include "radio.h"
#include "params.h"

static unsigned int     nfft = 400;
static iirfilt_cccf     dc_block;

static pthread_mutex_t  spectrum_mux;

static uint8_t          spectrum_factor = 1;
static firdecim_crcf    spectrum_decim;

static spgramcf         spectrum_sg;
static float            *spectrum_psd;
static float            *spectrum_psd_filtered;
static float            spectrum_beta = 0.7f;
static uint8_t          spectrum_fps_ms = (1000 / 15);
static uint64_t         spectrum_time;
static float complex    *spectrum_dec_buf;

static spgramcf         waterfall_sg;
static float            *waterfall_psd;
static uint8_t          waterfall_fps_ms = (1000 / 25);
static uint64_t         waterfall_time;

static float complex    *buf;
static float complex    *buf_filtered;

static uint8_t          delay;

void dsp_init() {
    pthread_mutex_init(&spectrum_mux, NULL);

    dc_block = iirfilt_cccf_create_dc_blocker(0.005f);

    spectrum_sg = spgramcf_create(nfft, LIQUID_WINDOW_HANN, nfft, nfft / 4);
    spectrum_psd = (float *) malloc(nfft * sizeof(float));
    spectrum_psd_filtered = (float *) malloc(nfft * sizeof(float));

    dsp_set_spectrum_factor(params.spectrum_factor);

    for (uint16_t i = 0; i < nfft; i++)
        spectrum_psd_filtered[i] = -130.0f;

    waterfall_sg = spgramcf_create(nfft, LIQUID_WINDOW_HANN, nfft, nfft / 4);
    waterfall_psd = (float *) malloc(nfft * sizeof(float));

    buf = (float complex*) malloc(RADIO_SAMPLES * sizeof(float complex));
    buf_filtered = (float complex*) malloc(RADIO_SAMPLES * sizeof(float complex));

    spectrum_time = get_time();
    waterfall_time = get_time();
    
    delay = 4;
}

void dsp_reset() {
    delay = 4;

    iirfilt_cccf_reset(dc_block);
    spgramcf_reset(spectrum_sg);
    spgramcf_reset(waterfall_sg);
}

void dsp_samples(float complex *buf_samples, uint16_t size) {
    int res;

    if (delay)
        delay--;

    uint64_t now = get_time();

    res = iirfilt_cccf_execute_block(dc_block, buf_samples, size, buf_filtered);

    if (res) {
        LV_LOG_ERROR("iirfilt_cccf_execute_block");
    }

    pthread_mutex_lock(&spectrum_mux);

    if (spectrum_factor > 1) {
        firdecim_crcf_execute_block(spectrum_decim, buf_filtered, size / spectrum_factor, spectrum_dec_buf);
        
        for (uint8_t i = 0; i < spectrum_factor / 2; i++) {
            res = spgramcf_write(spectrum_sg, spectrum_dec_buf, size / spectrum_factor);

            if (res) {
                LV_LOG_ERROR("spgramcf_write");
            }
        }
    } else {
        res = spgramcf_write(spectrum_sg, buf_filtered, size);
        
        if (res) {
            LV_LOG_ERROR("spgramcf_write");
        }
    }

    res = spgramcf_get_psd(spectrum_sg, spectrum_psd);

    if (res) {
        LV_LOG_ERROR("spgramcf_get_psd");
    }
    
    pthread_mutex_unlock(&spectrum_mux);
    
    if (now - spectrum_time > spectrum_fps_ms) {
        if (!delay) {
            for (uint16_t i = 0; i < nfft; i++) {
                float psd = spectrum_psd[i];
                
                if (psd < -130.0f) {
                    psd = -130.0f;
                } else if (psd > 60.0f) {
                    psd = 60.0f;
                }
            
                spectrum_psd_filtered[i] = spectrum_psd_filtered[i] * spectrum_beta + psd * (1.0f - spectrum_beta);
            }
        
            spectrum_data(spectrum_psd_filtered, nfft);
        }

        spgramcf_reset(spectrum_sg);
        spectrum_time = now;
    }

    spgramcf_write(waterfall_sg, buf_filtered, size);
    spgramcf_get_psd(waterfall_sg, waterfall_psd);

    if (now - waterfall_time > waterfall_fps_ms) {
        if (!delay) {
            for (uint16_t i = 0; i < nfft; i++) {
                float psd = waterfall_psd[i];
                
                if (psd < -130.0f) {
                    waterfall_psd[i] = -130.0f;
                } else if (psd > 60.0f) {
                    waterfall_psd[i] = 60.0f;
                }
            }
        
            waterfall_data(waterfall_psd, nfft);
        }

        spgramcf_reset(waterfall_sg);
        waterfall_time = now;
    }
}

uint8_t dsp_get_spectrum_factor() {
    return spectrum_factor;
}

void dsp_set_spectrum_factor(uint8_t x) {
    pthread_mutex_lock(&spectrum_mux);

    spectrum_factor = x;

    if (spectrum_decim) {
        firdecim_crcf_destroy(spectrum_decim);
        spectrum_decim = NULL;
    }

    if (spectrum_dec_buf) {
        free(spectrum_dec_buf);
        spectrum_dec_buf = NULL;
    }

    if (spectrum_factor > 1) {
        spectrum_decim = firdecim_crcf_create_kaiser(spectrum_factor, 1, 60.0f);
        spectrum_dec_buf = (float complex *) malloc(RADIO_SAMPLES * sizeof(float complex) / spectrum_factor);
    }

    spgramcf_reset(spectrum_sg);

    for (uint16_t i = 0; i < nfft; i++)
        spectrum_psd_filtered[i] = -130.0f;

    pthread_mutex_unlock(&spectrum_mux);
}

float dsp_get_spectrum_beta() {
    return spectrum_beta;
}

void dsp_set_spectrum_beta(float x) {
    spectrum_beta = x;
}
