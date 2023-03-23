/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <stdlib.h>
#include <aether_radio/x6100_control/low/flow.h>
#include "lvgl/lvgl.h"

#include "hkey.h"
#include "events.h"
#include "backlight.h"

static uint32_t prev_key = 0;

void hkey_put(uint32_t key) {
    if (prev_key != key) {
        event_hkey_t    *event = malloc(sizeof(event_hkey_t));
        uint32_t        k;

        if (key == 0) {
            event->pressed = false;
            k = prev_key;
        } else {
            event->pressed = true;
            k = key;
        }
        
        switch (k) {
            case X6100_HKEY_SPCH:
                event->key = HKEY_SPCH;
                break;
                
            case X6100_HKEY_TUNER:
                event->key = HKEY_TUNER;
                break;

            case X6100_HKEY_XFC:
                event->key = HKEY_XFC;
                break;

            case X6100_HKEY_UP:
                event->key = HKEY_UP;
                break;

            case X6100_HKEY_DOWN:
                event->key = HKEY_DOWN;
                break;

            case X6100_HKEY_VM:
                event->key = HKEY_VM;
                break;

            case X6100_HKEY_NW:
                event->key = HKEY_NW;
                break;

            case X6100_HKEY_F1:
                event->key = HKEY_F1;
                break;

            case X6100_HKEY_F2:
                event->key = HKEY_F2;
                break;

            case X6100_HKEY_1:
                event->key = HKEY_1;
                break;

            case X6100_HKEY_2:
                event->key = HKEY_2;
                break;

            case X6100_HKEY_3:
                event->key = HKEY_3;
                break;

            case X6100_HKEY_4:
                event->key = HKEY_4;
                break;

            case X6100_HKEY_5:
                event->key = HKEY_5;
                break;

            case X6100_HKEY_6:
                event->key = HKEY_6;
                break;

            case X6100_HKEY_7:
                event->key = HKEY_7;
                break;

            case X6100_HKEY_8:
                event->key = HKEY_8;
                break;

            case X6100_HKEY_9:
                event->key = HKEY_9;
                break;

            case X6100_HKEY_DOT:
                event->key = HKEY_DOT;
                break;

            case X6100_HKEY_0:
                event->key = HKEY_0;
                break;

            case X6100_HKEY_CE:
                event->key = HKEY_CE;
                break;

            case X6100_HKEY_MODE:
                event->key = HKEY_MODE;
                break;

            case X6100_HKEY_FIL:
                event->key = HKEY_FIL;
                break;

            case X6100_HKEY_GENE:
                event->key = HKEY_GENE;
                break;

            case X6100_HKEY_FINP:
                event->key = HKEY_FINP;
                break;
                
            default:
                event->key = HKEY_UNKNOWN;
                break;
        }

        event_send(lv_scr_act(), EVENT_HKEY, (void*) event);
        prev_key = key;
        
        backlight_tick();
    }
}
