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
#define TEST_TASK2_NAME "rt_test2"
#define TEST_TASK1_PRI  TEST_TASK_PRIORITY
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
    while (test_count < TEST_ITERATION) {
        time_start = HR_COUNT_GET();
        krhino_task_yield();
    }
}

static void test_task2(void *arg)
{
    hr_timer_t time_current;
    krhino_task_yield();
    while (test_count < TEST_ITERATION) {
        time_end = HR_COUNT_GET();

        if(rttest_aux_intrpt_occurred() == true) {
            krhino_task_yield();
            continue;
        }
        
        if (time_end >= time_start) {
            time_current = time_end - time_start;
            rttest_aux_record_result(time_current, &time_sum, &time_max, &time_min);

            test_count++;
        }
        krhino_task_yield();
    }

    krhino_sem_give(&wait_test_end);
}

void rttest_schedule_yield(void)
{
    test_data_init();
    krhino_sem_create(&wait_test_end, "test_sync", 0);
    
    krhino_task_create(&test_task1_tcb, TEST_TASK1_NAME, 0, TEST_TASK1_PRI, 50,
                       test_task1_stack, TEST_TASK_STACKSIZE, test_task1, 0);

    krhino_task_create(&test_task2_tcb, TEST_TASK2_NAME, 0, TEST_TASK2_PRI, 50,
                       test_task2_stack, TEST_TASK_STACKSIZE, test_task2, 0);
    
    rttest_aux_intrpt_check_init();
    /* insert into head of task ready list */
    krhino_task_resume(&test_task1_tcb);
    krhino_task_resume(&test_task2_tcb);

    krhino_sem_take(&wait_test_end, RHINO_WAIT_FOREVER);

    rttest_aux_show_result(SCHED_YIELD_ID, "sched_yield", test_count, time_sum, time_max, time_min);
    krhino_task_del(&test_task2_tcb);
    krhino_task_del(&test_task1_tcb);    
    krhino_sem_del(&wait_test_end);
}