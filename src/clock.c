/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>
#include "clock.h"
#include "styles.h"
#include "radio.h"
#include "util.h"

#define TIME_TIMEOUT    5000
#define POWER_TIMEOUT   3000
#define TX_TIMEOUT      1000

typedef enum {
    CLOCK_TIME = 0,
    CLOCK_POWER
} clock_state_t;

static lv_obj_t         *obj;
static pthread_mutex_t  power_mux;

static clock_state_t    state = CLOCK_TIME;
static uint64_t         timeout;

static float            v_ext;
static float            v_bat;
static uint8_t          cap_bat;

static char             str[32];

static void set_state(clock_state_t new_state) {
    state = new_state;
    
    switch (state) {
        case CLOCK_TIME:
            lv_obj_set_style_text_font(obj, &sony_38, 0);
            lv_obj_set_style_pad_ver(obj, 18, 0);
            break;

        case CLOCK_POWER:        
            lv_obj_set_style_text_font(obj, &sony_30, 0);
            lv_obj_set_style_pad_ver(obj, 8, 0);
            break;
    }
}

static void show_time() {
    time_t      now;
    struct tm   *t;
    uint64_t    ms = get_time();
    
    if (radio_get_state() == RADIO_RX) {
        if (ms > timeout) {
            switch (state) {
                case CLOCK_TIME:
                    set_state(CLOCK_POWER);
                    timeout = ms + POWER_TIMEOUT;
                    break;
                    
                case CLOCK_POWER:
                    set_state(CLOCK_TIME);
                    timeout = ms + TIME_TIMEOUT;
                    break;
            }
        }
    } else {
        set_state(CLOCK_POWER);
        timeout = ms + TX_TIMEOUT;
    }
    
    switch (state) {
        case CLOCK_TIME:
            now = time(NULL);
            t = localtime(&now);
            
            snprintf(str, sizeof(str), "%02i:%02i:%02i", t->tm_hour, t->tm_min, t->tm_sec);
            break;
            
        case CLOCK_POWER:
            pthread_mutex_lock(&power_mux);

            if (v_ext < 3.0f) {
                snprintf(str, sizeof(str), "BAT %.1fv\n%i%%", v_bat, cap_bat);
            } else {
                snprintf(str, sizeof(str), "BAT %.1fv\nEXT %.1fv", v_bat, v_ext);
            }

            pthread_mutex_unlock(&power_mux);
            break;
    }

    lv_label_set_text(obj, str);
}

lv_obj_t * clock_init(lv_obj_t * parent) {
    pthread_mutex_init(&power_mux, NULL);

    obj = lv_label_create(parent);

    lv_obj_add_style(obj, &clock_style, 0);
    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, 0);

    set_state(CLOCK_TIME);
    timeout = get_time() + TIME_TIMEOUT;

    show_time();
    lv_timer_create(show_time, 1000, NULL);
}

void clock_update_power(float ext, float bat, uint8_t cap) {
    pthread_mutex_lock(&power_mux);
    v_ext = ext;
    v_bat = bat;
    cap_bat = cap;
    pthread_mutex_unlock(&power_mux);
}
