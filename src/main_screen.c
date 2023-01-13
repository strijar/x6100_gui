/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
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
#include "band_info.h"
#include "tx_info.h"
#include "mfk.h"

#define BUTTONS         5

typedef enum {
    VOL_VOL = 0,
    VOL_RFG,
    VOL_FILTER_LOW,
    VOL_FILTER_HIGH,
    VOL_PWR,
    
    VOL_LAST
} vol_mode_t;

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
static lv_obj_t     *btn[BUTTONS];
static lv_obj_t     *msg;
static lv_obj_t     *meter;
static lv_obj_t     *tx_info;

typedef struct {
    char            *label;
    lv_event_cb_t   callback;
} button_item_t;

static void vol_update(int16_t diff);

static void button_next_page_cb(lv_event_t * e);
static void button_vol_cb(lv_event_t * e);
static void button_filter_low_cb(lv_event_t * e);
static void button_filter_high_cb(lv_event_t * e);
static void button_tx_power_cb(lv_event_t * e);
static void button_min_level_cb(lv_event_t * e);
static void button_max_level_cb(lv_event_t * e);
static void button_spectrum_factor_cb(lv_event_t * e);
static void button_spectrum_beta_cb(lv_event_t * e);

static void button_key_speed_cb(lv_event_t * e);
static void button_key_mode_cb(lv_event_t * e);
static void button_iambic_mode_cb(lv_event_t * e);
static void button_key_tone_cb(lv_event_t * e);
static void button_key_vol_cb(lv_event_t * e);
static void button_key_train_cb(lv_event_t * e);
static void button_qsk_time_cb(lv_event_t * e);
static void button_key_ratio_cb(lv_event_t * e);

typedef enum {
    PAGE_VOL = 0,
    PAGE_MFK,
    PAGE_KEY_1,
    PAGE_KEY_2
} button_page_t;

static button_page_t    buttons_page = PAGE_VOL;

static button_item_t    buttons[] = {
    { .label = "(VOL)",             .callback = button_next_page_cb },
    { .label = "Audio\nVol",        .callback = button_vol_cb },
    { .label = "Filter\nLow",       .callback = button_filter_low_cb },
    { .label = "Filter\nHigh",      .callback = button_filter_high_cb },
    { .label = "TX\nPower",         .callback = button_tx_power_cb },
    
    { .label = "(MFK)",             .callback = button_next_page_cb },
    { .label = "Min\nLevel",        .callback = button_min_level_cb },
    { .label = "Max\nLevel",        .callback = button_max_level_cb },
    { .label = "Spectrum\nZoom",    .callback = button_spectrum_factor_cb },
    { .label = "Spectrum\nBeta",    .callback = button_spectrum_beta_cb },

    /* CW */
    
    { .label = "(KEY 1:2)",         .callback = button_next_page_cb },
    { .label = "Speed",             .callback = button_key_speed_cb },
    { .label = "Key\nMode",         .callback = button_key_mode_cb },
    { .label = "Iambic\nMode",      .callback = button_iambic_mode_cb },
    { .label = "Tone",              .callback = button_key_tone_cb },
    
    { .label = "(KEY 2:2)",         .callback = button_next_page_cb },
    { .label = "Volume",            .callback = button_key_vol_cb },
    { .label = "Train",             .callback = button_key_train_cb },
    { .label = "QSK\nTime",         .callback = button_qsk_time_cb },
    { .label = "Ratio",             .callback = button_key_ratio_cb },
};

/* Buttons */

static void buttons_load_page() {
    for (uint8_t i = 0; i < BUTTONS; i++) {
        button_item_t   *item = &buttons[buttons_page * BUTTONS + i];
        lv_obj_t        *label = lv_obj_get_user_data(btn[i]);

        lv_obj_add_event_cb(btn[i], item->callback, LV_EVENT_PRESSED, NULL);
        lv_label_set_text(label, item->label);
    }
}

static void buttons_unload_page() {
    for (uint8_t i = 0; i < BUTTONS; i++) {
        button_item_t   *item = &buttons[buttons_page * BUTTONS + i];

        lv_obj_remove_event_cb(btn[i], item->callback);
    }
}

static void button_next_page_cb(lv_event_t * e) {
    buttons_unload_page();

    switch (buttons_page) {
        case PAGE_VOL:
            buttons_page = PAGE_MFK;
            break;
            
        case PAGE_MFK:
            buttons_page = PAGE_VOL;
            break;
            
        case PAGE_KEY_1:
            buttons_page = PAGE_KEY_2;
            break;
            
        case PAGE_KEY_2:
            buttons_page = PAGE_KEY_1;
            break;
    }
    
    buttons_load_page();
}

static void button_vol_cb(lv_event_t * e) {
    vol_mode = VOL_VOL;
    vol_update(0);
}

static void button_filter_low_cb(lv_event_t * e) {
    vol_mode = VOL_FILTER_LOW;
    vol_update(0);
}

static void button_filter_high_cb(lv_event_t * e) {
    vol_mode = VOL_FILTER_HIGH;
    vol_update(0);
}

static void button_tx_power_cb(lv_event_t * e) {
    vol_mode = VOL_PWR;
    vol_update(0);
}

static void button_min_level_cb(lv_event_t * e) {
    mfk_set_mode(MFK_MIN_LEVEL);
    mfk_update(0);
}

static void button_max_level_cb(lv_event_t * e) {
    mfk_set_mode(MFK_MAX_LEVEL);
    mfk_update(0);
}

static void button_spectrum_factor_cb(lv_event_t * e) {
    mfk_set_mode(MFK_SPECTRUM_FACTOR);
    mfk_update(0);
}

static void button_spectrum_beta_cb(lv_event_t * e) {
    mfk_set_mode(MFK_SPECTRUM_BETA);
    mfk_update(0);
}

static void button_key_speed_cb(lv_event_t * e) {
    mfk_set_mode(MFK_KEY_SPEED);
    mfk_update(0);
}

static void button_key_mode_cb(lv_event_t * e) {
    mfk_set_mode(MFK_KEY_MODE);
    mfk_update(0);
}

static void button_iambic_mode_cb(lv_event_t * e) {
    mfk_set_mode(MFK_IAMBIC_MODE);
    mfk_update(0);
}

static void button_key_tone_cb(lv_event_t * e) {
    mfk_set_mode(MFK_KEY_TONE);
    mfk_update(0);
}

static void button_key_vol_cb(lv_event_t * e) {
    mfk_set_mode(MFK_KEY_VOL);
    mfk_update(0);
}

static void button_key_train_cb(lv_event_t * e) {
    mfk_set_mode(MFK_KEY_TRAIN);
    mfk_update(0);
}

static void button_qsk_time_cb(lv_event_t * e) {
    mfk_set_mode(MFK_QSK_TIME);
    mfk_update(0);
}

static void button_key_ratio_cb(lv_event_t * e) {
    mfk_set_mode(MFK_KEY_RATIO);
    mfk_update(0);
}

/* * */

void main_screen_set_freq(uint64_t f) {
    uint16_t    mhz, khz, hz;

    split_freq(f - 50000, &mhz, &khz, &hz);
    lv_label_set_text_fmt(freq[0], "%i.%03i", mhz, khz);

    split_freq(f, &mhz, &khz, &hz);
    lv_label_set_text_fmt(freq[1], "%i.%03i.%03i", mhz, khz, hz);

    split_freq(f + 50000, &mhz, &khz, &hz);
    lv_label_set_text_fmt(freq[2], "%i.%03i", mhz, khz);
    
    band_info_update(f);
    radio_load_atu();
}

static void check_cross_band(uint64_t freq, uint64_t prev_freq) {
    params.freq_band = bands_find(freq);
    
    if (params.freq_band) {
        if (params.freq_band->type != 0) {
            if (params.freq_band->id != params.band) {
                params_band_freq_set(prev_freq);
                bands_activate(params.freq_band, &freq);
            }
        } else {
            params.freq_band = NULL;
        }
    }
}

static void vol_update(int16_t diff) {
    int32_t x;
    float   f;

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

        case VOL_PWR:
            f = radio_change_pwr(diff);
            msg_set_text_fmt("Power: %0.1f W", f);
            break;
            
        default:
            break;
    }
}

static void vol_press(int16_t dir) {
    vol_mode = (vol_mode + dir) % VOL_LAST;
    
    vol_update(0);
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
            freq = radio_change_freq(-rotary->diff * params_mode.freq_step, &prev_freq);
            waterfall_change_freq(freq - prev_freq);
            main_screen_set_freq(freq);
            check_cross_band(freq, prev_freq);
            break;
            
        case 1:
            vol_update(rotary->diff);
            break;
            
        case 2:
            mfk_update(rotary->diff);
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
                radio_change_pre();
                info_params_set();
            } else if (keypad->state == KEYPAD_LONG) {
                radio_change_att();
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

        case KEYPAD_ATU:
            if (keypad->state == KEYPAD_RELEASE) {
                radio_change_atu();
                info_params_set();
            } else if (keypad->state == KEYPAD_LONG) {
                radio_start_atu();
            }
            break;

        case KEYPAD_F1:
            if (keypad->state == KEYPAD_PRESS) {
                lv_event_send(btn[0], LV_EVENT_PRESSED, NULL);
            } else {
                lv_event_send(btn[0], LV_EVENT_RELEASED, NULL);
            }
            break;

        case KEYPAD_F2:
            if (keypad->state == KEYPAD_PRESS) {
                lv_event_send(btn[1], LV_EVENT_PRESSED, NULL);
            } else {
                lv_event_send(btn[1], LV_EVENT_RELEASED, NULL);
            }
            break;

        case KEYPAD_F3:
            if (keypad->state == KEYPAD_PRESS) {
                lv_event_send(btn[2], LV_EVENT_PRESSED, NULL);
            } else {
                lv_event_send(btn[2], LV_EVENT_RELEASED, NULL);
            }
            break;

        case KEYPAD_F4:
            if (keypad->state == KEYPAD_PRESS) {
                lv_event_send(btn[3], LV_EVENT_PRESSED, NULL);
            } else {
                lv_event_send(btn[3], LV_EVENT_RELEASED, NULL);
            }
            break;

        case KEYPAD_F5:
            if (keypad->state == KEYPAD_PRESS) {
                lv_event_send(btn[4], LV_EVENT_PRESSED, NULL);
            } else {
                lv_event_send(btn[4], LV_EVENT_RELEASED, NULL);
            }
            break;

        case KEYPAD_GEN:
            if (keypad->state == KEYPAD_PRESS) {
                mfk_set_mode(MFK_MIN_LEVEL);
                buttons_unload_page();
                buttons_page = PAGE_VOL;
                buttons_load_page();
            }
            break;

        case KEYPAD_KEY:
            if (keypad->state == KEYPAD_PRESS) {
                buttons_unload_page();
                buttons_page = PAGE_KEY_1;
                buttons_load_page();
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

static void main_screen_radio_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    
    lv_event_send(meter, code, NULL);
    lv_event_send(tx_info, code, NULL);
    lv_event_send(spectrum, code, NULL);
}

lv_obj_t * main_screen() {
    uint16_t y = pad;

    obj = lv_obj_create(NULL);

    lv_obj_add_event_cb(obj, main_screen_rotary_cb, EVENT_ROTARY, NULL);
    lv_obj_add_event_cb(obj, main_screen_keypad_cb, EVENT_KEYPAD, NULL);
    lv_obj_add_event_cb(obj, main_screen_hkey_cb, EVENT_HKEY, NULL);
    lv_obj_add_event_cb(obj, main_screen_radio_cb, EVENT_RADIO_TX, NULL);
    lv_obj_add_event_cb(obj, main_screen_radio_cb, EVENT_RADIO_RX, NULL);
    
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
        
        lv_obj_center(label);
        lv_obj_set_user_data(f, label);
    
        btn[i] = f;
    }

    buttons_load_page();
    msg = msg_init(obj);

    clock_init(obj);
    info_init(obj);
    
    meter = meter_init(obj);
    tx_info = tx_info_init(obj);
    
    main_screen_band_set();
    
    return obj;
}

void main_screen_band_set() {
    bool vfoa = (params_band.vfo == X6100_VFO_A);

    main_screen_set_freq(vfoa ? params_band.vfoa_freq : params_band.vfob_freq);
}
