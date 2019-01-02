/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef GLOBAL_OBJECT_H
#define GLOBAL_OBJECT_H

#include <aos/aos.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#include "real_time_test.h"

extern uint64_t   time_sum;
extern hr_timer_t time_max;
extern hr_timer_t time_min;
extern volatile uint32_t   test_count;

extern hr_timer_t   time_start, time_end;

extern uint64_t   time_sum_alloc;
extern hr_timer_t time_max_alloc;
extern hr_timer_t time_min_alloc;

extern uint64_t   time_sum_free;
extern hr_timer_t time_max_free;
extern hr_timer_t time_min_free;

extern uint64_t   time_sum_send;
extern hr_timer_t time_max_send;
extern hr_timer_t time_min_send;

extern uint64_t   time_sum_rev;
extern hr_timer_t time_max_rev;
extern hr_timer_t time_min_rev;

extern uint64_t   time_sum_request;
extern hr_timer_t time_max_request;
extern hr_timer_t time_min_request;

extern uint64_t   time_sum_release;
extern hr_timer_t time_max_release;
extern hr_timer_t time_min_release;

extern ktask_t     test_task1_tcb;
extern cpu_stack_t test_task1_stack[];

extern ktask_t     test_task2_tcb;
extern cpu_stack_t test_task2_stack[];

extern ktask_t     aux_task_tcb;
extern cpu_stack_t aux_task_stack[];

extern ksem_t  test_sem;
extern kmutex_t  test_mutex;
extern ksem_t    test_sem1,test_sem2;
extern ksem_t   aux_sem_regain_mutex;
extern ksem_t   aux_sem_cooperation;

extern ksem_t   wait_test_end;
extern ksem_t   aux_sem;

extern kqueue_t test_queue;
extern void    *test_queue_msg[];
extern void    *queue_send_msg;
extern void    *queue_recv_msg;

extern kqueue_t test_queue1;
extern void    *test_queue_msg1[];
extern void    *queue_send_msg1;
extern void    *queue_recv_msg1;

extern kqueue_t test_queue2;
extern void    *test_queue_msg2[];
extern void    *queue_send_msg2;
extern void    *queue_recv_msg2;

#endif
