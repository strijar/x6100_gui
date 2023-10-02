/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sndfile.h>
#include <dirent.h>

#include <aether_radio/x6100_control/control.h>

#include "lvgl/lvgl.h"
#include "audio.h"
#include "dialog.h"
#include "dialog_msg_voice.h"
#include "styles.h"
#include "params.h"
#include "events.h"
#include "util.h"
#include "pannel.h"
#include "keyboard.h"
#include "textarea_window.h"
#include "msg.h"
#include "meter.h"

static msg_voice_state_t    state = MSG_VOICE_OFF;
static char                 *path = "/mnt/msg";

static lv_obj_t             *table;
static int16_t              table_rows = 0;
static SNDFILE              *file = NULL;

static char                 *prev_filename;

static void construct_cb(lv_obj_t *parent);
static void destruct_cb();
static void key_cb(lv_event_t * e);

static dialog_t             dialog = {
    .run = false,
    .construct_cb = construct_cb,
    .destruct_cb = destruct_cb,
    .key_cb = NULL
};

dialog_t                    *dialog_msg_voice = &dialog;

static void load_table() {
    lv_table_set_row_cnt(table, 0);
    table_rows = 0;

    DIR             *dp;
    struct dirent   *ep;
    
    dp = opendir(path);

    if (dp != NULL) {
        while ((ep = readdir(dp)) != NULL) {
            if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0) {
                continue;
            }

            lv_table_set_cell_value(table, table_rows++, 0, ep->d_name);
        }
          
        closedir(dp);
    } else {
    }
}

static bool open_file() {
    SF_INFO sfinfo;

    memset (&sfinfo, 0, sizeof(sfinfo));

    sfinfo.samplerate = AUDIO_CAPTURE_RATE;
    sfinfo.channels = 1;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    
    char        filename[64];
    time_t      now = time(NULL);
    struct tm   *t = localtime(&now);

    snprintf(filename, sizeof(filename), 
        "%s/MSG_%04i%02i%02i_%02i%02i%02i.wav", 
        path, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec
    );
    
    file = sf_open(filename, SFM_WRITE, &sfinfo);
    
    if (file) {
        return true;
    }
    
    return false;
}

static void close_file() {
    sf_close(file);
}

static const char* get_item() {
    if (table_rows == 0) {
        return NULL;
    }

    int16_t     row = 0;
    int16_t     col = 0;

    lv_table_get_selected_cell(table, &row, &col);

    if (row == LV_TABLE_CELL_NONE) {
        return NULL;
    }
    
    return lv_table_get_cell_value(table, row, col);
}

static void textarea_window_close_cb() {
    lv_group_add_obj(keyboard_group, table);
    lv_group_set_editing(keyboard_group, true);
    
    free(prev_filename);
    prev_filename = NULL;
}

static void textarea_window_edit_ok_cb() {
    const char *new_filename = textarea_window_get();
    
    if (strcmp(prev_filename, new_filename) != 0) {
        char prev[64];
        char new[64];
        
        snprintf(prev, sizeof(prev), "%s/%s", path, prev_filename);
        snprintf(new, sizeof(new), "%s/%s", path, new_filename);

        if (rename(prev, new) == 0) {
            load_table();
            textarea_window_close_cb();
        }
    } else {
        free(prev_filename);
        prev_filename = NULL;
    }
}

static void construct_cb(lv_obj_t *parent) {
    dialog.obj = dialog_init(parent);

    table = lv_table_create(dialog.obj);
    
    lv_obj_remove_style(table, NULL, LV_STATE_ANY | LV_PART_MAIN);

    lv_obj_set_size(table, 775, 325);
    
    lv_table_set_col_cnt(table, 1);
    lv_table_set_col_width(table, 0, 770);

    lv_obj_set_style_border_width(table, 0, LV_PART_ITEMS);
    
    lv_obj_set_style_bg_opa(table, LV_OPA_TRANSP, LV_PART_ITEMS);
    lv_obj_set_style_text_color(table, lv_color_white(), LV_PART_ITEMS);
    lv_obj_set_style_pad_top(table, 5, LV_PART_ITEMS);
    lv_obj_set_style_pad_bottom(table, 5, LV_PART_ITEMS);
    lv_obj_set_style_pad_left(table, 0, LV_PART_ITEMS);
    lv_obj_set_style_pad_right(table, 0, LV_PART_ITEMS);

    lv_obj_set_style_text_color(table, lv_color_black(), LV_PART_ITEMS | LV_STATE_EDITED);
    lv_obj_set_style_bg_color(table, lv_color_white(), LV_PART_ITEMS | LV_STATE_EDITED);
    lv_obj_set_style_bg_opa(table, 128, LV_PART_ITEMS | LV_STATE_EDITED);

    lv_obj_add_event_cb(table, key_cb, LV_EVENT_KEY, NULL);
    lv_group_add_obj(keyboard_group, table);
    lv_group_set_editing(keyboard_group, true);

    lv_obj_center(table);
    
    mkdir(path, 0755);
    load_table();
}

static void destruct_cb() {
    x6100_control_record_set(false);
    state = MSG_VOICE_OFF;
}

static void key_cb(lv_event_t * e) {
    uint32_t key = *((uint32_t *)lv_event_get_param(e));

    switch (key) {
        case KEY_VOL_LEFT_EDIT:
        case KEY_VOL_LEFT_SELECT:
            radio_change_vol(-1);
            break;

        case KEY_VOL_RIGHT_EDIT:
        case KEY_VOL_RIGHT_SELECT:
            radio_change_vol(1);
            break;
    }
}

void dialog_msg_voice_send_cb(lv_event_t * e) {
}

void dialog_msg_voice_beacon_cb(lv_event_t * e) {
}

void dialog_msg_voice_period_cb(lv_event_t * e) {
}

void dialog_msg_voice_rec_cb(lv_event_t * e) {
    switch (state) {
        case MSG_VOICE_OFF:
            if (open_file()) {
                x6100_control_record_set(true);
                state = MSG_VOICE_RECORD;
            }
            break;

        case MSG_VOICE_RECORD:
            x6100_control_record_set(false);
            state = MSG_VOICE_OFF;
            close_file();
            load_table();
            break;

        default:
            break;
    }
}

void dialog_msg_voice_rename_cb(lv_event_t * e) {
    prev_filename = strdup(get_item());
    
    if (prev_filename) {
        lv_group_remove_obj(table);
        textarea_window_open(textarea_window_edit_ok_cb, textarea_window_close_cb);
        textarea_window_set(prev_filename);
    }
}

void dialog_msg_voice_delete_cb(lv_event_t * e) {
    const char *item = get_item();

    if (item) {
        char filename[64];
        
        strcpy(filename, path);
        strcat(filename, "/");
        strcat(filename, item);
        
        unlink(filename);
        load_table();
    }
}

msg_voice_state_t dialog_msg_voice_get_state() {
    return state;
}

void dialog_msg_voice_put_audio_samples(size_t nsamples, int16_t *samples) {
    int16_t peak = 0;
    
    for (uint16_t i = 0; i < nsamples; i++) {
        int16_t x = abs(samples[i]);
    
        if (x > peak) {
            peak = x;
        }
    }

    peak = S1 + (peak / 32768.0) * (S9_40 - S1);
    meter_update(peak, 0.25f);
    sf_write_short(file, samples, nsamples);
}
