/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#include "lvgl/lvgl.h"
#include "events.h"

uint32_t EVENT_ROTARY;

void event_init() {
    EVENT_ROTARY = lv_event_register_id();
}
