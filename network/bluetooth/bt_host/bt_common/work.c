/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <zephyr.h>
#include <common/log.h>
#include "errno.h"

struct k_work_q g_work_queue;

extern void event_callback(uint8_t event_type);
static void k_work_submit_to_queue(struct k_work_q *work_q, struct k_work *work)
{
    sys_snode_t *node = NULL;
    struct k_work *delayed_work = NULL;
    struct k_work *prev_delayed_work = NULL;
    uint32_t now = k_uptime_get_32();

    if (!atomic_test_and_set_bit(work->flags, K_WORK_STATE_PENDING)) {
        SYS_SLIST_FOR_EACH_NODE(&g_work_queue.queue.data_q, node) {
            delayed_work = (struct k_work *)node;
            if ((work->timeout + work->start_ms) < (delayed_work->start_ms + delayed_work->timeout)) {
                break;
            }
            prev_delayed_work = delayed_work;
        }

        delayed_work = k_queue_first_entry(&g_work_queue.queue);
        sys_slist_insert(&g_work_queue.queue.data_q,
                         (sys_snode_t *)prev_delayed_work, (sys_snode_t *)work);

        if (delayed_work &&
            work->start_ms + work->timeout < delayed_work->start_ms + delayed_work->timeout) {
            event_callback(K_POLL_TYPE_EARLIER_WORK);
        }
    }
}

static void k_work_rm_from_queue(struct k_work_q *work_q, struct k_work *work)
{
    k_queue_remove(&work_q->queue, work);
}

int k_work_q_start(void)
{
    k_queue_init(&g_work_queue.queue);
    return 0;
}

int k_work_init(struct k_work *work, k_work_handler_t handler)
{
    atomic_clear_bit(work->flags, K_WORK_STATE_PENDING);
    work->handler = handler;
    work->start_ms = 0;
    work->timeout = 0;
    return 0;
}

void k_work_submit(struct k_work *work)
{
    k_delayed_work_submit((struct k_delayed_work *)work, 0);
}

void k_delayed_work_init(struct k_delayed_work *work, k_work_handler_t handler)
{
    k_work_init(&work->work, handler);
}

int k_delayed_work_submit(struct k_delayed_work *work, uint32_t delay)
{
    int err = 0;
    int key = irq_lock();

    if (atomic_test_bit(work->work.flags, K_WORK_STATE_PENDING)) {
        k_delayed_work_cancel(work);
    }

    work->work.start_ms = k_uptime_get_32();
    work->work.timeout = delay;
    k_work_submit_to_queue(&g_work_queue, (struct k_work *)work);

done:
    irq_unlock(key);
    return err;
}

int k_delayed_work_cancel(struct k_delayed_work *work)
{
    int key = irq_lock();
    atomic_clear_bit(work->work.flags, K_WORK_STATE_PENDING);
    k_work_rm_from_queue(&g_work_queue, (struct k_work *)work);
    irq_unlock(key);
    return 0;
}

s32_t k_delayed_work_remaining_get(struct k_delayed_work *work)
{
    int32_t remain;

    if (work == NULL) {
        return 0;
    }

    remain = work->work.timeout - (k_uptime_get_32() - work->work.start_ms);
    if (remain < 0) {
        remain = 0;
    }
    return remain;
}
