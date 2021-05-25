/** mbed Microcontroller Library
  ******************************************************************************
  * @file    serial_ex_api.h
  * @author 
  * @version V1.0.0
  * @brief   This file provides mbed API for UART.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2015, Realtek Semiconductor Corp.
  * All rights reserved.
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  ****************************************************************************** 
  */
#ifndef MBED_SERIAL_EX_API_H
#define MBED_SERIAL_EX_API_H
 
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup uart_ex UART_EX
 *  @ingroup    hal
 *  @brief      uart extended functions
 *  @{
 */


///@name Ameba Common
///@{

/**
* @brief Define RX FIFO Level: RX interrupt trigger, RTS de-assert trigger
*/
typedef enum {
    FifoLv1Byte=0,    /*!< 1-byte */
    FifoLvQuarter=1,  /*!< 4-byte/8-byte(for 8195B) */
    FifoLvHalf=2,     /*!< 8-byte/16-byte(for 8195B) */
    FifoLvFull=3      /*!< 14-byte/30-bytes(for 8195B) */
} SerialFifoLevel;

/**
  * @brief  Clear TX fifo.
  * @param  obj: uart object define in application software.
  * @retval none
  */
void serial_clear_tx(serial_t *obj);

/**
  * @brief  Clear RX fifo.
  * @param  obj: uart object define in application software.
  * @retval none
  */
void serial_clear_rx(serial_t *obj);

/**
  * @brief  set TX complete handler.
  * @param  obj: uart object define in application software.
  * @param  handler: TX complete callback function.
  * @param  id: TX complete callback function parameter.
  * @retval none
  * @note this function is used when asynchronous API is used.
  */
void serial_send_comp_handler(serial_t *obj, void *handler, uint32_t id);

/**
  * @brief  set RX complete handler.
  * @param  obj: uart object define in application software.
  * @param  handler: RX complete callback function.
  * @param  id: RX complete callback function parameter.
  * @retval none
  * @note this function is used when asynchronous API is used.
  */
void serial_recv_comp_handler(serial_t *obj, void *handler, uint32_t id);

/**
  * @brief  recv target length data use poll mode, with time out.
  * @param  obj: uart object define in application software.
  * @param  ptxbuf: buffer to be written to Tx FIFO.
  * @param  len: number of data to be recv.
  * @param  timeout_ms: polling time before timeout.
  * @retval : return received bytes count
  * @note this function is synchronous API.
  */
int32_t serial_recv_blocked(serial_t *obj, char *prxbuf, uint32_t len, uint32_t timeout_ms);

/**
  * @brief  send target length data use poll mode, with time out.
  * @param  obj: uart object define in application software.
  * @param  ptxbuf: buffer to be written to Tx FIFO.
  * @param  len: number of data to be send.
  * @param  timeout_ms: polling time before timeout.
  * @retval : transmitted bytes count
  * @note this function is synchronous API.
  */
int32_t serial_send_blocked(serial_t *obj, char *ptxbuf, uint32_t len, uint32_t timeout_ms);

/**
  * @brief  recv target length data use interrupt mode.
  * @param  obj: uart object define in application software.
  * @param  prxbuf: buffer to save data read from UART FIFO.
  * @param  len: number of data to be read.
  * @retval : HAL_Status
  * @note this function is asynchronous API.
  */
int32_t serial_recv_stream(serial_t *obj, char *prxbuf, uint32_t len);

/**
  * @brief  send target length data use interrupt mode.
  * @param  obj: uart object define in application software.
  * @param  ptxbuf: buffer to be written to Tx FIFO.
  * @param  len: number of data to be send.
  * @retval : HAL_Status
  * @note this function is asynchronous API.
  */
int32_t serial_send_stream(serial_t *obj, char *ptxbuf, uint32_t len);

/**
  * @brief  recv target length data use DMA mode.
  * @param  obj: uart object define in application software.
  * @param  prxbuf: buffer to save data read from UART FIFO.
  * @param  len: number of data to be read.
  * @retval : HAL_Status
  * @note this function is asynchronous API.
  */
int32_t serial_recv_stream_dma(serial_t *obj, char *prxbuf, uint32_t len);

/**
  * @brief  send target length data use DMA mode.
  * @param  obj: uart object define in application software.
  * @param  ptxbuf: buffer to be written to Tx FIFO.
  * @param  len: number of data to be send.
  * @retval : HAL_Status
  * @note this function is asynchronous API.
  */
int32_t serial_send_stream_dma(serial_t *obj, char *ptxbuf, uint32_t len);

/**
  * @brief  stop the stream or stream_dma TX.
  * @param  obj: uart object define in application software.
  * @retval : number of bytes sent before stop
  */
int32_t serial_send_stream_abort(serial_t *obj);

/**
  * @brief  stop the stream or stream_dma RX.
  * @param  obj: uart object define in application software.
  * @retval : number of bytes received before stop
  */
int32_t serial_recv_stream_abort(serial_t *obj);

/**
  * @brief  disable uart clock and function.
  * @param  obj: uart object define in application software.
  * @retval none
  */
void serial_disable(serial_t *obj);

/**
  * @brief  enable uart clock and function.
  * @param  obj: uart object define in application software.
  * @retval none
  */
void serial_enable(serial_t *obj);

/**
  * @brief  recv target length data use interrupt mode.
  * @param  obj: uart object define in application software.
  * @param  prxbuf: buffer to save data read from UART FIFO.
  * @param  len: number of data to be recv.
  * @param  timeout_ms: polling time before timeout.
  * @param  force_cs: forcing context switch function.
  * @retval : the byte count received before timeout, or error(<0)
  * @note this function is asynchronous API.
  */
int32_t serial_recv_stream_timeout(serial_t *obj, char *prxbuf, uint32_t len, uint32_t timeout_ms,	void *force_cs);

/**
  * @brief  recv target length data use DMA mode.
  * @param  obj: uart object define in application software.
  * @param  prxbuf: buffer to save data read from UART FIFO.
  * @param  len: number of data to be recv.
  * @param  timeout_ms: polling time before timeout.
  * @param  force_cs: forcing context switch function.
  * @retval : the byte count received before timeout, or error(<0)
  * @note this function is asynchronous API. Some parameters have changed for AmebaD.
  */
int32_t serial_recv_stream_dma_timeout(serial_t *obj, char *prxbuf, uint32_t len, uint32_t timeout_ms,	void *force_cs);

///@}

/*\@}*/

#ifdef __cplusplus
}
#endif

#endif  // #ifndef MBED_SERIAL_EX_API_H
