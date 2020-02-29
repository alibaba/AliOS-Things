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
main task stask size(byte)
*/
#ifndef AOS_MAIN_TASK_STACK_SIZE
#define AOS_MAIN_TASK_STACK_SIZE (1536 * 4)
#endif

/*
aos_init\aos_start has been called in boot.
user_init is in a task also called in boot.
*/
int user_init(void)
{
    //board_init();
    //board_wifi_init();

    /* main task to run */
    aos_task_new("main_task", aos_maintask, NULL ,AOS_MAIN_TASK_STACK_SIZE);

    return 0;
}
