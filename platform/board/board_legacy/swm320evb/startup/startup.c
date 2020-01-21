/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "aos/kernel.h"
#include "aos/init.h"

#include "board.h"

#include "k_api.h"

/*
main task stask size(byte)
*/
#define OS_MAIN_TASK_STACK 0x400
#define OS_MAIN_TASK_PRI 32

#define AOS_START_STACK 1536

static ktask_t demo_task_obj;
cpu_stack_t demo_task_buf[AOS_START_STACK];
static kinit_t kinit = {0, NULL, 1};
static ktask_t *g_main_task;

extern void board_init(void);
extern int aos_components_init(kinit_t *kinit);


static void sys_init(void)
{
    /* user code start*/

    /*insert driver to enable irq for example: starting to run tick time.
     drivers to trigger irq is forbidden before aos_start, which will start core schedule.
    */
    /*user_trigger_irq();*/  //for example

    /*aos components init including middleware and protocol and so on
    jump to app entry: application_start !*/

#ifdef AOS_CPLUSPLUS
    cpp_init();
#endif
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
    board_init();   //including aos_heap_set();  flash_partition_init();

    /*kernel init, malloc can use after this!*/
    aos_init();

    /*main task to run */
    krhino_task_dyn_create(&g_main_task, "main_task", 0, OS_MAIN_TASK_PRI, 0, OS_MAIN_TASK_STACK, (task_entry_t)sys_init, 1);

    /*kernel start schedule!*/
    aos_start();
    /*never run here*/
    return 0;
}

