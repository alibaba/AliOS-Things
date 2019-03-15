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

#define XT25F16B_JEDEC 0x15400B

typedef enum {
	FLASH_INSTRUCTION_RDSR = 0x05,				/* read status register */
	FLASH_INSTRUCTION_WRSR = 0x01,				/* write status register */
	FLASH_INSTRUCTION_RDSR1 = 0x35,				/* read status register-1 */
} eSF_Instruction;

/*
	Default Flash Chip, Only support basic Interface
*/
typedef struct XT25FXXB_Flash
{
	FlashChipBase base;
} XT25FXXB_Flash;

#define INSTRUCT_ZCREATE(cmd, addr, dummy, data) \
	InstructionField cmd, addr, dummy, data; \
	do { \
		HAL_Memset(&cmd,	0, sizeof(InstructionField)); \
		HAL_Memset(&addr,	0, sizeof(InstructionField)); \
		HAL_Memset(&dummy,	0, sizeof(InstructionField)); \
		HAL_Memset(&data,	0, sizeof(InstructionField)); \
	} while (0)

static int XT25FXXB_WriteStatus(FlashChipBase *base, FlashStatus reg, uint8_t *status)
{
	int ret;
	uint8_t status_buf[2];

	PCHECK(base);
	INSTRUCT_ZCREATE(cmd, addr, dummy, data);

	if (!(reg & base->mWriteStatusSupport)) {
		FLASH_NOTSUPPORT();
		return HAL_INVALID;
	}
/*
	cmd.data = FLASH_INSTRUCTION_SRWREN;
	cmd.line = 1;

	base->driverWrite(base, &cmd, NULL, NULL, NULL);
*/

	memset(&cmd,	0, sizeof(InstructionField));
	memset(&addr,	0, sizeof(InstructionField));
	memset(&dummy,	0, sizeof(InstructionField));
	memset(&data,	0, sizeof(InstructionField));

	if (reg == FLASH_STATUS1)
	{
		cmd.data = FLASH_INSTRUCTION_RDSR1;
		cmd.line = 1;

		data.pdata = (uint8_t *)&status_buf[1];
		data.len = 1;
		data.line = 1;

		base->driverRead(base, &cmd, NULL, NULL, &data);

		status_buf[0] = *status;

		cmd.data = FLASH_INSTRUCTION_WRSR;

		data.pdata = status_buf;
		data.len = 2;
		data.line = 1;

		//printf("FLASH_STATUS1\n");
		//printf("SR1:0x%02x\n", status_buf[0]);
		//printf("SR2:0x%02x\n", status_buf[1]);
	}
	else if (reg == FLASH_STATUS2)
	{
		cmd.data = FLASH_INSTRUCTION_RDSR;
		cmd.line = 1;

		data.pdata = (uint8_t *)status_buf;
		data.len = 1;
		data.line = 1;

		base->driverRead(base, &cmd, NULL, NULL, &data);

		status_buf[1] = *status;

		cmd.data = FLASH_INSTRUCTION_WRSR;

		data.pdata = status_buf;
		data.len = 2;
		data.line = 1;

		//printf("FLASH_STATUS2\n");
		//printf("SR1:0x%02x\n", status_buf[0]);
		//printf("SR2:0x%02x\n", status_buf[1]);
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

static int XT25FXXB_FlashInit(FlashChipBase * base)
{
	PCHECK(base);

	XT25FXXB_Flash *impl = __containerof(base, XT25FXXB_Flash, base);

	impl->base.writeEnable = defaultWriteEnable;
	impl->base.writeDisable = defaultWriteDisable;
	impl->base.readStatus = defaultReadStatus;
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
	impl->base.writeStatus = XT25FXXB_WriteStatus;
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

	FLASH_DEBUG("XT25FXXB_Flash chip inited");

	return 0;
}

static int XT25FXXB_FlashDeinit(FlashChipBase * base)
{
	PCHECK(base);

	XT25FXXB_Flash *impl = __containerof(base, XT25FXXB_Flash, base);
	free(impl);

	return 0;
}

static FlashChipBase *XT25FXXB_FlashCtor(uint32_t arg)
{
	XT25FXXB_Flash *impl = malloc(sizeof(XT25FXXB_Flash));
	uint32_t jedec = arg;
	uint32_t size;
	PCHECK(impl);
	HAL_Memset(impl, 0, sizeof(XT25FXXB_Flash));

	if (jedec == XT25F16B_JEDEC) {
		size = 32 * 16 * 0x1000;
	}
	else {
		return NULL;
	}

	impl->base.mJedec = jedec;
	impl->base.mPageSize = 256;
	impl->base.mSize = size;
	impl->base.mMaxFreq = 120 * 1000 * 1000;
	impl->base.mMaxReadFreq = 80 * 1000 * 1000;
	impl->base.mEraseSizeSupport = FLASH_ERASE_64KB | FLASH_ERASE_32KB | FLASH_ERASE_4KB | FLASH_ERASE_CHIP;
	impl->base.mPageProgramSupport = FLASH_PAGEPROGRAM | FLASH_QUAD_PAGEPROGRAM;
	impl->base.mReadStausSupport = FLASH_STATUS1 | FLASH_STATUS2;
	impl->base.mWriteStatusSupport = FLASH_STATUS1 | FLASH_STATUS2;//
	impl->base.mReadSupport = FLASH_READ_NORMAL_MODE | FLASH_READ_FAST_MODE | FLASH_READ_DUAL_O_MODE
				| FLASH_READ_DUAL_IO_MODE | FLASH_READ_QUAD_O_MODE | FLASH_READ_QUAD_IO_MODE;//E7H,Quad I/O Word Fast Read not supported
	impl->base.mFlashStatus = 0;
	impl->base.mDummyCount = 1;

	return &impl->base;
}

FlashChipCtor  XT25F16B_FlashChip = {
		.mJedecId = XT25F16B_JEDEC,
		.create = XT25FXXB_FlashCtor,
		.init = XT25FXXB_FlashInit,
		.destory = XT25FXXB_FlashDeinit,
};
