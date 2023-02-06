/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

/* Based on idea Michael A. Maynard, a.k.a. "K4ICY" */

#include <math.h>
#include "lvgl/lvgl.h"
#include "cw_decoder.h"

#define HIST_SIZE       10
#define WPM_HIST_SIZE   20

static uint32_t debounce_factor = 15;
static uint32_t thr_geom_mean = 139;
static uint32_t thr_arth_mean = 160;

static uint32_t time_track = 0;

static int32_t  key_line_prior_event = 0;
static int32_t  key_line_new_event = 0;

static uint32_t event_hist_track = 0;
static uint32_t short_event_hist[HIST_SIZE];
static uint32_t long_event_hist[HIST_SIZE];
static uint32_t space_event_hist[HIST_SIZE];

static uint32_t wpm_hist_track = 0;
static uint16_t wpm_hist[WPM_HIST_SIZE];
static uint16_t old_wpm;
static uint32_t wpm;

static uint32_t key_line_duration = 0;
static uint32_t space_duration = 0;
static uint32_t old_space_duration = 0;
static uint32_t space_duration_ref;

static uint32_t word_space_duration = 0;
static uint32_t word_space_duration_ref = 0;
static float    word_space_timing = 3.0f;

static bool     key_line = false;

static float    compare_factor = 2.0;

static bool     character_step = false;
static bool     word_step = false;
static char     elements[32];

typedef struct {
    char    *morse;
    char    *character;
} characters_t;

static characters_t characters[] = {
    { .morse = ".-",        .character = "A" },
    { .morse = "-...",      .character = "B" },
    { .morse = "-.-.",      .character = "C" },
    { .morse = "-..",       .character = "D" },
    { .morse = ".",         .character = "E" },
    { .morse = "..-.",      .character = "F" },
    { .morse = "--.",       .character = "G" },
    { .morse = "....",      .character = "H" },
    { .morse = "..",        .character = "I" },
    { .morse = ".---",      .character = "J" },
    { .morse = "-.-",       .character = "K" },
    { .morse = ".-..",      .character = "L" },
    { .morse = "--",        .character = "M" },
    { .morse = "-.",        .character = "N" },
    { .morse = "---",       .character = "O" },
    { .morse = ".--.",      .character = "P" },
    { .morse = "--.-",      .character = "Q" },
    { .morse = ".-.",       .character = "R" },
    { .morse = "...",       .character = "S" },
    { .morse = "-",         .character = "T" },
    { .morse = "..-",       .character = "U" },
    { .morse = "...-",      .character = "V" },
    { .morse = ".--",       .character = "W" },
    { .morse = "-..-",      .character = "X" },
    { .morse = "-.--",      .character = "Y" },
    { .morse = "--..",      .character = "Z" },
    
    { .morse = ".----",     .character = "1" },
    { .morse = "..---",     .character = "2" },
    { .morse = "...--",     .character = "3" },
    { .morse = "....-",     .character = "4" },
    { .morse = ".....",     .character = "5" },
    { .morse = "-....",     .character = "6" },
    { .morse = "--...",     .character = "7" },
    { .morse = "---..",     .character = "8" },
    { .morse = "----.",     .character = "9" },
    { .morse = "-----",     .character = "0" },
    
    { .morse = "-.-.--",    .character = "!" },
    { .morse = "..--.",     .character = "!" },
    { .morse = ".-..-.",    .character = "\"" },
    { .morse = "...-..-",   .character = "$" },
    { .morse = ".----.",    .character = "'" },
    { .morse = "--..--",    .character = "," },
    { .morse = "-....-",    .character = "-" },
    { .morse = ".-.-.-",    .character = "." },
    { .morse = "-..-.",     .character = "/" },
    { .morse = "---...",    .character = ":" },
    { .morse = "-.-.-.",    .character = ";" },
    { .morse = "-...-",     .character = "=" },
    { .morse = "..--..",    .character = "?" },
    { .morse = ".--.-.",    .character = "@" },
    { .morse = "..--.-",    .character = "_" },
    
    { .morse = ".-.-.",     .character = "<AR>" },
    { .morse = ".-...",     .character = "<AS>" },
    { .morse = "-...-.-",   .character = "<BK>" },
    { .morse = "-.-..-..",  .character = "<CL>" },
    { .morse = "-.-.-",     .character = "<CT>" },
    { .morse = "-.--.",     .character = "<KN>" },
    { .morse = "...-.-",    .character = "<SK>" },
    { .morse = "...-.",     .character = "<SN>" },
    { .morse = "...---...", .character = "<SOS>" },
    { .morse = "-.-.--.-",  .character = "<CQ>" },
    
    { .morse = "......",    .character = "<ERR>" },
    { .morse = ".......",   .character = "<ERR>" },
    { .morse = "........",  .character = "<ERR>" },
    { .morse = NULL }
};

void cw_decoder_init() {
}

static void morse_decode() {
    characters_t *character = &characters[0];

    while (character->morse) {
        if (strcmp(elements, character->morse) == 0) {
            LV_LOG_INFO("[ %s ]", character->character);
            return;
        }
        
        character++;
    }
    
    LV_LOG_INFO("[ ? %s ]", elements);
}

static void morse_word() {
    LV_LOG_INFO("[ ]", elements);
}

void cw_decoder_signal(bool on, float ms) {
    time_track += (ms + 0.5f);
    
    /* Key down */
    
    if (on) {
        if (!key_line) {
            key_line_duration = time_track;
            word_space_duration_ref = time_track;
        }
        
        key_line = true;
    }
    
    /* Key up */
    
    if (!on) { 
        if (time_track >= (key_line_duration + debounce_factor) && key_line) {
            key_line = false;
            key_line_prior_event = key_line_new_event;
            key_line_new_event = time_track - key_line_duration;

            /* IF the Current Duration Event Compared to the Previous Event appears to be a Dot / Dash pair [ roughly (>2):1 ] */

            if ((key_line_new_event >= key_line_prior_event * compare_factor && old_space_duration <= key_line_prior_event * compare_factor) ||
                (key_line_prior_event >= key_line_new_event * compare_factor && old_space_duration <= key_line_new_event * compare_factor)) {
                
                /* Find out which one is the Dot and which is the Dash and roll them into a moving average of each */

                if (key_line_new_event >= key_line_prior_event) {
                    long_event_hist[event_hist_track] = key_line_new_event;
                    short_event_hist[event_hist_track] = key_line_prior_event;
                } else {
                    long_event_hist[event_hist_track] = key_line_prior_event;
                    short_event_hist[event_hist_track] = key_line_new_event;
                }

                /* Keep a moving average of the intra-element space duration */
                
                space_event_hist[event_hist_track] = old_space_duration;
                
                /* Keep a moving averages */

                uint64_t long_event_avr = 0;
                uint64_t short_event_avr = 0;
                uint64_t space_event_avr = 0;
                
                for (uint8_t i = 0; i < HIST_SIZE; i++) {
                    long_event_avr += long_event_hist[i];
                    short_event_avr += short_event_hist[i];
                    space_event_avr += space_event_hist[i];
                }
                    
                long_event_avr /= HIST_SIZE;
                short_event_avr /= HIST_SIZE;
                space_event_avr /= HIST_SIZE;

                /* Find threshold means */
                
                thr_geom_mean = sqrt(short_event_avr * long_event_avr);
                thr_arth_mean = (short_event_avr + long_event_avr) / 2;
                
                /* Bootstrap threshold values - - - If any are below or above known Dot/Dash pair ranges then move them instantly */
                
                if (thr_geom_mean < short_event_hist[event_hist_track] || thr_geom_mean < long_event_hist[event_hist_track]) {
                    thr_geom_mean = sqrt(short_event_hist[event_hist_track] * long_event_hist[event_hist_track]);

                    long_event_avr = long_event_hist[event_hist_track];
                    short_event_avr = short_event_hist[event_hist_track];
                    
                    for (uint8_t i = 0; i < HIST_SIZE; i++) {
                        long_event_hist[i] = long_event_avr;
                        short_event_hist[i] = short_event_avr;
                    }
                    
                    wpm_hist[wpm_hist_track] = (6000 / (long_event_avr + short_event_avr + old_space_duration));
                    
                    for (uint8_t i = 0; i < WPM_HIST_SIZE; i++)
                        wpm_hist[i] = wpm_hist[wpm_hist_track];
                }

                /* we will now calculate the WPM... */
                
                old_wpm = wpm;
                
                wpm_hist[wpm_hist_track] = (6000 / (long_event_avr + short_event_avr + space_event_avr));
                
                wpm = (6000 * 1.06) / (long_event_avr + short_event_avr + space_event_avr);
                
                if (wpm != old_wpm) {
//                    LV_LOG_INFO("WPM %i", wpm);
                }
                
                event_hist_track++;
                
                if (event_hist_track > HIST_SIZE)
                    event_hist_track = 0;
                    
                wpm_hist_track++;
                
                if (wpm_hist_track > WPM_HIST_SIZE)
                    wpm_hist_track = 0;
            }
            
            /* Reset space durations - in Key-Up state */
            
            space_duration_ref = time_track;
            word_space_duration_ref = time_track;

            /* Classify and add most likely Dots or Dashes to a string for eventual character decoding */
            
            if (key_line_new_event <= thr_geom_mean) {
                strcat(elements, ".");
                character_step = true;
                word_step = true;
            } else {
                strcat(elements, "-");
                character_step = true;
                word_step = true;
            }
        }
        
        /* Keep track of Key-Up space timing */
        
        old_space_duration = space_duration;
        space_duration = time_track - space_duration_ref;

        /* DECODE collected string of elements */

        /* check to see if inter-element space duration threshold has been exceeded - then decode   */
        /* it is assumed that the intra-space is longer than a Dot but shorter than a Dash          */

        if (space_duration >= thr_geom_mean) {  /* Using thr_arth_mean seems more stable, instead accurate */
            space_duration_ref = time_track;
            
            if (character_step) {
                morse_decode();
                elements[0] = '\0';
            }
            
            character_step = false;
        }
        
        /* Keep track of Key-Up timing and see if a word space is required */

        word_space_duration = time_track - word_space_duration_ref;
        
        if (word_space_duration >= thr_geom_mean * word_space_timing) {
            word_space_duration_ref = time_track;
            
            if (word_step) {
                morse_word();
            }
            
            word_step = false;
        }
    }
}
