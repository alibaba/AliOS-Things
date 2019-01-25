/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <zephyr.h>
#include <common/log.h>
#include "errno.h"

struct k_work_q g_work_queue;

static void k_work_submit_to_queue(struct k_work_q *work_q, struct k_work *work)
{
    k_queue_append(&work_q->queue, work);
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
    k_work_submit_to_queue(&g_work_queue, work);
}

void k_delayed_work_init(struct k_delayed_work *work, k_work_handler_t handler)
{
    k_work_init(&work->work, handler);
}

int k_delayed_work_submit(struct k_delayed_work *work, uint32_t delay)
{
    int err = 0;

    if (atomic_test_and_set_bit(work->work.flags, K_WORK_STATE_PENDING)) {
        err = -EADDRINUSE;
        goto done;
    }

    work->work.start_ms = k_uptime_get_32();
    work->work.timeout = delay;
    k_work_submit_to_queue(&g_work_queue, (struct k_work *)work);

done:
    return err;
}

int k_delayed_work_cancel(struct k_delayed_work *work)
{
    int err = 0;

    if (atomic_test_bit(work->work.flags, K_WORK_STATE_PENDING)) {
        err = -EINPROGRESS;
        goto exit;
    }
    k_work_rm_from_queue(&g_work_queue, (struct k_work *)work);

exit:
    return err;
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
