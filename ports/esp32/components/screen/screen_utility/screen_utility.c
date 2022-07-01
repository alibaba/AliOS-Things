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

#include "stdint.h"
#include "esp_log.h"
#include "screen_utility.h"

static const char *TAG = "screen utility";

void scr_utility_apply_offset(const scr_handle_t *lcd_handle, uint16_t res_hor, uint16_t res_ver, uint16_t *x0, uint16_t *y0, uint16_t *x1, uint16_t *y1)
{
    scr_dir_t dir = lcd_handle->dir;
    if (SCR_DIR_MAX < dir) {
        dir >>= 5;
    }
    uint16_t xoffset=0, yoffset=0;
    switch (dir) {
        case SCR_DIR_LRTB:
            xoffset = lcd_handle->offset_hor;
            yoffset = lcd_handle->offset_ver;
            break;
        case SCR_DIR_LRBT:
            xoffset = lcd_handle->offset_hor;
            yoffset = res_ver - lcd_handle->offset_ver - lcd_handle->original_height;
            break;
        case SCR_DIR_RLTB:
            xoffset += res_hor - lcd_handle->offset_hor - lcd_handle->original_width;
            yoffset += lcd_handle->offset_ver;
            break;
        case SCR_DIR_RLBT:
            xoffset = res_hor - lcd_handle->offset_hor - lcd_handle->original_width;
            yoffset = res_ver - lcd_handle->offset_ver - lcd_handle->original_height;
            break;

        case SCR_DIR_TBLR:
            xoffset = lcd_handle->offset_ver;
            yoffset = lcd_handle->offset_hor;
            break;
        case SCR_DIR_BTLR:
            yoffset = lcd_handle->offset_hor;
            xoffset = res_ver - lcd_handle->offset_ver - lcd_handle->original_height;
            break;
        case SCR_DIR_TBRL:
            yoffset += res_hor - lcd_handle->offset_hor - lcd_handle->original_width;
            xoffset += lcd_handle->offset_ver;
            break;
        case SCR_DIR_BTRL:
            yoffset = res_hor - lcd_handle->offset_hor - lcd_handle->original_width;
            xoffset = res_ver - lcd_handle->offset_ver - lcd_handle->original_height;
            break;
        default: break;
    }
    ESP_LOGD(TAG, "dir=%d, offset=(%d, %d)", dir, xoffset, yoffset);
    *x0 += xoffset;
    *x1 += xoffset;
    *y0 += yoffset;
    *y1 += yoffset;
}

