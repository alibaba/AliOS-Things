/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <k_api.h>
#include <test_fw.h>

#include "queue_test.h"

#define TEST_QUEUE_MSG0_SIZE 1

static ktask_t  *task_0_test;
static void    *queue_recv_msg = (void *)1;
static void    *g_test_queue_msg0[TEST_QUEUE_MSG0_SIZE];
static kqueue_t g_test_queue0;

static void queue_del_param_test(void)
{
    kstat_t ret;
    ksem_t  sem;

    ret = krhino_queue_del(NULL);
    QUEUE_VAL_CHK(ret == RHINO_NULL_PTR);

    ret = krhino_queue_dyn_del(&g_test_queue0);
    QUEUE_VAL_CHK(ret == RHINO_KOBJ_DEL_ERR);

    krhino_sem_create(&sem, "test_sem ", 0);
    ret = krhino_queue_del((kqueue_t *)&sem);
    QUEUE_VAL_CHK(ret == RHINO_KOBJ_TYPE_ERR);
    krhino_sem_del(&sem);
}

static void task_queue0_entry(void *arg)
{
    kstat_t ret;

    while (1) {
        ret = krhino_queue_create(&g_test_queue0, "test_queue0",
                                  (void **)&g_test_queue_msg0, TEST_QUEUE_MSG0_SIZE);
        QUEUE_VAL_CHK(ret == RHINO_SUCCESS);

        /* check krhino_queue_del param */
        queue_del_param_test();

        ret = krhino_queue_del(&g_test_queue0);
        QUEUE_VAL_CHK(ret == RHINO_SUCCESS);

        next_test_case_notify();
        krhino_task_dyn_del(task_0_test);
    }
}

kstat_t task_queue_del_test(void)
{
    kstat_t ret;

    ret = krhino_task_dyn_create(&task_0_test, "task_queue0_test", 0, 10,
                                 0, TASK_TEST_STACK_SIZE, task_queue0_entry, 1);
    QUEUE_VAL_CHK((ret == RHINO_SUCCESS) || (ret == RHINO_STOPPED));

    return 0;
}

