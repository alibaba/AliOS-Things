/**
  ******************************************************************************
  * @file    rtl8721d_tim.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Timer peripheral:
  *           - Initialization
  *           - TimeBase configuration and management
  *           - Intput Capture/Output Compare configuration and management
  *           - Interrupt and DMA management 
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
  
int TIMx_irq[6] = {
	TIMER0_IRQ,
	TIMER1_IRQ,
	TIMER2_IRQ,
	TIMER3_IRQ,
	TIMER4_IRQ,
	TIMER5_IRQ,
};

RTIM_TypeDef* TIMx[6] = {
	TIM0,
	TIM1,
	TIM2,
	TIM3,
	TIM4,
	TIM5
};

u32 TIM_IT_CCx[18] = {
	TIM_IT_CC0,				
	TIM_IT_CC1,				
	TIM_IT_CC2,				
	TIM_IT_CC3,				
	TIM_IT_CC4,				
	TIM_IT_CC5,				
	TIM_IT_CC6,				
	TIM_IT_CC7,				
	TIM_IT_CC8,				
	TIM_IT_CC9,				
	TIM_IT_CC10,			
	TIM_IT_CC11,			
	TIM_IT_CC12,			
	TIM_IT_CC13,			
	TIM_IT_CC14,			
	TIM_IT_CC15,			
	TIM_IT_CC16,			
	TIM_IT_CC17				
};

int TIMx_irq_LP[6] = {
	TIMER0_IRQ_LP,
	TIMER1_IRQ_LP,
	TIMER2_IRQ_LP,
	TIMER3_IRQ_LP,
	TIMER4_IRQ_LP,
	TIMER5_IRQ_LP,
};

RTIM_TypeDef* TIMx_LP[6] = {
	TIMM00,
	TIMM01,
	TIMM02,
	TIMM03,
	TIMM04,
	TIMM05
};

u32 TIM_IT_CCx_LP[6] = {
	TIM_IT_CC0,				
	TIM_IT_CC1,				
	TIM_IT_CC2,				
	TIM_IT_CC3,				
	TIM_IT_CC4,				
	TIM_IT_CC5				
};

/**
  * @brief  Enables or Disables the TIMx Update event(UEV).
  * @param  TIMx: where x can be 0-5 to select the TIM peripheral.
  * @param  NewState: new state of the TIMx UDIS bit
  *          This parameter can be:ENABLE or DISABLE
  * @note  
  *		- If NewState is ENABLE, Update Disable Bit is set, UEV disable and shadow registers keep their value.
  *		- If NewState is DISABLE, Update Disable Bit is clear, UEV enable and buffered registers are loaded with 
  *		their preload values when UEV happen.
  * @retval None
  */
void RTIM_UpdateDisableConfig(RTIM_TypeDef* TIMx, u32 NewState)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_TIM(TIMx));

	if (NewState != DISABLE) {
		/* Set the Update Disable Bit */
		TIMx->CR |= TIM_CR_UDIS;
	} else {
		/* Reset the Update Disable Bit */
		TIMx->CR &= ~TIM_CR_UDIS;
	}
}

/**
  * @brief  Enables or disables TIMx peripheral Preload register on ARR.
  * @param  TIMx: where x can be 0-5 to select the TIM peripheral.
  * @param  NewState: new state of the TIMx peripheral Preload register
  *          This parameter can be: ENABLE or DISABLE.
  * @note
  *		- DISABLE: TIMx_ARR register is not buffered, and shadow register will update immediately
  *		- ENABLE: TIMx_ARR register is buffered, and shadow register will update after overflow
  * @retval None
  */
void RTIM_ARRPreloadConfig(RTIM_TypeDef* TIMx, u32 NewState)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_TIM(TIMx));
  
	if (NewState != DISABLE) {
		/* Set the ARR Preload Bit */
		TIMx->CR |= TIM_CR_ARPE;
	} else {
		/* Reset the ARR Preload Bit */
		TIMx->CR &= ~TIM_CR_ARPE;
	}
}

/**
  * @brief  Configures the TIMx Update Request Interrupt source.
  * @param  TIMx: where x can be 0-5 to select the TIM peripheral.
  * @param  TIM_UpdateSource: specifies the Update source.
  *          This parameter can be one of the following values:
  *            @arg TIM_UpdateSource_Global: Source of update is the counter
  *                 overflow or the setting of UG bit.
  *            @arg TIM_UpdateSource_Overflow: Source of update is counter overflow.
  * @retval None
  */
void RTIM_UpdateRequestConfig(RTIM_TypeDef* TIMx, u32 TIM_UpdateSource)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_TIM(TIMx));
	assert_param(IS_TIM_UPDATE_SOURCE(TIM_UpdateSource));
  
	if (TIM_UpdateSource != TIM_UpdateSource_Global) {
		/* Set the URS Bit */
		TIMx->CR |= TIM_CR_URS;
	} else {
		/* Reset the URS Bit */
		TIMx->CR &= (u32)~TIM_CR_URS;
	}
}

/**
  * @brief  Configures the TIMx Prescaler.
  * @param  TIMx: where x can be  4 or 5 to select the TIM peripheral.
  * @param  Prescaler: specifies the Prescaler Register value,which can be a number in 0~0xFF range.
  * @param  TIM_PSCReloadMode: specifies the TIM Prescaler Reload mode
  *          This parameter can be one of the following values:
  *            @arg TIM_PSCReloadMode_Update: The Prescaler is loaded at the update event.
  *            @arg TIM_PSCReloadMode_Immediate: The Prescaler is loaded immediatly.
  * @retval None
  */
void RTIM_PrescalerConfig(RTIM_TypeDef* TIMx, u32 Prescaler, u32 TIM_PSCReloadMode)
{
	/* Check the parameters */
	assert_param(IS_TIM_40M_TIM(TIMx));
	assert_param(IS_TIM_PSC(Prescaler));
	assert_param(IS_TIM_PRESCALER_RELOAD(TIM_PSCReloadMode));
  
	/* Set the Prescaler value */
	TIMx->PSC = Prescaler;
	/* Set or reset the UG Bit */
	TIMx->EGR = TIM_PSCReloadMode;
}

/**
  * @brief  Configures the TIMx event to be generate by software.
  * @param  TIMx: where x can be 0-5 to select the TIM peripheral.
  * @param  TIM_EventSource: specifies the event source.
  *          This parameter can be one or more of the following values @ref TIMx_Event_Generation_definitons	   
  * @note  
  *		- TIM0~5/TIMM00~05 have Timer update Event source TIM_EventSource_Update
  *		- TIM4/TIMM04 have Timer Capture Compare 1 Event source TIM_EventSource_CC0
  *		- TIMM05 has 6 Timer Capture Compare x Event source TIM_EventSource_CC0-5.
  *		- TIM5 has 18 Timer Capture Compare x Event source TIM_EventSource_CC0-17.
  * @retval None
  */
void RTIM_GenerateEvent(RTIM_TypeDef* TIMx, u32 TIM_EventSource)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_TIM(TIMx));
	assert_param((IS_HP_TIM_EVENT_SOURCE(TIM_EventSource) || IS_LP_TIM_EVENT_SOURCE(TIM_EventSource)));
  
	/* Set the event sources */
	TIMx->EGR = TIM_EventSource;
}

/**
  * @brief  Sets the TIMx Autoreload Register(TIMx_ARR) value to change period
  * @param  TIMx: where x can be 0-5 to select the TIM peripheral.
  * @param  Autoreload: specifies the Autoreload register new value.
  *			To TIM0~3/TIMM00~TIMM03 this value can be a number in 0~0xFFFFFFFF range.
  *			To TIM4~5/TIMM04~TIMM05 this value can be a number in 0~0xFFFF range.
  * @retval None
  */
void RTIM_ChangePeriodImmediate(RTIM_TypeDef* TIMx, u32 Autoreload)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_TIM(TIMx));

	/* Reset the ARR Preload Bit */
	/* period will update immediatly */
	TIMx->CR &= ~TIM_CR_ARPE;
		
	/* Set the Autoreload Register value */
	TIMx->ARR = Autoreload;

	/* Generate an update event */
	/* 1) reload the Prescaler immediatly */
	/* 2) reload the ARR immediatly */
	/* 3) hadrware will clear this bit after reload  */
	/* 4) UEV will reset counter, and counter will start from 0 */
	/* 5) gen a interrupt if use TIM_UpdateSource_Global */
	TIMx->EGR = TIM_PSCReloadMode_Immediate;

	/* poll EGR UG done */
	while (1) {
		if (TIMx->SR & TIM_SR_UG_DONE)
			break;
	}
}

/**
  * @brief  Sets the TIMx Autoreload Register(TIMx_ARR) value to change period with protection
  * @param  TIMx: where x can be 0-5 to select the TIM peripheral.
  * @param  Autoreload: specifies the Autoreload register new value.
  *			To TIM0~3/TIMM00~TIMM03 this value can be a number in 0~0xFFFFFFFF range.
  *			To TIM4~5/TIMM04~TIMM05 this value can be a number in 0~0xFFFF range.
  * @retval None
  */
void RTIM_ChangePeriod(RTIM_TypeDef* TIMx, u32 Autoreload)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_TIM(TIMx));

	/* Set the Autoreload Register value */
	TIMx->ARR = Autoreload;
}

/**
  * @brief  Reset timer, counter will start from 0
  * @param  TIMx: where x can be 0-5 to select the TIM peripheral.
  * @retval None
  */
void RTIM_Reset(RTIM_TypeDef* TIMx)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_TIM(TIMx));

	/* Generate an update event */
	/* 1) reload the Prescaler immediatly */
	/* 2) reload the ARR immediatly */
	/* 3) hadrware will clear this bit after reload  */
	/* 4) UEV will reset counter, and counter will start from 0 */
	/* 5) gen a interrupt if use TIM_UpdateSource_Global */
	TIMx->EGR = TIM_PSCReloadMode_Immediate;
}

/**
  * @brief  Fills each TIM_CCInitStruct member with its default value.
  * @param  TIM_CCInitStruct: pointer to a TIM_CCInitTypeDef structure which will
  *         be initialized.
  * @retval None
  */
void RTIM_CCStructInit(TIM_CCInitTypeDef* TIM_CCInitStruct)
{
	/* Set the default configuration */
	TIM_CCInitStruct->TIM_CCMode = TIM_CCMode_PWM;
	TIM_CCInitStruct->TIM_CCPolarity = TIM_CCPolarity_High;
	TIM_CCInitStruct->TIM_OCProtection = TIM_OCPreload_Enable;
	TIM_CCInitStruct->TIM_OCPulse = 0xFFF;
}

/**
  * @brief  Initializes the TIMx Channel 0-5 according to the specified parameters in
  *         the TIM_CCInitStruct.
  * @param  TIMx: where x can be 4/5, to select the TIM peripheral.
  * @param  TIM_CCInitStruct: pointer to a TIM_CCInitTypeDef structure that contains
  *         the configuration information for the specified TIM peripheral.
  * @param  TIM_Channel: the channel need to be initialized,
  *		which can be one of the following parameters @ref TIM_Channel_definitions
  * @note  
  *		- TIM4/TIMM04 only has 1 channel: TIM_Channel_0
  *		- TIMM05 has 6 channels:  TIM_Channel_0-5.
  *		- TIM5 has 18 channels:  TIM_Channel_0-17.
  * @retval None
  */
void RTIM_CCxInit(RTIM_TypeDef* TIMx, TIM_CCInitTypeDef* TIM_CCInitStruct, u16 TIM_Channel)
{
	/* Check the parameters */
	assert_param(IS_TIM_CCM_TIM(TIMx)); 
	assert_param(IS_TIM_CC_MODE(TIM_CCInitStruct->TIM_CCMode));
	assert_param(IS_TIM_CC_POLARITY(TIM_CCInitStruct->TIM_CCPolarity));
	assert_param(IS_TIM_OCPRELOAD_STATE(TIM_CCInitStruct->TIM_OCProtection));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));

	/* Reset the CCMR Bit */
	TIMx->CCMRx[TIM_Channel] = 0;

	/* Write to TIMx CCMR */
	if (IS_TIM_PWM_TIM(TIMx)) {
		TIMx->CCMRx[TIM_Channel] = (TIM_CCInitStruct->TIM_CCPolarity |
			TIM_CCInitStruct->TIM_OCProtection |			
			TIM_CCInitStruct->TIM_CCMode |
			TIM_CCInitStruct->TIM_OCPulse);
	} else if (IS_TIM_INPULSE_TIM(TIMx)) {
		TIMx->CCMRx[TIM_Channel] = (TIM_CCInitStruct->TIM_CCPolarity |
			TIM_CCInitStruct->TIM_ICPulseMode);
	}
}

/**
  * @brief  Initializes the TIMx Channel 0-5 CCmode.
  * @param  TIMx: where x can be 5, to select the TIM peripheral.
  * @param  TIM_Channel: the channel need to be set mode,
  *		which can be one of the following parameters @ref TIM_Channel_definitions.
  * @param  TIM_CCMode: CCx working mode which can be one of the following parameters:
  *		@arg TIM_CCMode_PWM
  *		@arg TIM_CCMode_Inputcapture
  * @note  
  *		- TIM4/TIMM04 only has 1 channel: TIM_Channel_0
  *		- TIMM05 has 6 channels:  TIM_Channel_0-5.
  *		- TIM5 has 18 channels:  TIM_Channel_0-17.
  * @retval None
  */
void RTIM_CCRxMode(RTIM_TypeDef* TIMx, u16 TIM_Channel, u32 TIM_CCMode)
{
	u32 tmpccmr = TIMx->CCMRx[TIM_Channel];
	
	/* Check the parameters */
	assert_param(IS_TIM_PWM_TIM(TIMx)); 
	assert_param(IS_TIM_CC_MODE(TIM_CCMode));

	tmpccmr &= ~TIM_CCER_CCxM;
	tmpccmr |= TIM_CCMode;
	
	/* Write to TIMx CCMR */
	TIMx->CCMRx[TIM_Channel] = tmpccmr;
}

/**
  * @brief  Sets the TIMx Capture Compare X register value
  * @param  TIMx: where x can be 5, to select the TIM peripheral.
  * @param  Compare: the value specifies pulsewidth, which is in the 0x00~0xFFFF range.
  *					Duty cycle = Compare / (ARR+1).
  * @param  TIM_Channel: the channel to be set,
  *		which can be one of the following parameters @ref TIM_Channel_definitions.
  * @retval None
  * @note  
  *		- CCRx=0 will give 0% cycle pwm pulse.
  *		- CCRx>=TIM_Period there will be 100% pwm pulse.
  *		- TIMM05 has 6 channels:  TIM_Channel_0-5.
  *		- TIM5 has 18 channels:  TIM_Channel_0-17.
  */
void RTIM_CCRxSet(RTIM_TypeDef* TIMx, u32 Compare, u16 TIM_Channel)
{
	u32 PulseWidth = 0;
	u32 tmpccmr = 0;

	/* Check the parameters */
	assert_param(IS_TIM_PWM_TIM(TIMx));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));
	assert_param(IS_TIM_CC_PULSEWIDTH(Compare));

	/* CCRx=0 will give 0% cycle pwm pulse */
	PulseWidth = Compare;

	tmpccmr = TIMx->CCMRx[TIM_Channel];
	
	/* reset Compare val */
	tmpccmr &= ~TIM_CCMode_CCR;
		
	/* Set the Capture CompareX Register value */
	tmpccmr |= (u32)PulseWidth;

	/* set CCMR */
	TIMx->CCMRx[TIM_Channel] = tmpccmr;
}

/**
  * @brief  Gets the TIMx Capture Compare X register value.
  * @param  TIMx: where x can be 4/5, to select the TIM peripheral.
  * @param  TIM_Channel: the channel to be read,
  *		which can be one of the following parameters @ref TIM_Channel_definitions.
  * @retval  Capture Compare x Register value.
  * @note	If you want to get pulse width of PWM, remember to plus 1 to 
  *			the retval of this function.
  *		- TIMM05 has 6 channels:  TIM_Channel_0-5.
  *		- TIM5 has 18 channels:  TIM_Channel_0-17.
  */
u32 RTIM_CCRxGet(RTIM_TypeDef* TIMx, u16 TIM_Channel)
{
	/* Check the parameters */
	assert_param(IS_TIM_CCM_TIM(TIMx)); 
	assert_param(IS_TIM_CHANNEL(TIM_Channel));

	/* Get the Capture Compare x Register value */
	return (TIMx->CCMRx[TIM_Channel] & TIM_CCMode_CCR);
}

/**
  * @brief  Enables or disables the TIMx peripheral Preload register on CCRx.
  * @param  TIMx: where x can be 5, to select the TIM peripheral.
  * @param  TIM_OCProtection: new state of the TIMx peripheral Preload register
  *          This parameter can be one of the following values:
  *            @arg TIM_OCPreload_Enable: value is loaded in the active register at each update event.
  *            @arg TIM_OCPreload_Disable: new value is taken in account immediately
  * @param  TIM_Channel: the channel need to be set,
  *		which can be one of the following parameters @ref TIM_Channel_definitions
  * @note  
  *		- TIMM05 has 6 channels:  TIM_Channel_0-5.
  *		- TIM5 has 18 channels:  TIM_Channel_0-17.
  * @retval None
  */
void RTIM_OCxPreloadConfig(RTIM_TypeDef* TIMx, u32 TIM_OCProtection, u16 TIM_Channel)
{
	u32 tmpccmr = 0;

	/* Check the parameters */
	assert_param(IS_TIM_PWM_TIM(TIMx));
	assert_param(IS_TIM_OCPRELOAD_STATE(TIM_OCProtection));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));

	tmpccmr = TIMx->CCMRx[TIM_Channel];

	/* Reset the OC1PE Bit */
	tmpccmr &= ~(TIM_OCER_CCxPE);

	/* Enable or Disable the Output Compare Preload feature */
	tmpccmr |= TIM_OCProtection;

	/* Write to TIMx CCMR1 register */
	TIMx->CCMRx[TIM_Channel] = tmpccmr;
}

/**
  * @brief  Configures the TIMx channel x polarity.
  * @param  TIMx: where x can be 5, to select the TIM peripheral.
  * @param  TIM_OCPolarity: specifies the OCx Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM_CCPolarity_High: Output Compare active high
  *            @arg TIM_CCPolarity_Low: Output Compare active low
  * @retval None
  * @note
  *		-TIMM05 have CCR0-5
  *		-TIM5 have CCR0-17
  *		-TIM4/TIMM04 have CCR0
  *		-TIM0-3/TIMM00-3 dont have CCMR
  */
void RTIM_CCxPolarityConfig(RTIM_TypeDef* TIMx, u32 TIM_OCPolarity, u16 TIM_Channel)
{
	u32 tmpccmr = 0;

	/* Check the parameters */
	assert_param(IS_TIM_PWM_TIM(TIMx));
	assert_param(IS_TIM_CC_POLARITY(TIM_OCPolarity));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));
	
	tmpccmr = TIMx->CCMRx[TIM_Channel];

	/* Set or Reset the CCxP Bit */
	tmpccmr &= ~(TIM_CCER_CCxP);
	tmpccmr |= TIM_OCPolarity;

	/* Write to TIMx CCER register */
	TIMx->CCMRx[TIM_Channel] = tmpccmr;
}

/**
  * @brief  Enables or disables the TIM Capture Compare Channel x.
  * @param  TIMx: where x can be 4/5, to select the TIM peripheral.
  * @param  TIM_Channel: specifies the TIM Channel
  *          This parameter can be one of the following values @ref TIM_Channel_definitions
  * @param  TIM_CCx: specifies the TIM Channel CCxE bit new state.
  *          This parameter can be one of the following values: 
  *		@arg TIM_CCx_Enable
  *		@arg TIM_CCx_Disable
  * @note  
  *		- TIMM05 has 6 channels:  TIM_Channel_0-5.
  *		- TIM5 has 18 channels:  TIM_Channel_0-17.
  * @retval None
  */
void RTIM_CCxCmd(RTIM_TypeDef* TIMx, u16 TIM_Channel, u32 TIM_CCx)
{
	u32 tmpccmr = TIMx->CCMRx[TIM_Channel];

	/* Check the parameters */
	assert_param(IS_TIM_CCM_TIM(TIMx)); 
	assert_param(IS_TIM_CHANNEL(TIM_Channel));
	assert_param(IS_TIM_CCX(TIM_CCx));

	tmpccmr &= ~TIM_CCER_CCxE;
	tmpccmr |= TIM_CCx;

	/* Set or reset the CCxE Bit */ 
	TIMx->CCMRx[TIM_Channel] =  tmpccmr;
}

/**
  * @brief  Set the TIMx's One Pulse Mode (output one pulse PWM mode).
  * @param  TIMx: where x can be 5 to select the TIM peripheral.
  * @param  TIM_OPMode: specifies the OPM Mode to be used.
  *          This parameter can be one of the following values:
  *            @arg TIM_OPMode_Single
  *            @arg TIM_OPMode_Repetitive
  * @param  TrigerPolarity: specifies the OPM Mode Triger Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM_OPMode_ETP_positive
  *            @arg TIM_OPMode_ETP_negative
  * @note  You must select  TIM_OPMode_Single if you want to set One Pluse Mode,
  *		which makes the counter stop automatically at the next UEV.
  * @retval None
  */
void RTIM_SetOnePulseOutputMode(RTIM_TypeDef* TIMx, u32 TIM_OPMode, u32 TrigerPolarity)
{
	/* Check the parameters */
	assert_param(IS_TIM_ONE_PULSE_TIM(TIMx));
	assert_param(IS_TIM_OPM_MODE(TIM_OPMode));
	assert_param(IS_TIM_OPM_ETP_MODE(TrigerPolarity));

	/* Reset the OPM & ETP Bit */
	TIMx->CR &= (u32)~(TIM_CR_OPM | TIM_CR_ETP);

	/* Configure the OPM Mode */
	TIMx->CR |= TIM_OPMode | TrigerPolarity;
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
