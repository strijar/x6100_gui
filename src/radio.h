/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#pragma once

#include <stdbool.h>
#include <aether_radio/x6100_control/control.h>

#include "lvgl/lvgl.h"

#define RADIO_SAMPLES   (512)

typedef enum {
    RADIO_MODE_AM = 0,
    RADIO_MODE_CW,
    RADIO_MODE_SSB
} radio_mode_t;

void radio_init(lv_obj_t *obj);
bool radio_tick();

uint64_t radio_change_freq(int32_t df, uint64_t *prev_freq);
uint16_t radio_change_vol(int16_t df);
uint16_t radio_change_rfg(int16_t df);
uint32_t radio_change_filter_low(int32_t freq);
uint32_t radio_change_filter_high(int32_t freq);
bool radio_change_pre();
bool radio_change_att();
void radio_change_mode(radio_mode_t select);
void radio_change_agc();
void radio_change_atu();
float radio_change_pwr(int16_t d);

uint16_t radio_change_key_speed(int16_t d);
x6100_key_mode_t radio_change_key_mode(int16_t d);
x6100_iambic_mode_t radio_change_iambic_mode(int16_t d);
uint16_t radio_change_key_tone(int16_t d);
uint16_t radio_change_key_vol(int16_t d);
bool radio_change_key_train(int16_t d);
uint16_t radio_change_qsk_time(int16_t d);
uint8_t radio_change_key_ratio(int16_t d);

void radio_start_atu();
void radio_load_atu();

void radio_band_set();
void radio_mode_set();

void radio_filter_get(int32_t *from_freq, int32_t *to_freq);
