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

#if (RHINO_CONFIG_TIMER > 0)
aos_status_t aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *), void *arg,
                  uint32_t ms, bool repeat)
{
    kstat_t ret;

    if (timer == NULL) {
        return -EINVAL;
    }
    sys_time_t round = (repeat == false ? 0 : ms);

    ret = krhino_timer_dyn_create((ktimer_t **)(timer), "AOS", (timer_cb_t)fn, MS2TICK(ms), MS2TICK(round), arg, 1);


    return rhino2stderrno(ret);
}
bool aos_timer_is_valid(aos_timer_t *timer)
{
    ktimer_t *k_timer;

    if (timer == NULL) {
        return false;
    }

    k_timer = (ktimer_t *)*timer;

    if (k_timer == NULL) {
        return false;
    }

    if (k_timer->obj_type != RHINO_TIMER_OBJ_TYPE) {
        return false;
    }

    return true;
}
aos_status_t aos_timer_new_ext(aos_timer_t *timer, void (*fn)(void *, void *), void *arg,
                      uint32_t ms, bool repeat, bool auto_run)
{
    kstat_t ret;

    if (timer == NULL) {
        return -EINVAL;
    }

    sys_time_t round = (repeat == 0) ? 0 : ms;

    ret = krhino_timer_dyn_create((ktimer_t **)(timer), "AOS", (timer_cb_t)fn, MS2TICK(ms), MS2TICK(round), arg, (auto_run == true) ? 1 : 0);

    return rhino2stderrno(ret);
}

aos_status_t aos_timer_create(aos_timer_t *timer, void (*fn)(void *, void *), void *arg,
                      uint32_t ms, uint32_t options)
{
    kstat_t ret;
    sys_time_t round;
    if (timer == NULL) {
        return -EINVAL;
    }

    round = (options & AOS_TIMER_REPEAT) ? ms : 0;

    ret = krhino_timer_dyn_create((ktimer_t **)(timer), "AOS", (timer_cb_t)fn, MS2TICK(ms), MS2TICK(round), arg, (options & AOS_TIMER_AUTORUN) ? 1 : 0);

    return rhino2stderrno(ret);
}

void aos_timer_free(aos_timer_t *timer)
{
    if (timer == NULL || *timer == NULL) {
        return;
    }

    krhino_timer_dyn_del((ktimer_t *)*timer);
    *timer = NULL;
}

aos_status_t aos_timer_start(aos_timer_t *timer)
{
    int ret;

    CHECK_HANDLE(timer);

    ret = krhino_timer_start((ktimer_t *)*timer);
    return rhino2stderrno(ret);
}

aos_status_t aos_timer_stop(aos_timer_t *timer)
{
    int ret;

    CHECK_HANDLE(timer);

    ret = krhino_timer_stop((ktimer_t *)*timer);
    return rhino2stderrno(ret);
}

aos_status_t aos_timer_change(aos_timer_t *timer, uint32_t ms)
{
    int ret;

    CHECK_HANDLE(timer);

    ret = krhino_timer_change((ktimer_t *)*timer, MS2TICK(ms), MS2TICK(ms));
    return rhino2stderrno(ret);
}
aos_status_t aos_timer_change_once(aos_timer_t *timer, uint32_t ms)
{
    int ret;

    CHECK_HANDLE(timer);

    ret = krhino_timer_change((ktimer_t *)*timer, MS2TICK(ms), 0);

    return rhino2stderrno(ret);
}

int aos_timer_gettime(aos_timer_t *timer, uint64_t value[4])
{
    ktimer_t *ktimer = NULL;
    uint64_t init_ms;
    uint64_t round_ms;

    CHECK_HANDLE(timer);
    ktimer = (ktimer_t *)*timer;
    init_ms = (uint64_t)krhino_ticks_to_ms(ktimer->init_count);
    round_ms = (uint64_t)krhino_ticks_to_ms(ktimer->round_ticks);

    value[0] = init_ms / 1000;
    value[1] = (init_ms % 1000) * 1000000UL;
    value[2] = round_ms / 1000;
    value[3] = (round_ms % 1000) * 1000000UL ;

    return 0;
}
#endif





