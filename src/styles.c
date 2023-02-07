/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include "styles.h"

lv_style_t  background_style;
lv_style_t  spectrum_style;
lv_style_t  freq_style;
lv_style_t  freq_main_style;
lv_style_t  waterfall_style;
lv_style_t  btn_style;
lv_style_t  msg_style;
lv_style_t  clock_style;
lv_style_t  info_style;
lv_style_t  info_item_style;
lv_style_t  meter_style;
lv_style_t  tx_info_style;

lv_style_t  panel_top_style;
lv_style_t  panel_mid_style;
lv_style_t  panel_bottom_style;
lv_style_t  pannel_style;

lv_color_t  bg_color;

extern const lv_img_dsc_t img_msg;
extern const lv_img_dsc_t img_btn;
extern const lv_img_dsc_t img_top_short;
extern const lv_img_dsc_t img_top_long;
extern const lv_img_dsc_t img_top_big;
extern const lv_img_dsc_t img_pannel;

void styles_init() {
    bg_color = lv_color_hex(0x0040A0);

    /* * */

    lv_style_init(&background_style);
    lv_style_set_bg_color(&background_style, bg_color);

    lv_style_init(&spectrum_style);
    lv_style_set_bg_color(&spectrum_style, lv_color_hex(0x000000));
    lv_style_set_border_color(&spectrum_style, lv_color_hex(0xAAAAAA));
    lv_style_set_border_width(&spectrum_style, 0);
    lv_style_set_radius(&spectrum_style, 0);
    lv_style_set_width(&spectrum_style, 800);
    lv_style_set_x(&spectrum_style, 0);

    lv_style_init(&freq_style);
    lv_style_set_text_color(&freq_style, lv_color_white());
    lv_style_set_text_font(&freq_style, &sony_30);
    lv_style_set_pad_ver(&freq_style, 7);

    lv_style_init(&freq_main_style);
    lv_style_set_text_color(&freq_main_style, lv_color_white());
    lv_style_set_text_font(&freq_main_style, &sony_38);
    lv_style_set_pad_ver(&freq_main_style, 5);

    lv_style_init(&waterfall_style);
    lv_style_set_bg_color(&waterfall_style, lv_color_hex(0x000000));
    lv_style_set_border_color(&waterfall_style, lv_color_hex(0xAAAAAA));
    lv_style_set_border_width(&waterfall_style, 0);
    lv_style_set_radius(&waterfall_style, 0);
    lv_style_set_clip_corner(&waterfall_style, true);
    lv_style_set_width(&waterfall_style, 800);
    lv_style_set_x(&waterfall_style, 0);

    lv_style_init(&btn_style);
    lv_style_set_bg_img_src(&btn_style, &img_btn);
    lv_style_set_bg_img_opa(&btn_style, LV_OPA_COVER);
    lv_style_set_border_width(&btn_style, 0);
    lv_style_set_text_font(&btn_style, &sony_30);
    lv_style_set_text_color(&btn_style, lv_color_white());
    lv_style_set_radius(&btn_style, 0);
    lv_style_set_bg_opa(&btn_style, LV_OPA_0);

    lv_style_init(&msg_style);
    lv_style_set_text_color(&msg_style, lv_color_white());
    lv_style_set_text_font(&msg_style, &sony_38);
    lv_style_set_size(&msg_style, 603, 66);
    lv_style_set_x(&msg_style, 800 / 2 - (603 / 2));
    lv_style_set_y(&msg_style, 300);
    lv_style_set_radius(&msg_style, 0);
    lv_style_set_bg_img_src(&msg_style, &img_msg);
    lv_style_set_bg_img_opa(&msg_style, LV_OPA_COVER);
    lv_style_set_pad_ver(&msg_style, 20);

    lv_style_init(&pannel_style);
    lv_style_set_text_color(&pannel_style, lv_color_white());
    lv_style_set_text_font(&pannel_style, &sony_38);
    lv_style_set_size(&pannel_style, 795, 196);
    lv_style_set_x(&pannel_style, 800 / 2 - (795 / 2));
    lv_style_set_y(&pannel_style, 210);
    lv_style_set_radius(&pannel_style, 0);
    lv_style_set_bg_img_src(&pannel_style, &img_pannel);
    lv_style_set_bg_img_opa(&pannel_style, LV_OPA_COVER);
    lv_style_set_pad_ver(&pannel_style, 10);
    lv_style_set_pad_hor(&pannel_style, 10);

    lv_style_init(&clock_style);
    lv_style_set_text_color(&clock_style, lv_color_white());
    lv_style_set_radius(&clock_style, 0);
    lv_style_set_size(&clock_style, 206, 61);
    lv_style_set_x(&clock_style, 800 - 206);
    lv_style_set_bg_img_src(&clock_style, &img_top_short);
    lv_style_set_bg_img_opa(&clock_style, LV_OPA_COVER);

    lv_style_init(&info_style);
    lv_style_set_radius(&info_style, 0);
    lv_style_set_size(&info_style, 206, 61);
    lv_style_set_x(&info_style, 0);
    lv_style_set_y(&info_style, 0);
    lv_style_set_bg_img_src(&info_style, &img_top_short);
    lv_style_set_bg_img_opa(&info_style, LV_OPA_COVER);
    lv_style_set_pad_ver(&info_style, 0);
    lv_style_set_border_width(&info_style, 0);
    lv_style_set_bg_opa(&info_style, LV_OPA_0);

    lv_style_init(&info_item_style);
    lv_style_set_text_font(&info_item_style, &sony_20);
    lv_style_set_pad_ver(&info_item_style, 5);
    lv_style_set_radius(&info_item_style, 0);

    lv_style_init(&meter_style);
    lv_style_set_radius(&meter_style, 0);
    lv_style_set_size(&meter_style, 377, 61);
    lv_style_set_x(&meter_style, 800 / 2 - (377 / 2));
    lv_style_set_border_width(&meter_style, 0);
    lv_style_set_bg_img_src(&meter_style, &img_top_long);
    lv_style_set_bg_img_opa(&meter_style, LV_OPA_COVER);
    lv_style_set_bg_opa(&meter_style, LV_OPA_0);

    lv_style_init(&tx_info_style);
    lv_style_set_radius(&tx_info_style, 0);
    lv_style_set_size(&tx_info_style, 377, 123);
    lv_style_set_x(&tx_info_style, 800 / 2 - (377 / 2));
    lv_style_set_border_width(&tx_info_style, 0);
    lv_style_set_bg_img_src(&tx_info_style, &img_top_big);
    lv_style_set_bg_img_opa(&tx_info_style, LV_OPA_COVER);
    lv_style_set_bg_opa(&tx_info_style, LV_OPA_0);
}
