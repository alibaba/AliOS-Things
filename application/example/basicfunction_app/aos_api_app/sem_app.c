/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <aos/kernel.h>
#include <ulog/ulog.h>

/**
 * This use case shows how to use the aos API of sem. Create two tasks that both access the same
 * critical resource, ensuring the consistency of the critical resource through sem.
 * This case does the same thing as the mutex use case. The main differences between sem and mutex:
 * (a) Only tasks that obtain a mutex can release a mutex, while tasks that do not obtain an sem can also release an sem.
 * (b) Mutex supports resolution of priority rollover issues.
 * (c) The count of sem is increased by the release operation, so the task can obtain the sem multiple times.
 */

/* module name used by ulog */
#define MODULE_NAME "aos_sem"

/* task1 parameters */
#define TASK1_NAME      "task1"
#define TASK1_STACKSIZE 1024

/* task2 parameters */
#define TASK2_NAME      "task2"
#define TASK2_STACKSIZE 1024

/* Static memory for static creation */
static aos_sem_t sem_handle;

/*
 * simulated critical section resources.
 * The critical resource consistency condition is that all four fields are equal,
 * otherwise the critical resource is destroyed.
 */
static struct record {
    uint32_t field1;
    uint32_t field2;
    uint32_t field3;
    uint32_t field4;
} record_status;

/* task entry */
static void task1_entry(void *arg)
{
    uint32_t i = 0;
    int      status;

    while (1) {
        /**
         * In order to ensure the consistency of the critical resource,
         * the task must obtain a sem to access the critical resource.
         */
        status = aos_sem_wait(&sem_handle, AOS_WAIT_FOREVER);
        if (status != 0) {
            LOGW(MODULE_NAME, "wait sem error");
            continue;
        }

        /**
         * read critical resource.
         * If the four fields are not equal, the critical resource is destroyed.
         */
        LOGI(MODULE_NAME, "field1=%d, field2=%d, field3=%d, field4=%d", record_status.field1,
             record_status.field2, record_status.field3, record_status.field4);

        /* modify critical resources */
        i = rand();
        record_status.field1 = i;
        record_status.field2 = i;
        record_status.field3 = i;
        record_status.field4 = i;

        /* a semaphore is released when critical resource access is complete */
        aos_sem_signal(&sem_handle);

        aos_msleep(1000); /* sleep 1000ms */
    }
}

/* task entry */
static void task2_entry(void *arg)
{
    uint32_t i = 0;
    int      status;

    while (1) {
        /**
         * In order to ensure the consistency of the critical resource,
         * the task must obtain a sem to access the critical resource.
         */
        status = aos_sem_wait(&sem_handle, AOS_WAIT_FOREVER);
        if (status != 0) {
            LOGW(MODULE_NAME, "wait sem error");
            continue;
        }

        /**
         * read critical resource.
         * If the four fields are not equal, the critical resource is destroyed.
         */
        LOGI(MODULE_NAME, "field1=%d, field2=%d, field3=%d, field4=%d", record_status.field1,
             record_status.field2, record_status.field3, record_status.field4);

        /* modify critical resources */
        i = rand();
        record_status.field1 = i;
        record_status.field2 = i;
        record_status.field3 = i;
        record_status.field4 = i;

        /* a semaphore is released when critical resource access is complete */
        aos_sem_signal(&sem_handle);

        aos_msleep(1000);
    }
}

void sem_app(void)
{
    int status;

    /**
     * create sem. In this case, the sem is used to access the critical resource,
     * so count is initialized to 1
     */
    status = aos_sem_new(&sem_handle, 1);
    if (status != 0) {
        LOGE(MODULE_NAME, "create sem error");
        return;
    }

    /* Task1 and task2 both acess critical resources. */
    status = aos_task_new(TASK1_NAME, task1_entry, NULL, TASK1_STACKSIZE);
    if (status != 0) {
        aos_sem_free(&sem_handle);
        LOGE(MODULE_NAME, "create task1 error");
        return;
    }

    status = aos_task_new(TASK2_NAME, task2_entry, NULL, TASK2_STACKSIZE);
    if (status != 0) {
        aos_sem_free(&sem_handle);
        LOGE(MODULE_NAME, "create task2 error");
        return;
    }
}

