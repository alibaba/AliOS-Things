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

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)
#include <stdint.h>
#include <stdbool.h>

#include <cpu_tickless.h>

#include <rtl8710b.h>
#include <rtl8710b_delay.h>

#define TIMER_FREQ 32768

static pwr_status_t rtc_init(void);
static uint32_t     rtc_one_shot_max_msec(void);
static pwr_status_t rtc_one_shot_start(uint64_t planUs);
static pwr_status_t rtc_one_shot_stop(uint64_t *pPassedUs);

uint64_t expeted_sleep_ms = 0;

one_shot_timer_t rtc_one_shot = {
    rtc_init,
    rtc_one_shot_max_msec,
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
    if (expeted_sleep_ms < 3) {
        return PWR_ERR;
    }
    timer_counter_start = SYSTIMER_TickGet();
    return PWR_OK;
}

static pwr_status_t rtc_one_shot_stop(uint64_t *pPassedUs)
{
    uint32_t timer_counter_end = SYSTIMER_TickGet();

    if (timer_counter_end >= timer_counter_start) {
        *pPassedUs = (timer_counter_end - timer_counter_start) * 1000000 / TIMER_FREQ;
    } else {
        *pPassedUs = (0xffffffff + timer_counter_end - timer_counter_start) * 1000000 / TIMER_FREQ;
    }
    return PWR_OK;
}

static uint32_t rtc_one_shot_max_msec(void)
{
    if (pmu_get_wakelock_status() == 0) {
        return ((uint64_t)0xffffffff * 1000 / TIMER_FREQ);
    } else {
        return 0;
    }
}

#endif /* PWRMGMT_CONFIG_CPU_LOWPOWER */
