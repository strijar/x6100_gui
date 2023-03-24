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
static bool         on = true;

static lv_timer_t   *timer = NULL;

static void backlight_timer(lv_timer_t *t) {
    backlight_set_brightness(params.brightness_idle);
    timer = NULL;
}

static void set_brightness(int16_t value) {
    if (brightness > 0) {
        char    str[8];
        int     len = snprintf(str, sizeof(str), "%i\n", 10 - value);

        write(brightness, str, len);
    }
}

static void set_power(bool value) {
    if (power > 0) {
        char    str[8];
        int     len = snprintf(str, sizeof(str), "%i\n", value ? 0 : 1);
        
        write(power, str, len);
    }
}

void backlight_init() {
    power = open("/sys/class/backlight/backlight/bl_power", O_WRONLY);
    brightness = open("/sys/class/backlight/backlight/brightness", O_WRONLY);
    on = true;
    
    backlight_tick();
}

void backlight_tick() {
    if (timer) {
        lv_timer_set_period(timer, params.brightness_timeout * 1000);
        lv_timer_reset(timer);
    } else {
        timer = lv_timer_create(backlight_timer, params.brightness_timeout * 1000, NULL);
        lv_timer_set_repeat_count(timer, 1);

        backlight_set_brightness(params.brightness_normal);
    }
}

void backlight_set_brightness(int16_t value) {
    if (on) {
        if (value < 0) {
            set_power(false);
            
            /* Setting max PWM for reduce noice */
            set_brightness(9);
        } else {
            set_brightness(value);
            set_power(true);
        }
    }
}

void backlight_switch() {
    on = !on;
    
    if (on) {
        set_power(true);
        set_brightness(params.brightness_normal);
    } else {
        set_power(false);
        set_brightness(9);
    }
}

bool backlight_is_on() {
    return on;
}
