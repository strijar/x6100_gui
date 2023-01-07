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

static void rotary_input_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    struct input_event  in;
    rotary_t            *rotary = (rotary_t*) drv->user_data;
    int16_t             diff = 0;
    bool                send = false;

    while (read(rotary->fd, &in, sizeof(struct input_event)) > 0) {
        if (in.type == EV_REL) {
            diff += in.value;
            send = true;
        }
    }
    
    if (send) {
        if (rotary->reverse) {
            diff = -diff;
        }
    
        rotary->event.diff = diff;
        lv_event_send(lv_scr_act(), EVENT_ROTARY, (void*) &rotary->event);
    }
}

rotary_t * rotary_init(char *dev_name, uint8_t id) {
    int fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd == -1) {
        perror("unable to open rotary interface:");

        return NULL;
    }

    fcntl(fd, F_SETFL, O_ASYNC | O_NONBLOCK);

    rotary_t *rotary = malloc(sizeof(rotary_t));
    
    rotary->fd = fd;
    rotary->event.id = id;
    
    lv_indev_drv_init(&rotary->indev_drv);
    
    rotary->indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    rotary->indev_drv.read_cb = rotary_input_read;
    rotary->indev_drv.user_data = rotary;
    
    rotary->indev = lv_indev_drv_register(&rotary->indev_drv);
    
    return rotary;
}
