/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>
#include <sys/time.h>

#if (RHINO_CONFIG_REALTIME_TEST > 0)

#include <stm32l496xx.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_tim.h"
#include "stm32l4xx_hal_rcc.h"

typedef void (*intrpt_callback_t)(void);
extern intrpt_callback_t highpri_intrpt_func;
extern intrpt_callback_t lowpri_intrpt_func;

float soc_cpu_clock_freq_mhz(void)
{
   uint32_t hclk;
   hclk = HAL_RCC_GetHCLKFreq();
   return hclk/1000000.0;
}

void hal_rttest_intrpt_init() {
    HAL_NVIC_ClearPendingIRQ(SVCall_IRQn);
    HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0U);
    HAL_NVIC_EnableIRQ(SVCall_IRQn);

    HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0U);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

void hal_rttest_trigger_highintrpt(hr_timer_t* trig_time)
{
    *trig_time = HR_COUNT_GET();
    __asm volatile ("svc 1");
}

void hal_rttest_trigger_lowintrpt(hr_timer_t* trig_time)
{
    *trig_time = HR_COUNT_GET();
    HAL_NVIC_SetPendingIRQ(TIM2_IRQn);
}

void SVC_Handler(void)
{
    if(highpri_intrpt_func != NULL) {
        highpri_intrpt_func();
    }
}

extern TIM_HandleTypeDef Tim2Handle;
uint32_t tim_uper_32 = 0;
void TIM2_IRQHandler(void)
{
    if(lowpri_intrpt_func != NULL)
        lowpri_intrpt_func();

    tim_uper_32++;
    HAL_TIM_IRQHandler(&Tim2Handle);
}

#endif

