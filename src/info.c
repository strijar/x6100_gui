/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include "info.h"
#include "styles.h"
#include "params.h"

typedef enum {
    INFO_VFO = 0,
    INFO_MODE,
    INFO_AGC,
    INFO_PRE,
    INFO_ATT,
    INFO_ATU
} info_items_t;

static uint8_t      over = 25;
static uint8_t      info_height = 54;

static lv_obj_t     *obj;
static lv_obj_t     *items[6];

lv_obj_t * info_init(lv_obj_t * parent) {
    obj = lv_obj_create(parent);

    lv_obj_add_style(obj, &panel_top_style, 0);
    lv_obj_add_style(obj, &info_style, 0);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_height(obj, info_height + over);
    lv_obj_set_style_pad_top(obj, over, 0);
    lv_obj_set_y(obj, -over);

    lv_obj_set_style_pad_left(obj, 0, 0);
    
    uint8_t index = 0;
    
    for (uint8_t y = 0; y < 2; y++)
        for (uint8_t x = 0; x < 3; x++) {
            lv_obj_t *item = lv_label_create(obj);
            
            lv_obj_add_style(item, &info_item_style, 0);
            lv_obj_set_pos(item, x * 67, y * 25);
            lv_obj_set_style_text_align(item, LV_TEXT_ALIGN_CENTER, 0);
            
            lv_obj_set_style_text_color(item, lv_color_white(), 0);
            lv_obj_set_size(item, 190/3, 25);

            items[index] = item;
            index++;
        }

    lv_label_set_text(items[INFO_PRE], "PRE");
    lv_label_set_text(items[INFO_ATT], "ATT");
    lv_label_set_text(items[INFO_ATU], "ATU");
   
    info_params_set(); 
    return obj;
}

void info_params_set() {
    bool            vfoa = (params_band.vfo == X6100_VFO_A);
    x6100_mode_t    mode = vfoa ? params_band.vfoa_mode : params_band.vfob_mode;
    x6100_agc_t     agc = vfoa ? params_band.vfoa_agc : params_band.vfob_agc;
    x6100_att_t     att = vfoa ? params_band.vfoa_att : params_band.vfob_att;
    x6100_pre_t     pre = vfoa ? params_band.vfoa_pre : params_band.vfob_pre;

    char            *str;

    lv_label_set_text(items[INFO_VFO], vfoa ? "VFO-A" : "VFO-B");

    switch (mode) {
        case x6100_mode_lsb:
            str = "LSB";
            break;
            
        case x6100_mode_lsb_dig:
            str = "LSB-D";
            break;
            
        case x6100_mode_usb:
            str = "USB";
            break;
            
        case x6100_mode_usb_dig:
            str = "USB-D";
            break;
            
        case x6100_mode_cw:
            str = "CW";
            break;
            
        case x6100_mode_cwr:
            str = "CW-R";
            break;

        case x6100_mode_am:
            str = "AM";
            break;
            
        case x6100_mode_nfm:
            str = "NFM";
            break;
            
        default:
            str = "?";
            break;
    }
    
    lv_label_set_text(items[INFO_MODE], str);

    switch (agc) {
        case x6100_agc_off:
            str = "OFF";
            break;
            
        case x6100_agc_slow:
            str = "SLOW";
            break;
            
        case x6100_agc_fast:
            str = "FAST";
            break;
            
        case x6100_agc_auto:
            str = "AUTO";
            break;
            
        default:
            str = "?";
            break;

    }

    lv_label_set_text(items[INFO_AGC], str);

    if (att == x6100_att_off) {
        lv_obj_set_style_text_color(items[INFO_ATT], lv_color_white(), 0);
        lv_obj_set_style_bg_color(items[INFO_ATT], lv_color_black(), 0);
        lv_obj_set_style_bg_opa(items[INFO_ATT], LV_OPA_0, 0);
    } else {
        lv_obj_set_style_text_color(items[INFO_ATT], lv_color_black(), 0);
        lv_obj_set_style_bg_color(items[INFO_ATT], lv_color_white(), 0);
        lv_obj_set_style_bg_opa(items[INFO_ATT], LV_OPA_50, 0);
    }

    if (pre == x6100_pre_off) {
        lv_obj_set_style_text_color(items[INFO_PRE], lv_color_white(), 0);
        lv_obj_set_style_bg_color(items[INFO_PRE], lv_color_black(), 0);
        lv_obj_set_style_bg_opa(items[INFO_PRE], LV_OPA_0, 0);
    } else {
        lv_obj_set_style_text_color(items[INFO_PRE], lv_color_black(), 0);
        lv_obj_set_style_bg_color(items[INFO_PRE], lv_color_white(), 0);
        lv_obj_set_style_bg_opa(items[INFO_PRE], LV_OPA_50, 0);
    }
}
