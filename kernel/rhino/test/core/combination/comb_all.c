/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "comb_test.h"

static ktask_t *task_sem;
static ktask_t *task_mutex;
static ktask_t *task_event;
static ktask_t *task_queue;

static ksem_t  *sem_comb_all;
static kmutex_t mutex_comb_all;
static kevent_t event_comb_all;
static kqueue_t queue_comb_all;

#define TEST_MSG_SIZE   8
static void *queue1_msg_buff[TEST_MSG_SIZE];
void *queue1_trigger_msg;

#define MSG_SIGNATURE   0x5A
static void *msg_word;

static uint8_t mutex_count;

#define MODULE_NAME    "comb_all_opr"

static void task_sem_opr_entry(void *arg)
{
    kstat_t ret;

    ret = krhino_sem_take(sem_comb_all, RHINO_WAIT_FOREVER);
    if ((ret == RHINO_SUCCESS) && (mutex_count == 66)) {
        test_case_success++;
        PRINT_RESULT(MODULE_NAME, PASS);
    } else {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }

    krhino_sem_dyn_del(sem_comb_all);
    krhino_mutex_del(&mutex_comb_all);
    krhino_event_del(&event_comb_all);
    krhino_queue_del(&queue_comb_all);

    next_test_case_notify();
    krhino_task_dyn_del(krhino_cur_task_get());
}

static void task_mutex_opr_entry(void *arg)
{
    uint32_t flag;

    krhino_event_get(&event_comb_all, 0x1, RHINO_AND_CLEAR, &flag,
                     RHINO_WAIT_FOREVER);

    krhino_mutex_lock(&mutex_comb_all, RHINO_WAIT_FOREVER);
    krhino_mutex_lock(&mutex_comb_all, RHINO_WAIT_FOREVER);
    krhino_mutex_lock(&mutex_comb_all, RHINO_WAIT_FOREVER);
    krhino_mutex_lock(&mutex_comb_all, RHINO_WAIT_FOREVER);
    krhino_mutex_lock(&mutex_comb_all, RHINO_WAIT_FOREVER);
    krhino_mutex_lock(&mutex_comb_all, RHINO_WAIT_FOREVER);
    krhino_mutex_lock(&mutex_comb_all, RHINO_WAIT_FOREVER);
    krhino_mutex_lock(&mutex_comb_all, RHINO_WAIT_FOREVER);

    mutex_count = 55;

    krhino_mutex_unlock(&mutex_comb_all);
    krhino_mutex_unlock(&mutex_comb_all);
    krhino_mutex_unlock(&mutex_comb_all);
    krhino_mutex_unlock(&mutex_comb_all);
    krhino_mutex_unlock(&mutex_comb_all);
    krhino_mutex_unlock(&mutex_comb_all);
    krhino_mutex_unlock(&mutex_comb_all);
    krhino_mutex_unlock(&mutex_comb_all);

    mutex_count = 66;

    krhino_sem_give(sem_comb_all);
    krhino_task_dyn_del(krhino_cur_task_get());
}

static void task_event_opr_entry(void *arg)
{
    kstat_t ret;

    ret = krhino_queue_recv(&queue_comb_all, RHINO_WAIT_FOREVER,
                            (void *)&queue1_trigger_msg);
    if ((*(uint8_t *)&queue1_trigger_msg == MSG_SIGNATURE) &&
        (ret == RHINO_SUCCESS)) {
        krhino_event_set(&event_comb_all, 0x1, RHINO_OR);
        krhino_task_dyn_del(krhino_cur_task_get());
    }
}

static void task_queue_opr_entry(void *arg)
{
    *((char *)&msg_word) = MSG_SIGNATURE;
    krhino_queue_back_send(&queue_comb_all, (void *)msg_word);
    krhino_task_dyn_del(krhino_cur_task_get());
}

void comb_all_coopr_test(void)
{
    kstat_t ret;

    krhino_sem_dyn_create(&sem_comb_all, "semcomball", 0);
    krhino_mutex_create(&mutex_comb_all, "mutexcomball");
    krhino_event_create(&event_comb_all, "eventcomball", 0x2);
    krhino_queue_create(&queue_comb_all, "queue1", (void **)&queue1_msg_buff,
                        TEST_MSG_SIZE);

    ret = krhino_task_dyn_create(&task_sem, MODULE_NAME, 0, TASK_COMB_PRI,
                                 0, TASK_TEST_STACK_SIZE, task_sem_opr_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }

    ret = krhino_task_dyn_create(&task_mutex, MODULE_NAME, 0, TASK_COMB_PRI + 1,
                                 0, TASK_TEST_STACK_SIZE, task_mutex_opr_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }

    ret = krhino_task_dyn_create(&task_event, MODULE_NAME, 0, TASK_COMB_PRI + 2,
                                 0, TASK_TEST_STACK_SIZE, task_event_opr_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }

    ret = krhino_task_dyn_create(&task_queue, MODULE_NAME, 0, TASK_COMB_PRI + 3,
                                 0, TASK_TEST_STACK_SIZE, task_queue_opr_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }
}

