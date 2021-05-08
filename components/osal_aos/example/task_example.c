/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include <aos/kernel.h>

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

/**
 * 该示例使用任务管理函数来控制任务的执行状态，具体场景为任务2因等待某个信号量进入阻塞状态，而此时被任务1将其挂起，则任务2仍然是处于阻塞状态，\n\r
 * 如果在此过程中等到信号量，则任务2会解除阻塞进入挂起状态；如果未等到信号量，则任务2恢复状态后仍然处于阻塞状态。\n\r
 * 示例说明如下：
 *   1. 在t0时刻，任务task1、task2分别通过aos_task_new()和aos_task_new_ext()函数调用被创建，之后task1进入就绪状态，而task2处于挂起状态。
 *   2. Task1得到运行后，在t1时刻调用aos_task_resume()将task2恢复，task2进入就绪状态，之后task1通过调用aos_msleep()进入休眠状态，task2因为task1休眠而获得CPU执行权，task2运行后因等待信号量进入阻塞状态。
 *   3. Task1在t2时刻因延迟到期得到运行，并调用aos_task_suspend()将task2挂起，task2此时的状态为阻塞挂起。之后task1通过调用aos_msleep()进入休眠状态。
 *   4. Task2在t3时刻因延迟到期得到运行，并调用aos_task_resume()将task2恢复，此时task2的状态为阻塞状态。之后task1通过调用aos_msleep()进入休眠状态。
 *   5. Task1在t4时刻因延迟到期得到运行，并调用aos_sem_signal()释放信号量，这时task2因等到信号量而进入就绪状态。待到task1再次进入休眠转改后task2得到运行，进入运行状态。
 */

/* module name used by log print */
#define MODULE_NAME "aos_task_example"

/* sem handle for synchronize of task1 and task2 */
static aos_sem_t g_testsync_sem;

/* task handle for task1 and task2*/
static aos_task_t task1_handle;
static aos_task_t task2_handle;

/* task entry for task1*/
void task1_entry()
{
    aos_status_t status;

    printf("[%s]task1 is running!\n", MODULE_NAME);
    printf("[%s]task1 resume task2!\n", MODULE_NAME);

    status = aos_task_resume(&task2_handle);

    if (0 != status)
    {
        printf("[%s]task1 resume task2 failed(%ld)!\n", MODULE_NAME, status);
    }
    printf("[%s]task1 start to sleep and release CPU!\n", MODULE_NAME);
    aos_msleep(10000);

    printf("[%s]task1 suspend task2!\n", MODULE_NAME);
    if (0 != aos_task_suspend(&task2_handle))
    {
        printf("[%s]task1 resume task2 failed!\n", MODULE_NAME);
    }
    printf("[%s]task1 start to sleep and release CPU!\n", MODULE_NAME);
    aos_msleep(10000);

    printf("[%s]task1 resume task2 again!\n", MODULE_NAME);
    status = aos_task_resume(&task2_handle);

    if (0 != status)
    {
        printf("[%s]task1 resume task2 failed(%ld)!\n", MODULE_NAME, status);
    }

    printf("[%s]task1 start to sleep and release CPU!\n", MODULE_NAME);
    aos_msleep(10000);
    printf("[%s]task1 signal a semphone!\n", MODULE_NAME);
    aos_sem_signal(&g_testsync_sem);
    printf("[%s]task1 start to sleep and release CPU!\n", MODULE_NAME);
    aos_msleep(10000);
    aos_task_exit(0);
}

/* task entry for task2*/
void task2_entry()
{
    printf("[%s]task2 is running!\n", MODULE_NAME);
    if (0 != aos_sem_wait(&g_testsync_sem, AOS_WAIT_FOREVER))
    {
        printf("[%s]task2 wait semphone failed!\n", MODULE_NAME);
    }

    printf("[%s]task2 get semphone and is running!\n", MODULE_NAME);
    aos_task_exit(0);
}

/* task example init function*/
static void aos_task_example(int argc, char **argv)
{
    aos_status_t status;

    status = aos_sem_new(&g_testsync_sem, 0);
    if (status != 0)
    {
        printf("[%s]sem new failed, err=%ld\n", MODULE_NAME, status);
        return;
    }

    status = aos_task_create(&task2_handle, "task2", task2_entry, NULL, NULL, 4096, 50, AOS_TASK_NONE);
    if (status != 0) {
        aos_sem_free(&g_testsync_sem);
        printf("[%s]create %s error\n", MODULE_NAME, "task2");
        return;
    }

    status = aos_task_create(&task1_handle, "task1", task1_entry, NULL, NULL, 4096, 50, AOS_TASK_AUTORUN);
    if (status != 0) {
        aos_sem_free(&g_testsync_sem);
        printf("[%s]create %s error\n", MODULE_NAME, "task1");
        return;
    }

}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(aos_task_example, task_example, aos task example)
#endif