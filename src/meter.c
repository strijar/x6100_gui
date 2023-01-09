/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include "meter.h"
#include "styles.h"
#include "events.h"

static int16_t          min_db = -121;
static int16_t          max_db = -73 + 40;

static uint8_t          over = 25;
static uint8_t          meter_height = 54;
static int16_t          meter_db = -121;

static lv_obj_t         *obj;
static lv_grad_dsc_t    grad;

static void meter_draw_cb(lv_event_t * e) {
    lv_obj_t            *obj = lv_event_get_target(e);
    lv_draw_ctx_t       *draw_ctx = lv_event_get_draw_ctx(e);
    lv_draw_rect_dsc_t  rect_dsc;
    lv_draw_label_dsc_t label_dsc;
    lv_area_t           area;
    char                buf[8];

    lv_coord_t x1 = obj->coords.x1 + 15;
    lv_coord_t y1 = obj->coords.y1 + over + 5;

    lv_coord_t w = lv_obj_get_width(obj);
    lv_coord_t h = lv_obj_get_height(obj) - 1;

    lv_coord_t len = (w - 70);
    
    /* Labels */
    
    lv_draw_label_dsc_init(&label_dsc);
    
    label_dsc.color = lv_color_white();
    label_dsc.font = &eco_sans_32;

    area.x1 = x1;
    area.y1 = y1;
    area.x2 = x1 + 20;
    area.y2 = y1 + 32;

    lv_draw_label(draw_ctx, &label_dsc, &area, "S", NULL);

    label_dsc.font = &eco_sans_18;

    lv_point_t label_size;

    area.y1 = y1 + 15 - 9;
    area.y2 = area.y1 + 18;

    int db = min_db + 6;
    int s = 2;

    while (db <= max_db) {
        if (s <= 9 ) {
            snprintf(buf, sizeof(buf), "%i", s);
        } else if (s == 10) {
            strcpy(buf, "+20");
        } else {
            strcpy(buf, "+40");
        }

        lv_txt_get_size(&label_size, buf, label_dsc.font, 0, 0, LV_COORD_MAX, 0);

        area.x1 = x1 + 30 + len * (db  - min_db) / (max_db - min_db) - (label_size.x / 2);
        area.x2 = area.x1 + label_size.x;

        lv_draw_label(draw_ctx, &label_dsc, &area, buf, NULL);

        s++;

        if (s < 10) {
            db += 6;
        } else if (s == 10) {
            db += 6 + 4 + 10;
        } else {
            db += 20;
        }
    }

    /* Rect */

    lv_draw_rect_dsc_init(&rect_dsc);

    rect_dsc.bg_grad = grad;
    rect_dsc.bg_opa = LV_OPA_50;
    rect_dsc.radius = 10;
    rect_dsc.border_width = 2;
    rect_dsc.border_color = lv_color_white();
    rect_dsc.border_opa = LV_OPA_50;

    area.x1 = x1 + 30;
    area.y1 = y1;
    area.x2 = area.x1 + len * (meter_db - min_db) / (max_db - min_db);
    area.y2 = y1 + 32;

    lv_draw_rect(draw_ctx, &rect_dsc, &area);
}

static void tx_cb(lv_event_t * e) {
    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
}

static void rx_cb(lv_event_t * e) {
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
}

lv_obj_t * meter_init(lv_obj_t * parent) {
    obj = lv_obj_create(parent);

    lv_obj_add_style(obj, &panel_top_style, 0);
    lv_obj_add_style(obj, &meter_style, 0);
    
    lv_obj_set_height(obj, meter_height + over);
    lv_obj_set_style_pad_top(obj, over, 0);
    lv_obj_set_y(obj, -over);

    lv_obj_add_event_cb(obj, tx_cb, EVENT_RADIO_TX, NULL);
    lv_obj_add_event_cb(obj, rx_cb, EVENT_RADIO_RX, NULL);
    lv_obj_add_event_cb(obj, meter_draw_cb, LV_EVENT_DRAW_MAIN_END, NULL);

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

void meter_update(int16_t db) {
    if (db < min_db) {
        db = min_db;
    } else if (db > max_db) {
        db = max_db;
    }
    
    meter_db = meter_db * 0.8 + db * 0.2;
    event_send(obj, LV_EVENT_REFRESH, NULL);
}
