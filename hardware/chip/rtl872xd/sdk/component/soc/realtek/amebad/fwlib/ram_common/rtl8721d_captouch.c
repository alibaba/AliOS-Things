/**
  ******************************************************************************
  * @file    rtl8721dlp_captouch.c
  * @author
  * @version V1.0.0
  * @date    2017-11-06
  * @brief   This file contains all the functions prototypes for the CapTouch function
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
#include "rtl8721d_captouch.h"

/**
  * @brief  Fills each CapTouch_InitStruct member with its default value.
  * @param  CapTouch_InitStruct: pointer to an CapTouch_InitTypeDef structure which will be initialized.
  * @retval None
  */
void CapTouch_StructInit(CapTouch_InitTypeDef* CapTouch_InitStruct)
{
	u8 i;
	/* Load HAL initial data structure default value */
	CapTouch_InitStruct->CT_DebounceEn = 1;
	CapTouch_InitStruct->CT_SampleCnt = 6;
	CapTouch_InitStruct->CT_ScanInterval = 60;
	CapTouch_InitStruct->CT_ETCStep = 1;
	CapTouch_InitStruct->CT_ETCFactor = 4;
	CapTouch_InitStruct->CT_ETCScanInterval = 3;

	for(i = 0; i < CT_CHANNEL_NUM; i++) {
		CapTouch_InitStruct->CT_Channel[i].CT_CHEnable = DISABLE;
		CapTouch_InitStruct->CT_Channel[i].CT_DiffThrehold = 0xf0;
		CapTouch_InitStruct->CT_Channel[i].CT_MbiasCurrent = 0x11; 		//6uA
		CapTouch_InitStruct->CT_Channel[i].CT_ETCNNoiseThr = 100;
		CapTouch_InitStruct->CT_Channel[i].CT_ETCPNoiseThr = 100;
	}
}


/**
  * @brief  Initializes the CapTouch peripheral according to the specified 
  *			parameters in the CapTouch_InitStruct.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  CapTouch_InitStruct: pointer to a CapTouch_InitTypeDef structure that contains
  * 		the configuration information for the specified CapTouch peripheral.
  * @retval None
  */
void CapTouch_Init(CAPTOUCH_TypeDef *CapTouch, CapTouch_InitTypeDef* CapTouch_InitStruct)
{
	u8 i;
	
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(CapTouch_InitStruct->CT_DebounceEn <= 1 );
	assert_param(CapTouch_InitStruct->CT_SampleCnt<= 7 );
	assert_param(CapTouch_InitStruct->CT_ScanInterval <= 0xFFF );
	assert_param(CapTouch_InitStruct->CT_ETCStep <= 0xF );
	assert_param(CapTouch_InitStruct->CT_ETCFactor <= 0xF );
	assert_param(CapTouch_InitStruct->CT_ETCScanInterval <= 0x7F );
	
	for(i = 0; i < CT_CHANNEL_NUM ;i++) {
		assert_param(CapTouch_InitStruct->CT_Channel[i].CT_DiffThrehold  <= 0xFFF );
		assert_param(CapTouch_InitStruct->CT_Channel[i].CT_MbiasCurrent <= 0x3F );
		//assert_param(CapTouch_InitStruct->CT_Channel[i].CT_ETCNNoiseThr <= 0xFF );
		//assert_param(CapTouch_InitStruct->CT_Channel[i].CT_ETCPNoiseThr <= 0xFF );
	}

	/* Disable CTC, clear pending interrupt*/
	CapTouch_Cmd(CapTouch, DISABLE);
	CapTouch->CT_IER = 0;
	CapTouch->CT_ICR_ALL = BIT_CT_ALL_INT_CLR;

	/* Set control register*/
	CapTouch->CT_CTRL = BIT_CT_BL_ENABLE | (CapTouch_InitStruct->CT_DebounceEn << 4);
	
	CapTouch->CT_SP_CTRL = CapTouch_InitStruct->CT_SampleCnt << 16 | \
							CapTouch_InitStruct->CT_ScanInterval;
	
	CapTouch->CT_ETC_CTRL = CapTouch_InitStruct->CT_ETCStep << 12 | \
							CapTouch_InitStruct->CT_ETCFactor << 8 | \
							CapTouch_InitStruct->CT_ETCScanInterval << 1 | \
							BIT_CT_ETC_ENABLE;
	
	CapTouch->CT_MODE_CTRL = BIT_CT_AUTO_CHANNEL_ENABLE;

	/* Configure each channel */
	for(i = 0; i < CT_CHANNEL_NUM ;i++) {
		if(CapTouch_InitStruct->CT_Channel[i].CT_CHEnable) {
			CapTouch->CT_CH[i].CTRL = (CapTouch_InitStruct->CT_Channel[i].CT_DiffThrehold <<16) | BIT_CT_CHX_ENABLE;
			CapTouch->CT_CH[i].MBIAS = CapTouch_InitStruct->CT_Channel[i].CT_MbiasCurrent;
			CapTouch->CT_CH[i].ATHR = CapTouch_InitStruct->CT_Channel[i].CT_ETCNNoiseThr << 24 | \
									CapTouch_InitStruct->CT_Channel[i].CT_ETCPNoiseThr << 16;
		}
	}
}


/**
  * @brief  Enables or disables the specified CapTouch peripheral.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  NewState: new state of the CapTouch peripheral. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CapTouch_Cmd(CAPTOUCH_TypeDef *CapTouch, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	if (NewState != DISABLE) {
		/* Enable the CapTouch peripheral and baseline */
		CapTouch->CT_CTRL |= BIT_CT_ENABLE | BIT_CT_BL_ENABLE;
	} else {
		/* Disable the CapTouch peripheral */
		CapTouch->CT_CTRL &= (~BIT_CT_ENABLE);
	}
}


/**
  * @brief  Enables or disables the specified CapTouch interrupts.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  CapTouch_IT: specifies the CapTouch interrupt to be enabled or masked.
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_CT_OVER_N_NOISE_THRESHOLD_INT: CapTouch negetive noise overflow interrupt
  *            @arg BIT_CT_FIFO_OVERFLOW_INT: CapTouch FIFO overflow interrupt
  *            @arg BIT_CT_OVER_P_NOISE_THRESHOLD_INT:CapTouch positive noise overflow interrupt
  *            @arg CT_CHX_PRESS_INT(x): CapTouch channel(x) press interrupt, where x can be 0~3
  *            @arg CT_CHX_RELEASE_INT(x): CapTouch channel(x) release interrupt, where x can be 0~3
  * @param  NewState: new state of the specified CapTouch interrupts mask.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CapTouch_INTConfig(CAPTOUCH_TypeDef *CapTouch, uint32_t CapTouch_IT, u8 newState)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_INT_EN(CapTouch_IT));

	if (newState == ENABLE) {
		/* Enable CapTouch interrupts */
		CapTouch->CT_IER |= CapTouch_IT;
	} else {
		/* Disable CapTouch interrupts */
		CapTouch->CT_IER &= (~CapTouch_IT);
	}
}


/**
  * @brief  Clears the specified CapTouch interrupt pending bit.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  CapTouch_IT: specifies the CapTouch interrupt to be cleared.
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_CT_N_NOISE_OVERFLOW_INT_CLR:CapTouch negetive noise overflow interrupt
  *            @arg BIT_CT_FIFO_OVERFLOW_INT_CLR: CapTouch FIFO overflow interrupt
  *            @arg BIT_CT_P_NOISE_OVERFLOW_INT_CLR:CapTouch positive noise overflow interrupt
  *            @arg CT_CHX_PRESS_INT(x): CapTouch channel(x) press interrupt, where x can be 0~3
  *            @arg CT_CHX_RELEASE_INT(x): CapTouch channel(x) release interrupt, where x can be 0~3
  * @retval None
  */
void CapTouch_INTClearPendingBit(CAPTOUCH_TypeDef *CapTouch, u32 CapTouch_IT)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_INT_CLR(CapTouch_IT));
	
	CapTouch->CT_ICR |= CapTouch_IT;
}


/**
  * @brief  Get CapTouch Raw Interrupt Status.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @retval raw interrupt status
  */
u32 CapTouch_GetRawISR(CAPTOUCH_TypeDef *CapTouch)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	return CapTouch->CT_ISR_RAW;
}


/**
  * @brief  Get CapTouch interrupt status.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @retval interrupt status
  */
u32 CapTouch_GetISR(CAPTOUCH_TypeDef *CapTouch)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	return CapTouch->CT_ISR;
}

/**
  * @brief  Get CapTouch channel enable status.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~3
  * @retval channel enable status.
  */
u32 CapTouch_GetChStatus(CAPTOUCH_TypeDef *CapTouch, u32 Channel)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	if (CapTouch->CT_CH[Channel].CTRL & BIT_CT_CHX_ENABLE)
		return TRUE;
	else
		return FALSE;
}

/**
  * @brief  Enable or disable specified channel.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~3
  * @param  NewState: new state of the specified channel. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CapTouch_ChCmd(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	if (NewState != DISABLE) {
		/* Enable the CapTouch crossponding channel */
		CapTouch->CT_CH[Channel].CTRL |= BIT_CT_CHX_ENABLE;
	} else {
		/* Disable the CapTouch peripheral */
		CapTouch->CT_CH[Channel].CTRL &= ~BIT_CT_CHX_ENABLE;
	}
}

/**
  * @brief  Set CapTouch Scan interval.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param Interval: scan interval in units of ms
  * @retval None
  */
void CapTouch_SetScanInterval(CAPTOUCH_TypeDef *CapTouch, u32 Interval)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(Interval <= 4095);

	CapTouch_Cmd(CapTouch, DISABLE);
	TempVal = CapTouch->CT_SP_CTRL;
	TempVal &= ~BIT_CT_SCAN_INTERVAL;
	TempVal |= Interval;
	CapTouch->CT_SP_CTRL = TempVal;
	CapTouch_Cmd(CapTouch, ENABLE);
}

/**
  * @brief  Set Mbias current for specified channel.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~3.
  * @param  Mbias: Mbias value, relate current = 0.25*Mbias.
  * @retval None
  */
void CapTouch_SetChMbias(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u8 Mbias)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));
	assert_param(Mbias < 64);

	CapTouch_Cmd(CapTouch, DISABLE);
	CapTouch->CT_CH[Channel].MBIAS =Mbias;
	CapTouch_Cmd(CapTouch, ENABLE);
}

/**
  * @brief  Set relative touch threshold for related channel.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~3
  * @param  Threshold: Related Threshold value.
  * @retval None
  */
void CapTouch_SetChDiffThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u32 Threshold)
{
	u32 TempVal;
	
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));
	assert_param(Threshold < 4096);

	CapTouch_Cmd(CapTouch, DISABLE);
	TempVal = CapTouch->CT_CH[Channel].CTRL;
	TempVal &= ~BIT_CT_CHX_TOUCH_THRES;
	TempVal |= Threshold << 16;
	CapTouch->CT_CH[Channel].CTRL = TempVal;
	CapTouch_Cmd(CapTouch, ENABLE);
}

/**
  * @brief  Get relative threshold of touch judgement for specified channel.
   * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~3
  * @retval Difference threshold of specified channel.
  */
u32 CapTouch_GetChDiffThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel)
{	
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	return (CapTouch->CT_CH[Channel].CTRL & BIT_CT_CHX_TOUCH_THRES) >> 16;
}

/**
  * @brief  Get Absolute  threshold of touch judgement for specified channel.
   * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~3
  * @retval Difference threshold of specified channel.
  */
u32 CapTouch_GetChAbsThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel)
{	
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	return CapTouch->CT_CH[Channel].ATHR & BIT_CT_CHX_TOUCH_ATHRES;
}

/**
  * @brief  Get positive or negative noise threshold for specified channel.
   * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~3
  * @param  type: can be P_NOISE_THRES or N_NOISE_THRES
  * @retval  Noise threshold of specified channel.
  */
u32 CapTouch_GetNoiseThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u8 type)
{	
	u8 value;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	if(type)
		value = (CapTouch->CT_CH[Channel].ATHR & BIT_MASK_CHX_N_ENT) >> 24;
	else
		value = (CapTouch->CT_CH[Channel].ATHR & BIT_MASK_CHX_P_ENT) >> 16;
	
	return value;
}

/**
  * @brief  Read Baseline data from specified channel.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~3
  * @retval Baseline data
  */
u32 CapTouch_GetChBaseline(CAPTOUCH_TypeDef *CapTouch, u8 Channel)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	return ((CapTouch->CT_CH[Channel].CTRL & BIT_CT_CHX_BASELINE) >> 4);
}


/**
  * @brief  Read average data from specified channel.
   * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~3
  * @retval Average data
  */
u32 CapTouch_GetChAveData(CAPTOUCH_TypeDef *CapTouch, u8 Channel)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	return (CapTouch->CT_CH[Channel].DATA & BIT_CT_CHX_DATA);
}

/**
  * @brief  Enable or disable Debug mode.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  NewState: new state of the Debug mode. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CapTouch_DbgCmd(CAPTOUCH_TypeDef *CapTouch, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	if (NewState != DISABLE) {
		/* Enable the CapTouch debug mode */
		CapTouch->CT_MODE_CTRL |= BIT_CT_DBG_ENABLE;
	} else {
		/* Disable the CapTouch peripheral */
		CapTouch->CT_MODE_CTRL |= BIT_CT_AUTO_CHANNEL_ENABLE;
		CapTouch->CT_MODE_CTRL &= (~BIT_CT_DBG_ENABLE);
	}
}


/**
  * @brief select debug channel
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~3
  * @retval none
  */
void CapTouch_DbgChannel(CAPTOUCH_TypeDef *CapTouch, u8 Channel)
{
	u32 data;
	
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	data = CapTouch->CT_MODE_CTRL;
	data &= ~(BIT_CT_CHANNEL_SEL | BIT_CT_AUTO_CHANNEL_ENABLE);
	data |= Channel << 5;
	CapTouch->CT_MODE_CTRL = data;

}


/**
  * @brief Get Raw data in debug mode
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @retval Raw data
  */
u32 CapTouch_DbgRawData(CAPTOUCH_TypeDef *CapTouch)
{
	u32 loop = 0;
	u32 data;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	do {
		data = CapTouch->CT_FIFO;

		if(loop++ > 10000){
			DBG_8195A("sample timeout \n" );
			return  0xEAEA;
		}

	} while((data & BIT(31)) == 0);
	
	return (data & 0x0FFF);
		
}

/**
  * @brief Dump all registers
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @retval NA
  */
void CapTouch_DbgDumpReg(CAPTOUCH_TypeDef *CapTouch)
{
	DBG_8195A("\n%08x: %08x (CT_CTRL)\n", &(CapTouch->CT_CTRL), CapTouch->CT_CTRL);
	DBG_8195A("%08x: %08x (CT_SP_CTRL)\n", &(CapTouch->CT_SP_CTRL), CapTouch->CT_SP_CTRL);
	DBG_8195A("%08x: %08x (CT_ETC_CTRL)\n", &(CapTouch->CT_ETC_CTRL), CapTouch->CT_ETC_CTRL);
	DBG_8195A("%08x: %08x (CT_SNR)\n", &(CapTouch->CT_SNR), CapTouch->CT_SNR);
	DBG_8195A("%08x: %08x (CT_MODE_CTRL)\n", &(CapTouch->CT_MODE_CTRL), CapTouch->CT_MODE_CTRL);
	DBG_8195A("%08x: %08x (CT_FIFO_STATUS)\n", &(CapTouch->CT_FIFO_STATUS), CapTouch->CT_FIFO_STATUS);
	DBG_8195A("%08x: %08x (CT_FIFO)\n", &(CapTouch->CT_FIFO), CapTouch->CT_FIFO);
	DBG_8195A("%08x: %08x (CT_IER)\n", &(CapTouch->CT_IER), CapTouch->CT_IER);
	DBG_8195A("%08x: %08x (CT_ISR)\n", &(CapTouch->CT_ISR), CapTouch->CT_ISR);
	DBG_8195A("%08x: %08x (CT_ISR_RAW)\n", &(CapTouch->CT_ISR_RAW), CapTouch->CT_ISR_RAW);
	DBG_8195A("%08x: %08x (CT_ICR_ALL)\n", &(CapTouch->CT_ICR_ALL), CapTouch->CT_ICR_ALL);
	DBG_8195A("%08x: %08x (CT_ICR)\n", &(CapTouch->CT_ICR), CapTouch->CT_ICR);

	DBG_8195A("%08x: %08x (CT_CH[0].CTRL)\n", &(CapTouch->CT_CH[0].CTRL), CapTouch->CT_CH[0].CTRL);
	DBG_8195A("%08x: %08x (CT_CH[0].ATHR)\n", &(CapTouch->CT_CH[0].ATHR), CapTouch->CT_CH[0].ATHR);
	DBG_8195A("%08x: %08x (CT_CH[0].MBIAS)\n", &(CapTouch->CT_CH[0].MBIAS), CapTouch->CT_CH[0].MBIAS);
	DBG_8195A("%08x: %08x (CT_CH[0].DATA)\n", &(CapTouch->CT_CH[0].DATA), CapTouch->CT_CH[0].DATA);

	DBG_8195A("%08x: %08x (CT_ADC_REG0X_LPAD)\n", &(CapTouch->CT_ADC_REG0X_LPAD), CapTouch->CT_ADC_REG0X_LPAD);
	DBG_8195A("%08x: %08x (CT_ADC_REG1X_LPAD)\n", &(CapTouch->CT_ADC_REG1X_LPAD), CapTouch->CT_ADC_REG1X_LPAD);
	DBG_8195A("%08x: %08x (CT_ADC_REG0X_LPSD)\n", &(CapTouch->CT_ADC_REG0X_LPSD), CapTouch->CT_ADC_REG0X_LPSD);
	DBG_8195A("%08x: %08x (CT_ADC_TIME)\n", &(CapTouch->CT_ADC_TIME), CapTouch->CT_ADC_TIME);
}

/**
  * @brief Dump all registers
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  ch: specified channel index, which can be 0~3
  * @retval NA
  */
void CapTouch_DbgDumpETC(CAPTOUCH_TypeDef *CapTouch, u32 ch)
{
	/* To avoid gcc warnings */
	( void ) CapTouch;
	
	u32 DiffThres = 0;
	u32 Baseline = 0;
	u32 AveData = 0;
	u32 NoiseThresP = 0;
	u32 NoiseThresN = 0;
	u32 AbsoThres = 0;

	DiffThres = CapTouch_GetChDiffThres(CAPTOUCH_DEV, ch);			
	Baseline = CapTouch_GetChBaseline(CAPTOUCH_DEV, ch);
	AveData = CapTouch_GetChAveData(CAPTOUCH_DEV, ch);
	NoiseThresP = CapTouch_GetNoiseThres(CAPTOUCH_DEV, ch, P_NOISE_THRES);
	NoiseThresN = CapTouch_GetNoiseThres(CAPTOUCH_DEV, ch, N_NOISE_THRES);
	AbsoThres = CapTouch_GetChAbsThres(CAPTOUCH_DEV, ch);
	
	DBG_8195A("\nCH[%d] ETC: ChDiffThres:%08x (%d) \n", ch, DiffThres, DiffThres);			
	DBG_8195A("CH[%d] ETC: Baseline:%08x (%d) \n", ch, Baseline, Baseline);
	DBG_8195A("CH[%d] ETC: RawAveData:%08x (%d) Diff:%d \n", ch, AveData, AveData, AveData-Baseline);			
	DBG_8195A("CH[%d] ETC: NoiseThres P:%08x (%d) \n", ch, NoiseThresP, NoiseThresP);
	DBG_8195A("CH[%d] ETC: NoiseThres N:%08x (%d) \n", ch, NoiseThresN, NoiseThresN);			
	DBG_8195A("CH[%d] ETC: AbsoThres:%08x (%d) \n", ch, AbsoThres, AbsoThres);
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
