/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <zephyr.h>
#include <misc/util.h>
#include <misc/dlist.h>
#include "queue.h"

// TODO: rm this tmp code
#include "net/buf.h"
struct cmd_data {
    u8_t type;
    u8_t status;
    u16_t opcode;
    u8_t sync;
};
#define cmd(buf) ((struct cmd_data *)net_buf_user_data(buf))
extern struct k_sem g_poll_sem;

void k_queue_init(struct k_queue *queue)
{
    sys_slist_init(&queue->data_q);
    sys_dlist_init(&queue->poll_events);
}

static inline void handle_poll_events(struct k_queue *queue, u32_t state, uint8_t sync)
{
    _handle_obj_poll_events(&queue->poll_events, state, sync);
}

void k_queue_cancel_wait(struct k_queue *queue)
{
    handle_poll_events(queue, K_POLL_STATE_NOT_READY, 0);
}

static void queue_insert(struct k_queue *queue, void *prev, void *data)
{
    sys_snode_t *node = (sys_snode_t *)data;

    node->next = NULL;
    sys_slist_insert(&queue->data_q, prev, data);
    handle_poll_events(queue, K_POLL_STATE_DATA_AVAILABLE, cmd(data)->sync);
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
    uint8_t sync = cmd(head)->sync;

    if (sync == 0) {
        sync = 2;
    }
    sys_slist_append_list(&queue->data_q, head, tail);
    handle_poll_events(queue, K_POLL_STATE_DATA_AVAILABLE, sync);
    if (sync == 2) {
        k_sem_give(&g_poll_sem);
    }
}

extern void wait_for_event_done(int timeout);
void *k_queue_get(struct k_queue *queue, s32_t timeout)
{
    void *buf = sys_slist_get(&queue->data_q);
    uint32_t start_ms = k_uptime_get_32();

    if (timeout == 0) {
        return buf;
    }

    while (buf == NULL &&
           ((timeout > 0 && (timeout >= (k_uptime_get_32() - start_ms))) || (timeout == K_FOREVER))) {
        wait_for_event_done(timeout);
        buf = sys_slist_get(&queue->data_q);
    }
    return buf;
}

int k_queue_is_empty(struct k_queue *queue)
{
    return (int)sys_slist_is_empty(&queue->data_q);
}
