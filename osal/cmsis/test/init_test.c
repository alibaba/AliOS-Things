/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmsis_os.h>

#define DEMO_TASK_STACKSIZE    256 //256*cpu_stack_t = 1024byte
#define DEMO_TASK_PRIORITY     20

extern void stm32_soc_init(void);
static ktask_t demo_task_obj;
static cpu_stack_t demo_task_buf[DEMO_TASK_STACKSIZE];
static osThreadDef_t thread;

static void demo_task(void *arg)
{
    int count = 0;

    stm32_soc_init();

    printf("demo_task here!\n");
    printf("rhino memory is %d!\n", krhino_global_space_get());

    while (1)
    {
        printf("hello world! count  %d\n", count++);

        //sleep 1 second
        
        osDelay(RHINO_CONFIG_TICKS_PER_SECOND*10000);
    };
}

int main(void)
{
    osKernelInitialize();

    thread.name         = "demo_task";
    thread.pthread      = (os_pthread)demo_task;
    thread.tpriority    = osPriorityNormal;
    thread.stacksize    = DEMO_TASK_STACKSIZE;
    thread.ptcb         = &demo_task_obj;
    thread.pstackspace  = demo_task_buf;

    osThreadCreate (&thread, NULL);

    osKernelStart();

    return 0;
}

