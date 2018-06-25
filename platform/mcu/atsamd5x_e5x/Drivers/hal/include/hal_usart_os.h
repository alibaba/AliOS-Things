/**
 * \file
 *
 * \brief USART related functionality declaration.
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

#ifndef _HAL_USART_OS_H_INCLUDED
#define _HAL_USART_OS_H_INCLUDED

#include "hal_io.h"
#include <hal_rtos.h>
#include <hpl_usart_async.h>
#include <hpl_usart_sync.h>
#include <utils_ringbuffer.h>
#include <k_api.h>
/**
 * \addtogroup doc_driver_hal_usart_os
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief USART descriptor
 *
 * The USART descriptor forward declaration.
 */
struct usart_os_descriptor;

/**
 * \brief Asynchronous USART descriptor structure
 */
struct usart_os_descriptor {
	struct io_descriptor       io;
	struct _usart_async_device device;
	struct _usart_sync_device  sync_device;

        kbuf_queue_t kbuf;
        uint8_t *kbuf_data;
	struct ringbuffer rx;
	uint8_t *         rx_buffer;
	uint16_t          rx_size;
	uint16_t          rx_length;

	uint8_t *tx_buffer;
	uint16_t tx_por;
	uint16_t tx_buffer_length;

	aos_sem_t rx_sem;
	aos_sem_t tx_sem;

	aos_mutex_t rx_mutex; /* Mutex used for protect rx buffer, will be released until previous rx finished */
	aos_mutex_t tx_mutex; /* Mutex used for protect tx buffer, will be released until previous tx finished */
};
/**
 * \brief Initialize USART interface
 *
 * This function initializes the given I/O descriptor to be used as USART
 * interface descriptor.
 * It checks if the given hardware is not initialized and if the given hardware
 * is permitted to be initialized.
 *
 * \param[out] descr A USART descriptor which is used to communicate via the USART
 * \param[in] hw The pointer to the hardware instance
 * \param[in] rx_buffer An RX buffer
 * \param[in] rx_buffer_length The length of the buffer above
 *
 * \return Initialization status.
 * \retval -1 Passed parameters were invalid or the interface is already
 * initialized
 * \retval 0 The initialization is completed successfully
 */
int32_t usart_os_init(struct usart_os_descriptor *const descr, void *const hw, uint8_t *const rx_buffer,
                      const uint16_t rx_buffer_length, void *const func);

/**
 * \brief Deinitialize USART interface
 *
 * This function deinitializes the given I/O descriptor.
 * It checks if the given hardware is initialized and if the given hardware
 * is permitted to be deinitialized.
 *
 * \param[in] descr A USART descriptor which is used to communicate via USART
 *
 * \return De-initialization status.
 * \retval -1 Passed parameters were invalid or the interface is already
 * deinitialized
 * \retval 0 The de-initialization is completed successfully
 */
int32_t usart_os_deinit(struct usart_os_descriptor *const descr);

/**
 * \brief Enable USART interface
 *
 * Enables the USART interface
 *
 * \param[in] descr A USART descriptor which is used to communicate via USART
 *
 * \return Enabling status.
 */
int32_t usart_os_enable(struct usart_os_descriptor *const descr);

/**
 * \brief Disable USART interface
 *
 * Disables the USART interface
 *
 * \param[in] descr A USART descriptor which is used to communicate via USART
 *
 * \return Disabling status.
 */
int32_t usart_os_disable(struct usart_os_descriptor *const descr);

/**
 * \brief Retrieve I/O descriptor
 *
 * This function retrieves the I/O descriptor of the given USART descriptor.
 *
 * \param[in] descr A USART descriptor which is used to communicate via USART
 * \param[out] io An I/O descriptor to retrieve
 *
 * \return The status of I/O descriptor retrieving.
 * \retval -1 Passed parameters were invalid or the interface is already
 * deinitialized
 * \retval 0 The retrieving is completed successfully
 */
int32_t usart_os_get_io(struct usart_os_descriptor *const descr, struct io_descriptor **io);

/**
 * \brief Specify action for flow control pins
 *
 * This function sets the action (or state) for flow control pins if
 * the flow control is enabled.
 * It sets the state of flow control pins only if the automatic support of
 * the flow control is not supported by the hardware.
 *
 * \param[in] descr A USART descriptor which is used to communicate via USART
 * \param[in] state A state to set the flow control pins
 *
 * \return The status of the flow control action setup.
 * \retval -1 Passed parameters were invalid or the interface is not initialized
 * \retval 0 The flow control action is set successfully
 */
int32_t usart_os_set_flow_control(struct usart_os_descriptor *const descr, const union usart_flow_control_state state);

/**
 * \brief Set USART baud rate
 *
 * \param[in] descr A USART descriptor which is used to communicate via USART
 * \param[in] baud_rate A baud rate to set
 *
 * \return The status of the baudrate setting.
 * \retval -1 Passed parameters were invalid or the interface is not initialized
 * \retval 0 The flow control action is set successfully
 */
int32_t usart_os_set_baud_rate(struct usart_os_descriptor *const descr, const uint32_t baud_rate);

/**
 * \brief Set USART data order
 *
 * \param[in] descr A USART descriptor which is used to communicate via USART
 * \param[in] data_order A data order to set
 *
 * \return The status of data order setting.
 * \retval -1 Passed parameters were invalid or the interface is not initialized
 * \retval 0 The flow control action is set successfully
 */
int32_t usart_os_set_data_order(struct usart_os_descriptor *const descr, const enum usart_data_order data_order);

/**
 * \brief Set USART mode
 *
 * \param[in] descr A USART descriptor which is used to communicate via USART
 * \param[in] mode A mode to set
 *
 * \return The status of mode setting.
 * \retval -1 Passed parameters were invalid or the interface is not initialized
 * \retval 0 The flow control action is set successfully
 */
int32_t usart_os_set_mode(struct usart_os_descriptor *const descr, const enum usart_mode mode);

/**
 * \brief Set USART parity
 *
 * \param[in] descr A USART descriptor which is used to communicate via USART
 * \param[in] parity A parity to set
 *
 * \return The status of parity setting.
 * \retval -1 Passed parameters were invalid or the interface is not initialized
 * \retval 0 The flow control action is set successfully
 */
int32_t usart_os_set_parity(struct usart_os_descriptor *const descr, const enum usart_parity parity);

/**
 * \brief Set USART stop bits
 *
 * \param[in] descr A USART descriptor which is used to communicate via USART
 * \param[in] stop_bits Stop bits to set
 *
 * \return The status of stop bits setting.
 * \retval -1 Passed parameters were invalid or the interface is not initialized
 * \retval 0 The flow control action is set successfully
 */
int32_t usart_os_set_stopbits(struct usart_os_descriptor *const descr, const enum usart_stop_bits stop_bits);

/**
 * \brief Set USART character size
 *
 * \param[in] descr A USART descriptor which is used to communicate via USART
 * \param[in] size A character size to set
 *
 * \return The status of character size setting.
 * \retval -1 Passed parameters were invalid or the interface is not initialized
 * \retval 0 The flow control action is set successfully
 */
int32_t usart_os_set_character_size(struct usart_os_descriptor *const descr, const enum usart_character_size size);

/**
 * \brief Retrieve the state of flow control pins
 *
 * This function retrieves the flow control pins
 * if the flow control is enabled.
 * The function can return UASRT_OS_FLOW_CONTROL_STATE_UNAVAILABLE in case
 * if the flow control is done by the hardware
 * and pins state cannot be read out.
 *
 * \param[in] descr A USART descriptor which is used to communicate via USART
 * \param[out] state The state of flow control pins
 *
 * \return The status of flow control state reading.
 * \retval -1 Passed parameters were invalid or the interface is not initialized
 * \retval 0 The flow control state is retrieved successfully
 */
int32_t usart_os_flow_control_status(const struct usart_os_descriptor *const descr,
                                     union usart_flow_control_state *const   state);

/**
 * \brief flush USART ringbuf
 *
 * This function flush USART RX ringbuf.
 *
 * \param[in] descr The pointer to USART descriptor
 *
 * \return ERR_NONE
 */
int32_t usart_os_flush_rx_buffer(struct usart_os_descriptor *const descr);

/**
 * \brief Retrieve the current driver version
 *
 * \return Current driver version.
 */
uint32_t usart_os_get_version(void);

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HAL_USART_OS_H_INCLUDED */
