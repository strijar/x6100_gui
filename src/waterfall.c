/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <stdlib.h>

#include "waterfall.h"
#include "styles.h"
#include "radio.h"
#include "events.h"
#include "params.h"
#include "bands.h"
#include "band_info.h"

#define PX_BYTES    4

static lv_obj_t         *obj;
static lv_obj_t         *img;

static lv_coord_t       width;
static lv_coord_t       height;
static int32_t          width_hz = 100000;
static uint32_t         line_len;
static uint8_t          *line_buf = NULL;

static int              grid_min = -70;
static int              grid_max = -40;

static lv_img_dsc_t     *frame;
static lv_color_t       palette[256];
static int16_t          scroll_hor = 0;
static int16_t          scroll_hor_surplus = 0;

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
    uint8_t     *ptr = frame->data + frame->data_size - line_len * 2;

    for (int y = 0; y < height-1; y++) {
        memcpy(ptr + line_len, ptr, line_len);
        ptr -= line_len;
    }
}

static void scroll_right(int16_t px) {
    uint8_t     *ptr = frame->data;
    uint16_t    offset = px * PX_BYTES;
    uint16_t    tail = (width - px) * PX_BYTES;
    
    for (int y = 0; y < height; y++) {
        memset(line_buf + tail, 0, offset);
        memcpy(line_buf, ptr + offset, tail);
        memcpy(ptr, line_buf, line_len);
        
        ptr += line_len;
    }
}

static void scroll_left(int16_t px) {
    uint8_t     *ptr = frame->data;
    uint16_t    offset = px * PX_BYTES;
    uint16_t    tail = (width - px) * PX_BYTES;
    
    for (int y = 0; y < height; y++) {
        memset(line_buf, 0, offset);
        memcpy(line_buf + offset, ptr, tail);
        memcpy(ptr, line_buf, line_len);
        
        ptr += line_len;
    }
}

void waterfall_data(float *data_buf, uint16_t size) {
    if (scroll_hor) {
        return;
    }

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
        
        lv_img_buf_set_px_color(frame, width - x, 0, palette[id]);
    }
    
    event_send(img, LV_EVENT_REFRESH, NULL);
}

static void do_scroll_cb(lv_event_t * event) {
    if (scroll_hor == 0) {
        return;
    }

    int16_t px = (abs(scroll_hor) / 10) + 1;

    if (scroll_hor > 0) {
        scroll_right(px);
        scroll_hor -= px;
    } else {
        scroll_left(px);
        scroll_hor =+= px;
    }
    
    event_send(img, LV_EVENT_REFRESH, NULL);
}

void waterfall_set_height(lv_coord_t h) {
    lv_obj_set_height(obj, h);
    lv_obj_update_layout(obj);

    /* For more accurate horizontal scroll, it should be a "multiple of 500Hz" */
    /* 800 * 500Hz / 100000Hz = 4.0px */
    
    width = 800;
    height = lv_obj_get_height(obj);

    frame = lv_img_buf_alloc(width, height, LV_IMG_CF_TRUE_COLOR);

    line_len = frame->data_size / frame->header.h;
    line_buf = malloc(line_len);
    
    calc_palette();
    
    img = lv_img_create(obj);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    lv_img_set_src(img, frame);
    lv_obj_add_event_cb(img, do_scroll_cb, LV_EVENT_DRAW_POST, NULL);
    
    waterfall_band_set();
    band_info_init(obj);
}

void waterfall_clear() {
    memset(frame->data,0, frame->data_size);
    scroll_hor = 0;
    scroll_hor_surplus = 0;
}

void waterfall_band_set() {
    waterfall_set_min(params_band.grid_min);
    waterfall_set_max(params_band.grid_max);
}

void waterfall_set_max(int db) {
    grid_max = db;
}

void waterfall_set_min(int db) {
    grid_min = db;
}

void waterfall_change_freq(int16_t df) {
    uint16_t    div = width_hz / width;
    int16_t     surplus = df % div;

    scroll_hor += df / div;
    
    if (surplus) {
        scroll_hor_surplus += surplus;
    } else {
        scroll_hor_surplus = 0;
    }
    
    if (abs(scroll_hor_surplus) > div) {
        scroll_hor += scroll_hor_surplus / div;
        scroll_hor_surplus = scroll_hor_surplus % div;
    }

    if (scroll_hor) {
        lv_obj_invalidate(img);
    }
}
