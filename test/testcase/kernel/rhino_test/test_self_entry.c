/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>

#define      TEST_CASE_TASK_STACK_SIZE 1024
ktask_t      test_case_task;
cpu_stack_t  test_case_task_stack[TEST_CASE_TASK_STACK_SIZE];

void test_case_task_entry(void *arg)
{
    int item;

    test_case_init();

    for (item = 0 ;; item++) {
        if (test_fw_map[item].fn == NULL) {
            break;
        }

        (*test_fw_map[item].fn)();
    }

    printf("all test finished, successed test case %d, failed test case %d\n",
           (int)test_case_success, (int)test_case_fail);

    while (1) {
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    }
}

void test_case_task_start(void)
{
    krhino_task_create(&test_case_task, "test_case_task", NULL, 5,
                       0, test_case_task_stack, TEST_CASE_TASK_STACK_SIZE,
                       test_case_task_entry, 1);
}

