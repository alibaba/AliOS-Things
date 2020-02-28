/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include <aos/kernel.h>
#include <ulog/ulog.h>

/**
 * This case demonstrates the use of work queue.
 * This use case uses the work queue created by the user, rather than the default work queue.
 * Creating a new workqueue creates a task in the background, so there is a cost associated
 * with having the workqueue to yourself. The priority of the workqueue background task depends
 * on the application. If the work to be executed has a higher priority, the workqueue background
 * task has a higher priority, and vice versa.
 */

/* module name used by ulog */
#define MODULE_NAME "workqueue_app"

/* task1 parameters */
#define TASK1_NAME      "task1"
#define TASK1_STACKSIZE 1024

/* work handle */
static aos_work_t      message_work;
static aos_workqueue_t message_work_queue;

/**
 * This function executes in the background task of work queue
 * When work is inserted into the work queue, it is executed sequentially.
 * If one work blocks, subsequent work will not be executed.
 */
static void work_entry(void *arg)
{
    printf("[workqueue_new]perform the work\r\n");
}

void workqueue_new(void)
{
    int status;

    /**
     * Create a user-defined work queue.
     * workqueue: message_work_queue (aos_workqueue_t structure variable)
     * pri:       10(priority of the workqueue background task)
     * stack_size: 512(stack size of the workqueue background task)
     */
    status = aos_workqueue_create(&message_work_queue, 10, 512);
    if (status != 0) {
        LOGE(MODULE_NAME, "create workqueue error");
        return;
    }

    /* init the work. The work was executed with a delay of 1000ms after startup. */
    status = aos_work_init(&message_work, work_entry, NULL, 0);
    if (status != 0) {
        LOGE(MODULE_NAME, "create task1 error");
        return;
    }

    /**
     * start the work, using the newly created workqueue .
     */
    aos_work_run(&message_work_queue, &message_work);
}

