/**
  ******************************************************************************
  * @file    rtl8721d_ir.h
  * @author
  * @version V1.0.0
  * @date    2017-09-18
  * @brief   This file contains all the functions prototypes for the IR firmware
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

#ifndef _RTL8721D_IR_H_
#define _RTL8721D_IR_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup IR
  * @{
  */

/** @addtogroup IR
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * IR:
  *		- Base Address: IR_DEV
  *		- IPclk: SCLK, normally is 100MHz
  *		- carrier clock: 25k~500k
  *		- work mode: Half Duplex
  *		- SocPs: SleepMode not support
  *		- accuracy:3%
  *		- IRQ: IR_IRQ
  *		- GDMA support: not support
  *
  *****************************************************************************************
  * PINMUX
  *****************************************************************************************
  */	
 //1TODO: -to be define:derek
  /*
  *****************************************************************************************     
  * How to use IR TX
  *****************************************************************************************
  *      To use the IR tx function, the following steps are mandatory:
  *       
  *      1. Enable peripheral clock and power:
  *			RCC_PeriphClockCmd(APBPeriph_IR, APBPeriph_IR_CLOCK, ENABLE);
  *
  *      2. Configure the IR pinmux
  *			Pinmux_Config(Pin_Num, PINMUX_FUNCTION_IR)
  *        
  *      3. Set parameters, change some parameter if needed
  *			void IR_StructInit(IR_InitTypeDef *IR_InitStruct)
  *          
  *      4. Init hardware use step3 parameters.
  *			void IR_Init(IR_InitTypeDef *IR_InitStruct)
  *
  *      5. Enable IRQ using following function if needed
  *			IR_INTConfig(): IR IRQ Enable set
  *			InterruptRegister(): register the IR irq handler 
  *  
  *	  6. Send TX data to FIFO
  *				
  *****************************************************************************************      
  * How to use IR RX
  *****************************************************************************************   
  *      To use the IR rx function, the following steps are mandatory:
  *       
  *      1. Enable peripheral clock and power:
  *			RCC_PeriphClockCmd(APBPeriph_IR, APBPeriph_IR_CLOCK, ENABLE);
  *
  *      2. Configure the IR pinmux
  *			Pinmux_Config(Pin_Num, PINMUX_FUNCTION_IR)
  *        
  *      3. Set parameters, change some parameter if needed
  *			void IR_StructInit(IR_InitTypeDef *IR_InitStruct)
  *          
  *      4. Init hardware use step3 parameters.
  *			void IR_Init(IR_InitTypeDef *IR_InitStruct)
  *
  *      5. Enable IRQ using following function if needed
  *			IR_INTConfig(): IR IRQ Enable set
  *			InterruptRegister(): register the IR irq handler 
  *    
  *      6. Recieve Data from Rx fifo 
  * @endverbatim
  */

/*============================================================================*
 *                         Types
 *============================================================================*/

/** @defgroup IR_Exported_Types IR Exported Types
  * @{
  */

/**
 * @brief IR initialize parameters
 *
 * IR initialize parameters
 */
typedef struct
{
	u32 IR_Clock;				/*!< Specifies the IR IP core Input clock. */
	u32 IR_Freq;					/*!< Specifies the clock frequency. This parameter is IR carrier freqency whose unit is Hz.
							This parameter can be a value of @ref IR_Frequency */
	u32 IR_DutyCycle;			/*!< Specifies the IR duty cycle. */
	u32 IR_Mode;				/*!< Specifies the IR mode.
							This parameter can be a value of @ref IR_Mode */
	u32 IR_TxIdleLevel;			/*!< Specifies the IR output level in Tx mode
							This parameter can be a value of @ref IR_Idle_Status */
	u32 IR_TxInverse;			/*!< Specifies inverse FIFO data or not in TX mode
							This parameter can be a value of @ref IR_TX_Data_LEVEL */
	u32 IR_TxFIFOThrLevel;		/*!< Specifies TX FIFO interrupt threshold in TX mode. When TX FIFO depth <= threshold value, trigger interrupt.
							This parameter can be a value of @ref IR_Tx_Threshold */
	u32 IR_TxCOMP_CLK;			/*!< Specifies TX compensation clk. This CLK used when @ref IR_Tx_DATA_TYPE = IR_TX_CLK_Self_Def.*/
	u32 IR_RxStartMode;			/*!< Specifies Start mode in RX mode
							This parameter can be a value of @ref IR_Rx_Start_Mode */
	u32 IR_RxFIFOThrLevel;		/*!< Specifies RX FIFO interrupt threshold in RX mode. when RX FIFO depth > threshold value, trigger interrupt.
							This parameter can be a value of @ref IR_Rx_Threshold */
	u32 IR_RxFIFOFullCtrl;		/*!< Specifies data discard mode in RX mode when RX FIFO is full and receiving new data
							This parameter can be a value of @ref IR_RX_FIFO_DISCARD_SETTING */
	u32 IR_RxTriggerMode;		/*!< Specifies trigger in RX mode
							This parameter can be a value of @ref IR_RX_Trigger_Mode */
	u32 IR_RxFilterTime;			/*!< Specifies filter time in RX mode
							This parameter can be a value of @ref IR_RX_Filter_Time */
	u32 IR_RxCntThrType;		/*!< Specifies counter level type when trigger IR_INT_RX_CNT_THR interrupt in RX mode
							This parameter can be a value of @ref IR_RX_COUNTER_THRESHOLD_TYPE */
	u32 IR_RxCntThr;				/*!< Specifies counter threshold value when trigger IR_INT_RX_CNT_THR interrupt in RX mode */
} IR_InitTypeDef, *PIR_InitTypeDef;

/** End of group IR_Exported_Types
  * @}
  */


/*============================================================================*
 *                         Constants
 *============================================================================*/


/** @defgroup IR_Exported_Constants IR Exported Constants
  * @{
  */
  
  /** @defgroup IR_Peripheral_definitions 
  * @{
  */
#define IS_IR_ALL_PERIPH(PERIPH) ((PERIPH) == IR_DEV)
/**
  * @}
  */
  
/** @defgroup IR_Frequency IR Frequency
  * @{
  */
#define IS_IR_FREQUENCY(F) 							(((F) >= 5000) && ((F) <= 500000))
/**
  * @}
  */

/** @defgroup IR_TRXFIFO
  * @{
  */
#define IR_TX_FIFO_SIZE                   					32
#define IR_RX_FIFO_SIZE                  					32
/**
  * @}
  */

/** @defgroup IR_Mode IR Mode
  * @{
  */
#define IR_MODE_TX									(0x00000000U << 31)
#define IR_MODE_RX									(0x00000001U << 31)

#define IR_MODE(MODE)								((MODE) & (0x00000001 << 31))
#define IS_IR_MODE(MODE) 							(((MODE) == IR_MODE_TX) || ((MODE) == IR_MODE_RX))

/** 
  * @}
  */

  
/** @defgroup IR_TX_Data_LEVEL IR TX Data Level
  * @{
  */
#define IR_TX_DATA_NORMAL							(0x00000000 << 14)
#define IR_TX_DATA_INVERSE							(0x00000001 << 14)
#define IR_TX_DATA_CARRIER_NORMAL				(0x00000000 << 13)
#define IR_TX_DATA_CARRIER_INVERSE				(0x00000001 << 13)
#define IR_TX_DATA_LEVEL_MASK						(IR_TX_DATA_INVERSE|IR_TX_DATA_CARRIER_INVERSE)

#define IS_IR_TX_DATA_TYPE(TYPE) 					(((TYPE) & (~IR_TX_DATA_LEVEL_MASK)) == 0)

/** End of group IR_TX_Data_LEVEL
  * @}
  */
  
/** @defgroup IR_Tx_Threshold IR TX Threshold
  * @{
  */
#define IR_TX_FIFO_THRESHOLD(THD)					((THD) << 8)
#define IS_IR_TX_FIFO_THRESHOLD(THD)				((THD) <= IR_TX_FIFO_SIZE)
#define IR_TX_FIFO_THRESHOLD_MASK					((0x0000001f) << 8)

/** End of group IR_Tx_Threshold
  * @}
  */
  
/** @defgroup IR_Idle_Status  IR Idle Status
  * @{
  */
#define IR_IDLE_OUTPUT_LOW							(0x00000000 << 6)
#define IR_IDLE_OUTPUT_HIGH						(0x00000001 << 6)
#define IS_IR_IDLE_STATUS(LEVEL)					(((LEVEL) == IR_IDLE_OUTPUT_HIGH) || \
												((LEVEL) == IR_IDLE_OUTPUT_LOW))

/** End of group IR_Idle_Status
  * @}
  */
  
/** @defgroup IR_Rx_Start_Ctrl
  * @{
  */
#define IR_RX_AUTO_MODE							(0x00000001 << 27)
#define IR_RX_MANUAL_MODE							(0x00000000 << 27)

#define IS_RX_START_MODE(MODE) 					(((MODE) == IR_RX_AUTO_MODE) || \
												((MODE) == IR_RX_MANUAL_MODE))

#define IS_RX_RX_TRIGGER_EDGE(EDGE) 				(((EDGE) == IR_RX_FALL_EDGE) || \
												((EDGE) == IR_RX_RISING_EDGE) || \
												((EDGE) == IR_RX_DOUBLE_EDGE))
/** End of group IR_Rx_Start_Ctrl
  * @}
  */

/** @defgroup IR_RX_Filter_Time 
  * @{
  */
#define IR_RX_FILTER_TIME_20NS						(0x00000000 << 21)
#define IR_RX_FILTER_TIME_30NS						(0x00000001 << 21)
#define IR_RX_FILTER_TIME_40NS						(0x00000002 << 21)
#define IR_RX_FILTER_TIME_50NS						(0x00000003 << 21)
#define IR_RX_FILTER_TIME_60NS						(0x00000004 << 21)
#define IR_RX_FILTER_TIME_70NS						(0x00000005 << 21)
#define IR_RX_FILTER_TIME_80NS						(0x00000006 << 21)
#define IR_RX_FILTER_TIME_90NS						(0x00000007 << 21)

#define IS_IR_RX_FILTER_TIME_CTRL(CTRL)			(((CTRL) == IR_RX_FILTER_TIME_20NS) || \
												((CTRL) == IR_RX_FILTER_TIME_30NS) || \
												((CTRL) == IR_RX_FILTER_TIME_40NS) || \
												((CTRL) == IR_RX_FILTER_TIME_50NS) || \
												((CTRL) == IR_RX_FILTER_TIME_60NS) || \
												((CTRL) == IR_RX_FILTER_TIME_70NS) || \
												((CTRL) == IR_RX_FILTER_TIME_80NS) || \
												((CTRL) == IR_RX_FILTER_TIME_90NS))
/** End of group IR_RX_Filter_Time
  * @}
  */

/** @defgroup IR_RX_FIFO_DISCARD_SETTING 
  * @{
  */

#define IR_RX_FIFO_FULL_DISCARD_NEWEST			(0x00000001 << 13)
#define IR_RX_FIFO_FULL_DISCARD_OLDEST			(0x00000000 << 13)

#define IS_IR_RX_FIFO_FULL_DISCARD_CTRL(CTRL)	(((CTRL) == IR_RX_FIFO_FULL_DISCARD_NEWEST) || \
												((CTRL) == IR_RX_FIFO_FULL_DISCARD_OLDEST))

/** End of group IR_RX_FIFO_DISCARD_SETTING
  * @}
  */

/** @defgroup IR_Rx_Threshold IR RX Threshold
  * @{
  */
  
#define IR_RX_FIFO_THRESHOLD(THD)					((THD) <<8)
#define IS_IR_RX_FIFO_THRESHOLD(THD)				((THD) <= IR_RX_FIFO_SIZE)
#define IR_RX_FIFO_THRESHOLD_MASK 					((0x0000001f) << 8)

/** End of group IR_Rx_Threshold
  * @}
  */

/** @defgroup IR_INT_EN
  * @{
  */
#define IS_TX_INT_MASK(MASK)						(((MASK) & (~IR_TX_INT_ALL_MASK)) == 0)
#define IS_TX_INT(MASK)								(((MASK) & (~IR_TX_INT_ALL_EN)) == 0)	
#define IS_TX_INT_CLR(MASK)							(((MASK) & (~IR_TX_INT_ALL_CLR)) == 0)

#define IS_RX_INT_MASK(MASK)						(((MASK) & (~IR_RX_INT_ALL_MASK)) == 0)	
#define IS_RX_INT(MASK)								(((MASK) & (~IR_RX_INT_ALL_EN)) == 0)	
#define IS_RX_INT_CLR(MASK)							(((MASK) & (~IR_RX_INT_ALL_CLR)) == 0)	

/** End of group IR_Rx_INT_EN
  * @}
  */

/** @defgroup IR_RX_COUNTER_THRESHOLD
  * @{
  */

#define IR_RX_COUNT_LOW_LEVEL						(0x00000000U << 31)
#define IR_RX_COUNT_HIGH_LEVEL						(0x00000001U << 31)
#define IS_IR_RX_COUNT_LEVEL_CTRL(CTRL)			(((CTRL) == IR_RX_COUNT_LOW_LEVEL) || \
												((CTRL) == IR_RX_COUNT_HIGH_LEVEL))

#define IS_IR_RX_COUNTER_THRESHOLD(THD) 			((THD) <= 0x7fffffff)

/** End of group IR_RX_COUNTER_THRESHOLD
  * @}
  */

/** End of group IR_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/


/** @defgroup IR_Exported_Functions IR Exported Functions
  * @{
  */

void IR_DeInit(void);
void IR_Init(IR_TypeDef *IRx, IR_InitTypeDef *IR_InitStruct);
void IR_StructInit(IR_InitTypeDef *IR_InitStruct);
void IR_Cmd(IR_TypeDef *IRx, u32 mode, u32 NewState);
void IR_SetRxCounterThreshold(IR_TypeDef *IRx, u32 IR_RxCntThrType, u32 IR_RxCntThr);
void IR_SendBuf(IR_TypeDef *IRx, u32 *pBuf, u32 len, u32 IsLastPacket);
void IR_ReceiveBuf(IR_TypeDef *IRx, u32 *pBuf, u32 len);
void IR_INTConfig(IR_TypeDef *IRx, u32 IR_INT, u32 newState);
void IR_MaskINTConfig(IR_TypeDef *IRx, u32 IR_INT, u32 newState);
u32 IR_GetINTStatus(IR_TypeDef *IRx);
u32 IR_GetIMR(IR_TypeDef *IRx);
u32 IR_FSMRunning(IR_TypeDef *IRx);
void IR_ClearINTPendingBit(IR_TypeDef *IRx, u32 IR_CLEAR_INT);
void IR_SetTxThreshold(IR_TypeDef *IRx, uint8_t thd);
void IR_SetRxThreshold(IR_TypeDef *IRx, uint8_t thd);
u32 IR_GetTxFIFOFreeLen(IR_TypeDef *IRx);
u32 IR_GetRxDataLen(IR_TypeDef *IRx);
void IR_SendData(IR_TypeDef *IRx, u32 data);
void IR_StartManualRxTrigger(IR_TypeDef *IRx);
u32 IR_ReceiveData(IR_TypeDef *IRx);
void IR_ClearTxFIFO(IR_TypeDef *IRx);
void IR_ClearRxFIFO(IR_TypeDef *IRx);

/** @} */ /* End of group IR_Exported_Functions */

/* Registers Definitions --------------------------------------------------------*/

/**************************************************************************//**
 * @defgroup IR_Register_Definitions IR Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup IR_CLK_DIV
 * @{
 *****************************************************************************/
#define IR_CLOCK_DIV						((u32)0x0000003F)				 /*BIT[11:0], IR CLOCK DIV*/
/** @} */

/**************************************************************************//**
 * @defgroup IR_TX_CONFIG
 * @{
 *****************************************************************************/
#define IR_MODE_SEL							((u32)0x00000001 << 31)		/*BIT[31], IR Work mode*/
#define IR_TX_START							((u32)0x00000001 << 30)		/*BIT[30], IR TX FSM start*/
#define IR_TX_DUTY_NUM						((u32)0x0000003F << 16)		/*BIT[27:16], IR TX DUTY NUM*/
#define IR_TX_OUTPUT_INVERSE				((u32)0x00000001 << 14)		/*BIT[14], IR Inverse  active output*/
#define IR_TX_DE_INVERSE					((u32)0x00000001 << 13)		/*BIT[13], IR Inverse FIFO define*/
#define IR_TX_FIFO_LEVEL_TH				((u32)0x0000000F << 8)		/*BIT[12:8], IR TX FIFO interrupt threshold*/
#define IR_TX_IDEL_STATE					((u32)0x00000001 << 6)		/*BIT[6], IR TX output State in idle*/
#define IR_TX_FIFO_OVER_INT_MASK			((u32)0x00000001 << 5)		/*BIT[5], IR TX FIFO overflow Interrupt mask*/
#define IR_TX_FIFO_OVER_INT_EN				((u32)0x00000001 << 4)		/*BIT[4], IR TX FIFO overflow Interrupt enable*/
#define IR_TX_FIFO_LEVEL_INT_MASK			((u32)0x00000001 << 3)		/*BIT[3], IR TX FIFO Level Interrupt mask*/
#define IR_TX_FIFO_EMPTY_INT_MASK			((u32)0x00000001 << 2)		/*BIT[2], IR TX FIFO Empty Interrupt mask*/
#define IR_TX_FIFO_LEVEL_INT_EN			((u32)0x00000001 << 1)		/*BIT[1], IR TX FIFO Level Interrupt enable*/
#define IR_TX_FIFO_EMPTY_INT_EN			((u32)0x00000001 << 0)		/*BIT[0], IR TX FIFO Empty Interrupt enable*/

#define IR_TX_INT_ALL_MASK					((u32)0x0000002C)
#define IR_TX_INT_ALL_EN					((u32)0x00000013)

/** @} */

/**************************************************************************//**
 * @defgroup IR_TX_SR
 * @{
 *****************************************************************************/
#define IR_TX_FIFO_EMPTY_STATUS			((u32)0x00000001 << 15)		/*BIT[15], IR TX_FIFO_EMPTY status*/
#define IR_TX_FIFO_FULL_STATUS				((u32)0x00000001 << 14)		/*BIT[14], IR TX_FIFO_FULL status*/
#define IR_TX_FIFO_LEVEL					((u32)0x0000003f << 8)		/*BIT[13:8], IR TX FIFO DATA number*/
#define IR_TX_FSM_STATUS					((u32)0x00000001 << 4)		/*BIT[4], IR TX FSM status*/
#define IR_TX_FIFO_OVER_INT_STATUS		((u32)0x00000001 << 2)		/*BIT[2], IR TX FIFO overflow Interrupt status*/
#define IR_TX_FIFO_LEVEL_INT_STATUS		((u32)0x00000001 << 1)		/*BIT[1], IR TX FIFO Level Interrupt stauts*/
#define IR_TX_FIFO_EMPTY_INT_STATUS		((u32)0x00000001 << 0)		/*BIT[0], IR TX FIFO Empty Interrupt status*/
/** @} */

/**************************************************************************//**
 * @defgroup IR_TX_COMPE_DIV
 * @{
 *****************************************************************************/
#define IR_TX_COMPESATION_DIV				((u32)0x0000003F)			 	/*BIT[11:0], IR COMP CLOCK DIV*/
/** @} */

/**************************************************************************//**
 * @defgroup IR_TX_INT_CLR
 * @{
 *****************************************************************************/
 #define IR_TX_FIFO_OVER_INT_CLR			((u32)0x00000001 << 3)		/*BIT[3], IR TX FIFO overflow Interrupt clear*/
#define IR_TX_FIFO_LEVEL_INT_CLR			((u32)0x00000001 << 2)		/*BIT[2], IR TX FIFO Level Interrupt clear*/
#define IR_TX_FIFO_EMPTY_INT_CLR			((u32)0x00000001 << 1)		/*BIT[1], IR TX FIFO Empty Interrupt clear*/
#define IR_TX_FIFO_CLR						((u32)0x00000001 << 0)		/*BIT[0], IR TX FIFO clear*/

#define IR_TX_INT_ALL_CLR					((u32)0x0000000E)

/** @} */

/**************************************************************************//**
 * @defgroup IR_TX_FIFO
 * @{
 *****************************************************************************/
#define IR_TX_DATA_CARRIER_MASK			((u32)0x00000001 << 31)		/*BIT[31], IR TX DATA Carrier*/
#define IR_TX_DATA_END_MASK				((u32)0x00000001 << 30)		/*BIT[30], IR TX End Flag*/

#define IR_TX_CLK_NORMAL					((u32)0x00000000 << 28)		/*BIT[29:28], IR TX DATA cycle  unit = 1*carrier cycle*/
#define IR_TX_CLK_1P5						((u32)0x00000001 << 28)		/*BIT[29:28], IR TX DATA cycle  unit = 1.5*carrier cycle*/
#define IR_TX_CLK_1P25						((u32)0x00000002 << 28)		/*BIT[29:28], IR TX DATA cycle  unit = 1.25*carrier cycle*/
#define IR_TX_CLK_COMP						((u32)0x00000003 << 28)		/*BIT[29:28], IR TX DATA cycle  unit = 1*compensation cycle*/

#define IR_TX_DATA_TIME					((u32)0x0FFFFFFF << 0)		/*BIT[27:0], IR TX FIFO overflow Interrupt clear*/

/** @} */

/**************************************************************************//**
 * @defgroup IR_RX_CONFIG
 * @{
 *****************************************************************************/
#define IR_RX_START							((u32)0x00000001 << 28)		/*BIT[28], IR RX FSM start*/
#define IR_RX_START_MODE					((u32)0x00000001 << 27)		/*BIT[27], IR RX start mode*/
#define IR_RX_MANUAL_START				((u32)0x00000001 << 26)		/*BIT[26], IR RX manual start trigger*/

#define IR_RX_FALL_EDGE						((u32)0x00000000 << 24)		/*BIT[25:24], IR RX auto start trigger edge*/
#define IR_RX_RISING_EDGE					((u32)0x00000001 << 24)		/*BIT[25:24], IR RX auto start trigger edge*/
#define IR_RX_DOUBLE_EDGE					((u32)0x00000002 << 24)		/*BIT[25:24], IR RX auto start trigger edge*/
#define IR_RX_FILTER_STAGETX				((u32)0x00000002 << 21)		/*BIT[23:21], IR RX filter*/

#define IR_RX_FIFO_ERROR_INT_MASK			((u32)0x00000001 << 19)		/*BIT[19], IR RX FIFO read underflow Interrupt mask*/
#define IR_RX_CNT_THR_INT_MASK			((u32)0x00000001 << 18)		/*BIT[18], IR RX CNT threshold Interrupt mask*/
#define IR_RX_FIFO_OF_INT_MASK			((u32)0x00000001 << 17)		/*BIT[17], IR RX FIFO overflow Interrupt mask*/
#define IR_RX_CNT_OF_INT_MASK				((u32)0x00000001 << 16)		/*BIT[16], IR RX CNT overflow Interrupt mask*/
#define IR_RX_FIFO_LEVEL_INT_MASK			((u32)0x00000001 << 15)		/*BIT[15], IR RX FIFO threshold Interrupt mask*/
#define IR_RX_FIFO_FULL_INT_MASK			((u32)0x00000001 << 14)		/*BIT[14], IR RX FIFO FULL Interrupt mask*/
#define IR_RX_INT_ALL_MASK					((u32)0x0000003F << 14)		

#define IR_RX_FIFO_DISCARD_SET				((u32)0x00000001 << 13)		/*BIT[13], IR RX FIFO discard set*/
#define IR_RX_FIFO_LEVEL_TH				((u32)0x0000000F << 8)		/*BIT[12:8], IR TX FIFO interrupt threshold*/
#define IR_RX_FIFO_ERROR_INT_EN 			((u32)0x00000001 << 5)		/*BIT[5], IR RX FIFO read underflow Interrupt enable*/
#define IR_RX_CNT_THR_INT_EN				((u32)0x00000001 << 4)		/*BIT[4], IR RX CNT threshold Interrupt enable*/
#define IR_RX_FIFO_OF_INT_EN				((u32)0x00000001 << 3)		/*BIT[3], IR RX FIFO overflow Interrupt enable*/
#define IR_RX_CNT_OF_INT_EN				((u32)0x00000001 << 2)		/*BIT[2], IR RX CNT overflow Interrupt enable*/
#define IR_RX_FIFO_LEVEL_INT_EN			((u32)0x00000001 << 1)		/*BIT[1], IR RX FIFO threshold Interrupt enable*/
#define IR_RX_FIFO_FULL_INT_EN				((u32)0x00000001 << 0)		/*BIT[0], IR RX FIFO FULL Interrupt enable*/
#define IR_RX_INT_ALL_EN					((u32)0x0000003F)
/** @} */

/**************************************************************************//**
 * @defgroup IR_RX_SR
 * @{
 *****************************************************************************/
#define IR_RX_FIFO_EMPTY_STATUS			((u32)0x00000001 << 17)		/*BIT[17], IR RX FIFO Empty  status*/	
#define IR_RX_FIFO_LEVEL					((u32)0x0000003f << 8)		/*BIT[13:8], IR RX FIFO Level status*/	
#define IR_RX_FSM_STATUS					((u32)0x00000001 << 7)		/*BIT[7], IR RX FSM status*/	
#define IR_RX_FIFO_ERROR_INT_STATUS		((u32)0x00000001 << 5)		/*BIT[5], IR RX FIFO read underflow Interrupt status*/	
#define IR_RX_CNT_THR_INT_STATUS			((u32)0x00000001 << 4)		/*BIT[4], IR RX CNT threshold Interrupt status*/	
#define IR_RX_FIFO_OF_INT_STATUS			((u32)0x00000001 << 3)		/*BIT[3], IR_RX FIFO overflow Interrupt status*/	
#define IR_RX_CNT_OF_INT_STATUS			((u32)0x00000001 << 2)		/*BIT[2], IR_RX CNT overflow Interrupt status*/	
#define IR_RX_FIFO_LEVEL_INT_STATUS		((u32)0x00000001 << 1)		/*BIT[1], IR_RX FIFO threshold Interrupt status*/	
#define IR_RX_FIFO_FULL_INT_STATUS		((u32)0x00000001 << 0)		/*BIT[0], IR RX FIFO FULL Interrupt status*/	
/** @} */

/**************************************************************************//**
 * @defgroup IR_RX_INT_CLR
 * @{
 *****************************************************************************/
#define IR_RX_FIFO_CLR						((u32)0x00000001 << 8)		/*BIT[8], IR TX FIFO clear*/
#define IR_RX_FIFO_ERROR_INT_CLR			((u32)0x00000001 << 5)		/*BIT[5], IR RX FIFO read underflow Interrupt clear*/
#define IR_RX_CNT_THR_INT_CLR				((u32)0x00000001 << 4)		/*BIT[4], IR RX CNT threshold Interrupt clear*/
#define IR_RX_FIFO_OF_INT_CLR				((u32)0x00000001 << 3)		/*BIT[3], IR_RX FIFO overflow Interrupt clear*/
#define IR_RX_CNT_OF_INT_CLR				((u32)0x00000001 << 2)		/*BIT[2], IR_RX CNT overflow Interrupt clear*/
#define IR_RX_FIFO_LEVEL_INT_CLR			((u32)0x00000001 << 1)		/*BIT[1], IR_RX FIFO threshold Interrupt clear*/
#define IR_RX_FIFO_FULL_INT_CLR			((u32)0x00000001 << 0)		/*BIT[0], IR RX FIFO FULL Interrupt clear*/
#define IR_RX_INT_ALL_CLR					((u32)0x0000003F)
/** @} */

/**************************************************************************//**
 * @defgroup IR_RX_CNT_INT_SEL
 * @{
 *****************************************************************************/
#define IR_RX_COUNT_LEVEL					((u32)0x00000001 << 31)		/*BIT[31], IR RX CNT LEVEL*/
#define IR_RX_COUNTER_THRESHOLD 			((u32)0x7FFFFFFF)				/*BIT[30:0], IR RX CYCLE*/
/** @} */

/**************************************************************************//**
 * @defgroup IR_RX_FIFO
 * @{
 *****************************************************************************/
#define IR_RX_LEVEL							((u32)0x00000001 << 31)		/*BIT[31], IR RX  LEVEL*/
#define IR_RX_COUNTER						((u32)0x7FFFFFFF)				/*BIT[30:0], IR RX CYCLE*/
/** @} */

/**
  * @}
  */
  
/* Other Definitions --------------------------------------------------------*/


/** @} */ /* End of group IR */

/** @} */ /* End of group AmebaD_Periph_Driver */
#endif /* _RTL8721D_IR_H_ */

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
