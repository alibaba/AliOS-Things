/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <zephyr.h>
#include <misc/util.h>
#include <misc/dlist.h>
#include "queue.h"

void k_queue_init(struct k_queue *queue)
{
    sys_slist_init(&queue->data_q);
    sys_dlist_init(&queue->poll_events);
}

static inline void handle_poll_events(struct k_queue *queue, u32_t state)
{
    _handle_obj_poll_events(&queue->poll_events, state);
}

void k_queue_cancel_wait(struct k_queue *queue)
{
    handle_poll_events(queue, K_POLL_STATE_NOT_READY);
}

static void queue_insert(struct k_queue *queue, void *prev, void *data)
{
    sys_snode_t *node = (sys_snode_t *)data;

    node->next = NULL;
    sys_slist_insert(&queue->data_q, prev, data);
    handle_poll_events(queue, K_POLL_STATE_DATA_AVAILABLE);
}

void k_queue_insert(struct k_queue *queue, void *prev, void *data)
{
    queue_insert(queue, prev, data);
}

void k_queue_append(struct k_queue *queue, void *data)
{
    queue_insert(queue, sys_slist_peek_tail(&queue->data_q), data);
}

void k_queue_prepend(struct k_queue *queue, void *data)
{
    queue_insert(queue, NULL, data);
}

void k_queue_append_list(struct k_queue *queue, void *head, void *tail)
{
    sys_slist_append_list(&queue->data_q, head, tail);
    handle_poll_events(queue, K_POLL_STATE_DATA_AVAILABLE);
}

void *k_queue_get(struct k_queue *queue, s32_t timeout)
{
    return sys_slist_get(&queue->data_q);
}

int k_queue_is_empty(struct k_queue *queue)
{
    return (int)sys_slist_is_empty(&queue->data_q);
}
