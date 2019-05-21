/**
 ******************************************************************************
 * @file    BkDriverUart.h
 * @brief   This file provides all the headers of UART operation functions.
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2017 BEKEN Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */


#ifndef __BEKENDRIVERUART_H__
#define __BEKENDRIVERUART_H__

#pragma once
#include "include.h"
#include "uart_pub.h"

/** @addtogroup BK_PLATFORM
* @{
*/

/** @defgroup BK_UART _BK_ UART Driver
* @brief  Universal Asynchronous Receiver Transmitter (UART) Functions
* @{
*/

/******************************************************
 *					 Enumerations
 ******************************************************/
typedef enum
{
    BK_UART_1 = 0,
    BK_UART_2,
    BK_UART_MAX,
} bk_uart_t;

/******************************************************
 *					  Structures
 ******************************************************/
/******************************************************
 *                 Type Definitions
 ******************************************************/
typedef struct
{
    uint32_t					  baud_rate;
    uart_data_width_t             data_width;
    uart_parity_t 	              parity;
    uart_stop_bits_t	          stop_bits;
    uart_flow_control_t           flow_control;
    uint8_t					      flags;	 /**< if set, UART can wake up MCU from stop mode, reference: @ref UART_WAKEUP_DISABLE and @ref UART_WAKEUP_ENABLE*/
} platform_uart_config_t;

typedef struct
{
    uint32_t  size;
    uint32_t  head;
    uint32_t  tail;
    uint8_t  *buffer;
} ring_buffer_t;

typedef platform_uart_config_t          bk_uart_config_t;

/******************************************************
 *                 Function Declarations
 ******************************************************/
OSStatus bk_uart_initialize_test( bk_uart_t uart, uint8_t config, ring_buffer_t *optional_rx_buffer );
/**@brief Initialises a UART interface
 *
 * @note Prepares an UART hardware interface for communications
 *
 * @param  uart     : the interface which should be initialised
 * @param  config   : UART configuration structure
 * @param  optional_rx_buffer : Pointer to an optional RX ring buffer
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
OSStatus bk_uart_initialize( bk_uart_t uart, const bk_uart_config_t *config, ring_buffer_t *optional_rx_buffer );

/**@brief Deinitialises a UART interface
 *
 * @param  uart : the interface which should be deinitialised
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
OSStatus bk_uart_finalize( bk_uart_t uart );


/**@brief Transmit data on a UART interface
 *
 * @param  uart     : the UART interface
 * @param  data     : pointer to the start of data
 * @param  size     : number of bytes to transmit
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
OSStatus bk_uart_send( bk_uart_t uart, const void *data, uint32_t size );


/**@brief Receive data on a UART interface
 *
 * @param  uart     : the UART interface
 * @param  data     : pointer to the buffer which will store incoming data
 * @param  size     : number of bytes to receive
 * @param  timeout  : timeout in milisecond
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
OSStatus bk_uart_recv( bk_uart_t uart, void *data, uint32_t size, uint32_t timeout );
OSStatus bk_uart_recv_prefetch( bk_uart_t uart, void *data, uint32_t size, uint32_t timeout );

/**@brief Read the length of the data that is already recived by uart driver and stored in buffer
 *
 * @param uart     : the UART interface
 *
 * @return    Data length
 */
uint32_t bk_uart_get_length_in_buffer( bk_uart_t uart );

/**@brief set receive data callback on a UART interface, please notice that, the callback will beinvoked in ISR
 *
 * @param  uart     : the UART interface
 * @param  callback : callback to invoke when UART receive data, use NULL to clear callback
 * @param  param  : user param which will filled in callback as 2nd parameter
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
OSStatus bk_uart_set_rx_callback( bk_uart_t uart, uart_callback callback, void *param );

/** @} */
/** @} */

#endif
