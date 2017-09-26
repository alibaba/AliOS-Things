/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "mm_blk_test.h"

#define MODULE_NAME    "mm_blk_opr"
#define MODULE_NAME_CO "mm_blk_coopr"

static void    *co_ptr;
static ktask_t *blk_task;

static uint8_t mm_blk_opr_case1(void)
{
    void   *ptr;
    kstat_t ret;

    ret = krhino_mblk_pool_init(&mblk_pool_test, MODULE_NAME, (void *)mblk_pool,
                                MBLK_POOL_SIZE >> 2, MBLK_POOL_SIZE);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_mblk_alloc(&mblk_pool_test, &ptr);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_mblk_free(&mblk_pool_test, ptr);
    MYASSERT(ret == RHINO_SUCCESS);

    return 0;
}

static const test_func_t mm_blk_func_runner[] = {
    mm_blk_opr_case1,
    NULL
};

void mm_blk_opr_test(void)
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

static void task_mm_blk_co1_entry(void *arg)
{
    while (1) {
        krhino_mblk_alloc(&mblk_pool_test, &co_ptr);
        krhino_mblk_free(&mblk_pool_test, co_ptr);
        krhino_task_sleep(5);
        krhino_mblk_alloc(&mblk_pool_test, &co_ptr);
        krhino_mblk_free(&mblk_pool_test, co_ptr);
        krhino_task_sleep(5);
        krhino_mblk_alloc(&mblk_pool_test, &co_ptr);
        krhino_mblk_free(&mblk_pool_test, co_ptr);
        krhino_task_sleep(5);
        krhino_mblk_alloc(&mblk_pool_test, &co_ptr);
        krhino_mblk_free(&mblk_pool_test, co_ptr);
        krhino_task_sleep(5);

        break;
    }

    if (mblk_pool_test.blk_avail == (MBLK_POOL_SIZE / mblk_pool_test.blk_size)) {
        test_case_success++;
        PRINT_RESULT(MODULE_NAME_CO, PASS);
    } else {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME_CO, FAIL);
    }

    next_test_case_notify();
    krhino_task_dyn_del(krhino_cur_task_get());
}


void mm_blk_coopr_test(void)
{
    kstat_t ret;

    ret = krhino_mblk_pool_init(&mblk_pool_test, MODULE_NAME, (void *)mblk_pool,
                                MBLK_POOL_SIZE >> 2, MBLK_POOL_SIZE);
    if (ret != RHINO_SUCCESS) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME_CO, FAIL);
        return;
    }

    ret = krhino_task_dyn_create(&blk_task, MODULE_NAME, 0, TASK_MM_BLK_PRI + 1,
                                 0, TASK_TEST_STACK_SIZE, task_mm_blk_co1_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME_CO, FAIL);
    }
}

