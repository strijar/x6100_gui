/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#pragma once

#include "lvgl/lvgl.h"

#include <unistd.h>
#include <stdint.h>

typedef struct {
    uint8_t id;
    int16_t diff;
} event_rotary_t;

typedef enum {
    key_unknown = 0,
    
    key_rotary_vol,
    key_rotary_mfk,
    
    key_gen,
    key_app,
    key_key,
    key_msg,
    key_dfn,
    key_dfl,
    
    key_f1,
    key_f2,
    key_f3,
    key_f4,
    key_f5,
    key_lock,
    
    key_ptt,
    key_band_down,
    key_band_up,
    key_mode_am,
    key_mode_cw,
    key_mode_ssb,
    
    key_ab,
    key_pre,
    key_atu,
    key_vm,
    key_agc,
    key_fst
} keypad_key_t;

typedef struct {
    keypad_key_t    key;
    bool            pressed;
} event_keypad_t;

extern uint32_t EVENT_ROTARY;
extern uint32_t EVENT_KEYPAD;

void event_init();

void event_obj_invalidate(lv_obj_t *obj);
void event_obj_check();
