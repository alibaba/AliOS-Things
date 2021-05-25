/** mbed Microcontroller Library
  ******************************************************************************
  * @file    serial_api.h
  * @author 
  * @version V1.0.0
  * @brief   This file provides mbed API for UART.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2006-2013 ARM Limited
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  ****************************************************************************** 
  */

#ifndef MBED_SERIAL_API_H
#define MBED_SERIAL_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup uart UART
 *  @ingroup    hal
 *  @brief      uart functions
 *  @{
 */

///@name Ameba Common
///@{


/**
* @brief UART Parity mode
* used by serial_format
*/
typedef enum {
	ParityNone = 0,		/*!<parity disable */
	ParityOdd = 1,		/*!<odd parity enable */
	ParityEven = 2,		/*!<even paroty enable */
	ParityForced1 = 3,	/*!<same action with ParityOdd */
	ParityForced0 = 4	/*!<same action with ParityEven */
} SerialParity;

/**
 * @brief UART Interrupt enable/disable
 * used by serial_irq_set
 */
typedef enum {
	RxIrq,		/*!<RX IRQ enable/disable */
	TxIrq		/*!<TX IRQ enable/disable */
} SerialIrq;

/**
 * @brief UART FlowControl mode
 * used by serial_set_flow_control
 */
typedef enum {
	FlowControlNone,	/*!<none RTS/CTS */
	FlowControlRTS,		/*!<RTS enable */
	FlowControlCTS,		/*!<CTS enable */
	FlowControlRTSCTS	/*!<RTS/CTS enable */
} FlowControl;

typedef void (*uart_irq_handler)(uint32_t id, SerialIrq event);

typedef struct serial_s serial_t;

/**
  * @brief  Initializes the UART device, include clock/function/interrupt/UART registers.
  * @param  obj: uart object define in application software.
  * @param  tx: Tx PinName according to pinmux spec.
  * @param  rx: Rx PinName according to pinmux spec.
  * @retval none  
  */
void serial_init(serial_t *obj, PinName tx, PinName rx);

/**
  * @brief  Deinitializes the UART device, include clock/function/interrupt/UART registers.
  * @param  obj: uart object define in application software.
  * @retval none  
  */
void serial_free(serial_t *obj);

/**
  * @brief  Set UART device baudrate.
  * @param  obj: uart object define in application software.
  * @param  baudrate: Baud Rate Val, like 115200 (unit is HZ).
  * @retval none  
  */
void serial_baud(serial_t *obj, int baudrate);

/**
  * @brief  Set UART format.
  * @param  obj: uart object define in application software.
  * @param  data_bits: data bits, this parameter can be one of the following values:
  *		@arg 7
  *		@arg 8
  * @param  parity: this parameter can be one of the following values:
  *		@arg ParityNone
  *		@arg ParityOdd
  *		@arg ParityEven
  *		@arg ParityForced1: same action with ParityOdd
  *		@arg ParityForced0: same action with ParityEven
  * @param  stop_bits: this parameter can be one of the following values:
  *		@arg 2
  *		@arg 1
  * @retval none  
  */
void serial_format(serial_t *obj, int data_bits, SerialParity parity, int stop_bits);

/**
  * @brief  Set UART interrupt hander if needed.
  * @param  obj: uart object define in application software.
  * @param  handler: interrupt callback function
  * @param  id: interrupt callback parameter
  * @retval none  
  */
void serial_irq_handler(serial_t *obj, uart_irq_handler handler, uint32_t id);

/**
  * @brief  Enable/Disable UART interrupt.
  * @param  obj: uart object define in application software.
  * @param  irq: Tx or Rx interrupt, this parameter can be one of the following values:
  *		@arg RxIrq
  *		@arg TxIrq
  * @param  enable: this parameter can be one of the following values:
  *		@arg 0 disable
  *		@arg 1 enable
  * @retval none  
  */
void serial_irq_set(serial_t *obj, SerialIrq irq, uint32_t enable);

/**
  * @brief  get one byte from UART.
  * @param  obj: uart object define in application software.
  * @retval : received character
  * @note this function is asynchronous API.
  */
int  serial_getc(serial_t *obj);

/**
  * @brief  send one byte use UART.
  * @param  obj: uart object define in application software.
  * @param  c: the data to transmit.
  * @retval none
  * @note this function is asynchronous API.
  */
void serial_putc(serial_t *obj, int c);

/**
  * @brief  check if there is data in rx fifo.
  * @param  obj: uart object define in application software.
  * @retval 1: TRUE
  * @retval 0: FALSE   
  */
int  serial_readable(serial_t *obj);

/**
  * @brief  check if write data to tx fifo is permitted.
  * @param  obj: uart object define in application software.
  * @retval 1: TRUE
  * @retval 0: FALSE   
  */
int  serial_writable(serial_t *obj);

/**
  * @brief  Clear Rx fifo.
  * @param  obj: uart object define in application software.
  * @retval none
  */
void serial_clear(serial_t *obj);

/**
  * @brief  enable UART break contol function.
  * @param  obj: uart object define in application software.
  * @retval none
  */
void serial_break_set(serial_t *obj);

/**
  * @brief  disable UART break contol function.
  * @param  obj: uart object define in application software.
  * @retval none
  */
void serial_break_clear(serial_t *obj);

/**
  * @brief  set tx pinmux.
  * @param  tx: Tx PinName according to pinmux spec.
  * @retval none
  */
void serial_pinout_tx(PinName tx);

/**
  * @brief  uart autoflow control setting.
  * @param  obj: uart object define in application software.
  * @param  type: autoflow control type.
  * @param  rxflow: RTS pin.
  * @param  txflow: CTS pin.
  * @retval   none
  */
void serial_set_flow_control(serial_t *obj, FlowControl type, PinName rxflow, PinName txflow);

///@}

/*\@}*/
#ifdef __cplusplus
}
#endif

#endif
