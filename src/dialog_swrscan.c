/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include "lvgl/lvgl.h"
#include "dialog.h"
#include "dialog_swrscan.h"
#include "styles.h"
#include "params.h"
#include "radio.h"
#include "events.h"
#include "util.h"

#define AVR     2
#define STEPS   50

static lv_obj_t             *dialog;
static lv_obj_t             *chart;
static float                data[STEPS];

static lv_coord_t           w;
static lv_coord_t           h;

static bool                 run = false;
static float                avr_sum = 0.0f;
static uint8_t              avr_count = 0;

static uint32_t             span = 200000;
static bool                 linear = true;

static uint16_t             freq_index;
static uint64_t             freq_start;
static uint64_t             freq_center;
static uint64_t             freq_stop;

static void do_init() {
    for (uint16_t i = 0; i < STEPS; i++)
        data[i] = 1.0f;

    avr_sum = 0.0f;
    avr_count = 0;
    freq_index = 0;
        
    freq_center = params_band.vfo_x[params_band.vfo].freq;
        
    freq_start = freq_center - span / 2;
    freq_stop = freq_center + span / 2;
}

static void do_step(float vswr) {
    data[freq_index] = vswr;
    event_send(chart, LV_EVENT_REFRESH, NULL);

    freq_index++;
    
    if (freq_index >= STEPS) {
        freq_index = 0;
    }

    uint64_t freq = freq_start + (freq_stop - freq_start) * freq_index / STEPS;
    radio_set_freq(freq);
}

static lv_coord_t calc_y(float vswr) {
    float x;

    if (linear) {
        x = (vswr - 1.0f) / (5.0f - 1.0f);
    } else {
        float c = 1.0f / logf(10.0);
        
        x = log10f(1.0f + (vswr - 1.0f) / c);
    }

    return (1.0f - x) * h;
}

static void draw_cb(lv_event_t * e) {
    lv_obj_t            *obj = lv_event_get_target(e);
    lv_draw_ctx_t       *draw_ctx = lv_event_get_draw_ctx(e);
    lv_draw_line_dsc_t  line_dsc;
    char                str[32];

    lv_draw_line_dsc_init(&line_dsc);
    
    lv_coord_t x1 = obj->coords.x1;
    lv_coord_t y1 = obj->coords.y1;

    lv_coord_t w = lv_obj_get_width(obj);
    lv_coord_t h = lv_obj_get_height(obj);

    lv_point_t a, b;

    /* Grid */

    lv_area_t   area;

    lv_draw_label_dsc_t dsc_label;
    lv_draw_label_dsc_init(&dsc_label);
    
    dsc_label.color = lv_color_white();
    dsc_label.font = &sony_28;

    lv_point_t label_size;

    line_dsc.color = lv_color_hex(0xAAAAAA);
    line_dsc.width = 2;

    a.x = x1;
    b.x = x1 + w;

    for (float y = 1.5f; y <= 4.5f; y+= 0.5f) {
        a.y = y1 + calc_y(y);
        b.y = a.y;

        lv_draw_line(draw_ctx, &line_dsc, &a, &b);

        snprintf(str, sizeof(str), "%.1f", y);
        lv_txt_get_size(&label_size, str, dsc_label.font, 0, 0, LV_COORD_MAX, 0);

        area.x1 = a.x;
        area.y1 = a.y - label_size.y / 2;
        area.x2 = a.x + label_size.x;
        area.y2 = a.y + label_size.y / 2;

        lv_draw_label(draw_ctx, &dsc_label, &area, str, NULL);
    }
    
    uint64_t    freq = freq_center - span / 4;
    uint16_t    mhz, khz, hz;

    a.y = y1;
    b.y = y1 + h;
    
    for (int16_t x = -1; x <= 1; x++, freq += span / 4) {
        a.x = x1 + w / 2 + (w / 4) * x;
        b.x = a.x;

        lv_draw_line(draw_ctx, &line_dsc, &a, &b);

        split_freq(freq, &mhz, &khz, &hz);
        snprintf(str, sizeof(str), "%i.%03i.%03i", mhz, khz, hz);
        lv_txt_get_size(&label_size, str, dsc_label.font, 0, 0, LV_COORD_MAX, 0);

        area.x1 = a.x - label_size.x / 2;
        area.y1 = y1 + h - label_size.y;
        area.x2 = area.x1 + label_size.x;
        area.y2 = area.y1 + label_size.y;

        lv_draw_label(draw_ctx, &dsc_label, &area, str, NULL);
    }

    /* Chart */

    line_dsc.color = lv_color_white();
    line_dsc.width = 4;

    for (uint16_t i = 1; i < STEPS; i++) {
        float av = (data[i-1] - 1.0f) / (5.0f - 1.0f);
        float bv = (data[i] - 1.0f) / (5.0f - 1.0f);

        a.x = x1 + (i - 1) * w / STEPS;
        a.y = y1 + calc_y(data[i-1]);

        b.x = x1 + (i) * w / STEPS;
        b.y = y1 + calc_y(data[i]);

        lv_draw_line(draw_ctx, &line_dsc, &a, &b);
    }
}

lv_obj_t * dialog_swrscan(lv_obj_t *parent) {
    dialog = dialog_init(parent);

    chart  = lv_obj_create(dialog);

    w = 780;
    h = 330;
    
    lv_obj_add_event_cb(chart, draw_cb, LV_EVENT_DRAW_MAIN_END, NULL);
    lv_obj_set_size(chart, w, h);
    lv_obj_center(chart);

    lv_obj_set_style_bg_opa(chart, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(chart, 0, LV_PART_MAIN);

    do_init();

    return dialog;
}

void dialog_swrscan_run_cb(lv_event_t * e) {
    run = radio_start_swrscan();
    
    if (run) {
        do_init();
        radio_set_freq(freq_start);
    }
}

void dialog_swrscan_scale_cb(lv_event_t * e) {
    linear = !linear;
    event_send(chart, LV_EVENT_REFRESH, NULL);
}

void dialog_swrscan_span_cb(lv_event_t * e) {
    if (run) {
        return;
    }

    switch (span) {
        case 50000:
            span = 100000;
            break;
    
        case 100000:
            span = 200000;
            break;
            
        case 200000:
            span = 50000;
            break;
    }

    do_init();
    event_send(chart, LV_EVENT_REFRESH, NULL);
}

void dialog_swrscan_update(float vswr) {
    avr_sum += vswr;
    avr_count++;
    
    if (avr_count >= AVR) {
        do_step(avr_sum / AVR);
        avr_sum = 0;
        avr_count = 0;
    }
}

