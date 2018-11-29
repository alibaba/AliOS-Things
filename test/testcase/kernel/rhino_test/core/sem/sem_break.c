/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "sem_test.h"

#define MODULE_NAME "sem_break"

static uint8_t sem_break_case1(void)
{
    kstat_t ret;

    ret = krhino_sem_dyn_create(&test_sem, MODULE_NAME, 0);
    MYASSERT(ret == RHINO_SUCCESS);

    /* try to delete after change it */
    test_sem->blk_obj.obj_type = RHINO_EVENT_OBJ_TYPE;
    ret = krhino_sem_dyn_del(test_sem);
    MYASSERT(ret == RHINO_KOBJ_TYPE_ERR);

    /* try to delete after recover it */
    test_sem->blk_obj.obj_type = RHINO_SEM_OBJ_TYPE;
    ret = krhino_sem_dyn_del(test_sem);
    MYASSERT(ret == RHINO_SUCCESS);

    return 0;
}

static const test_func_t sem_func_runner[] = {
    sem_break_case1,
    NULL
};

void sem_break_test(void)
{
    kstat_t ret;

    task_sem_entry_register(MODULE_NAME, (test_func_t *)sem_func_runner,
                            sizeof(sem_func_runner) / sizeof(test_case_t));

    ret = krhino_task_dyn_create(&task_sem, MODULE_NAME, 0, TASK_SEM_PRI,
                                 0, TASK_TEST_STACK_SIZE, task_sem_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }
}

