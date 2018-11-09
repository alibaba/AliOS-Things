/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <posix_timer.h>
#include <posix_signal.h>

timer_list_t *timer_list_head;

static int64_t         timespec_to_nanosecond(struct timespec *value);
static int             timespec_abs_to_relate(struct timespec *time_abs, struct timespec *time_relate);
static struct timespec nanosecond_to_timespec(int64_t value);

int timer_create(clockid_t clockid, struct sigevent *restrict evp, timer_t *restrict timerid)
{
    int ret = -1;
    timer_list_t *timer_list_m;
    timer_list_t *timer_list;

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

    CPSR_ALLOC();
    RHINO_CRITICAL_ENTER();

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

    RHINO_CRITICAL_EXIT();

    /* create a timer */
    ret = krhino_timer_dyn_create(&timer_list_m->ktimer, "posix_timer", evp->sigev_notify_function,
                                  1, 1, NULL, 0);
    if (ret != 0) {
        return -1;
    }

    /* update the timerid */
    *timerid = timer_list_m->id;

    printf("timer_list_head->id %d\n", timer_list_head->id);

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
    printf("timer_list->id %d\n", timer_list->id);

    if (ret == 0) {
        CPSR_ALLOC();
        RHINO_CRITICAL_ENTER();
        /* delete the timer in the list and free the timer */
        if (timer_list_l == timer_list_head) {
            timer_list_head = timer_list->next;
        } else {
            timer_list_l->next = timer_list->next;
        }

        RHINO_CRITICAL_EXIT();

        krhino_mm_free(timer_list);
    }

    return ret;
}

int timer_settime(timer_t timerid, int flags, const struct itimerspec *restrict value,
	              struct itimerspec *restrict ovalue)
{
    int ret = -1;
    int64_t value_ns = 0;
    int64_t interval_ns = 0;
    int64_t value_ticks = 0;
    int64_t interval_ticks = 0;
    struct timespec value_spec;
    struct timespec interval_spec;
    timer_list_t *timer_list;

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
    uint64_t time_ns = 0;
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
    /* not support */
    return 0;
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
    *tp = nanosecond_to_timespec(time_ns);

    return 0;
}

int clock_settime(clockid_t clock_id, const struct timespec *tp)
{
    /* not support */
	return -1;
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
    struct timespec value_spec;
    int64_t value_ns = 0;
    int64_t value_ticks = 0;
    int ret = -1;

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
    struct timespec time;
    int64_t value = 0;
    int ret = -1;

    memset(&time,0,sizeof(time));

    ret = clock_gettime(CLOCK_REALTIME, &time);
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
