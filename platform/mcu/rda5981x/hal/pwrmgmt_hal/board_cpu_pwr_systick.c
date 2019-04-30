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
#include "rda5981.h"

#if (RHINO_CONFIG_CPU_PWR_MGMT > 0)

void systick_suspend(void)
{
    SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
}

void systick_resume(void)
{
    SysTick->CTRL |= (SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);
}
#endif /* RHINO_CONFIG_CPU_PWR_MGMT */

