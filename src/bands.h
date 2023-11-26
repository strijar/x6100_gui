/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    uint16_t    id;
    char        *name;
    uint64_t    start_freq;
    uint64_t    stop_freq;
    uint8_t     type;
} band_t;

void bands_activate(band_t *band, uint64_t *freq);
void bands_change(bool up);
