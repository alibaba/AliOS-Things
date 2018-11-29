/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <k_api.h>
#include <test_fw.h>

#include "queue_test.h"

#define TEST_QUEUE_MSG0_SIZE 30

static ktask_t   *task_0_test;
static ktask_t   *task_1_test;
static void     *queue_send_msg = (void *)1;
static void     *queue_recv_msg = (void *)1;
static kqueue_t *g_test_queue0;

static void queue_back_send_param_test(void)
{
    kstat_t ret;
    ksem_t  sem;

    ret = krhino_queue_back_send(NULL, queue_send_msg);
    QUEUE_VAL_CHK(ret == RHINO_NULL_PTR);

    krhino_sem_create(&sem, "test_sem ", 0);
    ret = krhino_queue_back_send((kqueue_t *)&sem, queue_send_msg);
    QUEUE_VAL_CHK(ret == RHINO_KOBJ_TYPE_ERR);
    krhino_sem_del(&sem);
}

static void krhino_queue_dyn_create_param_test(void)
{
    kstat_t ret;

    ret = krhino_queue_dyn_create(NULL, "test_queue0", TEST_QUEUE_MSG0_SIZE);
    QUEUE_VAL_CHK(ret == RHINO_NULL_PTR);
}

static void krhino_queue_dyn_del_param_test(void)
{
    kstat_t ret;
    ksem_t  sem;

    ret = krhino_queue_dyn_del(NULL);
    QUEUE_VAL_CHK(ret == RHINO_NULL_PTR);

    ret = krhino_queue_del(g_test_queue0);
    QUEUE_VAL_CHK(ret == RHINO_KOBJ_DEL_ERR);

    krhino_sem_create(&sem, "test_sem ", 0);
    ret = krhino_queue_dyn_del((kqueue_t *)&sem);
    QUEUE_VAL_CHK(ret == RHINO_KOBJ_TYPE_ERR);
    krhino_sem_del(&sem);
}

static void task_queue0_entry(void *arg)
{
    kstat_t ret;

    while (1) {
        /* check krhino_queue_back_send param */
        queue_back_send_param_test();

        /* check krhino_queue_back_send */
        ret = krhino_queue_recv(g_test_queue0, RHINO_WAIT_FOREVER, &queue_recv_msg);
        QUEUE_VAL_CHK(ret == RHINO_SUCCESS);

        if (queue_recv_msg == queue_send_msg) {
            test_case_success++;
            PRINT_RESULT("queue back send", PASS);
        } else {
            test_case_fail++;
            PRINT_RESULT("queue back send", FAIL);
        }

        krhino_queue_dyn_del_param_test();
        ret = krhino_queue_dyn_del(g_test_queue0);
        QUEUE_VAL_CHK(ret == RHINO_SUCCESS);
        next_test_case_notify();
        krhino_task_dyn_del(task_0_test);
    }
}

static void task_queue1_entry(void *arg)
{
    kstat_t ret;

    while (1) {
        /* check krhino_queue_back_send */
        ret = krhino_queue_back_send(g_test_queue0, queue_send_msg);
        QUEUE_VAL_CHK(ret == RHINO_SUCCESS);

        krhino_task_dyn_del(task_1_test);
    }
}

kstat_t task_queue_back_send_test(void)
{
    kstat_t ret;

    krhino_queue_dyn_create_param_test();

    ret = krhino_queue_dyn_create(&g_test_queue0, "test_queue0",
                                  TEST_QUEUE_MSG0_SIZE);
    QUEUE_VAL_CHK(ret == RHINO_SUCCESS);

    ret = krhino_task_dyn_create(&task_0_test, "task_queue0_test", 0, 10,
                                 0, TASK_TEST_STACK_SIZE, task_queue0_entry, 1);
    QUEUE_VAL_CHK((ret == RHINO_SUCCESS) || (ret == RHINO_STOPPED));

    ret = krhino_task_dyn_create(&task_1_test, "task_queue1_test", 0, 11,
                                 0, TASK_TEST_STACK_SIZE, task_queue1_entry, 1);
    QUEUE_VAL_CHK((ret == RHINO_SUCCESS) || (ret == RHINO_STOPPED));

    return 0;
}

