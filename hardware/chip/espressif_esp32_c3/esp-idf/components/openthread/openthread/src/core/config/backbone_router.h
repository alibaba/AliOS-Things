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
 *   This file includes compile-time configurations for Backbone Router services.
 *
 */

#ifndef CONFIG_BACKBONE_ROUTER_H_
#define CONFIG_BACKBONE_ROUTER_H_

/**
 * @def OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
 *
 * Define to 1 to enable Backbone Router support.
 *
 */
#ifndef OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
#define OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE
 *
 * Define to 1 to enable Backbone Router ND Proxying for Thread 1.2 DUA feature.
 * It's turned on by default for Thread 1.2 Backbone Router.
 *
 */
#ifndef OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE
#define OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
#endif

#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE && !OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
#error \
    "OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE is required for OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE"
#endif

/**
 * @def OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE
 *
 * Define to 1 to enable Backbone Router Multicast Routing for Thread 1.2 MLR feature.
 * It's turned on by default for Thread 1.2 Backbone Router.
 *
 */
#ifndef OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE
#define OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
#endif

#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE && !OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
#error \
    "OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE is required for OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE"
#endif

#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
#if !OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE && \
    !OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE
#error \
    "At least one of OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE and OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE must be enabled for OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE"
#endif
#endif

/**
 * @def OPENTHREAD_CONFIG_MAX_MULTICAST_LISTENERS
 *
 * This setting configures the maximum number of Multicast Listeners on a Backbone Router.
 *
 * Note: According to Thread Conformance v1.2.0, a Thread Border Router MUST be able to hold a Multicast Listeners Table
 * in memory with at least seventy five (75) entries.
 *
 * @sa MulticastListenersTable
 *
 */
#ifndef OPENTHREAD_CONFIG_MAX_MULTICAST_LISTENERS
#define OPENTHREAD_CONFIG_MAX_MULTICAST_LISTENERS 75
#endif

/**
 * @def OPENTHREAD_CONFIG_NDPROXY_TABLE_ENTRY_NUM
 *
 * The maximum number of supported DUA that Backbone Router could proxy.
 *
 * Note: According to Thread Conformance v1.2.0, a Thread Border Router MUST be able to hold a DUA Devices Table in
 * memory with at least two hundred and fifty (250) entries.
 *
 */
#ifndef OPENTHREAD_CONFIG_NDPROXY_TABLE_ENTRY_NUM
#define OPENTHREAD_CONFIG_NDPROXY_TABLE_ENTRY_NUM 250
#endif

#endif // CONFIG_BACKBONE_ROUTER_H_
