/**
  ******************************************************************************
  * @file    rtl8721d_comparator.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the Comparator firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef _RTL8721D_COMPARE_H_
#define _RTL8721D_COMPARE_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup Comparator
  * @brief Comparator driver modules
  * @{
  */

/** @addtogroup Comparator
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * Comparator:
  * 	- Base Address: Comparator
  * 	- Channel: 4
  * 	- Adjustable internal comparison voltage, 0~3.3v, each step 0.1v
  * 	- Adjustable internal divider resistors
  * 	- Cooperate with ADC comparator-assist mode
  *
  *****************************************************************************************
  * How to use Comparator
  *****************************************************************************************
  * 	  To use Comparator, the following steps are mandatory:
  *
  *      1. Enable the ADC & Comparator interface clock:
  *			RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);
  *
  *      2. Fill the CMP_InitStruct with the desired parameters.
  *			CMP_StructInit(CMP_InitTypeDef *CMP_InitStruct)
  *
  *      3. Init Comparator with the parameters in CMP_InitStruct.
  *			CMP_Init(CMP_InitTypeDef* CMP_InitStruct)
  *
  *      4. Activate the Comparator:
  *			CMP_Cmd(ENABLE).
  *
  *      5.  Enbale specified mode:
  *			CMP_SWTrigCmd(ENABLE)/CMP_AutoCSwCmd(ENABLE)/ \
  *                            CMP_TimerTrigCmd(Tim_Idx, PeriodMs, ENABLE)
  *
  *****************************************************************************************
  * @endverbatim
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup COMP_Exported_Types COMP Exported Types
  * @{
  */
  
/**
  * @brief	Comparator Init structure definition
  */
typedef struct {
	u8 CMP_ChIndex;			/*!< Specifies the channel index */
		
	u8 CMP_Ref0;			/*!< Specifies the internal reference voltage0, the value can be 0~31,
									Vref0 = CMP_Ref0*0.1v */
	u8 CMP_Ref1;			/*!< Specifies the internal reference voltage1, the value can be 0~31,
									Vref1 = CMP_Ref1*0.1v */
	u8 CMP_WakeType;		/*!< Specifies this channel wakeup system or ADC when criterion matches.
									This parameter can be a value or combination of 
									@ref COMP_Compare_Wakeup_Type_Definitions */
	u8 CMP_WakeSysCtrl;		/*!< Specifies the criteria of when comparator channel should wakeup
									system, which can be a value of 
									@ref COMP_Compare_Control_Definitions */
	u8 CMP_WakeADCCtrl;		/*!< Specifies the criteria of when comparator channel should wakeup
									ADC, which can be a value of 
									@ref COMP_Compare_Control_Definitions */
} CMP_CHTypeDef;

typedef struct {

	CMP_CHTypeDef CMP_ChanCtrl[4];  /*!< Specifies the comparator channel control parameters */

} CMP_InitTypeDef;
/**
  * @}
  */


/* Exported constants --------------------------------------------------------*/

/** @defgroup COMP_Exported_Constants COMP Exported Constants
  * @{
  */

/** @defgroup COMP_Chn_Selection
  * @{
  */
#define COMP_CH0							((u8)0x00)
#define COMP_CH1							((u8)0x01)
#define COMP_CH2							((u8)0x02)
#define COMP_CH3							((u8)0x03)
#define COMP_CH_NUM						(4)

#define IS_COMP_CHN_SEL(SEL)     (((SEL) == COMP_CH0) || \
									((SEL) == COMP_CH1) || \
									((SEL) == COMP_CH2) || \
									((SEL) == COMP_CH3))

/**
  * @}
  */

/** @defgroup COMP_Compare_Control_Definitions
  * @{
  */
#define COMP_SMALLER_THAN_REF0			((u8)0x00)	/*!< Vin < Vref0 */
#define COMP_GREATER_THAN_REF1			((u8)0x01)	/*!< Vin >= Vref1 */
#define COMP_WITHIN_REF0_AND_REF1			((u8)0x02)	/*!< Vin > Vref0 &&  Vin < Vref1 */

/**
  * @}
  */

/** @defgroup COMP_Compare_Wakeup_Type_Definitions
  * @{
  */
#define COMP_WK_SYS						BIT(1)
#define COMP_WK_ADC						BIT(0)
#define COMP_WK_NONE						0

/**
  * @}
  */


/**
  * @}
  */

/** @defgroup COMP_Exported_Functions COMP Exported Functions
  * @{
  */
_LONG_CALL_ void CMP_StructInit(CMP_InitTypeDef *CMP_InitStruct);
_LONG_CALL_ void CMP_Init(CMP_InitTypeDef* CMP_InitStruct);
_LONG_CALL_ void CMP_Cmd(u32 NewState);
_LONG_CALL_ u32 CMP_Busy(void);
_LONG_CALL_ u32 CMP_GetISR(void);
_LONG_CALL_ void CMP_INTClearPendingBit(u32 Cmp_IT);
_LONG_CALL_ u32 CMP_GetCompStatus(u8 channel);
_LONG_CALL_ u32 CMP_GetLastChan(void);
_LONG_CALL_ void CMP_ResetCSwList(void);
_LONG_CALL_ void CMP_AutoCSwCmd(u32 NewState);
_LONG_CALL_ void CMP_TimerTrigCmd(u8 Tim_Idx, u32 PeriodMs, u32 NewState);

/**
  * @}
  */

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup COMP_Register_Definitions COMP Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup REG_COMP_REF_CHx
 * @{
 *****************************************************************************/
#define BIT_SHIFT_COMP_REF1						16
#define BIT_MASK_COMP_REF1						(u32)(0x0000001F << BIT_SHIFT_COMP_REF1)
#define BIT_SHIFT_COMP_REF0						0
#define BIT_MASK_COMP_REF0						(u32)(0x0000001F << BIT_SHIFT_COMP_REF0)

/** @} */

/**************************************************************************//**
 * @defgroup REG_COMP_INTR_CTRL
 * @{
 *****************************************************************************/
 #define BIT_SHIFT_COMP_WK_SYS_CTRL(x)			(17 + 3*x)
 #define BIT_MASK_COMP_WK_SYS_CTRL(x)			(u32)(0x00000003 << BIT_SHIFT_COMP_WK_SYS_CTRL(x))
 #define BIT_SHIFT_COMP_WK_SYS_EN(x)			(16 + 3*x)
 #define BIT_COMP_WK_SYS_EN(x)					(u32)(0x00000001 << BIT_SHIFT_COMP_WK_SYS_EN(x))

 #define BIT_SHIFT_COMP_WK_ADC_CTRL(x)			(1 + 3*x)
 #define BIT_MASK_COMP_WK_ADC_CTRL(x)			(u32)(0x00000003 << BIT_SHIFT_COMP_WK_ADC_CTRL(x))
 #define BIT_SHIFT_COMP_WK_ADC_EN(x)			(3*x)
 #define BIT_COMP_WK_ADC_EN(x)					(u32)(0x00000001 << BIT_SHIFT_COMP_WK_ADC_EN(x))

/** @} */

/**************************************************************************//**
 * @defgroup REG_COMP_AUTOSW_EN
 * @{
 *****************************************************************************/
#define BIT_COMP_AUTOSW_EN					BIT(0)

/** @} */

/**************************************************************************//**
 * @defgroup REG_COMP_EN_TRIG
 * @{
 *****************************************************************************/
#define BIT_COMP_DBG_EN						BIT(2)
#define BIT_COMP_EN_TRIG						BIT(1)
#define BIT_COMP_ENABLE							BIT(0)

/** @} */


/**************************************************************************//**
 * @defgroup REG_COMP_CHSW_LIST
 * @{
 *****************************************************************************/
#define BIT_SHIFT_COMP_CHSW(x)					(4*x)
#define BIT_COMP_CHSW(x)						(u32)(0x0000000F << BIT_SHIFT_COMP_CHSW(x))

/** @} */

/**************************************************************************//**
 * @defgroup REG_COMP_LAST_CH
 * @{
 *****************************************************************************/
 #define BIT_COMP_LAST_CH						(u32)(0x00000003)

/** @} */

 /**************************************************************************//**
 * @defgroup REG_COMP_BUSY_STS
 * @{
 *****************************************************************************/
 #define BIT_COMP_BUSY_STS						BIT(0)
/** @} */

  /**************************************************************************//**
 * @defgroup REG_COMP_CH_STS
 * @{
 *****************************************************************************/
 #define BIT_SHIFT_COMP_CH_STS(x)				(2*x)
 #define BIT_COMP_CH_STS(x)						(u32)(0x3 << BIT_SHIFT_COMP_CH_STS(x))
 
/** @} */

/**************************************************************************//**
 * @defgroup REG_COMP_RST_LIST
 * @{
 *****************************************************************************/
#define BIT_COMP_RST_LIST						BIT(0)
/** @} */

/**************************************************************************//**
 * @defgroup REG_COMP_AUTO_SHUT
 * @{
 *****************************************************************************/
#define BIT_COMP_AUTO_SHUT						BIT(0)
/** @} */

/**************************************************************************//**
 * @defgroup REG_COMP_EXT_TRIG_CTRL
 * @{
 *****************************************************************************/
#define BIT_COMP_EXT_WK_TIMER					BIT(0)
/** @} */

/**************************************************************************//**
 * @defgroup REG_COMP_EXT_TRIG_TIMER_SEL
 * @{
 *****************************************************************************/
#define BIT_COMP_EXT_WK_TIMER_SEL				(u32)(0x00000007)

/** @} */

 /**************************************************************************//**
  * @defgroup REG_COMP_EXT_WK_SHUT_CTRL
  * @{
  *****************************************************************************/
#define BIT_SHIFT_COMP_CHSW_CNT				(8)	
#define BIT_MASK_COMP_CHSW_CNT				(u32)(0x000000FF << BIT_SHIFT_COMP_CHSW_CNT)	
#define BIT_MASK_COMP_EXT_WK_SHUT_CNT		(u32)(0x000000FF)
 
 /** @} */

 /**************************************************************************//**
  * @defgroup REG_COMP_ANALOG
  * @{
  *****************************************************************************/
#define BIT_SD_POSEDGE							BIT(1)

 /** @} */


/** @} */

/**
  * @}
  */

/**
  * @}
  */

/* Other Definitions --------------------------------------------------------*/


#endif /* _RTL8721D_COMPARE_H_ */

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
