/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "iot_import.h"
#include "iotx_utils.h"

#include "sal_util.h"

#define TAG "SAL_UTIL"

#define SAL_UTILS_MALLOC(size) LITE_malloc(size, MEM_MAGIC, "sal.utils")
#define SAL_UTILS_FREE(ptr)    LITE_free(ptr)

static sal_ringbuf_t *sal_ringbuf_create(int length);
static void sal_ringbuf_destroy(sal_ringbuf_t *buffer);
static int  sal_ringbuf_read(sal_ringbuf_t *ringbuf, void *target,
                             unsigned int ms, unsigned int *size);
static int  sal_ringbuf_write(sal_ringbuf_t *buffer, void *data, int size);
static int  sal_ringbuf_available_write_space(sal_ringbuf_t *buffer);
static int  sal_ringbuf_available_read_space(sal_ringbuf_t *buffer);
static void sal_ringbuf_clear_all(sal_ringbuf_t *buffer);
static int sal_ringbuf_full(sal_ringbuf_t *buffer);
static int sal_ringbuf_empty(sal_ringbuf_t *buffer);

void *HAL_QueueCreate(void *buf, unsigned int size, int max_msg)
{
    (void) buf;
    (void) max_msg;

    return sal_ringbuf_create(size);
}

void HAL_QueueDestroy(void *queue)
{
    sal_ringbuf_destroy((sal_ringbuf_t *) queue);
}

int HAL_QueueSend(void *queue, void *msg, unsigned int size)
{
    return sal_ringbuf_write((sal_ringbuf_t *)queue, msg, size);
}

int HAL_QueueRecv(void *queue, unsigned int ms, void *msg,
                  unsigned int *size)
{
    return sal_ringbuf_read((sal_ringbuf_t *)queue, msg, ms, size);
}

void *HAL_QueueBufPtr(void *queue)
{
    return NULL;
}

static sal_ringbuf_t *sal_ringbuf_create(int length)
{
    sal_ringbuf_t *ringbuf = NULL;

    if (length <= 0) {
        return NULL;
    }

    ringbuf = SAL_UTILS_MALLOC(sizeof(sal_ringbuf_t));
    if (ringbuf == NULL) {
        return NULL;
    }
    memset(ringbuf, 0, sizeof(sal_ringbuf_t));

    ringbuf->buf_sem = HAL_SemaphoreCreate();
    if (ringbuf->buf_sem == NULL) {
        goto err;
    }

    ringbuf->length = length;
    ringbuf->buffer = SAL_UTILS_MALLOC((ringbuf->length + 1) * sizeof(void *));
    if (ringbuf->buffer == NULL) {
        goto err;
    }

    return ringbuf;

err:
    if (ringbuf) {
        if (ringbuf->buf_sem) {
            HAL_SemaphoreDestroy(ringbuf->buf_sem);
            ringbuf->buf_sem = NULL;
        }

        if (ringbuf->buffer) {
            SAL_UTILS_FREE(ringbuf->buffer);
            ringbuf->buffer = NULL;
        }

        SAL_UTILS_FREE(ringbuf);
    }
    return NULL;
}

static void sal_ringbuf_clear_all(sal_ringbuf_t *ringbuf)
{
    while (ringbuf->head != ringbuf->tail) {
        if (((void **)ringbuf->buffer)[ringbuf->head] != NULL) {
            SAL_UTILS_FREE(((void **)ringbuf->buffer)[ringbuf->head]);
            ((void **)ringbuf->buffer)[ringbuf->head] = NULL;
        }

        ringbuf->head = (ringbuf->head + 1) % (ringbuf->length + 1);
    }

    ringbuf->head = ringbuf->tail = 0;
}

static void sal_ringbuf_destroy(sal_ringbuf_t *ringbuf)
{
    if (ringbuf) {
        if (ringbuf->buf_sem) {
            HAL_SemaphoreDestroy(ringbuf->buf_sem);
            ringbuf->buf_sem = NULL;
        }

        if (ringbuf->buffer) {
            sal_ringbuf_clear_all(ringbuf);

            SAL_UTILS_FREE(ringbuf->buffer);
            ringbuf->buffer = NULL;
        }

        SAL_UTILS_FREE(ringbuf);
    }
}

static int sal_ringbuf_available_read_space(sal_ringbuf_t *ringbuf)
{
    if (ringbuf->head == ringbuf->tail) {
        return 0;
    } else if (ringbuf->head < ringbuf->tail) {
        return ringbuf->tail - ringbuf->head;
    } else {
        return ringbuf->length + 1 - (ringbuf->head - ringbuf->tail);
    }
}

static int sal_ringbuf_write(sal_ringbuf_t *ringbuf, void *data, int size)
{
    if (ringbuf == NULL || data == NULL) {
        return -1;
    }

    if (sal_ringbuf_full(ringbuf)) {
        log_err(TAG, "sal ringbuf full!");
        return -1;
    }

    memcpy(&(((void **) ringbuf->buffer)[ringbuf->tail]), data, size);
    ringbuf->tail = (ringbuf->tail + 1) % (ringbuf->length + 1);

    HAL_SemaphorePost(ringbuf->buf_sem);

    return 0;
}

static int sal_ringbuf_read(sal_ringbuf_t *ringbuf, void *target,
                            unsigned int ms, unsigned int *size)
{
    *size = 0;

    if (ringbuf == NULL || target == NULL) {
        return -1;
    }

    HAL_SemaphoreWait(ringbuf->buf_sem, ms);

    if (sal_ringbuf_empty(ringbuf)) {
        return -1;
    }

    memcpy(((void **)target), &((void **)ringbuf->buffer)[ringbuf->head], sizeof(void *));
    ((void **)ringbuf->buffer)[ringbuf->head] = NULL;
    *size = sizeof(void *);
    ringbuf->head = (ringbuf->head + 1) % (ringbuf->length + 1);

    return 0;
}

static int sal_ringbuf_available_write_space(sal_ringbuf_t *ringbuf)
{
    return (ringbuf->length - sal_ringbuf_available_read_space(ringbuf));
}

static int sal_ringbuf_full(sal_ringbuf_t *ringbuf)
{
    return (sal_ringbuf_available_write_space(ringbuf) == 0);
}

static int sal_ringbuf_empty(sal_ringbuf_t *ringbuf)
{
    return (sal_ringbuf_available_read_space(ringbuf) == 0);
}
