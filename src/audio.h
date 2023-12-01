/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#pragma once

#include <stdbool.h>

#define AUDIO_PLAY_RATE     (44100)
#define AUDIO_CAPTURE_RATE  (44100)

void audio_init();
int audio_play(int16_t *buf, size_t samples);
void audio_play_wait();
void audio_play_en(bool on);

int16_t* audio_gain(int16_t *buf, size_t samples, uint16_t gain);
