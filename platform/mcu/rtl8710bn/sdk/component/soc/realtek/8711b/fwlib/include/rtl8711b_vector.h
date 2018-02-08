/**
  ******************************************************************************
  * @file    rtl8711b_vector.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the IRQ firmware
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

#ifndef _RTL8710B_VECTOR_TABLE_H_
#define _RTL8710B_VECTOR_TABLE_H_

/** @addtogroup AmebaZ_Platform
  * @{
  */

/** @defgroup IRQ 
  * @brief IRQ modules
  * @{
  */

/** @addtogroup IRQ
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * IRQ table, please refer to IRQ Exported Constants->IRQn_enum->IRQn
  *
  *****************************************************************************************
  * how to use
  *****************************************************************************************
  *		1. register/unregister IRQ use: InterruptRegister/InterruptUnRegister
  *		2. enable/disable IRQ use: InterruptEn/InterruptDis
  * 
  *****************************************************************************************
  * @endverbatim
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup IRQ_Exported_Types IRQ Exported Types
  * @{
  */
typedef s32 IRQn_Type;
typedef void (*HAL_VECTOR_FUN) (void);
typedef u32 (*IRQ_FUN)(void *Data);
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup IRQ_Exported_Constants IRQ Exported Constants
  * @{
  */

/** @defgroup IRQn_enum 
  * @{
  */ 
enum IRQn {
	/******  Cortex-M4 Processor Exceptions Numbers ********/  
	NonMaskableInt_IRQn		= -14,	/*!< 2 Non Maskable Interrupt                         */
	HardFault_IRQn				= -13,	/*!< 3  Hard Fault, all classes of Fault                                 */
	MemoryManagement_IRQn	= -12,	/*!< 4 Cortex-M3 Memory Management Interrupt          */
	BusFault_IRQn				= -11,	/*!< 5 Cortex-M3 Bus Fault Interrupt                  */
	UsageFault_IRQn			= -10,	/*!< 6 Cortex-M3 Usage Fault Interrupt                */
	SVCall_IRQn					= -5,	/*!< 11 Cortex-M3 SV Call Interrupt                   */
	DebugMonitor_IRQn			= -4,	/*!< 12 Cortex-M3 Debug Monitor Interrupt             */
	PendSV_IRQn				= -2,	/*!< 14 Cortex-M3 Pend SV Interrupt                   */
	SysTick_IRQn				= -1,	/*!< 15 Cortex-M3 System Tick Interrupt               */

	/******  RTL8710B Specific Interrupt Numbers ************/
	SYSTEM_ON_IRQ				=  0,	/*!< 16 SYS Interrupt for wakeup from power save */
	WDG_IRQ					=  1,	/*!< 17 Watch dog global insterrupt              */
	TIMER0_IRQ					=  2,	/*!< 18 Timer0 global interrupt               */
	TIMER1_IRQ					=  3,	/*!< 19 Timer1 global interrupt              */
	TIMER2_IRQ					=  4,	/*!< 20 Timer2 global interrupt               */
	TIMER3_IRQ					=  5,	/*!< 21 Timer3 global interrupt               */
	SPI0_IRQ					=  6,	/*!< 22 SPI0 global interrupt for communication spi              */
	GPIO_IRQ					=  7,	/*!< 23 GPIO portA global interrupt              */
	UART0_IRQ					=  8,	/*!< 24 UART0 global interrupt               */
	SPI_FLASH_IRQ				=  9,	/*!< 25 SPI Flash global interrupt               */
	UART1_IRQ					=  10,	/*!< 26 UART1 global interrupt               */
	TIMER4_IRQ					=  11,	/*!< 27 Timer4 global interrupt               */
	SDIO_DEVICE_IRQ			=  12,	/*!< 28 SDIO device global interrupt               */
	I2S0_PCM0_IRQ				=  13,	/*!< 29 I2S0 global interrupt               */
	TIMER5_IRQ					=  14,	/*!< 30 Timer5 global interrupt               */
	WL_DMA_IRQ				=  15,	/*!< 31 Wlan Host global interrupt              */
	WL_PROTOCOL_IRQ			=  16,	/*!< 32 Wlan Firmware Wlan global interrupt              */
	CRYPTO_IRQ					=  17,	/*!< 33 IPsec global interrupt               */
	SPI1_IRQ					=  18,	/*!< 34 SPI1 global interrupt for communication spi               */
	PERIPHERAL_IRQ				=  19,	/*!< 35 not used               */
	GDMA0_CHANNEL0_IRQ		=  20,	/*!< 36 GDMA0 channel 0 global interrupt               */
	GDMA0_CHANNEL1_IRQ		=  21,	/*!< 37 GDMA0 channel 1 global interrupt               */
	GDMA0_CHANNEL2_IRQ		=  22,	/*!< 38 GDMA0 channel 2 global interrupt               */
	GDMA0_CHANNEL3_IRQ		=  23,	/*!< 39 GDMA0 channel 3 global interrupt               */
	GDMA0_CHANNEL4_IRQ		=  24,	/*!< 40 GDMA0 channel 4 global interrupt               */
	GDMA0_CHANNEL5_IRQ		=  25,	/*!< 41 GDMA0 channel 5 global interrupt               */

	I2C0_IRQ                     		=  26,	/*!< 42 I2C0 global interrupt               */
	I2C1_IRQ                     		=  27,	/*!< 43 I2C1 global interrupt               */
	UART_LOG_IRQ                     	=  28,	/*!< 44 log uart intr              */
	ADC_IRQ                     		=  29,	/*!< 45 adc intr               */
	RDP_IRQ                     		=  30,	/*!< 46 cpu rdp protection interrupt               */
	RTC_IRQ                     			=  31,	/*!< 47 rtc timer interrupt               */

	GDMA1_CHANNEL0_IRQ		=  32,	/*!< 48 GDMA1 channel 0 global interrupt               */
	GDMA1_CHANNEL1_IRQ		=  33,	/*!< 49 GDMA1 channel 1 global interrupt               */
	GDMA1_CHANNEL2_IRQ		=  34,	/*!< 50 GDMA1 channel 2 global interrupt               */
	GDMA1_CHANNEL3_IRQ		=  35,	/*!< 51 GDMA1 channel 3 global interrupt               */
	GDMA1_CHANNEL4_IRQ		=  36,	/*!< 52 GDMA1 channel 4 global interrupt               */
	GDMA1_CHANNEL5_IRQ		=  37,	/*!< 53 GDMA1 channel 5 global interrupt               */
	USB_IRQ					=  38,	/*!< 54 USOC interrupt              */
	RXI300_IRQ					=  39,	/*!< 55 RXI300 interrupt               */
	USB_SIE_IRQ				=  40,	/*!< 56 USB SIE interrupt               */
};
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup IRQ_Exported_Functions IRQ Exported Functions
  * @{
  */
extern _LONG_CALL_ void VECTOR_TableInit(u32 StackP);
extern _LONG_CALL_ void VECTOR_TableInitForOS(void *PortSVC, void *PortPendSVH, void *PortSysTick);
extern _LONG_CALL_ BOOL VECTOR_IrqRegister(IRQ_FUN IrqFun, IRQn_Type IrqNum, u32 Data, u32 Priority);
extern _LONG_CALL_ BOOL VECTOR_IrqUnRegister(IRQn_Type IrqNum);
extern _LONG_CALL_ void VECTOR_IrqEn(IRQn_Type   IrqNum, u32 Priority);
extern _LONG_CALL_ void VECTOR_IrqDis(IRQn_Type   IrqNum);

#define InterruptForOSInit	VECTOR_TableInitForOS
#define InterruptRegister		VECTOR_IrqRegister
#define InterruptUnRegister	VECTOR_IrqUnRegister

#define InterruptEn			VECTOR_IrqEn
#define InterruptDis			VECTOR_IrqDis
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
extern IRQ_FUN UserIrqFunTable[64];
extern u32 UserIrqDataTable[64];
extern HAL_VECTOR_FUN  NewVectorTable[];

#define PERIPHERAL_IRQ_STATUS		0x04
#define PERIPHERAL_IRQ_MODE			0x08
#define PERIPHERAL_IRQ_EN			0x0C
#define LP_PERI_EXT_IRQ_STATUS		0x24
#define LP_PERI_EXT_IRQ_MODE		0x28
#define LP_PERI_EXT_IRQ_EN			0x2C
#define PERIPHERAL_IRQ_ALL_LEVEL	0

#endif //_RTL8710B_VECTOR_TABLE_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
