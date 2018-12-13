/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <k_api.h>
#include <test_fw.h>

#include "buf_queue_test.h"

#define TEST_BUFQUEUE_MSG0_SIZE 8
#define TEST_BUFQUEUE_BUF0_SIZE 16
#define TEST_BUFQUEUE_MSG_MAX   8

static ktask_t     *task_0_test;
static char         g_test_send_msg0[TEST_BUFQUEUE_MSG0_SIZE];
static char         g_test_bufqueue_buf0[TEST_BUFQUEUE_MSG0_SIZE];
static kbuf_queue_t g_test_bufqueue0;

static void buf_queue_info_get_param_test(void)
{
    kstat_t ret;
    kbuf_queue_info_t info;
    ksem_t  sem;

    ret = krhino_buf_queue_info_get(NULL, &info);
    BUFQUEUE_VAL_CHK(ret == RHINO_NULL_PTR);

    ret = krhino_buf_queue_info_get(&g_test_bufqueue0, NULL);
    BUFQUEUE_VAL_CHK(ret == RHINO_NULL_PTR);

    krhino_sem_create(& sem, "test_sem ", 0);
    ret = krhino_buf_queue_info_get((kbuf_queue_t *)&sem, &info);
    BUFQUEUE_VAL_CHK(ret == RHINO_KOBJ_TYPE_ERR);
    krhino_sem_del(&sem);
}

static void task_queue0_entry(void *arg)
{
    kstat_t ret;
    kbuf_queue_info_t info;

    while (1) {
        ret = krhino_buf_queue_create(&g_test_bufqueue0, "test_bufqueue0",
                                      g_test_bufqueue_buf0,
                                      TEST_BUFQUEUE_BUF0_SIZE, TEST_BUFQUEUE_MSG_MAX);

        BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

        /* check krhino_buf_queue_info_get param */
        buf_queue_info_get_param_test();

        ret = krhino_buf_queue_info_get(&g_test_bufqueue0, &info);
        BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);
        BUFQUEUE_VAL_CHK((info.free_buf_size == TEST_BUFQUEUE_BUF0_SIZE) &&
                         (info.buf_size == TEST_BUFQUEUE_BUF0_SIZE));

        ret = krhino_buf_queue_send(&g_test_bufqueue0, g_test_send_msg0,
                                    TEST_BUFQUEUE_MSG_MAX);

        BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);


        ret = krhino_buf_queue_info_get(&g_test_bufqueue0, &info);
        BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

        if (test_case_check_err == 0) {
            test_case_success++;
            PRINT_RESULT("buf queue info get", PASS);
        } else {
            test_case_check_err = 0;
            test_case_fail++;
            PRINT_RESULT("buf queue info get", FAIL);
        }

        next_test_case_notify();
        krhino_task_dyn_del(task_0_test);
    }
}

kstat_t task_buf_queue_info_get_test(void)
{
    kstat_t ret;

    ret = krhino_task_dyn_create(&task_0_test, "task_bufqueue0_test", 0, 10,
                                 0, TASK_TEST_STACK_SIZE, task_queue0_entry, 1);
    BUFQUEUE_VAL_CHK((ret == RHINO_SUCCESS) || (ret == RHINO_STOPPED));

    return 0;
}

