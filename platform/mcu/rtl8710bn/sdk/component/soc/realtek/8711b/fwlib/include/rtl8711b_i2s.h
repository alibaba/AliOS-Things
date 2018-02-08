/**
  ******************************************************************************
  * @file    rtl8711b_i2s.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the I2S firmware
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

#ifndef _RTL8710B_I2S_H_
#define _RTL8710B_I2S_H_

/** @addtogroup AmebaZ_Periph_Driver
  * @{
  */

/** @defgroup I2S
  * @brief I2S driver modules
  * @{
  */

/** @addtogroup I2S
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * I2S:
  * 	- Base Address: I2S_DEV
  * 	- Source clk: 22.579MHz or 24.576MHz(default)
  * 	- Sample rate: 8K, 16K, 24K, 32K, 48K, 96K, 7.35K, 14.7K, 22.05K, 29.4K, 44.1K, 88.2K
  * 	- Sample bit: 16 bit, 24 bit
  * 	- Page num & page size: Max page_num=4, Max page_size=16K byte
  * 	- IRQ: I2S0_PCM0_IRQ
  *
  *****************************************************************************************
  * PINMUX
  *****************************************************************************************
  * I2S:
  * 	- S0:  _PA_18(mck)/_PA_19(tx)/_PA_20(rx)/_PA_21(clk)/_PA_22(ws).
  * 	- S1:  _PB_4(mck)/_PB_5(tx)/_PB_6(ws)/_PA_31(clk)/_PA_24(rx).
  *
  *****************************************************************************************
  * How to use I2S
  *****************************************************************************************
  *      To use I2S peripheral, the following steps are mandatory:
  *
  *      1.  Enable I2S power:
  *			LXBUS_FCTRL(ON).
  *
  *      2. Enable the I2S interface clock using
  *			RCC_PeriphClockCmd(APBPeriph_I2S0, APBPeriph_I2S0_CLOCK, ENABLE);
  *
  *      3. I2S pinmux:
  *			Pinmux_Config(Pin_Num, PINMUX_FUNCTION_I2S).
  *
  *      4. Fill the I2S_InitStruct with the desired parameters.
  *
  *      5. configure I2S with the corresponding configuration.
  *			I2S_Init(I2S_DEV, &I2S_Adapter.I2SInitStruct)
  *
  *      6. Activate the I2S peripheral:
  *			I2S_Cmd(I2S_DEV, ENABLE).
  *
  *      7. Configure interrupts:
  *			I2S_INTConfig()
  *
  *      @note    All other functions can be used separately to modify, if needed,
  *			a specific feature of the I2S
  *****************************************************************************************
  * @endverbatim
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup I2S_Exported_Types I2S Exported Types
  * @{
  */

/**
  * @brief	I2S Init structure definition
  */
typedef struct {
	u32 I2S_SlaveMode;		/*!< Specifies the I2S operating mode
								This parameter can be a value of @ref I2S_device_mode */

	u32 I2S_WordLen;		/*!< Specifies the I2S word length
								This parameter can be a value of @ref I2S_word_length */

	u32 I2S_Justify; 		/*!< Specifies the I2S digital interface format
								This parameter can be a value of @ref I2S_format */

	u32 I2S_EndianSwap; 	/*!< Specifies the I2S endian mode
								This parameter can be a value of @ref I2S_endian_swap */

	u32 I2S_ChNum;			/*!< Specifies the I2S channel number
								This parameter can be a value of @ref I2S_channel_number */

	u32 I2S_PageNum;		/*!< Specifies the I2S page number
								This parameter must be set to a value in the 2~4 range */

	u32 I2S_PageSize;		/*!< Specifies the I2S page size
								This parameter must be set to a value in the 1~4096 Word range */

	u32 I2S_Rate; 			/*!< Specifies the I2S sample rate
								This parameter can be a value of @ref I2S_sample_rate */

	u32 I2S_TRxAct;		/*!< Specifies the I2S transfer direction
								This parameter can be a value of @ref I2S_direction */

	u32 I2S_InterLoopback;/*!< Specifies the I2S internal/external loopback
								This parameter must be set to a value 0(external) or 1(internal) */
} I2S_InitTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup I2S_Exported_Constants I2S Exported Constants
  * @{
  */

/** @defgroup I2S_device_mode I2S Device Mode
  * @{
  */
#define I2S_MASTER_MODE				((u32)0x00000000)
#define I2S_SLAVE_MODE					((u32)0x00000001)

#define IS_I2S_MODE(MODE) (((MODE) == I2S_MASTER_MODE) || \
							((MODE) == I2S_SLAVE_MODE))

/**
  * @}
  */

/** @defgroup I2S_word_length I2S Word Length
  * @{
  */
#define I2S_WL_16						((u32)0x00000000)
#define I2S_WL_24						((u32)0x00000001)

#define IS_I2S_WORD_LEN(LEN) (((LEN) == I2S_WL_16) || \
								((LEN) == I2S_WL_24))

/**
  * @}
  */

/** @defgroup I2S_format I2S Interface Format
  * @{
  */
#define I2S_JY_I2S					((u32)0x00000000)
#define I2S_JY_LEFT					((u32)0x00000001)
#define I2S_JY_RIGHT				((u32)0x00000002)


#define IS_I2S_JUSTIFY(FORMAT) (((FORMAT) == I2S_JY_I2S) || \
									((FORMAT) == I2S_JY_LEFT) || \
									((FORMAT) == I2S_JY_RIGHT))

/**
  * @}
  */

/** @defgroup I2S_endian_swap I2S Endian Swap
  * @{
  */
#define I2S_ES_LITTLE					((u32)0x00000000)
#define I2S_ES_BIG						((u32)0x00000001)


#define IS_I2S_ENDIAN_SWAP(SWAP) (((SWAP) == I2S_ES_LITTLE) || \
									((SWAP) == I2S_ES_BIG))

/**
  * @}
  */

/** @defgroup I2S_direction I2S Bus Direction(Transmit/Receive)
  * @{
  */
#define I2S_ONLY_RX					((u32)0x00000000)
#define I2S_ONLY_TX					((u32)0x00000001)
#define I2S_TXRX						((u32)0x00000002)

#define IS_I2S_DIR(DIR) (((DIR) == I2S_ONLY_RX) || \
							((DIR) == I2S_ONLY_TX) || \
							((DIR) == I2S_TXRX))

/**
  * @}
  */

/** @defgroup I2S_channel_number I2S Channel Number
  * @{
  */
#define I2S_CH_STEREO					((u32)0x00000000)
#define I2S_CH_RSVD					((u32)0x00000001)
#define I2S_CH_MONO					((u32)0x00000002)

#define IS_I2S_CHN_NUM(NUM) (((NUM) == I2S_CH_STEREO) || \
								((NUM) == I2S_CH_MONO))

/**
  * @}
  */

/** @defgroup I2S_sample_rate I2S Sample Rate
  * @{
  */
#define I2S_SR_8KHZ					((u32)0x00000000)
#define I2S_SR_16KHZ					((u32)0x00000001)
#define I2S_SR_24KHZ					((u32)0x00000002)
#define I2S_SR_32KHZ					((u32)0x00000003)
#define I2S_SR_48KHZ					((u32)0x00000005)
#define I2S_SR_96KHZ					((u32)0x00000006)
#define I2S_SR_7p35KHZ					((u32)0x00000010)
#define I2S_SR_14p7KHZ					((u32)0x00000011)
#define I2S_SR_22p05KHZ				((u32)0x00000012)
#define I2S_SR_29p4KHZ					((u32)0x00000013)
#define I2S_SR_44p1KHZ					((u32)0x00000015)
#define I2S_SR_88p2KHZ					((u32)0x00000016)

#define IS_I2S_SAMPLE_RATE(RATE) (((RATE) == I2S_SR_8KHZ) || \
									((RATE) == I2S_SR_16KHZ) || \
									((RATE) == I2S_SR_24KHZ) || \
									((RATE) == I2S_SR_32KHZ) || \
									((RATE) == I2S_SR_48KHZ) || \
									((RATE) == I2S_SR_96KHZ) || \
									((RATE) == I2S_SR_7p35KHZ) || \
									((RATE) == I2S_SR_14p7KHZ) || \
									((RATE) == I2S_SR_22p05KHZ) || \
									((RATE) == I2S_SR_29p4KHZ) || \
									((RATE) == I2S_SR_44p1KHZ) || \
									((RATE) == I2S_SR_88p2KHZ))

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup I2S_Exported_Functions I2S Exported Functions
  * @{
  */
_LONG_CALL_ void I2S_Init(I2S_TypeDef* I2Sx, I2S_InitTypeDef* I2S_InitStruct);
_LONG_CALL_ void I2S_Cmd(I2S_TypeDef* I2Sx, u8  NewState);
_LONG_CALL_ void I2S_INTConfig(I2S_TypeDef* I2Sx, u32 I2STxIntrMSK, u32 I2SRxIntrMSK);
_LONG_CALL_ void I2S_SetRate(I2S_TypeDef* I2Sx, u32 I2S_Rate);
_LONG_CALL_ void I2S_SetWordLen(I2S_TypeDef* I2Sx, u32 I2S_WordLen);
_LONG_CALL_ void I2S_SetChNum(I2S_TypeDef* I2Sx, u32 I2S_ChNum);
_LONG_CALL_ void I2S_SetPageNum(I2S_TypeDef* I2Sx, u32 I2S_PageNum);
_LONG_CALL_ void I2S_SetPageSize(I2S_TypeDef* I2Sx, u32 I2S_PageSize);
_LONG_CALL_ void I2S_SetDirection(I2S_TypeDef* I2Sx, u32 I2S_TRxAct);
_LONG_CALL_ void I2S_INTClear(I2S_TypeDef* I2Sx, u32 I2STxIntrClr, u32 I2SRxIntrClr);
_LONG_CALL_ void I2S_INTClearAll(I2S_TypeDef* I2Sx);
_LONG_CALL_ void I2S_ISRGet(I2S_TypeDef* I2Sx,	u32* I2STxIsr, u32* I2SRxIsr);
_LONG_CALL_ void I2S_SetDMABuf(I2S_TypeDef* I2Sx, u8 *I2STxData, u8 *I2SRxData);
_LONG_CALL_ u32 I2S_GetTxPage(I2S_TypeDef* I2Sx);
_LONG_CALL_ void I2S_TxPageDMA_EN(I2S_TypeDef* I2Sx, u32 I2STxIdx);
_LONG_CALL_ void I2S_RxPageDMA_EN(I2S_TypeDef* I2Sx, u32 I2SRxIdx);
_LONG_CALL_ void I2S_TxDmaCmd(I2S_TypeDef* I2Sx, u32 NewState);
_LONG_CALL_ void I2S_RxDmaCmd(I2S_TypeDef* I2Sx, u32 NewState);
/**
  * @}
  */

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup I2S_Register_Definitions I2S Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup I2S_CONTROL
 * @{
 *****************************************************************************/
#define BIT_CTRL_CTLX_I2S_EN						((u32)0x00000001 << 0)
#define BIT_CTRL_CTLX_I2S_TX_ACT_MASK			((u32)0x00000003 << 1)
#define BIT_CTRL_CTLX_I2S_CHN_NUM_MASK			((u32)0x00000003 << 3)
#define BIT_CTRL_CTLX_I2S_WORD_LEN_MASK			((u32)0x00000001 << 6)
#define BIT_CTRL_CTLX_I2S_INTERNAL				((u32)0x00000001 << 7)
#define BIT_CTRL_CTLX_I2S_ENDIAN_SWAP			((u32)0x00000001 << 12)
#define BIT_CTRL_CTLX_I2S_CLK_SRC				((u32)0x00000001 << 30)
#define BIT_CTRL_CTLX_I2S_SW_RSTN				((u32)0x00000001 << 31)
/** @} */

/**************************************************************************//**
 * @defgroup I2S_SETTING
 * @{
 *****************************************************************************/
#define BIT_SETTING_I2S_PAGE_SIZE_MASK			((u32)0x00000FFF)
#define BIT_SETTING_I2S_PAGE_NUM_MASK			((u32)0x00000003 << 12)
#define BIT_SETTING_I2S_RATE_MASK				((u32)0x00000007 << 14)
/** @} */

/**************************************************************************//**
 * @defgroup I2S_TX_INT_MASK or STATUS
 * @{
 *****************************************************************************/
#define I2S_TX_INT_PAGE0_OK						((u32)0x00000001 << 0)
#define I2S_TX_INT_PAGE1_OK						((u32)0x00000001 << 1)
#define I2S_TX_INT_PAGE2_OK						((u32)0x00000001 << 2)
#define I2S_TX_INT_PAGE3_OK						((u32)0x00000001 << 3)
#define I2S_TX_INT_PAGE0_UNAVA					((u32)0x00000001 << 4)
#define I2S_TX_INT_PAGE1_UNAVA					((u32)0x00000001 << 5)
#define I2S_TX_INT_PAGE2_UNAVA					((u32)0x00000001 << 6)
#define I2S_TX_INT_PAGE3_UNAVA					((u32)0x00000001 << 7)
#define I2S_TX_INT_EMPTY							((u32)0x00000001 << 8)
/** @} */

/**************************************************************************//**
 * @defgroup I2S_RX_INT_MASK or STATUS
 * @{
 *****************************************************************************/
#define I2S_RX_INT_PAGE0_OK						((u32)0x00000001 << 0)
#define I2S_RX_INT_PAGE1_OK						((u32)0x00000001 << 1)
#define I2S_RX_INT_PAGE2_OK						((u32)0x00000001 << 2)
#define I2S_RX_INT_PAGE3_OK						((u32)0x00000001 << 3)
#define I2S_RX_INT_PAGE0_UNAVA					((u32)0x00000001 << 4)
#define I2S_RX_INT_PAGE1_UNAVA					((u32)0x00000001 << 5)
#define I2S_RX_INT_PAGE2_UNAVA					((u32)0x00000001 << 6)
#define I2S_RX_INT_PAGE3_UNAVA					((u32)0x00000001 << 7)
#define I2S_RX_INT_FULL							((u32)0x00000001 << 8)
/** @} */

/**************************************************************************//**
 * @defgroup I2S_TX_OWN or RX_PAGE_OWN
 * @{
 *****************************************************************************/
#define BIT_IS_PAGE_OWN							((u32)0x80000000)
/** @} */
/** @} */

/**
  * @}
  */

/**
  * @}
  */

#endif /* _RTL8710B_I2S_H_ */

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
