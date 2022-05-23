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

#include <string.h>
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_smartconfig.h"
#include "esp_wifi.h"

#include "esp_peripherals.h"
#include "periph_wifi.h"
#include "wifibleconfig.h"
#include "audio_mem.h"

#if __has_include("esp_idf_version.h")
#include "esp_idf_version.h"
#else
#define ESP_IDF_VERSION_VAL(major, minor, patch) 1
#endif

static const char *TAG = "PERIPH_WIFI";

#define VALIDATE_WIFI(periph, ret)                                  \
    if (!(periph && esp_periph_get_id(periph) == PERIPH_ID_WIFI)) { \
        ESP_LOGE(TAG, "Invalid WIFI periph, at line %d", __LINE__); \
        return ret;                                                 \
    }

#define DEFAULT_RECONNECT_TIMEOUT_MS (1000)

/* Constants that aren't configurable in menuconfig */
#define EAP_PEAP 1
#define EAP_TTLS 2

typedef struct periph_wifi *periph_wifi_handle_t;

struct periph_wifi {
    periph_wifi_state_t wifi_state;
    bool disable_auto_reconnect;
    bool is_open;
    uint8_t max_recon_time;
    char *ssid;
    char *password;
    EventGroupHandle_t state_event;
    int reconnect_timeout_ms;
    periph_wifi_config_mode_t config_mode;
    periph_wpa2_enterprise_cfg_t *wpa2_e_cfg;
};

static const int CONNECTED_BIT = BIT0;
static const int DISCONNECTED_BIT = BIT1;
static const int SMARTCONFIG_DONE_BIT = BIT2;
static const int SMARTCONFIG_ERROR_BIT = BIT3;

static esp_periph_handle_t g_periph = NULL;

esp_err_t periph_wifi_wait_for_connected(esp_periph_handle_t periph, TickType_t tick_to_wait)
{
    VALIDATE_WIFI(periph, ESP_FAIL);
    periph_wifi_handle_t periph_wifi = (periph_wifi_handle_t)esp_periph_get_data(periph);
    EventBits_t connected_bit = xEventGroupWaitBits(periph_wifi->state_event, CONNECTED_BIT, false, true, tick_to_wait);
    if (connected_bit & CONNECTED_BIT) {
        return ESP_OK;
    }
#ifdef CONFIG_BLUEDROID_ENABLED
    if (periph_wifi->config_mode == WIFI_CONFIG_BLUEFI) {
        ble_config_stop();
    }
#endif
    return ESP_FAIL;
}

periph_wifi_state_t periph_wifi_is_connected(esp_periph_handle_t periph)
{
    VALIDATE_WIFI(periph, false);
    periph_wifi_handle_t wifi = (periph_wifi_handle_t)esp_periph_get_data(periph);
    return wifi->wifi_state;
}

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
static void _wifi_smartconfig_event_callback(void *arg, esp_event_base_t event_base,
        int32_t event_id, void *event_data)
{
    wifi_config_t sta_conf;
    periph_wifi_handle_t periph_wifi = (periph_wifi_handle_t)esp_periph_get_data(g_periph);
    switch (event_id) {
        case SC_EVENT_SCAN_DONE:
            ESP_LOGD(TAG, "SC_EVENT_SCAN_DONE");
            break;

        case SC_EVENT_FOUND_CHANNEL:
            ESP_LOGD(TAG, "SC_EVENT_FOUND_CHANNEL");
            break;

        case SC_EVENT_GOT_SSID_PSWD:
            ESP_LOGE(TAG, "SC_EVENT_GOT_SSID_PSWD");
            smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
            memset(&sta_conf, 0x00, sizeof(sta_conf));
            memcpy(sta_conf.sta.ssid, evt->ssid, sizeof(sta_conf.sta.ssid));
            memcpy(sta_conf.sta.password, evt->password, sizeof(sta_conf.sta.password));
            sta_conf.sta.bssid_set = evt->bssid_set;
            if (sta_conf.sta.bssid_set == true) {
                memcpy(sta_conf.sta.bssid, evt->bssid, sizeof(sta_conf.sta.bssid));
            }
            ESP_LOGE(TAG, "SSID=%s, PASS=%s", sta_conf.sta.ssid, sta_conf.sta.password);

            esp_wifi_disconnect();

            if (esp_wifi_set_config(WIFI_IF_STA, &sta_conf) != ESP_OK) {
                periph_wifi->wifi_state = PERIPH_WIFI_CONFIG_ERROR;
                xEventGroupSetBits(periph_wifi->state_event, SMARTCONFIG_ERROR_BIT);
            }
            if (esp_wifi_connect() != ESP_OK) {
                periph_wifi->wifi_state = PERIPH_WIFI_CONFIG_ERROR;
                xEventGroupSetBits(periph_wifi->state_event, SMARTCONFIG_ERROR_BIT);
                esp_periph_send_event(g_periph, PERIPH_WIFI_CONFIG_ERROR, NULL, 0);
                break;
            }
            break;

        case SC_EVENT_SEND_ACK_DONE:
            ESP_LOGE(TAG, "SC_EVENT_SEND_ACK_DONE");

            periph_wifi->wifi_state = PERIPH_WIFI_CONFIG_DONE;
            esp_periph_send_event(g_periph, PERIPH_WIFI_CONFIG_DONE, NULL, 0);
            xEventGroupSetBits(periph_wifi->state_event, SMARTCONFIG_DONE_BIT);
            esp_smartconfig_stop();
            break;
    }
}
#else
static void _wifi_smartconfig_event_callback(smartconfig_status_t status, void *pdata)
{
    wifi_config_t sta_conf;
    smartconfig_type_t *type;
    periph_wifi_handle_t periph_wifi = (periph_wifi_handle_t)esp_periph_get_data(g_periph);
    switch (status) {
        case SC_STATUS_WAIT:
            ESP_LOGD(TAG, "SC_STATUS_WAIT");
            break;

        case SC_STATUS_FIND_CHANNEL:
            ESP_LOGD(TAG, "SC_STATUS_FIND_CHANNEL");
            break;

        case SC_STATUS_GETTING_SSID_PSWD:
            type = pdata;
            ESP_LOGD(TAG, "SC_STATUS_GETTING_SSID_PSWD, SC_TYPE=%d", (int)*type);
            break;

        case SC_STATUS_LINK:
            ESP_LOGE(TAG, "SC_STATUS_LINK");
            memset(&sta_conf, 0x00, sizeof(sta_conf));
            memcpy(&sta_conf.sta, pdata, sizeof(wifi_sta_config_t));
            ESP_LOGE(TAG, "SSID=%s, PASS=%s", sta_conf.sta.ssid, sta_conf.sta.password);
            esp_wifi_disconnect();

            if (esp_wifi_set_config(WIFI_IF_STA, &sta_conf) != ESP_OK) {
                periph_wifi->wifi_state = PERIPH_WIFI_CONFIG_ERROR;
                xEventGroupSetBits(periph_wifi->state_event, SMARTCONFIG_ERROR_BIT);
            }
            if (esp_wifi_connect() != ESP_OK) {
                periph_wifi->wifi_state = PERIPH_WIFI_CONFIG_ERROR;
                xEventGroupSetBits(periph_wifi->state_event, SMARTCONFIG_ERROR_BIT);
                esp_periph_send_event(g_periph, PERIPH_WIFI_CONFIG_ERROR, NULL, 0);
                break;
            }
            break;

        case SC_STATUS_LINK_OVER:
            ESP_LOGE(TAG, "SC_STATUS_LINK_OVER");

            if (pdata != NULL) {
                char phone_ip[4] = { 0 };
                memcpy(phone_ip, (const void *)pdata, 4);
                ESP_LOGD(TAG, "Phone ip: %d.%d.%d.%d", phone_ip[0], phone_ip[1], phone_ip[2], phone_ip[3]);
                periph_wifi->wifi_state = PERIPH_WIFI_CONFIG_DONE;
                esp_periph_send_event(g_periph, PERIPH_WIFI_CONFIG_DONE, NULL, 0);
                xEventGroupSetBits(periph_wifi->state_event, SMARTCONFIG_DONE_BIT);
            } else {
                periph_wifi->wifi_state = PERIPH_WIFI_CONFIG_ERROR;
                esp_periph_send_event(g_periph, PERIPH_WIFI_CONFIG_ERROR, NULL, 0);
                xEventGroupSetBits(periph_wifi->state_event, SMARTCONFIG_ERROR_BIT);
            }
            esp_smartconfig_stop();
            break;
    }
}
#endif

esp_err_t periph_wifi_wait_for_disconnected(esp_periph_handle_t periph, TickType_t tick_to_wait)
{
    VALIDATE_WIFI(periph, ESP_FAIL);
    periph_wifi_handle_t periph_wifi = (periph_wifi_handle_t)esp_periph_get_data(periph);
    EventBits_t disconnected_bit = xEventGroupWaitBits(periph_wifi->state_event, DISCONNECTED_BIT, false, true, tick_to_wait);
    if (disconnected_bit & DISCONNECTED_BIT) {
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t periph_wifi_config_start(esp_periph_handle_t periph, periph_wifi_config_mode_t mode)
{
    VALIDATE_WIFI(periph, ESP_FAIL);
    periph_wifi_handle_t periph_wifi = (periph_wifi_handle_t)esp_periph_get_data(periph);
    esp_err_t err = ESP_OK;
    periph_wifi->disable_auto_reconnect = true;
    periph_wifi->config_mode = mode;
    esp_wifi_disconnect();

    if (periph_wifi_wait_for_disconnected(periph, portMAX_DELAY) != ESP_OK) {
        return ESP_FAIL;
    }
    periph_wifi->wifi_state = PERIPH_WIFI_SETTING;
    if (mode >= WIFI_CONFIG_ESPTOUCH && mode <= WIFI_CONFIG_ESPTOUCH_AIRKISS) {
        err = ESP_OK; //0;
        // esp_wifi_start();
        err |= esp_smartconfig_set_type(mode);
        err |= esp_smartconfig_fast_mode(true);
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
        smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
        err |= esp_smartconfig_start(&cfg);
        esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &_wifi_smartconfig_event_callback, NULL);
#else
        err |= esp_smartconfig_start(_wifi_smartconfig_event_callback, 0);
#endif
        xEventGroupClearBits(periph_wifi->state_event, SMARTCONFIG_DONE_BIT);
        xEventGroupClearBits(periph_wifi->state_event, SMARTCONFIG_ERROR_BIT);

    } else if (mode == WIFI_CONFIG_WPS) {
        //todo : add wps
        return ESP_OK;
    } else if (mode == WIFI_CONFIG_BLUEFI) {
#ifdef CONFIG_BLUEDROID_ENABLED
        ble_config_start(periph);
#endif
        return ESP_OK;
    }

    return err;
}

esp_err_t periph_wifi_config_wait_done(esp_periph_handle_t periph, TickType_t tick_to_wait)
{
    VALIDATE_WIFI(periph, ESP_FAIL);
    periph_wifi_handle_t periph_wifi = (periph_wifi_handle_t)esp_periph_get_data(periph);
    EventBits_t wificonfig_bit = xEventGroupWaitBits(periph_wifi->state_event,
                                 SMARTCONFIG_DONE_BIT | SMARTCONFIG_ERROR_BIT, false, false, tick_to_wait);

    if (wificonfig_bit & SMARTCONFIG_DONE_BIT) {
        return ESP_OK;
    }
    if (wificonfig_bit & SMARTCONFIG_ERROR_BIT) {
        return ESP_FAIL;
    }
    esp_smartconfig_stop();
    return ESP_FAIL;
}

static void wifi_reconnect_timer(xTimerHandle tmr)
{
    esp_periph_handle_t periph = (esp_periph_handle_t)pvTimerGetTimerID(tmr);
    periph_wifi_handle_t periph_wifi = (periph_wifi_handle_t)esp_periph_get_data(periph);
    esp_periph_stop_timer(periph);
    if (periph_wifi->disable_auto_reconnect != true) {
        esp_wifi_connect();
    }
}

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
static void _wifi_event_callback(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    esp_periph_handle_t self = (esp_periph_handle_t)arg;
    periph_wifi_handle_t periph_wifi = (periph_wifi_handle_t)esp_periph_get_data(self);
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    }  else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG, "Got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        periph_wifi->wifi_state = PERIPH_WIFI_CONNECTED;
        xEventGroupClearBits(periph_wifi->state_event, DISCONNECTED_BIT);
        esp_periph_send_event(self, PERIPH_WIFI_CONNECTED, NULL, 0);
        xEventGroupSetBits(periph_wifi->state_event, CONNECTED_BIT);
        wifi_config_t w_config;
        memset(&w_config, 0x00, sizeof(wifi_config_t));
        esp_wifi_get_config(WIFI_IF_STA, &w_config);
        strcpy(periph_wifi->ssid, (char *)w_config.sta.ssid);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        periph_wifi->wifi_state = PERIPH_WIFI_DISCONNECTED;
        xEventGroupClearBits(periph_wifi->state_event, CONNECTED_BIT);
        xEventGroupSetBits(periph_wifi->state_event, DISCONNECTED_BIT);
        esp_periph_send_event(self, PERIPH_WIFI_DISCONNECTED, NULL, 0);

        ESP_LOGW(TAG, "Wi-Fi disconnected from SSID %s, auto-reconnect %s, reconnect after %d ms",
                 periph_wifi->ssid,
                 periph_wifi->disable_auto_reconnect == 0 ? "enabled" : "disabled",
                 periph_wifi->reconnect_timeout_ms);
        if (periph_wifi->disable_auto_reconnect) {
            return;
        }
        esp_periph_start_timer(self, periph_wifi->reconnect_timeout_ms / portTICK_RATE_MS, wifi_reconnect_timer);

    } else {
        ESP_LOGW(TAG, "WiFi Event cb, Unhandle event_base:%s, event_id:%d", event_base, event_id);
    }
}
#else
static esp_err_t _wifi_event_callback(void *ctx, system_event_t *event)
{
    esp_periph_handle_t self = (esp_periph_handle_t)ctx;
    periph_wifi_handle_t periph_wifi = (periph_wifi_handle_t)esp_periph_get_data(self);
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            periph_wifi->wifi_state = PERIPH_WIFI_CONNECTING;
            esp_wifi_connect();
            break;

        case SYSTEM_EVENT_STA_CONNECTED:
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            periph_wifi->wifi_state = PERIPH_WIFI_CONNECTED;
            xEventGroupClearBits(periph_wifi->state_event, DISCONNECTED_BIT);
            esp_periph_send_event(self, PERIPH_WIFI_CONNECTED, NULL, 0);
            xEventGroupSetBits(periph_wifi->state_event, CONNECTED_BIT);
            wifi_config_t w_config;
            memset(&w_config, 0x00, sizeof(wifi_config_t));
            esp_wifi_get_config(WIFI_IF_STA, &w_config);
            strcpy(periph_wifi->ssid, (char *)w_config.sta.ssid);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            periph_wifi->wifi_state = PERIPH_WIFI_DISCONNECTED;
            xEventGroupClearBits(periph_wifi->state_event, CONNECTED_BIT);
            xEventGroupSetBits(periph_wifi->state_event, DISCONNECTED_BIT);
            esp_periph_send_event(self, PERIPH_WIFI_DISCONNECTED, NULL, 0);

            ESP_LOGW(TAG, "Wi-Fi disconnected from SSID %s, auto-reconnect %s, reconnect after %d ms",
                     periph_wifi->ssid,
                     periph_wifi->disable_auto_reconnect == 0 ? "enabled" : "disabled",
                     periph_wifi->reconnect_timeout_ms);
            if (periph_wifi->disable_auto_reconnect) {
                break;
            }
            esp_periph_start_timer(self, periph_wifi->reconnect_timeout_ms / portTICK_RATE_MS, wifi_reconnect_timer);
            break;
        default:
            break;
    }
    return ESP_OK;
}
#endif

static esp_err_t _wifi_run(esp_periph_handle_t self, audio_event_iface_msg_t *msg)
{
    esp_periph_send_event(self, msg->cmd, NULL, 0);
    return ESP_OK;
}

static esp_err_t _wifi_init(esp_periph_handle_t self)
{
    periph_wifi_handle_t periph_wifi = (periph_wifi_handle_t)esp_periph_get_data(self);
    wifi_config_t wifi_config;

    if (periph_wifi->is_open) {
        ESP_LOGE(TAG, "Wifi has initialized");
        return ESP_FAIL;
    }
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
    ESP_ERROR_CHECK(esp_event_loop_create_default());
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 1, 0))
    esp_netif_create_default_wifi_sta();
#endif
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &_wifi_event_callback, self));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &_wifi_event_callback, self));
#else
#include "esp_event_loop.h"
    if (esp_event_loop_get_queue() == NULL) {
        ESP_ERROR_CHECK(esp_event_loop_init(_wifi_event_callback, self));
    } else {
        esp_event_loop_set_cb(_wifi_event_callback, self);
    }
#endif

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    memset(&wifi_config, 0x00, sizeof(wifi_config_t));
    if (periph_wifi->ssid) {
        strcpy((char *)wifi_config.sta.ssid, periph_wifi->ssid);
        ESP_LOGD(TAG, "WIFI_SSID=%s", wifi_config.sta.ssid);
        if (periph_wifi->password) {
            strcpy((char *)wifi_config.sta.password, periph_wifi->password);
            ESP_LOGD(TAG, "WIFI_PASS=%s", wifi_config.sta.password);
        }
        ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    }
    if (periph_wifi->wpa2_e_cfg->diasble_wpa2_e) {
        unsigned int ca_pem_bytes = periph_wifi->wpa2_e_cfg->ca_pem_end - periph_wifi->wpa2_e_cfg->ca_pem_start;
        unsigned int client_crt_bytes = periph_wifi->wpa2_e_cfg->wpa2_e_cert_end - periph_wifi->wpa2_e_cfg->wpa2_e_cert_start;
        unsigned int client_key_bytes = periph_wifi->wpa2_e_cfg->wpa2_e_key_end - periph_wifi->wpa2_e_cfg->wpa2_e_key_start;

        ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_set_ca_cert((const unsigned char *)periph_wifi->wpa2_e_cfg->ca_pem_start, ca_pem_bytes));
        ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_set_cert_key((const unsigned char *)periph_wifi->wpa2_e_cfg->wpa2_e_cert_start, client_crt_bytes, \
                        (const unsigned char *)periph_wifi->wpa2_e_cfg->wpa2_e_key_start, client_key_bytes, NULL, 0));
        ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)periph_wifi->wpa2_e_cfg->eap_id, strlen(periph_wifi->wpa2_e_cfg->eap_id)));
        if (periph_wifi->wpa2_e_cfg->eap_method == EAP_PEAP || periph_wifi->wpa2_e_cfg->eap_method == EAP_TTLS) {
            ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_set_username((uint8_t *)periph_wifi->wpa2_e_cfg->eap_username, strlen(periph_wifi->wpa2_e_cfg->eap_username)));
            ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_set_password((uint8_t *)periph_wifi->wpa2_e_cfg->eap_password, strlen(periph_wifi->wpa2_e_cfg->eap_password)));
        }

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
        ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_enable());
#else
        esp_wpa2_config_t wpa2_config = WPA2_CONFIG_INIT_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_enable(&wpa2_config));
#endif

    }

    ESP_ERROR_CHECK(esp_wifi_start());
    periph_wifi->is_open = true;
    periph_wifi->wifi_state = PERIPH_WIFI_DISCONNECTED;
    xEventGroupClearBits(periph_wifi->state_event, CONNECTED_BIT);
    xEventGroupSetBits(periph_wifi->state_event, DISCONNECTED_BIT);
    return ESP_OK;
}

static esp_err_t _wifi_destroy(esp_periph_handle_t self)
{
    periph_wifi_handle_t periph_wifi = (periph_wifi_handle_t)esp_periph_get_data(self);
    esp_periph_stop_timer(self);
    periph_wifi->disable_auto_reconnect = true;
    esp_wifi_disconnect();
    periph_wifi_wait_for_disconnected(self, portMAX_DELAY);
    esp_wifi_stop();
    esp_wifi_deinit();
    audio_free(periph_wifi->ssid);
    audio_free(periph_wifi->password);

    vEventGroupDelete(periph_wifi->state_event);
    if (periph_wifi->wpa2_e_cfg != NULL) {
        audio_free(periph_wifi->wpa2_e_cfg);
        periph_wifi->wpa2_e_cfg = NULL;
    }
    audio_free(periph_wifi);
    g_periph = NULL;
    return ESP_OK;
}

esp_periph_handle_t periph_wifi_init(periph_wifi_cfg_t *config)
{
    esp_periph_handle_t periph = NULL;
    periph_wifi_handle_t periph_wifi = NULL;
    bool _success = ((periph = esp_periph_create(PERIPH_ID_WIFI, "periph_wifi"))
                     && (periph_wifi = audio_calloc(1, sizeof(struct periph_wifi)))
                     && (periph_wifi->state_event = xEventGroupCreate())
                     && (config->ssid ? (bool)(periph_wifi->ssid = audio_strdup(config->ssid)) : true)
                     && (config->password ? (bool)(periph_wifi->password = audio_strdup(config->password)) : true));

    AUDIO_MEM_CHECK(TAG, _success, goto _periph_wifi_init_failed);

    periph_wifi->reconnect_timeout_ms = config->reconnect_timeout_ms;
    if (periph_wifi->reconnect_timeout_ms == 0) {
        periph_wifi->reconnect_timeout_ms = DEFAULT_RECONNECT_TIMEOUT_MS;
    }
    periph_wifi->disable_auto_reconnect = config->disable_auto_reconnect;

    periph_wifi->wpa2_e_cfg = audio_malloc(sizeof(periph_wpa2_enterprise_cfg_t));
    AUDIO_NULL_CHECK(TAG, periph_wifi->wpa2_e_cfg, {
        audio_free(periph);
        goto _periph_wifi_init_failed;
    });
    memcpy(periph_wifi->wpa2_e_cfg, &config->wpa2_e_cfg, sizeof(periph_wpa2_enterprise_cfg_t));

    esp_periph_set_data(periph, periph_wifi);
    esp_periph_set_function(periph, _wifi_init, _wifi_run, _wifi_destroy);
    g_periph = periph;
    return periph;

_periph_wifi_init_failed:
    if (periph_wifi) {
        vEventGroupDelete(periph_wifi->state_event);
        audio_free(periph_wifi->ssid);
        audio_free(periph_wifi->password);
        audio_free(periph_wifi);
    }
    return NULL;
}
