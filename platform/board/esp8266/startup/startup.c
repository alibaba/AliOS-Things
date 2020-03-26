/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>

extern void aos_maintask(void* arg);
/*
main task stask size(cpu_stack_t)
*/
#ifndef AOS_MAIN_TASK_STACK_SIZE
#define AOS_MAIN_TASK_STACK_SIZE (6*1024/4)
#endif

/*
main task pri
*/
#define AOS_MAIN_TASK_PRI 32

static ktask_t *g_main_task;

/*
aos_init\aos_start has been called in boot.
user_init is in a task also called in boot.
*/
int user_init(void)
{
    //board_init();
    //board_wifi_init();

    /* main task to run */
    krhino_task_dyn_create(&g_main_task, "main_task", 0, AOS_MAIN_TASK_PRI, 0,
                            AOS_MAIN_TASK_STACK_SIZE, (task_entry_t)aos_maintask, 1);

    return 0;
}
