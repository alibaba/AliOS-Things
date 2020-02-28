/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include <aos/kernel.h>
#include <ulog/ulog.h>

/**
 * This use case shows how to use task private variables.
 * In the use case, a user private variable named task_loop_count is created
 * to record the number of loops per task. Task private variables are kept
 * in the task's private memory area, so each task does not overwrite each other
 * when accessing the private variables.
 */

/* module name used by ulog */
#define MODULE_NAME "task_key"

/* task parameters */
#define TASK1_NAME      "task1"
#define TASK2_NAME      "task2"
#define TASK1_STACKSIZE 1024
#define TASK2_STACKSIZE 1024

/* task private variable */
aos_task_key_t task_loop_count;

/* task entry */
static void task1_entry(void *arg)
{
    int loop_count = 0;

    /* initializes the task private variable */
    aos_task_setspecific(task_loop_count, (void *)loop_count);

    while (1) {
        /* read the task private variable */
        loop_count = (int)aos_task_getspecific(task_loop_count);

        LOGI(MODULE_NAME, "I'm task2, loop_count=%d", loop_count);

        /* write the task private variable */
        aos_task_setspecific(task_loop_count, (void *)(loop_count + 1));

        /* sleep 1000 ms */
        aos_msleep(1000);
    }
}

/* task entry */
static void task2_entry(void *arg)
{
    int loop_count = 0;

    /* initializes the task private variable */
    aos_task_setspecific(task_loop_count, (void *)loop_count);

    while (1) {
        /* read the task private variable */
        loop_count = (int)aos_task_getspecific(task_loop_count);

        LOGI(MODULE_NAME, "I'm task2, loop_count=%d", loop_count);

        /* write the task private variable */
        aos_task_setspecific(task_loop_count, (void *)(loop_count + 1));

        /* sleep 2000 ms */
        aos_msleep(2000);
    }
}

void task_key(void)
{
    int status;

    /* create a task private variable to record the number of cycles per task */
    status = aos_task_key_create(&task_loop_count);
    if (status != 0) {
        LOGE(MODULE_NAME, "create task key error");
        return;
    }

    /**
     * create task1
     * fn :        task1_entry (the task entry functionn)
     * arg:        NULL (no arguments are passed to the entry function)
     * stack_size: TASK1_STACKSIZE (this parameter depends on the needs of the task using the stack,
     *             an insufficient stack size will cause an exception.)
     */
    status = aos_task_new(TASK1_NAME, task1_entry, NULL, TASK1_STACKSIZE);
    if (status != 0) {
        LOGE(MODULE_NAME, "create task1 error");
        return;
    }

    /**
     * create task2
     * fn :        task2_entry (the task entry function)
     * arg:        NULL (no arguments are passed to the entry function)
     * stack_size: TASK2_STACKSIZE (this parameter depends on the needs of the task using the stack,
     *             an insufficient stack size will cause an exception.)
     */
    status = aos_task_new(TASK2_NAME, task2_entry, NULL, TASK2_STACKSIZE);
    if (status != 0) {
        LOGE(MODULE_NAME, "create task2 error");
        return;
    }
}

