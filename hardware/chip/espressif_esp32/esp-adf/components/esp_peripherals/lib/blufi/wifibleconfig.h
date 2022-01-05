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

#ifndef _WIFI_BLECONFIG_H_
#define _WIFI_BLECONFIG_H_

#include "esp_err.h"
#include "esp_peripherals.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      Start Wi-Fi BLE config
 *
 * @param[in]  periph   The peripheral handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t ble_config_start(esp_periph_handle_t periph);

/**
 * @brief      Stop Wi-Fi BLE config.
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t ble_config_stop(void);

#ifdef __cplusplus
}
#endif

#endif
