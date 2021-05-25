/**
  ******************************************************************************
  * @file    rtl8721d_vector.h
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

/** @addtogroup AmebaD_Platform
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
	SYSTEM_ON_IRQ				=  0,	/*!< 0 SYS Interrupt for wakeup from power save */
	WDG_IRQ					=  1,	/*!< 1 Watch dog global insterrupt              */
	RXI300_IRQ					=  2,	/*!< 2 RXI300 interrupt               */
	UART_LOG_IRQ				=  3,	/*!< 3 log uart intr              */
	GPIOA_IRQ					=  4,	/*!< 4 GPIOA portA global interrupt              */
	RTC_IRQ						=  5,	/*!< 5 rtc timer interrupt               */
	I2C0_IRQ					=  6,	/*!< 6 I2C0 global interrupt               */
	SPI_FLASH_IRQ				=  7,	/*!< 7 SPI Flash global interrupt               */
	GPIOB_IRQ					=  8,	/*!< 8 GPIOB portA global interrupt              */
	UARTLP_IRQ					=  9,	/*!< 9 UART0 global interrupt               */
	KEYSCAN_IRQ				=  10,	/*!< 10 KEYSCAN interrupt              */
	CTOUCH_IRQ					=  11,	/*!< 11 Cap-Touch interrupt              */
	BOR2_IRQ					=  12,	/*!< 12 BOR2 interrupt              */
	SGPIO_IRQ					=  13,	/*!< 13 SGPIO interrupt              */
	IPC_IRQ						=  14,	/*!< 14 IPC_KM0 interrupt              */
	ADC_IRQ					=  15,	/*!< 15 adc interrupt               */
	QDECODER_IRQ				=  16,	/*!< 16 Q-DECODER interrupt               */
	TIMER0_IRQ					=  17,	/*!< 17 Timer0 global interrupt               */
	TIMER1_IRQ					=  18,	/*!< 18 Timer1 global interrupt              */
	TIMER2_IRQ					=  19,	/*!< 19 Timer2 global interrupt               */
	TIMER3_IRQ					=  20,	/*!< 20 Timer3 global interrupt               */
	TIMER4_IRQ					=  21,	/*!< 21 Timer4 global interrupt               */
	TIMER5_IRQ					=  22,	/*!< 22 Timer5 global interrupt               */
	LCDC_IRQ					=  23,	/*!< 23 LCDC interrupt               */
	USB_OTG_IRQ				=  24,	/*!< 24 USOC interrupt              */
	SDIO_DEVICE_IRQ			=  25,	/*!< 25 SDIO device global interrupt               */
	SDIO_HOST_IRQ				=  26,	/*!< 26 SDIO host global interrupt               */
	CRYPTO_IRQ					=  27,	/*!< 27 IPsec global interrupt               */
	I2S0_PCM0_IRQ				=  28,	/*!< 28 I2S0 global interrupt               */
	PWR_DOWN_IRQ				=  29,	/*!< 29 power down enable interrupt               */
	ADC_COMP_IRQ				=  30,	/*!< 30 ADC compare interrupt               */
	WL_DMA_IRQ				=  31,	/*!< 31 Wlan Host global interrupt              */
	WL_PROTOCOL_IRQ			=  32,	/*!< 32 Wlan Firmware Wlan global interrupt              */
	PSRAMC_IRQ					=  33,	/*!< 33 PSRAM controller interrupt              */	
	UART0_IRQ					=  34,	/*!< 34 UART0 global interrupt               */
	UART1_IRQ					=  35,	/*!< 35 UART1 BT UART global interrupt               */
	SPI0_IRQ					=  36,	/*!< 36 SPI0 global interrupt for communication spi              */
	SPI1_IRQ					=  37,	/*!< 37 SPI1 global interrupt for communication spi               */
	USI_IRQ						=  38,	/*!< 38 USI global interrupt      */
	IR_IRQ						=  39,	/*!< 39 IR global interrupt      */
	BT2WL_STS_IRQ				=  40,	/*!< 40 BT to WL Status Interrupt      */

	GDMA0_CHANNEL0_IRQ		=  41,	/*!< 41 GDMA0 channel 0 global interrupt               */
	GDMA0_CHANNEL1_IRQ		=  42,	/*!< 42 GDMA0 channel 1 global interrupt               */
	GDMA0_CHANNEL2_IRQ		=  43,	/*!< 43 GDMA0 channel 2 global interrupt               */
	GDMA0_CHANNEL3_IRQ		=  44,	/*!< 44 GDMA0 channel 3 global interrupt               */
	GDMA0_CHANNEL4_IRQ		=  45,	/*!< 45 GDMA0 channel 4 global interrupt               */
	GDMA0_CHANNEL5_IRQ		=  46,	/*!< 46 GDMA0 channel 5 global interrupt               */

	CRYPTO_IRQ_S				=  50,	/*!< 50 IPsec global interrupt               */
	RXI300_IRQ_S				=  51,	/*!< 51 RXI300 interrupt               */
	GDMA0_CHANNEL0_IRQ_S		=  52,	/*!< 52 GDMA0 channel 0 global interrupt               */
	GDMA0_CHANNEL1_IRQ_S		=  53,	/*!< 53 GDMA0 channel 1 global interrupt               */
	GDMA0_CHANNEL2_IRQ_S		=  54,	/*!< 54 GDMA0 channel 2 global interrupt               */
	GDMA0_CHANNEL3_IRQ_S		=  55,	/*!< 55 GDMA0 channel 3 global interrupt               */
	GDMA0_CHANNEL4_IRQ_S		=  56,	/*!< 56 GDMA0 channel 4 global interrupt               */
	GDMA0_CHANNEL5_IRQ_S		=  57,	/*!< 57 GDMA0 channel 5 global interrupt               */
};
/**
  * @}
  */

/** @defgroup LPIRQn_enum 
  * @{
  */ 
enum LPIRQn {
	/******  Cortex-M4 Processor Exceptions Numbers ********/  
	NonMaskableInt_IRQn_LP			= -14,	/*!< 2 Non Maskable Interrupt                         */
	HardFault_IRQn_LP				= -13,	/*!< 3  Hard Fault, all classes of Fault                                 */
	MemoryManagement_IRQn_LP	= -12,	/*!< 4 Cortex-M3 Memory Management Interrupt          */
	BusFault_IRQn_LP				= -11,	/*!< 5 Cortex-M3 Bus Fault Interrupt                  */
	UsageFault_IRQn_LP				= -10,	/*!< 6 Cortex-M3 Usage Fault Interrupt                */
	SVCall_IRQn_LP					= -5,	/*!< 11 Cortex-M3 SV Call Interrupt                   */
	DebugMonitor_IRQn_LP			= -4,	/*!< 12 Cortex-M3 Debug Monitor Interrupt             */
	PendSV_IRQn_LP					= -2,	/*!< 14 Cortex-M3 Pend SV Interrupt                   */
	SysTick_IRQn_LP				= -1,	/*!< 15 Cortex-M3 System Tick Interrupt               */

	/******  RTL8710B Specific Interrupt Numbers ************/
	SYSTEM_ON_IRQ_LP				=  0,	/*!< 0 SYS Interrupt for wakeup from power save */
	WDG_IRQ_LP					=  1,	/*!< 1 Watch dog global insterrupt              */
	RXI300_IRQ_LP					=  2,	/*!< 2 RXI300 interrupt               */
	UART_LOG_IRQ_LP				=  3,	/*!< 3 log uart intr              */
	GPIOA_IRQ_LP					=  4,	/*!< 4 GPIOA portA global interrupt              */
	RTC_IRQ_LP						=  5,	/*!< 5 rtc timer interrupt               */
	I2C0_IRQ_LP					=  6,	/*!< 6 I2C0 global interrupt               */
	SPI_FLASH_IRQ_LP				=  7,	/*!< 7 SPI Flash global interrupt               */
	GPIOB_IRQ_LP					=  8,	/*!< 8 GPIOB portA global interrupt              */
	UARTLP_IRQ_LP					=  9,	/*!< 9 UART0 global interrupt               */
	KEYSCAN_IRQ_LP				=  10,	/*!< 10 KEYSCAN interrupt              */
	CTOUCH_IRQ_LP					=  11,	/*!< 11 Cap-Touch interrupt              */
	BOR2_IRQ_LP					=  12,	/*!< 12 BOR2 interrupt              */
	SGPIO_IRQ_LP					=  13,	/*!< 13 SGPIO interrupt              */
	IPC_IRQ_LP						=  14,	/*!< 14 IPC_KM4 interrupt              */
	ADC_IRQ_LP						=  15,	/*!< 15 adc interrupt               */
	QDECODER_IRQ_LP				=  16,	/*!< 16 Q-DECODER interrupt               */
	TIMER0_IRQ_LP					=  17,	/*!< 17 Timer0 global interrupt               */
	TIMER1_IRQ_LP					=  18,	/*!< 18 Timer1 global interrupt              */
	TIMER2_IRQ_LP					=  19,	/*!< 19 Timer2 global interrupt               */
	TIMER3_IRQ_LP					=  20,	/*!< 20 Timer3 global interrupt               */
	TIMER4_IRQ_LP					=  21,	/*!< 21 Timer4 global interrupt               */
	TIMER5_IRQ_LP					=  22,	/*!< 22 Timer5 global interrupt               */
	GDMA0_CHANNEL0_IRQ_LP		=  23,	/*!< 23 GDMA channel 0 global interrupt               */
	GDMA0_CHANNEL1_IRQ_LP		=  24,	/*!< 24 GDMA channel 1 global interrupt               */
	GDMA0_CHANNEL2_IRQ_LP		=  25,	/*!< 25 GDMA channel 2 global interrupt               */
	WIFI_FISR_FESR					=  26,	/*!< 26 WIFI_FISR_FESR interrupt               */
	WIFI_FTSR_MAILBOX				=  27,	/*!< 27 WIFI_FTSR_MAILBOX interrupt               */
	GDMA0_CHANNEL3_IRQ_LP		=  28,	/*!< 28 GDMA channel 3 global interrupt               */
	PWR_DOWN_IRQ_LP				=  29,	/*!< 29 power down enable interrupt               */
	ADC_COMP_IRQ_LP				=  30,	/*!< 30 ADC compare interrupt               */
	KM4_WAKE_EVENT_IRQ_LP		=  31,	/*!< 31 KM4 peripherals wakeup CPU event interrupt               */

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
extern _LONG_CALL_ void irq_table_init(u32 StackP);
extern _LONG_CALL_ BOOL irq_register(IRQ_FUN IrqFun, IRQn_Type IrqNum, u32 Data, u32 Priority);
extern _LONG_CALL_ BOOL irq_unregister(IRQn_Type IrqNum);
extern _LONG_CALL_ void irq_enable(IRQn_Type   IrqNum);
extern _LONG_CALL_ void irq_disable(IRQn_Type   IrqNum);

#define InterruptRegister			irq_register_check
#define InterruptUnRegister		irq_unregister

#define InterruptEn(a,b)			irq_enable(a)
#define InterruptDis(a)			irq_disable(a)
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
extern IRQ_FUN UserIrqFunTable[];
extern u32 UserIrqDataTable[];
extern HAL_VECTOR_FUN  NewVectorTable[];

#if defined (ARM_CORE_CM4)
#define MAX_VECTOR_TABLE_NUM			(64+16)
#define MAX_PERIPHERAL_IRQ_NUM		64 
#define MAX_IRQ_PRIORITY_VALUE			7
#define IRQ_PRIORITY_SHIFT				1
#else
#define MAX_VECTOR_TABLE_NUM			(16+32)
#define MAX_PERIPHERAL_IRQ_NUM		32 
#define MAX_IRQ_PRIORITY_VALUE			3
#define IRQ_PRIORITY_SHIFT				2
#endif

#define MSP_RAM_LP			0x0008FFFC
#define VCT_RAM_LP			0x00080000
#define MSP_RAM_HP			0x1007EFFC
#define MSP_RAM_HP_NS		0x10004FFC

static inline BOOL irq_register_check(IRQ_FUN IrqFun, IRQn_Type IrqNum, u32 Data,  u32 Priority) {
	if(Priority > MAX_IRQ_PRIORITY_VALUE) {
		Priority = MAX_IRQ_PRIORITY_VALUE;
	}
	Priority = (Priority << IRQ_PRIORITY_SHIFT);
	return irq_register(IrqFun, IrqNum, Data, Priority);
}
#endif //_RTL8710B_VECTOR_TABLE_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
