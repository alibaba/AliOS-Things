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
#include "esp_wifi_setting.h"
#include "audio_mem.h"
#include "wifi_service.h"

static const char *TAG = "ESP_WIFI_SETTING";

struct esp_wifi_setting {
    wifi_setting_func           start;
    wifi_setting_func           stop;
    wifi_setting_teardown_func  teardown;
    bool                        running;
    char                        *tag;
    void                        *user_data;
    void                        *notify_handle;
};

esp_wifi_setting_handle_t esp_wifi_setting_create(const char *tag)
{
    esp_wifi_setting_handle_t new_entry = audio_calloc(1, sizeof(struct esp_wifi_setting));
    AUDIO_MEM_CHECK(TAG, new_entry, return NULL);
    if (tag) {
        new_entry->tag = audio_strdup(tag);
    } else {
        new_entry->tag = audio_strdup("wifi_setting");
    }
    AUDIO_MEM_CHECK(TAG, new_entry->tag, {
        audio_free(new_entry);
        return NULL;
    })
    return new_entry;
}
esp_err_t esp_wifi_setting_destroy(esp_wifi_setting_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    audio_free(handle);
    return ESP_OK;
}

esp_err_t esp_wifi_setting_register_function(esp_wifi_setting_handle_t handle,
        wifi_setting_func start,
        wifi_setting_func stop,
        wifi_setting_teardown_func teardown)
{
    handle->start = start;
    handle->stop = stop;
    handle->teardown = teardown;
    return ESP_OK;
}

esp_err_t esp_wifi_setting_regitster_notify_handle(esp_wifi_setting_handle_t handle, void *on_handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    handle->notify_handle = on_handle;
    return ESP_OK;
}

esp_err_t esp_wifi_setting_info_notify(esp_wifi_setting_handle_t handle, wifi_config_t *info)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    wifi_service_update_sta_info((periph_service_handle_t)handle->notify_handle, info);
    return ESP_OK;
}

esp_err_t esp_wifi_setting_set_data(esp_wifi_setting_handle_t handle, void *data)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    handle->user_data = data;
    return ESP_OK;
}

void *esp_wifi_setting_get_data(esp_wifi_setting_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return NULL);
    return handle->user_data;
}

esp_err_t esp_wifi_setting_start(esp_wifi_setting_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    if (handle->start) {
        return handle->start(handle);
    }
    return ESP_OK;
}

esp_err_t esp_wifi_setting_stop(esp_wifi_setting_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    if (handle->stop) {
        return handle->stop(handle);
    }
    return ESP_OK;
}

esp_err_t esp_wifi_setting_teardown(esp_wifi_setting_handle_t handle, wifi_config_t *info)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    if (handle->teardown) {
        return handle->teardown(handle, info);
    }
    return ESP_OK;
}
