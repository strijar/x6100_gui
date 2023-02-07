/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#pragma once

#include <unistd.h>
#include <stdint.h>

#include "lvgl/lvgl.h"

lv_obj_t * spectrum_init(lv_obj_t * parent);
void spectrum_data(float *data_buf, uint16_t size);
void spectrum_band_set();
void spectrum_mode_set();

void spectrum_set_max(int db);
void spectrum_set_min(int db);
void spectrum_change_freq(int16_t df);
void spectrum_clear();
