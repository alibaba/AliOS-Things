/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include "lib_config.h"

extern void sdk_init(void);

#define DEMO_TASK_STACKSIZE    512 //512*cpu_stack_t = 2048byte
#define DEMO_TASK_PRIORITY     20

static ktask_t task1_obj;
static ktask_t task2_obj;


cpu_stack_t task1_buf[DEMO_TASK_STACKSIZE];
cpu_stack_t task2_buf[DEMO_TASK_STACKSIZE];

void task1(void *arg)
{
    int count = 0;
    UART_printf("task1 here!\n\r");
    
    while (1)
    {
        UART_printf("hello world1! count %d\n\r", count++);
        //sleep 1 second
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    };
}

void task2(void *arg)
{
    int count = 0;
    UART_printf("task2 here!\n\r");
    
    while (1)
    {
        UART_printf("hello world2! count %d\n\r", count++);
        //sleep 1 second
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    };
}

int main(void)
{
	//uart init 
    sdk_init(); 
    krhino_init();
    krhino_task_create( &task1_obj, "task1",  0, DEMO_TASK_PRIORITY,                 
                        50, task1_buf, DEMO_TASK_STACKSIZE, task1, 1); 
	krhino_task_create( &task2_obj , "task2" ,0 , DEMO_TASK_PRIORITY,
						50, task2_buf , DEMO_TASK_STACKSIZE,task2 , 1); 
    krhino_start();    
    return 0;
}
