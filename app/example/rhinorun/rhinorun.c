/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#define DEMO_TASK_STACKSIZE    256 //256*cpu_stack_t = 1024byte
#define DEMO_TASK_PRIORITY     20

extern void stm32_soc_init(void);
static ktask_t demo_task_obj;
cpu_stack_t demo_task_buf[DEMO_TASK_STACKSIZE];

void demo_task(void *arg)
{
#ifndef CONFIG_AOS_CLI
    int count = 0;
#endif
    //uart init 
    stm32_soc_init();

    printf("demo_task here!\n");
    printf("rhino memory is %d!\n", krhino_global_space_get());

    while (1)
    {
#ifndef CONFIG_AOS_CLI
        printf("hello world! count %d\n", count++);
#endif
        //sleep 1 second
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    };
}

int main(void)
{
    krhino_init();
   
#ifdef CONFIG_AOS_CLI
    extern int aos_cli_init(void);
    aos_cli_init();
#endif

    krhino_task_create(&demo_task_obj, "demo_task", 0,DEMO_TASK_PRIORITY, 
        50, demo_task_buf, DEMO_TASK_STACKSIZE, demo_task, 1);
    
    krhino_start();
    
    return 0;
}

