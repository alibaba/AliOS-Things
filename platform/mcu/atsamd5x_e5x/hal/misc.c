/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "atmel_start.h"

uint32_t atsamd5x_e5x_switch_flash_bank = 0;

void hal_reboot(void)
{
	printf("reboot!\n");
	if (atsamd5x_e5x_switch_flash_bank == FBSW_MAGIC) {
		atsamd5x_e5x_switch_flash_bank = 0;

		__disable_irq();
		__DMB();

		/* Switch Flash bank */
		while (!hri_nvmctrl_get_STATUS_READY_bit(NVMCTRL)) {
			/* Wait until this module isn't busy */
		}
		hri_nvmctrl_write_CTRLB_reg(NVMCTRL, NVMCTRL_CTRLB_CMD_BKSWRST | NVMCTRL_CTRLB_CMDEX_KEY);
		while (1); /* Waiting for MCU reboot */
	} else {
		_reset_mcu();
	}
}

