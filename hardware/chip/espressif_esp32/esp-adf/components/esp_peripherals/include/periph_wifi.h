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

#ifndef _DEV_WIFI_H_
#define _DEV_WIFI_H_

#include "sys/queue.h"
#include "audio_error.h"
#include "audio_common.h"
#include "esp_peripherals.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      Peripheral Wi-Fi event id
 */
typedef enum {
    PERIPH_WIFI_UNCHANGE = 0,
    PERIPH_WIFI_CONNECTING,
    PERIPH_WIFI_CONNECTED,
    PERIPH_WIFI_DISCONNECTED,
    PERIPH_WIFI_SETTING,
    PERIPH_WIFI_CONFIG_DONE,
    PERIPH_WIFI_CONFIG_ERROR,
    PERIPH_WIFI_ERROR,
} periph_wifi_state_t;

/**
 * @brief   Wi-Fi setup mode type
 */
typedef enum {
    WIFI_CONFIG_ESPTOUCH,           /*!< Using smartconfig with ESPTOUCH protocol */
    WIFI_CONFIG_AIRKISS,            /*!< Using smartconfig with AIRKISS protocol */
    WIFI_CONFIG_ESPTOUCH_AIRKISS,   /*!< Using smartconfig with ESPTOUCH_AIRKISS protocol */
    WIFI_CONFIG_WPS,                /*!< Using WPS (not support) */
    WIFI_CONFIG_BLUEFI,             /*!< Using BLUEFI*/
    WIFI_CONFIG_WEB,                /*!< Using HTTP Server (not support) */
} periph_wifi_config_mode_t;

/**
 * @brief   The WPA2 enterprise peripheral configuration
 */
typedef struct {
    bool          diasble_wpa2_e;       /*!< Disable wpa2 enterprise */
    int           eap_method;           /*!< TLS: 0, PEAP: 1, TTLS: 2 */
    char          *ca_pem_start;        /*!< binary wpa2 ca pem start */
    char          *ca_pem_end;          /*!< binary wpa2 ca pem end */
    char          *wpa2_e_cert_start;   /*!< binary wpa2 cert start */
    char          *wpa2_e_cert_end;     /*!< binary wpa2 cert end */
    char          *wpa2_e_key_start;    /*!< binary wpa2 key start */
    char          *wpa2_e_key_end;      /*!< binary wpa2 key end */
    const char    *eap_id;              /*!< Identity in phase 1 of EAP procedure */
    const char    *eap_username;        /*!< Username for EAP method (PEAP and TTLS)  */    
    const char    *eap_password;        /*!< Password for EAP method (PEAP and TTLS) */
} periph_wpa2_enterprise_cfg_t;

/**
 * @brief The Wi-Fi peripheral configuration
 */
typedef struct {
    bool                              disable_auto_reconnect;   /*!< Disable Wi-Fi auto reconnect */
    int                               reconnect_timeout_ms;     /*!< The reconnect timeout after disconnected from Wi-Fi network */
    const char                        *ssid;                    /*!< SSID of target AP */
    const char                        *password;                /*!< password of target AP */
    periph_wpa2_enterprise_cfg_t      wpa2_e_cfg;               /*!< wpa2 enterprise config */
} periph_wifi_cfg_t;

/**
 * @brief      Create the wifi peripheral handle for esp_peripherals
 *
 * @note       The handle was created by this function automatically destroy when `esp_periph_destroy` is called
 *
 * @param      config  The configuration
 *
 * @return     The esp peripheral handle
 */
esp_periph_handle_t periph_wifi_init(periph_wifi_cfg_t* config);

/**
 * @brief      This function will block current thread (in `tick_to_wait` tick) and wait until ESP32 connected to the Wi-Fi network, and got ip
 *
 * @param[in]  periph        The periph
 * @param[in]  tick_to_wait  The tick to wait
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t periph_wifi_wait_for_connected(esp_periph_handle_t periph, TickType_t tick_to_wait);

/**
 * @brief      Check the Wi-Fi connection status
 *
 * @param[in]  periph  The periph
 *
 * @return     Wi-Fi network status
 */
periph_wifi_state_t periph_wifi_is_connected(esp_periph_handle_t periph);

/**
 * @brief      Start Wi-Fi network setup in `mode`
 *
 * @param[in]  periph   The periph
 * @param[in]  mode     The mode
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t periph_wifi_config_start(esp_periph_handle_t periph, periph_wifi_config_mode_t mode);

/**
 * @brief      Wait for Wi-Fi setup done
 * @param[in]  periph        The periph
 * @param[in]  tick_to_wait  The tick to wait
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t periph_wifi_config_wait_done(esp_periph_handle_t periph, TickType_t tick_to_wait);

#ifdef __cplusplus
}
#endif

#endif


