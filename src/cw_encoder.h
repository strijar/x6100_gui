/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    CW_ENCODER_IDLE = 0,
    CW_ENCODER_SEND,
    CW_ENCODER_BEACON,
    CW_ENCODER_BEACON_IDLE
} cw_encoder_state_t;

void cw_encoder_send(const char *text, bool beacon);
void cw_encoder_stop();

cw_encoder_state_t cw_encoder_state();
