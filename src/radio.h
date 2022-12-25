/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#pragma once

#include <stdbool.h>

#define RADIO_THREAD

void radio_init();
bool radio_tick();

void radio_set_freq(uint64_t f);
void radio_change_freq(int32_t df);
