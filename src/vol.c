/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include "vol.h"
#include "msg.h"
#include "radio.h"
#include "main.h"
#include "params.h"

static vol_mode_t   vol_mode = VOL_VOL;

void vol_update(int16_t diff) {
    int32_t     x;
    float       f;
    char        *s;

    uint32_t    color = vol->mode == VOL_EDIT ? 0xFFFFFF : 0xBBBBBB;

    switch (vol_mode) {
        case VOL_VOL:
            x = radio_change_vol(diff);
            msg_set_text_fmt("#%3X Volume: %i", color, x);
            break;
            
        case VOL_RFG:
            x = radio_change_rfg(diff);
            msg_set_text_fmt("#%3X RF gain: %i", color, x);
            break;

        case VOL_SQL:
            x = radio_change_sql(diff);
            msg_set_text_fmt("#%3X Voice SQL: %i", color, x);
            break;

        case VOL_FILTER_LOW:
            x = radio_change_filter_low(diff);
            msg_set_text_fmt("#%3X Filter low: %i Hz", color, x);
            break;

        case VOL_FILTER_HIGH:
            x = radio_change_filter_high(diff);
            msg_set_text_fmt("#%3X Filter high: %i Hz", color, x);
            break;

        case VOL_PWR:
            f = radio_change_pwr(diff);
            msg_set_text_fmt("#%3X Power: %0.1f W", color, f);
            break;

        case VOL_MIC:
            x = radio_change_mic(diff);
            
            switch (x) {
                case x6100_mic_builtin:
                    s = "Built-In";
                    break;

                case x6100_mic_handle:
                    s = "Handle";
                    break;
                    
                case x6100_mic_auto:
                    s = "Auto";
                    break;
            }
            
            msg_set_text_fmt("#%3X MIC: %s", color, s);
            break;

        case VOL_HMIC:
            x = radio_change_hmic(diff);
            msg_set_text_fmt("#%3X H-MIC gain: %i", color, x);
            break;

        case VOL_IMIC:
            x = radio_change_imic(diff);
            msg_set_text_fmt("#%3X I-MIC gain: %i", color, x);
            break;

        case VOL_MONI:
            x = radio_change_moni(diff);
            msg_set_text_fmt("#%3X Moni level: %i", color, x);
            break;
            
        default:
            break;
    }
}

void vol_press(int16_t dir) {
    while (true) {
        if (dir > 0) {
            if (vol_mode == VOL_LAST-1) {
                vol_mode = 0;
            } else {
                vol_mode++;
            }
        } else {
            if (vol_mode == 0) {
                vol_mode = VOL_LAST-1;
            } else {
                vol_mode--;
            }
        }
        
        if (params.vol_modes & (1 << vol_mode)) {
            break;
        }
    }

    vol_update(0);
}

void vol_set_mode(vol_mode_t mode) {
    vol_mode = mode;
}
