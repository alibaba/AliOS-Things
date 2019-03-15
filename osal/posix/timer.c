/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "posix/timer.h"
#include "posix/signal.h"

#if (POSIX_CONFIG_TIMER_ENABLE > 0)

int64_t g_realtime_clock_base = 0;

timer_list_t *timer_list_head;

static int64_t timespec_to_nanosecond(struct timespec *value);
static int     timespec_abs_to_relate(struct timespec *time_abs, struct timespec *time_relate);

static struct timespec nanosecond_to_timespec(int64_t value);

kmutex_t g_timer_mutex;

int timer_lock_init(void)
{
    return krhino_mutex_create(&g_timer_mutex, "g_timer_mutex");
}

int timer_create(clockid_t clockid, struct sigevent *restrict evp, timer_t *restrict timerid)
{
    timer_list_t *timer_list_m;
    timer_list_t *timer_list;
    int ret = -1;

    if ((evp == NULL) || (timerid == NULL)) {
        return -1;
    }

    /* timer function must be none_null */
    if (evp->sigev_notify_function == NULL) {
        return -1;
    }

    /* malloc new timer struct */
    timer_list_m = (timer_list_t *)krhino_mm_alloc(sizeof(timer_list_t));
    if (timer_list_m == NULL) {
        return -1;
    }

    memset(timer_list_m, 0, sizeof(timer_list_t));

    ret = krhino_mutex_lock(&g_timer_mutex, RHINO_WAIT_FOREVER);
    if (ret != 0) {
        return -1;
    }

    /* find the last node add the new timer to the list */
    if (timer_list_head == NULL) {

        /* init the id to POSIX_TIMER_ID_MIN */
        timer_list_m->id = POSIX_TIMER_ID_MIN;
        timer_list_head = timer_list_m;
    } else {
        timer_list = timer_list_head;
        while(timer_list->next != NULL) {
            timer_list = timer_list->next;
        }

        /* the id of new timer equel to last id plus one */
        timer_list_m->id = timer_list->id + 1;
        timer_list->next = timer_list_m;
    }

    krhino_mutex_unlock(&g_timer_mutex);

    /* create a timer */
    ret = krhino_timer_dyn_create(&timer_list_m->ktimer, "posix_timer", evp->sigev_notify_function,
                                  1, 1, NULL, 0);
    if (ret != 0) {
        return -1;
    }

    /* update the timerid */
    *timerid = timer_list_m->id;

    return 0;
}

int timer_delete(timer_t timerid)
{
    timer_list_t *timer_list;
    timer_list_t *timer_list_l;
    int ret = -1;

    /* if no timer in the list return error */
    if (timer_list_head == NULL) {
        return -1;
    }

    ret = krhino_mutex_lock(&g_timer_mutex, RHINO_WAIT_FOREVER);
    if (ret != 0) {
        return -1;
    }

    /* scan the list to find the timer according to timerid */
    timer_list = timer_list_head;
    timer_list_l = timer_list_head;
    while(timer_list->id != timerid) {
        timer_list_l = timer_list;
        timer_list = timer_list->next;
    }

    /* stop and detete the timer */
    krhino_timer_stop(timer_list->ktimer);

    ret = krhino_timer_dyn_del(timer_list->ktimer);
    if (ret == 0) {

        /* delete the timer in the list and free the timer */
        if (timer_list_l == timer_list_head) {
            timer_list_head = timer_list->next;
        } else {
            timer_list_l->next = timer_list->next;
        }

        krhino_mm_free(timer_list);
    }

    krhino_mutex_unlock(&g_timer_mutex);

    return ret;
}

int timer_settime(timer_t timerid, int flags, const struct itimerspec *restrict value,
	              struct itimerspec *restrict ovalue)
{
    int     ret            = -1;
    int64_t value_ns       = 0;
    int64_t interval_ns    = 0;
    int64_t value_ticks    = 0;
    int64_t interval_ticks = 0;

    struct timespec value_spec;
    struct timespec interval_spec;
    timer_list_t   *timer_list;

    if (value == NULL) {
        return -1;
    }

    if (timer_list_head == NULL) {
        return -1;
    }

    /* if the time is absolute time transform it to relative time */
    if((flags & TIMER_ABSTIME) == TIMER_ABSTIME) {
        ret = timespec_abs_to_relate(&value->it_value, &value_spec);
        ret |= timespec_abs_to_relate(&value->it_interval, &interval_spec);
        if (ret != 0) {
            return -1;
        }

        value_ns = timespec_to_nanosecond(&value_spec);
        interval_ns = timespec_to_nanosecond(&interval_spec);
    } else {
        value_ns = timespec_to_nanosecond(&value->it_value);
        interval_ns = timespec_to_nanosecond(&value->it_interval);
    }

    /* get the old parameters of timer if ovalue is not NULL */
    if (ovalue != NULL) {
        ret = timer_gettime(timerid, ovalue);
        if (ret != 0) {
            return -1;
        }
    }

    /* scan the list to find the timer according to timerid */
    timer_list = timer_list_head;
    while(timer_list->id != timerid) {
        timer_list = timer_list->next;
    }

    /* first stop the timer and then change the parameters and finally turn on the timer */
    krhino_timer_stop(timer_list->ktimer);
    value_ticks = (value_ns * RHINO_CONFIG_TICKS_PER_SECOND) / NANOSECONDS_PER_SECOND;
    interval_ticks = (interval_ns * RHINO_CONFIG_TICKS_PER_SECOND) / NANOSECONDS_PER_SECOND;
    ret = krhino_timer_change(timer_list->ktimer, value_ticks, interval_ticks);
    krhino_timer_start(timer_list->ktimer);

    return ret;
}

int timer_gettime(timer_t timerid, struct itimerspec *value)
{
    uint64_t      time_ns = 0;
    timer_list_t *timer_list;

    if (value == NULL) {
        return -1;
    }

    /* scan the list to find the timer according to timerid */
    timer_list = timer_list_head;
    while(timer_list->id != timerid) {
        timer_list = timer_list->next;
    }

    /* get the parameters of the timer */
    time_ns = (timer_list->ktimer->init_count * NANOSECONDS_PER_SECOND) / RHINO_CONFIG_TICKS_PER_SECOND;
    value->it_value = nanosecond_to_timespec(time_ns);
    time_ns = (timer_list->ktimer->round_ticks * NANOSECONDS_PER_SECOND) / RHINO_CONFIG_TICKS_PER_SECOND;
    value->it_interval = nanosecond_to_timespec(time_ns);

    return 0;
}

int timer_getoverrun(timer_t timerid)
{
    /* kernel not support */
    return -1;
}

int clock_getres(clockid_t clock_id, struct timespec *res)
{
    if (res == NULL) {
        return -1;
    }

    res->tv_sec = 0;
    res->tv_nsec = NANOSECONDS_PER_SECOND / RHINO_CONFIG_TICKS_PER_SECOND;

    return 0;
}

int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
    uint64_t time_ns = 0;

    if (tp == NULL) {
        return -1;
    }

    time_ns = (krhino_sys_tick_get() * NANOSECONDS_PER_SECOND) / RHINO_CONFIG_TICKS_PER_SECOND;

    if (clock_id == CLOCK_MONOTONIC) {
        *tp = nanosecond_to_timespec(time_ns);
    } else if (clock_id == CLOCK_REALTIME) {
        *tp = nanosecond_to_timespec(g_realtime_clock_base + time_ns);
    } else {
        return -1;
    }

    return 0;
}

int clock_settime(clockid_t clock_id, const struct timespec *tp)
{
    int     ret      = -1;
    int64_t value_ns = 0;

    struct timespec timer_spec_cur;

    /* only CLOCK_REALTIME can be set */
    if (clock_id != CLOCK_REALTIME) {
        return -1;
    }

    ret = clock_gettime(CLOCK_MONOTONIC, &timer_spec_cur);
    if (ret != 0) {
        return -1;
    }

    value_ns = timespec_to_nanosecond(tp) - timespec_to_nanosecond(&timer_spec_cur);

    ret = krhino_mutex_lock(&g_timer_mutex, RHINO_WAIT_FOREVER);
    if (ret != 0) {
        return -1;
    }

    g_realtime_clock_base = value_ns;

    krhino_mutex_unlock(&g_timer_mutex);

    return 0;
}

int nanosleep(const struct timespec *rqtp, struct timespec *rmtp)
{
    int64_t time_ns = 0;

    if (rqtp == NULL) {
        return -1;
    }

    time_ns = timespec_to_nanosecond(rqtp);
    krhino_task_sleep((time_ns * RHINO_CONFIG_TICKS_PER_SECOND) / NANOSECONDS_PER_SECOND);

    return 0;
}

int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *rqtp, struct timespec *rmtp)
{
    int     ret         = -1;
    int64_t value_ns    = 0;
    int64_t value_ticks = 0;

    struct timespec value_spec;

    /* if the time is absolute time transform it to relative time */
    if((flags & TIMER_ABSTIME) == TIMER_ABSTIME) {
        ret = timespec_abs_to_relate(rqtp, &value_spec);
        if (ret != 0) {
            return -1;
        }

        value_ns = timespec_to_nanosecond(&value_spec);
    } else {
        value_ns = timespec_to_nanosecond(rqtp);
    }

    value_ticks = (value_ns * RHINO_CONFIG_TICKS_PER_SECOND) / NANOSECONDS_PER_SECOND;
    krhino_task_sleep(value_ticks);

    return 0;
}

unsigned int sleep(unsigned int seconds)
{
    krhino_task_sleep(seconds * RHINO_CONFIG_TICKS_PER_SECOND);

    return 0;
}

int64_t timespec_to_nanosecond(struct timespec *value)
{
    return (value->tv_sec * NANOSECONDS_PER_SECOND + value->tv_nsec);
}

struct timespec nanosecond_to_timespec(int64_t value)
{
    struct timespec time;

    time.tv_sec = value / NANOSECONDS_PER_SECOND;
    time.tv_nsec = value % NANOSECONDS_PER_SECOND;

    return time;
}

int timespec_abs_to_relate(struct timespec *time_abs, struct timespec *time_relate)
{
    int64_t value = 0;
    int     ret   = -1;

    struct timespec time;

    memset(&time,0,sizeof(time));

    ret = clock_gettime(CLOCK_MONOTONIC, &time);
    if (ret != 0) {
        return -1;
    }

    value = timespec_to_nanosecond(time_abs) - timespec_to_nanosecond(&time);
    if (value < 0) {
        return -1;
    }

    *time_relate = nanosecond_to_timespec(value);

    return 0;
}

#endif
