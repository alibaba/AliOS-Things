/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <k_api.h>
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

/* task tcb pointer */
static ktask_t *task1_tcb = NULL;
static ktask_t *task2_tcb = NULL;

/* Static memory for static creation */
static kmutex_t mutex_handle;

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
    kstat_t  status;
    tick_t   sleep_time;

    sleep_time = krhino_ms_to_ticks(1000); /* 1000ms to ticks */

    while (1) {
        /**
         * In order to ensure the consistency of the critical resource,
         * the task must obtain a mutex to access the critical resource.
         */
        status = krhino_mutex_lock(&mutex_handle, RHINO_WAIT_FOREVER);
        if (status != RHINO_SUCCESS) {
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

        status = krhino_mutex_unlock(&mutex_handle);
        if (status != RHINO_SUCCESS) {
            LOGE(MODULE_NAME, "mutex unlock error");
            return;
        }

        krhino_task_sleep(sleep_time); /* sleep 1000ms */
    }
}

/* task entry */
static void task2_entry(void *arg)
{
    uint32_t i = 0;
    kstat_t  status;
    tick_t   sleep_time;

    sleep_time = krhino_ms_to_ticks(1000); /* 1000ms to ticks */

    while (1) {
        /**
         * In order to ensure the consistency of the critical resource,
         * the task must obtain a mutex to access the critical resource.
         */
        status = krhino_mutex_lock(&mutex_handle, RHINO_WAIT_FOREVER);
        if (status != RHINO_SUCCESS) {
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

        status = krhino_mutex_unlock(&mutex_handle);
        if (status != RHINO_SUCCESS) {
            LOGE(MODULE_NAME, "mutex unlock error");
            return;
        }

        krhino_task_sleep(sleep_time); /* sleep 1000ms */
    }
}

void mutex_app(void)
{
    kstat_t status;

    /* create mutex statically */
    status = krhino_mutex_create(&mutex_handle, "mutex app");
    if (status != RHINO_SUCCESS) {
        LOGE(MODULE_NAME, "create mutex error");
        return;
    }

    /**
     * Task1 and task2 both acess critical resources.
     */
    status = krhino_task_dyn_create(&task1_tcb, TASK1_NAME, (void *)0, TASK1_PRI, 50,
                                    TASK1_STACKSIZE, task1_entry, 1);
    if (status != RHINO_SUCCESS) {
        krhino_mutex_del(&mutex_handle);
        LOGE(MODULE_NAME, "create task1 error");
        return;
    }

    status = krhino_task_dyn_create(&task2_tcb, TASK2_NAME, (void *)0, TASK2_PRI, 50,
                                    TASK2_STACKSIZE, task2_entry, 1);
    if (status != RHINO_SUCCESS) {
        krhino_mutex_del(&mutex_handle);
        LOGE(MODULE_NAME, "create task2 error");
        return;
    }
}

