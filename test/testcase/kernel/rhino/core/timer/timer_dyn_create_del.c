/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <k_api.h>
#include <test_fw.h>

#include "timer_test.h"

#define TIMER0_ROUND 1
#define TIMER0_MAGIC 0x12345678

static ktask_t   *task_0_test;
static ktimer_t *timer_0_test;
static ksem_t    sem_0_test;

static void timer_0_func(ktimer_t *timer, void *arg)
{
    TIMER_VAL_CHK(timer == timer_0_test);
    TIMER_VAL_CHK((size_t)arg == TIMER0_MAGIC);

    krhino_sem_give(&sem_0_test);
}


static void timer_dyn_create_param_test(void)
{
    kstat_t ret;

    ret = krhino_timer_dyn_create(NULL, "timer_0_test", (timer_cb_t)timer_0_func,
                                  TIMER0_ROUND, TIMER0_ROUND, (void *)TIMER0_MAGIC, 0);
    TIMER_VAL_CHK(ret == RHINO_NULL_PTR);

    ret = krhino_timer_dyn_create(&timer_0_test, NULL, (timer_cb_t)timer_0_func,
                                  TIMER0_ROUND, TIMER0_ROUND, (void *)TIMER0_MAGIC, 0);
    TIMER_VAL_CHK(ret == RHINO_NULL_PTR);

    ret = krhino_timer_dyn_create(&timer_0_test, "timer_0_test", NULL, TIMER0_ROUND,
                                  TIMER0_ROUND, (void *)TIMER0_MAGIC, 0);
    TIMER_VAL_CHK(ret == RHINO_NULL_PTR);

    ret = krhino_timer_dyn_create(&timer_0_test, "timer_0_test",
                                  (timer_cb_t)timer_0_func,
                                  0, TIMER0_ROUND, (void *)TIMER0_MAGIC, 0);
    TIMER_VAL_CHK(ret == RHINO_INV_PARAM);
}

static void timer_dyn_del_param_test(void)
{
    kstat_t  ret;
    ktimer_t timer;

    memset(&timer, 0 , sizeof(ktimer_t));

    ret = krhino_timer_dyn_del(NULL);
    TIMER_VAL_CHK(ret == RHINO_NULL_PTR);

    ret = krhino_timer_dyn_create(&timer_0_test, "timer_0_test",
                                  (timer_cb_t)timer_0_func,
                                  TIMER0_ROUND, TIMER0_ROUND, (void *)TIMER0_MAGIC, 0);
    TIMER_VAL_CHK(ret == RHINO_SUCCESS);

    ret = krhino_timer_dyn_del(timer_0_test);
    TIMER_VAL_CHK(ret == RHINO_SUCCESS);
}

static void task_timer0_entry(void *arg)
{
    kstat_t ret = 0;

    while (1) {
        /* check krhino_timer_dyn_create param */
        timer_dyn_create_param_test();

        /* check krhino_timer_dyn_del param */
        timer_dyn_del_param_test();

        ret = krhino_sem_create(&sem_0_test, "sem_0_test", 0);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_timer_dyn_create(&timer_0_test, "timer_0_test",
                                      (timer_cb_t)timer_0_func,
                                      TIMER0_ROUND, TIMER0_ROUND, (void *)TIMER0_MAGIC, 0);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_timer_dyn_del(timer_0_test);

        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_timer_dyn_create(&timer_0_test, "timer_0_test",
                                      (timer_cb_t)timer_0_func,
                                      TIMER0_ROUND, TIMER0_ROUND, (void *)TIMER0_MAGIC, 1);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        ret = krhino_sem_take(&sem_0_test, RHINO_WAIT_FOREVER);
        TIMER_VAL_CHK(ret == RHINO_SUCCESS);

        TIMER_VAL_CHK(ret == RHINO_SUCCESS);
        ret = krhino_timer_stop(timer_0_test);
        ret = krhino_timer_dyn_del(timer_0_test);
        if (ret == RHINO_SUCCESS) {
            test_case_success++;
            PRINT_RESULT("timer dyn create&del", PASS);
        } else {
            test_case_fail++;
            PRINT_RESULT("timer dyn create&del", FAIL);
        }

        krhino_sem_del(&sem_0_test);

        next_test_case_notify();
        krhino_task_dyn_del(task_0_test);
    }
}

kstat_t task_timer_dyn_create_del_test(void)
{
    kstat_t ret;

    ret = krhino_task_dyn_create(&task_0_test, "task_timer0_test", 0, 10,
                                 0, TASK_TEST_STACK_SIZE, task_timer0_entry, 1);
    TIMER_VAL_CHK((ret == RHINO_SUCCESS) || (ret == RHINO_STOPPED));

    return 0;
}

