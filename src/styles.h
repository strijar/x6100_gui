/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#pragma once

#include <unistd.h>
#include "lvgl/lvgl.h"

extern lv_color_t   bg_color;

extern lv_style_t   background_style;
extern lv_style_t   spectrum_style;
extern lv_style_t   freq_style;
extern lv_style_t   freq_main_style;
extern lv_style_t   waterfall_style;
extern lv_style_t   btn_style;
extern lv_style_t   msg_style;
extern lv_style_t   clock_style;
extern lv_style_t   info_style;
extern lv_style_t   info_item_style;
extern lv_style_t   meter_style;

extern lv_style_t   panel_top_style;
extern lv_style_t   panel_mid_style;
extern lv_style_t   panel_bottom_style;

extern lv_font_t    eco_sans_14;
extern lv_font_t    eco_sans_16;
extern lv_font_t    eco_sans_18;
extern lv_font_t    eco_sans_32;
extern lv_font_t    eco_sans_36;
extern lv_font_t    eco_sans_38;

void styles_init();
