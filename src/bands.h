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

typedef struct {
    band_t  *item;
    void    *next;
} bands_t;

void bands_init();
void bands_clear();
void bands_insert(uint16_t id, const char *name, uint64_t start_freq, uint64_t stop_freq, uint8_t used);

void bands_activate(band_t *band, uint64_t *freq);
void bands_change(bool up);
bands_t * bands_find_all(uint64_t freq, int32_t half_width);
band_t * bands_find(uint64_t freq);
