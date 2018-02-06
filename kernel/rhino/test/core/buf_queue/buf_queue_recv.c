/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <k_api.h>
#include <test_fw.h>

#include "buf_queue_test.h"

#define TEST_BUFQUEUE_MSG0_SIZE     TEST_BUFQUEUE_MSG_MAX+1u
#define TEST_BUFQUEUE_BUF0_ERR_SIZE 43
#define TEST_BUFQUEUE_BUF0_SIZE     48
#define TEST_BUFQUEUE_MSG_NUM       3
#define TEST_BUFQUEUE_MSG_MAX       (TEST_BUFQUEUE_BUF0_SIZE/TEST_BUFQUEUE_MSG_NUM)-sizeof(size_t)
/* four char here */
static char send_char[TEST_BUFQUEUE_MSG_NUM + 1] = "aos";

#define TEST_BUFQUEUE_RCV_TASK_RPI       10
#define TEST_BUFQUEUE_SND_TASK_HIGH_RPI  9
#define TEST_BUFQUEUE_SND_TASK_LOW_RPI   11

static ktask_t      *task_0_test;
static ktask_t      *task_1_test;
static ktask_t      *task_2_test;
static char         g_test_send_msg0[TEST_BUFQUEUE_MSG0_SIZE]     = {0};
static char         g_test_send_msg1[TEST_BUFQUEUE_MSG0_SIZE]     = {0};
static char         g_test_recv_msg0[TEST_BUFQUEUE_MSG0_SIZE]     = {0};
static char         g_test_bufqueue_buf0[TEST_BUFQUEUE_BUF0_SIZE] = {0};
static size_t       g_test_bufqueue_buf1[2] = {0};

static kbuf_queue_t g_test_bufqueue0;
static kbuf_queue_t g_test_bufqueue1;

static void buf_queue_recv_param_test(void)
{
    kstat_t ret;
    size_t  size;
    ksem_t  sem;

    memset(&sem, 0, sizeof(ksem_t));

    ret = krhino_buf_queue_recv(NULL, RHINO_WAIT_FOREVER, &g_test_recv_msg0, &size);
    BUFQUEUE_VAL_CHK(ret == RHINO_NULL_PTR);

    ret = krhino_buf_queue_recv(&g_test_bufqueue0, RHINO_WAIT_FOREVER, NULL, &size);
    BUFQUEUE_VAL_CHK(ret == RHINO_NULL_PTR);

    ret = krhino_buf_queue_recv(&g_test_bufqueue0, RHINO_WAIT_FOREVER,
                                &g_test_recv_msg0, NULL);
    BUFQUEUE_VAL_CHK(ret == RHINO_NULL_PTR);

    krhino_sem_create(& sem, "test_sem ", 0);
    ret = krhino_buf_queue_recv((kbuf_queue_t *)&sem, RHINO_WAIT_FOREVER,
                                &g_test_recv_msg0, &size);
    BUFQUEUE_VAL_CHK(ret == RHINO_KOBJ_TYPE_ERR);
    krhino_sem_del(&sem);
}

static void buf_queue_send_param_test(void)
{
    kstat_t ret;
    ksem_t  sem;

    memset(&sem, 0, sizeof(ksem_t));

    ret = krhino_buf_queue_send(&g_test_bufqueue0, NULL, TEST_BUFQUEUE_MSG_MAX);
    BUFQUEUE_VAL_CHK(ret == RHINO_NULL_PTR);


    ret = krhino_buf_queue_send(NULL, g_test_send_msg0, TEST_BUFQUEUE_MSG_MAX);
    BUFQUEUE_VAL_CHK(ret == RHINO_NULL_PTR);

    ret = krhino_buf_queue_send(&g_test_bufqueue0, g_test_send_msg0,
                                TEST_BUFQUEUE_MSG_MAX + 1);
    BUFQUEUE_VAL_CHK(ret == RHINO_BUF_QUEUE_MSG_SIZE_OVERFLOW);


    ret = krhino_sem_create(&sem, "test_sem ", 0);
    BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

    g_test_bufqueue0.blk_obj.obj_type = RHINO_OBJ_TYPE_NONE;
    ret = krhino_buf_queue_send(&g_test_bufqueue0, g_test_send_msg0,
                                TEST_BUFQUEUE_MSG_MAX);
    g_test_bufqueue0.blk_obj.obj_type = RHINO_BUF_QUEUE_OBJ_TYPE;
    BUFQUEUE_VAL_CHK(ret == RHINO_KOBJ_TYPE_ERR);

    ret = krhino_buf_queue_send((kbuf_queue_t *)&sem, g_test_send_msg0, 0);
    BUFQUEUE_VAL_CHK(ret == RHINO_KOBJ_TYPE_ERR);
    krhino_sem_del(&sem);
}


static void task_queue1_entry(void *arg)
{
    kstat_t ret;
    kbuf_queue_info_t info;

    while (1) {
        memset(g_test_send_msg0, 'y', TEST_BUFQUEUE_MSG_MAX);
        ret = krhino_buf_queue_send(&g_test_bufqueue0, g_test_send_msg0,
                                    TEST_BUFQUEUE_MSG_MAX);
        BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

        memset(g_test_send_msg0, 'o', TEST_BUFQUEUE_MSG_MAX);
        ret = krhino_buf_queue_send(&g_test_bufqueue0, g_test_send_msg0,
                                          TEST_BUFQUEUE_MSG_MAX);
        BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

        memset(g_test_send_msg0, 's', TEST_BUFQUEUE_MSG_MAX);
        ret = krhino_buf_queue_send(&g_test_bufqueue0, g_test_send_msg0,
                                    TEST_BUFQUEUE_MSG_MAX);
        BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

        krhino_task_dyn_del(task_1_test);
    }
}

static void task_queue2_entry(void *arg)
{
    kstat_t ret;
    kbuf_queue_info_t info;
    size_t count = 0;

    while (1) {

        memset(g_test_send_msg1, 's', TEST_BUFQUEUE_MSG_MAX);
        ret = krhino_buf_queue_send(&g_test_bufqueue0, g_test_send_msg1,
                                          TEST_BUFQUEUE_MSG_MAX);
        BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);
        count++;

        memset(g_test_send_msg1, 'o', TEST_BUFQUEUE_MSG_MAX);
        ret = krhino_buf_queue_send(&g_test_bufqueue0, g_test_send_msg1,
                                          TEST_BUFQUEUE_MSG_MAX);
        BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);
        count++;


        memset(g_test_send_msg1, 'y', TEST_BUFQUEUE_MSG_MAX);
        ret = krhino_buf_queue_send(&g_test_bufqueue0, g_test_send_msg1,
                                          TEST_BUFQUEUE_MSG_MAX);
        BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);
        count++;


        memset(g_test_send_msg1, 'w', TEST_BUFQUEUE_MSG_MAX);
        ret = krhino_buf_queue_send(&g_test_bufqueue0, g_test_send_msg1,
                                    TEST_BUFQUEUE_MSG_MAX);
        BUFQUEUE_VAL_CHK(ret == RHINO_BUF_QUEUE_FULL);

        krhino_buf_queue_info_get(&g_test_bufqueue0, &info);

        BUFQUEUE_VAL_CHK(count == info.cur_num);
        krhino_task_dyn_del(task_2_test);
    }
}

static void task_queue0_entry(void *arg)
{
    kstat_t ret;
    size_t  size;
    int     count = 0;
    size_t  msg;

    /* err param test */
    ret = krhino_buf_queue_create(&g_test_bufqueue0, "test_bufqueue0",
                                  (void *)g_test_bufqueue_buf0,
                                  TEST_BUFQUEUE_BUF0_SIZE, 0);

    BUFQUEUE_VAL_CHK(ret == RHINO_INV_PARAM);

    ret = krhino_buf_queue_create(&g_test_bufqueue0, "test_bufqueue0",
                                  (void *)g_test_bufqueue_buf0,
                                  TEST_BUFQUEUE_BUF0_SIZE, TEST_BUFQUEUE_MSG_MAX);

    BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

    ret = krhino_fix_buf_queue_create(&g_test_bufqueue1, "test_bufqueue1",
                                  (void *)g_test_bufqueue_buf1, sizeof(size_t), 2);

    BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

    msg = 0x11;
    ret = krhino_buf_queue_send(&g_test_bufqueue1, &msg, sizeof(size_t));
    BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

    ret = krhino_buf_queue_send(&g_test_bufqueue1, &msg, sizeof(size_t));
    BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

    ret = krhino_buf_queue_send(&g_test_bufqueue1, &msg, sizeof(size_t));
    BUFQUEUE_VAL_CHK(ret == RHINO_BUF_QUEUE_FULL);

    krhino_buf_queue_recv(&g_test_bufqueue1, RHINO_NO_WAIT, &msg, &size);
    krhino_buf_queue_recv(&g_test_bufqueue1, RHINO_NO_WAIT, &msg, &size);

    ret = krhino_buf_queue_recv(&g_test_bufqueue1, 1, &msg, &size);
    BUFQUEUE_VAL_CHK(ret == RHINO_BLK_TIMEOUT);

    /* check krhino_buf_queue_recv */
    buf_queue_recv_param_test();

    buf_queue_send_param_test();

    /* check RHINO_NO_WAIT */
    ret = krhino_buf_queue_recv(&g_test_bufqueue0, RHINO_NO_WAIT, &g_test_recv_msg0,
                                &size);
    BUFQUEUE_VAL_CHK(ret == RHINO_NO_PEND_WAIT);

    /* check sched disalbe */
    ret = krhino_sched_disable();
    BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);
    ret = krhino_buf_queue_recv(&g_test_bufqueue0, RHINO_WAIT_FOREVER,
                                &g_test_recv_msg0, &size);
    BUFQUEUE_VAL_CHK(ret == RHINO_SCHED_DISABLE);
    ret = krhino_sched_enable();
    BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

    ret = krhino_task_dyn_create(&task_1_test, "task_bufqueue1_test", 0,
                                 TEST_BUFQUEUE_SND_TASK_LOW_RPI,
                                 0, TASK_TEST_STACK_SIZE, task_queue1_entry, 1);

    BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

    do {
        ret = krhino_buf_queue_recv(&g_test_bufqueue0, RHINO_WAIT_FOREVER,
                                    g_test_recv_msg0, &size);
        BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

        memset(g_test_send_msg0, send_char[count], TEST_BUFQUEUE_MSG_MAX);
        ret = memcmp(g_test_send_msg0, g_test_recv_msg0, size);
        count ++;
    } while (count <  TEST_BUFQUEUE_MSG_NUM);


    ret = krhino_task_dyn_create(&task_2_test, "task_bufqueue2_test", 0,
                                 TEST_BUFQUEUE_SND_TASK_HIGH_RPI,
                                 0, TASK_TEST_STACK_SIZE, task_queue2_entry, 1);

    BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

    krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND / 10);

    count = 0;

    do {
        ret = krhino_buf_queue_recv(&g_test_bufqueue0, RHINO_WAIT_FOREVER,
                                    g_test_recv_msg0, &size);
        BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

        memset(g_test_send_msg1, send_char[count], TEST_BUFQUEUE_MSG_MAX);
        ret = memcmp(g_test_send_msg1, g_test_recv_msg0, size);
        count ++;
    } while (count <  TEST_BUFQUEUE_MSG_NUM);


    if (test_case_check_err == 0) {
        test_case_success++;
        PRINT_RESULT("buf queue recv", PASS);
    } else {
        test_case_check_err = 0;
        test_case_fail++;
        PRINT_RESULT("buf queue recv", FAIL);
    }

    next_test_case_notify();
    krhino_task_dyn_del(task_0_test);
}

kstat_t task_buf_queue_recv_test(void)
{
    kstat_t ret;
    test_case_check_err = 0;
    ret = krhino_task_dyn_create(&task_0_test, "task_bufqueue0_test", 0,
                                 TEST_BUFQUEUE_RCV_TASK_RPI,
                                 0, TASK_TEST_STACK_SIZE, task_queue0_entry, 1);
    BUFQUEUE_VAL_CHK(ret == RHINO_SUCCESS);

    return 0;
}

