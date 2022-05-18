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
 *   This file includes compile-time configurations for Channel Manager.
 *
 */

#ifndef CONFIG_CHANNEL_MANAGER_H_
#define CONFIG_CHANNEL_MANAGER_H_

/**
 * @def OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE
 *
 * Define as 1 to enable Channel Manager support.
 *
 */
#ifndef OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE
#define OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_CHANNEL_MANAGER_MINIMUM_DELAY
 *
 * The minimum delay (in seconds) used by Channel Manager module for performing a channel change.
 *
 * The minimum delay should preferably be longer than maximum data poll interval used by all sleepy-end-devices within
 * the Thread network.
 *
 * Applicable only if Channel Manager feature is enabled (i.e., `OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE` is set).
 *
 */
#ifndef OPENTHREAD_CONFIG_CHANNEL_MANAGER_MINIMUM_DELAY
#define OPENTHREAD_CONFIG_CHANNEL_MANAGER_MINIMUM_DELAY 120
#endif

/**
 * @def OPENTHREAD_CONFIG_CHANNEL_MANAGER_MINIMUM_MONITOR_SAMPLE_COUNT
 *
 * The minimum number of RSSI samples per channel by Channel Monitoring feature before the collected data can be used
 * by the Channel Manager module to (auto) select a better channel.
 *
 * Applicable only if Channel Manager and Channel Monitoring features are both enabled (i.e.,
 * `OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE` and `OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE` are set).
 *
 */
#ifndef OPENTHREAD_CONFIG_CHANNEL_MANAGER_MINIMUM_MONITOR_SAMPLE_COUNT
#define OPENTHREAD_CONFIG_CHANNEL_MANAGER_MINIMUM_MONITOR_SAMPLE_COUNT 500
#endif

/**
 * @def OPENTHREAD_CONFIG_CHANNEL_MANAGER_THRESHOLD_TO_SKIP_FAVORED
 *
 * This threshold specifies the minimum occupancy rate difference between two channels for the Channel Manager to
 * prefer an unfavored channel over the best favored one. This is used when (auto) selecting a channel based on the
 * collected channel quality data by "channel monitor" feature.
 *
 * The difference is based on the `ChannelMonitor::GetChannelOccupancy()` definition which provides the average
 * percentage of RSSI samples (within a time window) indicating that channel was busy (i.e., RSSI value higher than
 * a threshold). Value 0 maps to 0% and 0xffff maps to 100%.
 *
 * Applicable only if Channel Manager feature is enabled (i.e., `OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE` is set).
 *
 */
#ifndef OPENTHREAD_CONFIG_CHANNEL_MANAGER_THRESHOLD_TO_SKIP_FAVORED
#define OPENTHREAD_CONFIG_CHANNEL_MANAGER_THRESHOLD_TO_SKIP_FAVORED (0xffff * 7 / 100)
#endif

/**
 * @def OPENTHREAD_CONFIG_CHANNEL_MANAGER_THRESHOLD_TO_CHANGE_CHANNEL
 *
 * This threshold specifies the minimum occupancy rate difference required between the current channel and a newly
 * selected channel for Channel Manager to allow channel change to the new channel.
 *
 * The difference is based on the `ChannelMonitor::GetChannelOccupancy()` definition which provides the average
 * percentage of RSSI samples (within a time window) indicating that channel was busy (i.e., RSSI value higher than
 * a threshold). Value 0 maps to 0% rate and 0xffff maps to 100%.
 *
 * Applicable only if Channel Manager feature is enabled (i.e., `OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE` is set).
 *
 */
#ifndef OPENTHREAD_CONFIG_CHANNEL_MANAGER_THRESHOLD_TO_CHANGE_CHANNEL
#define OPENTHREAD_CONFIG_CHANNEL_MANAGER_THRESHOLD_TO_CHANGE_CHANNEL (0xffff * 10 / 100)
#endif

/**
 * @def OPENTHREAD_CONFIG_CHANNEL_MANAGER_DEFAULT_AUTO_SELECT_INTERVAL
 *
 * The default time interval (in seconds) used by Channel Manager for auto-channel-selection functionality.
 *
 * Applicable only if Channel Manager feature is enabled (i.e., `OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE` is set).
 *
 */
#ifndef OPENTHREAD_CONFIG_CHANNEL_MANAGER_DEFAULT_AUTO_SELECT_INTERVAL
#define OPENTHREAD_CONFIG_CHANNEL_MANAGER_DEFAULT_AUTO_SELECT_INTERVAL (3 * 60 * 60)
#endif

/**
 * @def OPENTHREAD_CONFIG_CHANNEL_MANAGER_CCA_FAILURE_THRESHOLD
 *
 * Default minimum CCA failure rate threshold on current channel before Channel Manager starts channel selection
 * attempt.
 *
 * Value 0 maps to 0% and 0xffff maps to 100%.
 *
 * Applicable only if Channel Manager feature is enabled (i.e., `OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE` is set).
 *
 */
#ifndef OPENTHREAD_CONFIG_CHANNEL_MANAGER_CCA_FAILURE_THRESHOLD
#define OPENTHREAD_CONFIG_CHANNEL_MANAGER_CCA_FAILURE_THRESHOLD (0xffff * 14 / 100)
#endif

#endif // CONFIG_CHANNEL_MANAGER_H_
