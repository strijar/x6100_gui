/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#include "rotary.h"
#include "keyboard.h"

static void rotary_input_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    struct input_event  in;
    rotary_t            *rotary = (rotary_t*) drv->user_data;
    int32_t             diff = 0;
    bool                send = false;

    while (read(rotary->fd, &in, sizeof(struct input_event)) > 0) {
        if (in.type == EV_REL) {
            diff += in.value;
            send = true;
        }
    }
    
    if (send) {
        if (rotary->left == 0 && rotary->right == 0) {
            lv_event_send(lv_scr_act(), EVENT_ROTARY, (void *) diff);
        } else {
            data->state = LV_INDEV_STATE_PRESSED;
            data->key = diff > 0 ? rotary->left[rotary->mode] : rotary->right[rotary->mode];
        }
    }
}

rotary_t * rotary_init(char *dev_name) {
    int fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd == -1) {
        perror("unable to open rotary interface:");

        return NULL;
    }

    fcntl(fd, F_SETFL, O_ASYNC | O_NONBLOCK);

    rotary_t *rotary = malloc(sizeof(rotary_t));
    
    memset(rotary, 0, sizeof(rotary_t));
    rotary->fd = fd;
    
    lv_indev_drv_init(&rotary->indev_drv);
    
    rotary->indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    rotary->indev_drv.read_cb = rotary_input_read;
    rotary->indev_drv.user_data = rotary;
    
    rotary->indev = lv_indev_drv_register(&rotary->indev_drv);

    lv_timer_t *timer = lv_indev_get_read_timer(rotary->indev);
    
    lv_timer_set_period(timer, 10);
    lv_indev_set_group(rotary->indev, keyboard_group());
    
    return rotary;
}
