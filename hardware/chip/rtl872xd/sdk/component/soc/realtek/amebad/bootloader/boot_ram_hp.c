/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"

#if defined ( __ICCARM__ )
#pragma section=".ram.bss"
#pragma section=".rom.bss"
#pragma section=".ram.start.table"
#pragma section=".ram_image1.bss"
#pragma section=".ram_image2.entry"

BOOT_RAM_RODATA_SECTION u8* __image2_entry_func__;
BOOT_RAM_RODATA_SECTION u8* __image1_bss_start__;
BOOT_RAM_RODATA_SECTION u8* __image1_bss_end__;
#endif

BOOT_RAM_TEXT_SECTION
PRAM_START_FUNCTION BOOT_RAM_SectionInit(void)
{
#if defined ( __ICCARM__ )
	// only need __bss_start__, __bss_end__
	__image2_entry_func__		= (u8*)__section_begin(".ram_image2.entry");
	__image1_bss_start__		= (u8*)__section_begin(".ram_image1.bss");
	__image1_bss_end__			= (u8*)__section_end(".ram_image1.bss");
#endif
	return (PRAM_START_FUNCTION)__image2_entry_func__;
}

BOOT_RAM_TEXT_SECTION
static VOID BOOT_RAM_Image1(VOID)
{
	PRAM_START_FUNCTION Image2EntryFun = BOOT_RAM_SectionInit();
	u32 BssLen = (__image1_bss_end__ - __image1_bss_start__);
	u8* image2_validate_code = (u8 *)((u32)__image2_entry_func__ + sizeof(RAM_START_FUNCTION));
	
	_memset((void *) __image1_bss_start__, 0, BssLen);

	DBG_8195A("IMG1 ENTER RAM\n");

	DBG_8195A("IMG2 SIGN[%s], START[0x%08x] \n",image2_validate_code,
		(u32)(Image2EntryFun->RamStartFun));
	
	if (_strcmp((const char *)image2_validate_code, (const char *)"RTKWin")) {
		while (1) {
			DBG_8195A("Invalid Image2 Signature\n");
			DelayMs(1000);//each delay is 100us
		}
	}

	Image2EntryFun->RamStartFun();
}

IMAGE1_VALID_PATTEN_SECTION
const u8 RAM_IMG1_VALID_PATTEN[] = {
	0x23, 0x79, 0x16, 0x88, 0xff, 0xff, 0xff, 0xff
};

IMAGE1_ENTRY_SECTION
RAM_FUNCTION_START_TABLE RamStartTable = {
	.RamStartFun = BOOT_RAM_Image1,
	.RamWakeupFun = NULL,
	.RamPatchFun0 = BOOT_FLASH_WakeFromPG,
	.RamPatchFun1 = BOOT_RAM_Image1,
	.RamPatchFun2 = BOOT_RAM_Image1,
	.FlashStartFun = BOOT_FLASH_Image1
};
