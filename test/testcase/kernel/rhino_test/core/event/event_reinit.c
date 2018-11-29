/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>
#include "event_test.h"

#define MODULE_NAME "event_reinit"

#define TEST_FLAG   0x5a5a5a5a

static uint8_t event_reinit_case1(void)
{
    kstat_t ret;

    ret = krhino_event_create(&test_event, MODULE_NAME, TEST_FLAG);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(test_event.blk_obj.obj_type == RHINO_EVENT_OBJ_TYPE);
    MYASSERT(test_event.blk_obj.blk_policy == BLK_POLICY_PRI);
    MYASSERT(test_event.blk_obj.name != NULL);
    MYASSERT(test_event.flags == TEST_FLAG);

    ret = krhino_event_del(&test_event);
    MYASSERT(ret == RHINO_SUCCESS);

    ret = krhino_event_create(&test_event, MODULE_NAME, TEST_FLAG & 0x0000ffff);
    MYASSERT(ret == RHINO_SUCCESS);
    MYASSERT(test_event.blk_obj.obj_type == RHINO_EVENT_OBJ_TYPE);
    MYASSERT(test_event.flags == (TEST_FLAG & 0x0000ffff));

    ret = krhino_event_del(&test_event);
    MYASSERT(ret == RHINO_SUCCESS);

    return 0;
}

static const test_func_t event_func_runner[] = {
    event_reinit_case1,
    NULL
};

void event_reinit_test(void)
{
    kstat_t ret;

    task_event_entry_register(MODULE_NAME, (test_func_t *)event_func_runner,
                              sizeof(event_func_runner) / sizeof(test_func_t));

    ret = krhino_task_dyn_create(&task_event, MODULE_NAME, 0, TASK_EVENT_PRI,
                                 0, TASK_TEST_STACK_SIZE, task_event_entry, 1);
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        test_case_fail++;
        PRINT_RESULT(MODULE_NAME, FAIL);
    }
}

