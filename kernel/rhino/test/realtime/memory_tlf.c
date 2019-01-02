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
    time_sum_alloc   = 0;
    time_max_alloc   = 0;
    time_min_alloc   = HR_TIMER_MAX;

    time_sum_free   = 0;
    time_max_free   = 0;
    time_min_free   = HR_TIMER_MAX;

    test_count = 0;
}

static void test_task1(void *arg)
{
    hr_timer_t time_start_alloc = HR_TIMER_MAX, time_end_alloc = 0;
    hr_timer_t time_start_free = HR_TIMER_MAX, time_end_free = 0;
    hr_timer_t time_current;
    void * tmp;

    while (test_count < TEST_ITERATION) {        
        time_start_alloc = HR_COUNT_GET();
        tmp = krhino_mm_alloc(TLF_MEMORY_ALLOC_SIZE);
        time_end_alloc = HR_COUNT_GET();

        time_start_free = HR_COUNT_GET();
        krhino_mm_free(tmp);
        time_end_free = HR_COUNT_GET();
        
        if(rttest_aux_intrpt_occurred() == true) {
            continue;
        }

        if ((time_end_alloc >= time_start_alloc) && (time_end_free >= time_start_free)) {
            time_current = time_end_alloc - time_start_alloc;
            rttest_aux_record_result(time_current, &time_sum_alloc, &time_max_alloc, &time_min_alloc);

            time_current = time_end_free - time_start_free;
            rttest_aux_record_result(time_current, &time_sum_free, &time_max_free, &time_min_free);
            test_count++;
        }
    }

    krhino_sem_give(&wait_test_end);
}

void rttest_memory_tlf_alloc(void)
{
    test_data_init();
    
    krhino_sem_create(&wait_test_end, "test_sync", 0);

    krhino_task_create(&test_task1_tcb, TEST_TASK1_NAME, 0, TEST_TASK1_PRI, 50,
                       test_task1_stack, TEST_TASK_STACKSIZE, test_task1, 0);

    rttest_aux_intrpt_check_init();
    /* insert into head of task ready list */
    krhino_task_resume(&test_task1_tcb);

    krhino_sem_take(&wait_test_end, RHINO_WAIT_FOREVER);
#if RHINO_CONFIG_MM_REGION_MUTEX
    rttest_aux_show_result(MEM_TLF_MUTEX_CRITICAL_ID, "mem_tlf_alloc", test_count, time_sum_alloc, time_max_alloc, time_min_alloc);
    rttest_aux_show_result(MEM_TLF_MUTEX_CRITICAL_ID, "mem_tlf_free", test_count, time_sum_free, time_max_free, time_min_free);
#else
    rttest_aux_show_result(MEM_TLF_INTRPT_CRITICAL_ID, "mem_tlf_alloc", test_count, time_sum_alloc, time_max_alloc, time_min_alloc);
    rttest_aux_show_result(MEM_TLF_INTRPT_CRITICAL_ID, "mem_tlf_free", test_count, time_sum_free, time_max_free, time_min_free);
#endif

    krhino_task_del(&test_task1_tcb);
    krhino_sem_del(&wait_test_end);    
}