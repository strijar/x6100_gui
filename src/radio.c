/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include <aether_radio/x6100_control/low/flow.h>
#include <aether_radio/x6100_control/low/gpio.h>

#include "util.h"
#include "radio.h"
#include "dsp.h"
#include "main_screen.h"
#include "waterfall.h"
#include "params.h"
#include "hkey.h"
#include "tx_info.h"
#include "meter.h"
#include "events.h"
#include "clock.h"

#define FLOW_RESTART_TIMOUT 300
#define IDLE_TIMEOUT        (5 * 1000)

static lv_obj_t         *main_obj;

static pthread_mutex_t  control_mux;

static x6100_flow_t     *pack;

static radio_state_t    state = RADIO_RX;
static uint64_t         now_time;
static uint64_t         prev_time;
static uint64_t         idle_time;

static void radio_lock() {
    pthread_mutex_lock(&control_mux);
}

static void radio_unlock() {
    idle_time = get_time();
    pthread_mutex_unlock(&control_mux);
}

bool radio_tick() {
    if (now_time < prev_time) {
        prev_time = now_time;
    }

    int32_t d = now_time - prev_time;

    if (x6100_flow_read(pack)) {
        prev_time = now_time;

        static uint8_t delay = 0;

        if (delay++ > 10) {
            delay = 0;
            clock_update_power(pack->vext * 0.1f, pack->vbat*0.1f, pack->batcap);
        }

        dsp_samples(pack->samples, RADIO_SAMPLES);

        switch (state) {
            case RADIO_RX:
                if (pack->flag.tx) {
                    state = RADIO_TX;
                    event_send(main_obj, EVENT_RADIO_TX, NULL);
                }
                break;

            case RADIO_TX:
                if (!pack->flag.tx) {
                    state = RADIO_RX;
                    event_send(lv_scr_act(), EVENT_RADIO_RX, NULL);
                } else {
                    tx_info_update(pack->tx_power * 0.1f, pack->vswr * 0.1f, pack->alc_level * 0.1f);
                }
                break;

            case RADIO_ATU_START:
                radio_lock();
                x6100_control_atu_tune(true);
                radio_unlock();
                state = RADIO_ATU_WAIT;
                break;
                
            case RADIO_ATU_WAIT:
                if (pack->flag.tx) {
                    event_send(main_obj, EVENT_RADIO_TX, NULL);
                    state = RADIO_ATU_RUN;
                }
                break;
                
            case RADIO_ATU_RUN:
                if (!pack->flag.tx) {
                    params_atu_save(pack->atu_params);
                    radio_lock();
                    x6100_control_atu_tune(false);
                    radio_unlock();
                    event_send(main_obj, EVENT_RADIO_RX, NULL);
                    
                    if (params.atu) {
                        radio_lock();
                        x6100_control_cmd(x6100_atu_network, pack->atu_params);
                        radio_unlock();
                    }
                    state = RADIO_RX;
                } else {
                    tx_info_update(pack->tx_power * 0.1f, pack->vswr * 0.1f, pack->alc_level * 0.1f);
                }
                break;
                
            case RADIO_POWEROFF:
                x6100_control_poweroff();
                state = RADIO_OFF;
                break;
                
            case RADIO_OFF:
                break;
        }
        
        hkey_put(pack->hkey);
    } else {
        if (d > FLOW_RESTART_TIMOUT) {
            LV_LOG_WARN("Flow reset");
            prev_time = now_time;
            x6100_flow_restart();
            dsp_reset();
        }
        return true;
    }
    return false;
}

static void * radio_thread(void *arg) { 
    while (true) {
        now_time = get_time();

        if (radio_tick()) {
            usleep(5000);
        }
        
        int32_t idle = now_time - idle_time;
        
        if (idle > IDLE_TIMEOUT && state == RADIO_RX) {
            pthread_mutex_lock(&control_mux);
            x6100_control_idle();
            pthread_mutex_unlock(&control_mux);
            
            idle_time = now_time;
        }
    }
}

void radio_vfo_set() {
    radio_lock();

    for (int i = 0; i < 2; i++) {
        x6100_control_vfo_mode_set(i, params_band.vfo_x[i].mode);
        x6100_control_vfo_agc_set(i, params_band.vfo_x[i].agc);
        x6100_control_vfo_pre_set(i, params_band.vfo_x[i].pre);
        x6100_control_vfo_att_set(i, params_band.vfo_x[i].att);
        x6100_control_vfo_freq_set(i, params_band.vfo_x[i].freq);
    }

    x6100_control_vfo_set(params_band.vfo);
    x6100_control_split_set(params_band.split);
    radio_unlock();

    params.freq_band = bands_find(params_band.vfo_x[params_band.vfo].freq);
}

void radio_mode_set() {
    radio_lock();
    x6100_control_cmd(x6100_filter1_low, params_mode.filter_low);
    x6100_control_cmd(x6100_filter2_low, params_mode.filter_low);

    x6100_control_cmd(x6100_filter1_high, params_mode.filter_high);
    x6100_control_cmd(x6100_filter2_high, params_mode.filter_high);
    radio_unlock();
}

void radio_init(lv_obj_t *obj) {
    if (!x6100_control_init())
        return;

    if (!x6100_gpio_init())
        return;

    if (!x6100_flow_init())
        return;

    main_obj = obj;

    pack = malloc(sizeof(x6100_flow_t));

    radio_vfo_set();
    radio_mode_set();
    radio_load_atu();

    x6100_control_rxvol_set(params.vol);
    x6100_control_rfg_set(params.rfg);
    x6100_control_atu_set(params.atu);
    x6100_control_txpwr_set(params.pwr);
    x6100_control_charger_set(params.charger);

    x6100_control_key_speed_set(params.key_speed);
    x6100_control_key_mode_set(params.key_mode);
    x6100_control_iambic_mode_set(params.iambic_mode);
    x6100_control_key_tone_set(params.key_tone);
    x6100_control_key_vol_set(params.key_vol);
    x6100_control_key_train_set(params.key_train);
    x6100_control_qsk_time_set(params.qsk_time);
    x6100_control_key_ratio_set(params.key_ratio * 0.1f);

    x6100_control_mic_set(params.mic);
    x6100_control_hmic_set(params.hmic);
    x6100_control_imic_set(params.imic);

    x6100_control_dnf_set(params.dnf);
    x6100_control_dnf_center_set(params.dnf_center);
    x6100_control_dnf_width_set(params.dnf_width);
    x6100_control_nb_set(params.nb);
    x6100_control_nb_level_set(params.nb_level);
    x6100_control_nb_width_set(params.nb_width);
    x6100_control_nr_set(params.nr);
    x6100_control_nr_level_set(params.nr_level);
    
    prev_time = get_time();
    idle_time = prev_time;

    pthread_mutex_init(&control_mux, NULL);

    pthread_t thread;

    pthread_create(&thread, NULL, radio_thread, NULL);
    pthread_detach(thread);
}

radio_state_t radio_get_state() {
    return state;
}

uint64_t radio_change_freq(int32_t df, uint64_t *prev_freq) {
    params_lock();

    *prev_freq = params_band.vfo_x[params_band.vfo].freq;
        
    params_band.vfo_x[params_band.vfo].freq = align_long(params_band.vfo_x[params_band.vfo].freq + df, abs(df));
    params_unlock(&params_band.vfo_x[params_band.vfo].durty.freq);
    
    radio_lock();
    x6100_control_vfo_freq_set(params_band.vfo, params_band.vfo_x[params_band.vfo].freq);
    radio_unlock();

    return params_band.vfo_x[params_band.vfo].freq;
}

uint16_t radio_change_vol(int16_t df) {
    if (df == 0) {
        return params.vol;
    }
    
    params_lock();
    params.vol += df;
    
    if (params.vol < 0 ) {
        params.vol = 0;
    } else if (params.vol > 50) {
        params.vol = 50;
    }

    params_unlock(&params.durty.vol);

    radio_lock();
    x6100_control_rxvol_set(params.vol);
    radio_unlock();
    
    return params.vol;
}

uint16_t radio_change_rfg(int16_t df) {
    if (df == 0) {
        return params.rfg;
    }
    
    params_lock();
    params.rfg += df;
    
    if (params.rfg < 0) {
        params.rfg = 0;
    } else if (params.rfg > 100) {
        params.rfg = 100;
    }
    params_unlock(&params.durty.rfg);

    radio_lock();
    x6100_control_rfg_set(params.rfg);
    radio_unlock();

    return params.rfg;
}

bool radio_change_pre() {
    bool vfoa = (params_band.vfo == X6100_VFO_A);

    params_lock();
    
    params_band.vfo_x[params_band.vfo].pre = !params_band.vfo_x[params_band.vfo].pre;
    params_unlock(&params_band.vfo_x[params_band.vfo].durty.pre);

    radio_lock();
    x6100_control_vfo_pre_set(params_band.vfo, params_band.vfo_x[params_band.vfo].pre);
    radio_unlock();

    return params_band.vfo_x[params_band.vfo].pre;
}

bool radio_change_att() {
    params_lock();
    
    params_band.vfo_x[params_band.vfo].att = !params_band.vfo_x[params_band.vfo].att;
    params_unlock(&params_band.vfo_x[params_band.vfo].durty.att);

    radio_lock();
    x6100_control_vfo_att_set(params_band.vfo, params_band.vfo_x[params_band.vfo].att);
    radio_unlock();

    return params_band.vfo_x[params_band.vfo].att;
}

void radio_filter_get(int32_t *from_freq, int32_t *to_freq) {
    x6100_mode_t    mode = params_band.vfo_x[params_band.vfo].mode;

    switch (mode) {
        case x6100_mode_lsb:
        case x6100_mode_lsb_dig:
            *from_freq = -params_mode.filter_high;
            *to_freq = -params_mode.filter_low;
            break;
            
        case x6100_mode_usb:
        case x6100_mode_usb_dig:
            *from_freq = params_mode.filter_low;
            *to_freq = params_mode.filter_high;
            break;

        case x6100_mode_cw:
            *from_freq = params_mode.filter_low;
            *to_freq = params_mode.filter_high;
            break;
            
        case x6100_mode_cwr:
            *from_freq = -params_mode.filter_high;
            *to_freq = -params_mode.filter_low;
            break;

        case x6100_mode_am:
        case x6100_mode_nfm:
            *from_freq = -params_mode.filter_high;
            *to_freq = params_mode.filter_high;
            break;
            
        default:
            *from_freq = 0;
            *to_freq = 0;
    }
}

void radio_change_mode(radio_mode_t select) {
    params_lock();

    x6100_mode_t    mode = params_band.vfo_x[params_band.vfo].mode;

    switch (select) {
        case RADIO_MODE_AM:
            switch (mode) {
                case x6100_mode_am:
                    mode = x6100_mode_nfm;
                    break;
                    
                case x6100_mode_nfm:
                    mode = x6100_mode_am;
                    break;
                    
                default:
                    mode = x6100_mode_am;
                    break;
            }
            break;
            
        case RADIO_MODE_CW:
            switch (mode) {
                case x6100_mode_cw:
                    mode = x6100_mode_cwr;
                    break;
                    
                case x6100_mode_cwr:
                    mode = x6100_mode_cw;
                    break;
                    
                default:
                    mode = x6100_mode_cw;
                    break;
            }
            break;
            
        case RADIO_MODE_SSB:
            switch (mode) {
                case x6100_mode_lsb:
                    mode = x6100_mode_lsb_dig;
                    break;
                    
                case x6100_mode_lsb_dig:
                    mode = x6100_mode_usb;
                    break;
                    
                case x6100_mode_usb:
                    mode = x6100_mode_usb_dig;
                    break;
                    
                case x6100_mode_usb_dig:
                    mode = x6100_mode_lsb;
                    break;
                    
                default: 
                    mode = x6100_mode_lsb;
                    break;
            }
            break;
            
        default:
            break;
    }

    params_mode_save();

    params_band.vfo_x[params_band.vfo].mode = mode;
    params_unlock(&params_band.vfo_x[params_band.vfo].durty.mode);

    radio_lock();
    x6100_control_vfo_mode_set(params_band.vfo, mode);
    radio_unlock();
}

uint32_t radio_change_filter_low(int32_t df) {
    params_lock();

    params_mode.filter_low = align_int(params_mode.filter_low + df * 50, 50);
    
    if (params_mode.filter_low < 0) {
        params_mode.filter_low = 0;
    } else if (params_mode.filter_low > 6000) {
        params_mode.filter_low = 6000;
    } else if (params_mode.filter_low > params_mode.filter_high) {
        params_mode.filter_low = params_mode.filter_high;
    }
    params_unlock(&params_mode.durty.filter_low);

    radio_lock();
    x6100_control_cmd(x6100_filter1_low, params_mode.filter_low);
    x6100_control_cmd(x6100_filter2_low, params_mode.filter_low);
    radio_unlock();
    
    return params_mode.filter_low;
}

uint32_t radio_change_filter_high(int32_t df) {
    params_lock();
    params_mode.filter_high = align_int(params_mode.filter_high + df * 50, 50);

    if (params_mode.filter_high < 0) {
        params_mode.filter_high = 0;
    } else if (params_mode.filter_high > 6000) {
        params_mode.filter_high = 6000;
    } else if (params_mode.filter_high < params_mode.filter_low) {
        params_mode.filter_high = params_mode.filter_low;
    }
    params_unlock(&params_mode.durty.filter_high);

    radio_lock();
    x6100_control_cmd(x6100_filter1_high, params_mode.filter_high);
    x6100_control_cmd(x6100_filter2_high, params_mode.filter_high);
    radio_unlock();
    
    return params_mode.filter_high;
}

void radio_change_agc() {
    params_lock();

    x6100_agc_t     agc = params_band.vfo_x[params_band.vfo].agc;
    
    switch (agc) {
        case x6100_agc_off:
            agc = x6100_agc_slow;
            break;
            
        case x6100_agc_slow:
            agc = x6100_agc_fast;
            break;
            
        case x6100_agc_fast:
            agc = x6100_agc_auto;
            break;
            
        case x6100_agc_auto:
            agc = x6100_agc_off;
            break;
    }

    params_band.vfo_x[params_band.vfo].agc = agc;
    params_unlock(&params_band.vfo_x[params_band.vfo].durty.agc);

    radio_lock();
    x6100_control_vfo_agc_set(params_band.vfo, agc);
    radio_unlock();
}

void radio_change_atu() {
    params_lock();
    params.atu = !params.atu;
    params_unlock(&params.durty.atu);

    radio_lock();
    x6100_control_atu_set(params.atu);
    radio_unlock();
    
    radio_load_atu();
}

void radio_start_atu() {
    if (state == RADIO_RX) {
        state = RADIO_ATU_START;
    }
}

void radio_load_atu() {
    if (params.atu) {
        uint32_t atu = params_atu_load();

        radio_lock();
        x6100_control_cmd(x6100_atu_network, atu);
        radio_unlock();
    }
}

float radio_change_pwr(int16_t d) {
    params_lock();
    params.pwr += d * 0.1f;
    
    if (params.pwr > 10.0f) {
        params.pwr = 10.0f;
    } else if (params.pwr < 0.1f) {
        params.pwr = 0.1f;
    }
    
    params_unlock(&params.durty.pwr);

    radio_lock();
    x6100_control_txpwr_set(params.pwr);
    radio_unlock();
    
    return params.pwr;
}

uint16_t radio_change_key_speed(int16_t d) {
    params_lock();
    params.key_speed += d;
    
    if (params.key_speed < 5) {
        params.key_speed = 5;
    } else if (params.key_speed > 50) {
        params.key_speed = 50;
    }

    params_unlock(&params.durty.key_speed);

    radio_lock();
    x6100_control_key_speed_set(params.key_speed);
    radio_unlock();

    return params.key_speed;
}

x6100_key_mode_t radio_change_key_mode(int16_t d) {
    if (d == 0) {
        return params.key_mode;
    }

    params_lock();

    switch (params.key_mode) {
        case x6100_key_manual:
            params.key_mode = d > 0 ? x6100_key_auto_left : x6100_key_auto_right;
            break;
            
        case x6100_key_auto_left:
            params.key_mode = d > 0 ? x6100_key_auto_right : x6100_key_manual;
            break;
            
        case x6100_key_auto_right:
            params.key_mode = d > 0 ? x6100_key_manual : x6100_key_auto_left;
            break;
    }

    params_unlock(&params.durty.key_mode);

    radio_lock();
    x6100_control_key_mode_set(params.key_mode);
    radio_unlock();

    return params.key_mode;
}

x6100_iambic_mode_t radio_change_iambic_mode(int16_t d) {
    if (d == 0) {
        return params.iambic_mode;
    }

    params_lock();

    params.iambic_mode = (params.iambic_mode == x6100_iambic_a) ? x6100_iambic_b : x6100_iambic_a;

    params_unlock(&params.durty.iambic_mode);

    radio_lock();
    x6100_control_iambic_mode_set(params.iambic_mode);
    radio_unlock();

    return params.iambic_mode;
}

uint16_t radio_change_key_tone(int16_t d) {
    if (d == 0) {
        return params.key_tone;
    }

    params_lock();

    params.key_tone += (d > 0) ? 10 : -10;
    
    if (params.key_tone < 400) {
        params.key_tone = 400;
    } else if (params.key_tone > 1200) {
        params.key_tone = 1200;
    }

    params_unlock(&params.durty.key_tone);

    radio_lock();
    x6100_control_key_tone_set(params.key_tone);
    radio_unlock();
    
    return params.key_tone;
}

uint16_t radio_change_key_vol(int16_t d) {
    if (d == 0) {
        return params.key_vol;
    }

    params_lock();

    int16_t x = params.key_vol + d;
    
    if (x < 0) {
        x = 0;
    } else if (x > 32) {
        x = 32;
    }

    params.key_vol = x;
    params_unlock(&params.durty.key_vol);

    radio_lock();
    x6100_control_key_vol_set(params.key_vol);
    radio_unlock();

    return params.key_vol;
}

bool radio_change_key_train(int16_t d) {
    if (d == 0) {
        return params.key_train;
    }

    params_lock();
    params.key_train = !params.key_train;
    params_unlock(&params.durty.key_train);

    radio_lock();
    x6100_control_key_train_set(params.key_train);
    radio_unlock();
    
    return params.key_train;
}

uint16_t radio_change_qsk_time(int16_t d) {
    if (d == 0) {
        return params.qsk_time;
    }

    params_lock();

    int16_t x = params.qsk_time;
    
    if (d > 0) {
        x += 10;
        
        if (x > 1000) {
            x = 1000;
        }
    } else {
        x -= 10;
        
        if (x < 0) {
            x = 0;
        }
    }
    
    params.qsk_time = x;
    params_unlock(&params.durty.qsk_time);

    radio_lock();
    x6100_control_qsk_time_set(params.qsk_time);
    radio_unlock();
    
    return params.qsk_time;
}

uint8_t radio_change_key_ratio(int16_t d) {
    if (d == 0) {
        return params.key_ratio;
    }

    params_lock();

    int16_t x = params.key_ratio;

    if (d > 0) {
        x += 5;
        
        if (x > 45) {
            x = 45;
        }
    } else {
        x -= 5;
        
        if (x < 25) {
            x = 25;
        }
    }

    params.key_ratio = x;
    params_unlock(&params.durty.key_ratio);

    radio_lock();
    x6100_control_key_ratio_set(params.key_ratio * 0.1f);
    radio_unlock();

    return params.key_ratio;
}

x6100_mic_sel_t radio_change_mic(int16_t d) {
    if (d == 0) {
        return params.mic;
    }
    
    params_lock();
    
    switch (params.mic) {
        case x6100_mic_builtin:
            params.mic = d > 0 ? x6100_mic_handle : x6100_mic_auto;
            break;
            
        case x6100_mic_handle:
            params.mic = d > 0 ? x6100_mic_auto : x6100_mic_builtin;
            break;
            
        case x6100_mic_auto:
            params.mic = d > 0 ? x6100_mic_builtin : x6100_mic_handle;
            break;
    }
    
    params_unlock(&params.durty.mic);
    
    radio_lock();
    x6100_control_mic_set(params.mic);
    radio_unlock();
    
    return params.mic;
}

uint8_t radio_change_hmic(int16_t d) {
    if (d == 0) {
        return params.hmic;
    }

    int16_t x = params.hmic + d;
    
    if (x < 0) {
        x = 0;
    } else if (x > 50) {
        x = 50;
    }
    
    params_lock();
    params.hmic = x;
    params_unlock(&params.durty.hmic);
    
    radio_lock();
    x6100_control_hmic_set(params.hmic);
    radio_unlock();
    
    return params.hmic;
}

uint8_t radio_change_imic(int16_t d) {
    if (d == 0) {
        return params.imic;
    }

    int16_t x = params.imic + d;
    
    if (x < 0) {
        x = 0;
    } else if (x > 50) {
        x = 50;
    }
    
    params_lock();
    params.imic = x;
    params_unlock(&params.durty.imic);
    
    radio_lock();
    x6100_control_hmic_set(params.imic);
    radio_unlock();
    
    return params.imic;
}

x6100_vfo_t radio_change_vfo() {
    params_lock();
    params_band.vfo = (params_band.vfo == X6100_VFO_A) ? X6100_VFO_B : X6100_VFO_A;
    params_unlock(&params_band.durty.vfo);
    
    radio_lock();
    x6100_control_vfo_set(params_band.vfo);
    radio_unlock();
    
    return params_band.vfo;
}

void radio_change_split() {
    params_lock();
    params_band.split = !params_band.split;
    params_unlock(&params_band.durty.split);
    
    radio_lock();
    x6100_control_split_set(params_band.split);
    radio_unlock();
}

void radio_poweroff() {
    state = RADIO_POWEROFF;
}

bool radio_change_charger(int16_t d) {
    if (d == 0) {
        return params.charger;
    }

    params_lock();
    params.charger = !params.charger;
    params_unlock(&params.durty.charger);
    
    radio_lock();
    x6100_control_charger_set(params.charger);
    radio_unlock();
    
    return params.charger;
}

bool radio_change_dnf(int16_t d) {
    if (d == 0) {
        return params.dnf;
    }

    params_lock();
    params.dnf = !params.dnf;
    params_unlock(&params.durty.dnf);
    
    radio_lock();
    x6100_control_dnf_set(params.dnf);
    radio_unlock();

    return params.dnf;
}

uint16_t radio_change_dnf_center(int16_t d) {
    if (d == 0) {
        return params.dnf_center;
    }

    int16_t x = params.dnf_center + d * 50;
    
    if (x < 100) {
        x = 100;
    } else if (x > 3000) {
        x = 3000;
    }
    
    params_lock();
    params.dnf_center = x;
    params_unlock(&params.durty.dnf_center);
    
    radio_lock();
    x6100_control_dnf_center_set(params.dnf_center);
    radio_unlock();

    return params.dnf_center;
}

uint16_t radio_change_dnf_width(int16_t d) {
    if (d == 0) {
        return params.dnf_width;
    }

    int16_t x = params.dnf_width + d * 5;
    
    if (x < 10) {
        x = 10;
    } else if (x > 100) {
        x = 100;
    }
    
    params_lock();
    params.dnf_width = x;
    params_unlock(&params.durty.dnf_width);
    
    radio_lock();
    x6100_control_dnf_width_set(params.dnf_width);
    radio_unlock();

    return params.dnf_width;
}

bool radio_change_nb(int16_t d) {
    if (d == 0) {
        return params.nb;
    }

    params_lock();
    params.nb = !params.nb;
    params_unlock(&params.durty.nb);
    
    radio_lock();
    x6100_control_nb_set(params.nb);
    radio_unlock();

    return params.nb;
}

uint8_t radio_change_nb_level(int16_t d) {
    if (d == 0) {
        return params.nb_level;
    }

    int16_t x = params.nb_level + d * 5;
    
    if (x < 0) {
        x = 0;
    } else if (x > 100) {
        x = 100;
    }
    
    params_lock();
    params.nb_level = x;
    params_unlock(&params.durty.nb_level);
    
    radio_lock();
    x6100_control_nb_level_set(params.nb_level);
    radio_unlock();

    return params.nb_level;
}

uint8_t radio_change_nb_width(int16_t d) {
    if (d == 0) {
        return params.nb_width;
    }

    int16_t x = params.nb_width + d * 5;
    
    if (x < 0) {
        x = 0;
    } else if (x > 100) {
        x = 100;
    }
    
    params_lock();
    params.nb_width = x;
    params_unlock(&params.durty.nb_width);
    
    radio_lock();
    x6100_control_nb_width_set(params.nb_width);
    radio_unlock();

    return params.nb_width;
}

bool radio_change_nr(int16_t d) {
    if (d == 0) {
        return params.nr;
    }

    params_lock();
    params.nr = !params.nr;
    params_unlock(&params.durty.nr);
    
    radio_lock();
    x6100_control_nr_set(params.nr);
    radio_unlock();

    return params.nr;
}

uint8_t radio_change_nr_level(int16_t d) {
    if (d == 0) {
        return params.nr_level;
    }

    int16_t x = params.nr_level + d * 5;
    
    if (x < 0) {
        x = 0;
    } else if (x > 60) {
        x = 60;
    }
    
    params_lock();
    params.nr_level = x;
    params_unlock(&params.durty.nr_level);
    
    radio_lock();
    x6100_control_nr_level_set(params.nr_level);
    radio_unlock();

    return params.nr_level;
}
