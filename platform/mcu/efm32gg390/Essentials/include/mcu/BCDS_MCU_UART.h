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
 *
 * @ingroup BCDS_HAL_MCU_IF
 * @defgroup BCDS_HAL_MCU_UART  UART
 *
 * @{
 * @brief Unified UART (Universal Asynchronous Receiver Transmitter) Interface
 *
 * @details A typical use of this API would start with initializing the UART instance.
 * This will initializes the UART with default settings.
 *
 * @code
 * retcode = MCU_UART_Initialize(uart, callback);
 * @endcode
 *
 * Now, the UART is configured and active such that the communication can start.
 * The application should use the function MCU_UART_Receive() to receive a certain
 * number bytes. If receiving is non-blocking, the call triggers the
 * receiving process and returns. The callback is invoked with an RxComplete
 * event when the expected number of bytes has been received.
 *
 * For sending, the application uses the function MCU_UART_Send(). In a
 * non-blocking scenario a callback will happen to notify the application about
 * the success or failure of the sending operation.
 *
 * The callback may be also called for other events such as errors.
 *
 * @note The callback function is usually invoked in the ISR context. The
 * implementation of the callback function must take this into account.

 * @note It is possible to disable the entire UART feature in the HAL-BSP
 * configuration file. I.e. the UART code will only be compiled and linked if
 * the feature <b>BCDS_FEATURE_UART</b> is set to 1 in the configuration file.
 *
 * @note The driver may work in an interrupt- or DMA-based mode. This depends
 * on the implementation of the driver as well as on the way how the
 * UART hardware is initialized and configured in the BSP.
 *
 * @file
 */

#ifndef BCDS_MCU_UART_H_
#define BCDS_MCU_UART_H_

#include "BCDS_HAL.h"

/* Code is only effective if feature is enabled in configuration */
#if BCDS_FEATURE_UART

/**
 * @brief   Data type for the UART handler type which is used to identify the
 * UART component to work with.
 */
typedef HWHandle_T UART_T;

/**
 * @brief   Structure to represent the events received from the UART. It is a
 * bit field where every bit represents a particular event.
 */
struct MCU_UART_Event_S
{
    uint32_t RxError :1; /**<  Some Receiver error has occurred. */
    uint32_t RxComplete :1; /**< The expected bytes have been received. This
     event follows the call of MCU_UART_Receive(). It
     is only relevant when configured for
     non-blocking transmissions. */
    uint32_t TxError :1; /**< Some transmitter error has occurred. This event
     is only relevant when configured for
     non-blocking transmissions. */
    uint32_t TxComplete :1; /**< All bytes passed with the last call of
     MCU_UART_Send() are sent. This event is only
     relevant when configured for non-blocking
     transmissions. */
    uint32_t Cts :1; /**< CTS (Clear To Send) line state has changed. */
    uint32_t Dsr :1; /**< DSR (Data Set Ready) line state has changed. */
    uint32_t Dcd :1; /**< DCD (Data Carrier Detect) line state has changed. */
    uint32_t Ri :1; /**< RI (Ring Indicator) line state has changed. */
    uint32_t Unused :24;
};

/**
 * @brief Union grouping MCU_UART_Event_S structure to an uint32_t value
 */
union MCU_UART_Event_U
{
    struct MCU_UART_Event_S bitfield;
    uint32_t registerValue;
};

/**
 * @brief enumeration of special Uart interface error return codes
 */
enum RETCODE_HAL_UART_E
{
    RETCODE_HAL_UART_DRIVER_RX_NOT_READY = RETCODE_FIRST_CUSTOM_CODE,
    RETCODE_HAL_UART_DRIVER_TX_NOT_READY,
    RETCODE_HAL_UART_DRIVER_RX_NOT_RUN,
    RETCODE_HAL_UART_DRIVER_RESSOURCE_NOT_AVAILABLE,
};
/**
 * @brief   Data type definition for the UART callback function.
 *
 * The callback is registered at the initialization. It is invoked by the UART
 * driver to notify the application about any events which occurs in which UART
 * hardware or the driver.
 *
 * @param[in] uart : The handle of uart
 *
 * @param[in] event : The event information.
 *
 * @note An event is a bit field which may combine multiple events.
 *
 * @see MCU_UART_Initialize()
 */
typedef void (*MCU_UART_Callback_T)(UART_T uart, struct MCU_UART_Event_S event);

/**
 * @brief  Initializes the UART instance.
 *
 * The initialization must be invoked before any use of the UART instance.
 * This command will not activate or turn-on the receiver or transmitter. This
 * means, still no communication can take place. A callback which is registered
 * here is used by the driver to notify the application about any events which
 * occur in the UART hardware or driver.
 *
 * @note The callback function is usually invoked in the ISR context.
 *
 * @param[in] uart : UART handle.
 *
 * @param[in] callback : Function pointer to a callback function which handles
 * the UART events. A callback function should always be passed with this
 * function in case of non blocking mode, Passing a NULL pointer in this case is
 * not allowed and will result in error.
 *
 * @retval RETCODE_OK on success, or an error code otherwise.
 */
Retcode_T MCU_UART_Initialize(UART_T uart, MCU_UART_Callback_T callback);

/**
 * @brief   Deinitializes the UART instance.
 *
 * After calling this function, the UART instance must not be used anymore.
 *
 * @param[in] uart : UART handle.
 *
 * @retval RETCODE_OK on success, or an error code otherwise.
 */
Retcode_T MCU_UART_Deinitialize(UART_T uart);


/**
 * @brief   Sets the OS aware parent handle.
 *
 * @param[in] uart : UART handle.
 * @param [in]  parentHandle : parent handle.
 *
 * @retval RETCODE_OK upon successful execution or an error code otherwise (see #Retcode_T for other codes).
 */
Retcode_T MCU_UART_SetParentHandle(UART_T uart, SWHandle_T parentHandle);

/**
 * @brief   Gets the OS aware parent handle.
 *
 * @param[in] uart : UART handle.
 *
 * @retval parent handle mapped to the SPI handle. Could be NULL if not initialized.
 */
SWHandle_T MCU_UART_GetParentHandle(UART_T uart);

/**
 * @brief   Sending data to the UART for transmission.
 *
 * The call to this function could be blocking or non-blocking.
 * In the non-blocking call case it just triggers the sending procedure.
 * The sending will happen asynchronously after the return of this function.
 * The return value informs only about the success or failure of triggering the
 * sending. The application will be informed about the success or failure of the
 * sending using the callback with a suitable event, e.g. TxComplete in the
 * successful case.
 *
 * In the blocking call case the function will return upon completion of the
 * operation or upon error.
 *
 * @note The UART instance must have been initialized and powered on.
 *
 * @note in non blocking call mode is not allowed to call MCU_UART_Send() a
 * second time before the callback of the first call. Also the passed data
 * buffer must not be accessed by the application before the callback. After the
 * callback the buffer becomes again available to the application for further
 * use.
 *
 * @param[in]  uart : UART handle.
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
Retcode_T MCU_UART_Send(UART_T uart, uint8_t * data, uint32_t len);

/**
 * @brief   Receiving data from the UART.
 *
 * The application invokes this function to receive a certain number of bytes.
 * Receiving is either blocking or non-blocking.
 * In blocking mode the function will return upon receiving of the desired amount
 * of data or in case an error occurred.
 *
 * In non-blocking call mode the callback is invoked with an RxComplete event
 * once the expected number of bytes has been received.The received bytes are
 * stored in the provided buffer. Unlike I2C and SPI, UART is designed to
 * receive streams. Hence, one call of MCU_UART_Receive(uart, buffer, n) in
 * non-blocking mode  will result in continuously receiving n bytes into the
 * given buffer and invoking the callback. The upper-layer does not need to call
 * MCU_UART_Receive() again to receive the next n bytes.
 *
 * In order to stop receiving, the upper-layer needs to call MCU_UART_Receive()
 * with the size of 0.
 * The non-blocking call to MCU_UART_Receive() overrides any previous still
 * active process.
 *
 * @param[in] uart : UART handle.
 * @param[in] buffer : Pointer to the buffer in which the received data is to be
 *                   stored.
 * @param[in]  size : The length of the data to be received. If 0, then any
 *                   previous receive process is cancelled.
 *
 * @retval RETCODE_OK on success or an error code otherwise.
 */
Retcode_T MCU_UART_Receive(UART_T uart, uint8_t* buffer, uint32_t size);

/**
 * @brief   Number of bytes available in the receiving buffer.
 *
 * The application may use this function check the progress of an ongoing
 * receiving process.
 *
 * @param[in]  uart  : UART handle.
 * @param[out] count : The number of available bytes.
 *
 * @retval  RETCODE_OK on success, or an error code otherwise.
 */
Retcode_T MCU_UART_GetRxCount(UART_T uart, uint32_t* count);


#endif /* BCDS_FEATURE_UART */
#endif /* BCDS_MCU_UART_H_ */
/**  @} */
