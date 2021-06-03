/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "aos_hal_timer.h"
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>

static void timer_common_cb(union sigval arg)
{
    timer_dev_t *amp_timer = (timer_dev_t *)arg.sival_ptr;

    if (amp_timer && amp_timer->config.cb) {
        amp_timer->config.cb(amp_timer->config.arg);
    }
}

int32_t aos_hal_timer_init(timer_dev_t *tim)
{
    struct sigevent ent;
    timer_t *timer = aos_malloc(sizeof(timer_t));

    memset(&ent, 0x00, sizeof(struct sigevent));

    /* create a timer */
    ent.sigev_notify          = SIGEV_THREAD;
    ent.sigev_notify_function = (void (*)(union sigval))timer_common_cb;
    ent.sigev_value.sival_ptr = tim;

    if (timer_create(CLOCK_MONOTONIC, &ent, timer) != 0) {
        aos_free(timer);
        return -1;
    }

    tim->priv = timer;
    return 0;
}

int32_t aos_hal_timer_start(timer_dev_t *tim)
{
    struct itimerspec ts;

    /* check parameter */
    if (tim == NULL) {
        return -1;
    }

    if (tim->config.reload_mode == TIMER_RELOAD_AUTO) {
        /* it_value=0: stop timer */
        ts.it_interval.tv_sec = tim->config.period / 1000000;
        ts.it_interval.tv_nsec = (tim->config.period % 1000000) * 1000;
    } else {
        /* it_interval=0: timer run only once */
        ts.it_interval.tv_sec = 0;;
        ts.it_interval.tv_nsec = 0;
    }

    /* it_value=0: stop timer */
    ts.it_value.tv_sec = tim->config.period / 1000000;
    ts.it_value.tv_nsec = (tim->config.period % 1000000) * 1000;

    return timer_settime(*(timer_t *)tim->priv, 0, &ts, NULL);
}

void aos_hal_timer_stop(timer_dev_t *tim)
{
    struct itimerspec ts;

    /* check parameter */
    if (tim == NULL) {
        return;
    }

    /* it_interval=0: timer run only once */
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    /* it_value=0: stop timer */
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;

    timer_settime(*(timer_t *)tim->priv, 0, &ts, NULL);

    return ;
}

int32_t aos_hal_timer_finalize(timer_dev_t *tim)
{
    int ret = 0;

    /* check parameter */
    if (tim == NULL) {
        return -1;
    }

    ret = timer_delete(*(timer_t *)tim->priv);

    aos_free(tim->priv);
    return 0;
}
