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

#define ROMVERSION			0x01 /* ACUT is 0x01, BCUT is 0x02 ... */
#define ROMVERSION_SUB		2 /* ROM sub version */
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
 * @defgroup AmebaD_Outline
 * @{
 *****************************************************************************/

/** @addtogroup AmebaD_Outline
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
  *	-GPIOB:				0~31, IN/OUT/INT
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
  *	-Set count:			1
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
  *	-D:					0~511
  *	-Alarm:				support D/H/M/S
  *	-Daylight:			support
  *	-Calibration:		support
  *
  ***************************************************************************************** 
  * PWM
  ***************************************************************************************** 
  *	-Channel count:		6(KM0)/18(KM4)
  *	-Max. Resolution:	16bits (customizable)
  *	-Max. Frequency:	40MHz@1-bit mode, 40/(2^x))MHz@x-bit
  *
  ***************************************************************************************** 
  * Capture
  ***************************************************************************************** 
  *	-Channel count:		1
  *	-Max. Resolution:	16bits (customizable)
  *	-Max. Frequency:	40MHz@1-bit mode, 40/(2^x))MHz@x-bit
  *	-capture width:		supoort
  *	-capture number:	support
  *
  ***************************************************************************************** 
  * Timer
  ***************************************************************************************** 
  *	-Set count:			12
  *	-32KHz timer:		8
  *	-40MHz timer:		4
  *
  ***************************************************************************************** 
  * I2S
  ***************************************************************************************** 
  *	-Set count:			1
  *	-Source clk:		45.1584MHz or 98.304MHz(default)
  *	-Sample rate:		8K, 12K, 16K, 24K, 32K, 48K, 64K, 96K, 192K, 384K, 7.35K, 11.025K, 14.7K, 22.05K, 29.4K, 44.1K, 58.8K, 88.2K, 176.4K
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
  *	-AES-CFB
  *	-AES-OFB
  *	-AES-GCM
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
 * @defgroup AmebaD_Address_Map AmebaD HS/LP Address Map
 * @{
 *****************************************************************************/
#if defined (ARM_CORE_CM4)
#define SYSTEM_CTRL_BASE		0x40000000
#define PERI_ON_BASE			0x40000000
#define VENDOR_REG_BASE             0x40002800
#else
#define SYSTEM_CTRL_BASE		0x48000000
#define PERI_ON_BASE			0x48000000
#define VENDOR_REG_BASE			0x48002800
#endif
#define LP_RW_HSYSON_OFFSET	0x00000800 //use LP_RW_HSYSON_OFFSET can access HS SYSON directly when KM4 power gate
#define SYSTEM_CTRL_BASE_HP	0x40000000
#define SYSTEM_CTRL_BASE_HP_S	0x50000000
#define SYSTEM_CTRL_BASE_LP	0x48000000
#define PINMUX_REG_BASE		0x48000400	/* 0x0400~0x05FF */
#define RSIP_REG_BASE			0x48000600	/* 0x0600~0x07FF */

#define TIMERM4_REG_BASE		0x40002000
#define TIMERM0_REG_BASE		0x48002000		/*KM0 LTimer */

#define UART0_REG_BASE			0x40004000		/*KM4 uart0 */
#define UART1_REG_BASE			0x4000A000		/*KM4 uart1_bt */
#define UART2_REG_BASE			0x48012000		/*KM0 log uart */
#define UART3_REG_BASE			0x4800E000		/*KM0 luart */
#define UARTLOG_REG_BASE		UART2_REG_BASE		/*KM0 log uart */

#define IPCM4_REG_BASE			0x40006000
#define IPCM0_REG_BASE			0x48006000		/*KM0 IPC */

#define USI_REG_BASE			0x40008000
#define RXI300M4_REG_BASE		0x4000C000
#define RXI300M0_REG_BASE		0x48018000		/*KM0 RXI300 */
#define SPI1_REG_BASE			0x4000E000
#define SPI0_REG_BASE			0x40078000

#define AUDIO_SI_REG_BASE		0x40010000
#define AUDIO_SPORT_REG_BASE	0x40010800
#define IR_REG_BASE				0x40012000
#define PSRAM_REG_BASE			0x40014000
#define PSRAM_BASE				0x02000000

#define I2S0_REG_BASE			0x40020000
#define CRYPTO_REG_BASE		0x40022000
#define CRYPTOS_REG_BASE		0x50022000
#define SDIOH_REG_BASE			0x40026000
#if defined (ARM_CORE_CM4)
#define GDMA0_REG_BASE			0x4002A000		/*KM4 GDMA */
#else
#define GDMA0_REG_BASE			0x48010000		/*KM0 GDMA */
#endif
#define GDMA0S_REG_BASE		0x5002A000		/*KM4 GDMAS */

#define SDIO_DEVICE_REG_BASE	0x4002C000
#define USB_OTG_REG_BASE		0x40040000
#define LCDC_REG_BASE			0x40070000

#define WIFI_REG_BASE			0x40080000
#define WIFI_BB_REG_BASE		0x40082800
#define WIFI_RF_REG_BASE		0x40083800

#define SPI_FLASH_CTRL_BASE	0x48080000
#define SPI_FLASH_BASE			0x08000000

#define RTC_BASE				0x48004000		/*KM0 RTC */
#define CTC_REG_BASE			0x4801c000		/*KM0 Cap-Touch */
#define KEYSCAN_REG_BASE		0x4800A000		/*KM0 Key-Scan */
#define I2C0_REG_BASE			0x4800C000		/*KM0 I2C0 */

#define GPIO_REG_BASE			0x48014000		/*KM0 GPIO */
#define SGPIO_REG_BASE			0x4801A000		/*KM0 SGPIO */
#define ADC_REG_BASE			0x4801C800		/*KM0 ADC */
#define CMP_REG_BASE			0x4801CC00		/*KM0 COMPARE*/
#define QDEC_REG_BASE			0x4801E000		/*KM0 QDEC */

#define KM4_IDAU_BASE			0x5000C100

#define BACKUP_REG_BASE		0x480003C0		/*KM0 BACKUP REG */
#define WDG_REG_BASE			VENDOR_REG_BASE

/* 1024B Retention Ram base address */
#define RETENTION_RAM_BASE				0x000C0000
#define RETENTION_RAM_SYS_OFFSET		0x80
#define RETENTION_RAM_WIFI_FW_OFFSET	0x130 /* 0x000C_0130~0x000C_0400-1: 720B for wifi firmware */
#define RETENTION_RAM_WIFI_FW_SIZE		720

#ifdef DEBUG
#undef DEBUG
#define __todo__debug__
#endif

/** @} End of group AmebaD_Address_Map */

/**************************************************************************//**
 * @defgroup AmebaD_Peripheral_Registers_Structures  AmebaD Peripheral_Registers_Structures
 * @{
 *****************************************************************************/

/**************************************************************************//**
 * @defgroup AMEBAD_IDAU
 * @{
 * @brief AMEBAD_IDAU Register Declaration
 *****************************************************************************/
typedef struct {
	__IO u32 IDAU_BARx;		/*!< ,	Address offset: 0x00 */
	__IO u32 IDAU_LARx;		/*!< ,	Address offset: 0x04 */
} IDAU_EntryTypeDef;

typedef struct {
	IDAU_EntryTypeDef ENTRY[8]; /*!< ,	Address offset: 0x00 ~ 0x3C*/
	__IO u32 IDAU_CTRL;		/*!< ,	Address offset: 0x40 */
	__IO u32 IDAU_LOCK;		/*!< ,	Address offset: 0x44 */
} IDAU_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_WDG
 * @{
 * @brief AMEBAD_WDG Register Declaration
 *****************************************************************************/
typedef struct {
	__IO u32 VENDOR;		/*!< WDG VENDOR register,	Address offset: 0x00 */
} WDG_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_UART
 * @{
 * @brief AMEBAD_UART Register Declaration
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
 * @defgroup AMEBAD_SPI
 * @{
 * @brief AMEBAD_SPI Register Declaration
 *****************************************************************************/
typedef struct {
    __IO  uint32_t CTRLR0;				/*!< SSI control register 0,								Address offset: 0x00 */
    __IO  uint32_t CTRLR1;				/*!< SSI control register 1,								Address offset: 0x04 */
    __IO  uint32_t SSIENR;				/*!< SSI enable register,									Address offset: 0x08 */
    __IO  uint32_t MWCR;				/*!< SSI microwire control register,						Address offset: 0x0C */
    __IO  uint32_t SER;					/*!< SSI slave enable register,								Address offset: 0x10 */
    __IO  uint32_t BAUDR;				/*!< SSI baud rate select register,							Address offset: 0x14 */
    __IO  uint32_t TXFTLR;				/*!< SSI transmit FIFO threshold level register,			Address offset: 0x18 */
    __IO  uint32_t RXFTLR;				/*!< SSI receive FIFO Threshold Level register,				Address offset: 0x1C */
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
 * @defgroup AMEBAD_SPIC
 * @{
 * @brief AMEBAD_SPIC Register Declaration
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
 * @defgroup AMEBAD_USI
 * @{
 * @brief AMEBAD_USI Register Declaration
 *****************************************************************************/

typedef struct {
	/*GLOBAL*/
	__IO u32 USI_MODE_CTRL;			/*!< USI mode control register,		Address offset: 0x00*/
	__IO u32 SW_RESET;				/*!< USI software reset register,   	Address offset: 0x04*/
	
	__IO u32 RSVD0[2];				/*!< reserved space, 2 Dwords*/
	/*DMA*/
	__IO u32 DMA_ENABLE;				/*!< USI DMA enable register,         	Address offset: 0x10*/
	__IO u32 DMA_REQ_SIZE;			/*!< USI DMA request size register,	Address offset: 0x14*/
	
	__IO u32 RSVD1[2];				/*!< reserved space, 2 Dwords*/	

	/*TX FIFO*/
	__IO u32 TX_FIFO_WRITE;			/*!< USI write TX FIFO register,       	Address offset: 0X20*/
	__IO u32 TX_FIFO_CTRL;			/*!< USI TX FIFO control register,    	Address offset: 0X24*/	
	__I    u32 TX_FIFO_STATUS;			/*!< USI TX FIFO status register,     	Address offset: 0X28*/
	
	__IO u32 RSVD2[1];				/*!< reserved space, 1 Dwords*/
	
	/*RX FIFO*/
	__IO  u32 RX_FIFO_READ;			/*!< USI RX FIFO read register,         	Address offset: 0X30*/
	__IO  u32 RX_FIFO_CTRL;			/*!< USI RX FIFO control register,     	Address offset: 0X34*/
	__I    u32 RX_FIFO_STATUS;		/*!< USI RX FIFO status register,      	Address offset: 0X38*/
	__IO	 u32 RX_FIFO_RD_CNT;		/*!< USI RX FIFO read cnt register,  	 Address offset: 0X3C*/

	/*INTERRUPT*/
	__IO	 u32 INTERRUPT_ENABLE;		/*!< USI interrupt enable register,    	Address offset: 0X40*/
	__I	 u32 INTERRUPT_STATUS;		/*!< USI interrupt status register,     	Address offset: 0X44*/
	__I	 u32 RAW_INTERRUPT_STATUS;	/*!< USI interrupt raw status register, 	Address offset: 0X48*/
	__I	 u32 I2C_TX_ABRT_SOURCE;	/*!< USI I2C abort source register,       	Address offset: 0X4C*/
	__IO	 u32 INTERRUPT_ALL_CLR;		/*!< USI clear all interrupt register,      	Address offset: 0X50*/
	__IO	 u32 INTERRUPT_STATUS_CLR;	/*!< USI clear interrupt register,          	Address offset: 0X54*/

	__IO u32 RSVD3[2];				/*!< reserved space, 2 Dwords*/	

	/*DEBUG*/
	__IO	 u32 DEBUG_SEL;				/*!< USI debug select register,              Address offset: 0X60*/
	__I	 u32 DEBUG_PORT;			/*!< USI debug port register,              	Address offset: 0X64*/
	__IO	 u32 ECO_USE0;				/*!< USI ECO register,              		Address offset: 0X68*/
	__IO	 u32 ECO_USE1;				/*!< USI ECO register,              		Address offset: 0X6C*/
	__IO	 u32 USI_COMP_VERSION;		/*!< USI version number register,         	Address offset: 0X70*/
	
	__IO u32 RSVD4[35];				/*!< reserved space, 35 Dwords*/	

	/*UART*/
	__IO	 u32 UART_CTRL;					/*!< USI UART control register,              				Address offset: 0X100*/
	__IO	 u32 UART_IRDA_CTRL;				/*!< USI UART IRDA control register,      			Address offset: 0X104*/
	__IO	 u32 UART_IRDA_TX_PULSE_WD;	/*!< USI UART IRDA TX pulse configure register,  		Address offset: 0X108*/
	__IO	 u32 UART_IRDA_RX_PULSE_WD;	/*!< USI UART IRDA RX pulse configure register,  		Address offset: 0X10C*/
	__IO	 u32 UART_TX_FRACTION_BAUD;	/*!< USI UART TX path fraction baud rate configure register,  Address offset: 0X110*/
	__IO	 u32 UART_RX_BAUD_OSC;			/*!< USI UART RX baudrate configure for OSC clock, 	Address offset: 0X114*/	
	__IO	 u32 UART_RX_BAUD_XTAL;			/*!< USI UART RX baudrate configure for XTAL clock,     Address offset: 0X118*/	
	__IO	 u32 UART_RX_FRACTION_BAUD_CTRL;   /*!< USI UART RX fraction baud rate configure register,      	Address offset: 0X11C*/	
	__IO	 u32 UART_BAUD_MON_CTRL;		/*!< USI UART baud rate monitor control register,   	Address offset: 0X120*/
	__IO	 u32 UART_BAUD_MON_STATUS0;	/*!< USI UART baud rate monitor status register0,    	Address offset: 0X124*/
	__IO	 u32 UART_BAUD_MON_STATUS1;	/*!< USI UART baud rate monitor status register1,     	Address offset: 0X128*/
	__IO	 u32 UART_TOGGLE_MON;			/*!< USI UART toggle monitor register,      			Address offset: 0X12C*/
	__IO	 u32 UART_FLOW_CTRL;			/*!< USI UART flow control register,      				Address offset: 0X130*/
	__IO	 u32 UART_RXFIFO_TO_TH;			/*!< USI UART RX FIFO timeout threshold,			Address offset: 0X134*/	
	__IO	 u32 UART_RXDMA_FLOW_CTRL;		/*!< USI UART RX DMA flow control,				Address offset: 0X138*/	

	__IO u32 RSVD5[49];				/*!< reserved space, 49 Dwords*/

	/*SPI*/
	__IO	 u32 SPI_CTRL;				/*!< USI SPI control register,      			Address offset: 0X200*/
	__IO	 u32 SPI_TRANS_EN;			/*!< USI SPI transmission enable register,   	Address offset: 0X204*/
	__I	 u32 SPI_TRANS_STATUS;		/*!< USI SPI transmission status register,    	Address offset: 0X208*/
	
	__IO u32 RSVD6[61];				/*!< reserved space, 61 Dwords*/

	/*I2C*/
	__IO	 u32 I2C_CTRL;				/*!< USI I2C control register,      Address offset: 0X300*/	
	__IO	 u32 I2C_SS_SCL_CNT;			/*!< USI I2C standard speed clock SCL low/high count register,    	Address offset: 0x304 */
	__IO	 u32 I2C_FS_SCL_CNT;			/*!< USI I2C fast speed clock SCL low/high count register,              	Address offset: 0x308 */
	__IO	 u32 I2C_HS_SCL_CNT;		/*!< USI I2C high speed clock SCL low/high count register,              	Address offset: 0x30C */
	__IO	 u32 I2C_SDA_TIMING;			/*!< USI I2C SDA timing configure register,        	Address offset: 0x310*/
	__IO	 u32 I2C_DIG_FILTER;			/*!< USI I2C filter register,              			Address offset: 0x314*/
	__IO	 u32 I2C_SLV_ACK_CTRL;		/*!< USI I2C slave ACK control register,              	Address offset: 0x318*/
	__IO	 u32 I2C_ENABLE;				/*!< USI I2C enable register,              			Address offset: 0x31C*/
	__IO	 u32 I2C_TAR;				/*!< USI I2C target address register,              	Address offset: 0x320*/
	__IO	 u32 I2C_DMA_CMD;			/*!< USI I2C DMA command register,              	Address offset: 0x324*/
	__I	 u32 I2C_STATUS;				/*!< USI I2C status register,              			Address offset: 0x328*/
	__IO u32 RSVD7[1];				/*!< reserved space, 1 Dwords*/
	__IO	 u32 I2C_ANA_FILTER1;		/*!< USI I2C filter register 1,              	Address offset: 0x330*/
	__IO	 u32 I2C_ANA_FILTER2;		/*!< USI I2C filter register 2,              	Address offset: 0x334*/
	__IO	 u32 I2C_ANA_FILTER3;		/*!< USI I2C filter register 3,              	Address offset: 0x338*/
	__IO	 u32 I2C_ANA_FILTER4;		/*!< USI I2C filter register 4,              	Address offset: 0x33C*/
	__IO	 u32 I2C_ANA_FILTER5;		/*!< USI I2C filter register 5,              	Address offset: 0x340*/
	__IO	 u32 I2C_ANA_FILTER6;		/*!< USI I2C filter register 6,              	Address offset: 0x344*/
	__IO	 u32 I2C_ANA_FILTER7;		/*!< USI I2C filter register 7,              	Address offset: 0x348*/
	__IO	 u32 I2C_ANA_FILTER8;		/*!< USI I2C filter register 8,      		Address offset: 0x34C*/
} USI_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_LCDC
 * @{
 * @brief AMEBAD_LCDC Register Declaration
 *****************************************************************************/
typedef struct {
	/*global control*/
	__IO u32 LCDC_CTRL;				/*!< LCDC control register,	Address offset: 0x00*/
	__IO u32 LCDC_PLANE_SIZE;		/*!< LCDC plane size configure register,              	Address offset: 0x04*/
	__IO u32 LCDC_UNDFLW_CFG;		/*!< LCDC DMA FIFO underflow configure register  	Address offset: 0x08*/
	__IO u32 LCDC_DMA_MODE_CFG;	/*!< LCDC DMA mode configure register        		Address offset: 0x0C*/
	__IO u32 LCDC_CLK_DIV;			/*!< LCDC DCLK clock divider		        		Address offset: 0x10*/

	__IO u32 RSVD0[3];				/*!< reserved space, 2 Dwords*/

	/*Interrupt and status*/
	__IO u32 LCDC_IRQ_EN;			/*!< LCDC interrupt enable register       		Address offset: 0x20*/
	__IO u32 LCDC_IRQ_STATUS;		/*!< LCDC interrupt status after mask and interrupt clear register  Address offset: 0x24*/
	__I u32 LCDC_IRQ_RAW;			/*!< LCDC RAW interrupt status  Address offset: 0x28*/
	__IO u32 LCDC_LINE_INT_POS;		/*!< LCDC line interrupt position	Address offset: 0x2C*/
	__I u32 LCDC_CUR_POS_STATUS;	/*!< LCDC current position status register	Address offset: 0x30*/
	__I u32 LCDC_STATUS;			/*!< LCDC DMA FIFO underflow interrupt count register	Address offset: 0x34*/

	__IO u32 RSVD1[2];				/*!< reserved space, 2 Dwords*/
	
	/*RGB control*/
	__IO u32 LCDC_RGB_CFG;			/*!< LCDC RGB configuration register       		Address offset: 0x40*/	
	__IO u32 LCDC_RGB_VSYNC_CFG;	/*!< LCDC RGB vertical synchronization configuration  register       		Address offset: 0x44*/
	__IO u32 LCDC_RGB_HSYNC_CFG;	/*!< LCDC RGB horizontal synchronization configuration  register       		Address offset: 0x48*/
	__IO u32 LCDC_RGB_SYNC_STATUS;	/*!< LCDC RGB synchronization status  register       		Address offset: 0x4C*/

	__IO u32 RSVD2[4];				/*!< reserved space, 4 Dwords*/

	/*MCU control*/
	__IO u32 LCDC_MCU_CFG;			/*!< LCDC MCU I/F configuration register       		Address offset: 0x60*/
	__IO u32 LCDC_MCU_VSYNC_CFG;	/*!< LCDC MCU I/F vertical synchronization configuration  register       		Address offset: 0x64*/
	__IO u32 LCDC_MCU_TIMING_CFG;	/*!< LCDC  MCU I/F timing configuration  register       		Address offset: 0x68*/
	__IO u32 LCDC_MCU_IO_DATA;		/*!< LCDC  MCU I/F write/read data register       		Address offset: 0x6C*/
	__IO u32 LCDC_MCU_IO_TO_CFG;	/*!< LCDC  MCU I/F IO write/read timeout configuration register       		Address offset: 0x70*/

	__IO u32 RSVD3[3];				/*!< reserved space, 3 Dwords*/

	/*LED control*/
	__IO u32 LCDC_LED_CFG;			/*!< LCDC LED I/F configuration register       		Address offset: 0x80*/
	__IO u32 LCDC_LED_TIMING;		/*!< LCDC LED I/F timing configuration register    	Address offset: 0x84*/

	__IO u32 LCDC_LED_IDLE;			/*!< LCDC LED I/F timing idle configuration register    	Address offset: 0x88*/
	__IO u32 RSVD4[1];				/*!< reserved space, 1 Dwords*/

	/*Image control*/
	__IO u32 LCDC_IMG_BASE_ADDR;	/*!< LCDC Image base address register    			Address offset: 0x90*/
} LCDC_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_CAPTOUCH
 * @{
 * @brief AMEBAD_CAPTOUCH Register Declaration
 *****************************************************************************/

/**
  * @brief CapTouch channel info
  */
typedef struct {
	__IO uint32_t CTRL;		/*!< ctrol register */
	__IO uint32_t ATHR;		/*!< Absolute threshold */
	__IO uint32_t MBIAS;		/*!< MBIAS current setting */
	__I uint32_t DATA;		/*!< RAW Data */
} CT_CH_TypeDef;

/**
  * @brief CapTouch register info
  */

typedef struct
{
	__IO uint32_t CT_CTRL;				/*!< CAPTOUCH ctrl register,				Address offset: 0x00 */
	__IO uint32_t CT_SP_CTRL;				/*!< CAPTOUCH scan period ctrl register 0,	Address offset: 0x04 */
	__IO uint32_t CT_ETC_CTRL;			/*!< CAPTOUCH timer configure register 1,	Address offset: 0x08 */
	
	__I   uint32_t CT_SNR;					/*!< CAPTOUCH SNR info register,			Address offset: 0x0c */
	__IO uint32_t CT_MODE_CTRL;			/*!< CAPTOUCH mode ctrl register,			Address offset: 0x10 */
	__IO uint32_t CT_FIFO_STATUS;		/*!< CAPTOUCH raw data fifo status register,	Address offset: 0x14 */
	__IO uint32_t CT_FIFO;				/*!< CAPTOUCH raw data fifo register,		Address offset: 0x18 */
	
	__I  uint32_t RSVD0;					/*!< CAPTOUCH reserved register,			Address offset: 0x1C */
	
	__IO uint32_t CT_IER;					/*!< CAPTOUCH interrupt enable register,		Address offset: 0x20 */
	__I   uint32_t CT_ISR;					/*!< CAPTOUCH interrupt status register,		Address offset: 0x24*/
	__I   uint32_t CT_ISR_RAW;			/*!< CAPTOUCH 							Address offset: 0x28*/
	
	__I  uint32_t RSVD1;					/*!< CAPTOUCH reserved register,			Address offset: 0x2C */
	
	__IO uint32_t CT_ICR_ALL;				/*!< CAPTOUCH interrupt all clear register,	Address offset: 0x30 */
	__IO uint32_t CT_ICR;					/*!< CAPTOUCH interrupt clear register,		Address offset: 0x34*/

	__I  uint32_t RSVD2[50];				/*!< CAPTOUCH reserved register,			Address offset: 0x38 */
	
	CT_CH_TypeDef CT_CH[5];				/*!< CAPTOUCH chanel register,				Address offset: 0x100*/
	
	__I  uint32_t RSVD3[172];				/*!< CAPTOUCH reserved register,			Address offset: 0x150 */

	__IO uint32_t CT_ADC_REG0X_LPAD;	/*!< CAPTOUCH ADC register,				Address offset: 0x400 */
	__IO uint32_t CT_ADC_REG1X_LPAD;	/*!< CAPTOUCH ADC register,				Address offset: 0x404 */
	__IO uint32_t CT_ADC_REG0X_LPSD;	/*!< CAPTOUCH ADC register,				Address offset: 0x408 */
	__IO uint32_t CT_ADC_TIME;			/*!< CAPTOUCH ADC register,				Address offset: 0x40C */
} CAPTOUCH_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_IR
 * @{
 * @brief AMEBAD_IR Register Declaration
 *****************************************************************************/
typedef struct /*!< IR Structure */
{
	__IO u32 IR_CLK_DIV; 			/*!< IR clock DIV register,              	Address offset: 0x00 */
	__IO u32 IR_TX_CONFIG; 			/*!< IR TX config register,              	Address offset: 0x04 */
	__I   u32 IR_TX_SR; 				/*!< IR TX status register,              	Address offset: 0x08 */
	__IO u32 IR_TX_COMPE_DIV;		/*!< IR TX compensation register,	Address offset: 0x0c */
	__IO u32 IR_TX_INT_CLR;			/*!< IR TX interrupt clear register,	Address offset: 0x10 */
	__O  u32 IR_TX_FIFO;				/*!< IR TX FIFO register,              	Address offset: 0x14 */
	__IO u32 IR_RX_CONFIG;			/*!< IR RX config register,              	Address offset: 0x18 */
	__I   u32 IR_RX_SR;				/*!< IR RX status register,              	Address offset: 0x1c */
	__IO u32 IR_RX_INT_CLR;			/*!< IR RX interrupt clear register,	Address offset: 0x20 */
	__IO u32 IR_RX_CNT_INT_SEL;		/*!< IR RX counter threshold register,	Address offset: 0x24 */
	__I   u32 IR_RX_FIFO;				/*!< IR RX FIFO register			Address offset: 0x28 */
	__I   u32 IR_VERSION;				/*!< IR HW version register,              Address offset: 0x2c */
} IR_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_QDEC
 * @{
 * @brief AMEBAD_QDEC Register Declaration
 *****************************************************************************/
typedef struct
{
	__IO uint32_t QDEC_CLK_CFG;				/*!< QDEC Clock Configuration Register,		Address offset: 0x00 */
	__IO uint32_t QDEC_CTRL;					/*!< QDEC Control Register,				Address offset: 0x04 */
	
	__IO uint32_t QDEC_MPC;					/*!< QDEC Max Position Counter Register,		Address offset: 0x08 */
	__IO uint32_t QDEC_RC;					/*!< QDEC Rotation Compare Register,		Address offset: 0x0c */
	__I   uint32_t QDEC_PC;					/*!< QDEC Position Counter Register,			Address offset: 0x10 */
	__IO uint32_t QDEC_ISC;					/*!< QDEC Index Signal Configuration Register,Address offset: 0x14 */
	
	__IO uint32_t QDEC_VCTRL;				/*!< QDEC Velocity Control Register,			Address offset: 0x18 */
	__I   uint32_t QDEC_VC;					/*!< QDEC Velocity Counter Register,			Address offset: 0x1C */
	__I   uint32_t QDEC_VCCAP;				/*!< QDEC Velocity Counter Capture Register,	Address offset: 0x20*/
	__I   uint32_t QDEC_PCCAP;				/*!< QDEC Position Counter Capture Register,	Address offset: 0x24 */
	__IO uint32_t QDEC_VTRLD;				/*!< QDEC Velocity Time Reload Register,		Address offset: 0x28 */
	__IO uint32_t QDEC_VT;					/*!< QDEC Velocity Timer Register,			Address offset: 0x2C */
	__IO uint32_t QDEC_VCOMP;				/*!< QDEC Velocity Compare Register,		Address offset: 0x30 */
	
	__IO uint32_t DUMMY[2];					/*!< QDEC Velocity Rsvd,					Address offset: 0x34 */
	
	__IO uint32_t QDEC_IMR;					/*!< QDEC Interrupt Mask Register ,			Address offset: 0x3C */
	__IO uint32_t QDEC_ISR;					/*!< QDEC raw Interrupt Status Register,		Address offset: 0x40 */
} QDEC_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_ADC
 * @{
 * @brief AMEBAD_ADC Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t ADC_CONF;				/*!< ADC configure register,				Address offset: 0x00 */
	__IO uint32_t ADC_IN_TYPE;			/*!< ADC channel input type register,   	Address offset: 0x04 */
	__IO uint32_t ADC_COMP_TH_CH[11];	/*!< ADC CH0~CH10 compare threshold register,   Address offset: 0x08~0x30 */
	__IO uint32_t ADC_COMP_CTRL;			/*!< ADC compare control register,   		Address offset: 0x34 */
	__I   uint32_t ADC_COMP_STS;			/*!< ADC compare status register,   		Address offset: 0x38 */
	__IO uint32_t ADC_CHSW_LIST[2];		/*!< ADC channel switch list register,   	Address offset: 0x3C~0x40 */
	__IO uint32_t ADC_RST_LIST;			/*!< ADC reset channel list register,   		Address offset: 0x44 */
	__IO uint32_t ADC_AUTO_CSW_CTRL;	/*!< ADC auto channel switch control register,Address offset: 0x48 */
	__IO uint32_t ADC_SW_TRIG;			/*!< ADC software trigger register,   		Address offset: 0x4C */
	__I   uint32_t ADC_LAST_CH;			/*!< ADC last channel register,   			Address offset: 0x50 */
	__I   uint32_t ADC_BUSY_STS;			/*!< ADC busy status register,   			Address offset: 0x54 */
	__IO uint32_t ADC_INTR_CTRL;			/*!< ADC interrupt control register,   		Address offset: 0x58 */
	__I   uint32_t ADC_INTR_RAW_STS;		/*!< ADC interrupt raw status register,   	Address offset: 0x5C */
	__IO uint32_t ADC_INTR_STS;			/*!< ADC interrupt status register,   		Address offset: 0x60 */
	__IO uint32_t ADC_IT_CHNO_CON;		/*!< ADC specified channel number register, Address offset: 0x64 */
	__IO uint32_t ADC_FULL_LVL;			/*!< ADC FIFO full level register,   			Address offset: 0x68 */
	__IO uint32_t ADC_EXT_TRIG_TIMER_SEL;/*!< ADC external trigger timer selection register,Address offset: 0x6C */
	__IO uint32_t ADC_DATA_CH[11];		/*!< ADC CH0~CH10 data register,   		Address offset: 0x70~0x98 */
	__IO uint32_t ADC_DATA_GLOBAL;		/*!< ADC global data register,   		Address offset: 0x9C */
	__IO uint32_t ADC_DMA_CON;			/*!< ADC DMA control register,   		Address offset: 0xA0 */
	__IO uint32_t ADC_FLR;				/*!< ADC FIFO entry number register,   		Address offset: 0xA4 */
	__IO uint32_t ADC_CLR_FIFO;			/*!< ADC clear FIFO register,   		Address offset: 0xA8 */
	__IO uint32_t ADC_CLK_DIV;			/*!< ADC clock divider register,   		Address offset: 0xAC */
	__IO uint32_t ADC_DELAY_CNT;			/*!< ADC channel switch delay register,   		Address offset: 0xB0 */
	__IO uint32_t ADC_PWR_CTRL;			/*!< ADC power control register,   		Address offset: 0xB4 */
} ADC_TypeDef;

/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_COMPARE
 * @{
 * @brief AMEBAD_COMPARE Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t COMP_REF_CH[4];				/*!< COMPARE CH0~3 reference voltage register ,	Address offset: 0x00~0x0C */
	__IO uint32_t COMP_INTR_CTRL;			/*!< COMPARE interrupt control register,   	Address offset: 0x10 */
	__IO uint32_t COMP_WK_STS;				/*!< COMPARE wakeup ADC/SYS status register,   	Address offset: 0x14 */
	__I   uint32_t COMP_WK_RAW_STS;			/*!< COMPARE wakeup ADC/SYS status raw register,   	Address offset: 0x18 */
	__IO uint32_t COMP_CHSW_LIST;			/*!< COMPARE channel switch list register,	Address offset: 0x1C */
	__IO uint32_t COMP_LAST_CH;				/*!< COMPARE last channel register,   		Address offset: 0x20 */
	__I   uint32_t COMP_BUSY_STS;				/*!< COMPARE busy status register,   		Address offset: 0x24 */
	__I   uint32_t COMP_CH_STS;				/*!< COMPARE channel status register,   	Address offset: 0x28 */
	__IO uint32_t COMP_AUTO_SHUT;			/*!< COMPARE automatic shut register,  		Address offset: 0x2C */
	__IO uint32_t COMP_EXT_TRIG_CTRL;		/*!< COMPARE ext trigger control register,   	Address offset: 0x30 */
	__IO uint32_t COMP_EXT_TRIG_TIMER_SEL;	/*!< COMPARE ext trigger timer select register,   		Address offset: 0x34 */
	__IO uint32_t COMP_RST_LIST;				/*!< COMPARE reset channel list register, Address offset: 0x38 */
	__IO uint32_t COMP_AUTOSW_EN;			/*!< COMPARE automatic channel switch enable register,   	Address offset: 0x3C */
	__IO uint32_t COMP_EN_TRIG;				/*!< COMPARE enable trigger register,  		Address offset: 0x40 */
	__IO uint32_t COMP_EXT_WK_SHUT_CTRL;	/*!< COMPARE ext wake shut count register,   Address offset: 0x44 */
	__IO uint32_t COMP_ANALOG;				/*!< COMPARE analog register,   Address offset: 0x48 */
} CMP_TypeDef;

/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_I2C
 * @{
 * @brief AMEBAD_I2C Register Declaration
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
	__IO uint32_t IC_DMA_CR;			/*!< I2C DMA Control Register for transmit and receive handshaking interface,              Address offset: 0x88 */
	__IO uint32_t IC_DMA_TDLR;		/*!< I2C DMA transmit data level register,              Address offset: 0x8C */

	__IO uint32_t IC_DMA_RDLR;		/*!< I2C DMA receive data level register,              Address offset: 0x90 */
	__IO uint32_t IC_SDA_SETUP;		/*!< I2C SDA setup register,              Address offset: 0x94 */
	__IO uint32_t IC_ACK_GENERAL_CALL;	/*!< I2C ACK general call register,              Address offset: 0x98 */
	__IO uint32_t IC_ENABLE_STATUS;	/*!< I2C enable status register,              Address offset: 0x9C */

	__IO uint32_t IC_DMA_CMD;		/*!< I2C DMA command register,              Address offset: 0xA0 */
	__IO uint32_t IC_DMA_DAT_LEN;	/*!< I2C DMA Mode Transfer Data Length Register,              Address offset: 0xA4 */
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
	__IO uint32_t IC_SAR1;			/*!< I2C slave1 address register,  not support in amebaD 	Address offset: 0xF4 */
	__IO uint32_t IC_DATA_S1;		/*!< I2C slave1 rx/tx data buffer register,  not support in amebaD	Address offset: 0xF8 */
	__I uint32_t IC_COMP_VERSION;	/*!< I2C component version ID register,              Address offset: 0xFC */
} I2C_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_I2S
 * @{
 * @brief AMEBAD_I2S Register Declaration
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
	__I uint32_t IS_VERSION_ID;		/*!< I2S RX page own bit register,	Address offset: 0x40 */
} I2S_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_AUDIO_SI
 * @{
 * @brief AMEBAD_AUDIO_SI Register Declaration
 *****************************************************************************/
typedef struct
{
	__IO uint32_t SI_CTRLR;				/*!< SI control register,				Address offset: 0x00 */
	__IO uint32_t SI_CLK_EN;			/*!< SI TX page pointer register,		Address offset: 0x04 */
} AUDIO_SI_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_AUDIO_SPORT
 * @{
 * @brief AMEBAD_AUDIO_SPORT Register Declaration
 *****************************************************************************/
typedef struct
{
	__O  uint32_t SP_TX_DR;				/*!< SPORT TX data register,			Address offset: 0x00 */
	__IO uint32_t SP_CTRLR0;			/*!< SPORT control 0 register,			Address offset: 0x04 */
	__IO uint32_t SP_CTRLR1;			/*!< SPORT control 1 register,			Address offset: 0x08 */
	__IO uint32_t SP_DSP_INT_CR;		/*!< SPORT DSP interrupt clear register,	Address offset: 0x0C */
	__I uint32_t SP_RX_DR;				/*!< SPORT RX data register,			Address offset: 0x10 */
	__I uint32_t  SP_FIFO_SR;			/*!< SPORT FIFO status register,		Address offset: 0x14 */
	__IO uint32_t SP_ERROR_CNT_SR;		/*!< SPORT error counter register,		Address offset: 0x18 */
	__IO uint32_t SP_CLK_DIV;			/*!< SPORT clock divide register,		Address offset: 0x1C */
} AUDIO_SPORT_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_TIMER
 * @{
 * @brief AMEBAD_TIMER Register Declaration
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
	__IO uint32_t DIER;		/*!< TIM interrupt enable register,   Address offset: 0x08 */
	__IO uint32_t SR;			/*!< TIM status register,                 Address offset: 0x0C */
	__O uint32_t EGR;		/*!< TIM event generation register,       Address offset: 0x10 */
	__IO uint32_t CNT;		/*!< TIM counter register,                Address offset: 0x14 */
	__IO uint32_t PSC;			/*!< TIM prescaler,                       Address offset: 0x18 */
	__IO uint32_t ARR;		/*!< TIM auto-reload register,            Address offset: 0x1C */

	__IO uint32_t CCMRx[18];	/*!< TIM capture/compare registers 		Address offset: 0x20~0x64*/
} RTIM_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_RTC
 * @{
 * @brief AMEBAD_RTC Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t TR;			/*!< RTC time register,              Address offset: 0x00 */
	__IO uint32_t CR;			/*!< RTC control register,   Address offset: 0x04 */
	__IO uint32_t ISR;			/*!< RTC initialization and status register,                 Address offset: 0x08 */
	__IO uint32_t PRER;		/*!< RTC prescaler register,       Address offset: 0x0C*/
	__IO uint32_t CALIBR;		/*!< RTC calibration register,		Address offset: 0x10  */
	__IO uint32_t ALMR1L;		/*!< RTC alarm register1,                Address offset: 0x14 */
	__IO uint32_t ALMR1H;		/*!< RTC alarm register2,                       Address offset: 0x18 */
	__IO uint32_t WPR;		/*!< RTC write protection register,            Address offset: 0x1C */
	__IO uint32_t CLKACALR;	/*!< RTC 32k auto calibration register,            Address offset: 0x20 */
} RTC_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_PINMUX
 * @{
 * @brief AMEBAD_PINMUX Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t PADCTR[64];			/*!< Pad control register */
} PINMUX_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_GPIO
 * @{
 * @brief AMEBAD_GPIO Register Declaration
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
	__IO uint32_t INT_BOTHEDGE;		/*!< GPIO Both Edge Interrupt,					Address offset: 0x68 */
	__IO uint32_t VERIDCODE;			/*!< GPIO component Version register, 			Address offset: 0x6C */
	__IO uint32_t CONFIG2;				/*!< GPIO configuration Register 2, 			Address offset: 0x70 */
	__IO uint32_t CONFIG1;				/*!< GPIO configuration Register 1, 			Address offset: 0x74 */
} GPIO_TypeDef;

/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_IPSEC
 * @{
 * @brief AMEBAD_IPSEC Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t SDSR;					/*!< IPsec source descriptor fifo status register,			Address offset: 0x0000 */
	__IO uint32_t SDFW;					/*!< IPsec source descriptor first word,					Address offset: 0x0004 */
	__IO uint32_t SDSW;					/*!< IPsec source descriptor second word,				Address offset: 0x0008 */
	uint32_t RSVD0;						/*!< Reserved, 									0x000C*/
	__IO uint32_t RST_ISR_CON;					/*!< IPsec reset and interrupt register,					Address offset: 0x0010 */
	__IO uint32_t IMR;						/*!< IPsec interrupt mask register,						Address offset: 0x0014 */
	__IO uint32_t DEBUG;					/*!< IPsec debug register,							Address offset: 0x0018 */
	__IO uint32_t ERR_STAT;				/*!< IPsec interrupt error status register,					Address offset: 0x001C */
	__I uint32_t A2EO_SUM;					/*!< IPsec a2eo sum register,							Address offset: 0x0020*/
	__I uint32_t ENL_SUM;					/*!< IPsec encryption length sum register,				Address offset: 0x0024 */
	__I uint32_t APL_SUM;					/*!< IPsec authentication padding length sum register,		Address offset: 0x0028*/
	__I uint32_t EPL_SUM;					/*!< IPsec encryption padding length sum register,			Address offset: 0x002C */
	__IO uint32_t SWAP_BURST;				/*!< IPsec swap/burst register,						Address offset: 0x0030 */
	uint32_t RSVD1[3];						/*!< Reserved, 									0x0034 -0x003C*/
	__I uint32_t ENG_DBG;					/*!< IPsec engine debug register,						Address offset: 0x0040*/
	__I uint32_t DMA_LX_DBG;				/*!< IPsec lexra bus debug register,						Address offset: 0x0044 */
	__I uint32_t DMA_RX_DBG;				/*!< IPsec dma rx debug register,						Address offset: 0x0048*/
	__I uint32_t DMA_TX_DBG;				/*!< IPsec dma tx debug register,						Address offset: 0x004C */
	__I uint32_t STA_SDES_CFG;				/*!<IPsec source descriptor configure debug register,		Address offset: 0x0050*/
	__I uint32_t STA_SDES_PTR;				/*!< IPsec source descriptor pointer debug register,			Address offset: 0x0054 */
	__I uint32_t STA_SDES_CMD1;			/*!< IPsec source descriptor command#1 debug register,		Address offset: 0x0058*/
	__I uint32_t STA_SDES_CMD2;			/*!< IPsec source descriptor command#2 debug register,		Address offset: 0x005C */
	__I uint32_t STA_SDES_CMD3;			/*!< IPsec source descriptor command#3 debug register,		Address offset: 0x0060*/
	__I uint32_t STA_DDES_CFG;				/*!< IPsec destination descriptor configure debug register,	Address offset: 0x0064 */
	__I uint32_t STA_DDES_PTR;				/*!< IPsec destination descriptor pointer debug register,		Address offset: 0x0068*/
	uint32_t RSVD2[3];						/*!< Reserved, 									0x006C -0x0074*/
	__IO uint32_t CFG_SSET_ADDR_L_LIMIT;	/*!< Source command  setting low boundary address register,	Address offset: 0x0078 */
	__IO uint32_t CFG_SSET_ADDR_H_LIMIT;	/*!< Source command  setting high boundary address register,	Address offset: 0x007C */
	__IO uint32_t CFG_SDATA_ADDR_L_LIMIT;	/*!< IPsec source data low boundary address register,		Address offset: 0x0080 */
	__IO uint32_t CFG_SDATA_ADDR_H_LIMIT;/*!< IPsec source data high boundary address register,		Address offset: 0x0084 */
	uint32_t RSVD3[2];						/*!< Reserved, 									0x0088 -0x008C*/
	__IO uint32_t CFG_DDATA_ADDR_L_LIMIT;/*!< IPsec destination data low boundary address register,	Address offset: 0x0090 */
	__IO uint32_t CFG_DDATA_ADDR_H_LIMIT;/*!< IPsec destination data high boundary address register,	Address offset: 0x0094 */
	uint32_t RSVD4[2];						/*!< Reserved, 									0x0098 -0x009C*/
	__O uint32_t SK0_R0;					/*!< IPsec secure key 0 (bit 224-255) register,			Address offset: 0x00A0 */
	__O uint32_t SK0_R1;					/*!< IPsec secure key 0 (bit 192-223) register,			Address offset: 0x00A4 */
	__O uint32_t SK0_R2;					/*!< IPsec secure key 0 (bit 160-191) register,			Address offset: 0x00A8 */
	__O uint32_t SK0_R3;					/*!< IPsec secure key 0 (bit 128-159) register,			Address offset: 0x00AC */
	__O uint32_t SK0_R4;					/*!< IPsec secure key 0 (bit 96-127) register,				Address offset: 0x00B0 */
	__O uint32_t SK0_R5;					/*!< IPsec secure key 0 (bit 64-95) register,				Address offset: 0x00B4 */
	__O uint32_t SK0_R6;					/*!< IPsec secure key 0 (bit 32-63) register,				Address offset: 0x00B8 */
	__O uint32_t SK0_R7;					/*!< IPsec secure key 0 (bit 0-31) register,				Address offset: 0x00BC */
	__O uint32_t SK1_R0;					/*!< IPsec secure key 1 (bit 224-255) register,			Address offset: 0x00C0 */
	__O uint32_t SK1_R1;					/*!< IPsec secure key 1 (bit 192-223) register,			Address offset: 0x00C4 */
	__O uint32_t SK1_R2;					/*!< IPsec secure key 1 (bit 160-191) register,			Address offset: 0x00C8 */
	__O uint32_t SK1_R3;					/*!< IPsec secure key 1 (bit 128-159) register,			Address offset: 0x00CC */
	__O uint32_t SK1_R4;					/*!< IPsec secure key 1 (bit 96-127) register,				Address offset: 0x00D0 */
	__O uint32_t SK1_R5;					/*!< IPsec secure key 1 (bit 64-95) register,				Address offset: 0x00D4 */
	__O uint32_t SK1_R6;					/*!< IPsec secure key 1 (bit 32-63) register,				Address offset: 0x00D8 */
	__O uint32_t SK1_R7;					/*!< IPsec secure key 1 (bit 0-31) register,				Address offset: 0x00DC */
	__O uint32_t SK2_R0;					/*!< IPsec secure key 2 (bit 224-255) register,			Address offset: 0x00E0 */
	__O uint32_t SK2_R1;					/*!< IPsec secure key 2 (bit 192-223) register,			Address offset: 0x00E4 */
	__O uint32_t SK2_R2;					/*!< IPsec secure key 2 (bit 160-191) register,			Address offset: 0x00E8 */
	__O uint32_t SK2_R3;					/*!< IPsec secure key 2 (bit 128-159) register,			Address offset: 0x00EC */
	__O uint32_t SK2_R4;					/*!< IPsec secure key 2 (bit 96-127) register,				Address offset: 0x00F0 */
	__O uint32_t SK2_R5;					/*!< IPsec secure key 2 (bit 64-95) register,				Address offset: 0x00F4 */
	__O uint32_t SK2_R6;					/*!< IPsec secure key 2 (bit 32-63) register,				Address offset: 0x00F8 */
	__O uint32_t SK2_R7;					/*!< IPsec secure key 2 (bit 0-31) register,				Address offset: 0x00FC */
	uint32_t CRC_RSVD[10];					/*!< CRC  Reserved,								0x0100 -0x0124*/
	uint32_t RSVD5[472];					/*!< Reserved, 									0x0128 -0x0884*/
	__I uint32_t DBG_VERSION;				/*!< IPsec version register,							Address offset: 0x0888 */
	uint32_t RSVD6[477];					/*!< Reserved, 									0x088C -0x0FFC*/
	__IO uint32_t DDSR;					/*!< IPsec destination descriptor fifo status register,			Address offset: 0x1000 */
	__IO uint32_t DDFW;					/*!< IPsec destination descriptor first word,				Address offset: 0x1004 */
	__IO uint32_t DDSW;					/*!< IPsec destination descriptor second word,				Address offset: 0x1008 */
	__IO uint32_t DESC_PKT_CONF;			/*!< IPsec descriptor packet setting register,				Address offset: 0x100C */
	__I uint32_t DBG_SD;					/*!< IPsec source descriptor data debug register,			Address offset: 0x1010 */
	__I uint32_t DBG_DD;					/*!< IPsec destination descriptor data debug register,		Address offset: 0x1014 */
} IPSEC_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_CRC
 * @{
 * @brief AMEBAD_CRC Register Declaration
 *****************************************************************************/
typedef struct {
	__O uint32_t CRC_RST;					/*!< CRC  reset register,								Address offset: 0x0000 */
	__IO uint32_t CRC_OP;					/*!< CRC  operation register,							Address offset: 0x0004 */
	__IO uint32_t CRC_POLY;				/*!< CRC  polynomia register,							Address offset: 0x0008 */
	__IO uint32_t CRC_IV;					/*!< CRC  initial value register,							Address offset: 0x000C */
	__IO uint32_t CRC_OXOR;				/*!< CRC  xor output register,							Address offset: 0x0010 */
	__IO uint32_t CRC_DATA;				/*!< CRC  data register,								Address offset: 0x0014 */
	__IO uint32_t CRC_STAT;				/*!< CRC  status register,							Address offset: 0x0018 */
	__I uint32_t CRC_RESULT;				/*!< CRC  result register,								Address offset: 0x001C */
	__I uint32_t CRC_CNT;					/*!< CRC  count register,								Address offset: 0x0020 */
	__I uint32_t CRC_DMA_RX_DBG;			/*!< CRC  dma rx debug register,						Address offset: 0x0024 */
} CRC_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_KEYSCAN
 * @{
 * @brief AMEBAD_KEYSCAN Register Declaration
 *****************************************************************************/
typedef struct
{
	__IO uint32_t KS_CLK_DIV;			/*!< KEYSCAN clock divide register,		Address offset: 0x00 */
	__IO uint32_t KS_TIM_CFG0;		/*!< KEYSCAN timer configure register 0,	Address offset: 0x04 */
	__IO uint32_t KS_TIM_CFG1;		/*!< KEYSCAN timer configure register 1,	Address offset: 0x08 */
	__IO uint32_t KS_CTRL;			/*!< KEYSCAN control register,			Address offset: 0x0c */

	__IO uint32_t KS_FIFO_CFG;		/*!< KEYSCAN FIFO configure register,		Address offset: 0x10 */
	__IO uint32_t KS_COL_CFG;			/*!< KEYSCAN column configure register,	Address offset: 0x14 */
	__IO uint32_t KS_ROW_CFG;		/*!< KEYSCAN row configure register,		Address offset: 0x18 */
	__I uint32_t KS_DATA_NUM;		/*!< KEYSCAN FIFO data number register,	Address offset: 0x1C */
	__I uint32_t KS_DATA_ENTRY;		/*!< KEYSCAN event code register,		Address offset: 0x20*/

	__IO uint32_t KS_IMR;				/*!< KEYSCAN interrupt mask register,		Address offset: 0x24 */
	__IO uint32_t KS_ICR;				/*!< KEYSCAN interrupt clear register,		Address offset: 0x28 */
	__I uint32_t KS_ISR;				/*!< KEYSCAN interrupt status register,	Address offset: 0x2C */
	__I uint32_t KS_ISR_RAW;			/*!< KEYSCAN raw interrupt status register,	Address offset: 0x30 */
} KEYSCAN_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_SGPIO
 * @{
 * @brief AMEBAD_SGPIO Register Declaration
 *****************************************************************************/
typedef struct
{
	/*RX Timer Control Registers*/
	__IO uint32_t SGPIO_RXTMR_CTRL;			/*!< SGPIO RX Timer Control Register,		Address offset: 0x00 */
	__IO uint32_t SGPIO_RXPRVAL;				/*!< SGPIO RX Maximum Prescale Register,		Address offset: 0x04 */
	__IO uint32_t SGPIO_RXPRTC;				/*!< SGPIO RX Timer Prescale Register,		Address offset: 0x08 */
	__IO uint32_t SGPIO_RXTC;					/*!< SGPIO RX Timer Counter Register,		Address offset: 0x0C */
	__IO uint32_t SGPIO_RXMC_CTRL;			/*!< SGPIO RX Match Control Register,		Address offset: 0x10 */
	__IO uint32_t SGPIO_RXMR0;				/*!< SGPIO RX Match Register 0,		Address offset: 0x14 */
	__IO uint32_t SGPIO_RXMR1;				/*!< SGPIO RX Match Register 1,		Address offset: 0x18 */
	__IO uint32_t SGPIO_RXMR2;				/*!< SGPIO RX Match Register 2,		Address offset: 0x1C */

	/*Capture Function Control Registers*/
	__IO uint32_t SGPIO_CAP_CTRL;			/*!< SGPIO Capture Control Register,		Address offset: 0x20 */
	__IO uint32_t SGPIO_CAPR;					/*!< SGPIO Capture Register,		Address offset: 0x24 */

	/*Multiple Timer Control Registers*/
	__IO uint32_t SGPIO_MULTMR_CTRL;		/*!< SGPIO Multiple Timer Control Register,		Address offset: 0x28 */
	__IO uint32_t SGPIO_MULPRVAL;			/*!< SGPIO Multiple Maximum Prescale Register,		Address offset: 0x2C */
	__IO uint32_t SGPIO_MULPRTC;				/*!< SGPIO Multiple Timer Prescale Register,		Address offset: 0x30 */
	__IO uint32_t SGPIO_MULTC;				/*!< SGPIO Multiple Timer Counter Register,		Address offset: 0x34 */
	__IO uint32_t SGPIO_MULMC_CTRL;			/*!< SGPIO Multiple Match Control Register,		Address offset: 0x38 */
	__IO uint32_t SGPIO_MULMR01GP0;			/*!< SGPIO Multiple Match Register 0 & 1 For Group 0,		Address offset: 0x3C */
	__IO uint32_t SGPIO_MULMR23GP0;			/*!< SGPIO Multiple Match Register 2 & 3 For Group 0,		Address offset: 0x40 */
	__IO uint32_t SGPIO_MULMR01GP1;			/*!< SGPIO Multiple Match Register 0 & 1 For Group 1,		Address offset: 0x44 */
	__IO uint32_t SGPIO_MULMR23GP1;			/*!< SGPIO Multiple Match Register 2 & 3 For Group 1,		Address offset: 0x48 */
	__IO uint32_t SGPIO_MUL_MCNT;			/*!< SGPIO Multiple Match Counter Register,		Address offset: 0x4C */

	/*Data and Interrupt control Registers*/
	__IO uint32_t SGPIO_MULEMR;				/*!< SGPIO External Match Control Register,		Address offset: 0x50 */
	__IO uint32_t SGPIO_DATA_CTRL;			/*!< SGPIO Data Control Register,		Address offset: 0x54 */
	__IO uint32_t SGPIO_RXDATA;				/*!< SGPIO RX Data Register,		Address offset: 0x58 */
	__IO uint32_t SGPIO_RXDATA_DP;			/*!< SGPIO RX Data Duplicate Register,		Address offset: 0x5C */
	__IO uint32_t SGPIO_RXPOSR;				/*!< SGPIO RX Position Register,		Address offset: 0x60 */
	__IO uint32_t SGPIO_RXDATAMASK;			/*!< SGPIO RX Data Mask Register,		Address offset: 0x64 */
	__IO uint32_t SGPIO_MULDATA;				/*!< SGPIO Multiple Data Register,		Address offset: 0x68 */
	__IO uint32_t SGPIO_MULDATA_DP;			/*!< SGPIO Multiple Data Duplicate Register,		Address offset: 0x6C */
	__IO uint32_t SGPIO_MULPOSR;				/*!< SGPIO Multiple Position Register,		Address offset: 0x70 */
	__IO uint32_t SGPIO_MULFDATA;			/*!< SGPIO Multiple FIFO Data Register,		Address offset: 0x74 */
	__IO uint32_t SGPIO_ISR;					/*!< SGPIO Interrupt Status Register,		Address offset: 0x78 */
	__IO uint32_t SGPIO_GF;					/*!< SGPIO Glitch Filter Register,		Address offset: 0x7C */
	__IO uint32_t SGPIO_OUTPUT;				/*!< SGPIO Output Register,		Address offset: 0x80 */
	__IO uint32_t SGPIO_IMR;					/*!< SGPIO Interrupt Mask Register,		Address offset: 0x84 */
} SGPIO_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_PSRAM
 * @{
 * @brief AMEBAD_PSRAM Register Declaration
 *****************************************************************************/
typedef struct
{
	__IO uint32_t CCR;				/*!< PSRAM Configuration control register,		Address offset: 0x000 */
	__IO uint32_t DCR;				/*!< PSRAM Device Configuration control register,		Address offset: 0x004 */
	__IO uint32_t IOCR0;			/*!< PSRAM I/O Configuration control register0,		Address offset: 0x008 */
	__IO uint32_t CSR;				/*!< PSRAM Control Status register,		Address offset: 0x00C */
	__IO uint32_t DRR;				/*!< PSRAM Device Refresh/Power-Up register,		Address offset: 0x010 */

	__IO u32 RSVD0[4];				/*!< reserved space, 4 Dwords*/	

	__IO uint32_t CMD_DPIN_NDGE;	/*!< PSRAM Device Cmd/Addr Pin register(Negative Edge),		Address offset: 0x024 */
	__IO uint32_t CMD_DPIN;		/*!< PSRAM Device Cmd/Addr Pin register(Positive Edge),		Address offset: 0x028 */
	__IO uint32_t CR_TDPIN;		/*!< PSRAM Tie DPin register,		Address offset: 0x02C */
	__IO uint32_t MR_INFO;		/*!< PSRAM Mode Latency Information register,		Address offset: 0x030 */
	__IO uint32_t MR0;			/*!< PSRAM Device CR0 register,		Address offset: 0x034 */
	__IO uint32_t MR1;			/*!< PSRAM Device CR1 register,		Address offset: 0x038 */

	__IO u32 RSVD1[9];				/*!< reserved space, 9 Dwords*/	

	__IO uint32_t DPDRI;			/*!< PSRAM DPIN Data Index register,		Address offset: 0x060 */
	__IO uint32_t DPDR;			/*!< PSRAM DPIN Data register,		Address offset: 0x064 */

	__IO u32 RSVD2[35];				/*!< reserved space, 35 Dwords*/	

	__IO uint32_t PCTL_SVN_ID;	/*!< PSRAM_LPC_CTRL Version Numbers register,		Address offset: 0x0F4 */
	__IO uint32_t PCTL_IDR;		/*!< PSRAM_LPC_CTRL Identification register,		Address offset: 0x0F8 */

	__IO u32 RSVD3[193];				/*!< reserved space, 193 Dwords*/	

	__IO uint32_t USER0_INDEX;	/*!< PSRAM User Extended Index register,		Address offset: 0x400 */
	__IO uint32_t USER0_DATA;		/*!< PSRAM User Extended Data register,		Address offset: 0x404 */
} PCTL_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_GDMA
 * @{
 * @brief AMEBAD_GDMA Register Declaration
 *****************************************************************************/
typedef struct
{
	__IO uint32_t SAR;					/*!< Channel x Source Address Register,			Address offset: 0x0000 */
	uint32_t RSVD0;
	__IO uint32_t DAR;					/*!< Channel x Destination Address Register,		Address offset: 0x0008 */
	uint32_t RSVD1;
	__IO uint32_t LLP;						/*!< Channel x Linked List Pointer Register,		Address offset: 0x0010 */
	uint32_t RSVD2;
	__IO uint32_t CTL_LOW;				/*!< Channel x Control Register,					Address offset: 0x0018 */
	__IO uint32_t CTL_HIGH;				/*!< Channel x Control Register,					Address offset: 0x001C */
	__IO uint32_t SSTAT;					/*!< Channel x Source Status Register,			Address offset: 0x0020 */
	uint32_t RSVD4;
	__IO uint32_t DSTAT;					/*!< Channel x Destination Status Register,		Address offset: 0x0028 */
	uint32_t RSVD5;
	__IO uint32_t SSTATAR;				/*!< Channel x Source Status Address Register,	Address offset: 0x0030 */
	uint32_t RSVD6;
	__IO uint32_t DSTATAR;				/*!< Channel x Destination Status Address Register,	Address offset: 0x0038 */
	uint32_t RSVD7;
	__IO uint32_t CFG_LOW;				/*!< Channel x Configuration Register,			Address offset: 0x0040 */
	__IO uint32_t CFG_HIGH;				/*!< Channel x Configuration Register,			Address offset: 0x0044 */
	__IO uint32_t SGR;					/*!< Channel x Source Gather Register,			Address offset: 0x0048 */
	uint32_t RSVD9;
	__IO uint32_t DSR;					/*!< Channel x Source Gather Register,			Address offset: 0x0050 */
	uint32_t RSVD10;						/*!< Channel x Last Register,					Address offset: 0x0054 */
} GDMA_ChannelTypeDef;

typedef struct
{
	GDMA_ChannelTypeDef CH[8];		/*!< GDMA IP have 8 channels,				Address offset: 0x00-0x2BC */
	
	__I uint32_t RAW_TFR;				/*!< Raw Status for IntTfr Interrupt,			Address offset: 0x02C0 */
	uint32_t RSVD0;
	__I uint32_t RAW_BLOCK;			/*!< Raw Status for IntBlock Interrupt,		Address offset: 0x02C8 */
	uint32_t RSVD1;
	__I uint32_t RAW_SRC_TRAN;		/*!< Raw Status for IntSrcTran Interrupt,		Address offset: 0x02D0 */
	uint32_t RSVD2;
	__I uint32_t RAW_DST_TRAN;		/*!< Raw Status for IntDstTran Interrupt,		Address offset: 0x02D0 */
	uint32_t RSVD3;
	__I uint32_t RAW_ERR;				/*!< Raw Status for IntErr Interrupt,			Address offset: 0x02E0 */
	uint32_t RSVD4;

	__I uint32_t STATUS_TFR;			/*!< Status for IntTfr Interrupt,				Address offset: 0x02E8 */
	uint32_t RSVD5;
	__I uint32_t STATUS_BLOCK;		/*!< Status for IntBlock Interrupt,			Address offset: 0x02F0 */
	uint32_t RSVD6;
	__I uint32_t STATUS_SRC_TRAN;	/*!< Status for IntSrcTran Interrupt,			Address offset: 0x02F8 */
	uint32_t RSVD7;
	__I uint32_t STATUS_DST_TRAN;	/*!< Status for IntDstTran Interrupt,			Address offset: 0x0300 */
	uint32_t RSVD8;
	__I uint32_t STATUS_ERR;			/*!< Status for IntErr Interrupt,				Address offset: 0x0308 */
	uint32_t RSVD9;

	__IO uint32_t MASK_TFR;			/*!< Mask for IntTfr Interrupt,				Address offset: 0x0310 */
	uint32_t RSVD10;
	__IO uint32_t MASK_BLOCK;		/*!< Mask for IntBlock Interrupt,				Address offset: 0x0318 */
	uint32_t RSVD11;
	__IO uint32_t MASK_SRC_TRAN;	/*!< Mask for IntSrcTran Interrupt,			Address offset: 0x0320 */
	uint32_t RSVD12;
	__IO uint32_t MASK_DST_TRAN;	/*!< Mask for IntDstTran Interrupt,			Address offset: 0x0328 */
	uint32_t RSVD13;
	__IO uint32_t MASK_ERR;			/*!< Mask for IntErr Interrupt,				Address offset: 0x0330 */
	uint32_t RSVD14;

	__O uint32_t CLEAR_TFR;			/*!< Clear for IntTfr Interrupt,				Address offset: 0x0338 */
	uint32_t RSVD15;
	__O uint32_t CLEAR_BLOCK;		/*!< Clear for IntBlock Interrupt,				Address offset: 0x0340 */
	uint32_t RSVD16;
	__O uint32_t CLEAR_SRC_TRAN;	/*!< Clear for IntSrcTran Interrupt,			Address offset: 0x0348 */
	uint32_t RSVD17;
	__O uint32_t CLEAR_DST_TRAN;	/*!< Clear for IntDstTran Interrupt,			Address offset: 0x0350 */
	uint32_t RSVD18;
	__O uint32_t CLEAR_ERR;			/*!< Clear for IntErr Interrupt,				Address offset: 0x0358 */
	uint32_t RSVD19;
	__O uint32_t StatusInt;			/*!< Status for each interrupt type,			Address offset: 0x0360 */
	uint32_t RSVD191;

	__IO uint32_t ReqSrcReg;			/*!< Source Software Transaction Request Register,			Address offset: 0x0368 */
	uint32_t RSVD20;
	__IO uint32_t ReqDstReg;			/*!< Destination Software Transaction Request Register,		Address offset: 0x0370 */
	uint32_t RSVD21;
	__IO uint32_t SglReqSrcReg;		/*!< Single Source Transaction Request Register,			Address offset: 0x0378 */
	uint32_t RSVD22;
	__IO uint32_t SglReqDstReg;		/*!< Single Destination Transaction Request Register,		Address offset: 0x0380 */
	uint32_t RSVD23;
	__IO uint32_t LstSrcReg;			/*!< Last Source Transaction Request Register,				Address offset: 0x0388 */
	uint32_t RSVD24;
	__IO uint32_t LstDstReg;			/*!< Last Destination Transaction Request Register,			Address offset: 0x0390 */
	uint32_t RSVD25;

	__IO uint32_t DmaCfgReg;			/*!< DMA Configuration Register,			Address offset: 0x0398 */
	uint32_t RSVD26;
	__IO uint32_t ChEnReg;			/*!< DMA Channel Enable Register,			Address offset: 0x03A0 */
	uint32_t RSVD27;
	__I uint32_t DmaIdReg;			/*!< DMA ID Register,						Address offset: 0x03A8 */
	uint32_t RSVD28;
	__IO uint32_t DmaTestReg;		/*!< DMA Test Register,					Address offset: 0x03B0 */
	uint32_t RSVD29;
} GDMA_TypeDef;

/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_SDIO_HOST
 * @{
 * @brief AMEBAD_SDIO_HOST Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t RSVD1[256];
	__IO uint32_t SRAM_CRL;			/*!< SRAM Control Register,		Address offset: 0x00000400*/
	__IO uint32_t DMA_CRL1;			/*!< DMA Control Register 1,		Address offset: 0x00000404*/
	__IO uint32_t DMA_CRL2;			/*!< DMA Control Register 2,		Address offset: 0x00000408*/
	__IO uint32_t DMA_CRL3;			/*!< DMA Control Register 3,		Address offset: 0x0000040C*/
	__IO uint32_t SYS_LOW_PWR;		/*!< Low Power Control Register,	Address offset: 0x00000410*/
	__IO uint32_t RSVD2[4];
	__IO uint32_t SD_ISR;				/*!< SD Interrupt Status Register,Address offset: 0x00000424*/
	__IO uint32_t SD_ISREN;			/*!< SD Interrupt Enable Register,Address offset: 0x00000428*/
	__IO uint32_t RSVD3[18];
	__IO uint32_t PAD_CTL;			/*!< Pad Control Register,		Address offset: 0x00000474*/
	__IO uint32_t CKGEN_CTL;			/*!< Clock Generation Control Register,	Address offset: 0x00000478*/
	__IO uint32_t RSVD4[33];
	__IO uint16_t RSVD5;
	__IO uint8_t   CARD_DRIVER_SEL;	/*!< Card Driving Selection Register,Address offset: 0x00000502*/
	__O  uint8_t   CARD_STOP;			/*!< Stop Transfer Register,Address offset: 0x00000503*/
	__IO uint32_t RSVD6[2];
	__IO uint16_t RSVD7;
	__IO uint8_t   CARD_SELECT;		/*!< Card Type Select Register,	Address offset: 0x0000050E*/
	__IO uint8_t   DUMMY1;				/*!< Dummy Register 1,			Address offset: 0x0000050F*/	
	__IO uint32_t	RSVD8[3];
	__IO uint16_t	RSVD9;
	__IO uint8_t	RSVD10;
	__I   uint8_t   CARD_EXIST;			/*!< Card Detection Register,	Address offset: 0x0000051F*/
	__IO uint8_t   CARD_INT_EN;		/*!< Card Interrupt Enable Register,Address offset: 0x00000520*/
	__IO uint8_t   CARD_INT_PEND;		/*!< Card Interrupt Status Register,Address offset: 0x00000521*/
	__IO uint16_t	RSVD11;
	__IO uint32_t	RSVD12;
	__IO uint8_t	RSVD13;
	__IO uint8_t   CARD_CLK_EN_CTL;	/*!< Card Clock Enable Control Register,Address offset: 0x00000529*/
	__IO uint16_t	RSVD14;
	__IO uint32_t	RSVD15;
	__IO uint8_t   CLK_PAD_DRIVE;		/*!< Clock Pad Driving Register,	Address offset: 0x00000530*/
	__IO uint8_t   CMD_PAD_DRIVE;		/*!< Command Pad Driving Register ,Address offset: 0x00000531*/
	__IO uint8_t   DAT_INT_PEND;		/*!< Data Pad Driving Register,	Address offset: 0x00000532*/
	__IO uint8_t	RSVD16;
	__IO uint32_t	RSVD17[19];
	__IO uint8_t   SD_CONFIG1;			/*!< SD Configuration Register 1,	Address offset: 0x00000580*/
	__IO uint8_t   SD_CONFIG2;			/*!< SD Configuration Register 2  ,Address offset: 0x00000581*/
	__IO uint8_t   SD_CONFIG3;			/*!< SD Configuration Register 3,	Address offset: 0x00000582*/
	__I   uint8_t   SD_STATUS1;			/*!< SD Status Register 1  ,		Address offset: 0x00000583*/
	__IO uint8_t   SD_STATUS2;			/*!< SD Status Register 2,		Address offset: 0x00000584*/
	__IO uint8_t   SD_BUS_STATUS;		/*!< SD Bus Status Register,	Address offset: 0x00000585*/
	__IO uint16_t	RSVD18;
	__IO uint8_t	RSVD19;
	__IO uint8_t   SD_CMD[6];			/*!< SD Command Register 0-5,	Address offset: 0x00000589-0x0000058E*/
	__IO uint8_t   SD_BYTE_CNT_L;		/*!< Byte Count Register (Low Byte),	Address offset: 0x0000058F*/
	__IO uint8_t   SD_BYTE_CNT_H;		/*!< Byte Count Register (High Byte),	Address offset: 0x00000590*/
	__IO uint8_t   SD_BLOCK_CNT_L;	/*!< Block Count Register (Low Byte),	Address offset: 0x00000591*/
	__IO uint8_t   SD_BLOCK_CNT_H;	/*!< Block Count Register (High Byte),	Address offset: 0x00000592*/
	__IO uint8_t   SD_TRANSFER;		/*!< SD Transfer Control Register,	Address offset: 0x00000593*/
	__IO uint8_t   RSVD20;
	__I   uint8_t   SD_CMD_STATE;		/*!< SD Command State Register,	Address offset: 0x00000595*/
	__I   uint8_t   SD_DATA_STATE;		/*!< SD Data State Register, 	Address offset: 0x00000596*/ 
} SDIOH_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_IPC
 * @{
 * @brief AMEBAD_IPC Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t IPCx_IER;		/*!< Interrupt Enable Register,		Address offset: 0x00, Writing a '0' have no effect */
	__IO uint32_t IPCx_IDR;		/*!< Interrupt Disable Register,		Address offset: 0x04, Writing a '0' have no effect */
	__IO uint32_t IPCx_IRR;		/*!< Interrupt Request Register,		Address offset: 0x08, Writing a '0' have no effect */
	__IO uint32_t IPCx_ICR;		/*!< Interrupt Clear Register,		Address offset: 0x0C, Writing a '0' have no effect */
	__IO uint32_t IPCx_CPUID;		/*!< CPU ID Register,				Address offset: 0x10, RO (Just CM0 Valid) */
	__IO uint32_t IPCx_ISR;		/*!< Interrupt Status Register,		Address offset: 0x14, RO */
	__IO uint32_t IPC0_SEM;		/*!< Hardware Semaphore Register,	Address offset: 0x18 (Just CM0 Valid) */
	__IO uint32_t IPCx_IER_R;		/*!< Current IER Read register,		Address offset: 0x1C, RO */
	__IO uint32_t IPCx_USR[12];	/*!< User defined register,			Address offset: 0x0020~0x004C, RW */
} IPC_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_BACKUP_REG
 * @{
 * @brief AMEBAD_BACKUP_REG Register Declaration
 * @note 16 Bytes total
 *****************************************************************************/
typedef struct {
	__IO uint32_t DWORD[8]; /* 0x3C0 */
} BACKUP_REG_TypeDef;
/** @} */

/**************************************************************************//**
 * @defgroup AMEBAD_RRAM
 * @{
 * @brief AMEBAD_RRAM Declaration
 * @ the Max space for RRAM_TypeDef is 0xB0, user can alloc space from RRAM_USER_RSVD
 *****************************************************************************/
typedef struct {
	uint8_t RRAM_WIFI_STATUS; /* RETENTION_RAM_SYS_OFFSET 0x80 */
	uint8_t RRAM_WIFI_P_SECURTY;
	uint8_t RRAM_WIFI_G_SECURTY;
	uint8_t RRAM_RSVD1;
	
	uint32_t RRAM_NET_IP;
	uint32_t RRAM_NET_GW;
	uint32_t RRAM_NET_GW_MASK;

	uint32_t FLASH_ID2;				/*offset 0x90*/		
	uint32_t FLASH_cur_cmd;
	uint32_t FALSH_quad_valid_cmd;
	uint32_t FALSH_dual_valid_cmd;
	uint32_t FLASH_QuadEn_bit;		/*offset 0xA0*/
	uint32_t FLASH_StructInit;
	
	uint8_t FLASH_phase_shift_idx;
	uint8_t FLASH_rd_sample_phase_cal;
	uint8_t FLASH_class;
	uint8_t FLASH_cur_bitmode;
	
	uint8_t FLASH_ClockDiv;
	uint8_t FLASH_ReadMode;
	uint8_t FLASH_pseudo_prm_en;
	uint8_t FLASH_addr_phase_len;

	uint8_t FLASH_cmd_wr_status2;	/*offset 0xB0*/
	uint8_t FLASH_rd_dummy_cyle0;	
	uint8_t FLASH_rd_dummy_cyle1;
	uint8_t FLASH_rd_dummy_cyle2;

	uint8_t RRAM_USER_RSVD[124];			/*usr can alloc from this RSVD space*/

	
} RRAM_TypeDef;
/** @} */


/**************************************************************************//**
 * @defgroup AMEBAD_RSIP_REG
 * @{
 * @brief AMEBAD_RSIP_REG Register Declaration
 * @note Flash MMU & Flash OTF
 *****************************************************************************/
typedef struct {
	__IO uint32_t OTF_MASK_ENTRYx_CTRL;			/*!< otf mask control */
	__IO uint32_t OTF_MASK_ENTRYx_STRADDR;		/*!< otf mask start address */
} OTF_Mask_TypeDef;

typedef struct {
	__IO uint32_t MMU_ENTRYx_CTRL;			/*!< flash MMU entry_x control */
	__IO uint32_t MMU_ENTRYx_STRADDR;		/*!< flash MMU entry_x virtual address start */
	__IO uint32_t MMU_ENTRYx_ENDADDR;		/*!< flash MMU entry_x virtual address end */
	__IO uint32_t MMU_ENTRYx_OFFSET;		/*!< flash PAddress = VAddress +/- MMU_ENTRYx_OFFSET */
} RSIP_MMU_TypeDef;

typedef struct {
	__IO uint32_t REG_OTF_DEC_CTRL;			/*!< Flash decryption control register,			Address offset: 0x000 */
	__IO uint32_t REG_OTF_DEC_IV_EXT[3];		/*!< OTF AES IV[95:0],					Address offset: 0x004~0x00C */
	OTF_Mask_TypeDef OTF_MASK[4];			/*!< Flash decryption control register,			Address offset: 0x010~0x02C */
	RSIP_MMU_TypeDef FLASH_MMU[8];			/*!< Flash MMU control register,				Address offset: 0x030~0x0AF */
} RSIP_REG_TypeDef;
/** @} */
/** @} End of group AmebaD_Peripheral_Registers_Structures */

/**************************************************************************//**
 * @defgroup AmebaD_Peripheral_Declaration  AmebaD HS/LP Peripheral Declarations
 * @{
 *****************************************************************************/
#define UART0_DEV			((UART_TypeDef			*) UART0_REG_BASE)		/*KM4 uart0 */
#define UART1_DEV			((UART_TypeDef			*) UART1_REG_BASE)		/*KM4 uart1_bt */
#define UART2_DEV			((UART_TypeDef			*) UART2_REG_BASE)		/*KM0 log uart */
#define UART3_DEV			((UART_TypeDef			*) UART3_REG_BASE)		/*KM0 luart*/

#define SPI0_DEV				((SPI_TypeDef			*) SPI0_REG_BASE)		/*KM4 */
#define SPI1_DEV				((SPI_TypeDef			*) SPI1_REG_BASE)		/*KM4 */
#define I2S_DEV				((I2S_TypeDef			*) I2S0_REG_BASE)
#define AUDIO_SI_DEV		((AUDIO_SI_TypeDef		*) AUDIO_SI_REG_BASE)			/*KM4 */
#define AUDIO_SPORT_DEV	((AUDIO_SPORT_TypeDef	*) AUDIO_SPORT_REG_BASE)		/*KM4 */
#define USI0_DEV			((USI_TypeDef			*) USI_REG_BASE)					/*KM4 */
#define LCDC					((LCDC_TypeDef 			*) LCDC_REG_BASE)				/*KM4 */
#define PSRAM_DEV			((PCTL_TypeDef 			*) PSRAM_REG_BASE)				/*KM4 */

#define SPIC					((SPIC_TypeDef			*) SPI_FLASH_CTRL_BASE)
#define ADC					((ADC_TypeDef			*) ADC_REG_BASE)
#define COMPARATOR				((CMP_TypeDef			*) CMP_REG_BASE)
#define I2C0_DEV				((I2C_TypeDef			*) I2C0_REG_BASE)

#define TIM0					((RTIM_TypeDef			*) TIMERM4_REG_BASE)
#define TIM1					((RTIM_TypeDef			*) (TIMERM4_REG_BASE + 0x080))
#define TIM2					((RTIM_TypeDef			*) (TIMERM4_REG_BASE + 0x100))
#define TIM3					((RTIM_TypeDef			*) (TIMERM4_REG_BASE + 0x180))
#define TIM4					((RTIM_TypeDef			*) (TIMERM4_REG_BASE + 0x200))
#define TIM5					((RTIM_TypeDef			*) (TIMERM4_REG_BASE + 0x280))
#define TIMM00				((RTIM_TypeDef			*) TIMERM0_REG_BASE)			/*KM0 */
#define TIMM01				((RTIM_TypeDef			*) (TIMERM0_REG_BASE + 0x080))	/*KM0 */
#define TIMM02				((RTIM_TypeDef			*) (TIMERM0_REG_BASE + 0x100))	/*KM0 */
#define TIMM03				((RTIM_TypeDef			*) (TIMERM0_REG_BASE + 0x180))	/*KM0 */
#define TIMM04				((RTIM_TypeDef			*) (TIMERM0_REG_BASE + 0x200))	/*KM0 */
#define TIMM05				((RTIM_TypeDef			*) (TIMERM0_REG_BASE + 0x280))	/*KM0 */

#define KEYSCAN_DEV		((KEYSCAN_TypeDef		*) KEYSCAN_REG_BASE)			/*KM0 */
#define SGPIO_DEV			((SGPIO_TypeDef			*) SGPIO_REG_BASE)			/*KM0 */
#define CAPTOUCH_DEV		((CAPTOUCH_TypeDef		*) CTC_REG_BASE)				/*KM0 */
#define IR_DEV				((IR_TypeDef 				*) IR_REG_BASE)				/*KM0 */
#define QDEC_DEV			((QDEC_TypeDef			*) QDEC_REG_BASE)			/*KM0 */
#define RTC_DEV				((RTC_TypeDef			*) (RTC_BASE - 0x08000000); 	/*KM0 */
#define PINMUX				((PINMUX_TypeDef			*) PINMUX_REG_BASE)			/*KM0 */
#define GPIOA_BASE			((GPIO_TypeDef			*) GPIO_REG_BASE)			/*KM0 */
#define GPIOB_BASE			((GPIO_TypeDef			*) (GPIO_REG_BASE + 0x400))	/*KM0 */
#define BACKUP_REG			((BACKUP_REG_TypeDef	*) BACKUP_REG_BASE)			/*KM0 */

#define KM4_IDAU			((IDAU_TypeDef			*) KM4_IDAU_BASE)			/*KM4 */
#define IPCM4_DEV 			((IPC_TypeDef 			*) IPCM4_REG_BASE)				/*KM4 */
#define IPCM0_DEV 			((IPC_TypeDef			*) IPCM0_REG_BASE)				/*KM0 */

#define GDMA_BASE			((GDMA_TypeDef			*) (GDMA0_REG_BASE))

#define WDG_BASE			((WDG_TypeDef			*) (VENDOR_REG_BASE))
#define RSIP_BASE			((RSIP_REG_TypeDef		*) (RSIP_REG_BASE))
#define SDIOH_BASE			((SDIOH_TypeDef			*) (SDIOH_REG_BASE))

#define RRAM_BASE			((RRAM_TypeDef			*) (RETENTION_RAM_BASE + RETENTION_RAM_SYS_OFFSET))
/** @} End of group AmebaD_Peripheral_Declaration */

/** @} End of group AmebaD_Outline */
#endif //_HAL_PLATFORM_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
