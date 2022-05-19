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

#ifndef OPENTHREAD_PLATFORM_CONFIG_H_
#define OPENTHREAD_PLATFORM_CONFIG_H_

#include "openthread-core-config.h"

/**
 * @file
 * @brief
 *   This file includes the POSIX platform-specific configurations.
 */

/**
 * @def OPENTHREAD_POSIX_CONFIG_RCP_PTY_ENABLE
 *
 * Define as 1 to enable PTY RCP support.
 *
 */
#ifndef OPENTHREAD_POSIX_CONFIG_RCP_PTY_ENABLE
#define OPENTHREAD_POSIX_CONFIG_RCP_PTY_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_POSIX_APP_TREL_INTERFACE_NAME
 *
 * Defines the default interface name used for TREL UDP6 platform. Empty string disables TREL platform.
 *
 */
#ifndef OPENTHREAD_CONFIG_POSIX_APP_TREL_INTERFACE_NAME
#define OPENTHREAD_CONFIG_POSIX_APP_TREL_INTERFACE_NAME ""
#endif

/**
 * @def OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET
 *
 * Defines whether the TREL UDP6 platform uses netlink socket to add/remove addresses on the TREL netif or `ioctl()`
 * command.
 *
 * When netlink is used Duplicate Address Detection (DAD) is disabled when a new address is added on the netif.
 *
 * Use of netlink is enabled by default on linux-based platforms.
 *
 */
#ifndef OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET
#ifdef __linux__
#define OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET 1
#else
#define OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET 0
#endif
#endif

/**
 * @def OPENTHREAD_POSIX_CONFIG_DAEMON_SOCKET_BASENAME
 *
 * Define socket basename used by POSIX app daemon.
 *
 */
#ifndef OPENTHREAD_POSIX_CONFIG_DAEMON_SOCKET_BASENAME
#ifdef __linux__
#define OPENTHREAD_POSIX_CONFIG_DAEMON_SOCKET_BASENAME "/run/openthread-%s"
#else
#define OPENTHREAD_POSIX_CONFIG_DAEMON_SOCKET_BASENAME "/tmp/openthread-%s"
#endif
#endif

/**
 * @def OPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE
 *
 * Define to 1 to enable POSIX daemon.
 *
 */
#ifndef OPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE
#define OPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE 0
#endif

/**
 * RCP bus UART.
 *
 * @note This value is also for simulated UART bus.
 *
 */
#define OT_POSIX_RCP_BUS_UART 1

/**
 * RCP bus SPI.
 *
 */
#define OT_POSIX_RCP_BUS_SPI 2

/**
 * @def OPENTHREAD_POSIX_CONFIG_RCP_BUS
 *
 * This setting configures what type of RCP bus to use.
 *
 */
#ifndef OPENTHREAD_POSIX_CONFIG_RCP_BUS
#define OPENTHREAD_POSIX_CONFIG_RCP_BUS OT_POSIX_RCP_BUS_UART
#endif

/**
 * @def OPENTHREAD_POSIX_CONFIG_MAX_POWER_TABLE_ENABLE
 *
 * Define as 1 to enable max power table support.
 *
 */
#ifndef OPENTHREAD_POSIX_CONFIG_MAX_POWER_TABLE_ENABLE
#define OPENTHREAD_POSIX_CONFIG_MAX_POWER_TABLE_ENABLE 0
#endif

/**
 * @def OPENTHREAD_POSIX_CONFIG_MAX_MULTICAST_FORWARDING_CACHE_TABLE
 *
 * This setting configures the maximum number of Multicast Forwarding Cache table for POSIX native multicast routing.
 *
 */
#ifndef OPENTHREAD_POSIX_CONFIG_MAX_MULTICAST_FORWARDING_CACHE_TABLE
#define OPENTHREAD_POSIX_CONFIG_MAX_MULTICAST_FORWARDING_CACHE_TABLE (OPENTHREAD_CONFIG_MAX_MULTICAST_LISTENERS * 10)
#endif

/**
 * @def OPENTHREAD_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
 *
 * Define as 1 to enable the secure settings. When defined to 1, the platform MUST implement the otPosixSecureSetting*
 * APIs defined in 'src/posix/platform/include/openthread/platform/secure_settings.h'.
 *
 */
#ifndef OPENTHREAD_POSIX_CONFIG_SECURE_SETTINGS_ENABLE
#define OPENTHREAD_POSIX_CONFIG_SECURE_SETTINGS_ENABLE 0
#endif

/**
 * @def OPENTHREAD_POSIX_CONFIG_INSTALL_EXTERNAL_ROUTES_ENABLE
 *
 * Define as 1 to add external routes to POSIX kernel when external routes are changed in netdata.
 *
 */
#ifdef __linux__
#ifndef OPENTHREAD_POSIX_CONFIG_INSTALL_EXTERNAL_ROUTES_ENABLE
#define OPENTHREAD_POSIX_CONFIG_INSTALL_EXTERNAL_ROUTES_ENABLE 1
#endif
#endif

/**
 * @def OPENTHREAD_POSIX_CONFIG_EXTERNAL_ROUTE_PRIORITY
 *
 * This macro defines the priority of external routes added to kernel.
 *
 */
#ifndef OPENTHREAD_POSIX_CONFIG_EXTERNAL_ROUTE_PRIORITY
#define OPENTHREAD_POSIX_CONFIG_EXTERNAL_ROUTE_PRIORITY 512
#endif

/**
 * @def OPENTHREAD_POSIX_CONFIG_MAX_EXTERNAL_ROUTE_NUM
 *
 * This macro defines the max number of external routes that can be added to kernel.
 *
 */
#ifndef OPENTHREAD_POSIX_CONFIG_MAX_EXTERNAL_ROUTE_NUM
#define OPENTHREAD_POSIX_CONFIG_MAX_EXTERNAL_ROUTE_NUM 8
#endif

#ifdef __APPLE__

/**
 * Use built-in utun driver on mac OS
 */
#define OT_POSIX_CONFIG_MACOS_UTUN 1

/**
 * Use open-source tun driver on mac OS
 */
#define OT_POSIX_CONFIG_MACOS_TUN 2

/**
 * @def OPENTHREAD_POSIX_CONFIG_MACOS_TUN_OPTION
 *
 * This setting configures which tunnel driver to use.
 *
 */
#ifndef OPENTHREAD_POSIX_CONFIG_MACOS_TUN_OPTION
#define OPENTHREAD_POSIX_CONFIG_MACOS_TUN_OPTION OT_POSIX_CONFIG_MACOS_UTUN
#endif

#endif // __APPLE__

#endif // OPENTHREAD_PLATFORM_CONFIG_H_
