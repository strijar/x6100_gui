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

#define FLOW_RESTART_TIMOUT 50
#define IDLE_TIMEOUT        (2 * 1000)

static pthread_mutex_t  control_mux;

static x6100_flow_t     *pack;

static uint64_t         now_time;
static uint64_t         prev_time;
static uint64_t         idle_time;

static uint64_t         freq;
static uint16_t         freq_step = 500;
static int16_t          vol = 0;

bool radio_tick() {
    uint32_t    d = now_time - prev_time;

    if (x6100_flow_read(pack)) {
        prev_time = now_time;
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

    x6100_control_vfo_mode_set(X6100_VFO_A, x6100_mode_usb);
    x6100_control_vfo_agc_set(X6100_VFO_A, x6100_agc_fast);
    x6100_control_vfo_pre_set(X6100_VFO_A, x6100_pre_off);
    
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
    x6100_control_vfo_freq_set(X6100_VFO_A, f);
    pthread_mutex_unlock(&control_mux);

    main_screen_set_freq(f);
}

void radio_change_freq(int32_t df) {
    int16_t d = df * freq_step;

    freq += d;
    radio_set_freq(freq);
    waterfall_change_freq(d);
}

void radio_change_vol(int32_t df) {
    vol += df;
    
    if (vol < 0 ) {
        vol = 0;
    } else if (vol > 50) {
        vol = 50;
    }
    
    pthread_mutex_lock(&control_mux);
    x6100_control_rxvol_set(vol);
    pthread_mutex_unlock(&control_mux);
}
