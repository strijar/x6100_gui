/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <math.h>
#include "lvgl/lvgl.h"
#include "rtty.h"
#include "audio.h"
#include "params.h"
#include "pannel.h"

#define SYMBOL_OVER         8
#define SYMBOL_FACTOR       2
#define SYMBOL_LEN          (SYMBOL_OVER * SYMBOL_FACTOR)

#define RTTY_SYMBOL_CODE    (0b11011)
#define RTTY_LETTER_CODE    (0b11111)

typedef enum {
    RX_STATE_IDLE,
    RX_STATE_START,
    RX_STATE_DATA,
    RX_STATE_STOP
} rx_state_t;

static fskdem           demod = NULL;

static nco_crcf         nco = NULL;
static float complex    *nco_buf = NULL;

static uint16_t         symbol_samples;
static uint16_t         symbol_over;

static cbuffercf        rx_buf;
static complex float    *rx_window;
static uint8_t          rx_symbol[SYMBOL_LEN];
static rx_state_t       rx_state = RX_STATE_IDLE;
static uint8_t          rx_counter = 0;
static uint8_t          rx_bitcntr = 0;
static uint8_t          rx_data = 0;
static bool             rx_letter = true;

static bool             ready = false;

static const char rtty_letters[32] = {
    '\0',   'E',    '\n',   'A',    ' ',    'S',    'I',    'U',
    '\0',   'D',    'R',    'J',    'N',    'F',    'C',    'K',
    'T',    'Z',    'L',    'W',    'H',    'Y',    'P',    'Q',
    'O',    'B',    'G',    ' ',    'M',    'X',    'V',    ' '
};

static const char rtty_symbols[32] = {
    '\0',   '3',    '\n',   '-',    ' ',    '\0',   '8',    '7',
    '\0',   '$',    '4',    '\'',   ',',    '!',    ':',    '(',
    '5',    '"',    ')',    '2',    '#',    '6',    '0',    '1',
    '9',    '?',    '&',    ' ',    '.',    '/',    ';',    ' '
};

void rtty_init() {
    symbol_samples = (float) AUDIO_CAPTURE_RATE / params.rtty_rate / (float) SYMBOL_FACTOR + 0.5f;
    symbol_over = symbol_samples / SYMBOL_OVER;

    float radians = 2.0f * (float) M_PI * (float) params.rtty_center / (float) AUDIO_CAPTURE_RATE;

    nco = nco_crcf_create(LIQUID_NCO);
    nco_crcf_set_phase(nco, 0.0f);
    nco_crcf_set_frequency(nco, radians);
    nco_buf = (float complex*) malloc(symbol_samples * sizeof(float complex));

    /* RX */

    demod = fskdem_create(1, symbol_samples, (float) params.rtty_width / (float) AUDIO_CAPTURE_RATE / 2.0f);
    rx_buf = cbuffercf_create(symbol_samples * 50);
    
    rx_window = malloc(symbol_samples * sizeof(complex float));

    for (uint16_t i = 0; i < symbol_samples; i++)
        rx_window[i] = liquid_hann(i, symbol_samples);
    
    ready = true;
}

static char baudot_decoder(uint8_t c) {
    if (c == RTTY_SYMBOL_CODE) {
        rx_letter = false;
        return 0;
    }

    if (c == RTTY_LETTER_CODE) {
        rx_letter = true;
        return 0;
    }

    return rx_letter ? rtty_letters[c] : rtty_symbols[c];
}

static bool is_mark_space(uint8_t *correction) {
    uint16_t res = 0;
    
    if (rx_symbol[0] && !rx_symbol[SYMBOL_LEN-1]) {
        for (int i = 0; i < SYMBOL_LEN; i++)
            res += rx_symbol[i];
            
        if (abs(SYMBOL_LEN/2 - res) < 1) {
            *correction = res;
            return true;
        }
    }
    return false;
}

static bool is_mark() {
    return rx_symbol[SYMBOL_LEN / 2];
}

static void add_symbol(uint8_t sym) {
    for (uint8_t i = 1; i < SYMBOL_LEN; i++)
        rx_symbol[i - 1] = rx_symbol[i];
        
    rx_symbol[SYMBOL_LEN - 1] = sym;
    
    uint8_t correction;
    
    switch (rx_state) {
        case RX_STATE_IDLE:
            if (is_mark_space(&correction)) {
                rx_state = RX_STATE_START;
                rx_counter = correction;
            }
            break;
            
        case RX_STATE_START:
            if (--rx_counter == 0) {
                if (!is_mark()) {
                    rx_state = RX_STATE_DATA;
                    rx_counter = SYMBOL_LEN;
                    rx_bitcntr = 0;
                    rx_data = 0;
                } else {
                    rx_state = RX_STATE_IDLE;
                }
            }
            break;
            
        case RX_STATE_DATA:
            if (--rx_counter == 0) {
                rx_data |= is_mark() << rx_bitcntr++;
                rx_counter = SYMBOL_LEN;
            }
        
            if (rx_bitcntr == params.rtty_bits)
                rx_state = RX_STATE_STOP;
            break;

        case RX_STATE_STOP:
            if (--rx_counter == 0) {
                if (is_mark()) {
                    char c = baudot_decoder(rx_data);
                    
                    if (c) {
                        char str[2] = { c, 0 };
                        
                        pannel_add_text(str);
                    }
                }
                rx_state = RX_STATE_IDLE;
            }
            break;
    }
}

void rtty_put_audio_samples(unsigned int n, float complex *samples) {
    if (!ready) {
        return;
    }

    cbuffercf_write(rx_buf, samples, n);
    
    while (cbuffercf_size(rx_buf) > symbol_samples) {
        unsigned int    symbol;
        unsigned int    n;
        float complex   *buf;
        
        cbuffercf_read(rx_buf, symbol_samples, &buf, &n);
        nco_crcf_mix_block_down(nco, buf, nco_buf, n);

        for (uint16_t i = 0; i < n; i++)
            nco_buf[i] *= rx_window[i];

        symbol = fskdem_demodulate(demod, nco_buf);
        
        symbol = (symbol == 0) ? 1 : 0;
        
        add_symbol(symbol);

        float pwr0 = fskdem_get_symbol_energy(demod, 0, 1);
        float pwr1 = fskdem_get_symbol_energy(demod, 1, 1);
        
//        LV_LOG_INFO("%3i %3i --- %i", (int) pwr0, (int) pwr1, symbol);

        cbuffercf_release(rx_buf, symbol_over);
    }
}
