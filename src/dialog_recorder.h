/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#pragma once

#include "lvgl/lvgl.h"
#include "dialog.h"

extern dialog_t *dialog_recorder;

void dialog_recorder_rec_cb(lv_event_t * e);
void dialog_recorder_play_cb(lv_event_t * e);
void dialog_recorder_rename_cb(lv_event_t * e);
void dialog_recorder_delete_cb(lv_event_t * e);

void dialog_recorder_set_on(bool on);
