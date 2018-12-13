/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "mm_test.h"

#define MODULE_NAME "mm_reinit"

static uint8_t mm_reinit_case1(void)
{
    kstat_t ret;

    ret = krhino_mm_pool_init(&mm_pool_test, MODULE_NAME, (void *)mm_pool,
                              MM_POOL_MIN_SIZE);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(mm_pool_test.fragments == 2);
    MYASSERT(mm_pool_test.pool_name != NULL);
    MYASSERT(mm_pool_test.avail == (MM_POOL_MIN_SIZE - (MM_HEAD_SIZE << 1)));

    ret = krhino_mm_pool_init(&mm_pool_test, MODULE_NAME, (void *)mm_pool,
                              MM_POOL_SIZE);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(mm_pool_test.fragments == 2);
    MYASSERT(mm_pool_test.pool_name != NULL);
    MYASSERT(mm_pool_test.avail == (MM_POOL_SIZE - (MM_HEAD_SIZE << 1)));

    return 0;
}

static const test_func_t mm_func_runner[] = {
    mm_reinit_case1,
    NULL
};

void mm_reinit_test(void)
{
    kstat_t ret;

    task_mm_entry_register(MODULE_NAME, (test_func_t *)mm_func_runner,
                           sizeof(mm_func_runner) / sizeof(test_func_t));

    ret = krhino_task_dyn_create(&task_mm, MODULE_NAME, 0, TASK_MM_PRI,
                                 0, TASK_TEST_STACK_SIZE, task_mm_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }
}

