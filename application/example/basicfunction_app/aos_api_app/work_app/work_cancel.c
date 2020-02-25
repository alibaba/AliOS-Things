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
#define MODULE_NAME "workcancel_app"

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
    printf("[work_new]perform the work\r\n");
}

void work_new(void)
{
    int status;

    /* init the work. There is no delay in the work */
    status = aos_work_init(&message_work, work_entry, NULL, 0);
    if (status != 0) {
        LOGE(MODULE_NAME, "create task1 error");
        return;
    }

    /**
     * start the work, using the default work queue.
     * There is no delay time set when creating work, so work is immediately
     * inserted into the work queue..
     */
    aos_work_sched(&message_work);
}

