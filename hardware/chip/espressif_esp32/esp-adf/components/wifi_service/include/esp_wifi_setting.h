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

#ifndef _ESP_WIFI_SETTING_H_
#define _ESP_WIFI_SETTING_H_

#include "audio_error.h"
#include "esp_wifi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_wifi_setting *esp_wifi_setting_handle_t;
typedef esp_err_t (* wifi_setting_func)(esp_wifi_setting_handle_t handle);
typedef esp_err_t (* wifi_setting_teardown_func)(esp_wifi_setting_handle_t handle, wifi_config_t *info);

/**
 * brief        Create wifi setting handle instance
 *
 * @param tag   Tag of the wifi setting handle
 *
 * @return
 *     - NULL, Fail
 *     - Others, Success
 */
esp_wifi_setting_handle_t esp_wifi_setting_create(const char *tag);

/**
 * brief        Destroy wifi setting handle instance
 *
 * @param handle   The wifi setting handle instance
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t esp_wifi_setting_destroy(esp_wifi_setting_handle_t handle);

/**
 * @brief      Register the wifi setting execute functions
 *
 * @param  handle       The wifi setting handle instance
 * @param  start        The start wifi setting
 * @param  stop         The stop
 * @param  teardown     The destroy
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t esp_wifi_setting_register_function(esp_wifi_setting_handle_t handle,
                                        wifi_setting_func start,
                                        wifi_setting_func stop,
                                        wifi_setting_teardown_func teardown
                                       );

/**
 * @brief      Register the notify execute handle
 *
 * @param      handle       The peripheral handle
 * @param      on_handle    The notify execute handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t esp_wifi_setting_regitster_notify_handle(esp_wifi_setting_handle_t handle, void *on_handle);

/**
 * @brief      Call this to notify the `wifi_config_t` to `on_handle`
 *
 * @param      handle     The wifi setting handle instance
 * @param      info       The `wifi_config_t`
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t esp_wifi_setting_info_notify(esp_wifi_setting_handle_t handle, wifi_config_t *info);

/**
 * @brief      Set the user data
 *
 * @note       Make sure the `data` lifetime is sufficient,
 *             this function does not copy all data, it only stores the data pointer
 *
 * @param[in]  handle  The wifi setting handle instance
 * @param      data    The user data
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t esp_wifi_setting_set_data(esp_wifi_setting_handle_t handle, void *data);

/**
 * @brief      Get the user data stored on `handle`
 *
 * @param[in]  handle  The wifi setting handle instance
 *
 * @return     user data pointer
 */
void *esp_wifi_setting_get_data(esp_wifi_setting_handle_t handle);

/**
 * @brief      Call this to execute `start` function of wifi setting instance
 *
 * @param      handle     The wifi setting handle instance
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t esp_wifi_setting_start(esp_wifi_setting_handle_t handle);

/**
 * @brief      Call this to execute `stop` function of wifi setting instance
 *
 * @param      handle     The wifi setting handle instance
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t esp_wifi_setting_stop(esp_wifi_setting_handle_t handle);

/**
 * @brief      Call this to execute `teardown` function of wifi setting instance
 *
 * @param      handle     The wifi setting handle instance
 * @param      info       The `wifi_config_t`
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t esp_wifi_setting_teardown(esp_wifi_setting_handle_t handle, wifi_config_t *info);

#ifdef __cplusplus
}
#endif

#endif
