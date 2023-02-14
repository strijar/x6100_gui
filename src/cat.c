/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

/* 
 * X6100 protocol implementation (Mfg 3087)
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
#include "events.h"
#include "waterfall.h"
#include "spectrum.h"

#define FRAME_PRE       0xFE
#define FRAME_END       0xFD

#define CODE_OK         0xFB
#define CODE_NG         0xFA

#define C_SND_FREQ      0x00    /* Send frequency data  transceive mode does not ack*/
#define C_SND_MODE      0x01    /* Send mode data, Sc  for transceive mode does not ack */
#define C_RD_BAND       0x02    /* Read band edge frequencies */
#define C_RD_FREQ       0x03    /* Read display frequency */
#define C_RD_MODE       0x04    /* Read display mode */
#define C_SET_FREQ      0x05    /* Set frequency data(1) */
#define C_SET_MODE      0x06    /* Set mode data, Sc */
#define C_SET_VFO       0x07    /* Set VFO */
#define C_SET_MEM       0x08    /* Set channel, Sc(2) */
#define C_WR_MEM        0x09    /* Write memory */
#define C_MEM2VFO       0x0a    /* Memory to VFO */
#define C_CLR_MEM       0x0b    /* Memory clear */
#define C_RD_OFFS       0x0c    /* Read duplex offset frequency; default changes with HF/6M/2M */
#define C_SET_OFFS      0x0d    /* Set duplex offset frequency */
#define C_CTL_SCAN      0x0e    /* Control scan, Sc */
#define C_CTL_SPLT      0x0f    /* Control split, and duplex mode Sc */
#define C_SET_TS        0x10    /* Set tuning step, Sc */
#define C_CTL_ATT       0x11    /* Set/get attenuator, Sc */
#define C_CTL_ANT       0x12    /* Set/get antenna, Sc */
#define C_CTL_ANN       0x13    /* Control announce (speech synth.), Sc */
#define C_CTL_LVL       0x14    /* Set AF/RF/squelch, Sc */
#define C_RD_SQSM       0x15    /* Read squelch condition/S-meter level, Sc */
#define C_CTL_FUNC      0x16    /* Function settings (AGC,NB,etc.), Sc */
#define C_SND_CW        0x17    /* Send CW message */
#define C_SET_PWR       0x18    /* Set Power ON/OFF, Sc */
#define C_RD_TRXID      0x19    /* Read transceiver ID code */
#define C_CTL_MEM       0x1a    /* Misc memory/bank/rig control functions, Sc */
#define C_SET_TONE      0x1b    /* Set tone frequency */
#define C_CTL_PTT       0x1c    /* Control Transmit On/Off, Sc */
#define C_CTL_EDGE      0x1e    /* Band edges */
#define C_CTL_DVT       0x1f    /* Digital modes calsigns & messages */
#define C_CTL_DIG       0x20    /* Digital modes settings & status */
#define C_CTL_RIT       0x21    /* RIT/XIT control */
#define C_CTL_DSD       0x22    /* D-STAR Data */
#define C_SEND_SEL_FREQ 0x25    /* Send/Recv sel/unsel VFO frequency */
#define C_SEND_SEL_MODE 0x26
#define C_CTL_SCP       0x27    /* Scope control & data */
#define C_SND_VOICE     0x28    /* Transmit Voice Memory Contents */
#define C_CTL_MTEXT     0x70    /* Microtelecom Extension */
#define C_CTL_MISC      0x7f    /* Miscellaneous control, Sc */

#define S_VFOA          0x00    /* Set to VFO A */
#define S_VFOB          0x01    /* Set to VFO B */
#define S_BTOA          0xa0    /* VFO A=B */
#define S_XCHNG         0xb0    /* Switch VFO A and B */
#define S_SUBTOMAIN     0xb1    /* MAIN = SUB */
#define S_DUAL_OFF      0xc0    /* Dual watch off */
#define S_DUAL_ON       0xc1    /* Dual watch on */
#define S_DUAL          0xc2    /* Dual watch (0 = off, 1 = on) */
#define S_MAIN          0xd0    /* Select MAIN band */
#define S_SUB           0xd1    /* Select SUB band */
#define S_SUB_SEL       0xd2    /* Read/Set Main/Sub selection */
#define S_FRONTWIN      0xe0    /* Select front window */

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

static void set_freq(uint64_t freq) {
    params.freq_band = bands_find(freq);
    
    if (params.freq_band) {
        bands_activate(params.freq_band, NULL);
    }

    radio_set_freq(freq);
    event_send(lv_scr_act(), EVENT_SCREEN_UPDATE, NULL);
}

static void frame_parse(uint16_t len) {
    if (frame[0] != FRAME_PRE && frame[1] != FRAME_PRE) {
        LV_LOG_ERROR("Incorrect frame");
        return;
    }
    
#if 0
    LV_LOG_WARN("Cmd %02X:%02X (Len %i)", frame[4], frame[5], len);
#endif

    switch (frame[4]) {
        case C_RD_FREQ:
            to_bcd(&frame[5], params_band.vfo_x[params_band.vfo].freq, 10);
            send_frame(11);
            break;

        case C_RD_MODE:
            /* TODO */
            frame[5] = 0;
            frame[6] = 0;
            send_frame(8);
            break;

        case C_SET_FREQ:
            set_freq(from_bcd(&frame[5], 10));
            send_code(CODE_OK);
            break;
            
        case C_SET_MODE:
            /* TODO */
            send_code(CODE_OK);
            break;
            
        case C_CTL_PTT:
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

        case C_SET_VFO:
            switch (frame[5]) {
                case S_VFOA:
                    radio_set_vfo(X6100_VFO_A);
                    event_send(lv_scr_act(), EVENT_SCREEN_UPDATE, NULL);
                    send_code(CODE_OK);
                    break;

                case S_VFOB:
                    radio_set_vfo(X6100_VFO_B);
                    event_send(lv_scr_act(), EVENT_SCREEN_UPDATE, NULL);
                    send_code(CODE_OK);
                    break;
                    
                default:
                    send_code(CODE_NG);
                    break;
            }
            break;

        case C_SEND_SEL_FREQ:
            if (frame[6] == FRAME_END) {
                uint64_t freq;
                
                if (frame[5] == 0x00) {
                    freq = params_band.vfo_x[X6100_VFO_A].freq;
                } else {
                    freq = params_band.vfo_x[X6100_VFO_B].freq;
                }
                to_bcd(&frame[6], freq, 10);
                send_frame(12);
            } else {
                uint64_t freq = from_bcd(&frame[6], 10);
                
                if (frame[5] == 0x00) {
                    params_band.vfo_x[X6100_VFO_A].freq = freq;
                    
                    if (params_band.vfo == X6100_VFO_A) {
                        set_freq(freq);
                    }
                } else {
                    params_band.vfo_x[X6100_VFO_B].freq = freq;
                    
                    if (params_band.vfo == X6100_VFO_B) {
                        set_freq(freq);
                    }
                }
                send_code(CODE_OK);
            }
            break;

        case C_SEND_SEL_MODE:
            if (frame[6] == FRAME_END) {
                /* TODO */
                frame[6] = 0;
                frame[7] = 0;
                frame[8] = CODE_OK;
                send_frame(10);
            } else {
                x6100_mode_t mode;
                
                switch (frame[6]) {
                    case 0:
                        switch (frame[7]) {
                            case 0:
                                mode = x6100_mode_lsb;
                                break;
                                
                            case 1:
                                mode = x6100_mode_lsb_dig;
                                break;
                        }
                        break;
                        
                    case 1:
                        switch (frame[7]) {
                            case 0:
                                mode = x6100_mode_usb;
                                break;
                                
                            case 1:
                                mode = x6100_mode_usb_dig;
                                break;
                        }
                        break;
                }
                
                if (frame[5] == 0x00) {
                    radio_set_mode(X6100_VFO_A, mode);
                } else {
                    radio_set_mode(X6100_VFO_B, mode);
                }
                
                send_code(CODE_OK);
                event_send(lv_scr_act(), EVENT_SCREEN_UPDATE, NULL);
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
