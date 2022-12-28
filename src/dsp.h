/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <liquid/liquid.h>

void dsp_init();
void dsp_samples(float complex *buf_samples, uint16_t size);
void dsp_reset();

uint8_t dsp_get_spectrum_factor();
void dsp_set_spectrum_factor(uint8_t x);

float dsp_get_spectrum_beta();
void dsp_set_spectrum_beta(float x);
