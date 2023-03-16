/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#pragma once

#include "rotary.h"

#define VERSION "v0.13.0"

typedef enum {
    VOL_EDIT = 0,
    VOL_SELECT,
} vol_rotary_t;

typedef enum {
    MFK_EDIT = 0,
    MFK_SELECT,
    MFK_NAVIGATE
} mfk_rotary_t;

extern rotary_t *vol;
extern rotary_t *mfk;
