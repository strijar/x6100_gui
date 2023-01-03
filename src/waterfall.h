/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#pragma once

#include <unistd.h>
#include <stdint.h>

#include "lvgl/lvgl.h"

lv_obj_t * waterfall_init(lv_obj_t * parent);
void waterfall_data(float *data_buf, uint16_t size);
void waterfall_set_height(lv_coord_t h);
void waterfall_clear();
void waterfall_band_set();

void waterfall_set_max(int db);
void waterfall_set_min(int db);
void waterfall_change_freq(int16_t df);
