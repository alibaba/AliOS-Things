/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "mm_test.h"

#define MODULE_NAME    "mm_opr"
#define MODULE_NAME_CO "mm_coopr"
static void *co_ptr;

#if (RHINO_CONFIG_MM_TLF > 0)

static uint8_t mm_opr_case1(void)
{
    char   *ptr;
    kstat_t ret;
    char    tmp;

    ret = krhino_init_mm_head(&pmmhead, (void *)mm_pool, MM_POOL_SIZE);
    MYASSERT(ret == RHINO_SUCCESS);

    ptr = k_mm_alloc(pmmhead, 64);
    MYASSERT(ptr != NULL);
    //for vagrind test
    //ptr[64] = ptr[-1];

    k_mm_free(pmmhead, ptr);

    krhino_deinit_mm_head(pmmhead);

    ret = krhino_init_mm_head(&pmmhead, (void *)mm_pool, MM_POOL_SIZE);
    MYASSERT(ret == RHINO_SUCCESS);

    krhino_deinit_mm_head(pmmhead);

    return 0;
}

static uint8_t mm_opr_case2(void)
{
    void   *r_ptr[16];
    int8_t  cnt;
    kstat_t ret;

    ret = krhino_init_mm_head(&pmmhead, (void *)mm_pool, MM_POOL_SIZE);
    MYASSERT(ret == RHINO_SUCCESS);

    /* alloc out of mm pools then free all */
    cnt = 0;
    for (cnt = 0; cnt < 16; ++cnt) {
        r_ptr[cnt] = k_mm_alloc(pmmhead, cnt + 26);
        if (r_ptr[cnt] ==  NULL) {
            break;
        }
    }

    do {
        k_mm_free(pmmhead, r_ptr[--cnt]);

    } while (cnt > 0);

    krhino_deinit_mm_head(pmmhead);

    return 0;
}

static const test_func_t mm_func_runner[] = {
    mm_opr_case1,
    mm_opr_case2,
    NULL
};

void mm_opr_test(void)
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

static void task_mm_co1_entry(void *arg)
{
    kstat_t ret;

    ret = krhino_init_mm_head(&pmmhead, (void *)mm_pool, MM_POOL_SIZE);
    if (ret != RHINO_SUCCESS) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME_CO, FAIL);
        return;
    }

    co_ptr = k_mm_alloc(pmmhead, 16);
    k_mm_free(pmmhead, co_ptr);

    co_ptr = k_mm_alloc(pmmhead, 18);
    k_mm_free(pmmhead, co_ptr);

    co_ptr = k_mm_alloc(pmmhead, 32);
    k_mm_free(pmmhead, co_ptr);

    co_ptr = k_mm_alloc(pmmhead, 65);
    k_mm_free(pmmhead, co_ptr);

    co_ptr = k_mm_alloc(pmmhead, 178);
    k_mm_free(pmmhead, co_ptr);

    co_ptr = k_mm_alloc(pmmhead, 333);
    k_mm_free(pmmhead, co_ptr);

    krhino_deinit_mm_head(pmmhead);

    next_test_case_notify();
    krhino_task_dyn_del(krhino_cur_task_get());
}

void mm_coopr_test(void)
{
    kstat_t ret;

    ret = krhino_init_mm_head(&pmmhead, (void *)mm_pool, MM_POOL_SIZE);
    if (ret != RHINO_SUCCESS) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME_CO, FAIL);
        return;
    }

    ret = krhino_task_dyn_create(&task_mm, MODULE_NAME, 0, TASK_MM_PRI,
                                 0, TASK_TEST_STACK_SIZE, task_mm_co1_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME_CO, FAIL);
    }

}

#endif

