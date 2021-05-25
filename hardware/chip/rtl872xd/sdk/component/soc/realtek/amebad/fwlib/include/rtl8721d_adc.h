/**
  ******************************************************************************
  * @file    rtl8721d_adc.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the ADC firmware
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

#ifndef _RTL8721D_ADC_H_
#define _RTL8721D_ADC_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup ADC
  * @brief ADC driver modules
  * @{
  */

/** @addtogroup ADC
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * ADC:
  * 	- Base Address: ADC
  * 	- Channel number: 11
  *		- CH8~10: internal ADC channel
  *		- CH0~7: external ADC channel
  * 	- Sample rate: configurable, frequency = CLK_ADC_CORE/divider, in which CLK_ADC_CORE is 2MHz, and can
  *		be divided by 12/16/32/64.
  * 	- Resolution: 12 bit
  * 	- Analog signal sampling: support 0 ~ 3.3V
  * 	- IRQ: ADC_IRQ
  * 	- Support Software-Trigger mode, Automatic mode, Timer-Trigger mode and Comparator-Assist mode.
  * 	- GDMA source handshake interface: GDMA_HANDSHAKE_INTERFACE_ADC_RX
  *
  *****************************************************************************************
  * How to use ADC in interrupt mode
  *****************************************************************************************
  * 	  To use ADC in interrupt mode, the following steps are mandatory:
  *
  *      1. Enable the ADC interface clock:
  *			RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);
  *
  *      2. Fill the ADC_InitStruct with the desired parameters.
  *
  *      3. Init Hardware with the parameters in ADC_InitStruct.
  *			ADC_Init(ADC_InitTypeDef* ADC_InitStruct)
  *
  *      4. Clear ADC interrupt:
  *			ADC_INTClear()
  *
  *      5. To configure interrupts:
  *			ADC_INTConfig(ADC_IT, ENABLE)
  *      
  *      6. Enable the NVIC and the corresponding interrupt using following function.
  *			-InterruptRegister(): register the ADC irq handler 
  *			-InterruptEn(): Enable the NVIC interrupt and set irq priority
  *
  *      7. Activate the ADC:
  *			ADC_Cmd(ENABLE).
  *
  *      8. Enbale specified mode:
  *			ADC_SWTrigCmd(ENABLE)/ADC_AutoCSwCmd(ENABLE)/ADC_TimerTrigCmd(Tim_Idx, PeriodMs, ENABLE)
  *
  *      @note	1. If use ADC compare mode, call ADC_SetComp(ADC_channel, CompThresH, CompThresL, CompCtrl) to configure
  *                  2. If use ADC comparator-assist mode, you need to setup and enable comparator.
  *
  *****************************************************************************************
  * How to use ADC in DMA mode
  *****************************************************************************************
  * 	  To use ADC in DMA mode, the following steps are mandatory:
  *
  *      1. Enable the ADC interface clock:
  *			RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);
  *
  *      2. Fill the ADC_InitStruct with the desired parameters.
  *
  *      3. Init Hardware use step2 parameters.
  *			ADC_Init(ADC_InitTypeDef* ADC_InitStruct).
  *
  *      4. Enable DMA read mode.
  *			ADC_SetDmaEnable().
  *
  *      5. Init and Enable ADC RX GDMA, configure GDMA related configurations(source address/destination address/block size etc.)
  *			ADC_RXGDMA_Init().
  *
  *      6. Activate the ADC peripheral:
  *			ADC_Cmd(ENABLE).
  *     
  *      7.  Enbale specified mode:
  *			ADC_SWTrigCmd(ENABLE)/ADC_AutoCSwCmd(ENABLE)/ \
  *                            ADC_TimerTrigCmd(Tim_Idx, PeriodMs, ENABLE)
  *
  *****************************************************************************************
  * @endverbatim
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup ADC_Exported_Types ADC Exported Types
  * @{
  */

/**
  * @brief  ADC Init structure definition
  */
typedef struct {
	u8 ADC_OpMode;				/*!< Specifies ADC operation mode.
									This parameter can be a value of @ref ADC_Operation_Mode_Definitions */

	u8 ADC_CvlistLen;				/*!< The number of valid items in the ADC conversion channel list is (ADC_CvlistLen + 1).
									This parameter can be set to 0~15 */

	u8 ADC_Cvlist[16];				/*!< Specifies the ADC channel conversion order. Each member should be 
									the channel index */

	u8 ADC_ClkDiv;				/*!< Specifies ADC clock divider.
									This parameter can be a value of @ref ADC_CLK_Divider_Definitions */

	u8 ADC_RxThresholdLevel;		/*!< Specifies the receive FIFO threshold level.
							    		When the number of rx FIFO entries is greater than or equal to this
							     		value +1, the receive FIFO full interrupt is triggered. */

	u8 ADC_DMAThresholdLevel;	/*!< Specifies ADC DMA operation threshold.
									The dma_rx_req is generated when the number of valid data entries in the
									receive FIFO is equal to or above this field value+1 and ADC_DMA_EN=1. */

	u8  ADC_SpecialCh;			/*!< Specifies ADC particular channel. This parameter defines that ADC module 
									should send interrupt signal to system when a conversion which of channel 
									number is the same as this parameter. Default 0xFF means there is no need
									to set particular channel. */

	u32 ADC_ChanInType; 		/*!< Specifies CH0~5 input type. Default all channels are in single-end mode.
									If some channels need to be set to differential mode, use a value or 
									combination of @ref ADC_CH_Input_Type_Definitions. */

	u8 ADC_ChIDEn;				/*!< Specifies whether ADC enables BIT_ADC_DAT_CHID or not. */

} ADC_InitTypeDef;
/**
  * @}
  */


/* Exported constants --------------------------------------------------------*/

/** @defgroup ADC_Exported_Constants ADC Exported Constants
  * @{
  */

/** @defgroup ADC_Chn_Selection
  * @{
  */
#define ADC_CH0							((u8)0x00)
#define ADC_CH1							((u8)0x01)
#define ADC_CH2							((u8)0x02)
#define ADC_CH3							((u8)0x03)
#define ADC_CH4							((u8)0x04)
#define ADC_CH5							((u8)0x05)
#define ADC_CH6							((u8)0x06)
#define ADC_CH7							((u8)0x07)
#define ADC_CH8							((u8)0x08)	/*!< ADC internal channel */
#define ADC_CH9							((u8)0x09)	/*!< ADC internal channel */
#define ADC_CH10							((u8)0x0a)	/*!< ADC internal channel */
#define ADC_CH_NUM						(11)
#define ADC_GLOBAL						((u8)0xFF)

#define IS_ADC_CHN_SEL(SEL)     (((SEL) == ADC_CH0) || \
									((SEL) == ADC_CH1) || \
									((SEL) == ADC_CH2) || \
									((SEL) == ADC_CH3) || \
									((SEL) == ADC_CH4) || \
									((SEL) == ADC_CH5) || \
									((SEL) == ADC_CH6) || \
									((SEL) == ADC_CH7) || \
									((SEL) == ADC_CH8) || \
									((SEL) == ADC_CH9) || \
									((SEL) == ADC_CH10))

/**
  * @}
  */


/** @defgroup ADC_CLK_Divider_Definitions
  * @{
  */
#define ADC_CLK_DIV_12					((u8)0x00)
#define ADC_CLK_DIV_16					((u8)0x01)
#define ADC_CLK_DIV_32					((u8)0x02)
#define ADC_CLK_DIV_64					((u8)0x03)

#define IS_ADC_SAMPLE_CLK(CLK)     (((CLK) == ADC_CLK_DIV_12) || \
										((CLK) == ADC_CLK_DIV_16) || \
										((CLK) == ADC_CLK_DIV_32) || \
										((CLK) == ADC_CLK_DIV_64))

/**
  * @}
  */


/** @defgroup ADC_Operation_Mode_Definitions
  * @{
  */
#define ADC_SW_TRI_MODE				((u8)0x00)	/*!< ADC software-trigger mode */
#define ADC_AUTO_MODE					((u8)0x01)	/*!< ADC automatic mode */
#define ADC_TIM_TRI_MODE				((u8)0x02)	/*!< ADC timer-trigger mode */
#define ADC_COMP_ASSIST_MODE			((u8)0x03)	/*!< ADC comparator-assist mode */

#define IS_ADC_MODE(mode)     (((mode) == ADC_SW_TRI_MODE) || \
										((mode) == ADC_AUTO_MODE) || \
										((mode) == ADC_TIM_TRI_MODE) || \
										((mode) == ADC_COMP_ASSIST_MODE))

/**
  * @}
  */


/** @defgroup ADC_CH_Input_Type_Definitions
  * @{
  */
#define ADC_DIFFERENTIAL_CH(x)				BIT(x)

/**
  * @}
  */

/** @defgroup ADC_Compare_Control_Definitions
  * @{
  */
#define ADC_COMP_SMALLER_THAN_THL			((u8)0x00)	/*!< Vin < ADC_COMP_TH_L */
#define ADC_COMP_GREATER_THAN_THH		((u8)0x01)	/*!< Vin > ADC_COMP_TH_H */
#define ADC_COMP_WITHIN_THL_AND_THH		((u8)0x02)	/*!< Vin >= ADC_COMP_TH_L &&  Vin <= ADC_COMP_TH_H */
#define ADC_COMP_OUTSIDE_THL_AND_THH		((u8)0x03)	/*!< Vin < ADC_COMP_TH_L  || Vin > ADC_COMP_TH_H */
#define IS_ADC_COMP_CRITERIA(rule)			(((rule) == ADC_COMP_SMALLER_THAN_THL) || \
												((rule) == ADC_COMP_GREATER_THAN_THH) || \
												((rule) == ADC_COMP_WITHIN_THL_AND_THH) || \
												((rule) == ADC_COMP_OUTSIDE_THL_AND_THH))

/**
  * @}
  */

/** @defgroup ADC_Compare_Threshold_Definitions
  * @{
  */
#define IS_ADC_VALID_COMP_TH(x)				(x < 0x1000)

/**
  * @}
  */

/** @defgroup ADC_Timer_Definitions
  * @{
  */
#define IS_ADC_VALID_TIM(idx)				(idx < 4)

/**
  * @}
  */


/**
  * @}
  */

/** @defgroup ADC_Exported_Functions ADC Exported Functions
  * @{
  */
_LONG_CALL_ void ADC_StructInit(ADC_InitTypeDef *ADC_InitStruct);
_LONG_CALL_ void ADC_Init(ADC_InitTypeDef* ADC_InitStruct);
_LONG_CALL_ void ADC_Cmd(u32 NewState);
_LONG_CALL_ void ADC_INTConfig(u32 ADC_IT, u32 NewState);
_LONG_CALL_ void ADC_INTClear(void);
_LONG_CALL_ void ADC_INTClearPendingBits(u32 ADC_IT);
_LONG_CALL_ u32 ADC_GetISR(void);
_LONG_CALL_ u32 ADC_GetRawISR(void);
_LONG_CALL_ u32 ADC_GetCompStatus(u8 ADC_Channel);
_LONG_CALL_ u32 ADC_GetRxCount(void);
_LONG_CALL_ u32 ADC_GetLastChan(void);
_LONG_CALL_ void ADC_SetComp(u8 ADC_channel, u16 CompThresH, u16 CompThresL, u8 CompCtrl);
_LONG_CALL_ void ADC_ResetCSwList(void);
_LONG_CALL_ u32 ADC_Readable(void);
_LONG_CALL_ u16 ADC_Read(void);
_LONG_CALL_ void ADC_ReceiveBuf(u16 *pBuf, u32 len);
_LONG_CALL_ void ADC_ClearFIFO(void);
_LONG_CALL_ u32 ADC_GetStatus(void);
_LONG_CALL_ void ADC_SWTrigCmd(u32 NewState);
_LONG_CALL_ void ADC_AutoCSwCmd(u32 NewState);
_LONG_CALL_ void ADC_TimerTrigCmd(u8 Tim_Idx, u32 PeriodMs, u32 NewState);
_LONG_CALL_ void ADC_SetDmaEnable(u32 newState);
_LONG_CALL_ u32 ADC_RXGDMA_Init(GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8* pDataBuf, u32 DataLen);
/**
  * @}
  */

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup ADC_Register_Definitions ADC Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup REG_ADC_CONF
 * @{
 *****************************************************************************/
#define BIT_ADC_ENABLE							BIT(9)
#define BIT_SHIFT_CVLIST_LEN					4
#define BIT_MASK_CVLIST_LEN						(u32)(0x0000000F << BIT_SHIFT_CVLIST_LEN)
#define BIT_SHIFT_OP_MODE						1					
#define BIT_MASK_OP_MODE						(u32)(0x00000007 << BIT_SHIFT_OP_MODE)
#define BIT_ADC_REF_IN_SEL						BIT(0)

/** @} */

/**************************************************************************//**
 * @defgroup REG_ADC_IN_TYPE
 * @{
 *****************************************************************************/
 #define BIT_SHIFT_IN_TYPE_CH(x)					(x)
 #define BIT_ADC_IN_TYPE_CH(x)					BIT(x)

/** @} */

/**************************************************************************//**
 * @defgroup REG_ADC_COMP_TH_CH
 * @{
 *****************************************************************************/
#define BIT_SHIFT_COMP_TH_H					16
#define BIT_MASK_COMP_TH_H						(u32)(0x00000FFF << BIT_SHIFT_COMP_TH_H)
#define BIT_SHIFT_COMP_TH_L						0
#define BIT_MASK_COMP_TH_L						(u32)(0x00000FFF << BIT_SHIFT_COMP_TH_L)

/** @} */

/**************************************************************************//**
 * @defgroup REG_ADC_COMP_CTRL
 * @{
 *****************************************************************************/
#define BIT_SHIFT_COMP_CTRL_CH(x)				(2*x)
#define BIT_MASK_COMP_CTRL_CH(x)				(u32)(0x00000003 << BIT_SHIFT_COMP_CTRL_CH(x))

/** @} */


/**************************************************************************//**
 * @defgroup REG_ADC_COMP_STS
 * @{
 *****************************************************************************/
#define BIT_SHIFT_COMP_STS_CH(x)				(2*x)
#define BIT_MASK_COMP_STS_CH(x)				(u32)(0x00000003 << BIT_SHIFT_COMP_STS_CH(x))

/** @} */

/**************************************************************************//**
 * @defgroup REG_ADC_CHSW_LIST0
 * @{
 *****************************************************************************/
#define BIT_SHIFT_CHSW0(x)						(4*x)
#define BIT_MASK_CHSW0(x)						(u32)(0x0000000F << BIT_SHIFT_CHSW0(x))

/** @} */

/**************************************************************************//**
 * @defgroup REG_ADC_CHSW_LIST1
 * @{
 *****************************************************************************/
 #define BIT_SHIFT_CHSW1(x)						(4*(x - 8))
 #define BIT_MASK_CHSW1(x)						(u32)(0x0000000F << BIT_SHIFT_CHSW1)

/** @} */

/**************************************************************************//**
 * @defgroup REG_ADC_RST_LIST
 * @{
 *****************************************************************************/
#define BIT_ADC_RST_LIST						BIT(0)

/** @} */

/**************************************************************************//**
 * @defgroup REG_ADC_AUTO_CSW_CTRL
 * @{
 *****************************************************************************/
#define BIT_ADC_AUTO_CSW_EN					BIT(0)

/** @} */

/**************************************************************************//**
 * @defgroup REG_ADC_SW_TRIG
 * @{
 *****************************************************************************/
#define BIT_ADC_SW_TRIG						BIT(0)

/** @} */

/**************************************************************************//**
 * @defgroup REG_ADC_LAST_CH
 * @{
 *****************************************************************************/
#define BIT_ADC_LAST_CH							(u32)(0x0000000F)

/** @} */

/**************************************************************************//**
 * @defgroup REG_ADC_BUSY_STS
 * @{
 *****************************************************************************/
#define BIT_ADC_FIFO_EMPTY						BIT(2)
#define BIT_ADC_FIFO_FULL_REAL					BIT(1)
#define BIT_ADC_BUSY_STS						BIT(0)

/** @} */

/**************************************************************************//**
 * @defgroup REG_ADC_INTR_CTRL
 * @{
 *****************************************************************************/
#define BIT_ADC_IT_COMP_CH_EN(x)				BIT(8+x)
#define BIT_ADC_IT_COMP_CH10_EN				BIT(18)
#define BIT_ADC_IT_COMP_CH9_EN					BIT(17)
#define BIT_ADC_IT_COMP_CH8_EN					BIT(16)
#define BIT_ADC_IT_COMP_CH7_EN					BIT(15)
#define BIT_ADC_IT_COMP_CH6_EN					BIT(14)
#define BIT_ADC_IT_COMP_CH5_EN					BIT(13)
#define BIT_ADC_IT_COMP_CH4_EN					BIT(12)
#define BIT_ADC_IT_COMP_CH3_EN					BIT(11)
#define BIT_ADC_IT_COMP_CH2_EN					BIT(10)
#define BIT_ADC_IT_COMP_CH1_EN					BIT(9)
#define BIT_ADC_IT_COMP_CH0_EN					BIT(8)
#define BIT_ADC_IT_ERR_EN						BIT(7)
#define BIT_ADC_IT_DAT_OVW_EN					BIT(6)
#define BIT_ADC_IT_FIFO_EMPTY_EN				BIT(5)
#define BIT_ADC_IT_FIFO_OVER_EN				BIT(4)
#define BIT_ADC_IT_FIFO_FULL_EN					BIT(3)
#define BIT_ADC_IT_CHCV_END_EN					BIT(2)
#define BIT_ADC_IT_CV_END_EN					BIT(1)
#define BIT_ADC_IT_CVLIST_END_EN				BIT(0)

/** @} */

/**************************************************************************//**
 * @defgroup REG_ADC_INTR_STS
 * @{
 *****************************************************************************/
#define BIT_ADC_IT_COMP_CH10_STS				BIT(18)
#define BIT_ADC_IT_COMP_CH9_STS				BIT(17)
#define BIT_ADC_IT_COMP_CH8_STS				BIT(16)
#define BIT_ADC_IT_COMP_CH7_STS				BIT(15)
#define BIT_ADC_IT_COMP_CH6_STS				BIT(14)
#define BIT_ADC_IT_COMP_CH5_STS				BIT(13)
#define BIT_ADC_IT_COMP_CH4_STS				BIT(12)
#define BIT_ADC_IT_COMP_CH3_STS				BIT(11)
#define BIT_ADC_IT_COMP_CH2_STS				BIT(10)
#define BIT_ADC_IT_COMP_CH1_STS				BIT(9)
#define BIT_ADC_IT_COMP_CH0_STS				BIT(8)
#define BIT_ADC_IT_ERR_STS						BIT(7)
#define BIT_ADC_IT_DAT_OVW_STS				BIT(6)
#define BIT_ADC_IT_FIFO_EMPTY_STS				BIT(5)
#define BIT_ADC_IT_FIFO_OVER_STS				BIT(4)
#define BIT_ADC_IT_FIFO_FULL_STS				BIT(3)
#define BIT_ADC_IT_CHCV_END_STS				BIT(2)
#define BIT_ADC_IT_CV_END_STS					BIT(1)
#define BIT_ADC_IT_CVLIST_END_STS				BIT(0)
#define BIT_ADC_IT_COMP_ALL_STS				( BIT_ADC_IT_COMP_CH0_STS | \
												BIT_ADC_IT_COMP_CH1_STS | \
												BIT_ADC_IT_COMP_CH2_STS | \
												BIT_ADC_IT_COMP_CH3_STS | \
												BIT_ADC_IT_COMP_CH4_STS | \
												BIT_ADC_IT_COMP_CH5_STS | \
												BIT_ADC_IT_COMP_CH6_STS | \
												BIT_ADC_IT_COMP_CH7_STS | \
												BIT_ADC_IT_COMP_CH8_STS | \
												BIT_ADC_IT_COMP_CH9_STS | \
												BIT_ADC_IT_COMP_CH10_STS )
#define BIT_ADC_IT_ALL_STS						(BIT_ADC_IT_COMP_ALL_STS | \
												BIT_ADC_IT_ERR_STS | \
												BIT_ADC_IT_DAT_OVW_STS |\
												BIT_ADC_IT_FIFO_EMPTY_STS |\
												BIT_ADC_IT_FIFO_OVER_STS |\
												BIT_ADC_IT_FIFO_FULL_STS |\
												BIT_ADC_IT_CHCV_END_STS |\
												BIT_ADC_IT_CV_END_STS |\
												BIT_ADC_IT_CVLIST_END_STS)

/** @} */

/**************************************************************************//**
 * @defgroup REG_ADC_INTR_RAW_STS
 * @{
 *****************************************************************************/
#define BIT_ADC_IT_COMP_CH10_RAW_STS				BIT(18)
#define BIT_ADC_IT_COMP_CH9_RAW_STS				BIT(17)
#define BIT_ADC_IT_COMP_CH8_RAW_STS				BIT(16)
#define BIT_ADC_IT_COMP_CH7_RAW_STS				BIT(15)
#define BIT_ADC_IT_COMP_CH6_RAW_STS				BIT(14)
#define BIT_ADC_IT_COMP_CH5_RAW_STS				BIT(13)
#define BIT_ADC_IT_COMP_CH4_RAW_STS				BIT(12)
#define BIT_ADC_IT_COMP_CH3_RAW_STS				BIT(11)
#define BIT_ADC_IT_COMP_CH2_RAW_STS				BIT(10)
#define BIT_ADC_IT_COMP_CH1_RAW_STS				BIT(9)
#define BIT_ADC_IT_COMP_CH0_RAW_STS				BIT(8)
#define BIT_ADC_IT_ERR_RAW_STS					BIT(7)
#define BIT_ADC_IT_DAT_OVW_RAW_STS				BIT(6)
#define BIT_ADC_IT_FIFO_EMPTY_RAW_STS				BIT(5)
#define BIT_ADC_IT_FIFO_OVER_RAW_STS				BIT(4)
#define BIT_ADC_IT_FIFO_FULL_RAW_STS				BIT(3)
#define BIT_ADC_IT_CHCV_END_RAW_STS				BIT(2)
#define BIT_ADC_IT_CV_END_RAW_STS					BIT(1)
#define BIT_ADC_IT_CVLIST_END_RAW_STS				BIT(0)

/** @} */


/**************************************************************************//**
 * @defgroup REG_ADC_IT_CHNO_CON
 * @{
 *****************************************************************************/
#define BIT_MASK_IT_CHNO_CON					(u32)(0x0000000F)

/** @} */

/**************************************************************************//**
 * @defgroup REG_ADC_FULL_LVL
 * @{
 *****************************************************************************/
#define BIT_MASK_FULL_LVL						(u32)(0x0000003F)

/** @} */

/**************************************************************************//**
 * @defgroup REG_ADC_ADC_EXT_TRIG_TIMER_SEL
 * @{
 *****************************************************************************/
#define BIT_MASK_EXT_TRIG_TIMER_SEL			(u32)(0x00000007)

/** @} */

/**************************************************************************//**
 * @defgroup REG_ADC_DATA_CH0_to_CH11
 * @{
 *****************************************************************************/
 #define BIT_ADC_DAT_RDY						BIT(17)
 #define BIT_ADC_DAT_OVW						BIT(16)
 #define BIT_MASK_DATA_CH						(u32)(0x00000FFF)

/** @} */

 /**************************************************************************//**
  * @defgroup REG_ADC_DATA_GLOBAL
  * @{
  *****************************************************************************/
 #define BIT_MASK_DAT_CH						(u32)(0x0000000F << 18)
 #define BIT_MASK_DAT_CHID						(u32)(0x0000000F << 12)
 #define BIT_MASK_DAT_GLOBAL					(u32)(0x00000FFF)
 
 /** @} */

 /**************************************************************************//**
  * @defgroup REG_ADC_DMA_CON
  * @{
  *****************************************************************************/
#define BIT_SHIFT_DMA_LVL						8
#define BIT_MASK_DMA_LVL						(u32)(0x0000000F << BIT_SHIFT_DMA_LVL)
#define BIT_ADC_DMA_EN							BIT(0)
 
 /** @} */

  /**************************************************************************//**
  * @defgroup REG_ADC_FLR
  * @{
  *****************************************************************************/
#define BIT_MASK_FLR							(u32)(0x0000007F)
 
 /** @} */

 /**************************************************************************//**
  * @defgroup REG_ADC_CLR_FIFO
  * @{
  *****************************************************************************/
#define BIT_ADC_CLR_FIFO						BIT(0)
 
 /** @} */

 /**************************************************************************//**
  * @defgroup REG_ADC_CLK_DIV
  * @{
  *****************************************************************************/
#define BIT_MASK_CLK_DIV						(u32)(0x00000007)
 
 /** @} */

 /**************************************************************************//**
  * @defgroup REG_ADC_DELAY_CNT
  * @{
  *****************************************************************************/
#define BIT_ADC_DAT_CHID						BIT(31)
#define BIT_ADC_DAT_DELAY						(u32)(0x00000003 << 2)
#define BIT_ADC_CTRL_DELAY						(u32)(0x00000003)
 /** @} */

 /**************************************************************************//**
  * @defgroup REG_ADC_PWR_CTRL
  * @{
  *****************************************************************************/
#define BIT_ADC_PWR_CTRL						BIT(0)
 
 /** @} */

/** @} */

/**
  * @}
  */

/**
  * @}
  */

/* Other Definitions --------------------------------------------------------*/


#endif /* _RTL8721D_ADC_H_ */

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
