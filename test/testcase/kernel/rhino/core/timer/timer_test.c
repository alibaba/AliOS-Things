/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>

#include "timer_test.h"

void timer_test(void)
{
    task_timer_create_del_test();
    next_test_case_wait();

    task_timer_dyn_create_del_test();
    next_test_case_wait();

    task_timer_start_stop_test();
    next_test_case_wait();

    task_timer_change_test();
    next_test_case_wait();
}

