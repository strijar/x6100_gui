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

static uint8_t  pad = 10;
static uint16_t spectrum_height = (480 / 3);
static uint16_t freq_height = 36;
static uint8_t  btn_height = 54;
static uint8_t  over = 30;

static lv_obj_t *spectrum;
static lv_obj_t *freq[3];
static lv_obj_t *waterfall;
static lv_obj_t *btn[5];

void main_screen_set_freq(uint64_t f) {
    uint16_t    mhz, khz, hz;

    lv_lock();

    split_freq(f - 50000, &mhz, &khz, &hz);
    lv_label_set_text_fmt(freq[0], "%i.%03i", mhz, khz);

    split_freq(f, &mhz, &khz, &hz);
    lv_label_set_text_fmt(freq[1], "%i.%03i.%03i", mhz, khz, hz);

    split_freq(f + 50000, &mhz, &khz, &hz);
    lv_label_set_text_fmt(freq[2], "%i.%03i", mhz, khz);

    lv_unlock();
}

void main_screen() {
    uint16_t y = pad;

    lv_obj_add_style(lv_scr_act(), &background_style, LV_PART_MAIN);
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
    
    spectrum = spectrum_init();

    lv_obj_set_y(spectrum, y);
    lv_obj_set_height(spectrum, spectrum_height);
    
    y += spectrum_height;
    
    for (uint8_t i = 0; i < 3; i++) {
        lv_obj_t *f = lv_label_create(lv_scr_act());

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

    waterfall = waterfall_init();
    
    lv_obj_set_y(waterfall, y);
    waterfall_set_height(480 - y - pad);
    
    y = 480 - btn_height;
    
    uint16_t x = 0;
    uint16_t width = (800 - pad * 4) / 5;

    for (uint8_t i = 0; i < 5; i++) {
        lv_obj_t *f = lv_btn_create(lv_scr_act());

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
}
