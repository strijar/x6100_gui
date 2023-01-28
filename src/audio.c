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

#include <pulse/pulseaudio.h>

#include "lvgl/lvgl.h"
#include "audio.h"
#include "meter.h"
#include "dsp.h"

static pa_threaded_mainloop *mloop;
static pa_mainloop_api      *mlapi;
static pa_context           *ctx;

static pa_stream            *play_stm;
static char                 *play_device_id;

static void on_state_change(pa_context *c, void *userdata) {
    pa_threaded_mainloop_signal(mloop, 0);
}

static void on_dev_sink(pa_context *c, const pa_sink_info *info, int eol, void *udata) {
    if (eol != 0) {
        pa_threaded_mainloop_signal(mloop, 0);
        return;
    }

    play_device_id = info->name;
    LV_LOG_INFO("Sink %s", info->name);
}

static float phase = 0;

static void write_callback(pa_stream *s, size_t nbytes, void *udata) {
    int16_t *buf = NULL;

    int res = pa_stream_begin_write(play_stm, &buf, &nbytes);
    
    if (res != 0 || buf == NULL) {
        return;
    }
    
    pa_stream_write(play_stm, buf, nbytes, NULL, 0, PA_SEEK_RELATIVE);
}

void audio_init() {
    pthread_t thread;

    mloop = pa_threaded_mainloop_new();
    pa_threaded_mainloop_start(mloop);
    
    mlapi = pa_threaded_mainloop_get_api(mloop);
    ctx = pa_context_new(mlapi, "X6100 GUI");

    pa_threaded_mainloop_lock(mloop);
    pa_context_set_state_callback(ctx, on_state_change, NULL);
    pa_context_connect(ctx, NULL, 0, NULL);
    pa_threaded_mainloop_unlock(mloop);
    
    while (PA_CONTEXT_READY != pa_context_get_state(ctx))  {
        pa_threaded_mainloop_wait(mloop);
    }
    
    LV_LOG_INFO("Conected");
    
    pa_operation    *op;
    
    op = pa_context_get_sink_info_list(ctx, on_dev_sink, NULL);

    /* Play */
    
    pa_sample_spec  spec = {
        .format = PA_SAMPLE_S16NE,
        .rate = 10000,
        .channels = 1
    };
    
    play_stm = pa_stream_new(ctx, "X6100 GUI", &spec, NULL);
    
    pa_buffer_attr  attr;
    memset(&attr, 0xff, sizeof(attr));

    int buffer_length_msec = 200;
    
    attr.tlength = spec.rate * 2 * spec.channels * buffer_length_msec / 1000;

    pa_threaded_mainloop_lock(mloop);
    pa_stream_set_write_callback(play_stm, write_callback, NULL);
    pa_stream_connect_playback(play_stm, play_device_id, &attr, 0, NULL, NULL);
    pa_threaded_mainloop_unlock(mloop);
    
    for (;;) {
        int r = pa_stream_get_state(play_stm);
        
        if (r == PA_STREAM_READY) {
            break;
        } else if (r == PA_STREAM_FAILED) {
            LV_LOG_ERROR("Stream failed");
        }

        pa_threaded_mainloop_wait(mloop);
    }
}
