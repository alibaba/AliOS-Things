/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <zephyr.h>
#include <bluetooth/log.h>
#include "errno.h"

struct timer *g_timer_list;

#if defined(__cplusplus)
extern "C"
{
#endif
static struct k_work_q g_work_queue_main;
#define WORK_QUEUE_LEN 10

static void k_work_submit_to_queue(struct k_work_q *work_q,
                                   struct k_work *work)
{
    if (!atomic_test_and_set_bit(work->flags, K_WORK_STATE_PENDING)) {
        k_fifo_put(&work_q->fifo, work);
    }
}

static void work_queue_thread(void *arg)
{
    struct k_work *work;
    UNUSED(arg);

    while (1) {
        work = k_fifo_get(&g_work_queue_main.fifo, K_FOREVER);

        if (atomic_test_and_clear_bit(work->flags, K_WORK_STATE_PENDING)) {
            work->handler(work);
        }

        k_yield();
    }
}

static void *work_queue_msg[WORK_QUEUE_LEN];
int k_work_q_start(const char *name, uint32_t *stack, uint32_t stack_size, int prio)
{
    k_fifo_init(&g_work_queue_main.fifo, "work queue main", (void **)&work_queue_msg, WORK_QUEUE_LEN);
    return k_thread_spawn(name, NULL, stack_size, work_queue_thread, NULL, prio);
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

static void work_timeout(void *args)
{
    struct k_delayed_work *w = (struct k_delayed_work *)args;

    /* submit work to workqueue */
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
    int key = irq_lock();

    if (atomic_test_bit(work->work.flags, K_WORK_STATE_PENDING)) {
        irq_unlock(key);
        return -EINPROGRESS;
    }

    if (!work->work_q) {
        irq_unlock(key);
        return -EINVAL;
    }

    /* Abort timeout, if it has expired this will do nothing */
    k_timer_stop(&work->timer);

    /* Detach from workqueue */
    work->work_q = NULL;

    irq_unlock(key);

    return 0;
}

#if defined(__cplusplus)
}
#endif
