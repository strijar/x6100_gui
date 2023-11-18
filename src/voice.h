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

void voice_say_text_fmt(const char * fmt, ...);
void voice_say_freq(uint64_t freq);

#ifdef __cplusplus
}
#endif
