/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#pragma once

#include <stdbool.h>

#define RADIO_SAMPLES   (512)

void radio_init();
bool radio_tick();

uint64_t radio_change_freq(int32_t df);
uint16_t radio_change_vol(int16_t df);
uint16_t radio_change_rfg(int16_t df);
bool radio_change_pre();
void radio_band_set();
