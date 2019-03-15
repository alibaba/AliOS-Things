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

static void queue_create_param_test(void)
{
    kstat_t ret;

    ret = krhino_queue_create(NULL, "test_queue0", (void **)&g_test_queue_msg0,
                              TEST_QUEUE_MSG0_SIZE);
    QUEUE_VAL_CHK(ret == RHINO_NULL_PTR);

    ret = krhino_queue_create(&g_test_queue0, NULL, (void **)&g_test_queue_msg0,
                              TEST_QUEUE_MSG0_SIZE);
    QUEUE_VAL_CHK(ret == RHINO_NULL_PTR);

    ret = krhino_queue_create(&g_test_queue0, "test_queue0", NULL,
                              TEST_QUEUE_MSG0_SIZE);
    QUEUE_VAL_CHK(ret == RHINO_NULL_PTR);

    ret = krhino_queue_create(&g_test_queue0, "test_queue0",
                              (void **)&g_test_queue_msg0, 0);
    QUEUE_VAL_CHK(ret == RHINO_INV_PARAM);
}

static void queue_recv_param_test(void)
{
    kstat_t ret;
    ksem_t  sem;

    ret = krhino_queue_recv(NULL, RHINO_NO_WAIT, &queue_recv_msg);
    QUEUE_VAL_CHK(ret == RHINO_NULL_PTR);

    ret = krhino_queue_recv(&g_test_queue0, RHINO_NO_WAIT, NULL);
    QUEUE_VAL_CHK(ret == RHINO_NULL_PTR);

    krhino_sem_create(&sem, "test_sem ", 0);
    ret = krhino_queue_recv((kqueue_t *)&sem, RHINO_WAIT_FOREVER, &queue_recv_msg);
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

        /* check krhino_queue_create param */
        queue_create_param_test();

        /* check krhino_queue_recv param */
        queue_recv_param_test();

        ret = krhino_queue_recv(&g_test_queue0, RHINO_NO_WAIT, &queue_recv_msg);

        if (ret == RHINO_NO_PEND_WAIT) {
            test_case_success++;
            PRINT_RESULT("queue nowait recv", PASS);
        } else {
            test_case_fail++;
            PRINT_RESULT("queue nowait recv", FAIL);
        }

        krhino_queue_del(&g_test_queue0);
        next_test_case_notify();
        krhino_task_dyn_del(task_0_test);
    }
}

kstat_t task_queue_nowait_recv_test(void)
{
    kstat_t ret;

    ret = krhino_task_dyn_create(&task_0_test, "task_queue0_test", 0, 10,
                                 0, TASK_TEST_STACK_SIZE, task_queue0_entry, 1);
    QUEUE_VAL_CHK((ret == RHINO_SUCCESS) || (ret == RHINO_STOPPED));

    return 0;
}

