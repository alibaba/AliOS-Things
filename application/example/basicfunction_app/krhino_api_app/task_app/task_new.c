/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include <k_api.h>
#include <ulog/ulog.h>

/**
 * This case demonstrates the use of the task creation interface. Two tasks are
 * created in two ways, task1 with a static interface and task2 with a dynamic interface.
 * Task1 was not activated when it was created, while task2 was activated when it was created.
 */

/* module name used by ulog */
#define MODULE_NAME "task_app"

/* task parameters */
#define TASK1_NAME      "task1"
#define TASK2_NAME      "task2"
/* Task priority must be less than or equal to RHINO_CONFIG_USER_PRI_MAX */
#define TASK1_PRI       50
#define TASK2_PRI       50
#define TASK1_STACKSIZE 256
#define TASK2_STACKSIZE 256

/* resources for tasks1 which use static memeory when created */
static ktask_t     task1_tcb;
static cpu_stack_t task1_stack[TASK1_STACKSIZE];

/* task tcb pointer for task2 which use heap memory when created */
static ktask_t *task2_tcb = NULL;

/* task entry */
static void task1_entry(void *arg)
{
    /* Parameters are passed through the arg */
    int count = (int)arg;

    for (int i = 0; i < count; i++) {
        LOGI(MODULE_NAME, "I'm task1");

        /**
         * sleep 1000 ticks
         * 1 second = RHINO_CONFIG_TICKS_PER_SECOND ticks
         */
        krhino_task_sleep(1000);
    }

    /* tasks are automatically deleted after the entry function completes */
}

/* task entry */
static void task2_entry(void *arg)
{
    tick_t sleep_time;

    sleep_time = krhino_ms_to_ticks(2000); /* 2000ms to ticks */

    while (1) {
        LOGI(MODULE_NAME, "I'm task2");
        krhino_task_sleep(sleep_time);     /* sleep 2000ms */
    }
}

void task_new(void)
{
    kstat_t status;

    /**
     * Create task with static memory, some of the task parameters are as follows:
     * arg:     20(passed to the task entry function)
     * ticks:   50(time slice length)
     * autorun: 0 (not activated when the task is created)
     */
    status = krhino_task_create(&task1_tcb, TASK1_NAME, (void *)20, TASK1_PRI, 50,
                                task1_stack, TASK1_STACKSIZE, task1_entry, 0);
    if (status != RHINO_SUCCESS) {
        LOGE(MODULE_NAME, "create task1 error");
        return;
    }

    /**
     * Autorun is set to 0 when task 1 is created, so you need to invoke
     * krhino_task_resum to activate the task.
     */
    krhino_task_resume(&task1_tcb);

    /*
     * Create task with memory form heap, some of the task parameters are as follows:
     * arg:     0 (passed to the task entry function)
     * ticks:   50(time slice length)
     * autorun: 1 (activated when the task is created)
     */
    status = krhino_task_dyn_create(&task2_tcb, TASK2_NAME, (void *)0, TASK2_PRI, 50,
                                    TASK2_STACKSIZE, task2_entry, 1);
    if (status != RHINO_SUCCESS) {
        LOGE(MODULE_NAME, "create task2 error");
        return;
    }
}

