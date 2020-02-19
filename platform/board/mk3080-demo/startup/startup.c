/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#include "aos/init.h"
#include "aos/kernel.h"

/*
main task stask size(byte)
*/
#ifndef AOS_MAIN_TASK_STACK_SIZE
#define AOS_MAIN_TASK_STACK_SIZE (2048 * 4)
#endif

extern void board_basic_init();
extern void aos_maintask(void* arg);
int main(void)
{
    /* board basic init: CLK, heap, define in board\aaboard_demo\startup\board.c */
    board_basic_init();

    /* kernel init, malloc can use after this! */
    aos_init();

    /* main task to run */
    aos_task_new("main_task", aos_maintask, NULL ,AOS_MAIN_TASK_STACK_SIZE);

    /* kernel start schedule! */
    aos_start();

    /* never run here */
    return 0;
}
