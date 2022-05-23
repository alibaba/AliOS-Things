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
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "audio_mem.h"
#include "audio_sys.h"
#include "battery_service.h"

typedef struct battery_service {
    xQueueHandle serv_q;
    EventGroupHandle_t sync_events;

    vol_monitor_handle_t vol_monitor;

    bool running;
} battery_service_t;

typedef struct battery_cmd {
    enum battery_service_msg {
        /* battery service ctrl */
        BATTERY_SERVICE_START,
        BATTERY_SERVICE_STOP,
        BATTERY_SERVICE_DESTROY,

        /* battery voltage monitor ctrl */
        BATTERY_VOL_SET_REPORT_FREQ,
        BATTERY_VOL_REPORT_START,
        BATTERY_VOL_REPORT_STOP,
        BATTERY_VOL_REPORT_FREQ,
        BATTERY_VOL_REPORT_FULL,
        BATTERY_VOL_REPORT_LOW,

        /* battery charger monitor ctrl - todo*/
    } msg_id;
    void *pdata;
} battery_msg_t;

#define BATTERY_SERV_SYNC_STARTED   (BIT0)
#define BATTERY_SERV_SYNC_STOPPED   (BIT1)
#define BATTERY_SERV_SYNC_DESTROYED (BIT2)

static const char *TAG = "BATTERY_SERVICE";

static esp_err_t battery_service_msg_send(void *queue, int msg_id, void *pdata)
{
    battery_msg_t msg = { 0 };
    msg.msg_id = msg_id;
    msg.pdata = pdata;

    if (xQueueSend(queue, &msg, 0) != pdPASS) {
        ESP_LOGE(TAG, "msg send failed %d", msg.msg_id);
        return ESP_FAIL;
    }

    return ESP_OK;
}

static void battery_vol_monitor_cb(int msg, void *msg_data, void *user_ctx)
{
    switch (msg) {
        case VOL_MONITOR_EVENT_FREQ_REPORT:
            battery_service_msg_send(user_ctx, BATTERY_VOL_REPORT_FREQ, msg_data);
            break;
        case VOL_MONITOR_EVENT_BAT_FULL:
            battery_service_msg_send(user_ctx, BATTERY_VOL_REPORT_FULL, msg_data);
            break;
        case VOL_MONITOR_EVENT_BAT_LOW:
            battery_service_msg_send(user_ctx, BATTERY_VOL_REPORT_LOW, msg_data);
            break;
        default:
            break;
    }
}

static void battery_task(void *pvParameters)
{
    periph_service_handle_t serv_handle = (periph_service_handle_t)pvParameters;
    battery_service_t *service = periph_service_get_data(serv_handle);
    periph_service_event_t evt = { 0 };
    battery_msg_t msg = { 0 };

    service->running = true;
    while (service->running) {
        if (xQueueReceive(service->serv_q, &msg, portMAX_DELAY) == pdTRUE) {
            switch (msg.msg_id) {
                case BATTERY_SERVICE_START: {
                    if (service->vol_monitor != NULL && vol_monitor_set_event_cb(service->vol_monitor, battery_vol_monitor_cb, service->serv_q) == ESP_OK) {
                        xEventGroupSetBits(service->sync_events, BATTERY_SERV_SYNC_STARTED);
                    } else {
                        xEventGroupSetBits(service->sync_events, BATTERY_SERV_SYNC_STOPPED);
                    }
                    break;
                }
                case BATTERY_SERVICE_DESTROY:
                    service->running = false;
                    FALL_THROUGH;
                    // No break, to share the actions of case `BATTERY_SERVICE_STOP`, clear all the monitors.
                case BATTERY_SERVICE_STOP: {
                    if (service->vol_monitor != NULL) {
                        vol_monitor_set_event_cb(service->vol_monitor, NULL, NULL);
                    }
                    xEventGroupSetBits(service->sync_events, BATTERY_SERV_SYNC_STOPPED);
                    break;
                }
                /* battery voltage monitor ctrl begin */
                case BATTERY_VOL_SET_REPORT_FREQ: {
                    if (service->vol_monitor != NULL) {
                        vol_monitor_set_report_freq(service->vol_monitor, (int)msg.pdata);
                    }
                    break;
                }
                case BATTERY_VOL_REPORT_START:
                    if (service->vol_monitor != NULL) {
                        vol_monitor_start_freq_report(service->vol_monitor);
                    }
                    break;
                case BATTERY_VOL_REPORT_STOP:
                    if (service->vol_monitor != NULL) {
                        vol_monitor_stop_freq_report(service->vol_monitor);
                    }
                    break;
                case BATTERY_VOL_REPORT_FREQ: {
                    evt.type = BAT_SERV_EVENT_VOL_REPORT;
                    evt.data = msg.pdata;
                    periph_service_callback(serv_handle, &evt);
                    break;
                }
                case BATTERY_VOL_REPORT_FULL: {
                    evt.type = BAT_SERV_EVENT_BAT_FULL;
                    evt.data = msg.pdata;
                    periph_service_callback(serv_handle, &evt);
                    break;
                }
                case BATTERY_VOL_REPORT_LOW: {
                    evt.type = BAT_SERV_EVENT_BAT_LOW;
                    evt.data = msg.pdata;
                    periph_service_callback(serv_handle, &evt);
                    break;
                }
                /* battery voltage monitor ctrl end */
                default:
                    break;
            }
        }
    }

    xEventGroupSetBits(service->sync_events, BATTERY_SERV_SYNC_DESTROYED);
    ESP_LOGI(TAG, "battery service destroyed");
    vTaskDelete(NULL);
}

static esp_err_t _battery_destroy(periph_service_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    battery_service_t *service = periph_service_get_data(handle);
    battery_service_msg_send(service->serv_q, BATTERY_SERVICE_DESTROY, NULL);

    EventBits_t bits = xEventGroupWaitBits(service->sync_events,
        BATTERY_SERV_SYNC_DESTROYED,
        true,
        true,
        pdMS_TO_TICKS(10000));
    if (bits & BATTERY_SERV_SYNC_DESTROYED) {
        if (service->serv_q) {
            vQueueDelete(service->serv_q);
        }
        if (service->sync_events) {
            vEventGroupDelete(service->sync_events);
        }
        free(service);

        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

static esp_err_t _battery_start(periph_service_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    battery_service_t *service = periph_service_get_data(handle);
    battery_service_msg_send(service->serv_q, BATTERY_SERVICE_START, NULL);

    EventBits_t bits = xEventGroupWaitBits(service->sync_events,
        BATTERY_SERV_SYNC_STARTED | BATTERY_SERV_SYNC_STOPPED,
        true,
        false,
        portMAX_DELAY);
    if (bits & BATTERY_SERV_SYNC_STARTED) {
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

static esp_err_t _battery_stop(periph_service_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    battery_service_t *service = periph_service_get_data(handle);
    battery_service_msg_send(service->serv_q, BATTERY_SERVICE_STOP, NULL);

    EventBits_t bits = xEventGroupWaitBits(service->sync_events,
        BATTERY_SERV_SYNC_STOPPED,
        true,
        true,
        portMAX_DELAY);
    if (bits & BATTERY_SERV_SYNC_STOPPED) {
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

periph_service_handle_t battery_service_create(battery_service_config_t *config)
{
    AUDIO_NULL_CHECK(TAG, config, return NULL);
    AUDIO_CHECK(TAG, config->task_stack > 0, return NULL, "battery service should set the task_stack");

    battery_service_t *battery_service = audio_calloc(1, sizeof(battery_service_t));
    AUDIO_MEM_CHECK(TAG, battery_service, return NULL);

    battery_service->serv_q = xQueueCreate(3, sizeof(battery_msg_t));
    AUDIO_MEM_CHECK(TAG, battery_service->serv_q, {
        goto err;
    });
    battery_service->sync_events = xEventGroupCreate();
    AUDIO_MEM_CHECK(TAG, battery_service->sync_events, {
        goto err;
    });

    battery_service->vol_monitor = config->vol_monitor;

    periph_service_config_t cfg = {
        .task_stack = config->task_stack,
        .task_prio = config->task_prio,
        .task_core = config->task_core,
        .extern_stack = config->extern_stack,
        .task_func = battery_task,
        .service_start = _battery_start,
        .service_stop = _battery_stop,
        .service_ioctl = NULL,
        .service_destroy = _battery_destroy,
        .service_name = "battery_serv",
        .user_data = (void *)battery_service,
    };

    periph_service_handle_t battery = periph_service_create(&cfg);
    AUDIO_MEM_CHECK(TAG, battery, {
        goto err;
    });
    periph_service_set_callback(battery, config->evt_cb, config->cb_ctx);

    return battery;

err:
    if (battery_service && battery_service->serv_q) {
        vQueueDelete(battery_service->serv_q);
    }
    if (battery_service && battery_service->sync_events) {
        vEventGroupDelete(battery_service->sync_events);
    }
    if (battery_service) {
        free(battery_service);
    }
    return NULL;
}

esp_err_t battery_service_vol_report_switch(periph_service_handle_t handle, bool on_off)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    battery_service_t *service = periph_service_get_data(handle);
    if (on_off) {
        battery_service_msg_send(service->serv_q, BATTERY_VOL_REPORT_START, NULL);
    } else {
        battery_service_msg_send(service->serv_q, BATTERY_VOL_REPORT_STOP, NULL);
    }
    return ESP_OK;
}

esp_err_t battery_service_set_vol_report_freq(periph_service_handle_t handle, int freq)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    battery_service_t *service = periph_service_get_data(handle);

    battery_service_msg_send(service->serv_q, BATTERY_VOL_SET_REPORT_FREQ, (void *)freq);
    return ESP_OK;
}
