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
 * @defgroup BCDS_HAL_MCU_SPI  SPI
 *
 * @{
 * @brief Unified SPI (Serial Peripheral Interface) API
 *
 * @details A typical use of this API would start with initializing the SPI instance.
 * This will initializes the SPI with default settings.
 *
 * @code
 * retcode = MCU_SPI_Initialize(spi, callback);
 * @endcode
 *
 * Now, the SPI is configured and active such that the communication can start.
 * The application should use the function MCU_SPI_Receive() to receive a certain
 * number bytes.
 *
 * For sending, the application uses the function MCU_SPI_Send().
 *
 * @note It is possible to disable the entire SPI feature in the HAL-BSP
 * configuration file. I.e. the SPI code will only be compiled and linked if
 * the feature <b>BCDS_FEATURE_SPI</b> is set to 1 in the configuration file.
 *
 * @file
 */

#ifndef BCDS_MCU_SPI_H
#define BCDS_MCU_SPI_H

#include "BCDS_HAL.h"

/* Code is only effective if feature is enabled in configuration */
#if BCDS_FEATURE_SPI && BCDS_SPI_COUNT

#include "BCDS_Basics.h"

/**
 * @brief Typedef for the SPI handler type which is used to identify the
 * SPI component to work with.
 */
typedef HWHandle_T SPI_T;

/**
 * @brief Structure to represent the events that can be received from the SPI
 * in the callback function.
 */
struct MCU_SPI_Event_S
{
    uint32_t RxReady    :1; /**< Occurs after one byte has arrived. */
    uint32_t RxError    :1; /**<  Some Receiver error has occurred. */
    uint32_t RxComplete :1; /**< The expected bytes have been received. This
                                 event follows the call of SPI_Receive(). */
    uint32_t TxError    :1; /**< Some transmitter error has occurred. */
    uint32_t TxComplete :1; /**< All bytes passed with the last call of
                                 MCU_SPI_Send() are sent. */
    uint32_t DataLoss   :1; /**< Occurs upon :
                             *  - Bus error.*/
};
/**
 * @brief union to combine event bitfield structure with a uint32_t value
 */
union MCU_SPI_Event_U
{
    struct MCU_SPI_Event_S bitfield;
    uint32_t registerValue;

};

/**
 * @brief enumeration of special Uart interface error return codes
 */
enum RETCODE_HAL_SPI_E
{
    RETCODE_HAL_SPI_DRIVER_RX_NOT_READY = RETCODE_FIRST_CUSTOM_CODE,
    RETCODE_HAL_SPI_DRIVER_TX_NOT_READY,
    RETCODE_HAL_SPI_DRIVER_RX_NOT_RUN,
    RETCODE_HAL_SPI_DRIVER_RESSOURCE_NOT_AVAILABLE,
};

typedef void * SPI_Owner_T;

/**
 * @brief This data type represents a function pointer which is used by SPI as
 * a callback whenever an event is to be notified to the application.
 *
 * @details Upon initialization of SPI the user should pass a function pointer
 * of this type to be able to receive event notifications from the SPI driver.
 *
 * @param [in] event : Structure containing actual event information.
 * @see SPI_Initialize()
 */
typedef void (*MCU_SPI_Callback_T)(SPI_T spi, struct MCU_SPI_Event_S event);

/**
 * @brief   Initialize the SPI Interface.
 *
 * @details Call this function before any other SPI-API function if you want
 * to use SPI. It will initialize SPI to default values.
 *
 * @note All other SPI-API calls require a call to MCU_SPI_Initialize() first else
 * they will report an error.
 *
 * @param [in]  spi : SPI handle.
 *
 * @param [in] callback : Function pointer to a callback function
 *                        which handles the SPI events.
 *
 * @retval  RETCODE_OK upon successful execution.
 */
Retcode_T MCU_SPI_Initialize(SPI_T spi, MCU_SPI_Callback_T callback);

/**
 * @brief   Deinitializes and powers down the SPI interface.
 *
 * @param [in]  spi : SPI handle.
 *
 * @retval RETCODE_OK upon successful execution.
 */
Retcode_T MCU_SPI_Deinitialize(SPI_T spi);

/**
 * @brief   Sets the OS aware parent handle.
 *
 * @param [in]  spi : SPI handle.
 * @param [in]  parentHandle : parent handle.
 *
 * @retval RETCODE_OK upon successful execution or an error code otherwise (see #Retcode_T for other codes).
 */
Retcode_T MCU_SPI_SetOwner(SPI_T spi, SPI_Owner_T parentHandle);

/**
 * @brief   Gets the OS aware parent handle.
 *
 * @param [in]  spi : SPI handle.
 *
 * @retval parent handle mapped to the SPI handle. Could be NULL if not initialized.
 */
SPI_Owner_T MCU_SPI_GetOwner(SPI_T spi);

/**
 * @brief Use this function to send data via SPI.
 * @details
 * The send operation may be configured to be blocking or non-blocking.
 * In blocking mode data will be sent completely over the bus before the
 * return of this function. The return value will then inform about the success
 * or failure of the operation.
 *
 * In <b>non blocking</b> mode a call to this function just starts sending the
 * data.
 * Sending will then happen asynchronously. In this case the return value just
 * informs about the success or failure of starting the send operation. The
 * application will then be informed by the driver via callback about the
 * success (TxComplete) or failure (TxError, DataLoss) of the send
 * operation.
 *
 * @note In non-blocking mode, it is not allowed to call MCU_SPI_Send() a
 * second time before the callback of the first call. Also the data buffer must
 * stay allocated and the contents of unsent data must not be modified. When
 * send operation is completed (requested number of items sent), the TxComplete
 * event is generated.
 *
 * @note The SPI instance must have been initialized and powered first.
 *
 * @note The function only supports transfers with 8 data bits or less.
 *
 * @param [in] spi : The handle of the SPI.
 * @param [in] data : Is a pointer on the buffer containing the data to be send.
 * @param [in] len : is the number of data elements to send. If 0 any pending
 * transmission will be canceled.
 * @retval RETCODE_OK upon successful execution.
 */
Retcode_T MCU_SPI_Send(SPI_T spi, uint8_t * data, uint32_t len);

/**
 * @brief Receives data via SPI
 * @details Use the function to receive data via SPI.
 *
 * A call to this function starts receiving the data.
 *
 * Receiving happens either in blocking or non-blocking modes.
 * In case of non blocking mode the return value just informs about the success
 * or failure of starting the receive operation. The application will then be
 * informed by the driver via callback about the success (RxComplete) or failure
 * (RxError, DataLoss) of the receive operation.
 * In case of blocking mode the function returns upon reception of the intended
 * number of bytes or upon error.
 *
 * @note in non-blocking mode it is not allowed to call this function a second
 * time before the callback of the first call else a pending transmission will
 * be canceled. Also the data buffer must stay allocated and the contents
 * must not be modified. When the receive operation is completed
 * (requested number of items received), the RxComplete event is generated.

 * @note The SPI instance must have been initialized and powered first.
 *
 * @note The function only supports transfers with 8 data bits or less.
 *
 * @param [in] spi : Is the handle of the SPI.
 * @param [out] buffer : Is a pointer on the buffer into which to store
 * received data.
 *
 * @note The developer has to ensure that the receive buffer is empty before
 * starting reception otherwise unexpected behaviour could occure.
 *
 * @param [in] len is the number of data elements expected to be received. If 0
 * then any pending transmission will be cancelled.
 * @retval RETCODE_OK upon successful execution.
 */
Retcode_T MCU_SPI_Receive(SPI_T spi, uint8_t * buffer, uint32_t len);

/**
 * @brief Start send/receive data to/from SPI transmitter/receiver.
 *
 * The function MCU_SPI_Transfer() transfers data via SPI. It synchronously
 * sends data to the SPI transmitter and receives data from the SPI receiver.
 *
 * Calling the function starts the transfer operation.
 * in non-blocking call mode it returns as soon as the driver has started the operation
 * (driver typically configures the interrupt system for continuous transfer).
 * In blocking mode the function return on completion of transfer or on error
 *
 * @note The function only supports transfers with 8 data bits or less.
 *
 * @param [in] spi : SPI handle.
 *
 * @param [in] data_out : Pointer to buffer with the data to send.
 *
 * @param [out] data_in : Pointer to buffer into which received data will be
 *                        stored.
 *
 * @param [in] numTransfer : The number of data items to transfer.
 *
 * @retval RETCODE_OK upon successful operation
 */

Retcode_T MCU_SPI_Transfer(SPI_T spi, uint8_t* data_out, uint8_t* data_in,
                      uint32_t numTransfer);


/**
 * @brief Get the transfered data count.
 *
 * The Function returns the number of currently transferred data items
 * during MCU_SPI_Send(), MCU_SPI_Receive(), MCU_SPI_Transfer() operation.
 *
 * @param [in] spi : SPI handle.
 *
 * @return Returns the transfered data count
 */
uint32_t MCU_SPI_GetDataCount(SPI_T spi);


#endif /* BCDS_FEATURE_SPI */
#endif /* BCDS_MCU_SPI_H */
/**  @} */
