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
#include "driver/chip/flashchip/flash_chip.h"
#include "flash_default.h"
#include "../hal_base.h"
#include "sys/xr_debug.h"


#define FLASH_DEBUG(fmt, arg...)	XR_DEBUG((DBG_OFF | XR_LEVEL_ALL), NOEXPAND, "[Flash chip DBG] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define FLASH_ALERT(fmt, arg...)	XR_ALERT((DBG_ON | XR_LEVEL_ALL), NOEXPAND, "[Flash chip ALT] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define FLASH_ERROR(fmt, arg...)	XR_ERROR((DBG_ON | XR_LEVEL_ALL), NOEXPAND, "[Flash chip ERR] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define FLASH_NOWAY()				XR_ERROR((DBG_ON | XR_LEVEL_ALL), NOEXPAND, "[Flash chip should not be here] <%s : %d> \n", __func__, __LINE__)
#define FLASH_NOTSUPPORT() 			FLASH_ALERT("not support CMD")

#define EN25QH64A_JEDEC 0x17701C

typedef enum {
	FLASH_INSTRUCTION_RDSR = 0x05,				/* read status register */
	FLASH_INSTRUCTION_WRSR = 0x01,				/* write status register */
	FLASH_INSTRUCTION_RDSR2 = 0x09,				/* read status register-2 */
	FLASH_INSTRUCTION_RDSR3 = 0x95,				/* read status register-3 */
	FLASH_INSTRUCTION_WRSR3 = 0xC0,				/* write status register-3 */
	FLASH_INSTRUCTION_SRWREN = 0x50,
} eSF_Instruction;

/*
	Default Flash Chip, Only support basic Interface
*/
typedef struct EN25QHXXA_Flash
{
	FlashChipBase base;
} EN25QHXXA_Flash;

#define INSTRUCT_ZCREATE(cmd, addr, dummy, data) \
	InstructionField cmd, addr, dummy, data; \
	do { \
		HAL_Memset(&cmd,	0, sizeof(InstructionField)); \
		HAL_Memset(&addr,	0, sizeof(InstructionField)); \
		HAL_Memset(&dummy,	0, sizeof(InstructionField)); \
		HAL_Memset(&data,	0, sizeof(InstructionField)); \
	} while (0)

int EN25QHXXA_ReadStatus(FlashChipBase *base, FlashStatus reg, uint8_t *status)
{
	PCHECK(base);
	INSTRUCT_ZCREATE(cmd, addr, dummy, data);

	if (!(reg & base->mReadStausSupport)) {
		FLASH_NOTSUPPORT();
		return -1;
	}

	if (reg == FLASH_STATUS1)
	{
		cmd.data = FLASH_INSTRUCTION_RDSR;
	}
	else if (reg == FLASH_STATUS2)
	{
		cmd.data = FLASH_INSTRUCTION_RDSR2;
	}
	else if (reg == FLASH_STATUS3)
	{
		cmd.data = FLASH_INSTRUCTION_RDSR3;
	}
	else
		FLASH_NOWAY();

	data.pdata = (uint8_t *)status;
	data.len = 1;
	data.line = 1;

	return base->driverRead(base, &cmd, NULL, NULL, &data);
}

static int EN25QHXXA_WriteStatus(FlashChipBase *base, FlashStatus reg, uint8_t *status)
{
	int ret;

	PCHECK(base);
	INSTRUCT_ZCREATE(cmd, addr, dummy, data);

	if (!(reg & base->mWriteStatusSupport)) {
		FLASH_NOTSUPPORT();
		return HAL_INVALID;
	}

	cmd.data = FLASH_INSTRUCTION_SRWREN;
	cmd.line = 1;

	base->driverWrite(base, &cmd, NULL, NULL, NULL);

	memset(&cmd,	0, sizeof(InstructionField));
	memset(&addr,	0, sizeof(InstructionField));
	memset(&dummy,	0, sizeof(InstructionField));
	memset(&data,	0, sizeof(InstructionField));

	if (reg == FLASH_STATUS1)
	{
		cmd.data = FLASH_INSTRUCTION_WRSR;

		data.pdata = (uint8_t *)status;
		data.len = 1;
		data.line = 1;
	}
	else if (reg == FLASH_STATUS3)
	{
		cmd.data = FLASH_INSTRUCTION_WRSR3;

		data.pdata = (uint8_t *)status;
		data.len = 1;
		data.line = 1;
	}
	else
		FLASH_NOWAY();

	base->writeEnable(base);

	ret = base->driverWrite(base, &cmd, NULL, NULL, &data);

	base->writeDisable(base);
/*
	while (base->isBusy(base)) {
		//printf("busy...\n");
	}
*/
	return ret;
}

static int EN25QHXXA_FlashInit(FlashChipBase * base)
{
	PCHECK(base);

	EN25QHXXA_Flash *impl = __containerof(base, EN25QHXXA_Flash, base);

	impl->base.writeEnable = defaultWriteEnable;
	impl->base.writeDisable = defaultWriteDisable;
	impl->base.readStatus = EN25QHXXA_ReadStatus;
	impl->base.erase = defaultErase;
	impl->base.jedecID = defaultGetJedecID;
	impl->base.pageProgram = defaultPageProgram;
	impl->base.read = defaultRead;

	impl->base.driverWrite = defaultDriverWrite;
	impl->base.driverRead = defaultDriverRead;
	impl->base.xipDriverCfg = defaultXipDriverCfg;
	impl->base.setFreq = defaultSetFreq;
	impl->base.switchReadMode = defaultSwitchReadMode;
	impl->base.enableXIP = defaultEnableXIP;
	impl->base.disableXIP = defaultDisableXIP;
	impl->base.isBusy = defaultIsBusy;
	impl->base.control = defaultControl;
	impl->base.minEraseSize = defaultGetMinEraseSize;
	//impl->base.writeStatus = defaultWriteStatus;
	impl->base.writeStatus = EN25QHXXA_WriteStatus;
	impl->base.enableQPIMode = defaultEnableQPIMode;
	impl->base.disableQPIMode = defaultDisableQPIMode;
//	impl->base.enableReset = defaultEnableReset;
	impl->base.reset = defaultReset;

	impl->base.suspendErasePageprogram = NULL;
	impl->base.resumeErasePageprogram = NULL;
	impl->base.powerDown = NULL;
	impl->base.releasePowerDown = NULL;
	impl->base.uniqueID = NULL;
	/*TODO: a NULL interface for showing invalid interface*/

	FLASH_DEBUG("EN25QHXXA_Flash chip inited");

	return 0;
}

static int EN25QHXXA_FlashDeinit(FlashChipBase * base)
{
	PCHECK(base);

	EN25QHXXA_Flash *impl = __containerof(base, EN25QHXXA_Flash, base);
	free(impl);

	return 0;
}

static FlashChipBase *EN25QHXXA_FlashCtor(uint32_t arg)
{
	EN25QHXXA_Flash *impl = malloc(sizeof(EN25QHXXA_Flash));
	uint32_t jedec = arg;
	uint32_t size;
	PCHECK(impl);
	HAL_Memset(impl, 0, sizeof(EN25QHXXA_Flash));

	if (jedec == EN25QH64A_JEDEC) {
		size = 128 * 16 * 0x1000;
	}
	else {
		return NULL;
	}

	impl->base.mJedec = jedec;
	impl->base.mPageSize = 256;
	impl->base.mSize = size;
	impl->base.mMaxFreq = 104 * 1000 * 1000;
	impl->base.mMaxReadFreq = 83 * 1000 * 1000;
	impl->base.mEraseSizeSupport = FLASH_ERASE_64KB | FLASH_ERASE_32KB | FLASH_ERASE_4KB | FLASH_ERASE_CHIP;
	impl->base.mPageProgramSupport = FLASH_PAGEPROGRAM | FLASH_QUAD_PAGEPROGRAM;
	impl->base.mReadStausSupport = FLASH_STATUS1 | FLASH_STATUS2 | FLASH_STATUS3;
	impl->base.mWriteStatusSupport = FLASH_STATUS1 | FLASH_STATUS3;//
	impl->base.mReadSupport = FLASH_READ_NORMAL_MODE | FLASH_READ_FAST_MODE | FLASH_READ_DUAL_O_MODE
				| FLASH_READ_DUAL_IO_MODE | FLASH_READ_QUAD_O_MODE | FLASH_READ_QUAD_IO_MODE;
	impl->base.mFlashStatus = 0;
	impl->base.mDummyCount = 1;

	return &impl->base;
}

FlashChipCtor  EN25QH64A_FlashChip = {
		.mJedecId = EN25QH64A_JEDEC,
		.create = EN25QHXXA_FlashCtor,
		.init = EN25QHXXA_FlashInit,
		.destory = EN25QHXXA_FlashDeinit,
};
