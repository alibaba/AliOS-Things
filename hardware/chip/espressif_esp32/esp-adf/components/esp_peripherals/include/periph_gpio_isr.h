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

#ifndef _PERIPH_GPIO_ISR_H_
#define _PERIPH_GPIO_ISR_H_

#include "esp_peripherals.h"
#include "driver/gpio.h"
#include "gpio_isr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @breif Set the gpio number and gpio interruption type
 */
typedef struct {
    int gpio_num;           /*!< gpio number */
    gpio_int_type_t type;   /*!< interruption type */
} gpio_isr_info_t;

/**
 * @brief The configuration of gpio isr
 */
typedef struct {
    int info_size;                    /*!< number of gpio to be register */
    gpio_isr_info_t *gpio_isr_info;   /*!< an array of gpio's infomation */
}periph_gpio_isr_cfg_t;

/**
 * @brief     Create the gpio's interrupt service routines handle for esp_peripherals
 *
 * @param     isr_config  The gpio isr configuration
 *
 * @return    The esp peripheral handle
 */
esp_periph_handle_t periph_gpio_isr_init(periph_gpio_isr_cfg_t *isr_config);

/**
 * @breif      Add a gpio to isr
 *
 * @param      gpio_info  The gpio interruption type and gpio number
 *
 * @return
 *      - ESP_OK   success
 *      - ESP_FAIL fail
 */
esp_err_t periph_gpio_isr_add(gpio_isr_info_t *gpio_info);

/**
 * @brief       Unregister a gpio from isr
 *
 * @param       The number of gpio to be unregistered
 *
 *@return
 *     - ESP_OK    success
 *     - ESP_FAIL  failed
 */
esp_err_t periph_gpio_isr_delete(int gpio_num);

#ifdef __cplusplus
}
#endif

#endif
