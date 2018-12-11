/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "board.h"
#include <aos/aos.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>

/*
main task stask size(byte)
*/
#define AOS_MAIN_TASK_STACK 0x400
/*  For user config
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
*/
static kinit_t kinit = {0, NULL, 1};

extern void board_init(void);

#ifndef AOS_BINS
extern int application_start(int argc, char **argv);
#endif

static void sys_init(void)
{
    /* user code start*/

    /*insert driver to enable irq for example: starting to run tick time.
     drivers to trigger irq is forbidden before aos_start, which will start core schedule.
    */
    /*user_trigger_irq();*/  //for example

    /*aos components init including middleware and protocol and so on !*/
    aos_components_init(&kinit);

    #ifndef AOS_BINS
    application_start(kinit->argc, kinit->argv);  /* jump to app/example entry */
    #endif
}

int main(void)
{
    /*irq initialized is approved here.But irq triggering is forbidden, which will enter CPU scheduling.
    Put them in sys_init which will be called after aos_start.
    Irq for task schedule should be enabled here, such as PendSV for cortex-M4.
    */
    board_init();   //including aos_heap_set(); hal_init(); flash_partition_init();

    /*General, users should call aos API but not krhino function API directly link krhino_init.
    If user mode and kernel mode are seperated when using SVC, you should use krhino API here, because the system firstly get into kernel mode.
    */
    aos_init();  //kernel init, malloc can use after this!
    aos_task_new("main_task",sys_init, NULL, AOS_MAIN_TASK_STACK);  /*main task to run */

    aos_start();//kernel start schedule!

    /*never run here*/
    return 0;
}

