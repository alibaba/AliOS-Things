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
 * @file
 * @brief
 *   This file includes the platform abstraction for UART communication.
 */

#ifndef OPENTHREAD_PLATFORM_UART_H_
#define OPENTHREAD_PLATFORM_UART_H_

#include <stdint.h>

#include <openthread/error.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup plat-uart
 *
 * @brief
 *   This module includes the platform abstraction for UART communication.
 *
 * @{
 *
 */

/**
 * Enable the UART.
 *
 * @retval OT_ERROR_NONE    Successfully enabled the UART.
 * @retval OT_ERROR_FAILED  Failed to enabled the UART.
 *
 */
otError otPlatUartEnable(void);

/**
 * Disable the UART.
 *
 * @retval OT_ERROR_NONE    Successfully disabled the UART.
 * @retval OT_ERROR_FAILED  Failed to disable the UART.
 *
 */
otError otPlatUartDisable(void);

/**
 * Send bytes over the UART.
 *
 * @param[in] aBuf        A pointer to the data buffer.
 * @param[in] aBufLength  Number of bytes to transmit.
 *
 * @retval OT_ERROR_NONE    Successfully started transmission.
 * @retval OT_ERROR_FAILED  Failed to start the transmission.
 *
 */
otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength);

/**
 * Flush the outgoing transmit buffer and wait for the data to be sent.
 * This is called when the CLI UART interface has a full buffer but still
 * wishes to send more data.
 *
 * @retval OT_ERROR_NONE                Flush succeeded, we can proceed to write more
 *                                      data to the buffer.
 *
 * @retval OT_ERROR_NOT_IMPLEMENTED     Driver does not support synchronous flush.
 * @retval OT_ERROR_INVALID_STATE       Driver has no data to flush.
 */
otError otPlatUartFlush(void);

/**
 * The UART driver calls this method to notify OpenThread that the requested bytes have been sent.
 *
 */
extern void otPlatUartSendDone(void);

/**
 * The UART driver calls this method to notify OpenThread that bytes have been received.
 *
 * @param[in]  aBuf        A pointer to the received bytes.
 * @param[in]  aBufLength  The number of bytes received.
 *
 */
extern void otPlatUartReceived(const uint8_t *aBuf, uint16_t aBufLength);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_PLATFORM_UART_H_
