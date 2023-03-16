/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#pragma once

#include <stdint.h>
#include "lvgl/lvgl.h"
#include "events.h"

typedef struct {
    int             fd;
    bool            pressed;
    
    lv_indev_drv_t  indev_drv;
    lv_indev_t      *indev;
} encoder_t;

encoder_t * encoder_init(char *dev_name);
