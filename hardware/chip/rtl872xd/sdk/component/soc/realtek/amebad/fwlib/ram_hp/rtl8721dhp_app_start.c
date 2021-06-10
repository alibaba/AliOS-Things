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
#include "psram_reserve.h"
#if defined ( __ICCARM__ )
#pragma section=".ram_image2.bss"
#pragma section=".ram_image2.nocache.data"
#pragma section=".psram.bss"

SECTION(".data") u8* __bss_start__ = 0;
SECTION(".data") u8* __bss_end__ = 0;
SECTION(".data") u8* __ram_nocache_start__ = 0;
SECTION(".data") u8* __ram_nocache_end__ = 0;
SECTION(".data") u8* __psram_bss_start__ = 0;
SECTION(".data") u8* __psram_bss_end__ = 0;
#endif
extern int main(void);
extern u32 GlobalDebugEnable;
void NS_ENTRY BOOT_IMG3(void);
extern void INT_HardFault_C(uint32_t mstack[], uint32_t pstack[], uint32_t lr_value, uint32_t fault_id);
void app_init_psram(void);
void app_section_init(void)
{
#if defined ( __ICCARM__ )
	__bss_start__               = (u8*)__section_begin(".ram_image2.bss");
	__bss_end__                 = (u8*)__section_end(".ram_image2.bss");
	__ram_nocache_start__       = (u8*)__section_begin(".ram_image2.nocache.data");
	__ram_nocache_end__         = (u8*)__section_end(".ram_image2.nocache.data");
	__ram_nocache_end__ =  (u8*) (((((u32)__ram_nocache_end__-1) >> 5) + 1) << 5);  //32-byte aligned
#endif
}

u32 app_mpu_nocache_check(u32 mem_addr) {
	mpu_region_config mpu_cfg;

	if (wifi_config.km4_cache_enable) {
		mpu_cfg.region_base = (uint32_t)__ram_nocache_start__;
		mpu_cfg.region_size = __ram_nocache_end__-__ram_nocache_start__;
	} else {
		mpu_cfg.region_base = 0x10000000;
		mpu_cfg.region_size = 512*1024;
	}

	if((mem_addr >= mpu_cfg.region_base) && (mem_addr < (mpu_cfg.region_base + mpu_cfg.region_size)))
		return TRUE;
	else
		return FALSE;
}

u32 app_mpu_nocache_init(void)
{
	mpu_region_config mpu_cfg;
	u32 mpu_entry = 0;

	mpu_entry = mpu_entry_alloc();
	if (wifi_config.km4_cache_enable) {
		mpu_cfg.region_base = (uint32_t)__ram_nocache_start__;
		mpu_cfg.region_size = __ram_nocache_end__-__ram_nocache_start__;
	} else {
		mpu_cfg.region_base = 0x10000000;
		mpu_cfg.region_size = 512*1024;
	}
	mpu_cfg.xn = MPU_EXEC_ALLOW;
	mpu_cfg.ap = MPU_UN_PRIV_RW;
	mpu_cfg.sh = MPU_NON_SHAREABLE;
	mpu_cfg.attr_idx = MPU_MEM_ATTR_IDX_NC;
	if (mpu_cfg.region_size >= 32) {
		mpu_region_cfg(mpu_entry, &mpu_cfg);
	}

	/* close 216K irom_ns cache */
	mpu_entry = mpu_entry_alloc();
	mpu_cfg.region_base = 0x1010A000;
	mpu_cfg.region_size = 0x10140000 - 0x1010A000;
	mpu_cfg.xn = MPU_EXEC_ALLOW;
	mpu_cfg.ap = MPU_UN_PRIV_RW;
	mpu_cfg.sh = MPU_NON_SHAREABLE;
	mpu_cfg.attr_idx = MPU_MEM_ATTR_IDX_NC;
	mpu_region_cfg(mpu_entry, &mpu_cfg);

	/* close 80K drom_ns cache */
	mpu_entry = mpu_entry_alloc();
	mpu_cfg.region_base = 0x101C0000;
	mpu_cfg.region_size = 0x101D4000 - 0x101C0000;
	mpu_cfg.xn = MPU_EXEC_ALLOW;
	mpu_cfg.ap = MPU_UN_PRIV_RW;
	mpu_cfg.sh = MPU_NON_SHAREABLE;
	mpu_cfg.attr_idx = MPU_MEM_ATTR_IDX_NC;
	mpu_region_cfg(mpu_entry, &mpu_cfg);

	/* set 1KB retention ram no-cache */
	mpu_entry = mpu_entry_alloc();
	mpu_cfg.region_base = 0x000C0000;
	mpu_cfg.region_size = 0x400;
	mpu_cfg.xn = MPU_EXEC_ALLOW;
	mpu_cfg.ap = MPU_UN_PRIV_RW;
	mpu_cfg.sh = MPU_NON_SHAREABLE;
	mpu_cfg.attr_idx = MPU_MEM_ATTR_IDX_NC;
	mpu_region_cfg(mpu_entry, &mpu_cfg);

	return 0;
}

VOID app_vdd1833_detect(VOID)
{
	u32 temp;

	if (FALSE == is_power_supply18()) {
		temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_RFAFE_IND_VIO1833);
		temp |= BIT_RFAFE_IND_VIO1833;
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HS_RFAFE_IND_VIO1833, temp);
	}

	DBG_8195A("REG_HS_RFAFE_IND_VIO1833 (0 is 1.8V): %x\n", HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HS_RFAFE_IND_VIO1833));
}

#if defined (__GNUC__)
/* Add This for C++ support to avoid compile error */
void _init(void) {}
#endif

void INT_HardFault_Patch_C(uint32_t mstack[], uint32_t pstack[], uint32_t lr_value, uint32_t fault_id)
{
	DBG_8195A("\r\nHard Fault Patch\r\n");

	INT_HardFault_C(mstack, pstack, lr_value, fault_id);
}

VOID
INT_HardFault_Patch(void)
{
	__ASM volatile(
		"MRS R0, MSP\n\t"
		"MRS R1, PSP\n\t"
		"MOV R2, LR\n\t" /* second parameter is LR current value */
		"MOV R3, #0\n\t"   
		"SUB.W	R4, R0, #128\n\t"
		"MSR MSP, R4\n\t"   // Move MSP to upper to for we can dump current stack contents without chage contents
		"LDR R4,=INT_HardFault_Patch_C\n\t"
		"BX R4\n\t"
	);
}

VOID
INT_UsageFault_Patch(void)
{
	__ASM volatile(
		"MRS R0, MSP\n\t"
		"MRS R1, PSP\n\t"
		"MOV R2, LR\n\t" /* second parameter is LR current value */
		"MOV R3, #1\n\t"   
		"SUB.W	R4, R0, #128\n\t"
		"MSR MSP, R4\n\t"   // Move MSP to upper to for we can dump current stack contents without chage contents
		"LDR R4,=INT_HardFault_Patch_C\n\t"
		"BX R4\n\t"
	);
}

VOID
INT_BusFault_Patch(void)
{
	__ASM volatile(
		"MRS R0, MSP\n\t"
		"MRS R1, PSP\n\t"
		"MOV R2, LR\n\t" /* second parameter is LR current value */
		"MOV R3, #2\n\t"   
		"SUB.W	R4, R0, #128\n\t"
		"MSR MSP, R4\n\t"   // Move MSP to upper to for we can dump current stack contents without chage contents
		"LDR R4,=INT_HardFault_Patch_C\n\t"
		"BX R4\n\t"
	);
}

VOID
INT_MemFault_Patch(void)
{
	__ASM volatile(
		"MRS R0, MSP\n\t"
		"MRS R1, PSP\n\t"
		"MOV R2, LR\n\t" /* second parameter is LR current value */
		"MOV R3, #3\n\t"   
		"SUB.W	R4, R0, #128\n\t"
		"MSR MSP, R4\n\t"   // Move MSP to upper to for we can dump current stack contents without chage contents
		"LDR R4,=INT_HardFault_Patch_C\n\t"
		"BX R4\n\t"
	);
}

extern void HardFault_Handler(void);
VOID VectorTableOverride(VOID)
{
	NewVectorTable[3] = (HAL_VECTOR_FUN)HardFault_Handler;
	NewVectorTable[4] = (HAL_VECTOR_FUN)HardFault_Handler;
	NewVectorTable[5] = (HAL_VECTOR_FUN)HardFault_Handler;
	NewVectorTable[6] = (HAL_VECTOR_FUN)HardFault_Handler;
}

extern  void PendSV_Handler (void);
void SysTick_Handler( void )
{
	krhino_intrpt_enter();
	krhino_tick_proc();
	krhino_intrpt_exit();
}

void app_init_psram(void)
{
	u32 temp;
	PCTL_InitTypeDef  PCTL_InitStruct;

	/*set rwds pull down*/
	temp = HAL_READ32(PINMUX_REG_BASE, 0x104);
	temp &= ~(PAD_BIT_PULL_UP_RESISTOR_EN | PAD_BIT_PULL_DOWN_RESISTOR_EN);
	temp |= PAD_BIT_PULL_DOWN_RESISTOR_EN;
	HAL_WRITE32(PINMUX_REG_BASE, 0x104, temp);

	PSRAM_CTRL_StructInit(&PCTL_InitStruct);
	PSRAM_CTRL_Init(&PCTL_InitStruct);

	PSRAM_PHY_REG_Write(REG_PSRAM_CAL_PARA, 0x02030310);

	/*check psram valid*/
	HAL_WRITE32(PSRAM_BASE, 0, 0);
	assert_param(0 == HAL_READ32(PSRAM_BASE, 0));

	if(_FALSE == PSRAM_calibration())
		return;

	if(FALSE == psram_dev_config.psram_dev_cal_enable) {
		temp = PSRAM_PHY_REG_Read(REG_PSRAM_CAL_CTRL);
		temp &= (~BIT_PSRAM_CFG_CAL_EN);
		PSRAM_PHY_REG_Write(REG_PSRAM_CAL_CTRL, temp);
	}

#if defined ( __ICCARM__ )
	__psram_bss_start__ = (u8*)__section_begin(".psram.bss");
	__psram_bss_end__   = (u8*)__section_end(".psram.bss");	
#endif

	/*init psram bss area*/
	memset(__psram_bss_start__, 0, __psram_bss_end__ - __psram_bss_start__);

	//pmu_register_sleep_callback(PMU_PSRAM_DEVICE, (PSM_HOOK_FUN)app_psram_suspend, NULL, (PSM_HOOK_FUN)app_psram_resume, NULL);
}

static void* app_psram_load_ns()
{
	IMAGE_HEADER *Image2Hdr = (IMAGE_HEADER *)((__flash_text_start__) - IMAGE_HEADER_LEN);
	IMAGE_HEADER * Image2DataHdr = (IMAGE_HEADER *)(__flash_text_start__ + Image2Hdr->image_size);
	IMAGE_HEADER *PsramHdr =  (IMAGE_HEADER *)((u32)Image2DataHdr + IMAGE_HEADER_LEN + Image2DataHdr->image_size);

	DBG_PRINTF(MODULE_BOOT, LEVEL_INFO,"IMG2 PSRAM_NS:[0x%x:%d:0x%x]\n", (u32)(PsramHdr + 1),
			PsramHdr->image_size, PsramHdr->image_addr);

	/* load psram code+data into PSRAM */
	if((PsramHdr->image_size != 0) && \
		(PsramHdr->image_addr == 0x02000020) && \
		(PsramHdr->signature[0] == 0x35393138) && \
		(PsramHdr->signature[1] == 0x31313738)) {

		_memcpy((void*)PsramHdr->image_addr, (void*)(PsramHdr + 1), PsramHdr->image_size);
	}
}

// The Main App entry point
void app_start(void)
{
	//cmse_address_info_t cmse_address_info = cmse_TT((void *)DiagPrintf);
#if defined (configUSE_SAU) && (configUSE_SAU == 1U)
	irq_table_init(MSP_RAM_HP_NS); /* NS Vector table init */
#endif
	VectorTableOverride();
	app_section_init();
	_memset((void *) __bss_start__, 0, (__bss_end__ - __bss_start__));
	if(HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_BOOT_CFG) & BIT_BOOT_IMG3_EXIST) {
		BOOT_IMG3();
	}
    
	/* enable non-secure cache */
	Cache_Enable(ENABLE);
	DBG_PRINTF(MODULE_BOOT, LEVEL_INFO,"KM4 BOOT REASON: %x \n", BOOT_Reason());
	
	SystemCoreClockUpdate();
	
	SOCPS_InitSYSIRQ_HP();

	/* Init PSRAM */
	if(TRUE == psram_dev_config.psram_dev_enable) {
		app_init_psram();
	}

	__NVIC_SetVector(SVCall_IRQn, (u32)(VOID*)NULL);
	__NVIC_SetVector(PendSV_IRQn, (u32)(VOID*)PendSV_Handler);
	__NVIC_SetVector(SysTick_IRQn, (u32)(VOID*)SysTick_Handler);

	/* load psram image if needed */
	if(TRUE == psram_dev_config.psram_dev_enable) {
		app_psram_load_ns();
	}

#if defined (__GNUC__)
extern void __libc_init_array(void);
/* Add This for C++ support */
	__libc_init_array();
#endif

	// force SP align to 8 byte not 4 byte (initial SP is 4 byte align)
	__asm( 
		"mov r0, sp\n"
		"bic r0, r0, #7\n" 
		"mov sp, r0\n"
	);

	mpu_init();
	app_mpu_nocache_init();
	app_vdd1833_detect();
	memcpy_gdma_init();
	//retention Ram space should not exceed 0xB0
	assert_param(sizeof(RRAM_TypeDef) <= 0xB0);

	main(); /* project/xxxx/src/main.c */

#if defined ( __ICCARM__ )
	//__iar_cstart_call_ctors(NULL);

	/* it is dummy code, but IAR linker need this */
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
RAM_START_FUNCTION Img2EntryFun0 = {
	app_start,	
	NULL,//BOOT_RAM_WakeFromPG,
	(u32)NewVectorTable
};

