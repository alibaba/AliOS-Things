/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>

#include "queue_test.h"

void queue_test(void)
{
    task_queue_back_send_test();
    next_test_case_wait();

    task_queue_nowait_recv_test();
    next_test_case_wait();

    task_queue_is_full_test();
    next_test_case_wait();

    task_queue_flush_test();
    next_test_case_wait();

    task_queue_del_test();
    next_test_case_wait();

    task_queue_info_get_test();
    next_test_case_wait();
}

