/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#pragma once

#include <stdint.h>

typedef struct {
    float   coef;
    float   s1;
    float   s2;
} goertzel_t;

void goertzel_freq_init(goertzel_t *goertzel, uint32_t freq, uint32_t rate, uint16_t bins);
void goertzel_bin_init(goertzel_t *goertzel, uint16_t bin, uint16_t bins);

void goertzel_input(goertzel_t *goertzel, float input);
float goertzel_output(goertzel_t *goertzel);
void goertzel_reset(goertzel_t *goertzel);
