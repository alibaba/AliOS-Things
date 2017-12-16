/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "comb_test.h"

static ktask_t *task_sem;
static ktask_t *task_ksem_trigger;
static ktask_t *task_event_trigger;
static ksem_t  *sem_comb;
static kevent_t event_sem;

#define MODULE_NAME    "sem_event_opr"


static void task_sem_opr_entry(void *arg)
{
    kstat_t ret;

    ret = krhino_sem_take(sem_comb, RHINO_WAIT_FOREVER);
    if (ret == RHINO_SUCCESS) {
        test_case_success++;
        PRINT_RESULT(MODULE_NAME, PASS);
    } else {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }

    next_test_case_notify();
    krhino_sem_dyn_del(sem_comb);
    krhino_task_dyn_del(krhino_cur_task_get());
}

static void task_ksem_trigger_opr_entry(void *arg)
{
    kstat_t  ret;
    uint32_t flag;

    ret = krhino_event_get(&event_sem, 0x1, RHINO_AND_CLEAR, &flag,
                           RHINO_WAIT_FOREVER);
    if ((ret == RHINO_SUCCESS) && (flag == 0x3)) {
        krhino_sem_give(sem_comb);
        krhino_event_del(&event_sem);
        krhino_task_dyn_del(krhino_cur_task_get());
    }
}

static void task_event_trigger_opr_entry(void *arg)
{
    kstat_t ret;

    ret = krhino_event_set(&event_sem, 0x1, RHINO_OR);
    if (ret == RHINO_SUCCESS) {
        krhino_task_dyn_del(krhino_cur_task_get());
    }
}

void sem_event_coopr_test(void)
{
    kstat_t ret;

    krhino_sem_dyn_create(&sem_comb, "semtest", 0);
    krhino_event_create(&event_sem, "eventtest", 0x2);

    ret = krhino_task_dyn_create(&task_sem, MODULE_NAME, 0, TASK_COMB_PRI,
                                 0, TASK_TEST_STACK_SIZE, task_sem_opr_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }

    ret = krhino_task_dyn_create(&task_ksem_trigger, MODULE_NAME, 0,
                                 TASK_COMB_PRI + 1,
                                 0, TASK_TEST_STACK_SIZE, task_ksem_trigger_opr_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }

    ret = krhino_task_dyn_create(&task_event_trigger, MODULE_NAME, 0,
                                 TASK_COMB_PRI + 2,
                                 0, TASK_TEST_STACK_SIZE, task_event_trigger_opr_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }
}

