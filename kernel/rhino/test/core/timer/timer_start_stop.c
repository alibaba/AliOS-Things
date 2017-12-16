/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <k_api.h>
#include <test_fw.h>

#include "timer_test.h"

#define TIMER0_ROUND      1
#define TIMER0_MAGIC      0x12345678
#define TIMER0_TEST_TIMES 100

static ktask_t  *task_0_test;
static ktimer_t  timer_0_test;
static ksem_t    sem_0_test;
static uint32_t timer_0_count;

static void timer_0_func(void *timer, void *arg)
{
    krhino_sem_give(&sem_0_test);
}

static void timer_start_param_test()
{
    kstat_t ret;

    ret = krhino_timer_start(NULL);
    TIMER_VAL_CHK(ret == RHINO_NULL_PTR);
}

static void timer_stop_param_test()
{
    kstat_t ret;

    ret = krhino_timer_stop(NULL);
    TIMER_VAL_CHK(ret == RHINO_NULL_PTR);
}

static void task_timer0_entry(void *arg)
{
    kstat_t ret = 0;

    while (1) {
        timer_0_count = 0;

        /* check krhino_timer_start param */
        timer_start_param_test();

        /* check krhino_timer_stop param */
        timer_stop_param_test();

        ret = krhino_sem_create(&sem_0_test, "sem_0_test", 0);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_timer_create(&timer_0_test, "timer_0_test", timer_0_func,
                                  TIMER0_ROUND, TIMER0_ROUND, (void *)TIMER0_MAGIC, 1);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_sem_take(&sem_0_test, RHINO_WAIT_FOREVER);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_timer_stop(&timer_0_test);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_timer_del(&timer_0_test);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        test_case_success++;
        PRINT_RESULT("timer start&stop", PASS);

        krhino_sem_del(&sem_0_test);
        next_test_case_notify();
        krhino_task_dyn_del(task_0_test);
    }
}

kstat_t task_timer_start_stop_test(void)
{
    kstat_t ret;

    ret = krhino_task_dyn_create(&task_0_test, "task_timer0_test", 0, 10,
                                 0, TASK_TEST_STACK_SIZE, task_timer0_entry, 1);
    TIMER_VAL_CHK((ret == RHINO_SUCCESS) || (ret == RHINO_STOPPED));

    return 0;
}

