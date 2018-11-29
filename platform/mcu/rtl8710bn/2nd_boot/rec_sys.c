/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "rec_sys.h"
#include "sys_api.h"

void rec_delayms(volatile int timesMS)
{
	DelayMs(timesMS);
}

void rec_hal_init()
{
    rec_uart_init();
    rec_flash_init();
}

void rec_err_print(void *errinfo)
{
    printf("rec Exception.\r\n");
}

void rec_reboot(void)
{
	printf("begin reboot!\n");
	/* Set processor clock to default(2: 31.25MHz) before system reset */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1, 0x00000021);
	DelayUs(100*1000);

	/*  Cortex-M3 SCB->AIRCR */
	SCB->AIRCR  = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) |	// VECTKEY
		(SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |		// PRIGROUP
		SCB_AIRCR_SYSRESETREQ_Msk);					// SYSRESETREQ
}

