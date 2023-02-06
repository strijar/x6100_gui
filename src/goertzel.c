/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <math.h>
#include "goertzel.h"

void goertzel_bin_init(goertzel_t *goertzel, uint16_t bin, uint16_t bins) {
    float       w = (float) (2.0 * M_PI * bin) / (float) bins;
    
    goertzel->coef = 2.0f * cos(w);

    goertzel->s1 = 0;
    goertzel->s2 = 0;
}

void goertzel_freq_init(goertzel_t *goertzel, uint32_t freq, uint32_t rate, uint16_t bins) {
    uint16_t    bin = freq * bins / rate;
    
    goertzel_bin_init(goertzel, bin, bins);
}

void goertzel_reset(goertzel_t *goertzel) {
    goertzel->s1 = 0;
    goertzel->s2 = 0;
}

void goertzel_input(goertzel_t *goertzel, float input) {
    float s0 = goertzel->coef * goertzel->s1 - goertzel->s2 + input;
            
    goertzel->s2 = goertzel->s1;
    goertzel->s1 = s0;
}

float goertzel_output(goertzel_t *goertzel) {
    return sqrt(goertzel->s2 * goertzel->s2 + goertzel->s1 * goertzel->s1 - goertzel->coef * goertzel->s1 * goertzel->s2);
}
