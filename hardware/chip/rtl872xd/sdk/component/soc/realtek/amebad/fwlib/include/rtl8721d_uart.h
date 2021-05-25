/**
  ******************************************************************************
  * @file    rtl8721d_uart.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the UART firmware
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

#ifndef _RTL8710B_UART_H_
#define _RTL8710B_UART_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup UART
  * @{
  */

/** @addtogroup UART
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * KM4 UART0:
  *		- Base Address: UART0_DEV
  *		- IPclk: XTAL, normally is 40MHz
  *		- BaudRate: 110~6000000
  *		- Low Power Rx: Support
  *		- SocPs: SleepMode (clock gating & power gating)
  *		- Boot From UART without Flash
  *		- IRQ: UART0_IRQ
  *		- GDMA TX handshake interface: GDMA_HANDSHAKE_INTERFACE_UART0_TX
  *		- GDMA RX handshake interface: GDMA_HANDSHAKE_INTERFACE_UART0_RX
  *
  * KM4 UART1_BT: The same as KM4 UART0 except following
  *		- Base Address: UART1_DEV
  *		- IRQ: UART1_IRQ
  *		- GDMA TX handshake interface: GDMA_HANDSHAKE_INTERFACE_UART1_TX
  *		- GDMA RX handshake interface: GDMA_HANDSHAKE_INTERFACE_UART1_RX
  *
  * KM0 LUART: The same as KM4 UART0 except following
  *		- Base Address: UART3_DEV
  *		- IRQ: UARTLP_IRQ
  *		- GDMA TX handshake interface: GDMA_HANDSHAKE_INTERFACE_UART3_TX
  *		- GDMA RX handshake interface: GDMA_HANDSHAKE_INTERFACE_UART3_RX
  *
  * KM0 LOG UART: Used as loguart
  *		- Base Address: UART2_DEV
  *		- IPclk: XTAL, normally is 40MHz
  *		- BaudRate: 110~6000000, default set 115200 for log
  *		- Low Power Rx: Support
  *		- Flash Program use LOGUART
  *		- IRQ: UART_LOG_IRQ
  *
  *****************************************************************************************
  * Low Power Rx
  *****************************************************************************************
  * All UART support
  * UART can receive data when soc enter power save mode
  * baudrate: 110~115200
  *
  *****************************************************************************************     
  * How to use Normal Uart
  *****************************************************************************************
  *      To use the normal uart mode, the following steps are mandatory:
  *       
  *      1. Enable peripheral clock and power(it is enable default):
  *			RCC_PeriphClockCmd(APBPeriph_UARTx, APBPeriph_UARTx_CLOCK, ENABLE);
  *
  *      2. configure the UART pinmux
  *			Pinmux_Config(Pin_Num, PINMUX_FUNCTION_UART)
  *        
  *      3. Set default parameters, change some parameter if needed
  *			UART_StructInit(UART_InitTypeDef* UART_InitStruct)
  *          
  *      4. init hardware use step3 parameters.
  *			UART_Init(UART_TypeDef* UARTx, UART_InitTypeDef *UART_InitStruct)
  *
  *      5. Set Baud Rate.
  *			UART_SetBaud(UART_TypeDef* UARTx, u32 BaudRate)
  *
  *      6. Enable IRQ using following function if needed
  *			UART_INTConfig(): UART IRQ Mask set
  *			InterruptRegister(): register the uart irq handler 
  *			InterruptEn(): Enable the NVIC interrupt
  *
  *      7. Enable uart rx path:
  *			UART_RxCmd().
  *  
  *      @note in UART_Normal_functions group, these functions below are about Interrupts 
  *          and flags management.
  *			UART_INTConfig()
  *			UART_IntStatus()
  *			UART_LineStatusGet()
  *
  *****************************************************************************************      
  * How to use uart in DMA mode
  *****************************************************************************************   
  *      To use the uart in DMA mode, the following steps are mandatory:
  *       
  *      1. Enable peripheral clock and power(it is enable default):
  *			RCC_PeriphClockCmd(APBPeriph_UARTx, APBPeriph_UARTx_CLOCK, ENABLE);
  *
  *      2. configure the UART pinmux
  *			Pinmux_Config(Pin_Num, PINMUX_FUNCTION_UART)
  *        
  *      3. Set default parameters, and change DMA mode open UART_InitStruct
  *			UART_StructInit(UART_InitTypeDef* UART_InitStruct)
  *          
  *      4. init hardware use step3 parameters.
  *			UART_Init(UART_TypeDef* UARTx, UART_InitTypeDef *UART_InitStruct)
  *
  *      5. Set Baud Rate.
  *			UART_SetBaud(UART_TypeDef* UARTx, u32 BaudRate)
  *
  *      6. Enable uart rx path:
  *			UART_RxCmd().
  *
  *      7. Configure the uart DMA burst size:
  *			UART_TXDMAConfig()
  *			UART_RXDMAConfig().
  *
  *      8. Active the UART TX/RX DMA Request:
  *			UART_TXDMACmd()
  *			UART_RXDMACmd().
  *
  *      9. GDMA related configurations(source address/destination address/block size etc.).
  *			UART_TXGDMA_Init()
  *			UART_RXGDMA_Init()
  *
  *****************************************************************************************    
  * How to use uart in Low Power mode
  *****************************************************************************************    
  *      To use the uart in Low Power mode, the following steps are mandatory:
  *       
  *      1. Enable peripheral clock and power(it is enable default):
  *			RCC_PeriphClockCmd(APBPeriph_UARTx, APBPeriph_UARTx_CLOCK, ENABLE);
  *
  *      2. configure the UART pinmux
  *			Pinmux_Config(Pin_Num, PINMUX_FUNCTION_UART)
  *        
  *      3. Set default parameters, change some parameter if needed
  *			UART_StructInit(UART_InitTypeDef* UART_InitStruct)
  *          
  *      4. init hardware use step3 parameters.
  *			UART_Init(UART_TypeDef* UARTx, UART_InitTypeDef *UART_InitStruct)
  *
  *      5. Set Baud Rate.
  *			UART_SetBaud(UART_TypeDef* UARTx, u32 BaudRate)
  *
  *      6. Enable IRQ using following function if needed
  *			UART_INTConfig(): UART IRQ Mask set
  *			InterruptRegister(): register the uart irq handler 
  *			InterruptEn(): Enable the NVIC interrupt
  *
  *      7. Configure monitor parameters:
  *			UART_MonitorParaConfig();
  *
  *      8. Enable monitor function if needed.
  *			UART_RxMonitorCmd()
  *
  *      9. Set lower power clock source
  *			RCC_PeriphClockSource_UART (UART_TypeDef* UARTx, u32 Source)
  *
  *      10. Set the low power RX Baud Rate
  *			UART_LPRxBaudSet(UART_TypeDef* UARTx, u32 BaudRate, u32 RxIPClockHz)
  *
  *      11. Enable uart rx path:
  *			UART_RxCmd().
  *
  *      @note     when uart work in low power rx mode, clock source can switch between 
  *          XTAL and OSC. As for how and when to excute switching action, 
  *          refer to related uart specifications for more details.
  *
  *      @note     Besides, if more details about the uart low power rx path contens  is needed, 
  *          please refer to uart specifications.
  *
  *****************************************************************************************     
  * How to use uart in IrDA mode
  *****************************************************************************************  
  *      To use the uart in IrDA mode, the following steps are mandatory:
  *       
  *      1. Enable peripheral clock and power(it is enable default):
  *			RCC_PeriphClockCmd(APBPeriph_UARTx, APBPeriph_UARTx_CLOCK, ENABLE);
  *
  *      2. configure the pinmux:
  *			Pinmux_Config(Pin_Num, PINMUX_FUNCTION_UART)
  *
  *      3. Disable rx path:
  *			UART_RxCmd().
  *
  *      4. Program the IrDA tx pulse width and location and IrDA rx pulse filter: 
  *			UART_IrDAStructInit(IrDA_InitTypeDef * IrDA_InitStruct)
  *
  *      5. Init Hardware: 
  *			UART_IrDAInit(UART_TypeDef* UARTx, IrDA_InitTypeDef * IrDA_InitStruct).  
  *
  *      6. Enable the IrDA function:
  *			UART_IrDACmd().
  *
  *      7. According to the IrDA SIR protocol data format requrement, program Word Length,
  *          Stop Bit, Parity and DMA Mode(ENABLE/DISABLE):
  *			UART_StructInit(UART_InitTypeDef* UART_InitStruct)
  *			UART_Init(UART_TypeDef* UARTx, UART_InitTypeDef *UART_InitStruct)
  *          
  *      8. Program the Baud Rate:
  *			UART_SetBaud().
  *
  *      9. Enable IRQ if needed:
  *			UART_INTConfig(): UART IRQ Mask set
  *			InterruptRegister(): register the uart irq handler 
  *			InterruptEn(): Enable the NVIC interrupt
  *
  *      10. Enable uart rx path:
  *			UART_RxCmd().
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

/** @defgroup UART_Exported_Types UART Exported Types
  * @{
  */

/** 
  * @brief  UART Init structure definition
  */
typedef struct
{
	u32	DmaModeCtrl; /*!< Specifies the uart DMA mode state.
	                             This parameter can be ENABLE or DISABLE. */
	                             
	u32	WordLen;       /*!< Specifies the UART word length.
                                      This parameter can be a value of @ref UART_Word_length_define. */
                                   
	u32	StopBit;         /*!< Specifies the UART stop bit number.
                                      This parameter can be a value of @ref UART_Stop_Bit_define. */
                                      
	u32	Parity;           /*!< Specifies the UART parity.
                                      This parameter can be a value of @ref UART_Parity_Enable_define. */

	u32	ParityType;    /*!< Specifies the UART parity type.
                                      This parameter can be a value of @ref UART_Parity_Type_define. */
                                      
	u32	StickParity;    /*!< Specifies the UART stick parity.
                                      This parameter can be a value of @ref UART_Stick_Parity_Type_define. */

	u32	FlowControl;  /*!< Specifies the UART auto flow control.
                                      This parameter can be ENABLE or DISABLE. */
                                      
	u32	RxFifoTrigLevel;   /*!< Specifies the UART rx fifo trigger level.
                                         This parameter can be a value of @ref UART_RX_FIFO_TRIGGER_LEVEL_define. */
                                         
	u32	RxErReportCtrl;   /*!< Specifies the UART rx error report control.
                                         This parameter can be a value of @ref UART_Rx_Err_Report_define. */

	u32 RxTimeOutCnt;    /*!< Specifies the UART rx time out counter.
                                         This parameter can be a number between 0x00 and 0xffff.. */								 
} UART_InitTypeDef;

/**
 * @brief UART IRDA Init structure definition
 *
 */
typedef struct
{
	u32 UART_IrDARxInv;	    /*!< Specifies the uart irda rx invert control.
                                              This parameter can be ENABLE or DISABLE. 
                                              ENABLE: invert the irda input signal.
                                              DISABLE: does't invert the irda input signal.
                                              @note This parameter is only used in IrDA mode. */
                                              
	u32 UART_IrDATxInv;		  /*!< Specifies the uart irda tx invert control.
                                              This parameter can be ENABLE or DISABLE. 
                                              ENABLE: invert the irda output signal.
                                              DISABLE: does't invert the irda output signal.
                                              @note This parameter is only used in IrDA mode. */
                                              
	u32 UART_UpperShift;		/*!< Specifies the uart irda tx pulse right edge shift direction.
                                               This parameter can be a value of @ref UART_IRDA_PULSE_SHIFT_define. */

	u32 UART_UpperShiftVal;   /*!< Specifies the uart irda tx pulse right edge shift value in the given direction.
                                                This parameter can be a number between 0x0000 and 0x7fff. */
 
	u32 UART_LowShift;		/*!< Specifies the uart irda tx pulse left edge shift direction.
                                              This parameter can be a value of @ref UART_IRDA_PULSE_SHIFT_define. */
                                             
	u32 UART_LowShiftVal;	/*!< Specifies the uart irda tx pulse left edge shift value in the given direction.
                                              This parameter can be a number between 0x0000 and 0x7fff. */
                                             
	u32 UART_RxFilterThres;  /*!< Specifies the uart irda rx filter threshold.
                                              This parameter can be a number between 0x0000 and 0x7fff
                                              @note This parameter is only used in IrDA mode. */
                                             
	u32 UART_RxFilterCmd;	/*!< Specifies the uart irda rx filter control.
                                              This parameter can be ENABLE or DISABLE. 
                                              ENABLE: uart IrDA rx filter is used.
                                              DISABLE: uart IrDA rx filter is not used.
                                              @note This parameter is only used in IrDA mode. */
}IrDA_InitTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup UART_Exported_Constants UART Exported Constants
  * @{
  */

/** @defgroup UART_IRDA_PULSE_SHIFT_define
  * @{
  */
#define UART_IRDA_PULSE_LEFT_SHIFT		((u32)0x00000000)
#define UART_IRDA_PULSE_RIGHT_SHIFT		((u32)0x00000001)
#define IS_IRDA_PUL_SHIFT(SHIFT) (((SHIFT) == UART_IRDA_PULSE_LEFT_SHIFT) || \
									((SHIFT) == UART_IRDA_PULSE_RIGHT_SHIFT))

/**
  * @}
  */

/** @defgroup UART_RX_FIFO_TRIGGER_LEVEL_define
  * @{
  */
  
#define UART_RX_FIFOTRIG_LEVEL_1BYTES        ((u32)0x00000000)
#define UART_RX_FIFOTRIG_LEVEL_4BYTES        ((u32)0x00000040)
#define UART_RX_FIFOTRIG_LEVEL_8BYTES        ((u32)0x00000080)
#define UART_RX_FIFOTRIG_LEVEL_14BYTES      ((u32)0x000000c0)

#define IS_UART_RXFIFO_LEVEL(LEVEL) (((LEVEL) == UART_RX_FIFOTRIG_LEVEL_1BYTES) || \
								((LEVEL) == UART_RX_FIFOTRIG_LEVEL_4BYTES) || \
								((LEVEL) == UART_RX_FIFOTRIG_LEVEL_8BYTES) || \
								((LEVEL) == UART_RX_FIFOTRIG_LEVEL_14BYTES))

/**
  * @}
  */

/** @defgroup UART_Word_length_define 
  * @{
  */
  
#define RUART_WLS_7BITS        ((u32)0x00000000)
#define RUART_WLS_8BITS        ((u32)0x00000001)

#define IS_UART_WLS(VAL) (((VAL) == RUART_WLS_7BITS) || \
								((VAL) == RUART_WLS_8BITS))

/**
  * @}
  */

/** @defgroup UART_Stop_Bit_define 
  * @{
  */
  
#define RUART_STOP_BIT_1        ((u32)0x00000000)
#define RUART_STOP_BIT_2        ((u32)0x00000001)

#define IS_UART_STOP_BIT(VAL) (((VAL) == RUART_STOP_BIT_1) || \
								((VAL) == RUART_STOP_BIT_2))

/**
  * @}
  */  

/** @defgroup UART_Parity_Enable_define 
  * @{
  */
  
#define RUART_PARITY_DISABLE		((u32)0x00000000)
#define RUART_PARITY_ENABLE		((u32)0x00000001)

#define IS_UART_PARITY_ENABLE(VAL) (((VAL) == RUART_PARITY_DISABLE) || \
								((VAL) == RUART_PARITY_ENABLE))

/**
  * @}
  */

/** @defgroup UART_Parity_Type_define
  * @{
  */
  
#define RUART_ODD_PARITY		((u32)0x00000000)
#define RUART_EVEN_PARITY		((u32)0x00000001)

#define IS_UART_PARITY_TYPE(VAL) (((VAL) == RUART_ODD_PARITY) || \
								((VAL) == RUART_EVEN_PARITY))

/**
  * @}
  */

/** @defgroup UART_Stick_Parity_Type_define
  * @{
  */
  
#define RUART_STICK_PARITY_DISABLE		((u32)0x00000000)
#define RUART_STICK_PARITY_ENABLE			((u32)0x00000001)

#define IS_UART_STICK_PARITY_ENABLE(VAL) (((VAL) == RUART_STICK_PARITY_DISABLE) || \
								((VAL) == RUART_STICK_PARITY_ENABLE))

/**
  * @}
  */

/** @defgroup UART_Interrupt_ID_define
  * @{
  */

#define RUART_MODEM_STATUS					((u32)0x00000000)
#define RUART_TX_FIFO_EMPTY					((u32)0x00000001)
#define RUART_RECEIVER_DATA_AVAILABLE		((u32)0x00000002)
#define RUART_RECEIVE_LINE_STATUS				((u32)0x00000003)
#define RUART_LP_RX_MONITOR_DONE			((u32)0x00000004)
#define RUART_TIME_OUT_INDICATION			((u32)0x00000006)

/**
  * @}
  */

/** @defgroup UART_RX_Clock_Source_define
  * @{
  */
  
#define UART_RX_CLK_XTAL_40M	((u32)0x00000000)
#define UART_RX_CLK_OSC_LP		((u32)0x00000001)
#define UART_RX_CLK_XTAL_LP	((u32)0x00000002)
#define IS_UART_RX_CLK(CLK) (((CLK) == UART_RX_CLK_XTAL_40M) || \
							((CLK) == UART_RX_CLK_XTAL_LP) || \
							((CLK) == UART_RX_CLK_OSC_LP))
/**
  * @}
  */

/** @defgroup UART_Rx_Err_Report_define 
  * @{
  */
  
#define UART_RX_EEROR_REPORT_DISABLE		((u32)0x00000000)
#define UART_RX_EEROR_REPORT_ENABLE			((u32)0x00000001)

#define IS_UART_RX_ERROR_REPORT(REPORT)  (((REPORT) == UART_RX_EEROR_REPORT_DISABLE) || \
							((REPORT) == UART_RX_EEROR_REPORT_ENABLE) )

/**
  * @}
  */

/** @defgroup UART_Low_Power_Peripheral_define 
  * @{
  */
#define IS_LPUART_PERIPH(PERIPH) (((PERIPH) == UART0_DEV) || ((PERIPH) == UART2_DEV) || ((PERIPH) == UART3_DEV))
/**
  * @}
  */ 

/** @defgroup UART_SoftWare_Status_define 
  * @{
  */
#define STATETX_DMA			1
#define STATETX_INT				2
#define STATETX_POLL			3
#define STATERX_DMA			1
#define STATERX_INT				2
#define STATERX_POLL			3
/**
  * @}
  */


/**
  * @}
  */ 

/* Exported functions --------------------------------------------------------*/
/** @defgroup UART_Exported_Functions UART Exported Functions
  * @{
  */
/** @defgroup UART_Normal_functions 
  * @{
  */
_LONG_CALL_ void UART_DeInit(UART_TypeDef* UARTx);
_LONG_CALL_ void UART_StructInit(UART_InitTypeDef* UART_InitStruct);
_LONG_CALL_ void UART_Init(UART_TypeDef* UARTx, UART_InitTypeDef *UART_InitStruct);
_LONG_CALL_ u32 UART_BaudParaGet(u32 baudrate, u32 *ovsr, u32 *ovsr_adj);
_LONG_CALL_ void UART_BaudParaGetFull(u32 IPclk, u32 baudrate, u32 *ovsr, u32 *ovsr_adj);
_LONG_CALL_ void UART_SetBaudExt(UART_TypeDef* UARTx, u32 Ovsr, u32 Ovsr_adj);
_LONG_CALL_ void UART_SetBaud(UART_TypeDef* UARTx, u32 BaudRate);
_LONG_CALL_ void UART_SetRxLevel(UART_TypeDef* UARTx, u32 FifoLv);
_LONG_CALL_ void UART_RxCmd(UART_TypeDef* UARTx, u32 NewState);
_LONG_CALL_ u32 UART_Writable(UART_TypeDef* UARTx);
_LONG_CALL_ u32 UART_Readable(UART_TypeDef* UARTx);
_LONG_CALL_ void UART_CharPut(UART_TypeDef* UARTx, u8 TxData);
_LONG_CALL_ void UART_CharGet(UART_TypeDef* UARTx, u8  *pRxByte);
_LONG_CALL_ void UART_ReceiveData(UART_TypeDef* UARTx, u8* OutBuf, u32 Count);
_LONG_CALL_ void UART_SendData(UART_TypeDef* UARTx, u8* InBuf, u32 Count);
_LONG_CALL_ u32 UART_ReceiveDataTO(UART_TypeDef* UARTx, u8* OutBuf, u32 Count, u32 Times);
_LONG_CALL_ u32 UART_SendDataTO(UART_TypeDef* UARTx,u8* InBuf,u32 Count, u32 Times);
_LONG_CALL_ void UART_RxByteCntClear(UART_TypeDef* UARTx);
_LONG_CALL_ u32 UART_RxByteCntGet(UART_TypeDef* UARTx);
_LONG_CALL_ void UART_BreakCtl(UART_TypeDef* UARTx, u32 NewState);
_LONG_CALL_ u32 UART_ClearRxFifo(UART_TypeDef* UARTx);
_LONG_CALL_ void UART_ClearTxFifo(UART_TypeDef* UARTx);
_LONG_CALL_ void UART_INTConfig(UART_TypeDef* UARTx, u32 UART_IT, u32 newState);
_LONG_CALL_ u32 UART_IntStatus(UART_TypeDef* UARTx);
_LONG_CALL_ u32 UART_ModemStatusGet(UART_TypeDef* UARTx);
_LONG_CALL_ u32 UART_LineStatusGet(UART_TypeDef* UARTx);
_LONG_CALL_ void UART_WaitBusy(UART_TypeDef* UARTx, u32 PollTimes);
/**
  * @}
  */ 

/** @defgroup UART_DMA_functions 
  * @{
  */
_LONG_CALL_ void UART_TXDMAConfig(UART_TypeDef* UARTx, u32 TxDmaBurstSize);
_LONG_CALL_ void UART_RXDMAConfig(UART_TypeDef* UARTx, u32 RxDmaBurstSize);
_LONG_CALL_ void UART_TXDMACmd(UART_TypeDef* UARTx, u32 NewState);
_LONG_CALL_ void UART_RXDMACmd(UART_TypeDef* UARTx, u32 NewState);
_LONG_CALL_ BOOL UART_TXGDMA_Init(u8 UartIndex, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pTxBuf, int TxCount); 
_LONG_CALL_ BOOL UART_RXGDMA_Init(u8 UartIndex, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pRxBuf, int RxCount);
/**
  * @}
  */ 

/** @defgroup UART_Low_Power_functions 
  * @{
  */
_LONG_CALL_ void UART_MonitorParaConfig(UART_TypeDef* UARTx, u32 BitNumThres, u32 OscPerbitUpdCtrl);
_LONG_CALL_ void UART_LPRxBaudSet(UART_TypeDef* UARTx, u32 BaudRate, u32 RxIPClockHz);
_LONG_CALL_	void UART_RxMonitorCmd(UART_TypeDef* UARTx, u32 NewState);
_LONG_CALL_ u32 UART_RxMonBaudCtrlRegGet(UART_TypeDef* UARTx);
_LONG_CALL_ u32 UART_RxMonitorSatusGet(UART_TypeDef* UARTx);
/**
  * @}
  */ 

/** @defgroup UART_IRDA_functions 
  * @{
  */
_LONG_CALL_ void UART_IrDAStructInit(IrDA_InitTypeDef * IrDA_InitStruct);
_LONG_CALL_ void UART_IrDAInit(UART_TypeDef* UARTx, IrDA_InitTypeDef * IrDA_InitStruct);
_LONG_CALL_ void UART_IrDACmd(UART_TypeDef* UARTx, u32 NewState);
/**
  * @}

  */
/**
  * @}
  */




/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup UART_Register_Definitions UART Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup UART_DLH_INTCR
 * @{
 *****************************************************************************/
#define RUART_IER_ERBI						((u32)0x00000001)	     /*BIT[0], Enable received data available interrupt (rx trigger)*/
#define RUART_IER_ETBEI					((u32)0x00000001<<1)	     /*BIT[1], Enable transmitter FIFO empty interrupt (tx fifo empty)*/
#define RUART_IER_ELSI						((u32)0x00000001<<2)	     /*BIT[2], Enable receiver line status interrupt (receiver line status)*/
#define RUART_IER_EDSSI					((u32)0x00000001<<3)	     /*BIT[3], Enable modem status interrupt (modem status transition)*/
#define RUART_IER_EDMI						((u32)0x00000001<<4)	     /*BIT[4], Enable low power rx monitor done interrupt (monitor done)*/
#define RUART_IER_ETOI						((u32)0x00000001<<5)	     /*BIT[5], Enable rx time out interrupt*/
/** @} */

/**************************************************************************//**
 * @defgroup UART_INTID
 * @{
 *****************************************************************************/
#define RUART_IIR_INT_PEND					((u32)0x00000001)		/*uart interrupt global status*/
#define RUART_IIR_INT_ID					((u32)0x00000007<<1)	       /*Uart Interrupt ID mask, register INTID[3:1]*/
/** @} */

/**************************************************************************//**
 * @defgroup UART_FCR
 * @{
 *****************************************************************************/
#define RUART_FIFO_CTL_RX_ERR_RPT		((u32)0x00000001)	      /*BIT[0], 0x01, RX error report control bit*/
#define RUART_FIFO_CTL_REG_CLEAR_RXFIFO	((u32)0x00000001<<1)	      /*BIT[1], 0x02, Write 1 clear*/
#define RUART_FIFO_CTL_REG_CLEAR_TXFIFO	((u32)0x00000001<<2)	      /*BIT[2], 0x04, Write 1 clear*/
#define RUART_FIFO_CTL_REG_DMA_ENABLE	((u32)0x00000001<<3)	      /*BIT[3], 0x08, Uart DMA control bit*/
#define RUART_FIFO_CTL_REG_RX_TRG_LEV	((u32)0x00000003<<6)	      /*BIT[7:6], 0xc0, Uart rx trigger level field*/
/** @} */

/**************************************************************************//**
 * @defgroup UART_MCR
 * @{
 *****************************************************************************/								
#define RUART_MCL_FLOW_ENABLE			((u32)((0x00000001 << 5) | (0x00000001 << 1))) /*BIT[1],BIT[5],Uart auto flow control enable bit*/
/** @} */

/**************************************************************************//**
 * @defgroup UART_LCR
 * @{
 *****************************************************************************/
#define BIT_UART_LCR_BREAK_CTRL			((u32)0x00000001<<6)       /*BIT[6], Uart break control function enable bit*/
#define RUART_LINE_CTL_REG_DLAB_ENABLE	((u32)0x00000001<<7)	      /*BIT[7], 0x80*/
/** @} */

/**************************************************************************//**
 * @defgroup UART_LSR
 * @{
 *****************************************************************************/
#define RUART_LINE_STATUS_REG_DR			((u32)0x00000001)	     /*BIT[0], Data ready indicator*/
#define RUART_LINE_STATUS_ERR_OVERRUN	((u32)0x00000001<<1)	     /*BIT[1], Over run*/
#define RUART_LINE_STATUS_ERR_PARITY		((u32)0x00000001<<2)	     /*BIT[2], Parity error*/
#define RUART_LINE_STATUS_ERR_FRAMING	((u32)0x00000001<<3)     /*BIT[3], Framing error*/
#define RUART_LINE_STATUS_ERR_BREAK		((u32)0x00000001<<4)	    /*BIT[4], Break interrupt error*/
#define RUART_LINE_STATUS_REG_THRE		((u32)0x00000001<<5)     /*BIT[5], 0x20, Transmit holding register empty interrupt enable*/
#define RUART_LINE_STATUS_REG_TEMT		((u32)0x00000001<<6)	    /*BIT[6], 0x40, Transmitter empty indicator(bit)*/
#define RUART_LINE_STATUS_ERR_RXFIFO		((u32)0x00000001<<7)	    /*BIT[7], RX FIFO error*/
#define RUART_LINE_STATUS_ERR				(RUART_LINE_STATUS_ERR_OVERRUN |RUART_LINE_STATUS_ERR_PARITY| \
								RUART_LINE_STATUS_ERR_FRAMING|RUART_LINE_STATUS_ERR_BREAK| \
								RUART_LINE_STATUS_ERR_RXFIFO)	    /*Line status error*/
/** @} */

/**************************************************************************//**
 * @defgroup UART_SPR
 * @{
 *****************************************************************************/
#define RUART_SP_REG_RXBREAK_INT_STATUS	((u32)0x00000001<<7)		/*BIT[7], 0x80, Write 1 clear*/
#define RUART_SP_REG_DBG_SEL				((u32)0x0000000F<<8)	       /*BIT[11:8], Debug port selection*/
#define RUART_SP_REG_XFACTOR_ADJ			((u32)0x000007FF<<16)	/*BIT[26:16], ovsr_adj parameter field*/
/** @} */

/**************************************************************************//**
 * @defgroup UART_STSR
 * @{
 *****************************************************************************/
//#define RUART_STS_REG_RESET_RCV			((u32)0x00000001<<3)		/*BIT[3], 0x08, Reset uart receiver*/
#define RUART_STS_REG_XFACTOR			((u32)0x000FFFFF<<4)       /*BIT[23:4]ovsr parameter field*/
/** @} */

/**************************************************************************//**
 * @defgroup UART_MISCR
 * @{
 *****************************************************************************/
#define RUART_IRDA_ENABLE					((u32)0x00000001)		/*BIT[0], Enable IrDA*/

#define RUART_IRDA_TX_INVERT				((u32)0x00000001 << 13)	/*BIT[13], Enable IrDA tx invert*/

#define RUART_IRDA_RX_INVERT				((u32)0x00000001 << 14)	/*BIT[14], Enable IrDA rx invert*/

#define RUART_TXDMA_BURSTSIZE_MASK		((u32)0x0000001F << 3)	/*BIT[7:3], Uart tx DMA burst size mask.
																	 This field value must be no more than 16. 
																	 Because tx fifo depth is 16 in UART IP hardware*/

#define RUART_RXDMA_BURSTSIZE_MASK		((u32)0x0000001F << 8)	/*BIT[12:8], Uart rx DMA burst size mask.
																	 This field value must be no more than 16. 
																	 Because rx fifo depth is 16 in uart IP hardware*/
																	 
#define RUART_TXDMA_ENABLE				((u32)0x00000001 << 1)	/*BIT[1], Uart tx DMA enable bit*/
#define RUART_RXDMA_ENABLE				((u32)0x00000001 << 2)	/*BIT[2], Uart rx DMA enable bit*/

#define RUART_RXDMA_OWNER				((u32)0x00000001 << 15)	/*BIT[15], the DMA flow controller selection(UART or DMA)*/
#define RUART_RXDMA_DUMMY_DATA		((u32)0x000000FF << 16)	/*BIT[23:16], dummy data when uart is the flow controller*/
#define RUART_RXDMA_DUMMY_FLAG		((u32)0x00000001 << 24)	/*BIT[24], this bit is set when master read dummy data from UART RX FIFO, it can be cleared by software
															by writing 1 to this bit*/

/** @} */

/**************************************************************************//**
 * @defgroup UART_TXPLSR
 * @{
 *****************************************************************************/
#define RUART_IRDA_TX_PUL_LOW_BUND_VAL			((u32)0x00007FFF)		/*BIT[14:0], IrDA tx pulse low bound edge shift value*/

#define RUART_IRDA_TX_PUL_LOW_BUND_SHIFT		((u32)0x00000001 << 15)	/*BIT[15], IrDA tx pulse low bound edge shift direction*/

#define RUART_IRDA_TX_PUL_UP_BUND_VAL			((u32)0x00007FFF << 16)	/*BIT[30:16], IrDA tx pulse upper bound edge shift value*/

#define RUART_IRDA_TX_PUL_UP_BUND_SHIFT			((u32)0x00000001 << 31)	/*BIT[31], IrDA tx pulse upper bound edge shift direction*/
/** @} */

/**************************************************************************//**
 * @defgroup UART_RXPLSR
 * @{
 *****************************************************************************/
#define RUART_IRDA_RX_FILTER_ENABLE				((u32)0x00000001)		/*BIT[0], IrDA rx filter enable*/

#define RUART_IRDA_RX_FILTER_THRES				((u32)0x00007FFF << 1)	/*BIT[15:1], IrDA rx filter threshold field*/
/** @} */

/**************************************************************************//**
 * @defgroup UART_REG_RX_PATH_CTRL
 * @{
 *****************************************************************************/
#define RUART_REG_LP_RX_PATH_SELECT				((u32)0x00000001)     		/*BIT[0], 0x01, Select uart low power rx path*/
#define RUART_REG_LP_RX_PATH_RESET				((u32)0x00000001 << 2)    	/*BIT[2], 0x40, Reset uart low power rx path receiver*/
#define RUART_REG_RX_XFACTOR_ADJ					((u32)0x000007FF << 3)	/*BIT[13:3], One factor of Baud rate calculation for rx path, similar with xfactor_adj */
#define RUART_REG_RX_TO_THRES						((u32)0x0000FFFF<<16)     /*BIT[31:16], rx timeout threshold, unit is one bit period*/
/** @} */

/**************************************************************************//**
 * @defgroup UART_REG_MON_BAUD_CTRL
 * @{
 *****************************************************************************/
#define RUART_LP_RX_MON_ENABLE				((u32)0x00000001)     		/*BIT[0], 0x01, Enable low power rx monitor function*/
#define RUART_LP_RX_BIT_NUM_THRES			((u32)0x000000FF << 1)    	/*BIT[8:1], Bit Number threshold of one monitor period*/
#define RUART_LP_RX_OSC_CYCNUM_PERBIT		((u32)0x000FFFFF << 9)    	/*BIT[28:9], Cycle number perbit for osc clock */
#define RUART_LP_RX_OSC_UPD_IN_XTAL			((u32)0x00000001 << 29)   	/*BIT[29], Control bit for osc monitor parameter update */
/** @} */

/**************************************************************************//**
 * @defgroup UART_REG_MON_BAUD_STS
 * @{
 *****************************************************************************/
#define RUART_LP_RX_XTAL_CYCNUM_PERBIT		((u32)0x000FFFFF)     		/*BIT[19:0], Cycle number perbit for xtal clock */
#define RUART_LP_RX_MON_RDY					((u32)0x00000001 << 20)    /*BIT[20], Monitor ready status*/
#define RUART_LP_RX_MON_TOTAL_BITS			((u32)0x0000000F << 21)    /*BIT[28:21], Actualy monitor bit number */
/** @} */

/**************************************************************************//**
 * @defgroup UART_REG_RX_BYTE_CNT
 * @{
 *****************************************************************************/
/********************  Bits definition for  register  *******************/
#define RUART_RX_READ_BYTE_CNTER				((u32)0x0000FFFF)     		/*BIT[15:0], Byte number of data read from rx fifo */
#define RUART_RX_BYTE_CNTER_CLEAR			((u32)0x00000001 << 16)    /*BIT[16], Write 1 clear rx byte counter*/
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
	u32 LOW_POWER_RX_ENABLE;	/*Enable low power RX*/
} UARTCFG_TypeDef;

typedef struct
{
	UART_TypeDef* UARTx;
	u32 Tx_HandshakeInterface;
	u32 Rx_HandshakeInterface;
	IRQn_Type	IrqNum;
} UART_DevTable;

extern UARTCFG_TypeDef uart_config[];
extern const UART_DevTable UART_DEV_TABLE[4];
extern u32 UART_StateTx[4];
extern u32 UART_StateRx[4];

#define MAX_UART_INDEX			(4)

static inline void
UART_SetTxFlag(u32 UartIdx, u32 Flag)
{
	UART_StateTx[UartIdx] = Flag;
}

static inline void
UART_SetRxFlag(u32 UartIdx, u32 Flag)
{	
	UART_StateRx[UartIdx] = Flag;
}

static inline u32
UART_GetTxFlag(u32 UartIdx)
{
	return (UART_StateTx[UartIdx]);
}

static inline u32
UART_GetRxFlag(u32 UartIdx)
{
	return (UART_StateRx[UartIdx]);
}
#endif
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
