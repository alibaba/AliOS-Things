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

#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_rtc.h"
#include "stm32l4xx_hal_uart.h"
#include "cpu_tickless.h"

//#define RTC_ONE_SHOT_DBG

extern void SystemClock_Config(void);

#define RTC_WAKEUP_SOURCE_FREQ_2000HZ
//#define RTC_WAKEUP_SOURCE_FREQ_1HZ

/* According reference manual of STM32496G, RTC_WUTR is 16bit */
#define RTC_WAKE_UP_RGE_MAX 0xffff

static uint32_t          one_shot_enabled     = 0;
static uint32_t          one_shot_start_value = 0;
static uint32_t          one_shot_max_period  = 0; /*microseconds */
static kspinlock_t       rtc_spin;
static RTC_HandleTypeDef RtcHandle;

#ifdef RTC_ONE_SHOT_DBG
static int               rtc_wakeup_irq_count  = 0;
#endif /* RTC_ONE_SHOT_DBG */

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

static uint32_t rtc_one_shot_max_msec(void)
{
    return one_shot_max_period;
}

static pwr_status_t rtc_init(void)
{
#ifdef RTC_ONE_SHOT_DBG
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    RTC_AlarmTypeDef alarm;
#endif
    RCC_PeriphCLKInitTypeDef PeriphClkInit;

    krhino_spin_lock_init(&rtc_spin);

    /* Configure the RTC IRQ priority */
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0, 0U);
    //HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0U);

    /* Enable the RTC global Interrupt bit from NVIC */
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
    //HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

    /* config the souce clock of RTC */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    /* LSI: low speed internal RC clock, 32000Hz */
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    //PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
#ifdef RTC_ONE_SHOT_DBG
        printf("HAL_RCCEx_PeriphCLKConfig() for RTC failed\n");
#endif
        return PWR_ERR;
    }

    __HAL_RCC_RTC_ENABLE();

    /* Initialize RTC */
    memset(&RtcHandle, 0, sizeof(RTC_HandleTypeDef));

    RtcHandle.Instance            = RTC;
    RtcHandle.Init.HourFormat     = RTC_HOURFORMAT_24;
    /* freq(ck_apre) = 256Hz = __HAL_RCC_GET_RTC_SOURCE()/(124+1) */
    RtcHandle.Init.AsynchPrediv   = 124;
    /* freq(ck_spre) = 1Hz = freq(ck_apre)/(255+1) */
    RtcHandle.Init.SynchPrediv    = 255;
    RtcHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
    RtcHandle.Init.OutPutRemap    = RTC_OUTPUT_REMAP_NONE;
    RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RtcHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;

#ifdef RTC_ONE_SHOT_DBG
    printf("__HAL_RCC_GET_RTC_SOURCE() = %d\n", __HAL_RCC_GET_RTC_SOURCE());
    printf("HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_RTC) = %dHz\n",
           HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_RTC));
#endif /* RTC_ONE_SHOT_DBG */

    if (HAL_RTC_Init(&RtcHandle) != HAL_OK) {
#ifdef RTC_ONE_SHOT_DBG
        printf("HAL_RTC_Init(&RtcHandle) failed\n");
#endif /* RTC_ONE_SHOT_DBG */
        return PWR_ERR;
    }

#ifdef RTC_WAKEUP_SOURCE_FREQ_2000HZ
    one_shot_max_period = (RTC_WAKE_UP_RGE_MAX * (uint64_t)1000 / 2000);
#endif
#ifdef RTC_WAKEUP_SOURCE_FREQ_1HZ
    one_shot_max_period = RTC_WAKE_UP_RGE_MAX * 1000;
#endif

#ifdef RTC_ONE_SHOT_DBG

    /* Initialize RTC and set the Time and Date */
    sTime.Hours          = 0x0;
    sTime.Minutes        = 0x0;
    sTime.Seconds        = 0x0;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    if (HAL_RTC_SetTime(&RtcHandle, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        return PWR_ERR;
    }

    sDate.WeekDay = RTC_WEEKDAY_WEDNESDAY;
    sDate.Month   = RTC_MONTH_JANUARY;
    sDate.Date    = 24;
    sDate.Year    = 2018;

    if (HAL_RTC_SetDate(&RtcHandle, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
        return PWR_ERR;
    }

#endif /* RTC_ONE_SHOT_DBG */

    return PWR_OK;
}

/**
 * rtc_one_shot_start - enable the timer in oneshot mode
 *
 * This function enables the timer in oneshot mode, the interrupt will be fired
 * after servral microseconds which is indicated by planUs.
 *
 * @return  PWR_OK or PWR_ERR if timer is not enabled
 */
static pwr_status_t rtc_one_shot_start(uint64_t planUs)
{
    uint32_t wakeup_counter = 0;
    uint64_t wakeup_ms      = planUs / 1000;
    uint32_t wakeup_s       = wakeup_ms / 1000;
    cpu_cpsr_t flags_cpsr;

    if ((wakeup_s > (one_shot_max_period / 1000))
#ifdef RTC_WAKEUP_SOURCE_FREQ_1HZ
        || (wakeup_s == 0))
#endif
        ) {
        return PWR_ERR;
    }

    krhino_spin_lock_irq_save (&rtc_spin, flags_cpsr);

    /* Clear all related wakeup flags, PWR_FLAG_WU + RTC_FLAG_WUTF*/
    /* Clear PWR wake up Flag */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

    /* Clear RTC Wake Up timer Flag */
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RtcHandle, RTC_FLAG_WUTF);

    /*
    RTC Wakeup Interrupt Generation:

    for RTC_WAKEUPCLOCK_RTCCLK_DIV16:
        Wakeup Time Base = (16 /(LSE))
        Wakeup Time = Wakeup Time Base * WakeUpCounter
                    = (16 /(LSE)) * WakeUpCounter
        ==> WakeUpCounter = Wakeup Time / Wakeup Time Base
        To configure the wake up timer to 2s the WakeUpCounter is set to 0xA017:
          RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16
          Wakeup Time Base = 16 /(32000Hz) = 0.5 ms
          Wakeup Time = 2s = 0.5ms  * WakeUpCounter
          ==> WakeUpCounter = 2s/0.5ms = 2000ms/0.5ms = 4000 = 0xFA0
    [code]
    wakeup_counter = (uint32_t)wakeup_ms * 2;
    HAL_RTCEx_SetWakeUpTimer_IT(&RtcHandle, wakeup_counter, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

    for RTC_WAKEUPCLOCK_CK_SPRE_16BITS:
    which means the clock source of WUTR is ck_spre, when ck_spre frequency
    is 1Hz, this allows to achieve a wakeup time from 1 s to around 18 hours
    with one-second resolution.

        Wakeup Time Base = 1second
    */

#ifdef RTC_WAKEUP_SOURCE_FREQ_2000HZ
    wakeup_counter = (uint32_t)wakeup_ms * 2;
    one_shot_start_value = wakeup_counter;

    HAL_RTCEx_SetWakeUpTimer_IT(&RtcHandle, wakeup_counter,
                                RTC_WAKEUPCLOCK_RTCCLK_DIV16);

#endif

#ifdef RTC_WAKEUP_SOURCE_FREQ_1HZ

    one_shot_start_value = wakeup_s;

    HAL_RTCEx_SetWakeUpTimer_IT(&RtcHandle, wakeup_s,
                                RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
#endif
    one_shot_enabled = 1;

    krhino_spin_unlock_irq_restore (&rtc_spin, flags_cpsr);

    return PWR_OK;
}

/**
 * one_shot_planed_cnt - cancel the timer which was in oneshot mode
 *
 * This function is called to cancel the timer which was in oneshot mode.
 * the time has passed(microseconds) will be return in pPassedUs.
 * @param[out]  pPassedUs
 * @return  PWR_OK or PWR_ERR if timer is not disabled
*/
static pwr_status_t rtc_one_shot_stop(uint64_t *pPassedUs)
{
    uint32_t wutr_hw = RtcHandle.Instance->WUTR;
#ifdef RTC_WAKEUP_SOURCE_FREQ_1HZ
    uint32_t passed_seconds = 0;
#endif
    cpu_cpsr_t flags_cpsr;

    krhino_spin_lock_irq_save (&rtc_spin, flags_cpsr);

    if (!one_shot_enabled) {
        *pPassedUs = 0;
        krhino_spin_unlock_irq_restore(&rtc_spin, flags_cpsr);
        return PWR_OK;
    }

    /* when system wakeup again, the clock system should be recovered */
    SystemClock_Config();

    /*
    WUTF: Wakeup timer flag
    This flag is set by hardware when the wakeup auto-reload counter reaches 0.
    This flag is cleared by software by writing 0.
    */

    if (__HAL_RTC_WAKEUPTIMER_GET_FLAG(&RtcHandle, RTC_FLAG_WUTF)) {
#ifdef RTC_WAKEUP_SOURCE_FREQ_2000HZ
        *pPassedUs = one_shot_start_value * 500;
#elif defined(RTC_WAKEUP_SOURCE_FREQ_1HZ)
        *pPassedUs = one_shot_start_value * 1000 * 1000;
#endif
    } else {
#ifdef RTC_WAKEUP_SOURCE_FREQ_2000HZ
        *pPassedUs = (one_shot_start_value - wutr_hw) * 500;
#elif defined(RTC_WAKEUP_SOURCE_FREQ_1HZ)
        passed_seconds = one_shot_start_value - wutr_hw;
        *pPassedUs = passed_seconds * 1000 * 1000;
#endif
    }

    /* Clear all related wakeup flags, PWR_FLAG_WU + RTC_FLAG_WUTF*/
    /* Clear PWR wake up Flag */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

    /* Clear RTC Wake Up timer Flag */
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RtcHandle, RTC_FLAG_WUTF);

    /* Disable RTC Wake-up timer */
    if (HAL_RTCEx_DeactivateWakeUpTimer(&RtcHandle) != HAL_OK) {
        krhino_spin_unlock_irq_restore (&rtc_spin, flags_cpsr);
        return PWR_ERR;
    }

    one_shot_enabled = 0;

    krhino_spin_unlock_irq_restore (&rtc_spin, flags_cpsr);

    return PWR_OK;
}

/**
 * RTC_WKUP_IRQHandler - ISR handle of RTC wakeup interrupt
 *
 * This function will replace weak one which is defined in
 * startup_stm32l496xx_keil.s
 */
void RTC_WKUP_IRQHandler(void)
{
    krhino_intrpt_enter();

    HAL_RTCEx_WakeUpTimerIRQHandler(&RtcHandle);

    krhino_intrpt_exit();
}

#ifdef RTC_ONE_SHOT_DBG
void rtc_info_show(void)
{
    RTC_TimeTypeDef rtcTime;
    RTC_DateTypeDef rtcDate;

    HAL_RTC_GetTime (&RtcHandle, &rtcTime, RTC_FORMAT_BIN );

    /*
     * According the test result, if only read the time information with
     * HAL_RTC_GetTime(), the second will not increase, sulotion is
     * calling HAL_RTC_GetDate() after HAL_RTC_GetTime().
     *
     * A example with same issue from internet:
     * http://www.eemaker.com/stm32-rtczoubudong.html
     */

    HAL_RTC_GetDate (&RtcHandle, &rtcDate, RTC_FORMAT_BIN );

    printf("[%d]Year(%04d)-Month(%02d)-Date(%02d)-Hours(%02d)-Minutes(%02d)-Seconds(%02d)\n",
           rtc_wakeup_irq_count,
           rtcDate.Year, rtcDate.Month, rtcDate.Date,
           rtcTime.Hours, rtcTime.Minutes, rtcTime.Seconds);
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    rtc_wakeup_irq_count++;
}

#endif /* RTC_ONE_SHOT_DBG */

#endif /* PWRMGMT_CONFIG_CPU_LOWPOWER */
