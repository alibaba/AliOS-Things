/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH.
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
 * @ingroup BCDS_HAL_MCU_IF
 * @defgroup BCDS_HAL_MCU_LEUART  LEUART
 * @{
 * @brief Unified LEUART (Universal Asynchronous Receiver Transmitter) Interface
 *
 * @details A typical use of this API would start with initializing the LEUART instance.
 * This will initializes the LEUART with default settings.
 *
 * @code
 * retcode = MCU_LEUART_Initialize(leuart, callback);
 * @endcode
 *
 * Now, the LEUART is configured and active such that the communication can start.
 * The application should use the function MCU_LEUART_Receive() to receive a certain
 * number bytes. The call triggers the
 * receiving process and returns. The callback is invoked with an RxComplete
 * event when the expected number of bytes has been received.
 *
 * For sending, the application uses the function MCU_LEUART_Send(), which is 
 * non-blocking. A callback will happen to notify the application about the 
 * success or failure of the sending operation.
 *
 * The callback may be also called for other events such as errors.
 *
 * @note The callback function is usually invoked in the ISR context. The
 * implementation of the callback function must take this into account.

 * @note It is possible to disable the entire LEUART feature in the HAL-BSP
 * configuration file. I.e. the LEUART code will only be compiled and linked if
 * the feature <b>BCDS_FEATURE_LEUART</b> is set to 1 in the configuration file.
 *
 * @note The driver may work in an interrupt- or DMA-based mode. This depends
 * on the implementation of the driver as well as on the way how the
 * UART hardware is initialized and configured in the BSP.
 *
 * @file
 */

#ifndef BCDS_MCU_LEUART_H_
#define BCDS_MCU_LEUART_H_

#include "BCDS_HAL.h"
#include "BCDS_HALConfig.h"
/* Code is only effective if feature is enabled in configuration */
#if BCDS_FEATURE_LEUART

/**
 * @brief   Data type for the LEUART handler type which is used to identify the
 * LEUART component to work with.
 */
typedef HWHandle_T LEUART_T;

/**
 * @brief   Structure to represent the events received from the LEUART. It is a
 * bit field where every bit represents a particular event.
 */
struct MCU_LEUART_Event_S
{
    uint32_t RxError :1; /**<  Some Receiver error has occurred. */
    uint32_t RxComplete :1; /**< The expected bytes have been received. This
     event follows the call of MCU_LEUART_Receive(). It
     is only relevant when configured for
     non-blocking transmissions. */
    uint32_t TxError :1; /**< Some transmitter error has occurred. This event
     is only relevant when configured for
     non-blocking transmissions. */
    uint32_t TxComplete :1; /**< All bytes passed with the last call of
     MCU_LEUART_Send() are sent. This event is only
     relevant when configured for non-blocking
     transmissions. */
    uint32_t Cts :1; /**< CTS (Clear To Send) line state has changed. */
    uint32_t Dsr :1; /**< DSR (Data Set Ready) line state has changed. */
    uint32_t Dcd :1; /**< DCD (Data Carrier Detect) line state has changed. */
    uint32_t Ri :1; /**< RI (Ring Indicator) line state has changed. */
	uint32_t Unused :24;
};

/**
 * @brief Union grouping MCU_LEUART_Event_S structure to an uint32_t value
 */
union MCU_LEUART_Event_U
{
    struct MCU_LEUART_Event_S bitfield;
    uint32_t registerValue;
};

/**
 * @brief enumeration of special LEUART interface error return codes
 */
enum RETCODE_HAL_LEUART_E
{
    RETCODE_HAL_LEUART_DRIVER_RX_NOT_READY = RETCODE_FIRST_CUSTOM_CODE,
    RETCODE_HAL_LEUART_DRIVER_TX_NOT_READY,
    RETCODE_HAL_LEUART_DRIVER_RX_NOT_RUN,
};

/**
 * @brief   Data type definition for the LEUART callback function.
 *
 * The callback is registered at the initialization. It is invoked by the LEUART
 * driver to notify the application about any events which occurs in which LEUART
 * hardware or the driver.
 *
 * @param[in] leuart : The handle of leuart
 *
 * @param[in] event : The event information.
 *
 * @note An event is a bit field which may combine multiple events.
 *
 * @see MCU_LEUART_Initialize()
 */
typedef void (*MCU_LEUART_Callback_T)(LEUART_T leuart, struct MCU_LEUART_Event_S event);

/**
 * @brief  Initializes the LEUART instance.
 *
 * The initialization must be invoked before any use of the LEUART instance.
 * This command will not activate or turn-on the receiver or transmitter. This
 * means, still no communication can take place. A callback which is registered
 * here is used by the driver to notify the application about any events which
 * occur in the LEUART hardware or driver.
 *
 * @note The callback function is usually invoked in the ISR context.
 *
 * @param[in] leuart : LEUART handle.
 *
 * @param[in] callback : Function pointer to a callback function which handles
 * the LEUART events. A callback function should always be passed with this
 * function. Passing a NULL pointer is not allowed and will result in error.
 *
 * @retval RETCODE_OK on success, or an error code otherwise.
 */
Retcode_T MCU_LEUART_Initialize(LEUART_T leuart, MCU_LEUART_Callback_T callback);

/**
 * @brief   Deinitializes the LEUART instance.
 *
 * After calling this function, the LEUART instance must not be used anymore.
 *
 * @param[in] leuart : LEUART handle.
 *
 * @retval RETCODE_OK on success, or an error code otherwise.
 */
Retcode_T MCU_LEUART_Deinitialize(LEUART_T leuart);
/**
 * @brief   Sending data to the LEUART for transmission.
 *
 * This function is non-blocking. It just triggers the sending procedure.
 * The sending will happen asynchronously after the return of this function.
 * The return value informs only about the success or failure of triggering the
 * sending.
 * The application will be informed about the success or failure of the sending
 * using the callback with a suitable event, e.g. TxComplete in the successful
 * case.
 *
 * @note The LEUART instance must have been initialized and powered on.
 *
 * @note It is not allowed to call MCU_LEUART_Send() a
 * second time before the callback of the first call. Also the passed data
 * buffer must not be accessed by the application before the callback. After the
 * callback the buffer becomes again available to the application for further
 * use.
 *
 * @param[in]  leuart : LEUART handle.
 *
 * @param[in]  data :
 * Pointer to the buffer which contains the data to be transmitted. The data buffer
 * must not be accessed by the application before
 * the callback. In and after the callback the buffer becomes again available to the
 * application for further use.
 *
 * @param[in]  len : The length of the data to be transmitted.
 *
 * @return RETCODE_OK on successful send or an error code otherwise.
 */
Retcode_T MCU_LEUART_Send(LEUART_T leuart, uint8_t * data, uint32_t len);

/**
 * @brief   Receiving data from the RT.
 *
 * The application invokes this function to receive a certain number of bytes.
 * Receiving is non-blocking. The callback is invoked with an
 * RxComplete event once the expected number of bytes has been received.
 * The received bytes are stored in the provided buffer.
 *
 * Unlike I2C and SPI, LEUART (as UART) is designed to receive streams. Hence, one
 * call of MCU_LEUART_Receive(leuart, buffer, n) will result in continuously
 * receiving n bytes into the given buffer and invoking the callback. The
 * upper-layer does not need to call MCU_LEUART_Receive() again to receive the next
 * n bytes.
 *
 * In order to stop receiving, the upper-layer needs to call MCU_LEUART_Receive()
 * with the size of 0.
 *
 * The call MCU_LEUART_Receive() overrides any previous still active process.
 *
 * @param[in] leuart : LEUART handle.
 * @param[in] buffer : Pointer to the buffer in which the received data is to be
 *                   stored.
 * @param[in]  size : The length of the data to be received. If 0, then any
 *                   previous receive process is cancelled.
 *
 * @retval RETCODE_OK on success or an error code otherwise.
 */
Retcode_T MCU_LEUART_Receive(LEUART_T leuart, uint8_t* data, uint32_t len);

/**
 * @brief   Number of bytes available in the receiving buffer.
 *
 * The application may use this function check the progress of an ongoing
 * receiving process.
 *
 * @param[in]  leuart  : LEUART handle.
 * @param[out] count : The number of available bytes.
 *
 * @return  returns RETCODE_OK on success, or an error code otherwise.
 */
Retcode_T MCU_LEUART_GetRxCount(LEUART_T leuart, uint32_t* count);

#endif /* BCDS_FEATURE_LEUART */
#endif /* BCDS_MCU_LEUART_H_ */
/**  @} */
