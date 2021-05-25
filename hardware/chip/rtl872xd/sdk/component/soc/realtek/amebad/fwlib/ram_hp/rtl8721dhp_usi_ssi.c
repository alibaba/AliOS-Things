/**
  ******************************************************************************
  * @file    rtl8721dhp_usi_ssi.c
  * @author
  * @version V1.0.0
  * @date    2017-11-27
  * @brief   This file contains all the functions prototypes for USI-SPI:
  *		- Initialization 
  *		- Clock polarity and phase setting
  *		- SPI data frame size setting
  *		- SPI baud rate setting
  *		- Receive/Send data interface
  *		- Get TRx FIFO valid entries
  *		- check SPI device busy status
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

/** @addtogroup USI_SPI_Exported_Functions
*@verbatim
  *      
  *          ===================================================================
  *                                 	How to use this driver
  *          ===================================================================
  *			1. Enable peripheral clock using the following functions 
  *
  *
  *			2. Configure the SPIx pinmux.
  *
  *
  *			3. Disable the SPI using the USI_SSI_Cmd() function 
  *
  *			4. Program the Polarity,Phase,Transfer Mode,Baud Rate Prescaler,DataFrameSize,
  *				Interrupt TRx Threshold level,DMA TRx Threshold level and other parameters using
  *				the USI_SSI_Init() function.
  *
  *			5. Enable the USI-SPI using the USI_SSI_Cmd() function 
  *
  *			6. When using poll:
  *				-Using USI_SSI_Writeable() function make sure that the transmit FIFO is not full,
  *				then using USI_SSI_WriteData() function to send data
  *			     
  *				-Using USI_SSI_Readable() function make sure that the receive FIFO is not empty,
  *				then using USI_SSI_ReadData() function to receive data
  *
  *			7. Enable corresponding interrupt using the function  USI_SSI_INTConfig() if you need to 
  *				use interrupt mode. 
  *
  *			8. When using the DMA mode 
  *				- Configure & Initialize the DMA 
  *				- Active the DMA Tx or Rx using USI_SSI_SetDmaEnable() function
  *
  * @endverbatim
  */
  
/**
  * @brief  Fills each USI_SSI_InitStruct member with its default value.
  * @param  USI_SSI_InitStruct: pointer to a USI_SSI_InitTypeDef structure which will be 
  *         initialized.
  * @retval None
  */
void
USI_SSI_StructInit(USI_SSI_InitTypeDef* USI_SSI_InitStruct)
{
	USI_SSI_InitStruct->USI_SPI_RxThresholdLevel  =    0;  // if number of entries in the RX FIFO >= RxThresholdLevel + 1, RX interrupt asserted
	USI_SSI_InitStruct->USI_SPI_TxThresholdLevel  =    32;  // if number of empty entries in th TX FIFO >= TxThresholdLevel, TX interrupt asserted
	USI_SSI_InitStruct->USI_SPI_DmaRxDataLevel   =      3;
	USI_SSI_InitStruct->USI_SPI_DmaTxDataLevel   =    56;
	USI_SSI_InitStruct->USI_SPI_ClockDivider      = 6;
	USI_SSI_InitStruct->USI_SPI_DataFrameNumber   =    0;
	USI_SSI_InitStruct->USI_SPI_DataFrameSize     = USI_SPI_DFS_8_BITS;
	USI_SSI_InitStruct->USI_SPI_InterruptMask     =  0x0;
	USI_SSI_InitStruct->USI_SPI_SclkPhase             = USI_SPI_SCPH_TOGGLES_AT_START;
	USI_SSI_InitStruct->USI_SPI_SclkPolarity          = USI_SPI_SCPOL_INACTIVE_IS_HIGH;
	USI_SSI_InitStruct->USI_SPI_TransferMode          = USI_SPI_TMOD_TR;

	USI_SSI_InitStruct->USI_SPI_RxSampleDelay	=	0;
	USI_SSI_InitStruct->USI_SPI_SSTogglePhase	=	0;
}


/**
  * @brief    Initializes the USI-SPI registers according to the specified parameters 
  *         in USI_SSI_InitStruct.
  * @param  usi_dev: where spi_dev can be USI0_DEV.
  * @param  USI_SSI_InitStruct: pointer to a USI_SSI_InitTypeDef structure that contains 
  *         the configuration information for the USI-SPI peripheral.
  * @retval None
  */

void USI_SSI_Init(USI_TypeDef *usi_dev, USI_SSI_InitTypeDef *USI_SSI_InitStruct)
{
	u32 TempValue1  = 0, TempValue2 = 0;

	assert_param(IS_USI_SPI_RxThresholdLevel(USI_SSI_InitStruct->USI_SPI_RxThresholdLevel));
	assert_param(IS_USI_SPI_TxThresholdLevel(USI_SSI_InitStruct->USI_SPI_TxThresholdLevel));

	/* Set USI to SPI mode */
	TempValue1 = usi_dev->USI_MODE_CTRL;
	TempValue1  &= ~USI_SERIAL_MODE;
	TempValue1 |= USI_SERIAL_SPI_MODE;
	usi_dev->USI_MODE_CTRL = TempValue1;

	/* Disable SPI and Tx/Rx Path, for some bits in SPI_CTRL are writeable only when Tx/Rx path are both disable.*/	
	USI_SSI_Cmd(usi_dev, DISABLE);
	USI_SSI_TRxPath_Cmd(usi_dev, USI_SPI_RX_ENABLE | USI_SPI_TX_ENABLE, DISABLE);

	/* Set SPI Control Register */
	TempValue1 = 0;
	TempValue1 |= USI_SSI_InitStruct->USI_SPI_DataFrameSize;
	TempValue1 |= (USI_SSI_InitStruct->USI_SPI_SclkPhase << 6);
	TempValue1 |= (USI_SSI_InitStruct->USI_SPI_SclkPolarity << 7);

	/* Master Only */
	if (USI_SSI_InitStruct->USI_SPI_Role & USI_SPI_MASTER) {
		TempValue1 |= (USI_SSI_InitStruct->USI_SPI_ClockDivider) << 16;
		TempValue1 |= (USI_SSI_InitStruct->USI_SPI_RxSampleDelay) << 8;
		TempValue1 |= (USI_SSI_InitStruct->USI_SPI_SSTogglePhase) << 5;
		TempValue1 |= USI_SPI_MASTER_MODE;

		TempValue2 |= (USI_SSI_InitStruct->USI_SPI_DataFrameNumber) << 16;
	}
	usi_dev->SPI_CTRL = TempValue1;

	/* Set Tx/Rx FIFO Threshold Level*/
	usi_dev->TX_FIFO_CTRL = USI_SPI_TX_FIFO_DEPTH - USI_SSI_InitStruct->USI_SPI_TxThresholdLevel;
	usi_dev->RX_FIFO_CTRL = USI_SSI_InitStruct->USI_SPI_RxThresholdLevel + 1;

	/* Set interrupt */
	usi_dev->INTERRUPT_ENABLE = USI_SSI_InitStruct->USI_SPI_InterruptMask & USI_SPI_INTERRUPT_MASK;

	/*DMA level set */
	USI_SSI_SetDmaLevel(usi_dev, USI_SSI_InitStruct->USI_SPI_DmaTxDataLevel, USI_SSI_InitStruct->USI_SPI_DmaRxDataLevel);

	/* Set Tx/Rx Path enable */
	switch(USI_SSI_InitStruct->USI_SPI_TransferMode){
		case USI_SPI_TMOD_TO:
			TempValue2 |= USI_SPI_TX_ENABLE;
		break;
		case USI_SPI_TMOD_RO:
			TempValue2 |= USI_SPI_RX_ENABLE;
		break;
		case USI_SPI_TMOD_TR:
			TempValue2 |= (USI_SPI_TX_ENABLE | USI_SPI_RX_ENABLE);
		break;
		default:
		break;
	}
	usi_dev->SPI_TRANS_EN  = TempValue2;

	/* Enable SPI. SPI can work normally when Tx/Rx Path and all logic are released.*/
	USI_SSI_Cmd(usi_dev, ENABLE);
}

/**
  * @brief  Enables or disables USI-SPI peripheral.
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @param  NewStatus: This parameter can be one of the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @retval None
  */

void USI_SSI_Cmd(USI_TypeDef *usi_dev, u32 NewStatus)
{
	if (NewStatus != DISABLE) {
		usi_dev->SW_RESET |= USI_SW_RESET_RSTB | USI_SW_RESET_RXFIFO_RSTB |
			USI_SW_RESET_TXFIFO_RSTB | USI_SW_RESET_RX_RSTB | USI_SW_RESET_TX_RSTB;
	} else {
		usi_dev->SW_RESET &= ~USI_SW_RESET_RSTB;
	}
}


/**
  * @brief  Enables or disables USI-SPI Tx or Rx path.
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @param  path: Tx or Rx path control bit, which can be 
  *            @arg USI_SPI_TX_ENABLE
  *            @arg USI_SPI_RX_ENABLE
  * @param  NewStatus: This parameter can be one of the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @retval None
  */

void USI_SSI_TRxPath_Cmd(USI_TypeDef *usi_dev, u32 path, u32 NewStatus)
{
	if(NewStatus != DISABLE){
		usi_dev->SPI_TRANS_EN |= path;
	} else {
		usi_dev->SPI_TRANS_EN &= ~path;
	}
}

/**
  * @brief  Get USI-SPI Tx/Rx path configuration.
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @retval  Current TRx Path setting, each bit of this value represents one  
  *		path which is as follows:
  *
  *  			bit 1 : USI_SPI_RX_ENABLE
  *			- 0 : Rx Path is disable
  *			- 1 : Rx Path is enable   
  *
  *  			bit 0 : USI_SPI_TX_ENABLE
  *			- 0 : Tx Path is disable
  *			- 1 : Tx Path is enable  
  *
  * @retval None
  */
u32 USI_SSI_GetTRxPath(USI_TypeDef *usi_dev)
{
	return usi_dev->SPI_TRANS_EN & (USI_SPI_RX_ENABLE | USI_SPI_TX_ENABLE);
}

/**
  * @brief  Set USI SPI as Master or Slave.
  * @param  usi_dev: where spi_dev can be USI0_DEV.
  * @param  role: This parameter can be one of the following values:
  *            @arg USI_SPI_MASTER
  *            @arg USI_SPI_SLAVE
  * @retval None
  */
void USI_SSI_SetRole(USI_TypeDef *usi_dev, u32 role)
{
	u32 Temp;
	u32 trxpath = USI_SSI_GetTRxPath(usi_dev);

	Temp = usi_dev->SPI_CTRL;
	Temp &= ~USI_SPI_MASTER_MODE;
	Temp |= (role << 4);

	/* USI_SPI_MASTER_MODE is writeable only when Tx and Rx path are both disable */
	USI_SSI_TRxPath_Cmd(usi_dev, trxpath, DISABLE);
	usi_dev->SPI_CTRL = Temp;
	USI_SSI_TRxPath_Cmd(usi_dev, trxpath, ENABLE);
}


/**
  * @brief  Enable or Disable USI-SPI interrupt.
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @param  USI_SSI_IT: This parameter can be one of the following values or mixed:
  *            @arg USI_TXFIFO_ALMOST_EMTY_INTR_EN
  *            @arg USI_TXFIFO_OVERFLOW_INTR_EN  
  *            @arg USI_TXFIFO_UNDERFLOW_INTR_EN  
  *            @arg USI_RXFIFO_ALMOST_FULL_INTR_EN  
  *            @arg USI_RXFIFO_OVERFLOW_INTR_EN  
  *            @arg USI_RXFIFO_UNDERFLOW_INTR_EN
  *            @arg USI_SPI_RX_DATA_FRM_ERR_INTER_EN
  * @param   newState:This parameter can be one of the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @retval None
  */
VOID
USI_SSI_INTConfig(USI_TypeDef* usi_dev, u32 USI_SSI_IT, u32 newState)
{
	if (newState == ENABLE) {
		/* Enable the selected SSI interrupts */
		usi_dev->INTERRUPT_ENABLE |= USI_SSI_IT;
	} else {
		/* Disable the selected SSI interrupts */
		usi_dev->INTERRUPT_ENABLE &= ~USI_SSI_IT;
	}
}

/**
  * @brief  Set USI-SPI clock polarity.
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @param  SclkPolarity: This parameter can be one of the following values:
  *            @arg USI_SPI_SCPOL_INACTIVE_IS_HIGH
  *            @arg USI_SPI_SCPOL_INACTIVE_IS_LOW
  * @retval None
  */

void USI_SSI_SetSclkPolarity(USI_TypeDef *usi_dev, u32 SclkPolarity)
{
	u32 Value;
	u32 trxpath = USI_SSI_GetTRxPath(usi_dev);

	Value = usi_dev->SPI_CTRL;
	Value &= ~ USI_SPI_CPOL;
	Value |= (SclkPolarity << 7);

	/* SPI_CPOL is writeable only when Tx and Rx path are both disable */
	USI_SSI_TRxPath_Cmd(usi_dev, trxpath, DISABLE);
	usi_dev->SPI_CTRL = Value;
	USI_SSI_TRxPath_Cmd(usi_dev, trxpath, ENABLE);
}

/**
  * @brief  Set USI-SPI clock phase.
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @param  SclkPhase: This parameter can be one of the following values:
  *            @arg USI_SPI_SCPH_TOGGLES_IN_MIDDLE
  *            @arg USI_SPI_SCPH_TOGGLES_AT_START
  * @retval None
  */

void USI_SSI_SetSclkPhase(USI_TypeDef *usi_dev, u32 SclkPhase)
{
	u32 Value;
	u32 trxpath = USI_SSI_GetTRxPath(usi_dev);

	Value = usi_dev->SPI_CTRL;
	Value &= ~USI_SPI_CPH;
	Value |= (SclkPhase << 6);

	/* SPI_CPH is writeable only when Tx and Rx path are both disable */
	USI_SSI_TRxPath_Cmd(usi_dev, trxpath, DISABLE);
	usi_dev->SPI_CTRL = Value;
	USI_SSI_TRxPath_Cmd(usi_dev, trxpath, ENABLE);
}


/**
  * @brief  Set USI-SPI SS Toggle Phase.
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @param  TogglePhase: This parameter can be one of the following values:
  *            @arg USI_SPI_SS_NOT_TOGGLE , means SPI support continuous transfer when spi_cph=0
  *            @arg USI_SPI_SS_TOGGLE, means SPI donot support continuous transfer when spi_cph=0 
  *              and SS(CS) needs break.
  * @retval None
  */

void USI_SSI_SetSSTogglePhase(USI_TypeDef *usi_dev, u32 TogglePhase)
{
	u32 Value;
	u32 trxpath = USI_SSI_GetTRxPath(usi_dev);

	Value = usi_dev->SPI_CTRL;
	Value &= ~USI_SPI_SS_TOG_PHASE;
	Value |= (TogglePhase << 5);

	/* SPI_SS_TOG_PHASE is writeable only when Tx and Rx path are both disable */
	USI_SSI_TRxPath_Cmd(usi_dev, trxpath, DISABLE);
	usi_dev->SPI_CTRL = Value;
	USI_SSI_TRxPath_Cmd(usi_dev, trxpath, ENABLE);
}


/**
  * @brief  Set USI-SPI data frame size.
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @param  DataFrameSize: This parameter can be one of the following values:
  *            @arg USI_SPI_DFS_4_BITS
  *            @arg USI_SPI_DFS_5_BITS
  *            @arg USI_SPI_DFS_6_BITS
  *            @arg USI_SPI_DFS_7_BITS
  *            @arg USI_SPI_DFS_8_BITS
  *            @arg USI_SPI_DFS_9_BITS
  *            @arg USI_SPI_DFS_10_BITS
  *            @arg USI_SPI_DFS_11_BITS
  *            @arg USI_SPI_DFS_12_BITS
  *            @arg USI_SPI_DFS_13_BITS
  *            @arg USI_SPI_DFS_14_BITS
  *            @arg USI_SPI_DFS_15_BITS
  *            @arg USI_SPI_DFS_16_BITS
  * @retval None
  */

void USI_SSI_SetDataFrameSize(USI_TypeDef *usi_dev, u32 DataFrameSize)
{
	u32 Value;
	u32 trxpath = USI_SSI_GetTRxPath(usi_dev);

	Value = usi_dev->SPI_CTRL;
	Value &= ~USI_SPI_DAT_FRM_SIZE;
	Value |= DataFrameSize;

	USI_SSI_TRxPath_Cmd(usi_dev, trxpath, DISABLE);
	usi_dev->SPI_CTRL = Value;
	USI_SSI_TRxPath_Cmd(usi_dev, trxpath, ENABLE);
}

/**
  * @brief  Set USI-SPI Rx Sample Dealy.
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @param  SampleDelay: The number of ssi_clk cycles that to be delayed.
  * @retval None
  */

void USI_SSI_SetSampleDelay(USI_TypeDef *usi_dev, u32 SampleDelay)
{
	u32 Value;
	u32 trxpath = USI_SSI_GetTRxPath(usi_dev);

	Value = usi_dev->SPI_CTRL;
	Value &= ~USI_SPI_RX_SAMPLE_DLY;
	Value |= (SampleDelay << 8);
	
	USI_SSI_TRxPath_Cmd(usi_dev, trxpath, DISABLE);
	usi_dev->SPI_CTRL = Value;
	USI_SSI_TRxPath_Cmd(usi_dev, trxpath, ENABLE);
}

/**
  * @brief  Set the number of data frame to be received.
  * @note Valid only when the device is configured as a master in RX Only mode.
  * @param  usi_dev: where usi_dev can only be USI0_DEV.
  * @param  DataFrameNumber: The number of data frames that to be received.
  * @retval None
  */

void USI_SSI_SetReadLen(USI_TypeDef *usi_dev, u32 DataFrameNumber)
{
	u32 Value;
	u32 trxpath = USI_SSI_GetTRxPath(usi_dev);
	assert_param((DataFrameNumber >= 1)&&(DataFrameNumber <= 0x10000));

	Value = usi_dev->SPI_TRANS_EN;
	Value &= ~USI_SPI_RXONLY_NUM;
	Value |= ((DataFrameNumber - 1) << 16);
	
	USI_SSI_TRxPath_Cmd(usi_dev, trxpath, DISABLE);
	usi_dev->SPI_TRANS_EN = Value;	
	USI_SSI_TRxPath_Cmd(usi_dev, trxpath, ENABLE);
}	

/**
  * @brief  Set USI-SPI clock divider.
  * @note Valid only when the device is configured as a master.
  *    And The LSB is always set to 0,and is unaffected by a write operation.
  * @param  usi_dev: where usi_dev can only be USI0_DEV.
  * @param  ClockDivider: Even value between 2 and 65534.
  *		     And Fsclk_out=Fssi_clk/ClockDivider.
  * @retval None
  */

void USI_SSI_SetBaudDiv(USI_TypeDef *usi_dev, u32 ClockDivider)
{
	u32 Value;
	u32 trxpath = USI_SSI_GetTRxPath(usi_dev);

	Value = usi_dev->SPI_CTRL;
	Value &= ~USI_SPI_MST_BAUD;
	Value |= (ClockDivider << 16);

	USI_SSI_TRxPath_Cmd(usi_dev, trxpath, DISABLE);
	usi_dev->SPI_CTRL = Value;
	USI_SSI_TRxPath_Cmd(usi_dev, trxpath, ENABLE);
}	

/**
  * @brief  Set USI-SPI Master clock.
  * @param  USIx: where SPIx can be USI0_DEV.
  * @param  BaudRate: baudrate for SPI bus, unit is HZ.
  * @param  IpClk: IP Clock
  * @retval None
  * @note Valid only when the device is configured as a master.
  * @note BaudRate <= (IpClk/2).
  * @note As the clock divider can only be an even number. The final baudrate this function generates
  * 		may be a little lower than the value of  BaudRate,which is setted by user.
  * @note Fsclk_out = Fssi_clk /SCKDV
  * @note for FPGA IpClk = CPU_ClkGet(_TRUE)
  * @note for ASIC IpClk = CPU_ClkGet(_FALSE)/2
  */

void USI_SSI_SetBaud(USI_TypeDef *USIx, u32 BaudRate, u32 IpClk)
{
	u32 ClockDivider;

	assert_param(USIx == USI0_DEV);
	assert_param(BaudRate <= (IpClk/2));
	
	/*Adjust SCKDV-Parameter to an even number */
	ClockDivider = IpClk/BaudRate + 1;
	if ((IpClk%BaudRate) > (BaudRate/2)) {
		ClockDivider++;
	}
	if (ClockDivider >= 0xFFFF) {
		/*  devider is 16 bits */
		ClockDivider = 0xFFFE;
	}
	ClockDivider &= 0xFFFE;     // bit 0 always is 0
	
	USI_SSI_SetBaudDiv(USIx, ClockDivider);
}

/**
  * @brief Enables or disables USI-SPI TDMA and RDMA .
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @param  newState:  This parameter can be one of the following values:
  *              @arg ENABLE
  *              @arg DISABLE
  * @param  Mask:  This parameter can be one of the following values or mixed:
  *              @arg USI_TX_DMA_ENABLE
  *              @arg USI_RX_DMA_ENABLE
  * @retval None
  */

void USI_SSI_SetDmaEnable(USI_TypeDef *usi_dev, u32 newState, u32 Mask)
{
	if (newState == DISABLE)
		usi_dev->DMA_ENABLE &= ~Mask;
	else
		usi_dev->DMA_ENABLE |= Mask;
}	

/**
  * @brief Set USI-SPI DMA TxLevel and RxLevel.
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @param  TxLevel:  Transmit FIFO Threshold level.Value range: 0 to 63.
  *		The dma_tx_req is generated when the number of valid data entries in 
  *		the transmit FIFO is equal to or below this field value.
  * @param  RxLevel:  Receive FIFO Threshold level.Value range: 0 to 63.
  *		The dma_rx_req is generated when the number of valid data entries in the 
  *		receive FIFO is equal to or above this field value + 1.
  * @retval None
  */

void USI_SSI_SetDmaLevel(USI_TypeDef *usi_dev, u32 TxLevel, u32 RxLevel)
{
	u32 value = usi_dev->DMA_REQ_SIZE;

	assert_param(IS_USI_SPI_TxDMALevel(TxLevel));
	assert_param(IS_USI_SPI_RxDMALevel(RxLevel));
	
	/* Set TX FIFO water level to trigger Tx DMA transfer */
	value &= ~USI_TX_DMA_BURST_SIZE;
	value |= (USI_SPI_TX_FIFO_DEPTH - TxLevel);

	/* Set RX FIFO water level to trigger Rx DMA transfer */
	value &= ~USI_RX_DMA_BURST_SIZE;
	value |= ((RxLevel + 1) << 16);

	usi_dev->DMA_REQ_SIZE = value;
}

/**
  * @brief    Init and Enable USI-SPI TX GDMA.
  * @param  Index: 0.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains 
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pTxData: Tx Buffer.
  * @param  Length: Tx Count.
  * @retval   TRUE/FLASE
  */

BOOL USI_SSI_TXGDMA_Init(
	u32 Index,
	PGDMA_InitTypeDef GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pTxData,
	u32 Length
	)
{
	USI_TypeDef *USIx = USI_DEV_TABLE[Index].USIx;
	u32 DataFrameSize = USI_SSI_GetDataFrameSize(USIx);
	u8 GdmaChnl;

	assert_param(GDMA_InitStruct != NULL);

	GdmaChnl = GDMA_ChnlAlloc(0, CallbackFunc, (u32)CallbackData, 12);//ACUT is 0x10, BCUT is 12
	if (GdmaChnl == 0xFF) {
		// No Available DMA channel
		return _FALSE;
	}

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	GDMA_InitStruct->GDMA_DIR      = TTFCMemToPeri;
	GDMA_InitStruct->GDMA_DstHandshakeInterface   = USI_DEV_TABLE[Index].Tx_HandshakeInterface;
	GDMA_InitStruct->GDMA_DstAddr = (u32)&USI_DEV_TABLE[Index].USIx->TX_FIFO_WRITE;
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
  * @brief    Init and Enable USI-SPI RX GDMA.
  * @param  Index: 0.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains 
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pRxData: Rx Buffer.
  * @param  Length: Rx Count.
  * @retval   TRUE/FLASE
  */

BOOL 
USI_SSI_RXGDMA_Init(
	u8 Index,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8  *pRxData,
	u32 Length
)
{
	USI_TypeDef *USIx = USI_DEV_TABLE[Index].USIx;
	u32 DataFrameSize = USI_SSI_GetDataFrameSize(USIx);
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
	GDMA_InitStruct->GDMA_SrcHandshakeInterface    = USI_DEV_TABLE[Index].Rx_HandshakeInterface;
	GDMA_InitStruct->GDMA_SrcAddr = (u32)&USI_DEV_TABLE[Index].USIx->RX_FIFO_READ;
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
  * @brief Clear USI-SPI interrupt status.
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @param  InterruptStatus:  The value of current interrupt status .
  * @retval None
  */

void USI_SSI_SetIsrClean(USI_TypeDef *usi_dev, u32 InterruptStatus)
{
	usi_dev->INTERRUPT_STATUS_CLR = InterruptStatus & USI_SPI_INTERRUPT_CLEAR_MASK;
}

/**
  * @brief Write data to USI-SPI transmit FIFO. 
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @param  value:  The data value that is to be transmitted .
  * @retval None
  */

void USI_SSI_WriteData(USI_TypeDef *usi_dev, u32 value)
{
	usi_dev->TX_FIFO_WRITE = (value & USI_TX_FIFO_WRITE_DATA);
}

/**
  * @brief Set USI-SPI Rx FIFO threshold level
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @param  RxThresholdLevel:  Rx FIFO Threshold Level.Value range: 0 to 63.
  *		When the number of receive FIFO entries is greater than or equal to 
  *		this value + 1, the receive FIFO full interrupt is triggered.
  * @retval None
  */

void USI_SSI_SetRxFifoLevel(USI_TypeDef *usi_dev, u32 RxThresholdLevel)
{
	assert_param(IS_USI_SPI_RxThresholdLevel(RxThresholdLevel));
	
	usi_dev->RX_FIFO_CTRL = RxThresholdLevel + 1;
}

/**
  * @brief Set USI-SPI Tx FIFO threshold level.
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @param  TxThresholdLevel:  Tx FIFO Threshold Level.Value range: 0 to 63.
  *		When the number of transmit FIFO entries is less than or equal to this 
  *		value, the transmit FIFO empty interrupt is triggered. 
  * @retval None
  */

void USI_SSI_SetTxFifoLevel(USI_TypeDef *usi_dev, u32 TxThresholdLevel)
{
	assert_param(IS_USI_SPI_TxThresholdLevel(TxThresholdLevel));

	usi_dev->TX_FIFO_CTRL = USI_SPI_TX_FIFO_DEPTH - TxThresholdLevel;
}

/**
  * @brief  Detemines whether USI-SPI transmit FIFO is full or not
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @retval Transmit FIFO is full or not:
  *        - 1: Not Full
  *        - 0: Full
  */

u32 USI_SSI_Writeable(USI_TypeDef *usi_dev)
{
	u32 Status = USI_SSI_GetTxFIFOStatus(usi_dev);
	u32 Writeable = (((Status & USI_TXFIFO_FULL) == 0) ? 1 : 0);

	return Writeable;
}

/**
  * @brief  Detemine USI-SPI Receive FIFO is empty or not.
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @retval Receive FIFO is empty or not:
  *        - 1: Not Empty
  *        - 0: Empty
  */

u32 USI_SSI_Readable(USI_TypeDef *usi_dev)
{
	u32 Status = USI_SSI_GetRxFIFOStatus(usi_dev);
	u32 Readable = (((Status & USI_RXFIFO_EMPTY) == 0) ? 1 : 0);

	return Readable;
}

/**
  * @brief Read data from USI-SPI receive FIFO .
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @retval The data received.
  */

u32 USI_SSI_ReadData(USI_TypeDef *usi_dev)
{
	return usi_dev->RX_FIFO_READ;
}

/**
  * @brief  receive data from rx FIFO
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @param  RxData: buffer to save data read from USI-SPI FIFO.
  * @param  Length: number of data to be read.
  * @retval transfer len
  */

u32 USI_SSI_ReceiveData(USI_TypeDef *usi_dev,
	void* RxData,
	u32 Length
	)
{
	u32 ReceiveLevel;
	u32 DataFrameSize = USI_SSI_GetDataFrameSize(usi_dev);
	volatile u32 Readable = USI_SSI_Readable(usi_dev);
	u32 RxLength = Length;

	while (Readable) {
		ReceiveLevel = USI_SSI_GetRxCount(usi_dev);

		while (ReceiveLevel--) {
			if (RxData != NULL) {
				if (DataFrameSize > 8) {
					/*  16~9 bits mode */
					*((u16*)(RxData)) = (u16)USI_SSI_ReadData(usi_dev);
					RxData = (VOID*)(((u16*)RxData) + 1);
				} else {
					/*  8~4 bits mode */
					*((u8*)(RxData)) = (u8)USI_SSI_ReadData(usi_dev);
					RxData = (VOID*)(((u8*)RxData) + 1);
				}
			} else {
				/*  for Master mode, doing TX also will got RX data, so drop the dummy data */
				USI_SSI_ReadData(usi_dev);
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

		Readable = USI_SSI_Readable(usi_dev);
	}

	return (Length - RxLength);
}

/**
  * @brief  Send data to tx FIFO
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @param  TxData: buffer to be written to Tx FIFO.
  * @param  Length: number of data to be written.
  * @param  Role: USI_SPI_MASTER or USI_SPI_SLAVE.
  * @retval transfer len
  */

u32 USI_SSI_SendData(USI_TypeDef *usi_dev,
	void* TxData,
	u32 Length,
	u32 Role
	)
{
	u32 Writeable     = USI_SSI_Writeable(usi_dev);
	u32 TxWriteMax    = USI_SPI_TX_FIFO_DEPTH - USI_SSI_GetTxCount(usi_dev);
	u32 DataFrameSize = USI_SSI_GetDataFrameSize(usi_dev);
	u32 TxLength = Length;

	if (Writeable) {
		/* Disable Tx FIFO Empty IRQ */
		USI_SSI_INTConfig(usi_dev, USI_TXFIFO_ALMOST_EMTY_INTR_EN, DISABLE);

		while (TxWriteMax--) {
			if (DataFrameSize > 8) {
				// 16~9 bits mode
				if (TxData != NULL) {
					USI_SSI_WriteData(usi_dev, *((u16*)(TxData)));
					TxData = (VOID*)(((u16*)TxData) + 1);
				} else {
					// For master mode: Push a dummy to TX FIFO for Read
					if (Role == USI_SPI_MASTER) {
						USI_SSI_WriteData(usi_dev, (u16)0);// Dummy byte
					}
				}
			} else {
				// 8~4 bits mode
				if (TxData != NULL) {
					USI_SSI_WriteData(usi_dev, *((u8*)(TxData)));
					TxData = (VOID*)(((u8*)TxData) + 1);
				} else {
					// For master mode: Push a dummy to TX FIFO for Read
					if (Role == USI_SPI_MASTER) {
						USI_SSI_WriteData(usi_dev, (u8)0);// Dummy byte
					}
				}
			}

			TxLength--;

			if (TxLength == 0)
				break;
		}

		/* Enable Tx FIFO Empty IRQ */
		USI_SSI_INTConfig(usi_dev, USI_TXFIFO_ALMOST_EMTY_INTR_EN, ENABLE);
	}

	return (Length - TxLength);
}

/**
  * @brief Get the number of valid entries in receive FIFO.
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @retval  The number of valid entries in receive FIFO.Value range:0-64.
  */

u32 USI_SSI_GetRxCount(USI_TypeDef *usi_dev)
{
	u32 valid_cnt = (USI_SSI_GetRxFIFOStatus(usi_dev) & USI_RXFIFO_VALID_CNT) >> 8;
	return valid_cnt;
}

/**
  * @brief Get the number of valid entries in transmit FIFO. 
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @retval  The number of valid entries in transmit FIFO.Value range:0-64.
  */

u32 USI_SSI_GetTxCount(USI_TypeDef *usi_dev)
{
	u32 empty_cnt = (USI_SSI_GetTxFIFOStatus(usi_dev) & USI_TXFIFO_EMPTY_SPACE) >> 8;
	return USI_SPI_TX_FIFO_DEPTH - empty_cnt;
}

/**
  * @brief Get USI-SPI Tx FIFO status.
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @retval  Current Tx FIFO status:
  *
  *		bit [8:14] : USI_TXFIFO_EMPTY_SPACE
  *			- Tx FIFO empty entry count
  *			
  *		bit 2 : USI_TXFIFO_ALMOST_EMPTY_COPY
  *			- 0 : Tx FIFO not almost empty
  *			- 1 : Tx FIFO almost empty
  *			
  *		bit 1 : USI_TXFIFO_EMPTY 
  *			- 0 : Tx FIFO not empty
  *			- 1 : Tx FIFO is empty
  *			
  *		bit 0 : USI_TXFIFO_FULL
  *			- 0 : Tx FIFO not full
  *			- 1 : Tx FIFO is full
  *	
  */
u32 USI_SSI_GetTxFIFOStatus(USI_TypeDef *usi_dev)
{
	return usi_dev->TX_FIFO_STATUS;
}

/**
  * @brief Get USI-SPI Rx FIFO status.
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @retval  Current Rx FIFO status:
  *
  *		bit [8:14] : USI_RXFIFO_VALID_CNT
  *			- Rx FIFO valid count
  *			
  *		bit 2 : USI_RXFIFO_ALMOST_FULL_COPY
  *			- 0 : Rx FIFO not almost full
  *			- 1 : Rx FIFO almost full
  *			
  *		bit 1 : USI_RXFIFO_EMPTY 
  *			- 0 : Rx FIFO not empty
  *			- 1 : Rx FIFO is empty
  *			
  *		bit 0 : USI_RXFIFO_FULL
  *			- 0 : Rx FIFO not full
  *			- 1 : Rx FIFO is full
  *	
  */
u32 USI_SSI_GetRxFIFOStatus(USI_TypeDef *usi_dev)
{
	return usi_dev->RX_FIFO_STATUS;
}

/**
  * @brief Get USI-SPI transfer status.
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @retval  Current transfer status,each bit of this value represents one  
  *		transfer status which is as follows:
  *			
  *		bit 4 : USI_SPI_SLV_RX_ACTIVITY	
  *			- 0 : SPI slave Rx path idle
  *			- 1 : SPI slave Rx path is active
  *			
  *		bit 3 : USI_SPI_SLV_TX_ACTIVITY
  *			- 0 : SPI slave Tx path idle
  *			- 1 : SPI slave Tx path is active
  *			
  *		bit 2 : USI_SPI_MST_RX_ACTIVITY
  *			- 0 : SPI master Rx path idle
  *			- 1 : SPI master Rx path is active
  *			
  *		bit 1 : USI_SPI_MST_TX_ACTIVITY
  *			- 0 : SPI master Tx path idle
  *			- 1 : SPI master Tx path is active
  *			
  *		bit 0 : USI_SPI_ACTIVITY 
  *			- 0 : idle
  *			- 1 : active transferring data
  *	
  */
u32 USI_SSI_GetTransStatus(USI_TypeDef *usi_dev)
{
	return usi_dev->SPI_TRANS_STATUS;
}


/**
  * @brief Get USI-SPI data frame size. 
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @retval  Data frame size(value range : 4-16):
  *		- n : current data frame length is n bits
  */

u32 USI_SSI_GetDataFrameSize(USI_TypeDef *usi_dev)
{
	u32 size = ((usi_dev->SPI_CTRL) & USI_SPI_DAT_FRM_SIZE) + 1;
	return size;
}

/**
  * @brief  Detemine USI-SPI is busy or not.
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @retval USI-SPI busy status value:
  *           - 1: Busy
  *           - 0: Not Busy
  */

u32 USI_SSI_Busy(USI_TypeDef *usi_dev)
{
	u32 Status = USI_SSI_GetTransStatus(usi_dev);
	u32 Busy = (((Status & USI_SPI_ACTIVITY) != 0) ? 1 : 0);

	return Busy;
}

/**
  * @brief Get USI-SPI Interrupt Status. 
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @retval Current Interrupt Status,each bit of this value represents one  
  *		interrupt status which is as follows:
  *
  *          bit 16: USI_SPI_RX_DATA_FRM_ERR_INTS
  *		  	- 0 : spi_rx_data_frame_err_intr interrupt not active after masking
  *			- 1 : spi_rx_data_frame_err_intr interrupt is active after masking
  
  *		bit 6 : USI_RXFIFO_UNDERFLOW_INTS
  *			- 0 : rxfifo_underflow_intr interrupt not active after masking
  *			- 1 : rxfifo_underflow_intr interrupt is active after masking
  *			
  *		bit 5 : USI_RXFIFO_OVERFLOW_INTS
  *			- 0 : rxfifo_overflow_intr interrupt is not active after masking
  *			- 1 : rxfifo_overflow_intr interrupt is full after masking
  *			
  *		bit 4 : USI_RXFIFO_ALMOST_FULL_INTS
  *			- 0 : rxfifo_almost_full_intr interrupt is not active after masking
  *			- 1 : rxfifo_almost_full_intr interrupt is active after masking
  *			
  *		bit 2 : USI_TXFIFO_UNDERFLOW_INTS
  *			- 0 : txfifo_underflow_intr interrupt is not active after masking
  * 		       - 1 : txfifo_underflow_intr interrupt is active after masking
  *  			
  *		bit 1 : USI_TXFIFO_OVERFLOW_INTS
  *			- 0 : txfifo_overflow_intr interrupt is not active after masking
  *			- 1 : txfifo_overflow_intr interrupt is active after masking
  *			
  *		bit 0 : USI_TXFIFO_ALMOST_EMTY_INTS
  *			- 0 : txfifo_almost_empty_intr interrupt is not active after masking
  *			- 1 : txfifo_almost_empty_intr interrupt is active after masking
  */

u32 USI_SSI_GetIsr(USI_TypeDef *usi_dev)
{
	return usi_dev->INTERRUPT_STATUS;
}

/**
  * @brief Get USI-SPI Raw Interrupt Status.
  * @param  usi_dev: where usi_dev can be USI0_DEV.
  * @retval  Current Raw Interrupt Status,each bit of this value represents one  
  *		raw interrupt status which is as follows:
  *
  *          bit 16: USI_SPI_RX_DATA_FRM_ERR_RSTS
  *		  	- 0 : spi_rx_data_frame_err_intr interrupt not active before masking
  *			- 1 : spi_rx_data_frame_err_intr interrupt is active before masking
  *
  *		bit 6 : USI_RXFIFO_UNDERFLOW_RSTS
  *			- 0 : rxfifo_underflow_intr interrupt not active before masking
  *			- 1 : rxfifo_underflow_intr interrupt is active before masking
  *			
  *		bit 5 : USI_RXFIFO_OVERFLOW_RSTS
  *			- 0 : rxfifo_overflow_intr interrupt is not active before masking
  *			- 1 : rxfifo_overflow_intr interrupt is full before masking
  *			
  *		bit 4 : USI_RXFIFO_ALMOST_FULL_RSTS
  *			- 0 : rxfifo_almost_full_intr interrupt is not active before masking
  *			- 1 : rxfifo_almost_full_intr interrupt is active before masking
  *			
  *		bit 2 : USI_TXFIFO_UNDERFLOW_RSTS
  *			- 0 : txfifo_underflow_intr interrupt is not active before masking
  * 		       - 1 : txfifo_underflow_intr interrupt is active before masking
  *  			
  *		bit 1 : USI_TXFIFO_OVERFLOW_RSTS
  *			- 0 : txfifo_overflow_intr interrupt is not active before masking
  *			- 1 : txfifo_overflow_intr interrupt is active before masking
  *			
  *		bit 0 : USI_TXFIFO_ALMOST_EMTY_RSTS
  *			- 0 : txfifo_almost_empty_intr interrupt is not active before masking
  *			- 1 : txfifo_almost_empty_intr interrupt is active before masking
  *
  */
u32 USI_SSI_GetRawIsr(USI_TypeDef *usi_dev)
{
	return usi_dev->RAW_INTERRUPT_STATUS;
}


/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/

