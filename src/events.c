/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#include "events.h"

#define QUEUE_SIZE  16

uint32_t        EVENT_ROTARY;
uint32_t        EVENT_KEYPAD;

static lv_obj_t *queue[QUEUE_SIZE];
static uint8_t  queue_write = 0;
static uint8_t  queue_read = 0;

void event_init() {
    EVENT_ROTARY = lv_event_register_id();
    EVENT_KEYPAD = lv_event_register_id();
}

void event_obj_invalidate(lv_obj_t *obj) {
    uint8_t next = (queue_write + 1) % QUEUE_SIZE;

    queue[next] = obj;
    queue_write = next;
}

void event_obj_check() {
    while (queue_read != queue_write) {
        queue_read = (queue_read + 1) % QUEUE_SIZE;
        
        lv_obj_invalidate(queue[queue_read]);
    }
}
