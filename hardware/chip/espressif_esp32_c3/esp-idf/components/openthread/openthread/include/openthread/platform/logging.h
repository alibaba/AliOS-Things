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
 *   This file includes the platform abstraction for the debug log service.
 */

#ifndef OPENTHREAD_PLATFORM_LOGGING_H_
#define OPENTHREAD_PLATFORM_LOGGING_H_

#include <stdarg.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup plat-logging
 *
 * @brief
 *   This module includes the platform abstraction for the debug log service.
 *
 * @{
 *
 */

/**
 * Log level None.
 *
 * @note Log Levels are defines so that embedded implementations can eliminate code at compile time via
 * #if/#else/#endif.
 *
 */
#define OT_LOG_LEVEL_NONE 0

/**
 * Log level Critical.
 *
 * @note Log Levels are defines so that embedded implementations can eliminate code at compile time via
 * #if/#else/#endif.
 *
 */
#define OT_LOG_LEVEL_CRIT 1

/**
 * Log level Warning.
 *
 * @note Log Levels are defines so that embedded implementations can eliminate code at compile time via
 * #if/#else/#endif.
 *
 */
#define OT_LOG_LEVEL_WARN 2

/**
 * Log level Notice.
 *
 * @note Log Levels are defines so that embedded implementations can eliminate code at compile time via
 * #if/#else/#endif.
 *
 */
#define OT_LOG_LEVEL_NOTE 3

/**
 * Log level Informational.
 *
 * @note Log Levels are defines so that embedded implementations can eliminate code at compile time via
 * #if/#else/#endif.
 *
 */
#define OT_LOG_LEVEL_INFO 4

/**
 * Log level Debug.
 *
 * @note Log Levels are defines so that embedded implementations can eliminate code at compile time via
 * #if/#else/#endif.
 *
 */
#define OT_LOG_LEVEL_DEBG 5

/**
 * This type represents the log level.
 *
 */
typedef int otLogLevel;

/**
 * This enumeration represents log regions.
 *
 */
typedef enum otLogRegion
{
    OT_LOG_REGION_API      = 1,  ///< OpenThread API
    OT_LOG_REGION_MLE      = 2,  ///< MLE
    OT_LOG_REGION_ARP      = 3,  ///< EID-to-RLOC mapping.
    OT_LOG_REGION_NET_DATA = 4,  ///< Network Data
    OT_LOG_REGION_ICMP     = 5,  ///< ICMPv6
    OT_LOG_REGION_IP6      = 6,  ///< IPv6
    OT_LOG_REGION_TCP      = 7,  ///< TCP
    OT_LOG_REGION_MAC      = 8,  ///< IEEE 802.15.4 MAC
    OT_LOG_REGION_MEM      = 9,  ///< Memory
    OT_LOG_REGION_NCP      = 10, ///< NCP
    OT_LOG_REGION_MESH_COP = 11, ///< Mesh Commissioning Protocol
    OT_LOG_REGION_NET_DIAG = 12, ///< Network Diagnostic
    OT_LOG_REGION_PLATFORM = 13, ///< Platform
    OT_LOG_REGION_COAP     = 14, ///< CoAP
    OT_LOG_REGION_CLI      = 15, ///< CLI
    OT_LOG_REGION_CORE     = 16, ///< OpenThread Core
    OT_LOG_REGION_UTIL     = 17, ///< Utility module
    OT_LOG_REGION_BBR      = 18, ///< Backbone Router (available since Thread 1.2)
    OT_LOG_REGION_MLR      = 19, ///< Multicast Listener Registration (available since Thread 1.2)
    OT_LOG_REGION_DUA      = 20, ///< Domain Unicast Address (available since Thread 1.2)
    OT_LOG_REGION_BR       = 21, ///< Border Router
    OT_LOG_REGION_SRP      = 22, ///< Service Registration Protocol (SRP)
    OT_LOG_REGION_DNS      = 23, ///< DNS
} otLogRegion;

/**
 * This function outputs logs.
 *
 * @param[in]  aLogLevel   The log level.
 * @param[in]  aLogRegion  The log region.
 * @param[in]  aFormat     A pointer to the format string.
 * @param[in]  ...         Arguments for the format specification.
 *
 */
void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...);

/**
 * This (optional) platform function outputs a prepared log line.
 *
 * This platform function is used by OpenThread core when `OPENTHREAD_CONFIG_LOG_DEFINE_AS_MACRO_ONLY` is not enabled
 * (in this case, the OT core itself will prepare a full log line).
 *
 * Note that this function is optional and if not provided by platform layer, a default (weak) implementation is
 * provided and used by OpenThread core as `otPlatLog(aLogLevel, aLogResion, "%s", aLogLine)`.
 *
 * @param[in]  aLogLevel   The log level.
 * @param[in]  aLogRegion  The log region.
 * @param[in]  aLogLine    A pointer to a log line string.
 *
 */
void otPlatLogLine(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aLogLine);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_PLATFORM_LOGGING_H_
