/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "lvgl/lvgl.h"

#include "cw_decoder.h"
#include "cw_encoder.h"
#include "params.h"
#include "radio.h"
#include "msg.h"

static cw_encoder_state_t   state = CW_ENCODER_IDLE;
static pthread_t            thread;

static char                 *current_msg = NULL;
static char                 *current_char = NULL;

static uint8_t get_morse(char *str, char **morse) {
    cw_characters_t *character = &cw_characters[0];

    while (character->morse) {
        uint8_t char_len = strlen(character->character);
        
        if (strncasecmp(character->character, str, char_len) == 0) {
            *morse = character->morse;

            return char_len;
        }
        
        character++;
    }
    
    return 0;
}

static void send_morse(char *str, uint32_t dit, uint32_t dah) {
    while (*str) {
        switch (*str) {
            case '.':
                radio_set_morse_key(true);
                usleep(dit);
                radio_set_morse_key(false);
                break;
                
            case '-':
                radio_set_morse_key(true);
                usleep(dah);
                radio_set_morse_key(false);
                break;
                
            default:
                break;
        }

        usleep(dit);
        str++;
    }

    usleep(dah - dit);
}

static void * endecode_thread(void *arg) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    uint32_t    dit = 60 * 1000000 / (params.key_speed * 50);
    uint32_t    dah = dit * params.key_ratio / 10;

    while (true) {
        char    *morse;
        uint8_t len;

        if (*current_char == ' ') {
            current_char++;
            usleep(dit * (7 - 3));
        } else {
            len = get_morse(current_char, &morse);
        
            if (len) {
                send_morse(morse, dit, dah);
                current_char += len;
            } else {
                current_char++;
                usleep(dit * (7 - 3));
            }
        }
        
        if (*current_char == 0) {
            if (state == CW_ENCODER_SEND) {
                state = CW_ENCODER_IDLE;
                break;
            } else {
                state = CW_ENCODER_BEACON_IDLE;
                msg_set_text_fmt("Beacon pause: %i s", params.cw_encoder_period);
                sleep(params.cw_encoder_period);
                
                state = CW_ENCODER_BEACON;
                current_char = current_msg;
            }
        }
    }
}

void cw_encoder_stop() {
    if (state != CW_ENCODER_IDLE) {
        pthread_cancel(thread);
        pthread_join(thread, NULL);

        radio_set_morse_key(false);
        state = CW_ENCODER_IDLE;
    }
}

void cw_encoder_send(const char *text, bool beacon) {
    cw_encoder_stop();

    if (current_msg) {
        free(current_msg);
    }
    
    current_msg = strdup(text);
    current_char = current_msg;
    state = beacon ? CW_ENCODER_BEACON : CW_ENCODER_SEND;

    pthread_create(&thread, NULL, endecode_thread, NULL);
}

cw_encoder_state_t cw_encoder_state() {
    return state;
}
