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

static uint8_t  pad = 10;
static uint16_t spectrum_height = (480 / 3);
static uint16_t freq_height = 36;
static uint8_t  btn_height = 54;
static uint8_t  over = 30;

static lv_obj_t *spectrum;
static lv_obj_t *freq[5];
static lv_obj_t *waterfall;
static lv_obj_t *btn[5];

static void calc_gradient() {
    lv_grad_dsc_t grad;

    grad.dir = LV_GRAD_DIR_HOR;
    grad.stops_count = 5;

    grad.stops[0].color = lv_color_hex(0x000000);
    grad.stops[1].color = lv_color_hex(0x0000FF);
    grad.stops[2].color = lv_color_hex(0xFF0000);
    grad.stops[3].color = lv_color_hex(0xFFFF00);
    grad.stops[4].color = lv_color_hex(0xFFFFFF);
    
    grad.stops[0].frac  = 0;
    grad.stops[1].frac  = 64 + 30;
    grad.stops[2].frac  = 128 + 30;
    grad.stops[3].frac  = 196 + 30;
    grad.stops[4].frac  = 255;
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
                lv_label_set_text(f, "14.024.000");
                lv_obj_set_style_text_align(f, LV_TEXT_ALIGN_LEFT, 0);
                break;

            case 1:
                lv_obj_add_style(f, &freq_main_style, 0);
                lv_label_set_text(f, "14.074.000");
                lv_obj_set_style_text_align(f, LV_TEXT_ALIGN_CENTER, 0);
                break;

            case 2:
                lv_obj_add_style(f, &freq_style, 0);
                lv_label_set_text(f, "14.124.000");
                lv_obj_set_style_text_align(f, LV_TEXT_ALIGN_RIGHT, 0);
                break;
        }
        
        lv_obj_set_pos(f, i * ((800 - pad * 2) / 3) + pad, y);
        lv_obj_set_size(f, (800 - pad * 2) / 3, freq_height);
        
        freq[i] = f;
    }
    
    y += freq_height;

    waterfall = lv_obj_create(lv_scr_act());
    lv_obj_add_style(waterfall, &waterfall_style, 0);
    
    lv_obj_set_y(waterfall, y);
    lv_obj_set_height(waterfall, 480 - y - pad);

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
