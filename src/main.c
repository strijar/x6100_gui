/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "main_screen.h"
#include "styles.h"
#include "radio.h"
#include "dsp.h"

#define DISP_BUF_SIZE (128 * 1024)

static pthread_mutex_t      lv_mux;

static lv_color_t           buf[DISP_BUF_SIZE];
static lv_disp_draw_buf_t   disp_buf;
static lv_disp_drv_t        disp_drv;
static lv_indev_drv_t       indev_drv_1;

void lv_lock() {
    pthread_mutex_lock(&lv_mux);
}

void lv_unlock() {
    pthread_mutex_unlock(&lv_mux);
}

int main(void) {
    lv_init();
    fbdev_init();
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    lv_disp_drv_init(&disp_drv);
    
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = 480;
    disp_drv.ver_res    = 800;
    disp_drv.sw_rotate  = 1;
    disp_drv.rotated    = LV_DISP_ROT_90;
    
    lv_disp_drv_register(&disp_drv);

    evdev_init();
 
    lv_indev_drv_init(&indev_drv_1);

    indev_drv_1.type = LV_INDEV_TYPE_POINTER;
    indev_drv_1.read_cb = evdev_read;

    lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);

    LV_IMG_DECLARE(mouse_cursor_icon)

    lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); 
    lv_img_set_src(cursor_obj, &mouse_cursor_icon);
    lv_indev_set_cursor(mouse_indev, cursor_obj);

    pthread_mutex_init(&lv_mux, NULL);

    styles_init();
    main_screen();
    dsp_init();
    radio_init();

    radio_set_freq(14074000);

    while (1) {
        lv_lock();
        lv_timer_handler();
#ifndef RADIO_THREAD
        radio_tick();
#endif
        lv_unlock();
        
        usleep(5000);
    }

    return 0;
}

/* Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR` */

uint32_t custom_tick_get(void) {
    static uint64_t start_ms = 0;
    
    if (start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    
    return time_ms;
}
