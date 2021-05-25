/**
  ******************************************************************************
  * @file    rtl8721d_uart.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the UART firmware
  *             library, including the following functionalities of the Universal Asynchronous 
  *             Receiver/Transmitter (UART) peripheral:
  *           - Uart DMA mode Initialization
  *           - Uart DMA mode config
  *           - Uart DMA mode control(enable or disable)
  *           - Uart Low Power Rx Initialization
  *           - Uart Low Power Rx Monitor Function config
  *           - Low Power Rx Baud Rate Setting
  *           - Uart IrDA Function Initialization
  *           - Uart IrDA Function control(enable or disable) 
  *
  *  @verbatim
  *      
  *          ===================================================================
  *                                 How to use this driver
  *          ===================================================================
  *          1. Enable peripheral clock using the following functions
  *              RCC_PeriphClockCmd(APBPeriph_UARTx, APBPeriph_UARTx_CLOCK, ENABLE); 
  *
  *          2. configure the UART pinmux.
  *		    Pinmux_Config(Pin_Num, PINMUX_FUNCTION_UART)
  *
  *          3. disable uart rx path
  *
  *          4. select rx path clock source(XTAL 40M Hz/XTAL 2M Hz/OSC 2M Hz) 
  *          
  *          5. clear rx fifo. 
  *
  *          6. Program Word Length , Stop Bit, Parity, Hardware flow control and DMA Mode(ENABLE/DISABLE) 
  *              using the UART_Init() function.
  *
  *          7. Program the Baud Rate, using function UART_SetBaud().
  *
  *          8. Enable the NVIC and the corresponding interrupt using the function 
  *             UART_INTConfig() and register the uart irq handler if you need to use interrupt mode. 
  *
  *          9. When using the DMA mode 
  *                   - GDMA related configurations(source address/destination address/block size etc.)  
  *                   - Configure the uart DMA burst size using UART_TXDMAConfig()/UART_RXDMAConfig() function
  *                   - Active the UART TX/RX DMA Request using UART_TXDMACmd()/UART_RXDMACmd() function
  * 
  *          10. enable uart rx path
  *
  *          Refer to related specifications for more details about IrDA function and Low Power Rx Path(monitoring and clock
  *           swicth).
  * 
  *          In order to reach higher communication baudrates, high rate rx path can be used.
  *          Low power rx can be used for saving power.
  *
  *  @endverbatim
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

const UART_DevTable UART_DEV_TABLE[4] = {
		{UART0_DEV, GDMA_HANDSHAKE_INTERFACE_UART0_TX, GDMA_HANDSHAKE_INTERFACE_UART0_RX, UART0_IRQ},   /*uart0 */
		{UART1_DEV, GDMA_HANDSHAKE_INTERFACE_UART1_TX, GDMA_HANDSHAKE_INTERFACE_UART1_RX, UART1_IRQ},	/*uart1_bt */
		{UART2_DEV, 0xFF, 0xFF, UART_LOG_IRQ},															/*log uart */
		{UART3_DEV, GDMA_HANDSHAKE_INTERFACE_UART3_TX, GDMA_HANDSHAKE_INTERFACE_UART3_RX, UARTLP_IRQ},	/*luart */
};

u32 UART_StateTx[4];
u32 UART_StateRx[4];

  /**
  * @brief    configure UART TX DMA burst size .
  * @param  UARTx: where x can be 0/1/3.
  * @param  TxDmaBurstSize: UART TX DMA burst size.
  * @note     Because UART tx fifo depth is 16 in hardware. 
  *              Therefore this value must be no more than 16.
  * @retval   None
  */
void
UART_TXDMAConfig(UART_TypeDef* UARTx, u32 TxDmaBurstSize)
{
	u32 TempMiscr;

	/* Set UART TX DMA burst size.*/
	TempMiscr = UARTx->MISCR;
	TempMiscr &= ~(RUART_TXDMA_BURSTSIZE_MASK);
	TempMiscr |= ((TxDmaBurstSize << 3) & RUART_TXDMA_BURSTSIZE_MASK);

	UARTx->MISCR = TempMiscr;
}

  /**
  * @brief    configure UART RX DMA burst size .
  * @param  UARTx: where x can be 0/1/3.
  * @param  RxDmaBurstSize: UART RX DMA burst size.
  * @note     Because UART rx fifo depth is 16 in hardare. 
  *              Therefore this value must be no more than 16.
  * @retval   None
  */
void
UART_RXDMAConfig(UART_TypeDef* UARTx, u32 RxDmaBurstSize)
{
	u32 TempMiscr;

	/* Set UART Rx DMA burst size */
	TempMiscr = UARTx->MISCR;
	TempMiscr &= ~(RUART_RXDMA_BURSTSIZE_MASK);
	TempMiscr |= ((RxDmaBurstSize << 8) & RUART_RXDMA_BURSTSIZE_MASK);

	UARTx->MISCR = TempMiscr;
}

  /**
  * @brief    enable or disable UART TX DMA .
  * @param  UARTx: where x can be 0/1/3.
  * @param  NewState: the new state of UART TX DMA.
  *               This parameter can be: ENABLE or DISABLE.
  * @retval   None
  */
void
UART_TXDMACmd(UART_TypeDef* UARTx, u32 NewState)
{
	if(NewState != DISABLE ){
		/* enable the UART TX DMA */
		UARTx->MISCR |= RUART_TXDMA_ENABLE;
	} else {
		/* disable the UART TX DMA */
		UARTx->MISCR &= (~ RUART_TXDMA_ENABLE);
	}
}

  /**
  * @brief    enable or disable UART RX DMA .
  * @param  UARTx: where x can be 0/1/3.
  * @param  NewState: the new state of UART RX DMA.
  *              This parameter can be: ENABLE or DISABLE.
  * @retval   None
  */
void
UART_RXDMACmd(UART_TypeDef* UARTx, u32 NewState)
{
	if(NewState != DISABLE ){
		/* enable the UART RX DMA */
		UARTx->MISCR |= RUART_RXDMA_ENABLE;
	} else {
		/* disable the UART RX DMA */
		UARTx->MISCR &= (~ RUART_RXDMA_ENABLE);
	}
}

/**
  * @brief    Init and Enable UART TX GDMA.
  * @param  UartIndex: 0/1/3.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains 
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pTxBuf: Tx Buffer.
  * @param  TxCount: Tx Count.
  * @retval   TRUE/FLASE
  */
BOOL UART_TXGDMA_Init(
	u8 UartIndex,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pTxBuf,
	int TxCount
	)
{
	u8 GdmaChnl;
	
	assert_param(GDMA_InitStruct != NULL);
	
	GdmaChnl = GDMA_ChnlAlloc(0, (IRQ_FUN)CallbackFunc, (u32)CallbackData, 12);//ACUT is 0x10, BCUT is 12
	if (GdmaChnl == 0xFF) {
		/*  No Available DMA channel */
		return _FALSE;
	}	

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));
	
	GDMA_InitStruct->MuliBlockCunt     = 0;
	GDMA_InitStruct->MaxMuliBlock      = 1;
	
	GDMA_InitStruct->GDMA_DIR      = TTFCMemToPeri;
	GDMA_InitStruct->GDMA_DstHandshakeInterface   = UART_DEV_TABLE[UartIndex].Tx_HandshakeInterface;
	GDMA_InitStruct->GDMA_DstAddr = (u32)&UART_DEV_TABLE[UartIndex].UARTx->RB_THR;
	GDMA_InitStruct->GDMA_Index   = 0;
	GDMA_InitStruct->GDMA_ChNum       = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType|TransferType|ErrType);

	GDMA_InitStruct->GDMA_DstMsize  = MsizeFour;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
	GDMA_InitStruct->GDMA_DstInc = NoChange;
	GDMA_InitStruct->GDMA_SrcInc = IncType;

	if (((TxCount & 0x03)==0) && (((u32)(pTxBuf) & 0x03)==0)) {
		/* 4-bytes aligned, move 4 bytes each transfer */
		GDMA_InitStruct->GDMA_SrcMsize   = MsizeOne;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
		GDMA_InitStruct->GDMA_BlockSize = TxCount >> 2;
	} else {
		/* move 1 byte each transfer */
		GDMA_InitStruct->GDMA_SrcMsize   = MsizeFour;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;
		GDMA_InitStruct->GDMA_BlockSize = TxCount;
	}

	assert_param(GDMA_InitStruct->GDMA_BlockSize <= 4096);
	
	GDMA_InitStruct->GDMA_SrcAddr = (u32)(pTxBuf);

	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);
	
	return _TRUE;
}

/**
  * @brief    Init and Enable UART RX GDMA.
  * @param  UartIndex:0/1/3.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains 
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pRxBuf: Rx Buffer.
  * @param  RxCount: Rx Count, 0 will use UART as DMA controller.
  * @retval   TRUE/FLASE
  */
BOOL UART_RXGDMA_Init(
	u8 UartIndex,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pRxBuf,
	int RxCount
	)
{
	u8 GdmaChnl;
	UART_TypeDef* UARTx;
	
	assert_param(GDMA_InitStruct != NULL);
	
	GdmaChnl = GDMA_ChnlAlloc(0, (IRQ_FUN)CallbackFunc, (u32)CallbackData, 12);
	if (GdmaChnl == 0xFF) {
		/* No Available DMA channel */
		return _FALSE;
	}

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	UARTx = UART_DEV_TABLE[UartIndex].UARTx;
	if(RxCount == 0) {
		/*if length is 0, configure uart as the flow controller*/
		GDMA_InitStruct->GDMA_DIR      =  TTFCPeriToMem_PerCtrl;
		UARTx->MISCR |= RUART_RXDMA_OWNER;
	} else {
		/*if length is 0, configure GDMA as the flow controller*/
		GDMA_InitStruct->GDMA_DIR      =  TTFCPeriToMem;
		UARTx->MISCR &= (~RUART_RXDMA_OWNER);
	}

	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->GDMA_SrcHandshakeInterface = UART_DEV_TABLE[UartIndex].Rx_HandshakeInterface;
	GDMA_InitStruct->GDMA_SrcAddr = (u32)&UART_DEV_TABLE[UartIndex].UARTx->RB_THR;
	GDMA_InitStruct->GDMA_Index   = 0;
	GDMA_InitStruct->GDMA_ChNum       = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType|TransferType|ErrType);
	GDMA_InitStruct->GDMA_SrcMsize   = MsizeFour;
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;
	GDMA_InitStruct->GDMA_DstInc = IncType;
	GDMA_InitStruct->GDMA_SrcInc = NoChange;

	if (((u32)(pRxBuf) & 0x03)==0) {
		/*  4-bytes aligned, move 4 bytes each DMA transaction */
		GDMA_InitStruct->GDMA_DstMsize   = MsizeOne;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
	} else {
		/*  move 1 byte each DMA transaction */
		GDMA_InitStruct->GDMA_DstMsize   = MsizeFour;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
	}
	GDMA_InitStruct->GDMA_BlockSize = RxCount;
	GDMA_InitStruct->GDMA_DstAddr = (u32)(pRxBuf);

	assert_param(GDMA_InitStruct->GDMA_BlockSize <= 4096);
	
	/* multi block close */
	GDMA_InitStruct->MuliBlockCunt     = 0;
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->MaxMuliBlock = 1;

	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);
	
	return _TRUE;
}

/**
  * @brief  configure uart monitor parameters.
  * @param  BitNumThres: Configure bit number threshold of one monitor period.
  * @param  OscPerbitUpdCtrl: the OSC cycnum_perbit update bit(can be ENABLE or DISABLE).  
  * @retval None
  */
void UART_MonitorParaConfig(UART_TypeDef* UARTx, u32 BitNumThres, u32 OscPerbitUpdCtrl)
{
	u32 RegValue = 0;

	/*check the parameters*/
	assert_param((BitNumThres>0) && (BitNumThres<128));

	/* Step 1: Reset Low Power Rx Path */
	UARTx->RX_PATH &= ~RUART_REG_LP_RX_PATH_RESET;

	RegValue = UARTx->MON_BAUD_CTRL;

	/*step 2: Configure bit number threshold of one monitor period.*/
	RegValue &= (~ RUART_LP_RX_BIT_NUM_THRES);
	RegValue |= ((BitNumThres << 1) & RUART_LP_RX_BIT_NUM_THRES);

	/*step 3: Configure the OSC cycnum_perbit update bit (REG_MON_BAUD_CTRL[29])*/	
	if(OscPerbitUpdCtrl != DISABLE){
		/*enable OSC cycnum_perbit update*/
		RegValue |= RUART_LP_RX_OSC_UPD_IN_XTAL;
	} else {
		/*disable OSC cycnum_perbit update*/
		RegValue &= ~ RUART_LP_RX_OSC_UPD_IN_XTAL;
	}
	UARTx->MON_BAUD_CTRL = RegValue;
}

/**
  * @brief    set uart baud rate of low power rx path
  * @param  UARTx: where x can be 0/1/3.
  * @param  BaudRate: the desired baud rate
  * @param  RxIPClockHz: the uart rx clock. unit: [Hz]
  * @note    according to the baud rate calculation formlula in low power rx path, method 
  *              implemented is as follows:
  *                                 - CycPerbit = round( fpclock/BaudRate)
  * @retval  None
  */
void
UART_LPRxBaudSet(UART_TypeDef* UARTx, u32 BaudRate, u32 RxIPClockHz)
{
	u32 CycPerbit = 0;
	u32 RegValue = 0;
	u32 RegOscBaud = 0;

	/*check the parameters*/
	assert_param((BaudRate > 0 && BaudRate <= 500000));

	/*Calculate the r_cycnum_perbit field of REG_ MON_BAUD_STS, 
	   according to clock and the desired baud rate*/
	if((RxIPClockHz % BaudRate) >= (BaudRate + 1) / 2){
		CycPerbit = RxIPClockHz / BaudRate + 1;
	} else {
		CycPerbit = RxIPClockHz / BaudRate;
	}

	/* Average clock cycle number of one bit. MON_BAUD_STS[19:0] */
	RegValue = UARTx->MON_BAUD_STS;
	RegValue &= (~ RUART_LP_RX_XTAL_CYCNUM_PERBIT);
	RegValue |= (CycPerbit & RUART_LP_RX_XTAL_CYCNUM_PERBIT);

	/* set CycPerbit */
	UARTx->MON_BAUD_STS = RegValue;

	/* Average clock cycle number of one bit OSC. REG_MON_BAUD_CTRL[28:9] */
	RegOscBaud = UARTx->MON_BAUD_CTRL;
	RegOscBaud &= (~ RUART_LP_RX_OSC_CYCNUM_PERBIT);
	RegOscBaud |= ((CycPerbit << 9) & RUART_LP_RX_OSC_CYCNUM_PERBIT);

	/*set the OSC CycPerbit*/
	UARTx->MON_BAUD_CTRL = RegOscBaud;

	UARTx->RX_PATH &= (~RUART_REG_RX_XFACTOR_ADJ);
}

/**
  * @brief    enable or disable the monitor function in Low Power Rx Path( REG_MON_BAUD_CTRL[0] ).
  * @param  UARTx: where x can be 0/1/3.
  * @param  NewState: the new state of monitoring.
  *              This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void
UART_RxMonitorCmd(UART_TypeDef* UARTx, u32 NewState)
{
	/* configure Low Power rx monitoring function*/
	if(NewState != DISABLE ){
		/* Function enable of monitoring rx baud */
		UARTx->MON_BAUD_CTRL |= RUART_LP_RX_MON_ENABLE;
	} else {
		/* Function disable of monitoring rx baud */
		UARTx->MON_BAUD_CTRL &= (~ RUART_LP_RX_MON_ENABLE);
	}
}

/**
  * @brief    Get the Monitor Baud Rate control register value of the Low Power Rx Path.
  * @param  UARTx: where x can be 0/1/3.
  * @note    MON_BAUD_CTRL[28:9] field value is the monitor result under OSC 2M Hz Clock. 
  * @retval   the value of the MON_BAUD_CTRL register
  */
u32
UART_RxMonBaudCtrlRegGet(UART_TypeDef* UARTx)
{
	/*return the monitor baud rate control register value(MON_BAUD_CTRL)*/
	return UARTx->MON_BAUD_CTRL;
}

/**
  * @brief    Get the status of the Low Power Rx Monitoring.
  * @param  UARTx: where x can be 0/1/3.
  * @note    Read this register can clear the monitor interrupt status.
  *              Besides,REG_MON_BAUD_STS[19:0] field value is the monitor 
  *              result under XTAL 2M Hz Clock.
  * @retval   the value of the REG_MON_BAUD_STS register
  */
u32
UART_RxMonitorSatusGet(UART_TypeDef* UARTx)
{
	/*return the monitor status register value*/
	return UARTx->MON_BAUD_STS;
}

/**
  * @brief    Fills each IrDA_InitStruct member with its default value.
  * @param  IrDA_InitStruct : pointer to a IrDA_InitTypeDef
  *              structure which will be initialized.
  * @retval   None
  */
void
UART_IrDAStructInit(IrDA_InitTypeDef * IrDA_InitStruct)
{
	/* Set the default value */
	IrDA_InitStruct->UART_IrDARxInv = DISABLE;
	IrDA_InitStruct->UART_IrDATxInv = DISABLE;
	IrDA_InitStruct->UART_LowShift = UART_IRDA_PULSE_LEFT_SHIFT;
	IrDA_InitStruct->UART_LowShiftVal = 0;
	IrDA_InitStruct->UART_UpperShift = UART_IRDA_PULSE_LEFT_SHIFT;
	IrDA_InitStruct->UART_UpperShiftVal = 0;
	IrDA_InitStruct->UART_RxFilterCmd = ENABLE;
	IrDA_InitStruct->UART_RxFilterThres = 7;
}

/**
  * @brief    Configures the UART's IrDA interface .
  * @param  UARTx: where x can be 0/1/3.
  * @param  IrDA_InitStruct: pointer to a IrDA_InitTypeDef structure that contains
  *              the configuration information for the IrDA module.
  * @note    the details of IrDA_InitStruct members are:
  * @verbatim 
  *	           IrDA_InitStruct-> UART_IrDARxInv:
  *				IrDA Rx invert bit--------------------MISCR[14]  
  *                 		ENABLE: invert irda rx
  *                 		DISABLE: not invert irda rx
  *
  *	           IrDA_InitStruct->UART_IrDATxInv:
  *				IrDA Tx invert bit---------------------MISCR[13]
  *                 		ENABLE: invert irda tx
  *                 		DISABLE: not invert irda tx
  *
  *	           IrDA_InitStruct->UART_UpperShift:
  *			 	Upperbound(right edge) Shift direction---TXPLSR[31]
  *			 		UART_IRDA_PULSE_LEFT_SHIFT: shift left
  *                 		UART_IRDA_PULSE_RIGHT_SHIFT: shift right
  *	           IrDA_InitStruct->UpperShiftVal:
  *				Upperbound Shift value---------------TXPLSR[30:16]
  *
  *	           IrDA_InitStruct->UART_LowShift:
  *				Lowbound(left edge) Shift direction-----TXPLSR[15]
  *			 		UART_IRDA_PULSE_LEFT_SHIFT: shift left
  *                 		UART_IRDA_PULSE_RIGHT_SHIFT: shift right
  *	           IrDA_InitStruct->UART_LowShiftVal:
  *				Lowbound Shift value----------------TXPLSR[14:0]
  *
  *	           IrDA_InitStruct->UART_RxFilterThres:
  *				IrDA RX filter threshold---------------RXPLSR[15:1]
  *
  *	           IrDA_InitStruct->UART_RxFilterCmd:
  *				IrDA RX filter enable or disable---------RXPLSR[0]
  *                 		ENABLE: enable IrDA rx filter
  *                 		DISABLE: disable IrDA rx filter
  * @endverbatim
  * @retval   None
  */
void
UART_IrDAInit(UART_TypeDef* UARTx, IrDA_InitTypeDef * IrDA_InitStruct)
{
	u32 TempMiscr;
	u32 TempTxpulse;
	u32 TempRxPulse;

	/* Check the parameters */
	assert_param(IrDA_InitStruct->UART_UpperShiftVal <= 0x7fff );
	assert_param(IrDA_InitStruct->UART_LowShiftVal <= 0x7fff);
	assert_param(IS_IRDA_PUL_SHIFT(IrDA_InitStruct->UART_LowShift));
	assert_param(IS_IRDA_PUL_SHIFT(IrDA_InitStruct->UART_UpperShift));							
	assert_param((IrDA_InitStruct->UART_RxFilterThres <= 0x7fff));
	
	/*Get the MISCR register value*/
	TempMiscr = UARTx->MISCR;
	
	/*configure the IrDA RX invert bit*/
	if(IrDA_InitStruct->UART_IrDARxInv != DISABLE){
		/*invert the irda_rx_i*/
		TempMiscr |= RUART_IRDA_RX_INVERT;
	} else {
		/*not invert the irda_rx_i*/
		TempMiscr &= (~ RUART_IRDA_RX_INVERT);
	}

	/*configure the IrDA TX invert bit*/
	if(IrDA_InitStruct->UART_IrDATxInv != DISABLE){
		/*invert the irda_tx_o*/
		TempMiscr |= RUART_IRDA_TX_INVERT;
	} else {
		/*not invert the irda_tx_o*/
		TempMiscr &= (~ RUART_IRDA_TX_INVERT);
	}
	UARTx->MISCR = TempMiscr;

	/*Get the TXPLSR register value*/
	TempTxpulse = UARTx->TXPLSR;
	
	/*configure IrDA SIR TX Pulse Width*/
	/*configure Upperbound(right edge) shift direction*/
	TempTxpulse &= (~ RUART_IRDA_TX_PUL_UP_BUND_SHIFT);
	TempTxpulse |= (IrDA_InitStruct->UART_UpperShift << 31);

	/*configure the Upperbound shift value*/
	TempTxpulse &= (~ RUART_IRDA_TX_PUL_UP_BUND_VAL);
	TempTxpulse |= (IrDA_InitStruct->UART_UpperShiftVal << 16);

	/*configure Lowbound(left edge) shift direction*/
	TempTxpulse &= (~ RUART_IRDA_TX_PUL_LOW_BUND_SHIFT);
	TempTxpulse |= (IrDA_InitStruct->UART_LowShift << 15);

	/*configure the Lowbound shift value*/
	TempTxpulse &= (~ RUART_IRDA_TX_PUL_LOW_BUND_VAL);
	TempTxpulse |= (IrDA_InitStruct->UART_LowShiftVal);	

	UARTx->TXPLSR = TempTxpulse;

	/*Get the RXPLSR register value*/
	TempRxPulse = UARTx->RXPLSR;

	/*configure IrDA RX filter threshold*/
	TempRxPulse &= (~ RUART_IRDA_RX_FILTER_THRES);
	TempRxPulse |= (IrDA_InitStruct->UART_RxFilterThres << 1);
	
	if(IrDA_InitStruct->UART_RxFilterCmd != DISABLE){
		/*enable IrDA rx filter*/
		TempRxPulse |= RUART_IRDA_RX_FILTER_ENABLE;
	} else {
		/*disable IrDA rx filter*/
		TempRxPulse &= (~ RUART_IRDA_RX_FILTER_ENABLE);
	}

	UARTx->RXPLSR = TempRxPulse;
}

/**
  * @brief    enable or disable the IrDA function.
  * @param  UARTx: where x can be 0/1/3.
  * @param  NewState: the new state of the IrDA.
  *              This parameter can be: ENABLE or DISABLE.
  * @retval   None
  */
void
UART_IrDACmd(UART_TypeDef* UARTx, u32 NewState)
{
	if(NewState != DISABLE ){
		/* enable the IrDA */
		UARTx->MISCR |= RUART_IRDA_ENABLE;
	} else {
		/* disable the IrDA */
		UARTx->MISCR &= (~ RUART_IRDA_ENABLE);
	}
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
