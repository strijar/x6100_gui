/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#include <stdlib.h>
#include <pthread.h>

#include "styles.h"
#include "spectrum.h"
#include "main.h"
#include "radio.h"

static lv_obj_t         *obj;

static int              grid_min = -80;
static int              grid_max = -40;

static uint16_t         spectrum_size = 512;
static float            *spectrum_buf = NULL;

static pthread_mutex_t  data_mux;

static void spectrum_draw_cb(lv_event_t * e) {
    lv_obj_t            *obj = lv_event_get_target(e);
    lv_draw_ctx_t       *draw_ctx = lv_event_get_draw_ctx(e);
    lv_draw_line_dsc_t  line_dsc;

    lv_draw_line_dsc_init(&line_dsc);
    
    line_dsc.color = lv_color_hex(0xAAAAAA);
    line_dsc.width = 2;

    if (spectrum_buf) {
        lv_coord_t w = lv_obj_get_width(obj);
        lv_coord_t h = lv_obj_get_height(obj);
    
        lv_coord_t x1 = obj->coords.x1;
        lv_coord_t y1 = obj->coords.y1;
        
        lv_point_t prev, current;
        
        prev.x = x1;
        prev.y = y1 + h;

        for (uint16_t i = 0; i < spectrum_size; i++) {
            float v = (spectrum_buf[i] - grid_min) / (grid_max - grid_min);

            /*        
            if (v < 0.0f) {
                v = 0.0f;
            } else if (v > 1.0f) {
                v = 1.0f;
            }
            */

            current.x = x1 + i * w / spectrum_size;
            current.y = y1 + (1.0f - v) * h;
            
            lv_draw_line(draw_ctx, &line_dsc, &prev, &current);
            prev = current;
        }
    }
}

lv_obj_t * spectrum_init() {
    pthread_mutex_init(&data_mux, NULL);

    spectrum_size = 512;
    spectrum_buf = malloc(spectrum_size * sizeof(lv_point_t));

    obj = lv_obj_create(lv_scr_act());

    lv_obj_add_style(obj, &spectrum_style, 0);
    lv_obj_add_event_cb(obj, spectrum_draw_cb, LV_EVENT_DRAW_MAIN, NULL);

    return obj;
}

void spectrum_data(float *data_buf, uint16_t size) {
    for (uint16_t i = 0; i < size; i++)
        spectrum_buf[i] = data_buf[size - i];
    
#ifdef RADIO_THREAD
    lv_lock();
#endif
    lv_obj_invalidate(obj);
#ifdef RADIO_THREAD
    lv_unlock();
#endif
}
