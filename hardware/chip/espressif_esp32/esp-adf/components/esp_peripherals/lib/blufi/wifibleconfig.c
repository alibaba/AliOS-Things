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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "audio_mem.h"
#include "audio_idf_version.h"

#ifdef CONFIG_BLUEDROID_ENABLED
#include "esp_system.h"
#include "esp_bt.h"
#include "esp_log.h"
#include "audio_error.h"
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
#include "esp_smartconfig.h"
#include "audio_error.h"
#include "esp_wifi.h"
#include "wifibleconfig.h"
#include "periph_wifi.h"
#include "blufi_security.h"

#define WIFI_BLE_TAG        "WIFI_BLE_CONFIG"
#define BLUFI_DEVICE_NAME   "BLUFI_DEVICE"
#define WIFI_LIST_NUM       (10)

static uint8_t wifi_ble_service_uuid128[32] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
};
typedef struct wifi_ble_config {
    uint8_t ble_server_if;
    uint16_t ble_conn_id;
    wifi_config_t sta_config;
    esp_periph_handle_t periph;
} wifi_ble_config_t;

static wifi_ble_config_t *g_wifi_ble_config = NULL;

static void wifi_ble_event_callback(esp_blufi_cb_event_t event, esp_blufi_cb_param_t *param);

static esp_ble_adv_data_t wifi_ble_adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x100,
    .max_interval = 0x100,
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data =  NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 16,
    .p_service_uuid = wifi_ble_service_uuid128,
    .flag = 0x6,
};

static esp_ble_adv_params_t wifi_ble_adv_params = {
    .adv_int_min        = 0x100,
    .adv_int_max        = 0x100,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static esp_blufi_callbacks_t wifi_ble_callbacks = {
    .event_cb = wifi_ble_event_callback,
    .negotiate_data_handler = blufi_dh_negotiate_data_handler,
    .encrypt_func = blufi_aes_encrypt,
    .decrypt_func = blufi_aes_decrypt,
    .checksum_func = blufi_crc_checksum,
};

static void wifi_ble_gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            esp_ble_gap_start_advertising(&wifi_ble_adv_params);
            break;
        default:
            break;
    }
}

esp_err_t ble_config_stop(void)
{
    if (g_wifi_ble_config != NULL) {
        audio_free(g_wifi_ble_config);
        g_wifi_ble_config = NULL;
    }
    blufi_security_deinit();
    esp_blufi_profile_deinit();
    esp_bluedroid_disable();
    esp_bluedroid_deinit();
    return ESP_OK;
}

static void wifi_ble_event_callback(esp_blufi_cb_event_t event, esp_blufi_cb_param_t *param)
{
    /* actually, should post to blufi_task handle the procedure,
     * now, as a audio_ble, we do it more simply */
    esp_err_t ret ;
    switch (event) {
        case ESP_BLUFI_EVENT_INIT_FINISH:
            ESP_LOGI(WIFI_BLE_TAG, "BLUFI init finish");
            esp_ble_gap_set_device_name(BLUFI_DEVICE_NAME);
            esp_ble_gap_config_adv_data(&wifi_ble_adv_data);
            break;
        case ESP_BLUFI_EVENT_DEINIT_FINISH:
            ESP_LOGI(WIFI_BLE_TAG, "BLUFI deinit finish");
            break;
        case ESP_BLUFI_EVENT_BLE_CONNECT:
            ESP_LOGI(WIFI_BLE_TAG, "BLUFI ble connect");
            esp_smartconfig_stop();
            g_wifi_ble_config->ble_server_if = param->connect.server_if;
            g_wifi_ble_config->ble_conn_id = param->connect.conn_id;
            break;
        case ESP_BLUFI_EVENT_BLE_DISCONNECT:
            ESP_LOGI(WIFI_BLE_TAG, "BLUFI ble disconnect");
            break;
        case ESP_BLUFI_EVENT_SET_WIFI_OPMODE:
            ESP_LOGI(WIFI_BLE_TAG, "BLUFI Set WIFI opmode %d", param->wifi_mode.op_mode);
            ESP_ERROR_CHECK( esp_wifi_set_mode(param->wifi_mode.op_mode) );
            break;
        case ESP_BLUFI_EVENT_REQ_CONNECT_TO_AP:
            ESP_LOGI(WIFI_BLE_TAG, "BLUFI request wifi connect to AP");
            esp_wifi_disconnect();
            if (ESP_OK != esp_wifi_connect()) {
                esp_periph_send_event(g_wifi_ble_config->periph, PERIPH_WIFI_CONFIG_ERROR, NULL, 0);
            } else {
                esp_periph_send_event(g_wifi_ble_config->periph, PERIPH_WIFI_CONFIG_DONE, NULL, 0);
                ble_config_stop();
            }
            break;
        case ESP_BLUFI_EVENT_REQ_DISCONNECT_FROM_AP:
            ESP_LOGI(WIFI_BLE_TAG, "BLUFI request wifi disconnect from AP");
            esp_wifi_disconnect();
            break;
        case ESP_BLUFI_EVENT_GET_WIFI_STATUS: {
                wifi_mode_t mode;
                esp_blufi_extra_info_t info;
                esp_wifi_get_mode(&mode);
                memset(&info, 0, sizeof(esp_blufi_extra_info_t));
                info.sta_bssid_set = true;
                info.sta_ssid = g_wifi_ble_config->sta_config.sta.ssid;
                esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_SUCCESS, 0, &info);
                ESP_LOGI(WIFI_BLE_TAG, "BLUFI get wifi status from AP");
                break;
            }
        case ESP_BLUFI_EVENT_RECV_SLAVE_DISCONNECT_BLE:
            ESP_LOGI(WIFI_BLE_TAG, "BLUFI close a gatt connection");
            esp_blufi_close(g_wifi_ble_config->ble_server_if, g_wifi_ble_config->ble_conn_id);
            break;
        case ESP_BLUFI_EVENT_DEAUTHENTICATE_STA:
            /* TODO */
            break;
        case ESP_BLUFI_EVENT_RECV_STA_BSSID:
            memcpy(g_wifi_ble_config->sta_config.sta.bssid, param->sta_bssid.bssid, 6);
            g_wifi_ble_config->sta_config.sta.bssid_set = 1;
            esp_wifi_set_config(WIFI_IF_STA, &g_wifi_ble_config->sta_config);
            ESP_LOGI(WIFI_BLE_TAG, "Recv STA BSSID %s", g_wifi_ble_config->sta_config.sta.bssid);
            break;
        case ESP_BLUFI_EVENT_RECV_STA_SSID:
            strncpy((char *)g_wifi_ble_config->sta_config.sta.ssid, (char *)param->sta_ssid.ssid, param->sta_ssid.ssid_len);
            g_wifi_ble_config->sta_config.sta.ssid[param->sta_ssid.ssid_len] = '\0';
            ret = esp_wifi_set_config(WIFI_IF_STA, &g_wifi_ble_config->sta_config);
            ESP_LOGI(WIFI_BLE_TAG, "Recv STA SSID ret %d %s", ret, g_wifi_ble_config->sta_config.sta.ssid);
            break;
        case ESP_BLUFI_EVENT_RECV_STA_PASSWD:
            strncpy((char *)g_wifi_ble_config->sta_config.sta.password, (char *)param->sta_passwd.passwd, param->sta_passwd.passwd_len);
            g_wifi_ble_config->sta_config.sta.password[param->sta_passwd.passwd_len] = '\0';
            esp_wifi_set_config(WIFI_IF_STA, &g_wifi_ble_config->sta_config);
            ESP_LOGI(WIFI_BLE_TAG, "Recv STA PASSWORD %s", g_wifi_ble_config->sta_config.sta.password);
            break;
        default:
            ESP_LOGE(WIFI_BLE_TAG, "Event %d is not supported", event);
            break;
    }
}

esp_err_t ble_config_start(esp_periph_handle_t periph)
{
    ESP_LOGI(WIFI_BLE_TAG, "ble_config_start");

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_IDLE) {
        if (esp_bt_controller_init(&bt_cfg) != ESP_OK) {
            ESP_LOGE(WIFI_BLE_TAG, "%s initialize controller failed", __func__);
            return ESP_FAIL;
        }

        if (esp_bt_controller_enable(ESP_BT_MODE_BLE) != ESP_OK) {
            ESP_LOGE(WIFI_BLE_TAG, "%s enable controller failed", __func__);
            return ESP_FAIL;
        }
    }
    if (esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_UNINITIALIZED) {
        if (esp_bluedroid_init() != ESP_OK) {
            ESP_LOGE(WIFI_BLE_TAG, "%s esp_bluedroid_init failed", __func__);
            return ESP_FAIL;
        }
        if (esp_bluedroid_enable() != ESP_OK) {
            ESP_LOGE(WIFI_BLE_TAG, "%s esp_bluedroid_enable failed", __func__);
            return ESP_FAIL;
        }
    }
    ESP_LOGI(WIFI_BLE_TAG, "BD ADDR: "ESP_BD_ADDR_STR"", ESP_BD_ADDR_HEX(esp_bt_dev_get_address()));
    ESP_LOGI(WIFI_BLE_TAG, "BLUFI VERSION %04x", esp_blufi_get_version());

    g_wifi_ble_config = audio_calloc(1, sizeof(wifi_ble_config_t));
    AUDIO_MEM_CHECK(WIFI_BLE_TAG, g_wifi_ble_config, return ESP_FAIL);
    g_wifi_ble_config->periph = periph;

    blufi_security_init();
    esp_ble_gap_register_callback(wifi_ble_gap_event_handler);
    esp_blufi_register_callbacks(&wifi_ble_callbacks);
    esp_blufi_profile_init();

    return ESP_OK;
}
#endif
