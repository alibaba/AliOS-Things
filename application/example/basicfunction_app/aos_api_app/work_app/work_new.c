/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include <aos/kernel.h>
#include <ulog/ulog.h>

/**
 * This case demonstrates the use of work queue.
 */

/* module name used by ulog */
#define MODULE_NAME "worknew_app"

/* task1 parameters */
#define TASK1_NAME      "task1"
#define TASK1_STACKSIZE 1024

/* work handle */
static aos_work_t message_work;

/**
 * This function executes in the background task of work queue
 * When work is inserted into the work queue, it is executed sequentially.
 * If one work blocks, subsequent work will not be executed.
 */
static void work_entry(void *arg)
{
    printf("[work_cancel]perform the work\r\n");
}

void work_cancel(void)
{
    int status;

    /* init the work. The work was executed with a delay of 1000ms after startup. */
    status = aos_work_init(&message_work, work_entry, NULL, 1000);
    if (status != 0) {
        LOGE(MODULE_NAME, "create task1 error");
        return;
    }

    /**
     * start the work, using the default work queue.
     * Work has a 1000ms delay, so it will not be inserted into the work queue
     * until 1000ms after the function is called.
     */
    aos_work_sched(&message_work);

    /* cancel the work. If the delay has expired, the cancel operation has no real effect. */
    aos_work_cancel(&message_work);
}

