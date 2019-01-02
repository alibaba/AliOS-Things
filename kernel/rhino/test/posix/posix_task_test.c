/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void posix_task_case1(void);
void posix_task_case2(void);
void posix_task_case3(void);
void posix_task_case4(void);
void posix_task_case5(void);
void posix_task_case6(void);

static void *demo_task(void *arg);
static void *demo_task1(void *arg);
static void *demo_task2(void *arg);
static void *demo_task3(void *arg);
static void *demo_task4(void *arg);
static void *demo_task5(void *arg);
static void *demo_task6(void *arg);

pthread_t thread1;
pthread_t thread2;

pthread_attr_t attr;

static void *demo_task(void *arg)
{
    int   count  = 0;
    void *status = NULL;

    while (1) {
        printf("hello world! count  %d\n", count++);

        // sleep 1 second
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
        printf("demo_task1 count  %d\n", count++);

        // sleep 1 second
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
        printf("demo_task2 count  %d\n", count++);

        // sleep 1 second
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 10) {
            pthread_exit(status);
        }
    };

    return NULL;
}


static void output_cleanup_msg(void *msg)
{
    printf("%s is running!\n", (char *)msg);
}

static void *demo_task3(void *arg)
{
    void *status = NULL;

    pthread_cleanup_push(output_cleanup_msg, "first cleanup function");
    pthread_cleanup_push(output_cleanup_msg, "second cleanup function");

    printf("demo_task3 is running!\n");

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

    printf("demo_task3 is running!\n");

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
        printf("demo_task5 count  %d\n", count++);

        // sleep 1 second
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 10) {
            printf("*********** posix_task_case5 end ***********\n");
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
        printf("demo_task6 count  %d\n", count++);

        // sleep 1 second
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 5) {
            pthread_detach(thread1);
            printf("demo_task6 detach demo_task5 \n");
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
        printf("demo_task7 count  %d\n", count++);

        // sleep 1 second
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
        printf("demo_task8 count  %d\n", count++);

        // sleep 1 second
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == 8) {
            pthread_detach(thread1);
            printf("demo_task8 detach demo_task5 \n");
        }

        if (count == 10) {
            printf("*********** posix_task_case6 end ***********\n");
            pthread_exit(status);
        }
    };

    return NULL;
}

/******************* test pthread_create and  pthread_join
 * *********************/
void posix_task_case1(void)
{
    pthread_t thread;
    int       ret    = -1;
    void *    status = NULL;

    printf("*********** posix_task_case1 start ***********\n");

    attr.stacksize                 = 2048;
    attr.schedparam.sched_priority = 30;
    attr.detachstate               = PTHREAD_CREATE_JOINABLE;

    ret = pthread_create(&thread, &attr, demo_task, NULL);

    if (ret == 0) {
        printf("pthread_create end\n");
    }

    if (ret == 0) {
        ret = pthread_join(thread, &status);
    }

    if (ret == 0) {
        printf("pthread resourse relased\n");
    }

    printf("*********** posix_task_case1 end ***********\n");
}

void posix_task_case2(void)
{
    int   ret1   = -1;
    int   ret2   = -1;
    int   ret    = -1;
    void *status = NULL;

    memset(&attr, 0, sizeof(attr));

    printf("*********** posix_task_case2 start ***********\n");

    attr.stacksize                 = 2048;
    attr.schedparam.sched_priority = 20;
    attr.detachstate               = PTHREAD_CREATE_JOINABLE;

    ret1 = pthread_create(&thread1, &attr, demo_task1, NULL);

    attr.stacksize                 = 2048;
    attr.schedparam.sched_priority = 30;
    attr.detachstate               = PTHREAD_CREATE_JOINABLE;

    ret2 = pthread_create(&thread2, NULL, demo_task2, NULL);

    if ((ret2 == 0) && (ret1 == 0)) {
        printf("thread2 create end\n");
        ret = pthread_join(thread1, &status);
    }

    if (ret == 0) {
        printf("pthread resourse relased\n");
    }

    printf("*********** posix_task_case2 end ***********\n");
}

/*************************** test pthread_cleanup *****************************/
void posix_task_case3(void)
{
    pthread_t thread;
    int       ret    = -1;
    void *    status = NULL;

    printf("*********** posix_task_case3 start ***********\n");

    attr.stacksize                 = 2048;
    attr.schedparam.sched_priority = 30;
    attr.detachstate               = PTHREAD_CREATE_JOINABLE;

    ret = pthread_create(&thread, &attr, demo_task3, NULL);

    if (ret == 0) {
        printf("pthread_create end\n");
    }

    if (ret == 0) {
        ret = pthread_join(thread, &status);
    }

    if (ret == 0) {
        printf("pthread resourse relased\n");
    }

    printf("*********** posix_task_case3 end ***********\n");
}

void posix_task_case4(void)
{
    pthread_t thread;
    int       ret    = -1;
    void *    status = NULL;

    printf("*********** posix_task_case4 start ***********\n");

    attr.stacksize                 = 2048;
    attr.schedparam.sched_priority = 30;
    attr.detachstate               = PTHREAD_CREATE_JOINABLE;

    ret = pthread_create(&thread, &attr, demo_task4, NULL);

    if (ret == 0) {
        printf("pthread_create end\n");
    }

    if (ret == 0) {
        ret = pthread_join(thread, &status);
    }

    if (ret == 0) {
        printf("pthread resourse relased\n");
    }

    printf("*********** posix_task_case4 end ***********\n");
}

/*************************** test pthread_detach *****************************/
void posix_task_case5(void)
{
    int   ret1   = -1;
    int   ret2   = -1;
    int   ret    = -1;
    void *status = NULL;

    memset(&attr, 0, sizeof(attr));

    printf("*********** posix_task_case5 start ***********\n");

    attr.stacksize                 = 2048;
    attr.schedparam.sched_priority = 20;
    attr.detachstate               = PTHREAD_CREATE_JOINABLE;

    ret1 = pthread_create(&thread1, &attr, demo_task5, NULL);

    attr.stacksize                 = 2048;
    attr.schedparam.sched_priority = 30;
    attr.detachstate               = PTHREAD_CREATE_JOINABLE;

    ret2 = pthread_create(&thread2, NULL, demo_task6, NULL);

    if ((ret2 == 0) && (ret1 == 0)) {
        printf("thread2 create end\n");
        ret = pthread_join(thread1, &status);
    }

    if (ret == 0) {
        printf("father thread end\n");
    }
}

void posix_task_case6(void)
{
    int ret1 = -1;
    int ret2 = -1;
    int ret  = -1;

    memset(&attr, 0, sizeof(attr));

    printf("*********** posix_task_case6 start ***********\n");

    attr.stacksize                 = 2048;
    attr.schedparam.sched_priority = 20;
    attr.detachstate               = PTHREAD_CREATE_JOINABLE;

    ret1 = pthread_create(&thread1, &attr, demo_task7, NULL);

    attr.stacksize                 = 2048;
    attr.schedparam.sched_priority = 30;
    attr.detachstate               = PTHREAD_CREATE_JOINABLE;

    ret2 = pthread_create(&thread2, NULL, demo_task8, NULL);

    if ((ret2 == 0) && (ret1 == 0)) {
        printf("thread2 create end\n");
    }

    if (ret == 0) {
        printf("father thread end\n");
    }
}
