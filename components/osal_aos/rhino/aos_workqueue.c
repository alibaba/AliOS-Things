/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <time.h>

#include <k_api.h>
#include <aos/errno.h>
#include <aos/kernel.h>

#include "rhino_p.h"

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC == 0)
#warning "RHINO_CONFIG_KOBJ_DYN_ALLOC is disabled!"
#endif

#if (RHINO_CONFIG_WORKQUEUE > 0)

aos_status_t aos_workqueue_create(aos_workqueue_t *workqueue, const char *name, int32_t prio, void *stack, size_t stack_size)
{
    aos_status_t ret;

    cpu_stack_t *stk = NULL;
    kworkqueue_t *wq;

    if (workqueue == NULL) {
        return -EINVAL;
    }

    if (stack_size < sizeof(cpu_stack_t)) {
        return -EINVAL;
    }

    wq = aos_malloc(sizeof(kworkqueue_t));
    if (wq == NULL) {
        return -ENOMEM;
    }

    if (NULL == stack) {
        stk = aos_malloc(stack_size);
        if (stk == NULL) {
            if (wq != NULL) {
                aos_free(wq);
            }
            return -ENOMEM;
        }
        workqueue->stk = stk;
    } else {
        workqueue->stk = NULL;
        stk = stack;
    }

    ret = krhino_workqueue_create(wq, name, prio, stk,
                                  stack_size / sizeof(cpu_stack_t));
    if (ret != RHINO_SUCCESS) {
        aos_free(wq);
        if (workqueue->stk != NULL) {
            aos_free(workqueue->stk);
        }
        return ret;
    }

    workqueue->hdl = wq;

    return 0;
}

void aos_workqueue_del(aos_workqueue_t *workqueue)
{
    if (!workqueue || !(workqueue->hdl)) {
        return;
    }

    krhino_workqueue_del(workqueue->hdl);

    aos_free(workqueue->hdl);

    if (NULL != workqueue->stk) {
        aos_free(workqueue->stk);
    }
}

aos_status_t aos_work_init(aos_work_t *work, void (*fn)(void *), void *arg, int dly)
{
    aos_status_t  ret;
    kwork_t *w;

    if (work == NULL) {
        return -EINVAL;
    }

    w = aos_malloc(sizeof(kwork_t));
    if (w == NULL) {
        return -ENOMEM;
    }

    ret = krhino_work_init(w, fn, arg, MS2TICK(dly));
    if (ret != RHINO_SUCCESS) {
        aos_free(w);
        return ret;
    }

    *work = w;

    return 0;
}

void aos_work_destroy(aos_work_t *work)
{
    kwork_t *w;

    if (work == NULL) {
        return;
    }

    w = *work;

    if (w->timer != NULL) {
        krhino_timer_stop(w->timer);
        krhino_timer_dyn_del(w->timer);
    }

    aos_free(*work);
    *work = NULL;
}

aos_status_t aos_work_run(aos_workqueue_t *workqueue, aos_work_t *work)
{
    aos_status_t ret;

    if ((workqueue == NULL) || (work == NULL)) {
        return -EINVAL;
    }

    ret = krhino_work_run(workqueue->hdl, *work);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    return ret;
}

aos_status_t aos_work_sched(aos_work_t *work)
{
    aos_status_t ret;

    if (work == NULL) {
        return -EINVAL;
    }

    ret = krhino_work_sched(*work);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    return ret;
}

aos_status_t aos_work_cancel(aos_work_t *work)
{
    aos_status_t ret;

    if (work == NULL) {
        return -EINVAL;
    }

    ret = krhino_work_cancel(*work);
    if (ret != RHINO_SUCCESS) {
        return -EBUSY;
    }

    return 0;
}
#endif



