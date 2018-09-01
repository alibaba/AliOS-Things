/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "alios_cpp.h"

using namespace AliOS;

#define STACK_BUF_BYTES  2048
#define BUF_QUEUE_BYTES  1024

static Thread *pa; 
static Thread *pb;
static Semaphore *mySem;

static void demo_task1(void *arg)
{
    int count = 0;

    while (1)
    {
        printf("demo_task1 count  %d\n", count++);

        mySem->wait(0xFFFFFFFF);

        printf("demo_task1 get Semaphore\n");

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

        mySem->release();
        printf("demo_task2 release Semaphore\n");

        if (count == 10){
            pb->terminate();
        }

        pb->sleep((uint32_t)1000);
    };
}

void test_semaphore(void)
{
    pa = new Thread;
    pb = new Thread;
    mySem = new Semaphore;

    mySem->create("sem", 0);

    pa->create("task", (void *)NULL, (uint8_t)20, (tick_t)50, 
              STACK_BUF_BYTES, demo_task1, (uint8_t)1);

    pb->create("task", (void *)NULL, (uint8_t)20, (tick_t)50, 
              STACK_BUF_BYTES, demo_task2, (uint8_t)1);
}
