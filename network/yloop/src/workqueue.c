/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "aos/yloop.h"

#include "k_api.h"

#include "aos/errno.h"

#if (RHINO_CONFIG_WORKQUEUE>0)

#define MS2TICK(ms) krhino_ms_to_ticks(ms)

typedef struct work_para {
    aos_work_t *work;
    aos_loop_t loop;
    aos_call_t action;
    void *arg1;
    aos_call_t fini_cb;
    void *arg2;
} work_par_t;

static void work_destroy(aos_work_t *work)
{
    kwork_t *w;

    if (work == NULL) {
        return;
    }

    w = work->hdl;

    if (w->timer != NULL) {
        krhino_timer_stop(w->timer);
        krhino_timer_dyn_del(w->timer);
    }

    aos_free(work->hdl);
    work->hdl = NULL;
}

static void free_wpar(work_par_t *wpar)
{
    work_destroy(wpar->work);
    aos_free(wpar->work);
    aos_free(wpar);
}

static void run_my_work(void *arg)
{
    work_par_t *wpar = arg;

    wpar->action(wpar->arg1);

    if (wpar->fini_cb) {
        aos_loop_schedule_call(wpar->loop, wpar->fini_cb, wpar->arg2);
    }

    free_wpar(wpar);
}

static int work_cancel(aos_work_t *work)
{
    int ret;

    if (work == NULL) {
        return -EINVAL;
    }

    ret = krhino_work_cancel(work->hdl);
    if (ret != RHINO_SUCCESS) {
        return -EBUSY;
    }

    return 0;
}
void aos_cancel_work(void *w, aos_call_t action, void *arg1)
{
    work_par_t *wpar = w;

    if (wpar == NULL) {
        return;
    }

    if (wpar->action != action) {
        return;
    }

    if (wpar->arg1 != arg1) {
        return;
    }

    int ret = work_cancel(wpar->work);
    if (ret != 0) {
        return;
    }

    free_wpar(wpar);
}
static int work_init(aos_work_t *work, void (*fn)(void *), void *arg, int dly)
{
    kstat_t  ret;
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

    work->hdl = w;

    return 0;
}

static int work_sched(aos_work_t *work)
{
    int ret;

    if (work == NULL) {
        return -EINVAL;
    }

    ret = krhino_work_sched(work->hdl);
    if (ret == RHINO_SUCCESS) {
        return 0;
    }

    return ret;
}

void *aos_loop_schedule_work(int ms, aos_call_t action, void *arg1,
                             aos_call_t fini_cb, void *arg2)
{
    int ret;

    if (action == NULL) {
        return NULL;
    }

    aos_work_t *work = aos_malloc(sizeof(*work));
    work_par_t *wpar = aos_malloc(sizeof(*wpar));

    if (!work || !wpar) {
        goto err_out;
    }

    wpar->work = work;
    wpar->loop = aos_current_loop();
    wpar->action = action;
    wpar->arg1 = arg1;
    wpar->fini_cb = fini_cb;
    wpar->arg2 = arg2;

    ret = work_init(work, run_my_work, wpar, ms);
    if (ret != 0) {
        goto err_out;
    }
    ret = work_sched(work);
    if (ret != 0) {
        goto err_out;
    }

    return wpar;
err_out:
    aos_free(work);
    aos_free(wpar);
    return NULL;
}

#endif
