/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
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

#ifndef _ESP_TOUCH_PAD_H_
#define _ESP_TOUCH_PAD_H_

#include "audio_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      { item_description }
 */
typedef enum {
    TOUCH_UNCHANGE = 0,
    TOUCH_TAP,
    TOUCH_RELEASE,
    TOUCH_LONG_TAP,
    TOUCH_LONG_RELEASE,
} touch_status_t;

/**
 * @brief      { item_description }
 */
typedef struct {
    int tap_mask;
    int release_mask;
    int long_tap_mask;
    int long_release_mask;
} touch_result_t;

typedef struct esp_touch *esp_touch_handle_t;
typedef void (*touch_intr_handler)(void *);

#define DEFAULT_LONG_TAP_TIME_MS        (2*1000)
#define DEFAULT_TOUCH_THRESHOLD_PERCENT (70)

/**
 * @brief      { item_description }
 */
typedef struct {
    int long_tap_time_ms;
    int touch_mask;
    int tap_threshold_percent;
    touch_intr_handler touch_intr_handler;
    void *intr_context;
} touch_config_t;

/**
 * @brief      { function_description }
 *
 * @param      config  The configuration
 *
 * @return     { description_of_the_return_value }
 */
esp_touch_handle_t esp_touch_init(touch_config_t *config);

/**
 * @brief      { function_description }
 *
 * @param[in]  touch   The touch
 * @param      result  The result
 *
 * @return     { description_of_the_return_value }
 */
bool esp_touch_read(esp_touch_handle_t touch, touch_result_t *result);

/**
 * @brief      { function_description }
 *
 * @param[in]  touch  The touch
 *
 * @return     { description_of_the_return_value }
 */
esp_err_t esp_touch_destroy(esp_touch_handle_t touch);

#ifdef __cplusplus
}
#endif

#endif
