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

#include "audio_queue.h"

audio_queue_handle_t audio_queue_create(uint32_t queue_len, uint32_t item_size)
{
    audio_queue_handle_t handle = NULL;
    handle = (QueueHandle_t)xQueueCreate(queue_len, item_size);
    return handle;
}

int audio_queue_delete(audio_queue_handle_t queue)
{
    vQueueDelete((QueueHandle_t)queue);
    return 0;
}

int audio_queue_send(audio_queue_handle_t queue, void *item, uint32_t block_time_ms)
{
    int ret = 0;
    ret = xQueueSend((QueueHandle_t)queue, item, block_time_ms / portTICK_PERIOD_MS);
    return ret;
}

int audio_queue_send_to_front(audio_queue_handle_t queue, void *item, uint32_t block_time_ms)
{
    int ret = 0;
    ret = xQueueSendToFront((QueueHandle_t)queue, item, block_time_ms / portTICK_PERIOD_MS);
    return ret;
}

int audio_queue_recv(audio_queue_handle_t queue, void *item, uint32_t block_time_ms)
{
    int ret = 0;
    ret = xQueueReceive((QueueHandle_t)queue, item, block_time_ms / portTICK_PERIOD_MS);
    return ret;
}

int audio_queue_message_available(const audio_queue_handle_t queue)
{
    int ret = 0;
    ret = uxQueueMessagesWaiting((QueueHandle_t)queue);
    return ret;
}

int audio_queue_spaces_available(const audio_queue_handle_t queue)
{
    int ret = 0;
    ret = uxQueueSpacesAvailable((QueueHandle_t)queue);
    return ret;
}
