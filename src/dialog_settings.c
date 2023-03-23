/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include "lvgl/lvgl.h"
#include "dialog.h"
#include "dialog_settings.h"
#include "styles.h"

static lv_obj_t     *dialog;
static lv_obj_t     *grid;

static lv_coord_t   col_dsc[] = { 330, 125, 125, 125, LV_GRID_TEMPLATE_LAST };
static lv_coord_t   row_dsc[] = { 54, 54, 54, 54, 54, 54, 54, 54, LV_GRID_TEMPLATE_LAST };

static uint8_t make_date(uint8_t row) {
    lv_obj_t    *obj;
    uint8_t     col = 0;

    /* Label */

    obj = lv_label_create(grid);

    lv_label_set_text(obj, "Day, Month, Year");
    lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_START, col++, 1, LV_GRID_ALIGN_CENTER, row, 1);

    /* Day */

    obj = lv_spinbox_create(grid);

    dialog_item(obj);

    lv_spinbox_set_value(obj, row);
    lv_spinbox_set_range(obj, 1, 32);
    lv_spinbox_set_digit_format(obj, 2, 0);
    lv_spinbox_set_digit_step_direction(obj, LV_DIR_LEFT);
    lv_obj_set_size(obj, 120, 56);
    
    lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_START, col++, 1, LV_GRID_ALIGN_CENTER, row, 1);

    /* Month */

    obj = lv_spinbox_create(grid);

    dialog_item(obj);

    lv_spinbox_set_value(obj, row);
    lv_spinbox_set_range(obj, 1, 12);
    lv_spinbox_set_digit_format(obj, 2, 0);
    lv_spinbox_set_digit_step_direction(obj, LV_DIR_LEFT);
    lv_obj_set_size(obj, 120, 56);
    
    lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_START, col++, 1, LV_GRID_ALIGN_CENTER, row, 1);

    /* Year */

    obj = lv_spinbox_create(grid);

    dialog_item(obj);

    lv_spinbox_set_value(obj, row);
    lv_spinbox_set_range(obj, 2020, 2038);
    lv_spinbox_set_digit_format(obj, 4, 0);
    lv_spinbox_set_digit_step_direction(obj, LV_DIR_LEFT);
    lv_obj_set_size(obj, 120, 56);
    
    lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_START, col++, 1, LV_GRID_ALIGN_CENTER, row, 1);

    return row + 1;
}

static uint8_t make_time(uint8_t row) {
    lv_obj_t    *obj;
    uint8_t     col = 0;

    /* Label */

    obj = lv_label_create(grid);

    lv_label_set_text(obj, "Hour, Min, Sec");
    lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_START, col++, 1, LV_GRID_ALIGN_CENTER, row, 1);

    /* Hour */

    obj = lv_spinbox_create(grid);

    dialog_item(obj);

    lv_spinbox_set_value(obj, row);
    lv_spinbox_set_range(obj, 0, 23);
    lv_spinbox_set_digit_format(obj, 2, 0);
    lv_spinbox_set_digit_step_direction(obj, LV_DIR_LEFT);
    lv_obj_set_size(obj, 120, 56);
    
    lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_START, col++, 1, LV_GRID_ALIGN_CENTER, row, 1);

    /* Min */

    obj = lv_spinbox_create(grid);

    dialog_item(obj);

    lv_spinbox_set_value(obj, row);
    lv_spinbox_set_range(obj, 0, 59);
    lv_spinbox_set_digit_format(obj, 2, 0);
    lv_spinbox_set_digit_step_direction(obj, LV_DIR_LEFT);
    lv_obj_set_size(obj, 120, 56);
    
    lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_START, col++, 1, LV_GRID_ALIGN_CENTER, row, 1);

    /* Sec */

    obj = lv_spinbox_create(grid);

    dialog_item(obj);

    lv_spinbox_set_value(obj, row);
    lv_spinbox_set_range(obj, 0, 59);
    lv_spinbox_set_digit_format(obj, 2, 0);
    lv_spinbox_set_digit_step_direction(obj, LV_DIR_LEFT);
    lv_obj_set_size(obj, 120, 56);
    
    lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_START, col++, 1, LV_GRID_ALIGN_CENTER, row, 1);

    return row + 1;
}

lv_obj_t * dialog_settings(lv_obj_t *parent) {
    dialog = dialog_init(parent);
    grid = lv_obj_create(dialog);
    
    lv_obj_set_grid_dsc_array(grid, col_dsc, row_dsc);
    lv_obj_set_size(grid, 780, 330);
    lv_obj_set_layout(grid, LV_LAYOUT_GRID);
    
    lv_obj_set_style_text_color(grid, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(grid, 0, LV_PART_MAIN);

    lv_obj_center(grid);

    uint8_t row = 0;
    
    row = make_date(row);
    row = make_time(row);

    return dialog;
}
