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
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "audio_mutex.h"
#include "esp_event_cast.h"
#include "sys/queue.h"
#include "audio_mem.h"
#include "audio_error.h"
#include "esp_log.h"

static const char *TAG = "EVT_CAST";

typedef struct esp_evt_cast_item {
    STAILQ_ENTRY(esp_evt_cast_item)     next;
    xQueueHandle                        que;
} esp_evt_cast_item_t;

typedef STAILQ_HEAD(esp_event_cast_list, esp_evt_cast_item) esp_event_cast_list_t;

typedef struct esp_event_cast {
    xSemaphoreHandle        _mux;
    esp_event_cast_list_t   evt_list;
} esp_event_cast_t;


esp_event_cast_handle_t esp_event_cast_create(void)
{
    esp_event_cast_handle_t obj = audio_calloc(1, sizeof(esp_event_cast_t));
    AUDIO_NULL_CHECK(TAG, obj, return NULL);
    obj->_mux =  mutex_create();
    AUDIO_NULL_CHECK(TAG, obj->_mux, {audio_free(obj);
                                      return NULL;
                                     });
    STAILQ_INIT(&obj->evt_list);
    return obj;
}

esp_err_t esp_event_cast_destroy(esp_event_cast_handle_t handle)
{
    if (handle) {
        esp_evt_cast_item_t *item, *tmp;
        STAILQ_FOREACH_SAFE(item, &handle->evt_list, next, tmp) {
            STAILQ_REMOVE(&handle->evt_list, item, esp_evt_cast_item, next);
            audio_free(item);
        }
        mutex_destroy(handle->_mux);
        free(handle);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t esp_event_cast_register(esp_event_cast_handle_t handle, xQueueHandle que)
{
    if ((handle == NULL) || (que == NULL)) {
        ESP_LOGE(TAG, "func:%s, invalid parameters, handle=%p, que=%p", __func__, handle, que);
        return ESP_FAIL;
    }
    esp_evt_cast_item_t *local_list = audio_calloc(1, sizeof(esp_evt_cast_item_t));
    AUDIO_MEM_CHECK(TAG, local_list, {return ESP_FAIL;});
    mutex_lock(handle->_mux);
    local_list->que = que;
    STAILQ_INSERT_TAIL(&handle->evt_list, local_list, next);
    mutex_unlock(handle->_mux);
    ESP_LOGD(TAG, "INERT, list[%p], que:%p", handle, que);

    return ESP_OK;
}

esp_err_t esp_event_cast_unregister(esp_event_cast_handle_t handle, xQueueHandle que)
{
    if ((handle == NULL) || (que == NULL)) {
        ESP_LOGE(TAG, "func:%s, invalid parameters, handle=%p, que=%p", __func__, handle, que);
        return ESP_FAIL;
    }
    mutex_lock(handle->_mux);
    esp_evt_cast_item_t *item, *tmp;
    STAILQ_FOREACH_SAFE(item,  &handle->evt_list, next, tmp) {
        ESP_LOGD(TAG, "func:%s, list=%p, que=%p, target que:%p", __func__, item, item->que, que);
        if (item->que == que) {
            STAILQ_REMOVE(&handle->evt_list, item, esp_evt_cast_item, next);
            audio_free(item);
            break;
        }
    }
    mutex_unlock(handle->_mux);
    ESP_LOGD(TAG, "func:%s, que size=%d", __func__, esp_event_cast_get_count(handle));
    return 0;
}

esp_err_t esp_event_cast_broadcasting(esp_event_cast_handle_t handle, void *data)
{
    if ((handle == NULL) || (data == NULL)) {
        ESP_LOGE(TAG, "func:%s, invalid parameters, handle=%p, data=%p", __func__, handle, data);
        return ESP_FAIL;
    }
    mutex_lock(handle->_mux);

    esp_evt_cast_item_t *item, *tmp;
    STAILQ_FOREACH_SAFE(item,  &handle->evt_list, next, tmp) {
        ESP_LOGD(TAG, "func:%s, list=%p, que=%p, data:%p", __func__, item, item->que, data);
        if (item->que) {
            if (pdFALSE == xQueueSend(item->que, data, 0)) {
                ESP_LOGW(TAG, "Queue[%p] send failed, free size:%d", item->que, uxQueueSpacesAvailable(item->que));
            }
        }
    }
    mutex_unlock(handle->_mux);
    return 0;
}

esp_err_t esp_event_cast_broadcasting_isr(esp_event_cast_handle_t handle, void *data)
{
    if ((handle == NULL) || (data == NULL)) {
        ESP_EARLY_LOGE(TAG, "func:%s, invalid parameters, handle=%p, data=%p", __func__, handle, data);
        return ESP_FAIL;
    }
    mutex_lock(handle->_mux);
    esp_evt_cast_item_t *item, *tmp;
    STAILQ_FOREACH_SAFE(item,  &handle->evt_list, next, tmp) {
        if (item->que) {
            if (pdFALSE == xQueueSendFromISR(item->que, data, 0)) {
                ESP_EARLY_LOGW(TAG, "Queue[%p] send failed, free size:%d", item->que, uxQueueSpacesAvailable(item->que));
            }
        }
    }
    mutex_unlock(handle->_mux);
    return 0;
}

esp_err_t esp_event_cast_get_count(esp_event_cast_handle_t handle)
{
    if (handle == NULL) {
        ESP_LOGE(TAG, "func:%s, invalid parameters, handle=%p", __func__, handle);
        return ESP_FAIL;
    }
    mutex_lock(handle->_mux);
    int cnt = 0;
    esp_evt_cast_item_t *item, *tmp;
    STAILQ_FOREACH_SAFE(item,  &handle->evt_list, next, tmp) {
        cnt ++;
    }
    mutex_unlock(handle->_mux);
    return cnt;
}
