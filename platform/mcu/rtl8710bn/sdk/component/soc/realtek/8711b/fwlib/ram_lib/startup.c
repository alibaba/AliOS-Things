/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "build_info.h"
#include "strproc.h"
#include "system_8195a.h"

u32 random_seed = 0x0;

#if defined ( __ICCARM__ )
#pragma section=".ram_image2.bss"
#pragma section=".ram_image2.skb.bss"
#pragma section=".rom.bss"
#pragma section=".ram.start.table"
#pragma section=".ram_image1.bss"
#pragma section=".image2.start.table1"
#pragma section=".image2.start.table2"

u8* __bss_start__;
u8* __bss_end__;

void __iar_data_init_app(void)
{
	__bss_start__               = (u8*)__section_begin(".ram_image2.bss");
	__bss_end__                 = (u8*)__section_end(".ram_image2.skb.bss");	
}
#endif

extern VOID SOCPS_WakeFromPG(VOID);

#if defined ( __ICCARM__ )
VOID
HalHardFaultHandler_user_define(u32 HardDefaultArg, u32 lr)
{
	cm_backtrace_fault(HardDefaultArg, lr);
}

VOID
HalHardFaultHandler_Patch_c(u32 HardDefaultArg, u32 lr)
{
	HalHardFaultHandler_user_define(HardDefaultArg, lr);
	INT_HardFault(HardDefaultArg);
}

VOID
HalHardFaultHandler_Patch_asm(void)
{
    asm("TST LR, #4\n"
        "ITE EQ\n"
        "MRSEQ R0, MSP\n"
        "MRSNE R0, PSP\n"
        "MOV R1, LR\n"
        "B HalHardFaultHandler_Patch_c");
}
#endif

// Override original Interrupt Vector Table
VOID BOOT_VectorTableOverride(u32 StackP)
{
	// Override NMI Handler
	//4 Initial NMI 
	//NewVectorTable[2] = (HAL_VECTOR_FUN)HalNMIHandler_Patch;

#if 0//defined ( __ICCARM__ )
	//Redefine Hardfault Handler
	NewVectorTable[3] = (HAL_VECTOR_FUN)HalHardFaultHandler_Patch_asm;
#endif
}

void BOOT_Reason(void)
{
	u32 backup_reg0 = BKUP_Read(BKUP_REG0);

	DBG_8195A("boot reason: %x \n", backup_reg0);
}

extern u32 GlobalDebugEnable;

VOID BOOT_InitDebugFlg(VOID)
{
	SYSTEM_DATA *SysData = (SYSTEM_DATA *)(SPI_FLASH_BASE + FLASH_SYSTEM_DATA_ADDR);

	/* reset */
	ConfigDebugErr = 0;
	ConfigDebugWarn = 0;
	ConfigDebugInfo = 0;

#if (defined(CONFIG_POST_SIM) || defined(CONFIG_CP))
	return;
#endif

	/* to initial ROM code using global variable */
#ifdef CONFIG_DEBUG_ERR_MSG
	ConfigDebugErr = 0xFFFFFFFF;//_DBG_MISC_;]
#endif
#ifdef CONFIG_DEBUG_WARN_MSG
	ConfigDebugWarn = 0xFFFFFFFF;
#endif
#ifdef CONFIG_DEBUG_INFO_MSG
	ConfigDebugInfo = 0xFFFFFFFF;
#endif
	if (SysData->UlogDbgEn == 0x0) {
		ConfigDebugErr = 0;
		ConfigDebugWarn = 0;
		ConfigDebugInfo = 0;
		GlobalDebugEnable = 0;
	}
}

VOID BOOT_RTC_Init(VOID)
{
	RTC_InitTypeDef RTC_InitStruct_temp;
	RTC_AlarmTypeDef RTC_AlarmStruct_temp;
	RTC_TimeTypeDef RTC_TimeStruct;

	/* for 32K more stable */
	NCO32K_Init(32768, XTAL_ClkGet(), 15, 2);
	
	RTC_ClokSource(0);
	RTC_StructInit(&RTC_InitStruct_temp);
	RTC_Init(&RTC_InitStruct_temp);

	/* 32760 need add need add 15 cycles (256Hz) every 4 min*/
	//RTC_SmoothCalibConfig(RTC_CalibSign_Positive, 15,
	//	RTC_CalibPeriod_4MIN, RTC_Calib_Enable);

	/* set time when power on */
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
	if (RTC_TimeStruct.RTC_Seconds == 0 && RTC_TimeStruct.RTC_Minutes == 0) {
		RTC_TimeStructInit(&RTC_TimeStruct);
		RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
	}
		
	/* set alarm */
	RTC_AlarmStructInit(&RTC_AlarmStruct_temp);			
	RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Days = 1;
	RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Hours = 1;
	RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Minutes = 1;
	RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Seconds = 30;
	RTC_AlarmStruct_temp.RTC_AlarmMask = RTC_AlarmMask_Hours | RTC_AlarmMask_Minutes;
	RTC_AlarmStruct_temp.RTC_Alarm2Mask = RTC_Alarm2Mask_Days;
	
	RTC_SetAlarm(RTC_Format_BIN, &RTC_AlarmStruct_temp);
	RTC_AlarmCmd(DISABLE);

	/* RTC interrupt hander is reserved for user */
	//InterruptRegister((IRQ_FUN)BOOT_RTC_Handler, RTC_IRQ, NULL, 4);
	//InterruptEn(RTC_IRQ, 4);
}

VOID BOOT_PlatformInit(VOID)
{
	u32 Temp = 0;
	
	/* Set SPS lower voltage */
	//Temp = ((HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_SYSCFG0)&0xf0ffffff)|0x6000000);
	//HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_SYSCFG0, Temp);

	/* Driving control of RF1 clock buffer, 11:large current, 00: small current */
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_XTAL_CTRL1);
	Temp &= ~(BIT_SYS_XTAL_DRV_RF1(3));
	Temp |= BIT_SYS_XTAL_DRV_RF1(1);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_XTAL_CTRL1, Temp);
}

//3 Imgae 2
VOID BOOT_Image2(VOID)
{
	int ret = 0;
	u32 OSC8MCali = _SUCCESS;

#ifdef CONFIG_FPGA
	MPU->RNR = 0; //0xE000ED00, 0x98 MPU Region RNRber Register
	MPU->RBAR = 0; //0xE000ED00, 0x9C MPU Region Base Address Register
	MPU->RASR = 0x6000027; //0xE000ED00, 0xA0 MPU Region Attribute and Size Register
	MPU->CTRL = 7; //0xE000ED00, 0x94 MPU Control Register
#endif

#if defined ( __ICCARM__ )   
	__iar_data_init_app();
#endif 

	BOOT_InitDebugFlg();
	
	BOOT_VectorTableOverride(0x1003EFFC);    

	/* set CPU clock if needed, default is 125MHz */
	//CPU_ClkSet((u8)(CPU_CLOCK_SEL_VALUE));
	//DelayUs(10);

#if (defined(CONFIG_CP))
	CPTest_EnterImg2Ok();
#endif

	DBG_8195A("===== Enter Image 2 ====\n");

	//3 0) Vendor Config function
	//4 Ram Bss Iitial
	u32 BssLen = (__bss_end__ - __bss_start__);

	_memset((void *) __bss_start__, 0, BssLen);

	SystemCoreClockUpdate();

	ret = boot_export_symbol.boot_system_init1();

	//BOOT_PlatformInit();

	OSC8M_CLOCK_GLB = 8388608;
#if (!defined(CONFIG_FPGA) && !defined(CONFIG_POST_SIM))
	OSC8MCali = OSC8M_Calibration(DISABLE, OSC32K_CALI_32KCYC_064, OSC8M_8388608HZ);
	DelayUs(90);
	DelayUs(90);
#endif  //CONFIG_FPGA

#ifdef CONFIG_CP
	CPTest_OSCCalibrationOk();
#endif

	/* Workaround for the GPIOA_7 didn't pull high: it may cause the 
	SDIO Device hardware be enabled automatically at power on and then 
	GPIOA[7:0] will be used for SDIO device */
#ifndef CONFIG_SDIO_DEVICE_EN
	// SDIO Pin Mux off
	SDIOD_PIN_FCTRL(OFF);
#endif

	DBG_8195A("OSC8M: %d \n", OSC8M_Get());

	BOOT_Reason();

	BOOT_RTC_Init();

#if (!defined(CONFIG_FPGA))
	if (SYSCFG0_BDOption() != SYSCFG_BD_QFN48_MCM_8MBFlash) {
		random_seed = Gen_RandomSeed();
	}
#endif

#if defined(CONFIG_WIFI_NORMAL) && defined(CONFIG_NETWORK)
	rtw_efuse_boot_write();
#endif
	ret = boot_export_symbol.boot_system_init2();
	
	APP_Start();

#if defined ( __ICCARM__ )
	// it is dummy code, but IAR linker need this
	__iar_data_init3();
#endif    
}

IMAGE2_VALID_PATTEN_SECTION
const u8 RAM_IMG2_VALID_PATTEN[20] = {
	'R', 'T', 'K', 'W', 'i', 'n', 0x0, 0xff, 
	(FW_VERSION&0xff), ((FW_VERSION >> 8)&0xff),
	(FW_SUBVERSION&0xff), ((FW_SUBVERSION >> 8)&0xff),
	(FW_CHIP_ID&0xff), ((FW_CHIP_ID >> 8)&0xff),
	(FW_CHIP_VER),
	(FW_BUS_TYPE),
	(FW_INFO_RSV1),
	(FW_INFO_RSV2),
	(FW_INFO_RSV3),
	(FW_INFO_RSV4)
};

IMAGE2_ENTRY_SECTION
RAM_START_FUNCTION gImage2EntryFun0 = {
	BOOT_Image2,
	SOCPS_WakeFromPG
	};
