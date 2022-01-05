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
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "sys/queue.h"
#include "esp_log.h"

#include "periph_service.h"
#include "audio_mutex.h"
#include "audio_mem.h"
#include "audio_error.h"
#include "audio_thread.h"

static const char *TAG = "PERIPH_SERVICE";

/**
 * @brief Peripheral service configurations
 */
typedef struct periph_service_impl {
    periph_service_ctrl                 service_start;
    periph_service_ctrl                 service_stop;
    periph_service_ctrl                 service_destroy;
    periph_service_io                   service_ioctl;
    periph_service_cb                   callback_func;
    void                                *user_cb_ctx;
    char                                *service_name;
    TaskHandle_t                        task_handle;
    audio_thread_t                      audio_thread;
    void                                *user_data;
} periph_service_impl_t;

periph_service_handle_t periph_service_create(periph_service_config_t *config)
{
    AUDIO_NULL_CHECK(TAG, config, return NULL);
    periph_service_handle_t impl = audio_calloc(1, sizeof(periph_service_impl_t));
    AUDIO_MEM_CHECK(TAG, impl, return NULL);
    impl->service_start         = config->service_start;
    impl->service_stop          = config->service_stop;
    impl->service_destroy       = config->service_destroy;
    impl->service_ioctl         = config->service_ioctl;
    impl->user_data             = config->user_data;
    if (config->service_name) {
        impl->service_name = audio_strdup(config->service_name);
        AUDIO_MEM_CHECK(TAG, impl, goto serv_failed);
    }
    if (config->task_stack > 0) {
        if ( audio_thread_create(&impl->audio_thread,
                                 config->service_name,
                                 config->task_func,
                                 impl,
                                 config->task_stack,
                                 config->task_prio,
                                 config->extern_stack,
                                 config->task_core) != ESP_OK) {
            ESP_LOGE(TAG, "Create task failed on %s", __func__);
            goto serv_failed;
        }
    }
    return impl;
serv_failed:
    audio_free(impl->service_name);
    impl->service_name = NULL;
    audio_free(impl);
    impl = NULL;
    return impl;
}

esp_err_t periph_service_destroy(periph_service_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    periph_service_impl_t *impl = (periph_service_impl_t *) handle;
    if (impl->service_destroy) {
        impl->service_destroy(handle);
    }
    audio_free(impl->service_name);
    impl->service_name = NULL;
    impl->task_handle = NULL;
    audio_free(impl);
    impl = NULL;
    return ESP_OK;
}

esp_err_t periph_service_start(periph_service_handle_t handle)
{
    periph_service_impl_t *impl = (periph_service_impl_t *) handle;
    AUDIO_NULL_CHECK(TAG, (handle && impl->service_start), return ESP_ERR_INVALID_ARG);
    return impl->service_start(handle);
}

esp_err_t periph_service_stop(periph_service_handle_t handle)
{
    periph_service_impl_t *impl = (periph_service_impl_t *) handle;
    AUDIO_NULL_CHECK(TAG, (handle && impl->service_stop), return ESP_ERR_INVALID_ARG);
    return impl->service_stop(handle);
}

esp_err_t periph_service_set_callback(periph_service_handle_t handle, periph_service_cb cb, void *ctx)
{
    periph_service_impl_t *impl = (periph_service_impl_t *) handle;
    impl->callback_func = cb;
    impl->user_cb_ctx = ctx;
    return ESP_OK;
}

esp_err_t periph_service_callback(periph_service_handle_t handle, periph_service_event_t *evt)
{
    periph_service_impl_t *impl = (periph_service_impl_t *) handle;
    AUDIO_NULL_CHECK(TAG, (handle && impl->callback_func), return ESP_ERR_INVALID_ARG);
    return impl->callback_func(handle, evt, impl->user_cb_ctx);
}

esp_err_t periph_service_set_data(periph_service_handle_t handle, void *data)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_ERR_INVALID_ARG);
    periph_service_impl_t *impl = (periph_service_impl_t *) handle;
    impl->user_data = data;
    return ESP_OK;
}

void *periph_service_get_data(periph_service_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return NULL);
    periph_service_impl_t *impl = (periph_service_impl_t *) handle;
    return impl->user_data;
}

esp_err_t periph_service_ioctl(periph_service_handle_t handle, void *ioctl_handle, int cmd, int value)
{
    periph_service_impl_t *impl = (periph_service_impl_t *) handle;
    AUDIO_NULL_CHECK(TAG, (handle && impl->service_ioctl), return ESP_ERR_INVALID_ARG);
    return impl->service_ioctl(ioctl_handle, cmd, value);
}
