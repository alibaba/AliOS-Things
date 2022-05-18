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
 *   This file includes compile-time configurations for the IP6 service.
 *
 */

#ifndef CONFIG_IP6_H_
#define CONFIG_IP6_H_

/**
 * @def OPENTHREAD_CONFIG_IP6_MAX_EXT_UCAST_ADDRS
 *
 * The maximum number of supported IPv6 addresses allows to be externally added.
 *
 */
#ifndef OPENTHREAD_CONFIG_IP6_MAX_EXT_UCAST_ADDRS
#define OPENTHREAD_CONFIG_IP6_MAX_EXT_UCAST_ADDRS 4
#endif

/**
 * @def OPENTHREAD_CONFIG_IP6_MAX_EXT_MCAST_ADDRS
 *
 * The maximum number of supported IPv6 multicast addresses allows to be externally added.
 *
 */
#ifndef OPENTHREAD_CONFIG_IP6_MAX_EXT_MCAST_ADDRS
#define OPENTHREAD_CONFIG_IP6_MAX_EXT_MCAST_ADDRS 2
#endif

/**
 * @def OPENTHREAD_CONFIG_IP6_HOP_LIMIT_DEFAULT
 *
 * This setting configures the default hop limit of IPv6.
 *
 */
#ifndef OPENTHREAD_CONFIG_IP6_HOP_LIMIT_DEFAULT
#define OPENTHREAD_CONFIG_IP6_HOP_LIMIT_DEFAULT 64
#endif

/**
 * @def OPENTHREAD_CONFIG_IP6_MAX_DATAGRAM_LENGTH
 *
 * This setting configures the max datagram length of IPv6.
 *
 */
#ifndef OPENTHREAD_CONFIG_IP6_MAX_DATAGRAM_LENGTH
#define OPENTHREAD_CONFIG_IP6_MAX_DATAGRAM_LENGTH 1280
#endif

/**
 * @def OPENTHREAD_CONFIG_ENABLE_IP6_FRAGMENTATION
 *
 * Define as 1 to enable IPv6 Fragmentation support.
 *
 */
#ifndef OPENTHREAD_CONFIG_IP6_FRAGMENTATION_ENABLE
#define OPENTHREAD_CONFIG_IP6_FRAGMENTATION_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_IP6_MAX_ASSEMBLED_DATAGRAM
 *
 * This setting configures the max datagram length of a unfragmented IPv6 packet.
 *
 */
#ifndef OPENTHREAD_CONFIG_IP6_MAX_ASSEMBLED_DATAGRAM
#define OPENTHREAD_CONFIG_IP6_MAX_ASSEMBLED_DATAGRAM 2000
#endif

/**
 * @def OPENTHREAD_CONFIG_IP6_REASSEMBLY_TIMEOUT
 *
 * This setting configures the reassembly timeout for IPv6 in seconds.
 *
 * RFC 2460 \> 60 seconds
 *
 */
#ifndef OPENTHREAD_CONFIG_IP6_REASSEMBLY_TIMEOUT
#define OPENTHREAD_CONFIG_IP6_REASSEMBLY_TIMEOUT 60
#endif

/**
 * @def OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE
 *
 * Define as 1 to enable support for adding of auto-configured SLAAC addresses by OpenThread.
 *
 */
#ifndef OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE
#define OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_IP6_SLAAC_NUM_ADDRESSES
 *
 * The number of auto-configured SLAAC addresses. Applicable only if OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE is enabled.
 *
 */
#ifndef OPENTHREAD_CONFIG_IP6_SLAAC_NUM_ADDRESSES
#define OPENTHREAD_CONFIG_IP6_SLAAC_NUM_ADDRESSES 4
#endif

/**
 * @def OPENTHREAD_CONFIG_MPL_SEED_SET_ENTRIES
 *
 * The number of MPL Seed Set entries for duplicate detection.
 *
 */
#ifndef OPENTHREAD_CONFIG_MPL_SEED_SET_ENTRIES
#define OPENTHREAD_CONFIG_MPL_SEED_SET_ENTRIES 32
#endif

/**
 * @def OPENTHREAD_CONFIG_MPL_SEED_SET_ENTRY_LIFETIME
 *
 * The MPL Seed Set entry lifetime in seconds.
 *
 */
#ifndef OPENTHREAD_CONFIG_MPL_SEED_SET_ENTRY_LIFETIME
#define OPENTHREAD_CONFIG_MPL_SEED_SET_ENTRY_LIFETIME 5
#endif

/**
 * @def OPENTHREAD_CONFIG_MPL_DYNAMIC_INTERVAL_ENABLE
 *
 * Define as 1 to enable dynamic MPL interval feature.
 *
 * If this feature is enabled, the MPL forward interval will be adjusted dynamically according to
 * the network scale, which helps to reduce multicast latency.
 *
 */
#ifndef OPENTHREAD_CONFIG_MPL_DYNAMIC_INTERVAL_ENABLE
#define OPENTHREAD_CONFIG_MPL_DYNAMIC_INTERVAL_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_UNSECURE_TRAFFIC_MANAGED_BY_STACK_ENABLE
 *
 * Define as 1 to enable dynamic unsecure port management
 *
 * If this feature is enabled, OpenThread will automaitically disable link-level
 * security for packets sent with unsecure source ports. Once we receive a secure
 * packet sent to the unsecure port, this port will be removed from the unsecure
 * port list.
 *
 * Enable only if you know well about its behavior.
 *
 */
#ifndef OPENTHREAD_CONFIG_UNSECURE_TRAFFIC_MANAGED_BY_STACK_ENABLE
#define OPENTHREAD_CONFIG_UNSECURE_TRAFFIC_MANAGED_BY_STACK_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_TCP_ENABLE
 *
 * Define as 1 to enable TCP.
 *
 */
#ifndef OPENTHREAD_CONFIG_TCP_ENABLE
#define OPENTHREAD_CONFIG_TCP_ENABLE 1
#endif

#endif // CONFIG_IP6_H_
