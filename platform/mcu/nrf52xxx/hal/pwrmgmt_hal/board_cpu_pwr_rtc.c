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
#include <nrf.h>
#include <nrf_drv_rtc.h>
#include <nrf_drv_clock.h>
#include "app_error.h"
#include <stdint.h>
#include <stdbool.h>

#include <cpu_tickless.h>

const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(1); /**< Declaring an instance of nrf_drv_rtc for RTC1. */
#define RTC_FREQ      (32768)
#define RTC_PRESCALER (RTC_FREQ_TO_PRESCALER(RTC_FREQ))

static pwr_status_t rtc_init(void);
static uint32_t     rtc_one_shot_max_msec(void);
static pwr_status_t rtc_one_shot_start(uint64_t planUs);
static pwr_status_t rtc_one_shot_stop(uint64_t *pPassedUs);

static void nrf_drv_rtc_user_callback(nrf_drv_rtc_int_type_t int_type);

one_shot_timer_t rtc_one_shot = {
    rtc_init,
    rtc_one_shot_max_msec,
    rtc_one_shot_start,
    rtc_one_shot_stop,
};

static void lfclk_config(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    if (err_code != NRF_ERROR_MODULE_ALREADY_INITIALIZED) {
        APP_ERROR_CHECK(err_code);
        nrf_drv_clock_lfclk_request(NULL);
    }
}

static pwr_status_t rtc_init(void)
{
    uint32_t err_code;

    lfclk_config();

    /* Initialize RTC instance */
    nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
    config.prescaler            = RTC_PRESCALER;

    err_code = nrf_drv_rtc_init(&rtc, &config, nrf_drv_rtc_user_callback);
    APP_ERROR_CHECK(err_code);

    nrf_drv_rtc_counter_clear(&rtc);

    if (err_code == NRF_SUCCESS) 
    {
        return PWR_OK;
    }
    else 
    {
        return PWR_ERR;
    }
}

static pwr_status_t rtc_one_shot_start(uint64_t planUs)
{
    ret_code_t err_code;
    uint32_t cc_counter = planUs * RTC_FREQ / 1000000;

    if (cc_counter < 3) {
        return PWR_ERR;
    }

    /* Set compare channel to trigger interrupt after COMPARE_COUNTERTIME seconds */
    err_code = nrf_drv_rtc_cc_set(&rtc, 0, cc_counter, true);
    APP_ERROR_CHECK(err_code);

    /* Power on RTC instance */
    nrf_drv_rtc_enable(&rtc);
    APP_ERROR_CHECK(err_code);

    if (err_code == NRF_SUCCESS) 
    {
        return PWR_OK;
    }
    else 
    {
        return PWR_ERR;
    }
}

static pwr_status_t rtc_one_shot_stop(uint64_t *pPassedUs)
{
    uint32_t counter = 0;
    counter = nrf_drv_rtc_counter_get(&rtc) & RTC_COUNTER_COUNTER_Msk;

    nrf_drv_rtc_cc_disable(&rtc, 0);
    nrf_drv_rtc_counter_clear(&rtc);
    nrf_drv_rtc_disable(&rtc);

    *pPassedUs = (uint64_t)counter * 1000000 / RTC_FREQ; 

    *pPassedUs = *pPassedUs + 30;

    return PWR_OK;
}

static uint32_t rtc_one_shot_max_msec(void)
{
    return (nrf_drv_rtc_max_ticks_get(&rtc) * (uint64_t)1000 / RTC_FREQ);
}

static void nrf_drv_rtc_user_callback(nrf_drv_rtc_int_type_t int_type)
{
    return;
}

#endif /* PWRMGMT_CONFIG_CPU_LOWPOWER */
