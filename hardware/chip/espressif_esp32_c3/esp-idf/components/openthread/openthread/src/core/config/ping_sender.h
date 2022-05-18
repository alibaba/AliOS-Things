/*
 *  Copyright (c) 2021, The OpenThread Authors.
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
 *   This file includes compile-time configurations for ping sender module.
 *
 */

#ifndef CONFIG_PING_SENDER_H_
#define CONFIG_PING_SENDER_H_

/**
 * @def OPENTHREAD_CONFIG_PING_SENDER_ENABLE
 *
 * Define to 1 to enable ping sender module.
 *
 * Ping sender module implements sending ICMPv6 Echo Request messages and processing ICMPv6 Echo Reply messages.
 *
 */
#ifndef OPENTHREAD_CONFIG_PING_SENDER_ENABLE
#define OPENTHREAD_CONFIG_PING_SENDER_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_PING_SENDER_DEFAULT_INTEVRAL
 *
 * Specifies the default ping interval (time between sending echo requests) in milliseconds.
 *
 */
#ifndef OPENTHREAD_CONFIG_PING_SENDER_DEFAULT_INTEVRAL
#define OPENTHREAD_CONFIG_PING_SENDER_DEFAULT_INTEVRAL 1000
#endif

/**
 * @def OPENTHREAD_CONFIG_PING_SENDER_DEFAULT_DEADLINE
 *
 * Specifies the default ping timeout in milliseconds. The timeout is the max acceptable time gap between each ping
 * request and its reply.
 *
 */
#ifndef OPENTHREAD_CONFIG_PING_SENDER_DEFAULT_TIMEOUT
#define OPENTHREAD_CONFIG_PING_SENDER_DEFAULT_TIMEOUT 3000
#endif

/**
 * @def OPENTHREAD_CONFIG_PING_SENDER_DEFAULT_SIZE
 *
 * Specifies the default ping data size in bytes. The data size specifies the Echo Request data payload which excludes
 * the IPv6 and ICMPv6 headers.
 *
 */
#ifndef OPENTHREAD_CONFIG_PING_SENDER_DEFAULT_SIZE
#define OPENTHREAD_CONFIG_PING_SENDER_DEFAULT_SIZE 8
#endif

/**
 * @def OPENTHREAD_CONFIG_PING_SENDER_DEFAULT_COUNT
 *
 * Specifies the default ping count (number of ping messages to send).
 *
 */
#ifndef OPENTHREAD_CONFIG_PING_SENDER_DEFAULT_COUNT
#define OPENTHREAD_CONFIG_PING_SENDER_DEFAULT_COUNT 1
#endif

#endif // CONFIG_PING_SENDER_H_
