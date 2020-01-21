/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#include "aos/init.h"

extern void board_init(void);

#ifndef AOS_BINS
extern int application_start(int argc, char **argv);
#endif

/* main task stask size(byte) */
#define OS_MAIN_TASK_STACK 0x200
#define OS_MAIN_TASK_PRI 32

static ktask_t g_main_task;
cpu_stack_t g_main_task_buf[OS_MAIN_TASK_STACK];
static kinit_t kinit = {0, NULL, 0};

static void sys_init(void)
{
    /* user code start*/
    board_init();
    /*insert driver to enable irq for example: starting to run tick time.
     drivers to trigger irq is forbidden before aos_start, which will start core schedule.
    */
    /*user_trigger_irq();*/  //for example

    /*aos components init including middleware and protocol and so on !*/
    aos_components_init(&kinit);

    #ifndef AOS_BINS
    application_start(0, NULL);  /* jump to app/example entry */
    #endif
}


int main(void)
{
    /*irq initialized is approved here.But irq triggering is forbidden, which will enter CPU scheduling.
    Put them in sys_init which will be called after aos_start.
    Irq for task schedule should be enabled here, such as PendSV for cortex-M4.
    */
    /*kernel init, malloc can use after this!*/
    krhino_init();

    /*main task to run */
    krhino_task_create(&g_main_task, "main_task", 0, 
                       OS_MAIN_TASK_PRI, 0, 
                       g_main_task_buf, OS_MAIN_TASK_STACK, 
                       (task_entry_t)sys_init, 1);

    /*kernel start schedule!*/
    krhino_start();

    /*never run here*/
    return 0;
}

