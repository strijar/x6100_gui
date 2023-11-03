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
void main_screen_app(uint8_t page_app);

void main_screen_lock_freq(bool lock);
void main_screen_lock_band(bool lock);
void main_screen_lock_mode(bool lock);

void mem_load(uint8_t x);
void mem_save(uint8_t x);
