/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <k_api.h>
#include <test_fw.h>

#include "buf_queue_test.h"

#define TEST_BUFQUEUE_MSG_MAX   4
#define TEST_BUFQUEUE_MAX_NUM   10
#define TEST_BUFQUEUE_SIZE      100
#define TEST_BUFQUEUE_MSG0_SIZE 10

static ktask_t      *task_0_test;
static ktask_t      *task_1_test;
static kbuf_queue_t *g_test_bufqueue0;
static char          g_test_recv_msg0[TEST_BUFQUEUE_MSG0_SIZE];

static void queue_dyn_create_param_test(void)
{
    kstat_t ret;

    ret = krhino_buf_queue_dyn_create(NULL, "test_bufqueue0", 0,
                                      TEST_BUFQUEUE_MSG_MAX);
    BUFQUEUE_VAL_CHK(ret == RHINO_NULL_PTR);

    ret = krhino_buf_queue_dyn_create(&g_test_bufqueue0, NULL, 4,
                                      TEST_BUFQUEUE_MSG_MAX);
    BUFQUEUE_VAL_CHK(ret == RHINO_NULL_PTR);

    ret = krhino_buf_queue_dyn_create(&g_test_bufqueue0, NULL, 0,
                                      TEST_BUFQUEUE_MSG_MAX);
    BUFQUEUE_VAL_CHK(ret == RHINO_BUF_QUEUE_SIZE_ZERO);
}

static void queue_dyn_del_param_test(void)
{
    kstat_t ret;
    ksem_t  sem;
    ret = krhino_buf_queue_dyn_del(NULL);
    BUFQUEUE_VAL_CHK(ret == RHINO_NULL_PTR);

    ret = krhino_buf_queue_dyn_create(&g_test_bufqueue0, "test_bufqueue0",
                                      100, TEST_BUFQUEUE_MSG_MAX);
    BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

    ret = krhino_buf_queue_del(g_test_bufqueue0);
    BUFQUEUE_VAL_CHK(ret == RHINO_KOBJ_DEL_ERR);

    ret = krhino_buf_queue_dyn_del(g_test_bufqueue0);
    BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);


    krhino_sem_create(& sem, "test_sem ", 0);
    ret = krhino_buf_queue_dyn_del((kbuf_queue_t *)&sem);
    BUFQUEUE_VAL_CHK(ret == RHINO_KOBJ_TYPE_ERR);
    krhino_sem_del(&sem);
}

static void task_queue0_entry(void *arg)
{
    int     i;
    size_t  size;
    kstat_t ret = 0;

    while (1) {
        /* check krhino_buf_queue_dyn_create param */
        queue_dyn_create_param_test();

        /* check krhino_buf_queue_del param */
        queue_dyn_del_param_test();

        for (i = 1; i < TEST_BUFQUEUE_MAX_NUM; i++) {
            ret = krhino_buf_queue_dyn_create(&g_test_bufqueue0, "test_bufqueue0",
                                              i * 8, TEST_BUFQUEUE_MSG_MAX);
            BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

            ret = krhino_buf_queue_dyn_del(g_test_bufqueue0);
            BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);
        }

        ret = krhino_buf_queue_dyn_create(&g_test_bufqueue0, "test_bufqueue0",
                                          TEST_BUFQUEUE_SIZE, TEST_BUFQUEUE_MSG_MAX);
        BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_buf_queue_recv(g_test_bufqueue0, RHINO_WAIT_FOREVER,
                                    g_test_recv_msg0, &size);
        BUFQUEUE_VAL_CHK(ret == RHINO_BLK_DEL);

        ret = krhino_buf_queue_dyn_del(g_test_bufqueue0);
        BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

        krhino_task_dyn_del(task_0_test);
    }
}

static void task_queue1_entry(void *arg)
{
    if (test_case_check_err == 0) {
        test_case_success++;
        PRINT_RESULT("buf queue dyn create&del", PASS);
    } else {
        test_case_check_err = 0;
        test_case_fail++;
        PRINT_RESULT("buf queue dyn create&del", FAIL);
    }

    next_test_case_notify();
    krhino_task_dyn_del(task_1_test);
}

kstat_t task_buf_queue_dyn_create_test(void)
{
    kstat_t ret;

    ret = krhino_task_dyn_create(&task_0_test, "task_bufqueue0_test", 0, 10,
                                 0, TASK_TEST_STACK_SIZE, task_queue0_entry, 1);
    BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

    ret = krhino_task_dyn_create(&task_1_test, "task_bufqueue0_test", 0, 11,
                                 0, TASK_TEST_STACK_SIZE, task_queue1_entry, 1);
    BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

    return 0;
}

