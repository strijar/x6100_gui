/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include "lvgl/lvgl.h"
#include "dialog.h"
#include "dialog_ft8.h"
#include "styles.h"
#include "params.h"
#include "radio.h"

static lv_obj_t             *dialog;
static ft8_state_t          state = FT8_OFF;

static void deleted_cb(lv_event_t * e) {
    state = FT8_OFF;
}

lv_obj_t * dialog_ft8(lv_obj_t *parent) {
    dialog = dialog_init(parent);

    lv_obj_add_event_cb(dialog, deleted_cb, LV_EVENT_DELETE, NULL);

    state = FT8_RX;
    return dialog;
}

ft8_state_t dialog_ft8_get_state() {
    return state;
}

void dialog_ft8_put_audio_samples(unsigned int n, float complex *samples) {
}
