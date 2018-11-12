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
 * @ingroup UTILS
 *
 * @defgroup I2CTRANSCEIVER I2CTransceiver
 * @{
 *
 * @brief Advanced API functions for sending and receiving via I2C
 *
 * @file
 */
#ifndef BCDS_I2CTRANSCEIVER_H_
#define BCDS_I2CTRANSCEIVER_H_

#include "BCDS_Utils.h"

#if BCDS_FEATURE_I2CTRANSCEIVER
/* Include all headers which are needed by this file. */
#include "BCDS_Retcode.h"
#include "BCDS_HAL.h"
#if BCDS_FEATURE_I2C
#include "BCDS_MCU_I2C.h"
//#include "FreeRTOS.h"
//#include "semphr.h"
#include <aos/kernel.h>
#include <k_api.h>
//#include <k_timer.h>
#include <k_queue.h>
#include <k_mutex.h>
#include <k_sem.h>
#include <k_time.h>
/* Put the type and macro definitions here */

/** Struct holding the I2C related configuration */
struct I2cTranceiverHandle_S
{
    bool InitializationStatus;
    /* I2C Handle used by the I2C driver in Essentials */
    I2C_T I2CHandle;
    /*semaphore used to synchronize the read/write process*/
    ksem_t I2CBusSync;
    /*mutex used to synchronize the access of read/write process*/
    kmutex_t I2CMutexLock;
    /* status of I2C transfer*/
    int8_t I2cTransferStatusFlag;

};
typedef struct I2cTranceiverHandle_S I2cTranceiverHandle_T,*I2cTranceiverHandlePtr_T;
/* Put the function declarations here */
/**
 * @brief Initializes the transceiver for the use with the passed I2C handle.
 *
 * @param [in] i2cTransceiver
 * a pointer to be initialized for I2C transceiver context structure to be initialized.
 *
 * @param [in] i2cHandle
 * The handle of the I2C to be used by the transceiver. The handle
 * must be initialized before passing it here.
 *
 * @retval #RETCODE_OK                       if successfully initialized.
 * @retval #RETCODE_NULL_POINTER             if any of the parameter is NULL.
 * @retval #RETCODE_SEMAPHORE_ERROR          if the semaphores are not created (see #I2cTranceiverHandle_T.I2CBusSync
 *                                           and #I2cTranceiverHandle_T.I2CMutexLock).
 * @retval #RETCODE_DOPPLE_INITIALIZATION    if you are trying to initialize the transceiver again.
 */
Retcode_T I2CTransceiver_Init(I2cTranceiverHandlePtr_T i2cTransceiver, I2C_T i2cHandle);

/**
 * @brief Function to read a register of a device connected to I2C
 *
 * @param[in]   i2cTransceiver
 * a pointer to the transceiver.
 *
 * @param [in]  i2cAddr
 * I2C address of the connected device from read has to happen.
 *
 * @param [in]  regAddr
 * Register address of the device to read.
 *
 * @param [out] buffer
 * Buffer in which the values will be stored.
 *
 * @param [in]  bytesToRead
 * Number of bytes to be read.
 *
 * @retval #RETCODE_OK                   if successfully read the register.
 * @retval #RETCODE_UNINITIALIZED        if called without initializing.
 * @retval #RETCODE_NULL_POINTER         if any of the parameter is NULL.
 * @retval #RETCODE_SEMAPHORE_ERROR      if semaphore could not be taken within given time.
 * @retval #RETCODE_FAILURE              if I2C transfer is not successful.
 */
Retcode_T I2CTransceiver_Read(I2cTranceiverHandlePtr_T i2cTransceiver,uint8_t i2cAddr,uint8_t regAddr,uint8_t *buffer,uint8_t bytesToRead );

/**
 * @brief Function to write a register of a device connected to I2C
 *
 * @param[in]   i2cTransceiver
 * a pointer to the transceiver.
 *
 * @param [in]  i2cAddr
 * I2C address of the connected device to which write has to happen.
 *
 * @param [in]  regAddr
 * Register address of the device to write.
 *
 * @param [in] buffer
 * Buffer in which the values to be written are stored.
 *
 * @param [in]  bytesToWrite
 * Number of bytes to be written.
 *
 * @retval #RETCODE_OK                   if successfully written to the register.
 * @retval #RETCODE_UNINITIALIZED        if called without initializing.
 * @retval #RETCODE_NULL_POINTER         if any of the parameter is NULL.
 * @retval #RETCODE_SEMAPHORE_ERROR      if semaphore could not be taken within given time.
 * @retval #RETCODE_FAILURE              if I2C transfer is not successful.
 */

Retcode_T I2CTransceiver_Write(I2cTranceiverHandlePtr_T i2cTransceiver,uint8_t i2cAddr,uint8_t regAddr,uint8_t *buffer,uint8_t bytesToWrite);

/**
 * @brief Function to loop the I2C callback.
 *
 * @details It is necessary for the I2CTransceiver to loop the I2Ccallback.
 * This means, the middle-layer driver which uses I2C and I2CTransceiver
 * must initialize I2C with a callback function which invokes
 * this function in order to loop the callback from I2C to the
 * transceiver.
 *
 * @param[in] transceiver
 * a pointer to the transceiver.
 *
 * @param[in] event
 * The event which is notified by the callback
 */
void I2CTransceiver_LoopCallback(I2cTranceiverHandlePtr_T transceiver,struct MCU_I2C_Event_S event);

/**
 * @brief De-initializes the transceiver.
 *
 * The transceiver must not be used after calling this function.
 *
 * @param[in] transceiver
 * a pointer to the transceiver context structure to be de-initialized
 *
 * @retval #RETCODE_OK              if successfully de-initialized
 * @retval #RETCODE_INVALID_PARAM   if the transceiver pointer parameter is NULL
 */
Retcode_T I2CTransceiver_Deinit(I2cTranceiverHandlePtr_T transceiver);
#endif /* BCDS_FEATURE_I2C */

#endif /* BCDS_FEATURE_I2CTRANSCEIVER */

#endif /* BCDS_I2CTRANSCEIVER_H_ */

/**@} */
