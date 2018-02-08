/**
  ******************************************************************************
  * @file    rtl8711b_tim.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the Timer firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8711B_TIMER_H_
#define _RTL8711B_TIMER_H_

/** @addtogroup AmebaZ_Periph_Driver
  * @{
  */

/** @defgroup Timer 
  * @brief Timer driver modules
  * @{
  */

  /** @addtogroup Timer
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * TIM0/TIM1/TIM2/TIM3: used as generic timers for time-base generation 
  *		- Base Address: TIM0/TIM1/TIM2/TIM3
  *		- Clock Source: 32kHz
  *		- Resolution: 32bit
  *		- Count Mode: Upcounting
  *		- Interrupt Generation
  *		- Support Upcounting mode
  * TIM4:
  *		- Base Address: TIM4
  *		- Channels: 1
  *		- Clock Source: XTAL, normally is 40MHz
  *		- Resolution: 16bit
  *		- Prescaler: 8bit
  *		- Count Mode: Upcounting
  *		- Input Pin: 1 input capture
  *		- Interrupt/DMA Generation
  *		- Support Upcounting mode/Statistic Pulse Width mode/Statistic Pulse Number mode
  * TIM5:
  *		- Base Address: TIM5
  *		- Channels: 6 
  *		- Clock Source: XTAL, normally is 40MHz
  *		- Resolution: 16bit
  *		- Prescaler: 8bit
  *		- Count Mode: Upcounting
  *		- Interrupt/DMA Generation
  *		- Input Pin: 1 input capture
  *		- Ouput Pin: 6 PWM out
  *		- Support Upcounting mode/Input capture mode/PWM mode/One Pulse mode
  *
  *****************************************************************************************
  * Sys Timer
  *****************************************************************************************
  * 	- TIM0 is used as systimer, so TIM0 can not be used for other purpose, see DELAY charpter for more information
  *
  *****************************************************************************************
  * PIN LOCATION
  *****************************************************************************************
  * TIM4:
  *		- Input Pin (TRIG):  PA_18
  *
  * TIM5:
  *		- Input Pin (TRIG):  PA_19
  *		- Output Pin:
  *            channel 0    channel 1    channel 2    channel 3    channel 4    channel 5
  *       S0:   PA_23        PA_15        PA_0         PA_30        PA_13        PA_22
  *       S1:   PA_14        PA_16        PA_17        PA_12        PA_5         NULL
  *       S2:   NULL         NULL         NULL         PA_21        PA_29        NULL
  *		
  *****************************************************************************************
  * Upcounting mode
  *****************************************************************************************
  * TIM0~5 support
  * The counter counts from 0 to the auto-reload value (content of the TIMx_ARR register), 
  * then restarts from 0 and generates a counter overflow interrupt.
  *
  *****************************************************************************************
  * Statistic Pulse Width mode
  *****************************************************************************************
  * Only TIM4 support
  * TIM4 can statistic the width of active level of TRGI. When the TRGI is transferred from 
  * inactive level to active level, the counter is enabled automatically and counter starts 
  * from 0. When the TRGI is transferred from active level to inactive level , the counter 
  * is disabled automatically, the current counter value will be copied to CCR0 field of 
  * TIMx_CCR0, the CCxIF will be set and an interrupt or a DMA request can be sent if they 
  * are enabled.
  *
  *****************************************************************************************
  * Statistic Pulse Number mode
  *****************************************************************************************
  * Only TIM4 support
  * TIM4 can statistic the number of active edge of TRGI in the given period. When the  
  * counter overflow, the number will be copied to CCR0 field of TIMx_CCR0, the CCxIF will 
  * be set and an interrupt or a DMA request can be sent if they  are enabled
  *
  *****************************************************************************************
  * Input capture mode
  *****************************************************************************************
  * Only TIM5 supports
  * In input capture mode, the CCRx field of TIMx_CCRx are used to latch the value of the  
  * counter after a transition detected by the TRGI signal. When a capture occurs, the 
  * corresponding CCXIF flag (TIMx_SR register) is set and an interrupt or a DMA request  
  * can be sent if they are enabled.
  *
  *****************************************************************************************
  * PWM mode
  *****************************************************************************************
  * Only TIM5 supports
  * Pulse Width Modulation mode allows you to generate a signal with a frequency determined  
  * by the value of the TIMx_ARR register and a duty cycle determined by the value of the 
  * CCRx field of TIMx_CCRx register.
  * Period = (ARR + 1)*Tcnt, Duty cycle = (CCRx+1)*Tcnt/Period, where Tcnt = Txtal *(PSC+1).
  *
  *****************************************************************************************
  * One Pulse mode
  *****************************************************************************************
  * Only TIM5 supports
  * This mode allows the counter to be started in response to a stimulus and to generate  
  * a pulse with a programmable length after a programmable delay. Starting the counter can 
  * be controlled through the active edge of TRGI. Generating the waveform can be done in PWM 
  * mode. You select One-pulse mode by setting the OPM bit in the TIMx_CR register. This makes 
  * the counter stop automatically at the next update event.
  *
  * NOTICE: you shoud pull up/down the pwm pin (PAD_PullCtrl) when you use one pulse mode based on polarity
  *
  *****************************************************************************************
  * How to use Base Timer
  *****************************************************************************************
  *      To use the Timer in Timing(Time base) mode, the following steps are mandatory:
  *       
  *      1. Enable TIM clock :
  *			RCC_PeriphClockCmd(APBPeriph_GTIMER, APBPeriph_GTIMER_CLOCK, ENABLE)
  *
  *      2. Fill the TIM_InitStruct with default parameters using:
  *			RTIM_TimeBaseStructInit(&TIM_InitStruct) 
  *			or setting the desired parameters manually.
  *        
  *      3. Configure  the Time Base unit with the corresponding configurations, register TimerIRQHandler  
  *      and enable the NVIC if you need to generate the update interrupt. 
  *			RTIM_TimeBaseInit(TIMx, &TIM_InitStruct, IrqNum, UserCB, UserCBData)
  *          
  *      4. Enable the corresponding interrupt using :
  *			RTIM_INTConfig(TIMx, TIM_IT_Update, ENABLE) 
  * 
  *      5. Enable the TIM counter.
  *			RTIM_Cmd(TIMx, ENABLE)
  *             
  *      Note1: All other functions can be used separately to modify, if needed,
  *			a specific feature of the Timer. 
  *
  *****************************************************************************************     
  * How to use Timer in Capture Compare Mode
  ***************************************************************************************** 
  *      To use the Timer in CC mode, the following steps are mandatory:
  *       
  *      1. Enable TIM clock :
  *			RCC_PeriphClockCmd(APBPeriph_GTIMER, APBPeriph_GTIMER_CLOCK, ENABLE)
  *       
  *      2. Configure the TIM pinmux:
  *		Pinmux_Config(PinName, PinFunc)
  *       
  *      3. Configure the Time base unit as described in the first part of this driver if needed, 
  *			else the Timer will run with the default configuration:
  *			- Autoreload value = 0xFFFF
  *			- Prescaler value = 0x0000
  *          
  *      4. Fill the TIM_CCInitStruct with the desired parameters including:
  *			- The TIM Output Compare mode: TIM_CCMode
  *			- TIM Output Compare Pulse value: TIM_OCMPulse
  *			- TIM Output Compare Polarity : TIM_CCPolarity
  *			- TIM Output Compare value update protection: TIM_OCProtection
  *       
  *      5. Configure the desired channel with the corresponding configuration
  *			RTIM_CCxInit(TIMx, &TIM_CCInitStruct, TIM_Channel)
  *       
  *      6. Enable corresponding TIM channel.
  *			RTIM_CCxCmd(TIMx, TIM_Channel, TIM_CCx_Enable)
  *
  *      7. Enable the TIM counter.
  *			RTIM_Cmd(TIMx, ENABLE)
  *       
  *      Note1: All other functions can be used separately to modify, if needed,
  *			a specific feature of the Timer. 
  *          
  *      Note2: In case of PWM mode, the TIMx peripheral Preload register on CCRx(TIM_OCProtection)
  *			should be enabled.
  *              
  *      Note3: If the corresponding interrupt or DMA request are needed, the user should:
  *			1. Enable the NVIC (or the DMA) to use the TIM interrupts (or DMA requests). 
  *			2. Enable the corresponding interrupt (or DMA request) using the function 
  *				RTIM_ITConfig(TIMx, TIM_IT_CCx, ENABLE) (or RTIM_DMACmd(TIMx, TIM_DMA_CCx, ENABLE))  
  *			3. GDMA related configurations(source address/destination address/block size etc.)
  *				if needed, by calling:
  *				GDMA_ChnlAlloc()
  *				GDMA_Init()
  *				GDMA_Cmd()
  *
  * @endverbatim
  */
  
/* Exported types ------------------------------------------------------------*/

/** @defgroup TIM_Exported_Types TIM Exported Types
  * @{
  */

/** 
  * @brief  TIM Basic Init structure definition  
  */
typedef struct {
	u32 TIM_Prescaler;		/*!< Specifies the prescaler value used to divide the TIM clock.
							This parameter can be a number between 0x00 and 0xFF, basic timer dont care */
	u32 TIM_Period;			/*!< Specifies the period value to be loaded into the active
							Auto-Reload Register at the next update event.
							This parameter is 16bits for TIM4-5, and 32bits for TIM0-TIM3
							you can get it from SourceClock & TIM_Prescaler */
	u32 TIM_UpdateEvent;		/*!< Specifies whether or not to enable update event(UEV).
								This parameter can be ENABLE or DISABLE. ENABLE means UEV Enable, DISABLE means UEV Disable*/
	u32 TIM_UpdateSource;		/*!< Specifies the update request source. This parameter can be 
								TIM_UpdateSource_Overflow or TIM_UpdateSource_Global.
								TIM_UpdateSource_Overflow means counter overflow generates an update event(UEV).
								TIM_UpdateSource_Global means both counter overflow and setting the UG bit can generate UEV.*/
	u32 TIM_ARRProtection;	/*!< DISABLE or ENABLE, when ENABLE: period will update when cnt = 0(counter overflow, an UEV happens), 
							or  period will update immediatly */

	u8	TIM_Idx;				/*!< 0~5 */
} RTIM_TimeBaseInitTypeDef;

/** 
  * @brief  TIM Output Compare Init structure definition  
  */
typedef struct
{
	u32 TIM_CCMode;        /*!< Specifies the TIM5 mode. This parameter can be a value of TIM_CCMode_PWM or TIM_CCMode_Inputcapture */						
	u32 TIM_CCPolarity;    /*!< Specifies the polarity. This parameter can be TIM_CCPolarity_High/TIM_CCPolarity_Low.
								If CCx channel is configured as output:
									TIM_CCPolarity_High means OCx active high.
									TIM_CCPolarity_Low means OCx active low.
								If CCx channel is configured as input:
									TIM_CCPolarity_High means positive edge of TRGI is active for capture.
									TIM_CCPolarity_Low means negative edge of TRGI is active for capture. */

	u32 TIM_OCProtection; 	/*!< Output Compare value update protection. TIM_OCPreload_Enable/TIM_OCPreload_Disable.
							TIM_OCPreload_Enable means duty cycle will update when UEV happens if write to CCRx field in TIMx_CCRX.
							TIM_OCPreload_Disable means duty cycle will update immediately if write to CCRx field in TIMx_CCRX.*/

	u32 TIM_OCPulse;        /*!< Specifies the output pulse value to be loaded into the CCRx Register, which decides the duty cycle.
							This parameter can be a number between 0x0000 and 0xFFFF */

	u32 TIM_ICPulseMode;	 /*!< Specifies the TIM4 mode, TIM_CCMode_PulseWidth or TIM_CCMode_PulseNumber */
} TIM_CCInitTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup TIM_Exported_constants TIM Exported constants
  * @{
  */

/** @defgroup TIM_Type_definitions
  * @{
  */
#define IS_TIM_ALL_TIM(PERIPH) (((PERIPH) == TIM0) || \
                                   ((PERIPH) == TIM1) || \
                                   ((PERIPH) == TIM2) || \
                                   ((PERIPH) == TIM3) || \
                                   ((PERIPH) == TIM4) || \
                                   ((PERIPH) == TIM5))

#define IS_TIM_ONE_PULSE_TIM(PERIPH) ((PERIPH) == TIM5)

#define IS_TIM_CCM_TIM(PERIPH) (((PERIPH) == TIM4) || \
                                   ((PERIPH) == TIM5))
                                   
#define IS_TIM_PWM_TIM(PERIPH) ((PERIPH) == TIM5)

#define IS_TIM_INPULSE_TIM(PERIPH) ((PERIPH) == TIM4)
                                     
#define IS_TIM_DMA_TIM(PERIPH) (((PERIPH) == TIM4) || \
                                     ((PERIPH) == TIM5))


#define IS_TIM_40M_TIM(PERIPH) (((PERIPH) == TIM4) || \
                                     ((PERIPH) == TIM5))
/**
  * @}
  */

/** @defgroup TIM_Channel_definitions
  * @{
  */
#define TIM_Channel_0                      ((u16)0x0000)
#define TIM_Channel_1                      ((u16)0x0001)
#define TIM_Channel_2                      ((u16)0x0002)
#define TIM_Channel_3                      ((u16)0x0003)
#define TIM_Channel_4                      ((u16)0x0004)
#define TIM_Channel_5                      ((u16)0x0005)

#define IS_TIM_CHANNEL(CHANNEL) (((CHANNEL) == TIM_Channel_0) || \
                                 ((CHANNEL) == TIM_Channel_1) || \
                                 ((CHANNEL) == TIM_Channel_2) || \
                                 ((CHANNEL) == TIM_Channel_3) || \
                                 ((CHANNEL) == TIM_Channel_4) || \
                                 ((CHANNEL) == TIM_Channel_5))
                                 
/**
  * @}
  */

/** @defgroup TIMx_Prescaler_definitons
  * @{
  */
#define IS_TIM_PSC(VAL) (VAL <= 0xFF)
/**
  * @}
  */
                                      
/** @defgroup TIMx_Control_Bit_definitions
  * @{
  */
#define TIM_OPMode_ETP_positive		((u32)0x00000000)
#define TIM_OPMode_ETP_negative	((u32)0x000000100)
#define IS_TIM_OPM_ETP_MODE(MODE) (((MODE) == TIM_OPMode_ETP_positive) || \
                               ((MODE) == TIM_OPMode_ETP_negative))

#define TIM_OPMode_Single			((u32)0x00000008)
#define TIM_OPMode_Repetitive		((u32)0x00000000) /* repeative is PWM mode */
#define IS_TIM_OPM_MODE(MODE) (((MODE) == TIM_OPMode_Single) || \
                               ((MODE) == TIM_OPMode_Repetitive))

#define TIM_UpdateSource_Global		((u32)0x00000000) /*!< Source of update is the counter overflow or the setting of UG bit. */
#define TIM_UpdateSource_Overflow	((u32)0x00000004) /*!< Source of update is counter overflow. */
#define IS_TIM_UPDATE_SOURCE(SOURCE) (((SOURCE) == TIM_UpdateSource_Global) || \
                                      ((SOURCE) == TIM_UpdateSource_Overflow))
/**
  * @}
  */ 

/** @defgroup TIMx_DMA_and_Interrupt_Enable_definitions
  * @{
  */

#define TIM_IT_Update			((u32)0x00000001)
#define TIM_IT_CC0				((u32)0x00000002)
#define TIM_IT_CC1				((u32)0x00000004)
#define TIM_IT_CC2				((u32)0x00000008)
#define TIM_IT_CC3				((u32)0x00000010)
#define TIM_IT_CC4				((u32)0x00000020)
#define TIM_IT_CC5				((u32)0x00000040)
#define IS_TIM_IT(IT) ((((IT) & (u32)0xFF80) == 0x0000) && (((IT) & (u32)0x7F) != 0x0000))

#define IS_TIM_GET_IT(IT) (((IT) == TIM_IT_Update) || \
                           ((IT) == TIM_IT_CC0) || \
                           ((IT) == TIM_IT_CC1) || \
                           ((IT) == TIM_IT_CC2) || \
                           ((IT) == TIM_IT_CC3) || \
                           ((IT) == TIM_IT_CC4) || \
                           ((IT) == TIM_IT_CC5))

#define TIM_DMA_Update			((u32)0x00000100)
#define TIM_DMA_CC0			((u32)0x00000200)
#define TIM_DMA_CC1			((u32)0x00000400)
#define TIM_DMA_CC2			((u32)0x00000800)
#define TIM_DMA_CC3			((u32)0x00001000)
#define TIM_DMA_CC4			((u32)0x00002000)
#define TIM_DMA_CC5			((u32)0x00004000)
#define IS_TIM_DMA_SOURCE(SOURCE) ((((SOURCE) & (u32)0x80FF) == 0x0000) && ((SOURCE) != 0x0000))

/**
  * @}
  */ 


/** @defgroup TIMx_Event_Generation_definitons
  * @{
  */
#define TIM_PSCReloadMode_Update		((u32)0x00000000)
#define TIM_PSCReloadMode_Immediate	((u32)0x00000001)
#define IS_TIM_PRESCALER_RELOAD(RELOAD) (((RELOAD) == TIM_PSCReloadMode_Update) || \
                                         ((RELOAD) == TIM_PSCReloadMode_Immediate))

#define TIM_EventSource_Update		((u32)0x00000001)
#define TIM_EventSource_CC0			((u32)0x00000002)
#define TIM_EventSource_CC1			((u32)0x00000004)
#define TIM_EventSource_CC2			((u32)0x00000008)
#define TIM_EventSource_CC3			((u32)0x00000010)
#define TIM_EventSource_CC4			((u32)0x00000020)
#define TIM_EventSource_CC5			((u32)0x00000040)
#define IS_TIM_EVENT_SOURCE(SOURCE) ((((SOURCE) & 0xFF80) == 0x0000) && \
						(((SOURCE) & 0x7F) != 0x0000))                                          
  
/**
  * @}
  */ 


/** @defgroup TIMx_Capture_Compare_definitions
  * @{
  */

#define TIM_CCx_Enable				((u32)0x01000000)
#define TIM_CCx_Disable				((u32)0x00000000)
#define IS_TIM_CCX(CCX) (((CCX) == TIM_CCx_Enable) || ((CCX) == TIM_CCx_Disable))

#define TIM_OCPreload_Enable		((u32)0x02000000)
#define TIM_OCPreload_Disable		((u32)0x00000000)
#define IS_TIM_OCPRELOAD_STATE(STATE) (((STATE) == TIM_OCPreload_Enable) || \
                                       ((STATE) == TIM_OCPreload_Disable))

#define TIM_CCPolarity_High			((u32)0x00000000) /*!< if input is set :  Positive edge of TRGI is active for capture */
#define TIM_CCPolarity_Low			((u32)0x04000000) /*!< if input is set :  negative edge of TRGI is active for capture */
#define IS_TIM_CC_POLARITY(POLARITY) (((POLARITY) == TIM_CCPolarity_High) || \
                                      ((POLARITY) == TIM_CCPolarity_Low))

/* TIM5 PWM or Inputcapture mode */
#define TIM_CCMode_PWM			((u32)0x00000000)
#define TIM_CCMode_Inputcapture		((u32)0x08000000)
#define IS_TIM_CC_MODE(MODE) (((MODE) == TIM_CCMode_PWM) || \
                              ((MODE) == TIM_CCMode_Inputcapture))

/* TIM4 pulse mode */      
#define TIM_CCMode_PulseWidth		((u32)0x00000000)
#define TIM_CCMode_PulseNumber	((u32)0x10000000)
#define IS_TIM_CC_PULSEMODE(MODE) (((MODE) == TIM_CCMode_PulseWidth) || \
                              ((MODE) == TIM_CCMode_PulseNumber))

#define TIM_CCMode_CCR				((u32)0x0000FFFF)
#define IS_TIM_CC_PULSEWIDTH(Compare) ((Compare) <= TIM_CCMode_CCR)
/**
  * @}
  */

/**
  * @}
  */
  
/** @defgroup TIM_Exported_Functions TIM Exported Functions
  * @{
  */

  /** @defgroup TimeBase_Management_Functions TimeBase Management Functions
  * @{
  */
_LONG_CALL_ void RTIM_TimeBaseStructInit(RTIM_TimeBaseInitTypeDef* TIM_InitStruct);
_LONG_CALL_ void RTIM_TimeBaseInit(RTIM_TypeDef* TIMx, RTIM_TimeBaseInitTypeDef* TIM_InitStruct, IRQn_Type   IrqNum, IRQ_FUN UserCB, u32 UserCBData);
_LONG_CALL_ void RTIM_Cmd(RTIM_TypeDef* TIMx, u32 NewState);
_LONG_CALL_ void RTIM_DeInit(RTIM_TypeDef* TIMx);
_LONG_CALL_ u32 RTIM_GetCount(RTIM_TypeDef* TIMx);
_LONG_CALL_ void RTIM_UpdateDisableConfig(RTIM_TypeDef* TIMx, u32 NewState);
_LONG_CALL_ void RTIM_ARRPreloadConfig(RTIM_TypeDef* TIMx, u32 NewState);
_LONG_CALL_ void RTIM_UpdateRequestConfig(RTIM_TypeDef* TIMx, u32 TIM_UpdateSource);
_LONG_CALL_ void RTIM_PrescalerConfig(RTIM_TypeDef* TIMx, u32 Prescaler, u32 TIM_PSCReloadMode);
_LONG_CALL_ void RTIM_GenerateEvent(RTIM_TypeDef* TIMx, u32 TIM_EventSource);
_LONG_CALL_ void RTIM_ChangePeriod(RTIM_TypeDef* TIMx, u32 Autoreload);
_LONG_CALL_ void RTIM_Reset(RTIM_TypeDef* TIMx);
/**
  * @}
  */

  /** @defgroup Capture_Compare_Management_Functions Capture Compare Management Functions
  * @{
  */
_LONG_CALL_ void RTIM_CCStructInit(TIM_CCInitTypeDef* TIM_CCInitStruct);
_LONG_CALL_ void RTIM_CCxInit(RTIM_TypeDef* TIMx, TIM_CCInitTypeDef* TIM_CCInitStruct, u16 TIM_Channel);
_LONG_CALL_ void RTIM_CCRxMode(RTIM_TypeDef* TIMx, u16 TIM_Channel, u32 TIM_CCMode);
_LONG_CALL_ void RTIM_CCRxSet(RTIM_TypeDef* TIMx, u32 Compare, u16 TIM_Channel);
_LONG_CALL_ u32 RTIM_CCRxGet(RTIM_TypeDef* TIMx, u16 TIM_Channel);
_LONG_CALL_ void RTIM_OCxPreloadConfig(RTIM_TypeDef* TIMx, u32 TIM_OCProtection, u16 TIM_Channel);
_LONG_CALL_ void RTIM_CCxPolarityConfig(RTIM_TypeDef* TIMx, u32 TIM_OCPolarity, u16 TIM_Channel);
_LONG_CALL_ void RTIM_CCxCmd(RTIM_TypeDef* TIMx, u16 TIM_Channel, u32 TIM_CCx);
_LONG_CALL_ void RTIM_SetOnePulseOutputMode(RTIM_TypeDef* TIMx, u32 TIM_OPMode, u32 TrigerPolarity);
/**
  * @}
  */

  /** @defgroup Interrupt_and_DMA_Management_Functions Interrupt and DMA Management Functions
  * @{
  */
_LONG_CALL_ void RTIM_DMACmd(RTIM_TypeDef* TIMx, u16 TIM_DMASource, u32 NewState);
_LONG_CALL_ void RTIM_INTConfig(RTIM_TypeDef* TIMx, u32 TIM_IT, u32 NewState);
_LONG_CALL_ void RTIM_INTClear(RTIM_TypeDef* TIMx);
_LONG_CALL_ void RTIM_INTClearPendingBit(RTIM_TypeDef* TIMx, u16 TIM_IT);
_LONG_CALL_ u32 RTIM_GetFlagStatus(RTIM_TypeDef* TIMx, u32 TIM_FLAG);
_LONG_CALL_ u32 RTIM_GetINTStatus(RTIM_TypeDef* TIMx, u32 TIM_IT);
_LONG_CALL_ u32 RTIM_TXGDMA_Init(u32 TIM_Channel, GDMA_InitTypeDef *GdmaInitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8* pDataBuf, u32 DataLen);
_LONG_CALL_ u32 RTIM_RXGDMA_Init(u32 TIM_Idx, u32 TIM_Channel, GDMA_InitTypeDef *GdmaInitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pDataBuf, int DataLen);
/**
  * @}
  */
  
/**
  * @}
  */


/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup TIM_Register_Definitions TIM Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup TIM_EN
 * @{
 *****************************************************************************/
#define  TIM_CR_CNT_START			((u32)0x00000001)            /*!<Counter start */
#define  TIM_CR_CNT_STOP			((u32)0x00000002)           	 /*!<Counter stop */
#define  TIM_CR_CNT_RUN				((u32)0x00000100)            /*!<Counter run status. polling bit */
#define  TIM_CR_CNT_STS				((u32)0x00010000)            /*!<Counter working status, polling bit */
/** @} */

/**************************************************************************//**
 * @defgroup TIM_CR
 * @{
 *****************************************************************************/
#define  TIM_CR_UDIS				((u32)0x00000002)            /*!<Update disable */
#define  TIM_CR_URS					((u32)0x00000004)            /*!<Update request source */
#define  TIM_CR_OPM					((u32)0x00000008)            /*!<One pulse mode */
#define  TIM_CR_ARPE				((u32)0x00000010)            /*!<Auto-reload preload enable */
#define  TIM_CR_ETP					((u32)0x00000100)            /*!<External trigger polarity for one pulse mode (TRGI). */
/** @} */

/**************************************************************************//**
 * @defgroup TIM_DIER
 * @{
 *****************************************************************************/
/* Interrupt EN */
#define  TIM_DIER_UIE				((u32)0x00000001)            /*!<Update interrupt enable */
#define  TIM_DIER_CC0IE				((u32)0x00000002)            /*!<Capture/Compare 0 interrupt enable */
#define  TIM_DIER_CC1IE				((u32)0x00000004)            /*!<Capture/Compare 1 interrupt enable */
#define  TIM_DIER_CC2IE				((u32)0x00000008)            /*!<Capture/Compare 2 interrupt enable */
#define  TIM_DIER_CC3IE				((u32)0x00000010)            /*!<Capture/Compare 3 interrupt enable */
#define  TIM_DIER_CC4IE				((u32)0x00000020)            /*!<Capture/Compare 4 interrupt enable */
#define  TIM_DIER_CC5IE				((u32)0x00000040)            /*!<Capture/Compare 5 interrupt enable */

/* DMA EN */
#define  TIM_DIER_UDE				((u32)0x00000100)            /*!<Update DMA request enable */
#define  TIM_DIER_CC0DE				((u32)0x00000200)            /*!<Capture/Compare 0 DMA request enable */
#define  TIM_DIER_CC1DE				((u32)0x00000400)            /*!<Capture/Compare 1 DMA request enable */
#define  TIM_DIER_CC2DE				((u32)0x00000800)            /*!<Capture/Compare 2 DMA request enable */
#define  TIM_DIER_CC3DE				((u32)0x00001000)            /*!<Capture/Compare 3 DMA request enable */
#define  TIM_DIER_CC4DE				((u32)0x00002000)            /*!<Capture/Compare 4 DMA request enable */
#define  TIM_DIER_CC5DE				((u32)0x00004000)            /*!<Capture/Compare 5 DMA request enable */
/** @} */

/**************************************************************************//**
 * @defgroup TIM_SR
 * @{
 *****************************************************************************/
#define  TIM_SR_UIF					((u32)0x00000001)            /*!<Update interrupt Flag */
#define  TIM_SR_CC0IF				((u32)0x00000002)            /*!<Capture/Compare 0 interrupt Flag */
#define  TIM_SR_CC1IF				((u32)0x00000004)            /*!<Capture/Compare 1 interrupt Flag */
#define  TIM_SR_CC2IF				((u32)0x00000008)            /*!<Capture/Compare 2 interrupt Flag */
#define  TIM_SR_CC3IF				((u32)0x00000010)            /*!<Capture/Compare 3 interrupt Flag */
#define  TIM_SR_CC4IF				((u32)0x00000020)            /*!<Capture/Compare 4 interrupt Flag */
#define  TIM_SR_CC5IF				((u32)0x00000040)            /*!<Capture/Compare 5 interrupt Flag */
#define  TIM_SR_UG_DONE			((u32)0x00010000)            /*!<UG operation status for TIMx_EGR UG bit, polling bit */
/** @} */

/**************************************************************************//**
 * @defgroup TIM_EGR
 * @{
 *****************************************************************************/
#define  TIM_EGR_UG					((u32)0x00000001)               /*!<Update Generation */
#define  TIM_EGR_CC0G				((u32)0x00000002)               /*!<Capture/Compare 0 Generation */
#define  TIM_EGR_CC1G				((u32)0x00000004)               /*!<Capture/Compare 1 Generation */
#define  TIM_EGR_CC2G				((u32)0x00000008)               /*!<Capture/Compare 2 Generation */
#define  TIM_EGR_CC3G				((u32)0x00000010)               /*!<Capture/Compare 3 Generation */
#define  TIM_EGR_CC4G				((u32)0x00000020)               /*!<Capture/Compare 4 Generation */
#define  TIM_EGR_CC5G				((u32)0x00000040)               /*!<Capture/Compare 5 Generation */
/** @} */

/**************************************************************************//**
 * @defgroup TIM_CCMR
 * @{
 *****************************************************************************/
#define  TIM_CCER_CCxE				((u32)0x01 << 24)		/*!<Capture/Compare x input/output enable */
#define  TIM_OCER_CCxPE				((u32)0x02 << 24)		/*!<Output Compare x Preload enable */
#define  TIM_CCER_CCxP				((u32)0x04 << 24)		/*!<Capture/Compare x input/output Polarity */
#define  TIM_CCER_CCxM				((u32)0x08 << 24)		/*!<CCx working mode input or output mode */
#define  TIM_ICER_CCxPULSE_MODE	((u32)0x10 << 24)		/*!<CCx input pulse mode: width or num, just CC1 valid */
/** @} */
/** @} */

  /**
  * @}
  */

  /**
  * @}
  */

extern int TIMx_irq[6];
extern RTIM_TypeDef* TIMx[6];
extern u32 TIM_IT_CCx[6];
extern u32 TIM_DMA_CCx[6];

#define TIMER_TICK_US		 			31
#define TIMER_TICK_US_X4				(4*1000000/32000) //32k clock, 31.25us every timer_tick
#endif //_RTL8711B_TIMER_H_

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
