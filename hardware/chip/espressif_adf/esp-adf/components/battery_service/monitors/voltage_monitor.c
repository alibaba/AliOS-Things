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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"

#include "esp_log.h"

#include "audio_error.h"
#include "audio_mem.h"
#include "audio_mutex.h"

#include "voltage_monitor.h"

typedef struct {
    SemaphoreHandle_t mutex;
    vol_monitor_param_t *config;
    vol_monitor_event_cb event_cb;
    void *user_ctx;

    esp_timer_handle_t check_timer;
    int read_cnt;
    int report_start;
    bool full_reported;
    bool low_reported;
} vol_monitor_ctx_t;

static const char *TAG = "VOL_MONITOR";

static void vol_check_timer_hdlr(void *arg)
{
    vol_monitor_ctx_t *vol_monitor = (vol_monitor_ctx_t *)arg;

    if (vol_monitor == NULL || vol_monitor->event_cb == NULL || vol_monitor->config->user_data == NULL) {
        return;
    }

    mutex_lock(vol_monitor->mutex);

    int voltage = vol_monitor->config->vol_get(vol_monitor->config->user_data);;

    if (vol_monitor->report_start != 0 && ++vol_monitor->read_cnt % vol_monitor->report_start == 0) {
        vol_monitor->read_cnt = 0;
        vol_monitor->event_cb(VOL_MONITOR_EVENT_FREQ_REPORT, (void *)voltage, vol_monitor->user_ctx);
    }

    if (vol_monitor->config->vol_low_threshold != 0) {
        if (vol_monitor->low_reported == false && voltage <= vol_monitor->config->vol_low_threshold) {
            vol_monitor->event_cb(VOL_MONITOR_EVENT_BAT_LOW, (void *)voltage, vol_monitor->user_ctx);
            vol_monitor->low_reported = true;
        } else if (voltage > vol_monitor->config->vol_low_threshold) {
            vol_monitor->low_reported = false;
        }
    }

    if (vol_monitor->config->vol_full_threshold != 0) {
        if (vol_monitor->full_reported == false && voltage >= vol_monitor->config->vol_full_threshold) {
            vol_monitor->event_cb(VOL_MONITOR_EVENT_BAT_FULL, (void *)voltage, vol_monitor->user_ctx);
            vol_monitor->full_reported = true;
        } else if (voltage < vol_monitor->config->vol_full_threshold) {
            vol_monitor->full_reported = false;
        }
    }
    mutex_unlock(vol_monitor->mutex);
}

static bool vol_monitor_param_check(vol_monitor_param_t *config)
{
    if (config->init == NULL) {
        ESP_LOGE(TAG, "init NULL");
        return false;
    }
    if (config->deinit == NULL) {
        ESP_LOGE(TAG, "deinit NULL");
        return false;
    }
    if (config->vol_get == NULL) {
        ESP_LOGE(TAG, "vol_get NULL");
        return false;
    }
    if (config->read_freq <= 0) {
        ESP_LOGE(TAG, "Read freq <= 0");
        return false;
    }
    if (config->report_freq < 0) {
        ESP_LOGE(TAG, "report freq < 0");
        return false;
    }
    if (config->vol_full_threshold < 0 ) {
        ESP_LOGE(TAG, "vol_full_threshold < 0");
        return false;
    }
    if (config->vol_low_threshold < 0 ) {
        ESP_LOGE(TAG, "vol_low_threshold < 0");
        return false;
    }
    if (config->vol_full_threshold != 0 && config->vol_low_threshold >= config->vol_full_threshold) {
        ESP_LOGE(TAG, "vol_low_threshold >= vol_full_threshold");
        return false;
    }
    return true;
}

vol_monitor_handle_t vol_monitor_create(vol_monitor_param_t *config)
{
    AUDIO_NULL_CHECK(TAG, config, return NULL);

    vol_monitor_ctx_t *vol_monitor = audio_calloc(1, sizeof(vol_monitor_ctx_t));
    AUDIO_MEM_CHECK(TAG, vol_monitor, return NULL);
    if (!vol_monitor_param_check(config)) {
        goto error;
    }
    vol_monitor->config = config;
    vol_monitor->config->init(vol_monitor->config->user_data);
    vol_monitor->mutex = mutex_create();
    /* init timer */
    if (vol_monitor->config->read_freq > 0) {
        const esp_timer_create_args_t timer_args = {
            .callback = vol_check_timer_hdlr,
            .arg = vol_monitor,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "report",
        };
        if (esp_timer_create(&timer_args, &vol_monitor->check_timer) != ESP_OK
            || esp_timer_start_periodic(vol_monitor->check_timer, vol_monitor->config->read_freq * 1000 * 1000) != ESP_OK) {
            goto error;
        }
    }

    return vol_monitor;

error:
    ESP_LOGE(TAG, "vol_monitor_create failed");
    if (vol_monitor->check_timer != NULL) {
        esp_timer_delete(vol_monitor->check_timer);
    }
    if (vol_monitor->mutex) {
        mutex_destroy(vol_monitor->mutex);
    }
    if (vol_monitor) {
        free(vol_monitor);
    }
    return NULL;
}

esp_err_t vol_monitor_destroy(vol_monitor_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    vol_monitor_ctx_t *vol_monitor = (vol_monitor_ctx_t *)handle;
    if (vol_monitor->check_timer != NULL) {
        esp_timer_stop(vol_monitor->check_timer);
        esp_timer_delete(vol_monitor->check_timer);
        vol_monitor->check_timer = NULL;
    }
    vol_monitor->config->deinit(vol_monitor->config->user_data);
    if (vol_monitor->mutex) {
        mutex_destroy(vol_monitor->mutex);
    }
    if (vol_monitor) {
        free(vol_monitor);
    }
    ESP_LOGI(TAG, "vol monitor destroyed");
    return ESP_OK;
}

esp_err_t vol_monitor_set_event_cb(vol_monitor_handle_t handle, vol_monitor_event_cb event_cb, void *user_ctx)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);

    vol_monitor_ctx_t *vol_monitor = (vol_monitor_ctx_t *)handle;
    mutex_lock(vol_monitor->mutex);
    vol_monitor->event_cb = event_cb;
    vol_monitor->user_ctx = user_ctx;
    mutex_unlock(vol_monitor->mutex);
    return ESP_OK;
}

esp_err_t vol_monitor_start_freq_report(vol_monitor_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    vol_monitor_ctx_t *vol_monitor = (vol_monitor_ctx_t *)handle;
    esp_err_t ret = ESP_OK;
    mutex_lock(vol_monitor->mutex);
    if (vol_monitor->config->report_freq > 0) {
        vol_monitor->report_start = vol_monitor->config->report_freq;
        ret = ESP_OK;
    } else {
        ESP_LOGW(TAG, "report freq <= 0");
        ret = ESP_FAIL;
    }
    mutex_unlock(vol_monitor->mutex);
    return ret;
}

esp_err_t vol_monitor_stop_freq_report(vol_monitor_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    vol_monitor_ctx_t *vol_monitor = (vol_monitor_ctx_t *)handle;
    mutex_lock(vol_monitor->mutex);
    vol_monitor->report_start = 0;
    mutex_unlock(vol_monitor->mutex);
    return ESP_OK;
}

esp_err_t vol_monitor_set_report_freq(vol_monitor_handle_t handle, int freq)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    vol_monitor_ctx_t *vol_monitor = (vol_monitor_ctx_t *)handle;
    mutex_lock(vol_monitor->mutex);
    vol_monitor->config->report_freq = freq;
    if (vol_monitor->config->report_freq > 0 && vol_monitor->report_start > 0) {
        vol_monitor->read_cnt = 0;
        vol_monitor->report_start = vol_monitor->config->report_freq;
    } else if (vol_monitor->config->report_freq == 0) {
        vol_monitor->read_cnt = 0;
        vol_monitor->report_start = 0;
    }
    mutex_unlock(vol_monitor->mutex);
    return ESP_OK;
}
