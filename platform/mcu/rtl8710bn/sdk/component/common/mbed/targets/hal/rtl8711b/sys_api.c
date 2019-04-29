/** mbed Microcontroller Library
  ******************************************************************************
  * @file    sys_api.c
  * @author 
  * @version V1.0.0
  * @date    2016-08-01
  * @brief   This file provides following mbed system API:
  *				-JTAG OFF
  *				-LOGUART ON/OFF
  *				-OTA image switch
  *				-System Reset
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#include "cmsis.h"
#include "sys_api.h"
#include "flash_api.h"
#include "device_lock.h"

void rtc_backup_timeinfo(void);

#define OTA_Signature			"81958711"
#define OTA_Signature_len		8
#define OTA_valid_offset			0x100000
#define printf					DiagPrintf

/**
  * @brief  Turn off the JTAG function
  * @retval none  
  */
void sys_jtag_off(void)
{
	boot_export_symbol.swd_off();
}

/**
  * @brief  switch OTA image if the othe OTA image is valid
  * @retval none
  * @note for AmebaZ, sys_clear_ota_signature is the same with sys_recover_ota_signature
  */
void sys_clear_ota_signature(void)
{
	flash_t flash;
	u32 part1_offset = 0;
	u32 part2_offset = 0;
	u32 ota1_valid = 0;
	u32 ota2_valid = 0;
	u8 signature[OTA_Signature_len+1] = {0};

	part1_offset = (FLASH_OTA1_CODE_ADDR - 0x20);
	flash_stream_read(&flash, part1_offset, OTA_Signature_len, signature);
	if(!_memcmp((char const*)signature, OTA_Signature, OTA_Signature_len)){
		ota1_valid = 1;
	}

	printf("\n\rOTA ota1_signature = %s \n", signature);
	
	flash_stream_read(&flash, FLASH_SYSTEM_DATA_ADDR, 4, (u8*)&part2_offset);
	flash_stream_read(&flash, (part2_offset -SPI_FLASH_BASE) , OTA_Signature_len, signature);
	if(!_memcmp((char const*)signature, OTA_Signature, OTA_Signature_len)){
		ota2_valid = 1;
	}
	
	printf("\n\rOTA ota2_signature = %s \n", signature);
	
	printf("\n\rOTA ota1_valid = 0x%08X ota2_valid = 0x%08X \n", ota1_valid, ota2_valid);

	if ((ota1_valid == 1) && (ota2_valid == 1)) {
		u32 ValidIMG2 = HAL_READ32(SPI_FLASH_BASE, FLASH_SYSTEM_DATA_ADDR + 4);
		u32 BitIdx = 0;

		if (ValidIMG2 == 0x00000000) {
			FLASH_EreaseDwordsXIP((FLASH_SYSTEM_DATA_ADDR + 4), 1);
			ValidIMG2 = 0xFFFFFFFF;
		}
		
		for (BitIdx = 0; BitIdx <= 31; BitIdx++) {
			if ((ValidIMG2 & BIT(BitIdx)) != 0) {
				break;
			}
		}

		/* switch OAT image */
		ValidIMG2 &= ~BIT(BitIdx);

		/* write flash */
		FLASH_TxData12BXIP((FLASH_SYSTEM_DATA_ADDR + 4), 4, (u8*)&ValidIMG2);
	}
}

/**
  * @brief  switch OTA image if the othe OTA image is valid
  * @retval none
  * @note for AmebaZ, sys_clear_ota_signature is the same with sys_recover_ota_signature
  */
void sys_recover_ota_signature(void)
{
	sys_clear_ota_signature();
}

/**
  * @brief  open log uart
  * @retval none
  */
void sys_log_uart_on(void)
{
	/* 0: S1 PA29 & PA30, 1: S0 PA16 & PA17 */
	/* EFUSE default 0: S1 */
	if (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_SYSCFG6) & BIT_SYS_UART2_DEFAULT_GPIO) {
		PinCtrl(PERIPHERAL_LOG_UART, S0, ON);
	} else {
		PinCtrl(PERIPHERAL_LOG_UART, S1, ON);
	}

	UART_INTConfig(UART2_DEV, RUART_IER_ERBI | RUART_IER_ELSI, ENABLE);
	UART_RxCmd(UART2_DEV, ENABLE);
}

/**
  * @brief  close log uart
  * @retval none
  */
void sys_log_uart_off(void)
{
	UART_INTConfig(UART2_DEV, RUART_IER_ERBI | RUART_IER_ELSI, DISABLE);
	UART_RxCmd(UART2_DEV, DISABLE);

	/* 0: S1 PA29 & PA30, 1: S0 PA16 & PA17 */
	/* EFUSE default 0: S1 */
	if (HAL_READ32(SYSTEM_CTRL_BASE, REG_SYS_EFUSE_SYSCFG6) & BIT_SYS_UART2_DEFAULT_GPIO) {
		PinCtrl(PERIPHERAL_LOG_UART, S0, OFF);
	} else {
		PinCtrl(PERIPHERAL_LOG_UART, S1, OFF);
	}
}


/**
  * @brief  store or load adc calibration parameter 
  * @param  write:  this parameter can be one of the following values:
  *		@arg 0: load adc calibration parameter offset & gain from flash system data region
  *		@arg 1: store adc calibration parameter offset & gain to flash system data region
  * @param  offset: pointer to adc parameter offset
  * @param  gain: pointer to adc parameter gain
  * @retval none
  */
void sys_adc_calibration(u8 write, u16 *offset, u16 *gain)
{
	flash_t flash;
	u32 flash_data = 0;

	flash_data = (u32)*offset;
	flash_data |= (u32)((*gain) << 16);
		
	if(write){
		FLASH_EreaseDwordsXIP((FLASH_SYSTEM_DATA_ADDR + FLASH_ADC_PARA_OFFSET), 1);
		FLASH_TxData12BXIP((FLASH_SYSTEM_DATA_ADDR + FLASH_ADC_PARA_OFFSET), 4, (u8*)&flash_data);

		printf("\n\rStore ADC calibration success.");
	}

	flash_stream_read(&flash, (FLASH_SYSTEM_DATA_ADDR + FLASH_ADC_PARA_OFFSET), 4, (u8*)&flash_data);

	printf("\n\rADC offset = 0x%04X, gain = 0x%04X.\n\r", (flash_data & 0xFFFF), (flash_data & 0xFFFF0000) >> 16);
}

/**
  * @brief  system software reset
  * @retval none
  */
void sys_reset(void)
{
    while(1) {
        rtc_backup_timeinfo();

        /* Set processor clock to default(2: 31.25MHz) before system reset */
        HAL_WRITE32(SYSTEM_CTRL_BASE, REG_SYS_CLK_CTRL1, 0x00000021);
        DelayUs(100*1000);

        /*  Cortex-M3 SCB->AIRCR */
        SCB->AIRCR  = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) |       // VECTKEY
            (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |             // PRIGROUP
            SCB_AIRCR_SYSRESETREQ_Msk);                         // SYSRESETREQ
        DelayUs(100*1000);
    }
}


/**
  * @brief vector reset
  * @retval none
  */
void sys_cpu_reset(void)
{
	u32 reg_value;

	rtc_backup_timeinfo();

	reg_value = HAL_READ32(PERI_ON_BASE, REG_SOC_FUNC_EN);
	reg_value |= BIT_SOC_PATCH_FUNC1;
	HAL_WRITE32(PERI_ON_BASE, REG_SOC_FUNC_EN, reg_value);

	/* Ensure all outstanding memory accesses included */
	/* buffered write are completed before reset */
	__DSB();

	/* CPU reset */
	/* Keep priority group unchanged */
	SCB->AIRCR  = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) |
		(SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
		SCB_AIRCR_VECTRESET_Pos | SCB_AIRCR_VECTCLRACTIVE_Pos); 

	/* Ensure completion of memory access */
	__DSB();

	while(1);
}

/**
  * @brief  get rdp status
  * @retval :RDP valid status
  *          This parameter can be one of the following values:
  *            @arg 0: rdp valid
  *            @arg 1: system.bin not load to flash
  *            @arg 2: rdp.bin not load to flash
  *            @arg 3: Key request timeout
  *            @arg 4: RDP not enable in efuse
  *            @arg 5: Check sum error
  */
u32 sys_get_rdp_valid(void)
{
	return boot_export_symbol.rdp_valid();
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
