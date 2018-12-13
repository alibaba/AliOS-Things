/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "event_test.h"

#define MODULE_NAME     "event_opr"
#define MODULE_NAME_CO  "event_coopr"

#define TEST_FLAG         0x5a5a5a5a
#define CHK_AND_ALL_FLAG  0x5a5a5a5a
#define CHK_AND_ONE_FLAG  0x00000002
#define CHK_AND_ZERO_FLAG 0x00000000
#define CHK_AND_PEND_FLAG 0x5a5a5a55

static uint8_t event_opr_case1(void)
{
    kstat_t  ret;
    uint32_t actl_flags;
    CPSR_ALLOC();

    ret = krhino_event_create(&test_event, MODULE_NAME, TEST_FLAG);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(test_event.flags == TEST_FLAG);

    /* check event AND FLAG */
    RHINO_CRITICAL_ENTER();
    test_event.blk_obj.obj_type = RHINO_SEM_OBJ_TYPE;
    RHINO_CRITICAL_EXIT();
    ret = krhino_event_set(&test_event, TEST_FLAG, RHINO_OR);
    MYASSERT(ret == RHINO_KOBJ_TYPE_ERR);

    RHINO_CRITICAL_ENTER();
    test_event.blk_obj.obj_type = RHINO_EVENT_OBJ_TYPE;
    RHINO_CRITICAL_EXIT();

    ret = krhino_event_set(&test_event, TEST_FLAG, RHINO_AND);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(test_event.flags == TEST_FLAG);

    ret = krhino_event_set(&test_event, TEST_FLAG, RHINO_OR);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(test_event.flags == TEST_FLAG);

    RHINO_CRITICAL_ENTER();
    test_event.blk_obj.obj_type = RHINO_SEM_OBJ_TYPE;
    RHINO_CRITICAL_EXIT();
    ret = krhino_event_get(&test_event, CHK_AND_ALL_FLAG, RHINO_AND, &actl_flags,
                           RHINO_NO_WAIT);
    MYASSERT(ret == RHINO_KOBJ_TYPE_ERR);

    RHINO_CRITICAL_ENTER();
    test_event.blk_obj.obj_type = RHINO_EVENT_OBJ_TYPE;
    RHINO_CRITICAL_EXIT();
    ret = krhino_event_get(&test_event, CHK_AND_ALL_FLAG, RHINO_AND, &actl_flags,
                           RHINO_NO_WAIT);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(actl_flags == TEST_FLAG);
    MYASSERT(test_event.flags == TEST_FLAG);

    ret = krhino_event_get(&test_event, CHK_AND_ONE_FLAG, RHINO_AND, &actl_flags,
                           RHINO_NO_WAIT);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(actl_flags == TEST_FLAG);
    MYASSERT(test_event.flags == TEST_FLAG);

    ret = krhino_event_get(&test_event, CHK_AND_ZERO_FLAG, RHINO_AND, &actl_flags,
                           RHINO_NO_WAIT);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(actl_flags == TEST_FLAG);
    MYASSERT(test_event.flags == TEST_FLAG);

    ret = krhino_event_del(&test_event);
    MYASSERT(ret == RHINO_SUCCESS);

    return 0;
}

static uint8_t event_opr_case2(void)
{
    kstat_t  ret;
    uint32_t actl_flags;

    ret = krhino_event_create(&test_event, MODULE_NAME, TEST_FLAG);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(test_event.flags == TEST_FLAG);

    /* check event AND_CLEAR FLAG */
    ret = krhino_event_set(&test_event, TEST_FLAG, RHINO_OR);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(test_event.flags == TEST_FLAG);

    ret = krhino_event_get(&test_event, CHK_AND_ALL_FLAG, RHINO_AND_CLEAR,
                           &actl_flags, RHINO_NO_WAIT);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(actl_flags == TEST_FLAG);
    MYASSERT(test_event.flags == (TEST_FLAG & (~CHK_AND_ALL_FLAG)));

    ret = krhino_event_set(&test_event, TEST_FLAG, RHINO_OR);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(test_event.flags == TEST_FLAG);

    ret = krhino_event_get(&test_event, CHK_AND_ONE_FLAG, RHINO_AND_CLEAR,
                           &actl_flags, RHINO_NO_WAIT);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(actl_flags == TEST_FLAG);
    MYASSERT(test_event.flags == (TEST_FLAG & (~CHK_AND_ONE_FLAG)));

    ret = krhino_event_set(&test_event, TEST_FLAG, RHINO_OR);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(test_event.flags == TEST_FLAG);

    ret = krhino_event_get(&test_event, CHK_AND_ZERO_FLAG, RHINO_AND_CLEAR,
                           &actl_flags, RHINO_NO_WAIT);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(actl_flags == TEST_FLAG);
    MYASSERT(test_event.flags == (TEST_FLAG & (~CHK_AND_ZERO_FLAG)));

    ret = krhino_event_del(&test_event);
    MYASSERT(ret == RHINO_SUCCESS);

    return 0;
}

static uint8_t event_opr_case3(void)
{
    kstat_t  ret;
    uint32_t actl_flags;

    ret = krhino_event_create(&test_event, MODULE_NAME, TEST_FLAG);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(test_event.flags == TEST_FLAG);

    /* check event OR FLAG */
    ret = krhino_event_set(&test_event, TEST_FLAG, RHINO_OR);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(test_event.flags == TEST_FLAG);

    ret = krhino_event_get(&test_event, CHK_AND_ALL_FLAG, RHINO_OR, &actl_flags,
                           RHINO_NO_WAIT);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(actl_flags == TEST_FLAG);
    MYASSERT(test_event.flags == TEST_FLAG);

    ret = krhino_event_get(&test_event, CHK_AND_ONE_FLAG, RHINO_OR, &actl_flags,
                           RHINO_NO_WAIT);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(actl_flags == TEST_FLAG);
    MYASSERT(test_event.flags == TEST_FLAG);

    ret = krhino_event_get(&test_event, CHK_AND_ZERO_FLAG, RHINO_OR, &actl_flags,
                           RHINO_NO_WAIT);
    MYASSERT(ret == RHINO_NO_PEND_WAIT);

    ret = krhino_event_del(&test_event);
    MYASSERT(ret == RHINO_SUCCESS);

    return 0;
}

static uint8_t event_opr_case4(void)
{
    kstat_t  ret;
    uint32_t actl_flags;

    ret = krhino_event_create(&test_event, MODULE_NAME, TEST_FLAG);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(test_event.flags == TEST_FLAG);

    /* check event OR_CLEAR FLAG */
    ret = krhino_event_set(&test_event, TEST_FLAG, RHINO_OR);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(test_event.flags == TEST_FLAG);

    ret = krhino_event_get(&test_event, CHK_AND_ALL_FLAG, RHINO_OR_CLEAR,
                           &actl_flags, RHINO_NO_WAIT);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(actl_flags == TEST_FLAG);
    MYASSERT(test_event.flags == (TEST_FLAG & (~CHK_AND_ALL_FLAG)));

    ret = krhino_event_set(&test_event, TEST_FLAG, RHINO_OR);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(test_event.flags == TEST_FLAG);

    ret = krhino_event_get(&test_event, CHK_AND_ONE_FLAG, RHINO_OR_CLEAR,
                           &actl_flags, RHINO_NO_WAIT);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(actl_flags == TEST_FLAG);
    MYASSERT(test_event.flags == (TEST_FLAG & (~CHK_AND_ONE_FLAG)));

    ret = krhino_event_set(&test_event, TEST_FLAG, RHINO_OR);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(test_event.flags == TEST_FLAG);

    ret = krhino_event_del(&test_event);
    MYASSERT(ret == RHINO_SUCCESS);

    return 0;
}

static uint8_t event_opr_case5(void)
{
    kstat_t  ret;
    uint32_t actl_flags;

    ret = krhino_event_create(&test_event, MODULE_NAME, TEST_FLAG);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(test_event.flags == TEST_FLAG);

    /* try to get event flag in case of sched disable */
    ret = krhino_event_set(&test_event, TEST_FLAG, RHINO_OR);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(test_event.flags == TEST_FLAG);

    krhino_sched_disable();

    ret = krhino_event_get(&test_event, CHK_AND_PEND_FLAG, RHINO_AND, &actl_flags,
                           RHINO_WAIT_FOREVER);
    MYASSERT(ret == RHINO_SCHED_DISABLE);

    krhino_sched_enable();

    ret = krhino_event_del(&test_event);
    MYASSERT(ret == RHINO_SUCCESS);

    return 0;
}

static const test_func_t event_func_runner[] = {
    event_opr_case1,
    event_opr_case2,
    event_opr_case3,
    event_opr_case4,
    event_opr_case5,
    NULL
};

void event_opr_test(void)
{
    kstat_t ret;

    task_event_entry_register(MODULE_NAME, (test_func_t *)event_func_runner,
                              sizeof(event_func_runner) / sizeof(test_func_t));

    ret = krhino_task_dyn_create(&task_event, MODULE_NAME, 0, TASK_EVENT_PRI,
                                 0, TASK_TEST_STACK_SIZE, task_event_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }
}

static void task_event_co1_entry(void *arg)
{
    kstat_t  ret;
    uint32_t actl_flags;

    while (1) {
        ret = krhino_event_get(&test_event, ~CHK_AND_ALL_FLAG, RHINO_AND, &actl_flags,
                               RHINO_WAIT_FOREVER);
        if (ret == RHINO_SUCCESS) {
            break;
        } else {
            test_case_fail++;
            PRINT_RESULT(MODULE_NAME_CO, FAIL);

            (void)krhino_event_del(&test_event);

            next_test_case_notify();
            krhino_task_dyn_del(krhino_cur_task_get());

            return;
        }
    }

    test_case_success++;
    PRINT_RESULT(MODULE_NAME_CO, PASS);

    (void)krhino_event_del(&test_event);

    next_test_case_notify();
    krhino_task_dyn_del(krhino_cur_task_get());
}

static void task_event_co2_entry(void *arg)
{
    while (1) {
        (void)krhino_event_set(&test_event, ~CHK_AND_ALL_FLAG, RHINO_OR);
        break;
    }

    krhino_task_dyn_del(krhino_cur_task_get());
}

void event_coopr_test(void)
{
    kstat_t ret;

    ret = krhino_event_create(&test_event, MODULE_NAME, TEST_FLAG);
    if (ret != RHINO_SUCCESS && test_event.flags != TEST_FLAG) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME_CO, FAIL);
        return;
    }

    ret = krhino_task_dyn_create(&task_event_co1, MODULE_NAME, 0, TASK_EVENT_PRI,
                                 0, TASK_TEST_STACK_SIZE, task_event_co1_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }

    ret = krhino_task_dyn_create(&task_event_co2, MODULE_NAME, 0, TASK_EVENT_PRI + 1,
                                 0, TASK_TEST_STACK_SIZE, task_event_co2_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }
}

