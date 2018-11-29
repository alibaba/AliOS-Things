/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "sem_test.h"

#define MODULE_NAME "sem_count"

static uint8_t sem_count_case1(void)
{
    kstat_t     ret;
    sem_count_t cnt;

    ret = krhino_sem_dyn_create(&test_sem, MODULE_NAME, 3);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(test_sem->count == 3);
    MYASSERT(test_sem->peak_count == 3);

    ret = krhino_sem_count_get(test_sem, &cnt);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(cnt == 3);

    ret = krhino_sem_take(test_sem, RHINO_NO_WAIT);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_sem_count_get(test_sem, &cnt);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(cnt == 2);

    test_sem->blk_obj.obj_type = RHINO_MUTEX_OBJ_TYPE;
    ret = krhino_sem_count_set(test_sem, 8);
    MYASSERT(ret == RHINO_KOBJ_TYPE_ERR);

    test_sem->blk_obj.obj_type = RHINO_SEM_OBJ_TYPE;
    ret = krhino_sem_count_set(test_sem, 8);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_sem_count_get(test_sem, &cnt);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(cnt == 8);

    ret = krhino_sem_dyn_del(test_sem);
    MYASSERT(ret == RHINO_SUCCESS);

    return 0;
}

static const test_func_t sem_func_runner[] = {
    sem_count_case1,
    NULL
};

void sem_count_test(void)
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

