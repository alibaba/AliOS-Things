/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/real_time_test.h"

#define TEST_TASK_STACKSIZE 256

#define TEST_TASK1_NAME "rt_test1"
#define TEST_TASK1_PRI  TEST_TASK_PRIORITY

static void test_data_init()
{
    time_sum_send   = 0;
    time_max_send   = 0;
    time_min_send   = HR_TIMER_MAX;

    time_sum_rev   = 0;
    time_max_rev   = 0;
    time_min_rev   = HR_TIMER_MAX;

    test_count = 0;
}

static void test_task1(void *arg)
{
    hr_timer_t time_start_send = HR_TIMER_MAX, time_end_send = 0;
    hr_timer_t time_start_rev = HR_TIMER_MAX, time_end_rev = 0;
    hr_timer_t time_current;

    while (test_count < TEST_ITERATION) {        
        time_start_send = HR_COUNT_GET();
        krhino_queue_back_send(&test_queue, queue_send_msg);
        time_end_send = HR_COUNT_GET();

        time_start_rev = HR_COUNT_GET();
        krhino_queue_recv(&test_queue, RHINO_WAIT_FOREVER, &queue_recv_msg);
        time_end_rev = HR_COUNT_GET();
        
        if (queue_recv_msg != queue_send_msg) {
            continue;
        }

        if(rttest_aux_intrpt_occurred() == true) {
            continue;
        }

        if ((time_end_send >= time_start_send) && (time_end_rev >= time_start_rev)) {
            time_current = time_end_send - time_start_send;
            rttest_aux_record_result(time_current, &time_sum_send, &time_max_send, &time_min_send);

            time_current = time_end_rev - time_start_rev;
            rttest_aux_record_result(time_current, &time_sum_rev, &time_max_rev, &time_min_rev); 

            test_count++; 
        }
    }

    krhino_sem_give(&wait_test_end);
}

void rttest_message_queue(void)
{
    kstat_t ret;

    test_data_init();

    krhino_sem_create(&wait_test_end, "test_sync", 0);

    ret = krhino_queue_create(&test_queue, "test_queue",
                              (void **)&test_queue_msg, TEST_QUEUE_MSG_SIZE);
    if (ret != RHINO_SUCCESS) {
        return;
    }

    krhino_task_create(&test_task1_tcb, TEST_TASK1_NAME, 0, TEST_TASK1_PRI, 50,
                       test_task1_stack, TEST_TASK_STACKSIZE, test_task1, 0);

    rttest_aux_intrpt_check_init();
    /* insert into head of task ready list */
    krhino_task_resume(&test_task1_tcb);

    krhino_sem_take(&wait_test_end, RHINO_WAIT_FOREVER);

    rttest_aux_show_result(MESSAGE_QUEUE_OPERATION_ID, "message_queue_send", test_count, time_sum_send, time_max_send, time_min_send);
    rttest_aux_show_result(MESSAGE_QUEUE_OPERATION_ID, "message_queue_rev", test_count, time_sum_rev, time_max_rev, time_min_rev);
    krhino_task_del(&test_task1_tcb);
    krhino_queue_del(&test_queue);
    krhino_sem_del(&wait_test_end);
}