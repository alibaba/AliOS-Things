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
    time_sum_request   = 0;
    time_max_request   = 0;
    time_min_request   = HR_TIMER_MAX;

    time_sum_release   = 0;
    time_max_release   = 0;
    time_min_release   = HR_TIMER_MAX;

    test_count = 0;
}

static void test_task1(void *arg)
{
    hr_timer_t time_start_request = HR_TIMER_MAX, time_end_request = 0;
    hr_timer_t time_start_release = HR_TIMER_MAX, time_end_release = 0;
    hr_timer_t time_current;

    while (test_count < TEST_ITERATION) {        
        time_start_request = HR_COUNT_GET();
        krhino_mutex_lock(&test_mutex, RHINO_WAIT_FOREVER);
        time_end_request = HR_COUNT_GET();

        time_start_release = HR_COUNT_GET();
        krhino_mutex_unlock(&test_mutex);
        time_end_release = HR_COUNT_GET();

        if(rttest_aux_intrpt_occurred() == true) {
            continue;
        }

        if ((time_end_request >= time_start_request) && (time_end_release >= time_start_release)) {
            time_current = time_end_request - time_start_request;
            rttest_aux_record_result(time_current, &time_sum_request, &time_max_request, &time_min_request);

            time_current = time_end_release - time_start_release;
            rttest_aux_record_result(time_current, &time_sum_release, &time_max_release, &time_min_release);

            test_count++;
        }
    }

    krhino_sem_give(&wait_test_end);
}

void rttest_sync_mutex(void)
{
    kstat_t ret;

    test_data_init();

    krhino_sem_create(&wait_test_end, "test_sync", 0);

    ret = krhino_mutex_create(&test_mutex, "test_mutex");
    if (ret != RHINO_SUCCESS) {
        return;
    }

    krhino_task_create(&test_task1_tcb, TEST_TASK1_NAME, 0, TEST_TASK1_PRI, 50,
                       test_task1_stack, TEST_TASK_STACKSIZE, test_task1, 0);

    rttest_aux_intrpt_check_init();
    /* insert into head of task ready list */
    krhino_task_resume(&test_task1_tcb);

    krhino_sem_take(&wait_test_end, RHINO_WAIT_FOREVER);

    rttest_aux_show_result(SYNC_MUTEX_OPERATION_ID, "syn_mutex_request", test_count, time_sum_request, time_max_request, time_min_request);
    rttest_aux_show_result(SYNC_MUTEX_OPERATION_ID, "syn_mutex_release", test_count, time_sum_release, time_max_release, time_min_release);
    krhino_task_del(&test_task1_tcb);
    krhino_mutex_del(&test_mutex);
    krhino_sem_del(&wait_test_end);
}