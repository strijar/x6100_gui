/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#include <stdlib.h>

#include "dsp.h"
#include "spectrum.h"
#include "util.h"

#define FIR_LEN 21

static unsigned int     nfft = 512;
static iirfilt_cccf     dc_block;

static uint8_t          spectrum_factor = 1;
static firdecim_crcf    spectrum_decim;

static spgramcf         spectrum_sg;
static float            *spectrum_psd;
static uint8_t          spectrum_fps_ms = (1000 / 25);
static uint64_t         spectrum_time;
static float complex    *spectrum_dec_buf;

static spgramcf         waterfall_sg;
static float            *waterfall_psd;
static uint8_t          waterfall_fps_ms = (1000 / 10);
static uint64_t         waterfall_time;

static float complex    *buf;
static float complex    *buf_filtered;

void dsp_init() {
    dc_block = iirfilt_cccf_create_dc_blocker(0.01f);

    if (spectrum_factor > 1) {
        spectrum_decim = firdecim_crcf_create_prototype(LIQUID_FIRFILT_RCOS, spectrum_factor, 2, 0.1f, 0.0f);
        spectrum_dec_buf = (float complex *) malloc(nfft * sizeof(float) / spectrum_factor);
    }
    
    spectrum_sg = spgramcf_create(nfft, LIQUID_WINDOW_HANN, nfft, nfft / 4);
    spectrum_psd = (float *) malloc(nfft * sizeof(float));

    waterfall_sg = spgramcf_create(nfft, LIQUID_WINDOW_HANN, nfft, nfft / 4);
    waterfall_psd = (float *) malloc(nfft * sizeof(float));

    buf = (float complex*) malloc(nfft * sizeof(float complex));
    buf_filtered = (float complex*) malloc(nfft * sizeof(float complex));

    spectrum_time = get_time();
    waterfall_time = get_time();
}

void dsp_samples(float complex *buf_samples, uint16_t size) {
    uint64_t now = get_time();

    iirfilt_cccf_execute_block(dc_block, buf_samples, size, buf_filtered);

    if (spectrum_factor > 1) {
        firdecim_crcf_execute_block(spectrum_decim, buf_filtered, size / spectrum_factor, spectrum_dec_buf);
        spgramcf_write(spectrum_sg, spectrum_dec_buf, size / spectrum_factor);
    } else {
        spgramcf_write(spectrum_sg, buf_filtered, size);
    }
   
    spgramcf_get_psd(spectrum_sg, spectrum_psd);
    
    if (now - spectrum_time > spectrum_fps_ms) {
        spectrum_data(spectrum_psd, nfft);
        spgramcf_reset(spectrum_sg);
        
        spectrum_time = now;
    }

    /*
    spgramcf_write(waterfall_sg, buf_filtered, size);
    spgramcf_get_psd(waterfall_sg, waterfall_psd);

    if (now - waterfall_time > waterfall_fps_ms) {
        spectrum_data(spectrum_psd, nfft);
        spgramcf_reset(waterfall_sg);
        
        waterfall_time = now;
    }
    */
}
