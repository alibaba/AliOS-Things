/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "aos_errno.h"
#include "aos_kernel.h"
#include "k_ringbuf.h"

typedef struct {
    void         *buf;               /**< ringbuf address */
    k_ringbuf_t  ringbuf;           /**< ringbuf management */
    size_t       max_msg_size;      /**< limited message length */
    size_t       cur_num;           /**< msgs used */
    size_t       peak_num;          /**< maximum msgs used */
} buf_queue_t;

aos_status_t aos_queue_new(aos_queue_t *queue, void *buf, size_t size, size_t max_msg)
{
    buf_queue_t *q;
    size_t malloc_len;

    if (queue == NULL || size == 0) {
        return -EINVAL;
    }

    malloc_len = sizeof(buf_queue_t) + ((buf == NULL) ? size : 0);
    q = (buf_queue_t *)aos_malloc(malloc_len);
    if (q == NULL) {
        return -ENOMEM;
    }

    memset(q, 0, sizeof(buf_queue_t));
    q->buf = buf;
    q->cur_num = 0u;
    q->max_msg_size = max_msg;

    ringbuf_init(&(q->ringbuf), buf, size, max_msg, RINGBUF_TYPE_DYN);

    *queue = q;
    return 0;
}

void aos_queue_free(aos_queue_t *queue)
{
    if (queue == NULL) {
        return;
    }
    buf_queue_t *q = (buf_queue_t *)queue;
    ringbuf_reset(&(q->ringbuf));
    aos_free(queue);
    *queue = NULL;
    return;
}

aos_status_t aos_queue_send(aos_queue_t *queue, void *msg, size_t size)
{
    if (queue == NULL) {
        return -EINVAL;
    }
    buf_queue_t *q = (buf_queue_t *)queue;
    ringbuf_push(&(q->ringbuf), msg, size);
    return 0;
}

aos_status_t aos_queue_recv(aos_queue_t *queue, uint32_t ms, void *msg,
                   size_t *size)
{
    if (queue == NULL) {
        return -EINVAL;
    }
    buf_queue_t *q = (buf_queue_t *)queue;
    ringbuf_pop(&(q->ringbuf), msg, size);
    return 0;
}

void *aos_queue_buf_ptr(aos_queue_t *queue)
{
    return ((buf_queue_t *)*queue)->buf;
}

aos_status_t aos_queue_get_count(aos_queue_t *queue)
{
    return 0;
}