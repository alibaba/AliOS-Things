/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "mm_blk_test.h"

#define MODULE_NAME "mm_blk_fragment"

static uint8_t mm_blk_fragment_case1(void)
{
    void   *ptr[16];
    kstat_t ret;
    uint8_t blkavail;

    ret = krhino_mblk_pool_init(&mblk_pool_test, MODULE_NAME, (void *)mblk_pool,
                                MBLK_POOL_SIZE >> 2, MBLK_POOL_SIZE);
    MYASSERT(ret == RHINO_SUCCESS);

    /* check malloc save pointer number is enough or not */
    MYASSERT(mblk_pool_test.blk_whole <= 16);

    /* alloc all blocks */
    blkavail = 0;
    do {
        ret = krhino_mblk_alloc(&mblk_pool_test, &ptr[blkavail]);
        if (ret == RHINO_SUCCESS) {
            blkavail++;
        }
    } while (ret == RHINO_SUCCESS);

    MYASSERT(mblk_pool_test.blk_avail == 0);
    MYASSERT(blkavail == mblk_pool_test.blk_whole);

    /* free all blocks */
    blkavail = 0;
    do {
        ret = krhino_mblk_free(&mblk_pool_test, ptr[blkavail]);
        if (ret == RHINO_SUCCESS) {
            blkavail++;
        }
    } while (ret == RHINO_SUCCESS);

    MYASSERT(mblk_pool_test.blk_avail == mblk_pool_test.blk_whole);
    MYASSERT(blkavail == mblk_pool_test.blk_whole);

    return 0;
}

static const test_func_t mm_blk_func_runner[] = {
    mm_blk_fragment_case1,
    NULL
};

void mm_blk_fragment_test(void)
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

