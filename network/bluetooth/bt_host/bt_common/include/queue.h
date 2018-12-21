/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef BLE_UTILS_QUEUE_H
#define BLE_UTILS_QUEUE_H

#include <misc/slist.h>
#include <misc/dlist.h>

struct k_queue {
    sys_slist_t data_q;
    sys_dlist_t poll_events;
};

#define _POLL_EVENT_OBJ_INIT(obj) \
    .poll_events = SYS_DLIST_STATIC_INIT(&obj.poll_events),

#define K_QUEUE_INITIALIZER(obj) {  \
    .data_q = SYS_SLIST_STATIC_INIT(&obj.data_q),  \
    _POLL_EVENT_OBJ_INIT(obj) \
}

void k_queue_init(struct k_queue *queue);
void k_queue_cancel_wait(struct k_queue *queue);
void k_queue_append(struct k_queue *queue, void *data);
void k_queue_prepend(struct k_queue *queue, void *data);
void k_queue_insert(struct k_queue *queue, void *prev, void *data);
void k_queue_append_list(struct k_queue *queue, void *head, void *tail);
void *k_queue_get(struct k_queue *queue, s32_t timeout);
int k_queue_is_empty(struct k_queue *queue);

static inline bool k_queue_remove(struct k_queue *queue, void *data)
{
    return sys_slist_find_and_remove(&queue->data_q, (sys_snode_t *)data);
}

static inline void *k_queue_first_entry(struct k_queue *queue)
{
    return sys_slist_peek_head(&queue->data_q);
}

struct k_lifo {
    struct k_queue _queue;
};

#define K_LIFO_INITIALIZER(obj) {  \
    ._queue = K_QUEUE_INITIALIZER(obj._queue) \
}

#define k_lifo_init(lifo) k_queue_init((struct k_queue *)lifo)
#define k_lifo_put(lifo, data) k_queue_prepend((struct k_queue *)lifo, data)
#define k_lifo_get(lifo, timeout) k_queue_get((struct k_queue *)lifo, timeout)

#define K_LIFO_DEFINE(name) struct k_lifo name = K_LIFO_INITIALIZER(name)

struct k_fifo {
    struct k_queue _queue;
};

#define K_FIFO_INITIALIZER(obj) {  \
    ._queue = K_QUEUE_INITIALIZER(obj._queue) \
}

#define k_fifo_init(fifo) k_queue_init((struct k_queue *)fifo)
#define k_fifo_cancel_wait(fifo) k_queue_cancel_wait((struct k_queue *)fifo)
#define k_fifo_put(fifo, data) k_queue_append((struct k_queue *)fifo, data)
#define k_fifo_put_list(fifo, head, tail) k_queue_append_list((struct k_queue *)fifo, head, tail)
#define k_fifo_get(fifo, timeout) k_queue_get((struct k_queue *)fifo, timeout)

#define K_FIFO_DEFINE(name) struct k_fifo name = K_FIFO_INITIALIZER(name)

#endif  // BLE_UTILS_QUEUE_H
