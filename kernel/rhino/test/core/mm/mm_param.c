/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "mm_test.h"

#define MODULE_NAME "mm_param"

#if (RHINO_CONFIG_MM_TLF > 0)

static uint8_t mm_param_case1(void)
{
    kstat_t ret;

    ret = krhino_init_mm_head(NULL, (void *)mm_pool, MM_POOL_SIZE);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_init_mm_head(&pmmhead, NULL, MM_POOL_SIZE);
    MYASSERT(ret == RHINO_NULL_PTR);

    ret = krhino_init_mm_head(&pmmhead, (void *)mm_pool, MIN_FREE_MEMORY_SIZE + RHINO_CONFIG_MM_TLF_BLK_SIZE - 1);
    MYASSERT(ret == RHINO_MM_POOL_SIZE_ERR);

    ret = krhino_init_mm_head(&pmmhead, (void *)mm_pool, MM_POOL_SIZE);
    MYASSERT(ret == RHINO_SUCCESS);
    ret = krhino_deinit_mm_head(pmmhead);
    MYASSERT(ret == RHINO_SUCCESS);

    return 0;
}

static uint8_t mm_param_case2(void)
{
    void   *ptr;
    void   *tmp;
    kstat_t ret;

    ret = krhino_init_mm_head(&pmmhead, (void *)mm_pool, MM_POOL_SIZE);
    MYASSERT(ret == RHINO_SUCCESS);

    ptr = k_mm_alloc( NULL, 64);
    MYASSERT(ptr == NULL);

    ptr = k_mm_alloc(pmmhead, 0);
    MYASSERT(ptr == NULL);

    ptr = k_mm_alloc(pmmhead, 64);
    MYASSERT((ptr > (void *)mm_pool) && (ptr < ((void *)mm_pool + MM_POOL_SIZE)));
    k_mm_free(pmmhead, ptr);

    ptr = k_mm_alloc(pmmhead, 16);

    tmp = pmmhead->fixedmblk->mbinfo.buffer;
    MYASSERT((ptr > (void *)pmmhead->fixedmblk->mbinfo.buffer) &&
             (ptr < (void *)tmp + MM_GET_BUF_SIZE(pmmhead->fixedmblk)));

    k_mm_free(pmmhead, ptr);

    krhino_deinit_mm_head(pmmhead);

    return 0;
}

static const test_func_t mm_func_runner[] = {
    mm_param_case1,
    mm_param_case2,
    NULL
};

void mm_param_test(void)
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

#endif

