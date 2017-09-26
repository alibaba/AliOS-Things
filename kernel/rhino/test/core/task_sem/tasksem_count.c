/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "tasksem_test.h"

#define MODULE_NAME "tasksem_count"

static uint8_t tasksem_count_case1(void)
{
    kstat_t     ret;
    sem_count_t count;

    ret = krhino_task_sem_create(task_tasksem, &test_tasksem, MODULE_NAME, 8);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_task_sem_count_get(NULL, &count);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_task_sem_count_get(task_tasksem, &count);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(count == 8);

    ret = krhino_task_sem_count_set(NULL, 3);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_task_sem_count_set(task_tasksem, (sem_count_t) - 1);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_task_sem_del(task_tasksem);
    MYASSERT(ret == RHINO_SUCCESS);

    return 0;
}

static const test_func_t tasksem_func_runner[] = {
    tasksem_count_case1,
    NULL
};

void tasksem_count_test(void)
{
    kstat_t ret;

    task_tasksem_entry_register(MODULE_NAME, (test_func_t *)tasksem_func_runner,
                                sizeof(tasksem_func_runner) / sizeof(test_case_t));

    ret = krhino_task_dyn_create(&task_tasksem, MODULE_NAME, 0, TASK_SEM_PRI,
                                 0, TASK_TEST_STACK_SIZE, task_tasksem_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }
}

