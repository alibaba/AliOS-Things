/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <k_api.h>
#include <ulog/ulog.h>

/**
 * The task deletes itself without synchronization problems, but you need to be careful
 * if one task deletes another. This case shows how to safely remove a task asynchronously.
 */

#define BUF_LEN 100

/* module name used by ulog */
#define MODULE_NAME "task_delete_app"

/* task1 parameters */
#define TASK1_NAME      "task1"
#define TASK1_PRI       50
#define TASK1_STACKSIZE 512

/* task2 parameters */
#define TASK2_NAME      "task2"
#define TASK2_PRI       50
#define TASK2_STACKSIZE 512

/* task tcb pointer */
static ktask_t *task1_tcb = NULL;
static ktask_t *task2_tcb = NULL;

/* task entry */
static void task1_entry(void *arg)
{
    kstat_t  status;
    tick_t   sleep_time;
    ktask_t *cur_task;
    char    *buf;

    sleep_time = krhino_ms_to_ticks(1000); /* 1000ms to ticks */
    buf        = malloc(BUF_LEN);

    while (1) {
        /* check cancel status. If cancel check is true, then the task should deletes itself */
        if (krhino_task_cancel_chk() == RHINO_TRUE) {
            break;
        }

        /* perform task function, this example only prints */
        memset(buf, 0, BUF_LEN);
        snprintf(buf, BUF_LEN, "I am task 1, at state 1");
        LOGI(MODULE_NAME, "%s\r\n", buf);

        /*
         * The blocking and sleeping states can be interrupted by cancel operations,
         * so the return value needs to be determined to determine whether the task needs to end
         */
        status = krhino_task_sleep(sleep_time); /* sleep 1000ms */
        if (status == RHINO_TASK_CANCELED) {
            break;
        }

        memset(buf, 0, BUF_LEN);
        snprintf(buf, BUF_LEN, "I am task 1, at state 2");
        LOGI(MODULE_NAME, "%s\r\n", buf);
    }

    /* released resources */
    if (buf != NULL) {
        free(buf);
    }

    /**
     * first get the task's tcb and then delete the task.
     * The task is automatically deleted when the entry function completes,
     * so it makes sense not to add this code.
     */
    cur_task= krhino_cur_task_get();
    krhino_task_dyn_del(cur_task);
}

/* task entry */
static void task2_entry(void *arg)
{
    kstat_t  status;
    tick_t   sleep_time;

    sleep_time = krhino_ms_to_ticks(10000); /* 10000ms to ticks */
    krhino_task_sleep(sleep_time);          /* Sleep 10 seconds to make sure task 1 is started */

    /**
     * delete task1.
     * Tasks are not deleted in real time, so do not delete resources used by task 1.
     */
    LOGI(MODULE_NAME, "detete task1");
    status = krhino_task_cancel(task1_tcb);
    if (status != RHINO_SUCCESS) {
        LOGE(MODULE_NAME, "delete task1 error");
    }
}

void task_asyn_delete_app(void)
{
    kstat_t status;

    status = krhino_task_dyn_create(&task1_tcb, TASK1_NAME, (void *)0, TASK1_PRI, 50,
                                    TASK1_STACKSIZE, task1_entry, 1);
    if (status != RHINO_SUCCESS) {
        LOGE(MODULE_NAME, "create task1 error");
        return;
    }

    /* Task2 delete task1. */
    status = krhino_task_dyn_create(&task2_tcb, TASK2_NAME, (void *)0, TASK2_PRI, 50,
                                    TASK2_STACKSIZE, task2_entry, 1);
    if (status != RHINO_SUCCESS) {
        LOGE(MODULE_NAME, "create task2 error");
        return;
    }
}

