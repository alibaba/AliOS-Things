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
 *   This file includes compile-time configurations for the DNS Client.
 *
 */

#ifndef CONFIG_DNS_CLIENT_H_
#define CONFIG_DNS_CLIENT_H_

#include "config/srp_client.h"

/**
 * @def OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
 *
 * Define to 1 to enable DNS Client support.
 *
 */
#ifndef OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
#define OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
 *
 * Define to 1 to enable support for NAT64 address translation (from IPv4 to IPv6) during address resolution by DNS
 * client.
 *
 */
#ifndef OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE
#define OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_NAT64_ALLOWED
 *
 * Specifies the default NAT64 mode, i.e., whether to allow or disallow NAT64 address translation during DNS client
 * address resolution. This mode is only available when `OPENTHREAD_CONFIG_DNS_CLIENT_NAT64_ENABLE` is enabled.
 *
 */
#ifndef OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_NAT64_ALLOWED
#define OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_NAT64_ALLOWED 1
#endif

/**
 * @def OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
 *
 * Define to 1 to enable DNS based Service Discovery (DNS-SD) client.
 *
 */
#ifndef OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
#define OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_ADDRESS_AUTO_SET_ENABLE
 *
 * Set to 1 for DNS client to automatically set and update the server IPv6 address in the default config (when it is
 * not explicitly set by user).
 *
 * This feature requires SRP client and its auto-start feature to be also enabled. SRP client will then monitor the
 * Thread Network Data for DNS/SRP Service entries to select an SRP server. The selected SRP server address is also set
 * as the DNS server address in the default config.
 *
 */
#ifndef OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_ADDRESS_AUTO_SET_ENABLE
#define OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_ADDRESS_AUTO_SET_ENABLE \
    (OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE && OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE)
#endif

/**
 * @def OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_IP6_ADDRESS
 *
 * Specifies the default DNS server IPv6 address.
 *
 * It MUST be a C string representation of the server IPv6 address.
 *
 * Default value is set to "2001:4860:4860::8888" which is the Google Public DNS IPv6 address.
 *
 */
#ifndef OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_IP6_ADDRESS
#define OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_IP6_ADDRESS "2001:4860:4860::8888"
#endif

/**
 * @def OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_PORT
 *
 * Specifies the default DNS server port number.
 *
 */
#ifndef OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_PORT
#define OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_PORT 53
#endif

/**
 * @def OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_RESPONSE_TIMEOUT
 *
 * Specifies the default wait time that DNS client waits for a response from server (in milliseconds).
 *
 */
#ifndef OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_RESPONSE_TIMEOUT
#define OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_RESPONSE_TIMEOUT 6000
#endif

/**
 * @def OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_MAX_TX_ATTEMPTS
 *
 * Specifies the default maximum number of DNS query tx attempts with no response before reporting failure.
 *
 */
#ifndef OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_MAX_TX_ATTEMPTS
#define OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_MAX_TX_ATTEMPTS 3
#endif

/**
 * @def OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_NO_RECURSION_FLAG
 *
 * Specifies the default "recursion desired" flag (indicates whether the server can resolve the query recursively or
 * not).
 *
 */
#ifndef OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_RECURSION_DESIRED_FLAG
#define OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_RECURSION_DESIRED_FLAG 1
#endif

#endif // CONFIG_DNS_CLIENT_H_
