/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

/* 200 * cpu_stack_t = 400byte */
#define DEMO_TASK_STACKSIZE     200
#define DEMO_TASK_PRIORITY1     20
#define DEMO_TASK_PRIORITY2     21

extern void tick_init(uint32_t ticks_per_sec);

static ktask_t      demo_task_obj1;
static ktask_t      demo_task_obj2;

static cpu_stack_t  demo_task_buf1[DEMO_TASK_STACKSIZE];
static cpu_stack_t  demo_task_buf2[DEMO_TASK_STACKSIZE];

static uint32_t     tst_cnt;
static uint32_t     tmr_cnt;

static ksem_t       sem_tst;
static ktimer_t     timer_test;
static kbuf_queue_t queue_test;

static char         queue_buf[50];
static char         queue_buf_recv[10];
static size_t       queue_recv_size;

static void timer_cb(void *timer, void *arg)
{
    char queue_buf_recv[10];
    memset(queue_buf_recv, 'a', 10);
    tmr_cnt++;
    krhino_buf_queue_send(&queue_test, queue_buf_recv, 10);
    krhino_timer_stop(&timer_test);
}

void demo_task1(void *arg)
{
    tick_init(RHINO_CONFIG_TICKS_PER_SECOND);
    krhino_sem_create(&sem_tst, "sem_tst", 0);

    krhino_buf_queue_create(&queue_test, "queue_test",
                             queue_buf, 50, 50);

    krhino_timer_create(&timer_test, "timer_test", timer_cb,
                            10, 10, 0, 0);

    krhino_timer_start(&timer_test);

    krhino_buf_queue_recv(&queue_test, RHINO_WAIT_FOREVER, queue_buf_recv,
                          &queue_recv_size);

    while (1)
    {
        krhino_sem_give(&sem_tst);
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    }
}

void demo_task2(void *arg)
{
    while (1)
    {
        tst_cnt++;
        krhino_sem_take(&sem_tst, RHINO_WAIT_FOREVER);
    }
}

int main(void)
{
    krhino_init();

    krhino_task_create(&demo_task_obj1, "demo_task", 0,DEMO_TASK_PRIORITY1, 
        50, demo_task_buf1, DEMO_TASK_STACKSIZE, demo_task1, 1);

    krhino_task_create(&demo_task_obj2, "demo_task2", 0,DEMO_TASK_PRIORITY2, 
        50, demo_task_buf2, DEMO_TASK_STACKSIZE, demo_task2, 1);
    
    krhino_start();
    
    return 0;
}

