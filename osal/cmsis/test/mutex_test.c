/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmsis_os.h>

#define DEMO_TASK_STACKSIZE    256 //256*cpu_stack_t = 1024byte
#define DEMO_TASK_PRIORITY     20

static ktask_t init_task_obj;
static ktask_t demo_task_obj1;
static ktask_t demo_task_obj2;
static cpu_stack_t init_task_buf[DEMO_TASK_STACKSIZE];
static cpu_stack_t demo_task_buf1[DEMO_TASK_STACKSIZE];
static cpu_stack_t demo_task_buf2[DEMO_TASK_STACKSIZE];

static osThreadDef_t thread_init;
static osThreadDef_t thread1;
static osThreadDef_t thread2;

static kmutex_t     mutex;
static osMutexDef_t mutex_def;
static osMutexId    pMutex;
static int mutex_count = 0;

static void init_task(void *arg)
{
    mutex_def.name = "mutex";
    mutex_def.mutex = &mutex;

    pMutex = osMutexCreate (&mutex_def);
    if (pMutex == NULL) {
        printf("osMutexCreate failed\n");
    } else {
        printf("osMutexCreate ok\n");
    }
}

static void demo_task1(void *arg)
{
    int count = 0;

    while (1) {
        printf("demo_task1 count %d\n", count++);

        osMutexWait(pMutex, osWaitForever);
        printf("demo_task1 get mutex\n");

        printf("demo_task1 release mutex\n");
        osMutexRelease(pMutex);

        osDelay(300);
    };
}

static void demo_task2(void *arg)
{
    int count = 0;
    long i = 0;
    long j = 0;

    while (1) {
        printf("demo_task2 count %d\n", count++);

        osMutexWait(pMutex, osWaitForever);
        printf("demo_task2 get mutex\n");

        for (i = 0; i < 0x1000; i++) {
            for (j = 0; j < 0x8000; j++) {
                ;
            }
        }

        printf("demo_task2 release mutex\n");
        osMutexRelease(pMutex);
    };
}

void cmsis_mutex_test(void)
{
    thread_init.name     = "init_task";
    thread_init.pthread  = (os_pthread)init_task;
    thread_init.tpriority = osPriorityHigh;
    thread_init.stacksize = DEMO_TASK_STACKSIZE;
    thread_init.ptcb     = &init_task_obj;
    thread_init.pstackspace = init_task_buf;

    thread1.name     = "demo_task1";
    thread1.pthread  = (os_pthread)demo_task1;
    thread1.tpriority = osPriorityRealtime;
    thread1.stacksize = DEMO_TASK_STACKSIZE;
    thread1.ptcb     = &demo_task_obj1;
    thread1.pstackspace = demo_task_buf1;

    thread2.name     = "demo_task2";
    thread2.pthread  = (os_pthread)demo_task2;
    thread2.tpriority = osPriorityNormal;
    thread2.stacksize = DEMO_TASK_STACKSIZE;
    thread2.ptcb     = &demo_task_obj2;
    thread2.pstackspace = demo_task_buf2;

    osThreadCreate (&thread_init, NULL);
    osThreadCreate (&thread1, NULL);
    osThreadCreate (&thread2, NULL);
}

