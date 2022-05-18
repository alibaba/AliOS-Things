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
 *   This file includes compile-time configurations for the logging service.
 *
 */

#ifndef CONFIG_LOGGING_H_
#define CONFIG_LOGGING_H_

/**
 * @def OPENTHREAD_CONFIG_LOG_OUTPUT
 *
 * Selects if, and where the LOG output goes to.
 *
 * There are several options available
 * - @sa OPENTHREAD_CONFIG_LOG_OUTPUT_NONE
 * - @sa OPENTHREAD_CONFIG_LOG_OUTPUT_DEBUG_UART
 * - @sa OPENTHREAD_CONFIG_LOG_OUTPUT_APP
 * - @sa OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED
 * - and others
 *
 * Note:
 *
 * 1) Because the default is: OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED
 *    The platform is expected to provide at least a stub for `otPlatLog()`.
 *
 * 2) This is effectively an ENUM so it can be if/else/endif at compile time.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_OUTPUT
#define OPENTHREAD_CONFIG_LOG_OUTPUT OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED
#endif

/** Log output goes to the bit bucket (disabled) */
#define OPENTHREAD_CONFIG_LOG_OUTPUT_NONE 0
/** Log output goes to the debug uart - requires OPENTHREAD_CONFIG_ENABLE_DEBUG_UART to be enabled */
#define OPENTHREAD_CONFIG_LOG_OUTPUT_DEBUG_UART 1
/** Log output goes to the "application" provided otPlatLog() in NCP and CLI code */
#define OPENTHREAD_CONFIG_LOG_OUTPUT_APP 2
/** Log output is handled by a platform defined function */
#define OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED 3

/**
 * @def OPENTHREAD_CONFIG_LOG_LEVEL
 *
 * The log level (used at compile time). If `OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE` is set, this defines the most
 * verbose log level possible. See `OPENTHREAD_CONFIG_LOG_LEVEL_INIT` to set the initial log level.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_LEVEL
#define OPENTHREAD_CONFIG_LOG_LEVEL OT_LOG_LEVEL_CRIT
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE
 *
 * Define as 1 to enable dynamic log level control.
 *
 * Note that the OPENTHREAD_CONFIG_LOG_LEVEL determines the log level at
 * compile time. The dynamic log level control (if enabled) only allows
 * decreasing the log level from the compile time value.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE
#define OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_LEVEL_INIT
 *
 * The initial log level used when OpenThread is initialized. See
 * `OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE`.
 */
#ifndef OPENTHREAD_CONFIG_LOG_LEVEL_INIT
#define OPENTHREAD_CONFIG_LOG_LEVEL_INIT OPENTHREAD_CONFIG_LOG_LEVEL
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_API
 *
 * Define to enable OpenThread API logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_API
#define OPENTHREAD_CONFIG_LOG_API 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_MLE
 *
 * Define to enable MLE logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_MLE
#define OPENTHREAD_CONFIG_LOG_MLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_MESHCOP
 *
 * Define to enable MeshCoP logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_MESHCOP
#define OPENTHREAD_CONFIG_LOG_MESHCOP 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_ARP
 *
 * Define to enable EID-to-RLOC map logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_ARP
#define OPENTHREAD_CONFIG_LOG_ARP 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_NETDATA
 *
 * Define to enable Network Data logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_NETDATA
#define OPENTHREAD_CONFIG_LOG_NETDATA 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_ICMP
 *
 * Define to enable ICMPv6 logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_ICMP
#define OPENTHREAD_CONFIG_LOG_ICMP 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_IP6
 *
 * Define to enable IPv6 logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_IP6
#define OPENTHREAD_CONFIG_LOG_IP6 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_TCP
 *
 * Define to enable IPv6 logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_TCP
#define OPENTHREAD_CONFIG_LOG_TCP 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_MAC
 *
 * Define to enable IEEE 802.15.4 MAC logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_MAC
#define OPENTHREAD_CONFIG_LOG_MAC 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_MEM
 *
 * Define to enable memory logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_MEM
#define OPENTHREAD_CONFIG_LOG_MEM 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_PKT_DUMP
 *
 * Define to enable log content of packets.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_PKT_DUMP
#define OPENTHREAD_CONFIG_LOG_PKT_DUMP 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_NETDIAG
 *
 * Define to enable network diagnostic logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_NETDIAG
#define OPENTHREAD_CONFIG_LOG_NETDIAG 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_PLATFORM
 *
 * Define to enable platform region logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_PLATFORM
#define OPENTHREAD_CONFIG_LOG_PLATFORM 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_CLI
 *
 * Define to enable CLI logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_CLI
#define OPENTHREAD_CONFIG_LOG_CLI 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_COAP
 *
 * Define to enable COAP logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_COAP
#define OPENTHREAD_CONFIG_LOG_COAP 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_CORE
 *
 * Define to enable OpenThread Core logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_CORE
#define OPENTHREAD_CONFIG_LOG_CORE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_UTIL
 *
 * Define to enable OpenThread Utility module logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_UTIL
#define OPENTHREAD_CONFIG_LOG_UTIL 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_BBR
 *
 * Note: available since Thread 1.2.
 *
 * Define to enable Backbone Router (BBR) region logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_BBR
#define OPENTHREAD_CONFIG_LOG_BBR 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_MLR
 *
 * Note: available since Thread 1.2.
 *
 * Define to enable Multicast Listener Registration (MLR) region logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_MLR
#define OPENTHREAD_CONFIG_LOG_MLR 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_DUA
 *
 * Note: available since Thread 1.2.
 *
 * Define to enable Domain Unicast Address (DUA) region logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_DUA
#define OPENTHREAD_CONFIG_LOG_DUA 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_BR
 *
 * Define to Border Router (BR) region logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_BR
#define OPENTHREAD_CONFIG_LOG_BR 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_SRP
 *
 * Define to enable Service Registration Protocol (SRP) region logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_SRP
#define OPENTHREAD_CONFIG_LOG_SRP 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_DNS
 *
 * Define to enable DNS region logging.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_DNS
#define OPENTHREAD_CONFIG_LOG_DNS 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_PREPEND_UPTIME
 *
 * Define as 1 to prepend the current uptime to all log messages.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_PREPEND_UPTIME
#define OPENTHREAD_CONFIG_LOG_PREPEND_UPTIME 0
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_PREPEND_LEVEL
 *
 * Define to prepend the log level to all log messages.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_PREPEND_LEVEL
#define OPENTHREAD_CONFIG_LOG_PREPEND_LEVEL 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_PREPEND_REGION
 *
 * Define to prepend the log region to all log messages.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_PREPEND_REGION
#define OPENTHREAD_CONFIG_LOG_PREPEND_REGION 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_SUFFIX
 *
 * Define suffix to append at the end of logs.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_SUFFIX
#define OPENTHREAD_CONFIG_LOG_SUFFIX ""
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_SRC_DST_IP_ADDRESSES
 *
 * If defined as 1 when IPv6 message info is logged in mesh-forwarder, the source and destination IPv6 addresses of
 * messages are also included.
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_SRC_DST_IP_ADDRESSES
#define OPENTHREAD_CONFIG_LOG_SRC_DST_IP_ADDRESSES 1
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY
 *
 * Set to 1 to require all the logging related definition to user macro only (up to the call to the platform log API).
 * Otherwise the logging implementation uses functions (which is preferred and recommended model).
 *
 * This is intended for special platform requirements where the logging needs to be defined a macro (e.g., for log
 * tokenization or similar features).
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY
#define OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY 0
#endif

/**
 * @def OPENTHREAD_CONFIG_PLAT_LOG_MACRO_NAME
 *
 * Defines the name of macro used for logging inside OpenThread, by default it is set to `otPlatLog()`. This is used
 * and applicable only when `OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY` is set to 1.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLAT_LOG_MACRO_NAME
#define OPENTHREAD_CONFIG_PLAT_LOG_MACRO_NAME otPlatLog
#endif

/**
 * @def OPENTHREAD_CONFIG_LOG_MAX_SIZE
 *
 * The maximum log string size (number of chars).
 *
 */
#ifndef OPENTHREAD_CONFIG_LOG_MAX_SIZE
#define OPENTHREAD_CONFIG_LOG_MAX_SIZE 150
#endif

#endif // CONFIG_LOGGING_H_
