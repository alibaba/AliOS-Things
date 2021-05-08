/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <aos/kernel.h>

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

/**
 * 该示例使用事件机制实现任务间同步，具体场景为创建任务A和认为B，以及一事件。任务A以“与”的方式等待事件1和事件2；任务B以“或”的方式等待事件1和事件2。测试任务T设置事件1，则任务B因获取事件得到运行，之后测试任务T设置事件2，则任务A因等到全部事件而得到运行。
 * 示例说明如下：
 * 1. t0时刻，任务T调用aos_event_new()创建一事件。任务T然后调用aos_task_create()创建任务A和任务B。任务A调用aos_event_get()以RHINO_AND为选项参数等待事件1和事件2的发生；任务B调用aos_event_get()以RHINO_OR为选项参数等待事件1或事件2的发生。
 * 2. t1时刻，任务T调用aos_event_get()设置事件1，任务B因等到事件1得到运行。
 * 3. t2时刻，任务T调用aos_event_get()设置事件2，任务A因等到了所有事件1和2而得到运行。
 */

/* module name used by log print */
#define MODULE_NAME "aos_event_example"

/* taskA parameters */
#define TASKA_NAME      "taskA"
#define TASKA_STACKSIZE 1024
#define TASKA_PRIO 30

/* taskB parameters */
#define TASKB_NAME      "taskB"
#define TASKB_STACKSIZE 1024
#define TASKB_PRIO 31

#define USER_EVENT_1 (1 << 0)         /**< user event 1*/
#define USER_EVENT_2 (1 << 1)         /**< user event 2 */

/* Static memory for static creation */
static aos_event_t event_handle;

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
static void taskA_entry(void *arg)
{
    uint32_t      i = 0;
    aos_status_t  status;
    uint32_t      actl_flags;

    /**
        * In order to ensure the consistency of the critical resource,
        * the task must obtain a sem to access the critical resource.
        */

    status = aos_event_get(&event_handle, USER_EVENT_1 | USER_EVENT_2, AOS_EVENT_OR, &actl_flags, AOS_WAIT_FOREVER);

    if (status != 0) {
        printf("[%s] %p sem wait error\n", MODULE_NAME, (void *)aos_task_self());
        return;
    }

    /**
        * read critical resource.
        * If the four fields are not equal, the critical resource is destroyed.
        */
    printf("[%s][%s]field1=%ld, field2=%ld, field3=%ld, field4=%ld\n", MODULE_NAME, TASKA_NAME, record_status.field1,
            record_status.field2, record_status.field3, record_status.field4);

    /* modify critical resources */
    i = rand();
    record_status.field1 = i;
    record_status.field2 = i;
    record_status.field3 = i;
    record_status.field4 = i;

    aos_msleep(1000); /* sleep 1000ms */
}

/* task entry */
static void taskB_entry(void *arg)
{
    uint32_t       i = 0;
    aos_status_t   status;
    uint32_t       actl_flags;

    /**
        * In order to ensure the consistency of the critical resource,
        * the task must obtain a sem to access the critical resource.
        */
    status = aos_event_get(&event_handle, USER_EVENT_1 | USER_EVENT_2, AOS_EVENT_AND, &actl_flags, AOS_WAIT_FOREVER);
    if (status != 0) {
        printf("[%s]%p sem wait error\n", MODULE_NAME, (void *)aos_task_self());
        return;
    }

    /**
        * read critical resource.
        * If the four fields are not equal, the critical resource is destroyed.
        */
    printf("[%s][%s]field1=%ld, field2=%ld, field3=%ld, field4=%ld\n", MODULE_NAME, TASKB_NAME, record_status.field1,
            record_status.field2, record_status.field3, record_status.field4);

    /* modify critical resources */
    i = rand();
    record_status.field1 = i;
    record_status.field2 = i;
    record_status.field3 = i;
    record_status.field4 = i;

    aos_msleep(1000); /* sleep 1000ms */
}

static void aos_event_example(int argc, char **argv)
{
    aos_status_t  status;
    aos_task_t    taskA_handle;
    aos_task_t    taskB_handle;

    /**
     * create event. In this case, the event is used to access the critical resource,
     * so flag is initialized to 0
     */
    status = aos_event_create(&event_handle, 0, 0);
    if (status != 0) {
        printf("[%s]create sem error\n", MODULE_NAME);
        return;
    }

    /* TaskA and taskB both acess critical resources. */
    status = aos_task_create(&taskA_handle, TASKA_NAME, taskA_entry, NULL, NULL, TASKA_STACKSIZE, TASKA_PRIO, AOS_TASK_AUTORUN);
    if (status != 0) {
        aos_event_free(&event_handle);
        printf("[%s]create %s error\n", MODULE_NAME, TASKA_NAME);
        return;
    }

    status = aos_task_create(&taskB_handle, TASKB_NAME, taskB_entry, NULL, NULL, TASKB_STACKSIZE, TASKB_PRIO, AOS_TASK_AUTORUN);
    if (status != 0) {
        aos_event_free(&event_handle);
        printf("[%s]create %s error\n", MODULE_NAME, TASKB_NAME);
        return;
    }

    aos_msleep(100);

    printf("[%s]set event 1!\n", MODULE_NAME);
    /* a semaphore is released when critical resource access is complete */
    aos_event_set(&event_handle, USER_EVENT_1, AOS_EVENT_OR);

    aos_msleep(100);

    printf("[%s]set event 2!\n", MODULE_NAME);
    aos_event_set(&event_handle, USER_EVENT_2, AOS_EVENT_OR);

    aos_msleep(100);

    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(aos_event_example, event_example, aos event example)
#endif