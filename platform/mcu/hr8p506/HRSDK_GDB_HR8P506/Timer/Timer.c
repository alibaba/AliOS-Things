/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include "lib_config.h"

/*--define ---------------------------------------------------*/
#define DEMO_TASK_STACKSIZE    512 //512*cpu_stack_t = 2048byte
#define TASK_PRIORITY     20

/*--typedef ---------------------------------------------------*/
extern void sdk_init(void);
void timer_printf(void *timer, void *arg);
void start_task(void *arg);

cpu_stack_t start_task_buf[DEMO_TASK_STACKSIZE];

ktask_t start_task_obj;
ktimer_t timer_obj;

/*--function -------------------------------------------------*/

/**
  * @brief main
  */
int main(void)
{
	/*硬件初始化*/
	sdk_init();
	/*初始化alios*/
	krhino_init();
	/*创建初始任务*/
	krhino_task_create(&start_task_obj,  //start_task对象 控制块
						"start_task",    //名称
						0,               //start_task输入参数
						TASK_PRIORITY,   //优先级
						50,              //时间片
						start_task_buf,  //任务堆栈地址
						DEMO_TASK_STACKSIZE, //任务堆栈大小
						start_task,      //start_task入口地址
						1);              //autorun
	/*alios start*/
	krhino_start();    
	return 0;
}

/**
  * @brief start_task
  */
void start_task(void *arg)
{
	/*创建定时器*/
	krhino_timer_create(&timer_obj,
						"timer",
						(timer_cb_t)timer_printf,
						RHINO_CONFIG_TICKS_PER_SECOND,     //1s
						RHINO_CONFIG_TICKS_PER_SECOND*2,   //2s
						0,
						0);
	/*开始计时*/
	krhino_timer_start(&timer_obj);
	/*删除初始任务*/
	krhino_task_del(&start_task_obj);
	
	while(1){
		printf("start_task del fail!\n\r");
	}
}

/**
  * @brief timer_printf
  */
static int count;
void timer_printf(void *timer, void *arg)
{
	if(timer == &timer_obj){
		count++;
		printf("timer run\n\r");
		printf("the value of count is:%d \n\r",count);
		krhino_timer_start(&timer_obj);
	}
}

