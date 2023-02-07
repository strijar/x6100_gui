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
#include "main.h"

#define BUTTONS         5

typedef enum {
    VOL_VOL = 0,
    VOL_RFG,
    VOL_FILTER_LOW,
    VOL_FILTER_HIGH,
    VOL_PWR,
    VOL_HMIC,
    VOL_LAST,

    VOL_MIC,
    VOL_IMIC,
} vol_mode_t;

static vol_mode_t   vol_mode = VOL_VOL;

static uint16_t     spectrum_height = (480 / 3);
static uint16_t     freq_height = 36;
static uint8_t      btn_height = 62;
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
    uint16_t        data;
} button_item_t;

static void vol_update(int16_t diff);

static void button_next_page_cb(lv_event_t * e);
static void button_vol_update_cb(lv_event_t * e);
static void button_mfk_update_cb(lv_event_t * e);

typedef enum {
    PAGE_VOL_1 = 0,
    PAGE_VOL_2,

    PAGE_MFK_1,
    PAGE_MFK_2,
    PAGE_MFK_3,

    PAGE_KEY_1,
    PAGE_KEY_2,

    PAGE_DFN_1,
    PAGE_DFN_2,
    PAGE_DFN_3
} button_page_t;

static button_page_t    buttons_page = PAGE_VOL_1;

static button_item_t    buttons[] = {
    { .label = "(VOL 1:2)",         .callback = button_next_page_cb,        .data = PAGE_VOL_2 },
    { .label = "Audio\nVol",        .callback = button_vol_update_cb,       .data = VOL_VOL },
    { .label = "Filter\nLow",       .callback = button_vol_update_cb,       .data = VOL_FILTER_LOW },
    { .label = "Filter\nHigh",      .callback = button_vol_update_cb,       .data = VOL_FILTER_HIGH },
    { .label = "TX\nPower",         .callback = button_vol_update_cb,       .data = VOL_PWR },

    { .label = "(VOL 2:2)",         .callback = button_next_page_cb,        .data = PAGE_MFK_1 },
    { .label = "MIC\nSelect",       .callback = button_vol_update_cb,       .data = VOL_MIC },
    { .label = "H-MIC\nGain",       .callback = button_vol_update_cb,       .data = VOL_HMIC },
    { .label = "I-MIC\nGain",       .callback = button_vol_update_cb,       .data = VOL_IMIC },
    { .label = "",                  .callback = NULL },
    
    { .label = "(MFK 1:3)",         .callback = button_next_page_cb,        .data = PAGE_MFK_2 },
    { .label = "Min\nLevel",        .callback = button_mfk_update_cb,       .data = MFK_MIN_LEVEL },
    { .label = "Max\nLevel",        .callback = button_mfk_update_cb,       .data = MFK_MAX_LEVEL },
    { .label = "Spectrum\nZoom",    .callback = button_mfk_update_cb,       .data = MFK_SPECTRUM_FACTOR },
    { .label = "Spectrum\nBeta",    .callback = button_mfk_update_cb,       .data = MFK_SPECTRUM_BETA },

    { .label = "(MFK 2:3)",         .callback = button_next_page_cb,        .data = PAGE_MFK_3 },
    { .label = "Spectrum\nFill",    .callback = button_mfk_update_cb,       .data = MFK_SPECTRUM_FILL },
    { .label = "Spectrum\nPeak",    .callback = button_mfk_update_cb,       .data = MFK_SPECTRUM_PEAK },
    { .label = "Peaks\nHold",       .callback = button_mfk_update_cb,       .data = MFK_PEAK_HOLD },
    { .label = "Peaks\nSpeed",      .callback = button_mfk_update_cb,       .data = MFK_PEAK_SPEED },

    { .label = "(MFK 3:3)",         .callback = button_next_page_cb,        .data = PAGE_VOL_1 },
    { .label = "Charger",           .callback = button_mfk_update_cb,       .data = MFK_CHARGER },
    { .label = "AGC\nHang",         .callback = button_mfk_update_cb,       .data = MFK_AGC_HANG },
    { .label = "AGC\nKnee",         .callback = button_mfk_update_cb,       .data = MFK_AGC_KNEE },
    { .label = "AGC\nSlope",        .callback = button_mfk_update_cb,       .data = MFK_AGC_SLOPE },

    /* CW */
    
    { .label = "(KEY 1:2)",         .callback = button_next_page_cb,        .data = PAGE_KEY_2 },
    { .label = "Speed",             .callback = button_mfk_update_cb,       .data = MFK_KEY_SPEED },
    { .label = "Volume",            .callback = button_mfk_update_cb,       .data = MFK_KEY_VOL },
    { .label = "Train",             .callback = button_mfk_update_cb,       .data = MFK_KEY_TRAIN },
    { .label = "Tone",              .callback = button_mfk_update_cb,       .data = MFK_KEY_TONE },
    
    { .label = "(KEY 2:2)",         .callback = button_next_page_cb,        .data = PAGE_KEY_1 },
    { .label = "Key\nMode",         .callback = button_mfk_update_cb,       .data = MFK_KEY_MODE },
    { .label = "Iambic\nMode",      .callback = button_mfk_update_cb,       .data = MFK_IAMBIC_MODE },
    { .label = "QSK\nTime",         .callback = button_mfk_update_cb,       .data = MFK_QSK_TIME },
    { .label = "Ratio",             .callback = button_mfk_update_cb,       .data = MFK_KEY_RATIO },
    
    /* DSP */

    { .label = "(DFN 1:3)",         .callback = button_next_page_cb,        .data = PAGE_DFN_2 },
    { .label = "DNF",               .callback = button_mfk_update_cb,       .data = MFK_DNF },
    { .label = "DNF\nCenter",       .callback = button_mfk_update_cb,       .data = MFK_DNF_CENTER },
    { .label = "DNF\nWidth",        .callback = button_mfk_update_cb,       .data = MFK_DNF_WIDTH },
    { .label = "",                  .callback = NULL },

    { .label = "(DFN 2:3)",         .callback = button_next_page_cb,        .data = PAGE_DFN_3 },
    { .label = "NB",                .callback = button_mfk_update_cb,       .data = MFK_NB },
    { .label = "NB\nLevel",         .callback = button_mfk_update_cb,       .data = MFK_NB_LEVEL },
    { .label = "NB\nWidth",         .callback = button_mfk_update_cb,       .data = MFK_NB_WIDTH },
    { .label = "",                  .callback = NULL },

    { .label = "(DFN 3:3)",         .callback = button_next_page_cb,        .data = PAGE_DFN_1 },
    { .label = "NR",                .callback = button_mfk_update_cb,       .data = MFK_NR },
    { .label = "NR\nLevel",         .callback = button_mfk_update_cb,       .data = MFK_NR_LEVEL },
    { .label = "",                  .callback = NULL },
    { .label = "",                  .callback = NULL },
};

/* Buttons */

static void buttons_load_page() {
    for (uint8_t i = 0; i < BUTTONS; i++) {
        button_item_t   *item = &buttons[buttons_page * BUTTONS + i];
        lv_obj_t        *label = lv_obj_get_user_data(btn[i]);

        lv_obj_add_event_cb(btn[i], item->callback, LV_EVENT_PRESSED, item);
        lv_label_set_text(label, item->label);
    }
}

static void buttons_unload_page() {
    for (uint8_t i = 0; i < BUTTONS; i++) {
        button_item_t   *item = &buttons[buttons_page * BUTTONS + i];

        lv_obj_remove_event_cb_with_user_data(btn[i], NULL, item);
    }
}

static void button_next_page_cb(lv_event_t * e) {
    button_item_t *item = lv_event_get_user_data(e);
    
    buttons_unload_page();
    buttons_page = item->data;
    buttons_load_page();
}

static void button_vol_update_cb(lv_event_t * e) {
    button_item_t *item = lv_event_get_user_data(e);

    vol_mode = item->data;
    vol_update(0);
}

static void button_mfk_update_cb(lv_event_t * e) {
    button_item_t *item = lv_event_get_user_data(e);

    mfk_set_mode(item->data);
    mfk_update(0);
}

/* * */

void main_screen_set_freq() {
    uint64_t    f;
    x6100_vfo_t vfo = params_band.vfo;

    if (params_band.split && radio_get_state() == RADIO_TX) {
        vfo = (vfo == X6100_VFO_A) ? X6100_VFO_B : X6100_VFO_A;
    }
    
    f = params_band.vfo_x[vfo].freq;

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
    char    *s;

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
            
            msg_set_text_fmt("MIC: %s", s);
            break;

        case VOL_HMIC:
            x = radio_change_hmic(diff);
            msg_set_text_fmt("H-MIC gain: %i", x);
            break;

        case VOL_IMIC:
            x = radio_change_imic(diff);
            msg_set_text_fmt("I-MIC gain: %i", x);
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
            spectrum_change_freq(freq - prev_freq);
            main_screen_set_freq();
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
                spectrum_mode_set();
                info_params_set();
            }
            break;
            
        case KEYPAD_MODE_CW:
            if (keypad->state == KEYPAD_RELEASE) {
                radio_change_mode(RADIO_MODE_CW);
                params_mode_load();
                radio_mode_set();
                spectrum_mode_set();
                info_params_set();
            }
            break;

        case KEYPAD_MODE_SSB:
            if (keypad->state == KEYPAD_RELEASE) {
                radio_change_mode(RADIO_MODE_SSB);
                params_mode_load();
                radio_mode_set();
                spectrum_mode_set();
                info_params_set();
            }
            break;

        case KEYPAD_AGC:
            if (keypad->state == KEYPAD_RELEASE) {
                radio_change_agc();
                info_params_set();
            } else if (keypad->state == KEYPAD_LONG) {
                radio_change_split();
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
            if (keypad->state == KEYPAD_RELEASE) {
                mfk_set_mode(MFK_MIN_LEVEL);
                buttons_unload_page();
                buttons_page = PAGE_VOL_1;
                buttons_load_page();
            }
            break;

        case KEYPAD_KEY:
            if (keypad->state == KEYPAD_RELEASE) {
                buttons_unload_page();
                buttons_page = PAGE_KEY_1;
                buttons_load_page();
            }
            break;

        case KEYPAD_DFN:
            if (keypad->state == KEYPAD_RELEASE) {
                buttons_unload_page();
                buttons_page = PAGE_DFN_1;
                buttons_load_page();
            }
            break;

        case KEYPAD_AB:
            if (keypad->state == KEYPAD_RELEASE) {
                radio_change_vfo();
                info_params_set();
                waterfall_clear();
                spectrum_clear();
                main_screen_band_set();
            } else if (keypad->state == KEYPAD_LONG) {
                params_band_vfo_clone();
                radio_vfo_set();
                msg_set_text_fmt("Clone VFO %s", params_band.vfo == X6100_VFO_A ? "A->B" : "B->A");
            }
            break;

        case KEYPAD_POWER:
            if (keypad->state == KEYPAD_LONG) {
                msg_set_text_fmt("Power off");
                radio_poweroff();
            }
            break;
            
        case KEYPAD_LOCK:
            if (keypad->state == KEYPAD_LONG) {
                exit(1);
            }
            break;

        case KEYPAD_PTT:
            switch (keypad->state) {
                case KEYPAD_PRESS:
                    x6100_control_ptt_set(true);
                    break;
                    
                case KEYPAD_RELEASE:
                case KEYPAD_LONG_RELEASE:
                    x6100_control_ptt_set(false);
                    break;
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

    if (params_band.split) {
        main_screen_set_freq();
    }
    
    lv_event_send(meter, code, NULL);
    lv_event_send(tx_info, code, NULL);
    lv_event_send(spectrum, code, NULL);
}

lv_obj_t * main_screen() {
    uint16_t y = 0;

    obj = lv_obj_create(NULL);

    lv_obj_add_event_cb(obj, main_screen_rotary_cb, EVENT_ROTARY, NULL);
    lv_obj_add_event_cb(obj, main_screen_keypad_cb, EVENT_KEYPAD, NULL);
    lv_obj_add_event_cb(obj, main_screen_hkey_cb, EVENT_HKEY, NULL);
    lv_obj_add_event_cb(obj, main_screen_radio_cb, EVENT_RADIO_TX, NULL);
    lv_obj_add_event_cb(obj, main_screen_radio_cb, EVENT_RADIO_RX, NULL);
    
    lv_obj_add_style(obj, &background_style, LV_PART_MAIN);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    
    spectrum = spectrum_init(obj);
    
    spectrum_band_set();

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
        
        lv_obj_set_pos(f, i * ((800 - 10 * 2) / 3) + 10, y);
        lv_obj_set_size(f, (800 - 10 * 2) / 3, freq_height);
        
        freq[i] = f;
    }
    
    y += freq_height;

    waterfall = waterfall_init(obj);

    waterfall_band_set();
    
    lv_obj_set_y(waterfall, y);
    waterfall_set_height(480 - y);
    
    y = 480 - btn_height;
    
    uint16_t x = 0;
    uint16_t width = 152;

    for (uint8_t i = 0; i < 5; i++) {
        lv_obj_t *f = lv_btn_create(obj);
        
        lv_obj_remove_style_all(f); 
        lv_obj_add_style(f, &btn_style, 0);

        lv_obj_set_pos(f, x, y);
        lv_obj_set_size(f, width, btn_height);

        x += width + 10;
        
        lv_obj_t *label = lv_label_create(f);
        
        lv_obj_center(label);
        lv_obj_set_user_data(f, label);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

        btn[i] = f;
    }

    buttons_load_page();
    msg = msg_init(obj);

    clock_init(obj);
    info_init(obj);
    
    meter = meter_init(obj);
    tx_info = tx_info_init(obj);
    
    main_screen_band_set();

    msg_set_text_fmt("X6100 de R1CBU " VERSION);
    msg_set_timeout(2000);
    
    return obj;
}

void main_screen_band_set() {
    main_screen_set_freq();
}
