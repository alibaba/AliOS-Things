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

#ifndef _WIFI_SSID_MANAGER_H_
#define _WIFI_SSID_MANAGER_H_

#include "audio_error.h"
#include "audio_mem.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct wifi_ssid_manager *wifi_ssid_manager_handle_t;

/*
 * @brief Create a wifi ssid manager instance
 *
 * @param max_ssid_num  Maximum number of ssid to be saved in flash
 *
 * @return
 *     - NULL, Fail
 *     - Others, Success
 */
wifi_ssid_manager_handle_t wifi_ssid_manager_create(uint8_t max_ssid_num);

/*
 * @brief Get the latest configuration that set by users. To put it another way, if users call function
 *        wifi_ssid_manager_save(), we get a latest configuration in flash.
 *
 * @param handle       The instance of wifi ssid manager
 * @param config[out]  Point to a address to save the configuration
 *
 * @return
 *     - ESP_FAIL, Fail
 *     - ESP_OK, Success
 */
esp_err_t wifi_ssid_manager_get_latest_config(wifi_ssid_manager_handle_t handle, wifi_config_t *config);

/*
 * @brief Save wifi ssid and password to flash
 *
 * @param handle       The instance of wifi ssid manager
 * @param ssid         wifi ssid to be saved
 * @param pwd          wifi password to be saved
 *
 * @return
 *     - ESP_FAIL, Fail
 *     - ESP_OK, Success
 */
esp_err_t wifi_ssid_manager_save(wifi_ssid_manager_handle_t handle, const char *ssid, const char *pwd);

/*
 * @brief Get the suitable wifi information (The judgment is based on its rssi)
 *
 * @param handle       The instance of wifi ssid manager
 * @param config[out]  Point to a address to save the configuration
 *
 * @return
 *     - ESP_FAIL, Fail
 *     - ESP_OK, Success
 */
esp_err_t wifi_ssid_manager_get_best_config(wifi_ssid_manager_handle_t handle, wifi_config_t *config);

/*
 * @brief Get the number of ssids saved in the wifi ssid manager
 *
 * @param handle       The instance of wifi ssid manager
 *
 * @return
 *     - ESP_FAIL, Fail
 *     - others, Success
 */
int wifi_ssid_manager_get_ssid_num(wifi_ssid_manager_handle_t handle);

/*
 * @brief Show the ssids and passwords that are saved in the flash
 *
 * @param handle       The instance of wifi ssid manager
 *
 * @return
 *     - ESP_FAIL, Fail
 *     - ESP_OK, Success
 */
esp_err_t wifi_ssid_manager_list_show(wifi_ssid_manager_handle_t handle);

/*
 * @brief Erase all the ssids saved in the flash
 *
 * @param handle       The instance of wifi ssid manager
 *
 * @return
 *     - ESP_FAIL, Fail
 *     - ESP_OK, Success
 */
esp_err_t wifi_ssid_manager_erase_all(wifi_ssid_manager_handle_t handle);

/*
 * @brief Destroy the wifi ssid manager
 *
 * @param handle       The instance of wifi ssid manager
 *
 * @return
 *     - ESP_FAIL, Fail
 *     - ESP_OK, Success
 */
esp_err_t wifi_ssid_manager_destroy(wifi_ssid_manager_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif
