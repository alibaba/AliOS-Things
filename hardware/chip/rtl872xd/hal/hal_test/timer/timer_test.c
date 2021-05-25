/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "aos/kernel.h"
#include "aos/hal/timer.h"
#include "timer_test.h"

#define TIMER_PERIOD_US_TEST1 100000
#define TIMER_PERIOD_US_TEST2 200000
#define TIMER_PERIOD_TOLERANCE_RANGE 0.01f

static timer_dev_t timer_dev;
static aos_sem_t timer_test_sem;
static char *msg = "AliOS Things";
static int success_flag = 1;

static uint64_t systime_cur = 0; 
static uint64_t systime_pre = 0; 
static uint64_t period_us_cur = 0;

void timer_handler(void *arg);

int error_flag = 0;
    uint64_t period_us_calc = 0;
void hal_timer_test(void)
{
    int ret = -1;

    printf("*********** timer test start ! ***********\n");

    timer_dev.port = PORT_TIMER_TEST;
    timer_dev.config.period = TIMER_PERIOD_US_TEST1;
    timer_dev.config.reload_mode = TIMER_RELOAD_AUTO;
    timer_dev.config.cb = &timer_handler;
    timer_dev.config.arg = msg;

    ret = aos_sem_new(&timer_test_sem, 0);
    if (ret != 0) {
        success_flag = 0;
        printf("sem new error ! test failed\n");
        return;
    }

    printf("step1: set timer period to %d us, and check the period !\n", timer_dev.config.period);

    ret = hal_timer_init(&timer_dev);
    if (ret != 0) {
        success_flag = 0;
        printf("hal_timer_init error ! test failed\n");
        return;
    }

    period_us_cur = timer_dev.config.period;

    ret = hal_timer_start(&timer_dev);
    if (ret != 0) {
        success_flag = 0;
        printf("hal_timer_start error ! test failed\n");
        return;
    }

    aos_sem_wait(&timer_test_sem, AOS_WAIT_FOREVER);

    ret = hal_timer_finalize(&timer_dev);
    if (ret != 0) {
        success_flag = 0;
        printf("hal_timer_finalize error ! test failed\n");
        return;
    }

    printf("error_flag %d\n", error_flag);
    printf("period_us_calc %d\n", (int)period_us_calc);

    if (success_flag == 1) {
        printf("timer test result: 【success】 !\n");
    } else {
        printf("timer test result: 【failed】 !\n");
    }

    printf("*********** timer test end ! ***********\n");
}

void timer_handler(void *arg)
{
    static int count = 0;

    int ret = -1;

    timer_config_t timer_conf;

    systime_pre = systime_cur;
    systime_cur = aos_now_ms();

    if (count > 0) {
        period_us_calc = (systime_cur - systime_pre) * 1000;
        if ((period_us_calc < (uint64_t)(period_us_cur * (1.0f - TIMER_PERIOD_TOLERANCE_RANGE)))
            || (period_us_calc > (uint64_t)(period_us_cur * (1.0f + TIMER_PERIOD_TOLERANCE_RANGE)))) {
            success_flag = 0;
            error_flag = 1;
            //printf("timer period error ! test failed\n");
            aos_sem_signal(&timer_test_sem);
        } 
    }

#if (TIMER_ARG_ENABLE == 1)
    if (strcmp(msg, arg) != 0) {
        success_flag = 0;
        //printf("timer_handler arg error ! test failed\n");
        error_flag = 2;
        aos_sem_signal(&timer_test_sem);
    }
#endif

    if (count == 3) {
        timer_conf.period = TIMER_PERIOD_US_TEST2;
        timer_conf.reload_mode = TIMER_RELOAD_AUTO;
        timer_conf.cb = &timer_handler;
        timer_conf.arg = msg;

        //printf("step2: set timer period to %d us, and check the period !\n", timer_conf.period);

        hal_timer_stop(&timer_dev);
        ret = hal_timer_para_chg(&timer_dev, timer_conf);
        if (ret != 0) {
            success_flag = 0;
            error_flag = 3;
            //printf("hal_timer_para_chg error ! test failed\n");
            aos_sem_signal(&timer_test_sem);
        }

        hal_timer_start(&timer_dev);
        period_us_cur = timer_conf.period;
    }

    if (count == 5) {
        aos_sem_signal(&timer_test_sem);
    }

    count++;
}
