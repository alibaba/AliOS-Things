/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
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

#include "esp_log.h"
#include "audio_mem.h"
#include "esp_spiffs.h"
#include "periph_spiffs.h"

static const char *TAG = "PERIPH_SPIFFS";

#define VALIDATE_SPIFFS(periph, ret) if (!(periph && esp_periph_get_id(periph) == PERIPH_ID_SPIFFS)) { \
    ESP_LOGE(TAG, "Invalid SPIFFS periph, at line %d", __LINE__);\
    return ret;\
}

#define SPIFFS_DEFAULT_MAX_FILES 5

static esp_err_t periph_spiffs_mount(esp_periph_handle_t periph);
static esp_err_t periph_spiffs_unmount(esp_periph_handle_t periph);

typedef struct {
    char *root;
    char *partition_label;
    size_t max_files;
    bool format_if_mount_failed;
    bool is_mounted;
} periph_spiffs_t;

static esp_err_t _spiffs_run(esp_periph_handle_t self, audio_event_iface_msg_t *msg)
{
    return ESP_OK;
}

static esp_err_t _spiffs_init(esp_periph_handle_t self)
{
    return periph_spiffs_mount(self);;
}

static esp_err_t _spiffs_destroy(esp_periph_handle_t self)
{
    VALIDATE_SPIFFS(self, ESP_FAIL);
    esp_err_t ret = ESP_OK;

    ret |= periph_spiffs_unmount(self);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG,  "Failed to unmount SPIFFS");
    }

    periph_spiffs_t *spiffs = esp_periph_get_data(self);
    audio_free(spiffs->root);
    if (spiffs->partition_label != NULL) {
        audio_free(spiffs->partition_label);
    }
    audio_free(spiffs);
    return ret;
}

esp_err_t periph_spiffs_mount(esp_periph_handle_t periph)
{
    VALIDATE_SPIFFS(periph, ESP_FAIL);

    periph_spiffs_t *spiffs = esp_periph_get_data(periph);

    esp_vfs_spiffs_conf_t conf = {
        .base_path = spiffs->root,
        .partition_label = spiffs->partition_label,
        .max_files = spiffs->max_files,
        .format_if_mount_failed = spiffs->format_if_mount_failed
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            printf("root %s ", spiffs->root);
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%d)", ret);
        }
        return ESP_FAIL;
    }

    if (ret == ESP_OK) {
        ESP_LOGD(TAG, "Mount SPIFFS success");
        spiffs->is_mounted = true;
        size_t total = 0, used = 0;
        ret = esp_spiffs_info(conf.partition_label, &total, &used);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%d)", ret);
        } else {
            ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
        }
        return esp_periph_send_event(periph, SPIFFS_STATUS_MOUNTED, NULL, 0);
    } else if (ret == ESP_ERR_INVALID_STATE) {
        ESP_LOGD(TAG, "Periph SPIFFS handle already mounted!");
        return ESP_OK;
    } else {
        esp_periph_send_event(periph, SPIFFS_STATUS_MOUNT_ERROR, NULL, 0);
        spiffs->is_mounted = false;
        ESP_LOGE(TAG, "Mount SPIFFS error!");
        return ESP_FAIL;
    }
}

esp_err_t periph_spiffs_unmount(esp_periph_handle_t periph)
{
    VALIDATE_SPIFFS(periph, ESP_FAIL);
    periph_spiffs_t *spiffs = esp_periph_get_data(periph);

    int ret = esp_vfs_spiffs_unregister(spiffs->partition_label);
    if (ret == ESP_OK) {
        ESP_LOGD(TAG, "Unmount SPIFFS success");
        spiffs->is_mounted = false;
        return esp_periph_send_event(periph, SPIFFS_STATUS_UNMOUNTED, NULL, 0);
    } else {
        esp_periph_send_event(periph, SPIFFS_STATUS_UNMOUNT_ERROR, NULL, 0);
        ESP_LOGE(TAG, "Unmount SPIFFS error!");
        spiffs->is_mounted = false;
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_periph_handle_t periph_spiffs_init(periph_spiffs_cfg_t *spiffs_cfg)
{
    esp_periph_handle_t periph = esp_periph_create(PERIPH_ID_SPIFFS, "periph_spiffs");
    AUDIO_MEM_CHECK(TAG, periph, return NULL);

    periph_spiffs_t *spiffs = audio_calloc(1, sizeof(periph_spiffs_t));
    AUDIO_MEM_CHECK(TAG, spiffs, {
        audio_free(periph);
        return NULL;
    });
    if (spiffs_cfg->root) {
        spiffs->root = audio_strdup(spiffs_cfg->root);
    } else {
        spiffs->root = audio_strdup("/spiffs");
    }

    if (spiffs_cfg->partition_label) {
        spiffs->partition_label = audio_strdup(spiffs_cfg->partition_label);
    } else {
        spiffs->partition_label = NULL;
    }

    if (spiffs_cfg->max_files < SPIFFS_DEFAULT_MAX_FILES) {
        spiffs->max_files = SPIFFS_DEFAULT_MAX_FILES;
    } else {
        spiffs->max_files = spiffs_cfg->max_files;
    }

    spiffs->format_if_mount_failed = spiffs_cfg->format_if_mount_failed;

    AUDIO_MEM_CHECK(TAG, spiffs->root, {
        audio_free(spiffs);
        audio_free(periph);
        return NULL;
    });

    esp_periph_set_data(periph, spiffs);
    esp_periph_set_function(periph, _spiffs_init, _spiffs_run, _spiffs_destroy);
    return periph;
}

bool periph_spiffs_is_mounted(esp_periph_handle_t periph)
{
    VALIDATE_SPIFFS(periph, ESP_FAIL);
    periph_spiffs_t *spiffs = esp_periph_get_data(periph);
    return spiffs->is_mounted;
}
