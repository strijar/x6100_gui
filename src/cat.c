/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

/* 
 * ICOM protocol implementation
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/poll.h>

#include <aether_radio/x6100_control/low/gpio.h>
#include "lvgl/lvgl.h"

#include "cat.h"
#include "radio.h"
#include "params.h"
#include "util.h"

#define FRAME_PRE       0xFE
#define FRAME_END       0xFD

#define CODE_OK         0xFB
#define CODE_NG         0xFA

#define TRAN_FREQ       0x00
#define READ_FREQ       0x03
#define READ_MODE       0x04
#define WRITE_FREQ      0x05
#define WRITE_MODE      0x06

#define SELECT_VFO      0x07

#define SELECT_VFO_A    0x00
#define SELECT_VFO_B    0x01

#define CONT_PTT        0x1C

static int      fd;

static uint8_t  frame[256];

static uint16_t frame_get() {
    uint16_t    len = 0;
    uint8_t     c;
    
    memset(frame, 0, sizeof(frame));
    
    while (true) {
        int res = read(fd, &c, 1);
        
        if (res > 0) {
            frame[len++] = c;
        
            if (c == FRAME_END) {
                return len;
            }
        
            if (len >= sizeof(frame)) {
                return 0;
            }
        } else {
            usleep(10000);
        }
    }
    
    return 0;
}

static void send_frame(uint16_t len) {
    frame[len - 1] = FRAME_END;
    write(fd, frame, len);
}

static void send_code(uint8_t code) {
    frame[4] = code;
    send_frame(6);
}

static void frame_parse(uint16_t len) {
    if (frame[0] != FRAME_PRE && frame[1] != FRAME_PRE) {
        LV_LOG_ERROR("Incorrect frame");
        return;
    }

    switch (frame[4]) {
        case TRAN_FREQ:
        case READ_FREQ:
            to_bcd(&frame[5], params_band.vfo_x[params_band.vfo].freq, 10);
            send_frame(11);
            break;

        case READ_MODE:
            frame[5] = 0;
            frame[6] = 0;
            send_frame(8);
            break;

        case WRITE_FREQ:
            send_code(CODE_OK);
            break;
            
        case CONT_PTT:
            if (frame[5] == 0x00) {
                if (frame[6] == FRAME_END) {
                    frame[6] = (radio_get_state() == RADIO_RX) ? 0 : 1;
                    send_frame(8);
                } else {
                    switch (frame[6]) {
                        case 0:
                            radio_set_ptt(false);
                            frame[6] = CODE_OK;
                            send_frame(8);
                            break;
                            
                        case 1:
                            radio_set_ptt(true);
                            frame[6] = CODE_OK;
                            send_frame(8);
                            break;
                    }
                }
            }
            break;

        case SELECT_VFO:
            switch (frame[5]) {
                case SELECT_VFO_A:
                case SELECT_VFO_B:
                    send_code(CODE_OK);
                    break;
                    
                default:
                    send_code(CODE_NG);
                    break;
            }
            break;
            
        default:
            LV_LOG_WARN("Unsuported %02X:%02X (Len %i)", frame[4], frame[5], len);
            send_code(CODE_NG);
            break;
    }
}

static void * cat_thread(void *arg) {
    while (true) {
        uint16_t len = frame_get();
        
        if (len >= 0) {
            frame_parse(len);
        }
    }
}

void cat_init() {
    /* UART */

    x6100_gpio_set(x6100_pin_usb, 1);  /* USB -> CAT */

    fd = open("/dev/ttyS2", O_RDWR | O_NONBLOCK | O_NOCTTY);
    
    if (fd > 0) {
        struct termios attr;

        tcgetattr(fd, &attr);

        cfsetispeed(&attr, B19200);
        cfsetospeed(&attr, B19200);
        cfmakeraw(&attr);
        
        if (tcsetattr(fd, 0, &attr) < 0) {
            close(fd);
            LV_LOG_ERROR("UART set speed");
        }
    } else {
        LV_LOG_ERROR("UART open");
    }

    /* * */

    pthread_t thread;

    pthread_create(&thread, NULL, cat_thread, NULL);
    pthread_detach(thread);
}
