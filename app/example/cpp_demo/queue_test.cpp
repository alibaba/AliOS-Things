/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "alios_cpp.h"

using namespace AliOS;

#define STACK_BUF_BYTES  2048
#define BUF_QUEUE_BYTES  1024
static char myQueue_buff[BUF_QUEUE_BYTES];

static Thread *pa; 
static Thread *pb;
static Queue *myQueue;

static void demo_task1(void *arg)
{
    int count = 0;
    int rx_data = 0;
    size_t rx_size = 0;

    while (1)
    {
        printf("demo_task1 count  %d\n", count++);

        myQueue->receive(&rx_data, &rx_size, 0xFFFFFFFF);

        if (rx_size == sizeof(int)){
            printf("demo_task2 received message: %d\n", rx_data);
        }

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

        myQueue->send(&count, sizeof(int));
        printf("demo_task2 send message : %d\n", count);

        if (count == 10){
            pb->terminate();
        }

        pb->sleep((uint32_t)1000);
    };
}

void test_queue(void)
{
    pa = new Thread;
    pb = new Thread;
    myQueue = new Queue;

    myQueue->create("queue", myQueue_buff, BUF_QUEUE_BYTES, sizeof(int));

    pa->create("task", (void *)NULL, (uint8_t)20, (tick_t)50, 
              STACK_BUF_BYTES, demo_task1, (uint8_t)1);

    pb->create("task", (void *)NULL, (uint8_t)20, (tick_t)50, 
              STACK_BUF_BYTES, demo_task2, (uint8_t)1);
}
