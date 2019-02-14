/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "k_api.h"
#include "posix/pthread.h"
#include "ulog/ulog.h"

static pthread_mutex_t mutex;

static void *init_task(void *arg)
{
    pthread_mutex_init(&mutex, NULL);

    return NULL;
}

static void *demo_task1(void *arg)
{
    int   count  = 0;
    void *status = NULL;

    while (1) {
        pthread_mutex_lock(&mutex);

        LOGI(TAG, "demo_task1 get mutex %d\n", count);
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND * 5);

        pthread_mutex_unlock(&mutex);

        if (count == 5) {
            pthread_exit(status);
        }

        count++;
    };

    return NULL;
}

static void *demo_task2(void *arg)
{
    int   count  = 0;
    void *status = NULL;

    while (1) {
        pthread_mutex_lock(&mutex);

        LOGI(TAG, "demo_task2 get mutex %d\n", count);
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        pthread_mutex_unlock(&mutex);

        if (count == 10) {
            pthread_exit(status);
        }

        count++;
    };

    return NULL;
}

/*
The correct serial port print data when executing this test case is as follows：

posix_mutex_case start !
demo_task1 get mutex 0
(interval 5s)
demo_task2 get mutex 0
(interval 1s)
demo_task1 get mutex 1
(interval 5s)
demo_task2 get mutex 1
(interval 1s)
demo_task1 get mutex 2
(interval 5s)
demo_task2 get mutex 2
(interval 1s)
demo_task1 get mutex 3
(interval 5s)
demo_task2 get mutex 3
(interval 1s)
demo_task1 get mutex 4
(interval 5s)
demo_task2 get mutex 4
(interval 1s)
demo_task2 get mutex 5
(interval 1s)
demo_task2 get mutex 6
(interval 1s)
demo_task2 get mutex 7
(interval 1s)
demo_task2 get mutex 8
(interval 1s)
demo_task2 get mutex 9
(interval 1s)
posix_mutex_case end !
*/
void posix_mutex_case(void)
{
    int   ret    = -1;
    void *status = NULL;

    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;

    LOGI(TAG, "posix_mutex_case start !\n");

    ret = pthread_create(&thread1, NULL, init_task, NULL);
    ret |= pthread_create(&thread2, NULL, demo_task1, NULL);
    ret |= pthread_create(&thread3, NULL, demo_task2, NULL);

    if (ret == 0) {
        ret = pthread_join(thread1, &status);
        ret = pthread_join(thread2, &status);
        ret = pthread_join(thread3, &status);
    }

    pthread_mutex_destroy(&mutex);

    LOGI(TAG, "posix_mutex_case end !\n");
}
