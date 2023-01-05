/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
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

lv_style_t  panel_top_style;
lv_style_t  panel_mid_style;
lv_style_t  panel_bottom_style;

lv_color_t  bg_color;

extern lv_font_t    eco_sans_14;
extern lv_font_t    eco_sans_16;
extern lv_font_t    eco_sans_18;
extern lv_font_t    eco_sans_32;
extern lv_font_t    eco_sans_36;
extern lv_font_t    eco_sans_38;

static const uint16_t radius = 25;

static lv_grad_dsc_t    grad_top;
static lv_grad_dsc_t    grad_mid;
static lv_grad_dsc_t    grad_bottom;

void styles_init() {
    bg_color = lv_color_hex(0x0040A0);

    /* Top */
    
    grad_top.dir = LV_GRAD_DIR_VER;
    grad_top.stops_count = 4;
    
    grad_top.stops[0].color = lv_color_lighten(bg_color, 196);
    grad_top.stops[1].color = bg_color;
    grad_top.stops[2].color = bg_color;
    grad_top.stops[3].color = lv_color_darken(bg_color, 196);
    
    grad_top.stops[0].frac  = 90;
    grad_top.stops[1].frac  = (255 + 90)/2 - 30;
    grad_top.stops[2].frac  = (255 + 90)/2 + 30;
    grad_top.stops[3].frac  = 255;

    lv_style_init(&panel_top_style);
    lv_style_set_bg_opa(&panel_top_style, LV_OPA_60);
    lv_style_set_bg_grad(&panel_top_style, &grad_top);

    /* Mid */
    
    grad_mid.dir = LV_GRAD_DIR_VER;
    grad_mid.stops_count = 4;

    grad_mid.stops[0].color = lv_color_lighten(bg_color, 196);
    grad_mid.stops[1].color = bg_color;
    grad_mid.stops[2].color = bg_color;
    grad_mid.stops[3].color = lv_color_darken(bg_color, 196);
    
    grad_mid.stops[0].frac  = 0;
    grad_mid.stops[1].frac  = 128 - 30;
    grad_mid.stops[2].frac  = 128 + 30;
    grad_mid.stops[3].frac  = 255;

    lv_style_init(&panel_mid_style);
    lv_style_set_bg_opa(&panel_mid_style, LV_OPA_60);
    lv_style_set_bg_grad(&panel_mid_style, &grad_mid);

    /* Bottom */
    
    grad_bottom.dir = LV_GRAD_DIR_VER;
    grad_bottom.stops_count = 4;

    grad_bottom.stops[0].color = lv_color_lighten(bg_color, 196);
    grad_bottom.stops[1].color = bg_color;
    grad_bottom.stops[2].color = bg_color;
    grad_bottom.stops[3].color = lv_color_darken(bg_color, 196);
    
    grad_bottom.stops[0].frac  = 0;
    grad_bottom.stops[1].frac  = (0 + 165)/2 - 30;
    grad_bottom.stops[2].frac  = (0 + 165)/2 + 30;
    grad_bottom.stops[3].frac  = 165;

    lv_style_init(&panel_bottom_style);
    lv_style_set_bg_opa(&panel_bottom_style, LV_OPA_60);
    lv_style_set_bg_grad(&panel_bottom_style, &grad_bottom);

    /* * */

    lv_style_init(&background_style);
    lv_style_set_bg_color(&background_style, bg_color);

    lv_style_init(&spectrum_style);
    lv_style_set_bg_color(&spectrum_style, lv_color_hex(0x000000));
    lv_style_set_border_color(&spectrum_style, lv_color_hex(0xAAAAAA));
    lv_style_set_border_width(&spectrum_style, 2);
    lv_style_set_radius(&spectrum_style, radius);
    lv_style_set_clip_corner(&spectrum_style, true);
    lv_style_set_width(&spectrum_style, 800-10);
    lv_style_set_x(&spectrum_style, 5);

    lv_style_init(&freq_style);
    lv_style_set_text_color(&freq_style, lv_color_white());
    lv_style_set_text_font(&freq_style, &eco_sans_16);
    lv_style_set_pad_ver(&freq_style, 10);

    lv_style_init(&freq_main_style);
    lv_style_set_text_color(&freq_main_style, lv_color_white());
    lv_style_set_text_font(&freq_main_style, &eco_sans_36);
    lv_style_set_pad_ver(&freq_main_style, 1);

    lv_style_init(&waterfall_style);
    lv_style_set_bg_color(&waterfall_style, lv_color_hex(0x000000));
    lv_style_set_border_color(&waterfall_style, lv_color_hex(0xAAAAAA));
    lv_style_set_border_width(&waterfall_style, 2);
    lv_style_set_radius(&waterfall_style, radius);
    lv_style_set_clip_corner(&waterfall_style, true);
    lv_style_set_width(&waterfall_style, 800-10);
    lv_style_set_x(&waterfall_style, 5);

    lv_style_init(&btn_style);
    lv_style_set_border_opa(&btn_style, LV_OPA_50);
    lv_style_set_border_color(&btn_style, lv_color_hex(0x000000));
    lv_style_set_border_width(&btn_style, 2);
    lv_style_set_text_font(&btn_style, &eco_sans_16);
    lv_style_set_radius(&btn_style, radius);

    lv_style_init(&msg_style);
    lv_style_set_text_color(&msg_style, lv_color_white());
    lv_style_set_text_font(&msg_style, &eco_sans_38);
    lv_style_set_border_opa(&msg_style, LV_OPA_50);
    lv_style_set_border_color(&msg_style, lv_color_hex(0x000000));
    lv_style_set_border_width(&msg_style, 2);
    lv_style_set_radius(&msg_style, radius);
    lv_style_set_size(&msg_style, 600, 60);
    lv_style_set_x(&msg_style, 800 / 2 - (600 / 2));
    lv_style_set_y(&msg_style, 260);
    lv_style_set_pad_ver(&msg_style, 10);

    lv_style_init(&clock_style);
    lv_style_set_border_color(&clock_style, lv_color_hex(0x000000));
    lv_style_set_border_width(&clock_style, 2);
    lv_style_set_radius(&clock_style, radius);
    lv_style_set_clip_corner(&clock_style, true);
    lv_style_set_width(&clock_style, 200);
    lv_style_set_x(&clock_style, 800 - 200);
    lv_style_set_text_font(&clock_style, &eco_sans_38);
    lv_style_set_text_color(&clock_style, lv_color_white());

    lv_style_init(&info_style);
    lv_style_set_border_color(&info_style, lv_color_hex(0x000000));
    lv_style_set_border_width(&info_style, 2);
    lv_style_set_radius(&info_style, radius);
    lv_style_set_clip_corner(&info_style, true);
    lv_style_set_width(&info_style, 200);
    lv_style_set_x(&info_style, 0);

    lv_style_init(&info_item_style);
    lv_style_set_text_font(&info_item_style, &eco_sans_18);
    lv_style_set_pad_ver(&info_item_style, 3);

    lv_style_init(&meter_style);
    lv_style_set_border_color(&meter_style, lv_color_hex(0x000000));
    lv_style_set_border_width(&meter_style, 2);
    lv_style_set_radius(&meter_style, radius);
    lv_style_set_clip_corner(&meter_style, true);
    lv_style_set_width(&meter_style, 380);
    lv_style_set_x(&meter_style, 800 / 2 - (380 / 2));
}
