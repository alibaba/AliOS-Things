/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "alios_cpp.h"
#include "aos/kernel.h"
#include "k_api.h"

using namespace AliOS;

#define STACK_BUF_BYTES  2048
char stack_buf1[STACK_BUF_BYTES];

#define BUF_QUEUE_BYTES  1024

static WorkQueue *pWorkQueue; 
static Work *pWork1; 
static Work *pWork2; 

static void demo_task1(void *arg)
{
    int count = 0;

    while (count < 5)
    {
        printf("demo_task1 count  %d\n", count++);

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    };
}

static void demo_task2(void *arg)
{
    int count = 0;

    while (count < 5)
    {
        printf("demo_task2 count  %d\n", count++);

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    };
}

void test_workqueue(void)
{
    pWorkQueue = new WorkQueue;
    pWork1 = new Work;
    pWork2 = new Work;

    pWorkQueue->create("task", 30, (cpu_stack_t *)stack_buf1, STACK_BUF_BYTES);

    pWork1->init(demo_task1, NULL, 0);
    pWork2->init(demo_task2, NULL, 0);

    pWork1->run(pWorkQueue->self());
    pWork2->run(pWorkQueue->self());
}
