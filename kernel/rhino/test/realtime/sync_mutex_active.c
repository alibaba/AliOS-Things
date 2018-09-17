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
#define AUX_TASK_NAME   "aux_test"
#define TEST_TASK1_PRI  (TEST_TASK_PRIORITY -2)      /* high priority task */
#define TEST_TASK2_PRI  (TEST_TASK_PRIORITY-1)       /*  mid priority task */
#define AUX_TASK_PRI    (TEST_TASK_PRIORITY)        /* lowest priority task */

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

    krhino_mutex_lock(&test_mutex, RHINO_WAIT_FOREVER);

    while (test_count < TEST_ITERATION) {
        krhino_sem_take(&aux_sem_cooperation, RHINO_WAIT_FOREVER);
        time_start = HR_COUNT_GET();
        krhino_mutex_unlock(&test_mutex);
        time_end = HR_COUNT_GET();

        if ((time_end >= time_start) && (rttest_aux_intrpt_occurred() != true)) {
            time_current = time_end - time_start;
            rttest_aux_record_result(time_current, &time_sum, &time_max, &time_min);
            test_count++;
        }

        krhino_sem_take(&aux_sem_regain_mutex, RHINO_WAIT_FOREVER);
        krhino_mutex_lock(&test_mutex, RHINO_WAIT_FOREVER);
    }

    krhino_sem_give(&wait_test_end);
}

static void test_task2(void *arg)
{
    while (test_count < TEST_ITERATION) {
        krhino_mutex_lock(&test_mutex, RHINO_WAIT_FOREVER);
        krhino_mutex_unlock(&test_mutex);
        krhino_sem_give(&aux_sem_regain_mutex);
    }
}

static void test_aux_task(void *arg)
{
    while (test_count < TEST_ITERATION) {
        krhino_sem_give(&aux_sem_cooperation);
    }
}

void rttest_mutex_active(void)
{
    kstat_t ret;

    test_data_init();

    krhino_sem_create(&wait_test_end, "test_sync", 0);

    ret = krhino_mutex_create(&test_mutex, "test_mutex");
    if (ret != RHINO_SUCCESS) {
        return;
    }

    krhino_sem_create(&aux_sem_regain_mutex, "aux_sem1", 0);
    krhino_sem_create(&aux_sem_cooperation, "aux_sem2", 0);
    
    krhino_task_create(&test_task1_tcb, TEST_TASK1_NAME, 0, TEST_TASK1_PRI, 50,
                       test_task1_stack, TEST_TASK_STACKSIZE, test_task1, 0);
    krhino_task_create(&test_task2_tcb, TEST_TASK2_NAME, 0, TEST_TASK2_PRI, 50,
                       test_task2_stack, TEST_TASK_STACKSIZE, test_task2, 0);
    krhino_task_create(&aux_task_tcb, AUX_TASK_NAME, 0, AUX_TASK_PRI, 50,
                       aux_task_stack, TEST_TASK_STACKSIZE, test_aux_task, 0);
    
    rttest_aux_intrpt_check_init();
    krhino_task_resume(&test_task1_tcb);
    krhino_task_resume(&test_task2_tcb);
    krhino_task_resume(&aux_task_tcb);

    krhino_sem_take(&wait_test_end, RHINO_WAIT_FOREVER);

    rttest_aux_show_result(SYNC_MUTEX_ACTIVE_TASK_ID, "sync_mutex_active_task", test_count, time_sum, time_max, time_min);

    krhino_task_del(&aux_task_tcb);
    krhino_task_del(&test_task2_tcb);
    krhino_task_del(&test_task1_tcb);   
    krhino_sem_del(&aux_sem_cooperation); 
    krhino_sem_del(&aux_sem_regain_mutex);
    krhino_mutex_del(&test_mutex);
    krhino_sem_del(&wait_test_end);
}