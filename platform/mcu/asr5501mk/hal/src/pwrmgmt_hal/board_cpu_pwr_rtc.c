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

#include <pmu.h>

#define RTC_FREQ      (32768)

static pwr_status_t rtc_init(void);
static uint32_t     rtc_one_shot_max_msec(void);
static pwr_status_t rtc_one_shot_start(uint64_t planUs);
static pwr_status_t rtc_one_shot_stop(uint64_t *pPassedUs);

one_shot_timer_t rtc_one_shot = {
    rtc_init,
    rtc_one_shot_max_msec,
    rtc_one_shot_start,
    rtc_one_shot_stop,
};

static pwr_status_t rtc_init(void)
{
    return PWR_OK;
}

static pwr_status_t rtc_one_shot_start(uint64_t planUs)
{
    uint32_t status = 0;
    uint32_t cc_counter = planUs * RTC_FREQ / 1000000;

    if ((planUs / 1000) < 4) {
        return PWR_ERR;
    }

    status = lega_drv_rtc_cc_set(cc_counter, PMU_STATE_MODEMSLEEP);
    if (status == 0) {
        lega_drv_rtc_enable();
        return PWR_OK;
    } else {
        return PWR_ERR;
    }
}

static pwr_status_t rtc_one_shot_stop(uint64_t *pPassedUs)
{
    uint32_t counter = 0;
    counter = lega_drv_rtc_counter_get();
    *pPassedUs = (uint64_t)counter * 1000000 / RTC_FREQ;
    lega_drv_goto_active();
    return PWR_OK;
}

static uint32_t rtc_one_shot_max_msec(void)
{
    return (lega_drv_rtc_max_ticks_get() * (uint64_t)1000 / RTC_FREQ);
}

#endif /* AOS_COMP_PWRMGMT */
