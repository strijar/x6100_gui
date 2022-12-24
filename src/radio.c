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
#include <sys/time.h>

#include <aether_radio/x6100_control/control.h>
#include <aether_radio/x6100_control/low/gpio.h>
#include <aether_radio/x6100_control/low/flow.h>

#include "radio.h"
#include "dsp.h"

#define FLOW_RESTART_TIMOUT 50

static x6100_flow_t *pack;

static uint64_t     prev_time;

static uint64_t get_time() {
    struct timeval now;
    
    gettimeofday(&now, NULL);

    return (now.tv_sec * 1000000 + now.tv_usec) / 1000;
}

#ifdef RADIO_THREAD
static void * radio_thread(void *arg) { 
    while (true) {
        uint64_t    now_time = get_time();
        uint32_t    d = now_time - prev_time;

        if (!x6100_flow_read(pack)) {
            if (d > FLOW_RESTART_TIMOUT) {
                printf("%i flow restarted\n", d);
                prev_time = now_time;
                x6100_flow_restart();
            } else {
                usleep(5000);
            }
            continue;
        }

        prev_time = now_time;
        dsp_samples(pack->samples, 512);
    }
}
#endif

void radio_tick() {
    uint64_t    now_time = get_time();
    uint32_t    d = now_time - prev_time;

    if (!x6100_flow_read(pack)) {
        if (d > FLOW_RESTART_TIMOUT) {
            printf("%i flow restarted\n", d);
            prev_time = now_time;
            x6100_flow_restart();
        }
        return;
    }

    prev_time = now_time;
    dsp_samples(pack->samples, 512);
}

void radio_init() {
    if (!x6100_control_init())
        return;

    if (!x6100_gpio_init())
        return;

    if (!x6100_flow_init())
        return;

    pack = malloc(sizeof(x6100_flow_t));

    x6100_control_rxvol_set(20);

    x6100_control_vfo_mode_set(X6100_VFO_A, x6100_mode_usb_dig);
    x6100_control_vfo_agc_set(X6100_VFO_A, x6100_agc_fast);
    
    x6100_control_vfo_freq_set(X6100_VFO_A, 14074000);

    prev_time = get_time();

#ifdef RADIO_THREAD
    pthread_t thread;

    pthread_create(&thread, NULL, radio_thread, NULL);
    pthread_detach(thread);
#endif
}
