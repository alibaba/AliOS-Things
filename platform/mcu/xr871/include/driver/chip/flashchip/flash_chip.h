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

#ifndef FLASH_CHIP_H_
#define FLASH_CHIP_H_

#include <stdlib.h>
#include <stdio.h>
#include "types.h"

typedef struct FlashChipBase FlashChipBase;
typedef struct FlashDrvierBase FlashDrvierBase;
typedef struct XipDriverBase XipDriverBase;

typedef struct InstructionField
{
	uint32_t data;
	uint8_t *pdata;
	uint32_t len;
	uint32_t line;
} InstructionField;

typedef int (*FlashDrvIface)(FlashChipBase *base, InstructionField *cmd, InstructionField *addr, InstructionField *dummy, InstructionField *data);

typedef enum FlashReadMode
{
	FLASH_READ_NORMAL_MODE	= 1 << 0,	/*!< cmd: 1 line, addr + dummy: 1 line, data: 1 line, freq can't too high */
	FLASH_READ_FAST_MODE	= 1 << 1,   /*!< cmd: 1 line, addr + dummy: 1 line, data: 1 line */
	FLASH_READ_DUAL_O_MODE	= 1 << 2,   /*!< cmd: 1 line, addr + dummy: 1 line, data: 2 line */
	FLASH_READ_DUAL_IO_MODE	= 1 << 3,   /*!< cmd: 1 line, addr + dummy: 2 line, data: 2 line */
	FLASH_READ_QUAD_O_MODE	= 1 << 4,   /*!< cmd: 1 line, addr + dummy: 1 line, data: 4 line */
	FLASH_READ_QUAD_IO_MODE	= 1 << 5,   /*!< cmd: 1 line, addr + dummy: 4 line, data: 4 line */
	FLASH_READ_QPI_MODE		= 1 << 6,   /*!< cmd: 4 line, addr + dummy: 4 line, data: 4 line */
} FlashReadMode;

typedef enum FlashEraseMode
{
	FLASH_ERASE_NOSUPPORT 	= 0,
	FLASH_ERASE_4KB			= 1 << 12,	/*!< 4KB  erase mode, command: 0x20 , esize: 4096 */
	FLASH_ERASE_32KB 		= 1 << 15,	/*!< 32KB erase mode, command: 0x52 , esize: 32768 */
	FLASH_ERASE_64KB 		= 1 << 16,	/*!< 64KB erase mode, command: 0xD8 , esize: 65536 */
	FLASH_ERASE_CHIP		= 1 << 1,	/*!< chip erase mode, command: 0xC7 *///0xC7,
} FlashEraseMode;

typedef enum FlashStatus
{
	FLASH_STATUS1 = 1 << 0, /*!< flash status 1 */
	FLASH_STATUS2 = 1 << 1, /*!< flash status 2 */
	FLASH_STATUS3 = 1 << 2,	/*!< flash status 3 */
} FlashStatus;

typedef enum FlashPageProgramMode
{
	FLASH_PAGEPROGRAM		= 1 << 0,   /*!< page program */
	FLASH_QUAD_PAGEPROGRAM	= 1 << 1,	/*!< quad page program */
} FlashPageProgramMode;

typedef enum FlashLockMode
{
	FlashLockMode_NOTHING	/*!< nothing to support for now */
	/*TODO: tbc...*/
} FlashLockMode;

struct FlashChipBase
{
	/*
		attribute
	*/
	uint32_t mJedec;
	uint32_t mMaxFreq;
	uint32_t mMaxReadFreq;		//no need?
	uint32_t mSize;
	uint32_t mCsEn;				//no need?
	uint32_t mPageSize;			//256bytes

	uint32_t mEraseSizeSupport;
	uint16_t mReadStausSupport;
	uint8_t  mWriteStatusSupport;
	uint8_t  mPageProgramSupport;
	uint16_t mReadSupport;
	uint16_t mLockSupport;

	uint16_t mFlashStatus;
	uint8_t  mDummyCount;

	struct FlashDrvierBase *mDriver;
	struct XipDriverBase *mXip;

	/*
		private interface
	*/
	void (*writeEnable)(FlashChipBase *base);
	void (*writeDisable)(FlashChipBase *base);
	int (*readStatus)(FlashChipBase *base, FlashStatus status, uint8_t *data);
	int (*writeStatus)(FlashChipBase *base, FlashStatus status, uint8_t *data);
	int (*erase)(FlashChipBase *base, FlashEraseMode mode, uint32_t addr);	//block count is realized in Flash erase.
	int (*suspendErasePageprogram)(FlashChipBase *base);
	int (*resumeErasePageprogram)(FlashChipBase *base);
	int (*powerDown)(FlashChipBase *base);
	int (*releasePowerDown)(FlashChipBase *base);
	int (*jedecID)(FlashChipBase *base, uint32_t *data);
	int (*enableQPIMode)(FlashChipBase *base);
	int (*disableQPIMode)(FlashChipBase *base);
//	int (*enableReset)(FlashChipBase *base);
	int (*reset)(FlashChipBase *base);
	int (*uniqueID)(FlashChipBase *base, uint8_t uid[8]);
	int (*pageProgram)(FlashChipBase *base, FlashPageProgramMode mode, uint32_t addr, uint8_t *data, uint32_t size);
	int (*read)(FlashChipBase *base, FlashReadMode mode, uint32_t addr, uint8_t *data, uint32_t size);
	// tbc...

	int (*driverWrite)(FlashChipBase *base, InstructionField *cmd, InstructionField *addr, InstructionField *dummy, InstructionField *data);
	int (*driverRead)(FlashChipBase *base, InstructionField *cmd, InstructionField *addr, InstructionField *dummy, InstructionField *data);
	int (*xipDriverCfg)(FlashChipBase *base, FlashReadMode mode);

	/*
		public interface
	*/
	int (*setFreq)(FlashChipBase *base, uint32_t freq);
	int (*switchReadMode)(FlashChipBase *base, FlashReadMode mode);
	int (*enableXIP)(FlashChipBase *base);
	int (*disableXIP)(FlashChipBase *base);
	int (*isBusy)(FlashChipBase *base);
	int (*control)(FlashChipBase *base, int op, void *param);
	FlashEraseMode (*minEraseSize)(FlashChipBase *base);
};

typedef enum DefaultFlashControlCmds
{
	DEFAULT_FLASH_SET_QPI_READ_P5P4,
} DefaultFlashControlCmds;

/*
	Default Flash Chip Interface
*/
void defaultWriteEnable(FlashChipBase *base);

void defaultWriteDisable(FlashChipBase *base);

int defaultReadStatus(FlashChipBase *base, FlashStatus status, uint8_t *data);

int defaultWriteStatus(FlashChipBase *base, FlashStatus reg, uint8_t *status);

int defaultErase(FlashChipBase *base, FlashEraseMode mode, uint32_t addr);

int defaultSuspendErasePageprogram(FlashChipBase *base);

int defaultResumeErasePageprogram(FlashChipBase *base);

int defaultPowerDown(FlashChipBase *base);

int defaultReleasePowerDown(FlashChipBase *base);

int defaultGetJedecID(FlashChipBase *base, uint32_t *data);

int defaultEnableQPIMode(FlashChipBase *base);

int defaultDisableQPIMode(FlashChipBase *base);

int defaultEnableReset(FlashChipBase *base);

int defaultReset(FlashChipBase *base);

int defaultGetUniqueID(FlashChipBase *, uint8_t uid[8]);

int defaultPageProgram(FlashChipBase *base, FlashPageProgramMode mode, uint32_t addr, uint8_t *data, uint32_t size);

int defaultRead(FlashChipBase *base, FlashReadMode mode, uint32_t addr, uint8_t *data, uint32_t size);

int defaultDriverWrite(FlashChipBase *base, InstructionField *cmd, InstructionField *addr, InstructionField *dummy, InstructionField *data);

int defaultDriverRead(FlashChipBase *base, InstructionField *cmd, InstructionField *addr, InstructionField *dummy, InstructionField *data);

int defaultSetFreq(FlashChipBase *base, uint32_t freq);

int defaultSwitchReadMode(FlashChipBase *base, FlashReadMode mode);

int defaultEnableXIP(FlashChipBase *base);

int defaultDisableXIP(FlashChipBase *base);

int defaultIsBusy(FlashChipBase *base);

int defaultControl(FlashChipBase *base, int op, void *param);

FlashEraseMode defaultGetMinEraseSize(FlashChipBase *base);

int defaultXipDriverCfg(FlashChipBase *base, FlashReadMode mode);


typedef struct FlashChipCtor
{
	uint32_t mJedecId;

	FlashChipBase *(*create)(uint32_t arg);
	int (*init)(FlashChipBase *);
	int (*destory)(FlashChipBase *);
} FlashChipCtor;

FlashChipBase *FlashChipCreate(FlashDrvierBase *driver);
int FlashChipDestroy(FlashChipBase *base);


/*
	Flash Chip Debug
*/
#define PCHECK(p)

#endif /* FLASH_CHIP_H_ */
