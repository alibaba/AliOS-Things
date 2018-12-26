/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cmd_util.h"
#include "driver/chip/hal_prcm.h"

#define CMD_REBOOT_BY_WDG	1

#if CMD_REBOOT_BY_WDG

#include "driver/chip/hal_wdg.h"

enum cmd_status cmd_reboot(PRCM_CPUABootFlag flag)
{
	cmd_write_respond(CMD_STATUS_OK, cmd_get_status_desc(CMD_STATUS_OK));

	HAL_PRCM_SetCPUABootFlag(flag);
	HAL_WDG_Reboot();
	return CMD_STATUS_ACKED;
}

#else /* CMD_REBOOT_BY_WDG */

#include "driver/chip/hal_nvic.h"
#include "driver/chip/hal_global.h"
#include "driver/chip/system_chip.h"
#include "sys/interrupt.h"

#define CMD_BROM_RESET_HANDLER	(*((volatile uint32_t *)0x4))

enum cmd_status cmd_reboot(PRCM_CPUABootFlag flag)
{
	uint32_t handler;

	cmd_write_respond(CMD_STATUS_OK, cmd_get_status_desc(CMD_STATUS_OK));
	cmd_msleep(10);

	HAL_PRCM_SetCPUABootFlag(flag);
	handler = CMD_BROM_RESET_HANDLER;
#ifdef __CONFIG_CHIP_XR871
	handler |= 0x1; /* set thumb bit */
#endif

	arch_irq_disable();
	HAL_PRCM_DisableSys2();
	HAL_PRCM_DisableSys2Power();
	HAL_GlobalInit();
	SystemDeInit(0);

	SCB->VTOR = 0x0;
	__set_CONTROL(0); /* reset to Privileged Thread mode and use MSP */
	__DSB();
	__ISB();
	((NVIC_IRQHandler)handler)();
	return CMD_STATUS_ACKED;
}

#endif /* CMD_REBOOT_BY_WDG */

enum cmd_status cmd_reboot_exec(char *cmd)
{
	return cmd_reboot(PRCM_CPUA_BOOT_FROM_COLD_RESET);
}

enum cmd_status cmd_upgrade_exec(char *cmd)
{
	return cmd_reboot(PRCM_CPUA_BOOT_FROM_SYS_UPDATE);
}
