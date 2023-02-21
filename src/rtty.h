/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <liquid/liquid.h>

void rtty_init();
void rtty_put_audio_samples(unsigned int n, float complex *samples);

void rtty_enable(bool on);
bool rtty_is_enabled();

float rtty_change_rate(int16_t df);
uint16_t rtty_change_shift(int16_t df);
uint16_t rtty_change_center(int16_t df);
bool rtty_change_reverse(int16_t df);
