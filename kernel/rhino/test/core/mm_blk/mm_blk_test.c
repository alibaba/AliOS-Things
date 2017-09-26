/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "mm_blk_test.h"

ktask_t     *task_mm_blk;
ktask_t     *task_mm_blk_co;
mblk_pool_t mblk_pool_test;
char        mblk_pool[MBLK_POOL_SIZE];

static test_func_t *module_runner;
static const char  *module_name;
static uint8_t      module_casenum;

static const test_case_t mm_blk_case_runner[] = {
    mm_blk_param_test,
    mm_blk_opr_test,
    mm_blk_break_test,
    mm_blk_reinit_test,
    mm_blk_fragment_test,
    mm_blk_coopr_test,
    NULL
};

void mm_blk_test(void)
{
    if (test_case_register((test_case_t *)mm_blk_case_runner) == 0) {
        test_case_run();
        test_case_unregister();
    }
}

void task_mm_blk_entry_register(const char *name, test_func_t *runner,
                                uint8_t casenum)
{
    module_runner  = runner;
    module_name    = name;
    module_casenum = casenum;
}

void task_mm_blk_entry(void *arg)
{
    test_func_t *runner;
    uint8_t      caseidx;
    char         name[64];
    uint8_t      casenum;

    runner  = (test_func_t *)module_runner;
    casenum = module_casenum;
    caseidx = 0;

    while (1) {
        if (*runner == NULL) {
            break;
        }

        if (casenum > 2) {
            caseidx++;
            sprintf(name, "%s_%d", module_name, caseidx);
        } else {
            sprintf(name, "%s", module_name);
        }

        if ((*runner)() == 0) {
            test_case_success++;
            PRINT_RESULT(name, PASS);
        } else {
            test_case_fail++;
            PRINT_RESULT(name, FAIL);
        }
        runner++;
    }

    next_test_case_notify();
    krhino_task_dyn_del(krhino_cur_task_get());
}

