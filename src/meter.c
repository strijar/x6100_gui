/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include "meter.h"
#include "styles.h"

static uint8_t      over = 25;
static uint8_t      meter_height = 54;

static lv_obj_t     *obj;

lv_obj_t * meter_init(lv_obj_t * parent) {
    obj = lv_obj_create(parent);

    lv_obj_add_style(obj, &panel_top_style, 0);
    lv_obj_add_style(obj, &meter_style, 0);
    
    lv_obj_set_height(obj, meter_height + over);
    lv_obj_set_style_pad_top(obj, over, 0);
    lv_obj_set_y(obj, -over);
    
    return obj;
}
