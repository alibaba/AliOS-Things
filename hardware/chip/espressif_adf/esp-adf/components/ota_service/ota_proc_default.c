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

#include "audio_mem.h"
#include "esp_https_ota.h"
#include "esp_fs_ota.h"
#include "esp_log.h"
#include "fatfs_stream.h"
#include "http_stream.h"

#include "ota_proc_default.h"

#define READER_BUF_LEN (1024 * 2)

typedef struct {
    audio_element_handle_t r_stream;
    const esp_partition_t *partition;

    int write_offset;
    char read_buf[READER_BUF_LEN];
} ota_data_upgrade_ctx_t;

typedef struct {
    void *ota_handle;
    esp_err_t (*get_img_desc)(void *handle, esp_app_desc_t *new_app_info);
    esp_err_t (*perform)(void *handle);
    int (*get_image_len_read)(void *handle);
    bool (*all_read)(void *handle);
    esp_err_t (*finish)(void *handle);
} ota_app_upgrade_ctx_t;

static const char *TAG = "OTA_DEFAULT";

static ota_service_err_reason_t validate_image_header(esp_app_desc_t *new_app_info)
{
    if (new_app_info == NULL) {
        return OTA_SERV_ERR_REASON_NULL_POINTER;
    }

    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_app_desc_t running_app_info;
    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK) {
        ESP_LOGI(TAG, "Running firmware version: %s, the incoming firmware version %s", running_app_info.version, new_app_info->version);
    }

    if (ota_get_version_number(new_app_info->version) < 0) {
        ESP_LOGE(TAG, "Error version incoming");
        return OTA_SERV_ERR_REASON_ERROR_VERSION;
    }

    if (ota_get_version_number(new_app_info->version) <= ota_get_version_number(running_app_info.version)) {
        ESP_LOGW(TAG, "Current running version is the same as or higher than a new. We will not continue the update.");
        return OTA_SERV_ERR_REASON_NO_HIGHER_VERSION;
    }
    return OTA_SERV_ERR_REASON_SUCCESS;
}

static ota_service_err_reason_t ota_app_partition_prepare(void **handle, ota_node_attr_t *node)
{
    ota_app_upgrade_ctx_t *context = audio_calloc(1, sizeof(ota_app_upgrade_ctx_t));
    AUDIO_NULL_CHECK(TAG, context, return OTA_SERV_ERR_REASON_NULL_POINTER);
    *handle = context;

    if (strstr(node->uri, "file://")) {
        context->get_img_desc = esp_fs_ota_get_img_desc;
        context->perform = esp_fs_ota_perform;
        context->get_image_len_read = esp_fs_ota_get_image_len_read;
        context->all_read = NULL;
        context->finish = esp_fs_ota_finish;

        esp_fs_ota_config_t ota_config = {
            .path = node->uri,
            .buffer_size = 5 * 1024,
        };
        esp_err_t err = esp_fs_ota_begin(&ota_config, &context->ota_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "ESP FS OTA Begin failed");
            return OTA_SERV_ERR_REASON_UNKNOWN;
        }
    } else if (strstr(node->uri, "https://") || strstr(node->uri, "http://")) {
        context->get_img_desc = esp_https_ota_get_img_desc;
        context->perform = esp_https_ota_perform;
        context->get_image_len_read = esp_https_ota_get_image_len_read;
        context->all_read = esp_https_ota_is_complete_data_received;
        context->finish = esp_https_ota_finish;

        esp_http_client_config_t config = {
            .url = node->uri,
            .cert_pem = node->cert_pem,
            .timeout_ms = 5000,
        };
        esp_https_ota_config_t ota_config = {
            .http_config = &config,
        };
        esp_err_t err = esp_https_ota_begin(&ota_config, &context->ota_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "ESP HTTPS OTA Begin failed");
            return OTA_SERV_ERR_REASON_UNKNOWN;
        }
    } else {
        return OTA_SERV_ERR_REASON_URL_PARSE_FAIL;
    }

    return OTA_SERV_ERR_REASON_SUCCESS;
}

static ota_service_err_reason_t ota_app_partition_need_upgrade(void *handle, ota_node_attr_t *node)
{
    ota_app_upgrade_ctx_t *context = (ota_app_upgrade_ctx_t *)handle;
    AUDIO_NULL_CHECK(TAG, context, return OTA_SERV_ERR_REASON_NULL_POINTER);
    AUDIO_NULL_CHECK(TAG, context->ota_handle, return OTA_SERV_ERR_REASON_NULL_POINTER);
    esp_app_desc_t app_desc;
    esp_err_t err = context->get_img_desc(context->ota_handle, &app_desc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "get_img_desc failed");
        return OTA_SERV_ERR_REASON_GET_NEW_APP_DESC_FAIL;
    }
    return validate_image_header(&app_desc);
}

static esp_err_t ota_app_partition_exec_upgrade(void *handle, ota_node_attr_t *node)
{
    ota_app_upgrade_ctx_t *context = (ota_app_upgrade_ctx_t *)handle;
    AUDIO_NULL_CHECK(TAG, context, return OTA_SERV_ERR_REASON_NULL_POINTER);
    AUDIO_NULL_CHECK(TAG, context->ota_handle, return OTA_SERV_ERR_REASON_NULL_POINTER);
    esp_err_t err = ESP_FAIL;

    while (1) {
        err = context->perform(context->ota_handle);
        if (err != ESP_ERR_HTTPS_OTA_IN_PROGRESS && err != ESP_ERR_FS_OTA_IN_PROGRESS ) {
            break;
        }
        ESP_LOGI(TAG, "Image bytes read: %d", context->get_image_len_read(context->ota_handle));
    }

    return err;
}

static esp_err_t ota_app_partition_finish(void *handle, ota_node_attr_t *node, ota_service_err_reason_t result)
{
    ota_app_upgrade_ctx_t *context = (ota_app_upgrade_ctx_t *)handle;
    AUDIO_NULL_CHECK(TAG, context->ota_handle, return OTA_SERV_ERR_REASON_NULL_POINTER);
    esp_err_t err = context->finish(context->ota_handle);
    if (err != ESP_OK) {
        if (err == ESP_ERR_OTA_VALIDATE_FAILED) {
            ESP_LOGE(TAG, "Image validation failed, image is corrupted");
        }
        ESP_LOGE(TAG, "upgrade failed %d", err);
    }
    audio_free(handle);
    return err;
}

void ota_app_get_default_proc(ota_upgrade_ops_t *ops)
{
    ops->prepare = ota_app_partition_prepare;
    ops->need_upgrade = ota_app_partition_need_upgrade;
    ops->execute_upgrade = ota_app_partition_exec_upgrade;
    ops->finished_check = ota_app_partition_finish;
}

static ota_service_err_reason_t ota_data_partition_prepare(void **handle, ota_node_attr_t *node)
{
    ota_data_upgrade_ctx_t *context = audio_calloc(1, sizeof(ota_data_upgrade_ctx_t));
    AUDIO_NULL_CHECK(TAG, context, return OTA_SERV_ERR_REASON_NULL_POINTER);
    *handle = context;

    context->partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, node->label);
    if (context->partition == NULL) {
        ESP_LOGE(TAG, "partition [%s] not found", node->label);
        return OTA_SERV_ERR_REASON_PARTITION_NOT_FOUND;
    }

    ESP_LOGI(TAG, "data upgrade uri %s", node->uri);
    if (strstr(node->uri, "file://")) {
        fatfs_stream_cfg_t fs_cfg = FATFS_STREAM_CFG_DEFAULT();
        fs_cfg.type = AUDIO_STREAM_READER;

        context->r_stream = fatfs_stream_init(&fs_cfg);
    } else if (strstr(node->uri, "https://") || strstr(node->uri, "http://")) {
        http_stream_cfg_t http_cfg = HTTP_STREAM_CFG_DEFAULT();
        http_cfg.type = AUDIO_STREAM_READER;

        context->r_stream = http_stream_init(&http_cfg);
    } else {
        ESP_LOGE(TAG, "not support uri");
        return OTA_SERV_ERR_REASON_URL_PARSE_FAIL;
    }
    audio_element_set_uri(context->r_stream, node->uri);
    if (audio_element_process_init(context->r_stream) != ESP_OK) {
        ESP_LOGE(TAG, "reader stream init failed");
        return OTA_SERV_ERR_REASON_STREAM_INIT_FAIL;
    }

    return OTA_SERV_ERR_REASON_SUCCESS;
}

static ota_service_err_reason_t ota_data_partition_exec_upgrade(void *handle, ota_node_attr_t *node)
{
    int r_size = 0;
    ota_data_upgrade_ctx_t *context = (ota_data_upgrade_ctx_t *)handle;
    AUDIO_NULL_CHECK(TAG, context, return OTA_SERV_ERR_REASON_NULL_POINTER);
    AUDIO_NULL_CHECK(TAG, context->r_stream, return OTA_SERV_ERR_REASON_NULL_POINTER);
    AUDIO_NULL_CHECK(TAG, context->partition, return OTA_SERV_ERR_REASON_NULL_POINTER);
    esp_err_t ret = ESP_OK;

    if ((ret = esp_partition_erase_range(context->partition, 0, context->partition->size)) != ESP_OK) {
        ESP_LOGE(TAG, "Erase [%s] partition failed, return value: %d", node->label, ret);
        return OTA_SERV_ERR_REASON_PARTITION_WT_FAIL;
    }

    while ((r_size = audio_element_input(context->r_stream, context->read_buf, READER_BUF_LEN)) > 0) {
        ESP_LOGI(TAG, "write_offset %d, r_size %d", context->write_offset, r_size);
        if (esp_partition_write(context->partition, context->write_offset, context->read_buf, r_size) == ESP_OK) {
            context->write_offset += r_size;
        } else {
            return OTA_SERV_ERR_REASON_PARTITION_WT_FAIL;
        }
    }
    if (r_size == AEL_IO_OK || r_size == AEL_IO_DONE) {
        ESP_LOGI(TAG, "partition %s upgrade successes", node->label);
        return OTA_SERV_ERR_REASON_SUCCESS;
    } else {
        return OTA_SERV_ERR_REASON_STREAM_RD_FAIL;
    }
}

static ota_service_err_reason_t ota_data_partition_finish(void *handle, ota_node_attr_t *node, ota_service_err_reason_t result)
{
    ota_data_upgrade_ctx_t *context = (ota_data_upgrade_ctx_t *)handle;
    AUDIO_NULL_CHECK(TAG, context->r_stream, return ESP_FAIL);
    audio_element_process_deinit(context->r_stream);
    audio_element_deinit(context->r_stream);

    audio_free(handle);
    return result;
}

void ota_data_get_default_proc(ota_upgrade_ops_t *ops)
{
    ops->prepare = ota_data_partition_prepare;
    ops->need_upgrade = NULL;
    ops->execute_upgrade = ota_data_partition_exec_upgrade;
    ops->finished_check = ota_data_partition_finish;
}

ota_service_err_reason_t ota_data_image_stream_read(void *handle, char *buf, int wanted_size)
{
    ota_data_upgrade_ctx_t *context = (ota_data_upgrade_ctx_t *)handle;

    if (context == NULL) {
        ESP_LOGE(TAG, "run prepare first");
        return OTA_SERV_ERR_REASON_NULL_POINTER;
    }
    AUDIO_NULL_CHECK(TAG, context->r_stream, return OTA_SERV_ERR_REASON_NULL_POINTER);
    int r_size = 0;
    do {
        int ret = audio_element_input(context->r_stream, buf, wanted_size - r_size);
        if (ret > 0) {
            r_size += ret;
        } else {
            break;
        }
    } while (r_size < wanted_size);

    if (r_size == wanted_size) {
        return OTA_SERV_ERR_REASON_SUCCESS;
    } else {
        return OTA_SERV_ERR_REASON_STREAM_RD_FAIL;
    }
}

ota_service_err_reason_t ota_data_partition_write(void *handle, char *buf, int size)
{
    ota_data_upgrade_ctx_t *context = (ota_data_upgrade_ctx_t *)handle;

    if (context == NULL) {
        ESP_LOGE(TAG, "run prepare first");
        return OTA_SERV_ERR_REASON_NULL_POINTER;
    }

    ESP_LOGI(TAG, "write_offset %d, size %d", context->write_offset, size);
    if (esp_partition_write(context->partition, context->write_offset, buf, size) == ESP_OK) {
        context->write_offset += size;
        return OTA_SERV_ERR_REASON_SUCCESS;
    } else {
        return OTA_SERV_ERR_REASON_PARTITION_WT_FAIL;
    }
}

int ota_get_version_number(char *version)
{
    AUDIO_NULL_CHECK(TAG, version, return -1);
    if (strlen(version) > 32) {
        ESP_LOGE(TAG, "Invalid version");
        return -1;
    }
    int ver_num = 0;
    char *ver = audio_calloc(1, strlen(version) + 1);
    AUDIO_NULL_CHECK(TAG, ver, return -1);
    memcpy(ver, version, strlen(version));
    char *p = strtok(ver, ".");
    AUDIO_NULL_CHECK(TAG, p, goto _err_ver);

    if (p[0] == 'v' || p[0] == 'V') {
        p++;
        int cnt = 2; // expect max version V255.255.255
        while (p) {
            for (int i = 0; i < strlen(p); i++) {
                if (p[i] < '0' || p[i] > '9') {
                    goto _err_ver;
                }
            }
            if (atoi(p) > 255) {
                goto _err_ver;
            }
            ver_num |= (atoi(p) << (cnt * 8));
            p = strtok(NULL, ".");
            cnt--;
        }
        if (cnt != -1) {
            goto _err_ver;
        }
        free(ver);
        return ver_num;
    } else {
        goto _err_ver;
    }

_err_ver:
    ESP_LOGE(TAG, "Got invalid version: %s, the version should be (V0.0.0 - V255.255.255)", version);
    free(ver);
    return -1;
}
