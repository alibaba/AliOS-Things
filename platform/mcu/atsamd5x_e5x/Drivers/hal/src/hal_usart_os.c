/**
 * \file
 *
 * \brief I/O USART related functionality implementation.
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

#include "hal_usart_os.h"
#include <utils_assert.h>
#include <hal_atomic.h>
#include <utils.h>

/**
 * \brief Driver version
 */
#define USART_OS_DRIVER_VERSION 0x00000001u

/**
 * \brief Maximum amount of USART interface instances
 */
#define MAX_USART_AMOUNT SERCOM_INST_NUM
#define MAX_BUF_UART_BYTES  2048
static int32_t usart_os_write(struct io_descriptor *const io_descr, const uint8_t *const buf, const uint16_t length, uint32_t timeout);
static int32_t usart_sync_write(struct io_descriptor *const io_descr, const uint8_t *const buf, const uint16_t length, uint32_t timeout);
static int32_t usart_os_read(struct io_descriptor *const io_descr, uint8_t *const buf, const uint16_t length, uint32_t timeout);
static int32_t usart_async_read(struct io_descriptor *const io_descr, uint8_t *const buf, const uint16_t length, uint32_t timeout);
static void usart_os_process_byte_sent(struct _usart_async_device *device);
static void usart_os_transmission_complete(struct _usart_async_device *device);
static void usart_os_error(struct _usart_async_device *device);
static void usart_os_fill_rx_buffer(struct _usart_async_device *device, uint8_t data);
static void usart_fill_rx_buffer(struct _usart_async_device *device, uint8_t data);

/**
 * \brief Initialize usart interface
 */
int32_t usart_os_init(struct usart_os_descriptor *const descr, void *const hw, uint8_t *rx_buffer,
                      uint16_t rx_buffer_length, void *const func)
{
	int32_t rc;
	ASSERT(descr && hw && rx_buffer && rx_buffer_length);

	if (ERR_NONE != ringbuffer_init(&descr->rx, rx_buffer, rx_buffer_length)) {
		return ERR_INVALID_ARG;
	}

	descr->kbuf_data = aos_malloc(MAX_BUF_UART_BYTES);
	if (descr->kbuf_data == NULL) {
		return -1;
	}
	memset(descr->kbuf_data, 0, MAX_BUF_UART_BYTES);
	rc = krhino_buf_queue_create(&descr->kbuf, "buf_queue_uart", descr->kbuf_data, MAX_BUF_UART_BYTES, 1);
        
	rc = sem_init(&descr->rx_sem, 0);
	if (rc < 0) {
		return rc;
	}
	rc = sem_init(&descr->tx_sem, 0);
	if (rc < 0) {
		sem_deinit(&descr->rx_sem);
		return rc;
	}
	rc = aos_mutex_new(&descr->rx_mutex);
	if (rc < 0) {
		sem_deinit(&descr->rx_sem);
		sem_deinit(&descr->tx_sem);
		return rc;
	}
	rc = aos_mutex_new(&descr->tx_mutex);
	if (rc < 0) {
		sem_deinit(&descr->rx_sem);
		sem_deinit(&descr->tx_sem);
		aos_mutex_free(&descr->rx_mutex);
		return rc;
	}
	descr->sync_device.hw = hw;
	rc = _usart_async_init(&descr->device, hw);

	if (rc) {
		sem_deinit(&descr->tx_sem);
		sem_deinit(&descr->rx_sem);
		aos_mutex_free(&descr->rx_mutex);
		aos_mutex_free(&descr->tx_mutex);
		return rc;
	}

	descr->rx_buffer = NULL;
	descr->rx_size   = 0;
	descr->rx_length = 0;

	descr->io.read  = usart_async_read;
	descr->io.write = usart_sync_write;

	descr->device.usart_cb.tx_byte_sent = usart_os_process_byte_sent;
	descr->device.usart_cb.rx_done_cb   = usart_fill_rx_buffer;
	descr->device.usart_cb.tx_done_cb   = usart_os_transmission_complete;
	descr->device.usart_cb.error_cb     = usart_os_error;
	_usart_async_set_irq_state(&descr->device, USART_ASYNC_RX_DONE, true);
	_usart_async_set_irq_state(&descr->device, USART_ASYNC_ERROR, true);

	return ERR_NONE;
}

/**
 * \brief Deinitialize usart interface
 */
int32_t usart_os_deinit(struct usart_os_descriptor *const descr)
{
	ASSERT(descr);
	_usart_async_deinit(&descr->device);
	descr->io.read  = NULL;
	descr->io.write = NULL;

	sem_deinit(&descr->rx_sem);
	sem_deinit(&descr->tx_sem);
	aos_mutex_free(&descr->rx_mutex);
	aos_mutex_free(&descr->tx_mutex);
	krhino_buf_queue_del(&descr->kbuf);
	aos_free(descr->kbuf_data);

	return ERR_NONE;
}

int32_t usart_os_enable(struct usart_os_descriptor *const descr)
{
	ASSERT(descr);
	_usart_sync_enable(&descr->sync_device);

	return ERR_NONE;
}

int32_t usart_os_disable(struct usart_os_descriptor *const descr)
{
	ASSERT(descr);
	_usart_sync_disable(&descr->sync_device);

	return ERR_NONE;
}

/**
 * \brief Retrieve I/O descriptor
 */
int32_t usart_os_get_io(struct usart_os_descriptor *const descr, struct io_descriptor **io)
{
	ASSERT(descr && io);

	*io = &descr->io;
	return ERR_NONE;
}

/**
 * \brief Specify action for flow control pins
 */
int32_t usart_os_set_flow_control(struct usart_os_descriptor *const descr, const union usart_flow_control_state state)
{
	ASSERT(descr);
	_usart_async_set_flow_control_state(&descr->device, state);

	return ERR_NONE;
}

/**
 * \brief Set usart baud rate
 */
int32_t usart_os_set_baud_rate(struct usart_os_descriptor *const descr, const uint32_t baud_rate)
{
	ASSERT(descr);
	_usart_async_set_baud_rate(&descr->device, baud_rate);

	return ERR_NONE;
}

/**
 * \brief Set usart data order
 */
int32_t usart_os_set_data_order(struct usart_os_descriptor *const descr, const enum usart_data_order data_order)
{
	ASSERT(descr);
	_usart_async_set_data_order(&descr->device, data_order);

	return ERR_NONE;
}

/**
 * \brief Set usart mode
 */
int32_t usart_os_set_mode(struct usart_os_descriptor *const descr, const enum usart_mode mode)
{
	ASSERT(descr);
	_usart_async_set_mode(&descr->device, mode);

	return ERR_NONE;
}

/**
 * \brief Set usart parity
 */
int32_t usart_os_set_parity(struct usart_os_descriptor *const descr, const enum usart_parity parity)
{
	ASSERT(descr);
	_usart_async_set_parity(&descr->device, parity);

	return ERR_NONE;
}

/**
 * \brief Set usart stop bits
 */
int32_t usart_os_set_stopbits(struct usart_os_descriptor *const descr, const enum usart_stop_bits stop_bits)
{
	ASSERT(descr);
	_usart_async_set_stop_bits(&descr->device, stop_bits);

	return ERR_NONE;
}

/**
 * \brief Set usart character size
 */
int32_t usart_os_set_character_size(struct usart_os_descriptor *const descr, const enum usart_character_size size)
{
	ASSERT(descr);
	_usart_async_set_character_size(&descr->device, size);

	return ERR_NONE;
}

/**
 * \brief Retrieve the state of flow control pins
 */
int32_t usart_os_flow_control_status(const struct usart_os_descriptor *const descr,
                                     union usart_flow_control_state *const   state)
{
	ASSERT(descr && state);
	*state = _usart_async_get_flow_control_state(&descr->device);

	return ERR_NONE;
}

/**
 * \brief flush usart rx ringbuf
 */
int32_t usart_os_flush_rx_buffer(struct usart_os_descriptor *const descr)
{
	ASSERT(descr);

	return ringbuffer_flush(&descr->rx);
}

/**
 * \brief Retrieve the current driver version
 */
uint32_t usart_os_get_version(void)
{
	return USART_OS_DRIVER_VERSION;
}

/*
 * \internal Write the given data to usart interface
 *
 * \param[in] descr The pointer to an io descriptor
 * \param[in] buf Data to write to usart
 * \param[in] length The number of bytes to write
 * \param[in] timeout The millisecond of timeout
 *
 * \return The number of bytes written or <0 for timeout.
 */
static int32_t usart_os_write(struct io_descriptor *const io_descr, const uint8_t *const buf, const uint16_t length, uint32_t timeout)
{
	int ret;
	struct usart_os_descriptor *descr = CONTAINER_OF(io_descr, struct usart_os_descriptor, io);

	if (aos_mutex_lock(&descr->tx_mutex, AOS_WAIT_FOREVER) != 0) {
		return -1;
	}

	descr->tx_buffer        = (uint8_t *)buf;
	descr->tx_buffer_length = length;
	descr->tx_por           = 0;
	_usart_async_enable_byte_sent_irq(&descr->device);

	ret = sem_down(&descr->tx_sem, timeout);
	aos_mutex_unlock(&descr->tx_mutex);

	return (ret == 0) ? length : ERR_TIMEOUT;
}

/*
 * \internal Read data from usart interface
 *
 * \param[in] io_descr The pointer to an io descriptor
 * \param[in] buf A buffer to read data to
 * \param[in] length The size of a buffer
 * \param[in] timeout The millisecond of timeout
 *
 * \return The number of bytes user want to read.
 */
static int32_t usart_os_read(struct io_descriptor *const io_descr, uint8_t *const buf, const uint16_t length, uint32_t timeout)
{
	uint16_t was_read = 0;
	struct usart_os_descriptor *descr = CONTAINER_OF(io_descr, struct usart_os_descriptor, io);
	ASSERT(buf);

	if (aos_mutex_lock(&descr->rx_mutex, AOS_WAIT_FOREVER) != 0) {
		return -1;
	}

	if (ringbuffer_num(&descr->rx) < length) {
		descr->rx_size   = 0;
		descr->rx_length = length;
		descr->rx_buffer = buf;

		while (ringbuffer_num(&descr->rx) > 0) {
			ringbuffer_get(&descr->rx, &descr->rx_buffer[descr->rx_size++]);
		}

		if (sem_down(&descr->rx_sem, timeout) != 0) {
			aos_mutex_unlock(&descr->rx_mutex);
			return ERR_TIMEOUT;
		}
	} else {
		while (was_read < length) {
			ringbuffer_get(&descr->rx, &buf[was_read++]);
		}
	}
	aos_mutex_unlock(&descr->rx_mutex);
	return (int32_t)length;
}

/**
 * \brief Process "byte is sent" interrupt
 *
 * \param[in] device The pointer to device structure
 */
static void usart_os_process_byte_sent(struct _usart_async_device *device)
{
	struct usart_os_descriptor *descr = CONTAINER_OF(device, struct usart_os_descriptor, device);
	/* Flush buffer to remove unexpected data */
	ringbuffer_flush(&descr->rx);
	if (descr->tx_por != descr->tx_buffer_length) {
		_usart_async_write_byte(&descr->device, descr->tx_buffer[descr->tx_por++]);
		_usart_async_enable_byte_sent_irq(&descr->device);
	} else {
		_usart_async_enable_tx_done_irq(&descr->device);
	}
}

/**
 * \brief Process completion of data sending
 *
 * \param[in] device The pointer to device structure
 */
static void usart_os_transmission_complete(struct _usart_async_device *device)
{
	struct usart_os_descriptor *descr = CONTAINER_OF(device, struct usart_os_descriptor, device);

	sem_up(&descr->tx_sem);
}

/**
 * \brief Process byte reception
 *
 * \param[in] device The pointer to device structure
 * \param[in] data Data read
 */
static void usart_os_fill_rx_buffer(struct _usart_async_device *device, uint8_t data)
{
	struct usart_os_descriptor *descr = CONTAINER_OF(device, struct usart_os_descriptor, device);

	if (descr->rx_buffer == NULL) {
		ringbuffer_put(&descr->rx, data);
	} else {
		descr->rx_buffer[descr->rx_size++] = data;

		if (descr->rx_size >= descr->rx_length) {
			descr->rx_buffer = NULL;
			sem_up(&descr->rx_sem);
		}
	}
}

/**
 * \brief Process error interrupt
 *
 * \param[in] device The pointer to device structure
 */
static void usart_os_error(struct _usart_async_device *device)
{
	struct usart_os_descriptor *descr = CONTAINER_OF(device, struct usart_os_descriptor, device);

	sem_up(&descr->rx_sem);
}

/*
 * \internal Write the given data to usart interface
 *
 * \param[in] descr The pointer to an io descriptor
 * \param[in] buf Data to write to usart
 * \param[in] length The number of bytes to write
 * \param[in] timeout The millisecond of timeout
 *
 * \return The number of bytes written.
 */
static int32_t usart_sync_write(struct io_descriptor *const io_descr, const uint8_t *const buf, const uint16_t length, uint32_t timeout)
{
	ASSERT(io_descr && buf && length);

	uint32_t offset = 0;
	struct usart_os_descriptor *descr = CONTAINER_OF(io_descr, struct usart_os_descriptor, io);

	if (aos_mutex_lock(&descr->tx_mutex, AOS_WAIT_FOREVER) != 0) {
		return -1;
	}

	do {
		_usart_sync_write_byte(&descr->sync_device, buf[offset]);
		while (!_usart_sync_is_ready_to_send(&descr->sync_device)) {
		};
	} while (++offset < length);
	while (!_usart_sync_is_transmit_done(&descr->sync_device)) {
	};
	aos_mutex_unlock(&descr->tx_mutex);

	return offset;
}

/*
 * \internal Read data from usart interface
 *
 * \param[in] descr The pointer to an io descriptor
 * \param[in] buf A buffer to read data to
 * \param[in] length The size of a buffer
 * \param[in] timeout The millisecond of timeout
 *
 * \return The number of bytes read.
 */
static int32_t usart_async_read(struct io_descriptor *const io_descr, uint8_t *const buf, const uint16_t length, uint32_t timeout)
{
	uint16_t was_read = 0;
	uint32_t num;
	size_t rev_size = 0;
	int32_t ret;
	struct usart_os_descriptor *descr = CONTAINER_OF(io_descr, struct usart_os_descriptor, io);

	ASSERT(descr && buf && length);
	if (aos_mutex_lock(&descr->rx_mutex, AOS_WAIT_FOREVER) != 0) {
		return -1;
	}
	while (was_read < length) {
		ret = krhino_buf_queue_recv(&descr->kbuf, timeout, &buf[was_read], &rev_size);
		if((ret == 0) && (rev_size == 1)) {
			was_read++;
		} else {
			break;
		}
	}
	aos_mutex_unlock(&descr->rx_mutex);
	return (int32_t)was_read;
}

/**
 * \brief Process byte reception
 *
 * \param[in] device The pointer to device structure
 * \param[in] data Data read
 */
static void usart_fill_rx_buffer(struct _usart_async_device *device, uint8_t data)
{
	struct usart_os_descriptor *descr = CONTAINER_OF(device, struct usart_os_descriptor, device);

	krhino_buf_queue_send(&descr->kbuf, &data, 1);
}

//@}
