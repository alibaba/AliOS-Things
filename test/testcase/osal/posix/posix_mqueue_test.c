/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "k_api.h"
#include "posix/pthread.h"
#include "posix/mqueue.h"
#include "ulog/ulog.h"

#define TOTAL_MSG_NUM 5

static mqd_t mqueue;

static char recv_buf[1024];
static int  succeed_flag = 1;

static char *test_msg[TOTAL_MSG_NUM] = {"os=AliOS Things", "version=v2.1", "component=osal.posix",
                                        "author=Jason","email=jason_sophia@aliyun.com"};

static void posix_mqueue_case1(void);
static void posix_mqueue_case2(void);
static void posix_mqueue_case3(void);

void posix_mqueue_test_case(void)
{
    posix_mqueue_case1();
    posix_mqueue_case2();
    posix_mqueue_case3();
}

static void *demo_task1(void *arg)
{
    int   count  = 0;
    void *status = NULL;
    int   ret    = NULL;
    int   len    = 0;

    while (1) {
        LOGI(TAG, "mqueue send message %d\n", count);

        if (count < TOTAL_MSG_NUM) {
            len = strlen(test_msg[count]);
            ret = mq_send(mqueue, test_msg[count], len, 0);
            if (ret != len) {
                succeed_flag = 0;
            }
        }

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == TOTAL_MSG_NUM) {
            pthread_exit(status);
        }

        count++;
    }

    return NULL;
}

static void *demo_task2(void *arg)
{
    int   count  = 0;
    void *status = NULL;
    int   ret    = NULL;
    int   len    = 0;

    while (1) {
        LOGI(TAG, "mqueue receive message %d\n", count);

        if (count < TOTAL_MSG_NUM) {
            len = strlen(test_msg[count]);
            ret = mq_receive(mqueue, recv_buf, len, NULL);
            if ((ret != len) || (strncmp(recv_buf, test_msg[count], len) != 0)) {
                succeed_flag = 0;
            }
        }

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == TOTAL_MSG_NUM) {
            pthread_exit(status);
        }

        count++;
    }

    return NULL;
}

static void *demo_task3(void *arg)
{
    int   count  = 0;
    void *status = NULL;
    int   ret    = NULL;
    int   len    = 0;

    while (1) {
        if (count < 3) {
            len = strlen(test_msg[count]);
            ret = mq_send(mqueue, test_msg[count], len, 0);
            if (ret != len) {
                succeed_flag = 0;
            }

            LOGI(TAG, "mqueue send message %d\n", count);
        }

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == TOTAL_MSG_NUM) {
            pthread_exit(status);
        }

        count++;
    }

    return NULL;
}

static void *demo_task4(void *arg)
{
    int   count  = 0;
    void *status = NULL;
    int   ret    = NULL;
    int   len    = 0;

    struct timespec ts;

    ts.tv_sec = 2;
    ts.tv_nsec = 0;

    while (1) {
        LOGI(TAG, "mqueue receive message %d\n", count);

        if (count < TOTAL_MSG_NUM) {
            len = strlen(test_msg[count]);
            ret = mq_timedreceive(mqueue, recv_buf, len, NULL, &ts);


            if ((count < 3)&&((ret != len)||(strncmp(recv_buf, test_msg[count], len) != 0))) {
                succeed_flag = 0;
            }

            if ((count >= 3)&&((ret == len)||(strncmp(recv_buf, test_msg[count], len) == 0))) {
                succeed_flag = 0;
            }
        }

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == TOTAL_MSG_NUM) {
            pthread_exit(status);
        }

        count++;
    }

    return NULL;
}

static void *demo_task5(void *arg)
{
    int   count  = 0;
    void *status = NULL;
    int   ret    = NULL;
    int   len    = 0;

    struct timespec ts;

    ts.tv_sec = 2;
    ts.tv_nsec = 0;

    while (1) {
        if (count == 0) {
            len = strlen(test_msg[0]);
            ret = mq_timedreceive(mqueue, recv_buf, len, NULL, &ts);

            if ((ret == len)||(strncmp(recv_buf, test_msg[count], len) == 0)) {
                succeed_flag = 0;
            }

            ret = mq_timedreceive(mqueue, recv_buf, len, NULL, &ts);

            if ((ret != len)||(strncmp(recv_buf, test_msg[count], len) != 0)) {
                succeed_flag = 0;
            }

            LOGI(TAG, "mqueue receive message %d\n", count);
        }

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == TOTAL_MSG_NUM) {
            pthread_exit(status);
        }

        count++;
    }

    return NULL;
}

static void *demo_task6(void *arg)
{
    int   count  = 0;
    void *status = NULL;
    int   ret    = NULL;
    int   len    = 0;

    while (1) {
        if (count == 0) {
            krhino_task_sleep(3 * RHINO_CONFIG_TICKS_PER_SECOND);

            len = strlen(test_msg[0]);
            ret = mq_send(mqueue, test_msg[0], len, 0);
            if (ret != len) {
                succeed_flag = 0;
            }

            LOGI(TAG, "mqueue send message %d\n", count);
        }

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);

        if (count == TOTAL_MSG_NUM) {
            pthread_exit(status);
        }

        count++;
    }

    return NULL;
}

void posix_mqueue_case1(void)
{
    int   ret    = -1;
    void *status = NULL;

    pthread_t thread1;
    pthread_t thread2;

    LOGI(TAG, "posix_mqueue_case1 start !\n");

    succeed_flag = 1;

    memset(recv_buf, 0, sizeof(recv_buf));

    mqueue = mq_open("mqueue", 0);
    if (mqueue == NULL) {
        succeed_flag = 0;
    }

    ret = pthread_create(&thread1, NULL, demo_task1, NULL);
    ret |= pthread_create(&thread2, NULL, demo_task2, NULL);

    if (ret == 0) {
        ret = pthread_join(thread1, &status);
        ret |= pthread_join(thread2, &status);
    }

    mq_close(mqueue);

    if (succeed_flag == 1) {
        printf("posix_mqueue_case1 test OK !\n");
    } else {
        printf("posix_mqueue_case1 test failed !\n");
    }

    LOGI(TAG, "posix_mqueue_case1 end !\n");
}

void posix_mqueue_case2(void)
{
    int   ret    = -1;
    void *status = NULL;

    pthread_t thread1;
    pthread_t thread2;

    LOGI(TAG, "posix_mqueue_case2 start !\n");

    succeed_flag = 1;

    memset(recv_buf, 0, sizeof(recv_buf));

    mqueue = mq_open("mqueue", 0);
    if (mqueue == NULL) {
        succeed_flag = 0;
    }

    ret = pthread_create(&thread1, NULL, demo_task3, NULL);
    ret |= pthread_create(&thread2, NULL, demo_task4, NULL);

    if (ret == 0) {
        ret = pthread_join(thread1, &status);
        ret |= pthread_join(thread2, &status);
    }

    mq_close(mqueue);

    if (succeed_flag == 1) {
        printf("posix_mqueue_case2 test OK !\n");
    } else {
        printf("posix_mqueue_case2 test failed !\n");
    }

    LOGI(TAG, "posix_mqueue_case2 end !\n");
}

void posix_mqueue_case3(void)
{
    int   ret    = -1;
    void *status = NULL;

    pthread_t thread1;
    pthread_t thread2;

    LOGI(TAG, "posix_mqueue_case3 start !\n");

    succeed_flag = 1;

    memset(recv_buf, 0, sizeof(recv_buf));

    mqueue = mq_open("mqueue", 0);
    if (mqueue == NULL) {
        succeed_flag = 0;
    }

    ret = pthread_create(&thread1, NULL, demo_task5, NULL);
    ret |= pthread_create(&thread2, NULL, demo_task6, NULL);

    if (ret == 0) {
        ret = pthread_join(thread1, &status);
        ret |= pthread_join(thread2, &status);
    }

    mq_close(mqueue);

    if (succeed_flag == 1) {
        printf("posix_mqueue_case3 test OK !\n");
    } else {
        printf("posix_mqueue_case3 test failed !\n");
    }

    LOGI(TAG, "posix_mqueue_case3 end !\n");
}
