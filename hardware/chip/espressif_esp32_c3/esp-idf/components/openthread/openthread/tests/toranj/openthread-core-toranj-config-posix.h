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

#ifndef OPENTHREAD_CORE_TORANJ_CONFIG_POSIX_H_
#define OPENTHREAD_CORE_TORANJ_CONFIG_POSIX_H_

/**
 * This header file defines the OpenThread core configuration options for toranj with POSIX platform.
 *
 */

// Include the common configuration for all platforms.
#include "openthread-core-toranj-config.h"

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_INFO
 *
 * The platform-specific string to insert into the OpenThread version string.
 *
 */
#define OPENTHREAD_CONFIG_PLATFORM_INFO "POSIX-toranj"

/**
 * @def OPENTHREAD_CONFIG_LOG_OUTPUT
 *
 * Selects if, and where the LOG output goes to.
 *
 */
#define OPENTHREAD_CONFIG_LOG_OUTPUT OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED

/**
 * @def OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
 *
 * Define to 1 to enable Border Router support.
 *
 */
#define OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE 1

/**
 * @def OPENTHREAD_CONFIG_POSIX_APP_TREL_INTERFACE_NAME
 *
 * Defines the default interface name used for TREL UDP6 platform. Empty string disables TREL platform.
 *
 */
#define OPENTHREAD_CONFIG_POSIX_APP_TREL_INTERFACE_NAME "trel"

/**
 * @def OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET
 *
 * Defines whether the TREL UDP6 platform uses netlink socket to add/remove addresses on the TREL netif or `ioctl()`
 * command.
 *
 * When netlink is used Duplicate Address Detection (DAD) is disabled when a new address is added on the netif.
 *
 */
#ifdef __linux__
#define OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET 1
#else
#define OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET 0
#endif

/**
 * @def OPENTHREAD_POSIX_CONFIG_RCP_PTY_ENABLE
 *
 * Define as 1 to enable PTY device support in POSIX app.
 *
 */
#define OPENTHREAD_POSIX_CONFIG_RCP_PTY_ENABLE 1

#define OPENTHREAD_POSIX_CONFIG_RCP_BUS OT_POSIX_RCP_BUS_UART

#endif /* OPENTHREAD_CORE_TORANJ_CONFIG_POSIX_H_ */
