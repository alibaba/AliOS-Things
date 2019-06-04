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

#if (PWRMGMT_CONFIG_CPU_LOWPOWER > 0)

#include <nrf_systick.h>

void systick_suspend(void)
{
    uint32_t ctrl;
    ctrl = nrf_systick_csr_get();
    ctrl &= ~(SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
    nrf_systick_csr_set(ctrl);
}

void systick_resume(void)
{
    uint32_t ctrl;
    ctrl = nrf_systick_csr_get();
    ctrl |= (SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);
    nrf_systick_csr_set(ctrl);
}
#endif /* PWRMGMT_CONFIG_CPU_LOWPOWER */

