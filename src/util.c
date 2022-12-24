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
