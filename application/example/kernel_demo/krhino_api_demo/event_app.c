/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <k_api.h>
#include <ulog/ulog.h>

/**
 * This case shows how to use event. Create three tasks, where task 1 and task 2 are the producers
 * and task 3 is the consumer. Task 3 cannot use resources until task 1 and task 2 have produced them.
 * Task 3 prints the resource information.
 */

/* module name used by ulog */
#define MODULE_NAME "event_app"

/* define events */
#define EVENT_RESOURCE1_OK   0x1
#define EVENT_RESOURCE2_OK   0x2
#define EVENT_RESOURCE1_USED 0x4
#define EVENT_RESOURCE2_USED 0x8

/* task1 parameters */
#define TASK1_NAME      "task1"
#define TASK1_PRI       50
#define TASK1_STACKSIZE 512

/* task2 parameters */
#define TASK2_NAME      "task2"
#define TASK2_PRI       50
#define TASK2_STACKSIZE 512

/* task3 parameters */
#define TASK3_NAME      "task3"
#define TASK3_PRI       50
#define TASK3_STACKSIZE 512

/* task tcb pointer */
static ktask_t *task1_tcb = NULL;
static ktask_t *task2_tcb = NULL;
static ktask_t *task3_tcb = NULL;

/* Static memory for event */
static kevent_t event_handle;

/*
 * Task1 handles resource1, and task2 handles resource2.
 * When both resources are ready, task 3 can use them.
 */
uint32_t resource1, resource2;

/* task entry */
static void task1_entry(void *arg)
{
    kstat_t  status;
    tick_t   sleep_time;
    uint32_t actl_flags;

    sleep_time = krhino_ms_to_ticks(1000); /* 1000ms to ticks */

    /* prepare resources for the first time */
    resource1 = rand();
    /* set event to notifies resource 1 is available */
    status = krhino_event_set(&event_handle, EVENT_RESOURCE1_OK, RHINO_OR);
    if (status != RHINO_SUCCESS) {
        LOGE(MODULE_NAME, "event set error");
    }

    while (1) {
        /**
         * wait for task 3 to run out of resources
         * RHINO_AND_CLEAR means that each event of the request has to be satisfied,
         * and the event is cleaned up after the request.
         * When the event is satisfied, actl_flags is set to the current flags of the event.
         * This return value is not used here.
         */
        status = krhino_event_get(&event_handle, EVENT_RESOURCE1_USED, RHINO_AND_CLEAR,
                                  &actl_flags, RHINO_WAIT_FOREVER);
        if (status != RHINO_SUCCESS) {
            LOGW(MODULE_NAME, "event get error");
            continue;
        }

        resource1 = rand();

        /* set event to notifies resource 1 is available */
        status = krhino_event_set(&event_handle, EVENT_RESOURCE1_OK, RHINO_OR);
        if (status != RHINO_SUCCESS) {
            LOGE(MODULE_NAME, "event set error");
        }

        krhino_task_sleep(sleep_time); /* sleep 1000ms */
    }
}

/* task entry */
static void task2_entry(void *arg)
{
    kstat_t  status;
    tick_t   sleep_time;
    uint32_t actl_flags;

    sleep_time = krhino_ms_to_ticks(1000); /* 1000ms to ticks */

    /* prepare resources for the first time */
    resource2 = rand();
    /* set event to notifies resource 2 is available */
    status = krhino_event_set(&event_handle, EVENT_RESOURCE2_OK, RHINO_OR);
    if (status != RHINO_SUCCESS) {
        LOGE(MODULE_NAME, "event set error");
    }

    while (1) {
        /**
         * wait for task 3 to run out of resources
         * RHINO_AND_CLEAR means that each event of the request has to be satisfied,
         * and the event is cleaned up after the request.
         * When the event is satisfied, actl_flags is set to the current flags of the event.
         * This return value is not used here.
         */
        status = krhino_event_get(&event_handle, EVENT_RESOURCE2_USED, RHINO_AND_CLEAR,
                                  &actl_flags, RHINO_WAIT_FOREVER);
        if (status != RHINO_SUCCESS) {
            LOGW(MODULE_NAME, "event get error");
            continue;
        }

        resource2 = rand();

        /* set event to notifies resource 2 is available */
        status = krhino_event_set(&event_handle, EVENT_RESOURCE2_OK, RHINO_OR);
        if (status != RHINO_SUCCESS) {
            LOGE(MODULE_NAME, "event set error");
        }

        krhino_task_sleep(sleep_time); /* sleep 1000ms */
    }
}


/* task entry */
static void task3_entry(void *arg)
{
    kstat_t  status;
    uint32_t actl_flags;

    while (1) {
        /**
         * Wait for both resources to be available
         * RHINO_AND_CLEAR means that each event of the request has to be satisfied,
         * and the event is cleaned up after the request.
         * When the event is satisfied, actl_flags is set to the current flags of the event.
         * This return value is not used here.
         */
        status = krhino_event_get(&event_handle, EVENT_RESOURCE1_OK | EVENT_RESOURCE2_OK, RHINO_AND_CLEAR,
                                  &actl_flags, RHINO_WAIT_FOREVER);
        if (status != RHINO_SUCCESS) {
            LOGW(MODULE_NAME, "event get error");
            continue;
        }

        /* access the resource */
        LOGI(MODULE_NAME, "resource1=%d, resource2=%d", resource1, resource2);

        /* Task3 has run out of resources1 */
        status = krhino_event_set(&event_handle, EVENT_RESOURCE1_USED, RHINO_OR);
        if (status != RHINO_SUCCESS) {
            LOGE(MODULE_NAME, "event set error");
        }

        /* Task3 has run out of resources2 */
        status = krhino_event_set(&event_handle, EVENT_RESOURCE2_USED, RHINO_OR);
        if (status != RHINO_SUCCESS) {
            LOGE(MODULE_NAME, "event set error");
        }
    }
}

void event_app(void)
{
    kstat_t status;

    /* create event statically */
    status = krhino_event_create(&event_handle, "event app", 0);
    if (status != RHINO_SUCCESS) {
        LOGE(MODULE_NAME, "create event error");
        return;
    }

    /* Task 1 and task 2 prepare resources. */
    status = krhino_task_dyn_create(&task1_tcb, TASK1_NAME, (void *)0, TASK1_PRI, 50,
                                    TASK1_STACKSIZE, task1_entry, 1);
    if (status != RHINO_SUCCESS) {
        krhino_event_del(&event_handle);
        LOGE(MODULE_NAME, "create task1 error");
        return;
    }

    status = krhino_task_dyn_create(&task2_tcb, TASK2_NAME, (void *)0, TASK2_PRI, 50,
                                    TASK2_STACKSIZE, task2_entry, 1);
    if (status != RHINO_SUCCESS) {
        krhino_event_del(&event_handle);
        LOGE(MODULE_NAME, "create task2 error");
        return;
    }

    /* Task 3 use resources. */
    status = krhino_task_dyn_create(&task3_tcb, TASK3_NAME, (void *)0, TASK3_PRI, 50,
                                    TASK3_STACKSIZE, task3_entry, 1);
    if (status != RHINO_SUCCESS) {
        krhino_event_del(&event_handle);
        LOGE(MODULE_NAME, "create task3 error");
        return;
    }
}

