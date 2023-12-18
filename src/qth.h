/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Xiegu X6100 LVGL GUI
 *
 *  Copyright (c) 2022-2023 Belousov Oleg aka R1CBU
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

void qth_set(const char *qth);
void qth_update(const char *qth);

bool grid_check(const char *grid);
void grid_pos(const char *grid, double *lat, double *lon);
const char *pos_grid(double lat, double lon);
int32_t grid_dist(const char *grid);
