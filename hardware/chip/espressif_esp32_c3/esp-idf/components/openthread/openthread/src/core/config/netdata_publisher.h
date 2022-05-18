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
 *   This file includes compile-time configurations for Network Data Publisher.
 *
 */

#ifndef CONFIG_NETDATA_PUBLISHER_H_
#define CONFIG_NETDATA_PUBLISHER_H_

#include "config/srp_server.h"

/**
 * @def OPENTHREAD_CONFIG_NETDATA_PUBLISHER_ENABLE
 *
 * Define to 1 to enable Network Data Publisher.
 *
 * Network Data Publisher provides mechanisms to limit the number of similar Service and/or Prefix (on-mesh prefix
 * or external route) entries in the Thread Network Data by monitoring the Network Data and managing if or when to add
 * or remove entries.
 *
 */
#ifndef OPENTHREAD_CONFIG_NETDATA_PUBLISHER_ENABLE
#define OPENTHREAD_CONFIG_NETDATA_PUBLISHER_ENABLE OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
#endif

/**
 * @def OPENTHREAD_CONFIG_NETDATA_PUBLISHER_MAX_DELAY_TO_ADD
 *
 * Specifies the maximum value (in milliseconds) of the random delay used before adding an entry in the Thread Network
 * Data.
 *
 */
#ifndef OPENTHREAD_CONFIG_NETDATA_PUBLISHER_MAX_DELAY_TO_ADD
#define OPENTHREAD_CONFIG_NETDATA_PUBLISHER_MAX_DELAY_TO_ADD 3500
#endif

/**
 * @def OPENTHREAD_CONFIG_NETDATA_PUBLISHER_MAX_DELAY_TO_REMOVE
 *
 * Specifies the maximum value (in milliseconds) of the random wait time used before removing an entry from the Thread
 * Network Data.
 *
 */
#ifndef OPENTHREAD_CONFIG_NETDATA_PUBLISHER_MAX_DELAY_TO_REMOVE
#define OPENTHREAD_CONFIG_NETDATA_PUBLISHER_MAX_DELAY_TO_REMOVE 15000
#endif

/**
 * @def OPENTHREAD_CONFIG_NETDATA_PUBLISHER_EXTRA_DELAY_TIME_TO_REMOVE_PREFERRED
 *
 * Specifies the extra wait time (in milliseconds) used when removing a preferred entry.
 *
 * When removing entries, certain entries are preferred over others. How the preference is determined depends on the
 * entry type (e.g., (for DNS/SRP service entries associated with smaller RLOC16 or smaller IPv6 address are preferred).
 * If the `Publisher` determines that its own entry is a preferred one, it adds the above extra wait time in addition
 * to the random wait selected based on `OPENTHREAD_CONFIG_NETDATA_PUBLISHER_MAX_DELAY_TO_REMOVE` before removing
 * its entry. This gives higher chance for a non-preferred entry from another device to be removed before removing a
 * preferred entry which helps towards quicker convergence of the process to the desired number of entries.
 *
 * It is recommended that `OPENTHREAD_CONFIG_NETDATA_PUBLISHER_EXTRA_DELAY_TIME_TO_REMOVE_PREFERRED` to be set to a
 * value larger than `OPENTHREAD_CONFIG_NETDATA_PUBLISHER_MAX_DELAY_TO_REMOVE` to ensure that non-preferred entries
 * are removed first.
 *
 */
#ifndef OPENTHREAD_CONFIG_NETDATA_PUBLISHER_EXTRA_DELAY_TIME_TO_REMOVE_PREFERRED
#define OPENTHREAD_CONFIG_NETDATA_PUBLISHER_EXTRA_DELAY_TIME_TO_REMOVE_PREFERRED 16000
#endif

/**
 * @def OPENTHREAD_CONFIG_NETDATA_PUBLISHER_DESIRED_NUM_ANYCAST_DNS_SRP_SERVICE_ENTRIES
 *
 * Specifies the desired number of "DNS/SRP Service Anycast Address" entries in the Thread Network Data.
 *
 * Publisher attempts to limit the number of such entries to this value.
 *
 */
#ifndef OPENTHREAD_CONFIG_NETDATA_PUBLISHER_DESIRED_NUM_ANYCAST_DNS_SRP_SERVICE_ENTRIES
#define OPENTHREAD_CONFIG_NETDATA_PUBLISHER_DESIRED_NUM_ANYCAST_DNS_SRP_SERVICE_ENTRIES 8
#endif

/**
 * @def OPENTHREAD_CONFIG_NETDATA_PUBLISHER_DESIRED_NUM_UNICAST_DNS_SRP_SERVICE_ENTRIES
 *
 * Specifies the desired number of "DNS/SRP Service Anycast Address" entries in the Thread Network Data.
 *
 * Publisher attempts to limit the number of such entries to this value.
 *
 */
#ifndef OPENTHREAD_CONFIG_NETDATA_PUBLISHER_DESIRED_NUM_UNICAST_DNS_SRP_SERVICE_ENTRIES
#define OPENTHREAD_CONFIG_NETDATA_PUBLISHER_DESIRED_NUM_UNICAST_DNS_SRP_SERVICE_ENTRIES 2
#endif

/**
 * @def OPENTHREAD_CONFIG_NETDATA_PUBLISHER_DESIRED_NUM_ON_MESH_PREFIX_ENTRIES
 *
 * Specifies the desired number of matching On-mesh Prefix entries in the Thread Network Data.
 *
 * Publisher attempts to limit the number of matching entries to this value.
 *
 */
#ifndef OPENTHREAD_CONFIG_NETDATA_PUBLISHER_DESIRED_NUM_ON_MESH_PREFIX_ENTRIES
#define OPENTHREAD_CONFIG_NETDATA_PUBLISHER_DESIRED_NUM_ON_MESH_PREFIX_ENTRIES 3
#endif

/**
 * @def OPENTHREAD_CONFIG_NETDATA_PUBLISHER_DESIRED_NUM_EXTERNAL_ROUTE_ENTRIES
 *
 * Specifies the desired number of matching External Route entries in the Thread Network Data.
 *
 * Publisher attempts to limit the number of matching entries to this value.
 *
 */
#ifndef OPENTHREAD_CONFIG_NETDATA_PUBLISHER_DESIRED_NUM_EXTERNAL_ROUTE_ENTRIES
#define OPENTHREAD_CONFIG_NETDATA_PUBLISHER_DESIRED_NUM_EXTERNAL_ROUTE_ENTRIES 10
#endif

/**
 * @def OPENTHREAD_CONFIG_NETDATA_PUBLISHER_MAX_PREFIX_ENTRIES
 *
 * Specifies maximum number of prefix (on-mesh prefix or external route) entries supported by Publisher.
 *
 */
#ifndef OPENTHREAD_CONFIG_NETDATA_PUBLISHER_MAX_PREFIX_ENTRIES
#define OPENTHREAD_CONFIG_NETDATA_PUBLISHER_MAX_PREFIX_ENTRIES 3
#endif

#endif // CONFIG_NETDATA_PUBLISHER_H_
