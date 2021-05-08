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
 * 该示例使用信号量实现多任务同步，具体场景为创建一个高优先级任务A，一个低优先级任务B，任务A和任务B同时等待同一信号量，\n\r
 * 此时测试任务T调用aos_sem_signal()释放信号量，任务A首先获得信号量，任务A操作完成后释放一次信号量，此时任务B获取信号量得到运行。
 * 示例说明如下：
 * 1. t0时刻，任务T调用aos_sem_new()创建一信号量，初始计数值为0。任务T然后调用aos_task_create()创建任务A和任务B，\n\r
 *    任务A优先级为30，任务B优先级为31。任务A和任务B运行后因等待信号量而阻塞。
 * 2. t1时刻，任务T调用aos_sem_signal()释放信号量，任务A获得信号量。
 * 3. t2时刻，任务A调用aos_sem_signal()释放信号量，任务B获得信号量。
 */

/* module name used by log print */
#define MODULE_NAME "aos_sem_example"

/* taskA parameters */
#define TASKA_NAME      "taskA"
#define TASKA_STACKSIZE 1024
#define TASKA_PRIO 30

/* taskB parameters */
#define TASKB_NAME      "taskB"
#define TASKB_STACKSIZE 1024
#define TASKB_PRIO 31

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

/* task entry for taskA and taskB*/
static void task_entry(void *arg)
{
    uint32_t      i = 0;
    aos_status_t  status;

    while (1) {
        /**
         * In order to ensure the consistency of the critical resource,
         * the task must obtain a sem to access the critical resource.
         */
        status = aos_sem_wait(&sem_handle, AOS_WAIT_FOREVER);
        if (status != 0) {
            printf("[%s] %p sem wait error\n", MODULE_NAME, (void *)aos_task_self());
            continue;
        }

        /**
         * read critical resource.
         * If the four fields are not equal, the critical resource is destroyed.
         */
        printf("[%s]%p field1=%ld, field2=%ld, field3=%ld, field4=%ld\n", MODULE_NAME, (void *)aos_task_self(), record_status.field1,
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

static void aos_sem_example(int argc, char **argv)
{
    aos_status_t status;
    aos_task_t   taskA_handle;
    aos_task_t   taskB_handle;

    /**
     * create sem. In this case, the sem is used to access the critical resource,
     * so count is initialized to 0
     */
    status = aos_sem_create(&sem_handle, 0, 0);
    if (status != 0) {
        printf("[%s]create sem error\n", MODULE_NAME);
        return ;
    }

    /* TaskA and taskB both acess critical resources. */
    status = aos_task_create(&taskA_handle, TASKA_NAME, task_entry, NULL, NULL, TASKA_STACKSIZE, TASKA_PRIO, AOS_TASK_AUTORUN);
    if (status != 0) {
        aos_sem_free(&sem_handle);
        printf("[%s]create %s error\n", MODULE_NAME, TASKA_NAME);
        return ;
    }

    status = aos_task_create(&taskB_handle, TASKB_NAME, task_entry, NULL, NULL, TASKB_STACKSIZE, TASKB_PRIO, AOS_TASK_AUTORUN);
    if (status != 0) {
        aos_sem_free(&sem_handle);
        printf("[%s]create %s error\n", MODULE_NAME, TASKB_NAME);
        return ;
    }

    aos_msleep(100);

    /* a semaphore is released when critical resource access is complete */
    aos_sem_signal(&sem_handle);

    aos_msleep(100);

}


#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(aos_sem_example, sem_example, aos sem example)
#endif
