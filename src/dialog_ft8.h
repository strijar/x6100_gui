/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#pragma once

#include <liquid/liquid.h>
#include "lvgl/lvgl.h"
#include "dialog.h"

typedef enum {
    FT8_OFF = 0,
    FT8_RX,
    FT8_TX
} ft8_state_t;

extern dialog_t *dialog_ft8;

ft8_state_t dialog_ft8_get_state();
void dialog_ft8_put_audio_samples(unsigned int n, float complex *samples);
