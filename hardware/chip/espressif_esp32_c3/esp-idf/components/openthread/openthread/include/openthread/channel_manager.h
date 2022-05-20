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
 *   This file includes the OpenThread API for Channel Manager module.
 */

#ifndef OPENTHREAD_CHANNEL_MANAGER_H_
#define OPENTHREAD_CHANNEL_MANAGER_H_

#include <openthread/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-channel-manager
 *
 * @brief
 *   This module includes functions for Channel Manager.
 *
 *   The functions in this module are available when Channel Manager feature
 *   (`OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE`) is enabled. Channel Manager is available only on an FTD build.
 *
 * @{
 *
 */

/**
 * This function requests a Thread network channel change.
 *
 * The network switches to the given channel after a specified delay (see otChannelManagerSetDelay()). The channel
 * change is performed by updating the Pending Operational Dataset.
 *
 * A subsequent call to this function will cancel an ongoing previously requested channel change.
 *
 * @param[in]  aInstance          A pointer to an OpenThread instance.
 * @param[in]  aChannel           The new channel for the Thread network.
 *
 */
void otChannelManagerRequestChannelChange(otInstance *aInstance, uint8_t aChannel);

/**
 * This function gets the channel from the last successful call to `otChannelManagerRequestChannelChange()`
 *
 * @returns The last requested channel or zero if there has been no channel change request yet.
 *
 */
uint8_t otChannelManagerGetRequestedChannel(otInstance *aInstance);

/**
 * This function gets the delay (in seconds) used by Channel Manager for a channel change.
 *
 * @param[in]  aInstance          A pointer to an OpenThread instance.
 *
 * @returns The delay (in seconds) for channel change.
 *
 */
uint16_t otChannelManagerGetDelay(otInstance *aInstance);

/**
 * This function sets the delay (in seconds) used for a channel change.
 *
 * The delay should preferably be longer than maximum data poll interval used by all sleepy-end-devices within the
 * Thread network.
 *
 * @param[in]  aInstance          A pointer to an OpenThread instance.
 * @param[in]  aDelay             Delay in seconds.
 *
 * @retval OT_ERROR_NONE          Delay was updated successfully.
 * @retval OT_ERROR_INVALID_ARGS  The given delay @p aDelay is too short.
 *
 */
otError otChannelManagerSetDelay(otInstance *aInstance, uint16_t aDelay);

/**
 * This function requests that `ChannelManager` checks and selects a new channel and starts a channel change.
 *
 * Unlike the `otChannelManagerRequestChannelChange()` where the channel must be given as a parameter, this function
 * asks the `ChannelManager` to select a channel by itself (based of collected channel quality info).
 *
 * Once called, the Channel Manager will perform the following 3 steps:
 *
 * 1) `ChannelManager` decides if the channel change would be helpful. This check can be skipped if
 *    `aSkipQualityCheck` is set to true (forcing a channel selection to happen and skipping the quality check).
 *    This step uses the collected link quality metrics on the device (such as CCA failure rate, frame and message
 *    error rates per neighbor, etc.) to determine if the current channel quality is at the level that justifies
 *    a channel change.
 *
 * 2) If the first step passes, then `ChannelManager` selects a potentially better channel. It uses the collected
 *    channel quality data by `ChannelMonitor` module. The supported and favored channels are used at this step.
 *    (see otChannelManagerSetSupportedChannels() and otChannelManagerSetFavoredChannels()).
 *
 * 3) If the newly selected channel is different from the current channel, `ChannelManager` requests/starts the
 *    channel change process (internally invoking a `RequestChannelChange()`).
 *
 * @param[in] aInstance                A pointer to an OpenThread instance.
 * @param[in] aSkipQualityCheck        Indicates whether the quality check (step 1) should be skipped.
 *
 * @retval OT_ERROR_NONE               Channel selection finished successfully.
 * @retval OT_ERROR_NOT_FOUND          Supported channel mask is empty, therefore could not select a channel.
 *
 */
otError otChannelManagerRequestChannelSelect(otInstance *aInstance, bool aSkipQualityCheck);

/**
 * This function enables/disables the auto-channel-selection functionality.
 *
 * When enabled, `ChannelManager` will periodically invoke a `RequestChannelSelect(false)`. The period interval
 * can be set by `SetAutoChannelSelectionInterval()`.
 *
 * @param[in]  aInstance    A pointer to an OpenThread instance.
 * @param[in]  aEnabled     Indicates whether to enable or disable this functionality.
 *
 */
void otChannelManagerSetAutoChannelSelectionEnabled(otInstance *aInstance, bool aEnabled);

/**
 * This function indicates whether the auto-channel-selection functionality is enabled or not.
 *
 * @param[in]  aInstance    A pointer to an OpenThread instance.
 *
 * @returns TRUE if enabled, FALSE if disabled.
 *
 */
bool otChannelManagerGetAutoChannelSelectionEnabled(otInstance *aInstance);

/**
 * This function sets the period interval (in seconds) used by auto-channel-selection functionality.
 *
 * @param[in] aInstance   A pointer to an OpenThread instance.
 * @param[in] aInterval   The interval in seconds.
 *
 * @retval OT_ERROR_NONE           The interval was set successfully.
 * @retval OT_ERROR_INVALID_ARGS   The @p aInterval is not valid (zero).
 *
 */
otError otChannelManagerSetAutoChannelSelectionInterval(otInstance *aInstance, uint32_t aInterval);

/**
 * This function gets the period interval (in seconds) used by auto-channel-selection functionality.
 *
 * @param[in]  aInstance    A pointer to an OpenThread instance.
 *
 * @returns The interval in seconds.
 *
 */
uint32_t otChannelManagerGetAutoChannelSelectionInterval(otInstance *aInstance);

/**
 * This function gets the supported channel mask.
 *
 * @param[in]  aInstance       A pointer to an OpenThread instance.
 *
 * @returns  The supported channels as a bit-mask.
 *
 */
uint32_t otChannelManagerGetSupportedChannels(otInstance *aInstance);

/**
 * This function sets the supported channel mask.
 *
 * @param[in]  aInstance     A pointer to an OpenThread instance.
 * @param[in]  aChannelMask  A channel mask.
 *
 */
void otChannelManagerSetSupportedChannels(otInstance *aInstance, uint32_t aChannelMask);

/**
 * This function gets the favored channel mask.
 *
 * @param[in]  aInstance       A pointer to an OpenThread instance.
 *
 * @returns  The favored channels as a bit-mask.
 *
 */
uint32_t otChannelManagerGetFavoredChannels(otInstance *aInstance);

/**
 * This function sets the favored channel mask.
 *
 * @param[in]  aInstance     A pointer to an OpenThread instance.
 * @param[in]  aChannelMask  A channel mask.
 *
 */
void otChannelManagerSetFavoredChannels(otInstance *aInstance, uint32_t aChannelMask);

/**
 * This function gets the CCA failure rate threshold
 *
 * @param[in]  aInstance     A pointer to an OpenThread instance.
 *
 * @returns  The CCA failure rate threshold. Value 0 maps to 0% and 0xffff maps to 100%.
 *
 */
uint16_t otChannelManagerGetCcaFailureRateThreshold(otInstance *aInstance);

/**
 * This function sets the CCA failure rate threshold
 *
 * @param[in]  aInstance     A pointer to an OpenThread instance.
 * @param[in]  aThreshold    A CCA failure rate threshold. Value 0 maps to 0% and 0xffff maps to 100%.
 *
 */
void otChannelManagerSetCcaFailureRateThreshold(otInstance *aInstance, uint16_t aThreshold);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_CHANNEL_MANAGER_H_
