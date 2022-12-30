/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

void bands_init();
void bands_clear();
void bands_insert(uint8_t id, const char *name, uint64_t start_freq, uint64_t stop_freq, uint8_t used);

void bands_change(bool up);
