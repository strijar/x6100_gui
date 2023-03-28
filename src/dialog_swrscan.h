/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#pragma once

#include "lvgl/lvgl.h"

lv_obj_t * dialog_swrscan(lv_obj_t *parent);

void dialog_swrscan_run_cb(lv_event_t * e);
void dialog_swrscan_scale_cb(lv_event_t * e);
void dialog_swrscan_span_cb(lv_event_t * e);

void dialog_swrscan_update(float vswr);
