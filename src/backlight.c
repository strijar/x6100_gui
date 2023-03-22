/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "lvgl/lvgl.h"
#include "backlight.h"
#include "params.h"
#include "util.h"

static int          power;
static int          brightness;

static lv_timer_t   *timer = NULL;

static void backlight_timer(lv_timer_t *t) {
    backlight_set_brightness(params.brightness_idle);
    timer = NULL;
}

void backlight_init() {
    power = open("/sys/class/backlight/backlight/bl_power", O_WRONLY);
    brightness = open("/sys/class/backlight/backlight/brightness", O_WRONLY);
    
    backlight_tick();
}

void backlight_tick() {
    if (timer) {
        lv_timer_reset(timer);
    } else {
        timer = lv_timer_create(backlight_timer, params.brightness_timeout * 1000, NULL);
        lv_timer_set_repeat_count(timer, 1);

        backlight_set_brightness(params.brightness_normal);
    }
}

void backlight_set_brightness(int16_t value) {
    if (brightness > 0) {
        if (value < 0) {
            backlight_set_power(false);
        } else if (value < 10) {
            char    str[8];
            int     len = snprintf(str, sizeof(str), "%i\n", 10 - value);
        
            write(brightness, str, len);
            backlight_set_power(true);
        }
    }
}

void backlight_set_power(bool value) {
    if (power > 0) {
        char    str[8];
        int     len = snprintf(str, sizeof(str), "%i\n", value ? 0 : 1);
        
        write(power, str, len);
    }
}

int16_t backlight_change_brightness(int16_t d) {
    if (d == 0) {
        return params.brightness_normal;
    }
    
    params_lock();
    params.brightness_normal = limit(params.brightness_normal + d, 0, 9);
    params_unlock(&params.durty.brightness_normal);

    backlight_set_brightness(params.brightness_normal);

    return params.brightness_normal;
}
