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
 *   This file includes compile-time configurations for the MLE service.
 *
 */

#ifndef CONFIG_MLE_H_
#define CONFIG_MLE_H_

/**
 * @def OPENTHREAD_CONFIG_MLE_MAX_ROUTERS
 *
 * The maximum number of routers in a Thread network.
 *
 * @note Thread specifies this value to be 32.  Changing this value may cause interoperability issues with standard
 *       Thread 1.1 devices.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_MAX_ROUTERS
#define OPENTHREAD_CONFIG_MLE_MAX_ROUTERS 32
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_MAX_CHILDREN
 *
 * The maximum number of children.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_MAX_CHILDREN
#define OPENTHREAD_CONFIG_MLE_MAX_CHILDREN 10
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_CHILD_TIMEOUT_DEFAULT
 *
 * The default child timeout value (in seconds).
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_CHILD_TIMEOUT_DEFAULT
#define OPENTHREAD_CONFIG_MLE_CHILD_TIMEOUT_DEFAULT 240
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_IP_ADDRS_PER_CHILD
 *
 * The maximum number of supported IPv6 address registrations per child.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_IP_ADDRS_PER_CHILD
#define OPENTHREAD_CONFIG_MLE_IP_ADDRS_PER_CHILD 4
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_IP_ADDRS_TO_REGISTER
 *
 * The maximum number of IPv6 address registrations for MTD.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_IP_ADDRS_TO_REGISTER
#define OPENTHREAD_CONFIG_MLE_IP_ADDRS_TO_REGISTER (OPENTHREAD_CONFIG_MLE_IP_ADDRS_PER_CHILD)
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_STEERING_DATA_SET_OOB_ENABLE
 *
 * Enable setting steering data out of band.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_STEERING_DATA_SET_OOB_ENABLE
#define OPENTHREAD_CONFIG_MLE_STEERING_DATA_SET_OOB_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_ENABLE
 *
 * Define as 1 to enable attach backoff feature
 *
 * When this feature is enabled, an exponentially increasing backoff wait time is added between attach attempts.
 * If device is sleepy, the radio will be put to sleep during the wait time. This ensures that a battery-powered sleepy
 * end-device does not drain its battery by continuously searching for a parent to attach to (when there is no
 * router/parent for it to attach).
 *
 * The backoff time starts from a minimum interval specified by
 * `OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_MINIMUM_INTERVAL`, and every attach attempt the wait time is doubled up to
 * `OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_MAXIMUM_INTERVAL` which specifies the maximum wait time.
 *
 * Once the wait time reaches the maximum, a random jitter interval is added to it. The maximum value for jitter is
 * specified by `OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_JITTER_INTERVAL`. The random jitter is selected uniformly within
 * range `[-JITTER, +JITTER]`. It is only added when the backoff wait interval is at maximum value.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_ENABLE
#define OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_MINIMUM_INTERVAL
 *
 * Specifies the minimum backoff wait interval (in milliseconds) used by attach backoff feature.
 *
 * Applicable only if attach backoff feature is enabled (see `OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_ENABLE`).
 *
 * Please see `OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_ENABLE` description for more details.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_MINIMUM_INTERVAL
#define OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_MINIMUM_INTERVAL 251
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_MAXIMUM_INTERVAL
 *
 * Specifies the maximum backoff wait interval (in milliseconds) used by attach backoff feature.
 *
 * Applicable only if attach backoff feature is enabled (see `OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_ENABLE`).
 *
 * Please see `OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_ENABLE` description for more details.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_MAXIMUM_INTERVAL
#define OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_MAXIMUM_INTERVAL 1200000 // 1200 seconds = 20 minutes
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_JITTER_INTERVAL
 *
 * Specifies the maximum jitter interval (in milliseconds) used by attach backoff feature.
 *
 * Applicable only if attach backoff feature is enabled (see `OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_ENABLE`).
 *
 * Please see `OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_ENABLE` description for more details.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_JITTER_INTERVAL
#define OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_JITTER_INTERVAL 2000
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_DELAY_TO_RESET_BACKOFF_INTERVAL
 *
 * Specifies the delay wait interval (in milliseconds) used by attach backoff feature after a successful attach before
 * it resets the current backoff interval back to the minimum value.
 *
 * If it is set to zero then the device resets its backoff attach interval immediately after a successful attach. With
 * a non-zero value, if after a successful attach, the device happens to detach within the delay interval, the reattach
 *  process resumes with the previous backoff interval (as if the attach did not happen).
 *
 * This behavior is helpful in the situation where a battery-powered device has poor link quality to its parent and
 * therefore attaches and detaches frequently from the parent.  Using a non-zero wait interval ensures that the attach
 * backoff interval does not reset on each attach and that the device does not drain its battery quickly trying to
 * re-attach too frequently.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_DELAY_TO_RESET_BACKOFF_INTERVAL
#define OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_DELAY_TO_RESET_BACKOFF_INTERVAL 20000
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_SEND_LINK_REQUEST_ON_ADV_TIMEOUT
 *
 * Define to 1 to send an MLE Link Request when MAX_NEIGHBOR_AGE is reached for a neighboring router.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_SEND_LINK_REQUEST_ON_ADV_TIMEOUT
#define OPENTHREAD_CONFIG_MLE_SEND_LINK_REQUEST_ON_ADV_TIMEOUT 1
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_LINK_REQUEST_MARGIN_MIN
 *
 * Specifies the minimum link margin in dBm required before attempting to establish a link with a neighboring router.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_LINK_REQUEST_MARGIN_MIN
#define OPENTHREAD_CONFIG_MLE_LINK_REQUEST_MARGIN_MIN 10
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_PARTITION_MERGE_MARGIN_MIN
 *
 * Specifies the minimum link margin in dBm required before attempting to merge to a different partition.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_PARTITION_MERGE_MARGIN_MIN
#define OPENTHREAD_CONFIG_MLE_PARTITION_MERGE_MARGIN_MIN 10
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_CHILD_ROUTER_LINKS
 *
 * Specifies the desired number of router links that a REED / FED attempts to maintain.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_CHILD_ROUTER_LINKS
#define OPENTHREAD_CONFIG_MLE_CHILD_ROUTER_LINKS 3
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_LONG_ROUTES_ENABLE
 *
 * Enable experimental mode for 'deep' networks, allowing packet routes up to 32 nodes.
 * This mode is incompatible with Thread 1.1.1 and older.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_LONG_ROUTES_ENABLE
#define OPENTHREAD_CONFIG_MLE_LONG_ROUTES_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_SEND_UNICAST_ANNOUNCE_RESPONSE
 *
 * Define as 1 to enable sending of a unicast MLE Announce message in response to a received Announce message from
 * a device.
 *
 * @note The unicast MLE announce message is sent in addition to (and after) the multicast MLE Announce.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_SEND_UNICAST_ANNOUNCE_RESPONSE
#define OPENTHREAD_CONFIG_MLE_SEND_UNICAST_ANNOUNCE_RESPONSE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_INFORM_PREVIOUS_PARENT_ON_REATTACH
 *
 * Define as 1 for a child to inform its previous parent when it attaches to a new parent.
 *
 * If this feature is enabled, when a device attaches to a new parent, it will send an IP message (with empty payload
 * and mesh-local IP address as the source address) to its previous parent.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_INFORM_PREVIOUS_PARENT_ON_REATTACH
#define OPENTHREAD_CONFIG_MLE_INFORM_PREVIOUS_PARENT_ON_REATTACH 0
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
 *
 * Define as 1 to enable Link Metrics initiator feature.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
#define OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
 *
 * Define as 1 to enable Link Metrics subject feature.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
#define OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_LINK_METRICS_MAX_SERIES_SUPPORTED
 *
 * The max number of series that a Link Metrics Subject can track simultaneously.
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_LINK_METRICS_MAX_SERIES_SUPPORTED
#define OPENTHREAD_CONFIG_MLE_LINK_METRICS_MAX_SERIES_SUPPORTED 10
#endif

#endif // CONFIG_MLE_H_
