/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#include <stdlib.h>

#include "main.h"
#include "waterfall.h"
#include "styles.h"
#include "radio.h"

static lv_obj_t         *obj;
static lv_obj_t         *img;

static lv_coord_t       width;
static lv_coord_t       height;

static int              grid_min = -70;
static int              grid_max = -40;

static lv_img_dsc_t     *dsc;
static lv_color_t       palette[256];

static void calc_palette() {
    lv_grad_dsc_t grad;

    grad.dir = LV_GRAD_DIR_HOR;
    grad.stops_count = 5;

    grad.stops[0].color = lv_color_hex(0x000000);
    grad.stops[1].color = lv_color_hex(0x0000FF);
    grad.stops[2].color = lv_color_hex(0xFF0000);
    grad.stops[3].color = lv_color_hex(0xFFFF00);
    grad.stops[4].color = lv_color_hex(0xFFFFFF);
    
    grad.stops[0].frac  = 255 * 0.00;
    grad.stops[1].frac  = 255 * 0.25;
    grad.stops[2].frac  = 255 * 0.50;
    grad.stops[3].frac  = 255 * 0.75;
    grad.stops[4].frac  = 255 * 1.00;

    for (int i = 0; i < 256; i++)
        palette[i] = lv_gradient_calculate(&grad, 256, i);
}

lv_obj_t * waterfall_init(lv_obj_t * parent) {
    obj = lv_obj_create(parent);
    lv_obj_add_style(obj, &waterfall_style, 0);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    return obj;
}

static void scroll_down() {
    uint32_t    line = dsc->data_size / dsc->header.h;
    uint8_t     *ptr = dsc->data + dsc->data_size - line * 2;

    for (int y = 0; y < height-1; y++) {
        memcpy(ptr + line, ptr, line);
        ptr -= line;
    }
}

void waterfall_data(float *data_buf, uint16_t size) {
    scroll_down();

    for (int x = 0; x < width; x++) {
        uint16_t    index = x * size / width;
        float       v = (data_buf[index] - grid_min) / (grid_max - grid_min);
        
        if (v < 0.0f) {
            v = 0.0f;
        } else if (v > 1.0f) {
            v = 1.0f;
        }
        
        uint8_t id = v * 255;
        
        lv_img_buf_set_px_color(dsc, width - x, 0, palette[id]);
    }

    lv_lock();
    lv_obj_invalidate(img);
    lv_unlock();
}

void waterfall_set_height(lv_coord_t h) {
    lv_obj_set_height(obj, h);
    lv_obj_update_layout(obj);

    width = lv_obj_get_width(obj);
    height = lv_obj_get_height(obj);

    dsc = lv_img_buf_alloc(width, height, LV_IMG_CF_TRUE_COLOR);
    calc_palette();
    
    img = lv_img_create(obj);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    lv_img_set_src(img, dsc);
}

void waterfall_set_max(int db) {
    grid_max = db;
}

void waterfall_set_min(int db) {
    grid_min = db;
}
