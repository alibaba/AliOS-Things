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

#ifndef _CONNECTIVITY_SERVICE_H_
#define _CONNECTIVITY_SERVICE_H_

#include "periph_service.h"
#include "esp_wifi_setting.h"
#include "esp_wifi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief WiFi STA service status
 */
typedef enum {
    WIFI_SERV_EVENT_UNKNOWN,
    WIFI_SERV_EVENT_CONNECTING,
    WIFI_SERV_EVENT_CONNECTED,
    WIFI_SERV_EVENT_DISCONNECTED,
    WIFI_SERV_EVENT_SETTING_TIMEOUT,
    WIFI_SERV_EVENT_SETTING_FAILED,
    WIFI_SERV_EVENT_SETTING_FINISHED,
} wifi_service_event_t;

/**
 * @brief WiFi STA disconnection reasons
 */
typedef enum {
    WIFI_SERV_STA_UNKNOWN,
    WIFI_SERV_STA_COM_ERROR,
    WIFI_SERV_STA_AUTH_ERROR,
    WIFI_SERV_STA_AP_NOT_FOUND,
    WIFI_SERV_STA_BY_USER,
    WIFI_SERV_STA_SET_INFO,
} wifi_service_disconnect_reason_t;

/**
 * @brief WiFi service configurations
 */
typedef struct {
    int                         task_stack;             /*!< >0 Service task stack; =0 with out task created */
    int                         task_prio;              /*!< Service task priority (based on freeRTOS priority) */
    int                         task_core;              /*!< Service task running in core (0 or 1) */
    bool                        extern_stack;           /*!< Task stack allocate on the extern ram */
    periph_service_cb           evt_cb;                 /*!< Service callback function */
    void                        *cb_ctx;                /*!< Callback context */
    char                        *user_data;             /*!< User data */
    int                         setting_timeout_s;      /*!< Timeout of setting WiFi */
    int                         max_retry_time;         /*!< Maximum times of reconnection */
    int                         max_prov_retry_time;    /*!< Maximum times of reconnection after wifi provision*/
    uint8_t                     max_ssid_num;           /*!< Maximum ssid that can be stored */
} wifi_service_config_t;

#define WIFI_SERVICE_DEFAULT_CONFIG() { \
    .task_stack = 3*1024, \
    .task_prio = 6, \
    .task_core = 0, \
    .extern_stack = true, \
    .evt_cb = NULL, \
    .cb_ctx = NULL, \
    .user_data = NULL, \
    .setting_timeout_s = 60, \
    .max_retry_time = 5,\
    .max_prov_retry_time = 3, \
    .max_ssid_num = 5, \
}

/*
 * @brief Create the WiFi service instance
 *
 * @param config  Configuration of the WiFi service
 *
 * @return
 *     - NULL, Fail
 *     - Others, Success
 */
periph_service_handle_t wifi_service_create(wifi_service_config_t *config);

/*
 * @brief Destroy the WiFi service instance
 *
 * @param handle  The periph_service_handle_t instance
 *
 * @return
 *     - ESP_OK, Success
 *     - Others, Fail
 */
esp_err_t wifi_service_destroy(periph_service_handle_t handle);

/*
 * @brief Rigster the WiFi setting handle instance to configurate wifi
 *
 * @param handle        The periph_service_handle_t instance
 * @param setting       Specific WiFi setting handle instance
 * @param out_index     Get the `setting` index
 *
 * @return
 *     - ESP_OK, Success
 *     - Others, Fail
 */
esp_err_t wifi_service_register_setting_handle(periph_service_handle_t handle, esp_wifi_setting_handle_t setting, int *out_index);

/*
 * @brief Start setting with given index.
 *
 * @param handle  The periph_service_handle_t instance
 * @param index   Specific index will be start WiFi setting
 *                -`0`, selected all registed instance
 *                - Others, got from by `wifi_service_register_setting_handle`
 *
 * @return
 *     - ESP_OK, Success
 *     - Others, Fail
 */
esp_err_t wifi_service_setting_start(periph_service_handle_t handle, int index);

/*
 * @brief Update ssid and password and connect to the ap.
 * @Note  It works only after the wifi service task runs up.
 *
 * @param handle  The periph_service_handle_t instance
 * @param info    A pointer to wifi_config_t
 *
 * @return
 *     - ESP_OK, Success
 *     - Others, Fail
 */
esp_err_t wifi_service_update_sta_info(periph_service_handle_t handle, wifi_config_t *wifi_conf);

/*
 * @brief Stop setting with given index
 *
 * @param handle  The periph_service_handle_t instance
 * @param index   Specific index will be stop WiFi setting
 *                -`0`, selected all registed instance
 *                - Others, got from by `wifi_service_register_setting_handle`
 *
 * @return
 *     - ESP_OK, Success
 *     - Others, Fail
 */
esp_err_t wifi_service_setting_stop(periph_service_handle_t handle, int index);

/*
 * @brief Connect wifi
 *
 * @param handle  The periph_service_handle_t instance
 *
 * @return
 *     - ESP_OK, Success
 *     - Others, Fail
 */
esp_err_t wifi_service_connect(periph_service_handle_t handle);

/*
 * @brief Disconnect wifi
 *
 * @param handle  The periph_service_handle_t instance
 *
 * @return
 *     - ESP_OK, Success
 *     - Others, Fail
 */
esp_err_t wifi_service_disconnect(periph_service_handle_t handle);

/*
 * @brief Set the WiFi ssid and password
 * @Note  The wifi ssid and password is set to connect only when there is no wifi information stored in flash
 *
 * @param handle  The periph_service_handle_t instance
 * @param info    A pointer to wifi_config_t
 *
 * @return
 *     - ESP_OK, Success
 *     - Others, Fail
 */
esp_err_t wifi_service_set_sta_info(periph_service_handle_t handle, wifi_config_t *info);

/*
 * @brief Get WiFi service state
 *
 * @param handle  The periph_service_handle_t instance
 *
 * @return The state of service
 *
 */
periph_service_state_t wifi_service_state_get(periph_service_handle_t handle);

/*
 * @brief Get WiFi disconnect reason
 *
 * @param handle  The periph_service_handle_t instance
 *
 * @return The reason of disconnect
 *
 */
wifi_service_disconnect_reason_t wifi_service_disconnect_reason_get(periph_service_handle_t handle);

/*
 * @brief Erase all the ssid information stored in ssid manager
 *
 * @param handle  The periph_service_handle_t instance
 *
 * @return
 *     - ESP_OK, Success
 *     - Others, Fail
 */
esp_err_t wifi_service_erase_ssid_manager_info(periph_service_handle_t handle);


/*
 * @brief Get the last wifi configure
 *
 * @param handle    The periph_service_handle_t instance
 * @param wifi_cfg  A pointer to wifi_config_t
 *
 * @return
 *     - ESP_OK, Success
 *     - Others, Fail
 */
esp_err_t wifi_service_get_last_ssid_cfg(periph_service_handle_t handle, wifi_config_t* wifi_cfg);

#ifdef __cplusplus
}
#endif

#endif
