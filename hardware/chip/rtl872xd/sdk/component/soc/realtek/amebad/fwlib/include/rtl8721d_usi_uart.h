/**
  ******************************************************************************
  * @file    rtl8721d_usi_uart.h
  * @author
  * @version V1.0.0
  * @date    2017-09-26
  * @brief   This file contains all the functions prototypes for the USI firmware
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

#ifndef _RTL8721D_USI_UART_H_
#define _RTL8721D_USI_UART_H_

#include "rtl8721d_usi.h"

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup USI-UART
  * @{
  */

/** @addtogroup USI-UART
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * USI0-UART:
  *		- Base Address: USI0_DEV
  *		- IPclk: XTAL, normally is 40MHz
  *		- BaudRate: 110~6000000
  *		- Low Power Rx: Support
  *		- Boot From UART without Flash
  *		- IRQ: USI_IRQ
  *		- GDMA TX handshake interface: GDMA_HANDSHAKE_INTERFACE_USI0_TX
  *		- GDMA RX handshake interface: GDMA_HANDSHAKE_INTERFACE_USI0_RX
  *
  *****************************************************************************************
  * USI UART Low Power Rx
  *****************************************************************************************
  * USI0-UART support
  * UART can receive data when soc enter power save mode
  * baudrate: 110~500000
  *
  *****************************************************************************************     
  * How to use USI Normal Uart
  *****************************************************************************************
  *      To use the normal uart mode, the following steps are mandatory:
  *       
  *      1. Enable peripheral clock and power:
   *
  *      2. configure the USI UART pinmux
  *        
  *      3. Set default parameters, change some parameter if needed
  *			USI_UARTStructInit(USI_UARTInitTypeDef* USI_UARTInitStruct)
  *          
  *      4. init hardware use step3 parameters.
  *			USI_UARTInit(USI_TypeDef* USIx, USI_UARTInitTypeDef *USI_UARTInitStruct)
  *
  *      5. Set Baud Rate.
  *			USI_UARTSetBaud(USI_TypeDef* USIx, u32 BaudRate)
  *
  *      6. Enable IRQ using following function if needed
  *			USI_UARTINTConfig(): USI UART IRQ Mask set
  *			InterruptRegister(): register the uart irq handler 
  *			InterruptEn(): Enable the NVIC interrupt
  *
  *      7. Enable uart rx path:
  *			USI_UARTRxCmd().
  *
  *****************************************************************************************      
  * How to use USI uart in DMA mode
  *****************************************************************************************   
  *      To use the uart in DMA mode, the following steps are mandatory:
  *       
  *      1. Enable peripheral clock and power:
  *
  *      2. configure the USI UART pinmux
  *        
  *      3. Set default parameters, and change DMA mode open USI_UARTInitStruct
  *			USI_UARTStructInit(USI_UARTInitTypeDef* USI_UARTInitStruct)
  *          
  *      4. init hardware use step3 parameters.
  *			USI_UARTInit(USI_TypeDef* USIx, USI_UARTInitTypeDef *USI_UARTInitStruct)
  *
  *      5. Set Baud Rate.
  *			USI_UARTSetBaud(USI_TypeDef* USIx, u32 BaudRate)
  *
  *      6. Enable uart rx path:
  *			USI_UARTRxCmd().
  *
  *      7. Configure the uart DMA burst size:
  *			USI_UARTTXDMAConfig()
  *			USI_UARTRXDMAConfig().
  *
  *      8. Active the UART TX/RX DMA Request:
  *			USI_UARTTXDMACmd()
  *			USI_UARTRXDMACmd().
  *
  *      9. GDMA related configurations(source address/destination address/block size etc.).
  *			USI_UARTTXGDMA_Init()
  *			USI_UARTRXGDMA_Init()
  *    
  *****************************************************************************************    
  * How to use USI uart in Low Power mode
  *****************************************************************************************    
  *      To use the uart in Low Power mode, the following steps are mandatory:
  *       
  *      1. Enable peripheral clock and power:
  *
  *      2. configure the USI UART pinmux
  *        
  *      3. Set default parameters, change some parameter if needed
  *			USI_UARTStructInit(USI_UARTInitTypeDef* USI_UARTInitStruct)
  *          
  *      4. init hardware use step3 parameters.
  *			USI_UARTInit(USI_TypeDef* USIx, USI_UARTInitTypeDef *USI_UARTInitStruct)
  *
  *      5. Set Baud Rate.
  *			USI_UARTSetBaud(USI_TypeDef* USIx, u32 BaudRate)
  *
  *      6. Enable IRQ using following function if needed
  *			USI_UARTINTConfig(): USI UART IRQ Mask set
  *			InterruptRegister(): register the uart irq handler 
  *			InterruptEn(): Enable the NVIC interrupt
  *
  *      6. Init Low power RX:
  *			USI_UARTLPRxStructInit(USI_LPUARTInitTypeDef* USI_LPUARTInitStruct)
  *			USI_UARTLPRxInit(USI_TypeDef* USIx, USI_LPUARTInitTypeDef *USI_LPUARTInitTypeDef)
  *
  *      7. Set the low power RX Baud Rate
  *			USI_UARTLPRxBaudSet(USI_TypeDef* USIx, u32 BaudRate, u32 RxIPClockHz)
  *
  *      8. Enable monitor function if needed.
  *			USI_UARTLPRxMonitorCmd()
  *
  *      9. Enable low power rx path:
  *			USI_UARTLPRxCmd().
  *
  *      @note     when uart work in low power rx mode, clock source can switch between 
  *          XTAL and OSC. As for how and when to excute switching action, 
  *          refer to related uart specifications for more details.
  *
  *      @note     Besides, if more details about the uart low power rx path contens  is needed, 
  *          please refer to uart specifications.
  *
  *****************************************************************************************     
  * How to use USI uart in IrDA mode
  *****************************************************************************************  
  *      To use the uart in IrDA mode, the following steps are mandatory:
  *       
  *      1. Enable peripheral clock and power:
  *
  *      2. configure the pinmux:
  *
  *      3. Disable rx path:
  *			USI_UARTRxCmd().
  *
  *      4. Program the IrDA tx pulse width and location and IrDA rx pulse filter: 
  *			USI_UARTIrDAStructInit(USI_UartIrDAInitTypeDef * IrDA_InitStruct)
  *
  *      5. Init Hardware: 
  *			USI_UARTIrDAInit(USI_TypeDef* USIx, USI_UartIrDAInitTypeDef * IrDA_InitStruct).  
  *
  *      6. Enable the IrDA function:
  *			USI_UARTIrDACmd().
  *
  *      7. According to the IrDA SIR protocol data format requrement, program Word Length,
  *          Stop Bit, Parity and DMA Mode(ENABLE/DISABLE):
  *			USI_UARTStructInit(USI_UARTInitTypeDef* USI_UARTInitStruct)
  *			USI_UARTInit(USI_TypeDef* USIx, USI_UARTInitTypeDef *USI_UARTInitStruct)
  *          
  *      8. Program the Baud Rate:
  *			USI_UARTSetBaud().
  *
  *      9. Enable IRQ if needed:
  *			USI_UARTINTConfig(): USI UART IRQ Mask set
  *			InterruptRegister(): register the uart irq handler 
  *			InterruptEn(): Enable the NVIC interrupt
  *
  *      10. Enable uart rx path:
  *			USI_UARTRxCmd().
  *
  *      @note  AmebaD IrDA just support IrDA SIR protocol, setting baud rate is no more than
  *          115200 bps.
  *      
  *      @note  because IrDA transfers data using infrared carrier and for the property of the
  *          IrDA transceiver, IrDA just work in half duplex mode. For details, refer to the IrDA
  *          protocol specification.
  *****************************************************************************************     
  * @endverbatim
  */

/* Exported Types --------------------------------------------------------*/

/** @defgroup USI_UART_Exported_Types USI-UART Exported Types
  * @{
  */

/** 
  * @brief  USI_UART Init structure definition
  */
typedef struct
{
	u32	USI_UARTDmaModeCtrl; /*!< Specifies the uart DMA mode state.
	                             This parameter can be ENABLE or DISABLE. */
	                             
	u32	USI_UARTWordLen;       /*!< Specifies the USI UART word length.
                                      This parameter can be a value of @ref USI_UART_Word_length_define. */
                                   
	u32	USI_UARTStopBit;         /*!< Specifies the USI UART stop bit number.
                                      This parameter can be a value of @ref USI_UART_Stop_Bit_define. */
                                      
	u32	USI_UARTParity;           /*!< Specifies the USI UART parity.
                                      This parameter can be a value of @ref USI_UART_Parity_Enable_define. */

	u32	USI_UARTParityType;    /*!< Specifies the USI UART parity type.
                                      This parameter can be a value of @ref USI_UART_Parity_Type_define. */
                                      
	u32	USI_UARTStickParity;    /*!< Specifies the USI UART stick parity.
                                      This parameter can be a value of @ref USI_UART_Stick_Parity_Type_define. */

	u32	USI_UARTFlowControl;  /*!< Specifies the USI UART auto flow control.
                                      This parameter can be ENABLE or DISABLE. */

	u32 	USI_UARTFlwCtrlRxHoldThd;	/*!< Specifies the USI UART uart auto flow control rx hold threshold.
                                         This parameter can be a value of  0 ~ 64. */	
									  
	u32	USI_UARTRxFifoTrigLevel;   /*!< Specifies the USI UART rx fifo trigger level.
                                         This parameter can be a value of 0 ~ 64 . */

	u32	USI_UARTTxFifoTrigLevel;   /*!< Specifies the USI UART rx error report control.
                                         This parameter can be a value of 0 ~ 64 . */
} USI_UARTInitTypeDef;

/**
 * @brief USI UART Low Power Init structure definition
 */
typedef struct
{
	u32 USI_LPUARTOscPerbitUpdCtrl;    /*!< Specifies the OSC perbit update control when use xtal 8M.
                                                         This parameter can be ENABLE or DISABLE. 
                                                         ENABLE: osc perbit updates with xtal perbit when use xtal 8M.
                                                         DISABLE: osc perbit does't update with xtal perbit  when use xtal 8M.
                                                        @note This parameter is only used in low power rx path with xtal 8M.
                                                        @note osc perbit will update when use osc 8M, even if USI_LPUARTOscPerbitUpdCtrl is disable */
	u32 USI_LPUARTBitNumThres;         /*!< Specifies the bit number threshold of one monitor period.
	                                                 This parameter is used to get the average clock cycles of one bit
	                                                 and can be a number between 0x00 and 0x7f. 
                                                        @note This parameter is only used in low power rx path. */
} USI_LPUARTInitTypeDef;

/**
 * @brief USI UART IRDA Init structure definition
 */
typedef struct
{
	u32 USI_UARTIrDARxInv;	    /*!< Specifies the uart irda rx invert control.
                                              This parameter can be ENABLE or DISABLE. 
                                              ENABLE: invert the irda input signal.
                                              DISABLE: does't invert the irda input signal.
                                              @note This parameter is only used in IrDA mode. */
                                              
	u32 USI_UARTIrDATxInv;		  /*!< Specifies the uart irda tx invert control.
                                              This parameter can be ENABLE or DISABLE. 
                                              ENABLE: invert the irda output signal.
                                              DISABLE: does't invert the irda output signal.
                                              @note This parameter is only used in IrDA mode. */
                                              
	u32 USI_UARTUpperShift;		/*!< Specifies the USI uart irda tx pulse right edge shift direction.
                                               This parameter can be a value of @ref USI_UART_IRDA_PULSE_SHIFT_define. */

	u32 USI_UARTUpperShiftVal;   /*!< Specifies the USI uart irda tx pulse right edge shift value in the given direction.
                                                This parameter can be a number between 0x0000 and 0x7fff. */
 
	u32 USI_UARTLowShift;		/*!< Specifies the USI uart irda tx pulse left edge shift direction.
                                              This parameter can be a value of @ref USI_UART_IRDA_PULSE_SHIFT_define. */
                                             
	u32 USI_UARTLowShiftVal;	/*!< Specifies the USI uart irda tx pulse left edge shift value in the given direction.
                                              This parameter can be a number between 0x0000 and 0x7fff. */
                                             
	u32 USI_UARTRxFilterThres;  /*!< Specifies the USI uart irda rx filter threshold.
                                              This parameter can be a number between 0x0000 and 0x7fff
                                              @note This parameter is only used in IrDA mode. */
             
	u32 USI_UARTRxFilterCmd;	/*!< Specifies the USI uart irda rx filter control.
                                              This parameter can be ENABLE or DISABLE. 
                                              ENABLE: USI uart IrDA rx filter is used.
                                              DISABLE: USI uart IrDA rx filter is not used.
                                              @note This parameter is only used in IrDA mode. */
}USI_UartIrDAInitTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup USI_UART_Exported_Constants USI-UART Exported Constants
  * @{
  */

/** @defgroup USI_UART_IRDA_PULSE_SHIFT_define
  * @{
  */
#define USI_UART_IRDA_PULSE_LEFT_SHIFT			((u32)0x00000000)
#define USI_UART_IRDA_PULSE_RIGHT_SHIFT		((u32)0x00000001)
#define IS_USI_IRDA_PUL_SHIFT(SHIFT) (((SHIFT) == USI_UART_IRDA_PULSE_LEFT_SHIFT) || \
									((SHIFT) == USI_UART_IRDA_PULSE_RIGHT_SHIFT))

/**
  * @}
  */

/** @defgroup USI_UART_Word_length_define 
  * @{
  */
  
#define USI_RUART_WLS_7BITS        ((u32)0x00000000)
#define USI_RUART_WLS_8BITS        ((u32)0x00000001)

#define IS_USI_UART_WLS(VAL) (((VAL) == USI_RUART_WLS_7BITS) || \
								((VAL) == USI_RUART_WLS_8BITS))

/**
  * @}
  */

/** @defgroup USI_UART_Stop_Bit_define 
  * @{
  */
  
#define USI_RUART_STOP_BIT_1        ((u32)0x00000000)
#define USI_RUART_STOP_BIT_2        ((u32)0x00000002)

#define IS_USI_UART_STOP_BIT(VAL) (((VAL) == USI_RUART_STOP_BIT_1) || \
								((VAL) == USI_RUART_STOP_BIT_2))

/**
  * @}
  */  

/** @defgroup USI_UART_Parity_Enable_define 
  * @{
  */
  
#define USI_RUART_PARITY_DISABLE	((u32)0x00000000)
#define USI_RUART_PARITY_ENABLE		((u32)0x00000010)

#define IS_USI_UART_PARITY_ENABLE(VAL) (((VAL) == USI_RUART_PARITY_DISABLE) || \
								((VAL) == USI_RUART_PARITY_ENABLE))

/**
  * @}
  */

/** @defgroup USI_UART_Parity_Type_define
  * @{
  */
  
#define USI_RUART_ODD_PARITY		((u32)0x00000000)
#define USI_RUART_EVEN_PARITY		((u32)0x00000020)

#define IS_USI_UART_PARITY_TYPE(VAL) (((VAL) == USI_RUART_ODD_PARITY) || \
								((VAL) == USI_RUART_EVEN_PARITY))

/**
  * @}
  */

/** @defgroup USI_UART_Stick_Parity_Type_define
  * @{
  */
  
#define USI_RUART_STICK_PARITY_DISABLE		((u32)0x00000000)
#define USI_RUART_STICK_PARITY_ENABLE		((u32)0x00000040)

#define IS_USI_UART_STICK_PARITY_ENABLE(VAL) (((VAL) == USI_RUART_STICK_PARITY_DISABLE) || \
								((VAL) == USI_RUART_STICK_PARITY_ENABLE))

/**
  * @}
  */

/** @defgroup USI_UART_Interrupt_ID_define
  * @{
  */
/*TX FIFO (UART, I2C, SPI)*/
#define USI_TX_FIFO_ALMOST_EMPTY_INTER		((u32)0x00000001)
#define USI_TX_FIFO_OVERFLOW_INTER			((u32)0x00000002)
#define USI_TX_FIFO_UNDERFLOW_INTER		((u32)0x00000004) /*USI UART do not have this type interrupt*/

/*RX FIFO (UART, I2C, SPI)*/
#define USI_RX_FIFO_ALMOST_FULL_INTER		((u32)0x00000010)
#define USI_RX_FIFO_OVERFLOW_INTER			((u32)0x00000020)
#define USI_RX_FIFO_UNDERFLOW_INTER		((u32)0x00000040)

/*UART related interrupt*/
#define USI_UART_PARITY_ERROR_INTER		((u32)0x00000100)
#define USI_UART_STOP_ERROR_INTER			((u32)0x00000200)
#define USI_UART_BREAK_INTER				((u32)0x00000400)
#define USI_RX_FIFO_TIMEOUT_INTER			((u32)0x00000800)
#define USI_RX_BAUDMON_DONE_INTER			((u32)0x00001000)
#define USI_TX_CTS_CHANGE_INTER				((u32)0x00002000)


/*JUST FOR USI UART USE*/
#define IS_USI_UART_GET_IT(IT) (((IT) == USI_TX_FIFO_ALMOST_EMPTY_INTER) || \
								((IT) == USI_TX_FIFO_OVERFLOW_INTER) || \
									((IT) == USI_RX_FIFO_ALMOST_FULL_INTER)|| \
									((IT) == USI_RX_FIFO_OVERFLOW_INTER)|| \
									((IT) == USI_RX_FIFO_UNDERFLOW_INTER)|| \
									((IT) == USI_UART_PARITY_ERROR_INTER)|| \
									((IT) == USI_UART_STOP_ERROR_INTER)|| \
									((IT) == USI_UART_BREAK_INTER)|| \
									((IT) == USI_RX_FIFO_TIMEOUT_INTER)|| \
									((IT) == USI_RX_BAUDMON_DONE_INTER)|| \
									((IT) == USI_TX_CTS_CHANGE_INTER))
									
#define IS_USI_UART_IT(IT) ((((IT) & (u32)0xFFFFC08C) == 0x00) && ((IT) != 0x00))

#define IS_USI_UART_CLEAR_IT(IT) ((((IT) & (u32)0xFFFFC89D) == 0x00) && ((IT) != 0x00))

/**
  * @}
  */

/** @defgroup USI_UART_Rx_DMA_mode_define 
  * @{
  */
  
#define USI_UART_RX_UART_IS_DMA_FLOW_CTRL			((u32)0x00000001)
#define USI_UART_RX_GDMA_IS_DMA_FLOW_CTRL		((u32)0x00000000)

#define IS_USI_UART_RX_DMA_MODE(MODE)  (((MODE) == USI_UART_RX_UART_IS_DMA_FLOW_CTRL) || \
							((MODE) == USI_UART_RX_GDMA_IS_DMA_FLOW_CTRL) )

/**
  * @}
  */

/** @defgroup USI_Peripheral_define 
  * @{
  */
#define IS_ALL_USI_PERIPH(PERIPH) (((PERIPH) == USI0_DEV))
#define IS_ALL_USI_LP_PERIPH(PERIPH) (((PERIPH) == USI0_DEV))
/**
  * @}
  */ 

/** @defgroup USI_UART_SoftWare_Status_define
  * @{
  */
#define USI_UART_STATETX_DMA			1
#define USI_UART_STATETX_INT				2
#define USI_UART_STATETX_POLL			3
#define USI_UART_STATERX_DMA			1
#define USI_UART_STATERX_INT				2
#define USI_UART_STATERX_POLL			3
/**
  * @}
  */


/**
  * @}
  */ 


/* Exported functions --------------------------------------------------------*/
/** @defgroup USI_UART_Exported_Functions USI-UART Exported Functions
  * @{
  */
/** @defgroup USI_UART_Normal_functions 
  * @{
  */
_LONG_CALL_ void USI_UARTDeInit(USI_TypeDef* USIx);
_LONG_CALL_ void USI_UARTStructInit(USI_UARTInitTypeDef* USI_UARTInitStruct);
_LONG_CALL_ void USI_UARTInit(USI_TypeDef* USIx, USI_UARTInitTypeDef* USI_UARTInitStruct);
_LONG_CALL_ u32 USI_UARTBaudParaGet(u32 baudrate, u32 *ovsr, u32 *ovsr_adj);
_LONG_CALL_ void USI_UARTBaudParaGetFull(u32 IPclk, u32 baudrate, u32 *ovsr, u32 *ovsr_adj);
_LONG_CALL_ void USI_UARTSetBaudExt(USI_TypeDef* USIx, u32 Ovsr, u32 Ovsr_adj);
_LONG_CALL_ void USI_UARTSetBaud(USI_TypeDef* USIx, u32 BaudRate);
_LONG_CALL_ void USI_UARTSetRxLevel(USI_TypeDef* USIx, u32 FifoLv);
_LONG_CALL_ void USI_UARTRxCmd(USI_TypeDef* USIx, u32 NewState);
_LONG_CALL_ u32 USI_UARTWritable(USI_TypeDef* USIx);
_LONG_CALL_ u32 USI_UARTReadable(USI_TypeDef* USIx);
_LONG_CALL_ void USI_UARTCharPut(USI_TypeDef* USIx, u8 TxData);
_LONG_CALL_ void USI_UARTCharGet(USI_TypeDef* USIx, u8  *pRxByte);
_LONG_CALL_ void USI_UARTReceiveData(USI_TypeDef* USIx, u8* OutBuf, u32 Count);
_LONG_CALL_ void USI_UARTSendData(USI_TypeDef* USIx, u8* InBuf, u32 Count);
_LONG_CALL_ u32 USI_UARTReceiveDataTO(USI_TypeDef* USIx, u8* OutBuf, u32 Count, u32 Times);
_LONG_CALL_ u32 USI_UARTSendDataTO(USI_TypeDef* USIx,u8* InBuf,u32 Count, u32 Times);
_LONG_CALL_ void USI_UARTRxByteCntClear(USI_TypeDef* USIx);
_LONG_CALL_ u32 USI_UARTRxByteCntGet(USI_TypeDef* USIx);
_LONG_CALL_ void USI_UARTBreakCtl(USI_TypeDef* USIx, u32 NewState);
_LONG_CALL_ u32 USI_UARTClearRxFifo(USI_TypeDef* USIx);
_LONG_CALL_ void USI_UARTClearTxFifo(USI_TypeDef* USIx);
_LONG_CALL_ u32 USI_UARTGetRxFifoValidCnt(USI_TypeDef* USIx);
_LONG_CALL_ u32 USI_UARTGetTxFifoEmptyCnt(USI_TypeDef* USIx);
_LONG_CALL_ void USI_UARTINTConfig(USI_TypeDef* USIx, u32 UART_IT, u32 newState);
_LONG_CALL_ u32 USI_UARTIntStatus(USI_TypeDef* USIx);
_LONG_CALL_ u32 USI_UARTGetRawIntStatus(USI_TypeDef* USIx);
_LONG_CALL_ void USI_UARTClearAllIntStatus(USI_TypeDef* USIx);
_LONG_CALL_ void USI_UARTClearIntStatus(USI_TypeDef* USIx, u32 USIUART_IT);
_LONG_CALL_ void USI_UARTWaitBusy(USI_TypeDef* USIx, u32 PollTimes);
_LONG_CALL_ void USI_UARTRxTimeOutConfig(USI_TypeDef* USIx, u32 TimeOutCnt);
_LONG_CALL_ void USI_UARTRxDMAModeConfig(USI_TypeDef* USIx, u32 Mode);
_LONG_CALL_ void USI_UARTRxDMADummyDataConfig(USI_TypeDef* USIx, u8 Byte);
_LONG_CALL_ u32 USI_UARTGetRxDMADummyFlag(USI_TypeDef* USIx);
_LONG_CALL_ void USI_UARTRxClearDMADummyFlag(USI_TypeDef* USIx);

/**
  * @}
  */ 

/** @defgroup USI_UART_DMA_functions 
  * @{
  */
_LONG_CALL_ void USI_UARTTXDMAConfig(USI_TypeDef* USIx, u32 TxDmaBurstSize);
_LONG_CALL_ void USI_UARTRXDMAConfig(USI_TypeDef* USIx, u32 RxDmaBurstSize);
_LONG_CALL_ void USI_UARTTXDMACmd(USI_TypeDef* USIx, u32 NewState);
_LONG_CALL_ void USI_UARTRXDMACmd(USI_TypeDef* USIx, u32 NewState);
_LONG_CALL_ BOOL USI_UARTTXGDMA_Init(u8 USIIndex, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pTxBuf, int TxCount); 
_LONG_CALL_ BOOL USI_UARTRXGDMA_Init(u8 USIIndex, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pRxBuf, int RxCount);
/**
  * @}
  */ 

/** @defgroup USI_UART_Low_Power_functions 
  * @{
  */
_LONG_CALL_ void USI_UARTLPRxStructInit(USI_LPUARTInitTypeDef* USI_UARTInitStruct);
_LONG_CALL_ void USI_UARTLPRxInit(USI_TypeDef* USIx, USI_LPUARTInitTypeDef *USI_UARTInitStruct);
_LONG_CALL_ void USI_UARTLPRxBaudSet(USI_TypeDef* USIx, u32 BaudRate, u32 RxIPClockHz);
_LONG_CALL_	void USI_UART_LPRxMonitorCmd(USI_TypeDef* USIx, u32 NewState);
_LONG_CALL_ void USI_UARTLPRxpathSet(USI_TypeDef* USIx, u32 LPRxpath);
_LONG_CALL_ void USI_UARTLPRxIPClockSet(USI_TypeDef* USIx, u32 RxIPClock);
_LONG_CALL_ void USI_UARTLPRxCmd(USI_TypeDef* USIx, u32 NewState);
/**
  * @}
  */ 

/** @defgroup USI_UART_IRDA_functions 
  * @{
  */
_LONG_CALL_ void USI_UARTIrDAStructInit(USI_UartIrDAInitTypeDef * IrDA_InitStruct);
_LONG_CALL_ void USI_UARTIrDAInit(USI_TypeDef* USIx, USI_UartIrDAInitTypeDef * IrDA_InitStruct);
_LONG_CALL_ void USI_UARTIrDACmd(USI_TypeDef* USIx, u32 NewState);
/** @} */
/** @} */

/**
  * @}
  */

/**
  * @}
  */ 

/* Other Definitions --------------------------------------------------------*/

/*----------------USI UART definations---------------*/
typedef struct
{		
	u32 LOW_POWER_RX_ENABLE;	/*Enable low power RX*/
} USI_UARTCFG_TypeDef;

extern USI_UARTCFG_TypeDef usi_uart_config[];
extern u32 USI_UART_StateTx[1];
extern u32 USI_UART_StateRx[1];

static inline void
USI_UART_SetTxFlag(u32 USIIdx, u32 Flag)
{
	USI_UART_StateTx[USIIdx] = Flag;
}

static inline void
USI_UART_SetRxFlag(u32 USIIdx, u32 Flag)
{	
	USI_UART_StateRx[USIIdx] = Flag;
}

static inline u32
USI_UART_GetTxFlag(u32 USIIdx)
{
	return (USI_UART_StateTx[USIIdx]);
}

static inline u32
USI_UART_GetRxFlag(u32 USIIdx)
{
	return (USI_UART_StateRx[USIIdx]);
}

#endif
/******************* (C) COPYRIGHT 2017 Realtek Semiconductor *****END OF FILE****/
