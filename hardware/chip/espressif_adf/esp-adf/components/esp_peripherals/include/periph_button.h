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

#ifndef _BUTTON_DEV_H_
#define _BUTTON_DEV_H_

#include "sys/queue.h"
#include "audio_error.h"
#include "audio_common.h"
#include "esp_peripherals.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The Button peripheral configuration
 */
typedef struct {
    uint64_t gpio_mask;     /*!< GPIO Mask using for this Button peripheral, it is BIT(GPIO_NUM), ex: GPIO_SEL_36 | GPIO_SEL_36 */
    int long_press_time_ms; /*!< Long press duration in milliseconds, default is 2000ms */
} periph_button_cfg_t;

/**
 * @brief      Peripheral button event id
 */
typedef enum {
    PERIPH_BUTTON_UNCHANGE = 0, /*!< No event */
    PERIPH_BUTTON_PRESSED,      /*!< When button is pressed */
    PERIPH_BUTTON_RELEASE,      /*!< When button is released */
    PERIPH_BUTTON_LONG_PRESSED, /*!< When button is pressed and kept for more than `long_press_time_ms` */
    PERIPH_BUTTON_LONG_RELEASE, /*!< When button is released and event PERIPH_BUTTON_LONG_PRESSED happened */
} periph_button_event_id_t;

/**
 * @brief      Create the button peripheral handle for esp_peripherals.
 *
 * @note       The handle was created by this function automatically destroy when `esp_periph_destroy` is called
 *
 * @param      but_cfg  The but configuration
 *
 * @return     The esp peripheral handle
 */
esp_periph_handle_t periph_button_init(periph_button_cfg_t* but_cfg);

#ifdef __cplusplus
}
#endif

#endif
