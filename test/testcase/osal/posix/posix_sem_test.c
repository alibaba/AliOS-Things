/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "k_api.h"
#include "posix/pthread.h"
#include "posix/semaphore.h"
#include "ulog/ulog.h"

void posix_sem_case1(void);
void posix_sem_case2(void);

static void *init_task(void *arg);
static void *demo_task1(void *arg);
static void *demo_task2(void *arg);
static void *demo_task3(void *arg);
static void *demo_task4(void *arg);

static sem_t     sem;
static pthread_t thread1;
static pthread_t thread2;

void posix_sem_test_case(void)
{
    posix_sem_case1();
    posix_sem_case2();
}

static void *init_task(void *arg)
{
    sem_init(&sem, 0, 0);

    return NULL;
}

static void *demo_task1(void *arg)
{
    int   count  = 0;
    void *status = NULL;

    while (1) {
        sem_wait(&sem);

        count++;
        LOGI(TAG, "demo_task1 get sem %d\n", count);

        if (count == 5) {
            pthread_exit(status);
        }
    }

    return NULL;
}

static void *demo_task2(void *arg)
{
    int   count     = 0;
    int   sem_count = 0;
    void *status    = NULL;

    while (1) {
        count++;
        LOGI(TAG, "demo_task2 run count %d\n", count);

        if (count % 2 == 0) {
            sem_post(&sem);

            sem_count++;
            LOGI(TAG, "demo_task2 release sem %d\n", sem_count);
        }

        if (sem_count == 5) {
            pthread_exit(status);
        }

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    }

    return NULL;
}

static void *demo_task3(void *arg)
{
    int   count     = 0;
    int   sem_count = 0;
    int   ret       = -1;
    void *status    = NULL;

    struct timespec abs_timeout;

    abs_timeout.tv_sec  = 1;
    abs_timeout.tv_nsec = 1000000000;

    while (1) {
        count++;
        LOGI(TAG, "demo_task3 run count %d\n", count);
        ret = sem_timedwait(&sem, &abs_timeout);

        if (ret == 0) {
            sem_count++;
            LOGI(TAG, "demo_task3 get sem %d\n", sem_count);
        } else {
            LOGI(TAG, "failed to get sem\n");
        }

        if (sem_count == 5) {
            pthread_exit(status);
        }
    }

    return NULL;
}

static void *demo_task4(void *arg)
{
    int   sem_count = 0;
    void *status    = NULL;

    while (1) {
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND * 3);
        sem_post(&sem);

        sem_count++;
        LOGI(TAG, "demo_task4 release sem %d\n", sem_count);

        if (sem_count == 5) {
            pthread_exit(status);
        }
    }

    return NULL;
}

void posix_sem_case1(void)
{
    pthread_t thread;
    void     *status = NULL;
    int       ret    = -1;

    LOGI(TAG, "*********** posix_sem_case1 start ***********\n");

    ret = pthread_create(&thread, NULL, init_task, NULL);
    ret |= pthread_create(&thread1, NULL, demo_task1, NULL);
    ret |= pthread_create(&thread2, NULL, demo_task2, NULL);

    if (ret == 0) {
        pthread_join(thread, &status);
        pthread_join(thread1, &status);
        pthread_join(thread2, &status);
    }

    LOGI(TAG, "*********** posix_sem_case1 end ***********\n");
}

void posix_sem_case2(void)
{
    pthread_t thread;
    void     *status = NULL;
    int       ret    = -1;

    LOGI(TAG, "*********** posix_sem_case2 start ***********\n");

    ret = pthread_create(&thread, NULL, init_task, NULL);
    ret |= pthread_create(&thread1, NULL, demo_task3, NULL);
    ret |= pthread_create(&thread2, NULL, demo_task4, NULL);

    if (ret == 0) {
        pthread_join(thread, &status);
        pthread_join(thread1, &status);
        pthread_join(thread2, &status);
    }

    LOGI(TAG, "*********** posix_sem_case2 end ***********\n");
}
