/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

/* Implement aos queue apis on freertos kernel.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "aos/kernel.h"
#include "aos/errno.h"

aos_status_t aos_queue_new(aos_queue_t *queue, void *buf, size_t size, size_t max_msg)
{
    QueueHandle_t q;
    (void)(buf);
    /* verify param */
    if (queue == NULL || size == 0 || max_msg == 0) {
        return -1;
    }

    /* create queue object */
    q = xQueueCreate(size / max_msg, max_msg);
    if (q == NULL) {
        return -1;
    }
    *queue = q;

    return 0;
}

void aos_queue_free(aos_queue_t *queue)
{
    /* delete queue object */
    QueueHandle_t p_handle;
    p_handle = (QueueHandle_t)(*queue);
    vQueueDelete(p_handle);

    return;
}

int aos_queue_send(aos_queue_t *queue, void *msg, unsigned int size)
{
    QueueHandle_t p_handle;

    /* verify param */
    if (queue == NULL || msg == NULL || size == 0) {
        return -1;
    }

    p_handle = (QueueHandle_t)(*queue);
    /* send msg  to specific queue */
    return xQueueSend(p_handle, msg, portMAX_DELAY) == pdPASS ? 0 : -1;
}

int aos_queue_recv(aos_queue_t *queue, unsigned int ms, void *msg,
                   unsigned int *size)
{
    QueueHandle_t p_handle;

    /* verify param */
    if (queue == NULL || msg == NULL || size == 0) {
        return -1;
    }

    p_handle = (QueueHandle_t)(*queue);

    /* receive msg from specific queue */
    return xQueueReceive(p_handle, msg, ms == AOS_WAIT_FOREVER ? portMAX_DELAY : pdMS_TO_TICKS(ms)) == pdPASS ? 0 : -1;

}

void *aos_queue_buf_ptr(aos_queue_t *queue)
{
    // todo  freertos doesn't support this feature
    (void)queue;
    return NULL;
}

int aos_queue_get_count(aos_queue_t *queue)
{
    QueueHandle_t p_handle;

    /* verify param */
    if (queue == NULL) {
        return -1;
    }

    p_handle = (QueueHandle_t)(*queue);

    return (int)uxQueueMessagesWaiting(p_handle);
}
