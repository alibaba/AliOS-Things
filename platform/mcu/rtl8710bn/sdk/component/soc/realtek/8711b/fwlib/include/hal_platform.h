/**
  ******************************************************************************
  * @file    hal_platform.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the definations of platform.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */
#ifndef _HAL_PLATFORM_
#define _HAL_PLATFORM_

#define ROMVERSION			0x01
#define ROMINFORMATION		(ROMVERSION)

#define HAL_READ32(base, addr)				rtk_le32_to_cpu(*((volatile u32*)(base + addr)))    
#define HAL_WRITE32(base, addr, value32)		((*((volatile u32*)(base + addr))) = rtk_cpu_to_le32(value32))
#define HAL_READ16(base, addr)				rtk_le16_to_cpu(*((volatile u16*)(base + addr)))        
#define HAL_WRITE16(base, addr, value)		((*((volatile u16*)(base + addr))) = rtk_cpu_to_le16(value)) 
#define HAL_READ8(base, addr)				(*((volatile u8*)(base + addr)))            
#define HAL_WRITE8(base, addr, value)		((*((volatile u8*)(base + addr))) = value)
 

#ifdef __cplusplus
#define __I		volatile             /*!< Defines 'read only' permissions                 */
#else
#define __I		volatile const       /*!< Defines 'read only' permissions                 */
#endif
#define __O		volatile             /*!< Defines 'write only' permissions                */
#define __IO	 	volatile             /*!< Defines 'read / write' permissions              */

/**************************************************************************//**
 * @defgroup AmebaZ_Outline
 * @{
 *****************************************************************************/

/** @addtogroup AmebaZ_Outline
  * @verbatim
  *****************************************************************************************
  * CPU
  *****************************************************************************************
  *	-Clock speed:		125MHz
  *	-Core:				CM4F
  *	-Working votage:	3.3V
  *
  *****************************************************************************************
  * Memory
  *****************************************************************************************
  *	-ROM:				512KB
  *	-RAM:				256KB
  *	-FLash:				128MB
  *
  *****************************************************************************************     
  * GPIO
  ***************************************************************************************** 
  *	-GPIOA:				0~31, IN/OUT/INT
  *	-GPIOB:				0~6, IN/OUT
  *
  *****************************************************************************************
  * Flash
  ***************************************************************************************** 
  *	-Max. speed:		100MHz
  *	-SPI Mode:			Quad IO mode
  *	-cache:				32K I/D read cache
  *	-XIP:				support executed in place
  *
  ***************************************************************************************** 
  * Firmware Protection
  ***************************************************************************************** 
  *	-RSIP(OTF):(Realsil Image Protection)
  *		-Flash Encryption
  *		-key store in OTP EFUSE Area, and can not read
  *	-RDP
  *		-4K RAM read protection
  *		-key store in OTP EFUSE Area, and can not read
  *
  ***************************************************************************************** 
  * UART
  ***************************************************************************************** 
  *	-Set count:			2
  *	-IrDA:				support
  *	-Low Power:			support
  *	-Sleep wakeup:		support
  *	-Baud rate:			110~6000000
  *	-GDMA:				support
  *
  ***************************************************************************************** 
  * UART LOG
  ***************************************************************************************** 
  *	-Set count:			1
  *	-IrDA:				support
  *	-Low Power:			not support
  *	-Sleep wakeup:		not support
  *	-Baud rate:			110~6000000
  *
  ***************************************************************************************** 
  * SPI Master
  ***************************************************************************************** 
  *	-Set count:			1
  *	-Max. Speed:		31.25MHz
  *	-GDMA:				support
  *
  ***************************************************************************************** 
  * SPI Slave
  ***************************************************************************************** 
  *	-Set count:			1
  *	-Max. Speed:		31.25MHz
  *	-GDMA:				support
  *
  ***************************************************************************************** 
  * I2C
  ***************************************************************************************** 
  *	-Set count:			2
  *	-Speed:				Standard (up to 100 kHz) and Fast (up to 400 kHz) Modes
  *	-Role:				Master & Slave
  *	-Sleep wakeup:		support
  *	-GDMA:				support
  *
  ***************************************************************************************** 
  * RTC
  ***************************************************************************************** 
  *	-Set count:			1
  *	-H:					12/24 hour mode
  *	-M:					0~59
  *	-S:					0~59
  *	-D:					0~255
  *	-Alarm:				support D/H/M/S
  *	-Daylight:			support
  *	-Calibration:		support
  *
  ***************************************************************************************** 
  * PWM
  ***************************************************************************************** 
  *	-Channel count:		6
  *	-Max. Resolution:	16bits (customizable)
  *	-Max. Frequency:	40MHz@1-bit mode, 40/(2^x))MHz@x-bit
  *	-GDMA:				support
  *
  ***************************************************************************************** 
  * PWM capture
  ***************************************************************************************** 
  *	-Channel count:		1
  *	-Max. Resolution:	16bits (customizable)
  *	-Max. Frequency:	40MHz@1-bit mode, 40/(2^x))MHz@x-bit
  *	-capture width:		supoort
  *	-capture number:	support
  *	-GDMA:				support
  *
  ***************************************************************************************** 
  * Timer
  ***************************************************************************************** 
  *	-Set count:			6
  *	-32KHz timer:		4
  *	-40MHz timer:		2
  *
  ***************************************************************************************** 
  * I2S
  ***************************************************************************************** 
  *	-Set count:			1
  *	-Source clk:		22.579MHz or 24.576MHz(default)
  *	-Sample rate:		8K, 16K, 24K, 32K, 48K, 96K, 7.35K, 14.7K, 22.05K, 29.4K, 44.1K, 88.2K
  *	-Sample bit:		16 bit, 24 bit
  *	-DMA:				support
  *
  ***************************************************************************************** 
  * WIFI
  ***************************************************************************************** 
  *	-Set count:			1
  *	-Mode:				B/G/N(2.4G)
  *	-BW:				20MHz/40MHz
  *	-Ant:				1T/1R
  *
  ***************************************************************************************** 
  * USB device
  ***************************************************************************************** 
  *	-Set count:			1
  *	-DMA:				support
  *	
  ***************************************************************************************** 
  * SDIO device
  ***************************************************************************************** 
  *	-Set count:			1
  *	-GSPI:				support
  *	-DMA:				support
  *
  ***************************************************************************************** 
  * BACKUP REG
  ***************************************************************************************** 
  *	-Set count:			4 dwords
  *
  ***************************************************************************************** 
  * CRYPTO
  ***************************************************************************************** 
  *	-MD5
  *	-SHA1
  *	-SHA2
  *	-HMAC-MD5
  *	-HMAC-SHA1
  *	-HMAC-SHA2
  *
  *	-AES-CBC
  *	-AES-ECB
  *	-AES-CTR
  *	-3DES-CBC
  *	-3DES-ECB
  *	-DES-CBC
  *	-DES-ECB
  *
  ***************************************************************************************** 
  * ADC
  ***************************************************************************************** 
  *	-Set count:			1
  *	-Channel:			4
  *	-Sample rate:		max frequency up to 1MHz per channel, configurable
  *	-Resolution:		12 bit
  *	-signal sampling:	CH1 & CH3: 0 ~ 3V, CH2: 0~5V
  *	-power save:		Support one shot mode
  *	-GDMA:				support
  *
  ***************************************************************************************** 
  * GDMA
  ***************************************************************************************** 
  *	-Set count:			2
  *	-Channel:			2*6
  *
  ***************************************************************************************** 
  * @endverbatim
  */
  
/**************************************************************************//**
 * @defgroup AmebaZ_Address_Map AmebaZ Address Map
 * @{
 *****************************************************************************/
#define SYSTEM_CTRL_BASE            0x40000000
#define PERI_ON_BASE			0x40000000
#define VENDOR_REG_BASE             0x40002800
#define SPI_FLASH_BASE			0x08000000

#define NCO1_REG_BASE			0x40000080
#define BACKUP_REG_BASE		0x40000138
#define NCO2_REG_BASE			0x4000026C
#define PINMUX_REG_BASE		0x40000280

#define GPIO_REG_BASE			0x40001000
#define TIMER_REG_BASE			0x40002000
#define LOG_UART_REG_BASE		0x40003000
#define RTC_BASE				0x40003400
#define SPIC_CACHE_BASE		0x40003C00
#define ADC_REG_BASE			0x40010000
#define SPI_FLASH_CTRL_BASE	0x40020000
#define UART0_REG_BASE			0x40040000
#define UART1_REG_BASE			0x40040400
#define UART2_REG_BASE			LOG_UART_REG_BASE
#define SPI0_REG_BASE			0x40042000
#define SPI1_REG_BASE			0x40042400
#define I2C0_REG_BASE			0x40044000
#define I2C1_REG_BASE			0x40044400
#define SDIO_DEVICE_REG_BASE	0x40050000
#define GDMA0_REG_BASE			0x40060000
#define GDMA1_REG_BASE              0x40061000
#define I2S0_REG_BASE			0x40062000
#define CRYPTO_REG_BASE		0x40070000
#define WIFI_REG_BASE			0x40080000
#define SIE_REG_BASE			0x400C0000
#define USOC_REG_BASE			0x400C2000
#define GDMA1_REG_OFF			0x1000
/** @} End of group AmebaZ_Address_Map */

/**************************************************************************//**
 * @defgroup AmebaZ_Peripheral_Registers_Structures  AmebaZ Peripheral_Registers_Structures
 * @{
 *****************************************************************************/

/**************************************************************************//**
 * @defgroup AMEBAZ_UART
 * @{
 * @brief AMEBAZ_UART Register Declaration
 *****************************************************************************/
typedef struct {
	__IO u32 DLL;				/*!< UART Divisor Latch register(not used in Amebaz),	Address offset: 0x00*/
	__IO u32 DLH_INTCR;			/*!< UART interrupt enable register,              			Address offset: 0x04*/
	__IO u32 INTID;				/*!< UART interrupt identification register,              	Address offset: 0x08*/
	__IO u32 LCR;				/*!< UART line control register,              				Address offset: 0x0C*/
	__IO u32 MCR;				/*!< UART modem control register,              			Address offset: 0x10*/
	__I u32 LSR;					/*!< UART line status register,              				Address offset: 0x14*/
	__I u32 MDSR;				/*!< UART modem status register,              			Address offset: 0x18*/
	__IO u32 SPR;				/*!< UART scratch pad register,              				Address offset: 0x1C*/
	__IO u32 STSR;				/*!< UART STS register,              					Address offset: 0x20*/
	__IO u32 RB_THR;			/*!< UART receive buffer/transmitter holding register,	Address offset: 0x24*/
	__IO u32 MISCR;				/*!< UART MISC control register,              			Address offset: 0x28*/
	__IO u32 TXPLSR;				/*!< UART IrDA SIR TX Pulse Width Control register,	Address offset: 0x2C*/
	
	__IO u32 RXPLSR;			/*!< UART IrDA SIR RX Pulse Width Control register,	Address offset: 0x30*/
	__IO u32 BAUDMONR;			/*!< UART baud monitor register,              			Address offset: 0x34*/
	__IO u32 RSVD2;				/*!< UART reserved field,              					Address offset: 0x38*/
	__IO u32 DBG_UART;			/*!< UART debug register,              					Address offset: 0x3C*/

	/* AmebaZ add for power save */
	__IO u32 RX_PATH;			/*!< UART rx path control register,              			Address offset: 0x40*/
	__IO u32 MON_BAUD_CTRL;	/*!< UART monitor baud rate control register,              	Address offset: 0x44*/
	__IO u32 MON_BAUD_STS;		/*!< UART monitor baud rate status register,              	Address offset: 0x48*/
	__IO u32 MON_CYC_NUM;		/*!< UART monitor cycle number register,              		Address offset: 0x4c*/
	__IO u32 RX_BYTE_CNT;		/*!< UART rx byte counter register,              			Address offset: 0x50*/

	/* AmebaZ change */
	__IO u32 FCR;				/*!< UART FIFO Control register,              			Address offset: 0x54*/
} UART_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAZ_SPI
 * @{
 * @brief AMEBAZ_SPI Register Declaration
 *****************************************************************************/
typedef struct {
    __IO  uint32_t CTRLR0;				/*!< SSI control register 0,								Address offset: 0x00 */
    __IO  uint32_t CTRLR1;				/*!< SSI control register 1,								Address offset: 0x04 */
    __IO  uint32_t SSIENR;				/*!< SSI enable register,									Address offset: 0x08 */
    __IO  uint32_t MWCR;				/*!< SSI microwire control register,						Address offset: 0x0C */
    __IO  uint32_t SER;					/*!< SSI slave enable register,								Address offset: 0x10 */
    __IO  uint32_t BAUDR;				/*!< SSI baud rate select register,							Address offset: 0x14 */
    __IO  uint32_t TXFTLR;				/*!< SSI transmit FIFO threshold level register,			Address offset: 0x18 */
    __IO  uint32_t RXFTLR;				/*!< SSI Receive FIFO Threshold Level register,				Address offset: 0x1C */
    __I  uint32_t TXFLR;				/*!< SSI transmit FIFO level register,						Address offset: 0x20 */
    __I  uint32_t RXFLR;				/*!< SSI receive FIFO level register,						Address offset: 0x24*/
    __I  uint32_t SR;					/*!< SSI status register,									Address offset: 0x28 */
    __IO  uint32_t IMR;					/*!< SSI interrupt mask register,							Address offset: 0x2C */
    __I  uint32_t ISR;					/*!< SSI interrupt status register,							Address offset: 0x30 */
    __I  uint32_t RISR;					/*!< SSI raw interrupt status register,						Address offset: 0x34 */
    __I  uint32_t TXOICR;				/*!< SSI transmit FIFO overflow interrupt clear register,	Address offset: 0x38 */
    __I  uint32_t RXOICR;				/*!< SSI receive FIFO overflow interrupt clear register,	Address offset: 0x3C */
    __I  uint32_t RXUICR;				/*!< SSI receive FIFO underflow interrupt clear register,	Address offset: 0x40 */
    __I  uint32_t MSTICR;				/*!< SSI multi-master interrupt clear register,				Address offset: 0x44 */
    __I  uint32_t ICR;					/*!< SSI interrupt clear register,							Address offset: 0x48 */
    __IO  uint32_t DMACR;				/*!< SSI DMA control register,								Address offset: 0x4C */
    __IO  uint32_t DMATDLR;				/*!< SSI DMA transmit data level register,					Address offset: 0x50 */
    __IO  uint32_t DMARDLR;				/*!< SSI DMA receive data level register,					Address offset: 0x54 */
    __I  uint32_t IDR;					/*!< SSI identification register,							Address offset: 0x58 */
    __I  uint32_t SSI_COMP_VERSION;		/*!< SSI coreKit version ID register,						Address offset: 0x5C */
    __IO  uint32_t DR[36];				/*!< SSI data register,										Address offset: 0x60 - 0xEC */
    __IO  uint32_t RX_SAMPLE_DLY;		/*!< SSI rxd sample delay register,							Address offset: 0xF0 */
}SPI_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAZ_SPIC
 * @{
 * @brief AMEBAZ_SPIC Register Declaration
 *****************************************************************************/
typedef struct
{
	__IO uint32_t ctrlr0;			/*!< SPIC control register0,		Address offset: 0x000 */
	__IO uint32_t ctrlr1;			/*!< SPIC control register1,		Address offset: 0x004 */
	__IO uint32_t ssienr;			/*!< SPIC enable register,		Address offset: 0x008 */
	__IO uint32_t mwcr;			/*!< N/A,					Address offset: 0x00C */
	__IO uint32_t ser;				/*!< SPIC slave enable register,	Address offset: 0x010 */
	__IO uint32_t baudr;			/*!< SPIC baudrate select register,	Address offset: 0x014 */
	__IO uint32_t txftlr;				/*!< SPIC transmit FIFO threshold level,	Address offset: 0x018 */
	__IO uint32_t rxftlr;				/*!< SPIC receive FIFO threshold level,	Address offset: 0x01C */
	__IO uint32_t txflr;				/*!< SPIC transmit FIFO level register,	Address offset: 0x020 */
	__IO uint32_t rxflr;				/*!< SPIC receive FIFO level register,	Address offset: 0x024 */  
	__IO uint32_t sr;				/*!< SPIC status register,				Address offset: 0x028 */
	__IO uint32_t imr;				/*!< SPIC interrupt mask register,		Address offset: 0x02C */
	__IO uint32_t isr;				/*!< SPIC interrupt status register,		Address offset: 0x030 */
	__IO uint32_t risr;				/*!< SPIC raw interrupt status register,	Address offset: 0x034 */
	__IO uint32_t txoicr;			/*!< SPIC transmit FIFO overflow interrupt clear register,	Address offset: 0x038 */
	__IO uint32_t rxoicr;			/*!< SPIC receive FIFO overflow interrupt clear register,	Address offset: 0x03C */    
	__IO uint32_t rxuicr;			/*!< SPIC receive FIFO underflow interrupt clear register,	Address offset: 0x040 */
	__IO uint32_t msticr;			/*!< SPIC master error interrupt clear register,	Address offset: 0x044 */
	__IO uint32_t icr;				/*!< SPIC interrupt clear register,	Address offset: 0x048 */ 
	__IO uint32_t dmacr;			/*!< N/A,					Address offset: 0x04C */
	__IO uint32_t dmatdlr;			/*!< N/A,					Address offset: 0x050 */
	__IO uint32_t dmardlr;			/*!< N/A,					Address offset: 0x054 */
	__IO uint32_t idr;				/*!< SPIC Identiation register,		Address offset: 0x058 */
	__IO uint32_t spi_flash_version;	/*!< SPIC version ID register,		Address offset: 0x05C */
	union{
		__IO uint8_t  byte;
		__IO uint16_t half;
		__IO uint32_t word;
	} dr[32];						/*!< SPIC data register,					Address offset: 0x060~0x0DC */
	__IO uint32_t rd_fast_single;		/*!< Fast read data command of SPI Flash,	Address offset: 0x0E0 */
	__IO uint32_t rd_dual_o;		/*!< Dual output read command of SPI Flash,	Address offset: 0x0E4 */
	__IO uint32_t rd_dual_io;		/*!< Dual I/O read command of SPI Flash,	Address offset: 0x0E8 */
	__IO uint32_t rd_quad_o; 		/*!< Quad output read command of SPI Flash,	Address offset: 0x0EC */
	__IO uint32_t rd_quad_io;		/*!< Quad I/O read command of SPI Flash,	Address offset: 0x0F0 */
	__IO uint32_t wr_single;			/*!< Page program command of SPI Flash,	Address offset: 0x0F4 */
	__IO uint32_t wr_dual_i;			/*!< Dual data input program command of SPI Flash,			Address offset: 0x0F8 */
	__IO uint32_t wr_dual_ii;		/*!< Dual address and data input program command of SPI Flash,	Address offset: 0x0FC */
	__IO uint32_t wr_quad_i;		/*!< Quad data input program command of SPI Flash,			Address offset: 0x100 */ 
	__IO uint32_t wr_quad_ii;		/*!< Quad address and data input program command of SPI Flash,	Address offset: 0x104 */
	__IO uint32_t wr_enable;		/*!< Write enabe command of SPI Flash,	Address offset: 0x108 */
	__IO uint32_t rd_status;			/*!< Read status command of SPI Flash,	Address offset: 0x10C */
	__IO uint32_t ctrlr2;			/*!< SPIC control register2,			Address offset: 0x110 */
	__IO uint32_t fbaudr;			/*!< SPIC fast baudrate select,			Address offset: 0x114 */
	__IO uint32_t addr_length;		/*!< SPIC address length register,		Address offset: 0x118 */
	__IO uint32_t auto_length;		/*!< SPIC auto address length register,	Address offset: 0x11C */
	__IO uint32_t valid_cmd;		/*!< SPIC valid command register,		Address offset: 0x120 */
	__IO uint32_t flash_size;			/*!< SPIC flash size register,			Address offset: 0x124 */
	__IO uint32_t flush_fifo;			/*!< SPIC flush FIFO register,			Address offset: 0x128 */
} SPIC_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAZ_ADC
 * @{
 * @brief AMEBAZ_ADC Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t FIFO_READ;		/*!< ADC chan0-chan3 RX FIFO read register,	Address offset: 0x00 */
	__IO uint32_t CONTROL;			/*!< ADC control register,   				Address offset: 0x04 */
	__IO uint32_t INTR_EN;			/*!< ADC interrupt enable register,   		Address offset: 0x08 */
	__IO uint32_t INTR_STS;			/*!< ADC interrupt status register,   		Address offset: 0x0C */
	__IO uint32_t COMP_VALUE_L;		/*!< ADC chan0-1 compare value register,   	Address offset: 0x10 */
	__IO uint32_t COMP_VALUE_H;		/*!< ADC chan2-3 compare value register,   	Address offset: 0x14 */
	__IO uint32_t COMP_SET;			/*!< ADC compare setting register,   		Address offset: 0x18 */
	__IO uint32_t POWER;			/*!< ADC power register,   					Address offset: 0x1C */
	__IO uint32_t ANAPAR_AD0;		/*!< ADC analog parameter ad0 register,   	Address offset: 0x20 */
	__IO uint32_t ANAPAR_AD1;		/*!< ADC analog parameter ad1 register,   	Address offset: 0x24 */
	__IO uint32_t ANAPAR_AD2;		/*!< ADC analog parameter ad2 register,   	Address offset: 0x28 */
	__IO uint32_t ANAPAR_AD3;		/*!< ADC analog parameter ad3 register,   	Address offset: 0x2C */
	__IO uint32_t ANAPAR_AD4;		/*!< ADC analog parameter ad4 register,   	Address offset: 0x30 */
	__IO uint32_t ANAPAR_AD5;		/*!< ADC analog parameter ad5 register,   	Address offset: 0x34 */
	__IO uint32_t CALI_DATA;		/*!< ADC calibration data register,   		Address offset: 0x38 */
} ADC_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAZ_I2C
 * @{
 * @brief AMEBAZ_I2C Register Declaration
 *****************************************************************************/
typedef struct
{
	__IO uint32_t IC_CON;				/*!< I2C control register,              Address offset: 0x00 */
	__IO uint32_t IC_TAR;				/*!< I2C target address register,              Address offset: 0x04 */
	__IO uint32_t IC_SAR; 				/*!< I2C slave0 address register,              Address offset: 0x08 */
	__IO uint32_t IC_HS_MADDR;		/*!< I2C HS master mode code address register,              Address offset: 0x0C */

	__IO uint32_t IC_DATA_CMD;		/*!< I2C rx/tx data buffer and command register,              Address offset: 0x10 */
	__IO uint32_t IC_SS_SCL_HCNT;	/*!< I2C standard speed clock SCL high count register,              Address offset: 0x14 */
	__IO uint32_t IC_SS_SCL_LCNT;	/*!< I2C standard speed clock SCL low count register,              Address offset: 0x18 */
	__IO uint32_t IC_FS_SCL_HCNT;	/*!< I2C fast speed clock SCL high count register,              Address offset: 0x1C */

	__IO uint32_t IC_FS_SCL_LCNT;	/*!< I2C fast speed clock SCL low count register,              Address offset: 0x20 */
	__IO uint32_t IC_HS_SCL_HCNT;	/*!< I2C high speed clock SCL high count register,              Address offset: 0x24 */
	__IO uint32_t IC_HS_SCL_LCNT; 	/*!< I2C high speed clock SCL low count register,              Address offset: 0x28 */
	__I uint32_t IC_INTR_STAT;		/*!< I2C interrupt status register,              Address offset: 0x2C */

	__IO uint32_t IC_INTR_MASK;		/*!< I2C interrupt mask register,              Address offset: 0x30 */
	__I uint32_t IC_RAW_INTR_STAT;	/*!< I2C raw interrupt status register,              Address offset: 0x34 */  
	__IO uint32_t IC_RX_TL;			/*!< I2C receive FIFO threshold register,              Address offset: 0x38 */
	__IO uint32_t IC_TX_TL;			/*!< I2C transmit FIFO threshold register,              Address offset: 0x3C */

	__I uint32_t IC_CLR_INTR;			/*!< I2C clear combined and individual interrupts register,              Address offset: 0x40 */
	__I uint32_t IC_CLR_RX_UNDER;	/*!< I2C clear RX_UNDER interrupt register,              Address offset: 0x44 */
	__I uint32_t IC_CLR_RX_OVER;		/*!< I2C clear RX_OVER interrupt register,              Address offset: 0x48 */
	__I uint32_t IC_CLR_TX_OVER;		/*!< I2C clear TX_OVER interrupt register,              Address offset: 0x4C */

	__I uint32_t IC_CLR_RD_REQ;		/*!< I2C clear RD_REQ interrupt register,              Address offset: 0x50 */
	__I uint32_t IC_CLR_TX_ABRT;		/*!< I2C clear TX_ABRT interrupt register,              Address offset: 0x54 */
	__I uint32_t IC_CLR_RX_DONE;   	/*!< I2C clear RX_DONE interrupt register,              Address offset: 0x58 */
	__I uint32_t IC_CLR_ACTIVITY;	/*!< I2C clear ACTIVITY interrupt register,              Address offset: 0x5C */

	__I uint32_t IC_CLR_STOP_DET;	/*!< I2C clear STOP_DET interrupt register,              Address offset: 0x60 */
	__I uint32_t IC_CLR_START_DET;	/*!< I2C clear START_DET interrupt register,              Address offset: 0x64 */
	__I uint32_t IC_CLR_GEN_CALL;	/*!< I2C clear GEN_CALL interrupt register,              Address offset: 0x68 */
	__IO uint32_t IC_ENABLE;			/*!< I2C enable register,              Address offset: 0x6C */

	__I uint32_t IC_STATUS;			/*!< I2C status register,              Address offset: 0x70 */
	__I uint32_t IC_TXFLR;				/*!< I2C transmit FIFO level register,              Address offset: 0x74 */
	__I uint32_t IC_RXFLR;				/*!< I2C receive FIFO level register,              Address offset: 0x78 */
	__IO uint32_t IC_SDA_HOLD;		/*!< I2C SDA hold time length register,              Address offset: 0x7C */

	__I uint32_t IC_TX_ABRT_SOURCE;	/*!< I2C transmit abort status register,              Address offset: 0x80 */
	__IO uint32_t IC_SLV_DATA_NACK_ONLY;	/*!< I2C generate SLV_DATA_NACK register,              Address offset: 0x84 */
	__IO uint32_t IC_DMA_CR;			/*!< I2C DMA control register,              Address offset: 0x88 */
	__IO uint32_t IC_DMA_TDLR;		/*!< I2C DMA transmit data level register,              Address offset: 0x8C */

	__IO uint32_t IC_DMA_RDLR;		/*!< I2C DMA receive data level register,              Address offset: 0x90 */
	__IO uint32_t IC_SDA_SETUP;		/*!< I2C SDA setup register,              Address offset: 0x94 */
	__IO uint32_t IC_ACK_GENERAL_CALL;	/*!< I2C ACK general call register,              Address offset: 0x98 */
	__IO uint32_t IC_ENABLE_STATUS;	/*!< I2C enable status register,              Address offset: 0x9C */

	/* AmebaZ added New registers */
	__IO uint32_t IC_DMA_CMD;		/*!< I2C DMA command register,              Address offset: 0xA0 */
	__IO uint32_t IC_DMA_DAT_LEN;	/*!< I2C DMA transmit data length register,              Address offset: 0xA4 */
	__IO uint32_t IC_DMA_MOD;		/*!< I2C DMA mode register,              Address offset: 0xA8 */
	__IO uint32_t IC_SLEEP;			/*!< I2C sleep control register,              Address offset: 0xAC */

	__IO uint32_t IC_RSVD1[4];			/*!< I2C reserved field,              Address offset: 0xB0 */
	__I uint32_t IC_RSVD2[4];			/*!< I2C reserved field,              Address offset: 0xC0 */
	__I uint32_t IC_RSVD3[4];			/*!< I2C reserved field,              Address offset: 0xD0 */

	__I uint32_t IC_RSVD4;			/*!< I2C reserved field,              Address offset: 0xE0 */
	__I uint32_t IC_CLR_ADDR_MATCH;	/*!< I2C clear ADDR_MATCH interrupt register,              Address offset: 0xE4 */
	__I uint32_t IC_CLR_DMA_I2C_DONE;	/*!< I2C clear DMA_I2C_DONE interrupt register,              Address offset: 0xE8 */
	__IO uint32_t IC_FILTER;			/*!< I2C filter register,              Address offset: 0xEC */

	__I uint32_t IC_RSVD5;			/*!< I2C reserved field,              Address offset: 0xF0 */
	__IO uint32_t IC_SAR1;			/*!< I2C slave1 address register,              Address offset: 0xF4 */
	__IO uint32_t IC_DATA_S1;		/*!< I2C slave1 rx/tx data buffer register,              Address offset: 0xF8 */
	__I uint32_t IC_COMP_VERSION;	/*!< I2C component version ID register,              Address offset: 0xFC */
} I2C_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAZ_I2S
 * @{
 * @brief AMEBAZ_I2S Register Declaration
 *****************************************************************************/
typedef struct
{
	__IO uint32_t IS_CTL;				/*!< I2S control register,              Address offset: 0x00 */
	__IO uint32_t IS_TX_PAGE_PTR;		/*!< I2S TX page pointer register,      Address offset: 0x04 */
	__IO uint32_t IS_RX_PAGE_PTR; 		/*!< I2S RX page pointer register,      Address offset: 0x08 */
	__IO uint32_t IS_SETTING;			/*!< I2S page size and sample rate setting register,      Address offset: 0x0c */

	__IO uint32_t IS_TX_MASK_INT;		/*!< I2S TX interrupt enable register,	Address offset: 0x10 */
	__IO uint32_t IS_TX_STATUS_INT;		/*!< I2S TX interrupt status register,	Address offset: 0x14 */
	__IO uint32_t IS_RX_MASK_INT;		/*!< I2S RX interrupt enable register,	Address offset: 0x18 */
	__IO uint32_t IS_RX_STATUS_INT;		/*!< I2S RX interrupt status register,	Address offset: 0x1C */

	__IO uint32_t IS_TX_PAGE_OWN[4];	/*!< I2S TX page own bit register,	Address offset: 0x20-0x2c */
	__IO uint32_t IS_RX_PAGE_OWN[4];	/*!< I2S RX page own bit register,	Address offset: 0x30-0x3c */
} I2S_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAZ_TIMER
 * @{
 * @brief AMEBAZ_TIMER Register Declaration
 * @note TIM1 have 6 CCR registers: bit[15:0] is CCR, bit[31:24] is CCMR
 * @note TIM3 have 1 CCR registesr: bit[15:0] is CCR, bit[31:24] is CCMR
 * @note TIM5-8 dont have CCR register
 *****************************************************************************/
/**
  * @brief RTK TIM CCR
  */
typedef struct {
	__IO uint16_t CCRx;		/*!< TIM capture/compare register */
	__IO uint8_t RSVD;		/*!< TIM capture/compare rsvd register */
	__IO uint8_t CCMRx;		/*!< TIM capture/compare register */
} RTIM_CCR_TypeDef;

/**
  * @brief RTK TIM
  */
typedef struct {
	__IO uint32_t EN;			/*!< TIM Enable register,              Address offset: 0x00 */
	__IO uint32_t CR;			/*!< TIM control register,              Address offset: 0x04 */
	__IO uint32_t DIER;		/*!< TIM DMA/interrupt enable register,   Address offset: 0x08 */
	__IO uint32_t SR;			/*!< TIM status register,                 Address offset: 0x0C */
	__IO uint32_t EGR;		/*!< TIM event generation register,       Address offset: 0x10 */
	__IO uint32_t CNT;		/*!< TIM counter register,                Address offset: 0x14 */
	__IO uint32_t PSC;			/*!< TIM prescaler,                       Address offset: 0x18 */
	__IO uint32_t ARR;		/*!< TIM auto-reload register,            Address offset: 0x1C */

	__IO uint32_t CCMRx[6];	/*!< TIM capture/compare register */
} RTIM_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAZ_RTC
 * @{
 * @brief AMEBAZ_RTC Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t TR;			/*!< RTC time register,              Address offset: 0x00 */
	__IO uint32_t CR;			/*!< RTC control register,   Address offset: 0x04 */
	__IO uint32_t ISR;			/*!< RTC initialization and status register,                 Address offset: 0x08 */
	__IO uint32_t PRER;		/*!< RTC prescaler register,       Address offset: 0x0C*/
	__IO uint32_t CALIBR;		/*!< RTC calibration register,		Address offset: 0x10  */
	__IO uint32_t ALMR1;		/*!< RTC alarm register1,                Address offset: 0x14 */
	__IO uint32_t ALMR2;		/*!< RTC alarm register2,                       Address offset: 0x18 */
	__IO uint32_t WPR;		/*!< RTC write protection register,            Address offset: 0x1C */
} RTC_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAZ_PINMUX
 * @{
 * @brief AMEBAZ_PINMUX Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t PADCTR[21];			/*!< Pad control register */
} PINMUX_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAZ_GPIO
 * @{
 * @brief AMEBAZ_GPIO Register Declaration
 *****************************************************************************/
/**
  * @brief GPIO port
  */
typedef struct {
	__IO uint32_t DR;		/*!< data register */
	__IO uint32_t DDR;		/*!< data direction */
	__IO uint32_t CTRL;		/*!< data source control, we should keep it as default: data source from software */
} GPIO_Port_TypeDef;

/**
  * @brief General purpose input and output. (GPIO)
  */
typedef struct {
	GPIO_Port_TypeDef PORT[4];			/*!< GPIO IP have 4 ports, 						Address offset: 0x00-0x2C */

	__IO uint32_t INT_EN;				/*!< GPIO interrupt enable register, 			Address offset: 0x30 */
	__IO uint32_t INT_MASK;				/*!< GPIO interrupt mask register, 				Address offset: 0x34 */
	__IO uint32_t INT_TYPE;				/*!< GPIO interrupt type(level/edge) register, 	Address offset: 0x38 */
	__IO uint32_t INT_POLARITY;			/*!< GPIO interrupt polarity(Active low/high) register, 	Address offset: 0x3C */
	__IO uint32_t INT_STATUS;			/*!< GPIO interrupt status register, 			Address offset: 0x40 */
	__IO uint32_t INT_RAWSTATUS;		/*!< GPIO interrupt status without mask register, 	Address offset: 0x44 */
	__IO uint32_t DEBOUNCE;				/*!< GPIO interrupt signal debounce register, 	Address offset: 0x48 */
	__IO uint32_t PORTA_EOI;			/*!< GPIO clear interrupt register, 			Address offset: 0x4C */

	__IO uint32_t EXT_PORT[4];			/*!< GPIO IN read or OUT read back register, 	Address offset: 0x50-0x5C */

	__IO uint32_t LSSYNC;				/*!< GPIO level-sensitive synchronization enable register, 		Address offset: 0x60 */
	__IO uint32_t IDCODE;				/*!< GPIO ID code register, 					Address offset: 0x64 */
	__IO uint32_t RSVD2;				/*!< Reserved, 		Address offset: 0x68 */
	__IO uint32_t VERIDCODE;			/*!< GPIO component Version register, 			Address offset: 0x6C */
	__IO uint32_t CONFIG2;				/*!< GPIO configuration Register 2, 			Address offset: 0x70 */
	__IO uint32_t CONFIG1;				/*!< GPIO configuration Register 1, 			Address offset: 0x74 */
} GPIO_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAZ_IPSEC
 * @{
 * @brief AMEBAZ_IPSEC Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t IPSSDAR;		/*!<  (0x00)	IPSec Source Descriptor Starting Address Register */
	__IO uint32_t IPSDDAR;		/*!<  (0x04)	IPSec Destination Descriptor Starting Address Register */
	__IO uint32_t IPSCSR;			/*!<  (0x08)	IPSec Command/Status Register */
	__IO uint32_t IPSCTR;			/*!<  (0x0C)	IPSec Control Register */
} IPSEC_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAZ_USOC
 * @{
 * @brief AMEBAZ_USOC Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t SIE_CR;				/*!<  SIE control register,						Address offset: 0x00 */
	__IO uint32_t CLK_RST_CTRL;			/*!<  USOC clock and reset control register,	Address offset: 0x04 */
	__IO uint32_t CHANN_CTRL;			/*!<  USOC channel control register,			Address offset: 0x08 */
	__IO uint32_t BUFF_SIZE_CTRL;		/*!<  USOC tx/rx buffer size control register,	Address offset: 0x0C */
	__IO uint32_t TXBD_BAR;				/*!<  USOC TXBD base address register,			Address offset: 0x10 */
	__IO uint32_t RXBD_BAR;				/*!<  USOC RXBD base address register,			Address offset: 0x14 */
	__IO uint32_t RING_SIZE_CTRL;		/*!<  USOC ring size control register,			Address offset: 0x18 */
	__IO uint32_t RSVD1;				/*!<  reserved ,								Address offset: 0x1C */
	__I	 uint32_t TXBD_HW_IDX;			/*!<  USOC TXBD hardware index,					Address offset: 0x20 */
	__IO uint32_t TXBD_SW_IDX;			/*!<  USOC TXBD software index,					Address offset: 0x24 */
	__I   uint32_t RXBD_HW_IDX;			/*!<  USOC RXBD hardware index,					Address offset: 0x28 */
	__IO uint32_t RXBD_SW_IDX;			/*!<  USOC RXBD software index,					Address offset: 0x2C */
	__IO uint32_t INTR_MASK;			/*!<  USOC interrupt mask register,				Address offset: 0x30 */
	__IO uint32_t INTR_CLR;				/*!<  USOC interrupt clear register,			Address offset: 0x34 */
	__IO uint32_t INTR_STAT;			/*!<  USOC interrupt status register,			Address offset: 0x38 */
	__IO uint32_t RSVD2;				/*!<  reserved ,								Address offset: 0x3C */
	__IO uint32_t TX_MIT;				/*!<  USOC TX mitigation register,				Address offset: 0x40 */
	__IO uint32_t RX_MIT;				/*!<  USOC RX mitigation register,				Address offset: 0x44 */
	__IO uint32_t RSVD3[2];				/*!<  Reserved register,						Address offset: 0x48 */
	__IO uint32_t IOREG_MAR;			/*!<  USOC HOST access device register,			Address offset: 0x50 */
	__IO uint32_t RSVD4[3];				/*!<  Reserved ,								Address offset: 0x54 */
	__IO uint32_t TX_MAIN_BUF_CTRL;		/*!<  USOC tx main buffer control register,		Address offset: 0x60 */
	__IO uint32_t TX_DEST_BUF_CTRL;		/*!<  USOC tx destination buffer control register,	Address offset: 0x64 */
	__IO uint32_t RX_MAIN_BUF_CTRL;		/*!<  USOC rx main buffer control register,		Address offset: 0x68 */
	__IO uint32_t RX_SRC_BUF_CTRL;		/*!<  USOC rx source buffer control register,	Address offset: 0x6C */
	__IO uint32_t TX_STUCK_TIMER;		/*!<  USOC tx stuck timer register,				Address offset: 0x70 */
	__IO uint32_t RX_STUCK_TIMER;		/*!<  USOC rx stuck timer register,				Address offset: 0x74 */
	__IO uint32_t QOS_CTRL;				/*!<  USOC QoS control register,				Address offset: 0x78 */
} USOC_REG_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAZ_NCO32k
 * @{
 * @brief AMEBAZ_NCO32k Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t CLK_INFO;			/*!<  (0x00)	[23:0]: BIT_FREQVALUE_UNREGCLK, [24]: BIT_32K_CLK_OUT_RDY, [25]: 32K calibration ready flag */
	__IO uint32_t CLK_OUT;			/*!<  (0x04)	expected frequency of NCO calibration output clk */
	__IO uint32_t CLK_REF;			/*!<  (0x08)	low 32 bits of  frequency value of ref clk, this will be used to gen clk_out, and clk_ref is used to monitor frequency  of clk_in (ASIC: OSC8M, FPGA: 128K) */
	__IO uint32_t CTRL;				/*!<  (0x0c)	[9:0]high 10 bits of  frequency value of ref clk, [16] 32k enable, [17] ref clk enable, [23:20] 32k monitor, [30:24]32k threshold*/
} NCO32k_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAZ_NCO8M
 * @{
 * @brief AMEBAZ_NCO8M Register Declaration
 * @note [0]: function enable
 * @note [15:1]: expected frequency of nco output clk, unit is 1KHz
 * @note [31:16] frequency of nco input clk, unit is 1KHz
 *****************************************************************************/
typedef struct {
	__IO uint32_t NCOReg;
} NCO8M_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAZ_BACKUP_REG
 * @{
 * @brief AMEBAZ_BACKUP_REG Register Declaration
 * @note 16 Bytes total
 *****************************************************************************/
typedef struct {
	__IO uint32_t DWORD[4]; /* 0x138 */
} BACKUP_REG_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAZ_CACHE
 * @{
 * @brief AMEBAZ_CACHE Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t SPICC_EN;					/*!< SPICC Enable register,              Address offset: 0x00 */
	__IO uint32_t SPICC_FLUSH;				/*!< SPICC Flush register,              Address offset: 0x4 */
	__IO uint32_t SPICC_INTR;					/*!< SPICC Interrupt register,              Address offset: 0x8 */
	__IO uint32_t SPICC_RST_CUNT;			/*!< SPICC Reset counter register,              Address offset: 0xC */
	__IO uint32_t SPICC_RD_EVT_CUNT;		/*!< SPICC Read event counter register,              Address offset: 0x10 */
	__IO uint32_t SPICC_HIT_EVT_CUNT;		/*!< SPICC Hit event counter register,              Address offset: 0x14 */
	__IO uint32_t SPICC_HIT_LSTW_EVT_CUNT;	/*!< SPICC Hit lastway event counter register,              Address offset: 0x18 */
	__IO uint32_t SPICC_RD_PEND_CUNT;		/*!< SPICC Read pending counter register,              Address offset: 0x1C */
} SPIC_CACHE_TypeDef;
/** @} */

/** @} End of group AmebaZ_Peripheral_Registers_Structures */

/**************************************************************************//**
 * @defgroup AmebaZ_Peripheral_Declaration  AmebaZ Peripheral Declarations
 * @{
 *****************************************************************************/
#define UART0_DEV			((UART_TypeDef			*) UART0_REG_BASE)
#define UART1_DEV			((UART_TypeDef			*) UART1_REG_BASE)
#define UART2_DEV			((UART_TypeDef			*) LOG_UART_REG_BASE)

#define SPI0_DEV				((SPI_TypeDef			*) SPI0_REG_BASE)
#define SPI1_DEV				((SPI_TypeDef			*) SPI1_REG_BASE)

#define SPIC					((SPIC_TypeDef			*) SPI_FLASH_CTRL_BASE)

#define ADC					((ADC_TypeDef			*) ADC_REG_BASE)
#define I2C0_DEV				((I2C_TypeDef			*) I2C0_REG_BASE)
#define I2C1_DEV				((I2C_TypeDef			*) I2C1_REG_BASE)
#define I2S_DEV				((I2S_TypeDef			*) I2S0_REG_BASE)
#define TIM0					((RTIM_TypeDef			*) TIMER_REG_BASE)
#define TIM1					((RTIM_TypeDef			*) (TIMER_REG_BASE + 0x040))
#define TIM2					((RTIM_TypeDef			*) (TIMER_REG_BASE + 0x080))
#define TIM3					((RTIM_TypeDef			*) (TIMER_REG_BASE + 0x0C0))
#define TIM4					((RTIM_TypeDef			*) (TIMER_REG_BASE + 0x100))
#define TIM5					((RTIM_TypeDef			*) (TIMER_REG_BASE + 0x140))
#define RTC_DEV				((RTC_TypeDef			*) (RTC_BASE);
#define PINMUX				((PINMUX_TypeDef			*) PINMUX_REG_BASE)
#define GPIO					((GPIO_TypeDef			*) GPIO_REG_BASE)
#define IPSEC				((IPSEC_TypeDef			*) CRYPTO_REG_BASE)
#define USOC_REG			((USOC_REG_TypeDef		*) USOC_REG_BASE)
#define NCO32k				((NCO32k_TypeDef		*) NCO1_REG_BASE)
#define NCO8M				((NCO8M_TypeDef			*) NCO2_REG_BASE)
#define BACKUP_REG			((BACKUP_REG_TypeDef	*) BACKUP_REG_BASE)
#define SPIC_CACHE			((SPIC_CACHE_TypeDef	*) SPIC_CACHE_BASE)

/** @} End of group AmebaZ_Peripheral_Declaration */

/** @} End of group AmebaZ_Outline */
#endif //_HAL_PLATFORM_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
