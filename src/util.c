/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#include <stdlib.h>
#include <sys/time.h>
#include "util.h"

uint64_t get_time() {
    struct timeval now;
    
    gettimeofday(&now, NULL);

    return (now.tv_sec * 1000000 + now.tv_usec) / 1000;
}

void split_freq(uint64_t freq, uint16_t *mhz, uint16_t *khz, uint16_t *hz) {
    *mhz = freq / 1000000;
    *khz = (freq / 1000) % 1000;
    *hz = freq % 1000;
}

int32_t align_int(int32_t x, uint16_t step) {
    return x - (x % step);
}
