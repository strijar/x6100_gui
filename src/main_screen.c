/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#include "lvgl/lvgl.h"
#include "main_screen.h"
#include "styles.h"
#include "spectrum.h"
#include "waterfall.h"
#include "util.h"
#include "radio.h"
#include "main.h"
#include "events.h"
#include "msg.h"
#include "dsp.h"
#include "params.h"
#include "bands.h"
#include "clock.h"
#include "info.h"
#include "meter.h"

typedef enum {
    VOL_VOL = 0,
    VOL_RFG,
    VOL_FILTER_LOW,
    VOL_FILTER_HIGH,
    
    VOL_LAST
} vol_mode_t;

typedef enum {
    MFK_MIN_LEVEL = 0,
    MFK_MAX_LEVEL,
    MFK_SPECTRUM_FACTOR,
    MFK_SPECTRUM_BETA,
    
    MFK_LAST
} mfk_mode_t;

static mfk_mode_t   mfk_mode = MFK_MIN_LEVEL;
static vol_mode_t   vol_mode = VOL_VOL;

static uint8_t      pad = 10;
static uint16_t     spectrum_height = (480 / 3);
static uint16_t     freq_height = 36;
static uint8_t      btn_height = 54;
static uint8_t      over = 25;

static lv_obj_t     *obj;

static lv_obj_t     *spectrum;
static lv_obj_t     *freq[3];
static lv_obj_t     *waterfall;
static lv_obj_t     *btn[5];
static lv_obj_t     *msg;

void main_screen_set_freq(uint64_t f) {
    uint16_t    mhz, khz, hz;

    split_freq(f - 50000, &mhz, &khz, &hz);
    lv_label_set_text_fmt(freq[0], "%i.%03i", mhz, khz);

    split_freq(f, &mhz, &khz, &hz);
    lv_label_set_text_fmt(freq[1], "%i.%03i.%03i", mhz, khz, hz);

    split_freq(f + 50000, &mhz, &khz, &hz);
    lv_label_set_text_fmt(freq[2], "%i.%03i", mhz, khz);
    
    waterfall_update_band(f);
}

static void check_cross_band(uint64_t freq, uint64_t prev_freq) {
    params.freq_band = bands_find(freq);
    
    if (params.freq_band) {
        if (params.freq_band->type != 0 && params.freq_band->id != params.band) {
            params_band_freq_set(prev_freq);
            bands_activate(params.freq_band, &freq);
        }
    }
}

static void vol_rotate(int16_t diff) {
    int32_t x;

    switch (vol_mode) {
        case VOL_VOL:
            x = radio_change_vol(diff);
            msg_set_text_fmt("Volume: %i", x);
            break;
            
        case VOL_RFG:
            x = radio_change_rfg(diff);
            msg_set_text_fmt("RF gain: %i", x);
            break;

        case VOL_FILTER_LOW:
            x = radio_change_filter_low(diff);
            msg_set_text_fmt("Filter low: %i Hz", x);
            break;

        case VOL_FILTER_HIGH:
            x = radio_change_filter_high(diff);
            msg_set_text_fmt("Filter high: %i Hz", x);
            break;
            
        default:
            break;
    }
}

static void vol_press(int16_t dir) {
    vol_mode = (vol_mode + dir) % VOL_LAST;
    
    vol_rotate(0);
}

static void mfk_rotate(int16_t diff) {
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
    }
}

static void mfk_press(int16_t dir) {
    mfk_mode = (mfk_mode + dir) % MFK_LAST;
    
    mfk_rotate(0);
}

static void next_freq_step() {
    params_lock();
    
    switch (params_mode.freq_step) {
        case 10:
            params_mode.freq_step = 100;
            break;
            
        case 100:
            params_mode.freq_step = 500;
            break;
            
        case 500:
            params_mode.freq_step = 1000;
            break;
            
        case 1000:
            params_mode.freq_step = 5000;
            break;
            
        case 5000:
            params_mode.freq_step = 10;
            break;
            
        default:
            break;
    }

    params_unlock(&params_mode.durty.freq_step);
    msg_set_text_fmt("Freq step: %i Hz", params_mode.freq_step);
}

static void main_screen_rotary_cb(lv_event_t * e) {
    event_rotary_t  *rotary = lv_event_get_param(e);
    uint64_t        freq, prev_freq;

    switch (rotary->id) {
        case 0:
            freq = radio_change_freq(rotary->diff * params_mode.freq_step, &prev_freq);
            waterfall_change_freq(rotary->diff * params_mode.freq_step);
            main_screen_set_freq(freq);
            check_cross_band(freq, prev_freq);
            break;
            
        case 1:
            vol_rotate(rotary->diff);
            break;
            
        case 2:
            mfk_rotate(rotary->diff);
            break;
        
    }
}

static void main_screen_keypad_cb(lv_event_t * e) {
    event_keypad_t *keypad = lv_event_get_param(e);
    
    switch (keypad->key) {
        case KEYPAD_ROTARY_VOL:
            if (keypad->state == KEYPAD_RELEASE) {
                vol_press(1);
            } else if (keypad->state == KEYPAD_LONG) {
                vol_press(-1);
            }
            break;
            
        case KEYPAD_ROTARY_MFK:
            if (keypad->state == KEYPAD_RELEASE) {
                mfk_press(1);
            } else if (keypad->state == KEYPAD_LONG) {
                mfk_press(-1);
            }
            break;

        case KEYPAD_PRE:
            if (keypad->state == KEYPAD_RELEASE) {
                radio_change_att();
                info_params_set();
            } else if (keypad->state == KEYPAD_LONG) {
                radio_change_pre();
                info_params_set();
            }
            break;
            
        case KEYPAD_BAND_UP:
            if (keypad->state == KEYPAD_RELEASE) {
                bands_change(true);
            }
            break;
            
        case KEYPAD_BAND_DOWN:
            if (keypad->state == KEYPAD_RELEASE) {
                bands_change(false);
            }
            break;
            
        case KEYPAD_MODE_AM:
            if (keypad->state == KEYPAD_RELEASE) {
                radio_change_mode(RADIO_MODE_AM);
                params_mode_load();
                radio_mode_set();
                info_params_set();
            }
            break;
            
        case KEYPAD_MODE_CW:
            if (keypad->state == KEYPAD_RELEASE) {
                radio_change_mode(RADIO_MODE_CW);
                params_mode_load();
                radio_mode_set();
                info_params_set();
            }
            break;

        case KEYPAD_MODE_SSB:
            if (keypad->state == KEYPAD_RELEASE) {
                radio_change_mode(RADIO_MODE_SSB);
                params_mode_load();
                radio_mode_set();
                info_params_set();
            }
            break;

        case KEYPAD_AGC:
            if (keypad->state == KEYPAD_RELEASE) {
                radio_change_agc();
                info_params_set();
            }
            break;

        case KEYPAD_FST:
            if (keypad->state == KEYPAD_RELEASE) {
                next_freq_step();
            }
            break;

        default:
            LV_LOG_WARN("Unsuported key");
            break;
    }
}

static void main_screen_hkey_cb(lv_event_t * e) {
    event_hkey_t *hkey = lv_event_get_param(e);

    switch (hkey->key) {
        case HKEY_UP:
            if (!hkey->pressed) {
                bands_change(true);
            }
            break;

        case HKEY_DOWN:
            if (!hkey->pressed) {
                bands_change(false);
            }
            break;
        
        default:
            break;
    }
}

lv_obj_t * main_screen() {
    uint16_t y = pad;

    obj = lv_obj_create(NULL);

    lv_obj_add_event_cb(obj, main_screen_rotary_cb, EVENT_ROTARY, NULL);
    lv_obj_add_event_cb(obj, main_screen_keypad_cb, EVENT_KEYPAD, NULL);
    lv_obj_add_event_cb(obj, main_screen_hkey_cb, EVENT_HKEY, NULL);
    lv_obj_add_style(obj, &background_style, LV_PART_MAIN);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    
    spectrum = spectrum_init(obj);
    
    spectrum_set_min(params_band.grid_min);
    spectrum_set_max(params_band.grid_max);

    lv_obj_set_y(spectrum, y);
    lv_obj_set_height(spectrum, spectrum_height);
    
    y += spectrum_height;
    
    for (uint8_t i = 0; i < 3; i++) {
        lv_obj_t *f = lv_label_create(obj);

        switch (i) {
            case 0:
                lv_obj_add_style(f, &freq_style, 0);
                lv_obj_set_style_text_align(f, LV_TEXT_ALIGN_LEFT, 0);
                break;

            case 1:
                lv_obj_add_style(f, &freq_main_style, 0);
                lv_obj_set_style_text_align(f, LV_TEXT_ALIGN_CENTER, 0);
                break;

            case 2:
                lv_obj_add_style(f, &freq_style, 0);
                lv_obj_set_style_text_align(f, LV_TEXT_ALIGN_RIGHT, 0);
                break;
        }
        
        lv_obj_set_pos(f, i * ((800 - pad * 2) / 3) + pad, y);
        lv_obj_set_size(f, (800 - pad * 2) / 3, freq_height);
        
        freq[i] = f;
    }
    
    y += freq_height;

    waterfall = waterfall_init(obj);

    waterfall_set_min(params_band.grid_min);
    waterfall_set_max(params_band.grid_max);
    
    lv_obj_set_y(waterfall, y);
    waterfall_set_height(480 - y - pad);
    
    y = 480 - btn_height;
    
    uint16_t x = 0;
    uint16_t width = (800 - pad * 4) / 5;

    for (uint8_t i = 0; i < 5; i++) {
        lv_obj_t *f = lv_btn_create(obj);

        lv_obj_add_style(f, &panel_bottom_style, 0);
        lv_obj_add_style(f, &btn_style, 0);

        lv_obj_set_pos(f, x, y);
        lv_obj_set_size(f, width, btn_height + over);
        lv_obj_set_style_pad_bottom(f, over, 0);
        
        if (i == 0) {
            lv_obj_set_style_pad_left(f, over, 0);
        } else if (i == 4) {
            lv_obj_set_style_pad_right(f, over, 0);
        }
        
        x += width + pad;
        
        lv_obj_t *label = lv_label_create(f);
        
        lv_label_set_text_fmt(label, "Button %i", i);
        lv_obj_center(label);
    
        btn[i] = f;
    }

    msg = msg_init(obj);

    clock_init(obj);
    info_init(obj);
    meter_init(obj);
    
    main_screen_band_set();
    
    return obj;
}

void main_screen_band_set() {
    bool vfoa = (params_band.vfo == X6100_VFO_A);

    main_screen_set_freq(vfoa ? params_band.vfoa_freq : params_band.vfob_freq);
}
