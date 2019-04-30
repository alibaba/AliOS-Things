/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
DESCRIPTION
This file provides two fundtions systick_suspend()/systick_resume()
which is used by cpu tickless module to suspend/resume system tick
interrupt.

Differrent board may has different way to suspend/resume system tick
interrupt, please reference your board/soc user manual to find the
detail for how to implement these two functions.
*/

#include <k_api.h>

#if (AOS_COMP_PWRMGMT > 0)

#include <rtl8710b.h>

#define NVIC_SYSTICK_CTRL_REG (*((volatile uint32_t *) 0xe000e010))

void systick_suspend(void)
{
    pmu_yield_os_set(0);
    NVIC_SYSTICK_CTRL_REG &= ~(SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
}

void systick_resume(void)
{
    NVIC_SYSTICK_CTRL_REG |= (SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);
    pmu_yield_os_set(1);
}

#endif /* AOS_COMP_PWRMGMT */
