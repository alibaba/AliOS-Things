/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <aos/kernel.h>
#include <ulog/ulog.h>

/**
 * This case shows how to use mutex. Create two tasks that both access the same critical resource,
 * ensuring the consistency of the critical resource through mutex.
 */

/* module name used by ulog */
#define MODULE_NAME "mutex_app"

/* task1 parameters */
#define TASK1_NAME      "task1"
#define TASK1_PRI       50
#define TASK1_STACKSIZE 512

/* task2 parameters */
#define TASK2_NAME      "task2"
#define TASK2_PRI       50
#define TASK2_STACKSIZE 512

/* mutex handle */
static aos_mutex_t mutex_handle;

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
         * the task must obtain a mutex to access the critical resource.
         */
        status = aos_mutex_lock(&mutex_handle, AOS_WAIT_FOREVER);
        if (status != 0) {
            LOGW(MODULE_NAME, "mutex lock error");
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

        status = aos_mutex_unlock(&mutex_handle);
        if (status != 0) {
            LOGE(MODULE_NAME, "mutex unlock error");
            return;
        }

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
         * the task must obtain a mutex to access the critical resource.
         */
        status = aos_mutex_lock(&mutex_handle, AOS_WAIT_FOREVER);
        if (status != 0) {
            LOGW(MODULE_NAME, "mutex lock error");
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

        status = aos_mutex_unlock(&mutex_handle);
        if (status != 0) {
            LOGE(MODULE_NAME, "mutex unlock error");
            return;
        }

        aos_msleep(1000); /* sleep 1000ms */
    }
}

void mutex_app(void)
{
    int status;

    /* create mutex statically */
    status = aos_mutex_new(&mutex_handle);
    if (status != 0) {
        LOGE(MODULE_NAME, "create mutex error");
        return;
    }

    /* Task1 and task2 both acess critical resources. */
    status = aos_task_new(TASK1_NAME, task1_entry, NULL, TASK1_STACKSIZE);
    if (status != 0) {
        aos_mutex_free(&mutex_handle);
        LOGE(MODULE_NAME, "create task1 error");
        return;
    }

    status = aos_task_new(TASK2_NAME, task2_entry, NULL, TASK2_STACKSIZE);
    if (status != 0) {
        aos_mutex_free(&mutex_handle);
        LOGE(MODULE_NAME, "create task2 error");
        return;
    }
}

