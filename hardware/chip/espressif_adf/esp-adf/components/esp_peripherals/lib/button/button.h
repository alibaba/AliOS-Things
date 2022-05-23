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

#ifndef _ESP_BUTTON_
#define _ESP_BUTTON_

#include "driver/gpio.h"
#include "audio_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      { item_description }
 */
typedef enum {
    BTN_UNCHANGE = 0,
    BTN_PRESSED,
    BTN_RELEASE,
    BTN_LONG_PRESS,
    BTN_LONG_RELEASE,
} button_status_t;

/**
 * @brief      { item_description }
 */
typedef struct {
    uint64_t press_mask;
    uint64_t release_mask;
    uint64_t long_press_mask;
    uint64_t long_release_mask;
} button_result_t;

typedef struct esp_button *esp_button_handle_t;
typedef void (*gpio_intr_handler)(void *);

/**
 * @brief      { item_description }
 */
typedef struct {
    int long_press_time_ms;
    uint64_t gpio_mask;
    gpio_intr_handler button_intr_handler;
    void *intr_context;
} button_config_t;

#define DEFAULT_LONG_PRESS_TIME_MS (2*1000)

/**
 * @brief      { function_description }
 *
 * @param      config  The configuration
 *
 * @return     { description_of_the_return_value }
 */
esp_button_handle_t button_init(button_config_t *config);

/**
 * @brief      { function_description }
 *
 * @param[in]  button  The button
 * @param      result  The result
 *
 * @return     { description_of_the_return_value }
 */
bool button_read(esp_button_handle_t button, button_result_t *result);

/**
 * @brief      { function_description }
 *
 * @param[in]  button  The button
 *
 * @return     { description_of_the_return_value }
 */
esp_err_t button_destroy(esp_button_handle_t button);

#ifdef __cplusplus
}
#endif

#endif
