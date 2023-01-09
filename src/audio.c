/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>

#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/gccmacro.h>

#include "lvgl/lvgl.h"
#include "audio.h"
#include "meter.h"

#define BUFSIZE 1000

static pa_simple    *rec_stream = NULL;
static int16_t      rec_buf[BUFSIZE];

static void * record_thread(void *arg) {
    int     error;

    while (true) {
        int res = pa_simple_read(rec_stream, rec_buf, sizeof(rec_buf), &error);
        
        if (res >= 0) {
        }
    }
}

void audio_init() {
    static const pa_sample_spec rec_spec = {
        .format = PA_SAMPLE_S16NE,
        .rate = 12000,
        .channels = 1
    };
    
    static const pa_buffer_attr buf_attr = {
        .maxlength = sizeof(rec_buf) * 4,
        .fragsize = sizeof(rec_buf),
    };
    
    int error;

    rec_stream = pa_simple_new(NULL, "x6100", PA_STREAM_RECORD, NULL, "record", &rec_spec, NULL, &buf_attr, &error);
    
    if (!rec_stream) {
        LV_LOG_ERROR("Record stream");
        return;
    }

    pthread_t thread;

    pthread_create(&thread, NULL, record_thread, NULL);
    pthread_detach(thread);
}
