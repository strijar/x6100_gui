/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include "lvgl/lvgl.h"
#include "bands.h"
#include "params.h"
#include "radio.h"
#include "info.h"
#include "spectrum.h"
#include "waterfall.h"
#include "main_screen.h"
#include "pannel.h"

#define BANDS_MAX   32

static band_t   *bands[BANDS_MAX];

void bands_init() {
    for (uint8_t i = 0; i < BANDS_MAX; i++)
        bands[i] = NULL;
}

void bands_clear() {
    for (uint8_t i = 0; i < BANDS_MAX; i++) {
        if (bands[i] != NULL) {
            if (bands[i]->name != NULL) {
                free(bands[i]->name);
            }
            free(bands[i]);
            bands[i] = NULL;
        }
    }
}

void bands_insert(uint8_t id, const char *name, uint64_t start_freq, uint64_t stop_freq, uint8_t type) {
    band_t *band = malloc(sizeof(band_t));
    
    band->id = id;
    band->name = strdup(name);
    band->start_freq = start_freq;
    band->stop_freq = stop_freq;
    band->type = type;
    
    for (uint8_t i = 0; i < BANDS_MAX; i++)
        if (bands[i] == NULL) {
            bands[i] = band;
            return;
        }

   LV_LOG_ERROR("Too many bands");
   free(band);
}

void bands_activate(band_t *band, uint64_t *freq) {
    params_lock();
    params_band_save();
    params.band = band->id;
    params_band_load();
    params_unlock(&params.durty.band);

    if (freq) {
        params_band_freq_set(*freq);
    }

    radio_vfo_set();
    radio_mode_set();
    spectrum_mode_set();
    spectrum_band_set();
    waterfall_band_set();
}

void bands_change(bool up) {
    int8_t index = 0;

    for (uint8_t i = 0; i < BANDS_MAX; i++)
        if (bands[i] == NULL) {
            LV_LOG_ERROR("Band not found");
            return;
        } else if (bands[i]->id == params.band) {
            index = i;
            break;
        }

    while (true) {
        if (up) {
            index++;
            
            if (index >= BANDS_MAX) {
                index = 0;
            }
        } else {
            index--;
            
            if (index < 0) {
                index = BANDS_MAX - 1;
            }
        }
        
        if (bands[index] != NULL && bands[index]->type != 0) {
            band_t *band = bands[index];

            bands_activate(band, NULL);
            radio_load_atu();
            info_params_set();
            pannel_visible();

            waterfall_clear();
            spectrum_clear();
            main_screen_band_set();

            return;
        }
    }
}

bands_t * bands_find_all(uint64_t freq, int32_t half_width) {
    uint64_t    left = freq - half_width;
    uint64_t    right = freq + half_width;
    bands_t     *res = NULL;

    for (uint8_t i = 0; i < BANDS_MAX; i++) {
        band_t *band = bands[i];
        
        if (band == NULL)
            return res;
        
        if ((band->stop_freq >= left && band->stop_freq <= right) ||
            (band->start_freq >= left && band->start_freq <= right) ||
            (band->start_freq <= left && band->stop_freq >= right))
        {
            bands_t *item = malloc(sizeof(bands_t));
            
            item->item = band;
            item->next = res;
            res = item;
        }
    }
    
    return res;
}

band_t * bands_find(uint64_t freq) {
    for (uint8_t i = 0; i < BANDS_MAX; i++) {
        band_t *band = bands[i];
        
        if (band == NULL)
            break;
            
        if (freq >= band->start_freq && freq <= band->stop_freq)
            return band;
    }
    
    return NULL;
}
