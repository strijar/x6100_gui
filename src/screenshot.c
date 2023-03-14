/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#include <stdlib.h>
#include <stdint.h>
#include <png.h>
#include <pthread.h>

#include "lvgl/lvgl.h"

#include "screenshot.h"
#include "util.h"
#include "msg.h"

static char         file_str[64];
static char         time_str[64];
static lv_img_dsc_t snapshot;
static uint8_t      *rows[480];
static uint8_t      *buf;

static void * screenshot_thread(void *arg) {
    get_time_str(time_str, sizeof(time_str));
    
    strcpy(file_str, "/mnt/");
    strcat(file_str, time_str);
    strcat(file_str, ".png");

    FILE *fp = fopen(file_str, "wb");
    
    if (!fp) {
        msg_set_text_fmt("Error write file");
        goto done;
    }
    
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    
    if (!png_ptr) {
        LV_LOG_ERROR("Create write struct");
        goto close_file;
    }

    png_infop png_info = png_create_info_struct(png_ptr);
    
    if (!png_info) {
        LV_LOG_ERROR("Create info struct");
        goto destroy_write;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        LV_LOG_ERROR("Set jump");
        goto destroy_write;
    }

    png_init_io(png_ptr, fp);
    
    png_set_IHDR(
        png_ptr, png_info, 
        800, 480,
        8, PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);
    
    for (uint16_t y = 0; y < 480; y++) {
        rows[y] = (uint8_t *) malloc(800 * 3);
        
        for (uint16_t x = 0; x < 800; x++) {
            uint32_t    to = x * 3;
            uint32_t    from = (y * 800 + x) * 4 + 2;
        
            rows[y][to++] = buf[from--];
            rows[y][to++] = buf[from--];
            rows[y][to++] = buf[from--];
        }
    }
    
    png_set_rows(png_ptr, png_info, rows);
    png_write_png(png_ptr, png_info, PNG_TRANSFORM_IDENTITY, NULL);
    png_write_end(png_ptr, png_info);
    
    for (uint16_t y = 0; y < 480; y++) {
        free(rows[y]);
    }
    
    msg_set_text_fmt("Saved %s", time_str);

destroy_write:
    png_destroy_write_struct(&png_ptr, NULL);
close_file:
    fclose(fp);
done:
    free(buf);
}

void screenshot_take() {
    uint32_t        buf_size = lv_snapshot_buf_size_needed(lv_scr_act(), LV_IMG_CF_TRUE_COLOR_ALPHA);
    
    buf = (uint8_t *) malloc(buf_size);
    
    lv_snapshot_take_to_buf(lv_scr_act(), LV_IMG_CF_TRUE_COLOR_ALPHA, &snapshot, buf, buf_size);

    pthread_t thread;

    pthread_create(&thread, NULL, screenshot_thread, NULL);
    pthread_detach(thread);
}
