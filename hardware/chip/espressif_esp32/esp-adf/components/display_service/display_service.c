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

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "board.h"
#include "sdkconfig.h"
#include "audio_mem.h"
#include "display_service.h"
#include "esp_log.h"
#include "audio_error.h"
#include "periph_service.h"

#define DISPLAY_TASK_PRIORITY           5
#define DISPLAY_TASK_STACK_SIZE         6*1024

static const char *TAG                  = "DISPLAY";

typedef struct display_service_impl {
    periph_service_handle_t          based;
    void                            *instance;
    char                            *service_name;
    void                            *user_data;
} display_service_impl_t;

display_service_handle_t display_service_create(display_service_config_t *config)
{
    AUDIO_NULL_CHECK(TAG, config, return NULL);
    display_service_impl_t *disp =  audio_calloc(1, sizeof(display_service_impl_t));
    AUDIO_MEM_CHECK(TAG, disp, return NULL);
    config->based_cfg.user_data = (void *)disp;
    disp->based = periph_service_create(&config->based_cfg);
    disp->instance = config->instance;

    return disp;
}

esp_err_t display_service_set_pattern(void *handle, int display_pattern, int value)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    display_service_impl_t *dis = (display_service_impl_t *)handle;
    esp_err_t ret = periph_service_ioctl(dis->based, (void *)dis->instance, display_pattern, value);
    return ret;
}

esp_err_t display_destroy(display_service_handle_t handle)
{
    AUDIO_NULL_CHECK(TAG, handle, return ESP_FAIL);
    periph_service_destroy(handle->based);
    audio_free(handle);
    return ESP_OK;
}
