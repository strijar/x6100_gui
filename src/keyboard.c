/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include "lv_drivers/indev/evdev.h"

#include "keyboard.h"

lv_group_t *keyboard_group;

static lv_indev_drv_t       indev_drv_2;
static lv_group_t           *group;
static bool                 ready = false;

void keyboard_init() {
    keyboard_group = lv_group_create();

    if (!evdev_set_file("/dev/input/event5")) {
        return;
    }

    lv_indev_drv_init(&indev_drv_2);

    indev_drv_2.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv_2.read_cb = evdev_read;

    lv_indev_t *keyboard_indev = lv_indev_drv_register(&indev_drv_2);

    lv_indev_set_group(keyboard_indev, keyboard_group);

    ready = true;
}

bool keyboard_ready() {
    return ready;
}
