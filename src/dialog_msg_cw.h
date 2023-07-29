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

extern dialog_t *dialog_msg_cw;

void dialog_msg_cw_send_cb(lv_event_t * e);
void dialog_msg_cw_beacon_cb(lv_event_t * e);
void dialog_msg_cw_period_cb(lv_event_t * e);

void dialog_msg_cw_new_cb(lv_event_t * e);
void dialog_msg_cw_edit_cb(lv_event_t * e);
void dialog_msg_cw_delete_cb(lv_event_t * e);

void dialog_msg_cw_append(uint32_t id, const char *val);
