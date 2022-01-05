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

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "audio_mem.h"
#include "wifi_service.h"
#include "wifi_ssid_manager.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_action_def.h"
#include "esp_delegate.h"

#if __has_include("esp_idf_version.h")
#include "esp_idf_version.h"
#else
#define ESP_IDF_VERSION_VAL(major, minor, patch) 1
#endif

static const char *TAG                  = "WIFI_SERV";
const static int WIFI_TASK_DESTROY_BIT  = BIT0;

typedef enum {
    WIFI_SERV_CMD_UNKNOWN,
    WIFI_SERV_CMD_UPDATE,
    WIFI_SERV_CMD_CONNECT,
    WIFI_SERV_CMD_DISCONNECT,
    WIFI_SERV_CMD_SETTING_START,
    WIFI_SERV_CMD_SETTING_STOP,
    WIFI_SERV_CMD_DESTROY,
} wifi_serv_cmd_t;

typedef struct wifi_setting_item {
    STAILQ_ENTRY(wifi_setting_item)     next;
    esp_wifi_setting_handle_t           on_handle;
    int                                 index;
    bool                                running;
} wifi_setting_item_t;

typedef STAILQ_HEAD(wifi_setting_list, wifi_setting_item) wifi_setting_list_t;

typedef struct {
    xQueueHandle                        wifi_serv_que;
    wifi_service_event_t                wifi_serv_state;
    wifi_config_t                       info;
    wifi_setting_list_t                 setting_list;
    int                                 setting_index;
    wifi_service_disconnect_reason_t    reason;
    wifi_ssid_manager_handle_t          ssid_manager;
    bool                                is_setting;
    esp_timer_handle_t                  retry_timer;
    esp_timer_handle_t                  setting_timer;
    int                                 setting_timeout_s;
    EventGroupHandle_t                  sync_evt;
    int                                 retry_times;
    int                                 max_retry_time;
    int                                 prov_retry_times;
    int                                 max_prov_retry_time;
    bool                                retrying;
} wifi_service_t;

typedef enum {
    WIFI_SERV_EVENT_TYPE_CMD,
    WIFI_SERV_EVENT_TYPE_STATE,
} wifi_serv_event_type_t;

typedef struct {
    wifi_serv_event_type_t  msg_type;
    int                     type;
    uint32_t                *pdata;
    int                     len;
} wifi_task_msg_t;

static void wifi_serv_cmd_send(void *que, int type, void *data, int len, int dir)
{
    wifi_task_msg_t evt = {0};
    evt.msg_type = WIFI_SERV_EVENT_TYPE_CMD;
    evt.type = type;
    evt.pdata = data;
    evt.len = len;
    if (dir) {
        xQueueSendToFront(que, &evt, 0) ;
    } else {
        xQueueSend(que, &evt, 0);
    }
}

static void wifi_serv_state_send(void *que, int type, void *data, int len, int dir)
{
    wifi_task_msg_t evt = {0};
    evt.msg_type = WIFI_SERV_EVENT_TYPE_STATE;
    evt.type = type;
    evt.pdata = data;
    evt.len = len;
    if (dir) {
        xQueueSendToFront(que, &evt, 0) ;
    } else {
        xQueueSend(que, &evt, 0);
    }
}

#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
static void wifi_event_cb(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    periph_service_handle_t serv_handle = (periph_service_handle_t)arg;
    wifi_service_t *serv = periph_service_get_data(serv_handle);
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    }  else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG, "Got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        wifi_serv_state_send(serv->wifi_serv_que, WIFI_SERV_EVENT_CONNECTED, 0, 0, 0);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t *event = (wifi_event_sta_disconnected_t *) event_data;
        if (serv->reason == WIFI_SERV_STA_BY_USER) {
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED, reason is WIFI_SERV_STA_BY_USER");
            wifi_serv_state_send(serv->wifi_serv_que, WIFI_SERV_EVENT_DISCONNECTED, 0, 0, 0);
            return;
        }
        wifi_serv_state_send(serv->wifi_serv_que, WIFI_SERV_EVENT_DISCONNECTED, 0, 0, 0);
        switch (event->reason) {
            case WIFI_REASON_AUTH_EXPIRE:
            case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
            case WIFI_REASON_BEACON_TIMEOUT:
            case WIFI_REASON_AUTH_FAIL:
            case WIFI_REASON_ASSOC_FAIL:
            case WIFI_REASON_HANDSHAKE_TIMEOUT:
                ESP_LOGI(TAG, "STA Auth Error, reason:%d", event->reason);
                serv->reason = WIFI_SERV_STA_AUTH_ERROR;
                break;
            case WIFI_REASON_NO_AP_FOUND:
                ESP_LOGI(TAG, "STA AP Not found");
                serv->reason = WIFI_SERV_STA_AP_NOT_FOUND;
                break;
            default:
                ESP_LOGI(TAG, "STA Error, reason:%d", event->reason);
                serv->reason = WIFI_SERV_STA_COM_ERROR;
                break;
        }
    } else {
        ESP_LOGW(TAG, "WiFi Event cb, Unhandle event_base:%s, event_id:%d", event_base, event_id);
    }
}
#else
static esp_err_t wifi_event_cb(void *ctx, system_event_t *event)
{
    periph_service_handle_t serv_handle = (periph_service_handle_t)ctx;
    wifi_service_t *serv = periph_service_get_data(serv_handle);
    switch (event->event_id) {
        case SYSTEM_EVENT_AP_START:
            ESP_LOGI(TAG, "SoftAP started");
            break;
        case SYSTEM_EVENT_AP_STOP:
            ESP_LOGI(TAG, "SoftAP stopped");
            break;
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "Got ip:%s",
                     ip4addr_ntoa((const ip4_addr_t *)&event->event_info.got_ip.ip_info.ip));
            wifi_serv_state_send(serv->wifi_serv_que, WIFI_SERV_EVENT_CONNECTED, 0, 0, 0);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            if (serv->reason == WIFI_SERV_STA_BY_USER || serv->reason == WIFI_SERV_STA_SET_INFO) {
                ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED, reason is %d", serv->reason);
                wifi_serv_state_send(serv->wifi_serv_que, WIFI_SERV_EVENT_DISCONNECTED, 0, 0, 0);
                break;
            }

            switch (event->event_info.disconnected.reason) {
                case WIFI_REASON_AUTH_EXPIRE:
                case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
                case WIFI_REASON_BEACON_TIMEOUT:
                case WIFI_REASON_AUTH_FAIL:
                case WIFI_REASON_ASSOC_FAIL:
                case WIFI_REASON_HANDSHAKE_TIMEOUT:
                    ESP_LOGI(TAG, "STA Auth Error, reason:%d", event->event_info.disconnected.reason);
                    serv->reason = WIFI_SERV_STA_AUTH_ERROR;
                    break;
                case WIFI_REASON_NO_AP_FOUND:
                    ESP_LOGI(TAG, "STA AP Not found");
                    serv->reason = WIFI_SERV_STA_AP_NOT_FOUND;
                    break;
                default:
                    ESP_LOGI(TAG, "STA Error, reason:%d", event->event_info.disconnected.reason);
                    serv->reason = WIFI_SERV_STA_COM_ERROR;
                    break;
            }
            wifi_serv_state_send(serv->wifi_serv_que, WIFI_SERV_EVENT_DISCONNECTED, 0, 0, 0);
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            ESP_LOGI(TAG, "Station:"MACSTR" join, AID=%d",
                     MAC2STR(event->event_info.sta_connected.mac),
                     event->event_info.sta_connected.aid);
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            ESP_LOGI(TAG, "Station:"MACSTR"leave, AID=%d",
                     MAC2STR(event->event_info.sta_disconnected.mac),
                     event->event_info.sta_disconnected.aid);
            break;
        default:
            break;
    }
    return ESP_OK;
}
#endif

esp_err_t configure_wifi_sta_mode(wifi_config_t *wifi_cfg)
{
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, wifi_cfg));
    return ESP_OK;
}

static void wifi_sta_setup(void *para)
{
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
    ESP_ERROR_CHECK(esp_event_loop_create_default());
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 1, 0))
    esp_netif_create_default_wifi_sta();
#endif
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_cb, para));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_cb, para));
#else
#include "esp_event_loop.h"
    if (esp_event_loop_get_queue() == NULL) {
        ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_cb, para));
    } else {
        esp_event_loop_set_cb(wifi_event_cb, para);
    }
#endif
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
}

static esp_err_t wifi_init(void *instance, action_arg_t *arg, action_result_t *result)
{
    wifi_config_t wifi_cfg = {0};

    wifi_sta_setup(instance);
    configure_wifi_sta_mode(&wifi_cfg);
    ESP_ERROR_CHECK(esp_wifi_start());
    return ESP_OK;
}

static void setting_timeout_callback(void *timer_arg)
{
    periph_service_handle_t serv_handle = (periph_service_handle_t)timer_arg;
    wifi_service_t *serv = periph_service_get_data(serv_handle);
    wifi_serv_state_send(serv->wifi_serv_que, WIFI_SERV_EVENT_SETTING_TIMEOUT, 0, 0, 0);
}

static void retry_timer_callback(void *timer_arg)
{
    periph_service_handle_t serv_handle = (periph_service_handle_t)timer_arg;
    wifi_service_connect(serv_handle);
}

static void wifi_task(void *pvParameters)
{
    periph_service_handle_t serv_handle = (periph_service_handle_t)pvParameters;
    wifi_service_t *serv = periph_service_get_data(serv_handle);
    wifi_config_t wifi_cfg = {0};
    wifi_task_msg_t wifi_msg = {0};
    bool task_run = true;
    periph_service_event_t cb_evt = {0};
    wifi_setting_item_t *item;
    wifi_config_t *stored_ssid = NULL;

    esp_dispatcher_handle_t dispatcher = esp_dispatcher_get_delegate_handle();
    action_result_t result = { 0 };
    esp_dispatcher_execute_with_func(dispatcher, wifi_init, pvParameters, NULL, &result);

    esp_timer_create_args_t tmr_args = {
        .callback = &setting_timeout_callback,
        .arg = serv_handle,
        .name = "setting_timeout",
    };
    esp_timer_create(&tmr_args, &serv->setting_timer);

    esp_timer_create_args_t retry_args = {
        .callback = &retry_timer_callback,
        .arg = serv_handle,
        .name = "retry_timer",
    };
    esp_timer_create(&retry_args, &serv->retry_timer);

    uint64_t retry_interval = 0;
    serv->retry_times = 0;
    while (task_run) {
        if (xQueueReceive(serv->wifi_serv_que, &wifi_msg, portMAX_DELAY)) {
            if (wifi_msg.msg_type == WIFI_SERV_EVENT_TYPE_STATE) {
                cb_evt.type = wifi_msg.type;
                cb_evt.source = serv_handle;
                cb_evt.data = wifi_msg.pdata;
                cb_evt.len = wifi_msg.len;
                ESP_LOGW(TAG, "STATE type:%d, pdata:%p, len:%d", wifi_msg.type, wifi_msg.pdata, wifi_msg.len);
                if (wifi_msg.type == WIFI_SERV_EVENT_SETTING_TIMEOUT) {
                    ESP_LOGI(TAG, "WIFI_SERV_EVENT_SETTING_TIMEOUT");
                    STAILQ_FOREACH(item, &serv->setting_list, next) {
                        if (item->running) {
                            esp_wifi_setting_stop(item->on_handle);
                        }
                    }
                    wifi_service_connect(serv_handle);
                    serv->reason = WIFI_SERV_STA_UNKNOWN;
                }
                if (wifi_msg.type == WIFI_SERV_EVENT_CONNECTED) {
                    serv->reason = WIFI_SERV_STA_UNKNOWN;
                    serv->retry_times = 0;
                    serv->prov_retry_times = 0;
                    serv->retrying = false;
                    if (serv->is_setting) {
                        esp_timer_stop(serv->setting_timer);
                        STAILQ_FOREACH(item, &serv->setting_list, next) {
                            if (item->running == false) {
                                continue;
                            }
                            item->running = false;
                            esp_wifi_setting_teardown(item->on_handle, NULL);
                        }
                        serv->is_setting = false;
                    }
                    wifi_ssid_manager_save(serv->ssid_manager, (const char *)wifi_cfg.sta.ssid,  (const char *)wifi_cfg.sta.password);
                }
                if (wifi_msg.type == WIFI_SERV_EVENT_DISCONNECTED) {
                    if ((serv->reason != WIFI_SERV_STA_BY_USER) && (serv->reason != WIFI_SERV_STA_SET_INFO)) {
                        retry_interval = serv->retry_times * 1000 * 1000 * 2;
                        if (retry_interval > 60 * 1000 * 1000) { // Longest interval is 60s
                            retry_interval = 60 * 1000 * 1000;
                        }
                        // reconnect the SSID
                        if (serv->max_retry_time >= 0) {
                            if (serv->retry_times < serv->max_retry_time) {
                                serv->retry_times++;
                                serv->retrying = true;
                                esp_timer_start_once(serv->retry_timer, retry_interval);
                            } else {
                                ESP_LOGW(TAG, "Reconnect wifi failed, retry times is %d", serv->retry_times);
                                serv->retrying = false;
                                if (wifi_ssid_manager_get_ssid_num(serv->ssid_manager) > 1) {
                                    ESP_LOGW(TAG, "Try to connect to ssid stored in flash ...");
                                    if (wifi_ssid_manager_get_best_config(serv->ssid_manager, &wifi_cfg) == ESP_OK) {
                                        serv->retry_times = 0;
                                        serv->retrying = true;
                                        ESP_LOGI(TAG, "Connect to stored wifi ssid: %s, pwd: %s", wifi_cfg.sta.ssid, wifi_cfg.sta.password);
                                        configure_wifi_sta_mode(&wifi_cfg);
                                        esp_timer_start_once(serv->retry_timer, retry_interval);
                                    }
                                }
                            }
                        } else {
                            serv->retrying = true;
                            serv->retry_times ++;
                            // The time interval for reconnection will gradually increase. At the maximum, the connection will be once a minute.
                            esp_timer_start_once(serv->retry_timer, retry_interval);
                            ESP_LOGW(TAG, "Got max_retry_time = %d, the station will try to reconnect until connected", serv->max_retry_time);
                        }
                        ESP_LOGW(TAG, "Disconnect reason %d", serv->reason);
                    } else if (serv->reason == WIFI_SERV_STA_SET_INFO) {
                        if (serv->prov_retry_times < serv->max_prov_retry_time) {
                            vTaskDelay(2000 / portTICK_PERIOD_MS);
                            configure_wifi_sta_mode(&wifi_cfg);
                            wifi_service_connect(serv_handle);
                            serv->prov_retry_times ++;
                            ESP_LOGW(TAG, "Fail to connect to provision wifi, reason: %d, start to reconnect ...", serv->reason);
                        } else {
                            serv->prov_retry_times = 0;
                            serv->reason = WIFI_SERV_STA_UNKNOWN;
                            ESP_LOGE(TAG, "Please configure wifi again");
                            cb_evt.type = WIFI_SERV_EVENT_SETTING_FAILED;
                        }
                    } else if (serv->reason == WIFI_SERV_STA_BY_USER) {
                        serv->reason = WIFI_SERV_STA_UNKNOWN;
                    }
                }
                periph_service_callback(serv_handle, &cb_evt);
            } else if (wifi_msg.msg_type == WIFI_SERV_EVENT_TYPE_CMD) {
                if (wifi_msg.type == WIFI_SERV_CMD_CONNECT) {
                    if (serv->reason != WIFI_SERV_STA_SET_INFO) {
                        if (wifi_ssid_manager_get_latest_config(serv->ssid_manager, &wifi_cfg) != ESP_OK) {
                            ESP_LOGW(TAG, "No ssid stored in flash, try to connect to wifi set by wifi_service_set_sta_info()");
                            if (serv->info.sta.ssid[0] == 0) {
                                ESP_LOGW(TAG, "There is no preset ssid, please set the wifi first");
                                continue;
                            }
                            memcpy(&wifi_cfg, &serv->info, sizeof(wifi_config_t));
                        }
                    }
                    ESP_LOGI(TAG, "Connect to wifi ssid: %s, pwd: %s", wifi_cfg.sta.ssid, wifi_cfg.sta.password);
                    configure_wifi_sta_mode(&wifi_cfg);
                    ESP_ERROR_CHECK(esp_wifi_connect());
                } else if (wifi_msg.type == WIFI_SERV_CMD_DISCONNECT) {
                    serv->reason = WIFI_SERV_STA_BY_USER;
                    ESP_LOGI(TAG, "WIFI_SERV_CMD_DISCONNECT");
                    esp_wifi_disconnect();
                } else if (wifi_msg.type == WIFI_SERV_CMD_SETTING_START) {
                    int idx = (int)wifi_msg.pdata;
                    ESP_LOGI(TAG, "WIFI_SERV_CMD_SETTING_START,index:%d", idx);
                    serv->reason = WIFI_SERV_STA_BY_USER;
                    esp_wifi_disconnect();
                    if (serv->retrying == true) {
                        esp_timer_stop(serv->retry_timer);
                    }
                    STAILQ_FOREACH(item, &serv->setting_list, next) {
                        if (idx == 0) {
                            esp_wifi_setting_start(item->on_handle);
                            item->running = true;
                        } else if (item->index == idx) {
                            esp_wifi_setting_start(item->on_handle);
                            item->running = true;
                            break;
                        }
                    }
                    serv->is_setting = true;
                    esp_timer_start_once(serv->setting_timer, (uint64_t)serv->setting_timeout_s * 1000 * 1000);
                } else if (wifi_msg.type == WIFI_SERV_CMD_SETTING_STOP)  {
                    int idx = (int)wifi_msg.pdata;
                    ESP_LOGI(TAG, "WIFI_SERV_CMD_SETTING_STOP,index:%d", idx);
                    esp_timer_stop(serv->setting_timer);
                    STAILQ_FOREACH(item, &serv->setting_list, next) {
                        if (idx == 0) {
                            esp_wifi_setting_stop(item->on_handle);
                        } else if (item->index == idx) {
                            esp_wifi_setting_stop(item->on_handle);
                            break;
                        }
                    }
                    serv->is_setting = false;
                } else if (wifi_msg.type == WIFI_SERV_CMD_DESTROY) {
                    task_run = false;
                    ESP_LOGI(TAG, "WIFI_SERV_CMD_DESTROY");
                } else if (wifi_msg.type == WIFI_SERV_CMD_UPDATE) {
                    wifi_config_t *info = (wifi_config_t *)wifi_msg.pdata;
                    if (serv->is_setting) {
                        ESP_LOGI(TAG, "WIFI_SERV_CMD_UPDATE got ssid: %s, pwd: %s", info->sta.ssid, info->sta.password);
                        serv->reason = WIFI_SERV_STA_SET_INFO;
                        memcpy(&wifi_cfg, info, sizeof(wifi_config_t));
                        configure_wifi_sta_mode(&wifi_cfg);
                        esp_wifi_connect();
                        audio_free(info);

                        cb_evt.type = WIFI_SERV_EVENT_SETTING_FINISHED;
                        cb_evt.source = serv_handle;
                        cb_evt.data = NULL;
                        cb_evt.len = 0;
                        periph_service_callback(serv_handle, &cb_evt);
                    } else {
                        ESP_LOGW(TAG, "Not setting state, ignore the wifi information, ssid: %s, pwd: %s", info->sta.ssid, info->sta.password);
                    }
                }
            } else {
                ESP_LOGI(TAG, "Not supported event type");
            }
        }
    }
    esp_timer_stop(serv->setting_timer);
    esp_timer_delete(serv->setting_timer);
    esp_timer_stop(serv->retry_timer);
    esp_timer_delete(serv->retry_timer);
    serv->setting_timer = NULL;
    serv->retry_timer = NULL;
    if (stored_ssid) {
        audio_free(stored_ssid);
        stored_ssid = NULL;
    }
    xEventGroupSetBits(serv->sync_evt, WIFI_TASK_DESTROY_BIT);
    vTaskDelete(NULL);
}

static esp_err_t _wifi_start(periph_service_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    return wifi_service_connect(handle);
}

static esp_err_t _wifi_stop(periph_service_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    return wifi_service_disconnect(handle);
}

esp_err_t wifi_service_register_setting_handle(periph_service_handle_t handle, esp_wifi_setting_handle_t setting, int *out_index)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    wifi_setting_item_t *item = audio_calloc(1, sizeof(wifi_setting_item_t));
    AUDIO_MEM_CHECK(TAG, item, {
        return ESP_FAIL;
    });
    wifi_service_t *serv = periph_service_get_data(handle);
    *out_index = serv->setting_index++;
    item->index = serv->setting_index;
    item->on_handle = setting;
    STAILQ_INSERT_TAIL(&serv->setting_list, item, next);
    return ESP_OK;
}

esp_err_t wifi_service_setting_start(periph_service_handle_t handle, int index)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    wifi_service_t *serv = periph_service_get_data(handle);
    wifi_serv_cmd_send(serv->wifi_serv_que, WIFI_SERV_CMD_SETTING_START, (void *)index, 0, 0);
    return ESP_OK;
}

esp_err_t wifi_service_update_sta_info(periph_service_handle_t handle, wifi_config_t *wifi_conf)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    wifi_service_t *serv = periph_service_get_data(handle);
    wifi_config_t *conf = (wifi_config_t *)audio_calloc(1, sizeof(wifi_config_t));
    AUDIO_NULL_CHECK(TAG, conf, return ESP_FAIL);
    memcpy(conf, wifi_conf, sizeof(wifi_config_t));
    wifi_serv_cmd_send(serv->wifi_serv_que, WIFI_SERV_CMD_UPDATE, conf, 0, 0);
    return ESP_OK;
}

esp_err_t wifi_service_setting_stop(periph_service_handle_t handle, int index)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    wifi_service_t *serv = periph_service_get_data(handle);
    wifi_serv_cmd_send(serv->wifi_serv_que, WIFI_SERV_CMD_SETTING_STOP, (void *)index, 0, 0);
    return ESP_OK;
}

esp_err_t wifi_service_connect(periph_service_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    wifi_service_t *serv = periph_service_get_data(handle);
    wifi_serv_cmd_send(serv->wifi_serv_que, WIFI_SERV_CMD_CONNECT, 0, 0, 0);
    return ESP_OK;
}

esp_err_t wifi_service_disconnect(periph_service_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    wifi_service_t *serv = periph_service_get_data(handle);
    wifi_serv_cmd_send(serv->wifi_serv_que, WIFI_SERV_CMD_DISCONNECT, 0, 0, 0);
    return ESP_OK;
}

esp_err_t wifi_service_set_sta_info(periph_service_handle_t handle, wifi_config_t *info)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    wifi_service_t *serv = periph_service_get_data(handle);
    memcpy(&(serv->info.sta), &info->sta, sizeof(wifi_sta_config_t));
    return ESP_OK;
}

periph_service_state_t wifi_service_state_get(periph_service_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    wifi_service_t *serv = periph_service_get_data(handle);
    return serv->wifi_serv_state;
}

wifi_service_disconnect_reason_t wifi_service_disconnect_reason_get(periph_service_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    wifi_service_t *serv = periph_service_get_data(handle);
    return serv->reason;
}

esp_err_t wifi_service_destroy(periph_service_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    wifi_service_t *serv = periph_service_get_data(handle);
    xEventGroupClearBits(serv->sync_evt, WIFI_TASK_DESTROY_BIT);
    wifi_serv_cmd_send(serv->wifi_serv_que, WIFI_SERV_CMD_DESTROY, 0, 0, 0);
    EventBits_t uxBits = xEventGroupWaitBits(serv->sync_evt, WIFI_TASK_DESTROY_BIT, false, true, portMAX_DELAY);
    esp_err_t ret = ESP_FAIL;
    if (uxBits & WIFI_TASK_DESTROY_BIT) {
        ret = ESP_OK;
    }
    vQueueDelete(serv->wifi_serv_que);
    vEventGroupDelete(serv->sync_evt);
    audio_free(serv);
    periph_service_destroy(handle);
    return ret;
}

esp_err_t wifi_service_erase_ssid_manager_info(periph_service_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_OK;
    wifi_service_t *serv = periph_service_get_data(handle);
    if (serv->ssid_manager) {
        ret = wifi_ssid_manager_erase_all(serv->ssid_manager);
        if (ret == ESP_OK) {
            ESP_LOGW(TAG, "Erase all the ssid information stored in flash");
        }
    }
    return ret;
}

esp_err_t wifi_service_get_last_ssid_cfg(periph_service_handle_t handle, wifi_config_t* wifi_cfg)
{
    wifi_service_t *serv = periph_service_get_data(handle);
    return wifi_ssid_manager_get_latest_config(serv->ssid_manager, wifi_cfg);
}

periph_service_handle_t wifi_service_create(wifi_service_config_t *config)
{
    wifi_service_t *serv =  audio_calloc(1, sizeof(wifi_service_t));
    AUDIO_MEM_CHECK(TAG, serv, return NULL);

    serv->ssid_manager = wifi_ssid_manager_create(config->max_ssid_num);
    AUDIO_MEM_CHECK(TAG, serv->ssid_manager, {
        audio_free(serv);
        return NULL;
    });
    serv->max_retry_time = config->max_retry_time;
    serv->max_prov_retry_time = config->max_prov_retry_time;

    STAILQ_INIT(&serv->setting_list);
    serv->wifi_serv_que = xQueueCreate(3, sizeof(wifi_task_msg_t));
    AUDIO_MEM_CHECK(TAG, serv->wifi_serv_que, {
        wifi_ssid_manager_destroy(serv->ssid_manager);
        audio_free(serv);
        return NULL;
    });
    serv->sync_evt = xEventGroupCreate();
    AUDIO_MEM_CHECK(TAG, serv->sync_evt, {
        vQueueDelete(serv->wifi_serv_que);
        wifi_ssid_manager_destroy(serv->ssid_manager);
        audio_free(serv);
        return NULL;
    });

    serv->wifi_serv_state = WIFI_SERV_EVENT_UNKNOWN;
    serv->setting_index = 0;
    serv->setting_timeout_s = config->setting_timeout_s;
    periph_service_config_t cfg = {
        .task_stack = config->task_stack,
        .task_prio  = config->task_prio,
        .task_core  = config->task_core,
        .extern_stack = config->extern_stack,
        .task_func  = wifi_task,
        .service_start = _wifi_start,
        .service_stop = _wifi_stop,
        .service_ioctl = NULL,
        .service_destroy = wifi_service_destroy,
        .service_name = "wifi_serv",
        .user_data = (void *)serv,
    };

    periph_service_handle_t wifi = periph_service_create(&cfg);
    AUDIO_MEM_CHECK(TAG, wifi, {
        vQueueDelete(serv->wifi_serv_que);
        vEventGroupDelete(serv->sync_evt);
        wifi_ssid_manager_destroy(serv->ssid_manager);
        audio_free(serv);
        return NULL;
    });
    periph_service_set_callback(wifi, config->evt_cb, config->cb_ctx);

    return wifi;
}

