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
#include "esp_partition.h"
#include "audio_mem.h"
#include "audio_error.h"
#include "partition_list.h"

#define DEFAULT_PARTITION_TYPE               ESP_PARTITION_TYPE_DATA
#define DEFAULT_PARTITION_URL_SUB_TYPE       0x06
#define DEFAULT_PARTITION_OFFSET_SUB_TYPE    0x07

#define PARTITION_LIST_URL_MAX_LENGTH        2048

static const char *TAG = "PARTITION_LIST";

/**
 * @brief Partition list management unit
 */
typedef struct partition_list {
    const esp_partition_t *url_part;    /*!< URL partition handle */
    const esp_partition_t *offset_part; /*!< offset partition handle */
    uint16_t url_num;                   /*!< number of URLs */
    uint16_t cur_url_id;                /*!< current URL id */
    uint32_t total_size_offset_part;    /*!< size of partition to save URLs */
    uint32_t total_size_url_part;       /*!< size of partition to save offset */
    char     *cur_url;                  /*!< point to current URL */
} partition_list_t;

esp_err_t partition_list_get_operation(playlist_operation_t *operation);

static esp_err_t partition_list_choose_id(partition_list_t *playlist, int id, char **url_buff)
{
    esp_err_t ret = ESP_OK;
    uint32_t pos = 0;
    uint16_t  size = 0;
    int offset = id * (sizeof(uint32_t) + sizeof(uint16_t));

    ret |= esp_partition_read(playlist->offset_part, offset, &pos, sizeof(uint32_t));
    ret |= esp_partition_read(playlist->offset_part, offset + sizeof(uint32_t), &size, sizeof(uint16_t));

    if (playlist->cur_url) {
        audio_free(playlist->cur_url);
    }

    playlist->cur_url = (char *)audio_calloc(1, size + 1);
    AUDIO_NULL_CHECK(TAG, playlist->cur_url, {
        ESP_LOGE(TAG, "Fail to allocate memory for url");
        return ESP_FAIL;
    });

    ret |= esp_partition_read(playlist->url_part, pos, playlist->cur_url, size);
    playlist->cur_url[size] = 0;

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "There is a mistake when choose id in partition playlist!");
        return ESP_FAIL;
    }

    *url_buff = playlist->cur_url;
    playlist->cur_url_id = id;
    return ESP_OK;
}

esp_err_t partition_list_create(playlist_operator_handle_t *handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);

    playlist_operator_handle_t partition_handle = (playlist_operator_handle_t )audio_calloc(1, sizeof(playlist_operator_t));
    AUDIO_NULL_CHECK(TAG, partition_handle, return ESP_FAIL);

    partition_list_t *partition_list = audio_calloc(1, sizeof(partition_list_t));
    AUDIO_NULL_CHECK(TAG, partition_list, {
        audio_free(partition_handle);
    });

    partition_handle->playlist = partition_list;
    partition_handle->get_operation = partition_list_get_operation;

    esp_err_t ret = ESP_OK;
    partition_list->url_part = esp_partition_find_first(DEFAULT_PARTITION_TYPE, DEFAULT_PARTITION_URL_SUB_TYPE, NULL);
    partition_list->offset_part = esp_partition_find_first(DEFAULT_PARTITION_TYPE, DEFAULT_PARTITION_OFFSET_SUB_TYPE, NULL);
    if (NULL == partition_list->url_part || NULL == partition_list->offset_part) {
        ESP_LOGE(TAG, "Can not find the offset partition or url partition, please check the partition table");
        audio_free(partition_handle);
        audio_free(partition_list);
        return ESP_FAIL;
    }
    const esp_partition_t *url_part = partition_list->url_part;
    const esp_partition_t *offset_part = partition_list->offset_part;
    ret |= esp_partition_erase_range(url_part, 0, url_part->size);
    ret |= esp_partition_erase_range(offset_part, 0, url_part->size);

    *handle = partition_handle;
    return ret;
}

esp_err_t partition_list_save(playlist_operator_handle_t handle, const char *url)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, url, return ESP_FAIL);
    partition_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    esp_err_t ret = ESP_OK;
    uint16_t len = strlen(url);

    ret |= esp_partition_write(playlist->offset_part, playlist->total_size_offset_part, &playlist->total_size_url_part, sizeof(uint32_t));
    playlist->total_size_offset_part += sizeof(uint32_t);
    ret |= esp_partition_write(playlist->offset_part, playlist->total_size_offset_part, &len, sizeof(uint16_t));
    playlist->total_size_offset_part += sizeof(uint16_t);
    ret |= esp_partition_write(playlist->url_part, playlist->total_size_url_part, url, len);
    playlist->total_size_url_part += len;

    if (ret != ESP_OK) {
        playlist->total_size_offset_part -= (sizeof(uint32_t) + sizeof(uint16_t));
        playlist->total_size_url_part -= len;
        ESP_LOGE(TAG, "Failed to save URL to partition list ");
        return ESP_FAIL;
    }
    playlist->url_num++;

    return ESP_OK;
}

esp_err_t partition_list_next(playlist_operator_handle_t handle, int step, char **url_buff)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, url_buff, return ESP_FAIL);
    partition_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    if (playlist->url_num == 0) {
        ESP_LOGE(TAG, "No url, please save urls to playlist first");
        return ESP_FAIL;
    }
    if (step < 0) {
        ESP_LOGE(TAG, "Number of steps should be larger than 0");
        return ESP_FAIL;
    }

    int id = 0, total_step = 0;
    total_step = playlist->cur_url_id + step;
    id = total_step;
    if (total_step >= playlist->url_num) {
        id = total_step % playlist->url_num;
    }

    return partition_list_choose_id(playlist, id, url_buff);
}

esp_err_t partition_list_prev(playlist_operator_handle_t handle, int step, char **url_buff)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, url_buff, return ESP_FAIL);
    partition_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    if (playlist->url_num == 0) {
        ESP_LOGE(TAG, "No url, please save urls to playlist first");
        return ESP_FAIL;
    }
    if (step < 0) {
        ESP_LOGE(TAG, "Number of steps should be larger than 0");
        return ESP_FAIL;
    }

    int id = 0, total_step = 0;
    total_step = playlist->cur_url_id - step;
    id = total_step;

    if (total_step < 0) {
        if (total_step % playlist->url_num == 0) {
            id = 0;
        } else {
            id = playlist->url_num + total_step % playlist->url_num;
        }
    }

    return partition_list_choose_id(playlist, id, url_buff);
}

esp_err_t partition_list_current(playlist_operator_handle_t handle, char **url_buff)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, url_buff, return ESP_FAIL);
    partition_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    if (playlist->url_num == 0) {
        ESP_LOGE(TAG, "No url, please save urls to playlist first");
        return ESP_FAIL;
    }
    return partition_list_choose_id(playlist, playlist->cur_url_id, url_buff);
}

esp_err_t partition_list_choose(playlist_operator_handle_t handle, int url_id, char **url_buff)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, url_buff, return ESP_FAIL);
    partition_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    if (playlist->url_num == 0) {
        ESP_LOGE(TAG, "No url, please save urls to playlist first");
        return ESP_FAIL;
    }
    if ((url_id < 0) || (url_id >= playlist->url_num)) {
        ESP_LOGE(TAG, "Invalid url id to be choosen");
        return ESP_FAIL;
    }

    return partition_list_choose_id(playlist, url_id, url_buff);
}

esp_err_t partition_list_show(playlist_operator_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    partition_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    esp_err_t ret = ESP_OK;
    uint32_t pos = 0, offset = 0;
    uint16_t  size = 0;
    char *url = audio_calloc(1, PARTITION_LIST_URL_MAX_LENGTH);

    ESP_LOGI(TAG, "ID   URL");
    for (int i = 0; i < playlist->url_num; i++) {
        memset(url, 0, PARTITION_LIST_URL_MAX_LENGTH);
        ret |= esp_partition_read(playlist->offset_part, offset, &pos, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        ret |= esp_partition_read(playlist->offset_part, offset, &size, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        ret |= esp_partition_read(playlist->url_part, pos, url, size);
        ESP_LOGI(TAG, "%d   %s", i, url);
    }

    audio_free(url);
    return ret;
}

bool partition_list_exist(playlist_operator_handle_t handle, const char *url)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    partition_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    esp_err_t ret = ESP_OK;
    uint32_t pos = 0, offset = 0;
    uint16_t  size = 0;
    char *url_buff = audio_calloc(1, PARTITION_LIST_URL_MAX_LENGTH);

    for (int i = 0; i < playlist->url_num; i++) {
        memset(url_buff, 0, PARTITION_LIST_URL_MAX_LENGTH);
        ret |= esp_partition_read(playlist->offset_part, offset, &pos, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        ret |= esp_partition_read(playlist->offset_part, offset, &size, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        ret |= esp_partition_read(playlist->url_part, pos, url_buff, size);
        if (strlen(url) != strlen(url_buff)) {
            continue;
        }
        if (strcmp(url, url_buff) == 0) {
            audio_free(url_buff);
            return true;
        }
    }

    audio_free(url_buff);
    return false;
}

esp_err_t partition_list_reset(playlist_operator_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    partition_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    esp_err_t ret = ESP_OK;
    const esp_partition_t *url_part = playlist->url_part;
    const esp_partition_t *offset_part = playlist->offset_part;
    ret |= esp_partition_erase_range(url_part, 0, url_part->size);
    ret |= esp_partition_erase_range(offset_part, 0, offset_part->size);
    if (playlist->cur_url) {
        audio_free(playlist->cur_url);
        playlist->cur_url = NULL;
    }

    playlist->url_num = 0;
    playlist->cur_url_id = 0;
    playlist->total_size_offset_part = 0;
    playlist->total_size_url_part = 0;
    return ret;
}

int partition_list_get_url_num(playlist_operator_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    partition_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    return playlist->url_num;
}

int partition_list_get_url_id(playlist_operator_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    partition_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    return playlist->cur_url_id;
}

esp_err_t partition_list_destroy(playlist_operator_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    partition_list_t *playlist = handle->playlist;
    AUDIO_NULL_CHECK(TAG, playlist, return ESP_FAIL);

    esp_err_t ret = ESP_OK;
    const esp_partition_t *url_part = playlist->url_part;
    const esp_partition_t *offset_part = playlist->offset_part;
    ret |= esp_partition_erase_range(url_part, 0, url_part->size);
    ret |= esp_partition_erase_range(offset_part, 0, offset_part->size);
    if (playlist->cur_url) {
        audio_free(playlist->cur_url);
        playlist->cur_url = NULL;
    }
    audio_free(playlist);
    handle->playlist = NULL;
    audio_free(handle);
    return ret;
}

esp_err_t partition_list_get_operation(playlist_operation_t *operation)
{
    AUDIO_NULL_CHECK(TAG, operation, return ESP_FAIL);
    operation->show = (void *)partition_list_show;
    operation->save = (void *)partition_list_save;
    operation->next = (void *)partition_list_next;
    operation->prev = (void *)partition_list_prev;
    operation->reset   = (void *)partition_list_reset;
    operation->exist   = (void *)partition_list_exist;
    operation->choose  = (void *)partition_list_choose;
    operation->current = (void *)partition_list_current;
    operation->destroy = (void *)partition_list_destroy;
    operation->get_url_num = (void *)partition_list_get_url_num;
    operation->get_url_id  = (void *)partition_list_get_url_id;
    operation->type = PLAYLIST_PARTITION;
    return ESP_OK;
}
