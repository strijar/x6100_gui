/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <stdlib.h>
#include <pthread.h>

#include "styles.h"
#include "spectrum.h"
#include "main.h"
#include "radio.h"
#include "events.h"
#include "dsp.h"
#include "params.h"

static lv_obj_t         *obj;

static int              grid_min = -70;
static int              grid_max = -40;
static bool             filled = false;

static int32_t          width_hz = 100000;
static int16_t          visor_height = 115;

static uint16_t         spectrum_size = 400;
static float            *spectrum_buf = NULL;

static lv_grad_dsc_t    filter_grad;

static pthread_mutex_t  data_mux;

static void spectrum_draw_cb(lv_event_t * e) {
    lv_event_code_t     code = lv_event_get_code(e);
    lv_obj_t            *obj = lv_event_get_target(e);
    lv_draw_ctx_t       *draw_ctx = lv_event_get_draw_ctx(e);
    lv_draw_line_dsc_t  line_dsc;
    
    if (!spectrum_buf) {
        return;
    }

    /* Main */
    
    lv_draw_line_dsc_init(&line_dsc);
    
    line_dsc.color = lv_color_hex(0xAAAAAA);
    line_dsc.width = 2;

    lv_coord_t x1 = obj->coords.x1;
    lv_coord_t y1 = obj->coords.y1;

    lv_coord_t w = lv_obj_get_width(obj);
    lv_coord_t h = lv_obj_get_height(obj);
    
    lv_point_t a, b;
    
    if (!filled) {
        b.x = x1;
        b.y = y1 + h;
    }
    
    for (uint16_t i = 0; i < spectrum_size; i++) {
        float v = (spectrum_buf[i] - grid_min) / (grid_max - grid_min);

        a.x = x1 + i * w / spectrum_size;
        a.y = y1 + (1.0f - v) * h;

        if (filled) {
            b.x = a.x;
            b.y = y1 + h;
        }
        
        lv_draw_line(draw_ctx, &line_dsc, &a, &b);
        
        if (!filled) {
            b = a;
        }
    }

    /* Filter */
    
    lv_draw_rect_dsc_t  rect_dsc;
    lv_area_t           area;

    lv_draw_rect_dsc_init(&rect_dsc);

    rect_dsc.bg_grad = filter_grad;
    rect_dsc.bg_opa = LV_OPA_50;
    
    uint32_t    w_hz = width_hz / dsp_get_spectrum_factor();
    int32_t     filter_from, filter_to;
    
    radio_filter_get(&filter_from, &filter_to);
    
    int32_t f1 = (int64_t)(w * filter_from) / w_hz;
    int32_t f2 = (int64_t)(w * filter_to) / w_hz;

    area.x1 = x1 + w / 2 + f1;
    area.y1 = y1 + h - visor_height;
    area.x2 = x1 + w / 2 + f2;
    area.y2 = y1 + h;

    lv_draw_rect(draw_ctx, &rect_dsc, &area);

    /* Center */

    line_dsc.width = 1;
    
    a.x = x1 + w / 2;
    a.y = y1 + h - visor_height;
    b.x = a.x;
    b.y = y1 + h;

    lv_draw_line(draw_ctx, &line_dsc, &a, &b);
}

lv_obj_t * spectrum_init(lv_obj_t * parent) {
    pthread_mutex_init(&data_mux, NULL);

    spectrum_buf = malloc(spectrum_size * sizeof(lv_point_t));

    filter_grad.dir = LV_GRAD_DIR_VER;
    filter_grad.stops_count = 4;

    filter_grad.stops[0].color = lv_color_lighten(bg_color, 196);
    filter_grad.stops[1].color = bg_color;
    filter_grad.stops[2].color = bg_color;
    filter_grad.stops[3].color = lv_color_darken(bg_color, 196);
    
    filter_grad.stops[0].frac  = 0;
    filter_grad.stops[1].frac  = 128 - 70;
    filter_grad.stops[2].frac  = 128 + 70;
    filter_grad.stops[3].frac  = 255;

    obj = lv_obj_create(parent);

    lv_obj_add_style(obj, &spectrum_style, 0);
    lv_obj_add_event_cb(obj, spectrum_draw_cb, LV_EVENT_DRAW_MAIN_END, NULL);

    spectrum_band_set();

    return obj;
}

void spectrum_data(float *data_buf, uint16_t size) {
    for (uint16_t i = 0; i < size; i++)
        spectrum_buf[i] = data_buf[size - i];

    event_send(obj, LV_EVENT_REFRESH, NULL);
}

void spectrum_band_set() {
    spectrum_set_min(params_band.grid_min);
    spectrum_set_max(params_band.grid_max);
}

void spectrum_set_max(int db) {
    grid_max = db;
}

void spectrum_set_min(int db) {
    grid_min = db;
}
