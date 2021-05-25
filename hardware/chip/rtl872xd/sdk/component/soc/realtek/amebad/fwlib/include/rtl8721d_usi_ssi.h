/**
  ******************************************************************************
  * @file    rtl8721d_usi_ssi.h
  * @author
  * @version V1.0.0
  * @date    2017-11-27
  * @brief   This file contains all the functions prototypes for the SPI firmware
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

#ifndef _RTL8721D_USI_SSI_H_
#define _RTL8721D_USI_SSI_H_

#include "rtl8721d_usi.h"

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup USI-SPI
  * @brief USI-SPI driver modules
  * @{
  */

/** @addtogroup USI-SPI
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * USI-SPI:
  *		- Support Motorola SPI interface
  *		- Role: Master or Slave
  *		- Base Address: USI0_DEV
  *		- Bus Clk: 50MHz
  *		- BaudRate: less than or equal to 25M
  *		- Transfer mode: Tx,Rx,TRx when configured as Master; TRx when configured as Slave
  *		- Data Frame Size: 4-16 bits supported
  *		- IRQ Number: USI_IRQ
  *		- GDMA TX handshake interface: GDMA_HANDSHAKE_INTERFACE_USI0_TX
  *		- GDMA RX handshake interface: GDMA_HANDSHAKE_INTERFACE_USI0_RX
  *
  *****************************************************************************************     
  * How to use Normal USI SPI
  *****************************************************************************************
  *	To use the SPI in normal mode, the following steps are mandatory:
  *
  *      1. Enable peripheral clock using the following functions:
  *			-RCC_PeriphClockCmd(APBPeriph_USI_REG, APBPeriph_USI_CLOCK, ENABLE);
  *
  *      2. Configure the SPIx pinmux:
  *			-Pinmux_Config(Pin_Num,  PINMUX_FUNCTION_SPIM) when configured as Master;
  *			-Pinmux_Config(Pin_Num,  PINMUX_FUNCTION_SPIS) when configured as Slave;
  *
  *      3. Program the Polarity,Phase,Transfer Mode,Baud Rate Prescaler,DataFrameSize,
  *			Interrupt TRx Threshold level,DMA TRx Threshold level and other parameters using
  *			USI_SSI_StructInit() and change some parameters if needed
  *
  *      4. Init Hardware use step3 parameters:
  *			USI_SSI_Init(USI_TypeDef *usi_dev, USI_SSI_InitTypeDef *USI_SSI_InitStruct)
  *
  *      5. Enable the SPI:
  *			USI_SSI_Cmd() 
  *
  *      6. When using poll:
  *			-Using USI_SSI_Writeable() function to make sure that the transmit FIFO is not full,
  *			then using USI_SSI_WriteData() function to send data
  *			     
  *			-Using USI_SSI_Readable() function to make sure that the receive FIFO is not empty,
  *			then using USI_SSI_ReadData() function to receive data
  *
  *      7. Enable the NVIC and the corresponding interrupt using following function if you need 
  *			to use interrupt mode. 
  *			-USI_SSI_INTConfig(): SPI IRQ Mask set
  *			-InterruptRegister(): register the SPI irq handler 
  *			-InterruptEn(): Enable the NVIC interrupt and set irq priority
  *
  *
  *      @note in SPI_Exported_Functions group, these functions below are about Interrupts 
  *			and flags management:
  *			-USI_SSI_GetIsr()
  *			-USI_SSI_GetRawIsr()
  *			-USI_SSI_INTConfig()
  *			-USI_SSI_SetRxFifoLevel()
  *			-USI_SSI_SetTxFifoLevel()
  *			-USI_SSI_SetIsrClean()
  *
  *
  *****************************************************************************************      
  * How to use USI SPI in DMA mode
  *****************************************************************************************   
  *	To use the USI SPI in DMA mode, the following steps are mandatory:
  *       
  *      1. Enable peripheral clock using the following functions:
  *			-RCC_PeriphClockCmd(APBPeriph_USI_REG, APBPeriph_USI_CLOCK, ENABLE);
  *
  *      2. Configure the SPIx pinmux:
  *			-Pinmux_Config(Pin_Num,  PINMUX_FUNCTION_SPIM) when configured as Master;
  *			-Pinmux_Config(Pin_Num,  PINMUX_FUNCTION_SPIS) when configured as Slave;
  *
  *      3. Program the Polarity,Phase,Transfer Mode,Baud Rate Prescaler,DataFrameSize,
  *			Interrupt TRx Threshold level,DMA TRx Threshold level and other parameters using
  *			USI_SSI_StructInit() and change some parameters if needed
  *
  *      4. Init Hardware use step3 parameters:
  *			USI_SSI_Init(USI_TypeDef *usi_dev, USI_SSI_InitTypeDef *USI_SSI_InitStruct)
  *
  *      5. Enable the SPI:
  *			USI_SSI_Cmd() 	
  *
  *      6. GDMA related configurations(DMA burst size/source address/destination address/block size etc).
  *
  *      7. Active the SPI  DMA TX/RX using USI_SSI_SetDmaEnable() function.
  *
  *      @note in SPI_Exported_Functions group, these functions below are about DMA:
  *			-USI_SSI_SetDmaEnable()
  *			-USI_SSI_SetDmaLevel()
  *
  *****************************************************************************************     
  * @endverbatim
  */
  
/* Exported Types --------------------------------------------------------*/
/** @defgroup USI_SPI_Exported_Types USI-SPI Exported Types
  * @{
  */

/** 
  * @brief  USI-SPI Init structure definition
  */
typedef struct
{
	u32  USI_SPI_DmaRxDataLevel;     /*!< Specifies the DMA receive data level.
							     The dma_rx_req is generated when the number of valid data entries in the
							     receive FIFO  is equal to or above this field value+1,and rxdma_en =1.
							     @note For AmebaD, the value range of this parameter should be 0 to 63,because
							     the depth of Rx FIFO is 64. */
							     
	u32  USI_SPI_DmaTxDataLevel;    /*!< Specifies the DMA transmit data level.
							     The dma_tx_req is generated when the number of valid data entries in the
							     transmit FIFO is equal to or below this field value,and txdma_en =1.
							     @note For AmebaD, the value range of this parameter should be 0 to 63,because  
							     the depth of Rx FIFO is 64. */
							     
	u32  USI_SPI_RxThresholdLevel;   /*!< Specifies the receive FIFO threshold level.
							     This Parameter controls the level of entries(or above) at which the receive FIFO controller 
							     triggers an interrupt.When the number of receive FIFO entries is greater than or equal to this
							     value +1,the receive FIFO full interrupt is triggered.
							      @note For AmebaD, the value range of this parameter should be 0 to 63,because the depth 
							      of Rx FIFO is 64. */
							     
	u32  USI_SPI_TxThresholdLevel;     /*!< Specifies the transmit FIFO threshold level.
							     This Parameter controls the level of entries (or below) at which the transmit FIFO controller 
							     triggers an interrupt.When the number of transmit FIFO entries is less than or equal to this
							     value,the transmit FIFO empty interrupt is triggered.
							      @note For AmebaD, the value range of this parameter should be 0 to 63,because of the depth 
							      of Rx FIFO is 64. */   
							     
	u32  USI_SPI_ClockDivider;		    /*!< Specifies the SPI Baud Rate.
							     The value of sclk_out equals to ssi_clk devides the value of this parameter
							     @note The LSB for this field is always set to 0 and is unaffected by a write operation,which ensures 
							     an even value is held. */   
							     
	u32  USI_SPI_DataFrameNumber;   /*!< Specifies the number of data frames master wants to receive.
							     When TMOD=10, SPI uses this value to set the number of data frames to 
							     be continuous received. 
							     @note The value of this parameter should be set to the number of data frames that to be received
							     minus one.And this parameter is used only when the device is master. */ 

	u32   USI_SPI_DataFrameSize;        /*!< Selects the data frame length .
							    This parameter can be a value of @ref USI_SPI_Data_Frame_Size_definitions. 
							    @note Need to right-justify transmit data before writting into the transmit FIFO
							    The transmit logic ignores the upper unused bits when transmitting the data. */
							    
	u32   USI_SPI_InterruptMask;	     /*!< Specifies which interrupt to enable.
							     Each bit in this parameter corresponds to a specific interrupt.*/ 
							   
	u32   USI_SPI_Role;                        /*!< Specifies the role of SPI device.
							    This parameter can be a value of @ref USI_SPI_ROLE_definitions. . */ 
							  	
	u32   USI_SPI_SclkPhase;		    /*!< Specifies the serial clock phase.
							    When USI_SPI_SclkPhase = 0, data are captured on the first edge of the serial clock. 
							    When USI_SPI_SclkPhase = 1, the serial clock starts toggling one cycle after the slave select line is activated, 
							    and data are captured on the second edge of the serial clock.
							    This parameter can be a value of @ref USI_SPI_SCPH_definitions. */ 
							  	
	u32   USI_SPI_SclkPolarity;              /*!< Specifies the serial clock polarity.
							    When USI_SPI_SclkPolarity = 0, the serial clock remains low when idle. 
							    When USI_SPI_SclkPolarity = 1, the serial clock remains high when idle.
							     This parameter can be a value of @ref USI_SPI_SCPOL_definitions. */ 
							    
	u32   USI_SPI_TransferMode;         /*!< Selects the mode of transfer for serial communication.
							    This parameter can be a value of @ref USI_SPI_TMOD_definitions. 
							    @note This transfer mode is only valid when the DW_apb_ssi is configured as a master device.*/ 

	u32   USI_SPI_RxSampleDelay;      /*!< Specifies the sample delay time of receive data input signal.The unit is spi_mst_clk.
							    @note This configuration is only valid when the DW_apb_ssi is configured as a master device.
							      For AmebaD, the value range of this parameter should be 0 to 0xFF */ 
								
	u32   USI_SPI_SSTogglePhase;     /*!< Specifies whether CS needs to toggle between transmissions when USI_SPI_SclkPhase = 0.
                                                       This parameter can be a value of @ref USI_SPI_SS_Toggle_Phase_definitions.
							    @note This configuration is only valid when the DW_apb_ssi is configured as a master device. */ 
}USI_SSI_InitTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup USI_SPI_Exported_Constants USI-SPI Exported Constants
  * @{
  */ 

/** @defgroup USI_SPI_TMOD_definitions 
  * @{
  */ 
#define USI_SPI_TMOD_TR				(0)
#define USI_SPI_TMOD_TO				(1)
#define USI_SPI_TMOD_RO				(2)
/**
  * @}
  */

/** @defgroup USI_SPI_SCPOL_definitions 
  * @{
  */ 
#define USI_SPI_SCPOL_INACTIVE_IS_LOW		(0)
#define USI_SPI_SCPOL_INACTIVE_IS_HIGH		(1)
/**
  * @}
  */

/** @defgroup USI_SPI_SCPH_definitions 
  * @{
  */ 
#define USI_SPI_SCPH_TOGGLES_IN_MIDDLE		(0)
#define USI_SPI_SCPH_TOGGLES_AT_START		(1)
/**
  * @}
  */

/** @defgroup USI_SPI_SS_Toggle_Phase_definitions 
  * @{
  */ 
#define USI_SPI_SS_NOT_TOGGLE		(0)
#define USI_SPI_SS_TOGGLE		(1)
/**
  * @}
  */


/** @defgroup USI_SPI_Data_Frame_Size_definitions 
  * @{
  */ 
#define USI_SPI_DFS_4_BITS				(3)
#define USI_SPI_DFS_5_BITS				(4)
#define USI_SPI_DFS_6_BITS				(5)
#define USI_SPI_DFS_7_BITS				(6)
#define USI_SPI_DFS_8_BITS				(7)
#define USI_SPI_DFS_9_BITS				(8)
#define USI_SPI_DFS_10_BITS				(9)
#define USI_SPI_DFS_11_BITS				(10)
#define USI_SPI_DFS_12_BITS				(11)
#define USI_SPI_DFS_13_BITS				(12)
#define USI_SPI_DFS_14_BITS				(13)
#define USI_SPI_DFS_15_BITS				(14)
#define USI_SPI_DFS_16_BITS				(15)
/**
  * @}
  */

/** @defgroup USI_SPI_ROLE_definitions 
  * @{
  */ 
#define USI_SPI_SLAVE				(0)
#define USI_SPI_MASTER				(1)
/**
  * @}
  */

/** @defgroup USI_SPI_DMA_Control_definitions 
  * @{
  */ 
#define USI_SPI_NODMA				(0)
#define USI_SPI_RXDMA_ENABLE		(1)
#define USI_SPI_TXDMA_ENABLE		(2)
#define USI_SPI_TRDMA_ENABLE		(3)
/**
  * @}
  */

/** @defgroup USI_SPI_FIFO_depth_definitions 
  * @{
  */ 
#define USI_SPI_TX_FIFO_DEPTH				(64)
#define USI_SPI_RX_FIFO_DEPTH				(64)
/**
  * @}
  */

/** @defgroup USI_SPI_Interrupt_definitions 
  * @{
  */ 
#define USI_SPI_INTERRUPT_MASK		(USI_TXFIFO_ALMOST_EMTY_INTR_EN | \
									USI_TXFIFO_OVERFLOW_INTR_EN | \
									USI_TXFIFO_UNDERFLOW_INTR_EN | \
									USI_RXFIFO_ALMOST_FULL_INTR_EN | \
									USI_RXFIFO_OVERFLOW_INTR_EN | \
									USI_RXFIFO_UNDERFLOW_INTR_EN | \
									USI_SPI_RX_DATA_FRM_ERR_INTER_EN)

#define USI_SPI_INTERRUPT_CLEAR_MASK	(USI_TXFIFO_OVERFLOW_CLR | \
										USI_TXFIFO_UNDERFLOW_CLR | \
										USI_RXFIFO_OVERFLOW_CLR | \
										USI_RXFIFO_UNDERFLOW_CLR | \
										USI_SPI_RX_DATA_FRM_ERR_CLR)
/**
  * @}
  */

/** @defgroup USI_TRX_Threshold_Level_definitions 
  * @{
  */ 
#define IS_USI_SPI_RxThresholdLevel(value)		(value <= 63)
#define IS_USI_SPI_TxThresholdLevel(value)		(value <= 63)
/**
  * @}
  */

/** @defgroup USI_TRX_DMA_Level_definitions 
  * @{
  */ 
#define IS_USI_SPI_RxDMALevel(value)		(value <= 63)
#define IS_USI_SPI_TxDMALevel(value)		(value <= 63)
/**
  * @}
  */
  

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup USI_SPI_Exported_Functions USI-SPI Exported Functions
  * @{
  */
/** @defgroup USI_SPI_Exported_Normal_Functions USI-SPI Exported Normal Functions
  * @{
  */
_LONG_CALL_ void USI_SSI_StructInit(USI_SSI_InitTypeDef* USI_SSI_InitStruct);
_LONG_CALL_ void USI_SSI_Init(USI_TypeDef *usi_dev, USI_SSI_InitTypeDef *USI_SSI_InitStruct);
_LONG_CALL_ void USI_SSI_Cmd(USI_TypeDef *usi_dev, u32 NewStatus);
_LONG_CALL_ void USI_SSI_TRxPath_Cmd(USI_TypeDef *usi_dev, u32 path, u32 NewStatus);
_LONG_CALL_ u32 USI_SSI_GetTRxPath(USI_TypeDef *usi_dev);
_LONG_CALL_ void USI_SSI_INTConfig(USI_TypeDef* usi_dev, u32 USI_SSI_IT, u32 newState);
_LONG_CALL_ void USI_SSI_SetSclkPolarity(USI_TypeDef *usi_dev, u32 SclkPolarity);
_LONG_CALL_ void USI_SSI_SetSclkPhase(USI_TypeDef *usi_dev, u32 SclkPhase);
_LONG_CALL_ void USI_SSI_SetSSTogglePhase(USI_TypeDef *usi_dev, u32 TogglePhase);
_LONG_CALL_ void USI_SSI_SetDataFrameSize(USI_TypeDef *usi_dev, u32 DataFrameSize);
_LONG_CALL_ void USI_SSI_SetSampleDelay(USI_TypeDef *usi_dev, u32 SampleDelay);
_LONG_CALL_ void USI_SSI_SetReadLen(USI_TypeDef *usi_dev, u32 DataFrameNumber);
_LONG_CALL_ void USI_SSI_SetBaudDiv(USI_TypeDef *usi_dev, u32 ClockDivider);
_LONG_CALL_ void USI_SSI_SetBaud(USI_TypeDef *USIx, u32 BaudRate, u32 IpClk);
_LONG_CALL_ void USI_SSI_SetIsrClean(USI_TypeDef *usi_dev, u32 InterruptStatus);
_LONG_CALL_ void USI_SSI_WriteData(USI_TypeDef *usi_dev, u32 value);
_LONG_CALL_ void USI_SSI_SetRxFifoLevel(USI_TypeDef *usi_dev, u32 RxThresholdLevel);
_LONG_CALL_ void USI_SSI_SetTxFifoLevel(USI_TypeDef *usi_dev, u32 TxThresholdLevel);
_LONG_CALL_ u32 USI_SSI_Writeable(USI_TypeDef *usi_dev);
_LONG_CALL_ u32 USI_SSI_Writeable(USI_TypeDef *usi_dev);
_LONG_CALL_ u32 USI_SSI_ReadData(USI_TypeDef *usi_dev);
_LONG_CALL_ u32 USI_SSI_ReceiveData(USI_TypeDef *usi_dev, void* RxData, u32 Length);
_LONG_CALL_ u32 USI_SSI_SendData(USI_TypeDef *usi_dev, void* TxData, u32 Length, u32 Role);
_LONG_CALL_ u32 USI_SSI_GetRxCount(USI_TypeDef *usi_dev);
_LONG_CALL_ u32 USI_SSI_GetTxCount(USI_TypeDef *usi_dev);
_LONG_CALL_ u32 USI_SSI_GetTxFIFOStatus(USI_TypeDef *usi_dev);
_LONG_CALL_ u32 USI_SSI_GetRxFIFOStatus(USI_TypeDef *usi_dev);
_LONG_CALL_ u32 USI_SSI_GetTransStatus(USI_TypeDef *usi_dev);
_LONG_CALL_ u32 USI_SSI_GetDataFrameSize(USI_TypeDef *usi_dev);
_LONG_CALL_ u32 USI_SSI_Busy(USI_TypeDef *usi_dev);
_LONG_CALL_ u32 USI_SSI_GetIsr(USI_TypeDef *usi_dev);
_LONG_CALL_ u32 USI_SSI_GetRawIsr(USI_TypeDef *usi_dev);

/**
  * @}
  */

/** @defgroup USI_SPI_Exported_DMA_Functions USI-SPI Exported DMA Functions
  * @{
  */
_LONG_CALL_ BOOL USI_SSI_TXGDMA_Init(u32 Index, PGDMA_InitTypeDef GDMA_InitStruct, void *CallbackData, 
										IRQ_FUN CallbackFunc, u8 *pTxData, u32 Length);
_LONG_CALL_ BOOL USI_SSI_RXGDMA_Init(u8 Index, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, 
										IRQ_FUN CallbackFunc, u8  *pRxData, u32 Length);
_LONG_CALL_ void USI_SSI_SetDmaEnable(USI_TypeDef *usi_dev, u32 newState, u32 Mask);
_LONG_CALL_ void USI_SSI_SetDmaLevel(USI_TypeDef *usi_dev, u32 TxLevel, u32 RxLevel);
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

#endif //_RTL8721D_USI_SSI_H_

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
