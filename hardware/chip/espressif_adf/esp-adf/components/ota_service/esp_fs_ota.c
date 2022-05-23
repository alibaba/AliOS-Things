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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_fs_ota.h>
#include <esp_log.h>
#include <esp_ota_ops.h>
#include <errno.h>
#include "audio_mem.h"
#include "audio_sys.h"

#define IMAGE_HEADER_SIZE    (sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t) + sizeof(esp_app_desc_t) + 1)
#define DEFAULT_OTA_BUF_SIZE IMAGE_HEADER_SIZE
static const char *TAG = "esp_fs_ota";

typedef enum {
    ESP_FS_OTA_INIT,
    ESP_FS_OTA_BEGIN,
    ESP_FS_OTA_IN_PROGRESS,
    ESP_FS_OTA_SUCCESS,
} esp_fs_ota_state;

struct esp_fs_ota_handle {
    esp_ota_handle_t update_handle;
    const esp_partition_t *update_partition;
    FILE *fp;
    char *ota_upgrade_buf;
    size_t ota_upgrade_buf_size;
    int binary_file_len;
    esp_fs_ota_state state;
};

typedef struct esp_fs_ota_handle esp_fs_ota_t;

static esp_err_t _ota_write(esp_fs_ota_t *fs_ota_handle, const void *buffer, size_t buf_len)
{
    if (buffer == NULL || fs_ota_handle == NULL) {
        return ESP_FAIL;
    }
    esp_err_t err = esp_ota_write(fs_ota_handle->update_handle, buffer, buf_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error: esp_ota_write failed! err=0x%d", err);
    } else {
        fs_ota_handle->binary_file_len += buf_len;
        ESP_LOGD(TAG, "Written image length %d", fs_ota_handle->binary_file_len);
        err = ESP_ERR_FS_OTA_IN_PROGRESS;
    }
    return err;
}

esp_err_t esp_fs_ota_begin(esp_fs_ota_config_t *ota_config, esp_fs_ota_handle_t *handle)
{
    esp_err_t err;

    if (handle == NULL || ota_config == NULL || ota_config->path == NULL) {
        ESP_LOGE(TAG, "esp_fs_ota_begin: Invalid argument");
        if (handle) {
            *handle = NULL;
        }
        return ESP_ERR_INVALID_ARG;
    }

    esp_fs_ota_t *fs_ota_handle = audio_calloc(1, sizeof(esp_fs_ota_t));
    if (!fs_ota_handle) {
        ESP_LOGE(TAG, "Couldn't allocate memory to upgrade data buffer");
        *handle = NULL;
        return ESP_ERR_NO_MEM;
    }

    /* open */
    fs_ota_handle->fp = fopen(&ota_config->path[6], "r");
    if (fs_ota_handle->fp == NULL) {
        ESP_LOGE(TAG, "Failed to open the app bin %s", &ota_config->path[6]);
        err = ESP_FAIL;
        goto failure;
    }

    fs_ota_handle->update_partition = NULL;
    ESP_LOGI(TAG, "Starting OTA...");
    fs_ota_handle->update_partition = esp_ota_get_next_update_partition(NULL);
    if (fs_ota_handle->update_partition == NULL) {
        ESP_LOGE(TAG, "Passive OTA partition not found");
        err = ESP_FAIL;
        goto failure;
    }
    ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%x",
        fs_ota_handle->update_partition->subtype, fs_ota_handle->update_partition->address);

    const int alloc_size = (ota_config->buffer_size > DEFAULT_OTA_BUF_SIZE) ? ota_config->buffer_size : DEFAULT_OTA_BUF_SIZE;
    fs_ota_handle->ota_upgrade_buf = (char *)audio_calloc(1, alloc_size);
    if (!fs_ota_handle->ota_upgrade_buf) {
        ESP_LOGE(TAG, "Couldn't allocate memory to upgrade data buffer");
        err = ESP_ERR_NO_MEM;
        goto failure;
    }
    fs_ota_handle->ota_upgrade_buf_size = alloc_size;
    fs_ota_handle->binary_file_len = 0;
    *handle = (esp_fs_ota_handle_t)fs_ota_handle;
    fs_ota_handle->state = ESP_FS_OTA_BEGIN;
    return ESP_OK;

failure:
    if (fs_ota_handle->fp) {
        fclose(fs_ota_handle->fp);
    }
    audio_free(fs_ota_handle);
    *handle = NULL;
    return err;
}

esp_err_t esp_fs_ota_get_img_desc(esp_fs_ota_handle_t fs_ota_handle, esp_app_desc_t *new_app_info)
{
    esp_fs_ota_t *handle = (esp_fs_ota_t *)fs_ota_handle;
    if (handle == NULL || new_app_info == NULL) {
        ESP_LOGE(TAG, "esp_fs_ota_read_img_desc: Invalid argument");
        return ESP_ERR_INVALID_ARG;
    }
    if (handle->state < ESP_FS_OTA_BEGIN) {
        ESP_LOGE(TAG, "esp_fs_ota_read_img_desc: Invalid state");
        return ESP_FAIL;
    }
    /*
     * `data_read_size` holds number of bytes needed to read complete header.
     * `bytes_read` holds number of bytes read.
     */
    int data_read_size = IMAGE_HEADER_SIZE;
    int data_read = 0, bytes_read = 0;
    /*
     * while loop is added to download complete image headers, even if the headers
     * are not sent in a single packet.
     */
    while (data_read_size > 0) {
        data_read = fread((handle->ota_upgrade_buf + bytes_read), 1, data_read_size, handle->fp);
        if (data_read <= 0) {
            ESP_LOGE(TAG, "Connection closed, errno = %d", errno);
            break;
        }
        data_read_size -= data_read;
        bytes_read += data_read;
    }
    if (data_read_size > 0) {
        ESP_LOGE(TAG, "Complete headers were not read");
        return ESP_FAIL;
    }
    handle->binary_file_len = bytes_read;
    memcpy(new_app_info, &handle->ota_upgrade_buf[sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t)], sizeof(esp_app_desc_t));
    return ESP_OK;
}

esp_err_t esp_fs_ota_perform(esp_fs_ota_handle_t fs_ota_handle)
{
    esp_fs_ota_t *handle = (esp_fs_ota_t *)fs_ota_handle;
    if (handle == NULL) {
        ESP_LOGE(TAG, "esp_fs_ota_perform: Invalid argument");
        return ESP_ERR_INVALID_ARG;
    }
    if (handle->state < ESP_FS_OTA_BEGIN) {
        ESP_LOGE(TAG, "esp_fs_ota_perform: Invalid state");
        return ESP_FAIL;
    }

    esp_err_t err;
    int data_read;
    switch (handle->state) {
        case ESP_FS_OTA_BEGIN:
            err = esp_ota_begin(handle->update_partition, OTA_SIZE_UNKNOWN, &handle->update_handle);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "esp_ota_begin failed (%s)", esp_err_to_name(err));
                return err;
            }
            handle->state = ESP_FS_OTA_IN_PROGRESS;
            /* In case `esp_fs_ota_read_img_desc` was invoked first,
               then the image data read there should be written to OTA partition
               */
            if (handle->binary_file_len) {
                return _ota_write(handle, (const void *)handle->ota_upgrade_buf, handle->binary_file_len);
            }
            /* falls through */
        case ESP_FS_OTA_IN_PROGRESS:
            data_read = fread(handle->ota_upgrade_buf, 1, handle->ota_upgrade_buf_size, handle->fp);
            if (data_read <= 0) {
                if (!feof(handle->fp)) {
                    return ESP_FAIL;
                }
            } else if (data_read > 0) {
                return _ota_write(handle, (const void *)handle->ota_upgrade_buf, data_read);
            }
            handle->state = ESP_FS_OTA_SUCCESS;
            break;
        default:
            ESP_LOGE(TAG, "Invalid ESP FS OTA State");
            return ESP_FAIL;
            break;
    }
    return ESP_OK;
}

esp_err_t esp_fs_ota_finish(esp_fs_ota_handle_t fs_ota_handle)
{
    esp_fs_ota_t *handle = (esp_fs_ota_t *)fs_ota_handle;
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (handle->state < ESP_FS_OTA_BEGIN) {
        return ESP_FAIL;
    }

    esp_err_t err = ESP_OK;
    switch (handle->state) {
        case ESP_FS_OTA_SUCCESS:
        case ESP_FS_OTA_IN_PROGRESS:
            err = esp_ota_end(handle->update_handle);
            FALL_THROUGH;
        case ESP_FS_OTA_BEGIN:
            if (handle->ota_upgrade_buf) {
                audio_free(handle->ota_upgrade_buf);
            }
            if (handle->fp) {
                fclose(handle->fp);
            }
            break;
        default:
            ESP_LOGE(TAG, "Invalid ESP FS OTA State");
            break;
    }

    if ((err == ESP_OK) && (handle->state == ESP_FS_OTA_SUCCESS)) {
        esp_err_t err = esp_ota_set_boot_partition(handle->update_partition);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_ota_set_boot_partition failed! err=0x%d", err);
        }
    }
    audio_free(handle);
    return err;
}

int esp_fs_ota_get_image_len_read(esp_fs_ota_handle_t fs_ota_handle)
{
    esp_fs_ota_t *handle = (esp_fs_ota_t *)fs_ota_handle;
    if (handle == NULL) {
        return -1;
    }
    if (handle->state < ESP_FS_OTA_IN_PROGRESS) {
        return -1;
    }
    return handle->binary_file_len;
}

esp_err_t esp_fs_ota(esp_fs_ota_config_t *ota_config)
{
    if (!ota_config) {
        ESP_LOGE(TAG, "fs ota path not found");
        return ESP_ERR_INVALID_ARG;
    }

    esp_fs_ota_handle_t fs_ota_handle = NULL;
    esp_err_t err = esp_fs_ota_begin(ota_config, &fs_ota_handle);
    if (fs_ota_handle == NULL) {
        return ESP_FAIL;
    }

    while (1) {
        err = esp_fs_ota_perform(fs_ota_handle);
        if (err != ESP_ERR_FS_OTA_IN_PROGRESS) {
            break;
        }
    }

    esp_err_t ota_finish_err = esp_fs_ota_finish(fs_ota_handle);
    if (err != ESP_OK) {
        /* If there was an error in esp_fs_ota_perform(),
           then it is given more precedence than error in esp_fs_ota_finish()
         */
        return err;
    } else if (ota_finish_err != ESP_OK) {
        return ota_finish_err;
    }
    return ESP_OK;
}
