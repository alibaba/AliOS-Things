/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "workqueue_test.h"

#define MODULE_NAME "workqueue_interface"

#if (RHINO_CONFIG_WORKQUEUE > 0)
static ksem_t g_wq_test_sem;

static void work0_func(void *arg)
{
    krhino_task_sleep(45);
    printf("--%s--\n", (char *)arg);
}

static void work1_func(void *arg)
{
    krhino_task_sleep(15);
    printf("--%s--\n", (char *)arg);
}

static void work2_func(void *arg)
{
    krhino_task_sleep(10);
    printf("--%s--\n", (char *)arg);
}

static void work3_func(void *arg)
{
    printf("--%s--\n", (char *)arg);
}

static void work4_func(void *arg)
{
    krhino_task_sleep(15);
    printf("--%s--\n", (char *)arg);
    krhino_sem_give(&g_wq_test_sem);
}

static void work5_func(void *arg)
{
    printf("--%s--\n", (char *)arg);
    krhino_sem_give(&g_wq_test_sem);
}

static void work6_func(void *arg)
{
    printf("--%s--\n", (char *)arg);
}

#define WORK_STACK_BUF 512

cpu_stack_t stack0_buf[WORK_STACK_BUF];
cpu_stack_t stack1_buf[WORK_STACK_BUF];
cpu_stack_t stack2_buf[WORK_STACK_BUF];

static kworkqueue_t wq0;
static kworkqueue_t wq1;
static kworkqueue_t wq2;
static kwork_t      work0;
static kwork_t      work1;
static kwork_t      work2;
static kwork_t      work3;
static kwork_t      work4;
static kwork_t      work5;
static kwork_t      work6;

static uint8_t workqueue_interface_case1(void)
{
    kstat_t      ret;

    size_t stack0_size = WORK_STACK_BUF;
    size_t stack1_size = WORK_STACK_BUF;
    size_t stack2_size = WORK_STACK_BUF;

    printf("==========WORKQUEUE TEST START!==========\n");

    krhino_sem_create(&g_wq_test_sem, "WORKQUEUE-SEM", 0);

    /* creat workqueues */
    ret = krhino_workqueue_create(&wq0, "WORKQUEUE1-TEST", TASK_WORKQUEUE_PRI,
                                  stack0_buf, stack0_size);
    if (ret != RHINO_SUCCESS) {
        MYASSERT(ret);
        krhino_sem_give(&g_wq_test_sem);
        return 1;
    }

    ret = krhino_workqueue_create(&wq1, "WORKQUEUE1-TEST", TASK_WORKQUEUE_PRI,
                                  stack1_buf, stack1_size);
    if (ret != RHINO_SUCCESS) {
        MYASSERT(ret);
        krhino_sem_give(&g_wq_test_sem);
        return 1;
    }

    ret = krhino_workqueue_create(&wq2, "WORKQUEUE2-TEST", TASK_WORKQUEUE_PRI,
                                  stack2_buf, stack2_size);
    if (ret != RHINO_SUCCESS) {
        MYASSERT(ret);
        krhino_sem_give(&g_wq_test_sem);
        return 1;
    }

    ret = krhino_workqueue_create(&wq1, "WORKQUEUE", TASK_WORKQUEUE_PRI,
                                  stack1_buf, stack1_size);
    if (ret != RHINO_WORKQUEUE_EXIST) {
        MYASSERT(ret);
        krhino_sem_give(&g_wq_test_sem);
        return 1;
    }

    /* init works */
    ret = krhino_work_init(&work0, work0_func, "WORK 0", 0);
    if (ret != RHINO_SUCCESS) {
        MYASSERT(ret);
        krhino_sem_give(&g_wq_test_sem);
        return 1;
    }

    ret = krhino_work_init(&work1, work1_func, "WORK 1", 0);
    if (ret != RHINO_SUCCESS) {
        MYASSERT(ret);
        krhino_sem_give(&g_wq_test_sem);
        return 1;
    }

    ret = krhino_work_init(&work2, work2_func, "WORK 2", 1);
    if (ret != RHINO_SUCCESS) {
        MYASSERT(ret);
        krhino_sem_give(&g_wq_test_sem);
        return 1;
    }

    ret = krhino_work_init(&work3, work3_func, "WORK 3", 20);
    if (ret != RHINO_SUCCESS) {
        MYASSERT(ret);
        krhino_sem_give(&g_wq_test_sem);
        return 1;
    }

    ret = krhino_work_init(&work4, work4_func, "WORK 4", 18);
    if (ret != RHINO_SUCCESS) {
        MYASSERT(ret);
        krhino_sem_give(&g_wq_test_sem);
        return 1;
    }

    ret = krhino_work_init(&work5, work5_func, "WORK 5", 40);
    if (ret != RHINO_SUCCESS) {
        MYASSERT(ret);
        krhino_sem_give(&g_wq_test_sem);
        return 1;
    }

    ret = krhino_work_init(&work6, work6_func, "WORK 6", 50);
    if (ret != RHINO_SUCCESS) {
        MYASSERT(ret);
        krhino_sem_give(&g_wq_test_sem);
        return 1;
    }

    /* work run */
    krhino_work_run(&wq1, &work0);
    krhino_work_run(&wq1, &work1);
    krhino_work_run(&wq1, &work1);

    krhino_work_run(&wq1, &work2);
    krhino_work_run(&wq1, &work2);
    krhino_work_run(&wq1, &work3);

    krhino_work_sched(&work4);
    krhino_work_sched(&work5);
    krhino_work_sched(&work6);

    /* wait for task4 */
    krhino_sem_take(&g_wq_test_sem, RHINO_WAIT_FOREVER);

    krhino_work_sched(&work5);

    /* wait for task6 */
    krhino_sem_take(&g_wq_test_sem, RHINO_WAIT_FOREVER);

    printf("=====FUNCTION TEST DONE!=====\n");

    ret = krhino_workqueue_create(NULL, "WORKQUEUE1-TEST", TASK_WORKQUEUE_PRI,
                                  stack1_buf, stack1_size);
    if (ret == RHINO_SUCCESS) {
        MYASSERT(ret);
        return 1;
    }

    ret = krhino_workqueue_create(&wq1, NULL, TASK_WORKQUEUE_PRI,
                                  stack1_buf, stack1_size);
    if (ret == RHINO_SUCCESS) {
        MYASSERT(ret);
        return 1;
    }

    ret = krhino_workqueue_create(&wq1, "WORKQUEUE1-TEST", RHINO_CONFIG_PRI_MAX,
                                  stack1_buf, stack1_size);
    if (ret == RHINO_SUCCESS) {
        MYASSERT(ret);
        return 1;
    }

    ret = krhino_workqueue_create(&wq1, "WORKQUEUE1-TEST", TASK_WORKQUEUE_PRI,
                                  NULL, stack1_size);
    if (ret == RHINO_SUCCESS) {
        MYASSERT(ret);
        return 1;
    }

    ret = krhino_workqueue_create(&wq1, "WORKQUEUE1-TEST", TASK_WORKQUEUE_PRI,
                                  stack1_buf, 0);
    if (ret == RHINO_SUCCESS) {
        MYASSERT(ret);
        return 1;
    }

    ret = krhino_work_init(NULL, work1_func, "WORK 1", 200);
    if (ret == RHINO_SUCCESS) {
        MYASSERT(ret);
        return 1;
    }

    ret = krhino_work_init(&work1, NULL, "WORK 1", 200);
    if (ret == RHINO_SUCCESS) {
        MYASSERT(ret);
        return 1;
    }

    krhino_work_run(NULL, &work3);
    krhino_work_run(&wq1, NULL);

    printf("=====PARAMTER TEST DONE!=====\n");

    krhino_sem_del(&g_wq_test_sem);

    printf("==========WORKQUEUE TEST DONE!==========\n");

    return 0;
}

static const test_func_t workqueue_func_runner[] = {
    workqueue_interface_case1,
    NULL
};

void workqueue_interface_test(void)
{
    kstat_t ret;

    task_workqueue_entry_register(MODULE_NAME,
                                  (test_func_t *)workqueue_func_runner,
                                  sizeof(workqueue_func_runner) / sizeof(test_func_t));

    ret = krhino_task_dyn_create(&task_workqueue, MODULE_NAME, 0,
                                 TASK_WORKQUEUE_PRI, 0, TASK_TEST_STACK_SIZE,
                                 task_workqueue_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }
}
#endif
