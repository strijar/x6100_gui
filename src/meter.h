/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#pragma once

#include <stdint.h>
#include "lvgl/lvgl.h"

#define S_MIN   (-127)
#define S1      (-121)
#define S2      (-115)
#define S3      (-109)
#define S4      (-103)
#define S5      (-97)
#define S6      (-91)
#define S7      (-85)
#define S8      (-79)
#define S9      (-73)
#define S9_20   (S9 + 20)
#define S9_40   (S9 + 40)

lv_obj_t * meter_init(lv_obj_t * parent);
void meter_update(int16_t db);
