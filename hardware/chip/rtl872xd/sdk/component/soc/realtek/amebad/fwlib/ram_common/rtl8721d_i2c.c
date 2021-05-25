/**
  ******************************************************************************
  * @file    rtl8721d_i2c.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
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
  *  @verbatim   
  *
  *			===================================================================
  *								   How to use this driver
  *			===================================================================
  *			1. Enable peripheral clock using the follwoing functions(it is enabled by default)
  *				RCC_PeriphClockCmd(APBPeriph_I2Cx, APBPeriph_I2Cx_CLOCK, ENABLE); for I2C0 
  * 
  *			2. Configure the I2C pinmux.
  *		   		Pinmux_Config(Pin_Num, PINMUX_FUNCTION_I2C)
  *
  *			3. Call I2C_Init(I2Cx, I2C_InitStruct) to configure the i2c with the corresponding configuration
  *
  *			4. Enable the NVIC and the corresponding interrupt using the function 
  *			   I2C_INTConfig() and register the i2c irq handler if you need to use interrupt mode. 
  *
  *			5. When using the DMA mode 
  *					 - GDMA related configurations(source address/destination address/block size etc.)	
  *					 - Enable the DMA using the I2C_DMAControl() function
  *
  *			6. Enable i2c module using I2C_Cmd(I2Cx, NewState) 
  *
  * @Note: All other functions can be used separately to modify, if needed,
  *         a specific feature of the I2C.
  *
  *	@endverbatim
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

const I2C_DevTable I2C_DEV_TABLE[1] = {
		{I2C0_DEV, GDMA_HANDSHAKE_INTERFACE_I2C0_TX, GDMA_HANDSHAKE_INTERFACE_I2C0_RX, I2C0_IRQ_LP},
};

u32 I2C_SLAVEWRITE_PATCH;
u32 IC_FS_SCL_HCNT_TRIM;
u32 IC_FS_SCL_LCNT_TRIM;

/**
  * @brief  Fills each I2C_InitStruct member with its default value.
  * @param  I2C_InitStruct: pointer to an I2C_InitTypeDef structure which will be initialized.
  * @retval None
  */
void I2C_StructInit(I2C_InitTypeDef* I2C_InitStruct)
{
	/* Load HAL initial data structure default value */
	I2C_InitStruct->I2CMaster     = I2C_MASTER_MODE;
	I2C_InitStruct->I2CAddrMod    = I2C_ADDR_7BIT;
	I2C_InitStruct->I2CSpdMod     = I2C_SS_MODE;
	I2C_InitStruct->I2CClk        = 100;
	I2C_InitStruct->I2CIPClk      = 10000000;
	I2C_InitStruct->I2CAckAddr    = 0x11;
	I2C_InitStruct->I2CSdaHd      = 2;
	I2C_InitStruct->I2CSlvSetup   = 0x3;
	I2C_InitStruct->I2CRXTL       = 0x00;
	I2C_InitStruct->I2CTXTL       = 0x10;
	I2C_InitStruct->I2CMstReSTR   = DISABLE;
	I2C_InitStruct->I2CMstGC      = DISABLE;
	I2C_InitStruct->I2CMstStartB  = DISABLE;
	I2C_InitStruct->I2CSlvNoAck   = DISABLE;
	I2C_InitStruct->I2CFilter     = 0x101;
	I2C_InitStruct->I2CTxDMARqLv  = 0x09;
	I2C_InitStruct->I2CRxDMARqLv  = 0x03;
	I2C_InitStruct->I2CAckAddr1   = 0x12;
}

/**
  * @brief  Initializes the I2Cx peripheral according to the specified 
  *			parameters in the I2C_InitStruct.
  * @param  I2Cx: where I2Cx can be I2C0_DEV .
  * @param  I2C_InitStruct: pointer to a I2C_InitTypeDef structure that contains
  * 		the configuration information for the specified I2C peripheral.
  * @retval None
  */
void I2C_Init(I2C_TypeDef *I2Cx, I2C_InitTypeDef* I2C_InitStruct)
{
	u8  Specical;
	
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	assert_param(IS_I2C_ADDR_MODE(I2C_InitStruct->I2CAddrMod));
	assert_param(IS_I2C_SPEED_MODE(I2C_InitStruct->I2CSpdMod));
	
	/* Disable the IC first */
	I2Cx->IC_ENABLE &= ~BIT_CTRL_IC_ENABLE;

	/* Master case*/
	if (I2C_InitStruct->I2CMaster) {
		/*RESTART MUST be set in these condition in Master mode. 
		But it might be NOT compatible in old slaves.*/
		if ((I2C_InitStruct->I2CAddrMod == I2C_ADDR_10BIT) || (I2C_InitStruct->I2CSpdMod == I2C_HS_MODE) || (I2C_InitStruct->I2CMstStartB != 0))
			I2C_InitStruct->I2CMstReSTR = ENABLE;

		I2Cx->IC_CON = (BIT_CTRL_IC_CON_IC_SLAVE_DISABLE_1 |
			BIT_CTRL_IC_CON_IC_SLAVE_DISABLE |
			(I2C_InitStruct->I2CMstReSTR << 5) |
			(I2C_InitStruct->I2CSpdMod << 1) |                            
			(I2C_InitStruct->I2CMaster));

		/* To set target addr.*/
		Specical = 0;
		if ((I2C_InitStruct->I2CMstGC != 0) || (I2C_InitStruct->I2CMstStartB != 0))
			Specical = 1;

		I2Cx->IC_TAR = ((I2C_InitStruct->I2CAddrMod << 12) |
			(Specical << 11)           |
			(I2C_InitStruct->I2CMstStartB << 10)      |
			(I2C_InitStruct->I2CAckAddr & BIT_CTRL_IC_TAR));

		/* To Set I2C clock*/
		I2C_SetSpeed(I2Cx, I2C_InitStruct->I2CSpdMod, I2C_InitStruct->I2CClk, I2C_InitStruct->I2CIPClk);
	}    /*if (Master)*/
	else {
		I2Cx->IC_CON = ((I2C_InitStruct->I2CMaster << 7) |
			(I2C_InitStruct->I2CMaster << 6) |
			(I2C_InitStruct->I2CAddrMod << 3) |
			(I2C_InitStruct->I2CSpdMod << 1) |
			(I2C_InitStruct->I2CMaster));

		/* To set slave0 addr. */
		I2Cx->IC_SAR = (I2C_InitStruct->I2CAckAddr & BIT_CTRL_IC_SAR);
		/* To set slave1 addr. */
		I2Cx->IC_SAR1 = (I2C_InitStruct->I2CAckAddr1 & BIT_CTRL_IC_SAR1);
		/* To set slave no ack */
		I2Cx->IC_SLV_DATA_NACK_ONLY = I2C_InitStruct->I2CSlvNoAck;
		/* Set ack general call. */
		I2Cx->IC_ACK_GENERAL_CALL = (I2C_InitStruct->I2CSlvAckGC & BIT_CTRL_IC_ACK_GENERAL_CALL);
		/* to set SDA setup time */
		I2Cx->IC_SDA_SETUP = (I2C_InitStruct->I2CSlvSetup & BIT_CTRL_IC_SDA_SETUP);
	}
	/* To set SDA hold time */
	I2Cx->IC_SDA_HOLD = (I2C_InitStruct->I2CSdaHd & BIT_CTRL_IC_SDA_HOLD);
	
	/* To set TX_Empty Level */
	I2Cx->IC_TX_TL = I2C_InitStruct->I2CTXTL;

	/* To set RX_Full Level */
	I2Cx->IC_RX_TL = I2C_InitStruct->I2CRXTL;

	/* To set IC_FILTER */
	I2Cx->IC_FILTER = I2C_InitStruct->I2CFilter;
	
	/* To set TX/RX FIFO level */
	I2Cx->IC_DMA_TDLR = I2C_InitStruct->I2CTxDMARqLv;
	I2Cx->IC_DMA_RDLR = I2C_InitStruct->I2CRxDMARqLv;

	/*I2C Clear all interrupts first*/
	I2C_ClearAllINT(I2Cx);

	/*I2C Disable all interrupts first*/
	I2C_INTConfig(I2Cx, 0xFFFFFFFF, DISABLE);
}

/**
  * @brief  Master sets I2C Speed Mode.
  * @param  I2Cx: where I2Cx can be I2C0_DEV.
  * @param  SpdMd: I2C Speed Mode. 
  *   This parameter can be one of the following values:
  *     @arg I2C_SS_MODE: 
  *     @arg I2C_FS_MODE:  
  *     @arg I2C_HS_MODE:(not support in amebaD )
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
void I2C_SetSpeed(I2C_TypeDef *I2Cx, u32 SpdMd, u32 I2Clk, u32 I2CIPClk)
{
	u32 ICHLcnt;
	u32 ICHtime;
	u32 ICLtime;
	u32 IPClkM = I2CIPClk /1000000;
	
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	switch (SpdMd)  
	{
		case I2C_SS_MODE:
		{
			ICHtime = ((1000000/I2Clk)*I2C_SS_MIN_SCL_HTIME)/(I2C_SS_MIN_SCL_HTIME+I2C_SS_MIN_SCL_LTIME);
			ICLtime = ((1000000/I2Clk)*I2C_SS_MIN_SCL_LTIME)/(I2C_SS_MIN_SCL_HTIME+I2C_SS_MIN_SCL_LTIME);

			ICHLcnt = (ICHtime * IPClkM)/1000;			
			I2Cx->IC_SS_SCL_HCNT = ICHLcnt + 1;
			ICHLcnt = (ICLtime * IPClkM)/1000;
			I2Cx->IC_SS_SCL_LCNT = ICHLcnt;

			break;
		}

		case I2C_FS_MODE:
		{
			ICHtime = ((1000000/I2Clk)*I2C_FS_MIN_SCL_HTIME)/(I2C_FS_MIN_SCL_HTIME+I2C_FS_MIN_SCL_LTIME);
			ICLtime = ((1000000/I2Clk)*I2C_FS_MIN_SCL_LTIME)/(I2C_FS_MIN_SCL_HTIME+I2C_FS_MIN_SCL_LTIME);

			ICHLcnt = (ICHtime * IPClkM)/1000;
			if (ICHLcnt > IC_FS_SCL_HCNT_TRIM)/*this part is according to the fine-tune result*/
				ICHLcnt -= IC_FS_SCL_HCNT_TRIM;
			I2Cx->IC_FS_SCL_HCNT = ICHLcnt + 1;

			ICHLcnt = (ICLtime * IPClkM)/1000;
			if (ICHLcnt > IC_FS_SCL_LCNT_TRIM)/*this part is according to the fine-tune result*/
				ICHLcnt -= IC_FS_SCL_LCNT_TRIM;
			I2Cx->IC_FS_SCL_LCNT = ICHLcnt;
			
			break;
		}

		case I2C_HS_MODE:
		{
			/*set Fast mode count for Master code*/
			ICHtime = ((1000000/I2Clk)*I2C_FS_MIN_SCL_HTIME)/(I2C_FS_MIN_SCL_HTIME+I2C_FS_MIN_SCL_LTIME);
			ICLtime = ((1000000/I2Clk)*I2C_FS_MIN_SCL_LTIME)/(I2C_FS_MIN_SCL_HTIME+I2C_FS_MIN_SCL_LTIME);

			ICHLcnt = (ICHtime * IPClkM)/1000;
			I2Cx->IC_FS_SCL_HCNT = ICHLcnt + 1;

			ICHLcnt = (ICLtime * IPClkM)/1000;
			I2Cx->IC_FS_SCL_LCNT = ICHLcnt;

			ICHtime = ((1000000/I2Clk)*I2C_HS_MIN_SCL_HTIME_100)/(I2C_HS_MIN_SCL_HTIME_100+I2C_HS_MIN_SCL_LTIME_100);
			ICLtime = ((1000000/I2Clk)*I2C_HS_MIN_SCL_LTIME_100)/(I2C_HS_MIN_SCL_HTIME_100+I2C_HS_MIN_SCL_LTIME_100);

			ICHLcnt = (ICHtime * IPClkM)/1000;
			//if (ICHLcnt>8)/*this part is according to the fine-tune result*/
			//	ICHLcnt -= 3;
			I2Cx->IC_HS_SCL_HCNT = ICHLcnt;

			ICHLcnt = (ICLtime * IPClkM)/1000;
			//if (ICHLcnt>6)/*this part is according to the fine-tune result*/
			//	ICHLcnt -= 6;
			I2Cx->IC_HS_SCL_LCNT = ICHLcnt;

			break;
		}    

		default:
		break;
	}
}

/**
  * @brief  Master transmits the address byte to select the slave device.
  * @param  I2Cx: where I2Cx can be I2C0_DEV.
  * @param  Address: specifies the slave address which will be transmitted
  * @retval None.
  */
void I2C_SetSlaveAddress(I2C_TypeDef *I2Cx, u16 Address)
{
	u32 tar = I2Cx->IC_TAR & ~(BIT_CTRL_IC_TAR);
	
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	/*set target address to generate start signal*/
	I2Cx->IC_TAR = (Address & BIT_CTRL_IC_TAR) | tar;
}

/**
  * @brief  Checks whether the specified I2C flag is set or not.
  * @param  I2Cx: where I2Cx can be I2C0_DEV .
  * @param  I2C_FLAG: specifies the flag to check. 
  *   This parameter can be one of the following values:
  *     @arg BIT_IC_STATUS_BUS_BUSY: 
  *     @arg I2C_FLAG_SLV_ACTIVITY: 
  *     @arg I2C_FLAG_MST_ACTIVITY:  
  *     @arg I2C_FLAG_RFF:  
  *     @arg I2C_FLAG_RFNE:  
  *     @arg I2C_FLAG_TFE: 
  *     @arg I2C_FLAG_TFNF: 
  *     @arg I2C_FLAG_ACTIVITY:
  * @retval The new state of I2C_FLAG:
  *          - 1: the specified I2C flag is set
  *          - 0: the specified I2C flag is not set 
  */
u8 I2C_CheckFlagState(I2C_TypeDef *I2Cx, u32 I2C_FLAG)
{
	u8 bit_status = 0;
	
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	if((I2Cx->IC_STATUS & I2C_FLAG) != 0)
	{
		/* I2C_FLAG is set */
		bit_status = 1;
	}

	/* Return the I2C_FLAG status */
	return  bit_status;
}

/**
  * @brief  ENABLE/DISABLE  the I2C's interrupt bits..
  * @param  I2Cx: where I2Cx can be I2C0_DEV .
  * @param  I2C_IT: specifies the I2Cx interrupt sources to be enabled or disabled.
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_IC_INTR_MASK_M_DMA_I2C_DONE: I2C DMA Done Interrupt
  *            @arg BIT_IC_INTR_MASK_M_ADDR_2_MATCH: I2C slave1 Address Match Interrupt
  *            @arg BIT_IC_INTR_MASK_M_ADDR_1_MATCH: I2C slave0 Address Match Interrupt
  *            @arg BIT_IC_INTR_MASK_M_GEN_CALL: General Call Interrupt
  *            @arg BIT_IC_INTR_MASK_M_START_DET: Start or Restart Condition Interrupt
  *            @arg BIT_IC_INTR_MASK_M_STOP_DET: Stop Condition Interrupt
  *            @arg BIT_IC_INTR_MASK_M_ACTIVITY: I2C Activity Interrupt
  *            @arg BIT_IC_INTR_MASK_M_RX_DONE: Slave Transmitter RX Done Interrupt
  *            @arg BIT_IC_INTR_MASK_M_TX_ABRT: Transmit Abort Interrupt
  *            @arg BIT_IC_INTR_MASK_M_RD_REQ: Read Request Interrupt
  *            @arg BIT_IC_INTR_MASK_M_TX_EMPTY: Transmit FIFO Empty Interrupt
  *            @arg BIT_IC_INTR_MASK_M_TX_OVER: Transmit FIFO Over Interrupt
  *            @arg BIT_IC_INTR_MASK_M_RX_FULL: Receive FIFO Full Interrupt
  *            @arg BIT_IC_INTR_MASK_M_RX_OVER: Receive FIFO Over Interrupt
  *            @arg BIT_IC_INTR_MASK_M_RX_UNDER: Receive FIFO Under Interrupt
  * @param  NewState: specifies the state of the interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void I2C_INTConfig(I2C_TypeDef *I2Cx, u32 I2C_IT, u32 NewState)
{
	u32 TempVal = I2Cx->IC_INTR_MASK;
	
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	if (NewState == ENABLE) {
		TempVal |= I2C_IT;
	} else {
		TempVal &= ~I2C_IT;
	}

	I2Cx->IC_INTR_MASK = TempVal;
}

/**
  * @brief  Clears the specified I2C interrupt pending bit.
  * @param  I2Cx: where I2Cx can be I2C0_DEV.
  * @param  INTrBit: specifies the interrupt to be cleared.
  *          This parameter can be one of the following values:
  *            @arg BIT_IC_INTR_STAT_R_ADDR_2_MATCH: I2C slave1 Address Match Interrupt
  *            @arg BIT_IC_INTR_STAT_R_ADDR_1_MATCH: I2C slave0 Address Match Interrupt
  *            @arg BIT_IC_INTR_STAT_R_GEN_CALL: General Call Interrupt
  *            @arg BIT_IC_INTR_STAT_R_START_DET: Start or Restart Condition Interrupt
  *            @arg BIT_IC_INTR_STAT_R_STOP_DET: Stop Condition Interrupt
  *            @arg BIT_IC_INTR_STAT_R_ACTIVITY: I2C Activity Interrupt
  *            @arg BIT_IC_INTR_STAT_R_RX_DONE: Slave Transmitter RX Done Interrupt
  *            @arg BIT_IC_INTR_STAT_R_TX_ABRT: Transmit Abort Interrupt
  *            @arg BIT_IC_INTR_STAT_R_RD_REQ: Read Request Interrupt
  *            @arg BIT_IC_INTR_STAT_R_TX_EMPTY: Transmit FIFO Empty Interrupt
  *            @arg BIT_IC_INTR_STAT_R_TX_OVER: Transmit FIFO Over Interrupt
  *            @arg BIT_IC_INTR_STAT_R_RX_FULL: Receive FIFO Full Interrupt
  *            @arg BIT_IC_INTR_STAT_R_RX_OVER: Receive FIFO Over Interrupt
  *            @arg BIT_IC_INTR_STAT_R_RX_UNDER: Receive FIFO Under Interrupt
  * @note   BIT_IC_INTR_STAT_R_TX_EMPTY is automatically cleared by hardware when the buffer 
  *         level goes above the I2CTXTL threshold.
  *         BIT_IC_INTR_STAT_R_RX_FULL is automatically cleared by hardware when the buffer 
  *         level goes below the I2CRXTL threshold. 
  * @retval None
  */
void I2C_ClearINT(I2C_TypeDef *I2Cx, u32 INTrBit)
{
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	switch (INTrBit) {
		case BIT_IC_INTR_STAT_R_ADDR_2_MATCH:
			I2Cx->IC_CLR_ADDR_MATCH;
		break;
		case BIT_IC_INTR_STAT_R_ADDR_1_MATCH:
			I2Cx->IC_CLR_ADDR_MATCH;
		break;
		case BIT_IC_INTR_STAT_R_GEN_CALL:
			I2Cx->IC_CLR_GEN_CALL;
		break;
		case BIT_IC_INTR_STAT_R_START_DET:
			I2Cx->IC_CLR_START_DET;
		break;
		case BIT_IC_INTR_STAT_R_STOP_DET:
			I2Cx->IC_CLR_STOP_DET;
		break;
		case BIT_IC_INTR_STAT_R_ACTIVITY:
			I2Cx->IC_CLR_ACTIVITY;
		break;
		case BIT_IC_INTR_STAT_R_RX_DONE:
			I2Cx->IC_CLR_RX_DONE;
		break;
		case BIT_IC_INTR_STAT_R_TX_ABRT:
			I2Cx->IC_CLR_TX_ABRT;
		break;
		case BIT_IC_INTR_STAT_R_RD_REQ:
			I2Cx->IC_CLR_RD_REQ;
		break;
		case BIT_IC_INTR_STAT_R_TX_OVER:
			I2Cx->IC_CLR_TX_OVER;
		break;
		case BIT_IC_INTR_STAT_R_RX_OVER:
			I2Cx->IC_CLR_RX_OVER;
		break;
		case BIT_IC_INTR_STAT_R_RX_UNDER:
			I2Cx->IC_CLR_RX_UNDER;
		break;
		case BIT_IC_INTR_STAT_R_TX_EMPTY:
		case BIT_IC_INTR_STAT_R_RX_FULL:
		default:
		break;
	}
}

/**
  * @brief  Clears all of the I2C interrupt pending bit.
  * @param  I2Cx: where I2Cx can be I2C0_DEV .
  * @retval None
  */
void I2C_ClearAllINT(I2C_TypeDef *I2Cx)
{
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	I2Cx->IC_CLR_INTR;
}

/**
  * @brief  Get I2C Raw Interrupt Status.
  * @param  I2Cx: where I2Cx can be I2C0_DEV .
  * @retval raw interrupt status
  */
u32 I2C_GetRawINT(I2C_TypeDef *I2Cx)
{
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	return I2Cx->IC_RAW_INTR_STAT;
}

/**
  * @brief  Get I2C interrupt status.
  * @param  I2Cx: where I2Cx can be I2C0_DEV .
  * @retval interrupt status
  */
u32 I2C_GetINT(I2C_TypeDef *I2Cx)
{
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	return I2Cx->IC_INTR_STAT;
}

/**
  * @brief  Master sends single byte through the I2Cx peripheral according to the set of the upper layer.
  * @param  I2Cx: where I2Cx can be I2C0_DEV .
  * @param  pBuf: point to the data that to be write.
  * @param  I2CCmd: specifies whether a read from FIFO or a write to FIFO is performed.
  * @param  I2CStop: specifies whether a STOP is issued after the byte is sent or received.
  * @param  I2CReSTR: specifies whether a RESTART is issued after the byte is sent or received.
  * @retval None
*/
void I2C_MasterSendNullData(I2C_TypeDef *I2Cx, u8* pBuf, u8  I2CCmd, u8  I2CStop, u8  I2CReSTR)
{
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	I2Cx->IC_DATA_CMD = *(pBuf) |
		(1 << 11) |
		(I2CReSTR << 10) |
		(I2CCmd << 8) |
		(I2CStop << 9);    
}

/**
  * @brief  Master sends single byte through the I2Cx peripheral according to the set of the upper layer.
  * @param  I2Cx: where I2Cx can be I2C0_DEV .
  * @param  pBuf: point to the data that to be write.
  * @param  I2CCmd: specifies whether a read from FIFO or a write to FIFO is performed.
  * @param  I2CStop: specifies whether a STOP is issued after the byte is sent or received.
  * @param  I2CReSTR: specifies whether a RESTART is issued after the byte is sent or received.
  * @retval None
*/
void I2C_MasterSend(I2C_TypeDef *I2Cx, u8* pBuf, u8  I2CCmd, u8  I2CStop, u8  I2CReSTR)
{
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	I2Cx->IC_DATA_CMD = *(pBuf) |
		(I2CReSTR << 10) |
		(I2CCmd << 8) |
		(I2CStop << 9);    
}

/**
  * @brief  Slave sends single byte through the I2Cx peripheral after receiving read request of Master.
  * @param  I2Cx: where I2Cx can be I2C0_DEV.
  * @param  Data: data to be transmitted.
  * @param  StopState: specifies whether a STOP is issued after the byte is sent.
  * @retval None
  */
void I2C_SlaveSend(I2C_TypeDef *I2Cx, u8 Data)
{
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	I2Cx->IC_DATA_CMD = Data;
}

/**
  * @brief  Returns the most recent received data by the I2Cx peripheral.
  * @param  I2Cx: where I2Cx can be I2C0_DEV .
  * @retval The value of the received data.
  */
u8 I2C_ReceiveData(I2C_TypeDef *I2Cx)
{
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	/* Return the data in the DR register */
	return (u8)I2Cx->IC_DATA_CMD;
}

/**
  * @brief  Send data with special length in master mode through the I2Cx peripheral.
  * @param  I2Cx: where I2Cx can be I2C0_DEV.
  * @param  pBuf: point to the data to be transmitted.
  * @param  len: the length of data that to be transmitted.
  * @retval None
  */
void I2C_MasterWrite(I2C_TypeDef *I2Cx, u8* pBuf, u8 len)
{
	u8 cnt = 0;
	
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	/* Write in the DR register the data to be sent */
	for(cnt = 0; cnt < len; cnt++)
	{
		while((I2C_CheckFlagState(I2Cx, BIT_IC_STATUS_TFNF)) == 0);
		
		if(cnt >= len - 1)
		{
			/*generate stop signal*/
			I2Cx->IC_DATA_CMD = (*pBuf++) | (1 << 9);
		}
		else
		{
			I2Cx->IC_DATA_CMD = (*pBuf++);
		}
	}

	while((I2C_CheckFlagState(I2Cx, BIT_IC_STATUS_TFE)) == 0);
}

/**
  * @brief  Send data with special length in master mode through the I2Cx peripheral.
  * @param  I2Cx: where I2Cx can be I2C0_DEV .
  * @param  pBuf: point to the data to be transmitted.
  * @param  len: the length of data that to be transmitted.
  * @retval: The length of data that have been sent to tx fifo
  */
u8 I2C_MasterWriteBrk(I2C_TypeDef *I2Cx, u8* pBuf, u8 len)
{
	u8 cnt = 0;
	
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	/* Write in the DR register the data to be sent */
	for(cnt = 0; cnt < len; cnt++)
	{
		while((I2C_CheckFlagState(I2Cx, BIT_IC_STATUS_TFNF)) == 0);
		
		if(cnt >= len - 1)
		{
			/*generate stop signal*/
			I2Cx->IC_DATA_CMD = (*pBuf++) | (1 << 9);
		}
		else
		{
			I2Cx->IC_DATA_CMD = (*pBuf++);
		}

		while((I2C_CheckFlagState(I2Cx, BIT_IC_STATUS_TFE)) == 0) {
			if(I2C_GetRawINT(I2Cx) & BIT_IC_RAW_INTR_STAT_TX_ABRT) {
				I2C_ClearAllINT(I2Cx);
				return cnt;
			}
		}
	}
	
	return cnt;
}

/**
  * @brief  Read data with special length in master mode through the I2Cx peripheral under DW IP.
  * @note   Under DW IP, master must send two times read cmd:
  *			flow is:
  * 		step 1: master request first data entry
  * 		step 2: slave send first data entry
  * 		step 3: master send seconed read cmd to ack first data and request second data
  * 		step 4: slave send second data
  * 		step 5: master rx full interrupt receive fisrt data and ack second data and request third data.
  * 		loop step 4 and step 5.
  * 		so last slave data have no ack, this is permitted by the spec.
  * @param  I2Cx: where I2Cx can be I2C0_DEV .
  * @param  pBuf: point to the buffer to hold the received data.
  * @param  len: the length of data that to be received.
  * @retval None
  */
void I2C_MasterReadDW(I2C_TypeDef *I2Cx, u8* pBuf, u8 len)
{
	u8 cnt = 0;
	
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	/* read in the DR register the data to be received */
	for(cnt = 0; cnt < len; cnt++)
	{
		if(cnt >= len - 1)
		{
			/* generate stop singal */
			I2Cx->IC_DATA_CMD = 0x0003 << 8;
		}
		else
		{
			I2Cx->IC_DATA_CMD = 0x0001 << 8;
		}

		/* read data */
		if(cnt > 0)
		{
			/* wait for I2C_FLAG_RFNE flag */			
			while((I2C_CheckFlagState(I2Cx, BIT_IC_STATUS_RFNE)) == 0);
			*pBuf++ = (u8)I2Cx->IC_DATA_CMD;
		}
	}

	/* recv last data and NACK */
	while((I2C_CheckFlagState(I2Cx, BIT_IC_STATUS_RFNE)) == 0);
	*pBuf++ = (u8)I2Cx->IC_DATA_CMD;
}

/**
  * @brief  Read data with special length in master mode through the I2Cx peripheral under in-house IP.
  * @param  I2Cx: where I2Cx can be I2C0_DEV .
  * @param  pBuf: point to the buffer to hold the received data.
  * @param  len: the length of data that to be received.
  * @retval The length of data that have received from rx fifo.
  */
u8 I2C_MasterRead(I2C_TypeDef *I2Cx, u8* pBuf, u8 len)
{
	u8 cnt = 0;
	
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	/* read in the DR register the data to be received */
	for(cnt = 0; cnt < len; cnt++)
	{
		if(cnt >= len - 1)
		{
			/* generate stop singal */
			I2Cx->IC_DATA_CMD = 0x0003 << 8;
		}
		else
		{
			I2Cx->IC_DATA_CMD = 0x0001 << 8;
		}

		/* wait for I2C_FLAG_RFNE flag */			
		while((I2C_CheckFlagState(I2Cx, BIT_IC_STATUS_RFNE)) == 0) {
			if(I2C_GetRawINT(I2Cx) & BIT_IC_RAW_INTR_STAT_TX_ABRT) {
				I2C_ClearAllINT(I2Cx);
				return cnt;
			}
		}
		*pBuf++ = (u8)I2Cx->IC_DATA_CMD;
	}

	return cnt;
}

/**
  * @brief  Send data with special length in slave mode through the I2Cx peripheral.
  * @param  I2Cx: where I2Cx can be I2C0_DEV.
  * @param  pBuf: point to the data to be transmitted.
  * @param  len: the length of data that to be transmitted.
  * @retval None
  */
void I2C_SlaveWrite(I2C_TypeDef *I2Cx, u8* pBuf, u8 len)
{
	u8 cnt = 0;
	
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));

	for(cnt = 0; cnt < len; cnt++) {
		/* Check I2C RD Request flag */
		while((I2Cx->IC_RAW_INTR_STAT & BIT_IC_RAW_INTR_STAT_RD_REQ) == 0);

		if (I2C_SLAVEWRITE_PATCH) {
			I2Cx->IC_CLR_RD_REQ;
		}
		/* Check I2C TX FIFO status */
		while((I2C_CheckFlagState(I2Cx, BIT_IC_STATUS_TFNF)) == 0);
		
		I2Cx->IC_DATA_CMD = (*pBuf++);
	}
	while((I2C_CheckFlagState(I2Cx, BIT_IC_STATUS_TFE)) == 0);
}

/**
  * @brief  Read data with special length in slave mode through the I2Cx peripheral.
  * @param  I2Cx: where I2Cx can be I2C0_DEV.
  * @param  pBuf: point to the buffer to hold the received data.
  * @param  len: the length of data that to be received.
  * @retval None
  */
void I2C_SlaveRead(I2C_TypeDef *I2Cx, u8* pBuf, u8 len)
{
	u8 cnt = 0;
	
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	for(cnt = 0; cnt < len; cnt++) {
		/* Check I2C RX FIFO status */
		while((I2C_CheckFlagState(I2Cx, (BIT_IC_STATUS_RFNE | BIT_IC_STATUS_RFF))) == 0);

		*pBuf++ = (u8)I2Cx->IC_DATA_CMD;
	}
}
/**
  * @brief  Sends data and read data in master mode through the I2Cx peripheral.
  * @param  I2Cx: where I2Cx can be I2C0_DEV.
  * @param  pWriteBuf: Byte to be transmitted.
  * @param  Writelen: Byte number to be transmitted.
  * @param  pReadBuf: Byte to be received.
  * @param  Readlen: Byte number to be received.
  * @retval None
  */
void I2C_MasterRepeatRead(I2C_TypeDef* I2Cx, u8* pWriteBuf, u8 Writelen, u8* pReadBuf, u8 Readlen)
{

	u8 cnt = 0;

	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	/* write in the DR register the data to be sent */
	for(cnt = 0; cnt < Writelen; cnt++)
	{
		while((I2C_CheckFlagState(I2Cx, BIT_IC_STATUS_TFNF)) == 0);
		
		if(cnt >= Writelen - 1)
		{
			/*generate restart signal*/
			I2Cx->IC_DATA_CMD = (*pWriteBuf++) | (1 << 10);
		}
		else
		{
			I2Cx->IC_DATA_CMD = (*pWriteBuf++);
		}
	}

	/*Wait I2C TX FIFO not full*/
	while((I2C_CheckFlagState(I2Cx, BIT_IC_STATUS_TFNF)) == 0);
	
	I2C_MasterRead(I2Cx, pReadBuf, Readlen);
}

/**
  * @brief  Enables or disables the specified I2C peripheral, this is one bit register.
  * @param  I2Cx: where I2Cx can be I2C0_DEV .
  * @param  NewState: new state of the I2Cx peripheral. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void I2C_Cmd(I2C_TypeDef *I2Cx, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	if (NewState != DISABLE)
	{
		/* Enable the selected I2C peripheral */
		I2Cx->IC_ENABLE |= BIT_CTRL_IC_ENABLE;
	}
	else
	{
		/* Disable the selected I2C peripheral */
		I2Cx->IC_ENABLE &= ~BIT_CTRL_IC_ENABLE;
	}
}

/**
  * @brief  Enable I2C Tx or Rx DMA.
  * @param  I2Cx: where I2Cx can be I2C0_DEV.
  * @param  DmaCtrl: control Transmit DMA Enable or Receive DMA Enable
  *   This parameter can be one of the following values:
  *     @arg BIT_IC_DMA_CR_TDMAE: 
  *     @arg BIT_IC_DMA_CR_RDMAE:
  * @param  NewState: the new state of the DMA function.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void I2C_DMAControl(I2C_TypeDef *I2Cx, u32 DmaCtrl, u8 NewState)
{
	u32  I2CDMAEn = I2Cx->IC_DMA_CR;
	
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	if (NewState == ENABLE)
		I2CDMAEn |= DmaCtrl;
	else
		I2CDMAEn &= ~DmaCtrl;

	I2Cx->IC_DMA_CR = I2CDMAEn;
}

/**
  * @brief  Initializes the I2Cx Control Register DMA mode.
  * @param  I2Cx: where I2Cx can be I2C0_DEV.
  * @param  I2C_DmaCmd: Command to control the read or write operation and 
  *         the action after the last data transferred.
  *          This parameter can be one or combinations of the following values:
  *            @arg BIT_IC_DMA_CMD_RESTART
  *            @arg BIT_IC_DMA_CMD_STOP
  *            @arg BIT_IC_DMA_CMD_RW
  *            @arg BIT_IC_DMA_CMD_ENABLE
  * @param  I2C_DmaBLen: length of data.
  * 		This parameter must be set to a value in the 0-0xFFFF range.
  * @retval None
  */
void I2C_DmaMode1Config(I2C_TypeDef *I2Cx, u32 I2C_DmaCmd, u32 I2C_DmaBLen)
{
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	assert_param(IS_I2C_DMA_DATA_LEN(I2C_DmaBLen));
	
	I2Cx->IC_DMA_MOD = I2C_DMA_REGISTER;
	I2Cx->IC_DMA_DAT_LEN = I2C_DmaBLen;
	I2Cx->IC_DMA_CMD = I2C_DmaCmd;
}

/**
  * @brief  Initializes the I2C Discripter DMA mode.
  * @param  I2Cx: where I2Cx can be I2C0_DEV .
  * @param  I2C_DmaCmd: set BIT[0] of IC_DMA_CMD to enable DMA mode.
  * @param  I2C_DmaBLen: length of data.
  * 		This parameter must be set to a value in the 0-0xFF range.
  * @retval None
  */
void I2C_DmaMode2Config(I2C_TypeDef *I2Cx, u32 I2C_DmaCmd, u32 I2C_DmaBLen)
{
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	assert_param(IS_I2C_DMA_DATA_LEN(I2C_DmaBLen));

	I2Cx->IC_DMA_MOD = I2C_DMA_DESCRIPTOR;
	I2Cx->IC_DMA_CMD = I2C_DmaCmd;
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
BOOL I2C_TXGDMA_Init(
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
	GDMA_InitStruct->GDMA_DstHandshakeInterface = I2C_DEV_TABLE[Index].Tx_HandshakeInterface;
	GDMA_InitStruct->GDMA_DstAddr = (u32)&I2C_DEV_TABLE[Index].I2Cx->IC_DATA_CMD;
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
  * @param  I2C Index: 0.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains 
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pRxBuf: Rx Buffer.
  * @param  RxCount: Rx Count.
  * @retval   TRUE/FLASE
  * @note support Master or Slave RXDMA
  */
BOOL I2C_RXGDMA_Init(
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
	GDMA_InitStruct->GDMA_SrcHandshakeInterface  = I2C_DEV_TABLE[Index].Rx_HandshakeInterface;
	GDMA_InitStruct->GDMA_SrcAddr = (u32)&I2C_DEV_TABLE[Index].I2Cx->IC_DATA_CMD;
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
  * @param  I2Cx: where I2Cx can be I2C0_DEV.
  * @param  NewState: the new state of the Rx Path.
  *              This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void I2C_Sleep_Cmd(I2C_TypeDef *I2Cx, u32 NewStatus)
{
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	if (NewStatus != DISABLE) {
		I2C_INTConfig(I2Cx, BIT_IC_INTR_MASK_M_ADDR_1_MATCH|BIT_IC_INTR_MASK_M_ADDR_2_MATCH, ENABLE);

		I2Cx->IC_SLEEP |= BIT_IC_SLEEP_CLOCK_CONTROL;
	} else {
		I2C_INTConfig(I2Cx, BIT_IC_INTR_MASK_M_ADDR_1_MATCH|BIT_IC_INTR_MASK_M_ADDR_2_MATCH, DISABLE);

		I2Cx->IC_SLEEP &= ~BIT_IC_SLEEP_CLOCK_CONTROL;
	}
}

/**
  * @brief  Wake up clock domain of Clock-gated I2C after address matching interrupts.
  * @param  I2Cx: where I2Cx can be I2C0_DEV.
  * @retval None
  */
void I2C_WakeUp(I2C_TypeDef *I2Cx)
{
	/* Check the parameters */
	assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	
	I2C_ClearINT(I2Cx, BIT_IC_INTR_STAT_R_ADDR_2_MATCH);
	I2C_ClearINT(I2Cx, BIT_IC_INTR_STAT_R_ADDR_1_MATCH);

	I2C_Sleep_Cmd(I2Cx, DISABLE);
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
