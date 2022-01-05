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

#include <string.h>
#include "audio_mem.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "blufi_config.h"
#include "blufi_security.h"
#include "esp_wifi_setting.h"
#include "audio_idf_version.h"

static esp_wifi_setting_handle_t bc_setting_handle;

typedef struct wifi_blufi_config {
    uint8_t                 ble_server_if;
    uint16_t                ble_conn_id;
    wifi_config_t           sta_config;
    bool                    sta_connected_flag;
    bool                    ble_connected_flag;
    void                    *user_data;
    int                     user_data_length;
} wifi_blufi_config_t;

static const char *TAG = "BLUFI_CONFIG";

#ifdef CONFIG_BLUEDROID_ENABLED

#include "esp_bt.h"
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 3, 0))
#include "esp_blufi.h"
#else
#include "esp_blufi_api.h"
#endif
#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"

#define WIFI_LIST_NUM       (10)

static void wifi_ble_event_callback(esp_blufi_cb_event_t event, esp_blufi_cb_param_t *param);
static esp_err_t _ble_config_stop(esp_wifi_setting_handle_t self);

static esp_blufi_callbacks_t wifi_ble_callbacks = {
    .event_cb = wifi_ble_event_callback,
    .negotiate_data_handler = blufi_dh_negotiate_data_handler,
    .encrypt_func = blufi_aes_encrypt,
    .decrypt_func = blufi_aes_decrypt,
    .checksum_func = blufi_crc_checksum,
};

static void wifi_ble_event_callback(esp_blufi_cb_event_t event, esp_blufi_cb_param_t *param)
{
    /* actually, should post to blufi_task handle the procedure,
     * now, as an audio_ble, we do it more simply */
    esp_err_t ret = ESP_OK;
    wifi_blufi_config_t *cfg = esp_wifi_setting_get_data(bc_setting_handle);
    switch (event) {
        case ESP_BLUFI_EVENT_INIT_FINISH:
            ESP_LOGI(TAG, "BLUFI init finish");
            break;
        case ESP_BLUFI_EVENT_DEINIT_FINISH:
            ESP_LOGI(TAG, "BLUFI deinit finish");
            break;
        case ESP_BLUFI_EVENT_BLE_CONNECT:
            ESP_LOGI(TAG, "BLUFI ble connect");
            cfg->ble_connected_flag = true;
            cfg->ble_server_if = param->connect.server_if;
            cfg->ble_conn_id = param->connect.conn_id;
            break;
        case ESP_BLUFI_EVENT_BLE_DISCONNECT:
            cfg->ble_connected_flag = false;
            ESP_LOGI(TAG, "BLUFI ble disconnect");
            break;
        case ESP_BLUFI_EVENT_SET_WIFI_OPMODE:
            ESP_LOGI(TAG, "BLUFI Set WIFI opmode %d", param->wifi_mode.op_mode);
            ESP_ERROR_CHECK( esp_wifi_set_mode(param->wifi_mode.op_mode) );
            break;
        case ESP_BLUFI_EVENT_REQ_CONNECT_TO_AP:
            ESP_LOGI(TAG, "BLUFI request wifi connect to AP");
            if (bc_setting_handle) {
                esp_wifi_setting_info_notify(bc_setting_handle, &cfg->sta_config);
            }
            _ble_config_stop(NULL);
            break;
        case ESP_BLUFI_EVENT_REQ_DISCONNECT_FROM_AP:
            ESP_LOGI(TAG, "BLUFI request wifi disconnect from AP");
            esp_wifi_disconnect();
            break;
        case ESP_BLUFI_EVENT_GET_WIFI_STATUS: {
                wifi_mode_t mode;
                esp_blufi_extra_info_t info = {0};
                esp_wifi_get_mode(&mode);

                if (cfg->sta_connected_flag) {
                    memset(&info, 0, sizeof(esp_blufi_extra_info_t));
                    info.sta_ssid = cfg->sta_config.sta.ssid;
                    info.sta_ssid_len = strlen((char *)cfg->sta_config.sta.ssid);
                    esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_SUCCESS, 0, &info);
                } else {
                    esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_FAIL, 0, NULL);
                }
                ESP_LOGI(TAG, "BLUFI get wifi status from AP");
                break;
            }
        case ESP_BLUFI_EVENT_RECV_SLAVE_DISCONNECT_BLE:
            ESP_LOGI(TAG, "BLUFI close a gatt connection");
            esp_blufi_close(cfg->ble_server_if, cfg->ble_conn_id);
            break;
        case ESP_BLUFI_EVENT_DEAUTHENTICATE_STA:
            /* TODO */
            break;
        case ESP_BLUFI_EVENT_RECV_STA_BSSID:
            memcpy(cfg->sta_config.sta.bssid, param->sta_bssid.bssid, 6);
            cfg->sta_config.sta.bssid_set = 1;
            ESP_LOGI(TAG, "Recv STA BSSID %s", cfg->sta_config.sta.bssid);
            break;
        case ESP_BLUFI_EVENT_RECV_STA_SSID:
            memcpy(cfg->sta_config.sta.ssid, param->sta_ssid.ssid, param->sta_ssid.ssid_len);
            cfg->sta_config.sta.ssid[param->sta_ssid.ssid_len] = '\0';
            ESP_LOGI(TAG, "Recv STA SSID ret %d %s", ret, cfg->sta_config.sta.ssid);
            break;
        case ESP_BLUFI_EVENT_RECV_STA_PASSWD:
            memcpy(cfg->sta_config.sta.password, param->sta_passwd.passwd, param->sta_passwd.passwd_len);
            cfg->sta_config.sta.password[param->sta_passwd.passwd_len] = '\0';
            ESP_LOGI(TAG, "Recv STA PASSWORD %s", cfg->sta_config.sta.password);
            break;
        default:
            ESP_LOGE(TAG, "Event %d is not supported", event);
            break;
    }
}
#endif

esp_err_t _ble_config_start(esp_wifi_setting_handle_t self)
{
#ifdef CONFIG_BLUEDROID_ENABLED
    ESP_LOGI(TAG, "blufi_config_start");
    ESP_LOGI(TAG, "BD ADDR: "ESP_BD_ADDR_STR"", ESP_BD_ADDR_HEX(esp_bt_dev_get_address()));
    ESP_LOGI(TAG, "BLUFI VERSION %04x", esp_blufi_get_version());

    blufi_security_init();
    esp_blufi_register_callbacks(&wifi_ble_callbacks);
    esp_blufi_profile_init();
#else
    ESP_LOGW(TAG, "blufi config selected, but CONFIG_BLUEDROID_ENABLED not enabled");
#endif
    return ESP_OK;
}

static esp_err_t _ble_config_stop(esp_wifi_setting_handle_t self)
{
    return ESP_OK;
}

esp_wifi_setting_handle_t blufi_config_create(void *info)
{
    bc_setting_handle = esp_wifi_setting_create("blufi_config");
    AUDIO_MEM_CHECK(TAG, bc_setting_handle, return NULL);
    wifi_blufi_config_t *cfg = audio_calloc(1, sizeof(wifi_blufi_config_t));
    AUDIO_MEM_CHECK(TAG, cfg, {
        audio_free(bc_setting_handle);
        return NULL;
    });
    esp_wifi_setting_set_data(bc_setting_handle, cfg);
    esp_wifi_setting_register_function(bc_setting_handle, _ble_config_start, _ble_config_stop, NULL);
    return bc_setting_handle;
}

esp_err_t blufi_set_sta_connected_flag(esp_wifi_setting_handle_t handle, bool flag)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    wifi_blufi_config_t *blufi_cfg = esp_wifi_setting_get_data(handle);
    blufi_cfg->sta_connected_flag = flag;
    return ESP_OK;
}

esp_err_t blufi_set_customized_data(esp_wifi_setting_handle_t handle, char *data, int data_len)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, data, return ESP_FAIL);
    wifi_blufi_config_t *blufi_cfg = esp_wifi_setting_get_data(handle);
    blufi_cfg->user_data = audio_calloc(1, data_len + 1);
    blufi_cfg->user_data_length = data_len;
    AUDIO_MEM_CHECK(TAG, blufi_cfg->user_data, return ESP_FAIL);
    memcpy(blufi_cfg->user_data, data, data_len);
    ESP_LOGI(TAG, "Set blufi customized data: %s, length: %d", data, data_len);
    return ESP_OK;
}

esp_err_t blufi_send_customized_data(esp_wifi_setting_handle_t handle)
{
#ifdef CONFIG_BLUEDROID_ENABLED
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    wifi_blufi_config_t *blufi_cfg = esp_wifi_setting_get_data(handle);
    if (blufi_cfg->ble_connected_flag == false) {
        ESP_LOGE(TAG, "No Ble device connected, fail to send customized data");
        return ESP_FAIL;
    }
    if (blufi_cfg->user_data && blufi_cfg->user_data_length) {
        ESP_LOGI(TAG, "Send a string to peer: %s, data len: %d", (char *)blufi_cfg->user_data, blufi_cfg->user_data_length);
        return esp_blufi_send_custom_data((uint8_t *)blufi_cfg->user_data, blufi_cfg->user_data_length);
    } else {
        ESP_LOGW(TAG, "Nothing to be sent, please set customer data first!");
        return ESP_FAIL;
    }
#else
    ESP_LOGW(TAG, "blufi config selected, but CONFIG_BLUEDROID_ENABLED not enabled");
    return ESP_FAIL;
#endif
}
