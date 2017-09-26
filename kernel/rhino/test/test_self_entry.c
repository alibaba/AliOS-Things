/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>

#define      TEST_CASE_TASK_STACK_SIZE 512
ktask_t       test_case_task;
cpu_stack_t  test_case_task_stack[TEST_CASE_TASK_STACK_SIZE];


#define      TEST_MTBF_TASK_STACK_SIZE 512
ktask_t       test_mtbf_task;
cpu_stack_t  test_mtbf_task_stack[TEST_MTBF_TASK_STACK_SIZE];

void test_case_task_entry(void *arg)
{
#if (RHINO_CONFIG_CPU_USAGE_STATS > 0)
    krhino_cpu_usage_stats_init();
#endif
    test_case_init();

    int item;

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

void test_mtbf_task_entry(void *arg)
{
#if (RHINO_CONFIG_CPU_USAGE_STATS > 0)
    krhino_cpu_usage_stats_init();
#endif
    test_case_init();
    int counter = 0;

    int item;

    while (1) {
        for (item = 0 ; ; item++) {
            if (test_fw_map[item].fn == NULL) {
                break;
            }

            (*test_fw_map[item].fn)();
        }

        counter ++;

        if (counter > 1000)        {
            break;
        }
    }

    printf("all test finished, successed test case %d, failed test case %d\n",
           (int)test_case_success, (int)test_case_fail);

    while (1) {
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    }
}

void test_mtbf_task_start(void)
{
    krhino_task_create(&test_mtbf_task, "test_mtbf_task", NULL, 9,
                       0, test_mtbf_task_stack, TEST_MTBF_TASK_STACK_SIZE,
                       test_mtbf_task_entry, 1);
}

