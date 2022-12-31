/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#include <stdlib.h>
#include "events.h"

#define QUEUE_SIZE  16

uint32_t        EVENT_ROTARY;
uint32_t        EVENT_KEYPAD;
uint32_t        EVENT_HKEY;

typedef struct {
    lv_obj_t        *obj;
    lv_event_code_t event_code;
    void            *param;
} item_t;

static item_t   *queue[QUEUE_SIZE];
static uint8_t  queue_write = 0;
static uint8_t  queue_read = 0;

void event_init() {
    EVENT_ROTARY = lv_event_register_id();
    EVENT_KEYPAD = lv_event_register_id();
    EVENT_HKEY = lv_event_register_id();
}

void event_obj_check() {
    while (queue_read != queue_write) {
        queue_read = (queue_read + 1) % QUEUE_SIZE;
        
        item_t *item = queue[queue_read];
        
        if (item->event_code == LV_EVENT_REFRESH) {
            lv_obj_invalidate(item->obj);
        } else {
            lv_event_send(item->obj, item->event_code, item->param);
        }
        
        if (item->param != NULL) {
            free(item->param);
        }
        
        free(item);
    }
}

void event_send(lv_obj_t *obj, lv_event_code_t event_code, void *param) {
    uint8_t next = (queue_write + 1) % QUEUE_SIZE;

    item_t *item = malloc(sizeof(item_t));
    
    item->obj = obj;
    item->event_code = event_code;
    item->param = param;

    queue[next] = item;
    queue_write = next;
}
