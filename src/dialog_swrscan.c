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
#include "keyboard.h"
#include "main_screen.h"

#define STEPS   50

static lv_obj_t             *chart;
static float                data[STEPS];
static float                data_filtered[STEPS];

static lv_coord_t           w;
static lv_coord_t           h;

static bool                 run = false;

static uint16_t             freq_index;
static uint64_t             freq_start;
static uint64_t             freq_center;
static uint64_t             freq_stop;

static void construct_cb(lv_obj_t *parent);
static void key_cb(lv_event_t * e);

static dialog_t             dialog = {
    .run = false,
    .construct_cb = construct_cb,
    .destruct_cb = NULL,
    .key_cb = key_cb
};

dialog_t                    *dialog_swrscan = &dialog;

static void do_init() {
    for (uint16_t i = 0; i < STEPS; i++) {
        data[i] = 1.0f;
        data_filtered[i] = 1.0f;
    }

    freq_index = 0;
    freq_center = params_band.vfo_x[params_band.vfo].freq;
        
    freq_start = freq_center - params.swrscan_span / 2;
    freq_stop = freq_center + params.swrscan_span / 2;
}

static void do_step(float vswr) {
    data[freq_index] = vswr;

    for (int16_t i = 0; i < STEPS; i++) {
        data_filtered[i] = 0.0;
    
        for (int16_t n = -2; n <= 2; n++) {
            int16_t index = i + n;
            
            if (index < 0) {
                index = 0;
            } else if (index > STEPS-1) {
                index = STEPS-1;
            }
        
            data_filtered[i] += data[index];
        }
        data_filtered[i] /= 5.0f;
    }

    event_send(chart, LV_EVENT_REFRESH, NULL);

    freq_index++;
    
    if (freq_index == STEPS) {
        freq_index = 0;
    }

    uint64_t freq = freq_start + (freq_stop - freq_start) * freq_index / STEPS;
    radio_set_freq(freq);
}

static lv_coord_t calc_y(float vswr) {
    float x;

    if (params.swrscan_linear) {
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
    
    uint64_t    freq = freq_center - params.swrscan_span / 4;
    uint16_t    mhz, khz, hz;

    a.y = y1;
    b.y = y1 + h;
    
    for (int16_t x = -1; x <= 1; x++, freq += params.swrscan_span / 4) {
        a.x = x1 + w / 2 + (w / 4) * x;
        b.x = a.x;

        lv_draw_line(draw_ctx, &line_dsc, &a, &b);

        split_freq(freq, &mhz, &khz, &hz);
        snprintf(str, sizeof(str), "%i.%03i.%03i", mhz, khz, hz);
        lv_txt_get_size(&label_size, str, dsc_label.font, 0, 0, LV_COORD_MAX, 0);

        area.x1 = a.x - label_size.x / 2;
        area.y1 = y1 + label_size.y / 2;
        area.x2 = area.x1 + label_size.x;
        area.y2 = area.y1 + label_size.y;

        lv_draw_label(draw_ctx, &dsc_label, &area, str, NULL);
    }

    /* Chart */

    line_dsc.color = lv_color_white();
    line_dsc.width = 4;

    for (uint16_t i = 0; i < STEPS; i++) {
        a.x = x1 + (i - 1) * w / STEPS;
        a.y = y1 + calc_y(data_filtered[i-1]);

        b.x = x1 + (i) * w / STEPS;
        b.y = y1 + calc_y(data_filtered[i]);

        lv_draw_line(draw_ctx, &line_dsc, &a, &b);
    }
}

static void freq_update_cb(lv_event_t * e) {
    do_init();
    lv_obj_invalidate(chart);
}

static void construct_cb(lv_obj_t *parent) {
    dialog.obj = dialog_init(parent);

    lv_obj_add_event_cb(dialog.obj, freq_update_cb, EVENT_FREQ_UPDATE, NULL);

    chart  = lv_obj_create(dialog.obj);

    w = 780;
    h = 330;
    
    lv_obj_add_event_cb(chart, draw_cb, LV_EVENT_DRAW_MAIN_END, NULL);
    lv_obj_set_size(chart, w, h);
    lv_obj_center(chart);

    lv_obj_set_style_bg_opa(chart, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(chart, 0, LV_PART_MAIN);

    lv_group_add_obj(keyboard_group, chart);
    lv_obj_add_event_cb(chart, key_cb, LV_EVENT_KEY, NULL);

    do_init();
}

static void key_cb(lv_event_t * e) {
    uint32_t key = *((uint32_t *)lv_event_get_param(e));

    switch (key) {
        case LV_KEY_ESC:
            dialog_destruct(&dialog);
            break;
            
        case KEY_VOL_LEFT_EDIT:
        case KEY_VOL_LEFT_SELECT:
            radio_change_vol(-1);
            break;

        case KEY_VOL_RIGHT_EDIT:
        case KEY_VOL_RIGHT_SELECT:
            radio_change_vol(1);
            break;
    }
}

void dialog_swrscan_run_cb(lv_event_t * e) {
    if (run) {
        radio_stop_swrscan();
        run = false;
        mem_load(MEM_BACKUP_ID);
    } else {
        mem_save(MEM_BACKUP_ID);
        do_init();
        radio_set_freq(freq_start);
        run = radio_start_swrscan();
    }
}

void dialog_swrscan_scale_cb(lv_event_t * e) {
    params_lock();
    params.swrscan_linear = !params.swrscan_linear;
    params_unlock(&params.durty.swrscan_linear);

    event_send(chart, LV_EVENT_REFRESH, NULL);
}

void dialog_swrscan_span_cb(lv_event_t * e) {
    if (run) {
        return;
    }

    params_lock();

    switch (params.swrscan_span) {
        case 50000:
            params.swrscan_span = 100000;
            break;
    
        case 100000:
            params.swrscan_span = 200000;
            break;
            
        case 200000:
            params.swrscan_span = 500000;
            break;

        case 500000:
            params.swrscan_span = 50000;
            break;
    }

    params_unlock(&params.durty.swrscan_span);
    do_init();
    event_send(chart, LV_EVENT_REFRESH, NULL);
}

void dialog_swrscan_update(float vswr) {
    do_step(vswr);
}
