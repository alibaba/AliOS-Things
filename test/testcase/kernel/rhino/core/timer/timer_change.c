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
static ktimer_t  timer_1_test;
static ksem_t    sem_0_test;

static void timer_0_func(void *timer, void *arg)
{
    krhino_sem_give(&sem_0_test);
}

static void timer_1_func(void *timer, void *arg)
{
    krhino_sem_give(&sem_0_test);
}

static void timer_change_param_test(void)
{
    kstat_t ret;

    ret = krhino_timer_change(NULL, TIMER0_ROUND, TIMER0_ROUND);
    TIMER_VAL_CHK(ret == RHINO_NULL_PTR);

    ret = krhino_timer_change(&timer_0_test, 0, TIMER0_ROUND);
    TIMER_VAL_CHK(ret == RHINO_INV_PARAM);
}

static void task_timer0_entry(void *arg)
{
    kstat_t ret = 0;

    while (1) {
        /* check krhino_timer_change param */
        timer_change_param_test();

        ret = krhino_sem_create(&sem_0_test, "sem_0_test", 0);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_timer_create(&timer_0_test, "timer_0_test", timer_0_func,
                                  TIMER0_ROUND * 10, TIMER0_ROUND * 10, (void *)TIMER0_MAGIC, 0);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_timer_create(&timer_1_test, "timer_1_test", timer_1_func,
                                  TIMER0_ROUND * 20, TIMER0_ROUND * 20, (void *)TIMER0_MAGIC, 0);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_timer_change(&timer_1_test, TIMER0_ROUND, TIMER0_ROUND);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_timer_start(&timer_1_test);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_timer_start(&timer_0_test);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_sem_take(&sem_0_test, RHINO_WAIT_FOREVER);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_sem_take(&sem_0_test, RHINO_WAIT_FOREVER);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_timer_stop(&timer_0_test);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_timer_del(&timer_0_test);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_timer_stop(&timer_1_test);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_timer_del(&timer_1_test);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        test_case_success++;

        PRINT_RESULT("timer change", PASS);
        krhino_sem_del(&sem_0_test);
        next_test_case_notify();
        krhino_task_dyn_del(task_0_test);
    }
}

kstat_t task_timer_change_test(void)
{
    kstat_t ret;

    ret = krhino_task_dyn_create(&task_0_test, "task_timer0_test", 0, 10,
                                 0, TASK_TEST_STACK_SIZE, task_timer0_entry, 1);
    TIMER_VAL_CHK((ret == RHINO_SUCCESS) || (ret == RHINO_STOPPED));

    return 0;
}

