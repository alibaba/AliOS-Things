// Copyright 2020 Espressif Systems (Shanghai) Co. Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _SCREEN_UTILITY_H_
#define _SCREEN_UTILITY_H_

#include "screen_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Declare screen parameters
 * 
 */
typedef struct {
    scr_interface_driver_t *interface_drv;
    uint16_t original_width;
    uint16_t original_height;
    uint16_t width;
    uint16_t height;
    uint16_t offset_hor;
    uint16_t offset_ver;
    scr_dir_t dir;
} scr_handle_t;

void scr_utility_apply_offset(const scr_handle_t *lcd_handle, uint16_t res_hor, uint16_t res_ver, uint16_t *x0, uint16_t *y0, uint16_t *x1, uint16_t *y1);

#ifdef __cplusplus
}
#endif

#endif
