/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmsis_os.h>

#define DEMO_TASK_STACKSIZE    256 //256*cpu_stack_t = 1024byte
#define DEMO_TASK_PRIORITY     20

static ktask_t          init_task_obj;
static ktask_t          demo_task_obj1;
static ktask_t          demo_task_obj2;
static cpu_stack_t      init_task_buf[DEMO_TASK_STACKSIZE];
static cpu_stack_t      demo_task_buf1[DEMO_TASK_STACKSIZE];
static cpu_stack_t      demo_task_buf2[DEMO_TASK_STACKSIZE];

static osThreadDef_t    thread_init;
static osThreadDef_t    thread1;
static osThreadDef_t    thread2;

static kbuf_queue_t     buf_queue;
static osMessageQDef_t  msg_queue_def;
static osMessageQId     p_msgqueue;
static uint32_t         msg_buf_space[32];

static void init_task(void *arg)
{
    printf("init_task here!\n");

    msg_queue_def.name      = "buf_queue";
    msg_queue_def.queue     = &buf_queue;
    msg_queue_def.queue_sz  = 32;
    msg_queue_def.item_sz   = 4;
    msg_queue_def.pool      = msg_buf_space;

    p_msgqueue = osMessageCreate (&msg_queue_def, (osThreadId)NULL);
    if (p_msgqueue == NULL) {
        printf("osMessageCreate failed\n");
    } else {
        printf("osMessageCreate ok\n");
    }
}

static void demo_task1(void *arg)
{
    osEvent event;

    while (1) {
        event = osMessageGet(p_msgqueue, osWaitForever);
        if (event.status == osEventMessage) {
            printf("demo_task1 get msg %d\n", event.value.v);
        } else {
            printf("demo_task1 get msg failed\n");
        }
    };
}

static void demo_task2(void *arg)
{
    uint32_t count = 0;

    while (1) {
        osMessagePut(p_msgqueue, count, 0xffffffff);

        printf("demo_task2 put msg %d\n", count++);
        osDelay(1000);
    };
}

void cmsis_msgqueue_test(void)
{
    thread_init.name        = "init_task";
    thread_init.pthread     = (os_pthread)init_task;
    thread_init.tpriority   = osPriorityHigh;
    thread_init.stacksize   = DEMO_TASK_STACKSIZE;
    thread_init.ptcb        = &init_task_obj;
    thread_init.pstackspace = init_task_buf;

    thread1.name            = "demo_task1";
    thread1.pthread         = (os_pthread)demo_task1;
    thread1.tpriority       = osPriorityNormal;
    thread1.stacksize       = DEMO_TASK_STACKSIZE;
    thread1.ptcb            = &demo_task_obj1;
    thread1.pstackspace     = demo_task_buf1;

    thread2.name            = "demo_task2";
    thread2.pthread         = (os_pthread)demo_task2;
    thread2.tpriority       = osPriorityNormal;
    thread2.stacksize       = DEMO_TASK_STACKSIZE;
    thread2.ptcb            = &demo_task_obj2;
    thread2.pstackspace     = demo_task_buf2;

    osThreadCreate (&thread_init, NULL);
    osThreadCreate (&thread1, NULL);
    osThreadCreate (&thread2, NULL);
}

