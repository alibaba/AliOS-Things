/**
  ******************************************************************************
  * @file    rtl8721dhp_ssi.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for Serial peripheral interface (SPI):
  *		- Initialization 
  *		- Clock polarity and phase setting
  *		- SPI data frame size setting
  *		- SPI baud rate setting
  *		- Receive/Send data interface
  *		- Get TRx FIFO valid entries
  *		- check SPI device busy status
  *		- SPI device pinmux initialization and deinitialization
  *		- DMA transfers management
  *		- Interrupts and management 
  *
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#include "ameba_soc.h"

/** @addtogroup SPI_Exported_Functions
*@verbatim
  *      
  *          ===================================================================
  *                                 	How to use this driver
  *          ===================================================================
  *			1. Enable peripheral clock using the following functions 
  *				RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE) for SPI0;
  *				RCC_PeriphClockCmd(APBPeriph_SPI1, APBPeriph_SPI1_CLOCK, ENABLE) for SPI1;
  *
  *			2. Configure the SPIx pinmux.
  *				PinCtrl(PERIPHERAL_SPI0, PinmuxSelect, ON) for SPI0;
  *				PinCtrl(PERIPHERAL_SPI1, PinmuxSelect, ON) for SPI1;
  *
  *			3. Disable the SPI using the SSI_Cmd() function 
  *
  *			4. Program the Polarity,Phase,Transfer Mode,Baud Rate Prescaler,DataFrameSize,
  *				Interrupt TRx Threshold level,DMA TRx Threshold level and other parameters using
  *				the SSI_Init() function.
  *
  *			5. Enable the SPI using the SSI_Cmd() function 
  *
  *			6. When using poll:
  *				-Using SSI_Writeable() function make sure that the transmit FIFO is not full,
  *				then using SSI_WriteData() function to send data
  *			     
  *				-Using SSI_Readable() function make sure that the receive FIFO is not empty,
  *				then using SSI_ReadData() function to receive data
  *
  *			7. Enable corresponding interrupt using the function  SSI_INTConfig() if you need to 
  *				use interrupt mode. 
  *
  *			8. When using the DMA mode 
  *				- Configure & Initialize the DMA 
  *				- Active the DMA Tx or Rx using SSI_SetDmaEnable() function
  *
  * @endverbatim
  */
  

const SPI_DevTable SPI_DEV_TABLE[2] = {
		{SPI0_DEV, GDMA_HANDSHAKE_INTERFACE_SPI0_TX, GDMA_HANDSHAKE_INTERFACE_SPI0_RX, SPI0_IRQ},
		{SPI1_DEV, GDMA_HANDSHAKE_INTERFACE_SPI1_TX, GDMA_HANDSHAKE_INTERFACE_SPI1_RX, SPI1_IRQ},
};

/**
  * @brief  Fills each SSI_InitStruct member with its default value.
  * @param  SSI_InitStruct: pointer to a SSI_InitTypeDef structure which will be 
  *         initialized.
  * @retval None
  */
void
SSI_StructInit(SSI_InitTypeDef* SSI_InitStruct)
{
	SSI_InitStruct->SPI_RxThresholdLevel  =    0;  // if number of entries in th RX FIFO >= (RxThresholdLevel+1), RX interrupt asserted
	SSI_InitStruct->SPI_TxThresholdLevel  =    32;  // if number of entries in th TX FIFO <= TxThresholdLevel, TX interrupt asserted
	SSI_InitStruct->SPI_DmaRxDataLevel   =      3;
#ifdef SPI_SLAVE_TXERR_WORK_AROUND
	SSI_InitStruct->SPI_DmaTxDataLevel	 =	  48;
#else
	SSI_InitStruct->SPI_DmaTxDataLevel   =    56;
#endif
	SSI_InitStruct->SPI_SlaveSelectEnable =    0;
	SSI_InitStruct->SPI_ClockDivider      = 6;    
	SSI_InitStruct->SPI_DataFrameNumber   =    0;
	SSI_InitStruct->SPI_DataFrameFormat   = FRF_MOTOROLA_SPI;
	SSI_InitStruct->SPI_DataFrameSize     = DFS_8_BITS;
	SSI_InitStruct->SPI_InterruptMask     =  0x0;
	SSI_InitStruct->SPI_SclkPhase             = SCPH_TOGGLES_AT_START;
	SSI_InitStruct->SPI_SclkPolarity          = SCPOL_INACTIVE_IS_HIGH;
	SSI_InitStruct->SPI_TransferMode          = TMOD_TR;
	SSI_InitStruct->SPI_MicrowireControlFrameSize  = CFS_1_BIT;
	SSI_InitStruct->SPI_MicrowireDirection    = MW_DIRECTION_MASTER_TO_SLAVE;
	SSI_InitStruct->SPI_MicrowireHandshaking  = MW_HANDSHAKE_DISABLE;
	SSI_InitStruct->SPI_MicrowireTransferMode = MW_TMOD_NONSEQUENTIAL;
}


/**
  * @brief    Initializes the SPI registers according to the specified parameters 
  *         in SSI_InitStruct.
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @param  SSI_InitStruct: pointer to a SSI_InitTypeDef structure that contains 
  *         the configuration information for the SPI peripheral.
  * @retval None
  */

void SSI_Init(SPI_TypeDef *spi_dev, SSI_InitTypeDef *SSI_InitStruct)
{
	u32 TempValue  = 0;
	
	SSI_Cmd(spi_dev, DISABLE);

	/* REG_DW_SSI_CTRLR0 */
	TempValue |= SSI_InitStruct->SPI_DataFrameSize;
	TempValue |= (SSI_InitStruct->SPI_DataFrameFormat << 4);
	TempValue |= (SSI_InitStruct->SPI_SclkPhase << 6);
	TempValue |= (SSI_InitStruct->SPI_SclkPolarity << 7);
	TempValue |= (SSI_InitStruct->SPI_TransferMode << 8);
	TempValue |= (SSI_InitStruct->SPI_MicrowireControlFrameSize << 12);
	TempValue &= ~BIT_CTRLR0_SLV_OE;//(SlaveOutputEnable);

	spi_dev->CTRLR0 = TempValue;

	/* REG_DW_SSI_TXFTLR */
	spi_dev->TXFTLR = SSI_InitStruct->SPI_TxThresholdLevel;

	/* REG_DW_SSI_RXFTLR */
	spi_dev->RXFTLR = SSI_InitStruct->SPI_RxThresholdLevel;

	/* Master Only:REG_DW_SSI_CTRLR1, REG_DW_SSI_SER, REG_DW_SSI_BAUDR*/
	if (SSI_InitStruct->SPI_Role & SSI_MASTER) {
		spi_dev->CTRLR1 = SSI_InitStruct->SPI_DataFrameNumber;
		SSI_SetSlaveEnable(spi_dev, SSI_InitStruct->SPI_SlaveSelectEnable);
		spi_dev->BAUDR = SSI_InitStruct->SPI_ClockDivider;
	}

	// Microwire
	TempValue = 0;
	TempValue |= SSI_InitStruct->SPI_MicrowireTransferMode;
	TempValue |= (SSI_InitStruct->SPI_MicrowireDirection << 1);
	TempValue |= (SSI_InitStruct->SPI_MicrowireHandshaking << 2);

	spi_dev->MWCR = TempValue;

	/* REG_DW_SSI_IMR */
	spi_dev->IMR = SSI_InitStruct->SPI_InterruptMask;
	
	/*DMA level set */
	SSI_SetDmaLevel(spi_dev, SSI_InitStruct->SPI_DmaTxDataLevel, SSI_InitStruct->SPI_DmaRxDataLevel);

	SSI_Cmd(spi_dev, ENABLE);
}

/**
  * @brief  Enables or disables SPIx peripheral.
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @param  NewStatus: This parameter can be one of the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @retval None
  */

void SSI_Cmd(SPI_TypeDef *spi_dev, u32 NewStatus)
{
	if (NewStatus != DISABLE) {
		spi_dev->SSIENR |= BIT_SSIENR_SSI_EN;
	} else {
		spi_dev->SSIENR &= ~BIT_SSIENR_SSI_EN;
	}
}

/**
  * @brief  Masks or unmasks SPIx interrupt.
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @param  SSI_IT: This parameter can be one of the following values or mixed:
  *            @arg BIT_IMR_TXEIM  
  *            @arg BIT_IMR_TXOIM  
  *            @arg BIT_IMR_RXUIM  
  *            @arg BIT_IMR_RXOIM  
  *            @arg BIT_IMR_RXFIM  
  *            @arg BIT_IMR_MSTIM
  *            @arg BIT_IMR_FAEIM
  *            @arg BIT_IMR_TXUIM
  *            @arg BIT_IMR_SSRIM
  * @param   newState:This parameter can be one of the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @retval None
  * @note  BIT_IMR_MSTIM is for Master only, and BIT_IMR_FAEIM, BIT_IMR_TXUIM, BIT_IMR_SSRIM
  *            are for Slave only.
  */
VOID
SSI_INTConfig(SPI_TypeDef* spi_dev, u32 SSI_IT, u32 newState)
{
	if (newState == ENABLE) {
		/* Enable the selected SSI interrupts */
		spi_dev->IMR |= SSI_IT;
	} else {
		/* Disable the selected SSI interrupts */
		spi_dev->IMR &= ~SSI_IT;
	}
}

/**
  * @brief  Set SPI0 as Master or Slave.
  * @param  spi_dev: where spi_dev can be SPI0_DEV.
  * @param  role: This parameter can be one of the following values:
  *            @arg SSI_MASTER
  *            @arg SSI_SLAVE
  * @retval None
  */
void SSI_SetRole(SPI_TypeDef *spi_dev, u32 role)
{
	u32 Temp;

	assert_param(spi_dev == SPI0_DEV);

	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_HS_SPI_CTRL);
	Temp &= ~BIT_HS_SPI0_ROLE_SELECT;
	Temp |= (role << 3);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_HS_SPI_CTRL, Temp);
}


/**
  * @brief  Set SPIx clock polarity.
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @param  SclkPolarity: This parameter can be one of the following values:
  *            @arg SCPOL_INACTIVE_IS_HIGH
  *            @arg SCPOL_INACTIVE_IS_LOW
  * @retval None
  */

void SSI_SetSclkPolarity(SPI_TypeDef *spi_dev, u32 SclkPolarity)
{
	u32 Ctrlr0Value;

	Ctrlr0Value = spi_dev->CTRLR0;
	Ctrlr0Value &= ~BIT_CTRLR0_SCPOL;
	Ctrlr0Value |= (SclkPolarity << 7);

	SSI_Cmd(spi_dev, DISABLE);
	spi_dev->CTRLR0 = Ctrlr0Value;
	SSI_Cmd(spi_dev, ENABLE);
}

/**
  * @brief  Set SPIx clock phase.
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @param  SclkPhase: This parameter can be one of the following values:
  *            @arg SCPH_TOGGLES_AT_START
  *            @arg SCPH_TOGGLES_IN_MIDDLE
  * @retval None
  */

void SSI_SetSclkPhase(SPI_TypeDef *spi_dev, u32 SclkPhase)
{
	u32 Ctrlr0Value;

	Ctrlr0Value = spi_dev->CTRLR0;
	Ctrlr0Value &= ~BIT_CTRLR0_SCPH;
	Ctrlr0Value |= (SclkPhase << 6);

	SSI_Cmd(spi_dev, DISABLE);
	spi_dev->CTRLR0 = Ctrlr0Value;
	SSI_Cmd(spi_dev, ENABLE);
}

/**
  * @brief  Set SPIx data frame size.
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @param  DataFrameSize: This parameter can be one of the following values:
  *            @arg DFS_4_BITS
  *            @arg DFS_5_BITS
  *            @arg DFS_6_BITS
  *            @arg DFS_7_BITS
  *            @arg DFS_8_BITS
  *            @arg DFS_9_BITS
  *            @arg DFS_10_BITS
  *            @arg DFS_11_BITS
  *            @arg DFS_12_BITS
  *            @arg DFS_13_BITS
  *            @arg DFS_14_BITS
  *            @arg DFS_15_BITS
  *            @arg DFS_16_BITS
  * @retval None
  */

void SSI_SetDataFrameSize(SPI_TypeDef *spi_dev, u32 DataFrameSize)
{
	u32 Ctrlr0Value;

	Ctrlr0Value = spi_dev->CTRLR0;
	Ctrlr0Value &= ~BIT_CTRLR0_DFS;
	Ctrlr0Value |= DataFrameSize;

	SSI_Cmd(spi_dev, DISABLE);
	spi_dev->CTRLR0 = Ctrlr0Value;
	SSI_Cmd(spi_dev, ENABLE);
}

/**
  * @brief  Set SPI SS Toggle Phase.
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @param  TogglePhase: This parameter can be one of the following values:
  *            @arg SPI_SS_NOT_TOGGLE , means SPI support continuous transfer when spi_cph=0
  *            @arg SPI_SS_TOGGLE, means SPI donot support continuous transfer when spi_cph=0 
  *              and SS(CS) needs break.
  * @retval None
  * @note  SS Toggle function is valid only for master.
  */
void SSI_SetSSTogglePhase(SPI_TypeDef *spi_dev, u32 TogglePhase)
{
	u32 Ctrlr0Value;

	Ctrlr0Value = spi_dev->CTRLR0;
	Ctrlr0Value &= ~BIT_CTRLR0_SSTOGGLE;
	Ctrlr0Value |= (TogglePhase << 31);

	SSI_Cmd(spi_dev, DISABLE);
	spi_dev->CTRLR0 = Ctrlr0Value;
	SSI_Cmd(spi_dev, ENABLE);
}


/**
  * @brief  Set or reset SPIx data swap bit.
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @param  SwapStatus: This parameter can be one of the following values or mixed:
  *            @arg BIT_CTRLR0_TXBYTESWP  
  *            @arg BIT_CTRLR0_TXBITSWP  
  *            @arg BIT_CTRLR0_RXBYTESWP  
  *            @arg BIT_CTRLR0_RXBITSWP
  * @param   newState:This parameter can be one of the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @retval None
  */
void SSI_SetDataSwap(SPI_TypeDef *spi_dev, u32 SwapStatus, u32 newState)
{
	u32 Ctrlr0Value = spi_dev->CTRLR0;
	
	if(newState == ENABLE)
		Ctrlr0Value |= SwapStatus;
	else
		Ctrlr0Value &= ~SwapStatus;

	SSI_Cmd(spi_dev, DISABLE);
	spi_dev->CTRLR0 = Ctrlr0Value;
	SSI_Cmd(spi_dev, ENABLE);
}

/**
  * @brief  Set SPIx Rx Sample Dealy. support form AZ BCUT
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @param  SampleDelay: The number of ssi_clk cycles that to be delayed.
  * @retval None
  */

void SSI_SetSampleDelay(SPI_TypeDef *spi_dev, u32 SampleDelay)
{
	SSI_Cmd(spi_dev, DISABLE);
	spi_dev->RX_SAMPLE_DLY = (SampleDelay & BIT_RX_SAMPLE_DELAY);
	SSI_Cmd(spi_dev, ENABLE);
}

/**
  * @brief  Set SPI1 the number of data frame to be received.
  * @note Valid only when the device is configured as a master in following mode:
  * 	 TMOD_RO || TMOD_EEPROM_R || (FRF_NS_MICROWIRE && MW_TMOD_SEQUENTIAL) 
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @param  DataFrameNumber: The number of data frames that to be received.
  * @retval None
  */

void SSI_SetReadLen(SPI_TypeDef *spi_dev, u32 DataFrameNumber)
{
	assert_param((DataFrameNumber >= 1)&&(DataFrameNumber <= 0x10000));
	SSI_Cmd(spi_dev, DISABLE);
	spi_dev->CTRLR1 = DataFrameNumber - 1;	
	SSI_Cmd(spi_dev, ENABLE);
}	

/**
  * @brief  Set SPI1 clock divider.
  * @note Valid only when the device is configured as a master.
  *    And The LSB is always set to 0,and is unaffected by a write operation.
  * @param  spi_dev: where spi_dev can only be SPI1_DEV.
  * @param  ClockDivider: Even value between 2 and 65534.
  *		     And Fsclk_out=Fssi_clk/ClockDivider.
  * @retval None
  */

void SSI_SetBaudDiv(SPI_TypeDef *spi_dev, u32 ClockDivider)
{
	SSI_Cmd(spi_dev, DISABLE);
	spi_dev->BAUDR = (ClockDivider & BIT_BAUDR_SCKDV);		
	SSI_Cmd(spi_dev, ENABLE);
}	



/**
  * @brief Enables or disables SPIx TDMA and RDMA .
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @param  newState:  This parameter can be one of the following values:
  *              @arg ENABLE
  *              @arg DISABLE
  * @param  Mask:  This parameter can be one of the following values or mixed:
  *              @arg BIT_SHIFT_DMACR_RDMAE
  *              @arg BIT_SHIFT_DMACR_TDMAE
  * @retval None
  */

void SSI_SetDmaEnable(SPI_TypeDef *spi_dev, u32 newState, u32 Mask)
{
	if (newState == DISABLE)
		spi_dev->DMACR &= ~Mask;
	else
		spi_dev->DMACR |= Mask;
}	

/**
  * @brief Set SPIx DMA TxLevel and RxLevel.
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @param  TxLevel:  Transmit FIFO Threshold level.Value range: 0 to 63.
  *		The dma_tx_req is generated when the number of valid data entries in 
  *		the transmit FIFO is equal to or below this field value.
  * @param  RxLevel:  Receive FIFO Threshold level.Value range: 0 to 63.
  *		The dma_rx_req is generated when the number of valid data entries in the 
  *		receive FIFO is equal to or above this field value + 1.
  * @retval None
  */

void SSI_SetDmaLevel(SPI_TypeDef *spi_dev, u32 TxLevel, u32 RxLevel)
{
	/* Set TX FIFO water level to trigger Tx DMA transfer */
	spi_dev->DMATDLR = TxLevel;
	
	/* Set RX FIFO water level to trigger Rx DMA transfer */
	spi_dev->DMARDLR = RxLevel;
}

/**
  * @brief    Init and Enable SPI TX GDMA.
  * @param  Index: 0 or 1.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains 
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pTxData: Tx Buffer.
  * @param  Length: Tx Count.
  * @retval   TRUE/FLASE
  */

BOOL SSI_TXGDMA_Init(
	u32 Index,
	PGDMA_InitTypeDef GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pTxData,
	u32 Length
	)
{
	SPI_TypeDef *SPIx = SPI_DEV_TABLE[Index].SPIx;
	u32 DataFrameSize = SSI_GetDataFrameSize(SPIx);
	u8 GdmaChnl;

	assert_param(GDMA_InitStruct != NULL);

	GdmaChnl = GDMA_ChnlAlloc(0, CallbackFunc, (u32)CallbackData, 12);//ACUT is 0x10, BCUT is 12
	if (GdmaChnl == 0xFF) {
		// No Available DMA channel
		return _FALSE;
	}

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	GDMA_InitStruct->GDMA_DIR      = TTFCMemToPeri;
	GDMA_InitStruct->GDMA_DstHandshakeInterface   = SPI_DEV_TABLE[Index].Tx_HandshakeInterface;
	GDMA_InitStruct->GDMA_DstAddr = (u32)&SPI_DEV_TABLE[Index].SPIx->DR;
	GDMA_InitStruct->GDMA_Index   = 0;
	GDMA_InitStruct->GDMA_ChNum       = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType|TransferType|ErrType);

	GDMA_InitStruct->GDMA_SrcMsize   = MsizeOne;
	GDMA_InitStruct->GDMA_DstMsize  = MsizeFour;
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
	GDMA_InitStruct->GDMA_DstInc = NoChange;
	GDMA_InitStruct->GDMA_SrcInc = IncType;
	
	/*  Cofigure GDMA transfer */
	if (DataFrameSize > 8) {
		/*  16~9 bits mode */
		if (((Length & 0x03)==0) && (((u32)(pTxData) & 0x03)==0)) {
			/*  4-bytes aligned, move 4 bytes each transfer */
			GDMA_InitStruct->GDMA_SrcMsize   = MsizeFour;
			GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
			GDMA_InitStruct->GDMA_DstMsize  = MsizeEight;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthTwoBytes;
			GDMA_InitStruct->GDMA_BlockSize = Length >> 2;
		} else if (((Length & 0x01)==0) && (((u32)(pTxData) & 0x01)==0)) {
			/*  2-bytes aligned, move 2 bytes each transfer */
			GDMA_InitStruct->GDMA_SrcMsize   = MsizeEight;
			GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthTwoBytes;
			GDMA_InitStruct->GDMA_DstMsize  = MsizeEight;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthTwoBytes;
			GDMA_InitStruct->GDMA_BlockSize = Length >> 1;
		} else {
			DBG_8195A("SSI_TXGDMA_Init: Aligment Err: pTxData=0x%x,  Length=%d\n", pTxData, Length);
			return _FALSE;
		}
	} else {
		/*  8~4 bits mode */
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
		GDMA_InitStruct->GDMA_DstMsize  = MsizeFour;
		GDMA_InitStruct->GDMA_DstDataWidth =  TrWidthOneByte;
	}

	assert_param(GDMA_InitStruct->GDMA_BlockSize <= 4096);
	
	GDMA_InitStruct->GDMA_SrcAddr = (u32)pTxData;
	
	/*  Enable GDMA for TX */
	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return _TRUE;
}

/**
  * @brief    Init and Enable SPI RX GDMA.
  * @param  Index: 0 or 1.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains 
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pRxData: Rx Buffer.
  * @param  Length: Rx Count.
  * @retval   TRUE/FLASE
  */

BOOL 
SSI_RXGDMA_Init(
	u8 Index,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8  *pRxData,
	u32 Length
)
{
	SPI_TypeDef *SPIx = SPI_DEV_TABLE[Index].SPIx;
	u32 DataFrameSize = SSI_GetDataFrameSize(SPIx);
	u8 GdmaChnl;

	assert_param(GDMA_InitStruct != NULL);
	
	GdmaChnl = GDMA_ChnlAlloc(0, CallbackFunc, (u32)CallbackData, 12);//ACUT is 0x10, BCUT is 12
	if (GdmaChnl == 0xFF) {
		// No Available DMA channel
		return _FALSE;
	}

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	GDMA_InitStruct->GDMA_DIR      = TTFCPeriToMem;
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->GDMA_SrcHandshakeInterface    = SPI_DEV_TABLE[Index].Rx_HandshakeInterface;
	GDMA_InitStruct->GDMA_SrcAddr = (u32)&SPI_DEV_TABLE[Index].SPIx->DR;
	GDMA_InitStruct->GDMA_Index   = 0;
	GDMA_InitStruct->GDMA_ChNum       = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType|TransferType|ErrType);

	GDMA_InitStruct->GDMA_SrcMsize   = MsizeEight;
	GDMA_InitStruct->GDMA_DstMsize  = MsizeFour;
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthTwoBytes;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
	GDMA_InitStruct->GDMA_DstInc = IncType;
	GDMA_InitStruct->GDMA_SrcInc = NoChange;
	
	/*  Cofigure GDMA transfer */
	if (DataFrameSize > 8) {
		/*  16~9 bits mode */
		GDMA_InitStruct->GDMA_SrcMsize   = MsizeFour;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthTwoBytes;
		GDMA_InitStruct->GDMA_BlockSize = Length >> 1;

		if (((Length & 0x03)==0) && (((u32)(pRxData) & 0x03)==0)) {
			/*  4-bytes aligned, move 4 bytes each transfer */
			GDMA_InitStruct->GDMA_DstMsize  = MsizeFour;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
		} else if (((Length & 0x01)==0) &&	(((u32)(pRxData) & 0x01)==0)) {
			/*  2-bytes aligned, move 2 bytes each transfer */
			GDMA_InitStruct->GDMA_DstMsize  = MsizeEight;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthTwoBytes;
		} else {
			DBG_8195A("SSI_RXGDMA_Init: Aligment Err: pTxData=0x%x,  Length=%d\n", pRxData, Length);
			return _FALSE;
		}
	} else {
		/*  8~4 bits mode */
		GDMA_InitStruct->GDMA_SrcMsize   = MsizeFour;
		GDMA_InitStruct->GDMA_SrcDataWidth =  TrWidthOneByte;
		GDMA_InitStruct->GDMA_BlockSize = Length;
		if (((Length & 0x03)==0) && (((u32)(pRxData) & 0x03)==0)) {
			/*  4-bytes aligned, move 4 bytes each transfer */
			GDMA_InitStruct->GDMA_DstMsize  = MsizeOne;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
		} else {
			GDMA_InitStruct->GDMA_DstMsize  = MsizeFour;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
		}
	}

	assert_param(GDMA_InitStruct->GDMA_BlockSize <= 4096);
	
	GDMA_InitStruct->GDMA_DstAddr = (u32)pRxData;

	/*  Enable GDMA for RX */
	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return _TRUE;
}

/**
  * @brief Clear SPIx interrupt status.
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @param  InterruptStatus:  The value of current interrupt status .
  * @retval None
  */

void SSI_SetIsrClean(SPI_TypeDef *spi_dev, u32 InterruptStatus)
{

	if (InterruptStatus & BIT_ISR_TXOIS) {
		spi_dev->TXOICR;
	}

	if (InterruptStatus & BIT_ISR_RXUIS) {
		spi_dev->RXUICR;
	}

	if (InterruptStatus & BIT_ISR_RXOIS) {
		spi_dev->RXOICR;
	}

	if (InterruptStatus & BIT_ISR_MSTIS) {
		/* Another master is actively transferring data */
		/* TODO: Do reading data... */
		spi_dev->MSTICR;
	}

	if(InterruptStatus & BIT_ISR_TXUIS) {
		/* For slave only. This register is used as TXUICR in slave mode*/
		spi_dev->IDR;
	}

	if(InterruptStatus & BIT_ISR_SSRIS) {
		/* For slave only. This register is used as SSRICR in slave mode*/
		spi_dev->SSI_COMP_VERSION;
	}

}

/**
  * @brief Write data to SPIx transmit FIFO. 
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @param  value:  The data value that is to be transmitted .
  * @retval None
  */

void SSI_WriteData(SPI_TypeDef *spi_dev, u32 value)
{
	spi_dev->DR[0] = (value & BIT_DR_DR);
}

/**
  * @brief Set SPIx Rx FIFO threshold level
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @param  RxThresholdLevel:  Rx FIFO Threshold Level.Value range: 0 to 63.
  *		When the number of receive FIFO entries is greater than or equal to 
  *		this value + 1, the receive FIFO full interrupt is triggered.
  * @retval None
  */

void SSI_SetRxFifoLevel(SPI_TypeDef *spi_dev, u32 RxThresholdLevel)
{
	spi_dev->RXFTLR = RxThresholdLevel;
}

/**
  * @brief Set SPIx Tx FIFO threshold level.
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @param  TxThresholdLevel:  Tx FIFO Threshold Level.Value range: 0 to 63.
  *		When the number of transmit FIFO entries is less than or equal to this 
  *		value, the transmit FIFO empty interrupt is triggered. 
  * @retval None
  */

void SSI_SetTxFifoLevel(SPI_TypeDef *spi_dev, u32 TxThresholdLevel)
{
	spi_dev->TXFTLR = TxThresholdLevel;
}

/**
  * @brief  Enables or disables slave .
  * @note  Valid only when the device is configured as a master.
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @param  SlaveIndex: the index of slave to be selected.
  * @retval None
  */

void SSI_SetSlaveEnable(SPI_TypeDef *spi_dev, u32 SlaveIndex)
{
	if (SSI_Busy(spi_dev)) {
		DBG_8195A("SSI%d is busy\n", SlaveIndex);
		return;
	}

	spi_dev->SER = (1 << SlaveIndex);
}

/**
  * @brief  Detemines whether SPIx transmit FIFO is full or not
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @retval Transmit FIFO is full or not:
  *        - 1: Not Full
  *        - 0: Full
  */

u32 SSI_Writeable(SPI_TypeDef *spi_dev)
{
	u8 status = 0;
	u32 value = 0;
	
#ifdef SPI_SLAVE_TXERR_WORK_AROUND
	value = SSI_GetTxCount(spi_dev);
	status = ((value < SSI_TX_FIFO_DEPTH - 1) ? 1 : 0);
#else
	value = SSI_GetStatus(spi_dev);
	status = (((value & BIT_SR_TFNF) != 0) ? 1 : 0);
#endif

	return status;
}

/**
  * @brief  Detemine SPIx Receive FIFO is empty or not.
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @retval Receive FIFO is empty or not:
  *        - 1: Not Empty
  *        - 0: Empty
  */

u32 SSI_Readable(SPI_TypeDef *spi_dev)
{
	u32 Status = SSI_GetStatus(spi_dev);
	u32 Readable = (((Status & BIT_SR_RFNE) != 0) ? 1 : 0);

	return Readable;
}

/**
  * @brief Read data from SPIx receive FIFO .
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @retval The data received.
  */

u32 SSI_ReadData(SPI_TypeDef *spi_dev)
{
	return spi_dev->DR[0];
}

/**
  * @brief  receive data from rx FIFO
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @param  RxData: buffer to save data read from SPI FIFO.
  * @param  Length: number of data to be read.
  * @retval transfer len
  */

u32 SSI_ReceiveData(SPI_TypeDef *spi_dev,
	void* RxData,
	u32 Length
	)
{
	u32 ReceiveLevel;
	u32 DataFrameSize = SSI_GetDataFrameSize(spi_dev);
	volatile u32 Readable = SSI_Readable(spi_dev);
	u32 RxLength = Length;

	while (Readable) {
		ReceiveLevel = SSI_GetRxCount(spi_dev);

		while (ReceiveLevel--) {
			if (RxData != NULL) {
				if (DataFrameSize > 8) {
					/*  16~9 bits mode */
					*((u16*)(RxData)) = (u16)SSI_ReadData(spi_dev);
					RxData = (VOID*)(((u16*)RxData) + 1);
				} else {
					/*  8~4 bits mode */
					*((u8*)(RxData)) = (u8)SSI_ReadData(spi_dev);
					RxData = (VOID*)(((u8*)RxData) + 1);
				}
			} else {
				/*  for Master mode, doing TX also will got RX data, so drop the dummy data */
				SSI_ReadData(spi_dev);
			}

			if (RxLength > 0) {
				RxLength--;
			}
			if (RxLength == 0) {
				break;
			}
		}

		if (RxLength == 0) {
			break;
		}

		Readable = SSI_Readable(spi_dev);
	}

	return (Length - RxLength);
}

/**
  * @brief  Send data to tx FIFO
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @param  TxData: buffer to be written to Tx FIFO.
  * @param  Length: number of data to be written.
  * @param  Role: SSI_MASTER or SSI_SLAVE.
  * @retval transfer len
  */

u32 SSI_SendData(SPI_TypeDef *spi_dev,
	void* TxData,
	u32 Length,
	u32 Role
	)
{
	u32 Writeable     = SSI_Writeable(spi_dev);
	u32 TxWriteMax    = SSI_TX_FIFO_DEPTH - SSI_GetTxCount(spi_dev);
	u32 DataFrameSize = SSI_GetDataFrameSize(spi_dev);
	u32 TxLength = Length;

#ifdef SPI_SLAVE_TXERR_WORK_AROUND
	if (Role == SSI_SLAVE)
		TxWriteMax = (TxWriteMax > 0) ? (TxWriteMax - 1) : 0;
#endif

	if (Writeable) {
		/* Disable Tx FIFO Empty IRQ */
		SSI_INTConfig(spi_dev, BIT_IMR_TXEIM, DISABLE);

		while (TxWriteMax--) {
			if (DataFrameSize > 8) {
				// 16~9 bits mode
				if (TxData != NULL) {
					SSI_WriteData(spi_dev, *((u16*)(TxData)));
					TxData = (VOID*)(((u16*)TxData) + 1);
				} else {
					// For master mode: Push a dummy to TX FIFO for Read
					if (Role == SSI_MASTER) {
						SSI_WriteData(spi_dev, (u16)0);// Dummy byte
					}
				}
			} else {
				// 8~4 bits mode
				if (TxData != NULL) {
					SSI_WriteData(spi_dev, *((u8*)(TxData)));
					TxData = (VOID*)(((u8*)TxData) + 1);
				} else {
					// For master mode: Push a dummy to TX FIFO for Read
					if (Role == SSI_MASTER) {
						SSI_WriteData(spi_dev, (u8)0);// Dummy byte
					}
				}
			}

			TxLength--;

			if (TxLength == 0)
				break;
		}

		/* Enable Tx FIFO Empty IRQ */
		SSI_INTConfig(spi_dev, BIT_IMR_TXEIM, ENABLE);
	}

	return (Length - TxLength);
}

/**
  * @brief Get SPIx the number of valid entries in receive FIFO.
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @retval  The number of valid entries in receive FIFO.Value range:0-64.
  */

u32 SSI_GetRxCount(SPI_TypeDef *spi_dev)
{
	return spi_dev->RXFLR & BIT_MASK_RXFLR_RXTFL;
}

/**
  * @brief Get SPIx the number of valid entries in transmit FIFO. 
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @retval  The number of valid entries in transmit FIFO.Value range:0-64.
  */

u32 SSI_GetTxCount(SPI_TypeDef *spi_dev)
{
	return spi_dev->TXFLR & BIT_MASK_TXFLR_TXTFL;
}

/**
  * @brief Get SPIx transfer status.
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @retval  Current transfer status,each bit of this value represents one  
  *		transfer status which is as follows:
  *	
  *		bit 6 : DCOL	Data Collision Error.(valid when device is master)
  *			- 0 : No Error
  *			- 1 : Transmit data collision error
  *			
  *		bit 5 : TXE	Transmission Error.(valid when device is slave)
  *			- 0 : No Error 
  *			- 1 : Transmission error
  *			
  *		bit 4 : RFF	Receive FIFO Full.
  *			- 0 : Receive FIFO is not full
  *			- 1 : Receive FIFO is full
  *			
  *		bit 3 : RFNE	Receive FIFO Not Empty.
  *			- 0 : Receive FIFO is empty
  *			- 1 : Receive FIFO is not empty
  *			
  *		bit 2 : TFE	Transmit FIFO Empty. 
  *			- 0 : Transmit FIFO is not empty
  *			- 1 : Transmit FIFO is empty
  *			
  *		bit 1 : TFNF	Transmit FIFO Not Full. 
  *			- 0 : Transmit FIFO is full
  *			- 1 : Transmit FIFO is not full
  *			
  *		bit 0 : BUSY	SSI Busy Flag. 
  *			- 0 : idle or disable
  *			- 1 : active transferring data
  *	
  */

u32 SSI_GetStatus(SPI_TypeDef *spi_dev)
{
	return spi_dev->SR;
}

/**
  * @brief Get SPIx data frame size. 
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @retval  Data frame size(value range : 4-16):
  *		- n : current data frame length is n bits
  */

u32 SSI_GetDataFrameSize(SPI_TypeDef *spi_dev)
{
	u32 size = ((spi_dev->CTRLR0) & BIT_CTRLR0_DFS) + 1;
	return size;
}

/**
  * @brief  Detemine SPIx is busy or not.
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @retval SPIx busy status value:
  *           - 1: Busy
  *           - 0: Not Busy
  */

u32 SSI_Busy(SPI_TypeDef *spi_dev)
{
	u32 Status = SSI_GetStatus(spi_dev);
	u32 Busy = (((Status & BIT_SR_BUSY) != 0) ? 1 : 0);

	return Busy;
}

/**
  * @brief Get SPIx Interrupt Status. 
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @retval Current Interrupt Status,each bit of this value represents one  
  *		interrupt status which is as follows:
  *
  *		bit 7 : SSRIS  SS_N Rising Edge Detect Interrupt Status. (valid when device is slave)
  *			- 0 : ssi_ssr_intr interrupt not active after masking
  *			- 1 : ssi_ssr_intr interrupt is active after masking
  *
  *		bit 6 : TXUIS  Transmit FIFO Under Flow Interrupt Status. (valid when device is slave)
  *			- 0 : ssi_txu_intr interrupt is not active after masking
  *			- 1 : ssi_txu_intr interrupt is active after masking
  *
  *		bit 5 : FAEIS  Frame Alignment Interrupt Status. (valid when device is slave)
  *			- 0 : ssi_fae_intr interrupt not active after masking
  *			- 1 : ssi_fae_intr interrupt is active after masking
  *
  *		bit 5 : MSTIS  Multi-Master Contention Interrupt Status. (valid when device is master)
  *			- 0 : ssi_mst_intr interrupt not active after masking
  *			- 1 : ssi_mst_intr interrupt is active after masking
  *			
  *		bit 4 : RXFIS	Receive FIFO Full Interrupt Status.
  *			- 0 : ssi_rxf_intr interrupt is not active after masking
  *			- 1 : ssi_rxf_intr interrupt is full after masking
  *			
  *		bit 3 : RXOIS	Receive FIFO Overflow Interrupt Status.
  *			- 0 : ssi_rxo_intr interrupt is not active after masking
  *			- 1 : ssi_rxo_intr interrupt is active after masking
  *			
  *		bit 2 : RXUIS	Receive FIFO Underflow Interrupt Status.
  *			- 0 : ssi_rxu_intr interrupt is not active after masking
  * 		- 1 : ssi_rxu_intr interrupt is active after masking
  *  			
  *		bit 1 : TXOIS	Transmit FIFO Overflow Interrupt Status.
  *			- 0 : ssi_txo_intr interrupt is not active after masking
  *			- 1 : ssi_txo_intr interrupt is active after masking
  *			
  *		bit 0 : TXEIS	Transmit FIFO Empty Interrupt Status.
  *			- 0 : ssi_txe_intr interrupt is not active after masking
  *			- 1 : ssi_txe_intr interrupt is active after masking
  */

u32 SSI_GetIsr(SPI_TypeDef *spi_dev)
{
	return spi_dev->ISR;
}

/**
  * @brief Get SPIx Raw Interrupt Status.
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @retval  Current Raw Interrupt Status,each bit of this value represents one  
  *		raw interrupt status which is as follows:
  *
  *		bit 7 : SSRIR  SS_N Rising Edge Detect Interrupt Status. (valid when device is slave)
  *			- 0 : ssi_ssr_intr interrupt not active after masking
  *			- 1 : ssi_ssr_intr interrupt is active after masking
  *
  *		bit 6 : TXUIR  Transmit FIFO Under Flow Interrupt Status. (valid when device is slave)
  *			- 0 : ssi_txu_intr interrupt is not active after masking
  *			- 1 : ssi_txu_intr interrupt is active after masking
  *
  *		bit 5 : FAEIR  Frame Alignment Interrupt Status. (valid when device is slave)
  *			- 0 : ssi_fae_intr interrupt not active after masking
  *			- 1 : ssi_fae_intr interrupt is active after masking
  *		
  *		bit 5 : MSTIR	Multi-Master Contention Raw Interrupt Status.(valid when device is master)
  *			- 0 : ssi_mst_intr interrupt not active prior to masking
  *			- 1 : ssi_mst_intr interrupt is active prior to masking
  *			
  *		bit 4 : RXFIR	Receive FIFO Full Raw Interrupt Status.
  *			- 0 : ssi_rxf_intr interrupt is not active prior to masking
  *			- 1 : ssi_rxf_intr interrupt is full prior to masking
  *			
  *		bit 3 : RXOIR	Receive FIFO Overflow Raw Interrupt Status.
  *			- 0 : ssi_rxo_intr interrupt is not active prior to masking
  *			- 1 : ssi_rxo_intr interrupt is active prior to masking
  *			
  *		bit 2 : RXUIR	Receive FIFO Underflow Raw Interrupt Status.
  *			- 0 : ssi_rxu_intr interrupt is not active prior to masking
  *  		- 1 : ssi_rxu_intr interrupt is active prior to masking
  *  			
  *		bit 1 : TXOIR	 Transmit FIFO Overflow Raw Interrupt Status.
  *			- 0 : ssi_txo_intr interrupt is not active prior to masking
  *			- 1 : ssi_txo_intr interrupt is active prior to masking
  *			
  *		bit 0 : TXEIR	Transmit FIFO Empty Raw Interrupt Status.
  *			- 0 : ssi_txe_intr interrupt is not active prior to masking
  *			- 1 : ssi_txe_intr interrupt is active prior to masking
  */

u32 SSI_GetRawIsr(SPI_TypeDef *spi_dev)
{
	return spi_dev->RISR;
}

/**
  * @brief Get which slave is selected.
  * @note  Valid only when the device is configured as a master.
  * @param  spi_dev: where spi_dev can be SPI0_DEV or SPI1_DEV.
  * @retval  Each bit of this value which is set to 1 corresponds to the slave being slected.
  */

u32 SSI_GetSlaveEnable(SPI_TypeDef *spi_dev)
{
	return spi_dev->SER;
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
