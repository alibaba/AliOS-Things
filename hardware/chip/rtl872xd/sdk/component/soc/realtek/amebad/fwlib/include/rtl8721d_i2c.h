/**
  ******************************************************************************
  * @file    rtl8721d_i2c.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the I2C firmware
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

#ifndef _RTL8710B_I2C_H_
#define _RTL8710B_I2C_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @addtogroup I2C I2C
  * @{
  */
 
/** @addtogroup I2C
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * I2C0:
  *		- Base Address: I2C0_DEV
  *		- IPclk: 62.5Mhz
  *		- Speed: Standard (up to 100 kHz) and Fast (up to 400 kHz) Modes
  *		- Address: 7/10-bit Address Mode 
  *		- SocPs: SleepMode (clock gating & power gating)
  *		- Slave: Slave0 
  *		- IRQ: I2C0_IRQ
  *		- GDMA TX handshake interface: GDMA_HANDSHAKE_INTERFACE_I2C0_TX
  *		- GDMA RX handshake interface: GDMA_HANDSHAKE_INTERFACE_I2C0_RX
  *
  *****************************************************************************************     
  * How to use Normal I2C
  *****************************************************************************************
  *      To use the normal i2c mode, the following steps are mandatory:
  *       
  *      1. Enable peripheral clock using the follwoing functions.(it is enabled by default)
  *			RCC_PeriphClockCmd(APBPeriph_I2Cx, APBPeriph_I2Cx_CLOCK, ENABLE);
  *
  *      2. configure the I2C pinmux.
  *			Pinmux_Config(Pin_Num, PINMUX_FUNCTION_I2C)
  *        
  *      3. Program Role, Address Mode, Speed Mode, I2C CLK, Slave address, Threshold, Feature Supports
  *			I2C_StructInit()
  *
  *      4. Init Hardware use step3 parameters:
  *			I2C_Init(I2C_TypeDef *I2Cx, I2C_InitTypeDef* I2C_InitStruct)
  * 
  *      5. Enable the NVIC and the corresponding interrupt using following function if you need 
  *			to use interrupt mode. 
  *			I2C_INTConfig(): I2C IRQ Mask set
  *			InterruptRegister(): register the i2c irq handler 
  *			InterruptEn(): Enable the NVIC interrupt
  *
  *      6. Enable i2c module using I2C_Cmd().
  *
  *****************************************************************************************      
  * How to use i2c in DMA Register mode
  *****************************************************************************************   
  *      To use the i2c in DMA mode, the following steps are mandatory:
  *       
  *      1. Enable peripheral clock using the follwoing functions.(it is enabled by default)
  *			RCC_PeriphClockCmd(APBPeriph_I2Cx, APBPeriph_I2Cx_CLOCK, ENABLE);
  *
  *      2. configure the I2C pinmux.
  *			Pinmux_Config(Pin_Num, PINMUX_FUNCTION_I2C)
  *        
  *      3. Program Role, Address Mode, Speed Mode, I2C CLK, Slave address, Threshold, Feature Supports
  *			I2C_StructInit()
  *
  *      4. Init Hardware use step3 parameters:
  *			I2C_Init(I2C_TypeDef *I2Cx, I2C_InitTypeDef* I2C_InitStruct)
  *
  *      5. Enable i2c module using I2C_Cmd().
  *
  *      6. GDMA related configurations(source address/destination address/block size etc.)
  *			I2C_TXGDMA_Init():Init and Enable I2C TX GDMA
  *			I2C_RXGDMA_Init():Init and Enable I2C RX GDMA
  *
  *      7. I2C DMA Mode Register mode set.
  *			I2C_DmaMode1Config():Configures the I2Cx Control Register DMA mode
  *          
  *      8. Active the I2C TX/RX DMA Request using I2C_DMAControl().
  *
  *****************************************************************************************    
  * How to use i2c in Sleep mode
  *****************************************************************************************    
  *      To use the i2c in Low Power mode, the following steps are mandatory:
  *       
  *      1. Enable peripheral clock using the follwoing functions.(it is enabled by default)
  *			RCC_PeriphClockCmd(APBPeriph_I2Cx, APBPeriph_I2Cx_CLOCK, ENABLE);
  *
  *      2. configure the I2C pinmux.
  *			Pinmux_Config(Pin_Num, PINMUX_FUNCTION_I2C)
  *        
  *      3. Program Role, Address Mode, Speed Mode, I2C CLK, Slave address, Threshold, Feature Supports
  *			I2C_StructInit()
  *
  *      4. Init Hardware use step3 parameters:
  *			I2C_Init(I2C_TypeDef *I2Cx, I2C_InitTypeDef* I2C_InitStruct)
  * 
  *      5. Enable the NVIC and the corresponding interrupt using following function if you need 
  *			to use interrupt mode. 
  *			I2C_INTConfig(): I2C IRQ Mask set
  *			InterruptRegister(): register the i2c irq handler 
  *			InterruptEn(): Enable the NVIC interrupt
  *    
  *      6. Enable address matching interrupts for wake up
  *			I2C_INTConfig(): I2C Addr match IRQ Mask set 
  *			BIT_IC_INTR_MASK_M_ADDR_1_MATCH refer to refer to I2C Slave0 Address Match
  *
  *      7. Set wake up event using the follwoing functions.
  *			SOCPS_SetWakeEvent()
  *
  *      8. Set power ext option BIT_SYSON_PMOPT_SLP_ANACK_EN to Enable ANA clock and 
  *			BIT_SYSON_PMOPT_SLP_ANACK_SEL to select 4M clock for power save mode, then hardware  
  *			will automatically switch to the 4M clock when enter sleep state.
  *			SOCPS_PWROptionExt()
  *          
  *      9. Clear address matching interrupts after address matching interrupts
  *			I2C_WakeUp()
  *
  *****************************************************************************************     
  * @endverbatim
  */
  
/* Exported types --------------------------------------------------------*/
/** @defgroup I2C_Exported_Types I2C Exported Types
  * @{
  */

/** 
  * @brief  I2C Init structure definition
  */
typedef struct {
	u32	I2CIdx;         /*!< Specifies the I2C Device Index.
				This parameter should be 0 */
					
	u32	I2CMaster;      /*!< Specifies the I2C Role.
				This parameter can be a value of @ref I2C_Role_definitions */
					
	u32	I2CAddrMod;     /*!< Specifies the I2C Addressing Mode.
				This parameter can be a value of @ref I2C_Addr_Mode_definitions */
					
	u32	I2CSpdMod;      /*!< Specifies the I2C Speed Mode. Now the circuit don't support High Speed Mode.
				This parameter can be a value of @ref I2C_Speed_Mode_definitions */	
					
	u32	I2CRXTL;        /*!< Specifies the I2C RX FIFO Threshold. It controls the level of
				entries(or above) that triggers the RX_FULL interrupt.
				This parameter must be set to a value in the 0-255 range. A value of 0 sets 
				the threshold for 1 entry, and a value of 255 sets the threshold for 256 entry*/
				
	u32	I2CTXTL;        /*!< Specifies the I2C TX FIFO Threshold.It controls the level of
				entries(or below) that triggers the TX_EMPTY interrupt.
				This parameter must be set to a value in the 0-255 range. A value of 0 sets 
				the threshold for 0 entry, and a value of 255 sets the threshold for 255 entry*/
	u32	I2CMstReSTR;    /*!< Specifies the I2C Restart Support of Master. */
				
	u32	I2CMstGC;       /*!< Specifies the I2C General Call Support of Master. */
				
	u32	I2CMstStartB;   /*!< Specifies the I2C Start Byte Support of Master. */
				
	u32	I2CSlvNoAck;    /*!< Specifies the I2C Slave No Ack Support. */
				
 	u32	I2CSlvAckGC;    /*!< Specifies the I2C Slave Acks to General Call. */ 
	
	u32	I2CAckAddr;     /*!< Specifies the I2C Target Address in I2C Master Mode or
				Ack Address in I2C Slave0 Mode.
				This parameter must be set to a value in the 0-127 range if the I2C_ADDR_7BIT
				is selected or 0-1023 range if the I2C_ADDR_10BIT is selected. */
				
	u32	I2CSlvSetup;       /*!< Specifies the I2C SDA Setup Time. It controls the amount of time delay
				introduced in the rising edge of SCL¡ªrelative to SDA changing¡ªby holding SCL low
				when I2C Device operating as a slave transmitter, in units of ic_clk period.
				This parameter must be set to a value in the 0-255 range. It must be set larger than I2CSdaHd */
				
	u32	I2CSdaHd;       /*!< Specifies the I2C SDA Hold Time. It controls the amount of 
				hold time on the SDA signal after a negative edge of SCL in both master
				and slave mode, in units of ic_clk period.
				This parameter must be set to a value in the 0-0xFFFF range. */
					
	u32	I2CClk;         /*!< Specifies the I2C Bus Clock (in kHz). It is closely related to I2CSpdMod */
				
	u32	I2CIPClk;		/*!< Specifies the I2C IP Clock (in Hz). */
					
	u32	I2CFilter;      /*!< Specifies the I2C SCL/SDA Spike Filter. */

	u32	I2CTxDMARqLv;   /*!< Specifies the I2C TX DMA Empty Level. dma_tx_req signal is generated when
				the number of valid data entries in the transmit FIFO is equal to or below the DMA 
				Transmit Data Level Register. The value of DMA Transmit Data Level Register is equal 
				to this value. This parameter must be set to a value in the 0-31 range. */
				
	u32	I2CRxDMARqLv;   /*!< Specifies the I2C RX DMA Full Level. dma_rx_req signal is generated when
				the number of valid data entries in the transmit FIFO is equal to or above the DMA  
				Receive Data Level Register. The value of DMA Receive Data Level Register is equal to 
				this value+1. This parameter must be set to a value in the 0-31 range. */ 
				
	u32	I2CDMAMod;     	/*!< Specifies the I2C DMA Mode.
				This parameter can be a value of @ref I2C_DMA_Mode_definitions */
					
	u32	I2CAckAddr1;    /*!< Specifies the I2C Ack Address in I2C Slave1 Mode. I2C Slave1 only
				support I2C_ADDR_7BIT mode. This parameter must be set to a value in the 0-127 range. */   
}I2C_InitTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup I2C_Exported_Constants I2C Exported Constants
  * @{
  */

/** @defgroup I2C_Peripheral_definitions 
  * @{
  */
#define IS_I2C_ALL_PERIPH(PERIPH) ((PERIPH) == I2C0_DEV)
/**
  * @}
  */

/** @defgroup I2C_Addr_Mode_definitions 
  * @{
  */
#define I2C_ADDR_7BIT			((u32)0x00000000)
#define I2C_ADDR_10BIT			((u32)0x00000001)
#define IS_I2C_ADDR_MODE(MODE) (((MODE) == I2C_ADDR_7BIT) || \
                                   ((MODE) == I2C_ADDR_10BIT))
/**
  * @}
  */
  
/** @defgroup I2C_Speed_Mode_definitions 
  * @{
  */
#define I2C_SS_MODE				((u32)0x00000001)
#define I2C_FS_MODE				((u32)0x00000002)
#define I2C_HS_MODE				((u32)0x00000003)
#define IS_I2C_SPEED_MODE(MODE) (((MODE) == I2C_SS_MODE) || \
                                   ((MODE) == I2C_FS_MODE) || \
                                   ((MODE) == I2C_HS_MODE))
/**
  * @}
  */

/** @defgroup I2C_Role_definitions 
  * @{
  */
#define I2C_SLAVE_MODE			((u32)0x00000000)
#define I2C_MASTER_MODE		((u32)0x00000001)
/**
  * @}
  */

/** @defgroup I2C_DMA_Mode_definitions 
  * @{
  */
#define I2C_DMA_LEGACY			((u32)0x00000000)
#define I2C_DMA_REGISTER		((u32)0x00000001)
#define I2C_DMA_DESCRIPTOR		((u32)0x00000002)
#define IS_I2C_DMA_MODE(MODE) (((MODE) == I2C_DMA_LEGACY) || \
                                   ((MODE) == I2C_DMA_REGISTER) || \
                                   ((MODE) == I2C_DMA_DESCRIPTOR))
/**
  * @}
  */

/** @defgroup I2C_DMA_DATA_LENGTH 
  * @{
  */ 
#define IS_I2C_DMA_DATA_LEN(LENGTH)   ((LENGTH) <= 0xFFFF)
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup I2C_Exported_Functions I2C Exported Functions
  * @{
  */

/** @defgroup I2C_Exported_Normal_Functions I2C Exported Normal Functions
  * @{
  */
_LONG_CALL_ void I2C_Init(I2C_TypeDef *I2Cx, I2C_InitTypeDef* I2C_InitStruct);
_LONG_CALL_ void I2C_Cmd(I2C_TypeDef *I2Cx, u8 NewState);
_LONG_CALL_ void I2C_ClearAllINT(I2C_TypeDef *I2Cx);
_LONG_CALL_ u32 I2C_GetRawINT(I2C_TypeDef *I2Cx);
_LONG_CALL_ u32 I2C_GetINT(I2C_TypeDef *I2Cx);
_LONG_CALL_ u8 I2C_CheckFlagState(I2C_TypeDef *I2Cx, u32 I2C_FLAG);
_LONG_CALL_ void I2C_INTConfig(I2C_TypeDef *I2Cx, u32 I2C_IT, u32 NewState);
_LONG_CALL_ void I2C_ClearINT(I2C_TypeDef *I2Cx, u32 INTrAddr);
_LONG_CALL_ void I2C_SetSpeed(I2C_TypeDef *I2Cx, u32 SpdMd, u32 I2Clk, u32 I2CIPClk);
_LONG_CALL_ void I2C_StructInit(I2C_InitTypeDef* I2C_InitStruct);
_LONG_CALL_ u8 I2C_ReceiveData(I2C_TypeDef *I2Cx);
/**
  * @}
  */

/** @defgroup I2C_Exported_Master_Functions I2C Exported Master Functions
  * @{
  */
_LONG_CALL_ void I2C_MasterSendNullData(I2C_TypeDef *I2Cx, u8* pBuf, u8  I2CCmd, u8  I2CStop, u8  I2CReSTR);
_LONG_CALL_ void I2C_MasterSend(I2C_TypeDef *I2Cx, u8* pBuf, u8  I2CCmd, u8  I2CStop, u8  I2CReSTR);
_LONG_CALL_ void I2C_MasterWrite(I2C_TypeDef *I2Cx, u8* pBuf, u8 len);
_LONG_CALL_ void I2C_MasterReadDW(I2C_TypeDef *I2Cx, u8* pBuf, u8 len);
_LONG_CALL_ u8 I2C_MasterRead(I2C_TypeDef *I2Cx, u8* pBuf, u8 len);
_LONG_CALL_ void I2C_MasterRepeatRead(I2C_TypeDef* I2Cx, u8* pWriteBuf, u8 Writelen, u8* pReadBuf, u8 Readlen);
_LONG_CALL_ void I2C_SetSlaveAddress(I2C_TypeDef *I2Cx, u16 Address);
/**
  * @}
  */

/** @defgroup I2C_Exported_Slave_Functions I2C Exported Slave Functions
  * @{
  */
_LONG_CALL_ void I2C_SlaveWrite(I2C_TypeDef *I2Cx, u8* pBuf, u8 len);
_LONG_CALL_ void I2C_SlaveRead(I2C_TypeDef *I2Cx, u8* pBuf, u8 len);
_LONG_CALL_ void I2C_SlaveSend(I2C_TypeDef *I2Cx, u8 Data);
/**
  * @}
  */

/** @defgroup I2C_Exported_DMA_Functions I2C Exported DMA Functions
  * @{
  */
_LONG_CALL_ void I2C_DMAControl(I2C_TypeDef *I2Cx, u32 DmaCtrl, u8 NewState);
_LONG_CALL_ void I2C_DmaMode1Config(I2C_TypeDef *I2Cx, u32 I2C_DmaCmd, u32 I2C_DmaBLen);
_LONG_CALL_ void I2C_DmaMode2Config(I2C_TypeDef *I2Cx, u32 I2C_DmaCmd, u32 I2C_DmaBLen);
_LONG_CALL_ BOOL I2C_TXGDMA_Init(u8 Index, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pTxBuf, int TxCount);
_LONG_CALL_ BOOL I2C_RXGDMA_Init(u8 Index, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pRxBuf, int RxCount);
/**
  * @}
  */
 
/** @defgroup I2C_Exported_PowerSave_Functions I2C Exported PowerSave Functions
  * @{
  */
_LONG_CALL_ void I2C_Sleep_Cmd(I2C_TypeDef *I2Cx, u32 NewStatus);
_LONG_CALL_ void I2C_WakeUp(I2C_TypeDef *I2Cx);
/**
  * @}
  */

/**
  * @}
  */
  

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup I2C_Register_Definitions I2C Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup IC_CON
 * @{
 *****************************************************************************/
#define BIT_CTRL_IC_CON_IC_SLAVE_DISABLE_1		((u32)0x00000001 << 7)
#define BIT_CTRL_IC_CON_IC_SLAVE_DISABLE			((u32)0x00000001 << 6)
#define BIT_CTRL_IC_CON_IC_RESTART_EN				((u32)0x00000001 << 5)
#define BIT_CTRL_IC_CON_IC_10BITADDR_SLAVE		((u32)0x00000001 << 3)
#define BIT_CTRL_IC_CON_SPEED						((u32)0x00000003 << 1)
#define BIT_CTRL_IC_CON_MASTER_MODE				((u32)0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup IC_TAR
 * @{
 *****************************************************************************/
#define BIT_CTRL_IC_TAR_IC_10BITADDR_MASTER		((u32)0x00000001 << 12)
#define BIT_CTRL_IC_TAR_SPECIAL					((u32)0x00000001 << 11)
#define BIT_CTRL_IC_TAR_GC_OR_START				((u32)0x00000001 << 10)
#define BIT_CTRL_IC_TAR								((u32)0x000003ff)
/** @} */

/**************************************************************************//**
 * @defgroup IC_SAR
 * @{
 *****************************************************************************/
#define BIT_CTRL_IC_SAR								((u32)0x000003ff)
/** @} */

/**************************************************************************//**
 * @defgroup IC_DATA_CMD
 * @{
 *****************************************************************************/
#define BIT_CTRL_IC_DATA_CMD_NULLDATA				((u32)0x00000001 << 11)
#define BIT_CTRL_IC_DATA_CMD_RESTART				((u32)0x00000001 << 10)
#define BIT_CTRL_IC_DATA_CMD_STOP					((u32)0x00000001 << 9)
#define BIT_CTRL_IC_DATA_CMD_CMD					((u32)0x00000001 << 8)
#define BIT_CTRL_IC_DATA_CMD_DAT					((u32)0x000000ff)
/** @} */

/**************************************************************************//**
 * @defgroup IC_INTR_STAT
 * @{
 *****************************************************************************/
#define BIT_IC_INTR_STAT_R_DMA_I2C_DONE			((u32)0x00000001 << 15)
#define BIT_IC_INTR_STAT_R_ADDR_2_MATCH			((u32)0x00000001 << 13) /* refer to I2C Slave1 Address Match */
#define BIT_IC_INTR_STAT_R_ADDR_1_MATCH			((u32)0x00000001 << 12) /* refer to I2C Slave0 Address Match */
#define BIT_IC_INTR_STAT_R_GEN_CALL           			((u32)0x00000001 << 11)
#define BIT_IC_INTR_STAT_R_START_DET          		((u32)0x00000001 << 10)
#define BIT_IC_INTR_STAT_R_STOP_DET           			((u32)0x00000001 << 9)
#define BIT_IC_INTR_STAT_R_ACTIVITY           			((u32)0x00000001 << 8)
#define BIT_IC_INTR_STAT_R_RX_DONE            			((u32)0x00000001 << 7)
#define BIT_IC_INTR_STAT_R_TX_ABRT            			((u32)0x00000001 << 6)
#define BIT_IC_INTR_STAT_R_RD_REQ             			((u32)0x00000001 << 5)
#define BIT_IC_INTR_STAT_R_TX_EMPTY           			((u32)0x00000001 << 4)
#define BIT_IC_INTR_STAT_R_TX_OVER            			((u32)0x00000001 << 3)
#define BIT_IC_INTR_STAT_R_RX_FULL            			((u32)0x00000001 << 2)
#define BIT_IC_INTR_STAT_R_RX_OVER            			((u32)0x00000001 << 1)
#define BIT_IC_INTR_STAT_R_RX_UNDER           			((u32)0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup IC_INTR_MASK
 * @{
 *****************************************************************************/
#define BIT_IC_INTR_MASK_M_DMA_I2C_DONE			((u32)0x00000001 << 15)
#define BIT_IC_INTR_MASK_M_ADDR_2_MATCH         	((u32)0x00000001 << 13) /* refer to I2C Slave1 Address Match */
#define BIT_IC_INTR_MASK_M_ADDR_1_MATCH         	((u32)0x00000001 << 12) /* refer to I2C Slave0 Address Match */
#define BIT_IC_INTR_MASK_M_GEN_CALL           		((u32)0x00000001 << 11)
#define BIT_IC_INTR_MASK_M_START_DET          		((u32)0x00000001 << 10)
#define BIT_IC_INTR_MASK_M_STOP_DET           		((u32)0x00000001 << 9)
#define BIT_IC_INTR_MASK_M_ACTIVITY           			((u32)0x00000001 << 8)
#define BIT_IC_INTR_MASK_M_RX_DONE            			((u32)0x00000001 << 7)
#define BIT_IC_INTR_MASK_M_TX_ABRT            			((u32)0x00000001 << 6)
#define BIT_IC_INTR_MASK_M_RD_REQ             			((u32)0x00000001 << 5)
#define BIT_IC_INTR_MASK_M_TX_EMPTY           		((u32)0x00000001 << 4)
#define BIT_IC_INTR_MASK_M_TX_OVER            			((u32)0x00000001 << 3)
#define BIT_IC_INTR_MASK_M_RX_FULL            			((u32)0x00000001 << 2)
#define BIT_IC_INTR_MASK_M_RX_OVER           		 	((u32)0x00000001 << 1)
#define BIT_IC_INTR_MASK_M_RX_UNDER           		((u32)0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup IC_RAW_INTR_STAT
 * @{
 *****************************************************************************/
#define BIT_IC_RAW_INTR_STAT_DMA_I2C_DONE		((u32)0x00000001 << 15)
#define BIT_IC_RAW_INTR_STAT_ADDR_2_MATCH		((u32)0x00000001 << 13) /* refer to I2C Slave1 Address Match */
#define BIT_IC_RAW_INTR_STAT_ADDR_1_MATCH		((u32)0x00000001 << 12) /* refer to I2C Slave0 Address Match */
#define BIT_IC_RAW_INTR_STAT_GEN_CALL			((u32)0x00000001 << 11)
#define BIT_IC_RAW_INTR_STAT_START_DET			((u32)0x00000001 << 10)
#define BIT_IC_RAW_INTR_STAT_STOP_DET         		((u32)0x00000001 << 9)
#define BIT_IC_RAW_INTR_STAT_ACTIVITY         		((u32)0x00000001 << 8)
#define BIT_IC_RAW_INTR_STAT_RX_DONE          		((u32)0x00000001 << 7)
#define BIT_IC_RAW_INTR_STAT_TX_ABRT          		((u32)0x00000001 << 6)
#define BIT_IC_RAW_INTR_STAT_RD_REQ           		((u32)0x00000001 << 5)
#define BIT_IC_RAW_INTR_STAT_TX_EMPTY         		((u32)0x00000001 << 4)
#define BIT_IC_RAW_INTR_STAT_TX_OVER          		((u32)0x00000001 << 3)
#define BIT_IC_RAW_INTR_STAT_RX_FULL          		((u32)0x00000001 << 2)
#define BIT_IC_RAW_INTR_STAT_RX_OVER          		((u32)0x00000001 << 1)
#define BIT_IC_RAW_INTR_STAT_RX_UNDER         		((u32)0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup IC_ENABLE
 * @{
 *****************************************************************************/
#define BIT_CTRL_IC_ENABLE							((u32)0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup IC_STATUS
 * @{
 *****************************************************************************/
#define BIT_IC_STATUS_BUS_BUSY            			((u32)0x00000001 << 7)
#define BIT_IC_STATUS_SLV_ACTIVITY            			((u32)0x00000001 << 6)
#define BIT_IC_STATUS_MST_ACTIVITY            			((u32)0x00000001 << 5)
#define BIT_IC_STATUS_RFF                     				((u32)0x00000001 << 4)
#define BIT_IC_STATUS_RFNE                    				((u32)0x00000001 << 3)
#define BIT_IC_STATUS_TFE                     				((u32)0x00000001 << 2)
#define BIT_IC_STATUS_TFNF                    				((u32)0x00000001 << 1)
#define BIT_IC_STATUS_ACTIVITY                			((u32)0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup IC_SDA_HOLD
 * @{
 *****************************************************************************/
#define BIT_CTRL_IC_SDA_HOLD						((u32)0x0000ffff)
/** @} */

/**************************************************************************//**
 * @defgroup IC_TX_ABRT_SOURCE
 * @{
 *****************************************************************************/
#define BIT_IC_TX_ABRT_SOURCE_ABRT_SLVRD_INTX 		((u32)0x00000001 << 15)
#define BIT_IC_TX_ABRT_SOURCE_ABRT_SLV_ARBLOST		((u32)0x00000001 << 14)
#define BIT_IC_TX_ABRT_SOURCE_ABRT_SLVFLUSH_TXFIFO	((u32)0x00000001 << 13)
#define BIT_IC_TX_ABRT_SOURCE_ARB_LOST        			((u32)0x00000001 << 12)
#define BIT_IC_TX_ABRT_SOURCE_ABRT_MASTER_DIS 		((u32)0x00000001 << 11)
#define BIT_IC_TX_ABRT_SOURCE_ABRT_10B_RD_NORSTRT	((u32)0x00000001 << 10)
#define BIT_IC_TX_ABRT_SOURCE_ABRT_SBYTE_NORSTRT	((u32)0x00000001 << 9)
#define BIT_IC_TX_ABRT_SOURCE_ABRT_HS_NORSTRT 		((u32)0x00000001 << 8)
#define BIT_IC_TX_ABRT_SOURCE_ABRT_SBYTE_ACKDET		((u32)0x00000001 << 7)
#define BIT_IC_TX_ABRT_SOURCE_ABRT_HS_ACKDET		((u32)0x00000001 << 6)
#define BIT_IC_TX_ABRT_SOURCE_ABRT_GCALL_READ 		((u32)0x00000001 << 5)
#define BIT_IC_TX_ABRT_SOURCE_ABRT_GCALL_NOACK		((u32)0x00000001 << 4)
#define BIT_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK	((u32)0x00000001 << 3)
#define BIT_IC_TX_ABRT_SOURCE_ABRT_10ADDR2_NOACK	((u32)0x00000001 << 2)
#define BIT_IC_TX_ABRT_SOURCE_ABRT_10ADDR1_NOACK	((u32)0x00000001 << 1)
#define BIT_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK	((u32)0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup IC_SLV_DATA_NACK_ONLY
 * @{
 *****************************************************************************/
#define BIT_CTRL_IC_SLV_DATA_NACK_ONLY				((u32)0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup IC_DMA_CR
 * @{
 *****************************************************************************/
#define BIT_IC_DMA_CR_TDMAE                   				((u32)0x00000001 << 1)
#define BIT_IC_DMA_CR_RDMAE                   				((u32)0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup IC_SDA_SETUP
 * @{
 *****************************************************************************/
#define BIT_CTRL_IC_SDA_SETUP              					((u32)0x000000ff)
/** @} */

/**************************************************************************//**
 * @defgroup IC_ACK_GENERAL_CALL
 * @{
 *****************************************************************************/
#define BIT_CTRL_IC_ACK_GENERAL_CALL					((u32)0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup IC_DMA_CMD
 * @{
 *****************************************************************************/
#define BIT_IC_DMA_CMD_RESTART						((u32)0x00000001 << 7)
#define BIT_IC_DMA_CMD_STOP							((u32)0x00000001 << 6)
#define BIT_IC_DMA_CMD_RW								((u32)0x00000001 << 5) /* 0 is write, 1 is read */
#define BIT_IC_DMA_CMD_ENABLE							((u32)0x00000001) /* HW auto clear after transfer done */
/** @} */

/**************************************************************************//**
 * @defgroup IC_DMA_MOD
 * @{
 *****************************************************************************/
#define BIT_IC_DMA_MOD									((u32)0x00000003)
/** @} */

/**************************************************************************//**
 * @defgroup IC_SLEEP
 * @{
 *****************************************************************************/
#define BIT_IC_SLEEP_CLOCK_GATED						((u32)0x00000001 << 1)
#define BIT_IC_SLEEP_CLOCK_CONTROL					((u32)0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup IC_FILTER
 * @{
 *****************************************************************************/
#define BIT_IC_FILTER_DIG_FLTR_SEL						((u32)0x00000001 << 8)
#define BIT_CTRL_IC_FILTER_DIG_FLTR_DEG				((u32)0x0000000F)
/** @} */

/**************************************************************************//**
 * @defgroup IC_SAR1
 * @{
 *****************************************************************************/
#define BIT_CTRL_IC_SAR1								((u32)0x0000007F)
/** @} */
/** @} */

/**
  * @}
  */

/**
  * @}
  */

/* Other Definitions --------------------------------------------------------*/
//I2C Timing Parameters
#define I2C_SS_MIN_SCL_HTIME		4000    //the unit is ns.
#define I2C_SS_MIN_SCL_LTIME		4700    //the unit is ns.

#define I2C_FS_MIN_SCL_HTIME		600     //the unit is ns.
#define I2C_FS_MIN_SCL_LTIME		1300    //the unit is ns.

#define I2C_HS_MIN_SCL_HTIME_100    60      //the unit is ns, with bus loading = 100pf
#define I2C_HS_MIN_SCL_LTIME_100    120     //the unit is ns., with bus loading = 100pf

#define I2C_HS_MIN_SCL_HTIME_400    160     //the unit is ns, with bus loading = 400pf
#define I2C_HS_MIN_SCL_LTIME_400    320     //the unit is ns., with bus loading = 400pf

typedef struct
{
	I2C_TypeDef* I2Cx;
	u32 Tx_HandshakeInterface;
	u32 Rx_HandshakeInterface;
	IRQn_Type IrqNum;
} I2C_DevTable;

extern const I2C_DevTable I2C_DEV_TABLE[1];
extern u32 I2C_SLAVEWRITE_PATCH;
extern u32 IC_FS_SCL_HCNT_TRIM;
extern u32 IC_FS_SCL_LCNT_TRIM;

#endif

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
