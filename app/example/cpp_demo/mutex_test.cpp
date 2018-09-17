/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "alios_cpp.h"
#include "aos/kernel.h"

using namespace AliOS;

#define STACK_BUF_BYTES  2048
#define BUF_QUEUE_BYTES  1024

static Thread *pa; 
static Thread *pb;
static Mutex *myMutex;

static void demo_task1(void *arg)
{
    int count = 0;
    long long time_start = 0;
    long long time_cur = 0;

    while (1)
    {
        printf("demo_task1 count  %d\n", count++);

        myMutex->lock(0xFFFFFFFF);
        printf("demo_task1 lock\n");

        time_start = aos_now_ms();

        do {
            time_cur = aos_now_ms();
        }while(time_cur <= (time_start + 3000));

        myMutex->unlock();
        printf("demo_task1 unlock\n");

        pa->sleep(1000);

        if (count == 10){
            pa->terminate();
        }
    };
}

static void demo_task2(void *arg)
{
    int count = 0;

    while (1)
    {
        printf("demo_task2 count  %d\n", count++);

        myMutex->lock(0xFFFFFFFF);
        printf("demo_task2 lock\n");

        myMutex->unlock();
        printf("demo_task2 unlock\n");

        pb->sleep(1000);

        if (count == 10){
            pb->terminate();
        }
    };
}

void test_mutex(void)
{
    pa = new Thread;
    pb = new Thread;
    myMutex = new Mutex;

    myMutex->create("mutex");

    pa->create("task", (void *)NULL, (uint8_t)20, (tick_t)50, 
              STACK_BUF_BYTES, demo_task1, (uint8_t)1);

    pb->create("task", (void *)NULL, (uint8_t)20, (tick_t)50, 
              STACK_BUF_BYTES, demo_task2, (uint8_t)1);
}
