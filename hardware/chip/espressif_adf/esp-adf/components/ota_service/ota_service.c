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
#include "freertos/queue.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_log.h"
#include "audio_mem.h"
#include "ota_service.h"

typedef enum {
    OTA_INIT,
    OTA_IDLE,
    OTA_START,
    OTA_END,
    OTA_DESTROY,
    OTA_ERROR,
    OTA_STATE_END
} ota_status_t;

typedef struct {
    int state;
    xQueueHandle srv_q;
    ota_upgrade_ops_t *upgrade_list;
    int list_len;
    int result;
} ota_service_t;

typedef enum {
    OTA_SERVICE_CMD_START,
    OTA_SERVICE_CMD_DESTROY,
} ota_cmd_t;

typedef struct {
    ota_upgrade_ops_t *list;
    int len;
} ota_list_msg_t;

typedef struct {
    int cmd;
    void *pdata;
} ota_msg_t;

static const char *TAG = "OTA_SERVICE";

static void ota_service_cmd_send(void *que, int cmd, void *data, int dir)
{
    ota_msg_t evt = { 0 };
    evt.cmd = cmd;
    evt.pdata = data;
    if (dir) {
        xQueueSendToFront(que, &evt, 0);
    } else {
        xQueueSend(que, &evt, 0);
    }
}

static ota_service_err_reason_t ota_service_process(ota_upgrade_ops_t *upgrade_info)
{
    ota_service_err_reason_t ret = OTA_SERV_ERR_REASON_UNKNOWN;
    void *handle = NULL;

    AUDIO_NULL_CHECK(TAG, upgrade_info, return OTA_SERV_ERR_REASON_NULL_POINTER);
    AUDIO_NULL_CHECK(TAG, upgrade_info->prepare, return OTA_SERV_ERR_REASON_NULL_POINTER);
    AUDIO_NULL_CHECK(TAG, upgrade_info->need_upgrade, return OTA_SERV_ERR_REASON_NULL_POINTER);
    AUDIO_NULL_CHECK(TAG, upgrade_info->execute_upgrade, return OTA_SERV_ERR_REASON_NULL_POINTER);

    ret = upgrade_info->prepare(&handle, &upgrade_info->node);
    if (ret != OTA_SERV_ERR_REASON_SUCCESS) {
        ESP_LOGE(TAG, "OTA prepared fail");
        return ret;
    }
    ret = upgrade_info->need_upgrade(handle, &upgrade_info->node);
    if (ret != OTA_SERV_ERR_REASON_SUCCESS) {
        ESP_LOGE(TAG, "No need to upgrade");
        return ret;
    }
    ret = upgrade_info->execute_upgrade(handle, &upgrade_info->node);
    if (ret != OTA_SERV_ERR_REASON_SUCCESS) {
        ESP_LOGE(TAG, "Fail to execute upgrade");
        return ret;
    }

    AUDIO_CHECK(TAG, upgrade_info->finished_check != NULL, return OTA_SERV_ERR_REASON_NULL_POINTER, "finished_check should not be NULL");
    return upgrade_info->finished_check(handle, &upgrade_info->node, ret);
}

static void ota_task(void *pvParameters)
{
    periph_service_handle_t serv_handle = (periph_service_handle_t)pvParameters;
    ota_service_t *ota = periph_service_get_data(serv_handle);
    ota_msg_t msg = { 0 };
    periph_service_event_t ser_evt = { 0 };
    ota_result_t result_data = { 0 };

    ota->state = OTA_IDLE;
    while (true) {
        switch (ota->state) {
            case OTA_INIT:
            case OTA_IDLE: {
                ota->result = 0;
                if (xQueueReceive(ota->srv_q, &msg, portMAX_DELAY) == pdTRUE) {
                    switch (msg.cmd) {
                        case OTA_SERVICE_CMD_START: {
                            if (ota->upgrade_list == NULL || ota->list_len == 0) {
                                ESP_LOGE(TAG, "Please set upgrade list first");
                            } else {
                                ota->state = OTA_START;
                            }
                            break;
                        }
                        case OTA_SERVICE_CMD_DESTROY: {
                            ota->state = OTA_DESTROY;
                            break;
                        }
                        default:
                            break;
                    }
                }
                break;
            }
            case OTA_START: {
                ota_service_err_reason_t ret = OTA_SERV_ERR_REASON_UNKNOWN;
                for (int i = 0; i < ota->list_len; i++) {
                    ota_upgrade_ops_t *cur_node = &ota->upgrade_list[i];
                    ret = ota_service_process(cur_node);
                    if (ret == OTA_SERV_ERR_REASON_SUCCESS) {
                        ota->result |= (0x01 << i);
                    }
                    ser_evt.type = OTA_SERV_EVENT_TYPE_RESULT;
                    ser_evt.source = serv_handle;
                    result_data.result = ret;
                    result_data.id = i;
                    ser_evt.data = (void *)&result_data;
                    ser_evt.len = ota->list_len;
                    periph_service_callback(serv_handle, &ser_evt);
                    if (ret != OTA_SERV_ERR_REASON_SUCCESS && cur_node->break_after_fail) {
                        ESP_LOGE(TAG, "upgrade_list[%d] OTA failed, break the update list", i);
                        break;
                    }
                }
                ota->state = OTA_END;
                break;
            }
            case OTA_END: {
                ESP_LOGW(TAG, "OTA_END!");
                for (int i = 0; i < ota->list_len; i++) {
                    if (ota->result & (0x01 << i)) {
                        if (ota->upgrade_list[i].reboot_flag == true) {
                            ESP_LOGW(TAG, "restart!");
                            esp_restart();
                        }
                    }
                }
                memset(&ser_evt, 0, sizeof(periph_service_event_t));
                ser_evt.type = OTA_SERV_EVENT_TYPE_FINISH;
                periph_service_callback(serv_handle, &ser_evt);

                ota->state = OTA_IDLE;
                break;
            }
            case OTA_ERROR: {
                ESP_LOGE(TAG, "OTA_ERROR");
                ota->state = OTA_IDLE;
                break;
            }
            case OTA_DESTROY: {
                ESP_LOGE(TAG, "OTA_DESTROY");
                break;
            }
            default:
                ESP_LOGE(TAG, "Unkown OTA state %d", ota->state);
                ota->state = OTA_ERROR;
                break;
        }
        if (ota->state == OTA_DESTROY) {
            break;
        }
    }

    vQueueDelete(ota->srv_q);
    if (ota->upgrade_list) {
        audio_free(ota->upgrade_list);
    }
    audio_free(ota);
    vTaskDelete(NULL);
}

static esp_err_t _ota_start(periph_service_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    ota_service_t *ota = periph_service_get_data(handle);
    ota_service_cmd_send(ota->srv_q, OTA_SERVICE_CMD_START, NULL, 0);
    return ESP_OK;
}

static esp_err_t _ota_destroy(periph_service_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    ota_service_t *ota = periph_service_get_data(handle);
    ota_service_cmd_send(ota->srv_q, OTA_SERVICE_CMD_DESTROY, NULL, 0);
    return ESP_OK;
}

esp_err_t ota_service_set_upgrade_param(periph_service_handle_t handle, ota_upgrade_ops_t *list, int list_len)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    AUDIO_NULL_CHECK(TAG, list, return ESP_ERR_INVALID_ARG);
    AUDIO_CHECK(TAG, list_len > 0, return ESP_FAIL, "list len <= 0");

    ota_service_t *ota = periph_service_get_data(handle);

    ota->upgrade_list = audio_calloc(1, list_len * sizeof(ota_upgrade_ops_t));
    AUDIO_NULL_CHECK(TAG, ota->upgrade_list, return ESP_FAIL);
    memcpy(ota->upgrade_list, list, list_len * sizeof(ota_upgrade_ops_t));
    ota->list_len = list_len;

    return ESP_OK;
}

periph_service_handle_t ota_service_create(ota_service_config_t *config)
{
    ota_service_t *ota = audio_calloc(1, sizeof(ota_service_t));
    AUDIO_MEM_CHECK(TAG, ota, return NULL);

    ota->srv_q = xQueueCreate(3, sizeof(ota_msg_t));
    AUDIO_MEM_CHECK(TAG, ota->srv_q, {
        audio_free(ota);
        return NULL;
    });
    ota->state = OTA_INIT;
    periph_service_config_t cfg = {
        .task_stack = config->task_stack,
        .task_prio = config->task_prio,
        .task_core = config->task_core,
        .task_func = ota_task,
        .service_start = _ota_start,
        .service_stop = NULL, /*Should not stop the upgrading process*/
        .service_ioctl = NULL,
        .service_destroy = _ota_destroy,
        .service_name = "ota_serv",
        .user_data = (void *)ota,
    };

    periph_service_handle_t periph_ota = periph_service_create(&cfg);
    AUDIO_MEM_CHECK(TAG, periph_ota, {
        vQueueDelete(ota->srv_q);
        audio_free(ota);
        return NULL;
    });
    periph_service_set_callback(periph_ota, config->evt_cb, config->cb_ctx);

    return periph_ota;
}