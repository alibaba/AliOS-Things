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
extern u8 IMG3_Exist;
extern void INT_HardFault_C(uint32_t mstack[], uint32_t pstack[], uint32_t lr_value, uint32_t fault_id);

/* Generate BLXNS instruction */
static __inline void BOOT_RAM_NsStart (u32 Addr)
{
#if defined( __ICCARM__ )
	__ASM volatile ("BLXNS   r0");		
#else
	__ASM volatile ("MOV r0, %0\n\t"
		"BLXNS   r0\n\t" : : "r" (Addr));
#endif
}

/* open some always on functions in this function */
BOOT_RAM_TEXT_SECTION
void BOOT_FLASH_FuncEnable(void)
{
	//RCC_PeriphClockCmd(APBPeriph_PSRAM, APBPeriph_PSRAM_CLOCK, ENABLE);//move into psram init, because 200ua leakage
	//RCC_PeriphClockCmd(APBPeriph_AUDIOC, APBPeriph_AUDIOC_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_VENDOR_REG, APBPeriph_VENDOR_REG_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_USI_REG, APBPeriph_USI_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_IRDA_REG, APBPeriph_IRDA_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_IPC, APBPeriph_IPC_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_GTIMER, APBPeriph_GTIMER_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_SPI1, APBPeriph_SPI1_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_UART1, APBPeriph_UART1_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_UART0, APBPeriph_UART0_CLOCK, ENABLE);

	/* close BT temp for simulation */
	//RCC_PeriphClockCmd(APBPeriph_BT, APBPeriph_BT_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_WL, APBPeriph_WL_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_GDMA0, APBPeriph_GDMA0_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_LCDC, APBPeriph_LCDC_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_I2S0, APBPeriph_I2S0_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_SECURITY_ENGINE, APBPeriph_SEC_ENG_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_LXBUS, APBPeriph_LXBUS_CLOCK, ENABLE);

	//RCC_PeriphClockCmd(APBPeriph_SPORT, APBPeriph_SPORT_CLOCK, ENABLE);
	//RCC_PeriphClockCmd(APBPeriph_OTG, APBPeriph_OTG_CLOCK, ENABLE);//move into otg init, because 150ua leakage
	//RCC_PeriphClockCmd(APBPeriph_SDIOH, APBPeriph_SDIOH_CLOCK, ENABLE);//move into sd init, because 600ua leakage
	RCC_PeriphClockCmd(APBPeriph_SDIOD, APBPeriph_SDIOD_CLOCK, ENABLE);


}

BOOT_RAM_TEXT_SECTION
static void BOOT_FLASH_RDP_Checksum(u32 len, u8* buf, u8* sum)
{
	u32 i = 0, j = 0;
	
	for (i = 0; i < len; i = i + 4){
		for (j = 0; j < 4; j++)
			sum[j] = (u8)(sum[j] ^ buf[i + j]);
	}
}

BOOT_RAM_TEXT_SECTION
void BOOT_FLASH_RDP_ImageDecrypt(u32 img_addr, u32* plen, u8* checksum)
{
	/* image2 bss temp, because it is bootloader, finaly img2 bss will be reset */
	u8* buf = (u8*)__image2_entry_func__;
	u32 dst_addr, src_addr, left_size, t_size = 0;
	IMAGE_HEADER *ImgHdr;
	u32 OTF_Enable = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG3) & BIT_SYS_FLASH_ENCRYPT_EN;

	/* RSIP mask to prevent IMG3 header from being decrypted. If customers use RDP+RSIP function, IMG3 only need
		RDP encryption, and don't need RSIP encryption. */
	if(OTF_Enable)
		RSIP_OTF_Mask(3, img_addr, 1, ENABLE);

	/* Decrypt Image Header */
	_memcpy(buf, (void*)img_addr, IMAGE_HEADER_LEN);
	rtl_crypto_aes_ecb_decrypt(buf,  IMAGE_HEADER_LEN , NULL, 0, buf + IMAGE_HEADER_LEN); 

	if(checksum != NULL)
		BOOT_FLASH_RDP_Checksum(IMAGE_HEADER_LEN, buf + IMAGE_HEADER_LEN, checksum);

	src_addr = img_addr + IMAGE_HEADER_LEN;
	ImgHdr = (IMAGE_HEADER *)(buf + IMAGE_HEADER_LEN);
	dst_addr = ImgHdr->image_addr;
	left_size = ImgHdr->image_size;

	/* Decrypt size should be 16-byte aligned */
	if((ImgHdr->signature[0] != 0x35393138) || (ImgHdr->signature[1] != 0x31313738)) {
		DBG_PRINTF(MODULE_BOOT, LEVEL_ERROR,"RDP signature error!\n");
		goto fail;
	}

	if((ImgHdr->image_size & 0xF) != 0) {
		DBG_PRINTF(MODULE_BOOT, LEVEL_ERROR,"RDP len should be 16byte-aligned\n");
		goto fail;
	}
	
	if( plen!= NULL)
		*plen = left_size;

	/* RSIP mask the whole image to prevent IMG3 Code/Data from being decrypted. Mask one more section for 
		checksum reading corner case. */
	if(OTF_Enable)
		RSIP_OTF_Mask(3, img_addr, ((IMAGE_HEADER_LEN + left_size - 1) >> 12) + 2, ENABLE);
	
	/* Decrypt Secure Image and DMA to Secure RAM Region*/
	do{
		t_size = (left_size > 4096) ? 4096 : left_size;
		_memcpy(buf, (u8*) src_addr, t_size);
		rtl_crypto_aes_ecb_decrypt(buf, t_size, NULL, 0, (u8*)dst_addr);

		if(checksum != NULL)
			BOOT_FLASH_RDP_Checksum(t_size, (u8*)dst_addr, checksum);

		left_size -= t_size;
		src_addr += t_size;
		dst_addr += t_size;
	}while(left_size > 0);

	return;
fail:
	while(1);
}

BOOT_RAM_TEXT_SECTION
void BOOT_FLASH_RDP(u32 ImageAddr)
{
	u32 OTF_Enable = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG3) & BIT_SYS_FLASH_ENCRYPT_EN;
	IMAGE_HEADER *Img3SecureHdr = (IMAGE_HEADER *)ImageAddr;
	IMAGE_HEADER *Img3NSCHdr = NULL;
	u32 chksum_rdp_flash, checksum_ipsec = 0, tmp;
	u32 rdp_len = 0;
	u8 key[16];

	/* Read key from Efuse 0x170~0x17F */ 
	for (tmp = 0; tmp < 16; tmp++) 
		EFUSERead8(HAL_READ32(SYSTEM_CTRL_BASE, REG_HS_EFUSE_CTRL1_S),
			EFUSE_RDP_KEY_ADDR + tmp, &key[tmp], L25EOUTVOLTAGE);
	
	/* Initialize IPSEC */
	rtl_cryptoEngine_init();
	/* Load key to IPSEC */
	rtl_crypto_aes_ecb_init(key, 16);
	
	/* Decrypt Secure Image and DMA to Secure Region*/
	BOOT_FLASH_RDP_ImageDecrypt((u32)Img3SecureHdr, &tmp, (u8*)&checksum_ipsec);
	rdp_len += IMAGE_HEADER_LEN + tmp;

	/* Decrypt NSC Image and DMA to NSC Region*/
	Img3NSCHdr = (IMAGE_HEADER *)((u32)Img3SecureHdr + IMAGE_HEADER_LEN + tmp);
	BOOT_FLASH_RDP_ImageDecrypt((u32)Img3NSCHdr, &tmp, (u8*)&checksum_ipsec);
	rdp_len += IMAGE_HEADER_LEN + tmp;

	rdp_len = ((((rdp_len - 1) >> 4) + 1) << 4);

	/* Check checksum*/
	chksum_rdp_flash = HAL_READ32((u32)Img3SecureHdr, rdp_len);
	if(chksum_rdp_flash == checksum_ipsec){
		DBG_PRINTF(MODULE_BOOT, LEVEL_INFO,"RDP bin decryption OK!\n");
	}else{
		DBG_PRINTF(MODULE_BOOT, LEVEL_ERROR,"RDP bin decryption Failed!\n");
		DBG_PRINTF(MODULE_BOOT, LEVEL_ERROR,"checksum_ipsec = 0x%x, checksum_rdp_flash = 0x%x\n", checksum_ipsec, chksum_rdp_flash);
	}

	/* Relase temporary-used RSIP Mask entry */
	if(OTF_Enable)
		RSIP_OTF_Mask(3, 0, 0, DISABLE);
}

BOOT_RAM_TEXT_SECTION
void BOOT_FLASH_Image3Load(void)
{
	u32 AddrEnd, AddrOffset, IsMinus, PhyAddr;
	u32 value = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_BOOT_CFG) | BIT_BOOT_IMG3_EXIST;
	RSIP_REG_TypeDef* RSIP = ((RSIP_REG_TypeDef *) RSIP_REG_BASE);
	IMAGE_HEADER *Img3SecureHdr;
	IMAGE_HEADER *Img3NSCHdr;

	/* Get IMG3 physical address, this image is not included in MMU entry */
	AddrEnd = RSIP->FLASH_MMU[1].MMU_ENTRYx_ENDADDR;
	AddrOffset = RSIP->FLASH_MMU[1].MMU_ENTRYx_OFFSET;
	IsMinus = (RSIP->FLASH_MMU[1].MMU_ENTRYx_CTRL) & MMU_BIT_ENTRY_OFFSET_MINUS;

	PhyAddr = ((((AddrEnd -1) >> 12) + 1) << 12);
	if(IsMinus)
		PhyAddr -= AddrOffset;
	else
		PhyAddr += AddrOffset;
	
	Img3SecureHdr = (IMAGE_HEADER *)PhyAddr;

	if(HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG3) & BIT_SYS_RDP_EN) {
		/* RDP ENABLE */
		DBG_PRINTF(MODULE_BOOT, LEVEL_INFO,"RDP EN\n");
		BOOT_FLASH_RDP((u32)Img3SecureHdr);
	}else{
		/* RDP is not enable, but IMG3 is appended to the tail of IMG2. This mode is for security verification, uses don't use. */
		if((Img3SecureHdr->signature[0] == 0x35393138) && (Img3SecureHdr->signature[1] == 0x31313738)) {			
			Img3NSCHdr = (IMAGE_HEADER *)((u32)Img3SecureHdr + IMAGE_HEADER_LEN + (Img3SecureHdr->image_size));

			DBG_PRINTF(MODULE_BOOT, LEVEL_INFO,"IMG3:[%x code_size:%d flashaddr:%x]\n", Img3SecureHdr->image_addr, Img3SecureHdr->image_size, (void*)(Img3SecureHdr+1));
			DBG_PRINTF(MODULE_BOOT, LEVEL_INFO,"IMG3:[%x nsc_size:%d flashaddr:%x]\n", Img3NSCHdr->image_addr, Img3NSCHdr->image_size, (void*)(Img3NSCHdr+1));

			_memcpy((void*)Img3SecureHdr->image_addr, (void*)(Img3SecureHdr+1), Img3SecureHdr->image_size);
			_memcpy((void*)Img3NSCHdr->image_addr, (void*)(Img3NSCHdr+1), Img3NSCHdr->image_size);
		} else {
			value &= ~BIT_BOOT_IMG3_EXIST;
		}
	}

	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_BOOT_CFG, value);
}

/**
  * @brief  Get boot reason from AON & SYSON register, and back up to REG_LP_SYSTEM_CFG2
  * @param  NA
  * @retval 0.
  * @note this function is called once by bootloader when KM0 boot to flash, user can not use it.
  */
BOOT_RAM_TEXT_SECTION
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
	reason_aon &= ~(BIT_AON_BOOT_EXIT_DSLP | BIT_BOOT_BOD_RESET_HAPPEN);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_BOOT_REASON1, reason_aon);
	BACKUP_REG->DWORD[0] &= ~BIT_MASK_BOOT_REASON;

	/* backup boot reason to REG_LP_SYSTEM_CFG2 */
	temp_bootcfg = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_SYSTEM_CFG2);
	temp_bootcfg |= tmp_reason;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_SYSTEM_CFG2, tmp_reason);
	
	return 0;
}

BOOT_RAM_TEXT_SECTION
void BOOT_FLASH_WakeFromPG(void)
{
	PRAM_START_FUNCTION Image2EntryFun = NULL;
	volatile uint32_t NonSecure_ResetHandler;
	u32 *vector_table = NULL;
	//cmse_address_info_t cmse_address_info = cmse_TT((void *)DiagPrintf);
		
	Image2EntryFun = (PRAM_START_FUNCTION)__image2_entry_func__;

	/* Config Non-Security World Registers */
	BOOT_RAM_TZCfg();

	vector_table = (u32 *)Image2EntryFun->VectorNS;
	vector_table[1] = (u32)Image2EntryFun->RamWakeupFun;
	SCB_NS->VTOR = (u32)vector_table;
	SCB->NSACR |= BIT(10) | BIT(11); // enable non-secure to access VFP
	/* Set non-secure main stack (MSP_NS) */
	__TZ_set_MSP_NS(MSP_RAM_HP_NS); 

	/* Set PSPS Temp */
	__set_PSP(MSP_RAM_HP_NS-2048); 

	/* Set non-secure main stack (MSP_NS) */
	//DBG_8195A ("NonSecure Start @ 0x%x MSP_NS:%x VCTOR:%x\r\n", vector_table[1], vector_table[0], vector_table);
#if defined (configUSE_SAU) && (configUSE_SAU == 1U)
	NonSecure_ResetHandler = cmse_nsfptr_create(vector_table[1]);
#else
	NonSecure_ResetHandler = vector_table[1];
#endif
	/* Start non-secure state software application */
	//DBG_8195A("Start NonSecure @ 0x%x ...\r\n", NonSecure_ResetHandler);
	//DBG_8195A("Cache Enable:%x\r\n", DCache_IsEnabled());
	BOOT_RAM_NsStart(NonSecure_ResetHandler);

	return;
}

BOOT_RAM_TEXT_SECTION
void INT_HardFault_Patch_C(uint32_t mstack[], uint32_t pstack[], uint32_t lr_value, uint32_t fault_id)
{
	DBG_8195A("\r\nHard Fault Patch\r\n");
	
	INT_HardFault_C(mstack, pstack, lr_value, fault_id);
}

BOOT_RAM_TEXT_SECTION
VOID
INT_HardFault_Patch(VOID)
{
	__ASM volatile(
		"MRS R0, MSP\n\t"
		"MRS R1, PSP\n\t"
		"MOV R2, LR\n\t" /* second parameter is LR current value */
		"MOV R3, #0\n\t"        
		"SUB.W	R4, R0, #128\n\t"
		"MSR MSP, R4\n\t"
		"LDR R4,=INT_HardFault_Patch_C\n\t"
		"BX R4\n\t"
	);
}

BOOT_RAM_TEXT_SECTION
VOID
INT_UsageFault_Patch(VOID)
{
	__ASM volatile(
		"MRS R0, MSP\n\t"
		"MRS R1, PSP\n\t"
		"MOV R2, LR\n\t" /* second parameter is LR current value */
		"MOV R3, #1\n\t"        
		"SUB.W	R4, R0, #128\n\t"
		"MSR MSP, R4\n\t"
		"LDR R4,=INT_HardFault_Patch_C\n\t"
		"BX R4\n\t"
	);
}

BOOT_RAM_TEXT_SECTION
VOID
INT_BusFault_Patch(VOID)
{
	__ASM volatile(
		"MRS R0, MSP\n\t"
		"MRS R1, PSP\n\t"
		"MOV R2, LR\n\t" /* second parameter is LR current value */
		"MOV R3, #2\n\t"        
		"SUB.W	R4, R0, #128\n\t"
		"MSR MSP, R4\n\t"
		"LDR R4,=INT_HardFault_Patch_C\n\t"
		"BX R4\n\t"
	);
}

BOOT_RAM_TEXT_SECTION
VOID
INT_MemFault_Patch(VOID)
{
	__ASM volatile(
		"MRS R0, MSP\n\t"
		"MRS R1, PSP\n\t"
		"MOV R2, LR\n\t" /* second parameter is LR current value */
		"MOV R3, #3\n\t"        
		"SUB.W	R4, R0, #128\n\t"
		"MSR MSP, R4\n\t"
		"LDR R4,=INT_HardFault_Patch_C\n\t"
		"BX R4\n\t"
	);
}

BOOT_RAM_TEXT_SECTION
VOID
INT_SecureFault_Patch(VOID)
{
	__ASM volatile(
		"MRS R0, MSP\n\t"
		"MRS R1, PSP\n\t"
		"MOV R2, LR\n\t" /* second parameter is LR current value */
		"MOV R3, #4\n\t"        
		"SUB.W	R4, R0, #128\n\t"
		"MSR MSP, R4\n\t"
		"LDR R4,=INT_HardFault_Patch_C\n\t"
		"BX R4\n\t"
	);
}

BOOT_RAM_TEXT_SECTION
VOID VectorTableOverride(VOID)
{
#if 0
	u32 *vector_table_s = SCB->VTOR;
	vector_table_s[3] = (u32) INT_HardFault_Patch;
	vector_table_s[4] = (u32) INT_MemFault_Patch;
	vector_table_s[5] = (u32) INT_BusFault_Patch;
	vector_table_s[6] = (u32) INT_UsageFault_Patch;
	vector_table_s[7] = (u32) INT_SecureFault_Patch;
#endif
}

//3 Image 1
BOOT_RAM_TEXT_SECTION
void BOOT_FLASH_Image1(void)
{
	IMAGE_HEADER *img2_hdr = NULL;
	IMAGE_HEADER *FlashImage2DataHdr = NULL;
	PRAM_START_FUNCTION Image2EntryFun = NULL;
	u32 seed;
	u8 *image2_validate_code = NULL;
	volatile uint32_t NonSecure_ResetHandler;
	u32 *vector_table = NULL;
	//cmse_address_info_t cmse_address_info = cmse_TT((void *)DiagPrintf);

	/* Secure Vector table init */
	irq_table_init(0x1007FFFC);
	__set_MSP(0x1007FFFC);

	if (Boot_Log_En) {
		LOG_MASK_MODULE(MODULE_BOOT, LEVEL_INFO, ENABLE);
	}
	LOG_MASK_MODULE(MODULE_BOOT, LEVEL_ERROR, ENABLE);

	//BOOT_FLASH_Reason_Set();
	
#ifdef CONFIG_FPGA
	IS_FPGA_VERIF = TRUE;

	/* for simulation set ASIC */
	if (BKUP_Read(BKUP_REG0) & BIT_SW_SIM_RSVD) {
		IS_FPGA_VERIF = FALSE;
	}
#endif

	DBG_PRINTF(MODULE_BOOT, LEVEL_INFO,"IMG1 ENTER MSP:[%08x]\n", __get_MSP());
	//DBG_PRINTF(MODULE_BOOT, LEVEL_INFO,"TT_CMSE:[%08x:%08x] \n", cmse_address_info.value, cmse_address_info.flags.secure);
	
	BOOT_RAM_SectionInit();

	_memcpy((void *)&flash_init_para, (const void *)BKUP_Read(BKUP_REG7), sizeof(FLASH_InitTypeDef));

	/* Get random seed from KM0 */
	seed = BKUP_Read(BKUP_REG6);
	rand_seed[0] = seed;
	rand_seed[1] = seed;
	rand_seed[2] = seed;
	rand_seed[3] = seed;
	rand_first = 1;
		
	img2_hdr = (IMAGE_HEADER *)((__flash_text_start__) - IMAGE_HEADER_LEN);
	Image2EntryFun = (PRAM_START_FUNCTION)__image2_entry_func__;
	image2_validate_code = (u8*)((u32)__image2_entry_func__ + sizeof(RAM_START_FUNCTION));

	_memset((void *) __image1_bss_start__, 0, (__image1_bss_end__ - __image1_bss_start__));

	BOOT_FLASH_FuncEnable();

	/* load image3 before image2, because it will use __image2_entry_func__ temp */
	BOOT_FLASH_Image3Load();

	if (img2_hdr->image_size == 0xFFFFFFFF) {
		DBG_PRINTF(MODULE_BOOT, LEVEL_ERROR,"IMG2 SIZE Invalid\n");
		goto INVALID_IMG2;	
	}

	FlashImage2DataHdr = (IMAGE_HEADER *)(__flash_text_start__ + img2_hdr->image_size);
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

	/* Enable SecureFault, UsageFault, MemManageFault, BusFault */
	SCB->SHCSR |= SCB_SHCSR_SECUREFAULTENA_Msk | SCB_SHCSR_USGFAULTENA_Msk | \
					SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk;

	SCB_NS->SHCSR |=  SCB_SHCSR_USGFAULTENA_Msk | \
					SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk;

	VectorTableOverride();

	/* Config Non-Security World Registers */
	BOOT_RAM_TZCfg();

	vector_table = (u32 *)Image2EntryFun->VectorNS;
	vector_table[1] = (u32)Image2EntryFun->RamStartFun;
	SCB_NS->VTOR = (u32)vector_table;
	SCB->NSACR |= BIT(10) | BIT(11); // enable non-secure to access VFP
	/* Set non-secure main stack (MSP_NS) */
	__TZ_set_MSP_NS(MSP_RAM_HP_NS); 

	/* Set PSPS Temp */
	__set_PSP(MSP_RAM_HP_NS-2048); 

	/* Set non-secure main stack (MSP_NS) */
	//DBG_8195A ("NonSecure Start @ 0x%x MSP_NS:%x VCTOR:%x\r\n", vector_table[1], vector_table[0], vector_table);
#if defined (configUSE_SAU) && (configUSE_SAU == 1U)
	NonSecure_ResetHandler = cmse_nsfptr_create(vector_table[1]);
#else
	NonSecure_ResetHandler = vector_table[1];
#endif

	/* Start non-secure state software application */
	DBG_PRINTF(MODULE_BOOT, LEVEL_INFO,"Start NonSecure @ 0x%x ...\r\n", NonSecure_ResetHandler);
	//DBG_8195A("Cache Enable:%x\r\n", DCache_IsEnabled());
	BOOT_RAM_NsStart(NonSecure_ResetHandler);

#if defined ( __ICCARM__ )
	// it is dummy code, but IAR linker need this
	__iar_data_init3();
#endif

	return;

INVALID_IMG2:
	while (1) {
		DelayMs(1000);//each delay is 100us
	}
}
