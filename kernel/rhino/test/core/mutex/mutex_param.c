/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "mutex_test.h"

#define MODULE_NAME "mutex_param"

static uint8_t mutex_param_case1(void)
{
    kstat_t ret;
    CPSR_ALLOC();

    ret = krhino_mutex_create(NULL, MODULE_NAME);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_mutex_create(&test_mutex, NULL);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_mutex_create(&test_mutex, MODULE_NAME);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_mutex_del(NULL);
    MYASSERT(ret == RHINO_NULL_PTR);

    RHINO_CRITICAL_ENTER();
    test_mutex.mm_alloc_flag = K_OBJ_DYN_ALLOC;
    RHINO_CRITICAL_EXIT();
    ret = krhino_mutex_del(&test_mutex);
    MYASSERT(ret == RHINO_KOBJ_DEL_ERR);

    RHINO_CRITICAL_ENTER();
    test_mutex.mm_alloc_flag = K_OBJ_STATIC_ALLOC;
    RHINO_CRITICAL_EXIT();
    ret = krhino_mutex_del(&test_mutex);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_mutex_dyn_create(NULL, MODULE_NAME);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_mutex_dyn_create(&test_mutex_dyn, NULL);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_mutex_dyn_create(&test_mutex_dyn, MODULE_NAME);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_mutex_dyn_del(NULL);
    MYASSERT(ret == RHINO_NULL_PTR);

    RHINO_CRITICAL_ENTER();
    test_mutex_dyn->blk_obj.obj_type = RHINO_SEM_OBJ_TYPE;
    RHINO_CRITICAL_EXIT();
    ret = krhino_mutex_dyn_del(test_mutex_dyn);
    MYASSERT(ret == RHINO_KOBJ_TYPE_ERR);

    RHINO_CRITICAL_ENTER();
    test_mutex_dyn->blk_obj.obj_type = RHINO_MUTEX_OBJ_TYPE;
    test_mutex_dyn->mm_alloc_flag = K_OBJ_STATIC_ALLOC;
    RHINO_CRITICAL_EXIT();
    ret = krhino_mutex_dyn_del(test_mutex_dyn);
    MYASSERT(ret == RHINO_KOBJ_DEL_ERR);

    RHINO_CRITICAL_ENTER();
    test_mutex_dyn->mm_alloc_flag = K_OBJ_DYN_ALLOC;
    RHINO_CRITICAL_EXIT();
    ret = krhino_mutex_dyn_del(test_mutex_dyn);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_mutex_dyn_create(NULL, MODULE_NAME);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_mutex_dyn_create(&test_mutex_dyn, NULL);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_mutex_dyn_create(&test_mutex_dyn, MODULE_NAME);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_mutex_dyn_del(NULL);
    MYASSERT(ret == RHINO_NULL_PTR);

    test_mutex_dyn->blk_obj.obj_type = RHINO_SEM_OBJ_TYPE;
    ret = krhino_mutex_dyn_del(test_mutex_dyn);
    MYASSERT(ret == RHINO_KOBJ_TYPE_ERR);

    test_mutex_dyn->blk_obj.obj_type = RHINO_MUTEX_OBJ_TYPE;
    test_mutex_dyn->mm_alloc_flag = K_OBJ_STATIC_ALLOC;
    ret = krhino_mutex_dyn_del(test_mutex_dyn);
    MYASSERT(ret == RHINO_KOBJ_DEL_ERR);

    test_mutex_dyn->mm_alloc_flag = K_OBJ_DYN_ALLOC;
    ret = krhino_mutex_dyn_del(test_mutex_dyn);
    MYASSERT(ret == RHINO_SUCCESS);

    return 0;
}

static uint8_t mutex_param_case2(void)
{
    kstat_t     ret;
    CPSR_ALLOC();

    ret = krhino_mutex_create(&test_mutex, MODULE_NAME);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_mutex_lock(NULL, RHINO_NO_WAIT);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_mutex_lock(&test_mutex, RHINO_NO_WAIT);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_mutex_unlock(NULL);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_mutex_unlock(&test_mutex);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_mutex_del(&test_mutex);
    MYASSERT(ret == RHINO_SUCCESS);

    return 0;
}

static const test_func_t mutex_func_runner[] = {
    mutex_param_case1,
    mutex_param_case2,
    NULL
};

void mutex_param_test(void)
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

