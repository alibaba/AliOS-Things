/*
 * flash_default.c
 *
 *  Created on: 2017Äê8ÔÂ21ÈÕ
 *      Author: lijunjie
 */

#include <stdlib.h>
#include <stdio.h>
#include "driver/chip/flashchip/flash_chip.h"
#include "flash_default.h"
#include "../hal_base.h"
#include "sys/xr_debug.h"


#define FLASH_DEBUG(fmt, arg...)	XR_DEBUG((DBG_OFF | XR_LEVEL_ALL), NOEXPAND, "[Flash chip debug] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define FLASH_ALERT(fmt, arg...)	XR_ALERT((DBG_ON | XR_LEVEL_ALL), NOEXPAND, "[Flash chip alert] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define FLASH_ERROR(fmt, arg...)	XR_ERROR((DBG_ON | XR_LEVEL_ALL), NOEXPAND, "[Flash chip error] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define FLASH_NOWAY()				XR_ERROR((DBG_ON | XR_LEVEL_ALL), NOEXPAND, "[Flash chip should not be here] <%s : %d> \n", __func__, __LINE__)
#define FLASH_NOTSUPPORT() 			FLASH_ALERT("not support command")


typedef struct SimpleFlashChipCfg
{
	uint32_t mJedec;
	uint32_t mSize;

	uint32_t mEraseSizeSupport;
	uint16_t mReadStausSupport;
	uint8_t mWriteStatusSupport;
	uint8_t mPageProgramSupport;
	uint16_t mReadSupport;

	uint32_t mMaxFreq;
	uint32_t mMaxReadFreq;
} SimpleFlashChipCfg;

static const SimpleFlashChipCfg simpleFlashChip[] =
{
/*
* #ifdef FLASH_xxxxx
* 	{
* 		.mJedec = 0x ID7-ID0 ID15-ID8 M7-M0 from jedec id,
* 		.mSize = total flash memory size,
* 		.mEraseSizeSupport = the flash erase commands is supported,
* 		.mPageProgramSupport = the flash pageprogram commands is supported,
* 		.mReadStausSupport = the flash status registers can be read,
* 		.mWriteStatusSupport = the flash status registers can be written,
* 		.mReadSupport = the flash read commands (modes) is supported,
* 		.mMaxFreq = max operation frequency to flash,
* 		.mMaxReadFreq = max read command frequency(only read command: 0x03h),
* 	},
* #endif
*/
	{
		/* default config must be at first */
		.mJedec = 0,	/* ID7-ID0 ID15-ID8 M7-M0 */
		.mSize = -1,
		.mEraseSizeSupport = FLASH_ERASE_64KB | FLASH_ERASE_32KB | FLASH_ERASE_4KB | FLASH_ERASE_CHIP,
		.mPageProgramSupport = FLASH_PAGEPROGRAM,
		.mReadStausSupport = FLASH_STATUS1 | FLASH_STATUS2 | FLASH_STATUS3,
		.mWriteStatusSupport = FLASH_STATUS1 | FLASH_STATUS2 | FLASH_STATUS3,
		.mReadSupport = FLASH_READ_NORMAL_MODE | FLASH_READ_FAST_MODE | FLASH_READ_DUAL_O_MODE
						| FLASH_READ_DUAL_IO_MODE | FLASH_READ_QUAD_O_MODE | FLASH_READ_QUAD_IO_MODE | FLASH_READ_QPI_MODE,
		.mMaxFreq = -1,
		.mMaxReadFreq = -1,
	},
#ifdef FLASH_PN25F16B
	{
		/* FLASH_PN25F16B */
		.mJedec = 0x15405E,
		.mSize = 32 * 16 * 0x1000,
		.mEraseSizeSupport = FLASH_ERASE_64KB | FLASH_ERASE_32KB | FLASH_ERASE_4KB | FLASH_ERASE_CHIP,
		.mPageProgramSupport = FLASH_PAGEPROGRAM,
		.mReadStausSupport = FLASH_STATUS1,
		.mWriteStatusSupport = FLASH_STATUS1,
		.mReadSupport = FLASH_READ_NORMAL_MODE | FLASH_READ_FAST_MODE | FLASH_READ_DUAL_O_MODE,
		.mMaxFreq = 100 * 1000 * 1000,
		.mMaxReadFreq = 55 * 1000 * 1000,
	},
#endif
#ifdef FLASH_M25P64
	{
		/* FLASH_M25P64 */
		.mJedec = 0x172020,
		.mSize = 128 * 0x10000,
		.mEraseSizeSupport = FLASH_ERASE_64KB | FLASH_ERASE_CHIP,
		.mPageProgramSupport = FLASH_PAGEPROGRAM,
		.mReadStausSupport = FLASH_STATUS1,
		.mWriteStatusSupport = 0,
		.mReadSupport = FLASH_READ_NORMAL_MODE | FLASH_READ_FAST_MODE,
		.mMaxFreq = 50 * 1000 * 1000,
		.mMaxReadFreq = 20 * 1000 * 1000,
	},
#endif
#ifdef FLASH_W25Q16FW
	{
		/* FLASH_W25Q16FW */
		.mJedec = 0x1560EF,
		.mSize = 32 *16 * 0x1000,
		.mEraseSizeSupport = FLASH_ERASE_64KB | FLASH_ERASE_32KB | FLASH_ERASE_4KB | FLASH_ERASE_CHIP,
		.mPageProgramSupport = FLASH_PAGEPROGRAM,
		.mReadStausSupport = FLASH_STATUS1 | FLASH_STATUS2 | FLASH_STATUS3,
		.mWriteStatusSupport = FLASH_STATUS1 | FLASH_STATUS2 | FLASH_STATUS3,
		.mReadSupport = FLASH_READ_NORMAL_MODE | FLASH_READ_FAST_MODE | FLASH_READ_DUAL_O_MODE
						| FLASH_READ_DUAL_IO_MODE | FLASH_READ_QUAD_O_MODE | FLASH_READ_QUAD_IO_MODE,
		.mMaxFreq = 80 * 1000 * 1000,
		.mMaxReadFreq = 50 * 1000 * 1000,
	},
#endif
#ifdef FLASH_PN25F08
	{
		/* FLASH_PN25F08 */
		.mJedec = 0x1440E0,
		.mSize = 16 * 16 * 0x1000,
		.mEraseSizeSupport = FLASH_ERASE_64KB | FLASH_ERASE_32KB | FLASH_ERASE_4KB | FLASH_ERASE_CHIP,
		.mPageProgramSupport = FLASH_PAGEPROGRAM,
		.mReadStausSupport = FLASH_STATUS1 | FLASH_STATUS2,
		.mWriteStatusSupport = FLASH_STATUS1 /* write status2 need to rewrite writeStatus */,
		.mReadSupport = FLASH_READ_NORMAL_MODE | FLASH_READ_FAST_MODE | FLASH_READ_DUAL_O_MODE
						| FLASH_READ_DUAL_IO_MODE | FLASH_READ_QUAD_O_MODE | FLASH_READ_QUAD_IO_MODE,
		.mMaxFreq = 108 * 1000 * 1000,
		.mMaxReadFreq = 55 * 1000 * 1000,
	},
#endif
#ifdef FLASH_PN25F16
	{
		/* FLASH_PN25F16 */
		.mJedec = 0x1540E0,
		.mSize = 32 * 16 * 0x1000,
		.mEraseSizeSupport = FLASH_ERASE_64KB | FLASH_ERASE_32KB | FLASH_ERASE_4KB | FLASH_ERASE_CHIP,
		.mPageProgramSupport = FLASH_PAGEPROGRAM,
		.mReadStausSupport = FLASH_STATUS1 | FLASH_STATUS2,
		.mWriteStatusSupport = FLASH_STATUS1 /* write status2 need to rewrite writeStatus */,
		.mReadSupport = FLASH_READ_NORMAL_MODE | FLASH_READ_FAST_MODE | FLASH_READ_DUAL_O_MODE
						| FLASH_READ_DUAL_IO_MODE | FLASH_READ_QUAD_O_MODE | FLASH_READ_QUAD_IO_MODE,
		.mMaxFreq = 108 * 1000 * 1000,
		.mMaxReadFreq = 55 * 1000 * 1000,
	},
#endif
#ifdef FLASH_MX25L1636E
	{
		/* FLASH_MX25L1636E */
		.mJedec = 0x1525C2,
		.mSize = 32 * 16 * 0x1000,
		.mEraseSizeSupport = FLASH_ERASE_64KB | FLASH_ERASE_4KB | FLASH_ERASE_CHIP,
		.mPageProgramSupport = FLASH_PAGEPROGRAM /* QPP need to rewrite pageProgram */,
		.mReadStausSupport = FLASH_STATUS1,
		.mWriteStatusSupport = FLASH_STATUS1,
		.mReadSupport = FLASH_READ_NORMAL_MODE | FLASH_READ_FAST_MODE | FLASH_READ_DUAL_O_MODE
						| FLASH_READ_DUAL_IO_MODE | FLASH_READ_QUAD_IO_MODE,
		.mMaxFreq = 108 * 1000 * 1000,
		.mMaxReadFreq = 50 * 1000 * 1000,
	},
#endif
#ifdef FLASH_MX25L1633E
	{
		/* FLASH_MX25L1633E */
		.mJedec = 0x1524C2,
		.mSize = 32 * 16 * 0x1000,
		.mEraseSizeSupport = FLASH_ERASE_64KB | FLASH_ERASE_4KB | FLASH_ERASE_CHIP,
		.mPageProgramSupport = FLASH_PAGEPROGRAM /* QPP need to rewrite pageProgram */,
		.mReadStausSupport = FLASH_STATUS1,
		.mWriteStatusSupport = FLASH_STATUS1,
		.mReadSupport = FLASH_READ_NORMAL_MODE | FLASH_READ_FAST_MODE
						| FLASH_READ_DUAL_IO_MODE | FLASH_READ_QUAD_IO_MODE,
		.mMaxFreq = 85 * 1000 * 1000,
		.mMaxReadFreq = 33 * 1000 * 1000,
	},
#endif
};


/*
	Default Flash Chip, Only support basic Interface
*/
typedef struct DefaultFlash
{
	FlashChipBase base;
} DefaultFlash;

static int DefaultFlashInit(FlashChipBase * base)
{
	PCHECK(base);

	DefaultFlash *impl = __containerof(base, DefaultFlash, base);

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
	impl->base.writeStatus = defaultWriteStatus;
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

	FLASH_DEBUG("default chip inited");

	return 0;
}

static int DefaultFlashDeinit(FlashChipBase * base)
{
	PCHECK(base);

	DefaultFlash *impl = __containerof(base, DefaultFlash, base);
	free(impl);

	return 0;
}

static FlashChipBase *DefaultFlashCtor(uint32_t arg)
{
	DefaultFlash *impl = malloc(sizeof(DefaultFlash));
//	SimpleFlashChipCfg *cfg = NULL;
	uint32_t simpleChipVecSize = sizeof(simpleFlashChip) / sizeof(simpleFlashChip[0]);
	uint32_t i = simpleChipVecSize;

	PCHECK(impl);
	HAL_Memset(impl, 0, sizeof(DefaultFlash));

	while (--i > 0)
	{
		if (simpleFlashChip[i].mJedec == arg)
			break;
	}
	const SimpleFlashChipCfg *cfg = &simpleFlashChip[i];
	if (i == 0)
		FLASH_ALERT("!!this chip is not in my support chip list, using default flash chip now!!");
	FLASH_DEBUG("create simple flash chip%d: 0x%x", i, cfg->mJedec);

	impl->base.mJedec = cfg->mJedec;
	impl->base.mPageSize = 256;
	impl->base.mSize = cfg->mSize;
	impl->base.mMaxFreq = cfg->mMaxFreq;
	impl->base.mMaxReadFreq = cfg->mMaxReadFreq;
	impl->base.mEraseSizeSupport = cfg->mEraseSizeSupport;
	impl->base.mPageProgramSupport = cfg->mPageProgramSupport;
	impl->base.mReadStausSupport = cfg->mReadStausSupport;
	impl->base.mWriteStatusSupport = cfg->mWriteStatusSupport;
	impl->base.mReadSupport = cfg->mReadSupport;
	impl->base.mFlashStatus = 0;
	impl->base.mDummyCount = 1;

	return &impl->base;
}

FlashChipCtor DefaultFlashChip = {
		.mJedecId = 0,
		.create = DefaultFlashCtor,
		.init = DefaultFlashInit,
		.destory = DefaultFlashDeinit,
};
