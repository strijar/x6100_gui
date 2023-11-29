/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#include <cstdarg>
#endif

typedef enum {
    VOICE_OFF = 0,
    VOICE_LCD,
    VOICE_ALWAYS
} voice_mode_t;

void voice_say_text_fmt(const char * fmt, ...);
void voice_delay_say_text_fmt(const char * fmt, ...);
void voice_say_freq(uint64_t freq);

void voice_say_bool(const char *prompt, bool x);

#ifdef __cplusplus
}
#endif