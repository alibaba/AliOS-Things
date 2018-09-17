/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmsis_os.h>

#define APP_TASK_STACKSIZE    1024 //1024*cpu_stack_t = 4096
#define APP_TASK_PRIORITY     20

extern void stm32_soc_init(void);
static ktask_t app_task_obj;
static cpu_stack_t app_task_buf[APP_TASK_STACKSIZE];
static osThreadDef_t thread;

extern void cmsis_thread_test(void);
extern void cmsis_mutex_test(void);
extern void cmsis_sem_test(void);
extern void cmsis_msgqueue_test(void);
extern void cmsis_timer_test(void);

static void app_task(void *arg)
{
    stm32_soc_init();

    cmsis_mutex_test();
}

int main(void)
{
    osKernelInitialize();

    thread.name         = "app_task";
    thread.pthread      = (os_pthread)app_task;
    thread.tpriority    = osPriorityNormal;
    thread.stacksize    = APP_TASK_STACKSIZE;
    thread.ptcb         = &app_task_obj;
    thread.pstackspace  = app_task_buf;

    osThreadCreate (&thread, NULL);

    osKernelStart();

    return 0;
}

