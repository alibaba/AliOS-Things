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

#define TEST_TASK2_NAME "rt_test2"
#define TEST_TASK2_PRI  TEST_TASK_PRIORITY

static void test_data_init()
{
    time_sum   = 0;
    time_max   = 0;
    time_min   = HR_TIMER_MAX;

    test_count = 0;

    time_start = HR_TIMER_MAX;
    time_end = 0;
}

static void test_task1(void *arg)
{
    hr_timer_t time_current;
    krhino_queue_back_send(&test_queue2, queue_send_msg2);
    while (test_count < TEST_ITERATION) {
        time_start = HR_COUNT_GET();
        krhino_queue_recv(&test_queue1, RHINO_WAIT_FOREVER, &queue_recv_msg1);
        krhino_queue_back_send(&test_queue2, queue_send_msg2);
        time_end = HR_COUNT_GET();
        
        if (queue_recv_msg1 != queue_send_msg1) {
            continue;
        }

        if(rttest_aux_intrpt_occurred() == true) {
            continue;
        }

        if (time_end >= time_start) {
            time_current = time_end - time_start;
            rttest_aux_record_result(time_current, &time_sum, &time_max, &time_min);

            test_count++; 
        }
    }

    krhino_sem_give(&wait_test_end);
}

static void test_task2(void *arg)
{
    krhino_queue_recv(&test_queue2, RHINO_WAIT_FOREVER, &queue_recv_msg2);
    while (test_count < TEST_ITERATION) {
        krhino_queue_back_send(&test_queue1, queue_send_msg1);
        krhino_queue_recv(&test_queue2, RHINO_WAIT_FOREVER, &queue_recv_msg2);
    }
}

void rttest_message_rrwp_task(void)
{
    kstat_t ret;

    test_data_init();

    krhino_sem_create(&wait_test_end, "test_sync", 0);

    ret = krhino_queue_create(&test_queue1, "test_queue1",
                                  (void **)&test_queue_msg1, TEST_QUEUE_MSG_SIZE);
    if (ret != RHINO_SUCCESS) {
        return;
    }

    ret = krhino_queue_create(&test_queue2, "test_queue2",
                                  (void **)&test_queue_msg2, TEST_QUEUE_MSG_SIZE);
    if (ret != RHINO_SUCCESS) {
        return;
    }

    krhino_task_create(&test_task1_tcb, TEST_TASK1_NAME, 0, TEST_TASK1_PRI, 50,
                       test_task1_stack, TEST_TASK_STACKSIZE, test_task1, 0);
    krhino_task_create(&test_task2_tcb, TEST_TASK2_NAME, 0, TEST_TASK2_PRI, 50,
                       test_task2_stack, TEST_TASK_STACKSIZE, test_task2, 0);

    rttest_aux_intrpt_check_init();
    /* insert into head of task ready list */
    krhino_task_resume(&test_task1_tcb);
    krhino_task_resume(&test_task2_tcb);

    krhino_sem_take(&wait_test_end, RHINO_WAIT_FOREVER);

    rttest_aux_show_result(MESSAGE_QUEUE_RRWP_TASK_ID, "message_queue_rrwp", test_count, time_sum, time_max, time_min);
    krhino_task_del(&test_task2_tcb);
    krhino_task_del(&test_task1_tcb);
    krhino_queue_del(&test_queue2);
    krhino_queue_del(&test_queue1);
    krhino_sem_del(&wait_test_end);
}