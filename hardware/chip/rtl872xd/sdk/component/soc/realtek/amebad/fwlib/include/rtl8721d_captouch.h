/**
  ******************************************************************************
  * @file    rtl8721d_captouch.h
  * @author
  * @version V1.0.0
  * @date    2017-10-16
  * @brief   This file contains all the functions prototypes for the captouch.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */


#ifndef _RTL8721D_CAPTOUCH_H_
#define _RTL8721D_CAPTOUCH_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @addtogroup CapTouch CapTouch
  * @{
  */

/** @addtogroup CapTouch
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * CAPTOUCH:
  *		- Base Address: CAPTOUCH_DEV
  *		- Clock source : 32.768KHz
  *		- Scan interval timer: 1.024KHz(32.768KHz/32)
  *		- Debounce Timer: Configurable
  *		- SocPs: Sleep Mode (clock gating & power gating)
  *		- IRQ: CapTouch_IRQ
  *
  *****************************************************************************************     
  * How to use Normal CapTouch
  *****************************************************************************************
  *      To use the normal CapTouch mode, the following steps are mandatory:
  *       
  *      1. Enable CapTouch peripheral clock
  *
  *      2. Configure the CapTouch pinmux.
  *        
  *      3. Init Captouch parameters:
  *			CapTouch_StructInit(CapTouch_InitTypeDef* CapTouch_InitStruct)
  *
  *      4. Init Hardware use step3 parameters:
  *			CapTouch_Init(CAPTOUCH_TypeDef *CapTouch, CapTouch_InitTypeDef* CapTouch_InitStruct)
  * 
  *      5. Enable the NVIC and the corresponding interrupt using following function if you need 
  *			to use interrupt mode. 
  *			CapTouch_INTConfig(): CapTouch IRQ Enable set
  *			CapTouch_INTMask(): CapTouch IRQ mask set
  *			InterruptRegister(): register the captouch irq handler 
  *			InterruptEn(): Enable the NVIC interrupt
  *
  *      6. Enable CapTouch module using CapTouch_Cmd().
  *
  *****************************************************************************************     
  * @endverbatim
  */
  
/* Exported types --------------------------------------------------------*/
/** @defgroup CapTouch_Exported_Types CapTouch Exported Types
  * @{
  */

/** 
  * @brief  CapTouch Channel Initialization structure definition
  */
typedef struct {

	u16 CT_DiffThrehold;	/*!< Difference threshold data of touch judgement for channelx :
							1. Configured during development; (0x0~0xFFF) (0~4095)
							2. Init number=0x0, need to be configured 
							3. recommend data=80%*(signal-baseline); */
					
	u8 CT_MbiasCurrent;		/*!< Channelx mbias current tuning(sensitivity tuning).  
							Touch bias current BIT[5] ~ BIT[0]: 8uA/4uA/2uA/1uA/0.5uA/0.25uA. 
							The sensitivity parameter is used to increase or decrease the strength 
							of the sensor signal (difference count). A higher value of sensitivity (bias current) 
							setting leads to a stronger signal from the sensors (more difference 
							count for the same capacitance change), but also increases the response 
							time and average power consumption.*/

	u8 CT_ETCNNoiseThr;		/*!< Specifies negetive noise threshold of ETC.
							This parameter must be set to a value in the 0x1-0xff range.
							The noise threshold indicates the raw data of the maximum capacitance change 
							caused by environmental change. The CTC system provides configurable 
							positive noise threshold and negative noise threshold for optimal calibration.
							The recommend value of noise threshold is 40%* touch threshold. Users need
							tune the two noise thresholds for different applications and noise environment.*/

	u8 CT_ETCPNoiseThr;		/*!< Specifies positive threshold of ETC.
							This parameter must be set to a value in the 0x1-0xff range. */

	u8 CT_CHEnable;			/*!< Specifies this channel is enable or not */
} CapTouch_CHInitTypeDef;

/** 
  * @brief  CapTouch Initialization structure definition
  */
typedef struct {
	u32 CT_DebounceEn;			/*!< Specifies CapTouch press event Debounce Enable.
							This parameter must be set to a value in the 0x0-1 range. The de-bounce 
							parameter can be configured by register CTC_CTRL.
							For example, when de-bounce is enabled and configured as 00 (2 times scan), 
							finger touch interrupt will not be sent to the host until 2 times continue 
							finger touch event is detected. Sensor de-bounce function is suitable for 
							both button application and proximity detection.*/

	u32 CT_SampleCnt;			/*!< Specifies sample cnt for average function,sample cnt = 2*exp(CT_SampleCnt+2). 
							This parameter can be a value of 0x0-0x7*/

	u32 CT_ScanInterval;			/*!< Specifies scan interval of every key.
							This parameter must be set to a value in the 0x1-0xfff range. */

	u32 CT_ETCStep;				/*!< Specifies baseline update setp of ETC.
							This parameter must be set to a value in the 0x0-0xfff range. */

	u32 CT_ETCFactor;			/*!< Specifies CapTouch ETC Factor.
							This parameter must be set to a value in the 0x0-0xf range. */

	u32 CT_ETCScanInterval;		/*!< Specifies ETC scan interval
							This parameter can be set to a value in the 0x1-0x7f range*/

	CapTouch_CHInitTypeDef CT_Channel[5];			/*!< Specifies the initialization parameters for each channel */
	
}CapTouch_InitTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup CapTouch_Exported_Constants CapTouch Exported Constants
  * @{
  */

/** @defgroup CapTouch_Peripheral_definitions 
  * @{
  */
#define IS_CAPTOUCH_ALL_PERIPH(PERIPH) ((PERIPH) == CAPTOUCH_DEV)
/**
  * @}
  */
  
/** @defgroup CapTouch_INT_Related_definitions 
  * @{
  */
#define  CT_CHX_PRESS_INT(x)			(0x00000001 << (x))
#define  CT_CHX_RELEASE_INT(x)			(0x00000001 << ((x) + 8))

#define IS_CT_INT_EN(IT)					(((IT) & ~BIT_CT_ALL_INT_EN) == 0)
#define IS_CT_INT_CLR(IT)				(((IT) & ~BIT_CT_ALL_INT_CLR_MASK) == 0)
/**
  * @}
  */

  
/** @defgroup CapTouch_Channel_Related_definitions 
  * @{
  */
#define  CT_CHANNEL_NUM			4
#define IS_CT_CHANNEL(CHANNEL_NUM)		(CHANNEL_NUM < CT_CHANNEL_NUM)
/**
  * @}
  */

/** @defgroup CapTouch_Noise_Threshold_Type_definitions 
  * @{
  */
#define  P_NOISE_THRES					0
#define  N_NOISE_THRES					1
/**
  * @}
  */

  
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup CapTouch_Exported_Functions CapTouch Exported Functions
  * @{
  */

/** @defgroup CapTouch_Exported_Normal_Functions CapTouch Normal Functions
  * @{
  */
void CapTouch_StructInit(CapTouch_InitTypeDef* CapTouch_InitStruct);
void CapTouch_Init(CAPTOUCH_TypeDef *CapTouch, CapTouch_InitTypeDef* CapTouch_InitStruct);
void CapTouch_Cmd(CAPTOUCH_TypeDef *CapTouch, u8 NewState);
void CapTouch_INTConfig(CAPTOUCH_TypeDef *CapTouch, uint32_t CapTouch_IT, u8 newState);
void CapTouch_INTClearPendingBit(CAPTOUCH_TypeDef *CapTouch, u32 CapTouch_IT);
u32 CapTouch_GetRawISR(CAPTOUCH_TypeDef *CapTouch);
u32 CapTouch_GetISR(CAPTOUCH_TypeDef *CapTouch);

void CapTouch_SetScanInterval(CAPTOUCH_TypeDef *CapTouch, u32 Interval);
void CapTouch_ChCmd(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u8 NewState);
u32 CapTouch_GetChStatus(CAPTOUCH_TypeDef *CapTouch, u32 Channel);
void CapTouch_SetChDiffThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u32 Threshold);
void CapTouch_SetChMbias(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u8 Mbias);
u32 CapTouch_GetChDiffThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel);
u32 CapTouch_GetNoiseThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u8 type);
u32 CapTouch_GetChBaseline(CAPTOUCH_TypeDef *CapTouch, u8 Channel);
u32 CapTouch_GetChAveData(CAPTOUCH_TypeDef *CapTouch, u8 Channel);
/**
  * @}
  */

/** @defgroup CapTouch_Exported_Debug_Functions CapTouch Debug Functions
  * @{
  */
void CapTouch_DbgCmd(CAPTOUCH_TypeDef *CapTouch, u8 NewState);
void CapTouch_DbgDumpReg(CAPTOUCH_TypeDef *CapTouch);
void CapTouch_DbgDumpETC(CAPTOUCH_TypeDef *CapTouch, u32 ch);
u32 CapTouch_DbgRawData(CAPTOUCH_TypeDef *CapTouch);

/**
  * @}
  */

/**
  * @}
  */
  

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup CapTouch_Register_Definitions CapTouch Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup CT_CTRL
 * @{
 *****************************************************************************/
#define BIT_CT_BL_ENABLE						((u32)0x0000001 << 8)		/*Bit[8], bits for base init enable*/
#define BIT_CT_DBN_CNT							((u32)0x0000003 << 5)		/*Bit[6:5], bits for debounce cnt*/
#define BIT_CT_DBN_ENABLE						((u32)0x0000001 << 4)		/*Bit[4], bits for base init enable*/
#define BIT_CT_ENABLE							((u32)0x0000001 << 0)		/*Bit[8], bits for captouch function enable*/

/** @} */

/**************************************************************************//**
 * @defgroup CT_SP_CTRL
 * @{
 *****************************************************************************/
#define BIT_CT_SMP_AVE							((u32)0x00000007 << 16)	/*Bit[18:16], bits for every chanel sample cnt*/
#define BIT_CT_SCAN_INTERVAL					((u32)0x00000fff << 0)	/*Bit[11:0], bits for pre guard timer set*/
/** @} */

/**************************************************************************//**
 * @defgroup CT_ETC_CTRL
 * @{
 *****************************************************************************/
#define BIT_CT_ETC_N_THRES						((u32)0x000000ff << 24)	/*Bit[31:24], bits for Max negative threshold*/
#define BIT_CT_ETC_P_THRES						((u32)0x000000ff << 16)	/*Bit[23:16], bits for Max positive threshold*/
#define BIT_CT_ETC_STEP							((u32)0x0000000f << 12)	/*Bit[15:12], bits for ETC setp for every update*/
#define BIT_CT_ETC_FACTOR						((u32)0x0000000f << 8)	/*Bit[11:8], bits for ETC interval timer*/
#define BIT_CT_ETC_SCAN_INTERVAL				((u32)0x0000007f << 1)	/*Bit[7:1], bits for ETC scan interval timer set*/
#define BIT_CT_ETC_ENABLE						((u32)0x00000001 << 0)	/*Bit[0], bits for ETC function Enable*/

/** @} */

/**************************************************************************//**
 * @defgroup CT_SNR
 * @{
 *****************************************************************************/
#define BIT_CT_SNR_NOISE						((u32)0x00000fff << 16)		/*Bit[27:16], bit raw noise data for SNR*/
#define BIT_CT_SNR_TOUCH						((u32)0x00000fff << 0)		/*Bit[11:0], bit raw touch data for SNR*/
/** @} */

/**************************************************************************//**
 * @defgroup CT_MODE_CTRL
 * @{
 *****************************************************************************/
#define BIT_CT_CHANNEL_SEL						((u32)0x00000007 << 5)		/*Bit[7:5], bits for captouch channel select*/
#define BIT_CT_AUTO_CHANNEL_ENABLE			((u32)0x00000001 << 4)		/*Bit[4], bits for captouch auto channel swith enable*/
#define BIT_CT_DBG_ENABLE						((u32)0x00000001 << 0)		/*Bit[0], bits for captouch debug mode enable*/
/** @} */

/**************************************************************************//**
 * @defgroup CT_FIFO_STATUS
 * @{
 *****************************************************************************/
#define BIT_CT_FIFO_OFFSET						((u32)0x00000007 << 4)		/*Bit[6:4], bits for key column select*/
#define BIT_CT_FIFO_EMPTY						((u32)0x00000001 << 1)		/*Bit[1], bit for fifo full flag*/
#define BIT_CT_FIFO_FULL						((u32)0x00000001 << 0)		/*Bit[0], bit for fifo full flag*/

/** @} */

/**************************************************************************//**
 * @defgroup CT_FIFO
 * @{
 *****************************************************************************/
#define BIT_CT_FIFO_VLD							((u32)0x00000001 << 31)		/*Bit[31], bit for fifo data valid flag*/
#define BIT_CT_FIFO_DATA						((u32)0x00000fff << 0) 		/*Bit[11:0], bit for fifo raw data*/
/** @} */

/**************************************************************************//**
 * @defgroup CT_IER_ISR_RAWISR
 * @{
 *****************************************************************************/
#define BIT_CT_OVER_N_NOISE_THRESHOLD_INT	((u32)0x00000001 << 18)		/*Bit[18], bit for captouch over negetive noise thresh interrupt enable*/
#define BIT_CT_FIFO_OVERFLOW_INT				((u32)0x00000001 << 17)		/*Bit[17], bit for captouch fifo overflow interrupt enable*/
#define BIT_CT_OVER_P_NOISE_THRESHOLD_INT	((u32)0x00000001 << 16)		/*Bit[16], bit for captouch over positive noise thresh interrupt enable*/
#define BIT_CT_TOUCH_RELEASE_INT				((u32)0x0000001f << 8)		/*Bit[12:8], bit for captouch release interrupt enable*/
#define BIT_CT_TOUCH_PRESS_INT				((u32)0x0000001f <<0)		/*Bit[4:0], bit for captouch press interrupt enable*/
#define BIT_CT_ALL_INT_EN						((u32)0x00031f1f)
/** @} */

/**************************************************************************//**
 * @defgroup CT_ICR_ALL
 * @{
 *****************************************************************************/
#define BIT_CT_ALL_INT_CLR			((u32)0x00000001 << 0)		/*Bit[0], bit for clear all interrupt status*/

/** @} */

/**************************************************************************//**
 * @defgroup CT_ICR
 * @{
 *****************************************************************************/
#define BIT_CT_N_NOISE_OVERFLOW_INT_CLR		((u32)0x00000001 << 18)		/*Bit[18], bit for captouch negetive noise interrupt clear*/
#define BIT_CT_FIFO_OVERFLOW_INT_CLR			((u32)0x00000001 << 17)		/*Bit[17], bit for captouch fifo overflow interrupt clear*/
#define BIT_CT_P_NOISE_OVERFLOW_INT_CLR		((u32)0x00000001 << 16)		/*Bit[16], bit for captouch positive noise interrupt clear*/
#define BIT_CT_TOUCH_RELEASE_INT_CLR			((u32)0x0000001f << 8)		/*Bit[12:8], bit for captouch release interrupt clear*/
#define BIT_CT_TOUCH_PRESS_INT_CLR			((u32)0x0000001f <<0)		/*Bit[4:0], bit for captouch press interrupt clear*/
#define BIT_CT_ALL_INT_CLR_MASK					((u32)0x00071f1f)
/** @} */

/**************************************************************************//**
 * @defgroup CT_CHX_CTRL
 * @{
 *****************************************************************************/
#define BIT_CT_CHX_TOUCH_THRES					((u32)0x00000fff << 16)		/*Bit[27:16], bit for touch difference threshold*/
#define BIT_CT_CHX_BASELINE						((u32)0x00000fff << 4)		/*Bit[15:4], bit for touch pad baseline*/
#define BIT_CT_CHX_ENABLE							((u32)0x00000001 << 0)		/*Bit[0], bit for touch pad channel enable*/
/** @} */

/**************************************************************************//**
 * @defgroup CT_CHX_ATHR
 * @{
 *****************************************************************************/
#define BIT_MASK_CHX_N_ENT							((u32)0x000000ff << 24)
#define BIT_MASK_CHX_P_ENT							((u32)0x000000ff << 16)
#define BIT_CT_CHX_TOUCH_ATHRES					((u32)0x00000fff << 0)		/*Bit[11:0], bit for touch difference absolute threshold*/
/** @} */

/**************************************************************************//**
 * @defgroup CT_CHX_MBIAS
 * @{
 *****************************************************************************/
#define BIT_CT_CHX_MBIAS							((u32)0x0000003f << 0)		/*Bit[6], bit for touch key mbias current*/
		
/** @} */

/**************************************************************************//**
 * @defgroup CT_CHX_DATA
 * @{
 *****************************************************************************/
#define BIT_CT_CHX_POLARITY						((u32)0x00000001 << 28)		/*Bit[28], bit for diff data ploarity*/
#define BIT_CT_CHX_DIFF								((u32)0x00000fff <<16)		/*Bit[27:16], bit for diff between average data and baseline*/
#define BIT_CT_CHX_DATA							((u32)0x00000fff << 0)		/*Bit[11:0], bit for captouch channelx average data*/
/** @} */


/** @} */

/**
  * @}
  */

/**
  * @}
  */

/* Other Definitions --------------------------------------------------------*/


#endif

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
