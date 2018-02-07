/**
  * @file  hal_xip.c
  * @author  XRADIO IOT WLAN Team
  */

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

#include <stdlib.h>
#include <stdio.h>
#include "hal_base.h"
#include "driver/chip/hal_flashctrl.h"
#include "driver/chip/hal_flash.h"
#include "driver/chip/hal_xip.h"
#include "hal_os.h"
#include "sys/param.h"

#include "pm/pm.h"


#define XIP_DEBUG(msg, arg...) XR_DEBUG((DBG_OFF | XR_LEVEL_ERROR), NOEXPAND, "[XIP iface debug] <%s : %d> " msg "\n", __func__, __LINE__, ##arg)
#define XIP_ERROR(msg, arg...) XR_ERROR((DBG_ON | XR_LEVEL_ALL), NOEXPAND, "[XIP iface error] <%s : %d> " msg "\n", __func__, __LINE__, ##arg)

FlashChipBase *getFlashChip(FlashDev *dev);
FlashReadMode getFlashMode(FlashDev *dev);
FlashDev *getFlashDev(uint32_t flash);
void insToFcIns(InstructionField *ins, FC_InstructionField *fcins);
static int setCmd(XipDriverBase *base, InstructionField *cmd, InstructionField *addr, InstructionField *dummy, InstructionField *data);
static int setContinue(XipDriverBase *base, uint32_t continueMode, void *arg);
static int setDelay(XipDriverBase *base, Flashc_Delay *delay);
static int setAddr(XipDriverBase *base, uint32_t addr);
/*void HAL_Flashc_Xip_RawEnable();
void HAL_Flashc_Xip_Enable();
void HAL_Flashc_Xip_RawDisable();
void HAL_Flashc_Xip_Disable();*/

static XipDriverBase xipDrv = {0};
#ifdef CONFIG_PM
static struct soc_device Xip_dev;
#endif


/**
  * @brief Initializes XIP module.
  * @note XIP is a module that cpu can run the code in flash but not ram.
  * @param flash: flash number, this flash must have been initialized, and must
  *               be connected to the flash controller pin.
  * @param xaddr: XIP code start address.
  * @retval HAL_Status: The status of driver
  */
HAL_Status HAL_Xip_Init(uint32_t flash, uint32_t xaddr)
{
	HAL_Status ret = HAL_OK;
	FlashDev *dev;
	FlashChipBase *chip;
	FlashBoardCfg *cfg;
	int devNum = HAL_MKDEV(HAL_DEV_MAJOR_FLASH, flash);

	HAL_BoardIoctl(HAL_BIR_GET_CFG, devNum, (uint32_t)&cfg);
	if (cfg->type != FLASH_DRV_FLASHC)
		return HAL_INVALID;

	dev = getFlashDev(flash);
	if (dev == NULL)
		return HAL_INVALID;

	xipDrv.flash = flash;
	xipDrv.dev = dev;
	xipDrv.setAddr = setAddr;
	xipDrv.setCmd = setCmd;
	xipDrv.setContinue = setContinue;
	xipDrv.setDelay = setDelay;
	HAL_Memset(&xipDrv.mCfg, 0, sizeof(xipDrv.mCfg));

	chip = getFlashChip(dev);

	chip->mXip = &xipDrv;

	chip->enableXIP(chip);
	chip->xipDriverCfg(chip, getFlashMode(dev));
	xipDrv.mCfg.freq = cfg->flashc.clk;
	xipDrv.mCfg.addr = xaddr;

	HAL_Flashc_Xip_Init(&xipDrv.mCfg);

#ifdef CONFIG_PM
	pm_register_ops(&Xip_dev);
#endif

	return ret;
}

/**
  * @brief Deinitializes XIP module.
  * @retval HAL_Status: The status of driver
  */
HAL_Status HAL_Xip_Deinit(void)
{
	HAL_Status ret = HAL_OK;
	FlashChipBase *chip = getFlashChip(xipDrv.dev);

#ifdef CONFIG_PM
	pm_unregister_ops(&Xip_dev);
#endif

	HAL_Flashc_Xip_Deinit();
	chip->disableXIP(chip);

	return ret;
}
/*
static HAL_Status HAL_XIP_Enable()
{

}

static HAL_Status HAL_XIP_Disable()
{

}*/

static int setCmd(struct XipDriverBase *base, InstructionField *cmd, InstructionField *addr, InstructionField *dummy, InstructionField *data)
{
	FC_InstructionField tmp;
	XIP_Instruction *ins = &base->mCfg.ins;

	insToFcIns(cmd, &tmp);
	ins->cmd = *tmp.pdata;
	ins->cmd_line = tmp.line;

	insToFcIns(addr, &tmp);
	ins->addr_line = tmp.line;

	insToFcIns(dummy, &tmp);
	ins->dummy_line = tmp.line;
	ins->dum_btyes = tmp.len;

	insToFcIns(data, &tmp);
	ins->data_line = tmp.line;

	return 0;
}

static int setContinue(struct XipDriverBase *base, uint32_t continueMode, void *arg)
{
	base->mCfg.cont_mode = continueMode;

	return 0;
}

static int setDelay(struct XipDriverBase *base, Flashc_Delay *delay)
{
	/*TODO: how and where should be the delay calculation*/

	return 0;
}

static int setAddr(struct XipDriverBase *base, uint32_t addr)
{
	base->mCfg.addr = addr;

	return 0;
}


#ifdef CONFIG_PM
//#define FLASH_POWERDOWN (PM_MODE_POWEROFF)

static int PM_XipSuspend(struct soc_device *dev, enum suspend_state_t state)
{
	FlashChipBase *chip = getFlashChip(xipDrv.dev);

	switch (state) {
	case PM_MODE_SLEEP:
		break;
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
	case PM_MODE_POWEROFF:
		chip->disableXIP(chip);
		break;
	default:
		break;
	}

	return 0;
}

static int PM_XipResume(struct soc_device *dev, enum suspend_state_t state)
{
	FlashChipBase *chip = getFlashChip(xipDrv.dev);

	switch (state) {
	case PM_MODE_SLEEP:
		break;
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
	case PM_MODE_POWEROFF:
		chip->enableXIP(chip);
		break;
	default:
		break;
	}

	return 0;
}

static struct soc_device_driver Xip_drv = {
	.name = "Xip",
	.suspend = PM_XipSuspend,
	.resume = PM_XipResume,
};

static struct soc_device Xip_dev = {
	.name = "Xip",
	.driver = &Xip_drv,
};

#endif

