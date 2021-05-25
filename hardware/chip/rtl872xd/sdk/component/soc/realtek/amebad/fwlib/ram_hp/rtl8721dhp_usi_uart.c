/**
  ******************************************************************************
  * @file    rtl8721dhp_usi_uart.c
  * @author
  * @version V1.0.0
  * @date    2017-09-30
  * @brief   This file contains all the functions prototypes for the USI UART firmware
  *             library, including the following functionalities of the Universal Asynchronous 
  *             Receiver/Transmitter (UART) peripheral:
  *           - Initialization
  *           - Clear UART TX/RX FIFO
  *           - Uart Rx Path Control (disable/enable)
  *           - Baud Rate Setting
  *           - Receive/Send Data Interface
  *           - Uart Low Power Rx Initialization
  *           - Uart Low Power Rx Monitor Function Initialization
  *           - Low Power Rx Baud Rate Setting
  *           - Uart IrDA Function Initialization
  *           - Uart Rx Byte Counter Interface
  *           - Interrupts and flags management
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
#include "rtl8721d_usi_uart.h"

const USI_DevTable USI_DEV_TABLE[1] = {
		{USI0_DEV, GDMA_HANDSHAKE_INTERFACE_USI0_TX, GDMA_HANDSHAKE_INTERFACE_USI0_RX, USI_IRQ}
};

u32 USI_UART_StateTx[1];
u32 USI_UART_StateRx[1];

USI_UARTCFG_TypeDef usi_uart_config[2]=
{
	/* USI0-UART */
	{
		.LOW_POWER_RX_ENABLE = DISABLE, /*Enable low power RX*/
	},	
	/* USI1-UART */
	{
		.LOW_POWER_RX_ENABLE = DISABLE,
	},
};

/**
  * @brief  Deinitializes the USIx peripheral registers to their default reset values in USI UART mode.
  * @param  USIx: where x can be 0.
  * @retval None
  */
void USI_UARTDeInit(USI_TypeDef* USIx)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	/*reset USI all logic*/
	USIx->SW_RESET &=(~USI_SW_RESET_RSTB);
	USIx->SW_RESET |=USI_SW_RESET_RSTB;

	/*disable USI UART*/
	USIx->USI_MODE_CTRL &= (~USI_SERIAL_MODE);

	/*clear all USI UART interrupts*/
	USIx->INTERRUPT_ALL_CLR = USI_INT_ALL_CLEAR;

	/*disable all interrupts*/
	USIx->INTERRUPT_ENABLE = 0x00000000;
}

/**
  * @brief  Fills each USI_UARTInitStruct member with its default value.
  * @param  USI_UARTInitStruct: pointer to an USI_UARTInitTypeDef structure which will be initialized.
  * @retval   None
  */
 void USI_UARTStructInit(USI_UARTInitTypeDef* USI_UARTInitStruct)
{
	/* USI_UARTInitStruct members default value */
	USI_UARTInitStruct->USI_UARTParity         = USI_RUART_PARITY_ENABLE;
	USI_UARTInitStruct->USI_UARTParityType = USI_RUART_ODD_PARITY;
	USI_UARTInitStruct->USI_UARTStickParity = USI_RUART_STICK_PARITY_DISABLE;
	USI_UARTInitStruct->USI_UARTStopBit       = USI_RUART_STOP_BIT_1;
	USI_UARTInitStruct->USI_UARTWordLen        = USI_RUART_WLS_8BITS;
	USI_UARTInitStruct->USI_UARTRxFifoTrigLevel = 1;
	USI_UARTInitStruct->USI_UARTDmaModeCtrl = ENABLE;
	USI_UARTInitStruct->USI_UARTFlowControl   = DISABLE;
	USI_UARTInitStruct->USI_UARTTxFifoTrigLevel = 0x40;
	USI_UARTInitStruct->USI_UARTFlwCtrlRxHoldThd = 0x04;
}

/**
  * @brief    Initializes the USIx peripheral according to the specified
  *              parameters in the USI_UARTInitStruct.
  * @param  USIx: where x can be 0.
  * @param  USI_UARTInitStruct: pointer to a USI_UARTInitTypeDef structure that contains
  *              the configuration information for the specified USART peripheral. 
  * @retval None
  */
 void USI_UARTInit(USI_TypeDef* USIx, USI_UARTInitTypeDef *USI_UARTInitStruct)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));
	assert_param(USI_UARTInitStruct->USI_UARTRxFifoTrigLevel <= 0x40);
	assert_param(IS_USI_UART_PARITY_ENABLE(USI_UARTInitStruct->USI_UARTParity));
	assert_param(IS_USI_UART_PARITY_TYPE(USI_UARTInitStruct->USI_UARTParityType));
	assert_param(IS_USI_UART_STICK_PARITY_ENABLE(USI_UARTInitStruct->USI_UARTStickParity));
	assert_param(IS_USI_UART_STOP_BIT(USI_UARTInitStruct->USI_UARTStopBit));	
	assert_param(IS_USI_UART_WLS(USI_UARTInitStruct->USI_UARTWordLen));	
	assert_param(USI_UARTInitStruct->USI_UARTTxFifoTrigLevel <= 0x40);
	assert_param(USI_UARTInitStruct->USI_UARTFlwCtrlRxHoldThd <= 0x40);

	/*reset USI all logic, this step will clear tx/rx fifo*/
	USIx->SW_RESET &=(~USI_SW_RESET_RSTB);
	USIx->SW_RESET |=USI_SW_RESET_RSTB;

	/*clear rx fifo and release rx fifo logic*/
	USIx->SW_RESET &= (~USI_SW_RESET_RXFIFO_RSTB);
	USIx->SW_RESET |= USI_SW_RESET_RXFIFO_RSTB;

	/*clear tx fifo and release tx fifo logic*/
	USIx->SW_RESET &= (~USI_SW_RESET_TXFIFO_RSTB);
	USIx->SW_RESET |= USI_SW_RESET_TXFIFO_RSTB;

	/*disable USI all mode, configure USI with UART mode*/
	USIx->USI_MODE_CTRL &= (~USI_SERIAL_MODE);
	USIx->USI_MODE_CTRL |= USI_SERIAL_UART_MODE;

	/*reset RX path*/
	USIx->SW_RESET &= (~USI_SW_RESET_RX_RSTB);

	/*release TX path*/
	USIx->SW_RESET |= USI_SW_RESET_TX_RSTB;

	/*disable all interrupts*/
	USIx->INTERRUPT_ENABLE = 0x00000000;

	/*clear all USI UART interrupts*/
	USIx->INTERRUPT_ALL_CLR = USI_INT_ALL_CLEAR;

	/*configure RX fifo trigger level*/
	USIx->RX_FIFO_CTRL &= (~USI_RXFIFO_ALMOST_FULL_TH);
	USIx->RX_FIFO_CTRL |= USI_UARTInitStruct->USI_UARTRxFifoTrigLevel;

	/*configure TX fifo trigger level*/
	USIx->TX_FIFO_CTRL &= (~USI_TXFIFO_ALMOST_EMPTY_TH);
	USIx->TX_FIFO_CTRL |= USI_UARTInitStruct->USI_UARTTxFifoTrigLevel;

	/*set auto flow control RTS hold level*/
	USIx->UART_FLOW_CTRL &= (~USI_UART_RX_HOLD_THRD);
	USIx->UART_FLOW_CTRL |= USI_UARTInitStruct->USI_UARTFlwCtrlRxHoldThd<<8;

	/* Configure FlowControl */
	if (USI_UARTInitStruct->USI_UARTFlowControl == ENABLE) {
		USIx->UART_FLOW_CTRL |= (USI_UART_AUTO_FLOW_EN|USI_UART_RX_RTS);
	} else {
		USIx->UART_FLOW_CTRL &= (~(USI_UART_AUTO_FLOW_EN|USI_UART_RX_RTS));
	}

	/*configure word length, stop bit, parity, parity type, stickparity*/
	USIx->UART_CTRL &= (~(USI_UART_WORD_LEN_MD|USI_UART_STOP_BIT_MD|USI_UART_PARITY_ENABLE|USI_UART_PARITY_EVEN|USI_UART_STICK_PARITY_BIT));
	USIx->UART_CTRL |= ((USI_UARTInitStruct->USI_UARTWordLen) |
		(USI_UARTInitStruct->USI_UARTStopBit) |
		(USI_UARTInitStruct->USI_UARTParity) |
		(USI_UARTInitStruct->USI_UARTParityType) |
		(USI_UARTInitStruct->USI_UARTStickParity));

	if(USI_UARTInitStruct->USI_UARTDmaModeCtrl  != ENABLE) {
		/* disable the USI UART TX/RX  DMA */
		USIx->DMA_ENABLE &= ~ (USI_TX_DMA_ENABLE|USI_RX_DMA_ENABLE);
	}
}

/**
  * @brief    get ovsr & ovsr_adj parameters for the given baudrate 
  * @param  USIx: where x can be 0.
  * @param  baudrate: the desired baudrate
  * @param  ovsr: the pointer to ovsr parameter
  * @param  ovsr_adj: the pointer to ovsr_adj parameter 
  * @retval   searching status: 
  *              - 1: found
  *              - 0: not found
  */
 u32 USI_UARTBaudParaGet(u32 baudrate, u32 *ovsr, u32 *ovsr_adj)
{
	USI_UARTBaudParaGetFull(XTAL_ClkGet(), baudrate, ovsr, ovsr_adj);
	return 1;
}

/**
  * @brief    get ovsr & ovsr_adj parameters according to the given baudrate and UART IP clock.
  * @param  USIx: where x can be 0.
  * @param  IPclk: the given UART IP clock, Unit: [ Hz ]
  * @param  baudrate: the desired baudrate, Unit: bps[ bit per second ]
  * @param  ovsr: integer parameter related to the desired baud rate
  * @param  ovsr_adj: fractional parameter related to the desired baud rate
  * @retval  None
  */
void USI_UARTBaudParaGetFull(u32 IPclk, u32 baudrate, u32 *ovsr, u32 *ovsr_adj)
{
	u32 i;
	u32 Remainder;
	u32 TempAdj = 0;
	u32 TempMultly;

	/*obtain the ovsr parameter*/
	*ovsr = IPclk / baudrate;

	/*get the remainder related to the ovsr_adj parameter*/
	Remainder = IPclk % baudrate;

	/*calculate the ovsr_adj parameter*/
	for(i = 0; i < 11; i++){
		TempAdj = TempAdj << 1;
		TempMultly = (Remainder * (12-i));
		TempAdj |= ((TempMultly / baudrate - (TempMultly - Remainder) / baudrate) ? 1 : 0);
	}

	/*obtain the ovsr_adj parameter*/
	*ovsr_adj = TempAdj;
}

/**
  * @brief    Set Uart Baud Rate use baudrate val.
  * @param  USIx: where x can be 0.
  * @param  BaudRate: Baud Rate Val, like 115200 (unit is HZ).
  * @retval  None
  */
void USI_UARTSetBaud(USI_TypeDef* USIx, u32 BaudRate)
{
	u32 Ovsr;
	u32 Ovsr_adj;
	
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));
	
	/* get baud rate parameter based on baudrate */
	USI_UARTBaudParaGetFull(XTAL_ClkGet(), BaudRate, &Ovsr, &Ovsr_adj);

	/*confgure TX path baudrate*/
	USIx->UART_TX_FRACTION_BAUD &=(~(USI_UART_TX_XFACTOR|USI_UART_TX_XFACTOR_ADJ));
	USIx->UART_TX_FRACTION_BAUD |= (Ovsr|(Ovsr_adj<<20));

	/*configure RX path baudrate*/
	USIx->UART_RX_BAUD_XTAL &= (~USI_RUART_LPRX_XTAL_CYCNUM_PERBIT);
	USIx->UART_RX_BAUD_XTAL |= Ovsr;
	USIx->UART_RX_FRACTION_BAUD_CTRL &= (~USI_UART_RX_XFACTOR_ADJ);
	USIx->UART_RX_FRACTION_BAUD_CTRL |= Ovsr_adj;
}
                         
/**
  * @brief    Set Uart Baud Rate use baudrate parameter Ovsr & Ovsr_adj.
  * @param  USIx: where x can be 0.
  * @param  Ovsr: Specifies the ovsr parameter related to baud rate.
  *				This parameter is caculated by using uart IP clock and the desired baud rate
  * @param  Ovsr_adj: Specifies the ovsr_adj parameter related fractional part for the desired baud rate.
  *				This parameter is also caculated by using uart IP clock and the desired baud rate
  * @retval  None
  */
 void USI_UARTSetBaudExt(USI_TypeDef* USIx, u32 Ovsr, u32 Ovsr_adj)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	/*confgure TX path baudrate*/
	USIx->UART_TX_FRACTION_BAUD &=(~(USI_UART_TX_XFACTOR|USI_UART_TX_XFACTOR_ADJ));
	USIx->UART_TX_FRACTION_BAUD |= (Ovsr|(Ovsr_adj<<20));

	/*configure RX path baudrate*/
	USIx->UART_RX_BAUD_XTAL &= (~USI_RUART_LPRX_XTAL_CYCNUM_PERBIT);
	USIx->UART_RX_BAUD_XTAL |= Ovsr;
	USIx->UART_RX_FRACTION_BAUD_CTRL &= (~USI_UART_RX_XFACTOR_ADJ);
	USIx->UART_RX_FRACTION_BAUD_CTRL |= Ovsr_adj;
}

/**
  * @brief    to set the RX FIFO level to trigger RX interrupt/RTS de-assert
  * @param  USIx: where x can be 0.
  * @param  FifoLv: USI UART RX FIFO level, this value can be 1 to 62.
  */	
void USI_UARTSetRxLevel(USI_TypeDef* USIx, u32 FifoLv) 
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	/*configure RX interrupt trigger level*/
	USIx->RX_FIFO_CTRL &= (~USI_RXFIFO_ALMOST_FULL_TH);
	USIx->RX_FIFO_CTRL |= FifoLv;

	/*configure RX CTS hold level*/
	USIx->UART_FLOW_CTRL &=(~USI_UART_RX_HOLD_THRD);
	USIx->UART_FLOW_CTRL |= (FifoLv<<8);
}

/**
  * @brief    enable or disable USI Uart Rx Path
  * @param  USIx: where x can be 0.
  * @param  NewState: the new state of the Rx Path.
  *              This parameter can be: ENABLE or DISABLE.
  * @note    Because the TX/RX of the IrDA transceiver are in the same module, 
  *              RX fifo can also receive available data when in the process of TX.Therefore,
  *              the RX Path must be disabled when TX is going on.Namely, IrDA must work
  *              in the Half duplex mode, though normal UART IP supports Full duplex.
  * @retval None
  */				
void USI_UARTRxCmd(USI_TypeDef* USIx, u32 NewState)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	if (NewState != DISABLE) {
		/*enable uart receiver*/
		USIx->SW_RESET |= USI_SW_RESET_RX_RSTB;
	} else {
		/*disable uart receiver*/
		USIx->SW_RESET &= (~USI_SW_RESET_RX_RSTB);
	}
}

  /**
  * @brief    check if write data to tx fifo.
  * @param  USIx: where x can be 0.
  * @retval   status value:
  *              - 1: tx fifo is empty and can write data to tx fifo
  *              - 0: tx fifo is not empty  
  */
 u32 USI_UARTWritable(USI_TypeDef* USIx)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	/*check if tx fifo is empty or not. empty return 1 and not empty return 0.*/
	if (USIx->TX_FIFO_STATUS & USI_TXFIFO_EMPTY) {
		return 1;
	}
	else {
		return 0;
	}
}

  /**
  * @brief    check if there is data in rx fifo.
  * @param  USIx: where x can be 0.
  * @retval   status value:
  *              - 1: rx fifo has valid data and can read data from rx fifo
  *              - 0: rx fifo is empty
  */
u32 USI_UARTReadable(USI_TypeDef* USIx)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	/*check if rx fifo has available data. 
	available data return 1 and no available data return 0.*/
	if (!(USIx->RX_FIFO_STATUS & USI_RXFIFO_EMPTY)) {
		return 1;
	}
	else {
		return 0;
	}
}

/**
  * @brief  Transmits single data through the USIx peripheral.
  * @param  USIx: where x can be 0.
  * @param  TxData: the data to transmit.
  * @retval None
  */
 void USI_UARTCharPut(USI_TypeDef* USIx, u8 TxData)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	/*write one byte to tx fifo*/
	USIx->TX_FIFO_WRITE = TxData;
}

/**
  * @brief  Get the received data by the USIx peripheral.
  * @param  USIx: where x can be 0.
  * @param  pRxByte: the pointer to received data.
  * @retval   None.
  */
void USI_UARTCharGet(USI_TypeDef* USIx, u8  *pRxByte)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	/*read one byte from rx fifo*/
	*pRxByte = (u8)USIx->RX_FIFO_READ;
}

/**
  * @brief  Receive data from rx FIFO, poll USI_UARTReadable.
  * @param  USIx: selected UART peripheral, where x can be 0.
  * @param[out]  outBuf: buffer to save data read from UART FIFO.
  * @param  count: number of data to be read.
  * @retval none
  */
void USI_UARTReceiveData(
	USI_TypeDef* USIx,
	u8* OutBuf,
	u32 Count
	)
{
	u32 cnt = 0;

	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));
		
	for(cnt = 0; cnt < Count; cnt++) {
		while (USI_UARTReadable(USIx) == 0);

		*OutBuf++ = (u8)USIx->RX_FIFO_READ;
	}
}

/**
  * @brief  Send data to tx FIFO, poll USI_UARTWritable.
  * @param  USIx: selected UART peripheral, where x can be 0.
  * @param  inBuf: buffer to be written to Tx FIFO.
  * @param  count: number of data to be written.
  * @retval none
  */
void USI_UARTSendData(
	USI_TypeDef* USIx,
	u8* InBuf,
	u32 Count
	)
{
	u32 cnt = 0;

	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));
	
	for(cnt = 0; cnt < Count; cnt++) {
		while (USI_UARTWritable(USIx) == 0);
		
		USIx->TX_FIFO_WRITE = *InBuf++;
	}
}

/**
  * @brief  Receive data from rx FIFO, with timeout.
  * @param  USIx: selected UART peripheral, where x can be 0.
  * @param[out]  outBuf: buffer to save data read from UART FIFO.
  * @param  count: number of data to be read.
  * @param  Times: poll USI_UARTReadable times before timeout.
  * @retval transfer len
  */
u32 USI_UARTReceiveDataTO(
	USI_TypeDef* USIx,
	u8* OutBuf,
	u32 Count,
	u32 Times
	)
{
	u32 cnt = 0;
	u32 polltimes = 0;

	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	while (1) {
		if (USI_UARTReadable(USIx)) {
			*(OutBuf + cnt) = (u8)USIx->RX_FIFO_READ;
			cnt++;
			polltimes = 0;

			if (cnt >= Count) {
				break;
			}
		} else {
			polltimes++;

			/* time out */
			if (polltimes > Times) {
				break;
			}
		}
	}

	return (cnt);
}

/**
  * @brief  Send data to tx FIFO, with timeout.
  * @param  USIx: selected UART peripheral, where x can be 0.
  * @param  InBuf: buffer to be written to Tx FIFO.
  * @param  Count: number of data to be written.
  * @param  Times: poll USI_UARTWritable times before timeout.
  * @retval transfer len
  */
u32 USI_UARTSendDataTO(
	USI_TypeDef* USIx,
	u8* InBuf,
	u32 Count,
	u32 Times
	)
{
	u32 cnt = 0;
	u32 polltimes = 0;

	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	while (1) {
		if (USI_UARTWritable(USIx)) {
			USIx->TX_FIFO_WRITE = *(InBuf + cnt);
			cnt++;
			polltimes = 0;

			if (cnt >= Count) {
				break;
			}
		} else {
			polltimes++;

			/* time out */
			if (polltimes > Times) {
				break;
			}
		}
	}

	return (cnt);
}

  /**
  * @brief    Clear Rx Byte Counter.
  * @param  USIx: where x can be 0.
  * @retval   None
  */
void USI_UARTRxByteCntClear(USI_TypeDef* USIx)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	/*write 1 to clear*/
	USIx->RX_FIFO_RD_CNT |= USI_RX_FIFO_RD_CNT_CLR;
}

/**
  * @brief    Get the Rx Byte Counter Value.
  * @param  USIx: where x can be 0.
  * @note    Get the Rx Byte Counter Value
  * @note    this counter will increase 1 for each reading the rx fifo, no matter rx fifo is empty or not.
  * @retval   the value of Rx Byte Counter
  */
u32 USI_UARTRxByteCntGet(USI_TypeDef* USIx)
{
	u32 RegValue;

	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	/*Read the REG_RX_BYTE_CNT register value*/
	RegValue = USIx->RX_FIFO_RD_CNT;

	/*Get the rx byte counter field (REG_RX_BYTE_CNT[15:0]) value*/
	RegValue &= USI_RX_FIFO_RD_CNT;

	return RegValue;
}

  /**
  * @brief    enable or disable UART break contol function .
  * @param  USIx: where x can be 0.
  * @param  NewState: This parameter can be: ENABLE or DISABLE.
  * @retval   None
  */
void USI_UARTBreakCtl(USI_TypeDef* USIx, u32 NewState)
{
	u32 RegValue;

	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	/*get the Line Control Register(LCR) value*/
	RegValue = USIx->UART_CTRL;

	if(NewState == ENABLE)
		/*enable break control*/
		RegValue |= USI_UART_BREAK_CTRL;
	else
		/*disable break control*/
		RegValue &= (~ USI_UART_BREAK_CTRL);

	USIx->UART_CTRL = RegValue;
}

/**
  * @brief    Clear Rx fifo
  * @param  USIx: where x can be 0.
  * @retval   None
  */
u32 USI_UARTClearRxFifo(USI_TypeDef* USIx)
{
	u32  Temp;
	u32  WaitTime = 5;

	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	/*write 0 to clear rx fifo*/
	USIx->SW_RESET &= (~USI_SW_RESET_RXFIFO_RSTB);

	while(WaitTime > 0) {
		/*check Rx fifo if empty or not*/
		Temp = USIx->RX_FIFO_STATUS & USI_RXFIFO_EMPTY;
		if (Temp != 0)
			break;

		WaitTime--;
	}

	USIx->SW_RESET |= USI_SW_RESET_RXFIFO_RSTB;

	if (Temp != 0)
		return _TRUE;
	else
		return _FALSE;
}


/**
  * @brief    Clear Tx fifo .
  * @param  USIx: where x can be 0.
  * @retval   None
  */
void USI_UARTClearTxFifo(USI_TypeDef* USIx)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	/*clear tx fifo*/
	USIx->SW_RESET &= (~USI_SW_RESET_TXFIFO_RSTB);
	USIx->SW_RESET |= USI_SW_RESET_TXFIFO_RSTB;	
}

/**
  * @brief    USI UART get valid entry count in RX FIFO .
  * @param  USIx: where x can be 0.
  * @retval   None
  */
u32 USI_UARTGetRxFifoValidCnt(USI_TypeDef* USIx)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	/*acquire RX FIFO valid count*/
	return (u32)((USIx->RX_FIFO_STATUS & USI_RXFIFO_VALID_CNT)>>8);
}

/**
  * @brief    USI UART get empty entry count in TX FIFO .
  * @param  USIx: where x can be 0.
  * @retval   None
  */
u32 USI_UARTGetTxFifoEmptyCnt(USI_TypeDef* USIx)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	/*acquire TX FIFO empty count*/
	return (u32)((USIx->TX_FIFO_STATUS & USI_TXFIFO_EMPTY_SPACE)>>8);
}

/**
  * @brief    Enables or disables the specified USIx UART interrupts.
  * @param  USIx: where x can be 0.
  * @param  USI_UARTIT: specifies the USIx interrupt sources to be enabled or disabled.
  *          This parameter can be one or combinations of the following values:
  *            @arg USI_TX_FIFO_ALMOST_EMPTY_INTER:  tx fifo almost empty interrupt
  *            @arg USI_TX_FIFO_OVERFLOW_INTER: tx fifo overflow interrupt
  *            @arg USI_RX_FIFO_ALMOST_FULL_INTER:   rx fifo almost full interrupt
  *            @arg USI_RX_FIFO_OVERFLOW_INTER: rx fifo overflow interrupt
  *            @arg USI_RX_FIFO_UNDERFLOW_INTER: rx fifo underflow interrupt
  *            @arg USI_UART_PARITY_ERROR_INTER:  uart parity error interrupt
  *            @arg USI_UART_STOP_ERROR_INTER: uart stop bit error interrupt
  *            @arg USI_UART_BREAK_INTER:  uart break interrupt
  *            @arg USI_RX_FIFO_TIMEOUT_INTER:   rx fifo timeout interrupt
  *            @arg USI_RX_BAUDMON_DONE_INTER: uart baud rate monitor done interrupt
  *            @arg USI_TX_CTS_CHANGE_INTER: uart CTS change interrupt
  * @param  newState: new state of the specified USIx interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void USI_UARTINTConfig(
        USI_TypeDef* USIx,
        u32 USI_UARTIT,
        u32 newState
        )
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));
	assert_param(IS_USI_UART_IT(USI_UARTIT));

	if (newState == ENABLE) {
		/* Enable the selected USIx interrupts */
		USIx->INTERRUPT_ENABLE |= USI_UARTIT;
	} else {
		/* Disable the selected USIx interrupts */
		USIx->INTERRUPT_ENABLE &= (u32)~USI_UARTIT;
	}
}

/**
  * @brief    get the interrupt identification register value.
  * @param  USIx: where x can be 0.
  * @retval   the interrupt identification register value.
  */
u32 USI_UARTIntStatus(USI_TypeDef* USIx)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	/*return the uart interrupt identification register value*/
	return USIx->INTERRUPT_STATUS;
}

/**
  * @brief    get the raw interrupt register value.
  * @param  USIx: where x can be 0.
  * @retval   the interrupt raw status value.
  */
u32 USI_UARTGetRawIntStatus(USI_TypeDef* USIx)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	/*return the uart interrupt raw status register value*/
	return USIx->RAW_INTERRUPT_STATUS;
}

/**
  * @brief    Clear all USI UART interrupt status.
  * @param  USIx: where x can be 0.
  * @note    Write "1" to this register to clear the combined interrupt, all individual interrupts, 
  *               interrupt status register and raw interrupt status register. This bit does not clear 
  *               hardware clearable interrupts but software dis-clearable interrupts, and the relate register
  *               (include txfifo_almost_empty, rxfifo_almost_full, uart_rxfifo_timeout).
  * @retval   None
  */
void USI_UARTClearAllIntStatus(USI_TypeDef* USIx)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	/*write 1 to clear all interrupts*/
	USIx->INTERRUPT_ALL_CLR = USI_INT_ALL_CLEAR;
}

/**
  * @brief    Clear specified USI UART interrupt pending bits.
  * @param  USIx: where x can be 0.
  * @param  USIUART_IT: specifies the interrupt to be cleared.
  *   This parameter can be any combination of the following values:
  *            @arg USI_TX_FIFO_OVERFLOW_INTER: tx fifo overflow interrupt
  *            @arg USI_RX_FIFO_OVERFLOW_INTER: rx fifo overflow interrupt
  *            @arg USI_RX_FIFO_UNDERFLOW_INTER: rx fifo underflow interrupt
  *            @arg USI_UART_PARITY_ERROR_INTER:  uart parity error interrupt
  *            @arg USI_UART_STOP_ERROR_INTER: uart stop bit error interrupt
  *            @arg USI_UART_BREAK_INTER:  uart break interrupt
  *            @arg USI_RX_BAUDMON_DONE_INTER: uart baud rate monitor done interrupt
  *            @arg USI_TX_CTS_CHANGE_INTER: uart CTS change interrupt
  * @retval   None
  */
void USI_UARTClearIntStatus(USI_TypeDef* USIx, u32 USIUART_IT)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));
	assert_param(IS_USI_UART_CLEAR_IT(USIUART_IT));
	
	/*write 1 to clear all interrupts*/
	USIx->INTERRUPT_STATUS_CLR =  USIUART_IT;
}

/**
  * @brief  wait uart tx complete.
  * @param  USIx: where x can be 0.
  * @retval: none
  */
void USI_UARTWaitBusy(USI_TypeDef* USIx, u32 PollTimes)
{
	u32 times = 0;

	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));
	
	/* Wait for Uart print out */
	while(1) {		
		if (USIx->TX_FIFO_STATUS & USI_TXFIFO_EMPTY){
			break;
		}

		DelayUs(100);

		times++;

		if (times > PollTimes)
			break;
	}
}

/**
  * @brief    USI-UART RX timeout configuration .
  * @param  USIx: where x can be 0.
  * @param  TimeOutCnt: timeout counter, unit: bit period.
  * @retval   None
  */
void USI_UARTRxTimeOutConfig(USI_TypeDef* USIx, u32 TimeOutCnt)
{
	u32 TempReg;

	/* Set rx timeout threshold.*/
	TempReg = USIx->UART_RXFIFO_TO_TH;
	TempReg &= ~(USI_UART_RXFIFO_TO_TH);
	TempReg |= TimeOutCnt;

	USIx->UART_RXFIFO_TO_TH = TempReg;
}

 /**
  * @brief    USI UART RX DMA flow control mode configuration .
  * @param  USIx: where x can be 0
  * @param  Mode: DMA flow control mode.
  			This parameter can be: USI_UART_RX_UART_IS_DMA_FLOW_CTRL or USI_UART_RX_GDMA_IS_DMA_FLOW_CTRL.
  * @retval   None
  */
void USI_UARTRxDMAModeConfig(USI_TypeDef* USIx, u32 Mode)
{
	u32 TempReg;

	assert_param(IS_USI_UART_RX_DMA_MODE(Mode));

	/* Set USI UART RX DMA flow control mode.*/
	TempReg = USIx->UART_RXDMA_FLOW_CTRL;
	TempReg &= ~(USI_UART_RXDMA_RXDMA_OWNER);
	TempReg |= Mode;

	USIx->UART_RXDMA_FLOW_CTRL = TempReg;
}

 /**
  * @brief    USI UART RX DMA dummy data configuration .
  * @param  USIx: where x can be 0~1.
  * @param  Byte: dummy data.
  * @retval   None
  */
void USI_UARTRxDMADummyDataConfig(USI_TypeDef* USIx, u8 Byte)
{
	u32 TempReg;

	/* Set UART RX DMA flow control mode.*/
	TempReg = USIx->UART_RXDMA_FLOW_CTRL;
	TempReg &= ~(USI_UART_RXDMA_DUMMY_DATA);
	TempReg |= (Byte<<8);

	USIx->UART_RXDMA_FLOW_CTRL = TempReg;
}

 /**
  * @brief    Get USI UART RX DMA dummy flag .
  * @param  USIx: where x can be 0.
  * @retval   status value:
		    - 0: master not read dummy data from rx fifo
		    - 1: master read dummy data from rx fifo
  */
u32 USI_UARTGetRxDMADummyFlag(USI_TypeDef* USIx)
{
	if(USIx->UART_RXDMA_FLOW_CTRL & USI_UART_RXDMA_DUMMY_FLAG) {
		return 1;
	} else {
		return 0;
	}
}

 /**
  * @brief    Clear USI UART RX DMA dummy flag .
  * @param  USIx: where x can be 0.
  * @retval   None
  */
void USI_UARTRxClearDMADummyFlag(USI_TypeDef* USIx)
{
	USIx->UART_RXDMA_FLOW_CTRL |= USI_UART_RXDMA_DUMMY_FLAG;
}

  /**
  * @brief    configure USI UART TX DMA burst size .
  * @param  USIx: where x can be 0.
  * @param  TxDmaBurstSize: UART TX DMA burst size.
  * @note     Because UART tx fifo depth is 64 in hardware. 
  *              Therefore this value must be no more than 64.
  * @retval   None
  */
void USI_UARTTXDMAConfig(USI_TypeDef* USIx, u32 TxDmaBurstSize)
{
	u32 Temp;

	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	/* Set UART TX DMA burst size.*/
	Temp = USIx->DMA_REQ_SIZE;
	Temp &= ~(USI_TX_DMA_BURST_SIZE|USI_TX_DMA_SINGLE_SIZE);
	Temp |= (TxDmaBurstSize|(1<<8));

	USIx->DMA_REQ_SIZE = Temp;
}

  /**
  * @brief    configure UART RX DMA burst size .
  * @param  USIx: where x can be 0.
  * @param  RxDmaBurstSize: UART RX DMA burst size.
  * @note     Because UART rx fifo depth is 64 in hardare. 
  *              Therefore this value must be no more than 64.
  * @retval   None
  */
void USI_UARTRXDMAConfig(USI_TypeDef* USIx, u32 RxDmaBurstSize)
{
	u32 Temp;

	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	/* Set UART RX DMA burst size.*/
	Temp = USIx->DMA_REQ_SIZE;
	Temp &= ~(USI_RX_DMA_BURST_SIZE|USI_RX_DMA_BURST_SIZE);
	Temp |= ((RxDmaBurstSize<<16)|(1<<24));

	USIx->DMA_REQ_SIZE = Temp;
}

  /**
  * @brief    enable or disable USI UART TX DMA .
  * @param  USIx: where x can be 0.
  * @param  NewState: the new state of UART TX DMA.
  *               This parameter can be: ENABLE or DISABLE.
  * @retval   None
  */
void USI_UARTTXDMACmd(USI_TypeDef* USIx, u32 NewState)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	if(NewState != DISABLE ){
		/* enable the UART TX DMA */
		USIx->DMA_ENABLE |= USI_TX_DMA_ENABLE;
	} else {
		/* disable the UART TX DMA */
		USIx->DMA_ENABLE &= (~ USI_TX_DMA_ENABLE);
	}
}

  /**
  * @brief    enable or disable USI UART RX DMA .
  * @param  USIx: where x can be 0.
  * @param  NewState: the new state of UART RX DMA.
  *              This parameter can be: ENABLE or DISABLE.
  * @retval   None
  */
void USI_UARTRXDMACmd(USI_TypeDef* USIx, u32 NewState)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	if(NewState != DISABLE ){
		/* enable the UART RX DMA */
		USIx->DMA_ENABLE |= USI_RX_DMA_ENABLE;
	} else {
		/* disable the UART RX DMA */
		USIx->DMA_ENABLE &= (~ USI_RX_DMA_ENABLE);
	}
}

/**
  * @brief    Init and Enable USI UART TX GDMA.
  * @param  USIIndex: 0.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains 
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pTxBuf: Tx Buffer.
  * @param  TxCount: Tx Count.
  * @retval   TRUE/FLASE
  */
BOOL USI_UARTTXGDMA_Init(
	u8 USIIndex,
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
	GDMA_InitStruct->GDMA_DstHandshakeInterface   = USI_DEV_TABLE[USIIndex].Tx_HandshakeInterface;
	GDMA_InitStruct->GDMA_DstAddr = (u32)&USI_DEV_TABLE[USIIndex].USIx->TX_FIFO_WRITE;
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

	assert_param(GDMA_InitStruct->GDMA_BlockSize < 4096);
	
	GDMA_InitStruct->GDMA_SrcAddr = (u32)(pTxBuf);

	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);
	
	return _TRUE;
}

/**
  * @brief    Init and Enable UART RX GDMA.
  * @param  USIIndex: 0.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains 
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pRxBuf: Rx Buffer.
  * @param  RxCount: Rx Count.
  * @retval   TRUE/FLASE
  */
BOOL USI_UARTRXGDMA_Init(
	u8 USIIndex,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pRxBuf,
	int RxCount
	)
{
	u8 GdmaChnl;
	
	assert_param(GDMA_InitStruct != NULL);
	
	GdmaChnl = GDMA_ChnlAlloc(0, (IRQ_FUN)CallbackFunc, (u32)CallbackData, 12);//ACUT is 0x10, BCUT is 12
	if (GdmaChnl == 0xFF) {
		/* No Available DMA channel */
		return _FALSE;
	}

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	GDMA_InitStruct->GDMA_DIR      = TTFCPeriToMem;
	GDMA_InitStruct->GDMA_ReloadSrc = 1;
	GDMA_InitStruct->GDMA_SrcHandshakeInterface = USI_DEV_TABLE[USIIndex].Rx_HandshakeInterface;
	GDMA_InitStruct->GDMA_SrcAddr = (u32)&USI_DEV_TABLE[USIIndex].USIx->RX_FIFO_READ;
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

	assert_param(GDMA_InitStruct->GDMA_BlockSize < 4096);
	
	/* multi block close */
	GDMA_InitStruct->MuliBlockCunt     = 0;
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->MaxMuliBlock = 1;

	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);
	
	return _TRUE;
}

/**
  * @brief  Fills USI_LPUARTInitStruct member low power rx path related with its default value.
  * @param  USI_LPUARTInitStruct: pointer to an USI_LPUARTInitTypeDef structure which will be initialized.
  * @retval None
  */
void USI_UARTLPRxStructInit(USI_LPUARTInitTypeDef* USI_LPUARTInitStruct)
{
	/* USI_LPUARTInitStruct members default value */
	USI_LPUARTInitStruct->USI_LPUARTBitNumThres  = 100;
	USI_LPUARTInitStruct->USI_LPUARTOscPerbitUpdCtrl = ENABLE;
}

/**
  * @brief    Initializes the the UART Low Power RX path peripheral according to the specified
  *              parameters in the USI_LPUARTInitStruct.
  * @param  USIx: where x can be 0.
  * @param  USI_LPUARTInitStruct: pointer to an USI_LPUARTInitTypeDef structure which has been configured.
  * @note    Initial UART Low Power Rx path  steps:
  *              - step1:  Reset Low Power Rx Path.
  *              - step2:  Configure bit number threshold of one monitor period.
  *              - step3:  Configure the update OSC cycnum_perbit bit.
  * @retval   None
  */
void USI_UARTLPRxInit(USI_TypeDef* USIx, USI_LPUARTInitTypeDef *USI_LPUARTInitStruct)
{
	u32 RegValue = 0;

	/*check the parameters*/
	assert_param(IS_ALL_USI_LP_PERIPH(USIx));
	assert_param((USI_LPUARTInitStruct->USI_LPUARTBitNumThres>0) && (USI_LPUARTInitStruct->USI_LPUARTBitNumThres<128));
	
	/* Step 1: Reset Low Power Rx Path */
	USIx->SW_RESET &= (~USI_SW_RESET_RX_RSTB);

	RegValue = USIx->UART_BAUD_MON_CTRL;

	/*step 2: Configure bit number threshold of one monitor period.*/
	RegValue &= (~ USI_UART_LP_RX_BIT_NUM_THRES);
	RegValue |= (USI_LPUARTInitStruct->USI_LPUARTBitNumThres << 8);

	/*step 3: Configure the OSC cycnum_perbit update bit*/	
	if(USI_LPUARTInitStruct->USI_LPUARTOscPerbitUpdCtrl != DISABLE){
		/*enable OSC cycnum_perbit update*/
		RegValue |= USI_UART_LP_RX_OSC_UPD_IN_XTAL;
	} else {
		/*disable OSC cycnum_perbit update*/
		RegValue &= ~ USI_UART_LP_RX_OSC_UPD_IN_XTAL;
	}
	USIx->UART_BAUD_MON_CTRL = RegValue;
}

/**
  * @brief    set uart baud rate of low power rx path
  * @param  USIx: where x can be 0.
  * @param  BaudRate: the desired baud rate
  * @param  RxIPClockHz: the uart rx clock. unit: [Hz]
  * @note    according to the baud rate calculation formlula in low power rx path, method 
  *              implemented is as follows:
  *                                 - CycPerbit = round( fpclock/BaudRate)
  * @retval  None
  */
void USI_UARTLPRxBaudSet(USI_TypeDef* USIx, u32 BaudRate, u32 RxIPClockHz)
{
	u32 CycPerbit = 0;
	u32 RegValue = 0;
	u32 RegOscBaud = 0;

	/*check the parameters*/
	assert_param(IS_ALL_USI_LP_PERIPH(USIx));
	assert_param((BaudRate > 0 && BaudRate <= 500000));

	/*Calculate the r_cycnum_perbit field, 
	   according to clock and the desired baud rate*/
	if((RxIPClockHz % BaudRate) >= (BaudRate + 1) / 2){
		CycPerbit = RxIPClockHz / BaudRate + 1;
	} else {
		CycPerbit = RxIPClockHz / BaudRate;
	}

	/* Average clock cycle number of XTAL clock. */
	RegValue = USIx->UART_RX_BAUD_XTAL;
	
	RegValue &= (~ USI_RUART_LPRX_XTAL_CYCNUM_PERBIT);
	RegValue |= CycPerbit;

	/* set XTAL CycPerbit */
	USIx->UART_RX_BAUD_OSC= RegValue;

	/* Average clock cycle number of one bit OSC. */
	RegOscBaud = USIx->UART_RX_BAUD_OSC;
	RegOscBaud &= (~ USI_UART_LPRX_OSC_CYCNUM_PERBIT);
	RegOscBaud |= CycPerbit;

	/*set the OSC CycPerbit*/
	USIx->UART_RX_BAUD_OSC = RegOscBaud;

	/*set uart_rxbaud_adj = 0*/
	USIx->UART_RX_FRACTION_BAUD_CTRL &= (~USI_UART_RX_XFACTOR_ADJ);
}

/**
  * @brief    enable or disable the monitor function in Low Power Rx Path.
  * @param  USIx: where x can be 0.
  * @param  NewState: the new state of monitoring.
  *              This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void USI_UARTLPRxMonitorCmd(USI_TypeDef* USIx, u32 NewState)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_LP_PERIPH(USIx));
	
	/* configure Low Power rx monitoring function*/
	if(NewState != DISABLE ){
		/* Function enable of monitoring rx baud */
		USIx->UART_BAUD_MON_CTRL |= USI_UART_LP_RX_MON_ENABLE;
	} else {
		/* Function disable of monitoring rx baud */
		USIx->UART_BAUD_MON_CTRL &= (~ USI_UART_LP_RX_MON_ENABLE);
	}
}

/**
  * @brief    select uart rx path.
  * @param  USIx: where x can be 0.
  * @param  LPRxpath: the new state of uart rx path.
  *              This parameter can be: ENABLE or DISABLE.
  *              DISABLE is high rate rx path, ENABLE is low power rx path
  * @retval  None
  */
void USI_UARTLPRxpathSet(USI_TypeDef* USIx, u32 LPRxpath)
{
	/* To avoid gcc warnings */
	( void ) USIx;
	( void ) LPRxpath;
	/*AmebaZ's API, AmebaD doesn't use this API any more; this API is reserved for compatibility with AmebaZ*/
}

/**
  * @brief    Configure USI UART RX Clock.
  * @param  USIx: where x can be 0.
  * @param  RxClock:  This parameter can be one of the following values:
  * @retval   None
  * @note    USI V01(Ameba-D) not support this function. 
  */
void USI_UARTLPRxIPClockSet(USI_TypeDef* USIx, u32 RxIPClock)
{
	/* To avoid gcc warnings */
	( void ) RxIPClock;
	
	/* Check the parameters */
	assert_param(IS_ALL_USI_PERIPH(USIx));
	//assert_param(IS_USI_UART_RX_CLK(RxIPClock));

	/* not support */
}

/**
  * @brief    enable or disable Uart Low Power Rx Path
  * @param  USIx: where x can be 0.
  * @param  NewState: the new state of the Rx Path.
  *              This parameter can be: ENABLE or DISABLE.
  * @note    Because the TX/RX of the IrDA transceiver are in the same module, 
  *              RX fifo can also receive available data when in the process of TX.Therefore,
  *              the RX Path must be disabled when TX is going on.Namely, IrDA must work
  *              in the Half duplex mode, though normal UART IP supports Full duplex.
  * @retval None
  */				
void USI_UARTLPRxCmd(USI_TypeDef* USIx, u32 NewState)
{

	/*check the parameters*/
	assert_param(IS_ALL_USI_LP_PERIPH(USIx));

	if (NewState != DISABLE) {
		/*enable uart receiver*/
		USIx->SW_RESET |= USI_SW_RESET_RX_RSTB;
	} else {
		/*disable uart receiver*/
		USIx->SW_RESET &= (~USI_SW_RESET_RX_RSTB);
	}
}

/**
  * @brief    Fills each IrDA_InitStruct member with its default value.
  * @param  IrDA_InitStruct : pointer to a USI_UartIrDAInitTypeDef
  *              structure which will be initialized.
  * @retval   None
  */
void USI_UARTIrDAStructInit(USI_UartIrDAInitTypeDef * IrDA_InitStruct)
{
	/* Set the default value */
	IrDA_InitStruct->USI_UARTIrDARxInv = DISABLE;
	IrDA_InitStruct->USI_UARTIrDATxInv = DISABLE;
	IrDA_InitStruct->USI_UARTLowShift = USI_UART_IRDA_PULSE_LEFT_SHIFT;
	IrDA_InitStruct->USI_UARTLowShiftVal = 0;
	IrDA_InitStruct->USI_UARTUpperShift = USI_UART_IRDA_PULSE_LEFT_SHIFT;
	IrDA_InitStruct->USI_UARTUpperShiftVal = 0;
	IrDA_InitStruct->USI_UARTRxFilterCmd = ENABLE;
	IrDA_InitStruct->USI_UARTRxFilterThres = 7;
}

/**
  * @brief    Configures the USI UART's IrDA interface .
  * @param  USIx: where x can be 0.
  * @param  IrDA_InitStruct: pointer to a USI_UartIrDAInitTypeDef structure that contains
  *              the configuration information for the IrDA module.
  * @note    the details of IrDA_InitStruct members are:
  * @verbatim 
  *	           IrDA_InitStruct-> USI_UARTIrDARxInv:
  *				IrDA Rx invert bit:
  *                 		ENABLE: invert irda rx
  *                 		DISABLE: not invert irda rx
  *
  *	           IrDA_InitStruct->USI_UARTIrDATxInv:
  *				IrDA Tx invert bit:
  *                 		ENABLE: invert irda tx
  *                 		DISABLE: not invert irda tx
  *
  *	           IrDA_InitStruct->USI_UARTUpperShift:
  *			 	Upperbound(right edge) Shift direction:
  *			 		UART_IRDA_PULSE_LEFT_SHIFT: shift left
  *                 		UART_IRDA_PULSE_RIGHT_SHIFT: shift right
  *	           IrDA_InitStruct->UpperShiftVal:
  *				Upperbound Shift value
  *
  *	           IrDA_InitStruct->USI_UARTLowShift:
  *				Lowbound(left edge) Shift direction:
  *			 		UART_IRDA_PULSE_LEFT_SHIFT: shift left
  *                 		UART_IRDA_PULSE_RIGHT_SHIFT: shift right
  *	           IrDA_InitStruct->USI_UARTLowShiftVal:
  *				Lowbound Shift value
  *
  *	           IrDA_InitStruct->USI_UARTRxFilterThres:
  *				IrDA RX filter threshold
  *
  *	           IrDA_InitStruct->USI_UARTRxFilterCmd:
  *				IrDA RX filter enable or disable:
  *                 		ENABLE: enable IrDA rx filter
  *                 		DISABLE: disable IrDA rx filter
  * @endverbatim
  * @retval   None
  */
void USI_UARTIrDAInit(USI_TypeDef* USIx, USI_UartIrDAInitTypeDef * IrDA_InitStruct)
{
	u32 TempCtrl;
	u32 TempTxpulse;
	u32 TempRxPulse;

	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));
	assert_param(IrDA_InitStruct->USI_UARTUpperShiftVal <= 0x7fff );
	assert_param(IrDA_InitStruct->USI_UARTLowShiftVal <= 0x7fff);
	assert_param(IS_USI_IRDA_PUL_SHIFT(IrDA_InitStruct->USI_UARTLowShift));
	assert_param(IS_USI_IRDA_PUL_SHIFT(IrDA_InitStruct->USI_UARTUpperShift));							
	assert_param((IrDA_InitStruct->USI_UARTRxFilterThres <= 0x7fff));
	
	/*Get the UART_IRDA_CTRL register value*/
	TempCtrl = USIx->UART_IRDA_CTRL;
	
	/*configure the IrDA RX invert bit*/
	if(IrDA_InitStruct->USI_UARTIrDARxInv != DISABLE){
		/*invert the irda_rx_i*/
		TempCtrl |= USI_UART_IRDA_RX_INV;
	} else {
		/*not invert the irda_rx_i*/
		TempCtrl &= (~ USI_UART_IRDA_RX_INV);
	}

	/*configure the IrDA TX invert bit*/
	if(IrDA_InitStruct->USI_UARTIrDATxInv != DISABLE){
		/*invert the irda_tx_o*/
		TempCtrl |= USI_UART_IRDA_TX_INV;
	} else {
		/*not invert the irda_tx_o*/
		TempCtrl &= (~ USI_UART_IRDA_TX_INV);
	}
	USIx->UART_IRDA_CTRL = TempCtrl;

	/*Get the UART_IRDA_TX_PULSE_WD register value*/
	TempTxpulse = USIx->UART_IRDA_TX_PULSE_WD;
	
	/*configure IrDA SIR TX Pulse Width*/
	/*configure Upperbound(right edge) shift direction*/
	TempTxpulse &= (~ USI_UART_IRDA_TX_PUL_UP_BUND_SHIFT);
	TempTxpulse |= (IrDA_InitStruct->USI_UARTUpperShift << 31);

	/*configure the Upperbound shift value*/
	TempTxpulse &= (~ USI_UART_IRDA_TX_PUL_UP_BUND_VAL);
	TempTxpulse |= (IrDA_InitStruct->USI_UARTUpperShiftVal << 16);

	/*configure Lowbound(left edge) shift direction*/
	TempTxpulse &= (~ USI_UART_IRDA_TX_PUL_LOW_BUND_SHIFT);
	TempTxpulse |= (IrDA_InitStruct->USI_UARTLowShift << 15);

	/*configure the Lowbound shift value*/
	TempTxpulse &= (~ USI_UART_IRDA_TX_PUL_LOW_BUND_VAL);
	TempTxpulse |= (IrDA_InitStruct->USI_UARTLowShiftVal);	

	USIx->UART_IRDA_TX_PULSE_WD = TempTxpulse;

	/*Get the RXPLSR register value*/
	TempRxPulse = USIx->UART_IRDA_RX_PULSE_WD;

	/*configure IrDA RX filter threshold*/
	TempRxPulse &= (~ USI_UART_IRDA_RX_FILTER_THRES);
	TempRxPulse |= (IrDA_InitStruct->USI_UARTRxFilterThres << 1);
	
	if(IrDA_InitStruct->USI_UARTRxFilterCmd != DISABLE){
		/*enable IrDA rx filter*/
		TempRxPulse |= USI_UART_IRDA_RX_FILTER_ENABLE;
	} else {
		/*disable IrDA rx filter*/
		TempRxPulse &= (~ USI_UART_IRDA_RX_FILTER_ENABLE);
	}

	USIx->UART_IRDA_RX_PULSE_WD = TempRxPulse;
}

/**
  * @brief    enable or disable the IrDA function.
  * @param  USIx: where x can be 0.
  * @param  NewState: the new state of the IrDA.
  *              This parameter can be: ENABLE or DISABLE.
  * @retval   None
  */
void USI_UARTIrDACmd(USI_TypeDef* USIx, u32 NewState)
{
	/*check the parameters*/
	assert_param(IS_ALL_USI_PERIPH(USIx));

	if(NewState != DISABLE ){
		/* enable the IrDA */
		USIx->UART_IRDA_CTRL|= USI_UART_IRDA_ENABLE;
	} else {
		/* disable the IrDA */
		USIx->UART_IRDA_CTRL &= (~ USI_UART_IRDA_ENABLE);
	}
}
/******************* (C) COPYRIGHT 2017 Realtek Semiconductor *****END OF FILE****/
