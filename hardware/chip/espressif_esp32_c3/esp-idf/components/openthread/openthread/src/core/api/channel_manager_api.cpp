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
 *   This file implements the OpenThread channel manager APIs.
 */

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE && OPENTHREAD_FTD

#include <openthread/channel_manager.h>

#include "common/as_core_type.hpp"
#include "common/locator_getters.hpp"
#include "utils/channel_manager.hpp"

using namespace ot;

void otChannelManagerRequestChannelChange(otInstance *aInstance, uint8_t aChannel)
{
    AsCoreType(aInstance).Get<Utils::ChannelManager>().RequestChannelChange(aChannel);
}

uint8_t otChannelManagerGetRequestedChannel(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Utils::ChannelManager>().GetRequestedChannel();
}

uint16_t otChannelManagerGetDelay(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Utils::ChannelManager>().GetDelay();
}

otError otChannelManagerSetDelay(otInstance *aInstance, uint16_t aDelay)
{
    return AsCoreType(aInstance).Get<Utils::ChannelManager>().SetDelay(aDelay);
}

#if OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
otError otChannelManagerRequestChannelSelect(otInstance *aInstance, bool aSkipQualityCheck)
{
    return AsCoreType(aInstance).Get<Utils::ChannelManager>().RequestChannelSelect(aSkipQualityCheck);
}
#endif

void otChannelManagerSetAutoChannelSelectionEnabled(otInstance *aInstance, bool aEnabled)
{
    AsCoreType(aInstance).Get<Utils::ChannelManager>().SetAutoChannelSelectionEnabled(aEnabled);
}

bool otChannelManagerGetAutoChannelSelectionEnabled(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Utils::ChannelManager>().GetAutoChannelSelectionEnabled();
}

otError otChannelManagerSetAutoChannelSelectionInterval(otInstance *aInstance, uint32_t aInterval)
{
    return AsCoreType(aInstance).Get<Utils::ChannelManager>().SetAutoChannelSelectionInterval(aInterval);
}

uint32_t otChannelManagerGetAutoChannelSelectionInterval(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Utils::ChannelManager>().GetAutoChannelSelectionInterval();
}

uint32_t otChannelManagerGetSupportedChannels(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Utils::ChannelManager>().GetSupportedChannels();
}

void otChannelManagerSetSupportedChannels(otInstance *aInstance, uint32_t aChannelMask)
{
    return AsCoreType(aInstance).Get<Utils::ChannelManager>().SetSupportedChannels(aChannelMask);
}

uint32_t otChannelManagerGetFavoredChannels(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Utils::ChannelManager>().GetFavoredChannels();
}

void otChannelManagerSetFavoredChannels(otInstance *aInstance, uint32_t aChannelMask)
{
    return AsCoreType(aInstance).Get<Utils::ChannelManager>().SetFavoredChannels(aChannelMask);
}

uint16_t otChannelManagerGetCcaFailureRateThreshold(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Utils::ChannelManager>().GetCcaFailureRateThreshold();
}

void otChannelManagerSetCcaFailureRateThreshold(otInstance *aInstance, uint16_t aThreshold)
{
    return AsCoreType(aInstance).Get<Utils::ChannelManager>().SetCcaFailureRateThreshold(aThreshold);
}

#endif // OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE && OPENTHREAD_FTD
