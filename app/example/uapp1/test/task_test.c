/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <task.h>
#include <systime.h>

#define LOOP_ROUND 50
#define STACK_SIZE 0x400

static task_t child_task;

static int child_task_cnt;

static void child_task_func(void *arg)
{
    child_task_cnt += 1;

    task_exit();
}

int task_test(void)
{
    int stat;
    int i;

    child_task_cnt = 0;

    for (i = 0; i < LOOP_ROUND; i++) {
        stat = task_create(&child_task, STACK_SIZE, child_task_func, NULL);
        if (stat != 0) {
            printf("%s create mutex_test task failed, ret code %d\r\n",
                    __func__, stat);
            goto out;
        }

        msleep(1);
    }

    /* wait all child tasks exit */
    msleep(10);

out:
    return (child_task_cnt == LOOP_ROUND)? 0: -1;
}

