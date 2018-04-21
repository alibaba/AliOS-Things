/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static void *demo_task(void *arg)
{
    int count = 0;

    while (1)
    {
        printf("hello world! count  %d\n", count++);

        //sleep 1 second
        
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    };

    return NULL;
}

/* test target : pthread_create() */
void posix_test_case1(void)
{
    pthread_t thread;

    pthread_create(&thread, NULL, demo_task, NULL);
}

