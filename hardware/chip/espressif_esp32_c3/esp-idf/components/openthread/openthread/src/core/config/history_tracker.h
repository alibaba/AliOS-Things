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
 *   This file includes compile-time configurations for History Tracker module.
 *
 */

#ifndef CONFIG_HISTORY_TRACKER_H_
#define CONFIG_HISTORY_TRACKER_H_

/**
 * @def OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
 *
 * Define as 1 to enable History Tracker module.
 *
 */
#ifndef OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
#define OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_HISTORY_TRACKER_NET_INFO_LIST_SIZE
 *
 * Specifies the maximum number of entries in Network Info (role, mode, partition ID, RLOC16) history list.
 *
 * Can be set to zero to configure History Tracker module not to collect any entries.
 *
 */
#ifndef OPENTHREAD_CONFIG_HISTORY_TRACKER_NET_INFO_LIST_SIZE
#define OPENTHREAD_CONFIG_HISTORY_TRACKER_NET_INFO_LIST_SIZE 32
#endif

/**
 * @def OPENTHREAD_CONFIG_HISTORY_TRACKER_UNICAST_ADDRESS_LIST_SIZE
 *
 * Specifies the maximum number of entries in unicast IPv6 address history list.
 *
 * Can be set to zero to configure History Tracker module not to collect any entries.
 *
 */
#ifndef OPENTHREAD_CONFIG_HISTORY_TRACKER_UNICAST_ADDRESS_LIST_SIZE
#define OPENTHREAD_CONFIG_HISTORY_TRACKER_UNICAST_ADDRESS_LIST_SIZE 20
#endif

/**
 * @def OPENTHREAD_CONFIG_HISTORY_TRACKER_MULTICAST_ADDRESS_LIST_SIZE
 *
 * Specifies the maximum number of entries in multicast IPv6 address history list.
 *
 * Can be set to zero to configure History Tracker module not to collect any entries.
 *
 */
#ifndef OPENTHREAD_CONFIG_HISTORY_TRACKER_MULTICAST_ADDRESS_LIST_SIZE
#define OPENTHREAD_CONFIG_HISTORY_TRACKER_MULTICAST_ADDRESS_LIST_SIZE 20
#endif

/**
 * @def OPENTHREAD_CONFIG_HISTORY_TRACKER_RX_LIST_SIZE
 *
 * Specifies the maximum number of entries in RX history list.
 *
 * Can be set to zero to configure History Tracker module not to collect any RX history.
 *
 */
#ifndef OPENTHREAD_CONFIG_HISTORY_TRACKER_RX_LIST_SIZE
#define OPENTHREAD_CONFIG_HISTORY_TRACKER_RX_LIST_SIZE 32
#endif

/**
 * @def OPENTHREAD_CONFIG_HISTORY_TRACKER_TX_LIST_SIZE
 *
 * Specifies the maximum number of entries in TX history list.
 *
 * Can be set to zero to configure History Tracker module not to collect any TX history.
 *
 */
#ifndef OPENTHREAD_CONFIG_HISTORY_TRACKER_TX_LIST_SIZE
#define OPENTHREAD_CONFIG_HISTORY_TRACKER_TX_LIST_SIZE 32
#endif

/**
 * @def OPENTHREAD_CONFIG_HISTORY_TRACKER_EXCLUDE_THREAD_CONTROL_MESSAGES
 *
 * Define as 1 to exclude Thread Control message (e.g., MLE, TMF) from TX and RX history.
 *
 */
#ifndef OPENTHREAD_CONFIG_HISTORY_TRACKER_EXCLUDE_THREAD_CONTROL_MESSAGES
#define OPENTHREAD_CONFIG_HISTORY_TRACKER_EXCLUDE_THREAD_CONTROL_MESSAGES 1
#endif

/**
 * @def OPENTHREAD_CONFIG_HISTORY_TRACKER_NEIGHBOR_LIST_SIZE
 *
 * Specifies the maximum number of entries in neighbor table history list.
 *
 * Can be set to zero to configure History Tracker module not to collect any neighbor table history.
 *
 */
#ifndef OPENTHREAD_CONFIG_HISTORY_TRACKER_NEIGHBOR_LIST_SIZE
#define OPENTHREAD_CONFIG_HISTORY_TRACKER_NEIGHBOR_LIST_SIZE 64
#endif

#endif // CONFIG_HISTORY_TRACKER_H_
