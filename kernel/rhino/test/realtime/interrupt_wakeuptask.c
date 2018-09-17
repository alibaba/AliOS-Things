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
#define TEST_TASK2_NAME "rt_test2"
#define TEST_TASK1_PRI  TEST_TASK_PRIORITY      /* low priority task */
#define TEST_TASK2_PRI  (TEST_TASK_PRIORITY  - 1)  /* high priority task */

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
    time_start = HR_COUNT_GET();
    krhino_sem_give(&test_sem);
}

static void test_task1(void *arg)
{ 
    hr_timer_t tmp;
    while (test_count < TEST_ITERATION) {        
        TRIGGER_HIGH_PRI_INTRTP(&tmp);
    }
}

static void test_task2(void *arg)
{
    hr_timer_t time_current;
    while (test_count < TEST_ITERATION) {
        krhino_sem_take(&test_sem, RHINO_WAIT_FOREVER);
        time_end = HR_COUNT_GET();

        if ((time_end >= time_start) && (rttest_aux_intrpt_occurred() != true)) {
            time_current = time_end - time_start;
            rttest_aux_record_result(time_current, &time_sum, &time_max, &time_min);
            test_count++;
        }
    }

    krhino_sem_give(&wait_test_end);
}

void rttest_intrpt_wakeuptask(void)
{
    kstat_t ret;

    test_data_init();

    rttest_aux_intrpt_test_init(highpri_intrpt_process, NULL);

    krhino_sem_create(&wait_test_end, "test_sync", 0);

    ret = krhino_sem_create(&test_sem, "test_sem", 0);
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

    rttest_aux_show_result(INTRPT_WAKEUPTASK_ID, "intrpt_wakeuptask", test_count, time_sum, time_max, time_min);
    krhino_task_del(&test_task2_tcb);
    krhino_task_del(&test_task1_tcb);
    krhino_sem_del(&test_sem);
    krhino_sem_del(&wait_test_end);
}

#endif
