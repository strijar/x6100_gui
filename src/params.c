/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022 Belousov Oleg aka R1CBU
 */

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <sqlite3.h>

#include "lvgl/lvgl.h"
#include "params.h"
#include "util.h"
#include "bands.h"

#define PARAMS_SAVE_TIMEOUT  (3 * 1000)

params_t params = {
    .spectrum_factor    = 1,
    .spectrum_beta      = 70,
    .freq_step          = 500,
    
    .vol                = 20,
    .rfg                = 63
};

params_band_t params_band = {
    .vfo                = X6100_VFO_A,

    .vfoa_freq          = 14000000,
    .vfoa_att           = x6100_att_off,
    .vfoa_pre           = x6100_pre_off,
    .vfoa_mode          = x6100_mode_usb,
    .vfoa_agc           = x6100_agc_fast,

    .vfob_freq          = 14100000,
    .vfob_att           = x6100_att_off,
    .vfob_pre           = x6100_pre_off,
    .vfob_mode          = x6100_mode_usb,
    .vfob_agc           = x6100_agc_fast,

    .grid_min           = -70,
    .grid_max           = -40,
};

static pthread_mutex_t  params_mux;
static uint64_t         durty_time;
static sqlite3          *db = NULL;
static sqlite3_stmt     *write_stmt;
static sqlite3_stmt     *write_band_stmt;

static bool params_exec(const char *sql);

/* Bands params */

void params_band_load() {
    sqlite3_stmt    *stmt;
    int             rc;

    rc = sqlite3_prepare_v2(db, "SELECT name,val FROM band_params WHERE bands_id = ?", -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        LV_LOG_ERROR("Prepare");
        return;
    }

    sqlite3_bind_int(stmt, 1, params.band);

    while (sqlite3_step(stmt) != SQLITE_DONE) {
        const char *name = sqlite3_column_text(stmt, 0);

        if (strcmp(name, "vfo") == 0) {
            params_band.vfo = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "vfoa_freq") == 0) {
            params_band.vfoa_freq = sqlite3_column_int64(stmt, 1);
        } else if (strcmp(name, "vfoa_att") == 0) {
            params_band.vfoa_att = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "vfoa_pre") == 0) {
            params_band.vfoa_pre = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "vfoa_mode") == 0) {
            params_band.vfoa_mode = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "vfoa_agc") == 0) {
            params_band.vfoa_agc = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "vfob_freq") == 0) {
            params_band.vfob_freq = sqlite3_column_int64(stmt, 1);
        } else if (strcmp(name, "vfob_att") == 0) {
            params_band.vfob_att = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "vfob_pre") == 0) {
            params_band.vfob_pre = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "vfob_mode") == 0) {
            params_band.vfob_mode = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "vfob_agc") == 0) {
            params_band.vfob_agc = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "grid_min") == 0) {
            params_band.grid_min = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "grid_max") == 0) {
            params_band.grid_max = sqlite3_column_int(stmt, 1);
        }
    }

    sqlite3_finalize(stmt);
}

static void params_band_write_int(const char *name, int data, bool *durty) {
    sqlite3_bind_int(write_band_stmt, 1, params.band);
    sqlite3_bind_text(write_band_stmt, 2, name, strlen(name), 0);
    sqlite3_bind_int(write_band_stmt, 3, data);
    sqlite3_step(write_band_stmt);
    sqlite3_reset(write_band_stmt);
    sqlite3_clear_bindings(write_band_stmt);
    
    *durty = false;
}

static void params_band_write_int64(const char *name, uint64_t data, bool *durty) {
    sqlite3_bind_int(write_band_stmt, 1, params.band);
    sqlite3_bind_text(write_band_stmt, 2, name, strlen(name), 0);
    sqlite3_bind_int64(write_band_stmt, 3, data);
    sqlite3_step(write_band_stmt);
    sqlite3_reset(write_band_stmt);
    sqlite3_clear_bindings(write_band_stmt);
    
    *durty = false;
}

bool params_band_save() {
    if (!params_exec("BEGIN")) {
        return false;
    }

    if (params_band.durty.vfo)          params_band_write_int("vfo", params_band.vfo, &params_band.durty.vfo);
    if (params_band.durty.vfoa_freq)    params_band_write_int64("vfoa_freq", params_band.vfoa_freq, &params_band.durty.vfoa_freq);
    if (params_band.durty.vfoa_att)     params_band_write_int("vfoa_att", params_band.vfoa_att, &params_band.durty.vfoa_att);
    if (params_band.durty.vfoa_pre)     params_band_write_int("vfoa_pre", params_band.vfoa_pre, &params_band.durty.vfoa_pre);
    if (params_band.durty.vfoa_mode)    params_band_write_int("vfoa_mode", params_band.vfoa_mode, &params_band.durty.vfoa_mode);
    if (params_band.durty.vfoa_agc)     params_band_write_int("vfoa_agc", params_band.vfoa_agc, &params_band.durty.vfoa_agc);

    if (params_band.durty.vfob_freq)    params_band_write_int64("vfob_freq", params_band.vfob_freq, &params_band.durty.vfob_freq);
    if (params_band.durty.vfob_att)     params_band_write_int("vfob_att", params_band.vfob_att, &params_band.durty.vfob_att);
    if (params_band.durty.vfob_pre)     params_band_write_int("vfob_pre", params_band.vfob_pre, &params_band.durty.vfob_pre);
    if (params_band.durty.vfob_mode)    params_band_write_int("vfob_mode", params_band.vfob_mode, &params_band.durty.vfob_mode);
    if (params_band.durty.vfob_agc)     params_band_write_int("vfob_agc", params_band.vfob_agc, &params_band.durty.vfob_agc);

    if (params_band.durty.grid_min)     params_band_write_int("grid_min", params_band.grid_min, &params_band.durty.grid_min);
    if (params_band.durty.grid_max)     params_band_write_int("grid_max", params_band.grid_max, &params_band.durty.grid_max);

    if (!params_exec("COMMIT")) {
        return false;
    }
    return true;
}

/* System params */

static bool params_load() {
    sqlite3_stmt    *stmt;
    int             rc;
    
    rc = sqlite3_prepare_v2(db, "SELECT * FROM params", -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        return false;
    }
    
    while (sqlite3_step(stmt) != SQLITE_DONE) {
        const char *name = sqlite3_column_text(stmt, 0);

        if (strcmp(name, "band") == 0) {
            params.band = sqlite3_column_int(stmt, 1);
            params_band_load();
        } else if (strcmp(name, "vol") == 0) {
            params.vol = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "rfg") == 0) {
            params.rfg = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "spectrum_factor") == 0) {
            params.spectrum_factor = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "spectrum_beta") == 0) {
            params.spectrum_beta = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "freq_step") == 0) {
            params.freq_step = sqlite3_column_int(stmt, 1);
        }
    }
    
    sqlite3_finalize(stmt);
    return true;
}

static bool params_exec(const char *sql) {
    char    *err = 0;
    int     rc;
    
    rc = sqlite3_exec(db, sql, NULL, NULL, &err);
    
    if (rc != SQLITE_OK) {
        LV_LOG_ERROR(err);
        return false;
    }
    
    return true;
}

static void params_write_int(const char *name, int data, bool *durty) {
    sqlite3_bind_text(write_stmt, 1, name, strlen(name), 0);
    sqlite3_bind_int(write_stmt, 2, data);
    sqlite3_step(write_stmt);
    sqlite3_reset(write_stmt);
    sqlite3_clear_bindings(write_stmt);
    
    *durty = false;
}

static bool params_save() {
    if (!params_exec("BEGIN")) {
        return false;
    }

    if (params.durty.band)              params_write_int("band", params.band, &params.durty.band);
    if (params.durty.vol)               params_write_int("vol", params.vol, &params.durty.vol);
    if (params.durty.rfg)               params_write_int("rfg", params.rfg, &params.durty.rfg);
    if (params.durty.spectrum_factor)   params_write_int("spectrum_factor", params.spectrum_factor, &params.durty.spectrum_factor);
    if (params.durty.spectrum_beta)     params_write_int("spectrum_beta", params.spectrum_beta, &params.durty.spectrum_beta);
    if (params.durty.freq_step)         params_write_int("freq_step", params.freq_step, &params.durty.freq_step);

    if (!params_exec("COMMIT")) {
        return false;
    }
    
    return true;
}

/* * */

bool params_bands_load() {
    sqlite3_stmt    *stmt;
    int             rc;
    
    rc = sqlite3_prepare_v2(db, "SELECT id,name,start_freq,stop_freq,used FROM bands ORDER BY used ASC", -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        return false;
    }
    
    while (sqlite3_step(stmt) != SQLITE_DONE) {
        int         id = sqlite3_column_int(stmt, 0);
        const char  *name = sqlite3_column_text(stmt, 1);
        uint64_t    start_freq = sqlite3_column_int64(stmt, 2);
        uint64_t    stop_freq = sqlite3_column_int64(stmt, 3);
        uint8_t     used = sqlite3_column_int(stmt, 4);
        
        bands_insert(id, name, start_freq, stop_freq, used);
    }
    
    sqlite3_finalize(stmt);
    return true;
}

static void * params_thread(void *arg) {
    while (true) {
        pthread_mutex_lock(&params_mux);

        if (durty_time) {
            uint64_t    now = get_time();
            int32_t     d = now - durty_time;
    
            if (d > PARAMS_SAVE_TIMEOUT) {
                durty_time = 0;
                
                if (!params_save()) {
                    LV_LOG_ERROR("Params not saved");
                }

                if (!params_band_save()) {
                    LV_LOG_ERROR("Band params not saved");
                }
            }
        }
        
        pthread_mutex_unlock(&params_mux);
        usleep(100000);
    }
}

void params_init() {
    int rc = sqlite3_open("params.db", &db);
    
    if (rc == SQLITE_OK) {
        if (!params_load()) {
            LV_LOG_ERROR("Load params");
            sqlite3_close(db);
            db = NULL;
        }

        rc = sqlite3_prepare_v2(db, "INSERT INTO params(name, val) VALUES(?, ?)", -1, &write_stmt, 0);

        if (rc != SQLITE_OK) {
            LV_LOG_ERROR("Prepare write");
        }

        rc = sqlite3_prepare_v2(db, "INSERT INTO band_params(bands_id, name, val) VALUES(?, ?, ?)", -1, &write_band_stmt, 0);

        if (rc != SQLITE_OK) {
            LV_LOG_ERROR("Prepare band write");
        }
        
        if (!params_bands_load()) {
            LV_LOG_ERROR("Load bands");
        }
    } else {
        LV_LOG_ERROR("Open params.db");
    }
    
    pthread_mutex_init(&params_mux, NULL);

    durty_time = 0;

    pthread_t thread;

    pthread_create(&thread, NULL, params_thread, NULL);
    pthread_detach(thread);
}

void params_lock() {
    pthread_mutex_lock(&params_mux);
}

void params_unlock(bool *durty) {
    if (durty != NULL) {
        *durty = true;
    }

    durty_time = get_time();
    pthread_mutex_unlock(&params_mux);
}
