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
#include "radio.h"
#include "clock.h"

typedef struct {
    int32_t         filter_low;
    int32_t         filter_high;

    uint16_t        freq_step;
    int16_t         spectrum_factor;

    /* durty flags */
    
    struct {
        bool    filter_low;
        bool    filter_high;

        bool    freq_step;
        bool    spectrum_factor;
    } durty;
} params_mode_t;

typedef struct {
    uint64_t        freq;
    x6100_att_t     att;
    x6100_pre_t     pre;
    x6100_mode_t    mode;
    x6100_agc_t     agc;
    
    struct {
        bool    freq;
        bool    att;
        bool    pre;
        bool    mode;
        bool    agc;
    } durty;
} params_vfo_t;

typedef struct {
    x6100_vfo_t     vfo;

    params_vfo_t    vfo_x[2];

    bool            split;
    int16_t         grid_min;
    int16_t         grid_max;

    /* durty flags */
    
    struct {
        bool    vfo;
        bool    split;
        bool    grid_min;
        bool    grid_max;
    } durty;
} params_band_t;

typedef enum {
    BUTTONS_DARK = 0,
    BUTTONS_LIGHT,
    BUTTONS_TEMPORARILY
} buttons_light_t;

typedef enum {
    LONG_ACTION_NONE = 0,
    LONG_ACTION_SCREENSHOT,
    LONG_ACTION_RECORDER,

    LONG_ACTION_APP_RTTY = 100,
    LONG_ACTION_APP_FT8,
    LONG_ACTION_APP_SWRSCAN,
    LONG_ACTION_APP_GPS,
    LONG_ACTION_APP_SETTINGS,
    LONG_ACTION_APP_RECORDER
} longpress_action_t;

typedef struct {
    uint64_t            vol_modes;
    uint64_t            mfk_modes;
    
    /* LCD */
    
    int16_t             brightness_normal;
    int16_t             brightness_idle;
    uint16_t            brightness_timeout; /* seconds */
    buttons_light_t     brightness_buttons;

    /* band info */
    
    band_t              *freq_band;

    /* radio */
    
    uint8_t             band;
    int16_t             vol;
    int16_t             rfg;
    uint8_t             sql;
    bool                atu;
    bool                atu_loaded;
    uint8_t             ant;
    float               pwr;
    x6100_mic_sel_t     mic;
    uint8_t             hmic;
    uint8_t             imic;
    radio_charger_t     charger;
    uint16_t            bias_drive;
    uint16_t            bias_final;
    int16_t             rit;
    int16_t             xit;
    uint8_t             line_in;
    uint8_t             line_out;
    int16_t             moni;
    
    /* DSP */
    
    bool                dnf;
    uint16_t            dnf_center;
    uint16_t            dnf_width;
    
    bool                nb;
    uint8_t             nb_level;
    uint8_t             nb_width;
    
    bool                nr;
    uint8_t             nr_level;
    
    /* AGC */
    
    bool                agc_hang;
    int8_t              agc_knee;
    uint8_t             agc_slope;
    
    /* VOX */
    
    bool                vox;
    uint8_t             vox_ag;
    uint16_t            vox_delay;
    uint8_t             vox_gain;
    
    /* main screen */
    
    int16_t             spectrum_beta;
    bool                spectrum_peak;
    uint16_t            spectrum_peak_hold;
    float               spectrum_peak_speed;
    bool                spectrum_filled;
    bool                mag_freq;
    bool                mag_info;
    bool                mag_alc;
    clock_view_t        clock_view;
    uint8_t             clock_time_timeout;     /* seconds */
    uint8_t             clock_power_timeout;    /* seconds */
    uint8_t             clock_tx_timeout;       /* seconds */
    
    /* key */
    
    uint8_t             key_speed;
    x6100_key_mode_t    key_mode;
    x6100_iambic_mode_t iambic_mode;
    uint16_t            key_tone;
    uint16_t            key_vol;
    bool                key_train;
    uint16_t            qsk_time;
    uint8_t             key_ratio;

    /* CW decoder */

    bool                cw_decoder;
    float               cw_decoder_snr;
    float               cw_decoder_snr_gist;
    float               cw_decoder_peak_beta;
    float               cw_decoder_noise_beta;

    /* Msg */
    
    uint16_t            cw_encoder_period;  /* seconds */
    uint16_t            voice_msg_period;   /* seconds */

    /* RTTY */
    
    uint16_t            rtty_center;
    uint16_t            rtty_shift;
    uint32_t            rtty_rate;
    bool                rtty_reverse;
    uint8_t             rtty_bits;
    float               rtty_snr;

    /* SWR Scan */

    bool                swrscan_linear;
    uint32_t            swrscan_span;
    
    /* Long press actions */
    
    uint8_t             long_gen;
    uint8_t             long_app;
    uint8_t             long_key;
    uint8_t             long_msg;
    uint8_t             long_dfn;
    uint8_t             long_dfl;
    
    /* durty flags */
    
    struct {
        bool    vol_modes;
        bool    mfk_modes;

        bool    brightness_normal;
        bool    brightness_idle;
        bool    brightness_timeout;
        bool    brightness_buttons;
        
        bool    band;
        bool    vol;
        bool    rfg;
        bool    sql;
        bool    atu;
        bool    ant;
        bool    pwr;
        bool    mic;
        bool    hmic;
        bool    imic;
        bool    charger;
        bool    rit;
        bool    xit;
        bool    line_in;
        bool    line_out;
        bool    moni;
        
        bool    dnf;
        bool    dnf_center;
        bool    dnf_width;
        bool    nb;
        bool    nb_level;
        bool    nb_width;
        bool    nr;
        bool    nr_level;

        bool    agc_hang;
        bool    agc_knee;
        bool    agc_slope;

        bool    vox;
        bool    vox_ag;
        bool    vox_delay;
        bool    vox_gain;
        
        bool    spectrum_beta;
        bool    spectrum_peak;
        bool    spectrum_peak_hold;
        bool    spectrum_peak_speed;
        bool    spectrum_filled;
        bool    mag_freq;
        bool    mag_info;
        bool    mag_alc;
        bool    clock_view;
        bool    clock_time_timeout;
        bool    clock_power_timeout;
        bool    clock_tx_timeout;
        
        bool    key_speed;
        bool    key_mode;
        bool    iambic_mode;
        bool    key_tone;
        bool    key_vol;
        bool    key_train;
        bool    qsk_time;
        bool    key_ratio;

        bool    cw_decoder;
        bool    cw_decoder_snr;
        bool    cw_decoder_peak_beta;
        bool    cw_decoder_noise_beta;
        
        bool    cw_encoder_period;
        bool    voice_msg_period;

        bool    rtty_center;
        bool    rtty_shift;
        bool    rtty_rate;
        bool    rtty_reverse;

        bool    swrscan_linear;
        bool    swrscan_span;

        bool    long_gen;
        bool    long_app;
        bool    long_key;
        bool    long_msg;
        bool    long_dfn;
        bool    long_dfl;
    } durty;
} params_t;

extern params_t params;
extern params_band_t params_band;
extern params_mode_t params_mode;

void params_init();
void params_lock();
void params_unlock(bool *durty);

void params_band_save();
void params_band_load();

void params_memory_save(uint16_t id);
void params_memory_load(uint16_t id);

void params_mode_save();
void params_mode_load();

void params_band_freq_set(uint64_t freq);

void params_atu_save(uint32_t val);
uint32_t params_atu_load(bool *loaded);

void params_band_vfo_clone();

void params_msg_cw_load();
void params_msg_cw_new(const char *val);
void params_msg_cw_edit(uint32_t id, const char *val);
void params_msg_cw_delete(uint32_t id);
