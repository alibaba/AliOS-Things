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
 *   This file implements the OpenThread channel monitor APIs.
 */

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE

#include <openthread/channel_monitor.h>

#include "common/as_core_type.hpp"
#include "common/locator_getters.hpp"

using namespace ot;

otError otChannelMonitorSetEnabled(otInstance *aInstance, bool aEnabled)
{
    Utils::ChannelMonitor &monitor = AsCoreType(aInstance).Get<Utils::ChannelMonitor>();

    return aEnabled ? monitor.Start() : monitor.Stop();
}

bool otChannelMonitorIsEnabled(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Utils::ChannelMonitor>().IsRunning();
}

uint32_t otChannelMonitorGetSampleInterval(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return Utils::ChannelMonitor::kSampleInterval;
}

int8_t otChannelMonitorGetRssiThreshold(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return Utils::ChannelMonitor::kRssiThreshold;
}

uint32_t otChannelMonitorGetSampleWindow(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return Utils::ChannelMonitor::kSampleWindow;
}

uint32_t otChannelMonitorGetSampleCount(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Utils::ChannelMonitor>().GetSampleCount();
}

uint16_t otChannelMonitorGetChannelOccupancy(otInstance *aInstance, uint8_t aChannel)
{
    return AsCoreType(aInstance).Get<Utils::ChannelMonitor>().GetChannelOccupancy(aChannel);
}

#endif // OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
