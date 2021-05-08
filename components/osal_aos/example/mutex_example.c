/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <aos/kernel.h>

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

/**
 * 该示例使用互斥量实现共享资源的互斥访问，具体场景为创建任务A和认为B，以及一互斥量。任务A和任务B使用互斥量同时访问共享数据区，访问共享数据区时使用互斥量做保护。
 * 示例说明如下：
 * 1. t0时刻，任务T调用aos_mutex_create()创建一互斥量。任务T然后调用aos_task_create()创建任务A和任务B。任务A得到运行，并获取互斥量对数据区record_status进行读写操作。
 * 2. t1时刻，任务A因时间片耗尽，让出CPU，任务B得到运行。
 * 3. t2时刻，任务B因无法获得互斥量，进入阻塞状态，任务A得到运行。
 * 4. t3时刻，任务A对数据区record_status的操作完成，释放互斥量，任务B获得互斥量开始对数据区record_status进行读写操作。
 */

/* module name used by log print*/
#define MODULE_NAME "aos_mutex_example"

/* taskA parameters */
#define TASKA_NAME      "taskA"
#define TASKA_PRI       50
#define TASKA_STACKSIZE 1024

/* taskB parameters */
#define TASKB_NAME      "taskB"
#define TASKB_PRI       50
#define TASKB_STACKSIZE 1024

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
static void task_entry(void *arg)
{
    uint32_t     i = 0;
    aos_status_t status;

    while (1) {
        /**
         * In order to ensure the consistency of the critical resource,
         * the task must obtain a mutex to access the critical resource.
         */
        status = aos_mutex_lock(&mutex_handle, AOS_WAIT_FOREVER);
        if (status != 0) {
            printf("[%s] %p mutex lock error\n", MODULE_NAME, (void *)aos_task_self());
            continue;
        }

        /**
         * read critical resource.
         * If the four fields are not equal, the critical resource is destroyed.
         */
        printf("[%s] %p field1=%ld, field2=%ld, field3=%ld, field4=%ld\n", MODULE_NAME, (void *)aos_task_self(), record_status.field1,
             record_status.field2, record_status.field3, record_status.field4);

        /* modify critical resources */
        i = rand();
        record_status.field1 = i;
        record_status.field2 = i;
        record_status.field3 = i;
        record_status.field4 = i;

        status = aos_mutex_unlock(&mutex_handle);
        if (status != 0) {
            printf("[%s]%p mutex unlock error\n", MODULE_NAME, (void *)aos_task_self());
            return;
        }

        aos_msleep(1000); /* sleep 1000ms */
    }
}

static void aos_mutex_example(int argc, char **argv)
{
    aos_status_t status;

    /* create mutex statically */
    status = aos_mutex_create(&mutex_handle, 0);
    if (status != 0) {
        printf("[%s]create mutex error\n", MODULE_NAME);
        return;
    }

    /* Task1 and task2 both acess critical resources. */
    status = aos_task_new(TASKA_NAME, task_entry, NULL, TASKA_STACKSIZE);
    if (status != 0) {
        aos_mutex_free(&mutex_handle);
        printf("[%s]create %s error\n", MODULE_NAME, TASKA_NAME);
        return;
    }

    status = aos_task_new(TASKB_NAME, task_entry, NULL, TASKB_STACKSIZE);
    if (status != 0) {
        aos_mutex_free(&mutex_handle);
        printf("[%s]create %s error\n", MODULE_NAME, TASKB_NAME);
        return;
    }
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(aos_mutex_example, mutex_example, aos mutex example)
#endif
