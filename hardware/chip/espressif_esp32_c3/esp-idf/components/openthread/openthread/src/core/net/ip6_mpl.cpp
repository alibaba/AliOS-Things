/*
 *  Copyright (c) 2016, The OpenThread Authors.
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
 *   This file implements MPL.
 */

#include "ip6_mpl.hpp"

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/message.hpp"
#include "common/random.hpp"
#include "net/ip6.hpp"

namespace ot {
namespace Ip6 {

Mpl::Mpl(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mMatchingAddress(nullptr)
    , mSeedSetTimer(aInstance, Mpl::HandleSeedSetTimer)
    , mSeedId(0)
    , mSequence(0)
#if OPENTHREAD_FTD
    , mRetransmissionTimer(aInstance, Mpl::HandleRetransmissionTimer)
    , mTimerExpirations(0)
#endif
{
    memset(mSeedSet, 0, sizeof(mSeedSet));
}

void Mpl::InitOption(OptionMpl &aOption, const Address &aAddress)
{
    aOption.Init();
    aOption.SetSequence(mSequence++);

    // Check if Seed Id can be elided.
    if (mMatchingAddress && aAddress == *mMatchingAddress)
    {
        aOption.SetSeedIdLength(OptionMpl::kSeedIdLength0);

        // Decrease default option length.
        aOption.SetLength(aOption.GetLength() - sizeof(mSeedId));
    }
    else
    {
        aOption.SetSeedIdLength(OptionMpl::kSeedIdLength2);
        aOption.SetSeedId(mSeedId);
    }
}

Error Mpl::ProcessOption(Message &aMessage, const Address &aAddress, bool aIsOutbound, bool &aReceive)
{
    Error     error;
    OptionMpl option;

    VerifyOrExit(aMessage.ReadBytes(aMessage.GetOffset(), &option, sizeof(option)) >= OptionMpl::kMinLength &&
                     (option.GetSeedIdLength() == OptionMpl::kSeedIdLength0 ||
                      option.GetSeedIdLength() == OptionMpl::kSeedIdLength2),
                 error = kErrorParse);

    if (option.GetSeedIdLength() == OptionMpl::kSeedIdLength0)
    {
        // Retrieve MPL Seed Id from the IPv6 Source Address.
        option.SetSeedId(HostSwap16(aAddress.mFields.m16[7]));
    }

    // Check if the MPL Data Message is new.
    error = UpdateSeedSet(option.GetSeedId(), option.GetSequence());

    if (error == kErrorNone)
    {
#if OPENTHREAD_FTD
        AddBufferedMessage(aMessage, option.GetSeedId(), option.GetSequence(), aIsOutbound);
#endif
    }
    else if (aIsOutbound)
    {
        aReceive = false;
        // In case MPL Data Message is generated locally, ignore potential error of the MPL Seed Set
        // to allow subsequent retransmissions with the same sequence number.
        ExitNow(error = kErrorNone);
    }

exit:
    return error;
}

/*
 * mSeedSet stores recently received (Seed ID, Sequence) values.
 * - (Seed ID, Sequence) values are grouped by Seed ID.
 * - (Seed ID, Sequence) groups are not sorted by Seed ID relative to other groups.
 * - (Seed ID, Sequence) values within a group are sorted by Sequence.
 * - All unused entries (marked by 0 lifetime) are grouped at the end.
 *
 * Update process:
 *
 * - Eviction selection:
 *   - If there are unused entries, mark the first unused entry for "eviction"
 *   - Otherwise, pick the first entry of the group that has the most entries.
 *
 * - Insert selection:
 *   - If there exists a group matching the Seed ID, select insert entry based on Sequence ordering.
 *   - Otherwise, set insert entry equal to evict entry.
 *
 * - If evicting a valid entry (lifetime non-zero):
 *   - Require group size to have >=2 entries.
 *   - If inserting into existing group, require Sequence to be larger than oldest stored Sequence in group.
 */
Error Mpl::UpdateSeedSet(uint16_t aSeedId, uint8_t aSequence)
{
    Error      error    = kErrorNone;
    SeedEntry *insert   = nullptr;
    SeedEntry *group    = mSeedSet;
    SeedEntry *evict    = mSeedSet;
    uint8_t    curCount = 0;
    uint8_t    maxCount = 0;

    for (uint32_t i = 0; i < kNumSeedEntries; i++, curCount++)
    {
        if (mSeedSet[i].mLifetime == 0)
        {
            // unused entries exist

            if (insert == nullptr)
            {
                // no existing group, set insert and evict entry to be the same
                insert = &mSeedSet[i];
            }

            // mark first unused entry for eviction
            evict = &mSeedSet[i];
            break;
        }

        if (mSeedSet[i].mSeedId != group->mSeedId)
        {
            // processing new group

            if (aSeedId == group->mSeedId && insert == nullptr)
            {
                // insert at end of existing group
                insert = &mSeedSet[i];
                curCount++;
            }

            if (maxCount < curCount)
            {
                // look to evict an entry from the seed with the most entries
                evict    = group;
                maxCount = curCount;
            }

            group    = &mSeedSet[i];
            curCount = 0;
        }

        if (aSeedId == mSeedSet[i].mSeedId)
        {
            // have existing entries for aSeedId

            int8_t diff = static_cast<int8_t>(aSequence - mSeedSet[i].mSequence);

            if (diff == 0)
            {
                // already received, drop message
                ExitNow(error = kErrorDrop);
            }
            else if (insert == nullptr && diff < 0)
            {
                // insert in order of sequence
                insert = &mSeedSet[i];
                curCount++;
            }
        }
    }

    if (evict->mLifetime != 0)
    {
        // no free entries available, look to evict an existing entry
        OT_ASSERT(curCount != 0);

        if (aSeedId == group->mSeedId && insert == nullptr)
        {
            // insert at end of existing group
            insert = &mSeedSet[kNumSeedEntries];
            curCount++;
        }

        if (maxCount < curCount)
        {
            // look to evict an entry from the seed with the most entries
            evict    = group;
            maxCount = curCount;
        }

        // require evict group size to have >= 2 entries
        VerifyOrExit(maxCount > 1, error = kErrorDrop);

        if (insert == nullptr)
        {
            // no existing entries for aSeedId
            insert = evict;
        }
        else
        {
            // require Sequence to be larger than oldest stored Sequence in group
            VerifyOrExit(insert > mSeedSet && aSeedId == (insert - 1)->mSeedId, error = kErrorDrop);
        }
    }

    if (evict > insert)
    {
        OT_ASSERT(insert >= mSeedSet);
        memmove(insert + 1, insert, static_cast<size_t>(evict - insert) * sizeof(SeedEntry));
    }
    else if (evict < insert)
    {
        OT_ASSERT(evict >= mSeedSet);
        memmove(evict, evict + 1, static_cast<size_t>(insert - 1 - evict) * sizeof(SeedEntry));
        insert--;
    }

    insert->mSeedId   = aSeedId;
    insert->mSequence = aSequence;
    insert->mLifetime = kSeedEntryLifetime;

    if (!mSeedSetTimer.IsRunning())
    {
        mSeedSetTimer.Start(kSeedEntryLifetimeDt);
    }

exit:
    return error;
}

void Mpl::HandleSeedSetTimer(Timer &aTimer)
{
    aTimer.Get<Mpl>().HandleSeedSetTimer();
}

void Mpl::HandleSeedSetTimer(void)
{
    bool startTimer = false;
    int  j          = 0;

    for (int i = 0; i < kNumSeedEntries && mSeedSet[i].mLifetime; i++)
    {
        mSeedSet[i].mLifetime--;

        if (mSeedSet[i].mLifetime > 0)
        {
            mSeedSet[j++] = mSeedSet[i];
            startTimer    = true;
        }
    }

    for (; j < kNumSeedEntries && mSeedSet[j].mLifetime; j++)
    {
        mSeedSet[j].mLifetime = 0;
    }

    if (startTimer)
    {
        mSeedSetTimer.Start(kSeedEntryLifetimeDt);
    }
}

#if OPENTHREAD_FTD

void Mpl::AddBufferedMessage(Message &aMessage, uint16_t aSeedId, uint8_t aSequence, bool aIsOutbound)
{
    Error    error       = kErrorNone;
    Message *messageCopy = nullptr;
    Metadata metadata;
    uint8_t  hopLimit = 0;

#if OPENTHREAD_CONFIG_MPL_DYNAMIC_INTERVAL_ENABLE
    // adjust the first MPL forward interval dynamically according to the network scale
    uint8_t interval = (kDataMessageInterval / Mle::kMaxRouters) * Get<RouterTable>().GetNeighborCount();
#else
    uint8_t interval = kDataMessageInterval;
#endif

    VerifyOrExit(GetTimerExpirations() > 0);
    VerifyOrExit((messageCopy = aMessage.Clone()) != nullptr, error = kErrorNoBufs);

    if (!aIsOutbound)
    {
        IgnoreError(aMessage.Read(Header::kHopLimitFieldOffset, hopLimit));
        VerifyOrExit(hopLimit-- > 1, error = kErrorDrop);
        messageCopy->Write(Header::kHopLimitFieldOffset, hopLimit);
    }

    metadata.mSeedId            = aSeedId;
    metadata.mSequence          = aSequence;
    metadata.mTransmissionCount = aIsOutbound ? 1 : 0;
    metadata.mIntervalOffset    = 0;
    metadata.GenerateNextTransmissionTime(TimerMilli::GetNow(), interval);

    SuccessOrExit(error = metadata.AppendTo(*messageCopy));
    mBufferedMessageSet.Enqueue(*messageCopy);

    mRetransmissionTimer.FireAtIfEarlier(metadata.mTransmissionTime);

exit:
    FreeMessageOnError(messageCopy, error);
}

void Mpl::HandleRetransmissionTimer(Timer &aTimer)
{
    aTimer.Get<Mpl>().HandleRetransmissionTimer();
}

void Mpl::HandleRetransmissionTimer(void)
{
    TimeMilli now      = TimerMilli::GetNow();
    TimeMilli nextTime = now.GetDistantFuture();
    Metadata  metadata;
    Message * message;
    Message * nextMessage;

    for (message = mBufferedMessageSet.GetHead(); message != nullptr; message = nextMessage)
    {
        nextMessage = message->GetNext();

        metadata.ReadFrom(*message);

        if (now < metadata.mTransmissionTime)
        {
            if (nextTime > metadata.mTransmissionTime)
            {
                nextTime = metadata.mTransmissionTime;
            }
        }
        else
        {
            // Update the number of transmission timer expirations.
            metadata.mTransmissionCount++;

            if (metadata.mTransmissionCount < GetTimerExpirations())
            {
                Message *messageCopy = message->Clone(message->GetLength() - sizeof(Metadata));

                if (messageCopy != nullptr)
                {
                    if (metadata.mTransmissionCount > 1)
                    {
                        messageCopy->SetSubType(Message::kSubTypeMplRetransmission);
                    }

                    Get<Ip6>().EnqueueDatagram(*messageCopy);
                }

                metadata.GenerateNextTransmissionTime(now, kDataMessageInterval);
                metadata.UpdateIn(*message);

                if (nextTime > metadata.mTransmissionTime)
                {
                    nextTime = metadata.mTransmissionTime;
                }
            }
            else
            {
                mBufferedMessageSet.Dequeue(*message);

                if (metadata.mTransmissionCount == GetTimerExpirations())
                {
                    if (metadata.mTransmissionCount > 1)
                    {
                        message->SetSubType(Message::kSubTypeMplRetransmission);
                    }

                    metadata.RemoveFrom(*message);
                    Get<Ip6>().EnqueueDatagram(*message);
                }
                else
                {
                    // Stop retransmitting if the number of timer expirations is already exceeded.
                    message->Free();
                }
            }
        }
    }

    if (nextTime < now.GetDistantFuture())
    {
        mRetransmissionTimer.FireAt(nextTime);
    }
}

void Mpl::Metadata::ReadFrom(const Message &aMessage)
{
    uint16_t length = aMessage.GetLength();

    OT_ASSERT(length >= sizeof(*this));
    IgnoreError(aMessage.Read(length - sizeof(*this), *this));
}

void Mpl::Metadata::RemoveFrom(Message &aMessage) const
{
    Error error = aMessage.SetLength(aMessage.GetLength() - sizeof(*this));

    OT_ASSERT(error == kErrorNone);
    OT_UNUSED_VARIABLE(error);
}

void Mpl::Metadata::UpdateIn(Message &aMessage) const
{
    aMessage.Write(aMessage.GetLength() - sizeof(*this), *this);
}

void Mpl::Metadata::GenerateNextTransmissionTime(TimeMilli aCurrentTime, uint8_t aInterval)
{
    // Emulate Trickle timer behavior and set up the next retransmission within [0,I) range.
    uint8_t t = (aInterval == 0) ? aInterval : Random::NonCrypto::GetUint8InRange(0, aInterval);

    // Set transmission time at the beginning of the next interval.
    mTransmissionTime = aCurrentTime + static_cast<uint32_t>(mIntervalOffset + t);
    mIntervalOffset   = aInterval - t;
}

#endif // OPENTHREAD_FTD

} // namespace Ip6
} // namespace ot
