/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */
 
#include <stdio.h>

#include "tx_info.h"
#include "styles.h"
#include "events.h"

static uint8_t          over = 25;
static uint8_t          height = 54 * 2;

static float            pwr = 10.0f;
static float            vswr = 5.0f;
static float            alc;

static lv_obj_t         *obj;
static lv_grad_dsc_t    grad;

static void tx_info_draw_cb(lv_event_t * e) {
    lv_obj_t            *obj = lv_event_get_target(e);
    lv_draw_ctx_t       *draw_ctx = lv_event_get_draw_ctx(e);
    lv_draw_rect_dsc_t  rect_dsc;
    lv_draw_label_dsc_t label_dsc;
    lv_area_t           area;
    char                buf[8];

    lv_coord_t x1 = obj->coords.x1 + 10;
    lv_coord_t y1 = obj->coords.y1 + over + 5;

    lv_coord_t w = lv_obj_get_width(obj);
    lv_coord_t h = lv_obj_get_height(obj) - 1;

    lv_coord_t len = (w - 70 - 30);
    
    /* SWR Labels */
    
    lv_draw_label_dsc_init(&label_dsc);
    
    label_dsc.color = lv_color_white();
    label_dsc.font = &eco_sans_32;

    area.x1 = x1;
    area.y1 = y1;
    area.x2 = x1 + 50;
    area.y2 = y1 + 32;

    lv_draw_label(draw_ctx, &label_dsc, &area, "SWR", NULL);

    label_dsc.font = &eco_sans_18;

    for (int i = 1; i <= 5; i++) {
        area.x1 = x1 + 60 + len * i / 5;
        area.y1 = y1 + 15 - 9;
        area.x2 = area.x1 + 20;
        area.y2 = area.y1 + 16;

        snprintf(buf, sizeof(buf), i == 5 ? ">%i" : "%i", i);
        lv_draw_label(draw_ctx, &label_dsc, &area, buf, NULL);
    }

    /* PWR Labels */

    label_dsc.font = &eco_sans_32;

    area.x1 = x1;
    area.y1 = y1 + 54;
    area.x2 = x1 + 50;
    area.y2 = y1 + 32 + 54;
    
    lv_draw_label(draw_ctx, &label_dsc, &area, "PWR", NULL);

    label_dsc.font = &eco_sans_18;

    for (int i = 2; i <= 10; i += 2) {
        area.x1 = x1 + 60 + len * i / 10;
        area.y1 = y1 + 54 + 15 - 9;
        area.x2 = area.x1 + 20;
        area.y2 = area.y1 + 16;

        snprintf(buf, sizeof(buf), "%i", i);
        lv_draw_label(draw_ctx, &label_dsc, &area, buf, NULL);
    }
    
    /* Rects */

    lv_draw_rect_dsc_init(&rect_dsc);

    rect_dsc.bg_grad = grad;
    rect_dsc.bg_opa = LV_OPA_50;
    rect_dsc.radius = 10;
    rect_dsc.border_width = 2;
    rect_dsc.border_color = lv_color_black();
    rect_dsc.border_opa = LV_OPA_50;

    area.x1 = x1 + 70;
    area.y1 = y1;
    area.x2 = area.x1 + len * vswr / 5.0f;
    area.y2 = y1 + 32;

    lv_draw_rect(draw_ctx, &rect_dsc, &area);

    area.y1 += 54;
    area.x2 = area.x1 + len * pwr / 10.0f;
    area.y2 += 54;

    lv_draw_rect(draw_ctx, &rect_dsc, &area);
}

static void tx_cb(lv_event_t * e) {
    pwr = 0;
    vswr = 0;
    alc = 0;

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
}

static void rx_cb(lv_event_t * e) {
    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
}

lv_obj_t * tx_info_init(lv_obj_t *parent) {
    obj = lv_obj_create(parent);

    lv_obj_add_style(obj, &panel_mid_style, 0);
    lv_obj_add_style(obj, &meter_style, 0);
    
    lv_obj_set_height(obj, height + over);
    lv_obj_set_y(obj, -over);
    
    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(obj, tx_cb, EVENT_RADIO_TX, NULL);
    lv_obj_add_event_cb(obj, rx_cb, EVENT_RADIO_RX, NULL);
    lv_obj_add_event_cb(obj, tx_info_draw_cb, LV_EVENT_DRAW_MAIN_END, NULL);

    grad.dir = LV_GRAD_DIR_VER;
    grad.stops_count = 4;

    grad.stops[0].color = lv_color_lighten(bg_color, 200);
    grad.stops[1].color = bg_color;
    grad.stops[2].color = bg_color;
    grad.stops[3].color = lv_color_darken(bg_color, 200);
    
    grad.stops[0].frac  = 0;
    grad.stops[1].frac  = 128 - 10;
    grad.stops[2].frac  = 128 + 10;
    grad.stops[3].frac  = 255;
   
    return obj;
}

void tx_info_update(float p, float s, float a) {
    pwr = p;
    alc = a;

    if (s <= 5.0f) {
        vswr = s;
    } else {
        vswr = 5.0f;
    }
    
    event_send(obj, LV_EVENT_REFRESH, NULL);
}
