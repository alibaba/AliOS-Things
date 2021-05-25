/**
  ******************************************************************************
  * @file    rtl8721dhp_usi_i2c.c
  * @author
  * @version V1.0.0
  * @date    2017-12-18
  * @brief   This file contains all the functions prototypes for the I2C firmware
  *          library, including the following functionalities of theIntel-Integrated 
  *             Circuit (I2C) peripheral:
  *           - Initialization
  *           - I2C Speed Setting
  *           - I2C Slave Address Updating
  *           - Receive/Send Data Interface
  *           - I2C Peripheral Control (disable/enable)
  *           - I2C SleepMode Control
  *           - Output pin Configuration
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
#include "rtl8721d_usi_i2c.h"

u32 USI_I2C_SLAVEWRITE_PATCH;
u32 USI_IC_FS_SCL_HCNT_TRIM;
u32 USI_IC_FS_SCL_LCNT_TRIM;

/**
  * @brief  Fills each USI_I2C_StructInit member with its default value.
  * @param  USI_I2C_StructInit: pointer to an USI_I2C_InitTypeDef structure which will be initialized.
  * @retval None
  */
void USI_I2C_StructInit(USI_I2C_InitTypeDef* USI_I2C_InitStruct)
{
	/* Load HAL initial data structure default value */
	USI_I2C_InitStruct->USI_I2CMaster     = USI_I2C_MASTER_MODE;
	USI_I2C_InitStruct->USI_I2CAddrMod    = USI_I2C_ADDR_7BIT;
	USI_I2C_InitStruct->USI_I2CSpdMod     = USI_I2C_SS_MODE;
	USI_I2C_InitStruct->USI_I2CClk        = 100;
	USI_I2C_InitStruct->USI_I2CIPClk      = 50000000;
	USI_I2C_InitStruct->USI_I2CAckAddr    = 0x11;
	USI_I2C_InitStruct->USI_I2CSdaHd      = 1;
	USI_I2C_InitStruct->USI_I2CSlvSetup   = 0x3;
	USI_I2C_InitStruct->USI_I2CRXTL       = 0x01;
	USI_I2C_InitStruct->USI_I2CTXTL       = 0x10;
	USI_I2C_InitStruct->USI_I2CMstReSTR   = DISABLE;
	USI_I2C_InitStruct->USI_I2CMstGC      = DISABLE;
	USI_I2C_InitStruct->USI_I2CMstStartB  = DISABLE;
	USI_I2C_InitStruct->USI_I2CSlvNoAck   = DISABLE;
	USI_I2C_InitStruct->USI_I2CFilter     = 0x11;
	USI_I2C_InitStruct->USI_I2CTxDMARqLv  = 0x09;
	USI_I2C_InitStruct->USI_I2CRxDMARqLv  = 0x04;
}

/**
  * @brief  Initializes the USIx peripheral according to the specified 
  *			parameters in the USI_I2C_InitStruct.
  * @param  USIx: where USIx can be USI0_DEV.
  * @param  USI_I2C_InitStruct: pointer to a USI_I2C_InitTypeDef structure that contains
  * 		the configuration information for the specified I2C peripheral.
  * @retval None
  */
void USI_I2C_Init(USI_TypeDef *USIx, USI_I2C_InitTypeDef* USI_I2C_InitStruct)
{
	u8  Specical;
	
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	assert_param(IS_USI_I2C_ADDR_MODE(USI_I2C_InitStruct->USI_I2CAddrMod));
	assert_param(IS_USI_I2C_SPEED_MODE(USI_I2C_InitStruct->USI_I2CSpdMod));
	
	/* Disable the IC first */
	USIx->I2C_ENABLE &= ~USI_I2C_ENABLE;

	/*reset USI all logic, this step will clear tx/rx fifo*/
	USIx->SW_RESET &=(~USI_SW_RESET_RSTB);
	USIx->SW_RESET = 0x1f;

	/*disable USI all mode, configure USI with I2C mode*/
	USIx->USI_MODE_CTRL &= (~USI_SERIAL_MODE);
	USIx->USI_MODE_CTRL |= USI_SERIAL_I2C_MODE;
	
	/* Master case*/
	if (USI_I2C_InitStruct->USI_I2CMaster) {
		/*RESTART MUST be set in these condition in Master mode. 
		But it might be NOT compatible in old slaves.*/
		if ((USI_I2C_InitStruct->USI_I2CAddrMod == USI_I2C_ADDR_10BIT) || (USI_I2C_InitStruct->USI_I2CSpdMod == USI_I2C_HS_MODE) || (USI_I2C_InitStruct->USI_I2CMstStartB != 0))
			USI_I2C_InitStruct->USI_I2CMstReSTR = ENABLE;

		USIx->I2C_CTRL = (USI_I2C_SLV_DISABLE |
			(USI_I2C_InitStruct->USI_I2CSpdMod << 2) |    
			(USI_I2C_InitStruct->USI_I2CMstReSTR << 1) |
			(USI_I2C_InitStruct->USI_I2CMaster));

		/* To set target addr.*/
		Specical = 0;
		if ((USI_I2C_InitStruct->USI_I2CMstGC != 0) || (USI_I2C_InitStruct->USI_I2CMstStartB != 0))
			Specical = 1;

		USIx->I2C_TAR = ((USI_I2C_InitStruct->USI_I2CAddrMod << 12) |
			(Specical << 11)           |
			(USI_I2C_InitStruct->USI_I2CMstStartB << 10)      |
			(USI_I2C_InitStruct->USI_I2CAckAddr & USI_I2C_IC_TAR));

		/* To Set I2C clock*/
		USI_I2C_SetSpeed(USIx, USI_I2C_InitStruct->USI_I2CSpdMod, USI_I2C_InitStruct->USI_I2CClk, USI_I2C_InitStruct->USI_I2CIPClk);
	}    /*if (Master)*/
	else {
		USIx->I2C_CTRL = (((USI_I2C_InitStruct->USI_I2CAckAddr <<12) & USI_I2C_SAR) |
			(USI_I2C_InitStruct->USI_I2CAddrMod << 9) |
			(USI_I2C_InitStruct->USI_I2CMaster << 8) |
			(USI_I2C_InitStruct->USI_I2CSpdMod << 2) |
			(USI_I2C_InitStruct->USI_I2CMaster));

		/* To set slave no ack and ack general call*/
		USIx->I2C_SLV_ACK_CTRL = USI_I2C_InitStruct->USI_I2CSlvNoAck |(USI_I2C_InitStruct->USI_I2CSlvAckGC << 1);
		/* to set SDA setup time */
		USIx->I2C_SDA_TIMING = (USI_I2C_InitStruct->USI_I2CSlvSetup & USI_I2C_SDA_SETUP);
	}
	/* To set SDA hold time */
	USIx->I2C_SDA_TIMING = (((USI_I2C_InitStruct->USI_I2CSdaHd << 16) & USI_I2C_SDA_HOLD) | (USIx->I2C_SDA_TIMING & USI_I2C_SDA_SETUP));
	
	/* To set TX almost empty Level */
	USIx->TX_FIFO_CTRL= USI_I2C_InitStruct->USI_I2CTXTL;

	/* To set RX almost full Level */
	USIx->RX_FIFO_CTRL = USI_I2C_InitStruct->USI_I2CRXTL;

	/* To set IC_FILTER */
	USIx->I2C_DIG_FILTER= USI_I2C_InitStruct->USI_I2CFilter;
	
	/* To set DMA TX/RX level */
	USIx->DMA_REQ_SIZE &= (~(USI_TX_DMA_BURST_SIZE | USI_RX_DMA_BURST_SIZE));
	USIx->DMA_REQ_SIZE |= (USI_I2C_InitStruct->USI_I2CTxDMARqLv | (USI_I2C_InitStruct->USI_I2CRxDMARqLv << 16));

	/*I2C Clear all interrupts first*/
	USI_I2C_ClearAllINT(USIx);

	/*I2C Disable all interrupts first*/
	USI_I2C_INTConfig(USIx, 0xFFFFFFFF, DISABLE);
}

/**
  * @brief  Master sets I2C Speed Mode.
  * @param  USIx: where USIx can be USI0_DEV.
  * @param  SpdMd: I2C Speed Mode. 
  *   This parameter can be one of the following values:
  *     @arg USI_I2C_SS_MODE: 
  *     @arg USI_I2C_FS_MODE:  
  *     @arg USI_I2C_HS_MODE:
  * @param  I2Clk: I2C Bus Clock, unit is KHz. 
  *   This parameter can be one of the following values:
  *     @arg 50: 
  *     @arg 100:  
  *     @arg 400:  
  *     @arg 1000: 
  *     @arg 3000: 
  * @param  IPClk: I2C IP Clock, unit is Hz. 
  * @retval None
  */
void USI_I2C_SetSpeed(USI_TypeDef *USIx, u32 SpdMd, u32 I2Clk, u32 I2CIPClk)
{
	u32 ICHLcnt;
	u32 ICHtime;
	u32 ICLtime;
	u32 IPClkM = I2CIPClk /1000000;
	
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	switch (SpdMd)  
	{
		case USI_I2C_SS_MODE:
		{
			ICHtime = ((1000000/I2Clk)*USI_I2C_SS_MIN_SCL_HTIME)/(USI_I2C_SS_MIN_SCL_HTIME+USI_I2C_SS_MIN_SCL_LTIME);
			ICLtime = ((1000000/I2Clk)*USI_I2C_SS_MIN_SCL_LTIME)/(USI_I2C_SS_MIN_SCL_HTIME+USI_I2C_SS_MIN_SCL_LTIME);

			ICHLcnt = (ICHtime * IPClkM)/1000;			
			USIx->I2C_SS_SCL_CNT = (ICHLcnt & USI_I2C_SS_SCL_HCNT) + 1;
			ICHLcnt = (ICLtime * IPClkM)/1000;
			USIx->I2C_SS_SCL_CNT |= (ICHLcnt << 16) & USI_I2C_SS_SCL_LCNT;

			break;
		}

		case USI_I2C_FS_MODE:
		{
			ICHtime = ((1000000/I2Clk)*USI_I2C_FS_MIN_SCL_HTIME)/(USI_I2C_FS_MIN_SCL_HTIME+USI_I2C_FS_MIN_SCL_LTIME);
			ICLtime = ((1000000/I2Clk)*USI_I2C_FS_MIN_SCL_LTIME)/(USI_I2C_FS_MIN_SCL_HTIME+USI_I2C_FS_MIN_SCL_LTIME);

			ICHLcnt = (ICHtime * IPClkM)/1000;
			if (ICHLcnt > IC_FS_SCL_HCNT_TRIM)/*this part is according to the fine-tune result*/
				ICHLcnt -= IC_FS_SCL_HCNT_TRIM;
			USIx->I2C_FS_SCL_CNT = (ICHLcnt & USI_I2C_FS_SCL_HCNT) + 1;

			ICHLcnt = (ICLtime * IPClkM)/1000;
			if (ICHLcnt > IC_FS_SCL_LCNT_TRIM)/*this part is according to the fine-tune result*/
				ICHLcnt -= IC_FS_SCL_LCNT_TRIM;
			USIx->I2C_FS_SCL_CNT |= (ICHLcnt << 16) & USI_I2C_FS_SCL_LCNT;
			
			break;
		}

		case USI_I2C_HS_MODE:
		{
			/*set Fast mode count for Master code*/
			ICHtime = ((1000000/400)*USI_I2C_FS_MIN_SCL_HTIME)/(USI_I2C_FS_MIN_SCL_HTIME+USI_I2C_FS_MIN_SCL_LTIME);
			ICLtime = ((1000000/400)*USI_I2C_FS_MIN_SCL_LTIME)/(USI_I2C_FS_MIN_SCL_HTIME+USI_I2C_FS_MIN_SCL_LTIME);

			ICHLcnt = (ICHtime * IPClkM)/1000;
			USIx->I2C_FS_SCL_CNT = ICHLcnt & USI_I2C_FS_SCL_HCNT;

			ICHLcnt = (ICLtime * IPClkM)/1000;
			USIx->I2C_FS_SCL_CNT |= (ICHLcnt << 16) & USI_I2C_FS_SCL_LCNT;

			ICHtime = ((1000000/I2Clk)*USI_I2C_HS_MIN_SCL_HTIME_100)/(USI_I2C_HS_MIN_SCL_HTIME_100+USI_I2C_HS_MIN_SCL_LTIME_100);
			ICLtime = ((1000000/I2Clk)*USI_I2C_HS_MIN_SCL_LTIME_100)/(USI_I2C_HS_MIN_SCL_HTIME_100+USI_I2C_HS_MIN_SCL_LTIME_100);

			ICHLcnt = (ICHtime * IPClkM)/1000;
			//if (ICHLcnt>8)/*this part is according to the fine-tune result*/
			//	ICHLcnt -= 3;
			USIx->I2C_HS_SCL_CNT = (ICHLcnt & USI_I2C_HS_SCL_HCNT)+1;

			ICHLcnt = (ICLtime * IPClkM)/1000;
			//if (ICHLcnt>6)/*this part is according to the fine-tune result*/
			//	ICHLcnt -= 6;
			USIx->I2C_HS_SCL_CNT |= (ICHLcnt << 16) & USI_I2C_HS_SCL_LCNT;

			break;
		}    

		default:
		break;
	}
}

/**
  * @brief  Master transmits the address byte to select the slave device.
  * @param  USIx: where USIx can be USI0_DEV.
  * @param  Address: specifies the slave address which will be transmitted
  * @retval None.
  */
void USI_I2C_SetSlaveAddress(USI_TypeDef *USIx, u16 Address)
{
	u32 tar = USIx->I2C_TAR & ~(USI_I2C_IC_TAR);
	
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	/*set target address to generate start signal*/
	USIx->I2C_TAR = (Address & USI_I2C_IC_TAR) | tar;
}

/**
  * @brief  Checks whether the specified I2C flag is set or not.
  * @param  USIx: where USIx can be USI0_DEV.
  * @param  USI_I2C_FLAG: specifies the flag to check. 
  *   This parameter can be one of the following values:
  *     @arg USI_I2C_BUS_ACTIVITY: BUS FSM Activity Status. 
  *     @arg USI_I2C_SLV_ACTIVITY: Slave FSM Activity Status.
  *     @arg USI_I2C_MST_ACTIVITY: Master FSM Activity Status.
  *     @arg USI_I2C_ACTIVITY_COMB: I2C Activity Status, combine Master and Slave.
  *     @arg USI_I2C_ENABLE_COPY: i2c_enable copy to here
  * @retval The new state of USI_I2C_FLAG:
  *          - 1: the specified I2C flag is set
  *          - 0: the specified I2C flag is not set 
  */
u8 USI_I2C_CheckFlagState(USI_TypeDef *USIx, u32 USI_I2C_FLAG)
{
	u8 bit_status = 0;
	
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	if((USIx->I2C_STATUS & USI_I2C_FLAG) != 0)
	{
		/* USI_I2C_FLAG is set */
		bit_status = 1;
	}

	/* Return the USI_I2C_FLAG status */
	return  bit_status;
}

/**
  * @brief  Checks whether the specified I2C TXFIFO flag is set or not.
  * @param  USIx: where USIx can be USI0_DEV.
  * @param  USI_I2C_TXFIFO_FLAG: specifies the flag to check. 
  *   This parameter can be one of the following values:
  *     @arg USI_TXFIFO_ALMOST_EMPTY_COPY: TX FIFO ALMOST EMPTY status
  *     @arg USI_TXFIFO_EMPTY: TX FIFO EMPTY status
  *     @arg USI_TXFIFO_FULL: TX FIFO FULL status
  * @retval The new state of USI_I2C_TXFIFO_FLAG:
  *          - 1: the specified I2C flag is set
  *          - 0: the specified I2C flag is not set 
  */
u8 USI_I2C_CheckTXFIFOState(USI_TypeDef *USIx, u32 USI_I2C_TXFIFO_FLAG)
{
	u8 bit_status = 0;
	
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	if((USIx->TX_FIFO_STATUS & USI_I2C_TXFIFO_FLAG) != 0)
	{
		/* USI_I2C_TXFIFO_FLAG is set */
		bit_status = 1;
	}

	/* Return the USI_I2C_TXFIFO_FLAG status */
	return  bit_status;
}

/**
  * @brief  Checks whether the specified I2C RXFIFO flag is set or not.
  * @param  USIx: where USIx can be USI0_DEV.
  * @param  USI_I2C_RXFIFO_FLAG: specifies the flag to check. 
  *   This parameter can be one of the following values:
  *     @arg USI_RXFIFO_ALMOST_FULL_COPY: RX FIFO ALMOST EMPTY status
  *     @arg USI_RXFIFO_EMPTY: RX FIFO EMPTY status
  *     @arg USI_RXFIFO_FULL: RX FIFO FULL status
  * @retval The new state of USI_I2C_RXFIFO_FLAG:
  *          - 1: the specified I2C flag is set
  *          - 0: the specified I2C flag is not set 
  */
u8 USI_I2C_CheckRXFIFOState(USI_TypeDef *USIx, u32 USI_I2C_RXFIFO_FLAG)
{
	u8 bit_status = 0;
	
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	if((USIx->RX_FIFO_STATUS & USI_I2C_RXFIFO_FLAG) != 0)
	{
		/* USI_I2C_RXFIFO_FLAG is set */
		bit_status = 1;
	}

	/* Return the USI_I2C_RXFIFO_FLAG status */
	return  bit_status;
}
/**
  * @brief  ENABLE/DISABLE  the I2C's interrupt bits..
  * @param  USIx: where USIx can be USI0_DEV.
  * @param  USI_I2C_IT: specifies the USIx interrupt sources to be enabled or disabled.
  *          This parameter can be one or combinations of the following values:
  *            @arg USI_I2C_DMA_DONE_INTER_EN: I2C DMA Done Interrupt
  *            @arg USI_I2C_LP_WAKE_INTER_EN: I2C Low power wakeup Interrupt
  *            @arg USI_I2C_GEN_CALL_INTER_EN: General Call Interrupt
  *            @arg USI_I2C_START_DET_INTER_EN: Start or Restart Condition Interrupt
  *            @arg USI_I2C_STOP_DET_INTER_EN: Stop Condition Interrupt
  *            @arg USI_I2C_ACTIVITY_INTER_EN: I2C Activity Interrupt
  *            @arg USI_I2C_RX_DONE_INTER_EN: Slave Transmitter RX Done Interrupt
  *            @arg USI_I2C_TX_ABRT_INTER_EN: Transmit Abort Interrupt
  *            @arg USI_I2C_RD_REQ_INTER_EN: Read Request Interrupt
  *            @arg USI_RXFIFO_UNDERFLOW_INTR_EN: RX FIFO underflow Interrupt
  *            @arg USI_RXFIFO_OVERFLOW_INTR_EN: RX FIFO overflow Interrupt
  *            @arg USI_RXFIFO_ALMOST_FULL_INTR_EN: RX FIFO almost Full Interrupt
  *            @arg USI_TXFIFO_OVERFLOW_INTR_EN: TX FIFO overflow Interrupt
  *            @arg USI_TXFIFO_ALMOST_EMTY_INTR_EN: TX FIFO almost Empty Interrupt
  * @param  NewState: specifies the state of the interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void USI_I2C_INTConfig(USI_TypeDef *USIx, u32 USI_I2C_IT, u32 NewState)
{
	u32 TempVal = USIx->INTERRUPT_ENABLE;
	
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	if (NewState == ENABLE) {
		TempVal |= USI_I2C_IT;
	} else {
		TempVal &= ~USI_I2C_IT;
	}

	USIx->INTERRUPT_ENABLE = TempVal;
}

/**
  * @brief  Clears the specified I2C interrupt pending bit.
  * @param  USIx: where USIx can be USI0_DEV.
  * @param  INTrBit: specifies the interrupt to be cleared.
  *          This parameter can be one of the following values:
  *            @arg USI_I2C_DMA_DONE_CLR: clear I2C DMA Done interrupt 
  *            @arg USI_I2C_LP_WAKE_CLR: clear I2C Low Power wakeup Interrupt
  *            @arg USI_I2C_GEN_CALL_CLR: clear General Call Interrupt
  *            @arg USI_I2C_START_DET_CLR: clear Start or Restart Condition Interrupt
  *            @arg USI_I2C_STOP_DET_CLR: clear Stop Condition Interrupt
  *            @arg USI_I2C_ACTIVITY_CLR: clear I2C Activity Interrupt
  *            @arg USI_I2C_RX_DONE_CLR: clear Slave Transmitter RX Done Interrupt
  *            @arg USI_I2C_TX_ABRT_CLR: clear Transmit Abort Interrupt
  *            @arg USI_I2C_RD_REQ_CLR: clear Read Request Interrupt
  *            @arg USI_RXFIFO_UNDERFLOW_CLR: clear RX FIFO underflow Interrupt
  *            @arg USI_RXFIFO_OVERFLOW_CLR: clear RX FIFO overflow Interrupt
  *            @arg USI_TXFIFO_OVERFLOW_CLR: clear TX FIFO overflow Interrupt
  * @note   USI_TXFIFO_ALMOST_EMTY_INTS is automatically cleared by hardware when the buffer 
  *         level goes above the TX_FIFO_CTRL threshold.
  *         USI_RXFIFO_ALMOST_FULL_INTS is automatically cleared by hardware when the buffer 
  *         level goes below the RX_FIFO_CTRL threshold. 
  * @retval None
  */
void USI_I2C_ClearINT(USI_TypeDef *USIx, u32 INTrBit)
{
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	/*write 1 to clear all interrupts*/
	USIx->INTERRUPT_STATUS_CLR =  INTrBit;
}

/**
  * @brief  Clears all of the I2C interrupt pending bit.
  * @param  USIx: where USIx can be USI0_DEV.
  * @retval None
  */
void USI_I2C_ClearAllINT(USI_TypeDef *USIx)
{
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));

	/*write 1 to clear all interrupts*/
	USIx->INTERRUPT_ALL_CLR = USI_INT_ALL_CLEAR;
}

/**
  * @brief  Get I2C Raw Interrupt Status.
  * @param  USIx: where USIx can be USI0_DEV.
  * @retval raw interrupt status
  */
u32 USI_I2C_GetRawINT(USI_TypeDef *USIx)
{
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	return USIx->RAW_INTERRUPT_STATUS;
}

/**
  * @brief  Get I2C interrupt status.
  * @param  USIx: where USIx can be USI0_DEV.
  * @retval interrupt status
  */
u32 USI_I2C_GetINT(USI_TypeDef *USIx)
{
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	return USIx->INTERRUPT_STATUS;
}

/**
  * @brief  Master sends single byte through the USIx peripheral according to the set of the upper layer.
  * @param  USIx: where USIx can be USI0_DEV.
  * @param  pBuf: point to the data that to be write.
  * @param  I2CCmd: specifies whether a read from FIFO or a write to FIFO is performed.
  * @param  I2CStop: specifies whether a STOP is issued after the byte is sent or received.
  * @param  I2CReSTR: specifies whether a RESTART is issued after the byte is sent or received.
  * @retval None
*/
void USI_I2C_MasterSendNullData(USI_TypeDef *USIx, u8* pBuf, u8  I2CCmd, u8  I2CStop, u8  I2CReSTR)
{
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	USIx->TX_FIFO_WRITE = *(pBuf) |
		(1 << 11) |
		(I2CReSTR << 10) |
		(I2CCmd << 8) |
		(I2CStop << 9);    
}

/**
  * @brief  Master sends single byte through the USIx peripheral according to the set of the upper layer.
  * @param  USIx: where USIx can be USI0_DEV.
  * @param  pBuf: point to the data that to be write.
  * @param  I2CCmd: specifies whether a read from FIFO or a write to FIFO is performed.
  * @param  I2CStop: specifies whether a STOP is issued after the byte is sent or received.
  * @param  I2CReSTR: specifies whether a RESTART is issued after the byte is sent or received.
  * @retval None
*/
void USI_I2C_MasterSend(USI_TypeDef *USIx, u8* pBuf, u8  I2CCmd, u8  I2CStop, u8  I2CReSTR)
{
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	USIx->TX_FIFO_WRITE = *(pBuf) |
		(I2CReSTR << 10) |
		(I2CCmd << 8) |
		(I2CStop << 9);    
}

/**
  * @brief  Slave sends single byte through the USIx peripheral after receiving read request of Master.
  * @param  USIx: where USIx can be USI0_DEV.
  * @param  Data: data to be transmitted.
  * @param  StopState: specifies whether a STOP is issued after the byte is sent.
  * @retval None
  */
void USI_I2C_SlaveSend(USI_TypeDef *USIx, u8 Data)
{
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	USIx->TX_FIFO_WRITE = Data;
}

/**
  * @brief  Returns the most recent received data by the USIx peripheral.
  * @param  USIx: where USIx can be USI0_DEV.
  * @retval The value of the received data.
  */
u8 USI_I2C_ReceiveData(USI_TypeDef *USIx)
{
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	/* Return the data in the DR register */
	return (u8)USIx->RX_FIFO_READ;
}

/**
  * @brief  Send data with special length in master mode through the USIx peripheral.
  * @param  USIx: where USIx can be USI0_DEV.
  * @param  pBuf: point to the data to be transmitted.
  * @param	len: the length of data that to be transmitted.
  * @note  deal with condition that master send address or data while slave no ack
  * @retval The length of data that have been sent to tx fifo
  */
u8 USI_I2C_MasterWrite(USI_TypeDef *USIx, u8* pBuf, u8 len)
{
	u8 cnt = 0;
	
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	/* Write in the DR register the data to be sent */
	for(cnt = 0; cnt < len; cnt++)
	{
		while((USI_I2C_CheckTXFIFOState(USIx, USI_TXFIFO_ALMOST_EMPTY_COPY)) == 0);
		
		if(cnt >= len - 1)
		{
			/*generate stop signal*/
			USIx->TX_FIFO_WRITE = (*pBuf++) | (1 << 9);
		}
		else
		{
			USIx->TX_FIFO_WRITE = (*pBuf++);
		}

		while((USI_I2C_CheckTXFIFOState(USIx, USI_TXFIFO_EMPTY)) == 0) {
			if(USI_I2C_GetRawINT(USIx) & USI_I2C_TX_ABRT_RSTS) {
				USI_I2C_ClearAllINT(USIx);
				return cnt;
			}
		}
	}

	return cnt;
}

/**
  * @brief  Read data with special length in master mode through the USIx peripheral under in-house IP.
  * @param  USIx: where USIx can be USI0_DEV.
  * @param  pBuf: point to the buffer to hold the received data.
  * @param	len: the length of data that to be received.
  * @retval The length of data that have received from rx fifo
  */
u8 USI_I2C_MasterRead(USI_TypeDef *USIx, u8* pBuf, u8 len)
{
	u8 cnt = 0;
	
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	/* read in the DR register the data to be received */
	for(cnt = 0; cnt < len; cnt++)
	{
		if(cnt >= len - 1)
		{
			/* generate stop singal */
			USIx->TX_FIFO_WRITE = 0x0003 << 8;
		}
		else
		{
			USIx->TX_FIFO_WRITE = 0x0001 << 8;
		}

		/* wait for USI_I2C_FLAG_RFNE flag */			
		while((USI_I2C_CheckRXFIFOState(USIx, USI_RXFIFO_EMPTY)) == 1) {
			if(USI_I2C_GetRawINT(USIx) & USI_I2C_TX_ABRT_RSTS) {
				USI_I2C_ClearAllINT(USIx);
				return cnt;
			}
		}
		*pBuf++ = (u8)USIx->RX_FIFO_READ;
	}

	return cnt;
}

/**
  * @brief  Send data with special length in slave mode through the USIx peripheral.
  * @param  USIx: where USIx can be USI0_DEV.
  * @param  pBuf: point to the data to be transmitted.
  * @param	len: the length of data that to be transmitted.
  * @retval None
  */
void USI_I2C_SlaveWrite(USI_TypeDef *USIx, u8* pBuf, u8 len)
{
	u8 cnt = 0;
	
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));

	for(cnt = 0; cnt < len; cnt++) {
		/* Check I2C RD Request flag */
		while((USI_I2C_GetRawINT(USIx) & USI_I2C_RD_REQ_RSTS) == 0);

		if (USI_I2C_SLAVEWRITE_PATCH) {
			USIx->INTERRUPT_STATUS_CLR =  USI_I2C_RD_REQ_CLR;
		}
		/* Check I2C TX FIFO status */
		while((USI_I2C_CheckTXFIFOState(USIx, USI_TXFIFO_ALMOST_EMPTY_COPY)) == 0);

		USIx->TX_FIFO_WRITE = (*pBuf++);
	}
	while((USI_I2C_CheckTXFIFOState(USIx, USI_TXFIFO_EMPTY)) == 0);
}

/**
  * @brief  Read data with special length in slave mode through the USIx peripheral.
  * @param  USIx: where USIx can be USI0_DEV.
  * @param  pBuf: point to the buffer to hold the received data.
  * @param	len: the length of data that to be received.
  * @retval None
  */
void USI_I2C_SlaveRead(USI_TypeDef *USIx, u8* pBuf, u8 len)
{
	u8 cnt = 0;
	
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	for(cnt = 0; cnt < len; cnt++) {
		/* Check I2C RX FIFO status */
		while((USI_I2C_CheckRXFIFOState(USIx, USI_RXFIFO_EMPTY)) == 1);

		*pBuf++ = (u8)USIx->RX_FIFO_READ;
	}
}
/**
  * @brief  Sends data and read data in master mode through the USIx peripheral.
  * @param  USIx: where USIx can be USI0_DEV to select the I2C peripheral.
  * @param  pWriteBuf: Byte to be transmitted.
  * @param  Writelen: Byte number to be transmitted (the length should less then buffer depth for A CUT).
  * @param  pReadBuf: Byte to be received.
  * @param  Readlen: Byte number to be received.
  * @retval None
  */
void USI_I2C_MasterRepeatRead(USI_TypeDef* USIx, u8* pWriteBuf, u8 Writelen, u8* pReadBuf, u8 Readlen)
{

	u8 cnt = 0;

	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	/* write in the DR register the data to be sent */
	for(cnt = 0; cnt < Writelen; cnt++)
	{
		while((USI_I2C_CheckTXFIFOState(USIx, USI_TXFIFO_ALMOST_EMPTY_COPY)) == 0);
		
		if(cnt >= Writelen - 1)
		{
			/*generate restart signal*/
			USIx->TX_FIFO_WRITE = (*pWriteBuf++) | (1 << 10);
		}
		else
		{
			USIx->TX_FIFO_WRITE = (*pWriteBuf++);
		}
	}

	/*Wait I2C TX FIFO not full*/
	while((USI_I2C_CheckTXFIFOState(USIx, USI_TXFIFO_ALMOST_EMPTY_COPY)) == 0);
	
	USI_I2C_MasterRead(USIx, pReadBuf, Readlen);
}

/**
  * @brief  Enables or disables the specified I2C peripheral, this is one bit register.
  * @param  USIx: where USIx can be USI0_DEV.
  * @param  NewState: new state of the USIx peripheral. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void USI_I2C_Cmd(USI_TypeDef *USIx, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	if (NewState != DISABLE)
	{
		/* Enable the selected I2C peripheral */
		USIx->I2C_ENABLE |= USI_I2C_ENABLE;
	}
	else
	{
		/* Disable the selected I2C peripheral */
		USIx->I2C_ENABLE &= ~USI_I2C_ENABLE;
	}
}

/**
  * @brief  Enable I2C Tx or Rx DMA.
  * @param  USIx: where USIx can be USI0_DEV.
  * @param  DmaCtrl: control Transmit DMA Enable or Receive DMA Enable
  *   This parameter can be one of the following values:
  *     @arg USI_TX_DMA_ENABLE: 
  *     @arg USI_RX_DMA_ENABLE:
  * @param  NewState: the new state of the DMA function.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void USI_I2C_DMAControl(USI_TypeDef *USIx, u32 DmaCtrl, u8 NewState)
{
	u32  I2CDMAEn = USIx->DMA_ENABLE;
	
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	if (NewState == ENABLE)
		I2CDMAEn |= DmaCtrl;
	else
		I2CDMAEn &= ~DmaCtrl;

	USIx->DMA_ENABLE = I2CDMAEn;
}

/**
  * @brief  Initializes the USIx Control Register DMA mode.
  * @param  USIx: where USIx can be USI0_DEV.
  * @param  USI_I2C_DmaCmd: Command to control the read or write operation and 
  *         the action after the last data transferred.
  *          This parameter can be one or combinations of the following values:
  *            @arg USI_I2C_DMODE_RESTART
  *            @arg USI_I2C_DMODE_STOP
  *            @arg USI_I2C_DMODE_WRITE_CMD
  *            @arg USI_I2C_DMODE_READ_CMD
  *            @arg USI_I2C_DMODE_ENABLE
  * @param  USI_I2C_DmaBLen: length of data.
  * 		This parameter must be set to a value in the 0-0xFFFF range.
  * @retval None
  */
void USI_I2C_DmaRegModeConfig(USI_TypeDef *USIx, u32 USI_I2C_DmaCmd, u32 USI_I2C_DmaBLen)
{
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	assert_param(IS_USI_I2C_DMA_DATA_LEN(USI_I2C_DmaBLen));
	
	USIx->I2C_DMA_CMD = ((USI_I2C_DmaBLen << 16) | USI_I2C_DMA_CTRL_REG_MODE | USI_I2C_DmaCmd);
}

/**
  * @brief    Init and Enable I2C TX GDMA.
  * @param  Index: 0 .
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains 
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pTxBuf: Tx Buffer.
  * @param  TxCount: Tx Count.
  * @retval   TRUE/FLASE
  * @note can not support legacy DMA mode
  */
BOOL USI_I2C_TXGDMA_Init(
	u8 Index,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pTxBuf,
	int TxCount
	)
{
	u8 GdmaChnl;
	
	assert_param(GDMA_InitStruct != NULL);
	
	GdmaChnl = GDMA_ChnlAlloc(0, (IRQ_FUN)CallbackFunc, (u32)CallbackData, 12);
	if (GdmaChnl == 0xFF) {
		/*  No Available DMA channel */
		return _FALSE;
	}	

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));
	
	GDMA_InitStruct->MuliBlockCunt     = 0;
	GDMA_InitStruct->MaxMuliBlock      = 1;
	
	GDMA_InitStruct->GDMA_DIR      = TTFCMemToPeri;
	GDMA_InitStruct->GDMA_DstHandshakeInterface = USI_DEV_TABLE[Index].Tx_HandshakeInterface;
	GDMA_InitStruct->GDMA_DstAddr = (u32)&USI_DEV_TABLE[Index].USIx->TX_FIFO_WRITE;
	GDMA_InitStruct->GDMA_Index   = 0;
	GDMA_InitStruct->GDMA_ChNum       = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType|TransferType|ErrType);

	GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
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
  * @brief    Init and Enable I2C RX GDMA.
  * @param  I2C: 0.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains 
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pRxBuf: Rx Buffer.
  * @param  RxCount: Rx Count.
  * @retval   TRUE/FLASE
  * @note support Master or Slave RXDMA
  */
BOOL USI_I2C_RXGDMA_Init(
	u8 Index,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pRxBuf,
	int RxCount
	)
{
	u8 GdmaChnl;
	
	assert_param(GDMA_InitStruct != NULL);
	
	GdmaChnl = GDMA_ChnlAlloc(0, (IRQ_FUN)CallbackFunc, (u32)CallbackData, 12);
	if (GdmaChnl == 0xFF) {
		/* No Available DMA channel */
		return _FALSE;
	}

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	GDMA_InitStruct->GDMA_DIR      = TTFCPeriToMem;
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->GDMA_SrcHandshakeInterface  = USI_DEV_TABLE[Index].Rx_HandshakeInterface;
	GDMA_InitStruct->GDMA_SrcAddr = (u32)&USI_DEV_TABLE[Index].USIx->RX_FIFO_READ;
	GDMA_InitStruct->GDMA_Index = 0;
	GDMA_InitStruct->GDMA_ChNum       = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType|TransferType|ErrType);
	GDMA_InitStruct->GDMA_SrcMsize = MsizeFour;
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;
	GDMA_InitStruct->GDMA_DstInc = IncType;
	GDMA_InitStruct->GDMA_SrcInc = NoChange;
	
	if (((u32)(pRxBuf) & 0x03)==0) {
		/*	4-bytes aligned, move 4 bytes each DMA transaction */
		GDMA_InitStruct->GDMA_DstMsize	 = MsizeOne;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
	} else {
		/*	move 1 byte each DMA transaction */
		GDMA_InitStruct->GDMA_DstMsize	 = MsizeFour;
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
  * @brief  Clock-gated I2C clock domain for address matching interrupts wake up.
  * @param  USIx: where USIx can be USI0_DEV.
  * @param  NewState: the new state of the Rx Path.
  *              This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void USI_I2C_Sleep_Cmd(USI_TypeDef *USIx, u32 NewStatus)
{
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	if (NewStatus != DISABLE) {
		USI_I2C_INTConfig(USIx, USI_I2C_LP_WAKE_INTER_EN, ENABLE);
		
		USIx->I2C_DIG_FILTER &= ~ USI_I2C_DIG_FLTR_SEL;
		USIx->I2C_CTRL &= ~(USI_I2C_SLV_DISABLE);
		USIx->I2C_ENABLE |= USI_I2C_SLEEP;
	} else {
		USI_I2C_INTConfig(USIx, USI_I2C_LP_WAKE_INTER_EN, DISABLE);
		
		USIx->I2C_DIG_FILTER |=  USI_I2C_DIG_FLTR_SEL; 
		USIx->I2C_ENABLE &= ~USI_I2C_SLEEP;
	}
}

/**
  * @brief  Wake up clock domain of Clock-gated I2C after address matching interrupts.
  * @param  USIx: where USIx can be USI0_DEV.
  * @retval None
  */
void USI_I2C_WakeUp(USI_TypeDef *USIx)
{
	/* Check the parameters */
	assert_param(IS_USI_I2C_ALL_PERIPH(USIx));
	
	USI_I2C_ClearINT(USIx, USI_I2C_LP_WAKE_CLR);

	USI_I2C_Sleep_Cmd(USIx, DISABLE);
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/

