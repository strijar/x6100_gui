/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    /* radio */
    
    uint8_t     band;
    int16_t     vol;
    int16_t     rfg;
    int16_t     pre;
    
    /* main screen */
    
    int16_t     grid_min;
    int16_t     grid_max;
    int16_t     spectrum_factor;
    int16_t     spectrum_beta;
    uint16_t    freq_step;
    
    /* durty flags */
    
    struct {
        bool    band;
        bool    vol;
        bool    rfg;
        bool    pre;
        
        bool    grid_min;
        bool    grid_max;
        bool    spectrum_factor;
        bool    spectrum_beta;
        bool    freq_step;
    } durty;
} params_t;

extern params_t params;

void params_init();
void params_lock();
void params_unlock(bool *durty);
