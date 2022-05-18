/*
 *  Copyright (c) 2016-2018, The OpenThread Authors.
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
 *   This file implements the AnnounceSender.
 */

#include "announce_sender.hpp"

#include <openthread/platform/radio.h>

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/random.hpp"
#include "meshcop/meshcop.hpp"
#include "meshcop/meshcop_tlvs.hpp"
#include "radio/radio.hpp"

namespace ot {

//---------------------------------------------------------------------------------------------------------------------
// AnnounceSenderBase

AnnounceSenderBase::AnnounceSenderBase(Instance &aInstance, Timer::Handler aHandler)
    : InstanceLocator(aInstance)
    , mPeriod(0)
    , mJitter(0)
    , mCount(0)
    , mChannel(0)
    , mStartingChannel(kChannelIteratorFirst)
    , mTimer(aInstance, aHandler)
{
}

void AnnounceSenderBase::SendAnnounce(uint8_t aCount)
{
    if (IsRunning())
    {
        mCount += aCount;
        ExitNow();
    }

    VerifyOrExit((mPeriod != 0) && !mChannelMask.IsEmpty());

    SelectStartingChannel();

    mCount   = aCount;
    mChannel = mStartingChannel;

    mTimer.Start(Random::NonCrypto::GetUint32InRange(0, mJitter + 1));

exit:
    return;
}

void AnnounceSenderBase::Stop(void)
{
    mTimer.Stop();
    mCount = 0;
}

void AnnounceSenderBase::SetChannelMask(Mac::ChannelMask aChannelMask)
{
    mChannelMask = aChannelMask;
    mChannelMask.Intersect(Get<Mac::Mac>().GetSupportedChannelMask());

    VerifyOrExit(!mChannelMask.IsEmpty(), Stop());
    SelectStartingChannel();

exit:
    return;
}

void AnnounceSenderBase::SetStartingChannel(uint8_t aStartingChannel)
{
    mStartingChannel = aStartingChannel;
    SelectStartingChannel();
}

void AnnounceSenderBase::SelectStartingChannel(void)
{
    // If the starting channel is not set or it is not present
    // in the channel mask, then start from the first channel
    // in the mask.

    VerifyOrExit(!mChannelMask.IsEmpty());
    VerifyOrExit((mStartingChannel == kChannelIteratorFirst) || !mChannelMask.ContainsChannel(mStartingChannel));

    mStartingChannel = kChannelIteratorFirst;
    IgnoreError(mChannelMask.GetNextChannel(mStartingChannel));

exit:
    return;
}

void AnnounceSenderBase::HandleTimer(void)
{
    Get<Mle::MleRouter>().SendAnnounce(mChannel);

    // Go to the next channel in the mask. If we have reached the end
    // of the channel mask, we start over from the first channel in
    // the mask. Once we get back to `mStartingChannel` we have
    // finished one full cycle and can decrement `mCount`.

    while (mChannelMask.GetNextChannel(mChannel) != kErrorNone)
    {
        mChannel = kChannelIteratorFirst;
    }

    if ((mChannel == mStartingChannel) && (mCount != 0))
    {
        mCount--;
        VerifyOrExit(mCount != 0);
    }

    mTimer.Start(Random::NonCrypto::AddJitter(mPeriod, mJitter));

exit:
    return;
}

//---------------------------------------------------------------------------------------------------------------------
// AnnounceSender

#if OPENTHREAD_CONFIG_ANNOUNCE_SENDER_ENABLE

AnnounceSender::AnnounceSender(Instance &aInstance)
    : AnnounceSenderBase(aInstance, AnnounceSender::HandleTimer)
    , mTrickleTimer(aInstance, AnnounceSender::HandleTrickleTimer)
{
    SetJitter(kMaxJitter);
}

void AnnounceSender::UpdateOnReceivedAnnounce(void)
{
    mTrickleTimer.IndicateConsistent();
}

void AnnounceSender::Stop(void)
{
    AnnounceSenderBase::Stop();
    mTrickleTimer.Stop();
    otLogInfoMle("[announce-sender] Stopped");
}

void AnnounceSender::HandleTimer(Timer &aTimer)
{
    aTimer.Get<AnnounceSender>().AnnounceSenderBase::HandleTimer();
}

void AnnounceSender::HandleTrickleTimer(TrickleTimer &aTimer)
{
    aTimer.Get<AnnounceSender>().HandleTrickleTimer();
}

void AnnounceSender::HandleTrickleTimer(void)
{
    // The trickle timer handler is called when
    // we do not receive enough Announce messages
    // within the current interval and therefore
    // the device itself needs to send Announce.
    // We then request one more cycle of Announce
    // message transmissions.

    SendAnnounce(1);
    otLogInfoMle("[announce-sender] Schedule tx for one cycle");
}

void AnnounceSender::HandleNotifierEvents(Events aEvents)
{
    if (aEvents.Contains(kEventThreadRoleChanged))
    {
        HandleRoleChanged();
    }

    if (aEvents.Contains(kEventActiveDatasetChanged))
    {
        HandleActiveDatasetChanged();
    }

    if (aEvents.Contains(kEventThreadChannelChanged))
    {
        HandleThreadChannelChanged();
    }
}

void AnnounceSender::HandleRoleChanged(void)
{
    switch (Get<Mle::Mle>().GetRole())
    {
    case Mle::kRoleLeader:
    case Mle::kRoleRouter:
        break;

    case Mle::kRoleChild:
#if OPENTHREAD_FTD
        if (Get<Mle::MleRouter>().IsRouterEligible() && Get<Mle::Mle>().IsRxOnWhenIdle())
        {
            break;
        }
#endif

        OT_FALL_THROUGH;

    case Mle::kRoleDisabled:
    case Mle::kRoleDetached:
        Stop();
        ExitNow();
    }

    // Start the trickle timer with same min and max interval as the
    // desired Announce Tx cycle interval.

    mTrickleTimer.Start(TrickleTimer::kModeTrickle, kInterval, kInterval, kRedundancyConstant);
    otLogInfoMle("[announce-sender] Started");

exit:
    return;
}

void AnnounceSender::HandleActiveDatasetChanged(void)
{
    Mac::ChannelMask channelMask;

    SuccessOrExit(Get<MeshCoP::ActiveDataset>().GetChannelMask(channelMask));
    VerifyOrExit(!channelMask.IsEmpty());

    VerifyOrExit(channelMask != GetChannelMask());

    SetChannelMask(channelMask);
    SetPeriod(kTxInterval / channelMask.GetNumberOfChannels());
    otLogInfoMle("[announce-sender] ChannelMask:%s, period:%u", GetChannelMask().ToString().AsCString(), GetPeriod());

    // When channel mask is changed, we also check and update the PAN
    // channel. This handles the case where `ThreadChannelChanged` event
    // may be received and processed before `ActiveDatasetChanged`
    // event.

    HandleThreadChannelChanged();

exit:
    return;
}

void AnnounceSender::HandleThreadChannelChanged(void)
{
    SetStartingChannel(Get<Mac::Mac>().GetPanChannel());
    otLogInfoMle("[announce-sender] StartingChannel:%d", GetStartingChannel());
}

#endif // OPENTHREAD_CONFIG_ANNOUNCE_SENDER_ENABLE

} // namespace ot
