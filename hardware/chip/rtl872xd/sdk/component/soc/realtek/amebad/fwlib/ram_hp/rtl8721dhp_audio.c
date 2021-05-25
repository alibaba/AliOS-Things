/**
  ******************************************************************************
  * @file    rtl8721dhp_audio.c
  * @author
  * @version V1.0.0
  * @date    2017-12-13
  * @brief   This file provides firmware functions to manage the following
  *          functionalities of the Audio codec peripheral:
  *           - SPORT Initialization
  *           - SPORT parameters management
  *           - Data transfers configuration
  *           - GDMA configuration
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

const AUDIO_DevTable AUDIO_DEV_TABLE[1] = {
		{AUDIO_SPORT_DEV, GDMA_HANDSHAKE_INTERFACE_AUDIO_TX, GDMA_HANDSHAKE_INTERFACE_AUDIO_RX},   /*audio sport */
};

/**
  * @brief  Fills each SP_StructInit member with its default value.
  * @param  SP_StructInit: pointer to an SP_StructInit structure which will be
  *         initialized.
  * @retval None
  */
void AUDIO_SP_StructInit(SP_InitTypeDef* SP_InitStruct)
{
	SP_InitStruct->SP_WordLen = SP_WL_16;
	SP_InitStruct->SP_DataFormat = SP_DF_I2S;
	SP_InitStruct->SP_MonoStereo = SP_CH_STEREO;
	SP_InitStruct->SP_SelRxCh = SP_RX_CH_LR;
}

/**
  * @brief  Initializes the AUDIO SPORT registers according to the specified parameters
  *         in SP_InitStruct.
  * @param  SPORTx: pointer to the base addr of AUDIO SPORT peripheral.
  * @param  SP_InitStruct: pointer to an SP_InitTypeDef structure that contains
  *         the configuration information for the specified AUDIO SPORT peripheral
  * @note   AUDIO SPORT has two clock sources, one is 98.304MHz, the other is 45.1584MHz.
  *         BIT_CTRL_CTLX_I2S_CLK_SRC can used to select the clock source.
  * @retval None
  */
void AUDIO_SP_Init(AUDIO_SPORT_TypeDef* SPORTx, SP_InitTypeDef* SP_InitStruct)
{
	u32 Tmp;

	/* Check the parameters */
	assert_param(IS_SP_WORD_LEN(SP_InitStruct->SP_WordLen));
	assert_param(IS_SP_DATA_FMT(SP_InitStruct->SP_DataFormat));
	assert_param(IS_SP_CHN_NUM(SP_InitStruct->SP_MonoStereo));
	assert_param(IS_SP_SEL_RX_CH(SP_InitStruct->SP_SelRxCh));

	/* Reset SPORT module */
	SPORTx->SP_CTRLR0 |= SP_CTRLR0_RST;
	SPORTx->SP_CTRLR0 &= ~ SP_CTRLR0_RST;

	/* Configure parameters: BCLK = 16*16K */
	SPORTx->SP_CLK_DIV = 0x271 | (0x10 << 16) | SP_CLK_MI_NI_UPDATE;
	
	/* Configure parameters: disable RX, disable TX, AUDIO SPORT mode */
	AUDIO_SP_TxStart(SPORTx, DISABLE);
	AUDIO_SP_RxStart(SPORTx, DISABLE);

	/* Configure parameters: data format, word length, channel number, etc */	
	Tmp = SPORTx->SP_CTRLR0;
	Tmp &= ~(SP_CTRLR0_DATA_LEN_SEL |SP_CTRLR0_DATA_FORMAT_SEL | SP_CTRLR0_EN_I2S_MONO | SP_CTRLR0_SEL_I2S_RX_CH);
	Tmp |= ((SP_InitStruct->SP_WordLen << 12) |
		(SP_InitStruct->SP_DataFormat << 8) |
		(SP_InitStruct->SP_MonoStereo << 11) |
		(SP_InitStruct->SP_SelRxCh << 28));
	SPORTx->SP_CTRLR0 = Tmp;
}

/**
  * @brief  Start or stop SPORT Tx.
  * @param  SPORTx: pointer to the base addr of AUDIO SPORT peripheral.
  * @param  NewState: new state of the SPORT Tx.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void AUDIO_SP_TxStart(AUDIO_SPORT_TypeDef* SPORTx, u32 NewState)
{
	if (NewState == ENABLE) {
		SPORTx->SP_CTRLR0 &= ~ SP_CTRLR0_TX_DISABLE;
		SPORTx->SP_CTRLR0 |= SP_CTRLR0_START_TX;
	} else {
		SPORTx->SP_CTRLR0 |= SP_CTRLR0_TX_DISABLE;
		SPORTx->SP_CTRLR0 &= ~ SP_CTRLR0_START_TX;
	}
}

/**
  * @brief  Start or stop SPORT Rx.
  * @param  SPORTx: pointer to the base addr of AUDIO SPORT peripheral.
  * @param  NewState: new state of the SPORT Rx.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void AUDIO_SP_RxStart(AUDIO_SPORT_TypeDef* SPORTx, u32 NewState)
{
	if (NewState == ENABLE) {
		SPORTx->SP_CTRLR0 &= ~ SP_CTRLR0_RX_DISABLE;
		SPORTx->SP_CTRLR0 |= SP_CTRLR0_START_RX;
	} else {
		SPORTx->SP_CTRLR0 |= SP_CTRLR0_RX_DISABLE;
		SPORTx->SP_CTRLR0 &= ~ SP_CTRLR0_START_RX;
	}
}

/**
  * @brief  SPORT Tx DMA request on or off.
  * @param  SPORTx: pointer to the base addr of AUDIO SPORT peripheral.
  * @param  NewState: new state of the SPORT Tx DMA request.
  *         This parameter can be: ENABLE or DISABLE.
  * @note If Tx DMA request is not enable, then should start Tx when GDMA complete every time. 
  * @retval None
  */
void AUDIO_SP_TdmaCmd(AUDIO_SPORT_TypeDef* SPORTx, u32 NewState)
{
	if (NewState == ENABLE) {
		SPORTx->SP_CTRLR1 |= SP_CTRLR1_TDMA_REQ;
		SPORTx->SP_DSP_INT_CR &= ~ SP_TX_DMA_SINGLE_NO_REQ;
	} else {
		SPORTx->SP_CTRLR1 &= ~ SP_CTRLR1_TDMA_REQ;
		SPORTx->SP_DSP_INT_CR |= SP_TX_DMA_SINGLE_NO_REQ;
	}
}

/**
  * @brief  SPORT Rx DMA request on or off.
  * @param  SPORTx: pointer to the base addr of AUDIO SPORT peripheral.
  * @param  NewState: new state of the SPORT Rx DMA request.
  *         This parameter can be: ENABLE or DISABLE.
  * @note If Rx DMA request is not enable, then should start Rx when GDMA complete every time. 
  * @retval None
  */
void AUDIO_SP_RdmaCmd(AUDIO_SPORT_TypeDef* SPORTx, u32 NewState)
{
	if (NewState == ENABLE) {
		SPORTx->SP_CTRLR1 |= SP_CTRLR1_RDMA_REQ;
		SPORTx->SP_DSP_INT_CR &= ~ SP_RX_DMA_SINGLE_NO_REQ;
	} else {
		SPORTx->SP_CTRLR1 &= ~ SP_CTRLR1_RDMA_REQ;
		SPORTx->SP_DSP_INT_CR |= SP_RX_DMA_SINGLE_NO_REQ;
	}
}

/**
  * @brief  Set the AUDIO SPORT word length.
  * @param  SPORTx: pointer to the base addr of AUDIO SPORT peripheral.
  * @param  SP_WordLen: the value of word length.
  *          This parameter can be one of the following values:
  *            @arg SP_WL_16: sample bit is 16 bit
  *            @arg SP_WL_24: sample bit is 24 bit
  *            @arg SP_WL_8: sample bit is 8 bit
  * @retval None
  */
void AUDIO_SP_SetWordLen(AUDIO_SPORT_TypeDef* SPORTx, u32 SP_WordLen)
{
	u32 reg_value;
	assert_param(IS_SP_WORD_LEN(SP_WordLen));

	reg_value = SPORTx->SP_CTRLR0;
	reg_value &= ~(SP_CTRLR0_DATA_LEN_SEL);
	reg_value |= (SP_WordLen << 12);
	SPORTx->SP_CTRLR0 = reg_value;
}

/**
  * @brief Get AUDIO SPORT word length. 
  * @param  SPORTx: pointer to the base addr of AUDIO SPORT peripheral.
  * @retval the value of word length.
  *            @arg 0: sample bit is 16 bit
  *            @arg 2: sample bit is 24 bit
  *            @arg 3: sample bit is 8 bit
  */

u32 AUDIO_SP_GetWordLen(AUDIO_SPORT_TypeDef* SPORTx)
{
	u32 len = ((SPORTx->SP_CTRLR0) & SP_CTRLR0_DATA_LEN_SEL) >> 12;
	return len;
}

/**
  * @brief  Set the AUDIO SPORT channel number.
  * @param  SPORTx: pointer to the base addr of AUDIO SPORT peripheral.
  * @param  SP_MonoStereo: mono or stereo.
  *          This parameter can be one of the following values:
  *            @arg SP_CH_STEREO: stereo channel, channel number is 2
  *            @arg SP_CH_MONO: mono channel, channel number is 1
  * @retval None
  */
void AUDIO_SP_SetMonoStereo(AUDIO_SPORT_TypeDef* SPORTx, u32 SP_MonoStereo)
{
	u32 reg_value;
	assert_param(IS_SP_CHN_NUM(SP_MonoStereo));

	reg_value = SPORTx->SP_CTRLR0;
	reg_value &= ~(SP_CTRLR0_EN_I2S_MONO);
	reg_value |= (SP_MonoStereo << 11);
	SPORTx->SP_CTRLR0 = reg_value;
}

/**
  * @brief    Initialize GDMA peripheral for sending data.
  * @param  Index: 0.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains 
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pTxData: Tx Buffer.
  * @param  Length: Tx Count.
  * @retval   TRUE/FLASE
  */
BOOL AUDIO_SP_TXGDMA_Init(
	u32 Index,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pTxData,
	u32 Length
	)
{
	u8 GdmaChnl;
	AUDIO_SPORT_TypeDef *SPORTx = AUDIO_DEV_TABLE[Index].SPORTx;
	u32 WordLen = AUDIO_SP_GetWordLen(SPORTx);

	assert_param(GDMA_InitStruct != NULL);
	/*obtain a DMA channel and register DMA interrupt handler*/
	GdmaChnl = GDMA_ChnlAlloc(0, CallbackFunc, (u32)CallbackData, 12);
	if (GdmaChnl == 0xFF) {
		// No Available DMA channel
		return _FALSE;
	}

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	/*set GDMA initial structure member value*/
	GDMA_InitStruct->MuliBlockCunt = 0;
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->MaxMuliBlock = 1;
	GDMA_InitStruct->GDMA_DIR = TTFCMemToPeri;
	GDMA_InitStruct->GDMA_DstHandshakeInterface = AUDIO_DEV_TABLE[Index].Tx_HandshakeInterface;
	GDMA_InitStruct->GDMA_DstAddr = (u32)&AUDIO_DEV_TABLE[Index].SPORTx->SP_TX_DR;
	GDMA_InitStruct->GDMA_Index = 0;
	GDMA_InitStruct->GDMA_ChNum = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType|TransferType|ErrType);
	GDMA_InitStruct->GDMA_DstInc = NoChange;
	GDMA_InitStruct->GDMA_SrcInc = IncType;

	/*  Cofigure GDMA transfer */
	if (WordLen == SP_WL_8) {
		/*  8bits mode */
		if (((Length & 0x03)==0) && (((u32)(pTxData) & 0x03)==0)) {
			/*  4-bytes aligned, move 4 bytes each transfer */
			GDMA_InitStruct->GDMA_SrcMsize   = MsizeOne;
			GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
			GDMA_InitStruct->GDMA_BlockSize = Length >> 2;
		} else {
			GDMA_InitStruct->GDMA_SrcMsize   = MsizeFour;
			GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;
			GDMA_InitStruct->GDMA_BlockSize = Length;
		}
		GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
	} else  {
		/*  24bits or 16bits mode */
		if (((Length & 0x03)==0) && (((u32)(pTxData) & 0x03)==0)) {
			/*  4-bytes aligned, move 4 bytes each transfer */
			GDMA_InitStruct->GDMA_SrcMsize   = MsizeFour;
			GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
			GDMA_InitStruct->GDMA_BlockSize = Length >> 2;
		} else if (((Length & 0x01)==0) && (((u32)(pTxData) & 0x01)==0)) {
			/*  2-bytes aligned, move 2 bytes each transfer */
			GDMA_InitStruct->GDMA_SrcMsize   = MsizeEight;
			GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthTwoBytes;
			GDMA_InitStruct->GDMA_BlockSize = Length >> 1;
		} else {
			DBG_8195A("AUDIO_SP_TXGDMA_Init: Aligment Err: pTxData=0x%x,  Length=%d\n", pTxData, Length);
			return _FALSE;
		}
		GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
	} 

	/*check GDMA block size*/
	assert_param(GDMA_InitStruct->GDMA_BlockSize <= 4096);

	/*configure GDMA source address */
	GDMA_InitStruct->GDMA_SrcAddr = (u32)pTxData;
	
	/*  Enable GDMA for TX */
	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return _TRUE;
}

/**
  * @brief    Initialize GDMA peripheral for receiving data.
  * @param  Index: 0.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains 
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pRxData: Rx Buffer.
  * @param  Length: Rx Count.
  * @retval   TRUE/FLASE
  */
BOOL AUDIO_SP_RXGDMA_Init(
	u32 Index,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pRxData,
	u32 Length
	)
{
	u8 GdmaChnl;
	AUDIO_SPORT_TypeDef *SPORTx = AUDIO_DEV_TABLE[Index].SPORTx;
	u32 WordLen = AUDIO_SP_GetWordLen(SPORTx);

	assert_param(GDMA_InitStruct != NULL);
	/*obtain a DMA channel and register DMA interrupt handler*/
	GdmaChnl = GDMA_ChnlAlloc(0, CallbackFunc, (u32)CallbackData, 12);
	if (GdmaChnl == 0xFF) {
		// No Available DMA channel
		return _FALSE;
	}

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	/*set GDMA initial structure member value*/
	GDMA_InitStruct->MuliBlockCunt = 0;
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->MaxMuliBlock = 1;
	GDMA_InitStruct->GDMA_DIR = TTFCPeriToMem;
	GDMA_InitStruct->GDMA_SrcHandshakeInterface = AUDIO_DEV_TABLE[Index].Rx_HandshakeInterface;
	GDMA_InitStruct->GDMA_SrcAddr = (u32)&AUDIO_DEV_TABLE[Index].SPORTx->SP_RX_DR;
	GDMA_InitStruct->GDMA_Index = 0;
	GDMA_InitStruct->GDMA_ChNum = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType|TransferType|ErrType);	
	GDMA_InitStruct->GDMA_DstInc = IncType;
	GDMA_InitStruct->GDMA_SrcInc = NoChange;

	/*  Cofigure GDMA transfer */
	if (WordLen == SP_WL_8)  {
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;
		GDMA_InitStruct->GDMA_SrcMsize = MsizeFour;
		
		GDMA_InitStruct->GDMA_DstMsize = MsizeOne;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
		GDMA_InitStruct->GDMA_BlockSize = Length;
	} else {
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
		GDMA_InitStruct->GDMA_SrcMsize = MsizeEight;
		
		GDMA_InitStruct->GDMA_DstMsize = MsizeEight;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
		GDMA_InitStruct->GDMA_BlockSize = Length >> 2;
	}
	
	/*check GDMA block size*/
	assert_param(GDMA_InitStruct->GDMA_BlockSize <= 4096);

	/*configure GDMA destination address */
	GDMA_InitStruct->GDMA_DstAddr = (u32)pRxData;
	
	/*  Enable GDMA for RX */
	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return _TRUE;
}


/******************* (C) COPYRIGHT 2017 Realtek Semiconductor *****END OF FILE****/
