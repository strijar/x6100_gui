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

#include "main_screen.h"
#include "styles.h"
#include "spectrum.h"
#include "waterfall.h"
#include "util.h"
#include "radio.h"
#include "events.h"
#include "msg.h"
#include "msg_tiny.h"
#include "dsp.h"
#include "params.h"
#include "bands.h"
#include "clock.h"
#include "info.h"
#include "meter.h"
#include "band_info.h"
#include "tx_info.h"
#include "mfk.h"
#include "vol.h"
#include "main.h"
#include "pannel.h"
#include "rtty.h"
#include "screenshot.h"
#include "keyboard.h"
#include "dialog.h"
#include "dialog_settings.h"
#include "dialog_freq.h"
#include "dialog_msg_cw.h"
#include "dialog_msg_voice.h"
#include "backlight.h"
#include "buttons.h"

static uint16_t     spectrum_height = (480 / 3);
static uint16_t     freq_height = 36;
static lv_obj_t     *obj;
static bool         freq_lock = false;

static lv_obj_t     *spectrum;
static lv_obj_t     *freq[3];
static lv_obj_t     *waterfall;
static lv_obj_t     *msg;
static lv_obj_t     *msg_tiny;
static lv_obj_t     *meter;
static lv_obj_t     *tx_info;

static void freq_update(int16_t diff);
static void next_freq_step(bool up);

static void main_screen_set_freq();

void mem_load(uint8_t x) {
    params_memory_load(x);

    params.freq_band = bands_find(params_band.vfo_x[params_band.vfo].freq);

    if (params.freq_band) {
        if (params.freq_band->type != 0) {
            params.band = params.freq_band->id;
        }
    }

    radio_vfo_set();
    radio_mode_set();
    spectrum_mode_set();
    spectrum_band_set();
    waterfall_band_set();

    radio_load_atu();
    info_params_set();
    pannel_visible();

    waterfall_clear();
    spectrum_clear();
    main_screen_set_freq();

    msg_set_text_fmt("Loaded from memory %i", x);
}

void mem_save(uint8_t x) {
    params_memory_save(x);
    msg_set_text_fmt("Saved in memory %i", x);
}

/* * */

static void main_screen_set_freq() {
    uint64_t    f;
    x6100_vfo_t vfo = params_band.vfo;
    uint32_t    color = freq_lock ? 0xBBBBBB : 0xFFFFFF;

    if (params_band.split && radio_get_state() == RADIO_TX) {
        vfo = (vfo == X6100_VFO_A) ? X6100_VFO_B : X6100_VFO_A;
    }
    
    f = params_band.vfo_x[vfo].freq;

    uint16_t    mhz, khz, hz;

    split_freq(f - 50000, &mhz, &khz, &hz);
    lv_label_set_text_fmt(freq[0], "#%03X %i.%03i", color, mhz, khz);

    split_freq(f, &mhz, &khz, &hz);

    if (params.mag_freq) {
        msg_tiny_set_text_fmt("%i.%03i.%03i", mhz, khz, hz);
    }

    if (params_band.split) {
        uint16_t    mhz2, khz2, hz2;
        uint64_t    f2 = params_band.vfo_x[(vfo == X6100_VFO_A) ? X6100_VFO_B : X6100_VFO_A].freq;

        split_freq(f2, &mhz2, &khz2, &hz2);
        
        lv_label_set_text_fmt(freq[1], "#%03X %i.%03i.%03i / %i.%03i.%03i", color, mhz, khz, hz, mhz2, khz2, hz2);
    } else {
        lv_label_set_text_fmt(freq[1], "#%03X %i.%03i.%03i", color, mhz, khz, hz);
    }

    split_freq(f + 50000, &mhz, &khz, &hz);
    lv_label_set_text_fmt(freq[2], "#%03X %i.%03i", color, mhz, khz);
    
    band_info_update(f);
}

static void check_cross_band(uint64_t freq, uint64_t prev_freq) {
    params.freq_band = bands_find(freq);
    
    if (params.freq_band) {
        if (params.freq_band->type != 0) {
            if (params.freq_band->id != params.band) {
                params_band_freq_set(prev_freq);
                bands_activate(params.freq_band, &freq);
                info_params_set();
                pannel_visible();
            }
        } else {
            params.freq_band = NULL;
        }
    }
}

static void next_freq_step(bool up) {
    params_lock();
    
    switch (params_mode.freq_step) {
        case 10:
            params_mode.freq_step = up ? 100 : 5000;
            break;
            
        case 100:
            params_mode.freq_step = up ? 500 : 10;
            break;
            
        case 500:
            params_mode.freq_step = up ? 1000 : 100;
            break;
            
        case 1000:
            params_mode.freq_step = up ? 5000 : 500;
            break;
            
        case 5000:
            params_mode.freq_step = up ? 10 : 1000;
            break;
            
        default:
            break;
    }

    params_unlock(&params_mode.durty.freq_step);
    msg_set_text_fmt("Freq step: %i Hz", params_mode.freq_step);
}

void main_screen_dialog_deleted_cb() {
    buttons_unload_page();
    buttons_load_page(PAGE_VOL_1);
}

static void apps_disable() {
    dialog_destruct();

    rtty_set_state(RTTY_OFF);
    pannel_visible();
}

static void main_screen_keypad_cb(lv_event_t * e) {
    event_keypad_t *keypad = lv_event_get_param(e);

    switch (keypad->key) {
        case KEYPAD_PRE:
            if (keypad->state == KEYPAD_RELEASE) {
                radio_change_pre();
                info_params_set();
                
                if (params.mag_info) {
                    msg_tiny_set_text_fmt("Pre: %s", info_params_pre() ? "On" : "Off");
                }
            } else if (keypad->state == KEYPAD_LONG) {
                radio_change_att();
                info_params_set();

                if (params.mag_info) {
                    msg_tiny_set_text_fmt("Att: %s", info_params_att() ? "On" : "Off");
                }
            }
            break;
            
        case KEYPAD_BAND_UP:
            if (keypad->state == KEYPAD_RELEASE) {
                bands_change(true);
                
                if (dialog_is_run()) {
                    dialog_send(EVENT_FREQ_UPDATE, NULL);
                }
            }
            break;
            
        case KEYPAD_BAND_DOWN:
            if (keypad->state == KEYPAD_RELEASE) {
                bands_change(false);

                if (dialog_is_run()) {
                    dialog_send(EVENT_FREQ_UPDATE, NULL);
                }
            }
            break;
            
        case KEYPAD_MODE_AM:
            if (keypad->state == KEYPAD_RELEASE) {
                radio_change_mode(RADIO_MODE_AM);
                params_mode_load();
                radio_mode_set();
                spectrum_mode_set();
                info_params_set();
                pannel_visible();

                if (params.mag_info) {
                    msg_tiny_set_text_fmt("%s", info_params_mode());
                }
            }
            break;
            
        case KEYPAD_MODE_CW:
            if (keypad->state == KEYPAD_RELEASE) {
                radio_change_mode(RADIO_MODE_CW);
                params_mode_load();
                radio_mode_set();
                spectrum_mode_set();
                info_params_set();
                pannel_visible();

                if (params.mag_info) {
                    msg_tiny_set_text_fmt("%s", info_params_mode());
                }
            }
            break;

        case KEYPAD_MODE_SSB:
            if (keypad->state == KEYPAD_RELEASE) {
                radio_change_mode(RADIO_MODE_SSB);
                params_mode_load();
                radio_mode_set();
                spectrum_mode_set();
                info_params_set();
                pannel_visible();

                if (params.mag_info) {
                    msg_tiny_set_text_fmt("%s", info_params_mode());
                }
            }
            break;

        case KEYPAD_AGC:
            if (keypad->state == KEYPAD_RELEASE) {
                radio_change_agc();
                info_params_set();

                if (params.mag_info) {
                    msg_tiny_set_text_fmt("AGC: %s", info_params_agc());
                }
            } else if (keypad->state == KEYPAD_LONG) {
                radio_change_split();
                info_params_set();
                waterfall_clear();
                spectrum_clear();
                main_screen_band_set();

                if (params.mag_info) {
                    msg_tiny_set_text_fmt("%s", info_params_vfo());
                }
            }
            break;

        case KEYPAD_FST:
            if (keypad->state == KEYPAD_RELEASE) {
                next_freq_step(true);
            } else if (keypad->state == KEYPAD_LONG) {
                next_freq_step(false);
            }
            break;

        case KEYPAD_ATU:
            if (keypad->state == KEYPAD_RELEASE) {
                radio_change_atu();
                info_params_set();

                if (params.mag_info) {
                    msg_tiny_set_text_fmt("ATU: %s", params.atu ? "On" : "Off");
                }
            } else if (keypad->state == KEYPAD_LONG) {
                radio_start_atu();
            }
            break;

        case KEYPAD_F1:
            if (keypad->state == KEYPAD_RELEASE) {
                buttons_press(0, false);
            } else if (keypad->state == KEYPAD_LONG) {
                buttons_press(0, true);
            }
            break;

        case KEYPAD_F2:
            if (keypad->state == KEYPAD_RELEASE) {
                buttons_press(1, false);
            } else if (keypad->state == KEYPAD_LONG) {
                buttons_press(1, true);
            }
            break;

        case KEYPAD_F3:
            if (keypad->state == KEYPAD_RELEASE) {
                buttons_press(2, false);
            } else if (keypad->state == KEYPAD_LONG) {
                buttons_press(2, true);
            }
            break;

        case KEYPAD_F4:
            if (keypad->state == KEYPAD_RELEASE) {
                buttons_press(3, false);
            } else if (keypad->state == KEYPAD_LONG) {
                buttons_press(3, true);
            }
            break;

        case KEYPAD_F5:
            if (keypad->state == KEYPAD_RELEASE) {
                buttons_press(4, false);
            } else if (keypad->state == KEYPAD_LONG) {
                buttons_press(4, true);
            }
            break;

        case KEYPAD_GEN:
            if (keypad->state == KEYPAD_RELEASE) {
                apps_disable();
                mfk_set_mode(MFK_MIN_LEVEL);
                buttons_unload_page();
                buttons_load_page(PAGE_VOL_1);
            } else if (keypad->state == KEYPAD_LONG) {
                screenshot_take();
            }
            break;

        case KEYPAD_APP:
            if (keypad->state == KEYPAD_RELEASE) {
                apps_disable();
                buttons_unload_page();
                buttons_load_page(PAGE_APP_1);
            }
            break;

        case KEYPAD_KEY:
            if (keypad->state == KEYPAD_RELEASE) {
                apps_disable();
                buttons_unload_page();
                buttons_load_page(PAGE_KEY_1);
            }
            break;

        case KEYPAD_MSG:
            if (keypad->state == KEYPAD_RELEASE) {
                switch (radio_current_mode()) {
                    case x6100_mode_cw:
                    case x6100_mode_cwr:
                        apps_disable();
                        buttons_unload_page();

                        pannel_hide();
                        dialog_construct(dialog_msg_cw, obj);
                        buttons_load_page(PAGE_MSG_CW_1);
                        break;
                        
                    case x6100_mode_lsb:
                    case x6100_mode_usb:
                    case x6100_mode_am:
                    case x6100_mode_nfm:
                        apps_disable();
                        buttons_unload_page();

                        pannel_hide();
                        dialog_construct(dialog_msg_voice, obj);
                        buttons_load_page(PAGE_MSG_VOICE_1);
                        break;
                }
            }
            break;

        case KEYPAD_DFN:
            if (keypad->state == KEYPAD_RELEASE) {
                apps_disable();
                buttons_unload_page();
                buttons_load_page(PAGE_DFN_1);
            }
            break;

        case KEYPAD_AB:
            if (keypad->state == KEYPAD_RELEASE) {
                radio_change_vfo();
                info_params_set();
                waterfall_clear();
                spectrum_clear();
                main_screen_band_set();

                if (params.mag_info) {
                    msg_tiny_set_text_fmt("%s", info_params_vfo());
                }
            } else if (keypad->state == KEYPAD_LONG) {
                params_band_vfo_clone();
                radio_vfo_set();
                msg_set_text_fmt("Clone VFO %s", params_band.vfo == X6100_VFO_A ? "A->B" : "B->A");
            }
            break;

        case KEYPAD_POWER:
            if (keypad->state == KEYPAD_RELEASE) {
                backlight_switch();
            } else if (keypad->state == KEYPAD_LONG) {
                msg_set_text_fmt("Power off");
                radio_poweroff();
            }
            break;
            
        case KEYPAD_LOCK:
            if (keypad->state == KEYPAD_RELEASE) {
                freq_lock = !freq_lock;
                main_screen_set_freq();
            } else if (keypad->state == KEYPAD_LONG) {
                radio_bb_reset();
                exit(1);
            }
            break;

        case KEYPAD_PTT:
            switch (keypad->state) {
                case KEYPAD_PRESS:
                    radio_set_ptt(true);

                    switch (radio_current_mode()) {
                        case x6100_mode_cw:
                        case x6100_mode_cwr:
                            radio_set_morse_key(true);
                            break;
                    }
                    break;
                    
                case KEYPAD_RELEASE:
                case KEYPAD_LONG_RELEASE:
                    switch (radio_current_mode()) {
                        case x6100_mode_cw:
                        case x6100_mode_cwr:
                            radio_set_morse_key(false);
                            break;
                    }

                    radio_set_ptt(false);
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
        case HKEY_1:
        case HKEY_2:
        case HKEY_3:
        case HKEY_4:
        case HKEY_5:
        case HKEY_6:
        case HKEY_7:
        case HKEY_8:
            if (hkey->state == HKEY_RELEASE) {
                mem_load(hkey->key - HKEY_1 + 1);
            } else if (hkey->state == HKEY_LONG) {
                mem_save(hkey->key - HKEY_1 + 1);
            }
            break;
            
        case HKEY_SPCH:
            if (hkey->state == HKEY_RELEASE) {
                freq_lock = !freq_lock;
                main_screen_set_freq();
            }
            break;
            
        case HKEY_TUNER:
            if (hkey->state == HKEY_RELEASE) {
                radio_change_atu();
                info_params_set();
            } else if (hkey->state == HKEY_LONG) {
                radio_start_atu();
            }
            break;

        case HKEY_XFC:
            if (hkey->state == HKEY_RELEASE) {
                radio_change_vfo();
                info_params_set();
                waterfall_clear();
                spectrum_clear();
                main_screen_band_set();
            }
            break;

        case HKEY_UP:
            if (hkey->state == HKEY_RELEASE) {
                freq_update(+1);
            } else if (hkey->state == HKEY_LONG) {
                bands_change(true);
                
                if (dialog_is_run()) {
                    dialog_send(EVENT_FREQ_UPDATE, NULL);
                }
            }
            break;

        case HKEY_DOWN:
            if (hkey->state == HKEY_RELEASE) {
                freq_update(-1);
            } else if (hkey->state == HKEY_LONG) {
                bands_change(false);

                if (dialog_is_run()) {
                    dialog_send(EVENT_FREQ_UPDATE, NULL);
                }
            }
            break;
        
        case HKEY_F1:
            if (hkey->state == HKEY_RELEASE) {
                next_freq_step(true);
            } else if (hkey->state == HKEY_LONG) {
                next_freq_step(false);
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

static void main_screen_update_cb(lv_event_t * e) {
    main_screen_set_freq();
    info_params_set();

    waterfall_clear();
    spectrum_clear();
}

static void main_screen_atu_update_cb(lv_event_t * e) {
    info_atu_update();
}

static void freq_update(int16_t diff) {
    if (freq_lock) {
        return;
    }
    
    uint64_t        freq, prev_freq;

    freq = radio_change_freq(diff * params_mode.freq_step, &prev_freq);
    waterfall_change_freq(freq - prev_freq);
    spectrum_change_freq(freq - prev_freq);
    main_screen_set_freq();
    check_cross_band(freq, prev_freq);
    
    if (dialog_is_run()) {
        dialog_send(EVENT_FREQ_UPDATE, NULL);
    }
}

static void main_screen_rotary_cb(lv_event_t * e) {
    int32_t     diff = lv_event_get_param(e);
    
    freq_update(diff);
}

static void spectrum_key_cb(lv_event_t * e) {
    uint32_t key = *((uint32_t *)lv_event_get_param(e));

    switch (key) {
        case '-':
            freq_update(-1);
            break;
            
        case '=':
            freq_update(+1);
            break;

        case '_':
            next_freq_step(false);
            break;

        case '+':
            next_freq_step(true);
            break;

        case KEY_VOL_LEFT_EDIT:
        case '[':
            vol_update(-1);
            break;
            
        case KEY_VOL_RIGHT_EDIT:
        case ']':
            vol_update(+1);
            break;

        case KEY_VOL_LEFT_SELECT:
        case '{':
            vol_press(-1);
            break;
            
        case KEY_VOL_RIGHT_SELECT:
        case '}':
            vol_press(+1);
            break;
            
        case KEYBOARD_F9:
            buttons_unload_page();
            buttons_load_page(PAGE_SETTINGS);
            
            dialog_construct(dialog_settings, obj);
            break;
            
        case LV_KEY_LEFT:
            switch (mfk_state) {
                case MFK_STATE_EDIT:
                    mfk_update(-1);
                    break;
                    
                case MFK_STATE_SELECT:
                    mfk_press(-1);
                    break;
            }
            break;
            
        case LV_KEY_RIGHT:
            switch (mfk_state) {
                case MFK_STATE_EDIT:
                    mfk_update(+1);
                    break;
                    
                case MFK_STATE_SELECT:
                    mfk_press(+1);
                    break;
            }
            break;

        case LV_KEY_ESC:
            if (!dialog_is_run()) {
                switch (vol->mode) {
                    case VOL_EDIT:
                        vol->mode = VOL_SELECT;
                        break;
                        
                    case VOL_SELECT:
                        vol->mode = VOL_EDIT;
                        break;
                }
                vol_update(0);
            }
            break;

        case KEYBOARD_PRINT:
        case KEYBOARD_PRINT_SCR:
            screenshot_take();
            break;
            
        case KEYBOARD_SCRL_LOCK:
            freq_lock = !freq_lock;
            main_screen_set_freq();
            break;

        case KEYBOARD_PGUP:
            bands_change(true);
            
            if (dialog_is_run()) {
                dialog_send(EVENT_FREQ_UPDATE, NULL);
            }
            break;

        case KEYBOARD_PGDN:
            bands_change(false);

            if (dialog_is_run()) {
                dialog_send(EVENT_FREQ_UPDATE, NULL);
            }
            break;
            
        case HKEY_FINP:
            dialog_construct(dialog_freq, obj);
            break;
            
        default:
            break;
    }
}

static void spectrum_pressed_cb(lv_event_t * e) {
    switch (mfk_state) {
        case MFK_STATE_EDIT:
            mfk_state = MFK_STATE_SELECT;
            break;
            
        case MFK_STATE_SELECT:
            mfk_state = MFK_STATE_EDIT;
            break;
    }
    mfk_update(0);
}

void main_screen_keys_enable(bool value) {
    if (value) {
        lv_group_add_obj(keyboard_group, spectrum);
        lv_group_set_editing(keyboard_group, true);
    } else {
        lv_group_remove_obj(spectrum);
        lv_group_set_editing(keyboard_group, false);
    }
}

lv_obj_t * main_screen() {
    uint16_t y = 0;

    obj = lv_obj_create(NULL);

    lv_obj_add_event_cb(obj, main_screen_rotary_cb, EVENT_ROTARY, NULL);
    lv_obj_add_event_cb(obj, main_screen_keypad_cb, EVENT_KEYPAD, NULL);
    lv_obj_add_event_cb(obj, main_screen_hkey_cb, EVENT_HKEY, NULL);
    lv_obj_add_event_cb(obj, main_screen_radio_cb, EVENT_RADIO_TX, NULL);
    lv_obj_add_event_cb(obj, main_screen_radio_cb, EVENT_RADIO_RX, NULL);
    lv_obj_add_event_cb(obj, main_screen_update_cb, EVENT_SCREEN_UPDATE, NULL);
    lv_obj_add_event_cb(obj, main_screen_atu_update_cb, EVENT_ATU_UPDATE, NULL);
    
    lv_obj_add_style(obj, &background_style, LV_PART_MAIN);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    
    spectrum = spectrum_init(obj);
    main_screen_keys_enable(true);
    
    lv_obj_add_event_cb(spectrum, spectrum_key_cb, LV_EVENT_KEY, NULL);
    lv_obj_add_event_cb(spectrum, spectrum_pressed_cb, LV_EVENT_PRESSED, NULL);
    
    spectrum_band_set();

    lv_obj_set_y(spectrum, y);
    lv_obj_set_height(spectrum, spectrum_height);
    
    y += spectrum_height;
    
    lv_obj_t *f;

    f = lv_label_create(obj);
    lv_obj_add_style(f, &freq_style, 0);
    lv_obj_set_pos(f, 0, y);
    lv_label_set_recolor(f, true);
    freq[0] = f;

    f = lv_label_create(obj);
    lv_obj_add_style(f, &freq_main_style, 0);
    lv_obj_set_pos(f, 800/2 - 500/2, y);
    lv_label_set_recolor(f, true);
    freq[1] = f;

    f = lv_label_create(obj);
    lv_obj_add_style(f, &freq_style, 0);
    lv_obj_set_pos(f, 800 - 110, y);
    lv_label_set_recolor(f, true);
    freq[2] = f;

    y += freq_height;

    waterfall = waterfall_init(obj);

    waterfall_band_set();
    
    lv_obj_set_y(waterfall, y);
    waterfall_set_height(480 - y);
    
    buttons_init(obj);
    buttons_load_page(PAGE_VOL_1);

    pannel_init(obj);
    msg = msg_init(obj);
    msg_tiny = msg_tiny_init(obj);

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
