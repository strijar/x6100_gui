/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_finder.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_finder_class

/**********************
 *  STATIC PROTOTYPES
 **********************/
 
static void lv_finder_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_finder_event(const lv_obj_class_t * class_p, lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_finder_class = {
    .constructor_cb = lv_finder_constructor,
    .base_class = &lv_obj_class,
    .event_cb = lv_finder_event,
    .instance_size = sizeof(lv_finder_t),
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_finder_create(lv_obj_t * parent) {
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);

    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_finder_set_range(lv_obj_t * obj, int16_t min, int16_t max) {
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_finder_t * finder = (lv_finder_t *)obj;
    
    finder->range_min = min;
    finder->range_max = max;
}

void lv_finder_set_cursor(lv_obj_t * obj, uint8_t index, int16_t value) {
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_finder_t * finder = (lv_finder_t *)obj;
    
    if (index > 0 && index <= LV_FINDER_CURSORS) {
        finder->cursor[index - 1] = value;
        
        if (index > finder->cursor_num) {
            finder->cursor_num = index;
        }
    }
}

void lv_finder_set_width(lv_obj_t * obj, uint16_t x) {
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_finder_t * finder = (lv_finder_t *)obj;
    
    finder->width = x;
}

void lv_finder_set_value(lv_obj_t * obj, int16_t x) {
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_finder_t * finder = (lv_finder_t *)obj;
    
    finder->value = x;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_finder_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj) {
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_finder_t * finder = (lv_finder_t *)obj;

    finder->value = 1000;
    finder->width = 50;
    finder->range_min = 50;
    finder->range_max = 3000;
    finder->cursor_num = 0;
    
    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_finder_event(const lv_obj_class_t * class_p, lv_event_t * e) {
    LV_UNUSED(class_p);

    lv_res_t res = lv_obj_event_base(MY_CLASS, e);

    if (res != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if (code == LV_EVENT_DRAW_MAIN_END) {
        lv_finder_t     *finder = (lv_finder_t *) obj;
        lv_draw_ctx_t   *draw_ctx = lv_event_get_draw_ctx(e);
        lv_area_t       area;

        lv_coord_t      x1 = obj->coords.x1;
        lv_coord_t      y1 = obj->coords.y1;
        lv_coord_t      w = lv_obj_get_width(obj);
        lv_coord_t      h = lv_obj_get_height(obj);
        uint16_t        border = lv_obj_get_style_border_width(obj, LV_PART_INDICATOR);

        int32_t         size_hz = finder->range_max - finder->range_min;
        int32_t         f = finder->value - finder->range_min;
        int64_t         f1 = w * (f - finder->width / 2) / size_hz;
        int64_t         f2 = w * (f + finder->width / 2) / size_hz;

        area.x1 = x1 + f1;
        area.y1 = y1 + border;
        area.x2 = x1 + f2;
        area.y2 = area.y1 + h - border * 2;

        /* Rectangle */

        lv_draw_rect_dsc_t  draw_dsc;

        lv_draw_rect_dsc_init(&draw_dsc);
        lv_obj_init_draw_rect_dsc(obj, LV_PART_INDICATOR, &draw_dsc);

        lv_draw_rect(draw_ctx, &draw_dsc, &area);

        /* Cursors */
        
        if (finder->cursor_num) {
            lv_draw_line_dsc_t  line_dsc;

            lv_draw_line_dsc_init(&line_dsc);
            lv_obj_init_draw_line_dsc(obj, LV_PART_INDICATOR, &line_dsc);
    
            lv_point_t a, b;

            for (uint8_t i = 0; i < finder->cursor_num; i++) {
                a.x = x1 + w * (f + finder->cursor[i]) / size_hz;
                a.y = area.y1;
                
                b.x = a.x;
                b.y = area.y2;
            
                lv_draw_line(draw_ctx, &line_dsc, &a, &b);
            }
        }
    }
}
