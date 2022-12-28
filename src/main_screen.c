/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#include "lvgl/lvgl.h"
#include "styles.h"
#include "spectrum.h"
#include "waterfall.h"
#include "util.h"
#include "radio.h"
#include "main.h"
#include "events.h"
#include "msg.h"

static uint8_t  pad = 10;
static uint16_t spectrum_height = (480 / 3);
static uint16_t freq_height = 36;
static uint8_t  btn_height = 54;
static uint8_t  over = 30;

static int16_t  grid_min = -70;
static int16_t  grid_max = -40;

static lv_obj_t *obj;

static lv_obj_t *spectrum;
static lv_obj_t *freq[3];
static lv_obj_t *waterfall;
static lv_obj_t *btn[5];
static lv_obj_t *msg;

void main_screen_set_freq(uint64_t f) {
    uint16_t    mhz, khz, hz;

    split_freq(f - 50000, &mhz, &khz, &hz);
    lv_label_set_text_fmt(freq[0], "%i.%03i", mhz, khz);

    split_freq(f, &mhz, &khz, &hz);
    lv_label_set_text_fmt(freq[1], "%i.%03i.%03i", mhz, khz, hz);

    split_freq(f + 50000, &mhz, &khz, &hz);
    lv_label_set_text_fmt(freq[2], "%i.%03i", mhz, khz);
}

static void main_screen_rotary_cb(lv_event_t * e) {
    event_rotary_t *rotary = lv_event_get_param(e);

    switch (rotary->id) {
        case 0:
            radio_change_freq(rotary->diff);
            break;
            
        case 1:
            radio_change_vol(rotary->diff);
            break;
            
        case 2:
            grid_min += rotary->diff;
            spectrum_set_min(grid_min);
            waterfall_set_min(grid_min);
            msg_set_text_fmt("Min level: %idb", grid_min);
            break;
        
    }
}

static void main_screen_keypad_cb(lv_event_t * e) {
    event_keypad_t *keypad = lv_event_get_param(e);
    
    switch (keypad->key) {
        case key_rotary_vol:
            LV_LOG_INFO("VOL");
            break;
            
        case key_rotary_mfk:
            LV_LOG_INFO("MFK");
            break;
            
        default:
            LV_LOG_WARN("Unsuported key");
            break;
    }
}

lv_obj_t * main_screen() {
    uint16_t y = pad;

    obj = lv_obj_create(NULL);

    lv_obj_add_event_cb(obj, main_screen_rotary_cb, EVENT_ROTARY, NULL);
    lv_obj_add_event_cb(obj, main_screen_keypad_cb, EVENT_KEYPAD, NULL);
    lv_obj_add_style(obj, &background_style, LV_PART_MAIN);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    
    spectrum = spectrum_init(obj);
    
    spectrum_set_min(grid_min);
    spectrum_set_max(grid_max);

    lv_obj_set_y(spectrum, y);
    lv_obj_set_height(spectrum, spectrum_height);
    
    y += spectrum_height;
    
    for (uint8_t i = 0; i < 3; i++) {
        lv_obj_t *f = lv_label_create(obj);

        switch (i) {
            case 0:
                lv_obj_add_style(f, &freq_style, 0);
                lv_obj_set_style_text_align(f, LV_TEXT_ALIGN_LEFT, 0);
                break;

            case 1:
                lv_obj_add_style(f, &freq_main_style, 0);
                lv_obj_set_style_text_align(f, LV_TEXT_ALIGN_CENTER, 0);
                break;

            case 2:
                lv_obj_add_style(f, &freq_style, 0);
                lv_obj_set_style_text_align(f, LV_TEXT_ALIGN_RIGHT, 0);
                break;
        }
        
        lv_obj_set_pos(f, i * ((800 - pad * 2) / 3) + pad, y);
        lv_obj_set_size(f, (800 - pad * 2) / 3, freq_height);
        
        freq[i] = f;
    }
    
    y += freq_height;

    waterfall = waterfall_init(obj);

    spectrum_set_min(grid_min);
    spectrum_set_max(grid_max);
    
    lv_obj_set_y(waterfall, y);
    waterfall_set_height(480 - y - pad);
    
    y = 480 - btn_height;
    
    uint16_t x = 0;
    uint16_t width = (800 - pad * 4) / 5;

    for (uint8_t i = 0; i < 5; i++) {
        lv_obj_t *f = lv_btn_create(obj);

        lv_obj_add_style(f, &btn_style, 0);
        lv_obj_set_pos(f, x, y);
        lv_obj_set_size(f, width, btn_height + over);
        lv_obj_set_style_pad_bottom(f, over, 0);
        
        if (i == 0) {
            lv_obj_set_style_pad_left(f, over, 0);
        } else if (i == 4) {
            lv_obj_set_style_pad_right(f, over, 0);
        }
        
        x += width + pad;
        
        lv_obj_t *label = lv_label_create(f);
        
        lv_label_set_text_fmt(label, "Button %i", i);
        lv_obj_center(label);
    
        btn[i] = f;
    }

    msg = msg_init(obj);
    
    return obj;
}
