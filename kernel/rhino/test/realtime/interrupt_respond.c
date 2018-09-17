/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/real_time_test.h"

#if ENABLE_INTRPT_RESPOND_WAKEUP

#define TEST_TASK_STACKSIZE 256

#define TEST_TASK1_NAME "rt_test1"
#define TEST_TASK1_PRI  TEST_TASK_PRIORITY

static void test_data_init()
{
    time_sum   = 0;
    time_max   = 0;
    time_min   = HR_TIMER_MAX;

    test_count = 0;

    time_start = HR_TIMER_MAX;
    time_end = 0;
}

static void highpri_intrpt_process()
{
    hr_timer_t time_current;

    time_end = HR_COUNT_GET();

    if ((time_end >= time_start) && (rttest_aux_intrpt_occurred() != true)) {
        time_current = time_end - time_start;
        rttest_aux_record_result(time_current, &time_sum, &time_max, &time_min);
        test_count++;
    }
}

static void test_task1(void *arg)
{ 
    while (test_count < TEST_ITERATION) {        
        TRIGGER_HIGH_PRI_INTRTP(&time_start);
    }

    krhino_sem_give(&wait_test_end);   
}

void rttest_intrpt_respond(void)
{
    kstat_t      stat;

    test_data_init();

    rttest_aux_intrpt_test_init(highpri_intrpt_process, NULL);

    krhino_sem_create(&wait_test_end, "test_sync", 0);

    krhino_task_create(&test_task1_tcb, TEST_TASK1_NAME, 0, TEST_TASK1_PRI, 50,
                       test_task1_stack, TEST_TASK_STACKSIZE, test_task1, 0);

    rttest_aux_intrpt_check_init();
    /* insert into head of task ready list */
    krhino_task_resume(&test_task1_tcb);

    krhino_sem_take(&wait_test_end, RHINO_WAIT_FOREVER);

    rttest_aux_show_result(INTRPT_RESPOND_ID, "intrpt_respond", test_count, time_sum, time_max, time_min);

    krhino_task_del(&test_task1_tcb);
    krhino_sem_del(&wait_test_end);
}

#endif
