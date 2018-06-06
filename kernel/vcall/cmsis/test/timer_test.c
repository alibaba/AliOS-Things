/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmsis_os.h>

#define DEMO_TASK_STACKSIZE    256 //256*cpu_stack_t = 1024byte
#define DEMO_TASK_PRIORITY     20

static ktask_t demo_task_obj;
static cpu_stack_t demo_task_buf[DEMO_TASK_STACKSIZE];
static osThreadDef_t thread;
static osThreadId thread_id;

static osTimerDef_t timer_def;
static ktimer_t timer_space;

static int timer_cb_count = 0;
static void timer_function(void)
{
    timer_cb_count ++;
    printf("timer cb is called, timer_cb_count = %d\n", timer_cb_count);
}

static void demo_task(void *arg)
{
    int count = 0;
    osTimerId pTimerId = NULL;
    osStatus ret = 0;

    timer_def.name = "testTimer";
    timer_def.cb   = (os_ptimer)timer_function;
    timer_def.timer = &timer_space;

    pTimerId = osTimerCreate (&timer_def, osTimerPeriodic, NULL);
    if (pTimerId == NULL) {
        printf("osTimerCreate failed, MAX_TIMER_TICKS = %d\n", MAX_TIMER_TICKS);
    } else {
        printf("osTimerCreate ok\n");
    }

    ret = osTimerStart (pTimerId, 1000);
    if (ret == 0) {
        printf("os Timer Start ok\n");
    }

    while (1) {
        printf("hello world! count  %d\n", count++);

        //sleep 1 second
        osDelay(1000);

        if (timer_cb_count == 10) {
            ret = osTimerStop(pTimerId);
            if (ret == 0) {
                printf("Timer stop OK\n");
                osThreadTerminate(thread_id);
            }
        }
    };
}


void cmsis_timer_test(void)
{
    thread.name     = "demo_task";
    thread.pthread  = (os_pthread)demo_task;
    thread.tpriority = osPriorityNormal;
    thread.stacksize = DEMO_TASK_STACKSIZE;
    thread.ptcb     = &demo_task_obj;
    thread.pstackspace = demo_task_buf;

    thread_id = osThreadCreate (&thread, NULL);
}

