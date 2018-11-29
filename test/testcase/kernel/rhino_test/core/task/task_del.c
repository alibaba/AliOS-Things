/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>

#define TASK_TEST_STACK_SIZE 512
#define LOOP_CNT 1

static ktask_t *task_del_test_0;
static ktask_t *task_del_test_1;
static ktask_t *task_del_test_2;

static void task_del_entry_0(void *arg)
{
    while (1) {
        krhino_task_dyn_del(NULL);
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    }
}

static void task_del_entry_1(void *arg)
{
    while (1) {
        krhino_task_dyn_del(NULL);
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    }
}

static void task_del_entry_2(void *arg)
{
    while (1) {
        krhino_task_dyn_del(NULL);
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    }
}

void task_del_test()
{
    test_case_check_err = 0;

    if (krhino_task_dyn_create(&task_del_test_0, "task_del_test0", NULL, 7,
                               0, TASK_TEST_STACK_SIZE,
                               task_del_entry_0, 1) != RHINO_SUCCESS) {
        test_case_check_err++;
        printf("task_del_test 0 creat fail \n");
    }

    if (krhino_task_dyn_create(&task_del_test_1, "task_del_test1", NULL, 5,
                               0, TASK_TEST_STACK_SIZE,
                               task_del_entry_1, 1) != RHINO_SUCCESS) {
        test_case_check_err++;
        printf("task_del_test1 creat fail \n");
    }

    if (krhino_task_dyn_create(&task_del_test_2, "task_del_test2", NULL, 59,
                               0, TASK_TEST_STACK_SIZE,
                               task_del_entry_2, 1) != RHINO_SUCCESS) {
        test_case_check_err++;
        printf("task_del_test2 creat fail \n");
    }

    if (test_case_check_err != 0) {
        test_case_fail ++;
        PRINT_RESULT("task_del", FAIL);
    } else {
        test_case_success++;
        PRINT_RESULT("task_del", PASS);
    }

    next_test_case_notify();
}

