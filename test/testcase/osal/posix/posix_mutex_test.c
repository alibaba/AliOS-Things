/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "k_api.h"
#include "posix/pthread.h"
#include "ulog/ulog.h"

static pthread_mutex_t mutex;

void posix_mutex_case(void);
void posix_pthread_mutexattr_init_case(void);
void posix_pthread_mutexattr_destroy_case(void);
void posix_pthread_mutexattr_settype_case(void);
void posix_pthread_mutexattr_gettype_case(void);
void posix_pthread_mutexattr_setprotocol_case(void);
void posix_pthread_mutexattr_getprotocol_case(void);
void posix_pthread_mutex_setprioceiling_case(void);
void posix_pthread_mutex_getprioceiling_case(void);
void posix_pthread_mutexattr_setprioceiling_case(void);
void posix_pthread_mutexattr_getprioceiling_case(void);
void posix_pthread_mutexattr_setpshared_case(void);
void posix_pthread_mutexattr_getpshared_case(void);

void posix_mutex_test_case(void)
{
    posix_mutex_case();
    posix_pthread_mutexattr_init_case();
    posix_pthread_mutexattr_destroy_case();
    posix_pthread_mutexattr_settype_case();
    posix_pthread_mutexattr_gettype_case();
    posix_pthread_mutexattr_setprotocol_case();
    posix_pthread_mutexattr_getprotocol_case();
    posix_pthread_mutex_setprioceiling_case();
    posix_pthread_mutex_getprioceiling_case();
    posix_pthread_mutexattr_setprioceiling_case();
    posix_pthread_mutexattr_getprioceiling_case();
    posix_pthread_mutexattr_setpshared_case();
    posix_pthread_mutexattr_getpshared_case();
}

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

/*************************** test pthread_mutexattr_init *****************************/
void posix_pthread_mutexattr_init_case(void)
{
    pthread_mutexattr_t attr;

    char *casename = "posix_pthread_mutexattr_init_case";
    int   flag     = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    pthread_mutexattr_init(&attr);

    if ((attr.type == DEFAULT_MUTEX_TYPE)
       &&(attr.protocol == DEFAULT_MUTEX_PROCOCOL)
       &&(attr.prioceiling == DEFAULT_MUTEX_PRIOCEILING)
       &&(attr.pshared == DEFAULT_MUTEX_PSHARED)
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

/*************************** test pthread_mutexattr_destroy *****************************/
void posix_pthread_mutexattr_destroy_case(void)
{
    pthread_mutexattr_t attr;

    char *casename = "posix_pthread_mutexattr_destroy_case";
    int   flag     = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    pthread_mutexattr_destroy(&attr);

    if ((attr.type == 0)
       &&(attr.protocol == 0)
       &&(attr.prioceiling == 0)
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

/*************************** test pthread_mutexattr_settype *****************************/
void posix_pthread_mutexattr_settype_case(void)
{
    pthread_mutexattr_t attr;

    char *casename = "posix_pthread_mutexattr_settype_case";
    int   flag     = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

    if (attr.type == PTHREAD_MUTEX_ERRORCHECK) {
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

/*************************** test pthread_mutexattr_gettype *****************************/
void posix_pthread_mutexattr_gettype_case(void)
{
    pthread_mutexattr_t attr;

    char *casename = "posix_pthread_mutexattr_gettype_case";
    int   flag     = 0;
    int   type     = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    attr.type = PTHREAD_MUTEX_ERRORCHECK;
    pthread_mutexattr_gettype(&attr, &type);

    if (type == PTHREAD_MUTEX_ERRORCHECK) {
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

/*************************** test pthread_mutexattr_setprotocol *****************************/
void posix_pthread_mutexattr_setprotocol_case(void)
{
    pthread_mutexattr_t attr;

    char *casename = "posix_pthread_mutexattr_setprotocol_case";
    int   flag     = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);

    if (attr.protocol == PTHREAD_PRIO_INHERIT) {
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

/*************************** test pthread_mutexattr_getprotocol *****************************/
void posix_pthread_mutexattr_getprotocol_case(void)
{
    pthread_mutexattr_t attr;

    char *casename = "posix_pthread_mutexattr_getprotocol_case";
    int   flag     = 0;
    int   protocol = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    attr.protocol = PTHREAD_PRIO_INHERIT;
    pthread_mutexattr_getprotocol(&attr, &protocol);

    if (protocol == PTHREAD_PRIO_INHERIT) {
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

/*************************** test pthread_mutex_setprioceiling *****************************/
void posix_pthread_mutex_setprioceiling_case(void)
{
    pthread_mutexattr_t attr;

    char *casename        = "posix_pthread_mutex_setprioceiling_case";
    int   flag            = 0;
    int   old_prioceiling = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    pthread_mutex_init(&mutex, &attr);
    pthread_mutex_setprioceiling(&mutex, POSIX_HIGH_PRI, &old_prioceiling);
    pthread_mutex_destroy(&mutex);

    if (mutex.attr->prioceiling == POSIX_HIGH_PRI) {
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

/*************************** test pthread_mutex_getprioceiling *****************************/
void posix_pthread_mutex_getprioceiling_case(void)
{
    pthread_mutexattr_t attr;

    char *casename    = "posix_pthread_mutex_getprioceiling_case";
    int   flag        = 0;
    int   prioceiling = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    attr.prioceiling = POSIX_HIGH_PRI;

    pthread_mutex_init(&mutex, &attr);
    pthread_mutex_getprioceiling(&mutex, &prioceiling);
    pthread_mutex_destroy(&mutex);

    if (prioceiling == POSIX_HIGH_PRI) {
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

/*************************** test pthread_mutexattr_setprioceiling *****************************/
void posix_pthread_mutexattr_setprioceiling_case(void)
{
    pthread_mutexattr_t attr;

    char *casename = "posix_pthread_mutexattr_setprioceiling_case";
    int   flag     = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    pthread_mutexattr_setprioceiling(&attr, POSIX_HIGH_PRI);

    if (attr.prioceiling == POSIX_HIGH_PRI) {
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

/*************************** test pthread_mutexattr_getprioceiling *****************************/
void posix_pthread_mutexattr_getprioceiling_case(void)
{
    pthread_mutexattr_t attr;

    char *casename    = "posix_pthread_mutexattr_getprioceiling_case";
    int   flag        = 0;
    int   prioceiling = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    attr.prioceiling = POSIX_HIGH_PRI;
    pthread_mutexattr_getprioceiling(&attr, &prioceiling);

    if (prioceiling == POSIX_HIGH_PRI) {
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

/*************************** test pthread_mutexattr_setpshared *****************************/
void posix_pthread_mutexattr_setpshared_case(void)
{
    pthread_mutexattr_t attr;

    char *casename = "posix_pthread_mutexattr_setpshared_case";
    int   flag     = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE);

    if (attr.pshared == PTHREAD_PROCESS_PRIVATE) {
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

/*************************** test pthread_mutexattr_getpshared *****************************/
void posix_pthread_mutexattr_getpshared_case(void)
{
    pthread_mutexattr_t attr;

    char *casename = "posix_pthread_mutexattr_getpshared_case";
    int   flag     = 0;
    int   pshared  = 0;

    LOGI(TAG, "*********** %s start ***********\n", casename);

    attr.pshared = PTHREAD_PROCESS_PRIVATE;
    pthread_mutexattr_getpshared(&attr, &pshared);

    if (pshared == PTHREAD_PROCESS_PRIVATE) {
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
