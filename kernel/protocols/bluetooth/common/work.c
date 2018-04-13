/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <zephyr.h>
#include <common/log.h>
#include "errno.h"

static struct k_thread work_q_thread;
static BT_STACK_NOINIT(work_q_stack, CONFIG_BT_WORK_QUEUE_STACK_SIZE);
static struct k_work_q g_work_queue_main;

static void k_work_submit_to_queue(struct k_work_q *work_q,
                                   struct k_work *work)
{
    if (!atomic_test_and_set_bit(work->flags, K_WORK_STATE_PENDING)) {
        k_fifo_put(&work_q->fifo, work);
    }
}

static void work_queue_thread(void *p1, void *p2, void *p3)
{
    struct k_work *work;
    UNUSED(p1);

    while (1) {
        work = k_fifo_get(&g_work_queue_main.fifo, K_FOREVER);

        if (atomic_test_and_clear_bit(work->flags, K_WORK_STATE_PENDING)) {
            work->handler(work);
        }

        k_yield();
    }
}

int k_work_q_start(void)
{
    k_fifo_init(&g_work_queue_main.fifo);
    return k_thread_create(&work_q_thread, work_q_stack,
                           K_THREAD_STACK_SIZEOF(work_q_stack),
                           work_queue_thread, NULL, NULL, NULL, CONFIG_BT_WORK_QUEUE_PRIO, 0, K_NO_WAIT);
}

int k_work_init(struct k_work *work, k_work_handler_t handler)
{
    ASSERT(work, "work is NULL");

    atomic_clear_bit(work->flags, K_WORK_STATE_PENDING);
    work->handler = handler;
    return 0;
}

void k_work_submit(struct k_work *work)
{
    k_work_submit_to_queue(&g_work_queue_main, work);
}

static void work_timeout(void *timer, void *args)
{
    struct k_delayed_work *w = (struct k_delayed_work *)args;

    /* submit work to workqueue */
    k_timer_stop(&w->timer);
    k_work_submit_to_queue(w->work_q, &w->work);
    /* detach from workqueue, for cancel to return appropriate status */
    w->work_q = NULL;
}

void k_delayed_work_init(struct k_delayed_work *work, k_work_handler_t handler)
{
    ASSERT(work, "delay work is NULL");
    k_work_init(&work->work, handler);
    k_timer_init(&work->timer, work_timeout, work);
    work->work_q = NULL;
}

static int k_delayed_work_submit_to_queue(struct k_work_q *work_q,
        struct k_delayed_work *work,
        uint32_t delay)
{
    int key = irq_lock();
    int err;

    /* Work cannot be active in multiple queues */
    if (work->work_q && work->work_q != work_q) {
        err = -EADDRINUSE;
        goto done;
    }

    /* Cancel if work has been submitted */
    if (work->work_q == work_q) {
        err = k_delayed_work_cancel(work);

        if (err < 0) {
            goto done;
        }
    }

    /* Attach workqueue so the timeout callback can submit it */
    work->work_q = work_q;
    if (!delay) {
        /* Submit work if no ticks is 0 */
        k_work_submit_to_queue(work_q, &work->work);
        work->work_q = NULL;
    } else {
        /* Add timeout */
        k_timer_start(&work->timer, delay);
    }

    err = 0;

done:
    irq_unlock(key);
    return err;
}

int k_delayed_work_submit(struct k_delayed_work *work, uint32_t delay)
{
    return k_delayed_work_submit_to_queue(&g_work_queue_main, work, delay);
}

int k_delayed_work_cancel(struct k_delayed_work *work)
{
    int err = 0;
    int key = irq_lock();

    if (atomic_test_bit(work->work.flags, K_WORK_STATE_PENDING)) {
        err = -EINPROGRESS;
        goto exit;
    }

    if (!work->work_q) {
        err = -EINVAL;
        goto exit;
    }

    k_timer_stop(&work->timer);
    work->work_q = NULL;

exit:
    irq_unlock(key);
    return err;
}

s32_t k_delayed_work_remaining_get(struct k_delayed_work *work)
{
    int32_t remain;
    k_timer_t *timer;

    if (work == NULL) {
        return 0;
    }

    timer = &work->timer;
    remain = timer->timeout - (aos_now_ms() - timer->start_ms);
    if (remain < 0) {
        remain = 0;
    }
    return remain;
}
