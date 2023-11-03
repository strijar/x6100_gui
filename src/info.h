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

lv_obj_t * info_init(lv_obj_t * parent);
void info_params_set();
void info_atu_update();

const char* info_params_mode();
const char* info_params_agc();
const char* info_params_vfo();
bool info_params_att();
bool info_params_pre();

void info_lock_mode(bool lock);
