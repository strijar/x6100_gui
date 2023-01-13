/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include "lvgl/lvgl.h"
#include "mfk.h"
#include "params.h"
#include "spectrum.h"
#include "waterfall.h"
#include "msg.h"
#include "dsp.h"
#include "radio.h"

mfk_mode_t   mfk_mode = MFK_MIN_LEVEL;

void mfk_update(int16_t diff) {
    int32_t i;
    char    *str;
    bool    b;

    switch (mfk_mode) {
        case MFK_MIN_LEVEL:
            if (diff != 0) {
                params_lock();
                params_band.grid_min += diff;
                params_unlock(&params_band.durty.grid_min);
                
                spectrum_set_min(params_band.grid_min);
                waterfall_set_min(params_band.grid_min);
            }
            msg_set_text_fmt("Min level: %idb", params_band.grid_min);
            break;
            
        case MFK_MAX_LEVEL:
            if (diff != 0) {
                params_lock();
                params_band.grid_max += diff;
                params_unlock(&params_band.durty.grid_max);
                
                spectrum_set_max(params_band.grid_max);
                waterfall_set_max(params_band.grid_max);
            }
            msg_set_text_fmt("Max level: %idb", params_band.grid_max);
            break;

        case MFK_SPECTRUM_FACTOR:
            if (diff != 0) {
                params_lock();
                params.spectrum_factor += diff;
                
                if (params.spectrum_factor < 1) {
                    params.spectrum_factor = 1;
                } else if (params.spectrum_factor > 4) {
                    params.spectrum_factor = 4;
                }
                params_unlock(&params.durty.spectrum_factor);
            
                dsp_set_spectrum_factor(params.spectrum_factor);
            }
            msg_set_text_fmt("Spectrum zoom: x%i", params.spectrum_factor);
            break;

        case MFK_SPECTRUM_BETA:
            if (diff != 0) {
                params_lock();
                params.spectrum_beta += (diff < 0) ? -5 : 5;
                
                if (params.spectrum_beta < 0) {
                    params.spectrum_beta = 0;
                } else if (params.spectrum_beta > 90) {
                    params.spectrum_beta = 90;
                }
                params_unlock(&params.durty.spectrum_beta);
            
                dsp_set_spectrum_beta(params.spectrum_beta / 100.0f);
            }
            msg_set_text_fmt("Spectrum beta: %i", params.spectrum_beta);
            break;
            
        case MFK_KEY_SPEED:
            i = radio_change_key_speed(diff);
            msg_set_text_fmt("Key speed: %i wpm", i);
            break;

        case MFK_KEY_MODE:
            i = radio_change_key_mode(diff);
            
            switch (i) {
                case x6100_key_manual:
                    str = "Manual";
                    break;

                case x6100_key_auto_left:
                    str = "Auto-L";
                    break;

                case x6100_key_auto_right:
                    str = "Auto-R";
                    break;
            }
            msg_set_text_fmt("Key mode: %s", str);
            break;
            
        case MFK_IAMBIC_MODE:
            i = radio_change_iambic_mode(diff);
            
            switch (i) {
                case x6100_iambic_a:
                    str = "A";
                    break;

                case x6100_iambic_b:
                    str = "B";
                    break;
            }
            msg_set_text_fmt("Iambic mode: %s", str);
            break;
            
        case MFK_KEY_TONE:
            i = radio_change_key_tone(diff);
            msg_set_text_fmt("Key tone: %i Hz", i);
            break;

        case MFK_KEY_VOL:
            i = radio_change_key_vol(diff);
            msg_set_text_fmt("Key volume: %i", i);
            break;

        case MFK_KEY_TRAIN:
            b = radio_change_key_train(diff);
            msg_set_text_fmt("Key train: %s", b ? "On" : "Off");
            break;

        case MFK_QSK_TIME:
            i = radio_change_qsk_time(diff);
            msg_set_text_fmt("QSK time: %i ms", i);
            break;

        case MFK_KEY_RATIO:
            i = radio_change_key_ratio(diff);
            msg_set_text_fmt("Key ratio: %.1f", i * 0.1f);
            break;
    }
}

void mfk_press(int16_t dir) {
    mfk_mode = (mfk_mode + dir) % MFK_LAST;
    mfk_update(0);
}

void mfk_set_mode(mfk_mode_t mode) {
    mfk_mode = mode;
}
