/*
 *  Copyright (c) 2019, The OpenThread Authors.
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
 *   This file includes the platform abstraction for Thread Radio Encapsulation Link (TREL) using an IPv6/UDP interface.
 *
 */

#ifndef OPENTHREAD_PLATFORM_TREL_UDP6_H_
#define OPENTHREAD_PLATFORM_TREL_UDP6_H_

#include <stdint.h>

#include <openthread/error.h>
#include <openthread/instance.h>
#include <openthread/ip6.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup plat-trel
 *
 * @brief
 *   This module includes the platform abstraction for Thread Radio Encapsulation Link (TREL) using an IPv6/UDP
 *   interface.
 *
 * @{
 *
 */

/**
 * This function initializes the TREL IPv6/UDP interface.
 *
 * This function is called before any other TREL platform functions.
 *
 * @param[in] aInstance        The OpenThread instance structure.
 * @param[in] aUnicastAddress  The unicast address to add to interface and use as tx source and rx destination.
 * @param[in] aUdpPort         A UDP port number to use.
 *
 */
void otPlatTrelUdp6Init(otInstance *aInstance, const otIp6Address *aUnicastAddress, uint16_t aUdpPort);

/**
 * This function updates the unicast IPv6 address for TREL IPv6/UDP interface.
 *
 * The interface should only have one unicast IPv6 address. Calling this function replaces any previously set unicast
 * IPv6 address (during initialization from `otPlatTrelUdp6Init` or earlier calls to `otPlatTrelUdp6UpdateAddress()`).
 *
 * @param[in] aInstance        The OpenThread instance structure.
 * @param[in] aUnicastAddress  The unicast address to add to interface and use for as tx source and rx destination.
 *
 */
void otPlatTrelUdp6UpdateAddress(otInstance *aInstance, const otIp6Address *aUnicastAddress);

/**
 * This function subscribes the TREL IPv6/UDP interface to a new multicast address.
 *
 * This function may be called multiple times to subscribe to different addresses. The interface should accept/receive
 * packets destined to any previously subscribed multicast address in addition to the unicast address added from the
 * `otPlatTrelUdp6Init()` function when interface was initialized.
 *
 * @param[in] aInstance          The OpenThread instance structure.
 * @param[in] aMulticastAddress  A multicast IPv6 address.
 *
 */
void otPlatTrelUdp6SubscribeMulticastAddress(otInstance *aInstance, const otIp6Address *aMulticastAddress);

/**
 * This function requests a packet to be sent to a given destination.
 *
 * @param[in] aInstance        The OpenThread instance structure.
 * @param[in] aBuffer          A pointer to buffer containing the packet to send.
 * @param[in] aLength          Packet length (number of bytes).
 * @param[in] aDestAddress     The destination IPv6 address (can be a unicast or a multicast IPv6 address).
 *
 * @retval OT_ERROR_NONE    The tx request was handled successfully.
 * @retval OT_ERROR_ABORT   The interface is not ready and tx was aborted
 *
 */
otError otPlatTrelUdp6SendTo(otInstance *        aInstance,
                             const uint8_t *     aBuffer,
                             uint16_t            aLength,
                             const otIp6Address *aDestAddress);

/**
 * This function is a callback from platform to notify of a received packet.
 *
 * @note The buffer content (up to its specified length) may get changed during processing by OpenThread core (e.g.,
 * decrypted in place), so the platform implementation should expect that after returning from this function the
 * packet @p aBuffer content may have been altered.
 *
 * @param[in] aInstance        The OpenThread instance structure.
 * @param[in] aBuffer          A buffer containing the received packet.
 * @param[in] aLength          Packet length (number of bytes).
 *
 */
extern void otPlatTrelUdp6HandleReceived(otInstance *aInstance, uint8_t *aBuffer, uint16_t aLength);

/**
 * This optional function is intended for testing only. It changes the test mode status for TREL interface.
 *
 * This function requests TREL interface to be temporarily disabled or enabled. When disabled all traffic flow through
 * the TREL interface should be silently dropped.
 *
 * A default weak implementation of this method is provided by OpenThread (returning NOT_IMPLEMENTED).
 *
 * @param[in] aInstance        The OpenThread instance structure.
 * @param[in] aEnable          Indicates whether to enable/disable the TREL interface.
 *
 * @retval OT_ERROR_NONE             Successfully changed the TREL interface test status (enabled/disabled).
 * @retval OT_ERROR_FAILED           Failed to enable the TREL interface.
 * @retval OT_ERROR_NOT_IMPLEMENTED  This function is not provided by the platform.
 *
 */
otError otPlatTrelUdp6SetTestMode(otInstance *aInstance, bool aEnable);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // end of extern "C"
#endif

#endif // OPENTHREAD_PLATFORM_TREL_UDP6_H_
