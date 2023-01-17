/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#pragma once

#include <stdint.h>

typedef enum {
    MFK_MIN_LEVEL = 0,
    MFK_MAX_LEVEL,
    MFK_SPECTRUM_FACTOR,
    MFK_SPECTRUM_BETA,
    MFK_PEAK_HOLD,
    MFK_PEAK_SPEED,
    MFK_LAST,

    MFK_SPECTRUM_FILL,
    MFK_SPECTRUM_PEAK,
    
    MFK_KEY_SPEED,
    MFK_KEY_MODE,
    MFK_IAMBIC_MODE,
    MFK_KEY_TONE,
    MFK_KEY_VOL,
    MFK_KEY_TRAIN,
    MFK_QSK_TIME,
    MFK_KEY_RATIO
} mfk_mode_t;

void mfk_update(int16_t diff);
void mfk_press(int16_t dir);
void mfk_set_mode(mfk_mode_t mode);
