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
    KEYPAD_UNKNOWN = 0,
    
    KEYPAD_ROTARY_VOL,
    KEYPAD_ROTARY_MFK,
    
    KEYPAD_GEN,
    KEYPAD_APP,
    KEYPAD_KEY,
    KEYPAD_MSG,
    KEYPAD_DFN,
    KEYPAD_DFL,
    
    KEYPAD_F1,
    KEYPAD_F2,
    KEYPAD_F3,
    KEYPAD_F4,
    KEYPAD_F5,
    KEYPAD_LOCK,
    
    KEYPAD_PTT,
    KEYPAD_BAND_DOWN,
    KEYPAD_BAND_UP,
    KEYPAD_MODE_AM,
    KEYPAD_MODE_CW,
    KEYPAD_MODE_SSB,
    
    KEYPAD_AB,
    KEYPAD_PRE,
    KEYPAD_ATU,
    KEYPAD_VM,
    KEYPAD_AGC,
    KEYPAD_FST
} keypad_key_t;

typedef enum {
    KEYPAD_PRESS = 0,
    KEYPAD_RELEASE,
    KEYPAD_LONG
} keypad_state_t;

typedef struct {
    keypad_key_t    key;
    keypad_state_t  state;
} event_keypad_t;

typedef enum {
    HKEY_UNKNOWN = 0,

    HKEY_SPCH,
    HKEY_TUNER,
    HKEY_XFC,
    HKEY_UP,
    HKEY_DOWN,
    HKEY_VM,
    HKEY_NW,
    HKEY_F1,
    HKEY_F2,
    HKEY_1,
    HKEY_2,
    HKEY_3,
    HKEY_4,
    HKEY_5,
    HKEY_6,
    HKEY_7,
    HKEY_8,
    HKEY_9,
    HKEY_DOT,
    HKEY_0, 
    HKEY_CE,
    HKEY_MODE,
    HKEY_FIL,
    HKEY_GENE,
    HKEY_FINP
} hkey_t;

typedef struct {
    hkey_t  key;
    bool    pressed;
} event_hkey_t;

extern uint32_t EVENT_ROTARY;
extern uint32_t EVENT_KEYPAD;
extern uint32_t EVENT_HKEY;

void event_init();

void event_obj_check();
void event_send(lv_obj_t *obj, lv_event_code_t event_code, void *param);
