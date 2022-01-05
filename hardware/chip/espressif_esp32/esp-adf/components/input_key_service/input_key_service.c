
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
#include "audio_error.h"
#include "audio_mem.h"
#include "sys/queue.h"
#include "esp_peripherals.h"
#include "input_key_service.h"
#include "audio_event_iface.h"

static const char *TAG = "INPUT_KEY_SERVICE";

typedef struct input_key {
    input_key_service_info_t    input_key_info;
    STAILQ_ENTRY(input_key)     entries;
} input_key_node_t;

typedef struct {
    periph_service_state_t              ser_state;
    esp_periph_set_handle_t             periph_set_handle;
    STAILQ_HEAD(key_list, input_key)    input_info_list;
} input_key_service_t;

static esp_err_t input_key_service_event_send(periph_service_handle_t input_key_handle, periph_service_state_t state)
{
    AUDIO_NULL_CHECK(TAG, input_key_handle, return ESP_ERR_INVALID_ARG);

    input_key_service_t *input_key_ser = (input_key_service_t *)periph_service_get_data(input_key_handle);
    QueueHandle_t input_ser_queue = esp_periph_set_get_queue(input_key_ser->periph_set_handle);

    audio_event_iface_msg_t msg = {0};
    msg.source = (void *)input_key_handle;
    msg.cmd = state;

    if (xQueueSend(input_ser_queue, &msg, portMAX_DELAY) != pdTRUE) {
        ESP_LOGW(TAG, "input key service event send failed");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t input_key_service_event_receive(periph_service_handle_t handle, void *msg, TickType_t ticks)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);

    input_key_service_t *input_key_ser = periph_service_get_data(handle);
    QueueHandle_t input_ser_queue = esp_periph_set_get_queue(input_key_ser->periph_set_handle);

    if (xQueueReceive(input_ser_queue, msg, ticks) != pdTRUE) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

static int get_input_key_user_id(input_key_service_t *input_key_ser, int source_type, int act_id)
{
    AUDIO_NULL_CHECK(TAG, input_key_ser, return INPUT_KEY_USER_ID_UNKNOWN);

    input_key_service_info_t *key_info = NULL;
    input_key_node_t *tmp_node = NULL;

    STAILQ_FOREACH(tmp_node, &input_key_ser->input_info_list, entries) {
        key_info = &(tmp_node->input_key_info);
        if (key_info->type == source_type && key_info->act_id == act_id) {
            return key_info->user_id;
        }
    }

    return INPUT_KEY_USER_ID_UNKNOWN;
}

static void input_key_service_task(void *parameters)
{
    periph_service_handle_t input_key_handle = (periph_service_handle_t)parameters;

    periph_service_event_t ser_evt = {0};
    input_key_service_t *input_key_ser = periph_service_get_data(input_key_handle);

    input_key_ser->ser_state = PERIPH_SERVICE_STATE_RUNNING;
    audio_event_iface_msg_t msg = {0};

    while (1) {
        if (input_key_service_event_receive(input_key_handle, &msg, portMAX_DELAY) == ESP_OK) {
            ser_evt.data = (void *)get_input_key_user_id(input_key_ser, msg.source_type, (int)msg.data);
            if ((int)ser_evt.data >= 0) {
                ser_evt.type = msg.cmd; // action
                ser_evt.source = (void *)msg.source_type;
                ser_evt.len = (int)msg.data;
                periph_service_callback(input_key_handle, &ser_evt);
            }
        }

        if (msg.source == input_key_handle && msg.cmd == PERIPH_SERVICE_STATE_STOPPED) {
            ESP_LOGW(TAG, "The input key service will be destroyed");
            break;
        }
    }

    input_key_node_t *item, *tmp;
    STAILQ_FOREACH_SAFE(item, &input_key_ser->input_info_list, entries, tmp) {
        STAILQ_REMOVE(&input_key_ser->input_info_list, item, input_key, entries);
        audio_free(item);
    }
    audio_free(input_key_ser);
    vTaskDelete(NULL);
}

static esp_err_t input_key_service_start(periph_service_handle_t input_key_handle)
{
    return ESP_OK;
}

static esp_err_t input_key_service_stop(periph_service_handle_t input_key_handle)
{
    return ESP_OK;
}

static esp_err_t input_key_service_destroy(periph_service_handle_t input_key_handle)
{
    AUDIO_NULL_CHECK(TAG, input_key_handle, return ESP_ERR_INVALID_ARG);
    input_key_service_event_send(input_key_handle, PERIPH_SERVICE_STATE_STOPPED);
    return ESP_OK;
}

periph_service_state_t get_input_key_service_state(periph_service_handle_t input_key_handle)
{
    AUDIO_NULL_CHECK(TAG, input_key_handle, return PERIPH_SERVICE_STATE_UNKNOWN);
    input_key_service_t *input_key_ser = (input_key_service_t *)periph_service_get_data(input_key_handle);
    return input_key_ser->ser_state;
}

esp_err_t input_key_service_add_key(periph_service_handle_t input_key_handle, input_key_service_info_t *input_key_info, int add_key_num)
{
    AUDIO_NULL_CHECK(TAG, input_key_handle, return ESP_ERR_INVALID_ARG);
    AUDIO_NULL_CHECK(TAG, input_key_info, return ESP_ERR_INVALID_ARG);
    if (add_key_num <= 0) {
        return ESP_FAIL;
    }

    input_key_service_t *input_key_ser = periph_service_get_data(input_key_handle);
    AUDIO_NULL_CHECK(TAG, input_key_ser, return ESP_FAIL);

    for (int i = 0; i < add_key_num; i++) {
        input_key_node_t *input_key_node = (input_key_node_t *)audio_calloc(1, sizeof(input_key_node_t));
        AUDIO_NULL_CHECK(TAG, input_key_node, return ESP_FAIL);
        memcpy(&input_key_node->input_key_info, &input_key_info[i], sizeof(input_key_service_info_t));
        STAILQ_INSERT_TAIL(&input_key_ser->input_info_list, input_key_node, entries);
    }
    return ESP_OK;
}

periph_service_handle_t input_key_service_create(input_key_service_cfg_t *input_key_config)
{
    AUDIO_NULL_CHECK(TAG, input_key_config, return NULL);
    periph_service_config_t *input_cfg = &input_key_config->based_cfg;

    periph_service_config_t service_cfg = {
        .task_stack = input_cfg->task_stack,
        .task_prio  = input_cfg->task_prio,
        .task_core  = input_cfg->task_core,
        .extern_stack = input_cfg->extern_stack,
        .task_func  = input_key_service_task,
        .service_start = input_key_service_start,
        .service_stop = input_key_service_stop,
        .service_destroy = input_key_service_destroy,
        .service_ioctl = NULL,
        .service_name = "input_key_service",
    };

    input_key_service_t *input_key_ser = NULL;
    periph_service_handle_t input_key_handle = NULL;

    input_key_ser = (input_key_service_t *)audio_calloc(1, sizeof(input_key_service_t));
    AUDIO_NULL_CHECK(TAG, input_key_ser, goto _create_service_failed);

    if (input_key_config->handle) {
        input_key_ser->periph_set_handle = input_key_config->handle;
    } else {
        ESP_LOGE(TAG, "peripherals set handle is NULL");
        free(input_key_ser);
        return NULL;
    }
    input_key_ser->ser_state = PERIPH_SERVICE_STATE_UNKNOWN;

    STAILQ_INIT(&input_key_ser->input_info_list);

    service_cfg.user_data = (void *)input_key_ser;

    input_key_handle = periph_service_create(&service_cfg);
    AUDIO_NULL_CHECK(TAG, input_key_handle, goto _create_service_failed);

    return input_key_handle;

_create_service_failed:
    if (input_key_handle) {
        audio_free(input_key_handle);
        input_key_handle = NULL;
    }
    if (input_key_ser) {
        audio_free(input_key_ser);
        input_key_ser = NULL;
    }
    return NULL;
}
