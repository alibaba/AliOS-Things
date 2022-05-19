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
 *   This file implements the channel monitoring module.
 */

#include "channel_monitor.hpp"

#if OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE

#include "common/code_utils.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/random.hpp"

namespace ot {
namespace Utils {

const uint32_t ChannelMonitor::mScanChannelMasks[kNumChannelMasks] = {
#if OPENTHREAD_CONFIG_RADIO_915MHZ_OQPSK_SUPPORT
    OT_CHANNEL_1_MASK | OT_CHANNEL_5_MASK | OT_CHANNEL_9_MASK,
    OT_CHANNEL_2_MASK | OT_CHANNEL_6_MASK | OT_CHANNEL_10_MASK,
    OT_CHANNEL_3_MASK | OT_CHANNEL_7_MASK,
    OT_CHANNEL_4_MASK | OT_CHANNEL_8_MASK,
#endif
#if OPENTHREAD_CONFIG_RADIO_2P4GHZ_OQPSK_SUPPORT
    OT_CHANNEL_11_MASK | OT_CHANNEL_15_MASK | OT_CHANNEL_19_MASK | OT_CHANNEL_23_MASK,
    OT_CHANNEL_12_MASK | OT_CHANNEL_16_MASK | OT_CHANNEL_20_MASK | OT_CHANNEL_24_MASK,
    OT_CHANNEL_13_MASK | OT_CHANNEL_17_MASK | OT_CHANNEL_21_MASK | OT_CHANNEL_25_MASK,
    OT_CHANNEL_14_MASK | OT_CHANNEL_18_MASK | OT_CHANNEL_22_MASK | OT_CHANNEL_26_MASK,
#endif
};

ChannelMonitor::ChannelMonitor(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mChannelMaskIndex(0)
    , mSampleCount(0)
    , mTimer(aInstance, ChannelMonitor::HandleTimer)
{
    memset(mChannelOccupancy, 0, sizeof(mChannelOccupancy));
}

Error ChannelMonitor::Start(void)
{
    Error error = kErrorNone;

    VerifyOrExit(!IsRunning(), error = kErrorAlready);
    Clear();
    mTimer.Start(kTimerInterval);
    otLogDebgUtil("ChannelMonitor: Starting");

exit:
    return error;
}

Error ChannelMonitor::Stop(void)
{
    Error error = kErrorNone;

    VerifyOrExit(IsRunning(), error = kErrorAlready);
    mTimer.Stop();
    otLogDebgUtil("ChannelMonitor: Stopping");

exit:
    return error;
}

void ChannelMonitor::Clear(void)
{
    mChannelMaskIndex = 0;
    mSampleCount      = 0;
    memset(mChannelOccupancy, 0, sizeof(mChannelOccupancy));

    otLogDebgUtil("ChannelMonitor: Clearing data");
}

uint16_t ChannelMonitor::GetChannelOccupancy(uint8_t aChannel) const
{
    uint16_t occupancy = 0;

    VerifyOrExit((Radio::kChannelMin <= aChannel) && (aChannel <= Radio::kChannelMax));
    occupancy = mChannelOccupancy[aChannel - Radio::kChannelMin];

exit:
    return occupancy;
}

void ChannelMonitor::HandleTimer(Timer &aTimer)
{
    aTimer.Get<ChannelMonitor>().HandleTimer();
}

void ChannelMonitor::HandleTimer(void)
{
    IgnoreError(Get<Mac::Mac>().EnergyScan(mScanChannelMasks[mChannelMaskIndex], 0,
                                           &ChannelMonitor::HandleEnergyScanResult, this));

    mTimer.StartAt(mTimer.GetFireTime(), Random::NonCrypto::AddJitter(kTimerInterval, kMaxJitterInterval));
}

void ChannelMonitor::HandleEnergyScanResult(Mac::EnergyScanResult *aResult, void *aContext)
{
    static_cast<ChannelMonitor *>(aContext)->HandleEnergyScanResult(aResult);
}

void ChannelMonitor::HandleEnergyScanResult(Mac::EnergyScanResult *aResult)
{
    if (aResult == nullptr)
    {
        if (mChannelMaskIndex == kNumChannelMasks - 1)
        {
            mChannelMaskIndex = 0;
            mSampleCount++;
            LogResults();
        }
        else
        {
            mChannelMaskIndex++;
        }
    }
    else
    {
        uint8_t  channelIndex = (aResult->mChannel - Radio::kChannelMin);
        uint32_t newAverage   = mChannelOccupancy[channelIndex];
        uint32_t newValue     = 0;
        uint32_t weight;

        OT_ASSERT(channelIndex < kNumChannels);

        otLogDebgUtil("ChannelMonitor: channel: %d, rssi:%d", aResult->mChannel, aResult->mMaxRssi);

        if (aResult->mMaxRssi != OT_RADIO_RSSI_INVALID)
        {
            newValue = (aResult->mMaxRssi >= kRssiThreshold) ? kMaxOccupancy : 0;
        }

        // `mChannelOccupancy` stores the average rate/percentage of RSS
        // samples that are higher than a given RSS threshold ("bad" RSS
        // samples). For the first `kSampleWindow` samples, the average is
        // maintained as the actual percentage (i.e., ratio of number of
        // "bad" samples by total number of samples). After `kSampleWindow`
        // samples, the averager uses an exponentially weighted moving
        // average logic with weight coefficient `1/kSampleWindow` for new
        // values. Practically, this means the average is representative
        // of up to `3 * kSampleWindow` samples with highest weight given
        // to the latest `kSampleWindow` samples.

        if (mSampleCount >= kSampleWindow)
        {
            weight = kSampleWindow - 1;
        }
        else
        {
            weight = mSampleCount;
        }

        newAverage = (newAverage * weight + newValue) / (weight + 1);

        mChannelOccupancy[channelIndex] = static_cast<uint16_t>(newAverage);
    }
}

void ChannelMonitor::LogResults(void)
{
#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO) && (OPENTHREAD_CONFIG_LOG_UTIL == 1)
    const size_t        kStringSize = 128;
    String<kStringSize> logString;

    for (uint16_t channel : mChannelOccupancy)
    {
        logString.Append("%02x ", channel >> 8);
    }

    otLogInfoUtil("ChannelMonitor: %u [%s]", mSampleCount, logString.AsCString());
#endif
}

Mac::ChannelMask ChannelMonitor::FindBestChannels(const Mac::ChannelMask &aMask, uint16_t &aOccupancy) const
{
    uint8_t          channel;
    Mac::ChannelMask bestMask;
    uint16_t         minOccupancy = 0xffff;

    bestMask.Clear();

    channel = Mac::ChannelMask::kChannelIteratorFirst;

    while (aMask.GetNextChannel(channel) == kErrorNone)
    {
        uint16_t occupancy = GetChannelOccupancy(channel);

        if (bestMask.IsEmpty() || (occupancy <= minOccupancy))
        {
            if (occupancy < minOccupancy)
            {
                bestMask.Clear();
            }

            bestMask.AddChannel(channel);
            minOccupancy = occupancy;
        }
    }

    aOccupancy = minOccupancy;

    return bestMask;
}

} // namespace Utils
} // namespace ot

#endif // OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
