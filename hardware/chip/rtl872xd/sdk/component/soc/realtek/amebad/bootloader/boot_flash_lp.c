/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"

extern u8 Boot_Log_En;
extern u8 Force_OTA1_GPIO;
extern u32 OTA_Region[2];
extern MMU_ConfDef Flash_MMU_Config[];
extern RSIP_MaskDef RSIP_Mask_Config[];
extern FuncPtr FwCheckCallback;
extern FuncPtr OTASelectHook;

extern u8 __ls_flash_text_start__[];
extern u8 __hs_flash_text_start__[];

BOOT_RAM_DATA_SECTION
CPU_PWR_SEQ SYSPLL_ON_SEQ[] = {
	/* Enable SYS PLL */
	{0x48000208, CPU_PWRSEQ_CMD_WRITE,		0x00000000,			(0x01 << 28)},
	{0x48000280, CPU_PWRSEQ_CMD_WRITE,		0x00000000,			0x00000003},
	{0x00000000, CPU_PWRSEQ_CMD_DELAY,		0x00000000,			4}, //delay 2us
	{0x48000280, CPU_PWRSEQ_CMD_WRITE,		0x00000000,			(0x01 << 2)},
	{0x480002B0, CPU_PWRSEQ_CMD_WRITE,		0x00000000,			(0x01 << 31)},
	{0x00000000, CPU_PWRSEQ_CMD_DELAY,		0x00000000,			400}, //delay 400us
	{0x48000264, CPU_PWRSEQ_CMD_WRITE,		(0x01 << 29),			0x00000000},
	{0x480002B0, CPU_PWRSEQ_CMD_WRITE,		0x00000000,			(0x03 << 29)},
	{0x48000200, CPU_PWRSEQ_CMD_WRITE,		(0x01 << 21),			0x00000000},
	//{0x480003F4, CPU_PWRSEQ_CMD_POLLING,	(0x01 << 12),			(0x01 << 12)}, /* temp use delay because FPGA dont have this function */
	{0x00000000, CPU_PWRSEQ_CMD_DELAY,		0x00000000,			100}, //delay 100us
	{0x480002B0, CPU_PWRSEQ_CMD_WRITE,		0x00000000,			(0x01 << 8)},

	/* End */
	{0xFFFFFFFF, CPU_PWRSEQ_CMD_END,			0x00000000,			0x00000000},
};

FLASH_BOOT_TEXT_SECTION
static u32 BOOT_FLASH_OTA_Force1Check(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	u32 gpio_pin;
	u32 force_to_OTA1 = FALSE;
	u32 active_state;

	/* polling GPIO to check if user force boot to OTA1 */
	gpio_pin = Force_OTA1_GPIO;

	if (gpio_pin != 0xff) {
		/* pin coding: [7]: active level, [6:5]: port, [4:0]: pin */
		GPIO_InitStruct.GPIO_Pin = gpio_pin & 0x3F;/* pin is 6bits */
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;

		if (gpio_pin & 0x80) {
			/*  High active */			
			GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
			active_state = GPIO_PIN_HIGH;
		} else {
			/*  Low active */
			GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
			active_state = GPIO_PIN_LOW;
		}

		GPIO_Init(&GPIO_InitStruct);
		if (GPIO_ReadDataBit(GPIO_InitStruct.GPIO_Pin) == active_state) {
			force_to_OTA1 = 1;
		} else {
			force_to_OTA1 = 0;
		}
		GPIO_DeInit(GPIO_InitStruct.GPIO_Pin);
	}

	return (force_to_OTA1);
}

FLASH_BOOT_TEXT_SECTION
static u8 BOOT_FLASH_OTA_MMU(u8 idx, u32 vAddr, u32 pAddr, u32 *size)
{
	IMAGE_HEADER *Img2Hdr, *Img2DataHdr;
	u32 ImgSize, IsMinus, Offset;
	u8 res = _TRUE;

	/* Calculate mapping offset */
	if(vAddr > pAddr) {
		IsMinus = 1;
		Offset = vAddr - pAddr;
	} else {
		IsMinus = 0;
		Offset = pAddr - vAddr;
	}

	/* Mapping a memory from virtual address to physical address, which is large enough 
		to get Code/Data header information. */
	RSIP_MMU_Config(idx, vAddr, vAddr + 0x02000000 - 1, IsMinus, Offset);
	RSIP_MMU_Cmd(idx, ENABLE);

	/* Verify signature */
	Img2Hdr = (IMAGE_HEADER *)vAddr;
	if((Img2Hdr->signature[0] != 0x35393138) || (Img2Hdr->signature[1] != 0x31313738)) {
		res = _FALSE;
		goto exit;
	}

	/* Get image size, which should be 4KB-aligned */
	Img2DataHdr = (IMAGE_HEADER *)(vAddr + IMAGE_HEADER_LEN + Img2Hdr->image_size);
	ImgSize = Img2Hdr->image_size + Img2DataHdr->image_size + IMAGE_HEADER_LEN * 2;
	ImgSize = (((ImgSize -1) >> 12) + 1) << 12;  /* 4KB aligned */

	/* Mapping the current image from virtual address to physical address */
	RSIP_MMU_Config(idx, vAddr, vAddr + ImgSize - 1, IsMinus, Offset);

	if(size != NULL)
		*size = ImgSize;

exit:
	/* Clean Dcache */
	DCache_Invalidate((u32)Img2Hdr, IMAGE_HEADER_LEN);
	DCache_Invalidate((u32)Img2DataHdr, IMAGE_HEADER_LEN);

	return res;
}

FLASH_BOOT_TEXT_SECTION
static u8 BOOT_FLASH_OTA_Check(u8 ota_idx)
{
	u32 LsSize;
	u8 res;

	/* Mapping KM0 IMG2 to OTA1*/
	res = BOOT_FLASH_OTA_MMU(0, (u32)__ls_flash_text_start__ - IMAGE_HEADER_LEN, OTA_Region[ota_idx], &LsSize);
	if(res == _FALSE)
		return res;

	/* Mapping KM4 IMG2 to OTA1, should be 4 KB aligned */
	res = BOOT_FLASH_OTA_MMU(1, (u32)__hs_flash_text_start__ - IMAGE_HEADER_LEN, OTA_Region[ota_idx] + LsSize, NULL);
	if(res == _FALSE) {
		while(1) {
			DBG_8195A("\n\nWarnning!!\nAmebaD Flash Memory Layout is modified!\n"
				"Please download km0_km4_image2.bin instead of km0_image2_all.bin & km4_image2_all.bin!!\n\n"
				"Location: project/realtek_amebaD_cm4_gcc_verification/asdk/image\n\n\n");
			DelayMs(2000);
		}
		return res;
	}
	
	/* Check hash/sum if needed */
	if((FwCheckCallback != NULL) && (FwCheckCallback() != _TRUE))
		res = _FALSE;
	else
		res = _TRUE;

	return res;
}

FLASH_BOOT_TEXT_SECTION
static u32 BOOT_FLASH_OTA_Select(void)
{
	u8 OtaValid;
	u8 OtaSelect;
	
	/* check OTA1 force pin trigger */ 
	if (BOOT_FLASH_OTA_Force1Check() == _TRUE) {
		DBG_PRINTF(MODULE_BOOT, LEVEL_INFO,"GPIO force OTA1 \n");
	} else {
		/* check OTA2 if valid or not */
		OtaValid = BOOT_FLASH_OTA_Check(OTA_INDEX_2);
		if(OtaValid == _TRUE) {
			OtaSelect = 2;
			DBG_PRINTF(MODULE_BOOT, LEVEL_INFO,"OTA2 USE\n");
			goto exit;
		}
	}

	/* check OTA1 if valid or not */
	OtaValid = BOOT_FLASH_OTA_Check(OTA_INDEX_1);
	if(OtaValid == _TRUE) {
		OtaSelect = 1;
		DBG_PRINTF(MODULE_BOOT, LEVEL_INFO,"OTA1 USE\n");
	} else {
		OtaSelect = 0;
		RSIP_MMU_Cmd(0, DISABLE);
		RSIP_MMU_Cmd(1, DISABLE);
	}

exit: 
	return OtaSelect;
}

/**
  * @brief  Get boot reason from AON & SYSON register, and back up to REG_LP_SYSTEM_CFG2
  * @param  NA
  * @retval 0.
  * @note this function is called once by bootloader when KM0 boot to flash, user can not use it.
  */
FLASH_BOOT_TEXT_SECTION
u32 BOOT_FLASH_Reason_Set(void)
{
	u32 tmp_reason = 0;
	u32 reason_aon = 0;
	u32 temp_bootcfg = 0;

	/* get aon boot reason */
	reason_aon = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_BOOT_REASON1);

	/* get syson boot reason */
	tmp_reason = BACKUP_REG->DWORD[0] & BIT_MASK_BOOT_REASON;
	tmp_reason = tmp_reason << BIT_BOOT_REASON_SHIFT;

	//DBG_8195A("BOOT_FLASH_Reason_Set: %x %x \n", reason_aon, tmp_reason);
	/* set dslp boot reason */
	if (reason_aon & BIT_AON_BOOT_EXIT_DSLP) {
		tmp_reason |= BIT_BOOT_DSLP_RESET_HAPPEN;
	} else {
		tmp_reason &= ~BIT_BOOT_DSLP_RESET_HAPPEN;
	}
	/* set BOD boot reason */
	if (reason_aon & BIT_AON_BOD_RESET_FLAG) {
		tmp_reason |= BIT_BOOT_BOD_RESET_HAPPEN;
	} else {
		tmp_reason &= ~BIT_BOOT_BOD_RESET_HAPPEN;
	}	

	if (tmp_reason == 0) {
		return 0;
	}

	/* reset reason register, because it can not be reset by HW */
	reason_aon &= ~(BIT_AON_BOOT_EXIT_DSLP | BIT_AON_BOD_RESET_FLAG);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_BOOT_REASON1, reason_aon);
	BACKUP_REG->DWORD[0] &= ~BIT_MASK_BOOT_REASON;

	/* backup boot reason to REG_LP_SYSTEM_CFG2 */
	temp_bootcfg = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_SYSTEM_CFG2);
	temp_bootcfg |= tmp_reason;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_SYSTEM_CFG2, tmp_reason);
	
	return 0;
}

FLASH_BOOT_TEXT_SECTION
static void BOOT_FLASH_MMUConfig(u32 NewStatus)
{
	u8 idx = 0;
	u32 vaddr_start, vaddr_end, OTA2Offset;

	for(idx = 0; ; idx++) {
		vaddr_start = Flash_MMU_Config[idx].VAddrStart;
		vaddr_end = Flash_MMU_Config[idx].VAddrEnd;
		OTA2Offset = Flash_MMU_Config[idx].PAddrStart;
	
		if(vaddr_start == 0xFFFFFFFF || idx >= 6)
			break;

		if(OTA2Offset > vaddr_start)
			RSIP_MMU_Config(idx + 2, vaddr_start, vaddr_end, 0, OTA2Offset - vaddr_start);
		else
			RSIP_MMU_Config(idx + 2, vaddr_start, vaddr_end, 1, vaddr_start - OTA2Offset);
		
		RSIP_MMU_Cmd(idx + 2, NewStatus);
	}
}

FLASH_BOOT_TEXT_SECTION
void BOOT_FLASH_OTFMaskConfig(void)
{
	u32 addr, size;
	u8 i = 0;
	
	for(i = 0;; i++) {
		addr = RSIP_Mask_Config[i].MaskAddr;
		size = RSIP_Mask_Config[i].MaskSize;
	
		if(addr != 0xFFFFFFFF) {
			if(i >= 2) {
				DBG_PRINTF(MODULE_BOOT, LEVEL_ERROR,"There can be at most 2 RSIP mask entries!\n");
				break;
			}
				
			if((addr >= SPI_FLASH_BASE) && (addr <= 0x0FFFFFFF))
				RSIP_OTF_Mask(1 + i, addr, size, ENABLE);
			else
				DBG_PRINTF(MODULE_BOOT, LEVEL_ERROR,"RSIP entry addr %x invalid\n", addr);
		} else
			break;
	}
}

FLASH_BOOT_TEXT_SECTION
void BOOT_FLASH_fasttimer_init(void)
{
	RTIM_TimeBaseInitTypeDef TIM_InitStruct;

	RCC_PeriphClockCmd(APBPeriph_GTIMER, APBPeriph_GTIMER_CLOCK, ENABLE);
	RTIM_TimeBaseStructInit(&TIM_InitStruct);

	TIM_InitStruct.TIM_Idx = 5;
	TIM_InitStruct.TIM_Prescaler = 39; //40M/40 =1M->1us
	TIM_InitStruct.TIM_Period = 0xFFFF;

	TIM_InitStruct.TIM_UpdateEvent = ENABLE; /* UEV enable */
	TIM_InitStruct.TIM_UpdateSource = TIM_UpdateSource_Overflow;
	TIM_InitStruct.TIM_ARRProtection = ENABLE;

	RTIM_TimeBaseInit(TIMM05, &TIM_InitStruct, 0, (IRQ_FUN) NULL, (u32)NULL);
	RTIM_Cmd(TIMM05, ENABLE);
}

//flash_operation_config();
BOOT_RAM_TEXT_SECTION
void BOOT_FLASH_DSLP_FlashInit(void)
{
	RRAM_TypeDef* RRAM = ((RRAM_TypeDef *) RRAM_BASE);
	FLASH_STRUCT_INIT_FUNC FLASH_StructInitTemp = (FLASH_STRUCT_INIT_FUNC)RRAM->FLASH_StructInit;

	if (FLASH_StructInitTemp != NULL) {
		FLASH_StructInitTemp(&flash_init_para);
	} else {
		FLASH_StructInit(&flash_init_para);
	}

	/* SPIC clock switch to PLL */
	FLASH_ClockDiv(RRAM->FLASH_ClockDiv);

	flash_init_para.FLASH_QuadEn_bit = RRAM->FLASH_QuadEn_bit;
	flash_init_para.FLASH_cmd_wr_status2 = RRAM->FLASH_cmd_wr_status2;
	flash_init_para.FLASH_addr_phase_len = RRAM->FLASH_addr_phase_len;
	flash_init_para.FLASH_cur_cmd = RRAM->FLASH_cur_cmd;
	flash_init_para.FALSH_quad_valid_cmd = RRAM->FALSH_quad_valid_cmd;
	flash_init_para.FALSH_dual_valid_cmd = RRAM->FALSH_dual_valid_cmd;
	flash_init_para.FLASH_pseudo_prm_en = RRAM->FLASH_pseudo_prm_en;
	flash_init_para.FLASH_rd_dummy_cyle[0] = RRAM->FLASH_rd_dummy_cyle0;
	flash_init_para.FLASH_rd_dummy_cyle[1] = RRAM->FLASH_rd_dummy_cyle1;
	flash_init_para.FLASH_rd_dummy_cyle[2] = RRAM->FLASH_rd_dummy_cyle2;

	flash_init_para.phase_shift_idx = RRAM->FLASH_phase_shift_idx;		
	flash_init_para.FLASH_rd_sample_phase_cal = RRAM->FLASH_rd_sample_phase_cal;
	flash_init_para.FLASH_rd_sample_phase = RRAM->FLASH_rd_sample_phase_cal;

	FLASH_CalibrationInit(30);

	/* we should open calibration new first, and then set phase index */
	FLASH_CalibrationNewCmd(ENABLE);
	FLASH_CalibrationPhaseIdx(flash_init_para.phase_shift_idx);
	
	/* this code is rom code, so it is safe */
	FLASH_Init(RRAM->FLASH_cur_bitmode);
}

//3 Image 1
FLASH_BOOT_TEXT_SECTION
void BOOT_FLASH_Image1(void)
{
	IMAGE_HEADER *img2_hdr = NULL;
	IMAGE_HEADER *FlashImage2DataHdr = NULL;
	PRAM_START_FUNCTION Image2EntryFun = NULL;
	u32 BssLen = 0;
	u32 Temp = 0;
	u8* image2_validate_code = NULL;
	//PRAM_FUNCTION_START_TABLE pRamStartFun = (PRAM_FUNCTION_START_TABLE)__ram_start_table_start__;

	if (Boot_Log_En) {
		LOG_MASK_MODULE(MODULE_BOOT, LEVEL_INFO, ENABLE);		
	}
	LOG_MASK_MODULE(MODULE_BOOT, LEVEL_ERROR, ENABLE);

	/* loguart use 40MHz */
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_GPIO_SHDN33_CTRL, 0xFFFF);

	/* temp for test */
	BOOT_FLASH_fasttimer_init();

	/* set dslp boot reason */
	if (HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_BOOT_REASON1) & BIT_AON_BOOT_EXIT_DSLP) {
		/* open PLL */
		BOOT_ROM_CM4PON((u32)SYSPLL_ON_SEQ);
		BOOT_FLASH_DSLP_FlashInit();
	}

	/*Get Chip Version*/
	Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_BOOT_REASON1);
	Temp &= (~(BIT_MASK_AON_CHIP_VERSION_SW << BIT_SHIFT_AON_CHIP_VERSION_SW));
	if(0 != HAL_READ32(0, 0x1298)) {
		Temp |= (SYSCFG_CUT_VERSION_A << BIT_SHIFT_AON_CHIP_VERSION_SW);
	} else {
		Temp |= (SYSCFG_CUT_VERSION_B << BIT_SHIFT_AON_CHIP_VERSION_SW);
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_BOOT_REASON1, Temp);

	BOOT_FLASH_Reason_Set();

	/* for simulation set ASIC */
	if (ROM_SIM_ENABLE != 0x12345678) {
		BKUP_Set(BKUP_REG0, BIT_SW_SIM_RSVD);
	}
#ifdef CONFIG_FPGA
	IS_FPGA_VERIF = TRUE;

	/* for simulation set ASIC */
	if (BKUP_Read(BKUP_REG0) & BIT_SW_SIM_RSVD) {
		IS_FPGA_VERIF = FALSE;
	}
#endif

	/*if OTF encrypt is enabled, the system data space(4kb) and 
	system backup space(4kb) in flash should be masked*/
	if(HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG3) & BIT_SYS_FLASH_ENCRYPT_EN) {		
		BOOT_FLASH_OTFMaskConfig();
	}
	BOOT_FLASH_MMUConfig(ENABLE);

	BOOT_RAM_SectionInit();

	img2_hdr = (IMAGE_HEADER *)((__ls_flash_text_start__) - IMAGE_HEADER_LEN);
	Image2EntryFun = (PRAM_START_FUNCTION)__image2_entry_func__;
	image2_validate_code = (u8*)((u32)__image2_entry_func__ + sizeof(RAM_START_FUNCTION));
	BssLen = (__image1_bss_end__ - __image1_bss_start__);

	DBG_PRINTF(MODULE_BOOT, LEVEL_INFO,"IMG1 ENTER ROMSUB:%d\n", (SYSCFG_ROMINFO_Get() >> 8) & 0xFF);

	_memset((void *) __image1_bss_start__, 0, BssLen);

	/* backup flash_init_para address for KM4 */
	BKUP_Write(BKUP_REG7, (u32)&flash_init_para);

	BOOT_RAM_FuncEnable();

	if (OTASelectHook == NULL)
		Temp = BOOT_FLASH_OTA_Select();
	else
		Temp = OTASelectHook();
	
	if(Temp == 0) {
		DBG_PRINTF(MODULE_BOOT, LEVEL_ERROR,"IMG2 Invalid\n");
		goto INVALID_IMG2;
	}

	/* Download image 2 */
	FlashImage2DataHdr = (IMAGE_HEADER *)(__ls_flash_text_start__ + img2_hdr->image_size);

	DBG_PRINTF(MODULE_BOOT, LEVEL_INFO,"IMG2 DATA[0x%x:%d:0x%x]\n", (u32)(FlashImage2DataHdr + 1),
		FlashImage2DataHdr->image_size, FlashImage2DataHdr->image_addr);

	if (FlashImage2DataHdr->image_addr == 0xFFFFFFFF || FlashImage2DataHdr->image_size > 0x100000) {
		DBG_PRINTF(MODULE_BOOT, LEVEL_ERROR,"IMG2 ADDR Invalid\n");
		goto INVALID_IMG2;
	}

	/* load image2 data into RAM */
	_memcpy((void*)FlashImage2DataHdr->image_addr, (void*)(FlashImage2DataHdr + 1), FlashImage2DataHdr->image_size);
	
	/* Jump to image 2 */
	DBG_PRINTF(MODULE_BOOT, LEVEL_ERROR,"IMG2 SIGN[%s(%x)]\n",image2_validate_code, (u32)image2_validate_code);
	DBG_PRINTF(MODULE_BOOT, LEVEL_ERROR,"IMG2 ENTRY[0x%x:0x%x]\n",__image2_entry_func__, Image2EntryFun->RamStartFun);

	if (_strcmp((const char *)image2_validate_code, (const char *)"RTKWin")) {
		DBG_PRINTF(MODULE_BOOT, LEVEL_ERROR,"IMG2 SIGN Invalid\n");
		goto INVALID_IMG2;
	}

	/* goto IMG2 */
	Image2EntryFun->RamStartFun();
	
#if defined ( __ICCARM__ )
	// it is dummy code, but IAR linker need this
	__iar_data_init3();
#endif    

	return;

INVALID_IMG2:
	while (1) {
		shell_rom(1000);//each delay is 100us
	}
}
