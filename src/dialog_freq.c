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
#include "dialog_freq.h"
#include "styles.h"
#include "radio.h"
#include "events.h"
#include "util.h"
#include "keyboard.h"
#include "params.h"
#include "bands.h"
#include "info.h"
#include "pannel.h"
#include "main_screen.h"

static lv_obj_t *text;

static void construct_cb(lv_obj_t *parent);
static void key_cb(lv_event_t * e);

static dialog_t             dialog = {
    .run = false,
    .construct_cb = construct_cb,
    .destruct_cb = NULL,
    .key_cb = key_cb
};

dialog_t                    *dialog_freq = &dialog;

static void construct_cb(lv_obj_t *parent) {
    lv_obj_t *obj = lv_obj_create(parent);

    lv_obj_remove_style_all(obj);

    lv_obj_add_style(obj, &msg_tiny_style, 0);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    dialog.obj = obj;

    text = lv_textarea_create(dialog.obj);

    lv_obj_remove_style(text, NULL, LV_STATE_ANY | LV_PART_MAIN);

    lv_obj_set_style_text_color(text, lv_color_white(), 0);
    lv_obj_set_style_bg_color(text, lv_color_white(), LV_PART_CURSOR);
    lv_obj_set_style_bg_opa(text, 255, LV_PART_CURSOR);
    
    lv_textarea_set_one_line(text, true);
    lv_textarea_set_accepted_chars(text, "0123456789.");
    lv_textarea_set_max_length(text, 9);
    lv_textarea_set_placeholder_text(text, "Freq in MHz");
    
    lv_obj_clear_flag(text, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_text_font(text, &sony_44, 0);

    lv_group_add_obj(keyboard_group, text);
    lv_obj_add_event_cb(text, key_cb, LV_EVENT_KEY, NULL);

    lv_obj_set_height(text, 35);
    lv_obj_set_width(text, 250);
    lv_obj_center(text);
}

static void enter_freq() {
    const char* str = lv_textarea_get_text(text);
    float       f = atof(str);
    
    if (f > 1.0f && f < 55.0f) {
        main_screen_set_freq(f * 1000000L);
    }
}

static void key_cb(lv_event_t * e) {
    uint32_t key = *((uint32_t *)lv_event_get_param(e));

    switch (key) {
        case HKEY_FINP:
        case LV_KEY_ENTER:
            enter_freq();
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
