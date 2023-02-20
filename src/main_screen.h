/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#pragma once

typedef enum {
    VOL_VOL = 0,
    VOL_SQL,
    VOL_RFG,
    VOL_FILTER_LOW,
    VOL_FILTER_HIGH,
    VOL_PWR,
    VOL_HMIC,
    VOL_MIC,
    VOL_IMIC,
    
    VOL_LAST
} vol_mode_t;

lv_obj_t * main_screen();
void main_screen_band_set();
