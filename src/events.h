/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#pragma once

#include "lvgl/lvgl.h"

#include <unistd.h>
#include <stdint.h>

typedef struct {
    uint8_t id;
    int16_t diff;
} event_rotary_t;

extern uint32_t EVENT_ROTARY;

void event_init();

void event_obj_invalidate(lv_obj_t *obj);
void event_obj_check();
