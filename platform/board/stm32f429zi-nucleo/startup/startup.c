/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>

extern void board_basic_init(void);
extern void aos_maintask(void* arg);
/*
main task stask size(cpu_stack_t)
*/
#ifndef AOS_MAIN_TASK_STACK_SIZE
#define AOS_MAIN_TASK_STACK_SIZE (1536)
#endif
/*
main task pri
*/
#define AOS_MAIN_TASK_PRI 32

static ktask_t *g_main_task;

int main(void)
{
    /* board basic init: CLK, heap, define in board\aaboard_demo\startup\board.c */
    board_basic_init();

    /* kernel init, malloc can use after this! */
    krhino_init();

    /* main task to run */
    krhino_task_dyn_create(&g_main_task, "main_task", 0, AOS_MAIN_TASK_PRI, 0,
                            AOS_MAIN_TASK_STACK_SIZE, (task_entry_t)aos_maintask, 1);

    /* kernel start schedule! */
    krhino_start();

    /* never run here */
    return 0;
}
