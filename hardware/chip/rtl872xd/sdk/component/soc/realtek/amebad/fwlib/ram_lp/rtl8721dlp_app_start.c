/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "rtl8721d_system.h"
#if defined ( __ICCARM__ )
#pragma section=".ram_image2.bss"

u8* __bss_start__;
u8* __bss_end__;
#endif

static u32 Cutversion;

extern int main(void);

void app_section_init(void)
{
#if defined ( __ICCARM__ )
	__bss_start__               = (u8*)__section_begin(".ram_image2.bss");
	__bss_end__                 = (u8*)__section_end(".ram_image2.bss");	
#endif
}

/* The binary data of generated ram_retention.bin should copy into retention_ram_patch_array. 
	Then fill in the patch size at the second dword */
const u32 retention_ram_patch_array[2][RETENTION_RAM_SYS_OFFSET/4] = {
	{
		0x000c0009,
		0x00000080,
		0x2210f240,
		0x0200f6c4,
		0x49106813,
		0xf240400b,
		0x430b2100,
		0x22906013,
		0x05d22380,
		0x045b6811,
		0xd1124219,
		0x6013430b,
		0x3300f240,
		0x0300f6c4,
		0x4907681a,
		0x601a400a,
		0x3318f240,
		0xf6c42101,
		0x681a0300,
		0x601a430a,
		0x4770bf30,
		0xfffff9ff,
		0xdfffffff,
	},
	{
		0x000c0009,
		0x00000080,
		0x2210f240,
		0x0200f6c4,
		0x49036813,
		0xf240400b,
		0x430b2100,
		0x47706013,
		0xfffff9ff,
	}
};

void app_load_patch_to_retention(void)
{
	u32 Temp = 0;

	if(Cutversion<=0x1){
		_memcpy(__retention_entry_func__, retention_ram_patch_array[0], RETENTION_RAM_SYS_OFFSET);
	}else {
		_memcpy(__retention_entry_func__, retention_ram_patch_array[1], RETENTION_RAM_SYS_OFFSET);
	}

	Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_BOOT_REASON1);
	Temp |= BIT_DSLP_RETENTION_RAM_PATCH;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_BOOT_REASON1, Temp);
}

RETENTION_TEXT_SECTION
void app_retention_ram_patch(void)
{
	u32 Temp = 0;

	/* SPIC clock source switch */
	Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_CLK_CTRL0);
	Temp &= ~ (BIT_MASK_FLASH_CLK_SEL << BIT_SHIFT_FLASH_CLK_SEL);
	Temp |= BIT_SHIFT_FLASH_CLK_XTAL;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_CLK_CTRL0, Temp);

	if( Cutversion<=0x1){
		/* dslp again to fix pad shutdown flashdownload issue */
		Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_PWR_CTRL);
		if ((Temp & BIT_DSLP_SNOOZE_MODE_LSPAD_SHUTDOWN) == 0) {
			Temp |= BIT_DSLP_SNOOZE_MODE_LSPAD_SHUTDOWN;
			HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_PWR_CTRL, Temp);

			/* Set Event */
			Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LP_SLP_WAKE_EVENT_MSK0);
			Temp &= ~BIT_LP_WEVT_AON_MSK;
			HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LP_SLP_WAKE_EVENT_MSK0, Temp);

			Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LP_PWRMGT_CTRL);
			Temp |= BIT_LSYS_PMC_PMEN_DSLP;
			HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LP_PWRMGT_CTRL, Temp);
			__WFI();
		}
	}
}

RETENTION_ENTRY_SECTION
DSLP_RETENTION_FUNC_TABLE app_dslp_patch_table = {
	.DSLPPatchFun0 = app_retention_ram_patch,
	.PatchLen = 0,
};

//set all KM0 memory no-cachable, just flash cachable
//0x0000_0000	0x0001_FFFF	128K	KM0 ITCM ROM (actually 96K)
//0x0002_0000	0x0002_7FFF	32K	KM0 DTCM ROM (actually 16K)
//0x0002_8000	0x0007_FFFF	352K	RSVD
//0x0008_0000	0x0008_FFFF	64K	KM0 SRAM
//0x0008_8000	0x000B_FFFF	224K	RSVD
//0x000C_0000	0x000C_3FFF	16K	Retention SRAM (1KB) (The same port with KM0 SRAM)
u32 app_mpu_nocache_init(void)
{
	mpu_region_config mpu_cfg;
	u32 mpu_entry = 0;

#if 0
	mpu_entry = mpu_entry_alloc();

	mpu_cfg.region_base = 0x00080000;//__ram_nocache_start__;
	mpu_cfg.region_size = 0x0008F000 - 0x00080000;//__ram_nocache_end__-__ram_nocache_start__;
	mpu_cfg.xn = MPU_EXEC_ALLOW;
	mpu_cfg.ap = MPU_UN_PRIV_RW;
	mpu_cfg.sh = MPU_NON_SHAREABLE;
	mpu_cfg.attr_idx = MPU_MEM_ATTR_IDX_NC;

	//DBG_8195A("app_mpu_nocache_init entry: %x[%x:%x]\n", mpu_entry,
	//	mpu_cfg.region_base, mpu_cfg.region_size);

	if (mpu_cfg.region_size >= 32) {
		mpu_region_cfg(mpu_entry, &mpu_cfg);
	}

	/* close 96K irom cache */
	mpu_entry = mpu_entry_alloc();
	mpu_cfg.region_base = 0x00000000;
	mpu_cfg.region_size = 0x18000;
	mpu_cfg.xn = MPU_EXEC_ALLOW;
	mpu_cfg.ap = MPU_UN_PRIV_RW;
	mpu_cfg.sh = MPU_NON_SHAREABLE;
	mpu_cfg.attr_idx = MPU_MEM_ATTR_IDX_NC;
	mpu_region_cfg(mpu_entry, &mpu_cfg);

	/* close 16K drom cache */
	mpu_entry = mpu_entry_alloc();
	mpu_cfg.region_base = 0x00020000;
	mpu_cfg.region_size = 0x4000;
	mpu_cfg.xn = MPU_EXEC_ALLOW;
	mpu_cfg.ap = MPU_UN_PRIV_RW;
	mpu_cfg.sh = MPU_NON_SHAREABLE;
	mpu_cfg.attr_idx = MPU_MEM_ATTR_IDX_NC;
	mpu_region_cfg(mpu_entry, &mpu_cfg);
#else
	mpu_entry = mpu_entry_alloc();
	mpu_cfg.region_base = 0x00000000;
	mpu_cfg.region_size = 0x000C4000;
	mpu_cfg.xn = MPU_EXEC_ALLOW;
	mpu_cfg.ap = MPU_UN_PRIV_RW;
	mpu_cfg.sh = MPU_NON_SHAREABLE;
	mpu_cfg.attr_idx = MPU_MEM_ATTR_IDX_NC;
	mpu_region_cfg(mpu_entry, &mpu_cfg);	
#endif
	return 0;
}

VOID app_start_autoicg(VOID)
{
	u32 temp = 0;
	
	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_KM0_CTRL);
	temp |= BIT_LSYS_PLFM_AUTO_ICG_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_KM0_CTRL, temp);
}

VOID app_pmc_patch(VOID)
{
	/* flash pin floating issue */
	//0x4800_00B0[31:0] = 0x0080_1A12
	//0x4800_00B8[31:0]  = 0x0A00_301A
	//0x4800_00BC[31:0] = 0x0801_3802
	//0x4800_00C0[31:0] = 0x00C0_0123
	Cutversion = SYSCFG_CUTVersion();
	//DBG_8195A("cut version:%d\n", Cutversion);

	if(Cutversion<=0x1){
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, 0xB0, 0x00801A12);
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, 0xB8, 0x0A00301A);
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, 0xBC, 0x08013802);
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, 0xC0, 0x00C00123);
	}

	/*PMC gateclock delay 8us patch*/
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_SYSON_PMC_PATCH_GRP1_L, 0x0080B482);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_SYSON_PMC_PATCH_GRP2_L, 0x3CC00FB4);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_SYSON_PMC_PATCH_GRP2_H, 0x080BC300);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_SYSON_PMC_PATCH_GRP3_L, 0x0E05DDBC);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_SYSON_PMC_PATCH_GRP3_H, 0x00000202);

	/* SPIC clock source switch patch */
	app_load_patch_to_retention();
}

VOID app_vdd1833_detect(VOID)
{
	u32 temp = 0;
	u32 vdd33_detected = 0;
	u32 data = 0;
	u8 swr_vol = 0;
	ADC_InitTypeDef ADC_InitStruct;
	u16 buf[16], i =0;

	/* check vddcore is 1.8V or 3.3V, default is 1.8V */
	RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);

	ADC_StructInit(&ADC_InitStruct);
	ADC_InitStruct.ADC_OpMode = ADC_AUTO_MODE;
	ADC_InitStruct.ADC_CvlistLen = 0;
	ADC_InitStruct.ADC_Cvlist[0]= ADC_CH10;
	ADC_Init(&ADC_InitStruct);
	
	ADC_Cmd(ENABLE);
	ADC_ReceiveBuf(buf, 16);
	ADC_Cmd(DISABLE);

	while(i < 16)
		data += buf[i++];

	data = data >> 4;
	if(data > 3000) {
		vdd33_detected = 1;
		DBG_8195A("VDDIO 3.3V\n");
	} else {
		vdd33_detected = 0;
		DBG_8195A("VDDIO 1.8V\n");
	}

	if (vdd33_detected) {
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL0);
		temp |= BIT_AON_V1833_SEL;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL0, temp);

		/*This value is given by MAC team yx_qi*/
		swr_vol = 0x0A;
	} else {
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL0);
		temp &= ~BIT_AON_V1833_SEL;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_LDO_CTRL0, temp);

		/*This value is given by MAC team yx_qi*/
		swr_vol =0x08;
	}
	/* To reduce the differ between 1.8v and 3.3v's SWR output value*/
	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG1);
	temp &= ~BIT_MASK_SWR_VOL_L1; 
	temp |= swr_vol;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG1,temp);
}

void app_dslp_wake_check(void)
{
	DBG_8195A("BOOT_Reason:%x %x\n", BOOT_Reason(), BIT_BOOT_DSLP_RESET_HAPPEN);

	/* set deep sleep wakeup status, you can use SOCPS_DsleepWakeStatusGet */
	/* to get this status after this point, some function like: */
	/* RTC/TOUCH/WIFI DLPS/Key-Scan and so on need this status */
	if(BIT_BOOT_DSLP_RESET_HAPPEN & BOOT_Reason()) {
		SOCPS_DsleepWakeStatusSet(TRUE);
	} else {
		SOCPS_DsleepWakeStatusSet(FALSE);
	}
}

static void app_gen_random_seed(void)
{
	u32 data;
	int i = 0, j = 0;
	u8 random[4], tmp;
	CAPTOUCH_TypeDef *CapTouch = (CAPTOUCH_TypeDef*) CTC_REG_BASE;

	PAD_PullCtrl((_PB_4), GPIO_PuPd_NOPULL);

	/* Set CTC to debug mode */
	CapTouch_Cmd(CAPTOUCH_DEV, DISABLE);
	CapTouch->CT_IER = 0;
	CapTouch->CT_ICR_ALL = 1;
	CapTouch->CT_MODE_CTRL = 0;
	CapTouch->CT_SP_CTRL = 0x00070000;
	CapTouch->CT_ETC_CTRL = 0;
	CapTouch->CT_CH[0].CTRL = 0;
	CapTouch->CT_CH[0].MBIAS = 0x6;

	CapTouch_DbgCmd(CAPTOUCH_DEV, ENABLE);
	CapTouch_Cmd(CAPTOUCH_DEV, ENABLE);

	/* continuous sample in debug mode */
	for(i = 0; i < 4; i++) {
retry:
		tmp = 0;
		for(j = 0; j < 8; j++) {
			data = CapTouch_DbgRawData(CAPTOUCH_DEV);
			tmp |= ((data & BIT(0)) << j);
		}

		if(tmp == 0 || tmp == 0xFF)
			goto retry;

		random[i] = tmp;
	}

	/* Disable debug mode */
	CapTouch_DbgCmd(CAPTOUCH_DEV, DISABLE);
	CapTouch_Cmd(CAPTOUCH_DEV, DISABLE);

	rand_first = 1;
	data = (random[3] << 24) | (random[2] << 16) | (random[1] << 8) | (random[0]);
	rand_seed[0] = data;
	rand_seed[1] = data;
	rand_seed[2] = data;
	rand_seed[3] = data;

	BKUP_Write(BKUP_REG6, data);

	return;
}

extern void TaskIdle_8721D(void);
IMAGE2_RAM_TEXT_SECTION
void
app_taskidle_handler(
    void
)
{
#if CONFIG_WIFI_FW_EN
	TaskIdle_8721D();
#endif
}

// The Main App entry point
void app_start(void)
{
	app_section_init();
	_memset((void *) __bss_start__, 0, (__bss_end__ - __bss_start__));

	SystemCoreClockUpdate();

	DBG_PRINTF(MODULE_BOOT, LEVEL_INFO,"KM0 BOOT_IMG2 BOOT REASON: %x \n", BOOT_Reason());

	/* check if enable WIFI */
	if (ps_config.km0_config_wifi_enable) {
		BKUP_Set(0, BIT_WIFI_ENABLE);
	}

	/* setting key & captouch information */
	if (ps_config.km0_enable_key_touch & BIT_KEY_ENABLE)
		BKUP_Set(0, BIT_KEY_ENABLE);
	if (ps_config.km0_enable_key_touch & BIT_CAPTOUCH_ENABLE)
		BKUP_Set(0, BIT_CAPTOUCH_ENABLE);
	if (ps_config.km0_enable_key_touch & BIT_GPIO_ENABLE)
		BKUP_Set(0, BIT_GPIO_ENABLE);

	/* check and set dsleep wakeup status */
	app_dslp_wake_check();

	/* do it before pinmap_init, or power leakage will happened */
	app_gen_random_seed();

	/* low power pin dont need pinmap init again after wake from dslp */
	pinmap_init(); /* 1.7ms */

	if(SOCPS_DsleepWakeStatusGet() == FALSE) {
		OSC131K_Calibration(30000); /* PPM=30000=3% *//* 7.5ms */

		SDM32K_Enable(SDM32K_ALWAYS_CAL); /* 0.6ms */
		SDM32K_RTCCalEnable(ps_config.km0_rtc_calibration); /* 0.3ms */

		// Retention Ram reset
		_memset((void*)RETENTION_RAM_BASE,0,1024);
		assert_param(sizeof(RRAM_TypeDef) <= 0xB0);
	} else {
		SOCPS_DsleepWakeStatusSet(TRUE); /* KM4 boot check it */
	}
	OSC4M_Init();
	OSC2M_Calibration(OSC2M_CAL_CYC_128, 30000); /* PPM=30000=3% *//* 0.5 */
	SYSTIMER_Init(); /* 0.2ms */

	SOCPS_InitSYSIRQ();
	km4_pm_init();

	app_start_autoicg();
	app_pmc_patch();

	if(!(BIT_BOOT_BOD_RESET_HAPPEN & BOOT_Reason())) {
		app_vdd1833_detect();
	}

	__NVIC_SetVector(SVCall_IRQn, (u32)vPortSVCHandler);
	__NVIC_SetVector(PendSV_IRQn, (u32)xPortPendSVHandler);
	__NVIC_SetVector(SysTick_IRQn, (u32)xPortSysTickHandler); 

#if defined ( __ICCARM__ )
	/* it is dummy code, but IAR linker need this */
	__iar_data_init3();
#endif

	mpu_init();
	app_mpu_nocache_init();

	main();
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
RAM_START_FUNCTION Img2EntryFun0 = {
	app_start,
		NULL,//SOCPS_WakeFromPG
		NULL,
	
};
