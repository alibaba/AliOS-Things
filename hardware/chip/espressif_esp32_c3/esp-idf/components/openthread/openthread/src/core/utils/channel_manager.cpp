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
 *   This file implements Channel Manager.
 *
 */

#include "channel_manager.hpp"

#if OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE && OPENTHREAD_FTD

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/random.hpp"
#include "meshcop/dataset_updater.hpp"
#include "radio/radio.hpp"

namespace ot {
namespace Utils {

ChannelManager::ChannelManager(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mSupportedChannelMask(0)
    , mFavoredChannelMask(0)
    , mDelay(kMinimumDelay)
    , mChannel(0)
    , mState(kStateIdle)
    , mTimer(aInstance, ChannelManager::HandleTimer)
    , mAutoSelectInterval(kDefaultAutoSelectInterval)
    , mAutoSelectEnabled(false)
    , mCcaFailureRateThreshold(kCcaFailureRateThreshold)
{
}

void ChannelManager::RequestChannelChange(uint8_t aChannel)
{
    otLogInfoUtil("ChannelManager: Request to change to channel %d with delay %d sec", aChannel, mDelay);

    if (aChannel == Get<Mac::Mac>().GetPanChannel())
    {
        otLogInfoUtil("ChannelManager: Already operating on the requested channel %d", aChannel);
        ExitNow();
    }

    if (mState == kStateChangeInProgress)
    {
        VerifyOrExit(mChannel != aChannel);
    }

    mState   = kStateChangeRequested;
    mChannel = aChannel;

    mTimer.Start(1 + Random::NonCrypto::GetUint32InRange(0, kRequestStartJitterInterval));

    Get<Notifier>().Signal(kEventChannelManagerNewChannelChanged);

exit:
    return;
}

Error ChannelManager::SetDelay(uint16_t aDelay)
{
    Error error = kErrorNone;

    VerifyOrExit(aDelay >= kMinimumDelay, error = kErrorInvalidArgs);
    mDelay = aDelay;

exit:
    return error;
}

void ChannelManager::StartDatasetUpdate(void)
{
    MeshCoP::Dataset::Info dataset;

    dataset.Clear();
    dataset.SetChannel(mChannel);
    dataset.SetDelay(Time::SecToMsec(mDelay));

    switch (Get<MeshCoP::DatasetUpdater>().RequestUpdate(dataset, HandleDatasetUpdateDone, this))
    {
    case kErrorNone:
        mState = kStateChangeInProgress;
        // Wait for the `HandleDatasetUpdateDone()` callback.
        break;

    case kErrorBusy:
    case kErrorNoBufs:
        mTimer.Start(kPendingDatasetTxRetryInterval);
        break;

    case kErrorInvalidState:
        otLogInfoUtil("ChannelManager: Request to change to channel %d failed. Device is disabled", mChannel);

        OT_FALL_THROUGH;

    default:
        mState = kStateIdle;
        StartAutoSelectTimer();
        break;
    }
}

void ChannelManager::HandleDatasetUpdateDone(Error aError, void *aContext)
{
    static_cast<ChannelManager *>(aContext)->HandleDatasetUpdateDone(aError);
}

void ChannelManager::HandleDatasetUpdateDone(Error aError)
{
    if (aError == kErrorNone)
    {
        otLogInfoUtil("ChannelManager: Channel changed to %d", mChannel);
    }
    else
    {
        otLogInfoUtil("ChannelManager: Canceling channel change to %d%s", mChannel,
                      (aError == kErrorAlready) ? " since current ActiveDataset is more recent" : "");
    }

    mState = kStateIdle;
    StartAutoSelectTimer();
}

void ChannelManager::HandleTimer(Timer &aTimer)
{
    aTimer.Get<ChannelManager>().HandleTimer();
}

void ChannelManager::HandleTimer(void)
{
    switch (mState)
    {
    case kStateIdle:
        otLogInfoUtil("ChannelManager: Auto-triggered channel select");
        IgnoreError(RequestChannelSelect(false));
        StartAutoSelectTimer();
        break;

    case kStateChangeRequested:
        StartDatasetUpdate();
        break;

    case kStateChangeInProgress:
        break;
    }
}

#if OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE

Error ChannelManager::FindBetterChannel(uint8_t &aNewChannel, uint16_t &aOccupancy)
{
    Error            error = kErrorNone;
    Mac::ChannelMask favoredAndSupported;
    Mac::ChannelMask favoredBest;
    Mac::ChannelMask supportedBest;
    uint16_t         favoredOccupancy;
    uint16_t         supportedOccupancy;

    if (Get<ChannelMonitor>().GetSampleCount() <= kMinChannelMonitorSampleCount)
    {
        otLogInfoUtil("ChannelManager: Too few samples (%d <= %d) to select channel",
                      Get<ChannelMonitor>().GetSampleCount(), kMinChannelMonitorSampleCount);
        ExitNow(error = kErrorInvalidState);
    }

    favoredAndSupported = mFavoredChannelMask;
    favoredAndSupported.Intersect(mSupportedChannelMask);

    favoredBest   = Get<ChannelMonitor>().FindBestChannels(favoredAndSupported, favoredOccupancy);
    supportedBest = Get<ChannelMonitor>().FindBestChannels(mSupportedChannelMask, supportedOccupancy);

    otLogInfoUtil("ChannelManager: Best favored %s, occupancy 0x%04x", favoredBest.ToString().AsCString(),
                  favoredOccupancy);
    otLogInfoUtil("ChannelManager: Best overall %s, occupancy 0x%04x", supportedBest.ToString().AsCString(),
                  supportedOccupancy);

    // Prefer favored channels unless there is no favored channel,
    // or the occupancy rate of the best favored channel is worse
    // than the best overall by at least `kThresholdToSkipFavored`.

    if (favoredBest.IsEmpty() || ((favoredOccupancy >= kThresholdToSkipFavored) &&
                                  (supportedOccupancy < favoredOccupancy - kThresholdToSkipFavored)))
    {
        if (!favoredBest.IsEmpty())
        {
            otLogInfoUtil("ChannelManager: Preferring an unfavored channel due to high occupancy rate diff");
        }

        favoredBest      = supportedBest;
        favoredOccupancy = supportedOccupancy;
    }

    VerifyOrExit(!favoredBest.IsEmpty(), error = kErrorNotFound);

    aNewChannel = favoredBest.ChooseRandomChannel();
    aOccupancy  = favoredOccupancy;

exit:
    return error;
}

bool ChannelManager::ShouldAttemptChannelChange(void)
{
    uint16_t ccaFailureRate = Get<Mac::Mac>().GetCcaFailureRate();
    bool     shouldAttempt  = (ccaFailureRate >= mCcaFailureRateThreshold);

    otLogInfoUtil("ChannelManager: CCA-err-rate: 0x%04x %s 0x%04x, selecting channel: %s", ccaFailureRate,
                  shouldAttempt ? ">=" : "<", mCcaFailureRateThreshold, shouldAttempt ? "yes" : "no");

    return shouldAttempt;
}

Error ChannelManager::RequestChannelSelect(bool aSkipQualityCheck)
{
    Error    error = kErrorNone;
    uint8_t  curChannel, newChannel;
    uint16_t curOccupancy, newOccupancy;

    otLogInfoUtil("ChannelManager: Request to select channel (skip quality check: %s)",
                  aSkipQualityCheck ? "yes" : "no");

    VerifyOrExit(!Get<Mle::Mle>().IsDisabled(), error = kErrorInvalidState);

    VerifyOrExit(aSkipQualityCheck || ShouldAttemptChannelChange());

    SuccessOrExit(error = FindBetterChannel(newChannel, newOccupancy));

    curChannel   = Get<Mac::Mac>().GetPanChannel();
    curOccupancy = Get<ChannelMonitor>().GetChannelOccupancy(curChannel);

    if (newChannel == curChannel)
    {
        otLogInfoUtil("ChannelManager: Already on best possible channel %d", curChannel);
        ExitNow();
    }

    otLogInfoUtil("ChannelManager: Cur channel %d, occupancy 0x%04x - Best channel %d, occupancy 0x%04x", curChannel,
                  curOccupancy, newChannel, newOccupancy);

    // Switch only if new channel's occupancy rate is better than current
    // channel's occupancy rate by threshold `kThresholdToChangeChannel`.

    if ((newOccupancy >= curOccupancy) ||
        (static_cast<uint16_t>(curOccupancy - newOccupancy) < kThresholdToChangeChannel))
    {
        otLogInfoUtil("ChannelManager: Occupancy rate diff too small to change channel");
        ExitNow();
    }

    RequestChannelChange(newChannel);

exit:

    if (error != kErrorNone)
    {
        otLogInfoUtil("ChannelManager: Request to select better channel failed, error: %s", ErrorToString(error));
    }

    return error;
}
#endif // OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE

void ChannelManager::StartAutoSelectTimer(void)
{
    VerifyOrExit(mState == kStateIdle);

    if (mAutoSelectEnabled)
    {
        mTimer.Start(Time::SecToMsec(mAutoSelectInterval));
    }
    else
    {
        mTimer.Stop();
    }

exit:
    return;
}

void ChannelManager::SetAutoChannelSelectionEnabled(bool aEnabled)
{
    if (aEnabled != mAutoSelectEnabled)
    {
        mAutoSelectEnabled = aEnabled;
        IgnoreError(RequestChannelSelect(false));
        StartAutoSelectTimer();
    }
}

Error ChannelManager::SetAutoChannelSelectionInterval(uint32_t aInterval)
{
    Error    error        = kErrorNone;
    uint32_t prevInterval = mAutoSelectInterval;

    VerifyOrExit((aInterval != 0) && (aInterval <= Time::MsecToSec(Timer::kMaxDelay)), error = kErrorInvalidArgs);

    mAutoSelectInterval = aInterval;

    if (mAutoSelectEnabled && (mState == kStateIdle) && mTimer.IsRunning() && (prevInterval != aInterval))
    {
        mTimer.StartAt(mTimer.GetFireTime() - Time::SecToMsec(prevInterval), Time::SecToMsec(aInterval));
    }

exit:
    return error;
}

void ChannelManager::SetSupportedChannels(uint32_t aChannelMask)
{
    mSupportedChannelMask.SetMask(aChannelMask & Get<Mac::Mac>().GetSupportedChannelMask().GetMask());

    otLogInfoUtil("ChannelManager: Supported channels: %s", mSupportedChannelMask.ToString().AsCString());
}

void ChannelManager::SetFavoredChannels(uint32_t aChannelMask)
{
    mFavoredChannelMask.SetMask(aChannelMask & Get<Mac::Mac>().GetSupportedChannelMask().GetMask());

    otLogInfoUtil("ChannelManager: Favored channels: %s", mFavoredChannelMask.ToString().AsCString());
}

void ChannelManager::SetCcaFailureRateThreshold(uint16_t aThreshold)
{
    mCcaFailureRateThreshold = aThreshold;

    otLogInfoUtil("ChannelManager: CCA threshold: 0x%04x", mCcaFailureRateThreshold);
}

} // namespace Utils
} // namespace ot

#endif // #if OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE
