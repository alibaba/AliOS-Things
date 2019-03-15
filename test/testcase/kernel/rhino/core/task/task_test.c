/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>

extern void task_sleep_test(void);
extern void task_suspend_test(void);
extern void task_yield_test(void);
extern void task_yield_test(void);
extern void task_misc_test(void);

extern void task_del_test(void);

void task_test(void)
{

    task_del_test();
    next_test_case_wait();

    task_sleep_test();
    next_test_case_wait();
    task_suspend_test();
    next_test_case_wait();
    task_yield_test();
    next_test_case_wait();
    task_misc_test();
    next_test_case_wait();
}

