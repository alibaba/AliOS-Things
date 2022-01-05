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
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "audio_mem.h"
#include "audio_error.h"
#include "audio_thread.h"

static const char *TAG = "AUDIO_THREAD";

BaseType_t __attribute__((weak)) xTaskCreateRestrictedPinnedToCore(const TaskParameters_t *const pxTaskDefinition, TaskHandle_t *pxCreatedTask, const BaseType_t xCoreID)
{
    ESP_LOGE(TAG, "Not found right %s.\r\nPlease enter IDF-PATH with \"cd $IDF_PATH\" and apply the IDF patch with \"git apply $ADF_PATH/idf_patches/idf_v3.3_freertos.patch\" first\r\n", __func__);
    return pdFALSE;
}

esp_err_t audio_thread_create(audio_thread_t *p_handle, const char *name, void(*main_func)(void *arg), void *arg,
                              uint32_t stack, int prio, bool stack_in_ext, int core_id)
{
    StackType_t *task_stack = NULL;
    if (stack_in_ext && audio_mem_spiram_stack_is_enabled()) {
        /*
         * Note: 1. ESP32-ECO3 chip support stack on external memory only.
         *       2. Make sure selected the `CONFIG_SPIRAM_BOOT_INIT` and `CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY` by `make menuconfig`
         *       3. Please apply the $ADF_PATH/idf_patches/idf_v3.3_freertos.patch
         *
         */
        ESP_LOGI(TAG, "The %s task allocate stack on external memory", name);
        /* task_stack freed by freertos*/
        task_stack = (StackType_t *) audio_calloc(1, stack);
        AUDIO_MEM_CHECK(TAG, task_stack, goto audio_thread_create_error );
        TaskParameters_t xRegParameters = {
            .pvTaskCode = main_func,
            .pcName = name,
            .usStackDepth = stack,
            .pvParameters = arg,
            .uxPriority = prio | portPRIVILEGE_BIT,
            .puxStackBuffer = task_stack,
            .xRegions = {{
                    .pvBaseAddress = 0x00,
                    .ulLengthInBytes = 0x00,
                    .ulParameters = 0x00,
                }
            }
        };
        if (xTaskCreateRestrictedPinnedToCore(&xRegParameters, (xTaskHandle)p_handle, core_id) != pdPASS) {
            ESP_LOGE(TAG, "Error creating RestrictedPinnedToCore %s", name);
            goto audio_thread_create_error;
        }
    } else {
        if (xTaskCreatePinnedToCore(main_func, name, stack, arg, prio, (xTaskHandle)p_handle, core_id) != pdPASS) {
            ESP_LOGE(TAG, "Error creating task %s", name);
            goto audio_thread_create_error;
        }
    }
    return ESP_OK;

audio_thread_create_error:
    if (task_stack) {
        audio_free(task_stack);
    }
    return ESP_FAIL;
}

esp_err_t audio_thread_cleanup(audio_thread_t *p_handle)
{
    // TODO nothing
    return ESP_OK;
}

esp_err_t audio_thread_delete_task(audio_thread_t *p_handle)
{
    vTaskDelete(NULL);
    return ESP_OK; /* Control never reach here if this is self delete */
}
