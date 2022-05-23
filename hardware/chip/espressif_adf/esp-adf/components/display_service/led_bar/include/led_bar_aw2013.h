/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _LED_BAR_AW2013_H_
#define _LED_BAR_AW2013_H_

#include "display_service.h"
#include "esp_peripherals.h"

#ifdef __cplusplus
extern "C" {
#endif

void aw2013_led_bar_task(void *parameters); // Defination a pointer to the task entry function that will be referred by users code

/**
 * @brief Initialize led bar instance
 *
 * @return
 *     - NULL   Error
 *     - others Success
 */
esp_periph_handle_t led_bar_aw2013_init(void);

/**
 * @brief      Set led bar display pattern.
 *
 * @param      handle   led bar instance
 * @param      pat      display pattern
 * @param      value    value of pattern
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t led_bar_aw2013_pattern(void *handle, int pat, int value);

/**
 * @brief      Set blinking period and times
 *
 * @param      handle  led bar instance
 * @param      time    times of blink
 * @param      period  period of blink
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t led_bar_aw2013_set_blink_time(void *handle, uint8_t time, int period);

/**
 * @brief      Destroy esp_periph_handle_t instance
 *
 * @param      handle  led bar instance
 *
 */
void led_bar_aw2013_deinit(esp_periph_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif
