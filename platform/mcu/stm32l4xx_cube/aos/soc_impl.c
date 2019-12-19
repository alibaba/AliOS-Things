/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <assert.h>
#include <stdio.h>
#include <sys/time.h>

extern void hal_reboot(void);

#if (RHINO_CONFIG_HW_COUNT > 0)
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_tim.h"
#include "stm32l4xx_hal_rcc.h"

#define TIMER_32BIT_MAX_COUNT 0xffffffff
TIM_HandleTypeDef Tim2Handle;

void soc_hw_timer_init(void)
{
    __HAL_RCC_TIM2_CLK_ENABLE();

    Tim2Handle.Instance               = TIM2;
    Tim2Handle.Init.Period            = TIMER_32BIT_MAX_COUNT;  /* ARR */
    Tim2Handle.Init.Prescaler         = 0;                      /* PSC */
    Tim2Handle.Init.ClockDivision     = 0;                      /* CR1 register CKD[1:0] bit */
    Tim2Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;     /* CR1 register DIR and CMS bit */
    Tim2Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; /* CR1 register ARPE bit */

    if (HAL_TIM_Base_Init(&Tim2Handle) != HAL_OK) {
        return;
    }

    __HAL_TIM_CLEAR_IT(&Tim2Handle, TIM_IT_UPDATE);
    (void)HAL_TIM_Base_Start_IT(&Tim2Handle);
    __HAL_TIM_URS_ENABLE(&Tim2Handle);
}

hr_timer_t soc_hr_hw_cnt_get(void)
{
   return __HAL_TIM_GET_COUNTER(&Tim2Handle);
}

lr_timer_t soc_lr_hw_cnt_get(void)
{
    return __HAL_TIM_GET_COUNTER(&Tim2Handle);
}

float soc_hr_hw_freq_mhz(void)
{
   uint32_t hclk;
   hclk = HAL_RCC_GetHCLKFreq();
   return hclk/1000000.0;
}

#endif /* RHINO_CONFIG_HW_COUNT */

#if (RHINO_CONFIG_MM_TLF > 0)

#if defined (__CC_ARM) /* Keil / armcc */
extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;
//comes from component board, may different to each other as hardware resource
extern size_t g_iram1_start;
extern size_t g_iram1_total_size;
k_mm_region_t g_mm_region[1];
int           g_region_num = 1;
void aos_heap_set(void)
{
    g_mm_region[0].start = (uint8_t*)&Image$$RW_IRAM1$$ZI$$Limit;
    g_mm_region[0].len   =
        (g_iram1_start + g_iram1_total_size - (size_t)&Image$$RW_IRAM1$$ZI$$Limit);
}
#elif defined (__ICCARM__)/* IAR */
#define HEAP_BUFFER_SIZE 1024*30
uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
k_mm_region_t g_mm_region[] = {{g_heap_buf, HEAP_BUFFER_SIZE}};
int           g_region_num = 1;
void aos_heap_set(void)
{
}
#else /* GCC */
extern void         *_estack;
extern void         *__bss_end__;
/* __bss_end__ and _estack is set by linkscript(*.ld)
   heap and stack begins from __bss_end__ to _estack */
k_mm_region_t g_mm_region[1];
int           g_region_num = 1;
void aos_heap_set(void)
{
    g_mm_region[0].start = (uint8_t*)&__bss_end__;
    g_mm_region[0].len   =
        ((uint8_t*)&_estack - (uint8_t*)&__bss_end__) - RHINO_CONFIG_SYSTEM_STACK_SIZE;
}
#endif
#endif


void soc_err_proc(kstat_t err)
{
    (void)err;

    hal_reboot();
}

krhino_err_proc_t g_err_proc = soc_err_proc;

