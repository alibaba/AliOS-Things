/**
  ******************************************************************************
  * @file    rtl8721d_keyscan.h
  * @author
  * @version V1.0.0
  * @date    2017-10-16
  * @brief   This file contains all the functions prototypes for the keyscan.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */


#ifndef _RTL8721D_KEYSCAN_H_
#define _RTL8721D_KEYSCAN_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @addtogroup KeyScan KeyScan
  * @{
  */

/** @addtogroup KeyScan
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * KEYSCAN:
  *		- Base Address: KEYSCAN_DEV
  *		- Sclk: 32K/10Mhz
  *		- Keypad Array: Up to 6*6 (36), multi-key detect
  *		- Scan Clock:  	Configurable, up to 10Mhz
  *		- Work Mode: Event Trigger Mode and Regular Scan Mode
  *		- Debounce Timer: Configurable
  *		- SocPs: Sleep Mode (clock gating & power gating)
  *		- IRQ: KeyScan_IRQ
  *
  *****************************************************************************************     
  * How to use Normal KeyScan
  *****************************************************************************************
  *      To use the normal KeyScan mode, the following steps are mandatory:
  *       
  *      1. Enable KeyScan peripheral clock
  *
  *      2. configure the KeyScan pinmux.
  *        
  *      3. Program Scan clock, Work Mode, Columns and Rows select, Dobounce Timer, Threshold
  *			KeyScan_StructInit(KeyScan_InitTypeDef* KeyScan_InitStruct)
  *
  *      4. Init Hardware use step3 parameters:
  *			KeyScan_Init(KEYSCAN_TypeDef *KeyScan, KeyScan_InitTypeDef* KeyScan_InitStruct)
  * 
  *      5. Enable the NVIC and the corresponding interrupt using following function if you need 
  *			to use interrupt mode. 
  *			KeyScan_INTConfig(): KeyScan IRQ Enable set
  *			KeyScan_INTMask(): KeyScan IRQ mask set
  *			InterruptRegister(): register the keyscan irq handler 
  *			InterruptEn(): Enable the NVIC interrupt
  *
  *      6. Enable KeyScan module using KeyScan_Cmd().
  *
  *****************************************************************************************     
  * @endverbatim
  */
  
/* Exported types --------------------------------------------------------*/
/** @defgroup KeyScan_Exported_Types KeyScan Exported Types
  * @{
  */

/** 
  * @brief  KeyScan Init structure definition
  */
typedef struct {
	u32 KS_ClkDiv;			/*!< Specifies Keyscan clock divider. Scan_clk = Bus clock/(KS_ClkDiv+1).
						This parameter must be set to a value in the 0x0-0xfff range. */

	u32 KS_WorkMode;		/*!< Specifies Keyscan work mode. 
						This parameter can be a value of @ref KeyScan_Work_Mode_definitions */

	u32 KS_RowSel;			/*!< Specifies which row(s) is used.
						This parameter must be set to a value in the 0x1-0xff range. */

	u32 KS_ColSel;			/*!< Specifies which column(s) is used.
						This parameter must be set to a value in the 0x1-0xff range. */

	u32 KS_DebounceCnt;		/*!< Specifies Keyscan Debounce Timer. Debounce Timer = (KS_DebounceCnt +1)* Scan_clk.
						This parameter must be set to a value in the 0x0-0xfff range. */

	u32 KS_IntervalCnt;		/*!< Specifies Keyscan Scan Interval Timer. Interval Timer = (KS_IntervalCnt +1)* Scan_clk.
						This parameter must be set to a value in the 0x0-0xfff range. */

	u32 KS_ReleaseCnt;		/*!< Specifies Keyscan All Release Timer. Release Timer = (KS_ReleaseCnt+1) * Scan_clk.
						This parameter must be set to a value in the 0x0-0xfff range. */

	u32 KS_LimitLevel;		/*!< Specifies the max allowable key number be pressed at a time
						This parameter can be a value of @ref KeyScan_FIFO_LimitLevel_Control */

	u32 KS_ThreholdLevel;	/*!< Specifies Keyscan FIFO threshold to trigger KS_FIFO_FULL
						This parameter can be a value of @ref KeyScan_FIFO_ThreholdLevel_Control */
					
	u32 KS_OverCtrl;		/*!< Specifies Keyscan FIFO over control.
						This parameter can be a value of @ref KeyScan_FIFO_Overflow_Control */
}KeyScan_InitTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup KeyScan_Exported_Constants KeyScan Exported Constants
  * @{
  */

/** @defgroup KeyScan_Peripheral_definitions 
  * @{
  */
#define IS_KEYSCAN_ALL_PERIPH(PERIPH) ((PERIPH) == KEYSCAN_DEV)
/**
  * @}
  */
  
/** @defgroup KeyScan_Work_Mode_definitions 
  * @{
  */
#define KS_REGULAR_SCAN_MODE			((u32)0x00000000 << 3)
#define KS_EVENT_TRIGGER_MODE			((u32)0x00000001 << 3)
#define IS_KS_WORK_MODE(MODE)			(((MODE) == KS_REGULAR_SCAN_MODE) || \
                                   ((MODE) == KS_EVENT_TRIGGER_MODE))
/**
  * @}
  */
  
/** @defgroup KeyScan_FIFO_Overflow_Control
  * @{
  */
#define KS_FIFO_OVER_CTRL_DIS_NEW		((u32)0x00000000 << 1)
#define KS_FIFO_OVER_CTRL_DIS_LAST		((u32)0x00000001 << 1)
#define IS_KS_FIFO_OVER_CTRL(CTRL)		(((CTRL) == KS_FIFO_OVER_CTRL_DIS_NEW) || \
					((CTRL) == KS_FIFO_OVER_CTRL_DIS_LAST))
/**
  * @}
  */

/** @defgroup KeyScan_FIFO_LimitLevel_Control 
  * @{
  */ 
#define IS_KS_FIFO_LIMIT_LEVEL(DATA_NUM)	((DATA_NUM) <= 6)
/**
  * @}
  */
  
/** @defgroup KeyScan_FIFO_ThreholdLevel_Control 
  * @{
  */ 
#define IS_KS_FIFO_TH_LEVEL(DATA_NUM)		(((DATA_NUM) > 0) && ((DATA_NUM) < 16))
/**
  * @}
  */
  
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup KeyScan_Exported_Functions KeyScan Exported Functions
  * @{
  */

/** @defgroup KeyScan_Exported_Normal_Functions KeyScan Exported Normal Functions
  * @{
  */
_LONG_CALL_ void KeyScan_StructInit(KeyScan_InitTypeDef* KeyScan_InitStruct);
_LONG_CALL_ void KeyScan_Init(KEYSCAN_TypeDef *KeyScan, KeyScan_InitTypeDef* KeyScan_InitStruct);
_LONG_CALL_ void KeyScan_INTConfig(KEYSCAN_TypeDef *KeyScan, uint32_t KeyScan_IT, u8 newState);
_LONG_CALL_ void KeyScan_ClearINT(KEYSCAN_TypeDef *KeyScan, u32 KeyScan_IT);
_LONG_CALL_ u32 KeyScan_GetRawINT(KEYSCAN_TypeDef *KeyScan);
_LONG_CALL_ u32 KeyScan_GetINT(KEYSCAN_TypeDef *KeyScan);
_LONG_CALL_ u8 KeyScan_GetDataNum(KEYSCAN_TypeDef *KeyScan);
_LONG_CALL_ void KeyScan_ClearFIFOData(KEYSCAN_TypeDef *KeyScan);
_LONG_CALL_ BOOL KeyScan_GetFIFOState(KEYSCAN_TypeDef *KeyScan, u32 KeyScan_Flag);
_LONG_CALL_ void KeyScan_Read(KEYSCAN_TypeDef *KeyScan, u32 *outBuf, u8 count);
_LONG_CALL_ void KeyScan_Cmd(KEYSCAN_TypeDef *KeyScan, u8 NewState);
/**
  * @}
  */

/**
  * @}
  */
  

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup KeyScan_Register_Definitions KeyScan Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup KS_CLK_DIV
 * @{
 *****************************************************************************/
#define BIT_KS_CLK_DIV						((u32)0x00000fff)		/*Bit[11:0], bits for clock division*/
/** @} */

/**************************************************************************//**
 * @defgroup KS_TIM_CFG0
 * @{
 *****************************************************************************/
#define BIT_KS_POST_GUARD_TIMER				((u32)0x0000000f << 24)	/*Bit[27:24], bits for post guard timer set*/
#define BIT_KS_PRE_GUARD_TIMER				((u32)0x0000000f << 16)	/*Bit[19:16], bits for pre guard timer set*/
#define BIT_KS_DEB_TIMER					((u32)0x00000fff)			/*Bit[11:0], bits for debounce timer set*/
/** @} */

/**************************************************************************//**
 * @defgroup KS_TIM_CFG1
 * @{
 *****************************************************************************/
#define BIT_KS_INTERVAL_TIMER				((u32)0x00000fff << 16)	/*Bit[27:16], bits for interval timer set*/
#define BIT_KS_RELEASE_TIMER				((u32)0x00000fff)			/*Bit[11:0], bits for all release timer set*/
/** @} */

/**************************************************************************//**
 * @defgroup KS_CTRL
 * @{
 *****************************************************************************/
#define BIT_KS_WORK_MODE					((u32)0x00000001 << 3)		/*Bit[3], bit for keyscan work mode select*/
#define BIT_KS_BUSY_STATUS					((u32)0x00000001 << 1)		/*Bit[1], bit for FSM busy status*/
#define BIT_KS_RUN_ENABLE					((u32)0x00000001)			/*Bit[0], bit for enable internal key scan scan clock*/
/** @} */

/**************************************************************************//**
 * @defgroup KS_FIFO_CFG
 * @{
 *****************************************************************************/
#define BIT_KS_FIFO_LIMIT_LEVEL				((u32)0x0000000f << 24)	/*Bit[27:24], bits for fifo limit level set*/
#define BIT_KS_FIFO_THREHOLD_LEVEL			((u32)0x0000000f << 16)	/*Bit[19:16], bits for fifo threshold set*/
#define BIT_KS_FIFO_OV_CTRL					((u32)0x00000001 << 1)		/*Bit[1], bit for fifo overflow control*/
#define BIT_KS_FIFO_CLR						((u32)0x00000001)			/*Bit[0], bit for fifo data clear*/
/** @} */

/**************************************************************************//**
 * @defgroup KS_COL_CFG
 * @{
 *****************************************************************************/
#define BIT_KS_COLUMN_SEL					((u32)0x000000ff)			/*Bit[7:0], bits for key column select*/
/** @} */

/**************************************************************************//**
 * @defgroup KS_ROW_CFG
 * @{
 *****************************************************************************/
#define BIT_KS_ROW_SEL						((u32)0x000000ff)			/*Bit[7:0], bits for key row select*/
/** @} */

/**************************************************************************//**
 * @defgroup KS_DATA_NUM
 * @{
 *****************************************************************************/
#define BIT_KS_FIFO_FULL					((u32)0x00000001 << 17)	/*Bit[17], bit for fifo full flag*/
#define BIT_KS_FIFO_EMPTY					((u32)0x00000001 << 16) 	/*Bit[16], bit for fifo empty flag*/
#define BIT_KS_FIFO_DATA_LEVEL				((u32)0x0000001f)			/*Bit[4:0], bits for fifo data level*/
/** @} */

/**************************************************************************//**
 * @defgroup KS_DATA
 * @{
 *****************************************************************************/
#define BIT_KS_PRESS_EVENT					((u32)0x00000001 << 8)		/*Bit[8], bit for key press event*/
#define BIT_KS_RELEASE_EVENT				((u32)0x00000000 << 8)		/*Bit[8], bit for key release event*/
#define BIT_KS_EVENT_FLAG					((u32)0x0000000f << 8)		/*Bit[11:8], bits for keyscan event*/
#define BIT_KS_ROW_INDEX					((u32)0x0000000f << 4)		/*Bit[7:4], bits for key row index*/
#define BIT_KS_COL_INDEX					((u32)0x0000000f)			/*Bit[3:0], bits for key column index*/
#define BIT_KS_DATA							((u32)0x00000fff)			/*Bit[11:8], bits for keyscan data*/
/** @} */

/**************************************************************************//**
 * @defgroup KS_IMR
 * @{
 *****************************************************************************/
#define BIT_KS_SCAN_EVENT_INT_MSK			((u32)0x00000001 << 6)		/*Bit[6], bit for key event interrupt mask*/
#define BIT_KS_FIFO_LIMIT_INT_MSK			((u32)0x00000001 << 5)		/*Bit[5], bit for keyscan fifo limit interrupt mask*/
#define BIT_KS_FIFO_OVERFLOW_INT_MSK		((u32)0x00000001 << 4)		/*Bit[4], bit for keyscan fifo overflow interrupt mask*/
#define BIT_KS_FIFO_FULL_INT_MSK			((u32)0x00000001 << 3)		/*Bit[3], bit for keyscan fifo full interrupt mask*/
#define BIT_KS_SCAN_FINISH_INT_MSK			((u32)0x00000001 << 2)		/*Bit[2], bit for keyscan finish interrupt mask*/
#define BIT_KS_FIFO_NOTEMPTY_INT_MSK		((u32)0x00000001 << 1)		/*Bit[1], bit for keyscan fifo not empty interrupt mask*/
#define BIT_KS_ALL_RELEASE_INT_MSK			((u32)0x00000001)			/*Bit[0], bit for keyscan all release interrupt mask*/
#define BIT_KS_ALL_INT_MSK					((u32)0x0000007f)
/** @} */

/**************************************************************************//**
 * @defgroup KS_ICR
 * @{
 *****************************************************************************/
#define BIT_KS_FIFO_LIMIT_INT_CLR			((u32)0x00000001 << 5)		/*Bit[5], bit for keyscan fifo limit interrupt clear*/
#define BIT_KS_FIFO_OVERFLOW_INT_CLR		((u32)0x00000001 << 4)		/*Bit[4], bit for keyscan fifo overflow interrupt clear*/
#define BIT_KS_SCAN_FINISH_INT_CLR			((u32)0x00000001 << 2)		/*Bit[2], bit for keyscan finish interrupt clear*/
#define BIT_KS_ALL_RELEASE_INT_CLR			((u32)0x00000001)			/*Bit[0], bit for keyscan all release interrupt clear*/
#define BIT_KS_ALL_INT_CLR					((u32)0x0000007f)
/** @} */

/**************************************************************************//**
 * @defgroup KS_ISR
 * @{
 *****************************************************************************/
#define BIT_KS_SCAN_EVENT_INT_STATUS		((u32)0x00000001 << 6)		/*Bit[6], bit for key event interrupt status*/
#define BIT_KS_FIFO_LIMIT_INT_STATUS		((u32)0x00000001 << 5)		/*Bit[5], bit for keyscan fifo limit interrupt status*/
#define BIT_KS_FIFO_OVERFLOW_INT_STATUS		((u32)0x00000001 << 4)		/*Bit[4], bit for keyscan fifo overflow interrupt status*/
#define BIT_KS_FIFO_FULL_INT_STATUS			((u32)0x00000001 << 3)		/*Bit[3], bit for keyscan fifo full interrupt status*/
#define BIT_KS_SCAN_FINISH_INT_STATUS		((u32)0x00000001 << 2)		/*Bit[2], bit for keyscan finish interrupt status*/
#define BIT_KS_FIFO_NOTEMPTY_INT_STATUS		((u32)0x00000001 << 1)		/*Bit[1], bit for keyscan fifo not empty interrupt status*/
#define BIT_KS_ALL_RELEASE_INT_STATUS		((u32)0x00000001)			/*Bit[0], bit for keyscan all release interrupt status*/
/** @} */

/**************************************************************************//**
 * @defgroup KS_ISR_RAW
 * @{
 *****************************************************************************/
#define BIT_KS_SCAN_EVENT_RAW_INT_STATUS		((u32)0x00000001 << 6)	/*Bit[6], bit for key event raw interrupt status*/
#define BIT_KS_FIFO_LIMIT_RAW_INT_STATUS		((u32)0x00000001 << 5)	/*Bit[5], bit for keyscan fifo limit raw interrupt status*/
#define BIT_KS_FIFO_OVERFLOW_RAW_INT_STATUS		((u32)0x00000001 << 4)	/*Bit[4], bit for keyscan fifo overflow raw interrupt status*/
#define BIT_KS_FIFO_FULL_RAW_INT_STATUS			((u32)0x00000001 << 3)	/*Bit[3], bit for keyscan fifo full raw interrupt status*/
#define BIT_KS_SCAN_FINISH_RAW_INT_STATUS		((u32)0x00000001 << 2)	/*Bit[2], bit for keyscan finish raw interrupt status*/
#define BIT_KS_FIFO_NOTEMPTY_RAW_INT_STATUS		((u32)0x00000001 << 1)	/*Bit[1], bit for keyscan fifo not empty raw interrupt status*/
#define BIT_KS_ALL_RELEASE_RAW_INT_STATUS		((u32)0x00000001)		/*Bit[0], bit for keyscan all release raw interrupt status*/
/** @} */

/**************************************************************************//**
 * @defgroup KS_DUMMY
 * @{
 *****************************************************************************/
#define BIT_DUMMY_H					((u32)0x00000001 << 8)	/*Bit[15:8], bit for Dummy_h*/
#define BIT_DUMMY_L					((u32)0x00000001 << 2)	/*Bit[7:2], bit for Dummy_l*/
#define BIT_KS_DISCHARGE			((u32)0x00000001 << 1)	/*Bit[1], bit for discharge the column spurious capacitance,1 for enable discharge*/
#define BIT_KS_INTERVAL_POLARITY	((u32)0x00000001)		/*Bit[0], bit for configure the column polarity in debounce and interval phase,1 for drive low*/
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

/******************* (C) COPYRIGHT 2017 Realtek Semiconductor *****END OF FILE****/
