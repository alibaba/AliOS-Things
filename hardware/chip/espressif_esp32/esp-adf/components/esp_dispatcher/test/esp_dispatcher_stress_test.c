/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
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

#include "unity.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_action_def.h"
#include "esp_delegate.h"
#include "esp_log.h"

#include "audio_thread.h"

static xQueueHandle que = NULL;

static esp_err_t delay(void *instance, action_arg_t *arg, action_result_t *result)
{
    vTaskDelay(pdMS_TO_TICKS(100));
    result->err = ESP_OK;
    return result->err;
}

static void delegate_sync_task(void *args)
{
    esp_dispatcher_handle_t dispatcher = (esp_dispatcher_handle_t)args;
    action_result_t result = { 0 };
    esp_dispatcher_execute_with_func(dispatcher, delay, NULL, NULL, &result);
    TEST_ASSERT_EQUAL(ESP_OK, result.err);
    int cmd = 0;
    xQueueSend(que, &cmd, portMAX_DELAY);
    vTaskDelete(NULL);
}

TEST_CASE("esp_dispatcher stress test sync", "esp-adf")
{
#define TASK_NUM (200)
    int core_id = 0;
    audio_thread_t thread = NULL;
    esp_dispatcher_config_t d_cfg = ESP_DISPATCHER_CONFIG_DEFAULT();
    d_cfg.stack_in_ext = false;
    void *dispatcher = esp_dispatcher_create(&d_cfg);
    que = xQueueCreate(10, sizeof(uint8_t));

    for (int i = 0; i < TASK_NUM; i++) {
        audio_thread_create(&thread, "sync", delegate_sync_task, dispatcher, 5 * 1024, 5, true, (core_id++) % 2);
    }
    for (int i = 0; i < TASK_NUM; i++) {
        int cmd;
        xQueueReceive(que, &cmd, portMAX_DELAY);
    }
    vQueueDelete(que);
    esp_dispatcher_destroy(dispatcher);
}

static void invoke_cb(action_result_t ret, void *user_data)
{
    int cmd = 0;
    xQueueSend(que, &cmd, portMAX_DELAY);
}

static void delegate_async_task(void *args)
{
    esp_dispatcher_handle_t dispatcher = (esp_dispatcher_handle_t)args;
    TEST_ASSERT_EQUAL(ESP_OK, esp_dispatcher_execute_with_func_async(dispatcher, delay, NULL, NULL, invoke_cb, NULL));

    vTaskDelete(NULL);
}

TEST_CASE("esp_dispatcher stress test async", "esp-adf")
{
#define TASK_NUM (200)
    int core_id = 0;
    audio_thread_t thread = NULL;
    esp_dispatcher_config_t d_cfg = ESP_DISPATCHER_CONFIG_DEFAULT();
    d_cfg.stack_in_ext = false;
    void *dispatcher = esp_dispatcher_create(&d_cfg);
    que = xQueueCreate(10, sizeof(uint8_t));

    for (int i = 0; i < TASK_NUM; i++) {
        audio_thread_create(&thread, "async", delegate_async_task, dispatcher, 5 * 1024, 5, true, (core_id++) % 2);
    }
    for (int i = 0; i < TASK_NUM; i++) {
        int cmd;
        xQueueReceive(que, &cmd, portMAX_DELAY);
    }
    vQueueDelete(que);
    esp_dispatcher_destroy(dispatcher);
}
