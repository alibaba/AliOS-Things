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

#ifndef __BLUFI_CONFIG_H_
#define __BLUFI_CONFIG_H_

#include "esp_wifi_setting.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      Create blufi setting handle instance
 *
 * @param[in]  info   A pointer to void
 *
 * @return
 *     - NULL, Fail
 *     - Others, Success
 */
esp_wifi_setting_handle_t blufi_config_create(void *info);

/**
 * @brief      Set flag to judge whether the station has connected to the AP
 *
 * @param[in]  handle   Wifi setting handle
 * @param[in]  flag     bool type of station connection state
 *
 * @return
 *     - NULL, Fail
 *     - Others, Success
 */
esp_err_t blufi_set_sta_connected_flag(esp_wifi_setting_handle_t handle, bool flag);

/**
 * @brief      Set customized data to be sent after configurate wifi successfully.
 *
 * @param[in]  handle   Wifi setting handle
 * @param[in]  data     Customized data
 * @param[in]  data_len Customized data length

 * @return
 *     - ESP_FAIL, Fail
 *     - ESP_OK, Success
 */
esp_err_t blufi_set_customized_data(esp_wifi_setting_handle_t handle, char *data, int data_len);

/**
 * @brief      Send customized data that be set before
 *
 * @param[in]  handle   Wifi setting handle

 * @return
 *     - ESP_FAIL, Fail
 *     - ESP_OK, Success
 */
esp_err_t blufi_send_customized_data(esp_wifi_setting_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif
