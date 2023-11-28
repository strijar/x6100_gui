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
#include "radio.h"
#include "events.h"
#include "dsp.h"
#include "params.h"
#include "util.h"
#include "meter.h"
#include "rtty.h"
#include "recorder.h"

float                   spectrum_auto_min;
float                   spectrum_auto_max;

static lv_obj_t         *obj;

static int              grid_min = -70;
static int              grid_max = -40;

static int32_t          width_hz = 100000;
static int16_t          visor_height = 100;

static uint16_t         spectrum_size = 400;
static float            *spectrum_buf = NULL;

static int16_t          delta_surplus = 0;

typedef struct {
    float       val;
    uint64_t    time;
} peak_t;

static peak_t           *spectrum_peak;

static pthread_mutex_t  data_mux;

static void spectrum_draw_cb(lv_event_t * e) {
    lv_event_code_t     code = lv_event_get_code(e);
    lv_obj_t            *obj = lv_event_get_target(e);
    lv_draw_ctx_t       *draw_ctx = lv_event_get_draw_ctx(e);
    lv_draw_line_dsc_t  main_line_dsc;
    lv_draw_line_dsc_t  peak_line_dsc;
    
    if (!spectrum_buf) {
        return;
    }

    /* Lines */
    
    lv_draw_line_dsc_init(&main_line_dsc);
    
    main_line_dsc.color = lv_color_hex(0xAAAAAA);
    main_line_dsc.width = 2;

    lv_draw_line_dsc_init(&peak_line_dsc);
    
    peak_line_dsc.color = lv_color_hex(0x555555);
    peak_line_dsc.width = 1;

    lv_coord_t x1 = obj->coords.x1;
    lv_coord_t y1 = obj->coords.y1;

    lv_coord_t w = lv_obj_get_width(obj);
    lv_coord_t h = lv_obj_get_height(obj);
    
    lv_point_t main_a, main_b;
    lv_point_t peak_a, peak_b;
    
    if (!params.spectrum_filled) {
        main_b.x = x1;
        main_b.y = y1 + h;
    }

    peak_b.x = x1;
    peak_b.y = y1 + h;

    float min = params.spectrum_auto_min ? spectrum_auto_min + 6.0f : grid_min;
    float max = params.spectrum_auto_max ? spectrum_auto_max + 10.0f : grid_max;
    
    for (uint16_t i = 0; i < spectrum_size; i++) {
        float       v = (spectrum_buf[i] - min) / (max - min);
        uint16_t    x = i * w / spectrum_size;

        /* Peak */
        
        if (params.spectrum_peak) {
            float v_peak = (spectrum_peak[i].val - min) / (max - min);

            peak_a.x = x1 + x;
            peak_a.y = y1 + (1.0f - v_peak) * h;

            lv_draw_line(draw_ctx, &peak_line_dsc, &peak_a, &peak_b);

            peak_b = peak_a;
        }

        /* Main */

        main_a.x = x1 + x;
        main_a.y = y1 + (1.0f - v) * h;

        if (params.spectrum_filled) {
            main_b.x = main_a.x;
            main_b.y = y1 + h;
        }
        
        lv_draw_line(draw_ctx, &main_line_dsc, &main_a, &main_b);
        
        if (!params.spectrum_filled) {
            main_b = main_a;
        }
    }

    /* Filter */
    
    lv_draw_rect_dsc_t  rect_dsc;
    lv_area_t           area;

    lv_draw_rect_dsc_init(&rect_dsc);

    rect_dsc.bg_color = bg_color;
    rect_dsc.bg_opa = LV_OPA_50;
    
    uint32_t    w_hz = width_hz / params_mode.spectrum_factor;
    int32_t     filter_from, filter_to;
    
    radio_filter_get(&filter_from, &filter_to);

    int16_t sign_from = (filter_from > 0) ? 1 : -1;
    int16_t sign_to = (filter_to > 0) ? 1 : -1;
    
    int32_t f1 = (int64_t)(w * filter_from) / w_hz;
    int32_t f2 = (int64_t)(w * filter_to) / w_hz;

    area.x1 = x1 + w / 2 + f1;
    area.y1 = y1 + h - visor_height;
    area.x2 = x1 + w / 2 + f2;
    area.y2 = y1 + h;

    lv_draw_rect(draw_ctx, &rect_dsc, &area);

    /* Notch */
    
    if (params.dnf) {
        rect_dsc.bg_color = lv_color_white();
        rect_dsc.bg_opa = LV_OPA_50;

        filter_from = sign_from * (params.dnf_center - params.dnf_width);
        filter_to = sign_to * (params.dnf_center + params.dnf_width);

        if (filter_from < filter_to) {
            f1 = (int64_t)(w * filter_from) / w_hz;
            f2 = (int64_t)(w * filter_to) / w_hz;
        } else {
            f1 = (int64_t)(w * filter_to) / w_hz;
            f2 = (int64_t)(w * filter_from) / w_hz;
        }

        area.x1 = x1 + w / 2 + f1;
        area.y1 = y1 + h - visor_height;
        area.x2 = x1 + w / 2 + f2;
        area.y2 = y1 + h;

        lv_draw_rect(draw_ctx, &rect_dsc, &area);
    }
    
    if (rtty_get_state() != RTTY_OFF) {
        filter_from = sign_from * (params.rtty_center - params.rtty_shift / 2);
        filter_to = sign_to * (params.rtty_center + params.rtty_shift / 2);

        f1 = (int64_t)(w * filter_from) / w_hz;
        f2 = (int64_t)(w * filter_to) / w_hz;

        main_a.x = x1 + w / 2 + f1;
        main_a.y = y1 + h - visor_height;
        main_b.x = main_a.x;
        main_b.y = y1 + h;
        lv_draw_line(draw_ctx, &main_line_dsc, &main_a, &main_b);

        main_a.x = x1 + w / 2 + f2;
        main_b.x = main_a.x;
        lv_draw_line(draw_ctx, &main_line_dsc, &main_a, &main_b);
    }

    /* Center */

    main_line_dsc.width = 1;
    
    main_a.x = x1 + w / 2;
    main_a.y = y1 + h - visor_height;
    main_b.x = main_a.x;
    main_b.y = y1 + h;

    if (recorder_is_on()) {
        main_line_dsc.color = lv_color_hex(0xFF0000);
    }

    lv_draw_line(draw_ctx, &main_line_dsc, &main_a, &main_b);
}

static void tx_cb(lv_event_t * e) {
    visor_height -= 61;
}

static void rx_cb(lv_event_t * e) {
    visor_height += 61;
}

lv_obj_t * spectrum_init(lv_obj_t * parent) {
    pthread_mutex_init(&data_mux, NULL);

    spectrum_buf = malloc(spectrum_size * sizeof(float));
    spectrum_peak = malloc(spectrum_size * sizeof(peak_t));

    obj = lv_obj_create(parent);

    lv_obj_add_style(obj, &spectrum_style, 0);
    lv_obj_add_event_cb(obj, spectrum_draw_cb, LV_EVENT_DRAW_MAIN_END, NULL);
    lv_obj_add_event_cb(obj, tx_cb, EVENT_RADIO_TX, NULL);
    lv_obj_add_event_cb(obj, rx_cb, EVENT_RADIO_RX, NULL);

    spectrum_clear();
    spectrum_band_set();

    return obj;
}

void spectrum_data(float *data_buf, uint16_t size) {
    uint64_t now = get_time();

    pthread_mutex_lock(&data_mux);

    for (uint16_t i = 0; i < size; i++) {
        spectrum_buf[i] = data_buf[size - i - 1];
        
        if (params.spectrum_peak) {
            float   v = spectrum_buf[i];
            peak_t  *peak = &spectrum_peak[i];

            if (v > peak->val) {
                peak->time = now;
                peak->val = v;
            } else {
                if (now - peak->time > params.spectrum_peak_hold) {
                    peak->val -= params.spectrum_peak_speed;
                }
            }
        }
    }

    pthread_mutex_unlock(&data_mux);
    event_send(obj, LV_EVENT_REFRESH, NULL);
}

void spectrum_band_set() {
    spectrum_set_min(params_band.grid_min);
    spectrum_set_max(params_band.grid_max);
}

void spectrum_mode_set() {
    dsp_set_spectrum_factor(params_mode.spectrum_factor);
}

void spectrum_set_max(int db) {
    grid_max = db;
}

void spectrum_set_min(int db) {
    grid_min = db;
}

void spectrum_clear() {
    uint64_t now = get_time();

    for (uint16_t i = 0; i < spectrum_size; i++) {
        spectrum_peak[i].val = S_MIN;
        spectrum_peak[i].time = now;
    }
}

void spectrum_change_freq(int16_t df) {
    peak_t      *from, *to;
    uint64_t    time = get_time();

    uint16_t    div = width_hz / spectrum_size / params_mode.spectrum_factor;
    int16_t     surplus = df % div;
    int32_t     delta = df / div;

    if (surplus) {
        delta_surplus += surplus;
    } else {
        delta_surplus = 0;
    }

    if (abs(delta_surplus) > div) {
        delta += delta_surplus / div;
        delta_surplus = delta_surplus % div;
    }

    if (delta == 0) {
        return;
    }

    if (delta > 0) {
        for (int16_t i = 0; i < spectrum_size - 1; i++) {
            to = &spectrum_peak[i];

            if (i >= spectrum_size - delta) {
                to->val = S_MIN;
                to->time = time;
            } else {
                from = &spectrum_peak[i + delta];
                *to = *from;
            }
        }
    } else {
        delta = -delta;

        for (int16_t i = spectrum_size - 1; i > 0; i--) {
            to = &spectrum_peak[i];
        
            if (i <= delta) {
                to->val = S_MIN;
                to->time = time;
            } else {
                from = &spectrum_peak[i - delta];
                *to = *from;
            }
        }
    }
}
