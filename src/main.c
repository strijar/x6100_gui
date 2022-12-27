/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "main_screen.h"
#include "styles.h"
#include "radio.h"
#include "dsp.h"
#include "util.h"
#include "mouse.h"
#include "rotary.h"
#include "spectrum.h"
#include "waterfall.h"

#define DISP_BUF_SIZE (128 * 1024)

static pthread_mutex_t      lv_mux;

static lv_color_t           buf[DISP_BUF_SIZE];
static lv_disp_draw_buf_t   disp_buf;
static lv_disp_drv_t        disp_drv;

void lv_lock() {
    pthread_mutex_lock(&lv_mux);
}

void lv_unlock() {
    pthread_mutex_unlock(&lv_mux);
}

int main(void) {
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

    rotary_t *main = rotary_init("/dev/input/event1", 0);
    rotary_t *vol = rotary_init("/dev/input/event2", 1);
    rotary_t *mfk = rotary_init("/dev/input/event3", 2);

    vol->reverse = true;
    mfk->reverse = true;

    pthread_mutex_init(&lv_mux, NULL);

    styles_init();
    
    lv_obj_t *main_obj = main_screen();

    dsp_init();
    radio_init();

    radio_set_freq(7074000);
    
    uint64_t prev_time = get_time();
    
//    lv_scr_load_anim(main_obj, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 2000, 0, false);
    lv_scr_load(main_obj);

    while (1) {
        lv_lock();
        lv_timer_handler();
        event_obj_check();
        lv_unlock();
        
        usleep(5000);
        
        uint64_t now = get_time();
        lv_tick_inc(now - prev_time);
        prev_time = now;
    }

    return 0;
}
