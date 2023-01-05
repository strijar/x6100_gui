/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#include <time.h>
#include <sys/time.h>
#include "clock.h"
#include "styles.h"

static uint8_t      over = 30;
static uint8_t      clock_height = 54;

lv_obj_t            *obj;

static void show_time() {
    time_t      now = time(NULL);
    struct tm   *time = localtime(&now);
    
    lv_label_set_text_fmt(obj, "%02i:%02i:%02i", time->tm_hour, time->tm_min, time->tm_sec);
}

lv_obj_t * clock_init(lv_obj_t * parent) {
    obj = lv_label_create(parent);

    lv_obj_add_style(obj, &panel_top_style, 0);
    lv_obj_add_style(obj, &clock_style, 0);
    
    lv_obj_set_height(obj, clock_height + over);
    lv_obj_set_style_pad_top(obj, over, 0);
    lv_obj_set_y(obj, -over);
    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, 0);

    show_time();
    lv_timer_create(show_time, 1000, NULL);
}
