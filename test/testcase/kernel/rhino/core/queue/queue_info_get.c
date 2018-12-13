/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <k_api.h>
#include <test_fw.h>

#include "queue_test.h"

#define TEST_QUEUE_MSG0_SIZE 2

static ktask_t  *task_0_test;
static ktask_t  *task_1_test;
static void    *queue_send_msg = (void *)1;
static void    *queue_recv_msg = (void *)1;
static void    *g_test_queue_msg0[TEST_QUEUE_MSG0_SIZE];
static kqueue_t g_test_queue0;

static void print_queue_info(msg_info_t *info)
{
#if 0
    printf("\nqueue info:\n");
    printf("msg_q.peak_num:    0x%x\n", (uint32_t)info->msg_q.peak_num);
    printf("msg_q.cur_num:     0x%x\n", (uint32_t)info->msg_q.cur_num);
    printf("msg_q.queue_start: 0x%x\n", (uint32_t)info->msg_q.queue_start);
    printf("msg_q.queue_end:   0x%x\n", (uint32_t)info->msg_q.queue_end);
    printf("msg_q.read:        0x%x\n", (uint32_t)info->msg_q.read);
    printf("msg_q.write:       0x%x\n", (uint32_t)info->msg_q.write);
    printf("msg_q.size:        0x%x\n", (uint32_t)info->msg_q.size);
    printf("pend_entry:        0x%x\n", (uint32_t)info->pend_entry);
#endif
}

static void queue_info_get_param_test(void)
{
    kstat_t    ret;
    msg_info_t info0;
    ksem_t     sem;

    ret = krhino_queue_info_get(NULL, &info0);
    QUEUE_VAL_CHK(ret == RHINO_NULL_PTR);

    ret = krhino_queue_info_get(&g_test_queue0, NULL);
    QUEUE_VAL_CHK(ret == RHINO_NULL_PTR);

    krhino_sem_create(&sem, "test_sem ", 0);
    ret = krhino_queue_info_get((kqueue_t *)&sem, &info0);
    QUEUE_VAL_CHK(ret == RHINO_KOBJ_TYPE_ERR);
    krhino_sem_del(&sem);
}

static void task_queue0_entry(void *arg)
{
    kstat_t    ret;
    msg_info_t info0;
    msg_info_t info1;

    while (1) {
        /* check krhino_info_get param */
        queue_info_get_param_test();

        ret = krhino_queue_info_get(&g_test_queue0, &info0);
        QUEUE_VAL_CHK(ret == RHINO_SUCCESS);
        print_queue_info(&info0);

        ret = krhino_queue_recv(&g_test_queue0, RHINO_WAIT_FOREVER, &queue_recv_msg);
        QUEUE_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_queue_info_get(&g_test_queue0, &info1);
        QUEUE_VAL_CHK(ret == RHINO_SUCCESS);
        print_queue_info(&info1);

        krhino_task_sleep(10);

        test_case_success++;
        PRINT_RESULT("queue info get", PASS);

        krhino_queue_del(&g_test_queue0);
        next_test_case_notify();
        krhino_task_dyn_del(task_0_test);
    }
}

static void task_queue1_entry(void *arg)
{
    kstat_t    ret;
    msg_info_t info0;

    while (1) {
        ret = krhino_queue_back_send(&g_test_queue0, queue_send_msg);
        QUEUE_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_queue_back_send(&g_test_queue0, queue_send_msg);
        QUEUE_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_queue_info_get(&g_test_queue0, &info0);
        QUEUE_VAL_CHK(ret == RHINO_SUCCESS);
        print_queue_info(&info0);

        krhino_task_dyn_del(task_1_test);
    }
}

kstat_t task_queue_info_get_test(void)
{
    kstat_t ret;

    ret = krhino_queue_create(&g_test_queue0, "test_queue0",
                              (void **)&g_test_queue_msg0, TEST_QUEUE_MSG0_SIZE);
    QUEUE_VAL_CHK(ret == RHINO_SUCCESS);

    ret = krhino_task_dyn_create(&task_0_test, "task_queue0_test", 0, 10,
                                 0, TASK_TEST_STACK_SIZE, task_queue0_entry, 1);
    QUEUE_VAL_CHK((ret == RHINO_SUCCESS) || (ret == RHINO_STOPPED));

    ret = krhino_task_dyn_create(&task_1_test, "task_queue1_test", 0, 11,
                                 0, TASK_TEST_STACK_SIZE, task_queue1_entry, 1);
    QUEUE_VAL_CHK((ret == RHINO_SUCCESS) || (ret == RHINO_STOPPED));

    return 0;
}

