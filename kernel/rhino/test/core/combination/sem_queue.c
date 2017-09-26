/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "comb_test.h"

#if (RHINO_CONFIG_KOBJ_SET > 0)
static ktask_t *task_select;
static ktask_t *task_queue_trigger;

static kobj_set_t  *handle;
static kqueue_t     queue1;
static kqueue_t     queue2;
static blk_obj_t   *select_obj;

#define TEST_MSG_SIZE   8
#define OBJ_SET_COUNT   8

static void *queue1_msg_buff[TEST_MSG_SIZE];
void *queue2_msg_buff[TEST_MSG_SIZE];

#define MSG_SIGNATURE   0x5A
static void *msg_word_send;
static void *msg_word_recv;

#define MODULE_NAME    "sem_queue_opr"

static void task_sem_opr_entry(void *arg)
{
    kstat_t ret;

    ret = krhino_kobj_select(handle, &select_obj, RHINO_WAIT_FOREVER);

    TEST_FW_VAL_CHK(MODULE_NAME, ret == RHINO_SUCCESS);
    /* queue1 will be selected first */
    TEST_FW_VAL_CHK(MODULE_NAME, ((size_t)select_obj == (size_t)(&queue1)));

    if (test_case_check_err == 0) {
        ret = krhino_queue_recv((kqueue_t *)select_obj, RHINO_NO_WAIT, &msg_word_recv);
        TEST_FW_VAL_CHK(MODULE_NAME, ret == RHINO_SUCCESS);
        TEST_FW_VAL_CHK(MODULE_NAME, *((uint8_t *)&msg_word_recv) == MSG_SIGNATURE);
        msg_word_recv = NULL;
    }

    ret = krhino_kobj_select(handle, &select_obj, RHINO_WAIT_FOREVER);
    TEST_FW_VAL_CHK(MODULE_NAME, ret == RHINO_SUCCESS);
    /* queue2 will be selected next */
    TEST_FW_VAL_CHK(MODULE_NAME, ((size_t)select_obj == (size_t)(&queue2)));

    if (test_case_check_err == 0) {
        ret = krhino_queue_recv((kqueue_t *)select_obj, RHINO_NO_WAIT, &msg_word_recv);
        TEST_FW_VAL_CHK(MODULE_NAME, ret == RHINO_SUCCESS);
        TEST_FW_VAL_CHK(MODULE_NAME, *((uint8_t *)&msg_word_recv) == MSG_SIGNATURE);
        msg_word_recv = NULL;
    }

    ret = krhino_kobj_set_rm((blk_obj_t *)&queue1);
    TEST_FW_VAL_CHK(MODULE_NAME, ret == RHINO_SUCCESS);

    ret = krhino_kobj_set_rm((blk_obj_t *)&queue2);
    TEST_FW_VAL_CHK(MODULE_NAME, ret == RHINO_SUCCESS);

    ret = krhino_kobj_set_dyn_del(handle);
    TEST_FW_VAL_CHK(MODULE_NAME, ret == RHINO_SUCCESS);

    ret = krhino_queue_del(&queue1);
    TEST_FW_VAL_CHK(MODULE_NAME, ret == RHINO_SUCCESS);

    ret = krhino_queue_del(&queue2);
    TEST_FW_VAL_CHK(MODULE_NAME, ret == RHINO_SUCCESS);

    if (test_case_check_err == 0) {
        PRINT_RESULT("sem queue test", PASS);
    }

    next_test_case_notify();
    krhino_task_dyn_del(krhino_cur_task_get());
}

static void task_queue_trigger_entry(void *arg)
{
    *((uint8_t *)&msg_word_send) = MSG_SIGNATURE;

    krhino_queue_front_send(&queue1, (void *)msg_word_send);
    krhino_queue_back_send(&queue2, (void *)msg_word_send);

    krhino_task_dyn_del(krhino_cur_task_get());
}

/*
 the sample case show how to work in event chain mode
 */
void sem_queue_coopr_test(void)
{
    kstat_t ret;

    test_case_check_err = 0;

    ret = krhino_kobj_set_dyn_create(&handle, "obj_set", OBJ_SET_COUNT);
    TEST_FW_VAL_CHK(MODULE_NAME, ret == RHINO_SUCCESS);

    krhino_queue_create(&queue1, "queue1", (void **)&queue1_msg_buff, TEST_MSG_SIZE);
    krhino_queue_create(&queue2, "queue2", (void **)&queue2_msg_buff, TEST_MSG_SIZE);

    krhino_kobj_set_insert((blk_obj_t *)&queue1, handle);
    krhino_kobj_set_insert((blk_obj_t *)&queue2, handle);

    ret = krhino_task_dyn_create(&task_select, MODULE_NAME, 0, TASK_COMB_PRI,
                                 0, TASK_TEST_STACK_SIZE, task_sem_opr_entry, 1);

    TEST_FW_VAL_CHK(MODULE_NAME, ret == RHINO_SUCCESS);

    ret = krhino_task_dyn_create(&task_queue_trigger, MODULE_NAME, 0,
                                 TASK_COMB_PRI + 1,
                                 0, TASK_TEST_STACK_SIZE, task_queue_trigger_entry, 1);

    TEST_FW_VAL_CHK(MODULE_NAME, ret == RHINO_SUCCESS);

}
#endif

