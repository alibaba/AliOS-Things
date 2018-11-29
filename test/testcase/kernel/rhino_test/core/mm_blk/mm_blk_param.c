/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "mm_blk_test.h"

#define MODULE_NAME "mm_blk_param"

static uint8_t mm_blk_param_case1(void)
{
    kstat_t ret;

    ret = krhino_mblk_pool_init(NULL, MODULE_NAME, (void *)mblk_pool,
                                MBLK_POOL_SIZE >> 1, MBLK_POOL_SIZE);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_mblk_pool_init(&mblk_pool_test, NULL, (void *)mblk_pool,
                                MBLK_POOL_SIZE >> 1, MBLK_POOL_SIZE);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_mblk_pool_init(&mblk_pool_test, MODULE_NAME, NULL,
                                MBLK_POOL_SIZE >> 1, MBLK_POOL_SIZE);
    MYASSERT(ret == RHINO_NULL_PTR);

    /* check block size is less than half of pool size */
    ret = krhino_mblk_pool_init(&mblk_pool_test, MODULE_NAME, (void *)mblk_pool,
                                (MBLK_POOL_SIZE >> 1) + 1, MBLK_POOL_SIZE);
    MYASSERT(ret == RHINO_BLK_POOL_SIZE_ERR);

    /* check block size is less than half of pool size */
    ret = krhino_mblk_pool_init(&mblk_pool_test, MODULE_NAME, (void *)mblk_pool,
                                MBLK_POOL_SIZE, MBLK_POOL_SIZE);
    MYASSERT(ret == RHINO_BLK_POOL_SIZE_ERR);

    /* check pool address is not align */
    ret = krhino_mblk_pool_init(&mblk_pool_test, MODULE_NAME,
                                (void *)(mblk_pool + 1),
                                MBLK_POOL_SIZE >> 2, MBLK_POOL_SIZE);
    MYASSERT(ret == RHINO_INV_ALIGN);

    /* check block size is not align */
    ret = krhino_mblk_pool_init(&mblk_pool_test, MODULE_NAME, (void *)mblk_pool,
                                (MBLK_POOL_SIZE >> 2) + 1, MBLK_POOL_SIZE);
    MYASSERT(ret == RHINO_INV_ALIGN);

    /* check pool size is not align */
    ret = krhino_mblk_pool_init(&mblk_pool_test, MODULE_NAME, (void *)mblk_pool,
                                MBLK_POOL_SIZE >> 2, MBLK_POOL_SIZE + 1);
    MYASSERT(ret == RHINO_INV_ALIGN);

    ret = krhino_mblk_pool_init(&mblk_pool_test, MODULE_NAME, (void *)mblk_pool,
                                MBLK_POOL_SIZE >> 2, MBLK_POOL_SIZE);
    MYASSERT(ret == RHINO_SUCCESS);

    return 0;
}

static uint8_t mm_blk_param_case2(void)
{
    kstat_t ret;
    void *ptr;

    ret = krhino_mblk_pool_init(&mblk_pool_test, MODULE_NAME, (void *)mblk_pool,
                                MBLK_POOL_SIZE >> 2, MBLK_POOL_SIZE);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_mblk_alloc(NULL, &ptr);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_mblk_alloc(&mblk_pool_test, NULL);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_mblk_free(NULL, ptr);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_mblk_free(&mblk_pool_test, NULL);
    MYASSERT(ret == RHINO_NULL_PTR);

    return 0;
}

static const test_func_t mm_blk_func_runner[] = {
    mm_blk_param_case1,
    mm_blk_param_case2,
    NULL
};

void mm_blk_param_test(void)
{
    kstat_t ret;

    task_mm_blk_entry_register(MODULE_NAME, (test_func_t *)mm_blk_func_runner,
                               sizeof(mm_blk_func_runner) / sizeof(test_func_t));

    ret = krhino_task_dyn_create(&task_mm_blk, MODULE_NAME, 0, TASK_MM_BLK_PRI,
                                 0, TASK_TEST_STACK_SIZE, task_mm_blk_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }
}

