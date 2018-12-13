/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "sys_test.h"

#define MODULE_NAME "sys_opr"

static uint8_t sys_opr_case1(void)
{
    kstat_t  ret;
    uint32_t version;

    version = krhino_version_get();
    if (version != RHINO_VERSION) {
        return 1;
    }

    ret = krhino_start();
    if (ret != RHINO_RUNNING) {
        MYASSERT(ret);
        return 1;
    }

    ret = krhino_sched_disable();
    if (ret != RHINO_SUCCESS) {
        MYASSERT(ret);
        return 1;
    }

    /* sched disable (out intrpt) */
    ret = krhino_sched_disable();
    if (ret != RHINO_SUCCESS) {
        MYASSERT(ret);
        return 1;
    }

    ret = krhino_sched_enable();
    if (ret != RHINO_SCHED_DISABLE) {
        MYASSERT(ret);
        return 1;
    }

    ret = krhino_sched_enable();
    if (ret != RHINO_SUCCESS) {
        MYASSERT(ret);
        return 1;
    }

    ret = krhino_sched_enable();
    if (ret != RHINO_SCHED_ALREADY_ENABLED) {
        MYASSERT(ret);
        return 1;
    }

    return 0;
}

static const test_func_t sys_func_runner[] = {
    sys_opr_case1,
    NULL
};

void sys_opr_test(void)
{
    kstat_t ret;

    task_sys_entry_register(MODULE_NAME, (test_func_t *)sys_func_runner,
                            sizeof(sys_func_runner) / sizeof(test_func_t));

    ret = krhino_task_dyn_create(&task_sys, MODULE_NAME, 0, TASK_SYS_PRI,
                                 0, TASK_TEST_STACK_SIZE, task_sys_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }
}

