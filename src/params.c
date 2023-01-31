/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
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
    .spectrum_factor        = 1,
    .spectrum_beta          = 70,
    .spectrum_filled        = true,
    .spectrum_peak          = true,
    .spectrum_peak_hold     = 5000,
    .spectrum_peak_speed    = 0.5f,
    
    .vol                    = 20,
    .rfg                    = 63,
    .ant                    = 1,
    .pwr                    = 5.0f,
    .mic                    = x6100_mic_auto,
    .hmic                   = 20,
    .imic                   = 30,
    .charger                = true,
    .bias_drive             = 450,
    .bias_final             = 650,

    .dnf                    = false,
    .dnf_center             = 1000,
    .dnf_width              = 50,
    .nb                     = false,
    .nb_level               = 10,
    .nb_width               = 10,
    .nr                     = false,
    .nr_level               = 0,

    .agc_hang               = false,
    .agc_knee               = -60,
    .agc_slope              = 6,

    .key_speed              = 15,
    .key_mode               = x6100_key_manual,
    .iambic_mode            = x6100_iambic_a,
    .key_tone               = 700,
    .key_vol                = 10,
    .key_train              = false,
    .qsk_time               = 100,
    .key_ratio              = 30
};

params_band_t params_band = {
    .vfo                = X6100_VFO_A,

    .vfo_x[X6100_VFO_A] = {
        .freq           = 14000000,
        .att            = x6100_att_off,
        .pre            = x6100_pre_off,
        .mode           = x6100_mode_usb,
        .agc            = x6100_agc_fast
    },

    .vfo_x[X6100_VFO_B] = {
        .freq           = 14100000,
        .att            = x6100_att_off,
        .pre            = x6100_pre_off,
        .mode           = x6100_mode_usb,
        .agc            = x6100_agc_fast
    },

    .split              = false,
    .grid_min           = -121,
    .grid_max           = -73,
};

params_mode_t params_mode = {
    .filter_low         = 50,
    .filter_high        = 2950,

    .freq_step          = 500
};

static pthread_mutex_t  params_mux;
static uint64_t         durty_time;
static sqlite3          *db = NULL;
static sqlite3_stmt     *write_stmt;
static sqlite3_stmt     *write_band_stmt;
static sqlite3_stmt     *write_mode_stmt;
static sqlite3_stmt     *save_atu_stmt;
static sqlite3_stmt     *load_atu_stmt;

static bool params_exec(const char *sql);

/* Mode params */

void params_mode_load() {
    sqlite3_stmt    *stmt;
    int             rc;

    rc = sqlite3_prepare_v2(db, "SELECT name,val FROM mode_params WHERE mode = ?", -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        LV_LOG_ERROR("Prepare");
        return;
    }

    x6100_mode_t    mode = params_band.vfo_x[params_band.vfo].mode;

    sqlite3_bind_int(stmt, 1, mode);

    while (sqlite3_step(stmt) != SQLITE_DONE) {
        const char *name = sqlite3_column_text(stmt, 0);

        if (strcmp(name, "filter_low") == 0) {
            params_mode.filter_low = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "filter_high") == 0) {
            params_mode.filter_high = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "freq_step") == 0) {
            params_mode.freq_step = sqlite3_column_int(stmt, 1);
        }
    }

    sqlite3_finalize(stmt);
}

static void params_mode_write_int(const char *name, int data, bool *durty) {
    x6100_mode_t    mode = params_band.vfo_x[params_band.vfo].mode;

    sqlite3_bind_int(write_mode_stmt, 1, mode);
    sqlite3_bind_text(write_mode_stmt, 2, name, strlen(name), 0);
    sqlite3_bind_int(write_mode_stmt, 3, data);
    sqlite3_step(write_mode_stmt);
    sqlite3_reset(write_mode_stmt);
    sqlite3_clear_bindings(write_mode_stmt);
    
    *durty = false;
}

bool params_mode_save() {
    if (!params_exec("BEGIN")) {
        return false;
    }

    if (params_mode.durty.filter_low)   params_mode_write_int("filter_low", params_mode.filter_low, &params_mode.durty.filter_low);
    if (params_mode.durty.filter_high)  params_mode_write_int("filter_high", params_mode.filter_high, &params_mode.durty.filter_high);
    if (params_mode.durty.freq_step)    params_mode_write_int("freq_step", params_mode.freq_step, &params_mode.durty.freq_step);

    if (!params_exec("COMMIT")) {
        return false;
    }

    return true;
}

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

    bool copy_freq = true;
    bool copy_att = true;
    bool copy_pre = true;
    bool copy_mode = true;
    bool copy_agc = true;

    while (sqlite3_step(stmt) != SQLITE_DONE) {
        const char *name = sqlite3_column_text(stmt, 0);

        if (strcmp(name, "vfo") == 0) {
            params_band.vfo = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "vfoa_freq") == 0) {
            params_band.vfo_x[X6100_VFO_A].freq = sqlite3_column_int64(stmt, 1);
        } else if (strcmp(name, "vfoa_att") == 0) {
            params_band.vfo_x[X6100_VFO_A].att = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "vfoa_pre") == 0) {
            params_band.vfo_x[X6100_VFO_A].pre = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "vfoa_mode") == 0) {
            params_band.vfo_x[X6100_VFO_A].mode = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "vfoa_agc") == 0) {
            params_band.vfo_x[X6100_VFO_A].agc = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "vfob_freq") == 0) {
            params_band.vfo_x[X6100_VFO_B].freq = sqlite3_column_int64(stmt, 1);
            copy_freq = false;
        } else if (strcmp(name, "vfob_att") == 0) {
            params_band.vfo_x[X6100_VFO_B].att = sqlite3_column_int(stmt, 1);
            copy_att = false;
        } else if (strcmp(name, "vfob_pre") == 0) {
            params_band.vfo_x[X6100_VFO_B].pre = sqlite3_column_int(stmt, 1);
            copy_pre = false;
        } else if (strcmp(name, "vfob_mode") == 0) {
            params_band.vfo_x[X6100_VFO_B].mode = sqlite3_column_int(stmt, 1);
            copy_mode = false;
        } else if (strcmp(name, "vfob_agc") == 0) {
            params_band.vfo_x[X6100_VFO_B].agc = sqlite3_column_int(stmt, 1);
            copy_agc = false;
        } else if (strcmp(name, "grid_min") == 0) {
            params_band.grid_min = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "grid_max") == 0) {
            params_band.grid_max = sqlite3_column_int(stmt, 1);
        }
    }

    if (copy_freq)  params_band.vfo_x[X6100_VFO_B].freq = params_band.vfo_x[X6100_VFO_A].freq;
    if (copy_att)   params_band.vfo_x[X6100_VFO_B].att = params_band.vfo_x[X6100_VFO_A].att;
    if (copy_pre)   params_band.vfo_x[X6100_VFO_B].pre = params_band.vfo_x[X6100_VFO_A].pre;
    if (copy_mode)  params_band.vfo_x[X6100_VFO_B].mode = params_band.vfo_x[X6100_VFO_A].mode;
    if (copy_agc)   params_band.vfo_x[X6100_VFO_B].agc = params_band.vfo_x[X6100_VFO_A].agc;

    sqlite3_finalize(stmt);
    params_mode_load();
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
    if (!params.freq_band) {
        return true;
    }

    if (!params_exec("BEGIN")) {
        return false;
    }

    if (params_band.durty.vfo)
        params_band_write_int("vfo", params_band.vfo, &params_band.durty.vfo);
    
    if (params_band.vfo_x[X6100_VFO_A].durty.freq)
        params_band_write_int64("vfoa_freq", params_band.vfo_x[X6100_VFO_A].freq, &params_band.vfo_x[X6100_VFO_A].durty.freq);
        
    if (params_band.vfo_x[X6100_VFO_A].durty.att)
        params_band_write_int("vfoa_att", params_band.vfo_x[X6100_VFO_A].att, &params_band.vfo_x[X6100_VFO_A].durty.att);
        
    if (params_band.vfo_x[X6100_VFO_A].durty.pre)
        params_band_write_int("vfoa_pre", params_band.vfo_x[X6100_VFO_A].pre, &params_band.vfo_x[X6100_VFO_A].durty.pre);
        
    if (params_band.vfo_x[X6100_VFO_A].durty.mode)
        params_band_write_int("vfoa_mode", params_band.vfo_x[X6100_VFO_A].mode, &params_band.vfo_x[X6100_VFO_A].durty.mode);
        
    if (params_band.vfo_x[X6100_VFO_A].durty.agc)
        params_band_write_int("vfoa_agc", params_band.vfo_x[X6100_VFO_A].agc, &params_band.vfo_x[X6100_VFO_A].durty.agc);

    if (params_band.vfo_x[X6100_VFO_B].durty.freq)
        params_band_write_int64("vfob_freq", params_band.vfo_x[X6100_VFO_B].freq, &params_band.vfo_x[X6100_VFO_B].durty.freq);
        
    if (params_band.vfo_x[X6100_VFO_B].durty.att)
        params_band_write_int("vfob_att", params_band.vfo_x[X6100_VFO_B].att, &params_band.vfo_x[X6100_VFO_B].durty.att);
        
    if (params_band.vfo_x[X6100_VFO_B].durty.pre)
        params_band_write_int("vfob_pre", params_band.vfo_x[X6100_VFO_B].pre, &params_band.vfo_x[X6100_VFO_B].durty.pre);
        
    if (params_band.vfo_x[X6100_VFO_B].durty.mode)
        params_band_write_int("vfob_mode", params_band.vfo_x[X6100_VFO_B].mode, &params_band.vfo_x[X6100_VFO_B].durty.mode);
        
    if (params_band.vfo_x[X6100_VFO_B].durty.agc)
        params_band_write_int("vfob_agc", params_band.vfo_x[X6100_VFO_B].agc, &params_band.vfo_x[X6100_VFO_B].durty.agc);

    if (params_band.durty.grid_min)
        params_band_write_int("grid_min", params_band.grid_min, &params_band.durty.grid_min);
        
    if (params_band.durty.grid_max)
        params_band_write_int("grid_max", params_band.grid_max, &params_band.durty.grid_max);

    if (!params_exec("COMMIT")) {
        return false;
    }

    params_mode_save();

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
        } else if (strcmp(name, "atu") == 0) {
            params.atu = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "pwr") == 0) {
            params.pwr = sqlite3_column_int(stmt, 1) * 0.1f;
        } else if (strcmp(name, "spectrum_factor") == 0) {
            params.spectrum_factor = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "spectrum_beta") == 0) {
            params.spectrum_beta = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "spectrum_filled") == 0) {
            params.spectrum_filled = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "spectrum_peak") == 0) {
            params.spectrum_peak = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "spectrum_peak_hold") == 0) {
            params.spectrum_peak_hold = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "spectrum_peak_speed") == 0) {
            params.spectrum_peak_speed = sqlite3_column_int(stmt, 1) * 0.1f;
        } else if (strcmp(name, "key_speed") == 0) {
            params.key_speed = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "key_mode") == 0) {
            params.key_mode = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "iambic_mode") == 0) {
            params.iambic_mode = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "key_tone") == 0) {
            params.key_tone = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "key_vol") == 0) {
            params.key_vol = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "key_train") == 0) {
            params.key_train = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "qsk_time") == 0) {
            params.qsk_time = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "key_ratio") == 0) {
            params.key_ratio = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "mic") == 0) {
            params.mic = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "hmic") == 0) {
            params.hmic = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "imic") == 0) {
            params.imic = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "charger") == 0) {
            params.charger = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "dnf") == 0) {
            params.dnf = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "dnf_center") == 0) {
            params.dnf_center = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "dnf_width") == 0) {
            params.dnf_width = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "nb") == 0) {
            params.nb = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "nb_level") == 0) {
            params.nb_level = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "nb_width") == 0) {
            params.nb_width = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "nr") == 0) {
            params.nr = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "nr_level") == 0) {
            params.nr_level = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "agc_hang") == 0) {
            params.agc_hang = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "agc_knee") == 0) {
            params.agc_knee = sqlite3_column_int(stmt, 1);
        } else if (strcmp(name, "agc_slope") == 0) {
            params.agc_slope = sqlite3_column_int(stmt, 1);
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

    if (params.durty.band)                  params_write_int("band", params.band, &params.durty.band);
    if (params.durty.vol)                   params_write_int("vol", params.vol, &params.durty.vol);
    if (params.durty.rfg)                   params_write_int("rfg", params.rfg, &params.durty.rfg);
    if (params.durty.atu)                   params_write_int("atu", params.atu, &params.durty.atu);
    if (params.durty.pwr)                   params_write_int("pwr", params.pwr * 10, &params.durty.pwr);

    if (params.durty.spectrum_factor)       params_write_int("spectrum_factor", params.spectrum_factor, &params.durty.spectrum_factor);
    if (params.durty.spectrum_beta)         params_write_int("spectrum_beta", params.spectrum_beta, &params.durty.spectrum_beta);
    if (params.durty.spectrum_filled)       params_write_int("spectrum_filled", params.spectrum_filled, &params.durty.spectrum_filled);
    if (params.durty.spectrum_peak)         params_write_int("spectrum_peak", params.spectrum_peak, &params.durty.spectrum_peak);
    if (params.durty.spectrum_peak_hold)    params_write_int("spectrum_peak_hold", params.spectrum_peak_hold, &params.durty.spectrum_peak_hold);
    if (params.durty.spectrum_peak_speed)   params_write_int("spectrum_peak_speed", params.spectrum_peak_speed * 10, &params.durty.spectrum_peak_speed);

    if (params.durty.key_speed)             params_write_int("key_speed", params.key_speed, &params.durty.key_speed);
    if (params.durty.key_mode)              params_write_int("key_mode", params.key_mode, &params.durty.key_mode);
    if (params.durty.iambic_mode)           params_write_int("iambic_mode", params.iambic_mode, &params.durty.iambic_mode);
    if (params.durty.key_tone)              params_write_int("key_tone", params.key_tone, &params.durty.key_tone);
    if (params.durty.key_vol)               params_write_int("key_vol", params.key_vol, &params.durty.key_vol);
    if (params.durty.key_train)             params_write_int("key_train", params.key_train, &params.durty.key_train);
    if (params.durty.qsk_time)              params_write_int("qsk_time", params.qsk_time, &params.durty.qsk_time);
    if (params.durty.key_ratio)             params_write_int("key_ratio", params.key_ratio, &params.durty.key_ratio);

    if (params.durty.mic)                   params_write_int("mic", params.mic, &params.durty.mic);
    if (params.durty.hmic)                  params_write_int("hmic", params.hmic, &params.durty.hmic);
    if (params.durty.imic)                  params_write_int("imic", params.imic, &params.durty.imic);

    if (params.durty.charger)               params_write_int("charger", params.charger, &params.durty.charger);

    if (params.durty.dnf)                   params_write_int("dnf", params.dnf, &params.durty.dnf);
    if (params.durty.dnf_center)            params_write_int("dnf_center", params.dnf_center, &params.durty.dnf_center);
    if (params.durty.dnf_width)             params_write_int("dnf_width", params.dnf_width, &params.durty.dnf_width);
    if (params.durty.nb)                    params_write_int("nb", params.nb, &params.durty.nb);
    if (params.durty.nb_level)              params_write_int("nb_level", params.nb_level, &params.durty.nb_level);
    if (params.durty.nb_width)              params_write_int("nb_width", params.nb_width, &params.durty.nb_width);
    if (params.durty.nr)                    params_write_int("nr", params.nr, &params.durty.nr);
    if (params.durty.nr_level)              params_write_int("nr_level", params.nr_level, &params.durty.nr_level);

    if (params.durty.agc_hang)              params_write_int("agc_hang", params.agc_hang, &params.durty.agc_hang);
    if (params.durty.agc_knee)              params_write_int("agc_knee", params.agc_knee, &params.durty.agc_knee);
    if (params.durty.agc_slope)             params_write_int("agc_slope", params.agc_slope, &params.durty.agc_slope);

    if (!params_exec("COMMIT")) {
        return false;
    }

    return true;
}

/* * */

bool params_bands_load() {
    sqlite3_stmt    *stmt;
    int             rc;
    
    rc = sqlite3_prepare_v2(db, "SELECT id,name,start_freq,stop_freq,type FROM bands ORDER BY start_freq ASC", -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        return false;
    }
    
    while (sqlite3_step(stmt) != SQLITE_DONE) {
        int         id = sqlite3_column_int(stmt, 0);
        const char  *name = sqlite3_column_text(stmt, 1);
        uint64_t    start_freq = sqlite3_column_int64(stmt, 2);
        uint64_t    stop_freq = sqlite3_column_int64(stmt, 3);
        uint8_t     type = sqlite3_column_int(stmt, 4);
        
        bands_insert(id, name, start_freq, stop_freq, type);
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

                if (!params_mode_save()) {
                    LV_LOG_ERROR("Mode params not saved");
                }
            }
        }
        
        pthread_mutex_unlock(&params_mux);
        usleep(100000);
    }
}

void params_init() {
    int rc = sqlite3_open("/var/lib/x6100/params.db", &db);
    
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

        rc = sqlite3_prepare_v2(db, "INSERT INTO mode_params(mode, name, val) VALUES(?, ?, ?)", -1, &write_mode_stmt, 0);

        if (rc != SQLITE_OK) {
            LV_LOG_ERROR("Prepare mode write");
        }

        rc = sqlite3_prepare_v2(db, "INSERT INTO atu(ant, freq, val) VALUES(?, ?, ?)", -1, &save_atu_stmt, 0);

        if (rc != SQLITE_OK) {
            LV_LOG_ERROR("Prepare atu save");
        }

        rc = sqlite3_prepare_v2(db, "SELECT val FROM atu WHERE ant = ? AND freq = ?", -1, &load_atu_stmt, 0);

        if (rc != SQLITE_OK) {
            LV_LOG_ERROR("Prepare atu load");
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

void params_band_freq_set(uint64_t freq) {
    params_lock();

    params_band.vfo_x[params_band.vfo].freq = freq;
    params_unlock(&params_band.vfo_x[params_band.vfo].durty.freq);
}

void params_atu_save(uint32_t val) {
    uint64_t freq = params_band.vfo_x[params_band.vfo].freq;

    params_lock();

    sqlite3_bind_int(save_atu_stmt, 1, params.ant);
    sqlite3_bind_int(save_atu_stmt, 2, freq / 50000);
    sqlite3_bind_int(save_atu_stmt, 3, val);
    
    sqlite3_step(save_atu_stmt);
    sqlite3_reset(save_atu_stmt);
    sqlite3_clear_bindings(save_atu_stmt);
    
    params_unlock(NULL);
}

uint32_t params_atu_load() {
    uint32_t    res = 0;
    uint64_t    freq = params_band.vfo_x[params_band.vfo].freq;

    params_lock();

    sqlite3_bind_int(load_atu_stmt, 1, params.ant);
    sqlite3_bind_int(load_atu_stmt, 2, freq / 50000);

    if (sqlite3_step(load_atu_stmt) != SQLITE_DONE) {
        res = sqlite3_column_int64(load_atu_stmt, 0);
    }

    sqlite3_reset(load_atu_stmt);
    sqlite3_clear_bindings(load_atu_stmt);

    params_unlock(NULL);
    
    return res;
}

void params_band_vfo_clone() {
    params_vfo_t *a = &params_band.vfo_x[X6100_VFO_A];
    params_vfo_t *b = &params_band.vfo_x[X6100_VFO_B];

    if (params_band.vfo == X6100_VFO_A) {
        *b = *a;
        
        b->durty.freq = true;
        b->durty.att = true;
        b->durty.pre = true;
        b->durty.mode = true;
        b->durty.agc = true;
    } else {
        *a = *b;

        a->durty.freq = true;
        a->durty.att = true;
        a->durty.pre = true;
        a->durty.mode = true;
        a->durty.agc = true;
    }
}
