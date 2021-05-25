/**
  ******************************************************************************
  * @file    rtl8721d_adc.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following
  *          functionalities of the Analog to Digital Convertor (ADC) peripheral:
  *           - Initialization and Configuration
  *           - Analog configuration
  *           - Mode configuration
  *           - Interrupts and flags management
  *
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#include "ameba_soc.h"

/**
  * @brief	 Initializes the parameters in the ADC_InitStruct with default values.
  * @param  ADC_InitStruct: pointer to a ADC_InitTypeDef structure that contains
  *         the configuration information for the ADC peripheral.
  * @retval  None
  */
void ADC_StructInit(ADC_InitTypeDef *ADC_InitStruct)
{
	u8 i;

	ADC_InitStruct->ADC_OpMode = ADC_SW_TRI_MODE;
	ADC_InitStruct->ADC_ClkDiv = ADC_CLK_DIV_12;
	ADC_InitStruct->ADC_RxThresholdLevel = 0;
	ADC_InitStruct->ADC_DMAThresholdLevel = 7;
	ADC_InitStruct->ADC_CvlistLen = ADC_CH_NUM - 1;
	ADC_InitStruct->ADC_ChanInType = 0;
	ADC_InitStruct->ADC_SpecialCh = 0xFF;
	ADC_InitStruct->ADC_ChIDEn = DISABLE;

	for(i = 0; i < ADC_InitStruct->ADC_CvlistLen + 1; i++)
		ADC_InitStruct->ADC_Cvlist[i] = i;
}

/**
  * @brief	 Initializes the ADC according to the specified parameters in ADC_InitStruct.
  * @param  ADC_InitStruct: pointer to a ADC_InitTypeDef structure that contains
  *         the configuration information for the ADC peripheral.
  * @retval  None
  */
void ADC_Init(ADC_InitTypeDef* ADC_InitStruct)
{
	ADC_TypeDef	*adc = ADC;
	u32 value = 0;
	u8 len, i;

	assert_param(IS_ADC_MODE(ADC_InitStruct->ADC_OpMode));
	assert_param(IS_ADC_SAMPLE_CLK(ADC_InitStruct->ADC_ClkDiv));
	assert_param(ADC_InitStruct->ADC_CvlistLen < 16);

	/* Disable ADC, clear pending interrupt, clear FIFO */
	ADC_Cmd(DISABLE);
	
	adc->ADC_INTR_CTRL= 0;
	ADC_INTClear();
	ADC_ClearFIFO();

	/* Set clock divider */
	adc->ADC_CLK_DIV = (u32)ADC_InitStruct->ADC_ClkDiv & BIT_MASK_CLK_DIV;

	/* Set adc configuration*/
	value = adc->ADC_CONF & (~(BIT_MASK_CVLIST_LEN | BIT_MASK_OP_MODE));
	value |= (ADC_InitStruct->ADC_OpMode << BIT_SHIFT_OP_MODE) | \
			(ADC_InitStruct->ADC_CvlistLen << BIT_SHIFT_CVLIST_LEN);
	adc->ADC_CONF = value;

	/* Set channel input type */
	adc->ADC_IN_TYPE = ADC_InitStruct->ADC_ChanInType;

	/* Set channel switch list */
	value = 0;
	len = ((ADC_InitStruct->ADC_CvlistLen + 1) > 8) ? 8 : (ADC_InitStruct->ADC_CvlistLen + 1);
	for(i = 0; i < len; i++) {
		value |= (u32)(ADC_InitStruct->ADC_Cvlist[i] << BIT_SHIFT_CHSW0(i));
	}
	adc->ADC_CHSW_LIST[0] = value;
	
	value = 0;
	if((ADC_InitStruct->ADC_CvlistLen + 1) > 8) {
		for(i = 8; i < (ADC_InitStruct->ADC_CvlistLen + 1); i++)
			value |= (u32)(ADC_InitStruct->ADC_Cvlist[i] << BIT_SHIFT_CHSW1(i));
	}
	adc->ADC_CHSW_LIST[1] = value;

	/* Set particular channel */
	if(ADC_InitStruct->ADC_SpecialCh < ADC_CH_NUM) {
		ADC_INTConfig(BIT_ADC_IT_CHCV_END_EN, ENABLE);
		adc->ADC_IT_CHNO_CON = (u32)ADC_InitStruct->ADC_SpecialCh;
	}

	/* Set FIFO full level */
	adc->ADC_FULL_LVL = (u32)ADC_InitStruct->ADC_RxThresholdLevel;

	/* Set DMA level */
	value = adc->ADC_DMA_CON & (~BIT_MASK_DMA_LVL);
	value |= (ADC_InitStruct->ADC_DMAThresholdLevel << BIT_SHIFT_DMA_LVL);
	adc->ADC_DMA_CON = value;

	/* Set channel ID included in data or not */
	if(ADC_InitStruct->ADC_ChIDEn)
		adc->ADC_DELAY_CNT |= BIT_ADC_DAT_CHID;
}

/**
  * @brief  Enable or Disable the ADC peripheral.
  * @param  NewState: new state of the ADC peripheral.
  *   			This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_Cmd(u32 NewState)
{
	ADC_TypeDef	*adc = ADC;

	if(NewState != DISABLE)
		adc->ADC_CONF |= BIT_ADC_ENABLE;
	else {
		adc->ADC_CONF &= ~BIT_ADC_ENABLE;

		/* Need to clear FIFO, or there will be waste data in FIFO next time ADC is enable. Ameba-D A-cut bug */
		ADC_ClearFIFO();
	}
}

/**
  * @brief	 ENABLE/DISABLE  the ADC interrupt bits.
  * @param  ADC_IT: specifies the ADC interrupt to be setup.
  *          This parameter can be one or combinations of the following values: 
  *            @arg BIT_ADC_IT_COMP_CH10_EN:	ADC channel 10 compare interrupt
  *            @arg BIT_ADC_IT_COMP_CH9_EN:	ADC channel 9 compare interrupt
  *            @arg BIT_ADC_IT_COMP_CH8_EN:	ADC channel 8 compare interrupt
  *            @arg BIT_ADC_IT_COMP_CH7_EN:	ADC channel 7 compare interrupt
  *            @arg BIT_ADC_IT_COMP_CH6_EN:	ADC channel 6 compare interrupt
  *            @arg BIT_ADC_IT_COMP_CH5_EN:	ADC channel 5 compare interrupt
  *            @arg BIT_ADC_IT_COMP_CH4_EN:	ADC channel 4 compare interrupt
  *            @arg BIT_ADC_IT_COMP_CH3_EN:	ADC channel 3 compare interrupt
  *            @arg BIT_ADC_IT_COMP_CH2_EN:	ADC channel 2 compare interrupt
  *            @arg BIT_ADC_IT_COMP_CH1_EN:	ADC channel 1 compare interrupt
  *            @arg BIT_ADC_IT_COMP_CH0_EN:	ADC channel 0 compare interrupt
  *            @arg BIT_ADC_IT_ERR_EN:			ADC error state interrupt
  *            @arg BIT_ADC_IT_DAT_OVW_EN:	ADC data overwritten interrupt
  *            @arg BIT_ADC_IT_FIFO_EMPTY_EN:	ADC FIFO empty interrupt
  *            @arg BIT_ADC_IT_FIFO_OVER_EN:	ADC FIFO overflow interrupt
  *            @arg BIT_ADC_IT_FIFO_FULL_EN:	ADC FIFO full interrupt
  *            @arg BIT_ADC_IT_CHCV_END_EN:	ADC particular channel conversion done interrupt
  *            @arg BIT_ADC_IT_CV_END_EN:		ADC conversion end interrupt
  *            @arg BIT_ADC_IT_CVLIST_END_EN:	ADC conversion list end interrupt
  * @param  NewState: ENABLE/DISABLE.
  * @retval  None
  */
void ADC_INTConfig(u32 ADC_IT, u32 NewState)
{
	ADC_TypeDef	*adc = ADC;

	if(NewState != DISABLE) {
		adc->ADC_INTR_CTRL |= ADC_IT;
	} else {
		adc->ADC_INTR_CTRL &= ~ADC_IT;
	}
}

/**
  * @brief	 Clears all the ADC interrupt pending bits.
  * @param  None
  * @retval  None
  * @note  This function can also used to clear raw status.
  */
void ADC_INTClear(void)
{
	ADC_TypeDef	*adc = ADC;

	/* Clear the all IT pending Bits */
	adc->ADC_INTR_STS = BIT_ADC_IT_ALL_STS;
}

/**
  * @brief  Clears the ADC interrupt pending bits.
  * @param  ADC_IT: specifies the pending bit to clear.
  *   This parameter can be any combination of the following values:
  *            @arg BIT_ADC_IT_COMP_CH10_STS
  *            @arg BIT_ADC_IT_COMP_CH9_STS
  *            @arg BIT_ADC_IT_COMP_CH8_STS
  *            @arg BIT_ADC_IT_COMP_CH7_STS
  *            @arg BIT_ADC_IT_COMP_CH6_STS
  *            @arg BIT_ADC_IT_COMP_CH5_STS
  *            @arg BIT_ADC_IT_COMP_CH4_STS
  *            @arg BIT_ADC_IT_COMP_CH3_STS
  *            @arg BIT_ADC_IT_COMP_CH2_STS
  *            @arg BIT_ADC_IT_COMP_CH1_STS
  *            @arg BIT_ADC_IT_COMP_CH0_STS
  *            @arg BIT_ADC_IT_ERR_STS
  *            @arg BIT_ADC_IT_DAT_OVW_STS
  *            @arg BIT_ADC_IT_FIFO_EMPTY_STS
  *            @arg BIT_ADC_IT_FIFO_OVER_STS
  *            @arg BIT_ADC_IT_FIFO_FULL_STS
  *            @arg BIT_ADC_IT_CHCV_END_STS
  *            @arg BIT_ADC_IT_CV_END_STS
  *            @arg BIT_ADC_IT_CVLIST_END_STS
  * @retval  None
  */
void ADC_INTClearPendingBits(u32 ADC_IT)
{
	ADC_TypeDef	*adc = ADC;

	adc->ADC_INTR_STS = ADC_IT;
}

/**
  * @brief	 Get ADC interrupt status.
  * @param  None
  * @retval  Current Interrupt Status, each bit of this value represents one  
  *		interrupt status which is as follows:
  *            @arg BIT_ADC_IT_COMP_CH10_STS
  *            @arg BIT_ADC_IT_COMP_CH9_STS
  *            @arg BIT_ADC_IT_COMP_CH8_STS
  *            @arg BIT_ADC_IT_COMP_CH7_STS
  *            @arg BIT_ADC_IT_COMP_CH6_STS
  *            @arg BIT_ADC_IT_COMP_CH5_STS
  *            @arg BIT_ADC_IT_COMP_CH4_STS
  *            @arg BIT_ADC_IT_COMP_CH3_STS
  *            @arg BIT_ADC_IT_COMP_CH2_STS
  *            @arg BIT_ADC_IT_COMP_CH1_STS
  *            @arg BIT_ADC_IT_COMP_CH0_STS
  *            @arg BIT_ADC_IT_ERR_STS
  *            @arg BIT_ADC_IT_DAT_OVW_STS
  *            @arg BIT_ADC_IT_FIFO_EMPTY_STS
  *            @arg BIT_ADC_IT_FIFO_OVER_STS
  *            @arg BIT_ADC_IT_FIFO_FULL_STS
  *            @arg BIT_ADC_IT_CHCV_END_STS
  *            @arg BIT_ADC_IT_CV_END_STS
  *            @arg BIT_ADC_IT_CVLIST_END_STS
  */
u32 ADC_GetISR(void)
{
	ADC_TypeDef	*adc = ADC;
	
	return adc->ADC_INTR_STS;
}

/**
  * @brief  Get ADC raw interrupt status.
  * @param  None
  * @retval  Current Raw Interrupt Status, each bit of this value represents one  
  *		raw interrupt status which is as follows:
  *            @arg BIT_ADC_IT_COMP_CH10_RAW_STS
  *            @arg BIT_ADC_IT_COMP_CH9_RAW_STS
  *            @arg BIT_ADC_IT_COMP_CH8_RAW_STS
  *            @arg BIT_ADC_IT_COMP_CH7_RAW_STS
  *            @arg BIT_ADC_IT_COMP_CH6_RAW_STS
  *            @arg BIT_ADC_IT_COMP_CH5_RAW_STS
  *            @arg BIT_ADC_IT_COMP_CH4_RAW_STS
  *            @arg BIT_ADC_IT_COMP_CH3_RAW_STS
  *            @arg BIT_ADC_IT_COMP_CH2_RAW_STS
  *            @arg BIT_ADC_IT_COMP_CH1_RAW_STS
  *            @arg BIT_ADC_IT_COMP_CH0_RAW_STS
  *            @arg BIT_ADC_IT_ERR_RAW_STS
  *            @arg BIT_ADC_IT_DAT_OVW_RAW_STS
  *            @arg BIT_ADC_IT_FIFO_EMPTY_RAW_STS
  *            @arg BIT_ADC_IT_FIFO_OVER_RAW_STS
  *            @arg BIT_ADC_IT_FIFO_FULL_RAW_STS
  *            @arg BIT_ADC_IT_CHCV_END_RAW_STS
  *            @arg BIT_ADC_IT_CV_END_RAW_STS
  *            @arg BIT_ADC_IT_CVLIST_END_RAW_STS
  */
u32 ADC_GetRawISR(void)
{
	ADC_TypeDef	*adc = ADC;
	
	return adc->ADC_INTR_RAW_STS;
}

/**
  * @brief  Get the number of valid entries in ADC receive FIFO.
  * @param  None.
  * @retval  The number of valid entries in receive FIFO.
  */
u32 ADC_GetRxCount(void)
{
	ADC_TypeDef	*adc = ADC;

	return adc->ADC_FLR & BIT_MASK_FLR;
}

/**
  * @brief  Get the last ADC used channel.
  * @param  None.
  * @retval  The last ADC used channel index.
  */
u32 ADC_GetLastChan(void)
{
	ADC_TypeDef	*adc = ADC;

	return adc->ADC_LAST_CH;
}

/**
  * @brief  Get comparison result of ADC channel.
  * @param  ADC_Channel: The channel index
  * @retval  The comparison result of specified ADC channel.
  */
u32 ADC_GetCompStatus(u8 ADC_Channel)
{
	ADC_TypeDef	*adc = ADC;
	u32 value = (adc->ADC_COMP_STS & BIT_MASK_COMP_STS_CH(ADC_Channel)) \
					>> BIT_SHIFT_COMP_STS_CH(ADC_Channel);

	return  value;
}

/**
  * @brief  Set ADC channel threshold and criteria for comparison.
  * @param  ADC_channel: can be a value of @ref ADC_Chn_Selection as following:
  *				@arg ADC_CH0
  *				@arg ADC_CH1
  *				@arg ADC_CH2
  *				@arg ADC_CH3
  *				@arg ADC_CH4
  *				@arg ADC_CH5
  *				@arg ADC_CH6
  *				@arg ADC_CH7
  *				@arg ADC_CH8
  *				@arg ADC_CH9
  *				@arg ADC_CH10
  * @param  CompThresH:  the higher threshold of channel for ADC automatic comparison
  * @param  CompThresH:  the lower threshold of channel for ADC automatic comparison
  * @param  CompCtrl:  This parameter can be a value of @ref ADC_Compare_Control_Definitions as following:
  *		 		@arg ADC_COMP_SMALLER_THAN_THL: less than the lower threshold
  *		 		@arg ADC_COMP_GREATER_THAN_THH: greater than the higher threshod
  *		 		@arg ADC_COMP_WITHIN_THL_AND_THH: between the lower and higher threshod
  *		 		@arg ADC_COMP_OUTSIDE_THL_AND_THH: out the range of the higher and lower threshod
  * @retval None
  */
void ADC_SetComp(u8 ADC_channel, u16 CompThresH, u16 CompThresL, u8 CompCtrl)
{
	ADC_TypeDef	*adc = ADC;
	u32 value;

	assert_param(IS_ADC_CHN_SEL(ADC_channel));
	assert_param(IS_ADC_VALID_COMP_TH(CompThresH));
	assert_param(IS_ADC_VALID_COMP_TH(CompThresL));
	assert_param(IS_ADC_COMP_CRITERIA(CompCtrl));

	/* Set ADC channel threshold for comparison */
	adc->ADC_COMP_TH_CH[ADC_channel] = 
		(u32)(CompThresH << BIT_SHIFT_COMP_TH_H) | (CompThresL << BIT_SHIFT_COMP_TH_L);

	/* Set ADC channel comparison criteria */
	value = adc->ADC_COMP_CTRL;
	value &= ~ BIT_MASK_COMP_CTRL_CH(ADC_channel);
	value |= (CompCtrl << BIT_SHIFT_COMP_CTRL_CH(ADC_channel));
	adc->ADC_COMP_CTRL = value;

	/* Enable comparison interrupt */
	ADC_INTConfig(BIT_ADC_IT_COMP_CH_EN(ADC_channel), ENABLE);
}

/**
  * @brief  Reset the channel switch to default state.
  * @param  None
  * @retval  None
  */
void ADC_ResetCSwList(void)
{
	ADC_TypeDef	*adc = ADC;

	adc->ADC_RST_LIST = BIT_ADC_RST_LIST;
	adc->ADC_RST_LIST = 0;
}

/**
  * @brief  Detemine ADC FIFO is empty or not.
  * @param  None.
  * @retval ADC FIFO is empty or not:
  *        - 0: Not Empty
  *        - 1: Empty
  */
u32 ADC_Readable(void)
{
	u32 Status = ADC_GetStatus();
	u32 Readable = (((Status & BIT_ADC_FIFO_EMPTY) == 0) ? 1 : 0);

	return Readable;
}

/**
  * @brief  Read data from ADC receive FIFO .
  * @param  None
  * @retval  The conversion data.
  */
u16 ADC_Read(void)
{
	u16 value = (u16)(ADC->ADC_DATA_GLOBAL & (BIT_MASK_DAT_GLOBAL | BIT_MASK_DAT_CHID));

	return value;
}

/**
  * @brief  Continuous Read data in auto mode.
  * @param  pBuf: pointer to buffer to keep sample data
  * @param  len: the number of sample data to be read
  * @retval  None.
  */
void ADC_ReceiveBuf(u16 *pBuf, u32 len)
{
	u32 i = 0;

	ADC_AutoCSwCmd(ENABLE);

	for(i = 0; i < len; i++){
		while(ADC_Readable() == 0);
		pBuf[i] = ADC_Read();
	}
	ADC_AutoCSwCmd(DISABLE);
}

/**
  * @brief  Clear ADC FIFO.
  * @param  None
  * @retval  None
  */
void ADC_ClearFIFO(void)
{
	ADC->ADC_CLR_FIFO = 1;
	ADC->ADC_CLR_FIFO = 0;
}

/**
  * @brief Get ADC status.
  * @param  None
  * @retval  Current status,each bit of this value represents one status which is as follows:
  *			
  *		bit 2 : BIT_ADC_FIFO_EMPTY  ADC FIFO Empty. 
  *			- 0: FIFO in ADC is not empty
  *			- 1: FIFO in ADC is empty
  *			
  *		bit 1 : BIT_ADC_FIFO_FULL_REAL  ADC FIFO real full flag. 
  *			- 0: FIFO in ADC is not full real
  *			- 1: FIFO in ADC is full real
  *			
  *		bit 0 : BIT_ADC_BUSY_STS  ADC busy Flag. 
  *			- 0: The ADC is ready
  *			- 1: The ADC is busy
  */
u32 ADC_GetStatus(void)
{
	ADC_TypeDef *adc = ADC;

	return adc->ADC_BUSY_STS;
}

/**
  * @brief  Control the ADC module to do a conversion. Used as a start-convert event which is controlled by software.
  * @param  NewState: can be one of the following value:
  *			@arg ENABLE: Enable the analog module and analog mux. And then start a new channel conversion.
  *			@arg DISABLE:  Disable the analog module and analog mux.
  * @retval  None.
  * @note  1. Every time this bit is set to 1, ADC module would switch to a new channel and do one conversion. 
  *			    Every time a conversion is done, software MUST clear this bit manually.
  *		  2. Used in Sotfware Trigger Mode
  */
void ADC_SWTrigCmd(u32 NewState)
{
	ADC_TypeDef	*adc = ADC;
	u8 div = adc->ADC_CLK_DIV;
	u8 sync_time[4] = {12, 16, 32, 64};

	if(NewState != DISABLE)
		adc->ADC_SW_TRIG = BIT_ADC_SW_TRIG;
	else
		adc->ADC_SW_TRIG = 0;

	/* Wait 2 clock to sync signal */
	DelayUs(sync_time[div]);
}

/**
  * @brief  Controls the automatic channel switch enabled or disabled.
  * @param  NewState: can be one of the following value:
  *		@arg ENABLE: Enable the automatic channel switch. 
  *			When setting this bit, an automatic channel switch starts from the first channel in the channel switch list.
  *		@arg DISABLE:  Disable the automatic channel switch. 
  *			If an automatic channel switch is in progess, writing 0 will terminate the automatic channel switch.
  * @retval  None.
  * @note  Used in Automatic Mode
  */
void ADC_AutoCSwCmd(u32 NewState)
{
	ADC_TypeDef	*adc = ADC;
	u8 div = adc->ADC_CLK_DIV;
	u8 sync_time[4] = {12, 16, 32, 64};

	if(NewState != DISABLE)
		adc->ADC_AUTO_CSW_CTRL = BIT_ADC_AUTO_CSW_EN;
	else
		adc->ADC_AUTO_CSW_CTRL = 0;

	/* Wait 2 clock to sync signal */
	DelayUs(sync_time[div]);
}

/**
  * @brief	Initialize the trigger timer when in ADC Timer-Trigger Mode.
  * @param  Tim_Idx: The timer index would be used to make ADC module do a conversion.
  * @param  PeriodMs: Indicate the period of trigger timer.
  * @param  NewState: can be one of the following value:
  *			@arg ENABLE: Enable the ADC timer trigger mode.
  *			@arg DISABLE: Disable the ADC timer trigger mode.
  * @retval  None.
  * @note  Used in Timer-Trigger Mode
  */
void ADC_TimerTrigCmd(u8 Tim_Idx, u32 PeriodMs, u32 NewState)
{
	ADC_TypeDef	*adc = ADC;
	RTIM_TimeBaseInitTypeDef TIM_InitStruct;

	assert_param(IS_ADC_VALID_TIM(Tim_Idx));

	adc->ADC_EXT_TRIG_TIMER_SEL = Tim_Idx;

	if(NewState != DISABLE) {
		RTIM_TimeBaseStructInit(&TIM_InitStruct);
		TIM_InitStruct.TIM_Idx = Tim_Idx;
		TIM_InitStruct.TIM_Period = (PeriodMs *32768)/1000/2;//ms to tick
		
		RTIM_TimeBaseInit(TIMx_LP[Tim_Idx], &TIM_InitStruct, TIMx_irq_LP[Tim_Idx], (IRQ_FUN)NULL, (u32)NULL);
		RTIM_Cmd(TIMx_LP[Tim_Idx], ENABLE);
	} else {
		RTIM_Cmd(TIMx_LP[Tim_Idx], DISABLE);
	}
}

/**
  * @brief  Enable or Disable DMA read mode.
  * @param  newState: ENABLE/DISABLE
  * @retval  None.
  */
void ADC_SetDmaEnable(u32 newState)
{
	if (newState == DISABLE)
		ADC->ADC_DMA_CON &= ~BIT_ADC_DMA_EN;
	else
		ADC->ADC_DMA_CON |= BIT_ADC_DMA_EN;
}

/**
  * @brief    Init and Enable ADC RX GDMA.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains 
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pDataBuf: Rx Buffer.
  * @param  DataLen: Rx Count.
  * @retval   TRUE/FLASE
  */
u32 ADC_RXGDMA_Init(
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8* pDataBuf,
	u32 DataLen)
{
	u8 GdmaChnl;

	GdmaChnl = GDMA_ChnlAlloc(0, (IRQ_FUN)CallbackFunc, (u32)CallbackData, 12);
	if (GdmaChnl == 0xFF) {
		DBG_8195A("ADC_RXGDMA_Init GDMA busy \n");
		return _FALSE;
	}
	
	/* ADC RX DMA */
	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	GDMA_InitStruct->MuliBlockCunt      = 0;
	GDMA_InitStruct->MaxMuliBlock       = 1;//MaxLlp;	
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthTwoBytes;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthTwoBytes;
	GDMA_InitStruct->GDMA_SrcMsize   = MsizeEight;
	GDMA_InitStruct->GDMA_DstMsize  = MsizeEight;
	GDMA_InitStruct->GDMA_SrcInc       = NoChange;
	GDMA_InitStruct->GDMA_DstInc       = IncType;
	GDMA_InitStruct->GDMA_DIR       = TTFCPeriToMem;
	GDMA_InitStruct->GDMA_SrcHandshakeInterface     = GDMA_HANDSHAKE_INTERFACE_ADC_RX;
	GDMA_InitStruct->GDMA_ReloadSrc  = 1;
	GDMA_InitStruct->GDMA_IsrType        = (BlockType|TransferType|ErrType);
	GDMA_InitStruct->GDMA_ChNum              = GdmaChnl;
	GDMA_InitStruct->GDMA_Index          =  0;

	GDMA_InitStruct->GDMA_BlockSize  =   DataLen;
	GDMA_InitStruct->GDMA_SrcAddr              =   (u32)&(ADC->ADC_DATA_GLOBAL);
	GDMA_InitStruct->GDMA_DstAddr              =   (u32)pDataBuf;

	/* GDMA initialization */
	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return _TRUE;
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
