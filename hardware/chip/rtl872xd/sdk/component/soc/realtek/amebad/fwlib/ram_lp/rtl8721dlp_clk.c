/**
  ******************************************************************************
  * @file    rtl8721dlp_clk.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware rom patch functions to manage clk 
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#include "ameba_soc.h"

/**
  * @brief  backup XTAL cap value to dummy register for DSLP mode with RTC trigger calib
  * @param none
  * @retval none
  */
VOID XTAL_CAP_BKP(void)
{
	u32 sysreg;
	u32 dumy_reg;

	/*Get XTAL cap from 0x4800_0228[30:20]*/
	sysreg = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_SYS_EFUSE_SYSCFG2);
	sysreg = (sysreg >> 20) & 0x7FF;

	/*backup XTAL cap to dummy reg 0x4800_00C8[10:0]*/
	dumy_reg = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_XTAL_CTRL);
	dumy_reg &= ~0x7FF;
	dumy_reg |= sysreg;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_XTAL_CTRL, dumy_reg);
}

u32 SDM32K_Read(u32 Address)
{
	u32 Temp = 0;

	/* set indirect access address */
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_CK32KGEN_AD);
	Temp &=  (~BIT_LS_MASK_SDM_ADDR);
	Temp |= Address;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_CK32KGEN_AD, Temp);

	/* get indirect access val */
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_CK32KGEN_RWD);

	return Temp;
}

void SDM32K_Write(u32 Address, u32 Val)
{
	u32 Temp = 0;

	/* set indirect access address */
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_CK32KGEN_AD);
	Temp &=  (~BIT_LS_MASK_SDM_ADDR);
	Temp |= Address;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_CK32KGEN_AD, Temp);

	/* set indirect access val */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_CK32KGEN_RWD, Val);
}

/**
  * @brief  32K clock Enable,
  * @param  SDM32KCalType: can be one of the following values:
  *		 @arg SDM32K_ONE_CAL calibration once when sdm power on
  *		 @arg SDM32K_AUTO_CAL open auto calibration
  *		 @arg SDM32K_ALWAYS_CAL open always calibration
  * @note NA
  */
void SDM32K_Enable(u32 SDM32KCalType)
{
	u32 Temp = 0;
	
	/* SDM function clock enable & SDM function enable */
	RCC_PeriphClockCmd(APBPeriph_CK32KGEN, APBPeriph_CK32KGEN_CLOCK, ENABLE);

	Temp = SDM32K_Read(REG_SDM_CTRL0);
	
	/*clear SDM32K auto/always calibration*/
	Temp &= ~(BIT_AON_SDM_AUTO_CAL_EN | BIT_AON_SDM_ALWAYS_CAL_EN);
	
	/*default enable once calibration*/
	Temp |= (BIT_AON_SDM_FUNC_EN | BIT_AON_SDM_ONE_CAL_EN);

	if (SDM32KCalType == SDM32K_ONE_CAL) {
		//bit[31]: calibration one when first enable SDM; bit[30]: SDM enable
		SDM32K_Write(REG_SDM_CTRL0, Temp);
	} else if (SDM32KCalType == SDM32K_AUTO_CAL) {
		//write threshold
		SDM32K_Write(AUTO_CAL_MAX_ERROR, 0x80);

		// bit[16]:auto calibration enable
		Temp |= BIT_AON_SDM_AUTO_CAL_EN;		
		SDM32K_Write(REG_SDM_CTRL0, Temp);
	} else if (SDM32KCalType == SDM32K_ALWAYS_CAL) {
		//bit[18]: always calibration enable
		Temp |= BIT_AON_SDM_ALWAYS_CAL_EN;
		SDM32K_Write(REG_SDM_CTRL0, Temp);	
	}
}

/**
  * @brief  32K clock RTC auto calibration Enable,
  * @param  RTCCalibration: can be one of the following values:
  *		 @arg TRUE open RTC auto-calibration
  *		 @arg FALSE not open RTC auto-calibration
  * @note NA
  */
void SDM32K_RTCCalEnable(u32 RTCCalibration)
{
	u32 Temp = 0;

	Temp = SDM32K_Read(REG_SDM_CTRL0);
	Temp &= ~ BIT_AON_SDM_RTC_CAL_EN;

	if (!RTCCalibration) {
		SDM32K_Write(REG_SDM_CTRL0, Temp);
	} else {
		//bit[17]: RTC auto calibration enable
		Temp |= BIT_AON_SDM_RTC_CAL_EN;
		SDM32K_Write(REG_SDM_CTRL0, Temp);		

		/*backup XTAL cap to dummy register*/
		XTAL_CAP_BKP();
	}
}

/* setbit/clearbit is: BIT0~BIT5 defined in BIT_AON_MASK_OSC32K_RCAL_5_0 */
VOID OSC131_R_Set(u32 setbit, u32 clearbit)
{
	u32 temp = 0;

	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_OSC32K_CTRL);
	
	if (setbit) {
		temp |= (setbit << BIT_AON_SHIFT_OSC32K_RCAL_5_0);
	} 

	if (clearbit) {
		temp &= ~(clearbit << BIT_AON_SHIFT_OSC32K_RCAL_5_0);
	}
	
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_OSC32K_CTRL, temp);

	/* It takes 1ms to stable */
	DelayMs(1);
}

/**
  * @brief  131.072K calibration
  * @param  max_delta: target_40m_counter-calibration_result, default 10
  * @note Here XTAL clock is used to count N cycles of calibration clock, the total used XTAL clock cycles can be read by this register, N= 8
  */
u32 OSC131K_Calibration(u32 ppm_limit)
{
	u32 temp = 0;
	u32 cal_n = 0;
	u32 rsel_bit_index = 0;
	u32 delta = 0;
	u32 min_delta = 0xFFFFFFFF;
	u32 cur_ppm = 0;
	u32 min_delta_r = 0;
	u32 target_40m_counter = 2441; //8*40000000/131072
	u32 clearbit = 0;

	/* Step1: Enable LS_VENDOR_REG clock/reset: 0x4800_020C[0] = 0x1, 0x4800_0214[0] = 0x1 */
	RCC_PeriphClockCmd(APBPeriph_VENDOR_REG, APBPeriph_VENDOR_REG_CLOCK, ENABLE);

	/* Step2: Power on OSC131K: 0x4800_002C[0]= 0x1 */
	/* BOOT_ROM_OSC131_Enable have done this in rom, so here we check it */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_OSC32K_CTRL);
	temp &= ~(BIT_AON_MASK_OSC32K_RCAL_5_0 << BIT_AON_SHIFT_OSC32K_RCAL_5_0); // reset R, or second calibration may fail in some chip
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_OSC32K_CTRL, temp);
	if ((temp & BIT_AON_OSC32K_EN) == 0) {
		temp |= BIT_AON_OSC32K_EN;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_OSC32K_CTRL, temp);
	}
	while (1) {
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_SYSTEM_CFG1);
		if (temp & BIT_OSC128K_CLKRDY) {
			break;
		}
	}	

	/* Step2.1: Set OSC calibration parameter: 0x4800_2840[25:24], select 131K as calibration source */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_OSC_CAL);
	temp &= ~(BIT_MASK_CAL_CLK_SEL << BIT_SHIFT_CAL_CLK_SEL);
	temp |= (0x03 << BIT_SHIFT_CAL_CLK_SEL); /* select 131K */
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_OSC_CAL, temp);

	/* Step3: Enter the following loop: each loop decides one bit of 4800002c[13:8]-RCAL, i.e. the first loop decides RCAL[5], the second loop decides RCAL[4] бн, suppose the current loop is N (N=1..6) */
	/* Loop Step1: Set RCAL[6-N] = 0x1 */
	/* Loop Step2: Wait some time for clock stable (this wait time decide by SW) */
	/* Loop Step3: Enable Calibration: 0x4800_2840 = 0x8200_0000 */
	/* Loop Step2: Poll 0x4800_2840[31] = 0x0, read calibration result: 0x4800_2840[15:0] */
	/* Loop Step3: */
	/* If(frequency is in the allowed deviation range) */
	/* 	Calibration will end successfully */
	/* Else if (frequency counter > target frequency counter) */ /* (frequency > target frequency) */
	/* 	RCAL[6-N] = 0x0 */
	/* Else */
	/* 	RCAL[6-N] keep 1. */
	/* If (N=6) */
	/* 	Calibration will end, and if the last loop frequency is OK , then calibration is OK, otherwise calibration is fail */
	/* Else */
	/* 	Go to Loop step1 */
	for (cal_n = 1; cal_n <= 6; cal_n++) {
		rsel_bit_index = 6 - cal_n;
		OSC131_R_Set(BIT(rsel_bit_index), clearbit);
		
		/* start calibration */
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_OSC_CAL);
		temp |= BIT_CAL_START;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_OSC_CAL, temp);

		/* poll calibration complete */
		while (1) {
			temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_OSC_CAL);
			if ((temp & BIT_CAL_START) == 0) {
				break;
			}
		}

		/* read calibration result */
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_OSC_CAL) >> BIT_SHIFT_CAL_RPT;
		temp = temp & BIT_MASK_CAL_RPT;
		
		if (temp < target_40m_counter) {
			delta = target_40m_counter - temp;
			clearbit = BIT(rsel_bit_index);
		} else {
			delta = temp - target_40m_counter;
			clearbit = 0;
		}

		if (delta < min_delta) {
			min_delta = delta;
			min_delta_r = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_OSC32K_CTRL);
			min_delta_r &= (BIT_AON_MASK_OSC32K_RCAL_5_0 << BIT_AON_SHIFT_OSC32K_RCAL_5_0);
		}

		//DBG_8195A("[CAL131K]:cal_n %d delta:%d \n", cal_n, delta);
	}

	/* the last one is not the best one */
	if (delta > min_delta) {
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_OSC32K_CTRL);
		temp &= ~(BIT_AON_MASK_OSC32K_RCAL_5_0 << BIT_AON_SHIFT_OSC32K_RCAL_5_0);
		temp |= min_delta_r;		
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_OSC32K_CTRL, temp);

		/* It takes 1ms to stable */
		DelayMs(1);
	
		/* start calibration */
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_OSC_CAL);
		temp |= BIT_CAL_START;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_OSC_CAL, temp);

		/* poll calibration complete */
		while (1) {
			temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_OSC_CAL);
			if ((temp & BIT_CAL_START) == 0) {
				break;
			}
		}

		/* read calibration result */
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_OSC_CAL) >> BIT_SHIFT_CAL_RPT;
		temp = temp & BIT_MASK_CAL_RPT;

		if (temp < target_40m_counter) {
			delta = target_40m_counter - temp;
		} else {
			delta = temp - target_40m_counter;
		}
	}

	cur_ppm = delta*1000000/target_40m_counter;
	DBG_8195A("[CAL131K]: delta:%d target:%d PPM: %d PPM_Limit:%d \n", delta, target_40m_counter, cur_ppm, ppm_limit);

	if (cur_ppm >= ppm_limit) {
		DBG_8195A("[CAL131K]: PPM: %d PPM_Limit:%d \n", cur_ppm, ppm_limit);
		assert_param(0);
	}

	return TRUE;
}

/* bitnum is: 0~7 defined in BIT_MASK_OSC2M_R_SEL */
VOID OSC2M_R_Set(u32 setbit, u32 clearbit)
{
	u32 temp = 0;

	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_OSC2M_CTRL);

	if (setbit) 
		temp |= (setbit << BIT_SHIFT_OSC2M_R_SEL);
	
	if (clearbit) 
		temp &= ~(clearbit << BIT_SHIFT_OSC2M_R_SEL);

	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_OSC2M_CTRL, temp);
}

/**
  * @brief  OSC2M calibration
  * @param  cal_osc_cycles: can be one of the following values:
  *		 @arg OSC2M_CAL_CYC_16
  *		 @arg OSC2M_CAL_CYC_32
  *		 @arg OSC2M_CAL_CYC_64
  *		 @arg OSC2M_CAL_CYC_128
  * @param  max_delta: target_40m_counter-calibration_result, default 10
  * @note NA
  */
u32 OSC2M_Calibration(u32 cal_osc_cycles, u32 ppm_limit)
{
	u32 temp = 0;
	u32 cal_n = 0;
	u32 rsel_bit_index = 0;
	u32 delta = 0;
	u32 min_delta = 0xFFFFFFFF;
	u32 cur_ppm = 0;
	u32 min_delta_r = 0;
	u32 target_40m_counter = (16 << cal_osc_cycles) * (40/2); //2560
	u32 clearbit = 0;
	
	/* Step1: Enable LS_VENDOR_REG clock/reset: 0x4800_020C[0] = 0x1, 0x4800_0214[0] = 0x1 */
	RCC_PeriphClockCmd(APBPeriph_VENDOR_REG, APBPeriph_VENDOR_REG_CLOCK, ENABLE);

	/* Step2: Power on OSC2M: 0x4800_0298[0]= 0x1 */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_OSC2M_CTRL);
	temp |= BIT_OSC2M_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_OSC2M_CTRL, temp);

	/* Step3: Set OSC2M calibration parameter: 0x4800_2844[13:12]/ 0x4800_2844[11:8] */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_OSC2M_CAL);
	temp |= (0x03 << BIT_SHIFT_OSC2M_CAL_CYC_SEL); /* "Target cycles of OSC2M2'b00: 16 2'b01: 322'b10: 642'b11: 128" */
	temp |= (0xF << BIT_SHIFT_OSC2M_CAL_DLY); /* HW will wait (BIT_OSC2M_CAL_DLY+1) OSC2M cycles before start to count N cycles of OSC2M clock by XTAL */
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_OSC2M_CAL, temp);
	
	/* Step4: Enable OSC2M calibration function: 0x4800_2844[1]= 0x1 */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_OSC2M_CAL);
	temp |= BIT_OSC2M_CAL_FEN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_OSC2M_CAL, temp);

	/* clear 2m calibration parameter first */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_OSC2M_CTRL);
	temp &= ~(BIT_MASK_OSC2M_R_SEL << BIT_SHIFT_OSC2M_R_SEL);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_OSC2M_CTRL, temp);
		
	/* Step5: Enter the following loop: suppose the current loop is N (N=1..8) */	
	/* Loop Step1: Set OSC2M_R_SEL[8-N] = 0x1 */
	/* Loop Step2: Enable Calibration: 0x4800_2844 [0] = 0x1 */
	/* Loop Step2: Poll 0x4800_2844[0] = 0x0, read calibration result: 0x4800_2844[31:16] */
	/* Loop Step3: */
	/* If(frequency is in the allowed deviation range)  */
	/* 	Calibration will end successfully */
	/* Else if (frequency counter > target frequency counter) */ /* (frequency < target frequency) */
	/* 	OSC2M_R_SEL[8-N] = 0x0 */
	/* Else  */
	/* 	OSC2M_R_SEL[8-N] keep 1. */
	for (cal_n = 1; cal_n <= 8; cal_n++) {
		rsel_bit_index = 8 - cal_n;
		OSC2M_R_Set(BIT(rsel_bit_index), clearbit);

		/* start calibration */
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_OSC2M_CAL);
		temp |= BIT_OSC2M_CAL_START;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_OSC2M_CAL, temp);

		/* poll calibration complete */
		while (1) {
			temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_OSC2M_CAL);
			if ((temp & BIT_OSC2M_CAL_START) == 0) {
				break;
			}
		}

		/* read calibration result */
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_OSC2M_CAL) >> BIT_SHIFT_OSC2M_CAL_RPT;
		//DBG_8195A("OSC2M_Calibration:cal_n %d result:%d target:%d delta:%d\n", cal_n, temp, target_40m_counter, delta);
		
		if (temp <= target_40m_counter) {
			delta = target_40m_counter - temp;
			clearbit = 0;
		} else {
			delta = temp - target_40m_counter;
			clearbit = BIT(rsel_bit_index);
		}

		if (delta < min_delta) {
			min_delta = delta;
			min_delta_r = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_OSC2M_CTRL);
			min_delta_r &= (BIT_MASK_OSC2M_R_SEL << BIT_SHIFT_OSC2M_R_SEL);
		}
		//DBG_8195A("[CAL2M]:cal_n %d delta:%d \n", cal_n, delta);
	}

	/* the last one is not the best one */
	if (delta > min_delta) {
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_OSC2M_CTRL);
		temp &= ~(BIT_MASK_OSC2M_R_SEL << BIT_SHIFT_OSC2M_R_SEL);
		temp |= min_delta_r;		
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_OSC2M_CTRL, temp);

		/* start calibration */
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_OSC2M_CAL);
		temp |= BIT_OSC2M_CAL_START;
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_OSC2M_CAL, temp);

		/* poll calibration complete */
		while (1) {
			temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_OSC2M_CAL);
			if ((temp & BIT_OSC2M_CAL_START) == 0) {
				break;
			}
		}

		/* read calibration result */
		temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_OSC2M_CAL) >> BIT_SHIFT_OSC2M_CAL_RPT;
		//DBG_8195A("OSC2M_Calibration:cal_n %d result:%d target:%d delta:%d\n", cal_n, temp, target_40m_counter, delta);
		
		if (temp < target_40m_counter) {
			delta = target_40m_counter - temp;
		} else {
			delta = temp - target_40m_counter;
		}
	}

	cur_ppm = delta*1000000/target_40m_counter;
	DBG_8195A("[CAL2M]: delta:%d target:%d PPM: %d PPM_Limit:%d \n", delta, target_40m_counter, cur_ppm, ppm_limit);

	if (cur_ppm >= ppm_limit) {
		DBG_8195A("[CAL2M]: PPM: %d PPM_Limit:%d \n", cur_ppm, ppm_limit);
		assert_param(0);
	}
	
	return TRUE;
}

/**
  * @brief    Configure NCO 2M Clock
  * @param  clk_out_Hz: the NCO output clock(unit: Hz).
  * @note [14:0]: expected frequency of nco output clk, unit is 1KHz
  * @note [16]: function enable
  * @retval   none
  */
void NCO2M_Init(u32 clk_out_Hz)
{
	u32 Temp;
	u32 clk_out = clk_out_Hz / 1000;

	/* Check the parameters */
	assert_param((clk_out > 0 && clk_out <= 0x7FFF));

	/*disable the NCO clock and configure NCO output  clock*/
	Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_UART_XTAL_DIV_CTRL);

	Temp &= (~(BIT_LSYS_UART_XTAL_DIV_EN |BIT_LSYS_MASK_UART_XTAL_DIV_FREQ));
	
	Temp |= clk_out;

	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_UART_XTAL_DIV_CTRL, Temp);
}

/**
  * @brief    enable or disable the NCO2M Clock
  * @param  NewState: the new state of the NCO 2M clock.
  *              This parameter can be: ENABLE or DISABLE.
  * @retval   none
  */
void NCO2M_Cmd(u32 NewState)
{
	u32 Temp;

	Temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_UART_XTAL_DIV_CTRL);	

	if(NewState != DISABLE) {
		/*enable XTAL division circuit for UART*/
		Temp |= BIT_LSYS_UART_XTAL_DIV_EN;
	} else {
		/*disable XTAL division circuit for UART*/
		Temp &= (~BIT_LSYS_UART_XTAL_DIV_EN);		
	}

	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_UART_XTAL_DIV_CTRL, Temp);	
}

/**
  * @brief  OSC4M Init
  * @param  NA
  * @note NA
  */
void OSC4M_Init(void)
{
	u32 temp = 0;

	/* Enable OSC4M wake up */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LP_OSC4M_CTRL);
	temp |= BIT_OSC4M_WAKE_EN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LP_OSC4M_CTRL, temp);
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
