/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "mutex_test.h"

#define MODULE_NAME     "mutex_opr"
#define MODULE_NAME_CO1 "mutex_coopr1"
#define MODULE_NAME_CO2 "mutex_coopr2"

static uint8_t mutex_opr_case1(void)
{
    kstat_t ret;
    uint8_t old_pri;

    ret = krhino_mutex_create(&test_mutex, MODULE_NAME);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_mutex_unlock(&test_mutex);
    MYASSERT(ret == RHINO_MUTEX_NOT_RELEASED_BY_OWNER);

    ret = krhino_mutex_lock(&test_mutex, RHINO_NO_WAIT);
    MYASSERT(ret == RHINO_SUCCESS);

    krhino_task_pri_change(krhino_cur_task_get(), TASK_MUTEX_PRI, &old_pri);

    ret = krhino_mutex_lock(&test_mutex, RHINO_NO_WAIT);
    MYASSERT(ret == RHINO_MUTEX_OWNER_NESTED);

    ret = krhino_mutex_unlock(&test_mutex);
    MYASSERT(ret == RHINO_MUTEX_OWNER_NESTED);

    ret = krhino_mutex_unlock(&test_mutex);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_mutex_del(&test_mutex);
    MYASSERT(ret == RHINO_SUCCESS);

    return 0;
}

static const test_func_t mutex_func_runner[] = {
    mutex_opr_case1,
    NULL
};

void mutex_opr_test(void)
{
    kstat_t ret;

    task_mutex_entry_register(MODULE_NAME, (test_func_t *)mutex_func_runner,
                              sizeof(mutex_func_runner) / sizeof(test_case_t));

    ret = krhino_task_dyn_create(&task_mutex, MODULE_NAME, 0, TASK_MUTEX_PRI,
                                 0, TASK_TEST_STACK_SIZE, task_mutex_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }
}

static uint8_t task_pri_get(ktask_t *task)
{
    CPSR_ALLOC();

    uint8_t pri;

    RHINO_CRITICAL_ENTER();

    pri = task->prio;

    RHINO_CRITICAL_EXIT();

    return pri;
}

void task_mutex_coopr1_co1_entry(void *arg)
{
    kstat_t ret;

    while (1) {
        krhino_task_sleep(5);

        krhino_sched_disable();
        ret = krhino_mutex_lock(&test_mutex, RHINO_WAIT_FOREVER);
        if (ret != RHINO_SCHED_DISABLE) {
            test_case_fail++;
            PRINT_RESULT(MODULE_NAME_CO1, FAIL);
        }
        krhino_sched_enable();

        ret = krhino_mutex_lock(&test_mutex, RHINO_NO_WAIT);
        if (ret != RHINO_NO_PEND_WAIT) {
            test_case_fail++;
            PRINT_RESULT(MODULE_NAME_CO1, FAIL);
        }

        ret = krhino_mutex_lock(&test_mutex, RHINO_WAIT_FOREVER);
        if (ret != RHINO_SUCCESS) {
            test_case_fail++;
            PRINT_RESULT(MODULE_NAME_CO1, FAIL);
        }

        krhino_mutex_unlock(&test_mutex);

        break;
    }

    krhino_task_dyn_del(krhino_cur_task_get());
}

void task_mutex_coopr1_co2_entry(void *arg)
{
    uint8_t pri;

    krhino_mutex_create(&test_mutex, MODULE_NAME_CO1);

    while (1) {
        krhino_mutex_lock(&test_mutex, RHINO_WAIT_FOREVER);
        krhino_task_sleep(10);

        /* now, the task's priority is revert */
        pri = task_pri_get(krhino_cur_task_get());
        if (pri != TASK_MUTEX_PRI) {
            test_case_fail++;
            PRINT_RESULT(MODULE_NAME_CO1, FAIL);
            krhino_mutex_del(&test_mutex);

            next_test_case_notify();
            krhino_task_dyn_del(krhino_cur_task_get());

            return;
        } else {
            krhino_task_dyn_del(task_mutex_co1);
            krhino_mutex_unlock(&test_mutex);
            break;
        }
    }

    test_case_success++;
    PRINT_RESULT(MODULE_NAME_CO1, PASS);

    krhino_mutex_del(&test_mutex);

    next_test_case_notify();
    krhino_task_dyn_del(krhino_cur_task_get());
}

/* the case is to test a mutex task owner 's priority revert in case of another higher
 * priority task try to get mutex */
void mutex_coopr1_test(void)
{
    kstat_t ret;

    ret = krhino_task_dyn_create(&task_mutex_co1, MODULE_NAME_CO1, 0, TASK_MUTEX_PRI,
                                 0, TASK_TEST_STACK_SIZE, task_mutex_coopr1_co1_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }

    ret = krhino_task_dyn_create(&task_mutex_co2, MODULE_NAME_CO1, 0,
                                 TASK_MUTEX_PRI + 1,
                                 0, TASK_TEST_STACK_SIZE, task_mutex_coopr1_co2_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }
}

static void task_mutex_coopr2_co1_entry(void *arg)
{
    kstat_t ret;

    while (1) {
        krhino_task_sleep(10);

        ret = krhino_mutex_lock(&test_mutex_co1, RHINO_WAIT_FOREVER);
        if (ret != RHINO_SUCCESS) {
            break;
        }

        krhino_mutex_unlock(&test_mutex_co1);

        break;
    }

    krhino_task_dyn_del(krhino_cur_task_get());
}

static void task_mutex_coopr2_co2_entry(void *arg)
{
    while (1) {
        krhino_task_sleep(10);

        krhino_mutex_lock(&test_mutex_co2, RHINO_WAIT_FOREVER);
        krhino_mutex_lock(&test_mutex_co2, RHINO_WAIT_FOREVER);

        break;
    }

    krhino_task_dyn_del(krhino_cur_task_get());
}

static void task_mutex_coopr2_co3_entry(void *arg)
{
    uint8_t pri;

    krhino_mutex_create(&test_mutex_co1, MODULE_NAME_CO2);
    krhino_mutex_create(&test_mutex_co2, MODULE_NAME_CO2);

    while (1) {
        krhino_mutex_lock(&test_mutex_co1, RHINO_WAIT_FOREVER);
        krhino_mutex_lock(&test_mutex_co2, RHINO_WAIT_FOREVER);

        krhino_task_sleep(20);
        pri = task_pri_get(krhino_cur_task_get());
        if (pri == TASK_MUTEX_PRI) {
            krhino_mutex_unlock(&test_mutex_co1);

            pri = task_pri_get(krhino_cur_task_get());
            if (pri == TASK_MUTEX_PRI + 1) {
                krhino_mutex_unlock(&test_mutex_co2);

                pri = task_pri_get(krhino_cur_task_get());
                if (pri == TASK_MUTEX_PRI + 2) {
                    break;
                } else {
                    test_case_fail++;
                    PRINT_RESULT(MODULE_NAME_CO2, FAIL);

                    krhino_mutex_del(&test_mutex);

                    next_test_case_notify();
                    krhino_task_dyn_del(krhino_cur_task_get());

                    return;
                }
            } else {
                test_case_fail++;
                PRINT_RESULT(MODULE_NAME_CO2, FAIL);

                krhino_mutex_del(&test_mutex);

                next_test_case_notify();
                krhino_task_dyn_del(krhino_cur_task_get());

                return;
            }
        } else {
            test_case_fail++;
            PRINT_RESULT(MODULE_NAME_CO2, FAIL);

            krhino_mutex_del(&test_mutex);

            next_test_case_notify();
            krhino_task_dyn_del(krhino_cur_task_get());

            return;
        }

    }

    test_case_success++;
    PRINT_RESULT(MODULE_NAME_CO2, PASS);

    krhino_mutex_del(&test_mutex);

    next_test_case_notify();
    krhino_task_dyn_del(krhino_cur_task_get());
}


void mutex_coopr2_test(void)
{
    kstat_t ret;

    ret = krhino_task_dyn_create(&task_mutex_co1, MODULE_NAME_CO2, 0, TASK_MUTEX_PRI,
                                 0, TASK_TEST_STACK_SIZE, task_mutex_coopr2_co1_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }

    ret = krhino_task_dyn_create(&task_mutex_co2, MODULE_NAME_CO2, 0,
                                 TASK_MUTEX_PRI + 1,
                                 0, TASK_TEST_STACK_SIZE, task_mutex_coopr2_co2_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }

    ret = krhino_task_dyn_create(&task_mutex_co3, MODULE_NAME_CO2, 0,
                                 TASK_MUTEX_PRI + 2,
                                 0, TASK_TEST_STACK_SIZE, task_mutex_coopr2_co3_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }
}

