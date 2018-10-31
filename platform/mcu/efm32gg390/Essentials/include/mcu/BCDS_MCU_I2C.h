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
 *  @ingroup BCDS_HAL_MCU_IF
 *  @defgroup BCDS_HAL_MCU_I2C I2C
 * @{
 * @brief Unified I2C Master Interface
 *
 * @details Usually the I2C hardware gets initialized by the HAL-BSP (Board support
 * package).
 * The application should then be able to get a handle on a fully initialized
 * I2C device that can be used by this driver to send and receive data.
 *
 * A typical use of this API would start with initializing the driver
 * instance with a correct handle, and a callback function for the application
 * layer in case the call mode is not blocking. This will initialize the driver
 * and bind it to the handle of the BSP.
 * After that it is ready to use according to settings made in the BSP.
 *
 * @code
 * Retcode_T retcode = MCU_I2C_Initialize(I2c2, MyI2CCallBack);
 * if(RETCODE_OK == retcode)
 * {
 *      // use the I2C driver
 * }
 * @endcode
 *
 * Now, the I2C is configured and active such that the communication can start.
 *
 * @note It is possible to disable the entire I2C feature in the HAL-BSP
 * configuration file. I.e. the I2C code will only be compiled and linked if
 * the feature <b>BCDS_FEATURE_I2C</b> is set to 1 in the configuration
 * file.
 *
 * @note This interface only supports the I2C master mode.
 *
 * @attention This I2C interface only supports 7 bit addresses for slave addressing.
 *
 * @file
 * ****************************************************************************/
#ifndef BCDS_MCU_I2C_H
#define BCDS_MCU_I2C_H

#include "BCDS_HAL.h"
/* Code is only effective if feature is enabled in configuration */
#if BCDS_FEATURE_I2C && BCDS_I2C_COUNT

#include "BCDS_Basics.h"
/**
 * @brief Typedef for the I2C handler type which is used to identify the
 * I2C component to work with.
 */
typedef HWHandle_T I2C_T;

/**
 * @brief Bit field structure to represent the events that can be received from
 * the I2C in the callback function.
 */
struct MCU_I2C_Event_S
{
    uint32_t RxReady :1; /**<   Occurs after one byte has arrived. This event is
     only relevant if receiving is set to non-blocking.*/
    uint32_t TransferError :1; /**<  Some Transfer error has occurred. */
    uint32_t RxComplete :1; /**< The expected bytes have been received. This
     event follows the call of MCU_UART_Receive(). It
     is only relevant when configured for
     non-blocking transmissions. */
    uint32_t TxComplete :1; /**< All bytes passed with the last call of
     MCU_UART_Send() are sent. This event is only
     relevant when configured for non-blocking
     transmissions. */
};

/**
 * @brief union of MCU_I2C_Event_S and a scalar unint32_t for optimized access
 */
union MCU_I2C_Event_U
{
    uint32_t value;
    struct MCU_I2C_Event_S bits;
};

/**
 * @brief This data type represents a function pointer which is used by I2C
 *  as a callback whenever an event is to be notified to the application.
 *
 * @details Upon initialization of I2C the user should pass a function pointer
 * of this type to be able to receive event notifications from the I2C driver.
 *
 * @param [in] i2c : Is the handle of the I2C instance that sends the event.
 * @param [in] event : Structure containing actual event information.
 * @see MCU_I2C_Initialize()
 */
typedef void (*MCU_I2C_Callback_T)(I2C_T i2c, struct MCU_I2C_Event_S event);

/**
 * @brief   Initializes the I2C Interface.
 *
 * @details Call this function before any other I2C-API function if you want
 * to use I2C. It will initialize I2C to default values.
 *
 * @note All other I2C-API calls require a call to MCU_I2C_Initialize() first
 * else they will report an error.
 *
 * @param [in]  i2c : I2C handle.
 *
 * @param [in] callback : Function pointer to a callback function  which handles
 * the I2C events. A callback function pointer should be passed with this
 * function in case the decided call mode is non blocking. Passing a NULL
 * pointer is allowed only in case the call mode is blocking.
 *
 * @return A combined error code including the package ID and severity in the
 * upper 16 bits and the error value in the lower 16 bits.
 * @retval  RETCODE_OK upon successful execution.
 * @retval  RETCODE_IC2_INIT_FAIL with severity WARNING upon failure.
 */
Retcode_T MCU_I2C_Initialize(I2C_T i2c, MCU_I2C_Callback_T callback);

/**
 * @brief   Deinitializes and powers down the I2C interface.
 *
 * If applicable, releases the I/O pin resources of the interface. After
 * calling this function, the I2C interface must not be used anymore.
 *
 * @param [in]  i2c : I2C handle.
 *
 * @return A combined error code including the package ID and severity in the
 * upper 16 bits and the error value in the lower 16 bits.
 * @retval  RETCODE_OK upon successful execution.
 */
Retcode_T MCU_I2C_Deinitialize(I2C_T i2c);

/**
 * @brief   Sets the OS aware parent handle.
 *
 * @param [in]  i2c : I2C handle.
 * @param [in]  parentHandle : parent handle.
 *
 * @retval RETCODE_OK upon successful execution or an error code otherwise (see #Retcode_T for other codes).
 */
Retcode_T MCU_I2C_SetParentHandle(I2C_T i2c, SWHandle_T parentHandle);

/**
 * @brief   Gets the OS aware parent handle.
 *
 * @param [in]  i2c : I2C handle.
 *
 * @retval parent handle mapped to the SPI handle. Could be NULL if not initialized.
 */
SWHandle_T MCU_I2C_GetParentHandle(I2C_T i2c);

/**
 * @brief Use this function to send data via I2C.
 *
 * The send Operation could be blocking or non blocking in the non-blocking case,
 * this function just triggers the sending procedure. Data transmission will
 * happen asynchronously after the return of this function. The return value
 * informs only about success or failure of triggering the transmission and the
 * application will be informed about success or failure of the transmission via
 * callback.
 * In the Blocking Case the function will only return upon completion of the
 * send operation or if an error occurred
 *
 * @note The I2C instance must have been initialized and powered prior to this
 * function call.
 *
 * @param [in] i2c : The handle of the I2C.
 * @param [in] slaveAddress : The I2C address of the slave to which to send
 * the data.
 * @param [in] data : Is a pointer on the buffer containing the data to be send.
 * The the data buffer must not be accessed by the application before the
 * callback happens. After the callback the buffer becomes available to the
 * application for further use.
 * @param [in] len : Is the number of data elements to send. Passing 0 will
 * abort any pending non-blocking transmission.
 *
 * @retval RETCODE_OK on success or an error code otherwise.
 */
Retcode_T MCU_I2C_Send(I2C_T i2c, uint16_t slaveAddress,
        uint8_t * data, uint32_t len);

/**
 * @brief Receives data via I2C.
 *
 * Use the function to receive data via I2C. As I2C is a master-slave bus,
 * the master always has to call this function first in order to be able to
 * receive data from a connected slave.
 * Receiving is either blocking or non-blocking depending on what has been
 * decided at BSP level.
 *
 * In the non-blocking case the driver will call back the application with event
 * RxComplete when it has received the defined number of data elements from the
 * slave.
 * In the blocking call mode the function will return if the specified number of
 * bytes have been received or if an error occurred.
 * The received data is stored in the provided buffer.
 * The driver will signal any error if the transfer fails.
 *
 * @param [in] i2c : Is the handle of the I2C.
 * @param [in] slaveAddr : The I2C address of the slave from which to
 * read the data.
 * @param [out] buffer : Is a pointer on the buffer into which to store
 * received data. The data buffer must not be accessed by the application before
 * the callback happens. After the callback the buffer becomes available to the
 * application for further use.
 * @param [in] len is the number of data elements expected to be received. If
 * you pass 0 here, then any pending receive will be cancelled.
 *
 * @retval RETCODE_OK on success or an error code otherwise.
 */
Retcode_T MCU_I2C_Receive(I2C_T i2c, uint16_t slaveAddr,
        uint8_t * buffer, uint32_t len);

/**
 * @brief Use this function to read a register of a device connected to I2C.
 * @param [in] i2c : Is the handle of the I2C.
 * @param [in] slaveAddr : Is the I2C address of the connected device from which
 * to read the register.
 * @param [in] registerAddr : Is the register address of the device to read.
 * @param [out] rxBuffer : Is a pointer on a buffer into which the read values
 * will be stored.
 * @param [in] rxLen : Is the length the number of data elements expected to be
 * received
 *
 * @retval RETCODE_OK on success or an error code otherwise.
 */
Retcode_T MCU_I2C_ReadRegister(I2C_T i2c, uint16_t slaveAddr, uint8_t registerAddr,
        uint8_t * rxBuffer, uint32_t rxLen);
/**
 * @brief Use this function to write a register of a device connected to I2C.
 * @param [in] i2c : Is the handle of the I2C.
 * @param [in] slaveAddr : Is the I2C address of the connected device to which
 * to write the register.
 * @param [in] registerAddr : Is the register address of the device to read.
 * @param [in] txdata : Is a pointer on a buffer containing the data to be send.
 * @param [in] txLen : Is number of data elements to be send.
 *
 * @retval RETCODE_OK on success or an error code otherwise.
 */
Retcode_T MCU_I2C_WriteRegister(I2C_T i2c, uint16_t slaveAddr,uint8_t registerAddr,
        uint8_t * txdata, uint32_t txLen);

#endif /* BCDS_FEATURE_I2C */
#endif /* BCDS_MCU_I2C_H */
/**  @} */
