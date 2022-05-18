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
 *   This file defines the platform diag interface.
 *
 */

#ifndef OPENTHREAD_PLATFORM_DIAG_H_
#define OPENTHREAD_PLATFORM_DIAG_H_

#include <stddef.h>
#include <stdint.h>

#include <openthread/error.h>
#include <openthread/platform/radio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup plat-factory-diagnostics
 *
 * @brief
 *   This module includes the platform abstraction for diagnostics features.
 *
 * @{
 *
 */

/**
 * This function processes a factory diagnostics command line.
 *
 * The output of this function (the content written to @p aOutput) MUST terminate with `\0` and the `\0` is within the
 * output buffer.
 *
 * @param[in]   aInstance       The OpenThread instance for current request.
 * @param[in]   aArgsLength     The number of arguments in @p aArgs.
 * @param[in]   aArgs           The arguments of diagnostics command line.
 * @param[out]  aOutput         The diagnostics execution result.
 * @param[in]   aOutputMaxLen   The output buffer size.
 *
 * @retval  OT_ERROR_INVALID_ARGS       The command is supported but invalid arguments provided.
 * @retval  OT_ERROR_NONE               The command is successfully process.
 * @retval  OT_ERROR_INVALID_COMMAND    The command is not valid or not supported.
 *
 */
otError otPlatDiagProcess(otInstance *aInstance,
                          uint8_t     aArgsLength,
                          char *      aArgs[],
                          char *      aOutput,
                          size_t      aOutputMaxLen);

/**
 * This function enables/disables the factory diagnostics mode.
 *
 * @param[in]  aMode  TRUE to enable diagnostics mode, FALSE otherwise.
 *
 */
void otPlatDiagModeSet(bool aMode);

/**
 * This function indicates whether or not factory diagnostics mode is enabled.
 *
 * @returns TRUE if factory diagnostics mode is enabled, FALSE otherwise.
 *
 */
bool otPlatDiagModeGet(void);

/**
 * This function sets the channel to use for factory diagnostics.
 *
 * @param[in]  aChannel  The channel value.
 *
 */
void otPlatDiagChannelSet(uint8_t aChannel);

/**
 * This function sets the transmit power to use for factory diagnostics.
 *
 * @param[in]  aTxPower  The transmit power value.
 *
 */
void otPlatDiagTxPowerSet(int8_t aTxPower);

/**
 * This function processes the received radio frame.
 *
 * @param[in]   aInstance   The OpenThread instance for current request.
 * @param[in]   aFrame      The received radio frame.
 * @param[in]   aError      The received radio frame status.
 *
 */
void otPlatDiagRadioReceived(otInstance *aInstance, otRadioFrame *aFrame, otError aError);

/**
 * This function processes the alarm event.
 *
 * @param[in]   aInstance   The OpenThread instance for current request.
 *
 */
void otPlatDiagAlarmCallback(otInstance *aInstance);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // end of extern "C"
#endif

#endif // OPENTHREAD_PLATFORM_DIAG_H_
