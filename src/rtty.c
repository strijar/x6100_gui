/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <math.h>
#include <pthread.h>
#include "lvgl/lvgl.h"
#include "rtty.h"
#include "audio.h"
#include "params.h"
#include "pannel.h"
#include "util.h"

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

static pthread_mutex_t  rtty_mux;

static fskdem           demod = NULL;

static nco_crcf         nco = NULL;
static float complex    *nco_buf = NULL;

static uint16_t         symbol_samples;
static uint16_t         symbol_over;

static cbuffercf        rx_buf;
static complex float    *rx_window = NULL;
static uint8_t          rx_symbol[SYMBOL_LEN];
static rx_state_t       rx_state = RX_STATE_IDLE;
static uint8_t          rx_counter = 0;
static uint8_t          rx_bitcntr = 0;
static uint8_t          rx_data = 0;
static bool             rx_letter = true;

static bool             ready = false;
static bool             enable = false;

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

static void update_nco() {
    float radians = 2.0f * (float) M_PI * (float) params.rtty_center / (float) AUDIO_CAPTURE_RATE;

    nco_crcf_set_phase(nco, 0.0f);
    nco_crcf_set_frequency(nco, radians);
}

static void init() {
    symbol_samples = (float) AUDIO_CAPTURE_RATE / (float) (params.rtty_rate / 100.0f) / (float) SYMBOL_FACTOR + 0.5f;
    symbol_over = symbol_samples / SYMBOL_OVER;

    nco = nco_crcf_create(LIQUID_NCO);
    nco_buf = (float complex*) malloc(symbol_samples * sizeof(float complex));
    update_nco();

    /* RX */

    demod = fskdem_create(1, symbol_samples, (float) params.rtty_shift / (float) AUDIO_CAPTURE_RATE / 2.0f);
    rx_buf = cbuffercf_create(symbol_samples * 50);
    
    rx_window = malloc(symbol_samples * sizeof(complex float));

    for (uint16_t i = 0; i < symbol_samples; i++)
        rx_window[i] = liquid_hann(i, symbol_samples);
    
    ready = true;
}

static void done() {
    ready = false;

    nco_crcf_destroy(nco);
    free(nco_buf);

    fskdem_destroy(demod);
    cbuffercf_destroy(rx_buf);
    free(rx_window);
}

static void update() {
    pthread_mutex_lock(&rtty_mux);
    done();
    init();
    pthread_mutex_unlock(&rtty_mux);
}

void rtty_init() {
    pthread_mutex_init(&rtty_mux, NULL);
    
    init();    
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
    pthread_mutex_lock(&rtty_mux);

    if (!ready) {
        pthread_mutex_unlock(&rtty_mux);
        return;
    }

    cbuffercf_write(rx_buf, samples, n);
    
    x6100_mode_t    mode = params_band.vfo_x[params_band.vfo].mode;

    while (cbuffercf_size(rx_buf) > symbol_samples) {
        unsigned int    symbol;
        unsigned int    n;
        float complex   *buf;
        
        cbuffercf_read(rx_buf, symbol_samples, &buf, &n);
        nco_crcf_mix_block_down(nco, buf, nco_buf, n);

        for (uint16_t i = 0; i < n; i++)
            nco_buf[i] *= rx_window[i];

        symbol = fskdem_demodulate(demod, nco_buf);

        if (((mode == x6100_mode_usb || mode == x6100_mode_usb_dig) && !params.rtty_reverse) || 
            ((mode == x6100_mode_lsb || mode == x6100_mode_lsb_dig) && params.rtty_reverse))
        {
            symbol = (symbol == 0) ? 1 : 0;
        }
        
        add_symbol(symbol);

        float pwr0 = fskdem_get_symbol_energy(demod, 0, 1);
        float pwr1 = fskdem_get_symbol_energy(demod, 1, 1);
        
//        LV_LOG_INFO("%3i %3i --- %i", (int) pwr0, (int) pwr1, symbol);

        cbuffercf_release(rx_buf, symbol_over);
    }
    
    pthread_mutex_unlock(&rtty_mux);
}

void rtty_enable(bool on) {
    enable = on;
}

bool rtty_is_enabled() {
    return enable;
}

float rtty_change_rate(int16_t df) {
    if (df == 0) {
        return (float) params.rtty_rate / 100.0f;
    }

    params_lock();
    
    if (params.rtty_rate == 4500) {
        params.rtty_rate = df > 0 ? 4545 : 10000;
    } else if (params.rtty_rate == 4545) {
        params.rtty_rate = df > 0 ? 5000 : 4500;
    } else if (params.rtty_rate == 5000) {
        params.rtty_rate = df > 0 ? 7500 : 4545;
    } else if (params.rtty_rate == 7500) {
        params.rtty_rate = df > 0 ? 10000 : 5000;
    } else if (params.rtty_rate == 10000) {
        params.rtty_rate = df > 0 ? 4500 : 7500;
    }

    params_unlock(&params.durty.rtty_rate);
    update();

    return (float) params.rtty_rate / 100.0f;
}

uint16_t rtty_change_shift(int16_t df) {
    if (df == 0) {
        return params.rtty_shift;
    }

    params_lock();
    
    if (params.rtty_shift == 170) {
        params.rtty_shift = df > 0 ? 425 : 850;
    } else if (params.rtty_shift == 425) {
        params.rtty_shift = df > 0 ? 450 : 170;
    } else if (params.rtty_shift == 450) {
        params.rtty_shift = df > 0 ? 850 : 425;
    } else if (params.rtty_shift == 850) {
        params.rtty_shift = df > 0 ? 170 : 450;
    }
    
    params_unlock(&params.durty.rtty_shift);
    update();

    return params.rtty_shift;
}

uint16_t rtty_change_center(int16_t df) {
    if (df == 0) {
        return params.rtty_center;
    }

    params_lock();
    params.rtty_center = limit(align_int(params.rtty_center + df * 10, 10), 800, 1600);
    params_unlock(&params.durty.rtty_center);

    pthread_mutex_lock(&rtty_mux);
    update_nco();
    pthread_mutex_unlock(&rtty_mux);

    return params.rtty_center;
}

bool rtty_change_reverse(int16_t df) {
    if (df == 0) {
        return params.rtty_reverse;
    }

    params_lock();
    params.rtty_reverse = !params.rtty_reverse;
    params_unlock(&params.durty.rtty_reverse);

    return params.rtty_reverse;
}
