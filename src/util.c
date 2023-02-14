/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <stdlib.h>
#include <sys/time.h>
#include "util.h"

uint64_t get_time() {
    struct timeval now;
    
    gettimeofday(&now, NULL);

    uint64_t usec = (uint64_t) now.tv_sec * 1000000L + now.tv_usec;

    return usec / 1000;
}

void split_freq(uint64_t freq, uint16_t *mhz, uint16_t *khz, uint16_t *hz) {
    *mhz = freq / 1000000;
    *khz = (freq / 1000) % 1000;
    *hz = freq % 1000;
}

int32_t align_int(int32_t x, uint16_t step) {
    if (step == 0) {
        return x;
    }
    
    return x - (x % step);
}

uint64_t align_long(uint64_t x, uint16_t step) {
    if (step == 0) {
        return x;
    }

    return x - (x % step);
}

int32_t limit(int32_t x, int32_t min, int32_t max) {
    if (x < min) {
        return min;
    } else if (x > max) {
        return max;
    }
    
    return x;
}

float sqr(float x) {
    return x * x;
}

void lpf(float *x, float current, float beta) {
    *x = *x * beta + current * (1.0f - beta);
}

void to_bcd(uint8_t bcd_data[], uint64_t data, uint8_t len) {
    int16_t i;
    
    for (i = 0; i < len / 2; i++) {
        uint8_t a = data % 10;
        
        data /= 10;
        a |= (data % 10) << 4;
        data /= 10;
        bcd_data[i] = a;
    }

    if (len & 1) {
        bcd_data[i] &= 0x0f;
        bcd_data[i] |= data % 10;
    }
}

uint64_t from_bcd(const uint8_t bcd_data[], uint8_t len) {
    int16_t     i;
    uint64_t    data = 0;
    
    if (len & 1) {
        data = bcd_data[len / 2] & 0x0F;
    }
    
    for (i = (len / 2) - 1; i >= 0; i--) {
        data *= 10;
        data += bcd_data[i] >> 4;
        data *= 10;
        data += bcd_data[i] & 0x0F;
    }
    
    return data;
}
