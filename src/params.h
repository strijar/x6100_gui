/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <aether_radio/x6100_control/control.h>
#include "bands.h"

typedef struct {
    int32_t         filter_low;
    int32_t         filter_high;

    uint16_t        freq_step;

    /* durty flags */
    
    struct {
        bool    filter_low;
        bool    filter_high;

        bool    freq_step;
    } durty;
} params_mode_t;

typedef struct {
    uint8_t         vfo;
    
    uint64_t        vfoa_freq;
    x6100_att_t     vfoa_att;
    x6100_pre_t     vfoa_pre;
    x6100_mode_t    vfoa_mode;
    x6100_agc_t     vfoa_agc;

    uint64_t        vfob_freq;
    x6100_att_t     vfob_att;
    x6100_pre_t     vfob_pre;
    x6100_mode_t    vfob_mode;
    x6100_agc_t     vfob_agc;

    int16_t         grid_min;
    int16_t         grid_max;

    /* durty flags */
    
    struct {
        bool    vfo;
    
        bool    vfoa_freq;
        bool    vfoa_att;
        bool    vfoa_pre;
        bool    vfoa_mode;
        bool    vfoa_agc;

        bool    vfob_freq;
        bool    vfob_att;
        bool    vfob_pre;
        bool    vfob_mode;
        bool    vfob_agc;

        bool    grid_min;
        bool    grid_max;
    } durty;
} params_band_t;

typedef struct {
    /* band info */
    
    band_t              *freq_band;

    /* radio */
    
    uint8_t             band;
    int16_t             vol;
    int16_t             rfg;
    bool                atu;
    uint8_t             ant;
    float               pwr;
    
    /* main screen */
    
    int16_t             spectrum_factor;
    int16_t             spectrum_beta;
    
    /* key */
    
    uint8_t             key_speed;
    x6100_key_mode_t    key_mode;
    x6100_iambic_mode_t iambic_mode;
    uint16_t            key_tone;
    uint16_t            key_vol;
    bool                key_train;
    uint16_t            qsk_time;
    uint8_t             key_ratio;
    
    /* durty flags */
    
    struct {
        bool    band;
        bool    vol;
        bool    rfg;
        bool    atu;
        bool    ant;
        bool    pwr;
        
        bool    spectrum_factor;
        bool    spectrum_beta;
        bool    freq_step;

        bool    key_speed;
        bool    key_mode;
        bool    iambic_mode;
        bool    key_tone;
        bool    key_vol;
        bool    key_train;
        bool    qsk_time;
        bool    key_ratio;
    } durty;
} params_t;

extern params_t params;
extern params_band_t params_band;
extern params_mode_t params_mode;

void params_init();
void params_lock();
void params_unlock(bool *durty);

bool params_band_save();
void params_band_load();

bool params_mode_save();
void params_mode_load();

void params_band_freq_set(uint64_t freq);

void params_atu_save(uint32_t val);
uint32_t params_atu_load();
