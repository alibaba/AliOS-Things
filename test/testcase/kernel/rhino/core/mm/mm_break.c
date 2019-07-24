/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "mm_test.h"

#define MODULE_NAME "mm_break"

#if (RHINO_CONFIG_MM_TLF > 0)

static uint8_t mm_break_case1(void)
{
    void   *ptr, *newptr;
    kstat_t ret;
    char   *ptrarray[10];
    int     i;
    size_t  oldsize;
    ret = krhino_init_mm_head(&pmmhead, (void *)mm_pool, MM_POOL_SIZE);
    MYASSERT(ret == RHINO_SUCCESS);

#if (K_MM_STATISTIC > 0)

    oldsize = pmmhead->used_size;
    ptr = k_mm_alloc(pmmhead, 8);
    MYASSERT(ptr != NULL);

    MYASSERT((pmmhead->used_size - oldsize ) == RHINO_CONFIG_MM_BLK_SIZE);

    k_mm_free(pmmhead, ptr);

    oldsize = pmmhead->used_size;
    ptr = k_mm_alloc(pmmhead, RHINO_CONFIG_MM_BLK_SIZE);
    MYASSERT(ptr != NULL);
    MYASSERT((pmmhead->used_size - oldsize ) == RHINO_CONFIG_MM_BLK_SIZE);
    k_mm_free(pmmhead, ptr);

    oldsize = pmmhead->used_size;
    ptr = k_mm_alloc(pmmhead, RHINO_CONFIG_MM_BLK_SIZE + 1);

    MYASSERT(ptr != NULL);
    MYASSERT((pmmhead->used_size - oldsize ) == 
      (RHINO_CONFIG_MM_BLK_SIZE + MM_ALIGN_SIZE + MMLIST_HEAD_SIZE));
    k_mm_free(pmmhead, ptr);
#endif


    ptr = k_mm_alloc(pmmhead, RHINO_CONFIG_MM_BLK_SIZE / 2);
    MYASSERT(ptr != NULL);

    newptr = k_mm_realloc(pmmhead, ptr, RHINO_CONFIG_MM_BLK_SIZE / 2 + 1 );
    MYASSERT(newptr == ptr);

    newptr = k_mm_realloc(pmmhead, ptr, RHINO_CONFIG_MM_BLK_SIZE - 1);
    MYASSERT(newptr == ptr);

    ptr = newptr;
    newptr = k_mm_realloc(pmmhead, ptr, RHINO_CONFIG_MM_BLK_SIZE + 1);
    MYASSERT(newptr != ptr);

    ptr = newptr;
    newptr = k_mm_realloc(pmmhead, ptr, RHINO_CONFIG_MM_BLK_SIZE + 2);
    MYASSERT(newptr == ptr);

    ptr = newptr;
    newptr = k_mm_realloc(pmmhead, ptr, RHINO_CONFIG_MM_BLK_SIZE * 4);
    MYASSERT(newptr == ptr);

    ptr = newptr;
    newptr = k_mm_realloc(pmmhead, ptr, RHINO_CONFIG_MM_BLK_SIZE * 3);
    MYASSERT(newptr == ptr);

    newptr = k_mm_realloc(pmmhead, ptr, 0);
    MYASSERT(newptr == NULL);

    ptr =  k_mm_realloc(pmmhead, NULL, RHINO_CONFIG_MM_BLK_SIZE * 3);
    MYASSERT(ptr != NULL);

    k_mm_free(pmmhead, ptr);

    for (i = 0; i < 10; i++) {
        ptrarray[i] =  k_mm_alloc(pmmhead, (i + 1) * 2);
        MYASSERT(ptrarray[i]);
    }

    for (i = 0; i < 10; i++) {
        if (i % 2 == 0) {
            k_mm_free(pmmhead, ptrarray[i]);
        }
    }

    for (i = 0; i < 10; i++) {
        if (i % 2 != 0) {
            ptrarray[i] = k_mm_realloc(pmmhead, ptrarray[i], (i + 1) * 3);
        }
        MYASSERT(ptrarray[i]);
    }

    for (i = 0; i < 10; i++) {
        if (i % 2 != 0) {
            k_mm_free(pmmhead, ptrarray[i]);
        }
    }
    krhino_deinit_mm_head(pmmhead);

    return 0;
}

static const test_func_t mm_func_runner[] = {
    mm_break_case1,
    NULL
};

void mm_break_test(void)
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

