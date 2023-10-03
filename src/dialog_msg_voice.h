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

typedef enum {
    MSG_VOICE_OFF = 0,
    MSG_VOICE_RECORD,
    MSG_VOICE_PLAY
} msg_voice_state_t;

extern dialog_t *dialog_msg_voice;

void dialog_msg_voice_send_cb(lv_event_t * e);
void dialog_msg_voice_beacon_cb(lv_event_t * e);
void dialog_msg_voice_period_cb(lv_event_t * e);

void dialog_msg_voice_rec_cb(lv_event_t * e);
void dialog_msg_voice_play_cb(lv_event_t * e);
void dialog_msg_voice_rename_cb(lv_event_t * e);
void dialog_msg_voice_delete_cb(lv_event_t * e);

msg_voice_state_t dialog_msg_voice_get_state();
void dialog_msg_voice_put_audio_samples(size_t nsamples, int16_t *samples);
