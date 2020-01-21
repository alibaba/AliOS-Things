/**
 ******************************************************************************
 * @file    mx_hal_serial.c
 * @author  William Xu
 * @version V1.0.0
 * @date    9-Apr-2018
 * @brief   UART driver used for AT parser
 ******************************************************************************
 *
 * Copyright (c) 2009-2018 MXCHIP Co.,Ltd.
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
 *
 ******************************************************************************
 */

#include <hal_usart_async.h>
#include <utils.h>

#include "mx_hal.h"

#define MCU_DRVIER_RECV     0

/******************************************************************************
 *                              Variable Definitions
 ******************************************************************************/

extern struct usart_async_descriptor USART_AT;
static struct io_descriptor *io_at;
static volatile bool tx_complete = true;
static int _timeout = 100;

#if MCU_DRVIER_RECV


uint8_t at_buffer[MX_SERIAL_RX_BUF_SIZE];
struct ringbuffer at_rx;
void SERIAL_RX_ISR(void);
#endif

/******************************************************************************
 *                              Function Definitions
 ******************************************************************************/

static void rx_cb_USART_AT(const struct usart_async_descriptor *const io_descr)
{
	/* Driver will store the received data to its buffer */
}

static void tx_cb_USART_AT(const struct usart_async_descriptor *const io_descr)
{
	tx_complete = true;
}

void mx_hal_serial_init(int timeout)
{
	_timeout = timeout;
	
#if MCU_DRVIER_RECV
	ringbuffer_init(&at_rx, at_buffer, MX_SERIAL_RX_BUF_SIZE);
#endif
	
	usart_async_register_callback(&USART_AT, USART_ASYNC_RXC_CB, rx_cb_USART_AT);
	usart_async_register_callback(&USART_AT, USART_ASYNC_TXC_CB, tx_cb_USART_AT);
	usart_async_get_io_descriptor(&USART_AT, &io_at);
	usart_async_enable(&USART_AT);
}

void mx_hal_serial_set_timeout(int timeout)
{
	_timeout = timeout;	
}

int mx_hal_serial_putc(char c)
{
	io_write(io_at, (uint8_t *)&c, 1);
	tx_complete = false;
	while (tx_complete == false);
	return 0;
}

int mx_hal_serial_getc(void)
{
	uint32_t current = mx_hal_ms_ticker_read();
	uint8_t ch;
	
	do {
		if (io_read(io_at, &ch, 1) == 1) return ch;
	} while((mx_hal_ms_ticker_read() - current) < _timeout);
	
	return -1;
}

bool mx_hal_serial_readable(void)
{
	struct usart_async_descriptor *descr = CONTAINER_OF(io_at, struct usart_async_descriptor, io);
	if(ringbuffer_num(&descr->rx)) return true;
	return false;
}


void mx_hal_serial_flush(void)
{
	uint32_t                       num;
	uint8_t                        tmp;
	struct usart_async_descriptor *descr = CONTAINER_OF(io_at, struct usart_async_descriptor, io);
	
	CRITICAL_SECTION_ENTER()
	for(num = ringbuffer_num(&descr->rx); num>0; num--) {
		ringbuffer_get(&descr->rx, &tmp);
	}
	CRITICAL_SECTION_LEAVE()
}

/////////// MCU Serial driver, save data to ringbuffer, this function is already in ATMEL hal library //////////////
#if MCU_DRVIER_RECV

void SERIAL_RX_ISR(void)
{
	uint8_t RXData = mcu_hal_uart_recv();
	ringbuffer_put(&at_rx, RXData);
}


static int32_t io_read(uint8_t *const buf, const uint16_t length)
{
	uint16_t                       was_read = 0;
	uint32_t                       num;

	if (buf == 0 || length == 0) return 0;

	CRITICAL_SECTION_ENTER()
	num = ringbuffer_num(&at_rx);
	CRITICAL_SECTION_LEAVE()

	while ((was_read < num) && (was_read < length)) {
		ringbuffer_get(&at_rx, &buf[was_read++]);
	}

	return (int32_t)was_read;
}

#endif