/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <aos/errno.h>
#include <aos/kernel.h>

#define POSIX_TIMER_ID_MIN 1

typedef struct timer_list_s {
    timer_t   id;
    aos_timer_t aos_timer;
    void *evp;  /* The sigevent as the parameter of timer_callback. */
    struct timer_list_s *next;
} timer_list_t;

timer_list_t *g_timer_list_head = NULL; /* The timer list head. */
pthread_mutex_t g_timer_lock = PTHREAD_MUTEX_INITIALIZER;  /* The lock to protect timer list. */

/* Add the unify timer_callback to avoid the argument of callback is an automatic variable,
 * we malloc a sigevent and store the argument it it when timer_create, and free it when
 * timer_delete.
 */
static void timer_callback(void *timer, void *arg)
{
    struct sigevent *evp =(struct sigevent *)arg;

    if ((evp != NULL) && (evp->sigev_notify_function != NULL)) {
            evp->sigev_notify_function(evp->sigev_value);
    }
}

static inline int64_t timespec_to_nanosecond(struct timespec *t)
{
    return ((uint64_t)t->tv_sec * 1000000000UL + t->tv_nsec);
}

static inline struct timespec nanosecond_to_timespec(int64_t ns)
{
    struct timespec ret_time;

    ret_time.tv_sec = ns / 1000000000UL;
    ret_time.tv_nsec = ns % 1000000000UL;

    return ret_time;
}

static int timespec_abs_to_relate(struct timespec *time_abs, struct timespec *time_relate)
{
    int ret = 0;
    int64_t ns = 0;
    struct timespec time_now;

    memset(&time_now, 0, sizeof(time_now));

    ret = clock_gettime(CLOCK_MONOTONIC, &time_now);
    if (ret != 0) {
        return -1;
    }

    ns = timespec_to_nanosecond(time_abs) - timespec_to_nanosecond(&time_now);
    if (ns < 0) {
        return -1;
    }

    *time_relate = nanosecond_to_timespec(ns);

    return 0;
}

int timer_create(clockid_t clockid, struct sigevent * evp, timer_t * timerid)
{
    int ret = 0;
    timer_list_t *timer_list_m = NULL;
    timer_list_t *timer_list = NULL;

    if ((evp == NULL) || (timerid == NULL) ||
        ((clockid != CLOCK_REALTIME) && (clockid != CLOCK_MONOTONIC))) {
        errno = EINVAL;
        return -1;
    }

    /* Only support SIGEV_THREAD. */
    if (evp->sigev_notify != SIGEV_THREAD) {
        errno = ENOTSUP;
        return -1;
    }

    /* The sigev_notify_function should be set for SIGEV_THREAD */
    if (evp->sigev_notify_function == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* malloc new timer struct */
    timer_list_m = (timer_list_t *)malloc(sizeof(timer_list_t));
    if (timer_list_m == NULL) {
        return -1;
    }

    memset(timer_list_m, 0, sizeof(timer_list_t));

    timer_list_m->evp = malloc(sizeof(struct sigevent));
    if(timer_list_m->evp == NULL) {
        free(timer_list_m);
        timer_list_m = NULL;
        return -1;
    }
    memcpy(timer_list_m->evp, evp, sizeof(struct sigevent));

    ret = pthread_mutex_lock(&g_timer_lock);
    if (ret != 0) {
        free(timer_list_m->evp);
        free(timer_list_m);
        return -1;
    }

    /* find the last node add the new timer to the list */
    if (g_timer_list_head == NULL) {

        /* init the id to POSIX_TIMER_ID_MIN */
        timer_list_m->id = (timer_t) POSIX_TIMER_ID_MIN;
        g_timer_list_head = timer_list_m;
    } else {
        timer_list = g_timer_list_head;
        while(timer_list->next != NULL) {
            timer_list = timer_list->next;
        }

        /* the id of new timer equel to last id plus one */
        timer_list_m->id = (int)timer_list->id + 1;
        timer_list->next = timer_list_m;
    }

    pthread_mutex_unlock(&g_timer_lock);

    /* create a timer */
    ret = aos_timer_new_ext(&(timer_list_m->aos_timer), timer_callback, timer_list_m->evp,
                            1, 1, 0);
    if (ret != 0) {
        return -1;
    }

    /* update the timerid */
    *timerid = timer_list_m->id;

    return 0;
}

int timer_delete(timer_t timerid)
{
    timer_list_t *timer_list = NULL;
    timer_list_t *timer_list_l = NULL;
    int ret = 0;

    /* Timer list is empty. */
    if (g_timer_list_head == NULL) {
        return -1;
    }

    ret = pthread_mutex_lock(&g_timer_lock);
    if (ret != 0) {
        return -1;
    }

    /* Scan the list to find the timer according to timerid. */
    timer_list = g_timer_list_head;
    timer_list_l = g_timer_list_head;
    while ((timer_list != NULL) && (timer_list->id != timerid)) {
        timer_list_l = timer_list;
        timer_list = timer_list->next;
    }

    /* Do not find the timerid. */
    if (timer_list == NULL) {
        pthread_mutex_unlock(&g_timer_lock);
        return -1;
    }

    /* Stop and detete the timer. */
    aos_timer_stop(&(timer_list->aos_timer));

    aos_timer_free(&(timer_list->aos_timer));
    /* Delete the timer from the list and free the timer. */
    if (timer_list_l == g_timer_list_head) {
        g_timer_list_head = timer_list->next;
    } else {
        timer_list_l->next = timer_list->next;
    }

    if (timer_list->evp != NULL) {
        free(timer_list->evp);
    }
    free(timer_list);

    pthread_mutex_unlock(&g_timer_lock);

    return ret;
}

int timer_settime(timer_t timerid, int flags, const struct itimerspec *restrict value,
                  struct itimerspec *restrict ovalue)
{
    int     ret            = 0;
    int64_t value_ns       = 0;
    int64_t interval_ns    = 0;
    struct timespec value_spec = {0};
    struct timespec interval_spec = {0};
    timer_list_t   *timer_list = NULL;

    if (value == NULL) {
        return -1;
    }

    if (g_timer_list_head == NULL) {
        return -1;
    }

    /* If the time is absolute time transform it to relative time. */
    if((flags & TIMER_ABSTIME) == TIMER_ABSTIME) {
        ret = timespec_abs_to_relate((struct timespec *)&value->it_value, &value_spec);
        ret |= timespec_abs_to_relate((struct timespec *)&value->it_interval, &interval_spec);
        if (ret != 0) {
            return -1;
        }

        value_ns = timespec_to_nanosecond(&value_spec);
        interval_ns = timespec_to_nanosecond(&interval_spec);
    } else {
        value_ns = timespec_to_nanosecond((struct timespec *)&value->it_value);
        interval_ns = timespec_to_nanosecond((struct timespec *)&value->it_interval);
    }

    /* Get the old parameters of timer if ovalue is not NULL. */
    if (ovalue != NULL) {
        ret = timer_gettime(timerid, ovalue);
        if (ret != 0) {
            return -1;
        }
    }

    ret = pthread_mutex_lock(&g_timer_lock);
    if (ret != 0) {
        return -1;
    }

    /* Scan the list to find the timer according to timerid. */
    timer_list = g_timer_list_head;
    while ((timer_list != NULL) && (timer_list->id != timerid)) {
        timer_list = timer_list->next;
    }

    /* Do not find the timer. */
    if (timer_list == NULL) {
        pthread_mutex_unlock(&g_timer_lock);
        return -1;
    }

    aos_timer_stop(&(timer_list->aos_timer));
    /* TODO: support interval and first round timer different. */
    ret = aos_timer_change(&(timer_list->aos_timer), value_ns / 1000000);
    aos_timer_start(&(timer_list->aos_timer));

    pthread_mutex_unlock(&g_timer_lock);
    return ret;
}

int timer_gettime(timer_t timerid, struct itimerspec *value)
{
    int ret = 0;
    uint64_t time_ns[4] = {0};
    timer_list_t *timer_list = NULL;

    if (value == NULL) {
        return -1;
    }

    ret = pthread_mutex_lock(&g_timer_lock);
    if (ret != 0) {
        return -1;
    }

    /* Scan the list to find the timer according to timerid */
    timer_list = g_timer_list_head;
    while(timer_list != NULL) {
        if (timer_list->id == timerid) {
            break;
        }
        timer_list = timer_list->next;
    }

    if (timer_list == NULL) {
        /* The timerid is not found. */
        pthread_mutex_unlock(&g_timer_lock);
        return -1;
    }

    ret = aos_timer_gettime(&(timer_list->aos_timer), time_ns);
    if (ret != 0) {
        pthread_mutex_unlock(&g_timer_lock);
        return -1;
    }
    value->it_interval.tv_sec = time_ns[0];
    value->it_interval.tv_nsec = time_ns[1];
    value->it_value.tv_sec = time_ns[2];
    value->it_value.tv_nsec = time_ns[3];

    pthread_mutex_unlock(&g_timer_lock);
    return 0;
}

int timer_getoverrun(timer_t timerid)
{
    errno = ENOSYS;
    return -1;
}
