/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "event_test.h"

#define MODULE_NAME "event_param"

#define TEST_FLAG   0x5a5a5a5a

static uint8_t event_param_case1(void)
{
    kstat_t ret;
    CPSR_ALLOC();

    ret = krhino_event_create(NULL, MODULE_NAME, TEST_FLAG);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_event_create(&test_event, NULL, TEST_FLAG);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_event_create(&test_event, MODULE_NAME, TEST_FLAG);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_event_del(NULL);
    MYASSERT(ret == RHINO_NULL_PTR);

    RHINO_CRITICAL_ENTER();
    test_event.mm_alloc_flag = K_OBJ_DYN_ALLOC;
    RHINO_CRITICAL_EXIT();
    ret = krhino_event_del(&test_event);
    MYASSERT(ret == RHINO_KOBJ_DEL_ERR);

    RHINO_CRITICAL_ENTER();
    test_event.mm_alloc_flag = K_OBJ_STATIC_ALLOC;
    RHINO_CRITICAL_EXIT();
    ret = krhino_event_del(&test_event);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_event_dyn_create(NULL, MODULE_NAME, TEST_FLAG);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_event_dyn_create(&test_event_ext, NULL, TEST_FLAG);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_event_dyn_create(&test_event_ext, MODULE_NAME, TEST_FLAG);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_event_dyn_del(NULL);
    MYASSERT(ret == RHINO_NULL_PTR);

    RHINO_CRITICAL_ENTER();
    test_event_ext->blk_obj.obj_type = RHINO_SEM_OBJ_TYPE;
    RHINO_CRITICAL_EXIT();
    ret = krhino_event_dyn_del(test_event_ext);
    MYASSERT(ret == RHINO_KOBJ_TYPE_ERR);

    RHINO_CRITICAL_ENTER();
    test_event_ext->blk_obj.obj_type = RHINO_EVENT_OBJ_TYPE;
    RHINO_CRITICAL_EXIT();

    RHINO_CRITICAL_ENTER();
    test_event_ext->mm_alloc_flag = K_OBJ_STATIC_ALLOC;
    RHINO_CRITICAL_EXIT();
    ret = krhino_event_dyn_del(test_event_ext);
    MYASSERT(ret == RHINO_KOBJ_DEL_ERR);

    RHINO_CRITICAL_ENTER();
    test_event_ext->mm_alloc_flag = K_OBJ_DYN_ALLOC;
    RHINO_CRITICAL_EXIT();
    ret = krhino_event_dyn_del(test_event_ext);
    MYASSERT(ret == RHINO_SUCCESS);

    return 0;
}

static uint8_t event_param_case2(void)
{
    kstat_t  ret;
    uint32_t actl_flags;

    ret = krhino_event_create(&test_event, MODULE_NAME, TEST_FLAG);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_event_get(NULL, 0, RHINO_AND, &actl_flags, RHINO_WAIT_FOREVER);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_event_get(&test_event, 0, RHINO_AND, NULL, RHINO_WAIT_FOREVER);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_event_get(&test_event, 0, 0xff, &actl_flags, RHINO_WAIT_FOREVER);
    MYASSERT(ret == RHINO_NO_THIS_EVENT_OPT);

    ret = krhino_event_set(NULL, TEST_FLAG, RHINO_OR);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_event_set(&test_event, TEST_FLAG, 0xff);
    MYASSERT(ret == RHINO_NO_THIS_EVENT_OPT);

    ret = krhino_event_del(&test_event);
    MYASSERT(ret == RHINO_SUCCESS);

    return 0;
}

static const test_func_t event_func_runner[] = {
    event_param_case1,
    event_param_case2,
    NULL
};

void event_param_test(void)
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

