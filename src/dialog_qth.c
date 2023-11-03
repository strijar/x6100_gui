/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include "textarea_window.h"
#include "params.h"
#include "main_screen.h"

static void close_cb() {
    main_screen_keys_enable(true);
}

static void edit_ok_cb() {
    const char *qth = textarea_window_get();

    params_lock();    
    strncpy(params.qth, qth, sizeof(params.qth) - 1);
    params_unlock(&params.durty.qth);

    main_screen_keys_enable(true);
}

void dialog_qth() {
    textarea_window_open(edit_ok_cb, close_cb);
    
    lv_obj_t *text = textarea_window_text();
    
    lv_textarea_set_accepted_chars(text, 
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    );

    lv_textarea_set_max_length(text, 6);
    lv_textarea_set_placeholder_text(text, "QTH Grid");

    textarea_window_set(params.qth);
    main_screen_keys_enable(false);
}
