/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>

#include "aos/kernel.h"

#include "test_realtime.h"

#define TEST_ALL_CASE 0

#define TEST_MAIN_STACKSIZE 256

#define TEST_MAIN_NAME "rt_main"
#define TEST_MAIN_PRI  (TEST_TASK_PRIORITY - 3)

static ktask_t     test_main_tcb;
static cpu_stack_t test_main_stack[TEST_MAIN_STACKSIZE];
static ksem_t      wait_main_end;

static void rttest_entry(void * arg)
{
    krhino_overhead_measure();
    rttest_aux_show_sysconfig();
    rttest_aux_show_result_header();

#if (TEST_ALL_CASE > 0)
#if ENABLE_INTRPT_RESPOND_WAKEUP
    test_realtime_intrpt_respond();
#endif
#if ENABLE_INTRPT_PREEMPT
    test_realtime_intrpt_preempt();
#endif

#if ENABLE_INTRPT_RESPOND_WAKEUP
    test_realtime_intrpt_wakeuptask();
#endif

    test_realtime_sched_yield();
    test_realtime_sched_preempt();
    test_realtime_sched_resume_active();
    test_realtime_sync_mutex();
    test_realtime_sync_sem();
    test_realtime_shuffling_sem();
    test_realtime_shuffling_mutex();
    test_realtime_sem_active();
    test_realtime_mutex_active();
    test_realtime_sync_sem_rrwp_task();

    test_realtime_message_queue();
    test_realtime_message_rrwp_task();

    test_realtime_memory_blk_alloc();
    test_realtime_memory_tlf_alloc();
#else
    test_realtime_sched_yield();
    test_realtime_sched_preempt();
    test_realtime_shuffling_sem();
    test_realtime_shuffling_mutex();
    test_realtime_message_queue();
    test_realtime_memory_tlf_alloc();
#endif /* TEST_ALL_CASE > 0 */

    rttest_aux_show_result_end();

#if (RHINO_CONFIG_SYS_STATS > 0)
    rttest_max_disintrpt();
#endif

    krhino_sem_give(&wait_main_end);
}

void test_realtime_main(uint32_t id)
{
    krhino_sem_create(&wait_main_end, "main_sync", 0);

    krhino_task_create(&test_main_tcb, TEST_MAIN_NAME, id, TEST_MAIN_PRI, 50,
                      test_main_stack, TEST_MAIN_STACKSIZE, rttest_entry, 1);

    krhino_sem_take(&wait_main_end, RHINO_WAIT_FOREVER);

    krhino_task_del(&test_main_tcb);
    krhino_sem_del(&wait_main_end);
}
