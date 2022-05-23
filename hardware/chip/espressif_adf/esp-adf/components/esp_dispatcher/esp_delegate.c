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

#include <stddef.h>

#include "esp_err.h"
#include "esp_delegate.h"

static void *shared_handle = NULL;

esp_dispatcher_handle_t esp_dispatcher_get_delegate_handle(void)
{
    if (shared_handle == NULL) {
        esp_dispatcher_config_t d_cfg = ESP_DISPATCHER_CONFIG_DEFAULT();
        d_cfg.task_core = CONFIG_ESP_DISPATCHER_DELEGATE_TASK_CORE;
        d_cfg.task_prio = CONFIG_ESP_DISPATCHER_DELEGATE_TASK_PRIO;
        d_cfg.task_stack = CONFIG_ESP_DISPATCHER_DELEGATE_STACK_SIZE;
        d_cfg.stack_in_ext = false;
        shared_handle = esp_dispatcher_create(&d_cfg);
    }
    return shared_handle;
}

esp_err_t esp_dispatcher_release_delegate_handle(void)
{
    esp_err_t ret = ESP_OK;
    if (shared_handle) {
        ret = esp_dispatcher_destroy(shared_handle);
        if (ret == ESP_OK) {
            shared_handle = NULL;
        }
    }
    return ret;
}
