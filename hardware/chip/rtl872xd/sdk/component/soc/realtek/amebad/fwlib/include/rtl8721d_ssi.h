/**
  ******************************************************************************
  * @file    rtl8721d_ssi.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
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

#ifndef _RTL8710B_SPI_H_
#define _RTL8710B_SPI_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup SPI
  * @brief SPI driver modules
  * @{
  */

/** @addtogroup SPI
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * SPI0:
  *		- Support Motorola SPI interface
  *		- Role: Master or Slave
  *		- Base Address: SPI0_DEV
  *		- Bus Clk: 100MHz
  *		- BaudRate: less than or equal to 50M
  *		- Transfer mode:Tx,Rx,TRx,EEPROM Read when configured as Master; TRx when configured as Slave
  *		- Data Frame Size: 4-16 bits supported
  *		- IRQ Number: SPI0_IRQ
  *		- GDMA TX handshake interface: GDMA_HANDSHAKE_INTERFACE_SPI0_TX
  *		- GDMA RX handshake interface: GDMA_HANDSHAKE_INTERFACE_SPI0_RX
  *
  * SPI1: 
  *		- Support Motorola SPI interface
  *		- Role: Master
  *		- Base Address: SPI1_DEV
  *		- Bus Clk: 50MHz
  *		- BaudRate: less than or equal to 25M
  *		- Transfer mode:Tx,Rx,TRx,EEPROM Read 
  *		- Data Frame Size: 4-16 bits supported
  *		- IRQ Number: SPI1_IRQ
  *		- GDMA TX handshake interface: GDMA_HANDSHAKE_INTERFACE_SPI1_TX
  *		- GDMA RX handshake interface: GDMA_HANDSHAKE_INTERFACE_SPI1_RX
  *
  *****************************************************************************************     
  * How to use Normal SPI
  *****************************************************************************************
  *	To use the SPI in DMA mode, the following steps are mandatory:
  *
  *      1. Enable peripheral clock using the following functions:
  *			-RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE) for SPI0;
  *			-RCC_PeriphClockCmd(APBPeriph_SPI1, APBPeriph_SPI1_CLOCK, ENABLE) for SPI1;
  *
  *      2. Configure the SPIx pinmux:
  *			-Pinmux_Config(Pin_Num,  PINMUX_FUNCTION_SPIM) when configured as Master;
  *			-Pinmux_Config(Pin_Num,  PINMUX_FUNCTION_SPIS) when configured as Slave;
  *
  *      3. Program the Polarity,Phase,Transfer Mode,Baud Rate Prescaler,DataFrameSize,
  *			Interrupt TRx Threshold level,DMA TRx Threshold level and other parameters using
  *			SSI_StructInit() and change some parameters if needed
  *
  *      4. Init Hardware use step3 parameters:
  *			SSI_Init(SPI_TypeDef *spi_dev, SSI_InitTypeDef *SSI_InitStruct)
  *
  *      5. Enable the SPI:
  *			SSI_Cmd() 
  *
  *      6. When using poll:
  *			-Using SSI_Writeable() function to make sure that the transmit FIFO is not full,
  *			then using SSI_WriteData() function to send data
  *			     
  *			-Using SSI_Readable() function to make sure that the receive FIFO is not empty,
  *			then using SSI_ReadData() function to receive data
  *
  *      7. Enable the NVIC and the corresponding interrupt using following function if you need 
  *			to use interrupt mode. 
  *			-SSI_INTConfig(): SPI IRQ Mask set
  *			-InterruptRegister(): register the SPI irq handler 
  *			-InterruptEn(): Enable the NVIC interrupt and set irq priority
  *
  *
  *      @note in SPI_Exported_Functions group, these functions below are about Interrupts 
  *			and flags management:
  *			-SSI_GetIsr()
  *			-SSI_GetRawIsr()
  *			-SSI_INTConfig()
  *			-SSI_SetRxFifoLevel()
  *			-SSI_SetTxFifoLevel()
  *			-SSI_SetIsrClean()
  *
  *
  *****************************************************************************************      
  * How to use SPI in DMA mode
  *****************************************************************************************   
  *	To use the SPI in DMA mode, the following steps are mandatory:
  *       
  *      1. Enable peripheral clock using the following functions:
  *			-RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE) for SPI0;
  *			-RCC_PeriphClockCmd(APBPeriph_SPI1, APBPeriph_SPI1_CLOCK, ENABLE) for SPI1;
  *
  *      2. Configure the SPIx pinmux:
  *			-Pinmux_Config(Pin_Num,  PINMUX_FUNCTION_SPIM) when configured as Master;
  *			-Pinmux_Config(Pin_Num,  PINMUX_FUNCTION_SPIS) when configured as Slave;
  *
  *      3. Program the Polarity,Phase,Transfer Mode,Baud Rate Prescaler,DataFrameSize,
  *			Interrupt TRx Threshold level,DMA TRx Threshold level and other parameters using
  *			SSI_StructInit() and change some parameters if needed
  *
  *      4. Init Hardware use step3 parameters:
  *			SSI_Init(SPI_TypeDef *spi_dev, SSI_InitTypeDef *SSI_InitStruct)
  *
  *      5. Enable the SPI:
  *			SSI_Cmd() 	
  *
  *      6. GDMA related configurations(DMA burst size/source address/destination address/block size etc).
  *
  *      7. Active the SPI  DMA TX/RX using SSI_SetDmaEnable() function.
  *
  *      @note in SPI_Exported_Functions group, these functions below are about DMA:
  *			-SSI_SetDmaEnable()
  *			-SSI_SetDmaLevel()
  *
  *****************************************************************************************     
  * @endverbatim
  */
  
/* Exported Types --------------------------------------------------------*/
/** @defgroup SPI_Exported_Types SPI Exported Types
  * @{
  */

/** 
  * @brief  SPI Init structure definition
  */
typedef struct
{
	u32  SPI_DmaRxDataLevel;     /*!< Specifies the DMA receive data level.
							     The dma_rx_req is generated when the number of valid data entries in the
							     receive FIFO  is equal to or above this field value+1,and RDMAE=1.
							     @note For Amebaz, the value range of this parameter should be 0 to 63,because
							     the depth of Rx FIFO is 64. */
							     
	u32  SPI_DmaTxDataLevel;    /*!< Specifies the DMA transmit data level.
							     The dma_tx_req is generated when the number of valid data entries in the
							     transmit FIFO is equal to or below this field value,and TDMAE=1.
							     @note For Amebaz, the value range of this parameter should be 0 to 63,because  
							     the depth of Rx FIFO is 64. */
							     
	u32  SPI_RxThresholdLevel;   /*!< Specifies the receive FIFO threshold level.
							     This Parameter controls the level of entries(or above) at which the receive FIFO controller 
							     triggers an interrupt.When the number of receive FIFO entries is greater than or equal to this
							     value +1,the receive FIFO full interrupt is triggered.
							      @note For Amebaz, the value range of this parameter should be 0 to 63,because the depth 
							      of Rx FIFO is 64. */
							     
	u32  SPI_TxThresholdLevel;     /*!< Specifies the transmit FIFO threshold level.
							     This Parameter controls the level of entries (or below) at which the transmit FIFO controller 
							     triggers an interrupt.When the number of transmit FIFO entries is less than or equal to this
							     value,the transmit FIFO empty interrupt is triggered.
							      @note For Amebaz, the value range of this parameter should be 0 to 63,because of the depth 
							      of Rx FIFO is 64. */
							     
	u32  SPI_SlaveSelectEnable;     /*!< Set the slave select enable flag.
							     This Parameter controls which slave to be selected by master,each bit in SER register
							     corresponds to a slave select line(ss_x_n) from spi master.
							     @note The default vlaue of this parameter is 0,and one slave is selected.if more slaves to be selected,
							     you may use SW way to do this.And this parameter is used only when the device is master. */   
							     
	u32  SPI_ClockDivider;		    /*!< Specifies the SPI Baud Rate.
							     The value of sclk_out equals to ssi_clk devides the value of this parameter
							     @note The LSB for this field is always set to 0 and is unaffected by a write operation,which ensures 
							     an even value is held. */   
							     
	u32  SPI_DataFrameNumber;   /*!< Specifies the number of data frames master wants to receive .
							     When TMOD=10 or TMOD=11,Ctrl1 register uses this value to set the number of data frames to 
							     be continuous received. 
							     @note The value of this parameter should be set to the number of data frames that to be received
							     minus one.And this parameter is used only when the device is master. */ 
							     
	u32   SPI_DataFrameFormat;    /*!< Selects which serial protocol transfers the data .
							    This parameter can be a value of @ref SPI_Frame_Format_definitions. */ 
							     
	u32   SPI_DataFrameSize;        /*!< Selects the data frame length .
							    This parameter can be a value of @ref SPI_Data_Frame_Size_definitions. 
							    @note Need to right-justify transmit data before writting into the transmit FIFO
							    The transmit logic ignores the upper unused bits when transmitting the data. */
							    
	u32   SPI_InterruptMask;	     /*!< Specifies which interrupt to be enable.
							     Each bit in this parameter corresponds to a specific interrupt.*/ 
							   
	u32   SPI_Role;                        /*!< Specifies the role of SPI device.
							    This parameter can be a value of @ref SPI_ROLE_definitions. . */ 
							  	
	u32   SPI_SclkPhase;		    /*!< Specifies the serial clock phase.
							    When SPI_SclkPhase = 0, data are captured on the first edge of the serial clock. When SPI_SclkPhase = 1,
							    the serial clock starts toggling one cycle after the slave select line is activated, and data
							    are captured on the second edge of the serial clock.
							    This parameter can be a value of @ref SPI_SCPH_definitions. 
							    @note Valid when the frame format(FRF) is set to Motorola SPI. */ 
							  	
	u32   SPI_SclkPolarity;              /*!< Specifies the serial clock polarity.
							    When SPI_SclkPolarity = 0, the serial clock remains low when idle. When SPI_SclkPolarity = 1,
							    the serial clock remains high when idle.
							     This parameter can be a value of @ref SPI_SCPOL_definitions. 
							    @note Valid when the frame format(FRF) is set to Motorola SPI.*/ 
							    
	u32   SPI_TransferMode;         /*!< Selects the mode of transfer for serial communication.
							    This parameter can be a value of @ref SPI_TMOD_definitions. 
							    @note This transfer mode is only valid when the DW_apb_ssi is configured as a master device.*/ 
								
	u32   SPI_MicrowireControlFrameSize;  /*!< Selects the length of the control word for the Microwire frame format.
	                                                        This parameter can be a value of @ref SPI_MW_Control_Frame_Size_definitions. */ 
																
	u32   SPI_MicrowireDirection;	         /*!< Specifies of the data word when the Microwire serial protocol is used.
	                                                        This parameter can be a value of @ref SPI_MW_Direction_definitions. */ 
																
	u32   SPI_MicrowireHandshaking;        /*!< Specifies Microwire Handshaking.
	                                                        This parameter can be a value of @ref SPI_MW_Handshake_Enable_definitions. */ 
																
	u32   SPI_MicrowireTransferMode;       /*!< Specifies Microwire Transfer Mode.
	                                                        This parameter can be a value of @ref SPI_MW_TMOD_definitions. */ 
}SSI_InitTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup SPI_Exported_Constants SPI Exported Constants
  * @{
  */ 

/** @defgroup SPI_TMOD_definitions 
  * @{
  */ 
#define TMOD_TR				(0)
#define TMOD_TO				(1)
#define TMOD_RO				(2)
#define TMOD_EEPROM_R		(3)
/**
  * @}
  */

/** @defgroup SPI_SCPOL_definitions 
  * @{
  */ 
#define SCPOL_INACTIVE_IS_LOW		(0)
#define SCPOL_INACTIVE_IS_HIGH		(1)
/**
  * @}
  */

/** @defgroup SPI_SCPH_definitions 
  * @{
  */ 
#define SCPH_TOGGLES_IN_MIDDLE		(0)
#define SCPH_TOGGLES_AT_START		(1)
/**
  * @}
  */

/** @defgroup SPI_Data_Frame_Size_definitions 
  * @{
  */ 
#define DFS_4_BITS				(3)
#define DFS_5_BITS				(4)
#define DFS_6_BITS				(5)
#define DFS_7_BITS				(6)
#define DFS_8_BITS				(7)
#define DFS_9_BITS				(8)
#define DFS_10_BITS				(9)
#define DFS_11_BITS				(10)
#define DFS_12_BITS				(11)
#define DFS_13_BITS				(12)
#define DFS_14_BITS				(13)
#define DFS_15_BITS				(14)
#define DFS_16_BITS				(15)
/**
  * @}
  */

/** @defgroup SPI_SS_TOGGLE_PHASE_definitions 
  * @{
  */ 
#define SPI_SS_NOT_TOGGLE		(0)
#define SPI_SS_TOGGLE		(1)
/**
  * @}
  */

/** @defgroup SPI_MW_Control_Frame_Size_definitions 
  * @{
  */ 
#define CFS_1_BIT				(0)
#define CFS_2_BITS				(1)
#define CFS_3_BITS				(2)
#define CFS_4_BITS				(3)
#define CFS_5_BITS				(4)
#define CFS_6_BITS				(5)
#define CFS_7_BITS				(6)
#define CFS_8_BITS				(7)
#define CFS_9_BITS				(8)
#define CFS_10_BITS				(9)
#define CFS_11_BITS				(10)
#define CFS_12_BITS				(11)
#define CFS_13_BITS				(12)
#define CFS_14_BITS				(13)
#define CFS_15_BITS				(14)
#define CFS_16_BITS				(15)
/**
  * @}
  */

/** @defgroup SPI_ROLE_definitions 
  * @{
  */ 
#define SSI_SLAVE				(0)
#define SSI_MASTER				(1)
/**
  * @}
  */

/** @defgroup SPI_Frame_Format_definitions 
  * @{
  */ 
#define FRF_MOTOROLA_SPI		(0)
#define FRF_TI_SSP				(1)
#define FRF_NS_MICROWIRE		(2)
#define FRF_RSVD				(3)
/**
  * @}
  */

/** @defgroup SPI_DMA_Control_definitions 
  * @{
  */ 
#define SSI_NODMA				(0)
#define SSI_RXDMA_ENABLE		(1)
#define SSI_TXDMA_ENABLE		(2)
#define SSI_TRDMA_ENABLE		(3)
/**
  * @}
  */

/** @defgroup SPI_MW_Handshake_Enable_definitions 
  * @{
  */ 
#define MW_HANDSHAKE_DISABLE		(0)
#define MW_HANDSHAKE_ENABLE			(1)
/**
  * @}
  */

/** @defgroup SPI_MW_Direction_definitions 
  * @{
  */ 
#define MW_DIRECTION_SLAVE_TO_MASTER	(0)
#define MW_DIRECTION_MASTER_TO_SLAVE	(1)
/**
  * @}
  */

/** @defgroup SPI_MW_TMOD_definitions 
  * @{
  */ 
#define MW_TMOD_NONSEQUENTIAL		(0)
#define MW_TMOD_SEQUENTIAL			(1)
/**
  * @}
  */

/** @defgroup SPI_FIFO_depth_definitions 
  * @{
  */ 
#define SSI_TX_FIFO_DEPTH				(64)
#define SSI_RX_FIFO_DEPTH				(64)
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SPI_Exported_Functions SPI Exported Functions
  * @{
  */
/** @defgroup SPI_Exported_Normal_Functions SPI Exported Normal Functions
  * @{
  */
_LONG_CALL_ void SSI_Cmd(SPI_TypeDef *spi_dev, u32 NewStaus);
_LONG_CALL_ void SSI_SetSclkPolarity(SPI_TypeDef *spi_dev, u32 SclkPolarity);
_LONG_CALL_ void SSI_SetSclkPhase(SPI_TypeDef *spi_dev, u32 SclkPhase);
_LONG_CALL_ void SSI_WriteData(SPI_TypeDef *spi_dev, u32 value);
_LONG_CALL_ VOID SSI_INTConfig(SPI_TypeDef* spi_dev, u32 SSI_IT, u32 newState);
_LONG_CALL_ void SSI_SetRxFifoLevel(SPI_TypeDef *spi_dev, u32 RxThresholdLevel);
_LONG_CALL_ void SSI_SetTxFifoLevel(SPI_TypeDef *spi_dev, u32 TxThresholdLevel);
_LONG_CALL_ void SSI_SetSlaveEnable(SPI_TypeDef *spi_dev, u32 SlaveIndex);
_LONG_CALL_ u32 SSI_Busy(SPI_TypeDef *spi_dev);
_LONG_CALL_ u32 SSI_Writeable(SPI_TypeDef *spi_dev);
_LONG_CALL_ u32 SSI_Readable(SPI_TypeDef *spi_dev);
_LONG_CALL_ u32 SSI_GetRxCount(SPI_TypeDef *spi_dev);
_LONG_CALL_ u32 SSI_GetTxCount(SPI_TypeDef *spi_dev);
_LONG_CALL_ u32 SSI_GetStatus(SPI_TypeDef *spi_dev);
_LONG_CALL_ u32 SSI_GetIsr(SPI_TypeDef *spi_dev);
_LONG_CALL_ u32 SSI_ReadData(SPI_TypeDef *spi_dev);
_LONG_CALL_ u32 SSI_ReceiveData(SPI_TypeDef *spi_dev, void* RxData, u32 Length);
_LONG_CALL_ u32 SSI_SendData(SPI_TypeDef *spi_dev, void* TxData, u32 Length, u32 Role);
_LONG_CALL_ u32 SSI_GetRawIsr(SPI_TypeDef *spi_dev);
_LONG_CALL_ u32 SSI_GetSlaveEnable(SPI_TypeDef *spi_dev);
_LONG_CALL_ u32 SSI_GetDataFrameSize(SPI_TypeDef *spi_dev);
_LONG_CALL_ void SSI_SetSampleDelay(SPI_TypeDef *spi_dev, u32 SampleDelay);
_LONG_CALL_ void SSI_Init(SPI_TypeDef *spi_dev, SSI_InitTypeDef *SSI_InitStruct);
_LONG_CALL_ void SSI_StructInit(SSI_InitTypeDef* SSI_InitStruct);
_LONG_CALL_ void SSI_SetDataFrameSize(SPI_TypeDef *spi_dev, u32 DataFrameSize);
_LONG_CALL_ void SSI_SetBaud(SPI_TypeDef *SPIx, u32 BaudRate, u32 IpClk);
_LONG_CALL_ void SSI_SetIsrClean(SPI_TypeDef *spi_dev, u32 InterruptStatus);
_LONG_CALL_ void SSI_SetReadLen(SPI_TypeDef *spi_dev, u32 DataFrameNumber);
/**
  * @}
  */

/** @defgroup SPI_Exported_DMA_Functions SPI Exported DMA Functions
  * @{
  */
_LONG_CALL_ BOOL SSI_TXGDMA_Init(u32 Index, PGDMA_InitTypeDef GDMA_InitStruct, void *CallbackData, 
										IRQ_FUN CallbackFunc, u8 *pTxData, u32 Length);
_LONG_CALL_ BOOL SSI_RXGDMA_Init(u8 Index, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, 
										IRQ_FUN CallbackFunc, u8  *pRxData, u32 Length);
_LONG_CALL_ void SSI_SetDmaEnable(SPI_TypeDef *spi_dev, u32 newState, u32 Mask);
_LONG_CALL_ void SSI_SetDmaLevel(SPI_TypeDef *spi_dev, u32 TxLeve, u32 RxLevel);
_LONG_CALL_ void SSI_SetBaudDiv(SPI_TypeDef *spi_dev, u32 ClockDivider);
_LONG_CALL_ void SSI_SetRole(SPI_TypeDef *spi_dev, u32 role);

/**
  * @}
  */

/**
  * @}
  */



/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup SPI_Register_Definitions SPI Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup SPI_CTRLR0
 * @{
 *****************************************************************************/
#define BIT_CTRLR0_DFS				((u32)0x0000000F)
#define BIT_CTRLR0_FRF				((u32)0x00000003 << 4)
#define BIT_CTRLR0_SCPH				((u32)0x00000001 << 6)
#define BIT_CTRLR0_SCPOL			((u32)0x00000001 << 7)
#define BIT_CTRLR0_TMOD				((u32)0x00000003 << 8)
#define BIT_CTRLR0_SLV_OE			((u32)0x00000001 << 10)
#define BIT_CTRLR0_SRL				((u32)0x00000001 << 11)
#define BIT_CTRLR0_CFS				((u32)0x0000000F << 12)
#define BIT_CTRLR0_TXBYTESWP		((u32)0x00000001 << 21)
#define BIT_CTRLR0_TXBITSWP		((u32)0x00000001 << 22)
#define BIT_CTRLR0_RXBYTESWP		((u32)0x00000001 << 23)
#define BIT_CTRLR0_RXBITSWP		((u32)0x00000001 << 24)
#define BIT_CTRLR0_SSTOGGLE		((u32)0x00000001 << 31)
/** @} */

/**************************************************************************//**
 * @defgroup SPI_CTRLR1
 * @{
 *****************************************************************************/
#define BIT_CTRLR1_NDF				((u32)0x0000FFFF)
/** @} */

/**************************************************************************//**
 * @defgroup SPI_SSIENR
 * @{
 *****************************************************************************/
#define BIT_SSIENR_SSI_EN			((u32)0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup SPI_MWCR
 * @{
 *****************************************************************************/
#define BIT_MWCR_MWMOD				((u32)0x00000001)
#define BIT_MWCR_MDD				((u32)0x00000001 << 1)
#define BIT_MWCR_MHS				((u32)0x00000001 << 2)
/** @} */

/**************************************************************************//**
 * @defgroup SPI_SER
 * @{
 *****************************************************************************/
#define BIT_SER_SER					((u32)0x0000FFFF)
/** @} */

/**************************************************************************//**
 * @defgroup SPI_BAUDR
 * @{
 *****************************************************************************/
#define BIT_BAUDR_SCKDV			((u32)0x0000FFFF)
/** @} */

/**************************************************************************//**
 * @defgroup SPI_TXFLTR
 * @{
 *****************************************************************************/
#define BIT_TXFTLR_TFT				((u32)0x0000003F)//(TX_ABW-1):0
/** @} */

/**************************************************************************//**
 * @defgroup SPI_RXFLTR
 * @{
 *****************************************************************************/
#define BIT_RXFTLR_RFT				((u32)0x0000003F)  // (RX_ABW-1):0
/** @} */

/**************************************************************************//**
 * @defgroup SPI_TXFLR
 * @{
 *****************************************************************************/
#define BIT_MASK_TXFLR_TXTFL		((u32)0x0000007F)  // (TX_ABW):0
/** @} */

/**************************************************************************//**
 * @defgroup SPI_RXFLR
 * @{
 *****************************************************************************/
#define BIT_MASK_RXFLR_RXTFL		((u32)0x0000007F)  // (RX_ABW):0
/** @} */

/**************************************************************************//**
 * @defgroup SPI_SR
 * @{
 *****************************************************************************/
#define BIT_SR_BUSY					((u32)0x00000001)
#define BIT_SR_TFNF					((u32)0x00000001 << 1)
#define BIT_SR_TFE					((u32)0x00000001 << 2)
#define BIT_SR_RFNE					((u32)0x00000001 << 3)
#define BIT_SR_RFF					((u32)0x00000001 << 4)
#define BIT_SR_TXE					((u32)0x00000001 << 5)
#define BIT_SR_DCOL					((u32)0x00000001 << 6)
/** @} */

/**************************************************************************//**
 * @defgroup SPI_IMR
 * @{
 *****************************************************************************/
#define BIT_IMR_TXEIM				((u32)0x00000001)
#define BIT_IMR_TXOIM				((u32)0x00000001 << 1)
#define BIT_IMR_RXUIM				((u32)0x00000001 << 2)
#define BIT_IMR_RXOIM				((u32)0x00000001 << 3)
#define BIT_IMR_RXFIM				((u32)0x00000001 << 4)
#define BIT_IMR_MSTIM				((u32)0x00000001 << 5)       /*Master only*/
#define BIT_IMR_FAEIM				((u32)0x00000001 << 5)       /*Slave only*/
#define BIT_IMR_TXUIM				((u32)0x00000001 << 6)       /*Slave only*/
#define BIT_IMR_SSRIM				((u32)0x00000001 << 7)       /*Slave only*/
/** @} */

/**************************************************************************//**
 * @defgroup SPI_ISR
 * @{
 *****************************************************************************/
#define BIT_ISR_TXEIS				((u32)0x00000001)
#define BIT_ISR_TXOIS				((u32)0x00000001 << 1)
#define BIT_ISR_RXUIS				((u32)0x00000001 << 2)
#define BIT_ISR_RXOIS				((u32)0x00000001 << 3)
#define BIT_ISR_RXFIS				((u32)0x00000001 << 4)
#define BIT_ISR_MSTIS				((u32)0x00000001 << 5)       /*Master only*/
#define BIT_ISR_FAEIS				((u32)0x00000001 << 5)       /*Slave only*/
#define BIT_ISR_TXUIS				((u32)0x00000001 << 6)       /*Slave only*/
#define BIT_ISR_SSRIS				((u32)0x00000001 << 7)       /*Slave only*/
/** @} */

/**************************************************************************//**
 * @defgroup SPI_RISR
 * @{
 *****************************************************************************/
#define BIT_RISR_TXEIR				((u32)0x00000001)
#define BIT_RISR_TXOIR				((u32)0x00000001 << 1)
#define BIT_RISR_RXUIR				((u32)0x00000001 << 2)
#define BIT_RISR_RXOIR				((u32)0x00000001 << 3)
#define BIT_RISR_RXFIR				((u32)0x00000001 << 4)
#define BIT_RISR_MSTIR				((u32)0x00000001 << 5)       /*Master only*/
#define BIT_RISR_FAEIS				((u32)0x00000001 << 5)       /*Slave only*/
#define BIT_RISR_TXUIS				((u32)0x00000001 << 6)       /*Slave only*/
#define BIT_RISR_SSRIS				((u32)0x00000001 << 7)       /*Slave only*/
/** @} */

/**************************************************************************//**
 * @defgroup SPI_DMACR
 * @{
 *****************************************************************************/
#define BIT_SHIFT_DMACR_RDMAE		((u32)0x00000001)
#define BIT_SHIFT_DMACR_TDMAE		((u32)0x00000001 << 1)
/** @} */

/**************************************************************************//**
 * @defgroup SPI_DMATDLR
 * @{
 *****************************************************************************/
#define BIT_DMATDLR_DMATDL		((u32)0x0000003F) // (TX_ABW-1):0
/** @} */

/**************************************************************************//**
 * @defgroup SPI_DMARDLR
 * @{
 *****************************************************************************/
#define BIT_DMARDLR_DMARDL		((u32)0x0000003F )// (RX_ABW-1):0
/** @} */

/**************************************************************************//**
 * @defgroup SPI_DR
 * @{
 *****************************************************************************/
#define BIT_DR_DR					((u32)0x0000FFFF)
/** @} */

/**************************************************************************//**
 * @defgroup SPI_RX_SAMPLE_DELAY
 * @{
 *****************************************************************************/
#define BIT_RX_SAMPLE_DELAY				((u32)0x000000FF)
/** @} */
/** @} */

/**
  * @}
  */

/**
  * @}
  */

/* Other Definitions --------------------------------------------------------*/
typedef struct
{
	SPI_TypeDef* SPIx;
	u32 Tx_HandshakeInterface;
	u32 Rx_HandshakeInterface;
	IRQn_Type IrqNum;
} SPI_DevTable;

extern const SPI_DevTable SPI_DEV_TABLE[2];

#define SPI_SLAVE_TXERR_WORK_AROUND		1

#endif //_RTL8710B_SPI_H_

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
