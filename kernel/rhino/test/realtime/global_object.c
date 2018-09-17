/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/real_time_test.h"

uint64_t   time_sum   = 0;
hr_timer_t time_max   = 0;
hr_timer_t time_min   = HR_TIMER_MAX;
volatile uint32_t   test_count = 0;

hr_timer_t   time_start = HR_TIMER_MAX, time_end = 0;

uint64_t   time_sum_alloc   = 0;
hr_timer_t time_max_alloc   = 0;
hr_timer_t time_min_alloc   = HR_TIMER_MAX;

uint64_t   time_sum_free   = 0;
hr_timer_t time_max_free   = 0;
hr_timer_t time_min_free   = HR_TIMER_MAX;

uint64_t   time_sum_send   = 0;
hr_timer_t time_max_send   = 0;
hr_timer_t time_min_send   = HR_TIMER_MAX;

uint64_t   time_sum_rev   = 0;
hr_timer_t time_max_rev   = 0;
hr_timer_t time_min_rev   = HR_TIMER_MAX;

uint64_t   time_sum_request   = 0;
hr_timer_t time_max_request   = 0;
hr_timer_t time_min_request   = HR_TIMER_MAX;

uint64_t   time_sum_release   = 0;
hr_timer_t time_max_release   = 0;
hr_timer_t time_min_release   = HR_TIMER_MAX;

ktask_t     test_task1_tcb;
cpu_stack_t test_task1_stack[TEST_TASK_STACKSIZE];

ktask_t     test_task2_tcb;
cpu_stack_t test_task2_stack[TEST_TASK_STACKSIZE];

ktask_t     aux_task_tcb;
cpu_stack_t aux_task_stack[TEST_TASK_STACKSIZE];

ksem_t  test_sem;
kmutex_t  test_mutex;
ksem_t    test_sem1,test_sem2;
ksem_t   aux_sem;
ksem_t   aux_sem_regain_mutex;
ksem_t   aux_sem_cooperation;

kqueue_t test_queue;
void    *test_queue_msg[TEST_QUEUE_MSG_SIZE];
void    *queue_send_msg = (void *)1;
void    *queue_recv_msg = NULL;

kqueue_t test_queue1;
void    *test_queue_msg1[TEST_QUEUE_MSG_SIZE];
void    *queue_send_msg1 = (void *)1;
void    *queue_recv_msg1 = NULL;

kqueue_t test_queue2;
void    *test_queue_msg2[TEST_QUEUE_MSG_SIZE];
void    *queue_send_msg2 = (void *)1;
void    *queue_recv_msg2 = NULL;

ksem_t   wait_test_end;
