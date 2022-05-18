/*
 *  Copyright (c) 2018, The OpenThread Authors.
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
 *  This file defines the top-level sntp functions for the OpenThread library.
 */

#ifndef OPENTHREAD_SNTP_H_
#define OPENTHREAD_SNTP_H_

#include <stdint.h>

#include <openthread/ip6.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-sntp
 *
 * @brief
 *   This module includes functions that control SNTP communication.
 *
 * @{
 *
 */

#define OT_SNTP_DEFAULT_SERVER_IP "2001:4860:4806:8::" ///< Defines default SNTP Server address - Google NTP Server.
#define OT_SNTP_DEFAULT_SERVER_PORT 123                ///< Defines default SNTP Server port.

/**
 * This structure implements SNTP Query parameters.
 *
 */
typedef struct otSntpQuery
{
    const otMessageInfo *mMessageInfo; ///< A reference to the message info related with SNTP Server.
} otSntpQuery;

/**
 * This function pointer is called when a SNTP response is received.
 *
 * @param[in]  aContext   A pointer to application-specific context.
 * @param[in]  aTime      Specifies the time at the server when the response left for the client, in UNIX time.
 * @param[in]  aResult    A result of the SNTP transaction.
 *
 * @retval  OT_ERROR_NONE              A response was received successfully and time is provided
 *                                     in @p aTime.
 * @retval  OT_ERROR_ABORT             A SNTP transaction was aborted by stack.
 * @retval  OT_ERROR_BUSY              The Kiss-o'-death packet has been received.
 * @retval  OT_ERROR_RESPONSE_TIMEOUT  No SNTP response has been received within timeout.
 * @retval  OT_ERROR_FAILED            A response was received but contains incorrect data.
 *
 */
typedef void (*otSntpResponseHandler)(void *aContext, uint64_t aTime, otError aResult);

/**
 * This function sends a SNTP query.
 *
 * This function is available only if feature `OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE` is enabled.
 *
 * @param[in]  aInstance   A pointer to an OpenThread instance.
 * @param[in]  aQuery      A pointer to specify SNTP query parameters.
 * @param[in]  aHandler    A function pointer that shall be called on response reception or time-out.
 * @param[in]  aContext    A pointer to arbitrary context information.
 *
 */
otError otSntpClientQuery(otInstance *          aInstance,
                          const otSntpQuery *   aQuery,
                          otSntpResponseHandler aHandler,
                          void *                aContext);

/**
 * This function sets the unix era number.
 *
 * The default value of unix era is set to 0. The subsequent eras start after year 2106.
 *
 * @param[in]  aInstance   A pointer to an OpenThread instance.
 * @param[in]  aUnixEra    Unix era number.
 *
 */
void otSntpClientSetUnixEra(otInstance *aInstance, uint32_t aUnixEra);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_SNTP_H_
