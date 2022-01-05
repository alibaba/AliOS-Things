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

#include "sys/queue.h"
#include "esp_log.h"
#include "audio_mem.h"
#include "audio_error.h"
#include "audio_mutex.h"
#include "playlist.h"

static const char *TAG = "PLAYLIST";

/**
 * @brief Information of playlist
 */
typedef struct playlist_info {
    playlist_operator_handle_t      list_handle;                /*!< Specific playlist's handle */
    uint8_t                         list_id;                    /*!< List id*/
    STAILQ_ENTRY(playlist_info)     entries;                    /*!< List node */
} playlist_info_t;

/**
 * @brief Handle of playlist
 */
struct playlist_handle {
    uint8_t list_num;                                           /*!< The number of all the playlists in the handle */
    playlist_info_t *cur_playlist;                              /*!< Pointer of current playlist */
    xSemaphoreHandle playlist_operate_lock;                     /*!< A semaphore of operations */
    STAILQ_HEAD(list_info, playlist_info) playlist_info_list;   /*!< List head of playlists */
};

playlist_handle_t playlist_create(void)
{
    playlist_handle_t handle = (playlist_handle_t)audio_calloc(1, sizeof(struct playlist_handle));
    AUDIO_NULL_CHECK(TAG, handle, return NULL);
    handle->playlist_operate_lock = mutex_create();
    AUDIO_NULL_CHECK(TAG, handle->playlist_operate_lock, {
        audio_free(handle);
        return NULL;
    });
    STAILQ_INIT(&handle->playlist_info_list);

    return handle;
}

esp_err_t playlist_next(playlist_handle_t handle, int step, char **url_buff)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, handle->cur_playlist, return ESP_FAIL);

    esp_err_t ret = ESP_OK;
    mutex_lock(handle->playlist_operate_lock);

    playlist_info_t *cur_list = handle->cur_playlist;
    playlist_operator_handle_t cur_handle = cur_list->list_handle;
    playlist_operation_t operation = {0};
    cur_handle->get_operation(&operation);

    ret = operation.next(cur_handle, step, url_buff);
    mutex_unlock(handle->playlist_operate_lock);
    return ret;
}

esp_err_t playlist_prev(playlist_handle_t handle, int step, char **url_buff)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, handle->cur_playlist, return ESP_FAIL);

    esp_err_t ret = ESP_OK;
    mutex_lock(handle->playlist_operate_lock);

    playlist_info_t *cur_list = handle->cur_playlist;
    playlist_operator_handle_t cur_handle = cur_list->list_handle;
    playlist_operation_t operation = {0};
    cur_handle->get_operation(&operation);

    ret = operation.prev(cur_handle, step, url_buff);
    mutex_unlock(handle->playlist_operate_lock);
    return ret;
}

esp_err_t playlist_choose(playlist_handle_t handle, int url_id, char **url_buff)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, handle->cur_playlist, return ESP_FAIL);

    esp_err_t ret = ESP_OK;
    mutex_lock(handle->playlist_operate_lock);

    playlist_info_t *cur_list = handle->cur_playlist;
    playlist_operator_handle_t cur_handle = cur_list->list_handle;
    playlist_operation_t operation = {0};
    cur_handle->get_operation(&operation);

    ret = operation.choose(cur_handle, url_id, url_buff);
    mutex_unlock(handle->playlist_operate_lock);
    return ret;
}

esp_err_t playlist_save(playlist_handle_t handle, const char *url)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, handle->cur_playlist, return ESP_FAIL);

    esp_err_t ret = ESP_OK;
    mutex_lock(handle->playlist_operate_lock);

    playlist_info_t *cur_list = handle->cur_playlist;
    playlist_operator_handle_t cur_handle = cur_list->list_handle;
    playlist_operation_t operation = {0};
    cur_handle->get_operation(&operation);

    ret = operation.save(cur_handle, url);
    mutex_unlock(handle->playlist_operate_lock);
    return ret;
}

esp_err_t playlist_reset(playlist_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, handle->cur_playlist, return ESP_FAIL);

    esp_err_t ret = ESP_OK;
    mutex_lock(handle->playlist_operate_lock);

    playlist_info_t *cur_list = handle->cur_playlist;
    playlist_operator_handle_t cur_handle = cur_list->list_handle;
    playlist_operation_t operation = {0};
    cur_handle->get_operation(&operation);

    ret = operation.reset(cur_handle);
    mutex_unlock(handle->playlist_operate_lock);
    return ret;
}

bool playlist_exist(playlist_handle_t handle, const char *url)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, handle->cur_playlist, return ESP_FAIL);

    mutex_lock(handle->playlist_operate_lock);

    playlist_info_t *cur_list = handle->cur_playlist;
    playlist_operator_handle_t cur_handle = cur_list->list_handle;
    playlist_operation_t operation = {0};
    cur_handle->get_operation(&operation);

    bool ret = operation.exist(cur_handle, url);
    mutex_unlock(handle->playlist_operate_lock);
    return ret;
}

esp_err_t playlist_show(playlist_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, handle->cur_playlist, return ESP_FAIL);

    esp_err_t ret = ESP_OK;
    mutex_lock(handle->playlist_operate_lock);

    playlist_info_t *cur_list = handle->cur_playlist;
    playlist_operator_handle_t cur_handle = cur_list->list_handle;
    playlist_operation_t operation = {0};
    cur_handle->get_operation(&operation);

    ret = operation.show(cur_handle);
    mutex_unlock(handle->playlist_operate_lock);
    return ret;
}

esp_err_t playlist_remove_by_url(playlist_handle_t handle, const char *url)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, handle->cur_playlist, return ESP_FAIL);

    esp_err_t ret = ESP_OK;
    mutex_lock(handle->playlist_operate_lock);

    playlist_info_t *cur_list = handle->cur_playlist;
    playlist_operator_handle_t cur_handle = cur_list->list_handle;
    playlist_operation_t operation = {0};
    cur_handle->get_operation(&operation);

    if (operation.remove_by_url) {
        ret = operation.remove_by_url(cur_handle, url);
    } else {
        ESP_LOGE(TAG, "Remove by url only can be use in dram list now");
        mutex_unlock(handle->playlist_operate_lock);
        return ESP_ERR_NOT_SUPPORTED;
    }
    mutex_unlock(handle->playlist_operate_lock);
    return ret;
}

esp_err_t playlist_remove_by_url_id(playlist_handle_t handle, uint16_t url_id)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, handle->cur_playlist, return ESP_FAIL);

    esp_err_t ret = ESP_OK;
    mutex_lock(handle->playlist_operate_lock);

    playlist_info_t *cur_list = handle->cur_playlist;
    playlist_operator_handle_t cur_handle = cur_list->list_handle;
    playlist_operation_t operation = {0};
    cur_handle->get_operation(&operation);

    if (operation.remove_by_id) {
        ret = operation.remove_by_id(cur_handle, url_id);
    } else {
        ESP_LOGE(TAG, "Remove by url id only can be use in dram list now");
        mutex_unlock(handle->playlist_operate_lock);
        return ESP_ERR_NOT_SUPPORTED;
    }
    mutex_unlock(handle->playlist_operate_lock);
    return ret;
}

esp_err_t playlist_destroy(playlist_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);

    esp_err_t ret = ESP_OK;
    mutex_lock(handle->playlist_operate_lock);
    playlist_info_t *item, *tmp;
    playlist_operation_t operation = {0};
    playlist_operator_handle_t cur_handle = NULL;

    STAILQ_FOREACH_SAFE(item, &handle->playlist_info_list, entries, tmp) {
        cur_handle = item->list_handle;
        ret |= cur_handle->get_operation(&operation);
        ret |= operation.destroy(cur_handle);
        STAILQ_REMOVE(&handle->playlist_info_list, item, playlist_info, entries);
        audio_free(item);
    }
    mutex_unlock(handle->playlist_operate_lock);
    mutex_destroy(handle->playlist_operate_lock);
    audio_free(handle);
    ESP_LOGW(TAG, "Handle of playlists has been destroyed");
    return ret;
}

esp_err_t playlist_add(playlist_handle_t handle, playlist_operator_handle_t list_handle, uint8_t list_id)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, list_handle, return ESP_FAIL);

    esp_err_t ret = ESP_OK;
    mutex_lock(handle->playlist_operate_lock);

    playlist_info_t *tmp = NULL;
    STAILQ_FOREACH(tmp, &handle->playlist_info_list, entries) {
        if (tmp->list_id == list_id) {
            ESP_LOGE(TAG, "This id has been registered, id: %d", tmp->list_id);
            mutex_unlock(handle->playlist_operate_lock);
            return ESP_FAIL;
        }
    }

    playlist_info_t *list_node = audio_calloc(1, sizeof(playlist_info_t));
    AUDIO_NULL_CHECK(TAG, list_node, {
        mutex_unlock(handle->playlist_operate_lock);
        return ESP_FAIL;
    });

    list_node->list_handle = list_handle;
    list_node->list_id     = list_id;

    STAILQ_INSERT_TAIL(&handle->playlist_info_list, list_node, entries);
    handle->list_num ++;
    if (NULL == handle->cur_playlist) {
        ESP_LOGD(TAG, "Set the first node as current playlist");
        handle->cur_playlist = list_node;
    }
    mutex_unlock(handle->playlist_operate_lock);
    return ret;
}

esp_err_t playlist_checkout_by_id(playlist_handle_t handle, uint8_t id)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    mutex_lock(handle->playlist_operate_lock);
    playlist_info_t *tmp = NULL;
    STAILQ_FOREACH(tmp, &handle->playlist_info_list, entries) {
        if (tmp->list_id == id) {
            handle->cur_playlist = tmp;
            mutex_unlock(handle->playlist_operate_lock);
            ESP_LOGW(TAG, "checkout to another playlist, ID: %d, TYPE: %d", tmp->list_id, playlist_get_current_list_type(handle));
            return ESP_OK;
        }
    }

    ESP_LOGE(TAG, "Can not find id %d", id);
    mutex_unlock(handle->playlist_operate_lock);
    return ESP_FAIL;
}

int playlist_get_list_num(playlist_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return -1);
    mutex_lock(handle->playlist_operate_lock);
    int num = handle->list_num;
    mutex_unlock(handle->playlist_operate_lock);
    return num;
}

int playlist_get_current_list_id(playlist_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return -1);
    AUDIO_NULL_CHECK(TAG, handle->cur_playlist, return -1);

    mutex_lock(handle->playlist_operate_lock);
    playlist_info_t *tmp = handle->cur_playlist;
    mutex_unlock(handle->playlist_operate_lock);
    return tmp->list_id;
}

playlist_type_t playlist_get_current_list_type(playlist_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return -1);
    AUDIO_NULL_CHECK(TAG, handle->cur_playlist, return -1);

    mutex_lock(handle->playlist_operate_lock);
    playlist_info_t *cur_list = handle->cur_playlist;
    playlist_operator_handle_t cur_handle = cur_list->list_handle;
    playlist_operation_t operation = {0};
    cur_handle->get_operation(&operation);
    mutex_unlock(handle->playlist_operate_lock);
    return operation.type;
}

esp_err_t playlist_get_current_list_url(playlist_handle_t handle, char **url_buff)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, handle->cur_playlist, return ESP_FAIL);

    esp_err_t ret = ESP_OK;
    mutex_lock(handle->playlist_operate_lock);

    playlist_info_t *cur_list = handle->cur_playlist;
    playlist_operator_handle_t cur_handle = cur_list->list_handle;
    playlist_operation_t operation = {0};
    cur_handle->get_operation(&operation);

    ret = operation.current(cur_handle, url_buff);
    mutex_unlock(handle->playlist_operate_lock);
    return ret;
}

int playlist_get_current_list_url_num(playlist_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, handle->cur_playlist, return ESP_FAIL);

    mutex_lock(handle->playlist_operate_lock);

    playlist_info_t *cur_list = handle->cur_playlist;
    playlist_operator_handle_t cur_handle = cur_list->list_handle;
    playlist_operation_t operation = {0};
    cur_handle->get_operation(&operation);

    int ret = operation.get_url_num(cur_handle);
    mutex_unlock(handle->playlist_operate_lock);
    return ret;
}

int playlist_get_current_list_url_id(playlist_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, handle->cur_playlist, return ESP_FAIL);

    mutex_lock(handle->playlist_operate_lock);
    playlist_info_t *cur_list = handle->cur_playlist;
    playlist_operator_handle_t cur_handle = cur_list->list_handle;
    playlist_operation_t operation = {0};
    cur_handle->get_operation(&operation);
    int ret = operation.get_url_id(cur_handle);;
    mutex_unlock(handle->playlist_operate_lock);
    return ret;
}
