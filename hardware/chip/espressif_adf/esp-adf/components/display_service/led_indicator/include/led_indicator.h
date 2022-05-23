/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
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

#ifndef _LED_INDICATOR_H_
#define _LED_INDICATOR_H_

#include "display_service.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct led_indicator_impl *led_indicator_handle_t;

/**
 * @brief      Initialize led_indicator_handle_t instance
 *
 * @param      num  led gpio number
 *
 * @return
 *     - NULL, Fail
 *     - Others, Success
 */
led_indicator_handle_t led_indicator_init(gpio_num_t num);

/**
 * @brief      Set led indicator display pattern.
 *
 * @param      handle   led indicator instance
 * @param      pat      display pattern
 * @param      value    value of pattern
 *
* @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t led_indicator_pattern(void *handle, int pat, int value);

/**
 * @brief      Destroy led_indicator_handle_t instance
 *
 * @param      handle  led indicator instance
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
void led_indicator_deinit(led_indicator_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif

