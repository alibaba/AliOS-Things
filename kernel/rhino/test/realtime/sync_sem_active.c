/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/real_time_test.h"

#define TEST_TASK_STACKSIZE 256

#define TEST_TASK1_NAME    "rt_test1"
#define TEST_TASK2_NAME    "rt_test2"
#define AUX_TASK_NAME      "aux_test"
#define TEST_TASK1_PRI     (TEST_TASK_PRIORITY-2)      /* highest priority task */
#define TEST_TASK2_PRI     (TEST_TASK_PRIORITY-1)      /* mid priority task */
#define AUX_TASK_PRI       (TEST_TASK_PRIORITY)        /* lowest priority task */

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

    while (test_count < TEST_ITERATION) {
        krhino_sem_take(&aux_sem, RHINO_WAIT_FOREVER);

        time_start = HR_COUNT_GET();
        krhino_sem_give(&test_sem);
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
    while (test_count < TEST_ITERATION) {
        krhino_sem_take(&test_sem, RHINO_WAIT_FOREVER);
    }
}

static void test_aux_task(void *arg)
{
    while (test_count < TEST_ITERATION) {
        krhino_sem_give(&aux_sem);
    }
}

void rttest_sem_active(void)
{
    kstat_t ret;

    test_data_init();

    krhino_sem_create(&wait_test_end, "test_sync", 0);

    ret = krhino_sem_create(&test_sem, "test_sem", 0);
    if (ret != RHINO_SUCCESS) {
        return;
    }

    krhino_sem_create(&aux_sem, "aux_sem", 0);

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

    rttest_aux_show_result(SYNC_SEM_ACTIVE_TASK_ID, "sync_sem_active_task", test_count, time_sum, time_max, time_min);

    krhino_task_del(&aux_task_tcb);
    krhino_task_del(&test_task2_tcb);
    krhino_task_del(&test_task1_tcb);
    krhino_sem_del(&aux_sem);
    krhino_sem_del(&test_sem);
    krhino_sem_del(&wait_test_end);
}