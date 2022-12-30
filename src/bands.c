/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#include "lvgl/lvgl.h"
#include "bands.h"
#include "params.h"
#include "radio.h"

#define BANDS_MAX   32

typedef struct {
    uint8_t     id;
    char        *name;
    uint64_t    start_freq;
    uint64_t    stop_freq;
    uint8_t     used;
} band_t;

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

void bands_insert(uint8_t id, const char *name, uint64_t start_freq, uint64_t stop_freq, uint8_t used) {
    band_t *band = malloc(sizeof(band_t));
    
    band->id = id;
    band->name = strdup(name);
    band->start_freq = start_freq;
    band->stop_freq = stop_freq;
    band->used = used;
    
    for (uint8_t i = 0; i < BANDS_MAX; i++)
        if (bands[i] == NULL) {
            bands[i] = band;
            return;
        }

   LV_LOG_ERROR("Too many bands");
   free(band);
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
        
        if (bands[index] != NULL && bands[index]->used != 0) {
            params_lock();
            params_band_save();
            params.band = bands[index]->id;
            params_band_load();
            params_unlock(NULL);
            
            radio_band_set();

            return;
        }
    }
}
