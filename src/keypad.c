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
                    event.key = key_rotary_vol;
                    break;
                    
                case BTN_TRIGGER_HAPPY27:
                    event.key = key_rotary_mfk;
                    break;
                
                /* Front side */
                    
                case BTN_TRIGGER_HAPPY1:
                    event.key = key_gen;
                    break;

                case BTN_TRIGGER_HAPPY7:
                    event.key = key_app;
                    break;

                case BTN_TRIGGER_HAPPY2:
                    event.key = key_key;
                    break;

                case BTN_TRIGGER_HAPPY8:
                    event.key = key_msg;
                    break;

                case BTN_TRIGGER_HAPPY3:
                    event.key = key_dfn;
                    break;

                case BTN_TRIGGER_HAPPY9:
                    event.key = key_dfl;
                    break;

                case BTN_TRIGGER_HAPPY13:
                    event.key = key_f1;
                    break;

                case BTN_TRIGGER_HAPPY14:
                    event.key = key_f2;
                    break;

                case BTN_TRIGGER_HAPPY15:
                    event.key = key_f3;
                    break;

                case BTN_TRIGGER_HAPPY19:
                    event.key = key_f4;
                    break;

                case BTN_TRIGGER_HAPPY20:
                    event.key = key_f5;
                    break;

                case BTN_TRIGGER_HAPPY25:
                    event.key = key_lock;
                    break;

                /* Top side */ 

                case BTN_TRIGGER_HAPPY4:
                    event.key = key_ptt;
                    break;

                case BTN_TRIGGER_HAPPY5:
                    event.key = key_band_down;
                    break;

                case BTN_TRIGGER_HAPPY6:
                    event.key = key_band_up;
                    break;

                case BTN_TRIGGER_HAPPY10:
                    event.key = key_mode_am;
                    break;

                case BTN_TRIGGER_HAPPY11:
                    event.key = key_mode_cw;
                    break;

                case BTN_TRIGGER_HAPPY12:
                    event.key = key_mode_ssb;
                    break;

                case BTN_TRIGGER_HAPPY16:
                    event.key = key_ab;
                    break;

                case BTN_TRIGGER_HAPPY17:
                    event.key = key_pre;
                    break;

                case BTN_TRIGGER_HAPPY18:
                    event.key = key_atu;
                    break;

                case BTN_TRIGGER_HAPPY22:
                    event.key = key_vm;
                    break;

                case BTN_TRIGGER_HAPPY23:
                    event.key = key_agc;
                    break;

                case BTN_TRIGGER_HAPPY24:
                    event.key = key_fst;
                    break;
                    
                default:
                    event.key = key_unknown;
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
