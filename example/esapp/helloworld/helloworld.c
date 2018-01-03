/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>

extern void sdk_init(void);

#define DEMO_TASK_STACKSIZE    512 //512*cpu_stack_t = 2048byte
#define DEMO_TASK_PRIORITY     20
static ktask_t demo_task_obj;
cpu_stack_t demo_task_buf[DEMO_TASK_STACKSIZE];

void demo_task(void *arg)
{
    int count = 0;
    printf("demo_task here!\n\r");
    
    while (1)
    {
        printf("hello world! count %d\n\r", count++);
        //sleep 1 second
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    };
}

int main(void)
{
    krhino_init();
    krhino_task_create( &demo_task_obj,             //任务对象
                        "demo_task",                //任务名字
                        0,                          //传入任务的参数，没有参数传入则为0
                        DEMO_TASK_PRIORITY,         //任务优先级    
                        50,                         //任务时间片
                        demo_task_buf,              //任务栈的基地址
                        DEMO_TASK_STACKSIZE,        //任务栈的大小
                        demo_task,                  //任务的执行函数
                        1                           //任务进入就绪状态
                        );
    //uart init 
    sdk_init(); 
    
    krhino_start();
    
    return 0;
}

