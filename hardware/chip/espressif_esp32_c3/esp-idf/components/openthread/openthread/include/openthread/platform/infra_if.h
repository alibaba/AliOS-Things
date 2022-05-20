/*
 *  Copyright (c) 2020, The OpenThread Authors.
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
 *   This file includes the platform abstraction for the infrastructure network interface.
 *
 */

#ifndef OPENTHREAD_PLATFORM_INFRA_IF_H_
#define OPENTHREAD_PLATFORM_INFRA_IF_H_

#include <stdint.h>

#include <openthread/error.h>
#include <openthread/instance.h>
#include <openthread/ip6.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This method tells whether an infra interface has the given IPv6 address assigned.
 *
 * @param[in]  aInfraIfIndex  The index of the infra interface.
 * @param[in]  aAddress       The IPv6 address.
 *
 * @returns  TRUE if the infra interface has given IPv6 address assigned, FALSE otherwise.
 *
 */
bool otPlatInfraIfHasAddress(uint32_t aInfraIfIndex, const otIp6Address *aAddress);

/**
 * This method sends an ICMPv6 Neighbor Discovery message on given infrastructure interface.
 *
 * See RFC 4861: https://tools.ietf.org/html/rfc4861.
 *
 * @param[in]  aInfraIfIndex  The index of the infrastructure interface this message is sent to.
 * @param[in]  aDestAddress   The destination address this message is sent to.
 * @param[in]  aBuffer        The ICMPv6 message buffer. The ICMPv6 checksum is left zero and the
 *                            platform should do the checksum calculate.
 * @param[in]  aBufferLength  The length of the message buffer.
 *
 * @note  Per RFC 4861, the implementation should send the message with IPv6 link-local source address
 *        of interface @p aInfraIfIndex and IP Hop Limit 255.
 *
 * @retval OT_ERROR_NONE    Successfully sent the ICMPv6 message.
 * @retval OT_ERROR_FAILED  Failed to send the ICMPv6 message.
 *
 */
otError otPlatInfraIfSendIcmp6Nd(uint32_t            aInfraIfIndex,
                                 const otIp6Address *aDestAddress,
                                 const uint8_t *     aBuffer,
                                 uint16_t            aBufferLength);

/**
 * The infra interface driver calls this method to notify OpenThread
 * that an ICMPv6 Neighbor Discovery message is received.
 *
 * See RFC 4861: https://tools.ietf.org/html/rfc4861.
 *
 * @param[in]  aInstance      The OpenThread instance structure.
 * @param[in]  aInfraIfIndex  The index of the infrastructure interface on which the ICMPv6 message is received.
 * @param[in]  aSrcAddress    The source address this message is received from.
 * @param[in]  aBuffer        The ICMPv6 message buffer.
 * @param[in]  aBufferLength  The length of the ICMPv6 message buffer.
 *
 * @note  Per RFC 4861, the caller should enforce that the source address MUST be a IPv6 link-local
 *        address and the IP Hop Limit MUST be 255.
 *
 */
extern void otPlatInfraIfRecvIcmp6Nd(otInstance *        aInstance,
                                     uint32_t            aInfraIfIndex,
                                     const otIp6Address *aSrcAddress,
                                     const uint8_t *     aBuffer,
                                     uint16_t            aBufferLength);

/**
 * The infra interface driver calls this method to notify OpenThread
 * of the interface state changes.
 *
 * It is fine for the platform to call to method even when the running state
 * of the interface hasn't changed. In this case, the Routing Manager state is
 * not affected.
 *
 * @param[in]  aInstance          The OpenThread instance structure.
 * @param[in]  aInfraIfIndex      The index of the infrastructure interface.
 * @param[in]  aIsRunning         A boolean that indicates whether the infrastructure
 *                                interface is running.
 *
 * @retval  OT_ERROR_NONE           Successfully updated the infra interface status.
 * @retval  OT_ERROR_INVALID_STATE  The Routing Manager is not initialized.
 * @retval  OT_ERROR_INVALID_ARGS   The @p aInfraIfIndex doesn't match the infra interface the
 *                                  Routing Manager are initialized with.
 *
 */
extern otError otPlatInfraIfStateChanged(otInstance *aInstance, uint32_t aInfraIfIndex, bool aIsRunning);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_PLATFORM_INFRA_IF_H_
