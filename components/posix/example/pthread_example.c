/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <pthread.h>

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

static int count = 0;
static pthread_mutex_t count_lock = PTHREAD_MUTEX_INITIALIZER;
static int ret_value = 0;

static void *new_thread_func(void *arg)
{
    int increase_count = *((int *)arg);

    printf("new thread:0x%x, arg:%d\n", pthread_self(), increase_count);

    pthread_mutex_lock(&count_lock);
    printf("new thread hold the lock.\n");
    count += increase_count;
    pthread_mutex_unlock(&count_lock);

    ret_value = 100;
    pthread_exit(&ret_value);
}

static int pthread_example(int argc, char **argv)
{
    int ret = 0;
    int increase_count;
    pthread_t new_thread;
    pthread_attr_t attr;
    int *retval;

    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 1024);
    increase_count = 10;
    ret = pthread_create(&new_thread, &attr, new_thread_func, &increase_count);
    if (ret != 0) {
        printf("pthread_create failed, ret:%d\n", ret);
        pthread_attr_destroy(&attr);
        return -1;
    }

    pthread_mutex_lock(&count_lock);
    printf("Main thread hold the lock.\n");
    count++;
    pthread_mutex_unlock(&count_lock);

    ret = pthread_join(new_thread, (void **)&retval);
    if (ret != 0) {
        printf("pthread_join return :%d\n", ret);
    }
    printf("retval:%p, &ret_value:%p\n", retval, &ret_value);
    printf("New thread:0x%x exited with vaule: %d\n", new_thread, *retval);
    printf("The count is %d\n", count);
    if ((*retval == 100) && (count == 11)) {
        printf("pthread_example test success!\n");
    } else {
        printf("pthread_example test fail!\n");
    }

    return 0;
}


#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(pthread_example, pthread_example, pthread example)
#endif

