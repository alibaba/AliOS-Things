/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
 * According user manual of STM32L496-DISCOVERY
 * TIM2 and TIM5 are 32-bit timer, other timer is 16-bit.
 * This file will use TIM5 to supply one-shot start/stop service
 * for CPU tickless module.
 * verifyied on STM32L496-DISCOVERY with C1/C2 mode.
 * C1: sleep mode.
 * C2: low power sleep mode.
 */

#include <k_api.h>

#if (AOS_COMP_PWRMGMT > 0)

#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_tim.h"
#include "cpu_tickless.h"

//define TIM5_DBG

#define TIMER_32BIT_MAX_COUNT 0xffffffff

static TIM_HandleTypeDef Tim5Handle;

static uint32_t    freq_of_timer          = 0;
static uint32_t    internal_prescaler_val = 0;
static uint32_t    freq_of_internal_count = 0;
static uint32_t    one_shot_planed_cnt    = 0;
static uint32_t    one_shot_enabled       = 0;
static uint32_t    init_flag              = 0;

static kspinlock_t tim5_spin;

#ifdef TIM5_DBG
static uint32_t    tim5_irq_count         = 0;
static uint32_t    count_per_tick         = 0;

void tim5_info_show(void);
#endif /* TIM5_DBG */

static pwr_status_t tim5_timer_init(void);
static uint32_t     tim5_one_shot_max_seconds(void);
static pwr_status_t tim5_one_shot_start(uint64_t planUs);
static pwr_status_t tim5_one_shot_stop(uint64_t *pPassedUs);

one_shot_timer_t tim5_one_shot = {
    tim5_timer_init,
    tim5_one_shot_max_seconds,
    tim5_one_shot_start,
    tim5_one_shot_stop,
};

pwr_status_t tim5_timer_init(void)
{
    RCC_ClkInitTypeDef clkconfig;
    uint32_t           uwAPB1Prescaler = 0U;
    uint32_t           pFLatency;

    if (init_flag == 1) {
        return PWR_OK;
    }

    krhino_spin_lock_init (&tim5_spin);

    /* Configure the TIM5 IRQ priority */
    HAL_NVIC_SetPriority(TIM5_IRQn, 0, 0U);

    /* Enable the TIM6 global Interrupt */
    HAL_NVIC_EnableIRQ(TIM5_IRQn);

    /* Enable TIM5 clock */
    __HAL_RCC_TIM5_CLK_ENABLE();

    /* Get clock configuration */
    HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

    /* Get APB1 prescaler */
    uwAPB1Prescaler = clkconfig.APB1CLKDivider;

    /* Compute TIM5 clock */
    if (uwAPB1Prescaler == RCC_HCLK_DIV1) {
        /* freq_of_timer = 14200000 = 14.2Mhz, which is the input frequency of TIM5 */
        freq_of_timer = HAL_RCC_GetPCLK1Freq();
    } else {
        freq_of_timer = 2 * HAL_RCC_GetPCLK1Freq();
    }

    /*
     * According STM32L4x5 and STM32L4x6 advanced ARM®-based 32-bit MCUs Reference
     * Manual, section 31.4.11 TIMx prescaler (TIMx_PSC):
     *
     * The counter clock frequency CK_CNT is equal to freq(CK_PSC) / (PSC[15:0] + 1).
     * PSC contains the value to be loaded in the active prescaler register at each
     * update event (including when the counter is cleared through UG bit of TIMx_EGR
     * register or through trigger controller when configured in “reset mode”).
     */

    /*
     * Compute the prescaler value to have TIM5 counter clock equal to 0.71 MHz,
     * 0.71Mhz = 14.2Mhz / (prescaler + 1) -> prescaler = 19
     * freq_of_internal_count = 0.71MHz
     */

    internal_prescaler_val = 19;

    freq_of_internal_count = freq_of_timer / (internal_prescaler_val + 1);

#ifdef TIM5_DBG
    count_per_tick = (freq_of_internal_count / RHINO_CONFIG_TICKS_PER_SECOND);
#endif

    /* Initialize TIM5 */
    Tim5Handle.Instance = TIM5;

    /*
     * Initialize TIMx peripheral as follow:
     * Period = [(freq_of_internal_count/100) - 1]. to have a (1/100) s time base.
     * ClockDivision = 0
     * Counter direction = Up
     */
    Tim5Handle.Init.Period            = TIMER_32BIT_MAX_COUNT;
    Tim5Handle.Init.Prescaler         = internal_prescaler_val;
    Tim5Handle.Init.ClockDivision     = 0;
    Tim5Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    Tim5Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&Tim5Handle) != HAL_OK) {
        return PWR_ERR;
    }

    /* Return function status */
    init_flag = 1;

    return PWR_OK;
}

/**
 * TIM5_IRQHandler - ISR handle of TIM5 interrupt
 *
 * This function will replace weak one which is defined in
 * startup_stm32l496xx_keil.s
 */
void TIM5_IRQHandler(void)
{
#ifdef TIM5_DBG
    tim5_irq_count++;
#endif

    krhino_intrpt_enter();
    HAL_TIM_IRQHandler(&Tim5Handle);
    krhino_intrpt_exit();
}

/* return the max period(in second) that could trigger interrupt */
uint32_t tim5_one_shot_max_seconds(void)
{
    /* the max 32 bit value / count frequency */
    return (0xffffffff / 710000);
}

/**
 * tim5_one_shot_start - enable the timer in oneshot mode
 *
 * This function enables the timer in oneshot mode, the interrupt will be fired
 * after servral microseconds which is indicated by planUs.
 *
 * @return  PWR_OK or PWR_ERR if timer is not enabled
 */
pwr_status_t tim5_one_shot_start
(
    uint64_t  planUs      /* IN */
)
{
    uint64_t cnt;

    krhino_spin_lock_irq_save(&tim5_spin);

    /*
     * convert microsecond into count value according
     * current frequency.
     */

    cnt = planUs * freq_of_internal_count / 1000000;
#ifdef TIM5_DBG
    printf("planUs = %lld, cnt = %lld, tim5_irq_count = %d\n",
           planUs, cnt, tim5_irq_count);
#endif

    /*
     * if request plan time is longer than the time support,
     * just return ERORR, it is responsibility of caller to
     * to handle this PWR_ERR.
     */
    if (cnt > (uint64_t)TIMER_32BIT_MAX_COUNT) {
        krhino_spin_unlock_irq_restore (&tim5_spin);
        return (PWR_ERR);
    }

    /*set counter to 0, it will increase from 0 */
    __HAL_TIM_SET_COUNTER(&Tim5Handle, 0);

    /* update hardware with new one autoload value */
    __HAL_TIM_SET_AUTORELOAD(&Tim5Handle, cnt);

    one_shot_planed_cnt = cnt;
    one_shot_enabled    = 1;

    /*
    Bit 2 URS: Update request source
    This bit is set and cleared by software to select the UEV event sources.
    0:  Any of the following events generate an update interrupt or DMA request
        if enabled.
        These events can be:
        – Counter overflow/underflow
        – Setting the UG bit
        – Update generation through the slave mode controller
    1:  Only counter overflow/underflow generates an update interrupt or DMA
        request if enabled.
    */
    __HAL_TIM_URS_ENABLE(&Tim5Handle);

    /* clear any pending interrupt of update interrupt */
    __HAL_TIM_CLEAR_IT(&Tim5Handle, TIM_IT_UPDATE);

    /* enable update interrupt */
    (void)HAL_TIM_Base_Start_IT(&Tim5Handle);

    krhino_spin_unlock_irq_restore (&tim5_spin);

    return PWR_OK;

}

/**
 * tim5_one_shot_stop - cancel the timer which was in oneshot mode
 *
 * This function is called to cancel the timer which was in oneshot mode.
 * the time has passed(microseconds) will be return in pPassedUs.
 * @param[out]  pPassedUs
 * @return  PWR_OK or PWR_ERR if timer is not disabled
 */
pwr_status_t tim5_one_shot_stop(uint64_t *pPassedUs)
{

    uint32_t count;
    uint32_t passedCount32;
    uint64_t passedCount64;

    /* validate the parameters */
    *pPassedUs = 0;

    krhino_spin_lock_irq_save(&tim5_spin);

    if (!one_shot_enabled) {
        krhino_spin_unlock_irq_restore (&tim5_spin);
        return PWR_OK;
    }

    /* read the current count from hardware */
    count = __HAL_TIM_GET_COUNTER(&Tim5Handle);

    /*
    tim5_one_shot_stop() will be called in this path:
    interrupt happened -> TIM5_IRQHandler() -> krhino_intrpt_enter()
    -> cpu_pwr_up() -> _func_cpu_tickless_up() ->
    cpu_tickless_exit() -> tickless_one_shot_stop() -> tim5_one_shot_stop()

    So this function will called before the real ISR handle of TIM5 which
    is HAL_TIM_IRQHandler(), we can check the interrupt status before
    ISR handle of TIM5 is executed, and leave the cleaning job be finished
    in ISR handle of TIM5.
    */

    if (__HAL_TIM_GET_FLAG(&Tim5Handle, TIM_FLAG_UPDATE) != RESET) {
        if (__HAL_TIM_GET_IT_SOURCE(&Tim5Handle, TIM_IT_UPDATE) != RESET) {

            /*
            if the interrupt flag is set, that means that count has
            already run from 0 to one_shot_planed_cnt, and reload
            from 0, so the real count should be added with
            one_shot_planed_cnt.
            */
            count += one_shot_planed_cnt;
        }
    }

    /* reset the one_shot_planed_cnt to zero */

    one_shot_planed_cnt = 0;

    /* calculate how many count has passed sinced last compare value */
    passedCount32 = count;

    passedCount64 = (uint64_t)passedCount32;

    /* convert count value into microseconds */
    * pPassedUs = (passedCount64 * 1000 * 1000) / freq_of_internal_count;

    HAL_TIM_Base_Stop_IT(&Tim5Handle);

    one_shot_enabled = 0;

    krhino_spin_unlock_irq_restore (&tim5_spin);

    return PWR_OK;
}

#ifdef TIM5_DBG
void tim5_info_show(void)
{
    printf("freq_of_timer          = %d\n", freq_of_timer);
    printf("freq_of_internal_count = %d\n", freq_of_internal_count);
    printf("internal_prescaler_val = %d\n", internal_prescaler_val);
    printf("count_per_tick         = %d\n", count_per_tick);
    printf("max intrpt seconds     = %d\n", tim5_one_shot_max_seconds());
    printf("tim5_irq_count         = %d\n", tim5_irq_count);
}
#endif /* TIM5_DBG */

#endif /* AOS_COMP_PWRMGMT */
