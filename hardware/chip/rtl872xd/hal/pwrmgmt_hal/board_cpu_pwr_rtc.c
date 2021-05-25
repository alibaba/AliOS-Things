/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
 * This file supplied RTC one-shot start/stop services for CPU tickless
 * module, verifyied on STM32L496-DISCOVERY with C3/C4 mode.
 * C3: stop mode.
 * C4: standby mode.
 */
#include <k_api.h>

#if (AOS_COMP_PWRMGMT > 0)
#include <stdint.h>
#include <stdbool.h>

#include <cpu_tickless.h>

#include "ameba_soc.h"

#define TIMER_FREQ 32768

int cpu_pwr_ready_status_get(void);

uint32_t pmu_set_sysactive_time(uint32_t timeout);

static pwr_status_t rtc_init(void);
static uint32_t     rtc_one_shot_max_seconds(void);
static pwr_status_t rtc_one_shot_start(uint64_t planUs);
static pwr_status_t rtc_one_shot_stop(uint64_t *pPassedUs);

uint64_t expeted_sleep_ms = 0;

one_shot_timer_t rtc_one_shot = {
    rtc_init,
    rtc_one_shot_max_seconds,
    rtc_one_shot_start,
    rtc_one_shot_stop,
};

static uint32_t timer_counter_start = 0;

static pwr_status_t rtc_init(void)
{
    SYSTIMER_Init();
    return PWR_OK;
}

static pwr_status_t rtc_one_shot_start(uint64_t planUs)
{
    expeted_sleep_ms = planUs / 1000;
    timer_counter_start = SYSTIMER_TickGet();
    return PWR_OK;
}

static pwr_status_t rtc_one_shot_stop(uint64_t *pPassedUs)
{
    uint32_t timer_counter_end = SYSTIMER_TickGet();

    if (timer_counter_end >= timer_counter_start) {
        *pPassedUs = (timer_counter_end - timer_counter_start) * (uint64_t)1000000 / TIMER_FREQ;
    } else {
        *pPassedUs = (0xffffffff + timer_counter_end - timer_counter_start) * (uint64_t)1000000 / TIMER_FREQ;
    }

//    pmu_set_sysactive_time(2 + (*pPassedUs) / 1000);
    return PWR_OK;
}

static uint32_t rtc_one_shot_max_seconds(void)
{
    if (cpu_pwr_ready_status_get() == 1) {
        return (0xffffffff / TIMER_FREQ);
    } else {
        return 0;
    }
}

#endif /* AOS_COMP_PWRMGMT */
