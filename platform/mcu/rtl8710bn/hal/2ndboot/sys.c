/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "sys.h"
#include "sys_api.h"

void sys_delayms(volatile int ms)
{
    DelayMs(ms);
}

void sys_reboot(void)
{
    printf("begin reboot!\n");
    /* Set processor clock to default(2: 31.25MHz) before system reset */
    HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1, 0x00000021);
    DelayUs(100*1000);

    /*  Cortex-M3 SCB->AIRCR */
    SCB->AIRCR  = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) |	// VECTKEY
                (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |	// PRIGROUP
                SCB_AIRCR_SYSRESETREQ_Msk);		// SYSRESETREQ
}

