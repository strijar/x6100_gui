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

extern char *recorder_path;

void recorder_set_on(bool on);
bool recorder_is_on();
void recorder_put_audio_samples(size_t nsamples, int16_t *samples);
