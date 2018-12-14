/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>

#include "aos/kernel.h"

#include "test_realtime.h"

#define TEST_MAIN_STACKSIZE 256

#define TEST_MAIN_NAME "rt_main"
#define TEST_MAIN_PRI  (TEST_TASK_PRIORITY - 3)

static ktask_t     test_main_tcb;
static cpu_stack_t test_main_stack[TEST_MAIN_STACKSIZE];
static ksem_t      wait_main_end;

static intrpt_callback_t test_case[] = {
#if ENABLE_INTRPT_RESPOND_WAKEUP
    test_realtime_intrpt_respond, /* t1 */
#else
    NULL,
#endif
#if ENABLE_INTRPT_PREEMPT
    test_realtime_intrpt_preempt, /* t2 */
#else
    NULL,
#endif

#if ENABLE_INTRPT_RESPOND_WAKEUP
    test_realtime_intrpt_wakeuptask, /* t3 */
#else
    NULL,
#endif

    test_realtime_sched_yield,         /* t4 */
    test_realtime_sched_preempt,       /* t5 */
    test_realtime_sched_resume_active, /* t6 */
    test_realtime_sync_mutex,          /* t7 */
    test_realtime_sync_sem,            /* t8 */
    test_realtime_shuffling_sem,       /* t9 */
    test_realtime_shuffling_mutex,     /* t10 */
    test_realtime_sem_active,          /* t11 */
    test_realtime_mutex_active,        /* t12 */
    test_realtime_sync_sem_rrwp_task,  /* t13 */

    test_realtime_message_queue,       /* t14 */
    test_realtime_message_rrwp_task,   /* t15 */

    test_realtime_memory_blk_alloc,    /* t16 t18*/
    test_realtime_memory_tlf_alloc,    /* t17 t19*/
};

uint32_t id_to_index[] = {0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 15, 16};

static void rttest_entry(void * arg)
{
    uint32_t id = (uint32_t)arg;
    uint32_t index;

    if(id >= sizeof(id_to_index) / sizeof(id_to_index[0])) {
        printf("the test id is too large\n");
        krhino_sem_give(&wait_main_end);
        return;
    }

    index = id_to_index[id];

    krhino_overhead_measure();

    if (id == 0) {
        rttest_aux_show_sysconfig();
        rttest_aux_show_result_header();
        for (uint32_t i=0; i< sizeof(test_case)/sizeof(intrpt_callback_t); i++) {
            if(test_case[i] != NULL) {
                test_case[i]();
            }
        }
        rttest_aux_show_result_end();
#if (RHINO_CONFIG_INTRPT_STATS > 0)
        rttest_max_disintrpt();
#endif
    } else {
        rttest_aux_show_result_header();
        if (test_case[index] != NULL) {
            test_case[index]();
        }
        rttest_aux_show_result_end();
    }

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
