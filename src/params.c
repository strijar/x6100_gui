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

#define PARAMS_SAVE_TIMEOUT  (3 * 1000)

params_t params = {
    .grid_min           = -70,
    .grid_max           = -40,
    .spectrum_factor    = 1,
    .spectrum_beta      = 70,
    .freq_step          = 500,
    
    .vol                = 20,
    .rfg                = 63
};

static pthread_mutex_t  params_mux;
static uint64_t         durty_time;
static sqlite3          *db = NULL;
static sqlite3_stmt     *write_stmt;

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
        } else if (strcmp(name, "vol") == 0) {
            params.vol = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "rfg") == 0) {
            params.rfg = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "grid_min") == 0) {
            params.grid_min = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "grid_max") == 0) {
            params.grid_max = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "spectrum_factor") == 0) {
            params.spectrum_factor = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "spectrum_beta") == 0) {
            params.spectrum_beta = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "freq_step") == 0) {
            params.freq_step = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "bands") == 0) {
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
    if (params.durty.grid_min)          params_write_int("grid_min", params.grid_min, &params.durty.grid_min);
    if (params.durty.grid_max)          params_write_int("grid_max", params.grid_max, &params.durty.grid_max);
    if (params.durty.spectrum_factor)   params_write_int("spectrum_factor", params.spectrum_factor, &params.durty.spectrum_factor);
    if (params.durty.spectrum_beta)     params_write_int("spectrum_beta", params.spectrum_beta, &params.durty.spectrum_beta);
    if (params.durty.freq_step)         params_write_int("freq_step", params.freq_step, &params.durty.freq_step);

    if (!params_exec("COMMIT")) {
        return false;
    }
    
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
                
                if (params_save()) {
                    LV_LOG_INFO("Params saved");
                } else {
                    LV_LOG_ERROR("Params not saved");
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
    *durty = true;
    durty_time = get_time();
    pthread_mutex_unlock(&params_mux);
}
