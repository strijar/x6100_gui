/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "lvgl/lvgl.h"

#include <aether_radio/x6100_control/control.h>
#include <aether_radio/x6100_control/low/gpio.h>
#include <aether_radio/x6100_control/low/flow.h>

#include "util.h"
#include "radio.h"
#include "dsp.h"
#include "main_screen.h"
#include "waterfall.h"
#include "params.h"
#include "hkey.h"

#define FLOW_RESTART_TIMOUT 300
#define IDLE_TIMEOUT        (2 * 1000)

static pthread_mutex_t  control_mux;

static x6100_flow_t     *pack;

static uint64_t         now_time;
static uint64_t         prev_time;
static uint64_t         idle_time;

bool radio_tick() {
    if (now_time < prev_time) {
        prev_time = now_time;
    }

    int32_t d = now_time - prev_time;

    if (x6100_flow_read(pack)) {
        prev_time = now_time;
#if 0
        static uint8_t delay = 0;

        if (delay++ > 10) {
            delay = 0;
            printf("tx=%d "
                   "txpwr=%.1f swr=%.1f alc=%.1f vext=%.1f vbat=%.1f bat=%d hkey=%02X CRC=%08X\n",
                   pack->flag.tx, pack->tx_power * 0.1, pack->vswr * 0.1f, pack->alc_level * 0.1,
                   pack->vext * 0.1f, pack->vbat * 0.1f, pack->batcap, pack->hkey, pack->crc);
        }
#endif        
        dsp_samples(pack->samples, RADIO_SAMPLES);
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
        
        if (idle > IDLE_TIMEOUT) {
            pthread_mutex_lock(&control_mux);
            x6100_control_idle();
            pthread_mutex_unlock(&control_mux);
            
            idle_time = now_time;
        }
    }
}

void radio_band_set() {
    bool vfoa = (params_band.vfo == X6100_VFO_A);

    x6100_control_vfo_mode_set(params_band.vfo, vfoa ? params_band.vfoa_mode : params_band.vfob_mode);
    x6100_control_vfo_agc_set(params_band.vfo, vfoa ? params_band.vfoa_agc : params_band.vfob_agc);
    x6100_control_vfo_pre_set(params_band.vfo, vfoa ? params_band.vfoa_pre : params_band.vfob_pre);
    x6100_control_vfo_att_set(params_band.vfo, vfoa ? params_band.vfoa_att : params_band.vfob_att);
    x6100_control_vfo_freq_set(params_band.vfo, vfoa ? params_band.vfoa_freq : params_band.vfob_freq);

    x6100_control_cmd(x6100_filter1_low, params_band.filter_low);
    x6100_control_cmd(x6100_filter2_low, params_band.filter_low);

    x6100_control_cmd(x6100_filter1_high, params_band.filter_high);
    x6100_control_cmd(x6100_filter2_high, params_band.filter_high);
}

void radio_init() {
    if (!x6100_control_init())
        return;

    if (!x6100_gpio_init())
        return;

    if (!x6100_flow_init())
        return;

    pack = malloc(sizeof(x6100_flow_t));

    radio_band_set();

    x6100_control_rxvol_set(params.vol);
    x6100_control_rfg_set(params.rfg);
    
    prev_time = get_time();
    idle_time = prev_time;

    pthread_mutex_init(&control_mux, NULL);

    pthread_t thread;

    pthread_create(&thread, NULL, radio_thread, NULL);
    pthread_detach(thread);
}

uint64_t radio_change_freq(int32_t df) {
    bool vfoa = (params_band.vfo == X6100_VFO_A);

    params_lock();

    if (vfoa) {
        params_band.vfoa_freq += df;
        params_unlock(&params_band.durty.vfoa_freq);
    } else {
        params_band.vfob_freq += df;
        params_unlock(&params_band.durty.vfob_freq);
    }
    
    x6100_control_vfo_freq_set(params_band.vfo, vfoa ? params_band.vfoa_freq : params_band.vfob_freq);

    return vfoa ? params_band.vfoa_freq : params_band.vfob_freq;
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
    
    pthread_mutex_lock(&control_mux);
    x6100_control_rxvol_set(params.vol);
    pthread_mutex_unlock(&control_mux);
    
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

    pthread_mutex_lock(&control_mux);
    x6100_control_rfg_set(params.rfg);
    pthread_mutex_unlock(&control_mux);

    return params.rfg;
}

bool radio_change_pre() {
    bool vfoa = (params_band.vfo == X6100_VFO_A);

    params_lock();
    
    if (vfoa) {
        params_band.vfoa_pre = !params_band.vfoa_pre;
        params_unlock(&params_band.durty.vfoa_pre);
    } else {
        params_band.vfob_pre = !params_band.vfob_pre;
        params_unlock(&params_band.durty.vfob_pre);
    }

    pthread_mutex_lock(&control_mux);
    x6100_control_vfo_pre_set(params_band.vfo, vfoa ? params_band.vfoa_pre : params_band.vfob_pre);
    pthread_mutex_unlock(&control_mux);

    return vfoa ? params_band.vfoa_pre : params_band.vfoa_pre;
}

bool radio_change_att() {
    bool vfoa = (params_band.vfo == X6100_VFO_A);

    params_lock();
    
    if (vfoa) {
        params_band.vfoa_att = !params_band.vfoa_att;
        params_unlock(&params_band.durty.vfoa_att);
    } else {
        params_band.vfob_att = !params_band.vfob_att;
        params_unlock(&params_band.durty.vfob_att);
    }

    pthread_mutex_lock(&control_mux);
    x6100_control_vfo_att_set(params_band.vfo, vfoa ? params_band.vfoa_att : params_band.vfob_att);
    pthread_mutex_unlock(&control_mux);

    return vfoa ? params_band.vfoa_att : params_band.vfoa_att;
}

void radio_filter_get(int32_t *from_freq, int32_t *to_freq) {
    bool            vfoa = (params_band.vfo == X6100_VFO_A);
    x6100_mode_t    mode = vfoa ? params_band.vfoa_mode : params_band.vfob_mode;

    switch (mode) {
        case x6100_mode_lsb:
        case x6100_mode_lsb_dig:
            *from_freq = -params_band.filter_high;
            *to_freq = -params_band.filter_low;
            break;
            
        case x6100_mode_usb:
        case x6100_mode_usb_dig:
            *from_freq = params_band.filter_low;
            *to_freq = params_band.filter_high;
            break;

        case x6100_mode_cw:
            *from_freq = params_band.filter_low;
            *to_freq = params_band.filter_high;
            break;
            
        case x6100_mode_cwr:
            *from_freq = -params_band.filter_high;
            *to_freq = -params_band.filter_low;
            break;

        case x6100_mode_am:
        case x6100_mode_nfm:
            *from_freq = -params_band.filter_high;
            *to_freq = params_band.filter_high;
            break;
            
        default:
            *from_freq = 0;
            *to_freq = 0;
    }
}

void radio_change_mode(radio_mode_t select) {
    params_lock();

    bool            vfoa = (params_band.vfo == X6100_VFO_A);
    x6100_mode_t    mode = vfoa ? params_band.vfoa_mode : params_band.vfob_mode;

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

    if (vfoa) {
        params_band.vfoa_mode = mode;
        params_unlock(&params_band.durty.vfoa_mode);
    } else {
        params_band.vfob_mode = mode;
        params_unlock(&params_band.durty.vfob_mode);
    }

    pthread_mutex_lock(&control_mux);
    x6100_control_vfo_mode_set(params_band.vfo, mode);
    pthread_mutex_unlock(&control_mux);
}

uint32_t radio_change_filter_low(int32_t df) {
    params_lock();

    params_band.filter_low = align_int(params_band.filter_low + df * 50, 50);
    
    if (params_band.filter_low < 0) {
        params_band.filter_low = 0;
    } else if (params_band.filter_low > 6000) {
        params_band.filter_low = 6000;
    } else if (params_band.filter_low > params_band.filter_high) {
        params_band.filter_low = params_band.filter_high;
    }
    params_unlock(&params_band.durty.filter_low);

    pthread_mutex_lock(&control_mux);
    x6100_control_cmd(x6100_filter1_low, params_band.filter_low);
    x6100_control_cmd(x6100_filter2_low, params_band.filter_low);
    pthread_mutex_unlock(&control_mux);
    
    return params_band.filter_low;
}

uint32_t radio_change_filter_high(int32_t df) {
    params_lock();
    params_band.filter_high = align_int(params_band.filter_high + df * 50, 50);

    if (params_band.filter_high < 0) {
        params_band.filter_high = 0;
    } else if (params_band.filter_high > 6000) {
        params_band.filter_high = 6000;
    } else if (params_band.filter_high < params_band.filter_low) {
        params_band.filter_high = params_band.filter_low;
    }
    params_unlock(&params_band.durty.filter_high);

    pthread_mutex_lock(&control_mux);
    x6100_control_cmd(x6100_filter1_high, params_band.filter_high);
    x6100_control_cmd(x6100_filter2_high, params_band.filter_high);
    pthread_mutex_unlock(&control_mux);
    
    return params_band.filter_high;
}
