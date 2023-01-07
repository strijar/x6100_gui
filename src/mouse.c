/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include "lvgl/lvgl.h"
#include "lv_drivers/indev/evdev.h"

#include "mouse.h"

static lv_indev_drv_t       indev_drv_1;

void mouse_init() {
    evdev_init();
 
    lv_indev_drv_init(&indev_drv_1);

    indev_drv_1.type = LV_INDEV_TYPE_POINTER;
    indev_drv_1.read_cb = evdev_read;

    lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);

    LV_IMG_DECLARE(mouse_cursor_icon)

    lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); 
    lv_img_set_src(cursor_obj, &mouse_cursor_icon);
    lv_indev_set_cursor(mouse_indev, cursor_obj);
}
