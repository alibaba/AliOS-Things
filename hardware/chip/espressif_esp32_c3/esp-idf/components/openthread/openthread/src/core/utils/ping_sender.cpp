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
 *   This file implements the ping sender module.
 */

#include "ping_sender.hpp"

#if OPENTHREAD_CONFIG_PING_SENDER_ENABLE

#include "common/encoding.hpp"
#include "common/locator_getters.hpp"
#include "common/random.hpp"

namespace ot {
namespace Utils {

using Encoding::BigEndian::HostSwap32;

void PingSender::Config::SetUnspecifiedToDefault(void)
{
    if (mSize == 0)
    {
        mSize = kDefaultSize;
    }

    if (mCount == 0)
    {
        mCount = kDefaultCount;
    }

    if (mInterval == 0)
    {
        mInterval = kDefaultInterval;
    }

    if (mTimeout == 0)
    {
        mTimeout = kDefaultTimeout;
    }
}

void PingSender::Config::InvokeReplyCallback(const Reply &aReply) const
{
    VerifyOrExit(mReplyCallback != nullptr);
    mReplyCallback(&aReply, mCallbackContext);

exit:
    return;
}

void PingSender::Config::InvokeStatisticsCallback(const Statistics &aStatistics) const
{
    VerifyOrExit(mStatisticsCallback != nullptr);
    mStatisticsCallback(&aStatistics, mCallbackContext);

exit:
    return;
}

PingSender::PingSender(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mIdentifier(0)
    , mTargetEchoSequence(0)
    , mTimer(aInstance, PingSender::HandleTimer)
    , mIcmpHandler(PingSender::HandleIcmpReceive, this)
{
    IgnoreError(Get<Ip6::Icmp>().RegisterHandler(mIcmpHandler));
}

Error PingSender::Ping(const Config &aConfig)
{
    Error error = kErrorNone;

    VerifyOrExit(!mTimer.IsRunning(), error = kErrorBusy);

    mConfig = aConfig;
    mConfig.SetUnspecifiedToDefault();

    VerifyOrExit(mConfig.mInterval <= Timer::kMaxDelay, error = kErrorInvalidArgs);

    mStatistics.Clear();
    mStatistics.mIsMulticast = static_cast<Ip6::Address *>(&mConfig.mDestination)->IsMulticast();

    mIdentifier++;
    SendPing();

exit:
    return error;
}

void PingSender::Stop(void)
{
    mTimer.Stop();
    mIdentifier++;
}

void PingSender::SendPing(void)
{
    TimeMilli        now     = TimerMilli::GetNow();
    Message *        message = nullptr;
    Ip6::MessageInfo messageInfo;

    messageInfo.SetSockAddr(mConfig.GetSource());
    messageInfo.SetPeerAddr(mConfig.GetDestination());
    messageInfo.mHopLimit          = mConfig.mHopLimit;
    messageInfo.mAllowZeroHopLimit = mConfig.mAllowZeroHopLimit;

    message = Get<Ip6::Icmp>().NewMessage(0);
    VerifyOrExit(message != nullptr);

    SuccessOrExit(message->Append(HostSwap32(now.GetValue())));

    if (mConfig.mSize > message->GetLength())
    {
        SuccessOrExit(message->SetLength(mConfig.mSize));
    }

    mTargetEchoSequence = Get<Ip6::Icmp>().GetEchoSequence();
    SuccessOrExit(Get<Ip6::Icmp>().SendEchoRequest(*message, messageInfo, mIdentifier));
    mStatistics.mSentCount++;

#if OPENTHREAD_CONFIG_OTNS_ENABLE
    Get<Utils::Otns>().EmitPingRequest(mConfig.GetDestination(), mConfig.mSize, now.GetValue(), mConfig.mHopLimit);
#endif

    message = nullptr;

exit:
    FreeMessage(message);
    mConfig.mCount--;

    if (mConfig.mCount > 0)
    {
        mTimer.Start(mConfig.mInterval);
    }
    else
    {
        mTimer.Start(mConfig.mTimeout);
    }
}

void PingSender::HandleTimer(Timer &aTimer)
{
    aTimer.Get<PingSender>().HandleTimer();
}

void PingSender::HandleTimer(void)
{
    if (mConfig.mCount > 0)
    {
        SendPing();
    }
    else // The last reply times out, triggering the callback to print statistics in CLI.
    {
        mConfig.InvokeStatisticsCallback(mStatistics);
    }
}

void PingSender::HandleIcmpReceive(void *               aContext,
                                   otMessage *          aMessage,
                                   const otMessageInfo *aMessageInfo,
                                   const otIcmp6Header *aIcmpHeader)
{
    reinterpret_cast<PingSender *>(aContext)->HandleIcmpReceive(*static_cast<Message *>(aMessage),
                                                                *static_cast<const Ip6::MessageInfo *>(aMessageInfo),
                                                                *static_cast<const Ip6::Icmp::Header *>(aIcmpHeader));
}

void PingSender::HandleIcmpReceive(const Message &          aMessage,
                                   const Ip6::MessageInfo & aMessageInfo,
                                   const Ip6::Icmp::Header &aIcmpHeader)
{
    Reply    reply;
    uint32_t timestamp;

    VerifyOrExit(mTimer.IsRunning());
    VerifyOrExit(aIcmpHeader.GetType() == Ip6::Icmp::Header::kTypeEchoReply);
    VerifyOrExit(aIcmpHeader.GetId() == mIdentifier);

    SuccessOrExit(aMessage.Read(aMessage.GetOffset(), timestamp));
    timestamp = HostSwap32(timestamp);

    reply.mSenderAddress = aMessageInfo.GetPeerAddr();
    reply.mRoundTripTime =
        static_cast<uint16_t>(OT_MIN(TimerMilli::GetNow() - TimeMilli(timestamp), NumericLimits<uint16_t>::kMax));
    reply.mSize           = aMessage.GetLength() - aMessage.GetOffset();
    reply.mSequenceNumber = aIcmpHeader.GetSequence();
    reply.mHopLimit       = aMessageInfo.GetHopLimit();

    mStatistics.mReceivedCount++;
    mStatistics.mTotalRoundTripTime += reply.mRoundTripTime;
    mStatistics.mMaxRoundTripTime = OT_MAX(mStatistics.mMaxRoundTripTime, reply.mRoundTripTime);
    mStatistics.mMinRoundTripTime = OT_MIN(mStatistics.mMinRoundTripTime, reply.mRoundTripTime);

#if OPENTHREAD_CONFIG_OTNS_ENABLE
    Get<Utils::Otns>().EmitPingReply(aMessageInfo.GetPeerAddr(), reply.mSize, timestamp, reply.mHopLimit);
#endif
    // Received all ping replies, no need to wait longer.
    if (!mStatistics.mIsMulticast && mConfig.mCount == 0 && aIcmpHeader.GetSequence() == mTargetEchoSequence)
    {
        mTimer.Stop();
    }
    mConfig.InvokeReplyCallback(reply);
    // Received all ping replies, no need to wait longer.
    if (!mStatistics.mIsMulticast && mConfig.mCount == 0 && aIcmpHeader.GetSequence() == mTargetEchoSequence)
    {
        mConfig.InvokeStatisticsCallback(mStatistics);
    }

exit:
    return;
}

} // namespace Utils
} // namespace ot

#endif // #if OPENTHREAD_CONFIG_PING_SENDER_ENABLE
