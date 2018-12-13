/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "mutex_test.h"

#define MODULE_NAME "mutex_reinit"

static uint8_t mutex_reinit_case(void)
{
    kstat_t ret;

    ret = krhino_mutex_create(&test_mutex, MODULE_NAME);
    MYASSERT(ret == RHINO_SUCCESS);

    test_mutex.blk_obj.obj_type = RHINO_SEM_OBJ_TYPE;
    ret = krhino_mutex_lock(&test_mutex, RHINO_NO_WAIT);
    MYASSERT(ret == RHINO_KOBJ_TYPE_ERR);

    test_mutex.blk_obj.obj_type = RHINO_MUTEX_OBJ_TYPE;
    ret = krhino_mutex_lock(&test_mutex, RHINO_NO_WAIT);
    MYASSERT(ret == RHINO_SUCCESS);

    test_mutex.blk_obj.obj_type = RHINO_SEM_OBJ_TYPE;
    ret = krhino_mutex_unlock(&test_mutex);
    MYASSERT(ret == RHINO_KOBJ_TYPE_ERR);

    test_mutex.blk_obj.obj_type = RHINO_MUTEX_OBJ_TYPE;
    ret = krhino_mutex_unlock(&test_mutex);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_mutex_del(&test_mutex);
    MYASSERT(ret == RHINO_SUCCESS);

    return 0;
}

static const test_func_t mutex_func_runner[] = {
    mutex_reinit_case,
    NULL
};

void mutex_reinit_test(void)
{
    kstat_t ret;

    task_mutex_entry_register(MODULE_NAME, (test_func_t *)mutex_func_runner,
                              sizeof(mutex_func_runner) / sizeof(test_func_t));

    ret = krhino_task_dyn_create(&task_mutex, MODULE_NAME, 0, TASK_MUTEX_PRI,
                                 0, TASK_TEST_STACK_SIZE, task_mutex_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }
}

