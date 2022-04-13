//
// jquick_condition.c
//
// Created by liujuncheng@alibaba-inc.com on 2020/11/11.
//
// Copyright (c) 2020 Alibaba All rights reserved.
//

#include "jquick_condition.h"

#include <pthread.h>
#include <stdlib.h>
#ifdef __STANDARD_TIMER__
#include <time.h>
#else
#include <posix/timer.h>
#endif

//#include "utils/log.h"
#include "HaasLog.h"

JQuick_Condition jquick_condition_create()
{
    JQuick_Condition cond = (JQuick_Mutex)malloc(sizeof(pthread_cond_t));
    if (!cond) {
        LOG_E("JQuick_Condition: out of memory\n");

        return NULL;
    }

    int v = pthread_cond_init((pthread_cond_t*)cond, NULL);
    if (0 == v) {
        return cond;
    }

    LOG_E("JQuick_Condition: Init JQuick_Condition failed\n");

    free(cond);

    return NULL;
}

int jquick_condition_wait(JQuick_Condition condition, JQuick_Mutex mutex)
{
    if (!condition || !mutex) {
        LOG_E("JQuick_Condition: Condition or Mutex is NULL\n");

        return -1;
    }

    return pthread_cond_wait((pthread_cond_t*)condition, (pthread_mutex_t*)mutex);
}

int jquick_condition_wait_with_timeout(JQuick_Condition condition,
                                       JQuick_Mutex mutex,
                                       int millisecond)
{
    if (!condition || !mutex) {
        LOG_E("JQuick_Condition: Condition or Mutex is NULL\n");

        return -1;
    }

    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    tv.tv_sec += millisecond / 1000;
    tv.tv_nsec += (millisecond % 1000) * 1000 * 1000;
    if (tv.tv_nsec >= 1000000000) {
        tv.tv_sec++;
        tv.tv_nsec -= 1000000000;
    }

    return pthread_cond_timedwait((pthread_cond_t*)condition,
                                  (pthread_mutex_t*)mutex,
                                  &tv);
}

int jquick_condition_signal(JQuick_Condition condition)
{
    if (!condition) {
        LOG_E("JQuick_Condition: Condition is NULL\n");

        return -1;
    }

    return pthread_cond_signal((pthread_cond_t*)condition);
}

int jquick_condition_broadcast(JQuick_Condition condition)
{
    if (!condition) {
        LOG_E("JQuick_Condition: Condition is NULL\n");

        return -1;
    }

    return pthread_cond_broadcast((pthread_cond_t*)condition);
}

int jquick_condition_destroy(JQuick_Condition condition)
{
    if (!condition) {
        LOG_E("JQuick_Condition: Condition is NULL\n");

        return -1;
    }

    int v = pthread_cond_destroy((pthread_cond_t*)condition);
    if (v == 0) {
        free(condition);
    }

    return v;
}
