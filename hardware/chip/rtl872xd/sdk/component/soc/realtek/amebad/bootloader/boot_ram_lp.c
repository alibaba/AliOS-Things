/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"

//#define SDM32K_NEW Please close it in rtl8721dlp_clk.c

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

/* open some always on functions in this function */
BOOT_RAM_TEXT_SECTION
void BOOT_RAM_FuncEnable(void)
{
	u32 Temp = 0;

	if(SYSCFG_CUTVersion() == 0) {
		/* A-Cut: enable HWPD */
		Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_PM_OPT);
		Temp |= BIT_AON_PMC_DIRECT_PDN;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_PM_OPT, Temp);
	}

	RCC_PeriphClockCmd(APBPeriph_GDMA0, APBPeriph_GDMA0_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_QDEC0, APBPeriph_QDEC0_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_SGPIO, APBPeriph_SGPIO_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_I2C0, APBPeriph_I2C0_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_UART1, APBPeriph_UART1_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_LOGUART, APBPeriph_LOGUART_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_GTIMER, APBPeriph_GTIMER_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_IPC, APBPeriph_IPC_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_VENDOR_REG, APBPeriph_VENDOR_REG_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_KEYSCAN, APBPeriph_KEYSCAN_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_RTC, APBPeriph_RTC_CLOCK, ENABLE);

	/* CLK SEL */
	RCC_PeriphClockSource_QDEC(0, 1);
	
	/* AON */
	RCC_PeriphClockCmd(APBPeriph_CTOUCH, APBPeriph_CTOUCH_CLOCK, ENABLE);

}

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
static VOID BOOT_RAM_WakeFromPG(VOID)
{
	PRAM_START_FUNCTION Image2EntryFun = BOOT_RAM_SectionInit();
	u32 Temp = 0;

	/* to initial ROM code using global variable */
	if ((HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG3) & BIT_SYS_DIS_BOOT_LOG_EN) == 0) {
		if (ROM_SIM_ENABLE == 0x12345678) {
			LOG_MASK(LEVEL_ERROR, 0xFFFFFFFF);
		} else {
			Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG3);
			Temp |= BIT_SYS_DIS_BOOT_LOG_EN;
			HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG3, Temp);
		}
	}

	/* Need Modify */
	irq_table_init(MSP_RAM_LP);

	/* Open IPC First for LOGUART IPC_SEM */
	RCC_PeriphClockCmd(APBPeriph_IPC, APBPeriph_IPC_CLOCK, ENABLE);

	//DBG_8195A("BOOT_RAM_WakeFromPG \n");

	/*enable HS platform power for high 8k txpktbuffer.*/
	Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_PWR_ISO_CTRL);
	Temp |= BIT_MASK_LSYS_HPLAT_PWC_EN_BIT2;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_PWR_ISO_CTRL, Temp);
	/*enable flash clock and init spic*/
	RCC_PeriphClockCmd(APBPeriph_FLASH, APBPeriph_FLASH_CLOCK_XTAL, ENABLE);
	
	FLASH_Init(flash_init_para.FLASH_cur_bitmode);
	
	/* we should Cache_Flush when we wake */
	Cache_Enable(ENABLE);

	/* soc deepstadby or deepsleep need it, or wakeup will fail */
	if ( HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_SYSCFG3) & BIT_SYS_FLASH_DEEP_SLEEP_EN ) {
		FLASH_DeepPowerDown(DISABLE);
	}

	/*release HS platform power Isolation.*/
	Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_PWR_ISO_CTRL);
	Temp |= BIT_MASK_LSYS_HPLAT_PWC_EN_BIT3;//power on hs platform for txpktbuffer high 8k
	Temp &= ~BIT_LSYS_ISO_HSOC;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_PWR_ISO_CTRL, Temp);

	Image2EntryFun->RamWakeupFun();
}
//3 Image 1
BOOT_RAM_TEXT_SECTION
static VOID BOOT_RAM_Image1(VOID)
{
	PRAM_START_FUNCTION Image2EntryFun = BOOT_RAM_SectionInit();
	u32 BssLen = ((u32)__image1_bss_end__ - (u32)__image1_bss_start__);
	u8* image2_validate_code = (u8*)((u32)__image2_entry_func__ + sizeof(RAM_START_FUNCTION));

	_memset((void *) __image1_bss_start__, 0, BssLen);

	DBG_8195A("IMG1 ENTER RAM\n");
	
	BOOT_RAM_FuncEnable();

	DBG_8195A("IMG2 SIGN[%s], START[0x%08x] \n",image2_validate_code,
		(u32)(Image2EntryFun->RamStartFun));

	if (_strcmp((const char *)image2_validate_code, (const char *)"RTKWin")) {
		while (1) {
			DBG_8195A("Invalid Image2 Signature\n");
			shell_rom(1000);//each delay is 100us
		}
	}

	//Jump to image 2
	Image2EntryFun->RamStartFun();
}

IMAGE1_VALID_PATTEN_SECTION
const u8 RAM_IMG1_VALID_PATTEN[] = {
	0x23, 0x79, 0x16, 0x88, 0xff, 0xff, 0xff, 0xff
};

IMAGE1_ENTRY_SECTION
RAM_FUNCTION_START_TABLE RamStartTable = {
	.RamStartFun = BOOT_RAM_Image1,
	.RamWakeupFun = BOOT_RAM_WakeFromPG,
	.RamPatchFun0 = BOOT_RAM_Image1,
	.RamPatchFun1 = BOOT_RAM_Image1,
	.RamPatchFun2 = BOOT_RAM_Image1,
	.FlashStartFun = BOOT_FLASH_Image1,
	.Img1ValidCode = (u32)RAM_IMG1_VALID_PATTEN,
};
