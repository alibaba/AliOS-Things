/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"

BOOT_RAM_DATA_SECTION
const TZ_CFG_TypeDef tz_config[]=
{
//  Start					End						NSC
	{0x40000000,			0x50000000-1,			0},	/* entry0: Peripherals NS */
	{0x1010A000,			0x101D4000-1,			0},	/* entry1: IROM & DROM NS */
	{0x00080000,			(u32)__ram_image3_start__ -1,0},	/* entry2: KM0 SRAM, Retention SRAM, PSRAM & FLASH & SRAM NS */
	{(u32)__ram_image3_end__,	0x1007C000-1,			1},	/* entry3: NSC 4K */
	{0x100E0000,			0x10100000-1,			0},	/* entry4: BT/WIFI Extention SRAM */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry5: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry6: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},	/* entry7: TODO */
	{0xFFFFFFFF,			0xFFFFFFFF,				0},
};
