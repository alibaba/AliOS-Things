/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>

#define TASK_TEST_STACK_SIZE 1024
#define LOOP_CNT 1

ktask_t *task_0_test;
static uint32_t cnt;

void task_sleep_entry(void *arg)
{
    while (1) {
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
        cnt++;

        if (cnt == LOOP_CNT) {
            test_case_success++;
            cnt = 0u;
            PRINT_RESULT("task_sleep", PASS);
            next_test_case_notify();
            krhino_task_dyn_del(krhino_cur_task_get());
        }
    }
}

void task_sleep_test(void)
{
    if (krhino_task_dyn_create(&task_0_test, "task_sleep_test", 0, 10,
                               0, TASK_TEST_STACK_SIZE,
                               task_sleep_entry, 1) != RHINO_SUCCESS) {
        printf("task_sleep_test fail \n");
    }
}

