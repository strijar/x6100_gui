/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <backtrace.h>

#include "main_screen.h"
#include "styles.h"
#include "radio.h"
#include "dsp.h"
#include "util.h"
#include "mouse.h"
#include "rotary.h"
#include "spectrum.h"
#include "waterfall.h"
#include "keypad.h"
#include "params.h"
#include "bands.h"
#include "audio.h"

#define DISP_BUF_SIZE (128 * 1024)

static lv_color_t           buf[DISP_BUF_SIZE];
static lv_disp_draw_buf_t   disp_buf;
static lv_disp_drv_t        disp_drv;

struct backtrace_state      *bt_state;

void handle_signal(int sig, siginfo_t *, void *) {
    printf("-- Crash stack --\n");
    
    backtrace_print(bt_state, 1, stdout);
    exit(EXIT_FAILURE);
}
       
int main(void) {
    bt_state = backtrace_create_state(NULL, 1, NULL, NULL);

    struct sigaction sa;
    
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = &handle_signal;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL);
    
    lv_init();
    fbdev_init();
    event_init();
    
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);
    lv_disp_drv_init(&disp_drv);
    
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = 480;
    disp_drv.ver_res    = 800;
    disp_drv.sw_rotate  = 1;
    disp_drv.rotated    = LV_DISP_ROT_90;
    
    lv_disp_drv_register(&disp_drv);

    lv_disp_set_bg_color(lv_disp_get_default(), lv_color_black());
    lv_disp_set_bg_opa(lv_disp_get_default(), LV_OPA_COVER);

    mouse_init();

    keypad_t *keypad = keypad_init("/dev/input/event0");

    rotary_t *main = rotary_init("/dev/input/event1", 0);
    rotary_t *vol = rotary_init("/dev/input/event2", 1);
    rotary_t *mfk = rotary_init("/dev/input/event3", 2);

    main->reverse = true;
    vol->reverse = true;
    mfk->reverse = true;

    bands_init();
    params_init();
    styles_init();
    
    lv_obj_t *main_obj = main_screen();

    dsp_init();
    radio_init(main_obj);
    audio_init();

    uint64_t prev_time = get_time();

#if 0    
    lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_0, 0);
    lv_scr_load_anim(main_obj, LV_SCR_LOAD_ANIM_FADE_IN, 250, 0, false);
#else
    lv_scr_load(main_obj);
#endif

    while (1) {
        lv_timer_handler();
        event_obj_check();
        
        usleep(1000);
        
        uint64_t now = get_time();
        lv_tick_inc(now - prev_time);
        prev_time = now;
    }

    return 0;
}
