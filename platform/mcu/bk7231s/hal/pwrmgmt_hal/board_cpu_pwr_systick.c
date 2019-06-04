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


void systick_suspend(void)
{
}

void systick_resume(void)
{
}
#endif /* PWRMGMT_CONFIG_CPU_LOWPOWER */

