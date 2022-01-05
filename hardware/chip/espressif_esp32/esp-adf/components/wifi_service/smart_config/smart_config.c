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

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_wifi_setting.h"

#include "audio_mem.h"
#include "smart_config.h"

#if __has_include("esp_idf_version.h")
#include "esp_idf_version.h"
#else
#define ESP_IDF_VERSION_VAL(major, minor, patch) 1
#endif

static char *TAG = "SMART_CONFIG";
static esp_wifi_setting_handle_t sm_setting_handle;

typedef struct {
    smartconfig_type_t type;
} smart_config_info;

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
static void smartconfg_cb(void *arg, esp_event_base_t event_base,
    int32_t event_id, void *event_data)
{
    wifi_config_t sta_conf;
    switch (event_id) {
        case SC_EVENT_SCAN_DONE:
            ESP_LOGI(TAG, "SC_EVENT_SCAN_DONE");
            break;
        case SC_EVENT_FOUND_CHANNEL:
            ESP_LOGI(TAG, "SC_EVENT_FOUND_CHANNEL");
            break;
        case SC_EVENT_GOT_SSID_PSWD:
            ESP_LOGI(TAG, "SC_EVENT_GOT_SSID_PSWD");
            esp_wifi_disconnect();
            smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
            memset(&sta_conf, 0x00, sizeof(sta_conf));
            memcpy(sta_conf.sta.ssid, evt->ssid, sizeof(sta_conf.sta.ssid));
            memcpy(sta_conf.sta.password, evt->password, sizeof(sta_conf.sta.password));
            sta_conf.sta.bssid_set = evt->bssid_set;
            if (sta_conf.sta.bssid_set == true) {
                memcpy(sta_conf.sta.bssid, evt->bssid, sizeof(sta_conf.sta.bssid));
            }
            ESP_LOGE(TAG, "SSID=%s, PASS=%s", sta_conf.sta.ssid, sta_conf.sta.password);

            if (sm_setting_handle) {
                esp_wifi_setting_info_notify(sm_setting_handle, &sta_conf);
            }
            break;
        case SC_EVENT_SEND_ACK_DONE:
            ESP_LOGI(TAG, "SC_EVENT_SEND_ACK_DONE");
            esp_smartconfig_stop();
            break;
    }
}
#else
static void smartconfg_cb(smartconfig_status_t status, void *pdata)
{
    wifi_config_t sta_conf;
    switch (status) {
        case SC_STATUS_WAIT:
            ESP_LOGI(TAG, "SC_STATUS_WAIT");
            break;
        case SC_STATUS_FIND_CHANNEL:
            ESP_LOGI(TAG, "SC_STATUS_FIND_CHANNEL");
            break;
        case SC_STATUS_GETTING_SSID_PSWD:
            ESP_LOGI(TAG, "SC_STATUS_GETTING_SSID_PSWD");
            smartconfig_type_t *type = pdata;
            if (*type == SC_TYPE_ESPTOUCH) {
                ESP_LOGD(TAG, "SC_TYPE:SC_TYPE_ESPTOUCH");
            } else {
                ESP_LOGD(TAG, "SC_TYPE:SC_TYPE_AIRKISS");
            }
            break;
        case SC_STATUS_LINK:
            ESP_LOGI(TAG, "SC_STATUS_LINK");
            esp_wifi_disconnect();
            memset(&sta_conf, 0x00, sizeof(sta_conf));
            memcpy(&(sta_conf.sta), pdata, sizeof(wifi_sta_config_t));
            ESP_LOGI(TAG, "<link>ssid:%s", sta_conf.sta.ssid);
            ESP_LOGI(TAG, "<link>pass:%s", sta_conf.sta.password);

            if (sm_setting_handle) {
                esp_wifi_setting_info_notify(sm_setting_handle, &sta_conf);
            }
            break;
        case SC_STATUS_LINK_OVER:
            ESP_LOGI(TAG, "SC_STATUS_LINK_OVER");
            if (pdata != NULL) {
                uint8_t phone_ip[4] = { 0 };
                memcpy(phone_ip, (const void *)pdata, 4);
                ESP_LOGI(TAG, "Phone ip: %d.%d.%d.%d\n", phone_ip[0], phone_ip[1], phone_ip[2], phone_ip[3]);
            }
            esp_smartconfig_stop();
            break;
    }
}
#endif

static esp_err_t _smart_config_start(esp_wifi_setting_handle_t self)
{
    smart_config_info *info = esp_wifi_setting_get_data(self);
    esp_err_t ret = ESP_OK;
    ret = esp_smartconfig_set_type(info->type);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_smartconfig_set_type fail");
        return ESP_FAIL;
    }
    ret = esp_smartconfig_fast_mode(true);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_smartconfig_fast_mode fail");
        return ESP_FAIL;
    }

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ret = esp_smartconfig_start(&cfg);
    esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &smartconfg_cb, NULL);
#else
    ret = esp_smartconfig_start(smartconfg_cb, 1);
#endif
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_smartconfig_start fail");
        return ESP_FAIL;
    }
    return ret;
}

static esp_err_t _smart_config_stop(esp_wifi_setting_handle_t self)
{
    esp_smartconfig_stop();
    return ESP_OK;
}

esp_wifi_setting_handle_t smart_config_create(smart_config_info_t *info)
{
    sm_setting_handle = esp_wifi_setting_create("esp_smart_config");
    AUDIO_MEM_CHECK(TAG, sm_setting_handle, return NULL);
    smart_config_info *cfg = audio_calloc(1, sizeof(smart_config_info));
    AUDIO_MEM_CHECK(TAG, cfg, {
        audio_free(sm_setting_handle);
        return NULL;
    });
    cfg->type = info->type;
    esp_wifi_setting_set_data(sm_setting_handle, cfg);
    esp_wifi_setting_register_function(sm_setting_handle, _smart_config_start, _smart_config_stop, NULL);
    return sm_setting_handle;
}
