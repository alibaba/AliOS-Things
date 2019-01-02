/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/real_time_test.h"

hr_timer_t rt_measure_waste = 0;
sys_time_t old_systick = 0;

intrpt_callback_t  highpri_intrpt_func = NULL;
intrpt_callback_t  lowpri_intrpt_func = NULL;

#if RTTEST_DEBUG
hr_timer_t debug_data[TEST_ITERATION];
uint32_t   debud_data_count = 0;
#endif

void rttest_aux_measure_waste(void)
{
    hr_timer_t m1;
    hr_timer_t m2;

    m1 = HR_COUNT_GET();
    m2 = HR_COUNT_GET();

    rt_measure_waste = m2 - m1;
}

void rttest_aux_show_sysconfig()
{
    printf("\n=======================AliOS Things Test Environment===================\n");
    printf("test evironment congfig:\n");
    printf("  systick frequency    :  %dHZ\n",RHINO_CONFIG_TICKS_PER_SECOND);
    printf("  CPU clock frequency  :  %dHZ\r\n", (uint32_t)(CPU_CLOCK_REQ * 1000000));
    printf("  timer clock frequency:  %dHZ\r\n", (uint32_t)(HR_COUNT_FREQ() * 1000000));
    printf("========================================================================\n"); 
}

void rttest_aux_show_result_header(void)
{
    printf("\n=======================AliOS Things RealTime Test======================\n");
    printf("TestId    Function              Avg(us)   Min(us)   Max(us)   TestCount \n");
    printf("------------------------------------------------------------------------\n");
}

void rttest_aux_show_result(uint32_t test_id, char * test_name, uint32_t test_count, uint64_t time_sum,
                        hr_timer_t time_max, hr_timer_t time_min) 
{
#if RTTEST_DEBUG
    printf("test record data:");
    for (uint32_t i = 0; i < debud_data_count; i++) {
        if(i % 10 == 0) {
            printf("\n");   
        }
        printf("%d ", debug_data[i]);
    }
    printf("\n");
    debud_data_count = 0;
#endif

    printf("T%-6d",test_id);
    printf("%-25s",test_name);
    
    printf("%-10.2f", (float)time_sum/test_count/HR_COUNT_FREQ());
    printf("%-10.2f", (float)time_min/HR_COUNT_FREQ());
    printf("%-10.2f", (float)time_max/HR_COUNT_FREQ());    

    printf("%-10d\n",test_count);
}

void rttest_aux_show_result_end(void)
{
    printf("========================================================================\n");
}

void rttest_aux_record_result(hr_timer_t time_current, uint64_t *time_sum,
                          hr_timer_t *time_max, hr_timer_t *time_min)
{
    if(time_current > rt_measure_waste) {
        time_current -= rt_measure_waste;
    } else {
        time_current = 0;
    }

#if RTTEST_DEBUG
    if (debud_data_count < TEST_ITERATION) {
        debug_data[debud_data_count++] = time_current;
    }
#endif

    *time_sum += time_current;
    if (time_current > *time_max) {
        *time_max = time_current;
    }
    if (time_current < *time_min) {
        *time_min = time_current;
    }
}

/* try to avoid systick interrupt */
void rttest_aux_intrpt_check_init(void)
{
    sys_time_t systick;
    systick = krhino_sys_tick_get();

    while (systick == krhino_sys_tick_get());

    old_systick = krhino_sys_tick_get();    
}

/* check systick interrupt */
bool rttest_aux_intrpt_occurred(void)
{
    sys_time_t systick;
    systick = krhino_sys_tick_get();
    if(old_systick != systick) {
        old_systick = systick;
        return true;
    }
    return false;
}

void rttest_aux_intrpt_test_init(intrpt_callback_t highp_func, intrpt_callback_t lowp_func)
{
    hal_rttest_intrpt_init();

    highpri_intrpt_func = highp_func;
    lowpri_intrpt_func  = lowp_func;
}

void rttest_max_disintrpt()
{
    hr_timer_t intrpt_disalbe_time = g_intrpt_disable_max_time;
    if(intrpt_disalbe_time > rt_measure_waste) {
        intrpt_disalbe_time -= rt_measure_waste;
    } else {
        intrpt_disalbe_time = 0;
    }

    printf("\nMax intrpt disable time :%6.2fus\n", (float)intrpt_disalbe_time/HR_COUNT_FREQ());
}
