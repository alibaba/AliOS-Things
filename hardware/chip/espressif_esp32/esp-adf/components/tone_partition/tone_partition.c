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

#include "audio_error.h"
#include "audio_mem.h"

#include "esp_delegate.h"
#include "esp_log.h"
#include "esp_partition.h"

#include "partition_action.h"
#include "tone_partition.h"

#define FLASH_TONE_FILE_TAG        (0x28)
#define FLASH_TONE_FILE_INFO_BLOCK (64)

typedef struct tone_partition_s {
    const esp_partition_t *partition;
    flash_tone_header_t header;
    const esp_partition_t *(*find)(esp_partition_type_t, esp_partition_subtype_t, const char *);
    esp_err_t (*read)(const esp_partition_t *, size_t, void *, size_t);
} tone_partition_t;

static const char *TAG = "TONE_PARTITION";

static const esp_partition_t *partition_find_with_dispatcher(esp_partition_type_t type, esp_partition_subtype_t subtype, const char *label)
{
    esp_dispatcher_handle_t dispatcher = esp_dispatcher_get_delegate_handle();
    if (!dispatcher) {
        return NULL;
    }
    partition_find_args_t config = {
        .type = type,
        .subtype = subtype,
        .label = label,
    };
    action_arg_t arg = {
        .data = &config,
        .len = sizeof(partition_find_args_t),
    };
    action_result_t result = { 0 };
    esp_dispatcher_execute_with_func(dispatcher, partition_find_action, NULL, &arg, &result);
    return (const esp_partition_t *)result.data;
}

static esp_err_t partition_read_with_dispatcher(const esp_partition_t *partition, size_t src_offset, void *dst, size_t size)
{
    esp_dispatcher_handle_t dispatcher = esp_dispatcher_get_delegate_handle();
    if (!dispatcher) {
        return ESP_FAIL;
    }
    partition_read_args_t read_arg = {
        .partition = partition,
        .src_offset = src_offset,
        .dst = dst,
        .size = size,
    };
    action_arg_t arg = {
        .data = &read_arg,
        .len = sizeof(partition_read_args_t),
    };
    action_result_t result = { 0 };
    esp_dispatcher_execute_with_func(dispatcher, partition_read_action, NULL, &arg, &result);
    return result.err;
}

esp_err_t tone_partition_get_file_info(tone_partition_handle_t handle, uint16_t index, tone_file_info_t *info)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, info, return ESP_FAIL);

    if (handle->header.total_num < index) {
        ESP_LOGE(TAG, "Wanted index out of range index[%d]", index);
        return ESP_FAIL;
    }
    tone_file_info_t info_tmp = { 0 };
    int start_adr = 0;
    if (handle->header.format == TONE_VERSION_0) {
        start_adr = sizeof(flash_tone_header_t) + FLASH_TONE_FILE_INFO_BLOCK * index;
    } else if (handle->header.format == TONE_VERSION_1) {
        start_adr = sizeof(flash_tone_header_t) + sizeof(esp_app_desc_t) + FLASH_TONE_FILE_INFO_BLOCK * index;
    } else {
        ESP_LOGE(TAG, "Tone format not support!");
        return ESP_FAIL;
    }
    if (ESP_OK == handle->read(handle->partition, start_adr, &info_tmp, sizeof(info_tmp))) {
        //TODO check crc
        if (info_tmp.file_tag == FLASH_TONE_FILE_TAG) {
            memcpy(info, &info_tmp, sizeof(info_tmp));
        }
    } else {
        ESP_LOGE(TAG, "Get tone file tag error %x", info_tmp.file_tag);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t tone_partition_file_read(tone_partition_handle_t handle, tone_file_info_t *file, uint32_t offset, char *dst, int read_len)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    AUDIO_NULL_CHECK(TAG, file, return ESP_FAIL);

    esp_err_t err = handle->read(handle->partition, file->song_adr + offset, dst, read_len);
    if (ESP_OK != err) {
        ESP_LOGE(TAG, "Tone file read error[0x%x]", err);
    }
    return err;
}

static esp_err_t tone_partition_get_tail(tone_partition_handle_t handle, uint16_t *tail)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);

    if (handle->header.format == TONE_VERSION_1) {
        tone_file_info_t last_file = { 0 };
        tone_partition_get_file_info(handle, handle->header.total_num - 1, &last_file);
        int tail_addr = last_file.song_adr + last_file.song_len + ((4 - last_file.song_len % 4) % 4) + 4;
        ESP_LOGD(TAG, "addr %X, len %X, tail %X", last_file.song_adr, last_file.song_len, tail_addr);
        return handle->read(handle->partition, tail_addr, tail, sizeof(uint16_t));
    } else {
        *tail = 0;
        ESP_LOGE(TAG, "No tail");
        return ESP_FAIL;
    }
}

esp_err_t tone_partition_get_app_desc(tone_partition_handle_t handle, esp_app_desc_t *desc)
{
    if (handle != NULL && handle->header.format == TONE_VERSION_1) {
        if (ESP_OK == handle->read(handle->partition, sizeof(flash_tone_header_t), desc, sizeof(esp_app_desc_t))) {
            return ESP_OK;
        }
    }
    ESP_LOGE(TAG, "Get desc failed");
    return ESP_FAIL;
}

tone_partition_handle_t tone_partition_init(const char *partition_label, bool use_delegate)
{
    AUDIO_NULL_CHECK(TAG, partition_label, return NULL);
    tone_partition_t *tone = audio_calloc(1, sizeof(tone_partition_t));
    AUDIO_NULL_CHECK(TAG, tone, return NULL);
    if (use_delegate) {
        tone->find = partition_find_with_dispatcher;
        tone->read = partition_read_with_dispatcher;
    } else {
        tone->find = esp_partition_find_first;
        tone->read = esp_partition_read;
    }
    tone->partition = tone->find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, partition_label);
    if (!tone->partition) {
        ESP_LOGE(TAG, "Can not found tone[%s] partition", partition_label);
        goto error;
    }
    if (ESP_OK != tone->read(tone->partition, 0, &tone->header, sizeof(flash_tone_header_t))) {
        ESP_LOGE(TAG, "Read flash tone file header failed");
        goto error;
    }
    if (tone->header.header_tag != FLASH_TONE_HEADER) {
        ESP_LOGE(TAG, "Not flash tone partition");
        goto error;
    }
    if (tone->header.format == TONE_VERSION_1) {
        uint16_t tail = 0;
        if (ESP_OK != tone_partition_get_tail(tone, &tail) || tail != FLASH_TONE_TAIL) {
            ESP_LOGE(TAG, "Flash tone init failed at tail check %X", tail);
            goto error;
        }
    }
    ESP_LOGI(TAG, "tone partition format %d, total %d", tone->header.format, tone->header.total_num);
    return (tone_partition_handle_t)tone;

error:
    if (tone) {
        free(tone);
    }
    return NULL;
}

esp_err_t tone_partition_deinit(tone_partition_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    free(handle);
    return ESP_OK;
}
