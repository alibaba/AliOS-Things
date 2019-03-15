/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "k_api.h"
#include "posix/pthread.h"
#include "ulog/ulog.h"

static pthread_t       thread1;
static pthread_t       thread2;
static pthread_t       thread3;
static pthread_cond_t  cond;
static pthread_mutex_t mutex;

static int   flag   = 1;
static void *status = NULL;

extern int gettimeofday(struct timeval *tv, void *tzp);

void posix_cond_case(void);
void posix_pthread_condattr_init_case(void);
void posix_pthread_condattr_destroy_case(void);

void posix_cond_test_case(void)
{
    posix_cond_case();
    posix_pthread_condattr_init_case();
    posix_pthread_condattr_destroy_case();
}

static void *demo_task1(void *arg)
{
    struct timeval  now;
    struct timespec outtime;

    LOGI(TAG, "demo_task1 !\n");

    pthread_mutex_lock(&mutex);

    while (flag) {
        LOGI(TAG, "demo_task1\n");
        gettimeofday(&now, NULL);

        outtime.tv_sec  = now.tv_sec + 5;
        outtime.tv_nsec = now.tv_usec * 1000;

        pthread_cond_timedwait(&cond, &mutex, &outtime);
    }

    pthread_mutex_unlock(&mutex);
    LOGI(TAG, "demo_task1 exit\n");

    pthread_exit(status);

    return NULL;
}

static void *demo_task2(void *arg)
{
    LOGI(TAG, "demo_task2 !\n");

    krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND * 10);

    LOGI(TAG, "Now terminate the thread!\n");

    flag = 0;

    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    pthread_exit(status);

    return NULL;
}

static void *init_task(void *arg)
{
    LOGI(TAG, "init_task !\n");

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_create(&thread2, NULL, demo_task1, NULL);
    pthread_create(&thread3, NULL, demo_task2, NULL);

    pthread_join(thread2, &status);
    pthread_join(thread3, &status);

    pthread_exit(status);

    return NULL;
}

/*************************** test pthread_cond *****************************/
void posix_cond_case(void)
{
    char *casename = "posix_cond_case";

    LOGI(TAG, "*********** %s start ***********\n", casename);

    pthread_create(&thread1, NULL, init_task, NULL);

    pthread_join(thread1, &status);

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

/*************************** test pthread_condattr_init *****************************/
void posix_pthread_condattr_init_case(void)
{
    pthread_condattr_t attr;

    char *casename = "posix_pthread_condattr_init_case";
    int   flag     = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    pthread_condattr_init(&attr);

    if ((attr.clock == DEFAULT_COND_CLOCK)
       &&(attr.pshared == DEFAULT_COND_SHARED)
       &&(attr.is_initialized == 1)) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

/*************************** test pthread_condattr_destroy *****************************/
void posix_pthread_condattr_destroy_case(void)
{
    pthread_condattr_t attr;

    char *casename = "posix_pthread_condattr_destroy_case";
    int   flag     = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    pthread_condattr_destroy(&attr);

    if ((attr.clock == 0)
       &&(attr.pshared == 0)
       &&(attr.is_initialized == 0)) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag == 1) {
        LOGI(TAG, "%s succeed !\n", casename);
    } else {
        LOGI(TAG, "%s failed !\n", casename);
    }

    LOGI(TAG, "*********** %s end ***********\n", casename);
}

