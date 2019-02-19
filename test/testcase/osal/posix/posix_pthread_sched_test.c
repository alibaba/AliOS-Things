/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "k_api.h"
#include "posix/pthread.h"
#include "ulog/ulog.h"

static pthread_t thread1;
static pthread_t thread2;

static pthread_attr_t attr;

void posix_sched_yield_case(void);
void posix_sched_setscheduler_case(void);
void posix_sched_rr_get_interval_case(void);
void posix_sched_get_priority_max_case(void);
void posix_sched_get_priority_min_case(void);

void posix_sched_test_case(void)
{
    posix_sched_yield_case();
    posix_sched_setscheduler_case();
    posix_sched_rr_get_interval_case();
    posix_sched_get_priority_max_case();
    posix_sched_get_priority_min_case();
}

static void *demo_task1(void *arg)
{
    void *status = NULL;

    LOGI(TAG, "demo_task1 is running !\n");
    LOGI(TAG, "demo_task1 sched_yield !\n");
    sched_yield();
    LOGI(TAG, "demo_task1 is running %d\n");

    pthread_exit(status);

    return NULL;
}

static void *demo_task2(void *arg)
{
    void *status = NULL;

    LOGI(TAG, "demo_task2 is running !\n");
    LOGI(TAG, "demo_task2 sleep 1s !\n");
    krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

    pthread_exit(status);

    return NULL;
}

static void *demo_task3(void *arg)
{
    int   count  = 0;
    void *status = NULL;
    pid_t pid    = NULL;
    int   ret    = -1;

    struct sched_param param;

    param.sched_priority = 33;
    param.slice = 20;

    pid = _pthread_to_pid(thread1);

    ret = sched_setscheduler(pid, SCHED_FIFO, &param);

    if ((ret != 0)
        ||(thread1->sched_policy != KSCHED_FIFO)
        ||(thread1->prio != PRI_CONVERT_PX_RH(param.sched_priority))) {
        LOGI(TAG, "sched_setscheduler error1 !\n");
    }

    ret = sched_setscheduler(pid, SCHED_RR, &param);

    if ((ret != 0)
        ||(thread1->sched_policy != KSCHED_RR)
        ||(thread1->prio != PRI_CONVERT_PX_RH(param.sched_priority))
        ||(thread1->time_slice != (param.slice * 1000) / RHINO_CONFIG_TICKS_PER_SECOND)) {
        LOGI(TAG, "sched_setscheduler error2 !\n");
    }

    pthread_exit(status);

    return NULL;
}

static void *demo_task4(void *arg)
{
    int   count  = 0;
    void *status = NULL;
    pid_t pid    = NULL;
    int   ret    = -1;

    struct sched_param param;
    struct timespec    interval;

    param.sched_priority = 33;
    param.slice = 20;

    pid = _pthread_to_pid(thread1);

    ret = sched_setscheduler(pid, SCHED_RR, &param);

    if (ret == 0) {
        ret = sched_rr_get_interval(pid, &interval);
        if ((ret != 0)||(interval.tv_sec != 0)||(interval.tv_nsec != param.slice * 1000000)) {
            LOGI(TAG, "posix_sched_rr_get_interval_case error1 !\n");
        }
    }

    pthread_exit(status);

    return NULL;
}

void posix_sched_yield_case(void)
{
    int ret1 = -1;
    int ret2 = -1;
    int ret  = -1;

    void *status = NULL;

    LOGI(TAG, "*********** posix_sched_yield_case start ***********\n");

    ret1 = pthread_create(&thread1, NULL, demo_task1, NULL);
    ret2 = pthread_create(&thread2, NULL, demo_task2, NULL);

    if ((ret1 == 0) && (ret2 == 0)) {
        ret = pthread_join(thread1, &status);
        ret |= pthread_join(thread2, &status);
    }

    if (ret == 0) {
        LOGI(TAG, "pthread resourse relased\n");
    }

    LOGI(TAG, "*********** posix_sched_yield_case end ***********\n");
}

void posix_sched_setscheduler_case(void)
{
    int   ret    = -1;
    void *status = NULL;

    LOGI(TAG, "*********** posix_sched_setscheduler_case start ***********\n");

    ret = pthread_create(&thread1, NULL, demo_task3, NULL);

    if (ret == 0) {
        ret = pthread_join(thread1, &status);
    }

    if (ret == 0) {
        LOGI(TAG, "pthread resourse relased\n");
    }

    LOGI(TAG, "*********** posix_sched_setscheduler_case end ***********\n");
}

void posix_sched_rr_get_interval_case(void)
{
    int   ret    = -1;
    void *status = NULL;

    LOGI(TAG, "*********** posix_sched_rr_get_interval_case start ***********\n");

    ret = pthread_create(&thread1, NULL, demo_task3, NULL);

    if (ret == 0) {
        ret = pthread_join(thread1, &status);
    }

    if (ret == 0) {
        LOGI(TAG, "pthread resourse relased\n");
    }

    LOGI(TAG, "*********** posix_sched_rr_get_interval_case end ***********\n");
}

void posix_sched_get_priority_max_case(void)
{
    int ret = -1;

    LOGI(TAG, "*********** posix_sched_get_priority_max_case start ***********\n");

    ret = sched_get_priority_max(0);
    if (ret == POSIX_HIGH_PRI) {
        LOGI(TAG, "sched_get_priority_max test OK !\n");
    }

    LOGI(TAG, "*********** posix_sched_get_priority_max_case end ***********\n");
}

void posix_sched_get_priority_min_case(void)
{
    int ret = -1;

    LOGI(TAG, "*********** posix_sched_get_priority_min_case start ***********\n");

    ret = sched_get_priority_min(0);
    if (ret == POSIX_LOW_PRI) {
        LOGI(TAG, "sched_get_priority_min test OK !\n");
    }

    LOGI(TAG, "*********** posix_sched_get_priority_min_case end ***********\n");
}
