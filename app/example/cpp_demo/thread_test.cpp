/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "alios_cpp.h"

using namespace AliOS;

#define STACK_BUF_BYTES  2048

static Thread *pa; 
static Thread *pb;
static Thread *pc;

static void demo_task1(void *arg)
{
    int count = 0;

    while (1)
    {
        printf("demo_task1 count  %d\n", count++);

        pa->sleep((uint32_t)1000);

        if (count == 5){
            pc->start();
        }

        if (count == 15){
            pc->terminate();
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

        if (count == 10){
            pc->stop();
            pb->terminate();
        }

        pb->sleep((uint32_t)1000);
    };
}

static void demo_task3(void *arg)
{
    int count = 0;

    while (1)
    {
        printf("demo_task3 count  %d\n", count++);

        pc->sleep((uint32_t)1000);
    };
}

void test_thread(void)
{
    char *name = "task";
    pa = new Thread;
    pb = new Thread;
    pc = new Thread;

    pa->create((const name_t *)name, (void *)NULL, (uint8_t)20, (tick_t)50, 
      STACK_BUF_BYTES, demo_task1, (uint8_t)1);

    pb->create((const name_t *)name, (void *)NULL, (uint8_t)20, (tick_t)50, 
      STACK_BUF_BYTES, demo_task2, (uint8_t)1);

    pc->create((const name_t *)name, (void *)NULL, (uint8_t)20, (tick_t)50, 
      STACK_BUF_BYTES, demo_task3, (uint8_t)0);
}
