/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#include "keypad.h"

static event_keypad_t   event;

static void keypad_input_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    struct input_event  in;
    keypad_t            *keypad = (keypad_t*) drv->user_data;

    while (read(keypad->fd, &in, sizeof(struct input_event)) > 0) {
        if (in.type == EV_KEY) {
            switch (in.code) {
                /* Rotary */
                
                case BTN_TRIGGER_HAPPY21:
                    event.key = KEYPAD_ROTARY_VOL;
                    break;
                    
                case BTN_TRIGGER_HAPPY27:
                    event.key = KEYPAD_ROTARY_MFK;
                    break;
                
                /* Front side */
                    
                case BTN_TRIGGER_HAPPY1:
                    event.key = KEYPAD_GEN;
                    break;

                case BTN_TRIGGER_HAPPY7:
                    event.key = KEYPAD_APP;
                    break;

                case BTN_TRIGGER_HAPPY2:
                    event.key = KEYPAD_KEY;
                    break;

                case BTN_TRIGGER_HAPPY8:
                    event.key = KEYPAD_MSG;
                    break;

                case BTN_TRIGGER_HAPPY3:
                    event.key = KEYPAD_DFN;
                    break;

                case BTN_TRIGGER_HAPPY9:
                    event.key = KEYPAD_DFL;
                    break;

                case BTN_TRIGGER_HAPPY13:
                    event.key = KEYPAD_F1;
                    break;

                case BTN_TRIGGER_HAPPY14:
                    event.key = KEYPAD_F2;
                    break;

                case BTN_TRIGGER_HAPPY15:
                    event.key = KEYPAD_F3;
                    break;

                case BTN_TRIGGER_HAPPY19:
                    event.key = KEYPAD_F4;
                    break;

                case BTN_TRIGGER_HAPPY20:
                    event.key = KEYPAD_F5;
                    break;

                case BTN_TRIGGER_HAPPY25:
                    event.key = KEYPAD_LOCK;
                    break;

                /* Top side */ 

                case BTN_TRIGGER_HAPPY4:
                    event.key = KEYPAD_PTT;
                    break;

                case BTN_TRIGGER_HAPPY5:
                    event.key = KEYPAD_BAND_DOWN;
                    break;

                case BTN_TRIGGER_HAPPY6:
                    event.key = KEYPAD_BAND_UP;
                    break;

                case BTN_TRIGGER_HAPPY10:
                    event.key = KEYPAD_MODE_AM;
                    break;

                case BTN_TRIGGER_HAPPY11:
                    event.key = KEYPAD_MODE_CW;
                    break;

                case BTN_TRIGGER_HAPPY12:
                    event.key = KEYPAD_MODE_SSB;
                    break;

                case BTN_TRIGGER_HAPPY16:
                    event.key = KEYPAD_AB;
                    break;

                case BTN_TRIGGER_HAPPY17:
                    event.key = KEYPAD_PRE;
                    break;

                case BTN_TRIGGER_HAPPY18:
                    event.key = KEYPAD_ATU;
                    break;

                case BTN_TRIGGER_HAPPY22:
                    event.key = KEYPAD_VM;
                    break;

                case BTN_TRIGGER_HAPPY23:
                    event.key = KEYPAD_AGC;
                    break;

                case BTN_TRIGGER_HAPPY24:
                    event.key = KEYPAD_FST;
                    break;
                    
                default:
                    event.key = KEYPAD_UNKNOWN;
                    LV_LOG_WARN("Unknown key");
                    break;
            }
        
            event.pressed = in.value;
            lv_event_send(lv_scr_act(), EVENT_KEYPAD, (void*) &event);
        }
    }
}

keypad_t * keypad_init(char *dev_name) {
    int fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd == -1) {
        perror("unable to open keypad interface:");

        return NULL;
    }

    fcntl(fd, F_SETFL, O_ASYNC | O_NONBLOCK);

    keypad_t *keypad = malloc(sizeof(keypad_t));
    
    keypad->fd = fd;
    
    lv_indev_drv_init(&keypad->indev_drv);
    
    keypad->indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    keypad->indev_drv.read_cb = keypad_input_read;
    keypad->indev_drv.user_data = keypad;
    
    keypad->indev = lv_indev_drv_register(&keypad->indev_drv);
    
    return keypad;
}
