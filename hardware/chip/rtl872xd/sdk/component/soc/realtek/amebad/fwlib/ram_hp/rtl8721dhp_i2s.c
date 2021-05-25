/**
  ******************************************************************************
  * @file    rtl8721dhp_i2s.c
  * @author
  * @version V1.0.0
  * @date    2017-11-03
  * @brief   This file provides firmware functions to manage the following
  *          functionalities of the Inter-IC Sound (I2S) peripheral:
  *           - Initialization and Deinitialization
  *           - Interrupts management
  *           - Data transfers configuration
  *           - Pinmux clock configuration
  *
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */
#include "ameba_soc.h"

u32 i2s_page_num;			/* page num: 2~4 */
u32 i2s_cur_tx_page;			/* this is the dma page last time */
u32 i2s_cur_rx_page;			/*I2S RX page index */
u32 i2s_txpage_entry[4];	/* The Tx DAM buffer: pointer of each page */
u32 i2s_rxpage_entry[4];	/* The Rx DAM buffer: pointer of each page */
	
/**
  * @brief  Fills each I2S_InitStruct member with its default value.
  * @param  I2S_InitStruct: pointer to an I2S_InitTypeDef structure which will be
  *         initialized.
  * @note   I2S_InterLoopback parameter is only used for loopback test.
  * @retval None
  */
void
I2S_StructInit(I2S_InitTypeDef* I2S_InitStruct)
{
	I2S_InitStruct->I2S_SlaveMode = I2S_MASTER_MODE;
	I2S_InitStruct->I2S_WordLen = I2S_WL_16;
	I2S_InitStruct->I2S_Justify = I2S_JY_I2S;
	I2S_InitStruct->I2S_EndianSwap = I2S_ES_BIG;
	I2S_InitStruct->I2S_ChNum = I2S_CH_STEREO;
	I2S_InitStruct->I2S_PageNum = 4;
	I2S_InitStruct->I2S_PageSize = (768/4);
	I2S_InitStruct->I2S_Rate = I2S_SR_48KHZ;
	I2S_InitStruct->I2S_TRxAct = I2S_TXRX;
	I2S_InitStruct->I2S_InterLoopback = 0;
	I2S_InitStruct->I2S_Mute = I2S_UNMUTE;
	I2S_InitStruct->I2S_BurstSize = I2S_BURST_16;
	I2S_InitStruct->I2S_SckSwap = I2S_SCK_NOINV;
	I2S_InitStruct->I2S_WsSwap = I2S_WS_LEFT_PHA;
	I2S_InitStruct->I2S_EdgeSwap = I2S_NEGATIVE_EDGE;
}

/**
  * @brief  Initializes the I2S registers according to the specified parameters
  *         in I2S_InitStruct.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @param  I2S_InitStruct: pointer to an I2S_InitTypeDef structure that contains
  *         the configuration information for the specified I2S peripheral
  * @note   I2S has two clock sources, one is 98.304MHz, the other is 45.1584MHz.
  *         BIT_CTRL_CTLX_I2S_CLK_SRC can used to select the clock source.
  * @retval None
  */
void
I2S_Init(
    I2S_TypeDef* I2Sx, I2S_InitTypeDef* I2S_InitStruct
)
{
	u32 Tmp;

	/* Check the parameters */
	assert_param(IS_I2S_WORD_LEN(I2S_InitStruct->I2S_WordLen));
	assert_param(IS_I2S_JUSTIFY(I2S_InitStruct->I2S_Justify));
	assert_param(IS_I2S_ENDIAN_SWAP(I2S_InitStruct->I2S_EndianSwap));
	assert_param(IS_I2S_CHN_NUM(I2S_InitStruct->I2S_ChNum));
	assert_param(IS_I2S_MODE(I2S_InitStruct->I2S_SlaveMode));
	assert_param(IS_I2S_DIR(I2S_InitStruct->I2S_TRxAct));
	assert_param(IS_I2S_SAMPLE_RATE(I2S_InitStruct->I2S_Rate));
	assert_param(IS_I2S_MUTE(I2S_InitStruct->I2S_Mute));
	assert_param(IS_I2S_BST_NUM(I2S_InitStruct->I2S_BurstSize));
	assert_param(IS_I2S_SCK_SWAP(I2S_InitStruct->I2S_SckSwap));
	assert_param(IS_I2S_WS_SWAP(I2S_InitStruct->I2S_WsSwap));
	assert_param(IS_I2S_EDGE_SWAP(I2S_InitStruct->I2S_EdgeSwap));
	assert_param((I2S_InitStruct->I2S_PageNum >= 2) && (I2S_InitStruct->I2S_PageNum <= 4));

	/* Disable the I2S first, and reset to default */
	I2Sx->IS_CTL = BIT_CTRL_CTLX_I2S_SW_RSTN;
	I2Sx->IS_CTL &= ~(BIT_CTRL_CTLX_I2S_SW_RSTN);
	I2Sx->IS_CTL = BIT_CTRL_CTLX_I2S_SW_RSTN;

	Tmp = I2Sx->IS_CTL;

	Tmp |= ((I2S_InitStruct->I2S_WordLen << 29) |
		(I2S_InitStruct->I2S_ChNum << 3) |
		(I2S_InitStruct->I2S_SlaveMode << 28) |
		(I2S_InitStruct->I2S_EndianSwap << 12) |
		(I2S_InitStruct->I2S_InterLoopback << 7) |
		(I2S_InitStruct->I2S_TRxAct << 1) |
		(I2S_InitStruct->I2S_Mute << 27) |
		(I2S_InitStruct->I2S_BurstSize << 18) |
		(I2S_InitStruct->I2S_SckSwap << 11) |
		(I2S_InitStruct->I2S_WsSwap << 10) |
		(I2S_InitStruct->I2S_EdgeSwap << 5) |
		(I2S_InitStruct->I2S_Justify << 8));
	/* set mute, burst size, word length, channel number, master or slave, trx act */
	I2Sx->IS_CTL = Tmp;

	Tmp = (I2S_InitStruct->I2S_PageSize - 1) |
		((I2S_InitStruct->I2S_PageNum - 1) << 12) |
		(I2S_InitStruct->I2S_Rate << 14);
	
	if (I2S_InitStruct->I2S_Rate & 0x10)
	{
		Tmp |= BIT_CTRL_CTLX_I2S_CLK_SRC;
	}
	/* set clock source, page size, page number, sample rate */
	I2Sx->IS_SETTING = Tmp;

	/* I2S Clear all interrupts first */
	I2S_INTClearAll(I2Sx);

	/* clear all dma own bit first */
	I2S_TxDmaCmd(I2Sx, DISABLE);
	I2S_RxDmaCmd(I2Sx, DISABLE);

	/* I2S Disable all interrupts first */
	I2S_INTConfig(I2Sx, 0, 0);

	/* init current page */
	i2s_cur_tx_page = 0;
	i2s_cur_rx_page = 0;
	i2s_page_num = I2S_InitStruct->I2S_PageNum;
}

/**
  * @brief  Enables or disables the specified I2S peripheral.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @param  NewState: new state of the I2Sx peripheral.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void
I2S_Cmd(
	I2S_TypeDef* I2Sx,
	u8  NewState
)
{
	u32 Tmp;

	Tmp = I2Sx->IS_CTL;

	if (NewState == ENABLE)
		Tmp |= (BIT_CTRL_CTLX_I2S_EN | BIT_CTRL_CTLX_I2S_SW_RSTN);
	else
		Tmp &= ~(BIT_CTRL_CTLX_I2S_EN | BIT_CTRL_CTLX_I2S_SW_RSTN);

	I2Sx->IS_CTL = Tmp;
}

/**
  * @brief  Start or stop Tx DMA. When stop Tx DMA, clear DMA OWM bit.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @param  NewState: new state of the Tx DMA.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void
I2S_TxDmaCmd(I2S_TypeDef* I2Sx, u32 NewState)
{
	u32 i;
	u32 own = 0x00000000;

	/* TX owned by CPU, We should set it to I2S when we I2S_TxPageDMA_EN */
	if (NewState == DISABLE) {
		own &= ~BIT_IS_PAGE_OWN;
	} else {
		own = BIT_IS_PAGE_OWN;
	}

	for (i = 0; i < 4; i++) {
		I2Sx->IS_TX_PAGE_OWN[i] = own;
	}
}

/**
  * @brief  Start or stop Rx DMA. When stop RX DMA, clear DMA OWM bit.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @param  NewState: new state of the Rx DMA.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void
I2S_RxDmaCmd(I2S_TypeDef* I2Sx, u32 NewState)
{
	u32 i;
	u32 own = 0x00000000;

	/* RX owned by CPU, We should set it to I2S when we need recv */
	if (NewState == DISABLE) {
		own &= ~BIT_IS_PAGE_OWN;
	} else {
		own = BIT_IS_PAGE_OWN;
	}

	for (i = 0; i < 4; i++) {
		I2Sx->IS_RX_PAGE_OWN[i] = own;
	}
}

/**
  * @brief  Enables or disables the specified I2S interrupts.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @param  I2STxIntrMSK: specifies the I2S Tx interrupt to be enabled or disabled.
  *          This parameter can be one or combinations of the following values:
  *            @arg I2S_TX_INT_PAGE0_OK:  Tx page0 OK interrupt
  *            @arg I2S_TX_INT_PAGE1_OK:  Tx page1 OK interrupt
  *            @arg I2S_TX_INT_PAGE2_OK:  Tx page2 OK interrupt
  *            @arg I2S_TX_INT_PAGE3_OK:  Tx page3 OK interrupt
  *            @arg I2S_TX_INT_PAGE0_UNAVA: Tx page0 unavailable interrupt
  *            @arg I2S_TX_INT_PAGE1_UNAVA: Tx page1 unavailable interrupt
  *            @arg I2S_TX_INT_PAGE2_UNAVA: Tx page2 unavailable interrupt
  *            @arg I2S_TX_INT_PAGE3_UNAVA: Tx page3 unavailable interrupt
  *            @arg I2S_TX_INT_EMPTY:  Tx FIFO empty interrupt
  * @param  I2SRxIntrMSK: specifies the I2S Rx interrupt to be enabled or disabled.
  *          This parameter can be one or combinations of the following values:
  *            @arg I2S_RX_INT_PAGE0_OK:  Rx page0 OK interrupt
  *            @arg I2S_RX_INT_PAGE1_OK:  Rx page1 OK interrupt
  *            @arg I2S_RX_INT_PAGE2_OK:  Rx page2 OK interrupt
  *            @arg I2S_RX_INT_PAGE3_OK:  Rx page3 OK interrupt
  *            @arg I2S_RX_INT_PAGE0_UNAVA: Rx page0 unavailable interrupt
  *            @arg I2S_RX_INT_PAGE1_UNAVA: Rx page1 unavailable interrupt
  *            @arg I2S_RX_INT_PAGE2_UNAVA: Rx page2 unavailable interrupt
  *            @arg I2S_RX_INT_PAGE3_UNAVA: Rx page3 unavailable interrupt
  *            @arg I2S_RX_INT_FULL:  Rx FIFO full interrupt
  * @retval None
  */
void
I2S_INTConfig(
	I2S_TypeDef* I2Sx,
	u32 I2STxIntrMSK,
	u32 I2SRxIntrMSK
)
{
	I2Sx->IS_TX_MASK_INT = I2STxIntrMSK;
	I2Sx->IS_RX_MASK_INT = I2SRxIntrMSK;
}

/**
  * @brief  Clears the specified I2S interrupt pending bit.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @param  I2STxIntrClr: specifies the I2S Tx interrupt pending bit to clear.
  * @param  I2SRxIntrClr: specifies the I2S Rx interrupt pending bit to clear.
  * @retval None
  */
void
I2S_INTClear(
	I2S_TypeDef* I2Sx,
	u32 I2STxIntrClr,
	u32 I2SRxIntrClr
)
{
	I2Sx->IS_TX_STATUS_INT = I2STxIntrClr;
	I2Sx->IS_RX_STATUS_INT = I2SRxIntrClr;
}

/**
  * @brief  Clears all of the I2S interrupt pending bit.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @retval None
  */
void
I2S_INTClearAll(
	I2S_TypeDef* I2Sx
)
{
	I2Sx->IS_TX_STATUS_INT = 0x1ff;
	I2Sx->IS_RX_STATUS_INT = 0x1ff;
}

/**
  * @brief  Get all of the I2S interrupt status.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @param  I2STxIsr: pointer to the I2S Tx interrupt status.
  * @param  I2SRxIsr: pointer to the I2S Rx interrupt status.
  * @retval None
  */
void
I2S_ISRGet(
	I2S_TypeDef* I2Sx,
	u32* I2STxIsr,
	u32* I2SRxIsr
)
{
	*I2STxIsr = I2Sx->IS_TX_STATUS_INT;
	*I2SRxIsr = I2Sx->IS_RX_STATUS_INT;
}

/**
  * @brief  Set the I2S sample rate.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @param  I2SRate: the value of sample rate.
  *          This parameter can be one of the following values:
  *            @arg I2S_SR_8KHZ: sample rate is 8kHz
  *            @arg I2S_SR_12KHZ: sample rate is 12kHz
  *            @arg I2S_SR_16KHZ: sample rate is 16kHz
  *            @arg I2S_SR_24KHZ: sample rate is 24kHz
  *            @arg I2S_SR_32KHZ: sample rate is 32kHz
  *            @arg I2S_SR_48KHZ: sample rate is 48kHz
  *            @arg I2S_SR_64KHZ: sample rate is 64kHz
  *            @arg I2S_SR_96KHZ: sample rate is 96kHz
  *            @arg I2S_SR_192KHZ: sample rate is 192kHz
  *            @arg I2S_SR_384KHZ: sample rate is 384kHz
  *            @arg I2S_SR_7p35KHZ: sample rate is 7.35kHz
  *            @arg I2S_SR_11p025KHZ: sample rate is 11.025kHz
  *            @arg I2S_SR_14p7KHZ: sample rate is 14.7kHz
  *            @arg I2S_SR_22p05KHZ: sample rate is 22.05kHz
  *            @arg I2S_SR_29p4KHZ: sample rate is 29.4kHz
  *            @arg I2S_SR_44p1KHZ: sample rate is 44.1kHz
  *            @arg I2S_SR_58p8KHZ: sample rate is 58.8kHz
  *            @arg I2S_SR_88p2KHZ: sample rate is 88.2kHz
  *            @arg I2S_SR_176p4KHZ: sample rate is 176.4kHz
  * @retval None
  */
void
I2S_SetRate(
    I2S_TypeDef* I2Sx, u32 I2S_Rate
)
{
	u32 reg_value;
	assert_param(IS_I2S_SAMPLE_RATE(I2S_Rate));

	reg_value = I2Sx->IS_SETTING;
	reg_value &= ~(BIT_SETTING_I2S_RATE_MASK | BIT_CTRL_CTLX_I2S_CLK_SRC);
	if (I2S_Rate & 0x10)
	{
		reg_value |= BIT_CTRL_CTLX_I2S_CLK_SRC;
	}
	reg_value |= ((I2S_Rate << 14) & BIT_SETTING_I2S_RATE_MASK);
	I2Sx->IS_SETTING = reg_value;
}

/**
  * @brief  Get I2S version ID .
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @retval The value of version ID.
  */
u32
I2S_GetVersion(
	I2S_TypeDef* I2Sx
)
{
	return I2Sx->IS_VERSION_ID;
}

/**
  * @brief  Set the I2S mute function.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @param  NewState: enable or disable mute function.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void
I2S_SetMute(
    I2S_TypeDef* I2Sx, u32 NewState
)
{
	u32 Tmp;

	Tmp = I2Sx->IS_CTL;

	if (NewState == ENABLE)
		Tmp |= BIT_CTRL_CTLX_I2S_MUTE;
	else
		Tmp &= ~BIT_CTRL_CTLX_I2S_MUTE;

	I2Sx->IS_CTL = Tmp;
}

/**
  * @brief  Set the I2S burst size.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @param  I2S_BurstSize: the value of burst size.
  *          This parameter can be one of the following values:
  *            @arg I2S_BURST_4: sample bit is 16 byte
  *            @arg I2S_BURST_8: sample bit is 32 byte
  *            @arg I2S_BURST_12: sample bit is 48 byte
  *            @arg I2S_BURST_16: burst size is 64 byte
  * @retval None
  */
void
I2S_SetBurstSize(
    I2S_TypeDef* I2Sx, u32 I2S_BurstSize
)
{
	u32 reg_value;
	assert_param(IS_I2S_BST_NUM(I2S_BurstSize));

	reg_value = I2Sx->IS_CTL;
	reg_value &= ~(BIT_CTRL_CTLX_I2S_BURST_SIZE_MASK);
	reg_value |= (I2S_BurstSize << 18);
	I2Sx->IS_CTL = reg_value;
}

/**
  * @brief  Set the I2S word length.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @param  I2SWordLen: the value of word length.
  *          This parameter can be one of the following values:
  *            @arg I2S_WL_16: sample bit is 16 bit
  *            @arg I2S_WL_24: sample bit is 24 bit
  *            @arg I2S_WL_32: sample bit is 32 bit
  * @retval None
  */
void
I2S_SetWordLen(
    I2S_TypeDef* I2Sx, u32 I2S_WordLen
)
{
	u32 reg_value;
	assert_param(IS_I2S_WORD_LEN(I2S_WordLen));

	reg_value = I2Sx->IS_CTL;
	reg_value &= ~(BIT_CTRL_CTLX_I2S_WORD_LEN_MASK);
	reg_value |= (I2S_WordLen << 29);
	I2Sx->IS_CTL = reg_value;
}

/**
  * @brief  Set the I2S channel number.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @param  I2SChNum: the value of channel number.
  *          This parameter can be one of the following values:
  *            @arg I2S_CH_STEREO: stereo channel, channel number is 2
  *            @arg I2S_CH_MONO: mono channel, channel number is 1
  *            @arg I2S_CH_5p1: 5.1 channel, channel number is 3
  * @retval None
  */
void
I2S_SetChNum(
    I2S_TypeDef* I2Sx, u32 I2S_ChNum
)
{
    u32 reg_value;
	assert_param(IS_I2S_CHN_NUM(I2S_ChNum));

    reg_value = I2Sx->IS_CTL;
    reg_value &= ~(BIT_CTRL_CTLX_I2S_CHN_NUM_MASK);
    reg_value |= (I2S_ChNum << 3);
    I2Sx->IS_CTL = reg_value;
}

/**
  * @brief  Set the I2S page number.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @param  I2SPageNum: the value of page number.
  *          This parameter can be one of the following values:
  *            @arg 2: two pages
  *            @arg 3: three pages
  *            @arg 4: four pages
  * @retval None
  */
void
I2S_SetPageNum(
	I2S_TypeDef* I2Sx, u32 I2S_PageNum
)
{
	u32 reg_value;
	assert_param((I2S_PageNum >= 2) && (I2S_PageNum <= 4));

	reg_value = I2Sx->IS_SETTING;
	reg_value &= ~(BIT_SETTING_I2S_PAGE_NUM_MASK);
	reg_value |= ((I2S_PageNum - 1) << 12);
	I2Sx->IS_SETTING = reg_value;

	i2s_page_num = I2S_PageNum;
}

/**
  * @brief  Set the I2S page size.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @param  I2SPageSize: the value of page size.
  * 		 This parameter must be set to a value in the 1~4096 Word range
  * @note   I2S page size in unit of word
  * @retval None
  */
void
I2S_SetPageSize(
    I2S_TypeDef* I2Sx, u32 I2S_PageSize
)
{
	u32 reg_value;

	reg_value = I2Sx->IS_SETTING;
	reg_value &= ~(BIT_SETTING_I2S_PAGE_SIZE_MASK);
	reg_value |= (I2S_PageSize - 1);
	I2Sx->IS_SETTING = reg_value;
}

/**
  * @brief  Get the I2S page size.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @note   I2S page size in unit of word
  * @retval the value of page size,  1~4096
  */
u32
I2S_GetPageSize(
    I2S_TypeDef* I2Sx
)
{
	u32 reg_value;

	reg_value = I2Sx->IS_SETTING;
	reg_value &= (BIT_SETTING_I2S_PAGE_SIZE_MASK);
	reg_value += 1;

	return reg_value;
}

/**
  * @brief  Set the I2S data transfer direction.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @param  I2STRxAct: specifies the data transfer direction.
  *          This parameter can be one of the following values:
  *            @arg I2S_ONLY_RX: Selects Rx receive direction
  *            @arg I2S_ONLY_TX: Selects Tx transmission direction
  *            @arg I2S_TXRX: Selects Tx & Rx bi-direction
  * @retval None
  */
void
I2S_SetDirection(
    I2S_TypeDef* I2Sx, u32 I2S_TRxAct
)
{
	u32 reg_value;
	assert_param(IS_I2S_DIR(I2S_TRxAct));

	reg_value = I2Sx->IS_CTL;
	reg_value &= ~(BIT_CTRL_CTLX_I2S_TX_ACT_MASK);
	reg_value |= (I2S_TRxAct << 1);
	I2Sx->IS_CTL = reg_value;
}

/**
  * @brief  Set tx&rx page start address and initialize page own.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @param  I2STxData: pointer to the start address of Tx page.
  * @param  I2SRxData: pointer to the start address of Rx page.
  * @retval None
  */
void
I2S_SetDMABuf(
	I2S_TypeDef* I2Sx,
	u8 *I2STxData,
	u8 *I2SRxData

)
{
	if (I2STxData) {
		I2Sx->IS_TX_PAGE_PTR = (u32)I2STxData;

		/* TX owned by CPU, We should set it to I2S when we I2S_TxPageDMA_EN */
		I2S_TxDmaCmd(I2Sx, DISABLE);
	}

	if (I2SRxData) {
		I2Sx->IS_RX_PAGE_PTR = (u32)I2SRxData;

		/* RX owned by i2s, we should set it to CPU when we recv Rx interrupt */
		I2S_RxDmaCmd(I2Sx, ENABLE);
	}
}

/**
  * @brief  Check tx page is own by CPU or I2S.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @param  page_index: specifies page index.
  * @retval The state of page own
  *          - 0: tx page is own by CPU
  *          - 1: tx page is own by I2S
  */
u32
I2S_TxPageBusy(
	I2S_TypeDef* I2Sx,
	u32 page_index
)
{
	u32 reg;

	reg = I2Sx->IS_TX_PAGE_OWN[page_index];
	if ((reg & (1<<31)) == 0) { //zero is by CPU
		return 0;
	} else {
		return 1;
	}
}

/**
  * @brief  Get current tx page .
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @retval The value of current tx page
  */
u32
I2S_GetTxPage(
	I2S_TypeDef* I2Sx
)
{
	/* To avoid gcc warnings */
	( void ) I2Sx;
	return i2s_cur_tx_page;
}

/**
  * @brief  Get current rx page .
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @retval The value of current rx page
  */
u32
I2S_GetRxPage(
	I2S_TypeDef* I2Sx
)
{
	/* To avoid gcc warnings */
	( void ) I2Sx;
	return i2s_cur_rx_page;
}

/**
  * @brief  Set the address of specified tx page.
  * @param  page_index: specifies tx page index.
  * @param  page_address: the address set to tx page.
  * @retval None
  */
void
I2S_SetTxPageAddr(
	u32 page_index,
	u32 page_address
)
{
	i2s_txpage_entry[page_index] = page_address;
}

/**
  * @brief  Get the address of specified tx page.
  * @param  page_index: specifies tx page index.
  * @retval Address of the specified tx page
  */
u32
I2S_GetTxPageAddr(
	u32 page_index
)
{
	return i2s_txpage_entry[page_index];
}

/**
  * @brief  Set the address of specified rx page.
  * @param  page_index: specifies rx page index.
  * @param  page_address: the address set to rx page.
  * @retval None
  */
void
I2S_SetRxPageAddr(
	u32 page_index,
	u32 page_address
)
{
	i2s_rxpage_entry[page_index] = page_address;
}

/**
  * @brief  Get the address of specified rx page.
  * @param  page_index: specifies rx page index.
  * @retval Address of the specified rx page
  */
u32
I2S_GetRxPageAddr(
	u32 page_index
)
{
	return i2s_rxpage_entry[page_index];
}

/**
  * @brief  set i2s_cur_tx_page own by i2s.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @param  I2STxIdx: specifies tx current page index.
  * @retval None
  * @note i2s_cur_tx_page index is managed by this function
  * @note i2s_cur_tx_page++ in this function
  * @note you should check txpage busy use I2S_TxPageBusy before this function
  */
void
I2S_TxPageDMA_EN(
	I2S_TypeDef* I2Sx,
	u32 I2STxIdx
)
{
	u32 I2STxIdxTmp = I2STxIdx;

	I2Sx->IS_TX_PAGE_OWN[I2STxIdxTmp] = (1<<31);

	I2STxIdxTmp++;
	if (I2STxIdxTmp == i2s_page_num) {
		I2STxIdxTmp = 0;
	}
	
	i2s_cur_tx_page = I2STxIdxTmp;
}

/**
  * @brief  set i2s_cur_rx_page own by i2s.
  * @param  I2Sx: pointer to the base addr of I2S peripheral.
  * @param  I2SRxIdx: specifies rx current page index.
  * @retval None
  * @note i2s_cur_rx_page index is managed by this function
  * @note i2s_cur_rx_page++ in this function
  */
void
I2S_RxPageDMA_EN(
	I2S_TypeDef* I2Sx,
	u32 I2SRxIdx
)
{
	u32 I2SRxIdxTmp = I2SRxIdx;
	u32 reg;

	reg = I2Sx->IS_RX_PAGE_OWN[I2SRxIdxTmp];
	if ((reg & (1<<31)) != 0) {
		DBG_8195A("I2S_RxPageDMA_EN: No Idle Rx Page\r\n");
		return;
	}

	I2Sx->IS_RX_PAGE_OWN[I2SRxIdxTmp] = (1 <<31);

	I2SRxIdxTmp++;
	if (I2SRxIdxTmp == i2s_page_num) {
		I2SRxIdxTmp = 0;
	}	
	
	i2s_cur_rx_page = I2SRxIdxTmp;
}
/******************* (C) COPYRIGHT 2017 Realtek Semiconductor *****END OF FILE****/
