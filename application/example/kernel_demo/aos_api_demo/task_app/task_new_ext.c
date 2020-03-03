/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include <aos/kernel.h>
#include <ulog/ulog.h>

/**
 * This use case demonstrates task creation using the aos_task_new_ext() interface.
 * This interface can specify task priorities as opposed to the aos_task_new() interface.
 */

/* module name used by ulog */
#define MODULE_NAME "task_new_ext"

/* task parameters */
#define TASK1_NAME      "task1"
#define TASK2_NAME      "task2"
#define TASK1_STACKSIZE 1024
#define TASK2_STACKSIZE 1024
/* Task priority must be less than or equal to RHINO_CONFIG_USER_PRI_MAX */
#define TASK1_PRI       50
#define TASK2_PRI       50

/* task handle */
aos_task_t task1_handle;
aos_task_t task2_handle;

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

void task_new_ext(void)
{
    int status;

    /**
     * create task1
     * task:       task1_handle (user needs to define a task handle variable)
     * fn :        task1_entry (the task entry function)
     * arg:        20 (This parameter is passed to the task entry function)
     * stack_size: TASK1_STACKSIZE (this parameter depends on the needs of the task using the stack,
     *             an insufficient stack size will cause an exception.)
     * prio:       TASK1_PRI (Priority is a relative value, so the user sets the priority value for
     *             a particular task based on other tasks throughout the application.)
     */
    status = aos_task_new_ext(&task1_handle, TASK1_NAME, task1_entry, (void *)20, TASK1_STACKSIZE, TASK1_PRI);
    if (status != 0) {
        LOGE(MODULE_NAME, "create task1 error");
        return;
    }

    /**
     * create task2
     * task:       task2_handle (user needs to define a task handle variable)
     * fn :        task2_entry (the task entry function)
     * arg:        NULL (no arguments are passed to the entry function)
     * stack_size: TASK2_STACKSIZE (this parameter depends on the needs of the task using the stack,
     *             an insufficient stack size will cause an exception.)
     * prio:       TASK2_PRI (Priority is a relative value, so the user sets the priority value for
     *             a particular task based on other tasks throughout the application.)
     */
    status = aos_task_new_ext(&task2_handle, TASK2_NAME, task2_entry, NULL, TASK2_STACKSIZE, TASK2_PRI);
    if (status != 0) {
        LOGE(MODULE_NAME, "create task2 error");
        return;
    }
}

