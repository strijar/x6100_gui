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

#define FLOW_RESTART_TIMOUT 50
#define IDLE_TIMEOUT        (2 * 1000)

static pthread_mutex_t  control_mux;

static x6100_flow_t     *pack;

static uint64_t         now_time;
static uint64_t         prev_time;
static uint64_t         idle_time;

static uint8_t          vfo = X6100_VFO_A;
static uint64_t         freq;
static int16_t          pre = x6100_pre_off;

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
        for (uint16_t i = 0; i < RADIO_SAMPLES; i++) {
            complex float *s = &pack->samples[i];
        
            if (__real__ *s < -1.0f) {
                __real__ *s = -1.0f;
            } else if (__real__ *s > 1.0f) {
                __real__ *s = 1.0f;
            }
            
            if (__imag__ *s < -1.0f) {
                __imag__ *s = -1.0f;
            } else if (__imag__ *s > 1.0f) {
                __imag__ *s = 1.0f;
            }
        }
        
        dsp_samples(pack->samples, RADIO_SAMPLES);
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

void radio_init() {
    if (!x6100_control_init())
        return;

    if (!x6100_gpio_init())
        return;

    if (!x6100_flow_init())
        return;

    pack = malloc(sizeof(x6100_flow_t));

    x6100_control_vfo_mode_set(vfo, x6100_mode_usb);
    x6100_control_vfo_agc_set(vfo, x6100_agc_fast);
    x6100_control_vfo_pre_set(vfo, pre);

    x6100_control_rxvol_set(params.vol);
    x6100_control_rfg_set(params.rfg);
    
    prev_time = get_time();
    idle_time = prev_time;

    pthread_mutex_init(&control_mux, NULL);

    pthread_t thread;

    pthread_create(&thread, NULL, radio_thread, NULL);
    pthread_detach(thread);
}

void radio_set_freq(uint64_t f) {
    freq = f;

    pthread_mutex_lock(&control_mux);
    x6100_control_vfo_freq_set(vfo, f);
    pthread_mutex_unlock(&control_mux);

    main_screen_set_freq(f);
}

uint64_t radio_change_freq(int32_t df) {
    freq += df;
    radio_set_freq(freq);
    
    return freq;
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
    pre = (pre == x6100_pre_off) ? x6100_pre_on : x6100_pre_off;

    pthread_mutex_lock(&control_mux);
    x6100_control_vfo_pre_set(vfo, pre);
    pthread_mutex_unlock(&control_mux);
}
