/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#pragma once

#include "lvgl/lvgl.h"

lv_obj_t * main_screen();
void main_screen_band_set();

void main_screen_keys_enable(bool value);
void main_screen_dialog_deleted_cb();

void mem_load(uint8_t x);
void mem_save(uint8_t x);
