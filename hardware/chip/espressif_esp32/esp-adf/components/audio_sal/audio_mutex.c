/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2021 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
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

#include "audio_mutex.h"
#include "audio_idf_version.h"
#include "esp_log.h"

#if (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 3, 0))

// The xQueueSemaphoreTake is not available on FreeRTOS v8.2.0, it's compatible implementation.
BaseType_t __attribute__((weak)) xQueueSemaphoreTake( QueueHandle_t xQueue, TickType_t xTicksToWait )
{
    xSemaphoreTake(xQueue, xTicksToWait);
    return pdPASS;
}
#endif

void *mutex_create(void)
{
    void *handle = NULL;
    handle = xSemaphoreCreateMutex();
    return (void *) handle;
}

int mutex_destroy(void *mutex)
{
    vSemaphoreDelete((QueueHandle_t)mutex);
    return 0;
}

int mutex_lock(void *mutex)
{
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 3, 0))
    while (xSemaphoreTake((QueueHandle_t)mutex, portMAX_DELAY) != pdPASS);
#else
    while (xSemaphoreTake((QueueHandle_t)mutex, portMAX_DELAY) != pdPASS);
#endif
    return 0;
}

int mutex_unlock(void *mutex)
{
    int ret = 0;
    ret = xSemaphoreGive((QueueHandle_t)mutex);
    return ret;
}
