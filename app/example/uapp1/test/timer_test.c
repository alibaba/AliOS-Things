/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>

#define LOOP_ROUND 20

static ktimer_t  timer;
static ksem_t   *timer_sem;

static int timer_cb_cnt = 0;

static void timer_cb(void *timer, void *arg)
{
    timer_cb_cnt++;

    if (timer_cb_cnt == LOOP_ROUND)
        krhino_sem_give(timer_sem);
}

int timer_test(void)
{
    kstat_t stat;

    stat = krhino_sem_dyn_create(&timer_sem, "timer_mutex", 0);
    if (stat != RHINO_SUCCESS) {
        printf("%s: create sem failed \r\n", __func__);
        return -2;
    }

    stat = krhino_timer_create(&timer, "app_timer", timer_cb,
                               1, 10, NULL, 1);
    if (stat != RHINO_SUCCESS) {
        printf("%s: create timer failed\r\n", __func__);
        goto out;
    }

    stat = krhino_sem_take(timer_sem, RHINO_WAIT_FOREVER);
    if (stat == RHINO_SUCCESS) {
        krhino_timer_del(&timer);
    }

out:
    krhino_sem_dyn_del(timer_sem);

    return (timer_cb_cnt == LOOP_ROUND)? 0: -1;
}

