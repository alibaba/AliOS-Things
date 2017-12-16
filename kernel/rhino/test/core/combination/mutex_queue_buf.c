/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "comb_test.h"

static ktask_t *task_mutex;
static ktask_t *task_buf_queue;
static ktask_t *task_buf_queue_trigger;

static kmutex_t     *test_mutex;
static kbuf_queue_t test_buf_queue;
static uint8_t buf_queue_test_buf[1];
static uint8_t buf_queue_recv[1];
static uint8_t buf_queue_send[1];

static uint8_t notify_flag;

#define MODULE_NAME    "mutex_queue_buf_opr"

static void task_mutex_opr_entry(void *arg)
{
    kstat_t ret;

    krhino_mutex_lock(test_mutex, RHINO_WAIT_FOREVER);

    while (notify_flag != 0x5a) {
        krhino_task_sleep(5);
    }

    ret = krhino_mutex_unlock(test_mutex);
    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT(MODULE_NAME, PASS);
    } else {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }

    next_test_case_notify();
    ret = krhino_mutex_dyn_del(test_mutex);
    if (ret != RHINO_SUCCESS) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }

    krhino_task_dyn_del(krhino_cur_task_get());
}

static void task_buf_queue_entry(void *arg)
{
    kstat_t ret;
    size_t  size;

    ret = krhino_buf_queue_recv(&test_buf_queue, RHINO_WAIT_FOREVER,
                                (void *)buf_queue_recv, &size);
    if ((ret == RHINO_SUCCESS) && (*(uint8_t *)buf_queue_recv == 0x5a)) {
        notify_flag = 0x5a;
        krhino_buf_queue_del(&test_buf_queue);
        krhino_task_dyn_del(krhino_cur_task_get());
    }
}

static void task_buf_queue_trigger_entry(void *arg)
{
    *(uint8_t *)buf_queue_send = 0x5a;

    krhino_buf_queue_send(&test_buf_queue, (void *)buf_queue_send, 1);
    krhino_task_dyn_del(krhino_cur_task_get());
}

void mutex_buf_queue_coopr_test(void)
{
    kstat_t ret;

    krhino_mutex_dyn_create(&test_mutex, "mutextest");
    krhino_buf_queue_create(&test_buf_queue, "bugqueue", (void *)buf_queue_test_buf,
                            8, 1);

    ret = krhino_task_dyn_create(&task_mutex, MODULE_NAME, 0, TASK_COMB_PRI,
                                 0, TASK_TEST_STACK_SIZE, task_mutex_opr_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }

    ret = krhino_task_dyn_create(&task_buf_queue, MODULE_NAME, 0, TASK_COMB_PRI + 1,
                                 0, TASK_TEST_STACK_SIZE, task_buf_queue_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }

    ret = krhino_task_dyn_create(&task_buf_queue_trigger, MODULE_NAME, 0,
                                 TASK_COMB_PRI + 2,
                                 0, TASK_TEST_STACK_SIZE, task_buf_queue_trigger_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }
}

