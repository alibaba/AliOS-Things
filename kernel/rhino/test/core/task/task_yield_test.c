/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>

#define TASK_TEST_STACK_SIZE 512

static ktask_t *task_1_test;
static ktask_t *task_2_test;

void task_yield_1_entry(void *arg)
{
#if (RHINO_CONFIG_SCHED_RR > 0)
    krhino_sched_policy_set(krhino_cur_task_get(), KSCHED_FIFO);
#endif

    while (1) {
        krhino_task_yield();
        krhino_task_dyn_del(krhino_cur_task_get());
    }

}

void task_yield_2_entry(void *arg)
{
    while (1) {
        test_case_success++;
        PRINT_RESULT("task_yield", PASS);
        next_test_case_notify();
        krhino_task_dyn_del(krhino_cur_task_get());
    }
}

void task_yield_test(void)
{
    (void)krhino_task_dyn_create(&task_1_test, "task_yield_test_1", 0, 10,
                                 0, TASK_TEST_STACK_SIZE,
                                 task_yield_1_entry, 1);

    (void)krhino_task_dyn_create(&task_2_test, "task_yield_test_2", 0, 10,
                                 0, TASK_TEST_STACK_SIZE,
                                 task_yield_2_entry, 1);
}

