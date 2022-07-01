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
#ifndef __IOT_LCD_SSD1322_H__
#define __IOT_LCD_SSD1322_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "screen_driver.h"


/**
 * @brief device initialization
 *
 * @param lcd_conf configuration struct of ssd1306
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Fail
 */
esp_err_t lcd_ssd1322_init(const scr_controller_config_t *lcd_conf);

/**
 * @brief   Deinitial screen
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Fail
 */
esp_err_t lcd_ssd1322_deinit(void);

/**
 * @brief Get screen information
 * 
 * @param info Pointer to a scr_info_t structure.
 * 
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Fail
 */
esp_err_t lcd_ssd1322_get_info(scr_info_t *info);

/**
 * @brief Set screen direction of rotation
 * 
 * @note  Only the first four directions defined by scr_dir_t are supported
 *
 * @param dir Pointer to a scr_dir_t structure.
 * 
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Fail
 */
esp_err_t lcd_ssd1322_set_rotate(scr_dir_t dir);

/**
 * @brief Set screen window
 *
 * @param x0 Starting point in X direction
 * @param y0 Starting point in Y direction
 * @param x1 End point in X direction
 * @param y1 End point in Y direction
 * 
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL Failed
 */
esp_err_t lcd_ssd1322_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

/**
 * @brief Draw one pixel in screen with color
 * 
 * @param x X co-ordinate of set orientation
 * @param y Y co-ordinate of set orientation
 * @param color New color of the pixel
 * 
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Fail
 */
esp_err_t lcd_ssd1322_draw_pixel(uint16_t x, uint16_t y, uint16_t color);

/**
 * @brief Fill the pixels on LCD screen with bitmap
 * 
 * @param x Starting point in X direction
 * @param y Starting point in Y direction
 * @param w width of image in bitmap array
 * @param h height of image in bitmap array
 * @param bitmap pointer to bitmap array
 * 
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Fail
 */
esp_err_t lcd_ssd1322_draw_bitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *bitmap);

/**
 * @brief Set the contrast of screen
 * 
 * @param contrast Contrast to set
 * 
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Fail
 */
esp_err_t lcd_ssd1322_set_contrast(uint8_t contrast);

/**
 * @brief Set screen color invert
 * 
 * @param is_invert true: color invert on, false: color invert off
 * 
 * @return 
 *      - ESP_OK on success
 *      - ESP_FAIL Failed
 */
esp_err_t lcd_ssd1322_set_invert(uint8_t is_invert);

#ifdef __cplusplus
}
#endif

#endif
