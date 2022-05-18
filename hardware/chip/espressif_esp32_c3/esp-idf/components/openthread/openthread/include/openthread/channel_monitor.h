/*
 *  Copyright (c) 2018, The OpenThread Authors.
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
 *   This file includes the OpenThread API for channel monitoring feature
 */

#ifndef OPENTHREAD_CHANNEL_MONITOR_H_
#define OPENTHREAD_CHANNEL_MONITOR_H_

#include <openthread/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-channel-monitor
 *
 * @brief
 *   This module includes functions for channel monitoring feature.
 *
 *   The functions in this module are available when channel monitor feature
 *   (`OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE`) is enabled.
 *
 *   Channel monitoring will periodically monitor all channels to help determine the cleaner channels (channels
 *   with less interference).
 *
 *   When channel monitoring is active, a zero-duration Energy Scan is performed, collecting a single RSSI sample on
 *   every channel per sample interval. The RSSI samples are compared with a pre-specified RSSI threshold. As an
 *   indicator of channel quality, the channel monitoring module maintains and provides the average rate/percentage of
 *   RSSI samples that are above the threshold within (approximately) a specified sample window (referred to as channel
 *   occupancy).
 *
 * @{
 *
 */

/**
 * This function enables/disables the Channel Monitoring operation.
 *
 * Once operation starts, any previously collected data is cleared. However, after operation is disabled, the previous
 * collected data is still valid and can be read.
 *
 * @note OpenThread core internally enables/disables the Channel Monitoring operation when the IPv6 interface is
 * brought up/down (i.e., call to `otIp6SetEnabled()`).
 *
 * @param[in]  aInstance       A pointer to an OpenThread instance.
 * @param[in]  aEnabled        TRUE to enable/start Channel Monitoring operation, FALSE to disable/stop it.
 *
 * @retval OT_ERROR_NONE      Channel Monitoring state changed successfully
 * @retval OT_ERROR_ALREADY   Channel Monitoring is already in the same state.
 *
 */
otError otChannelMonitorSetEnabled(otInstance *aInstance, bool aEnabled);

/**
 * This function indicates whether the Channel Monitoring operation is enabled and running.
 *
 * @param[in]  aInstance       A pointer to an OpenThread instance.
 *
 * @returns TRUE if the Channel Monitoring operation is enabled, FALSE otherwise.
 *
 */
bool otChannelMonitorIsEnabled(otInstance *aInstance);

/**
 * Get channel monitoring sample interval in milliseconds.
 *
 * @param[in]  aInstance       A pointer to an OpenThread instance.
 *
 * @returns  The channel monitor sample interval in milliseconds.
 *
 */
uint32_t otChannelMonitorGetSampleInterval(otInstance *aInstance);

/**
 * Get channel monitoring RSSI threshold in dBm.
 *
 * @param[in]  aInstance       A pointer to an OpenThread instance.
 *
 * @returns  The RSSI threshold in dBm.
 *
 */
int8_t otChannelMonitorGetRssiThreshold(otInstance *aInstance);

/**
 * Get channel monitoring averaging sample window length (number of samples).
 *
 * @param[in]  aInstance       A pointer to an OpenThread instance.
 *
 * @returns  The averaging sample window.
 *
 */
uint32_t otChannelMonitorGetSampleWindow(otInstance *aInstance);

/**
 * Get channel monitoring total number of RSSI samples (per channel).
 *
 * The count indicates total number samples per channel by channel monitoring module since its start (since Thread
 * network interface was enabled).
 *
 * @param[in]  aInstance       A pointer to an OpenThread instance.
 *
 * @returns  Total number of RSSI samples (per channel) taken so far.
 *
 */
uint32_t otChannelMonitorGetSampleCount(otInstance *aInstance);

/**
 * Gets the current channel occupancy for a given channel.
 *
 * The channel occupancy value represents the average rate/percentage of RSSI samples that were above RSSI threshold
 * ("bad" RSSI samples).
 *
 * For the first "sample window" samples, the average is maintained as the actual percentage (i.e., ratio of number
 * of "bad" samples by total number of samples). After "window" samples, the averager uses an exponentially
 * weighted moving average. Practically, this means the average is representative of up to `3 * window` last samples
 * with highest weight given to the latest `kSampleWindow` samples.
 *
 * Max value of `0xffff` indicates all RSSI samples were above RSSI threshold (i.e. 100% of samples were "bad").
 *
 * @param[in]  aInstance       A pointer to an OpenThread instance.
 * @param[in]  aChannel        The channel for which to get the link occupancy.
 *
 * @returns The current channel occupancy for the given channel.
 *
 */
uint16_t otChannelMonitorGetChannelOccupancy(otInstance *aInstance, uint8_t aChannel);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_CHANNEL_MONITOR_H_
