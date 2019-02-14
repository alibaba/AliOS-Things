/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "k_api.h"
#include "posix/pthread.h"
#include "ulog/ulog.h"

void posix_pthread_create_case(void);
void posix_pthread_join_case(void);
void posix_pthread_cleanup_case1(void);
void posix_pthread_cleanup_case2(void);
void posix_pthread_detach_case1(void);
void posix_pthread_detach_case2(void);
void posix_pthread_once_case(void);
void posix_pthread_setschedparam_case1(void);
void posix_pthread_setschedparam_case2(void);
void posix_pthread_setschedprio_case(void);

pthread_t thread1;
pthread_t thread2;

pthread_attr_t attr;

static void *demo_task(void *arg)
{
    int   count  = 0;
    void *status = NULL;

    while (1) {
        count++;
        LOGI(TAG, "demo_task is running %d\n", count);

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 10) {
            pthread_exit(status);
        }
    };

    return NULL;
}

static void *demo_task1(void *arg)
{
    int   count  = 0;
    void *status = NULL;

    while (1) {
        count++;
        LOGI(TAG, "demo_task1 is running %d\n", count);

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 2) {
            pthread_join(thread2, &status);
        }

        if (count == 5) {
            pthread_exit(status);
        }
    };

    return NULL;
}

static void *demo_task2(void *arg)
{
    int   count  = 0;
    void *status = NULL;

    while (1) {
        count++;
        LOGI(TAG, "demo_task2 is running %d\n", count);

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 10) {
            pthread_exit(status);
        }
    };

    return NULL;
}

static void output_cleanup_msg(void *msg)
{
    LOGI(TAG, "%s is running!\n", (char *)msg);
}

static void *demo_task3(void *arg)
{
    void *status = NULL;

    pthread_cleanup_push(output_cleanup_msg, "first cleanup function");
    pthread_cleanup_push(output_cleanup_msg, "second cleanup function");

    LOGI(TAG, "demo_task3 is running!\n");

    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);

    pthread_exit(status);

    return NULL;
}

static void *demo_task4(void *arg)
{
    void *status = NULL;

    pthread_cleanup_push(output_cleanup_msg, "first cleanup function");
    pthread_cleanup_push(output_cleanup_msg, "second cleanup function");

    LOGI(TAG, "demo_task4 is running!\n");

    pthread_exit(status);

    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);

    return NULL;
}

static void *demo_task5(void *arg)
{
    int   count  = 0;
    void *status = NULL;

    while (1) {
        count++;
        LOGI(TAG, "demo_task5 is running %d\n", count);

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 10) {
            LOGI(TAG, "*********** posix_pthread_detach_case1 end ***********\n");
            pthread_exit(status);
        }
    };

    return NULL;
}

static void *demo_task6(void *arg)
{
    int   count  = 0;
    void *status = NULL;

    while (1) {
        count++;
        LOGI(TAG, "demo_task6 is running %d\n", count);

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 5) {
            pthread_detach(thread1);
            LOGI(TAG, "demo_task6 detach demo_task5 \n");
            pthread_exit(status);
        }
    };

    return NULL;
}

static void *demo_task7(void *arg)
{
    int   count  = 0;
    void *status = NULL;

    while (1) {
        count++;
        LOGI(TAG, "demo_task7 is running %d\n", count);

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 5) {
            pthread_exit(status);
        }
    };

    return NULL;
}

static void *demo_task8(void *arg)
{
    int   count  = 0;
    void *status = NULL;

    while (1) {
        count++;
        LOGI(TAG, "demo_task8 is running %d\n", count);

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 8) {
            pthread_detach(thread1);
            LOGI(TAG, "demo_task8 detach demo_task5 \n");
        }

        if (count == 10) {
            LOGI(TAG, "*********** posix_pthread_detach_case2 end ***********\n");
            pthread_exit(status);
        }
    };

    return NULL;
}

void once_task(void)
{
    LOGI(TAG, "once_task is running !\n");
}

static void *demo_task9(void *arg)
{
    int   count  = 0;
    void *status = NULL;

    static int flag = PTHREAD_ONCE_INIT;

    while (1) {
        count++;
        LOGI(TAG, "demo_task9 is running %d\n", count);

        pthread_once(&flag, once_task);

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 10) {
            pthread_exit(status);
        }
    };

    return NULL;
}

static void *demo_task10(void *arg)
{
    void *status = NULL;
    int   ret    = -1;

    struct sched_param param;

    param.sched_priority = 31;
    param.slice = 10;

    krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

    ret = pthread_setschedparam(pthread_self(), SCHED_RR, &param);
    if (ret == 0) {
        if ((pthread_self()->prio == PRI_CONVERT_PX_RH(param.sched_priority))
            && (pthread_self()->time_slice == (param.slice * 1000) / RHINO_CONFIG_TICKS_PER_SECOND)) {
            LOGI(TAG, "posix_pthread_setschedparam_case1 test OK !\n");
        }
    }

    pthread_exit(status);

    return NULL;
}

static void *demo_task11(void *arg)
{
    void *status = NULL;
    int   ret    = -1;

    struct sched_param param;

    param.sched_priority = 31;
    param.slice = 10;

    krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

    ret = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
    if (ret == 0) {
        if ((pthread_self()->prio == PRI_CONVERT_PX_RH(param.sched_priority))
            && (pthread_self()->time_slice != (param.slice * 1000) / RHINO_CONFIG_TICKS_PER_SECOND)) {
            LOGI(TAG, "posix_pthread_setschedparam_case2 test OK !\n");
        }
    }

    pthread_exit(status);

    return NULL;
}

static void *demo_task12(void *arg)
{
    void *status = NULL;
    int   ret    = -1;

    krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

    ret = pthread_setschedprio(pthread_self(), 10);
    if (ret == 0) {
        if (pthread_self()->prio == PRI_CONVERT_PX_RH(10)) {
            LOGI(TAG, "posix_pthread_setschedprio_case test OK !\n");
        }
    }

    pthread_exit(status);

    return NULL;
}

/******************* test pthread_create and  pthread_join **********************/
void posix_pthread_create_case(void)
{
    pthread_t thread;
    int       ret = -1;
    void     *status = NULL;

    LOGI(TAG, "*********** posix_pthread_create_case start ***********\n");

    attr.stacksize = 2048;
    attr.schedparam.sched_priority = 30;
    attr.detachstate = PTHREAD_CREATE_JOINABLE;

    ret = pthread_create(&thread, &attr, demo_task, NULL);

    if (ret == 0) {
        LOGI(TAG, "pthread_create end\n");
    }

    if (ret == 0) {
        ret = pthread_join(thread, &status);
    }

    if (ret == 0) {
        LOGI(TAG, "pthread resourse relased\n");
    }

    LOGI(TAG, "*********** posix_pthread_create_case end ***********\n");
}

void posix_pthread_join_case(void)
{
    int ret1 = -1;
    int ret2 = -1;
    int ret = -1;
    void *status = NULL;

    memset(&attr, 0, sizeof(attr));

    LOGI(TAG, "*********** posix_pthread_join_case start ***********\n");

    attr.stacksize = 2048;
    attr.schedparam.sched_priority = 20;
    attr.detachstate = PTHREAD_CREATE_JOINABLE;

    ret1 = pthread_create(&thread1, &attr, demo_task1, NULL);

    attr.stacksize = 2048;
    attr.schedparam.sched_priority = 30;
    attr.detachstate = PTHREAD_CREATE_JOINABLE;

    ret2 = pthread_create(&thread2, NULL, demo_task2, NULL);

    if ((ret2 == 0) && (ret1 == 0)) {
        LOGI(TAG, "thread2 create end\n");
        ret = pthread_join(thread1, &status);
    }

    if (ret == 0) {
        LOGI(TAG, "pthread resourse relased\n");
    }

    LOGI(TAG, "*********** posix_pthread_join_case end ***********\n");
}

/*************************** test pthread_cleanup *****************************/
void posix_pthread_cleanup_case1(void)
{
    pthread_t thread;
    int ret = -1;
    void *status = NULL;

    LOGI(TAG, "*********** posix_pthread_cleanup_case1 start ***********\n");

    attr.stacksize = 2048;
    attr.schedparam.sched_priority = 30;
    attr.detachstate = PTHREAD_CREATE_JOINABLE;

    ret = pthread_create(&thread, &attr, demo_task3, NULL);

    if (ret == 0) {
        LOGI(TAG, "pthread_create end\n");
    }

    if (ret == 0) {
        ret = pthread_join(thread, &status);
    }

    if (ret == 0) {
        LOGI(TAG, "pthread resourse relased\n");
    }

    LOGI(TAG, "*********** posix_pthread_cleanup_case1 end ***********\n");
}

void posix_pthread_cleanup_case2(void)
{
    pthread_t thread;
    int       ret = -1;
    void      *status = NULL;

    LOGI(TAG, "*********** posix_pthread_cleanup_case2 start ***********\n");

    attr.stacksize = 2048;
    attr.schedparam.sched_priority = 30;
    attr.detachstate = PTHREAD_CREATE_JOINABLE;

    ret = pthread_create(&thread, &attr, demo_task4, NULL);

    if (ret == 0) {
        LOGI(TAG, "pthread_create end\n");
    }

    if (ret == 0) {
        ret = pthread_join(thread, &status);
    }

    if (ret == 0) {
        LOGI(TAG, "pthread resourse relased\n");
    }

    LOGI(TAG, "*********** posix_pthread_cleanup_case2 end ***********\n");
}

/*************************** test pthread_detach *****************************/
void posix_pthread_detach_case1(void)
{
    int   ret1 = -1;
    int   ret2 = -1;
    int   ret  = -1;
    void *status = NULL;

    memset(&attr, 0, sizeof(attr));

    LOGI(TAG, "*********** posix_pthread_detach_case1 start ***********\n");

    attr.stacksize = 2048;
    attr.schedparam.sched_priority = 20;
    attr.detachstate = PTHREAD_CREATE_JOINABLE;

    ret1 = pthread_create(&thread1, &attr, demo_task5, NULL);

    attr.stacksize = 2048;
    attr.schedparam.sched_priority = 30;
    attr.detachstate = PTHREAD_CREATE_JOINABLE;

    ret2 = pthread_create(&thread2, NULL, demo_task6, NULL);

    if ((ret2 == 0) && (ret1 == 0)) {
        LOGI(TAG, "thread2 create end\n");
        ret = pthread_join(thread1, &status);
    }

    if (ret == 0) {
        LOGI(TAG, "father thread end\n");
    }
}

void posix_pthread_detach_case2(void)
{
    int ret1 = -1;
    int ret2 = -1;
    int ret  = -1;

    memset(&attr, 0, sizeof(attr));

    LOGI(TAG, "*********** posix_pthread_detach_case2 start ***********\n");

    attr.stacksize = 2048;
    attr.schedparam.sched_priority = 20;
    attr.detachstate = PTHREAD_CREATE_JOINABLE;

    ret1 = pthread_create(&thread1, &attr, demo_task7, NULL);

    attr.stacksize = 2048;
    attr.schedparam.sched_priority = 30;
    attr.detachstate = PTHREAD_CREATE_JOINABLE;

    ret2 = pthread_create(&thread2, NULL, demo_task8, NULL);

    if ((ret2 == 0) && (ret1 == 0)) {
        LOGI(TAG, "thread2 create end\n");
    }

    if (ret == 0) {
        LOGI(TAG, "father thread end\n");
    }
}

/*************************** test pthread_once *****************************/
void posix_pthread_once_case(void)
{
    pthread_t thread;
    int       ret = -1;
    void     *status = NULL;

    LOGI(TAG, "*********** posix_pthread_once_case start ***********\n");

    attr.stacksize = 2048;
    attr.schedparam.sched_priority = 30;
    attr.detachstate = PTHREAD_CREATE_JOINABLE;

    ret = pthread_create(&thread, &attr, demo_task9, NULL);

    if (ret == 0) {
        LOGI(TAG, "pthread_create end\n");
    }

    if (ret == 0) {
        ret = pthread_join(thread, &status);
    }

    if (ret == 0) {
        LOGI(TAG, "pthread resourse relased\n");
    }

    LOGI(TAG, "*********** posix_pthread_once_case end ***********\n");
}

/*************************** test pthread_setschedparam *****************************/
void posix_pthread_setschedparam_case1(void)
{
    pthread_t thread;
    int       ret = -1;
    void     *status = NULL;

    LOGI(TAG, "*********** posix_pthread_setschedparam_case1 start ***********\n");

    attr.stacksize = 2048;
    attr.schedparam.sched_priority = 30;
    attr.detachstate = PTHREAD_CREATE_JOINABLE;

    ret = pthread_create(&thread, &attr, demo_task10, NULL);
    if (ret == 0) {
        LOGI(TAG, "pthread_create end\n");
    }

    if (ret == 0) {
        ret = pthread_join(thread, &status);
    }

    LOGI(TAG, "*********** posix_pthread_setschedparam_case1 end ***********\n");
}

void posix_pthread_setschedparam_case2(void)
{
    pthread_t thread;
    int       ret = -1;
    void     *status = NULL;

    LOGI(TAG, "*********** posix_pthread_setschedparam_case2 start ***********\n");

    attr.stacksize = 2048;
    attr.schedparam.sched_priority = 30;
    attr.detachstate = PTHREAD_CREATE_JOINABLE;

    ret = pthread_create(&thread, &attr, demo_task11, NULL);
    if (ret == 0) {
        LOGI(TAG, "pthread_create end\n");
    }

    if (ret == 0) {
        ret = pthread_join(thread, &status);
    }

    LOGI(TAG, "*********** posix_pthread_setschedparam_case2 end ***********\n");
}

/*************************** test pthread_setschedprio *****************************/
void posix_pthread_setschedprio_case(void)
{
    pthread_t thread;
    int       ret = -1;
    void     *status = NULL;

    LOGI(TAG, "*********** posix_pthread_setschedprio_case start ***********\n");

    attr.stacksize = 2048;
    attr.schedparam.sched_priority = 30;
    attr.detachstate = PTHREAD_CREATE_JOINABLE;

    ret = pthread_create(&thread, &attr, demo_task12, NULL);
    if (ret == 0) {
        LOGI(TAG, "pthread_create end\n");
    }

    if (ret == 0) {
        ret = pthread_join(thread, &status);
    }

    LOGI(TAG, "*********** posix_pthread_setschedprio_case end ***********\n");
}
