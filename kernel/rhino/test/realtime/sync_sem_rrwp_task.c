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
    hr_timer_t time_current;
    krhino_sem_give(&test_sem2);
    while (test_count < TEST_ITERATION) {  
        time_start = HR_COUNT_GET();
        krhino_sem_take(&test_sem1, RHINO_WAIT_FOREVER);
        krhino_sem_give(&test_sem2);

        time_end = HR_COUNT_GET();
        if ((time_end >= time_start) && (rttest_aux_intrpt_occurred() != true)) {
            time_current = time_end - time_start;
            rttest_aux_record_result(time_current, &time_sum, &time_max, &time_min);
            test_count++;
        }
    }
    krhino_sem_give(&wait_test_end);
}

static void test_task2(void *arg)
{
    krhino_sem_take(&test_sem2, RHINO_WAIT_FOREVER);  /* rerun from pend */
    while (test_count < TEST_ITERATION) {
        krhino_sem_give(&test_sem1);
        krhino_sem_take(&test_sem2, RHINO_WAIT_FOREVER);
    }
}

void rttest_sync_sem_rrwp_task(void)
{
    kstat_t ret;

    test_data_init();

    krhino_sem_create(&wait_test_end, "test_sync", 0);

    ret = krhino_sem_create(&test_sem1, "test_sem1", 0);
    if (ret != RHINO_SUCCESS) {
        return;
    }

    ret = krhino_sem_create(&test_sem2, "test_sem2", 0);
    if (ret != RHINO_SUCCESS) {
        return;
    }

    krhino_task_create(&test_task1_tcb, TEST_TASK1_NAME, 0, TEST_TASK1_PRI, 50,
                       test_task1_stack, TEST_TASK_STACKSIZE, test_task1, 0);
    krhino_task_create(&test_task2_tcb, TEST_TASK2_NAME, 0, TEST_TASK2_PRI, 50,
                       test_task2_stack, TEST_TASK_STACKSIZE, test_task2, 0);
    
    rttest_aux_intrpt_check_init();
    
    krhino_task_resume(&test_task1_tcb);  
    krhino_task_resume(&test_task2_tcb);    /* task2 run first */
    
    krhino_sem_take(&wait_test_end, RHINO_WAIT_FOREVER);

    rttest_aux_show_result(SYNC_SEM_RRWP_TASK_ID, "syn_sem_rrwaketask", test_count, time_sum, time_max, time_min);

    krhino_task_del(&test_task2_tcb);
    krhino_task_del(&test_task1_tcb);    
    krhino_sem_del(&test_sem2);
    krhino_sem_del(&test_sem1);
    krhino_sem_del(&wait_test_end);
}