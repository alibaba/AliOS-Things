/**
  ******************************************************************************
  * @file    rtl8721d_usi_i2c.h
  * @author
  * @version V1.0.0
  * @date    2017-12-18
  * @brief   This file contains all the functions prototypes for the I2C firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8721D_USI_I2C_H_
#define _RTL8721D_USI_I2C_H_

#include "rtl8721d_usi.h"

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @addtogroup USI-I2C
  * @{
  */
 
/** @addtogroup USI-I2C
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * USI_I2C:
  *		- Base Address: USI0_DEV
  *		- IPclk: 50Mhz
  *		- Speed: Standard (up to 100 kHz) , Fast (up to 400 kHz), High (up to 3.33 MHz) mode
  *		- Address: 7/10-bit Address Mode
  *		- IRQ: USI_IRQ
  *		- GDMA TX handshake interface: GDMA_HANDSHAKE_INTERFACE_USI0_TX
  *		- GDMA RX handshake interface: GDMA_HANDSHAKE_INTERFACE_USI0_RX
  *
  *****************************************************************************************     
  * How to use USI_I2C
  *****************************************************************************************
  *      To use the normal USI_I2C mode, the following steps are mandatory:
  *       
  *      1. Enable peripheral clock using the follwoing functions.(it is enabled by default)
  *			RCC_PeriphClockCmd(APBPeriph_USI_REG, APBPeriph_USI_CLOCK, ENABLE);
  *
  *      2. configure the I2C pinmux.
  *			Pinmux_Config(Pin_Num, PINMUX_FUNCTION_I2C)
  *        
  *      3. Program Role, Address Mode, Speed Mode, USI_I2C CLK, Slave address, Threshold, Feature Supports
  *			USI_I2C_StructInit()
  *
  *      4. Init Hardware use step3 parameters:
  *			USI_I2C_Init(USI_TypeDef *USIx, USI_I2C_InitTypeDef* USI_I2C_InitStruct)
  * 
  *      5. Enable the NVIC and the corresponding interrupt using following function if you need 
  *			to use interrupt mode. 
  *			USI_I2C_INTConfig(): USI_I2C IRQ Mask set
  *			InterruptRegister(): register the i2c irq handler 
  *			InterruptEn(): Enable the NVIC interrupt
  *
  *      6. Enable USI_I2C module using USI_I2C_Cmd().
  *
  *****************************************************************************************      
  * How to use USI_I2C in DMA Register mode
  *****************************************************************************************   
  *      To use the USI_I2C in DMA Register mode, the following steps are mandatory:
  *       
  *      1. Enable peripheral clock using the follwoing functions.(it is enabled by default)
  *			RCC_PeriphClockCmd(APBPeriph_USI_REG, APBPeriph_USI_CLOCK, ENABLE);
  *
  *      2. configure the I2C pinmux.
  *			Pinmux_Config(Pin_Num, PINMUX_FUNCTION_I2C)
  *        
  *      3. Program Role, Address Mode, Speed Mode, USI_I2C CLK, Slave address, Threshold, Feature Supports
  *			USI_I2C_StructInit()
  *
  *      4. Init Hardware use step3 parameters:
  *			USI_I2C_Init(USI_TypeDef *USIx, USI_I2C_InitTypeDef* USI_I2C_InitStruct)
  *
  *      5. Enable USI_I2C module using USI_I2C_Cmd().
  *
  *      6. GDMA related configurations(source address/destination address/block size etc.)
  *			USI_I2C_TXGDMA_Init():Init and Enable USI_I2C TX GDMA
  *			USI_I2C_RXGDMA_Init():Init and Enable USI_I2C RX GDMA
  *
  *      7. USI_I2C DMA Register Mode set.
  *			USI_I2C_DmaRegModeConfig():Configures the USI_I2C Control Register DMA mode
  *          
  *      8. Active the USI_I2C TX/RX DMA Request using USI_I2C_DMAControl().
  *
  *****************************************************************************************     
  * @endverbatim
  */
  
/* Exported types --------------------------------------------------------*/
/** @defgroup USI_I2C_Exported_Types USI-I2C Exported Types
  * @{
  */

/** 
  * @brief  USI_I2C Init structure definition
  */
typedef struct {
	u32	USI_I2CIdx;         /*!< Specifies the USI_I2C Device Index.
				This parameter can be a value of @ref USI_I2C_Peripheral_definitions */
					
	u32	USI_I2CMaster;      /*!< Specifies the USI_I2C Role.
				This parameter can be a value of @ref USI_I2C_Role_definitions */
					
	u32	USI_I2CAddrMod;     /*!< Specifies the USI_I2C Addressing Mode.
				This parameter can be a value of @ref USI_I2C_Addr_Mode_definitions */
					
	u32	USI_I2CSpdMod;      /*!< Specifies the USI_I2C Speed Mode. 
				This parameter can be a value of @ref USI_I2C_Speed_Mode_definitions */	
					
	u32	USI_I2CRXTL;        /*!< Specifies the USI_I2C RX FIFO Threshold. It controls the level of
				entries(or above) that triggers the RX_FULL interrupt.
				This parameter must be set to a value in the 0-255 range. A value of 0 sets 
				the threshold for 1 entry, and a value of 255 sets the threshold for 256 entry*/
				
	u32	USI_I2CTXTL;        /*!< Specifies the I2C TX FIFO Threshold.It controls the level of
				entries(or below) that triggers the TX_EMPTY interrupt.
				This parameter must be set to a value in the 0-255 range. A value of 0 sets 
				the threshold for 0 entry, and a value of 255 sets the threshold for 255 entry*/
	u32	USI_I2CMstReSTR;    /*!< Specifies the USI_I2C Restart Support of Master. */
				
	u32	USI_I2CMstGC;       /*!< Specifies the USI_I2C General Call Support of Master. */
				
	u32	USI_I2CMstStartB;   /*!< Specifies the USI_I2C Start Byte Support of Master. */
				
	u32	USI_I2CSlvNoAck;    /*!< Specifies the USI_I2C Slave No Ack Support. */
				
 	u32	USI_I2CSlvAckGC;    /*!< Specifies the USI_I2C Slave Acks to General Call. */ 
	
	u32	USI_I2CAckAddr;     /*!< Specifies the USI_I2C Target Address in I2C Master Mode or
				Ack Address in USI_I2C Slave0 Mode.
				This parameter must be set to a value in the 0-127 range if the USI_I2C_ADDR_7BIT
				is selected or 0-1023 range if the USI_I2C_ADDR_10BIT is selected. */
				
	u32	USI_I2CSlvSetup;       /*!< Specifies the USI_I2C SDA Setup Time. It controls the amount of time delay
				introduced in the rising edge of SCL¡ªrelative to SDA changing¡ªby holding SCL low
				when USI_I2C Device operating as a slave transmitter, in units of ic_clk period.
				This parameter must be set to a value in the 0-255 range, it must be set larger than USI_I2CSdaHd*/
				
	u32	USI_I2CSdaHd;       /*!< Specifies the USI_I2C SDA Hold Time. It controls the amount of 
				hold time on the SDA signal after a negative edge of SCL in both master
				and slave mode, in units of ic_clk period.
				This parameter must be set to a value in the 0-0xFFFF range. */
					
	u32	USI_I2CClk;         /*!< Specifies the USI_I2C Bus Clock (in kHz). It is closely related to USI_I2CSpdMod */
				
	u32	USI_I2CIPClk;		/*!< Specifies the USI_I2C IP Clock (in Hz). */
					
	u32	USI_I2CFilter;      /*!< Specifies the USI_I2C SCL/SDA Spike Filter. */

	u32	USI_I2CTxDMARqLv;   /*!< Specifies the USI_I2C TX DMA Empty Level. dma_tx_req signal is generated when
				the number of valid data entries in the transmit FIFO is equal to or below the DMA 
				Transmit Data Level Register. The value of DMA Transmit Data Level Register is equal 
				to this value. This parameter must be set to a value in the 0-31 range. */
				
	u32	USI_I2CRxDMARqLv;   /*!< Specifies the USI_I2C RX DMA Full Level. dma_rx_req signal is generated when
				the number of valid data entries in the transmit FIFO is equal to or above the DMA  
				Receive Data Level Register. The value of DMA Receive Data Level Register is equal to 
				this value+1. This parameter must be set to a value in the 0-31 range. */ 
				
	u32	USI_I2CDMAMod;     	/*!< Specifies the USI_I2C DMA Mode.
				This parameter can be a value of @ref USI_I2C_DMA_Mode_definitions */ 
}USI_I2C_InitTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup USI_I2C_Exported_Constants USI-I2C Exported Constants
  * @{
  */

/** @defgroup USI_I2C_Peripheral_definitions 
  * @{
  */
#define IS_USI_I2C_ALL_PERIPH(PERIPH) ((PERIPH) == USI0_DEV)
/**
  * @}
  */

/** @defgroup USI_I2C_Addr_Mode_definitions 
  * @{
  */
#define USI_I2C_ADDR_7BIT			((u32)0x00000000)
#define USI_I2C_ADDR_10BIT			((u32)0x00000001)
#define IS_USI_I2C_ADDR_MODE(MODE) (((MODE) == USI_I2C_ADDR_7BIT) || \
                                   ((MODE) == USI_I2C_ADDR_10BIT))
/**
  * @}
  */
  
/** @defgroup USI_I2C_Speed_Mode_definitions 
  * @{
  */
#define USI_I2C_SS_MODE				((u32)0x00000001)
#define USI_I2C_FS_MODE				((u32)0x00000002)
#define USI_I2C_HS_MODE				((u32)0x00000003)
#define IS_USI_I2C_SPEED_MODE(MODE) (((MODE) == USI_I2C_SS_MODE) || \
                                   	((MODE) == USI_I2C_FS_MODE) || \
                                   	((MODE) == USI_I2C_HS_MODE))
/**
  * @}
  */

/** @defgroup USI_I2C_Role_definitions 
  * @{
  */
#define USI_I2C_SLAVE_MODE			((u32)0x00000000)
#define USI_I2C_MASTER_MODE			((u32)0x00000001)
/**
  * @}
  */

/** @defgroup USI_I2C_DMA_Mode_definitions 
  * @{
  */
#define USI_I2C_DMA_LEGACY			((u32)0x00000000)
#define USI_I2C_DMA_REGISTER		((u32)0x00000001)
#define USI_I2C_DMA_DESCRIPTOR		((u32)0x00000002)
#define IS_USI_I2C_DMA_MODE(MODE) 	(((MODE) == USI_I2C_DMA_LEGACY) || \
                                   	((MODE) == USI_I2C_DMA_REGISTER) || \
                                   	((MODE) == USI_I2C_DMA_DESCRIPTOR))
/**
  * @}
  */

/** @defgroup USI_I2C_DMA_DATA_LENGTH 
  * @{
  */ 
#define IS_USI_I2C_DMA_DATA_LEN(LENGTH)   ((LENGTH) <= 0xFFFF)
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup USI_I2C_Exported_Functions USI-I2C Exported Functions
  * @{
  */

/** @defgroup USI_I2C_Exported_Normal_Functions USI-I2C Exported Normal Functions
  * @{
  */
_LONG_CALL_ void USI_I2C_StructInit(USI_I2C_InitTypeDef* USI_I2C_InitStruct);
_LONG_CALL_ void USI_I2C_Init(USI_TypeDef *USIx, USI_I2C_InitTypeDef* USI_I2C_InitStruct);
_LONG_CALL_ void USI_I2C_SetSpeed(USI_TypeDef *USIx, u32 SpdMd, u32 I2Clk, u32 I2CIPClk);
_LONG_CALL_ void USI_I2C_SetSlaveAddress(USI_TypeDef *USIx, u16 Address);
_LONG_CALL_ u8 USI_I2C_CheckFlagState(USI_TypeDef *USIx, u32 USI_I2C_FLAG);
_LONG_CALL_ u8 USI_I2C_CheckTXFIFOState(USI_TypeDef *USIx, u32 USI_I2C_TXFIFO_FLAG);
_LONG_CALL_ u8 USI_I2C_CheckRXFIFOState(USI_TypeDef *USIx, u32 USI_I2C_RXFIFO_FLAG);
_LONG_CALL_ void USI_I2C_INTConfig(USI_TypeDef *USIx, u32 USI_I2C_IT, u32 NewState);
_LONG_CALL_ void USI_I2C_ClearINT(USI_TypeDef *USIx, u32 INTrBit);
_LONG_CALL_ void USI_I2C_ClearAllINT(USI_TypeDef *USIx);
_LONG_CALL_ u32 USI_I2C_GetRawINT(USI_TypeDef *USIx);
_LONG_CALL_ u32 USI_I2C_GetINT(USI_TypeDef *USIx);
_LONG_CALL_ void USI_I2C_Cmd(USI_TypeDef *USIx, u8 NewState);
_LONG_CALL_ u8 USI_I2C_ReceiveData(USI_TypeDef *USIx);
/**
  * @}
  */

/** @defgroup USI_I2C_Exported_Master_Functions USI-I2C Exported Master Functions
  * @{
  */
_LONG_CALL_ void USI_I2C_MasterSendNullData(USI_TypeDef *USIx, u8* pBuf, u8  I2CCmd, u8  I2CStop, u8  I2CReSTR);
_LONG_CALL_ void USI_I2C_MasterSend(USI_TypeDef *USIx, u8* pBuf, u8  I2CCmd, u8  I2CStop, u8  I2CReSTR);
_LONG_CALL_ u8 USI_I2C_MasterWrite(USI_TypeDef *USIx, u8* pBuf, u8 len);
_LONG_CALL_ u8 USI_I2C_MasterRead(USI_TypeDef *USIx, u8* pBuf, u8 len);
_LONG_CALL_ void USI_I2C_MasterRepeatRead(USI_TypeDef* USIx, u8* pWriteBuf, u8 Writelen, u8* pReadBuf, u8 Readlen);
_LONG_CALL_ void USI_I2C_SetSlaveAddress(USI_TypeDef *USIx, u16 Address);
/**
  * @}
  */

/** @defgroup USI_I2C_Exported_Slave_Functions USI-I2C Exported Slave Functions
  * @{
  */
_LONG_CALL_ void USI_I2C_SlaveWrite(USI_TypeDef *USIx, u8* pBuf, u8 len);
_LONG_CALL_ void USI_I2C_SlaveRead(USI_TypeDef *USIx, u8* pBuf, u8 len);
_LONG_CALL_ void USI_I2C_SlaveSend(USI_TypeDef *USIx, u8 Data);
/**
  * @}
  */

/** @defgroup USI_I2C_Exported_DMA_Functions USI-I2C Exported DMA Functions
  * @{
  */
_LONG_CALL_ void USI_I2C_DMAControl(USI_TypeDef *USIx, u32 DmaCtrl, u8 NewState);
_LONG_CALL_ void USI_I2C_DmaRegModeConfig(USI_TypeDef *USIx, u32 USI_I2C_DmaCmd, u32 USI_I2C_DmaBLen);
_LONG_CALL_ BOOL USI_I2C_TXGDMA_Init(u8 Index, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pTxBuf, int TxCount);
_LONG_CALL_ BOOL USI_I2C_RXGDMA_Init(u8 Index, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pRxBuf, int RxCount);
/**
  * @}
  */
 
/** @defgroup USI_I2C_Exported_PowerSave_Functions USI-I2C Exported PowerSave Functions
  * @{
  */
_LONG_CALL_ void USI_I2C_Sleep_Cmd(USI_TypeDef *USIx, u32 NewStatus);
_LONG_CALL_ void USI_I2C_WakeUp(USI_TypeDef *USIx);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/* Other Definitions --------------------------------------------------------*/
//USI_I2C Timing Parameters
#define USI_I2C_SS_MIN_SCL_HTIME		4000    //the unit is ns.
#define USI_I2C_SS_MIN_SCL_LTIME		4700    //the unit is ns.

#define USI_I2C_FS_MIN_SCL_HTIME		600     //the unit is ns.
#define USI_I2C_FS_MIN_SCL_LTIME		1300    //the unit is ns.

#define USI_I2C_HS_MIN_SCL_HTIME_100	60      //the unit is ns, with bus loading = 100pf
#define USI_I2C_HS_MIN_SCL_LTIME_100	100     //the unit is ns., with bus loading = 100pf

#define USI_I2C_HS_MIN_SCL_HTIME_400	160     //the unit is ns, with bus loading = 400pf
#define USI_I2C_HS_MIN_SCL_LTIME_400	320     //the unit is ns., with bus loading = 400pf

extern u32 USI_I2C_SLAVEWRITE_PATCH;
extern u32 USI_IC_FS_SCL_HCNT_TRIM;
extern u32 USI_IC_FS_SCL_LCNT_TRIM;

#endif

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/

