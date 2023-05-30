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
#include <errno.h>
#include <pthread.h>
#include <math.h>

#include "lvgl/lvgl.h"
#include "events.h"
#include "gps.h"
#include "dialog_gps.h"

static struct gps_data_t    gpsdata;
static uint64_t             prev_time = 0;

static void * gps_thread(void *arg) {
    while (true) {
        if (gps_waiting(&gpsdata, 1000000)) {
            if (gps_read(&gpsdata, NULL, 0) != -1) {
                if (prev_time != gpsdata.fix.time.tv_sec) {
                    prev_time = gpsdata.fix.time.tv_sec;
                    
                    if (dialog_gps->run) {
                        struct gps_data_t *msg = malloc(sizeof(struct gps_data_t));
                        
                        memcpy(msg, &gpsdata, sizeof(*msg));
                        event_send(dialog_gps->obj, EVENT_GPS, msg);
                    }
                }
            }
        }
    }
}

void gps_init() {
    if (gps_open("localhost", "2947", &gpsdata) == -1) {
        LV_LOG_ERROR("GPSD open: %s", gps_errstr(errno));
        return;
    }
    
    gps_stream(&gpsdata, WATCH_ENABLE | WATCH_JSON, NULL);
    
    pthread_t thread;

    pthread_create(&thread, NULL, gps_thread, NULL);
    pthread_detach(thread);
}
