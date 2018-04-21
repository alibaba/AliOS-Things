/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static pthread_mutex_t           mutex;

static void *init_task(void *arg)
{  
    pthread_mutex_init(&mutex, NULL);
    

    return NULL;
}

static void *demo_task1(void *arg)
{
    int count = 0;

    while (1)
    {
        pthread_mutex_lock(&mutex);
        printf("demo_task1 get sem %d\n", count++);
    };

    return NULL;
}

static void *demo_task2(void *arg)
{
    int count = 0;  

    while (1)
    {
        pthread_mutex_unlock(&mutex);

        printf("demo_task2 release sem %d\n", count++);      
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    };

    return NULL;
}

void posix_test_case3(void)
{    
    pthread_t thread;  

    pthread_create(&thread, NULL, init_task, NULL);
    pthread_create(&thread, NULL, demo_task1, NULL);
    pthread_create(&thread, NULL, demo_task2, NULL);
}



