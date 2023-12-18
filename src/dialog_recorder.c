/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sndfile.h>
#include <dirent.h>
#include <pthread.h>

#include <aether_radio/x6100_control/control.h>

#include "lvgl/lvgl.h"
#include "audio.h"
#include "recorder.h"
#include "dialog.h"
#include "dialog_recorder.h"
#include "styles.h"
#include "params.h"
#include "events.h"
#include "util.h"
#include "pannel.h"
#include "keyboard.h"
#include "textarea_window.h"
#include "msg.h"
#include "buttons.h"

#define BUF_SIZE 1024

static lv_obj_t             *table;
static int16_t              table_rows = 0;
static SNDFILE              *file = NULL;
static bool                 play_state = false;

static char                 *prev_filename;
static pthread_t            thread;
static int16_t              samples_buf[BUF_SIZE];

static void construct_cb(lv_obj_t *parent);
static void destruct_cb();
static void key_cb(lv_event_t * e);
static void rec_stop_cb(lv_event_t * e);
static void play_stop_cb(lv_event_t * e);

static button_item_t button_rec_stop = { .label = "Rec\nStop", .press = rec_stop_cb };
static button_item_t button_play_stop = { .label = "Play\nStop", .press = play_stop_cb };

static dialog_t             dialog = {
    .run = false,
    .construct_cb = construct_cb,
    .destruct_cb = destruct_cb,
    .audio_cb = NULL,
    .key_cb = NULL
};

dialog_t                    *dialog_recorder = &dialog;

static void load_table() {
    lv_table_set_row_cnt(table, 1);
    table_rows = 0;

    DIR             *dp;
    struct dirent   *ep;
    
    dp = opendir(recorder_path);

    if (dp != NULL) {
        while ((ep = readdir(dp)) != NULL) {
            if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0) {
                continue;
            }

            lv_table_set_cell_value(table, table_rows++, 0, ep->d_name);
        }
          
        closedir(dp);
    }
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

static void play_item() {
    const char *item = get_item();

    if (!item) {
        return;
    }
    
    char filename[64];
        
    strcpy(filename, recorder_path);
    strcat(filename, "/");
    strcat(filename, item);

    SF_INFO sfinfo;

    memset(&sfinfo, 0, sizeof(sfinfo));

    SNDFILE *file = sf_open(filename, SFM_READ, &sfinfo);

    if (!file) {
        return;
    }
    
    if (recorder_is_on()) {
        recorder_set_on(false);
    }

    play_state = true;

    while (play_state) {
        int res = sf_read_short(file, samples_buf, BUF_SIZE);
            
        if (res > 0) {
            int16_t *samples = audio_gain(samples_buf, res, params.play_gain);
            
            audio_play(samples, res);
            free(samples);
        } else {
            play_state = false;
        }
    }

    sf_close(file);
    audio_play_wait();
}

static void * play_thread(void *arg) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    audio_play_en(true);
    play_item();
    audio_play_en(false);

    if (dialog.run) {
        buttons_unload_page();
        buttons_load_page(PAGE_RECORDER);
    }
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
        
        snprintf(prev, sizeof(prev), "%s/%s", recorder_path, prev_filename);
        snprintf(new, sizeof(new), "%s/%s", recorder_path, new_filename);

        if (rename(prev, new) == 0) {
            load_table();
            textarea_window_close_cb();
        }
    } else {
        free(prev_filename);
        prev_filename = NULL;
    }
}

static void tx_cb(lv_event_t * e) {
    if (play_state) {
        play_state = false;

        buttons_unload_page();
        buttons_load_page(PAGE_RECORDER);
    }
}

static void construct_cb(lv_obj_t *parent) {
    dialog.obj = dialog_init(parent);

    lv_obj_add_event_cb(dialog.obj, tx_cb, EVENT_RADIO_TX, NULL);

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
    
    mkdir(recorder_path, 0755);
    load_table();
    
    if (recorder_is_on()) {
        buttons_unload_page();
        buttons_load(1, &button_rec_stop);
    }
}

static void destruct_cb() {
    audio_play_en(false);
    play_state = false;
    textarea_window_close();
}

static void key_cb(lv_event_t * e) {
    uint32_t key = *((uint32_t *)lv_event_get_param(e));

    switch (key) {
        case LV_KEY_ESC:
            dialog_destruct(&dialog);
            break;

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

void dialog_recorder_rec_cb(lv_event_t * e) {
    recorder_set_on(true);
}

static void rec_stop_cb(lv_event_t * e) {
    recorder_set_on(false);
    load_table();
}

void dialog_recorder_play_cb(lv_event_t * e) {
    pthread_create(&thread, NULL, play_thread, NULL);

    buttons_unload_page();
    buttons_load(4, &button_play_stop);
}

void play_stop_cb(lv_event_t * e) {
    play_state = false;
}

void dialog_recorder_rename_cb(lv_event_t * e) {
    prev_filename = strdup(get_item());
    
    if (prev_filename) {
        lv_group_remove_obj(table);
        textarea_window_open(textarea_window_edit_ok_cb, textarea_window_close_cb);
        textarea_window_set(prev_filename);
    }
}

void dialog_recorder_delete_cb(lv_event_t * e) {
    const char *item = get_item();

    if (item) {
        char filename[64];
        
        strcpy(filename, recorder_path);
        strcat(filename, "/");
        strcat(filename, item);
        
        unlink(filename);
        load_table();
    }
}

void dialog_recorder_set_on(bool on) {
    if (!dialog.run) {
        return;
    }

    buttons_unload_page();

    if (on) {
        buttons_load(1, &button_rec_stop);
    } else {
        buttons_load_page(PAGE_RECORDER);
        load_table();
    }
}
