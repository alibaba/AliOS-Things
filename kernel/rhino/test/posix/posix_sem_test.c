/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

static sem_t           sem;

static void *init_task(void *arg)
{  
    sem_init(&sem, 0, 0);

    return NULL;
}

static void *demo_task1(void *arg)
{
    int count = 0;

    while (1)
    {
        sem_wait(&sem);
        printf("demo_task1 get sem %d\n", count++);
    };

    return NULL;
}

static void *demo_task2(void *arg)
{
    int count = 0;  

    while (1)
    {
        sem_post(&sem);

        printf("demo_task2 release sem %d\n", count++);      
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    };

    return NULL;
}

void posix_test_case2(void)
{    
    pthread_t thread;  

    pthread_create(&thread, NULL, init_task, NULL);
    pthread_create(&thread, NULL, demo_task1, NULL);
    pthread_create(&thread, NULL, demo_task2, NULL);
}


