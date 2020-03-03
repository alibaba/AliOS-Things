/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include <aos/kernel.h>
#include <ulog/ulog.h>

/**
 * This use case demonstrates task creation using the aos_task_new() interface.
 */

/* module name used by ulog */
#define MODULE_NAME "task_new"

/* task parameters */
#define TASK1_NAME      "task1"
#define TASK2_NAME      "task2"
#define TASK1_STACKSIZE 1024
#define TASK2_STACKSIZE 1024

/* task entry */
static void task1_entry(void *arg)
{
    int i;

    /* Parameters are passed through the arg, used to control the number of loops */
    int count = (int)arg;

    for (i = 0; i < count; i++) {
        LOGI(MODULE_NAME, "I'm task1");

        /* sleep 1000 ms */
        aos_msleep(1000);
    }

    /* tasks are automatically deleted after the entry function completes */
}

/* task entry */
static void task2_entry(void *arg)
{
    while (1) {
        LOGI(MODULE_NAME, "I'm task2");

        /* sleep 1000 ms */
        aos_msleep(1000);
    }
}

void task_new(void)
{
    int status;

    /**
     * create task1
     * fn :        task1_entry (the task entry function)
     * arg:        20 (This parameter is passed to the task entry function)
     * stack_size: TASK1_STACKSIZE (this parameter depends on the needs of the task using the stack,
     *             an insufficient stack size will cause an exception.)
     */
    status = aos_task_new(TASK1_NAME, task1_entry, (void *)20, TASK1_STACKSIZE);
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

