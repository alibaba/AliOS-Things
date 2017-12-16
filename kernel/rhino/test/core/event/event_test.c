/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "event_test.h"

ktask_t  *task_event;
ktask_t  *task_event_co1;
ktask_t  *task_event_co2;
kevent_t  test_event;
kevent_t *test_event_ext;

static test_func_t *module_runner;
static const char  *module_name;
static uint8_t      module_casenum;

static const test_case_t event_case_runner[] = {
    event_param_test,
    event_break_test,
    event_reinit_test,
    event_opr_test,
    event_coopr_test,
    NULL
};

void event_test(void)
{
    if (test_case_register((test_case_t *)event_case_runner) == 0) {
        test_case_run();
        test_case_unregister();
    }
}

void task_event_entry_register(const char *name, test_func_t *runner,
                               uint8_t casenum)
{
    module_runner  = runner;
    module_name    = name;
    module_casenum = casenum;
}

void task_event_entry(void *arg)
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

