/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include "lvgl/lvgl.h"
#include "dialog_settings.h"
#include "styles.h"
#include "main_screen.h"
#include "keyboard.h"

static lv_obj_t     *obj;

static void key_cb(lv_event_t * e) {
    uint32_t key = *((uint32_t *)lv_event_get_param(e));

    switch (key) {
        case LV_KEY_ESC:
            main_screen_keys_enable(true);
            lv_obj_del(obj);
            break;
    }
}

lv_obj_t * dialog_init(lv_obj_t *parent) {
    obj = lv_obj_create(parent);
    
    lv_obj_remove_style_all(obj);
    lv_obj_add_style(obj, &dialog_style, 0);

    main_screen_keys_enable(false);

    lv_obj_add_event_cb(obj, key_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(keyboard_group(), obj);

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    
    return obj;
}

void dialog_item(lv_obj_t *obj) {
    lv_obj_add_style(obj, &dialog_item_style, LV_STATE_DEFAULT);
    lv_obj_add_style(obj, &dialog_item_focus_style, LV_STATE_FOCUSED);
    lv_obj_add_style(obj, &dialog_item_edited_style, LV_STATE_EDITED);

    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, LV_PART_CURSOR);
    lv_obj_set_style_text_color(obj, lv_color_white(), LV_PART_CURSOR);
    lv_obj_set_style_text_color(obj, lv_color_black(), LV_PART_CURSOR | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_opa(obj, 128, LV_PART_CURSOR | LV_STATE_EDITED);

    lv_group_add_obj(keyboard_group(), obj);
    lv_obj_add_event_cb(obj, key_cb, LV_EVENT_KEY, NULL);
}
