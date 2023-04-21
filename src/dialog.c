/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include "lvgl/lvgl.h"
#include "dialog.h"
#include "styles.h"
#include "main_screen.h"
#include "keyboard.h"

static lv_obj_t     *obj;

dialog_t * dialog_construct(dialog_t *dialog, lv_obj_t *parent) {
    if (dialog && !dialog->run) {
        main_screen_keys_enable(false);
        dialog->construct_cb(parent);
        
        dialog->run = true;
    }
    
    return dialog;
}

void dialog_destruct(dialog_t *dialog) {
    if (dialog && dialog->run) {
        dialog->run = false;
        
        if (dialog->destruct_cb) {
            dialog->destruct_cb();
        }

        lv_obj_del(dialog->obj);
        main_screen_dialog_deleted_cb();
        main_screen_keys_enable(true);
    }
}

bool dialog_key(dialog_t *dialog, lv_event_t * e) {
    if (dialog && dialog->key_cb && dialog->run) {
        dialog->key_cb(e);
        return true;
    }
    
    return false;
}

bool dialog_is_run(dialog_t *dialog) {
    return (dialog != NULL) && dialog->run;
}

lv_obj_t * dialog_init(lv_obj_t *parent) {
    obj = lv_obj_create(parent);
    
    lv_obj_remove_style_all(obj);
    lv_obj_add_style(obj, &dialog_style, 0);

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    return obj;
}

void dialog_item(dialog_t *dialog, lv_obj_t *obj) {
    lv_obj_add_style(obj, &dialog_item_style, LV_STATE_DEFAULT);
    lv_obj_add_style(obj, &dialog_item_focus_style, LV_STATE_FOCUSED);
    lv_obj_add_style(obj, &dialog_item_edited_style, LV_STATE_EDITED);

    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, LV_PART_CURSOR);
    lv_obj_set_style_text_color(obj, lv_color_white(), LV_PART_CURSOR);
    lv_obj_set_style_text_color(obj, lv_color_black(), LV_PART_CURSOR | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_opa(obj, 128, LV_PART_CURSOR | LV_STATE_EDITED);

    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    
    lv_group_add_obj(keyboard_group(), obj);
    
    if (dialog->key_cb) {
        lv_obj_add_event_cb(obj, dialog->key_cb, LV_EVENT_KEY, NULL);
    }
}
