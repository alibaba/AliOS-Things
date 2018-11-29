/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <test_fw.h>

#include "buf_queue_test.h"

void buf_queue_test(void)
{
    task_buf_queue_recv_test();
    next_test_case_wait();

    task_buf_queue_del_test();
    next_test_case_wait();

    task_buf_queue_flush_test();
    next_test_case_wait();

    task_buf_queue_info_get_test();
    next_test_case_wait();

    task_buf_queue_dyn_create_test();
    next_test_case_wait();

}

