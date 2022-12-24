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

static unsigned int     nfft = 512;
static spgramcf         sg;
static iirfilt_cccf     dc_block;
static float            *psd;

static float complex    *buf;
static float complex    *buf_filtered;

void dsp_init() {
    sg = spgramcf_create(nfft, LIQUID_WINDOW_HANN, nfft, nfft / 4);
    dc_block = iirfilt_cccf_create_dc_blocker(0.01f);

    psd = (float *) malloc(nfft * sizeof(float));
    buf = (float complex*) malloc(nfft * sizeof(float complex));
    buf_filtered = (float complex*) malloc(nfft * sizeof(float complex));

    for (int i = 0; i < nfft; i++)
        psd[i] = -130.0f;
}

void dsp_samples(float complex *buf_samples, uint16_t size) {
    iirfilt_cccf_execute_block(dc_block, buf_samples, size, buf_filtered);
    spgramcf_write(sg, buf_filtered, size);
    spgramcf_get_psd(sg, psd);

    spectrum_data(psd, nfft);
    
    spgramcf_reset(sg);
}
