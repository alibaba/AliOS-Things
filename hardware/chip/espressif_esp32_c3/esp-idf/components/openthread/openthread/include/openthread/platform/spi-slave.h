/*
 *  Copyright (c) 2016, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file spi-slave.h
 * @brief
 *   This file includes the platform abstraction for SPI slave communication.
 */

#ifndef OPENTHREAD_PLATFORM_SPI_SLAVE_H_
#define OPENTHREAD_PLATFORM_SPI_SLAVE_H_

#include <stdbool.h>
#include <stdint.h>

#include <openthread/error.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup plat-spi-slave
 *
 * @brief
 *   This module includes the platform abstraction for SPI slave communication.
 *
 * @{
 *
 */

/**
 * Indicates that a SPI transaction has completed with the given length. The data written to the slave has been written
 * to the pointer indicated by the `aInputBuf` argument to the previous call to `otPlatSpiSlavePrepareTransaction()`.
 *
 * Once this function is called, `otPlatSpiSlavePrepareTransaction()` is invalid and must be called again for the next
 * transaction to be valid.
 *
 * Note that this function is always called at the end of a transaction, even if `otPlatSpiSlavePrepareTransaction()`
 * has not yet been called. In such cases, `aOutputBufLen` and `aInputBufLen` will be zero.
 *
 * This callback can be called from ISR context. The return value from this function indicates if any further
 * processing is required. If `TRUE` is returned the platform spi-slave driver implementation must invoke the
 * transaction process callback (`aProcessCallback` set in `otPlatSpiSlaveEnable()`) which unlike this callback must be
 * called from the same OS context that any other OpenThread API/callback is called.
 *
 * @param[in] aContext           Context pointer passed into `otPlatSpiSlaveEnable()`.
 * @param[in] aOutputBuf         Value of `aOutputBuf` from last call to `otPlatSpiSlavePrepareTransaction()`.
 * @param[in] aOutputBufLen      Value of `aOutputBufLen` from last call to `otPlatSpiSlavePrepareTransaction()`.
 * @param[in] aInputBuf          Value of aInputBuf from last call to `otPlatSpiSlavePrepareTransaction()`.
 * @param[in] aInputBufLen       Value of aInputBufLen from last call to `otPlatSpiSlavePrepareTransaction()`
 * @param[in] aTransactionLength Length of the completed transaction, in bytes.
 *
 * @returns  TRUE if after this call returns the platform should invoke the process callback `aProcessCallback`,
 *           FALSE if there is nothing to process and no need to invoke the process callback.
 */
typedef bool (*otPlatSpiSlaveTransactionCompleteCallback)(void *   aContext,
                                                          uint8_t *aOutputBuf,
                                                          uint16_t aOutputBufLen,
                                                          uint8_t *aInputBuf,
                                                          uint16_t aInputBufLen,
                                                          uint16_t aTransactionLength);

/**
 * Invoked after a transaction complete callback is called and returns `TRUE` to do any further processing required.
 * Unlike `otPlatSpiSlaveTransactionCompleteCallback` which can be called from any OS context (e.g., ISR), this
 * callback MUST be called from the same OS context as any other OpenThread API/callback.
 *
 * @param[in] aContext           Context pointer passed into `otPlatSpiSlaveEnable()`.
 *
 */
typedef void (*otPlatSpiSlaveTransactionProcessCallback)(void *aContext);

/**
 * Initialize the SPI slave interface.

 * Note that SPI slave is not fully ready until a transaction is prepared using `otPlatSPISlavePrepareTransaction()`.
 *
 * If `otPlatSPISlavePrepareTransaction() is not called before the master begins a transaction, the resulting SPI
 * transaction will send all `0xFF` bytes and discard all received bytes.
 *
 * @param[in] aCompleteCallback  Pointer to transaction complete callback.
 * @param[in] aProcessCallback   Pointer to process callback.
 * @param[in] aContext           Context pointer to be passed to callbacks.
 *
 * @retval OT_ERROR_NONE     Successfully enabled the SPI Slave interface.
 * @retval OT_ERROR_ALREADY  SPI Slave interface is already enabled.
 * @retval OT_ERROR_FAILED   Failed to enable the SPI Slave interface.
 *
 */
otError otPlatSpiSlaveEnable(otPlatSpiSlaveTransactionCompleteCallback aCompleteCallback,
                             otPlatSpiSlaveTransactionProcessCallback  aProcessCallback,
                             void *                                    aContext);

/**
 * Shutdown and disable the SPI slave interface.
 */
void otPlatSpiSlaveDisable(void);

/**
 * Prepare data for the next SPI transaction. Data pointers MUST remain valid until the transaction complete callback
 * is called by the SPI slave driver, or until after the next call to `otPlatSpiSlavePrepareTransaction()`.
 *
 * This function may be called more than once before the SPI master initiates the transaction. Each *successful* call
 * to this function will cause the previous values from earlier calls to be discarded.
 *
 * Not calling this function after a completed transaction is the same as if this function was previously called with
 * both buffer lengths set to zero and `aRequestTransactionFlag` set to `false`.
 *
 * Once `aOutputBufLen` bytes of `aOutputBuf` has been clocked out, the MISO pin shall be set high until the master
 * finishes the SPI transaction. This is the functional equivalent of padding the end of `aOutputBuf` with `0xFF` bytes
 * out to the length of the transaction.
 *
 * Once `aInputBufLen` bytes of aInputBuf have been clocked in from MOSI, all subsequent values from the MOSI pin are
 * ignored until the SPI master finishes the transaction.
 *
 * Note that even if `aInputBufLen` or `aOutputBufLen` (or both) are exhausted before the SPI master finishes a
 * transaction, the ongoing size of the transaction must still be kept track of to be passed to the transaction
 * complete callback. For example, if `aInputBufLen` is equal to 10 and `aOutputBufLen` equal to 20 and the SPI master
 * clocks out 30 bytes, the value 30 is passed to the transaction complete callback.
 *
 * If a `NULL` pointer is passed in as `aOutputBuf` or `aInputBuf` it means that that buffer pointer should not change
 * from its previous/current value. In this case, the corresponding length argument should be ignored. For example,
 * `otPlatSpiSlavePrepareTransaction(NULL, 0, aInputBuf, aInputLen, false)` changes the input buffer pointer and its
 * length but keeps the output buffer pointer same as before.
 *
 * Any call to this function while a transaction is in progress will cause all of the arguments to be ignored and the
 * return value to be `OT_ERROR_BUSY`.
 *
 * @param[in] aOutputBuf              Data to be written to MISO pin
 * @param[in] aOutputBufLen           Size of the output buffer, in bytes
 * @param[in] aInputBuf               Data to be read from MOSI pin
 * @param[in] aInputBufLen            Size of the input buffer, in bytes
 * @param[in] aRequestTransactionFlag Set to true if host interrupt should be set
 *
 * @retval OT_ERROR_NONE           Transaction was successfully prepared.
 * @retval OT_ERROR_BUSY           A transaction is currently in progress.
 * @retval OT_ERROR_INVALID_STATE  otPlatSpiSlaveEnable() hasn't been called.
 *
 */
otError otPlatSpiSlavePrepareTransaction(uint8_t *aOutputBuf,
                                         uint16_t aOutputBufLen,
                                         uint8_t *aInputBuf,
                                         uint16_t aInputBufLen,
                                         bool     aRequestTransactionFlag);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_PLATFORM_SPI_SLAVE_H_
