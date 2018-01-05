/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "GUIDEMO.h"

#define DEMO_TASK_STACKSIZE    512 //512*cpu_stack_t = 2048byte
#define DEMO_TASK_PRIORITY     20

extern void stm32_soc_init(void);
static ktask_t demo_task_obj;
cpu_stack_t demo_task_buf[DEMO_TASK_STACKSIZE];

void demo_task(void *arg)
{
    int count = 0;
    printf("demo_task here!\n");
	
	  GUIDEMO_Main();
	
    while (1)
    {
        printf("hello world! count %d\n", count++);
        //sleep 1 second
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    };
}

int main(void)
{
    krhino_init();
    krhino_task_create(&demo_task_obj, "demo_task", 0,DEMO_TASK_PRIORITY, 
        50, demo_task_buf, DEMO_TASK_STACKSIZE, demo_task, 1);

    //uart init 
    stm32_soc_init();
    
    krhino_start();
    
    return 0;
}

