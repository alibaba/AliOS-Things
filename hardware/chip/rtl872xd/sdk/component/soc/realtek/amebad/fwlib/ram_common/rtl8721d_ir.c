/**
  ******************************************************************************
  * @file    rtl8721dhp_ir.c
  * @author
  * @version V1.0.0
  * @date    2017-09-17
  * @brief   This file contains all the functions prototypes for the IR firmware
  *             library, including the following functionalities of the Universal Asynchronous 
  *             Receiver/Transmitter peripheral:
  *           -Initialization
  *           -IR Control (disable/enable)
  *           -Receive/Send Data Interface 
  *           -Interrupts and flags management 
  *           -Threshold setting interface
  *           -Clear TX/RX FIFO
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

/**
  * @brief  Deinitializes the IR peripheral registers to their default values.
  * @param  None.
  * @retval None
  */
void IR_DeInit(void)
{
	//1TODO:
	// RCC_PeriphClockCmd(APBPeriph_IR, APBPeriph_IR_CLOCK, DISABLE);
}


/**
  * @brief Initializes the IR peripheral according to the specified
  *   parameters in the IR_InitStruct
  * @param  IR_InitStruct: pointer to a IR_InitTypeDef structure that
  *   contains the configuration information for the specified IR peripheral
  * @retval None
  */
void IR_Init(IR_TypeDef *IRx, IR_InitTypeDef *IR_InitStruct)
{
	/* Check the parameters */
	assert_param(IS_IR_ALL_PERIPH(IRx));
	assert_param(IS_IR_MODE(IR_InitStruct->IR_Mode));

	/* Configure IR clock divider. Formula: IR_CLK = IO_CLK/(1+IR_CLK_DIV) */
	IRx->IR_CLK_DIV = (IR_InitStruct->IR_Clock) / (IR_InitStruct->IR_Freq) - 1;
	IRx->IR_TX_COMPE_DIV = (IR_InitStruct->IR_Clock) / (IR_InitStruct->IR_TxCOMP_CLK) - 1;

	if (IR_InitStruct->IR_Mode == IR_MODE_TX) {
		/* Check the parameters in TX mode */
		assert_param(IS_IR_FREQUENCY(IR_InitStruct->IR_Freq));
		assert_param(IS_IR_IDLE_STATUS(IR_InitStruct->IR_TxIdleLevel));
		assert_param(IS_IR_TX_DATA_TYPE(IR_InitStruct->IR_TxInverse));
		assert_param(IS_IR_TX_FIFO_THRESHOLD(IR_InitStruct->IR_TxFIFOThrLevel));

		/* Configure TX mode parameters and disable all TX interrupt */
		IRx->IR_TX_CONFIG = (IR_InitStruct->IR_Mode) | \
			(IR_InitStruct->IR_TxIdleLevel) | \
			(IR_InitStruct->IR_TxInverse) | \
			(IR_TX_FIFO_THRESHOLD(IR_InitStruct->IR_TxFIFOThrLevel)) | \
			(((IRx->IR_CLK_DIV + 1) / (IR_InitStruct->IR_DutyCycle)) << 16);

		/* Clear all TX interrupt and TX FIFO */
		IRx->IR_TX_INT_CLR = IR_TX_INT_ALL_CLR | IR_TX_FIFO_CLR;
	} else {
		/* Check the parameters in RX mode */
		assert_param(IS_RX_START_MODE(IR_InitStruct->IR_RxStartMode));
		assert_param(IS_IR_RX_FIFO_THRESHOLD(IR_InitStruct->IR_RxFIFOThrLevel));
		assert_param(IS_IR_RX_FIFO_FULL_DISCARD_CTRL(IR_InitStruct->IR_RxFIFOFullCtrl));
		assert_param(IS_RX_RX_TRIGGER_EDGE(IR_InitStruct->IR_RxTriggerMode));
		assert_param(IS_IR_RX_FILTER_TIME_CTRL(IR_InitStruct->IR_RxFilterTime));
		assert_param(IS_IR_RX_COUNT_LEVEL_CTRL(IR_InitStruct->IR_RxCntThrType));
		assert_param(IS_IR_RX_COUNTER_THRESHOLD(IR_InitStruct->IR_RxCntThr));

		/* Enable RX mode */
		IRx->IR_TX_CONFIG |= (IR_InitStruct->IR_Mode);
		/* Configure RX mode parameters and disable all RX interrupt */
		IRx->IR_RX_CONFIG = (IR_InitStruct->IR_RxStartMode) | \
			(IR_InitStruct->IR_RxTriggerMode)  | \
			(IR_InitStruct->IR_RxFilterTime)   | \
			(IR_RX_FIFO_THRESHOLD(IR_InitStruct->IR_RxFIFOThrLevel));
		/* Configure IR RX counter threshold parameters */
		IRx->IR_RX_CNT_INT_SEL = (IR_InitStruct->IR_RxCntThrType) | (IR_InitStruct->IR_RxCntThr);
		/* Clear all RX interrupt and RX FIFO */
		IRx->IR_RX_INT_CLR = IR_RX_INT_ALL_CLR | IR_RX_FIFO_CLR;
	}
}

/**
  * @brief  Fills each IR_InitStruct member with its default value.
  * @param  IR_InitStruct: pointer to an IR_InitTypeDef structure which will be initialized.
  * @retval None
  */
void IR_StructInit(IR_InitTypeDef *IR_InitStruct)
{
	IR_InitStruct->IR_Clock				= 100000000; /*100M*/
	IR_InitStruct->IR_Freq				= 38000;
	IR_InitStruct->IR_DutyCycle			= 3;
	IR_InitStruct->IR_Mode				= IR_MODE_TX;
	IR_InitStruct->IR_TxIdleLevel			= IR_IDLE_OUTPUT_LOW;
	IR_InitStruct->IR_TxInverse			= IR_TX_DATA_NORMAL | IR_TX_DATA_CARRIER_NORMAL;
	IR_InitStruct->IR_TxFIFOThrLevel		= 0;
	IR_InitStruct->IR_TxCOMP_CLK		= 1000000; /*1M*/
	IR_InitStruct->IR_RxStartMode			= IR_RX_AUTO_MODE;
	IR_InitStruct->IR_RxFIFOThrLevel		= 0;
	IR_InitStruct->IR_RxFIFOFullCtrl		= IR_RX_FIFO_FULL_DISCARD_NEWEST;
	IR_InitStruct->IR_RxTriggerMode		= IR_RX_DOUBLE_EDGE;
	IR_InitStruct->IR_RxFilterTime			= IR_RX_FILTER_TIME_50NS;
	IR_InitStruct->IR_RxCntThrType		= IR_RX_COUNT_LOW_LEVEL;
	IR_InitStruct->IR_RxCntThr			= 0x23a; /* This value can be 0 to 0x7fffffff */
}

/**
  * @brief   Enable or disable the selected IR mode.
  * @param  mode: selected IR operation mode.
  *   This parameter can be the following values:
  *     @arg IR_MODE_TX: Transmission mode.
  *     @arg IR_MODE_RX: Receiving mode.
  * @param  NewState: new state of the operation mode.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void IR_Cmd(IR_TypeDef *IRx, u32 mode, u32 NewState)
{
	/* Check the parameters */
	assert_param(IS_IR_ALL_PERIPH(IRx));
	assert_param(IS_IR_MODE(mode));

	if (NewState == ENABLE) {	
		if (mode == IR_MODE_TX) 
			IRx->IR_TX_CONFIG |= IR_TX_START;
		else 
			IRx->IR_RX_CONFIG |= IR_RX_START;
	
	} else if (NewState == DISABLE) {
		if (mode == IR_MODE_TX) 
			IRx->IR_TX_CONFIG &= ~IR_TX_START;
		else
			IRx->IR_RX_CONFIG &= ~IR_RX_START;
	
	}
}

/**
  * @brief   Configure counter threshold value in receiving mode.You can use it to stop receiving IR data.
  * @param  IR_RxCntThrType:
  *   This parameter can be the following values:
  *     @arg IR_RX_Count_Low_Level: Low level counter value >= IR_RxCntThr, trigger IR_INT_RX_CNT_THR interrupt.
  *     @arg IR_RX_Count_High_Level: High level counter value >= IR_RxCntThr, trigger IR_INT_RX_CNT_THR interrupt.
  * @param  IR_RxCntThr: Configure IR Rx counter threshold value which can be 0 to 0x7fffffffUL.
  * @retval None
  */
void IR_SetRxCounterThreshold(IR_TypeDef *IRx, u32 IR_RxCntThrType, u32 IR_RxCntThr)
{
	/* Check the parameters */
	assert_param(IS_IR_ALL_PERIPH(IRx));
	assert_param(IS_IR_RX_COUNT_LEVEL_CTRL(IR_RxCntThrType));
	assert_param(IS_IR_RX_COUNTER_THRESHOLD(IR_RxCntThr));

	/* Configure IR RX counter threshold parameters */
	IRx->IR_RX_CNT_INT_SEL = (IR_RxCntThrType) | (IR_RxCntThr);
}

/**
  * @brief  Send data.
  * @param  buf: data buffer to send.
  * @param  length: buffer length.
  * @param IsLastPacket:
  *   This parameter can be the following values:
  *     @arg ENABLE: The last data in IR packet and there is no continous data.In other words, An infrared data transmission is completed.
  *     @arg DISABLE: There is data to be transmitted continuously.
  * @retval None
  */
void IR_SendBuf(IR_TypeDef *IRx, u32 *pBuf, u32 len, u32 IsLastPacket)
{
	assert_param(IS_IR_ALL_PERIPH(IRx));

	if (len == 0)
		return;

	while (--len)
		IRx->IR_TX_FIFO = *pBuf++;

	/* If send the last IR packet, SET the following bit */
	if (IsLastPacket == _TRUE) 
		IRx->IR_TX_FIFO =  *pBuf |IR_TX_DATA_END_MASK;
	else
		IRx->IR_TX_FIFO =  *pBuf;
	
}

/**
  * @brief  Read data From RX FIFO.
  * @param  buf: buffer address to receive data.
  * @param  length: read data length.
  * @retval None
  */
void IR_ReceiveBuf(IR_TypeDef *IRx, u32 *pBuf, u32 len)
{
	assert_param(IS_IR_ALL_PERIPH(IRx));

	while (len--)
		*pBuf++ = IRx->IR_RX_FIFO;

}

/**
  * @brief  Enables or disables the specified IR interrupts.
  * @param  IR_INT: specifies the IR interrupts sources to be enabled or disabled.
  *   This parameter can be the following values:
  *     @arg IR_TX_FIFO_EMPTY_INT_EN: TX FIFO empty interrupt.
  *     @arg IR_TX_FIFO_LEVEL_INT_EN: TX FIFO threshold interrupt.
  *     @arg IR_TX_FIFO_OVER_INT_EN: TX FIFO overflow interrupt.
  *     @arg IR_RX_FIFO_FULL_INT_EN: RX FIFO  full interrupt.
  *     @arg IR_RX_FIFO_LEVEL_INT_EN: RX FIFO threshold interrupt.
  *     @arg IR_RX_CNT_OF_INT_EN: RX counter overflow interrupt.
  *     @arg IR_RX_FIFO_OF_INT_EN: RX FIFO overflow interrupt.
  *     @arg IR_RX_CNT_THR_INT_EN: RX counter threshold interrupt.
  *     @arg IR_RX_FIFO_ERROR_INT_EN: RX FIFO error read interrupt. Trigger when RX FIFO empty and read RX FIFO.
  * @param  NewState: new state of the specified IR interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void IR_INTConfig(IR_TypeDef *IRx, u32 IR_INT, u32 newState)
{
	assert_param(IS_IR_ALL_PERIPH(IRx));

	if (IR_MODE(IRx->IR_TX_CONFIG) == IR_MODE_RX) {
		assert_param(IS_RX_INT(IR_INT));
		
		if (newState == ENABLE)
			IRx->IR_RX_CONFIG |= IR_INT;
		else
			IRx->IR_RX_CONFIG &= ~IR_INT;
	} else {
		assert_param(IS_TX_INT(IR_INT));
		
		if (newState == ENABLE)
			IRx->IR_TX_CONFIG |=IR_INT;
		else
			IRx->IR_TX_CONFIG &= ~IR_INT;
	}
}

/**
  * @brief  Mask or unmask the specified IR interrupts.
  * @param  IR_INT: specifies the IR interrupts sources to be mask or unmask.
  *   This parameter can be the following values:
  *     @arg IR_TX_FIFO_EMPTY_INT_MASK: TX FIFO empty interrupt mask.
  *     @arg IR_TX_FIFO_LEVEL_INT_MASK: TX FIFO threshold interrupt mask.
  *     @arg IR_TX_FIFO_OVER_INT_MASK: TX FIFO overflow interrupt mask.
  *     @arg IR_RX_FIFO_FULL_INT_Msk: RX FIFO  full interrupt mask.
  *     @arg IR_RX_FIFO_LEVEL_INT_Msk: RX FIFO threshold interrupt mask.
  *     @arg IR_RX_CNT_OF_INT_Msk: RX counter overflow interrupt mask.
  *     @arg IR_RX_FIFO_OF_INT_Msk: RX FIFO overflow interrupt mask.
  *     @arg IR_RX_CNT_THR_INT_Msk: RX counter threshold interrupt mask.
  *     @arg IR_RX_FIFO_ERROR_INT_Msk: RX FIFO error read interrupt mask. Trigger when RX FIFO empty and read RX FIFO.
  * @param  NewState: new state of the specified IR interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void IR_MaskINTConfig(IR_TypeDef *IRx, u32 IR_INT, u32 newState)
{
	assert_param(IS_IR_ALL_PERIPH(IRx));
	
	if (IR_MODE(IRx->IR_TX_CONFIG) == IR_MODE_RX) {
		assert_param(IS_RX_INT_MASK(IR_INT));
		
		if (newState == ENABLE)
			IRx->IR_RX_CONFIG |= IR_INT;
		else
			IRx->IR_RX_CONFIG &= ~IR_INT;
	
	} else {
		assert_param(IS_TX_INT_MASK(IR_INT));
		
		if (newState == ENABLE)
			IRx->IR_TX_CONFIG |= IR_INT;
		else
			IRx->IR_TX_CONFIG &= ~IR_INT;
	}
}

/**
  * @brief Get the specified IR interrupt status.
  * @retval The new state of IR_INT (SET or RESET).
  */
u32 IR_GetINTStatus(IR_TypeDef *IRx)
{
	u32 status;

	assert_param(IS_IR_ALL_PERIPH(IRx));

	if (IR_MODE(IRx->IR_TX_CONFIG) == IR_MODE_RX)
		status = (IRx->IR_RX_SR);	
	else
		status = (IRx->IR_TX_SR);

	/* Return the IR_INT status */
	return  status;
}

/**
  * @brief Get the specified IR interrupt mask status.
  * @retval The new mask state of IR_INT (SET or RESET).
  */
u32 IR_GetIMR(IR_TypeDef *IRx)
{
	u32 mask;

	assert_param(IS_IR_ALL_PERIPH(IRx));

	if (IR_MODE(IRx->IR_TX_CONFIG) == IR_MODE_RX)
		mask = IRx->IR_RX_CONFIG  & IR_RX_INT_ALL_MASK;	
	else
		mask = IRx->IR_TX_CONFIG  & IR_TX_INT_ALL_MASK;

	/* Return the IR_INT mask status */
	return  mask;
}

/**
  * @brief Get the specified IR FSM status.
  * @retval The new state of FSM (IDLE or RUN).
  * @TRUE:RUN, FALSE:IDLE.
  */
u32 IR_FSMRunning(IR_TypeDef *IRx)
{
	u32 status = FALSE;

	assert_param(IS_IR_ALL_PERIPH(IRx));

	if (IR_MODE(IRx->IR_TX_CONFIG) == IR_MODE_RX) {
		if((IRx->IR_RX_SR) & IR_RX_FSM_STATUS)
			status = TRUE;
	} else {
		if((IRx->IR_TX_SR) & IR_TX_FSM_STATUS)
			status = TRUE;
	}

	return  status;
}

/**
  * @brief  Clears the IR interrupt pending bits.
  * @param  IR_IT: specifies the interrupt pending bit to clear.
  *   This parameter can be any combination of the following values:
  *     @arg IR_TX_FIFO_EMPTY_INT_CLR: Clear TX FIFO empty interrupt.
  *     @arg IR_TX_FIFO_LEVEL_INT_CLR: Clear TX FIFO threshold interrupt.
  *     @arg IR_TX_FIFO_OVER_INT_CLR: Clear TX FIFO overflow interrupt.
  *     @arg IR_RX_FIFO_FULL_INT_CLR: Clear RX FIFO  full interrupt.
  *     @arg IR_RX_FIFO_LEVEL_INT_CLR: Clear RX FIFO threshold interrupt.
  *     @arg IR_RX_CNT_OF_INT_CLR: Clear RX counter overflow interrupt.
  *     @arg IR_RX_FIFO_OF_INT_CLR: Clear RX FIFO overflow interrupt.
  *     @arg IR_RX_CNT_THR_INT_CLR: Clear RX counter threshold interrupt.
  *     @arg IR_RX_FIFO_ERROR_INT_CLR: Clear RX FIFO error read interrupt. Trigger when RX FIFO empty and read RX FIFO.
  * @retval None
  */
void IR_ClearINTPendingBit(IR_TypeDef *IRx, u32 IR_CLEAR_INT)
{
	assert_param(IS_IR_ALL_PERIPH(IRx));

	if (IR_MODE(IRx->IR_TX_CONFIG) == IR_MODE_RX) {
		assert_param(IS_RX_INT_CLR(IR_CLEAR_INT));
		IRx->IR_RX_INT_CLR |= IR_CLEAR_INT;
	} else {
		assert_param(IS_TX_INT_CLR(IR_CLEAR_INT));
		IRx->IR_TX_INT_CLR |= IR_CLEAR_INT;
	}
}


/**
  * @brief  set tx threshold.when TX FIFO depth <= threshold value, trigger  interrupt
  * @param  thd: tx threshold.
  * @retval None
  */
 void IR_SetTxThreshold(IR_TypeDef *IRx, uint8_t thd)
{
	u32 Tx_config;

	assert_param(IS_IR_ALL_PERIPH(IRx));
	assert_param(IS_IR_RX_FIFO_THRESHOLD(thd));
	
	Tx_config = IRx->IR_TX_CONFIG;
	Tx_config &= ~IR_TX_FIFO_THRESHOLD_MASK;
	Tx_config |= IR_TX_FIFO_THRESHOLD(thd);
	IRx->IR_TX_CONFIG = Tx_config;
}


/**
  * @brief  set rx threshold.when RX FIFO depth > threshold value, trigger  interrupt
  * @param  thd: rx threshold.
  * @retval None
  */
void IR_SetRxThreshold(IR_TypeDef *IRx, uint8_t thd)
{
	u32 Rx_config;

	assert_param(IS_IR_ALL_PERIPH(IRx));
	assert_param(IS_IR_RX_FIFO_THRESHOLD(thd));
	
	Rx_config = IRx->IR_RX_CONFIG;
	Rx_config &= ~IR_RX_FIFO_THRESHOLD_MASK;
	Rx_config |= IR_RX_FIFO_THRESHOLD(thd);
	IRx->IR_RX_CONFIG = Rx_config;
}


/**
  * @brief  Get free size of TX FIFO .
  * @param  None
  * @retval the free size of TX FIFO
  */
u32 IR_GetTxFIFOFreeLen(IR_TypeDef *IRx)
{
	assert_param(IS_IR_ALL_PERIPH(IRx));
	
	return (IR_TX_FIFO_SIZE - ((IRx->IR_TX_SR & (IR_TX_FIFO_LEVEL)) >> 8));   //IR_TX_FIFO_SIZE - TX FIFO offset
}


/**
  * @brief  Get data size in RX FIFO.
  * @param  None
  * @retval current data size in RX FIFO.
  */
u32 IR_GetRxDataLen(IR_TypeDef *IRx)
{
	assert_param(IS_IR_ALL_PERIPH(IRx));
	
	return ((((IRx->IR_RX_SR) & IR_RX_FIFO_LEVEL) >> 8));
}


/**
  * @brief  Send one data.
  * @param  data: send data.
  * @retval None
  */
void IR_SendData(IR_TypeDef *IRx, u32 data)
{
	assert_param(IS_IR_ALL_PERIPH(IRx));
	
	IRx->IR_TX_FIFO = data;
}


/**
  * @brief   Start trigger only in manual receive mode.
  * @param  None.
  * @retval None
  */
void IR_StartManualRxTrigger(IR_TypeDef *IRx)
{
	assert_param(IS_IR_ALL_PERIPH(IRx));
	
	/* Start Rx manual mode */
	IRx->IR_RX_CONFIG |= IR_RX_MANUAL_START;
}


/**
  * @brief Read one data.
  * @param none
  * @retval data which read from RX FIFO.
  */
u32 IR_ReceiveData(IR_TypeDef *IRx)
{
	assert_param(IS_IR_ALL_PERIPH(IRx));

	return IRx->IR_RX_FIFO;
}

/**
  * @brief  Clear IR TX FIFO.
  * @param  none
  * @retval None
  */
void IR_ClearTxFIFO(IR_TypeDef *IRx)
{
	assert_param(IS_IR_ALL_PERIPH(IRx));
	
	IRx->IR_TX_INT_CLR = IR_TX_FIFO_CLR;
}


/**
  * @brief  Clear IR RX FIFO.
  * @param  none
  * @retval None
  */
void IR_ClearRxFIFO(IR_TypeDef *IRx)
{
	assert_param(IS_IR_ALL_PERIPH(IRx));
	
	IRx->IR_RX_INT_CLR = IR_RX_FIFO_CLR;
}

