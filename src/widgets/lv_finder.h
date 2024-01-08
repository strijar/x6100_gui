/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#ifndef LV_FINDER_H
#define LV_FINDER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lvgl/lvgl.h"

/**********************
 *      TYPEDEFS
 **********************/

#define LV_FINDER_CURSORS    2

typedef struct {
    lv_obj_t        obj;

    uint16_t        width;
    int16_t         value;

    int16_t         range_min;
    int16_t         range_max;
    
    uint8_t         cursor_num;
    int16_t         cursor[LV_FINDER_CURSORS];
} lv_finder_t;

extern const lv_obj_class_t lv_finder_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * lv_finder_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

void lv_finder_set_range(lv_obj_t * obj, int16_t min, int16_t max);
void lv_finder_set_cursor(lv_obj_t * obj, uint8_t index, int16_t value);
void lv_finder_set_width(lv_obj_t * obj, uint16_t x);
void lv_finder_set_value(lv_obj_t * obj, int16_t x);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
