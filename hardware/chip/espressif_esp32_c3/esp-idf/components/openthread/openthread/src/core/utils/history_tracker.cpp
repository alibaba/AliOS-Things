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
 *   This file implements the History Tracker module.
 */

#include "history_tracker.hpp"

#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/string.hpp"
#include "common/timer.hpp"

namespace ot {
namespace Utils {

//---------------------------------------------------------------------------------------------------------------------
// HistoryTracker

HistoryTracker::HistoryTracker(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mTimer(aInstance, HandleTimer)
{
    mTimer.Start(kAgeCheckPeriod);
}

void HistoryTracker::RecordNetworkInfo(void)
{
    NetworkInfo *   entry = mNetInfoHistory.AddNewEntry();
    Mle::DeviceMode mode;

    VerifyOrExit(entry != nullptr);

    entry->mRole        = static_cast<otDeviceRole>(Get<Mle::Mle>().GetRole());
    entry->mRloc16      = Get<Mle::Mle>().GetRloc16();
    entry->mPartitionId = Get<Mle::Mle>().GetLeaderData().GetPartitionId();
    mode                = Get<Mle::Mle>().GetDeviceMode();
    mode.Get(entry->mMode);

exit:
    return;
}

void HistoryTracker::RecordMessage(const Message &aMessage, const Mac::Address &aMacAddresss, MessageType aType)
{
    MessageInfo *     entry = nullptr;
    Ip6::Header       ip6Header;
    Ip6::Icmp::Header icmp6Header;
    uint8_t           ip6Proto;
    uint16_t          checksum;
    uint16_t          sourcePort;
    uint16_t          destPort;

    VerifyOrExit(aMessage.GetType() == Message::kTypeIp6);

    SuccessOrExit(MeshForwarder::ParseIp6UdpTcpHeader(aMessage, ip6Header, checksum, sourcePort, destPort));

    ip6Proto = ip6Header.GetNextHeader();

#if OPENTHREAD_CONFIG_HISTORY_TRACKER_EXCLUDE_THREAD_CONTROL_MESSAGES
    if (ip6Proto == Ip6::kProtoUdp)
    {
        uint16_t port = 0;

        switch (aType)
        {
        case kRxMessage:
            port = destPort;
            break;

        case kTxMessage:
            port = sourcePort;
            break;
        }

        VerifyOrExit((port != Mle::kUdpPort) && (port != Tmf::kUdpPort));
    }
#endif

    if (ip6Proto == Ip6::kProtoIcmp6)
    {
        SuccessOrExit(aMessage.Read(sizeof(Ip6::Header), icmp6Header));
        checksum = icmp6Header.GetChecksum();
    }
    else
    {
        icmp6Header.Clear();
    }

    switch (aType)
    {
    case kRxMessage:
        entry = mRxHistory.AddNewEntry();
        break;

    case kTxMessage:
        entry = mTxHistory.AddNewEntry();
        break;
    }

    VerifyOrExit(entry != nullptr);

    entry->mPayloadLength        = ip6Header.GetPayloadLength();
    entry->mNeighborRloc16       = aMacAddresss.IsShort() ? aMacAddresss.GetShort() : kInvalidRloc16;
    entry->mSource.mAddress      = ip6Header.GetSource();
    entry->mSource.mPort         = sourcePort;
    entry->mDestination.mAddress = ip6Header.GetDestination();
    entry->mDestination.mPort    = destPort;
    entry->mChecksum             = checksum;
    entry->mIpProto              = ip6Proto;
    entry->mIcmp6Type            = icmp6Header.GetType();
    entry->mAveRxRss             = (aType == kRxMessage) ? aMessage.GetRssAverager().GetAverage() : kInvalidRss;
    entry->mLinkSecurity         = aMessage.IsLinkSecurityEnabled();
    entry->mTxSuccess            = (aType == kTxMessage) ? aMessage.GetTxSuccess() : true;
    entry->mPriority             = aMessage.GetPriority();

    if (aMacAddresss.IsExtended())
    {
        Neighbor *neighbor = Get<NeighborTable>().FindNeighbor(aMacAddresss, Neighbor::kInStateAnyExceptInvalid);

        if (neighbor != nullptr)
        {
            entry->mNeighborRloc16 = neighbor->GetRloc16();
        }
    }

#if OPENTHREAD_CONFIG_MULTI_RADIO
    if (aMessage.IsRadioTypeSet())
    {
        switch (aMessage.GetRadioType())
        {
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
        case Mac::kRadioTypeIeee802154:
            entry->mRadioIeee802154 = true;
            break;
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
        case Mac::kRadioTypeTrel:
            entry->mRadioTrelUdp6 = true;
            break;
#endif
        }

        // Radio type may not be set on a tx message indicating that it
        // was sent over all radio types (e.g., for broadcast frame).
        // In such a case, we set all supported radios from `else`
        // block below.
    }
    else
#endif // OPENTHREAD_CONFIG_MULTI_RADIO
    {
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
        entry->mRadioIeee802154 = true;
#endif

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
        entry->mRadioTrelUdp6 = true;
#endif
    }

exit:
    return;
}

void HistoryTracker::RecordNeighborEvent(NeighborTable::Event aEvent, const NeighborTable::EntryInfo &aInfo)
{
    NeighborInfo *entry = mNeighborHistory.AddNewEntry();

    VerifyOrExit(entry != nullptr);

    switch (aEvent)
    {
    case NeighborTable::kChildAdded:
    case NeighborTable::kChildRemoved:
    case NeighborTable::kChildModeChanged:
        entry->mExtAddress       = aInfo.mInfo.mChild.mExtAddress;
        entry->mRloc16           = aInfo.mInfo.mChild.mRloc16;
        entry->mAverageRssi      = aInfo.mInfo.mChild.mAverageRssi;
        entry->mRxOnWhenIdle     = aInfo.mInfo.mChild.mRxOnWhenIdle;
        entry->mFullThreadDevice = aInfo.mInfo.mChild.mFullThreadDevice;
        entry->mFullNetworkData  = aInfo.mInfo.mChild.mFullNetworkData;
        entry->mIsChild          = true;
        break;

    case NeighborTable::kRouterAdded:
    case NeighborTable::kRouterRemoved:
        entry->mExtAddress       = aInfo.mInfo.mRouter.mExtAddress;
        entry->mRloc16           = aInfo.mInfo.mRouter.mRloc16;
        entry->mAverageRssi      = aInfo.mInfo.mRouter.mAverageRssi;
        entry->mRxOnWhenIdle     = aInfo.mInfo.mRouter.mRxOnWhenIdle;
        entry->mFullThreadDevice = aInfo.mInfo.mRouter.mFullThreadDevice;
        entry->mFullNetworkData  = aInfo.mInfo.mRouter.mFullNetworkData;
        entry->mIsChild          = false;
        break;
    }

    switch (aEvent)
    {
    case NeighborTable::kChildAdded:
        if (aInfo.mInfo.mChild.mIsStateRestoring)
        {
            entry->mEvent = kNeighborRestoring;
            break;
        }

        OT_FALL_THROUGH;

    case NeighborTable::kRouterAdded:
        entry->mEvent = kNeighborAdded;
        break;

    case NeighborTable::kChildRemoved:
    case NeighborTable::kRouterRemoved:
        entry->mEvent = kNeighborRemoved;
        break;

    case NeighborTable::kChildModeChanged:
        entry->mEvent = kNeighborChanged;
        break;
    }

exit:
    return;
}

void HistoryTracker::RecordAddressEvent(Ip6::Netif::AddressEvent          aEvent,
                                        const Ip6::Netif::UnicastAddress &aUnicastAddress)
{
    UnicastAddressInfo *entry = mUnicastAddressHistory.AddNewEntry();

    VerifyOrExit(entry != nullptr);

    entry->mAddress       = aUnicastAddress.GetAddress();
    entry->mPrefixLength  = aUnicastAddress.GetPrefixLength();
    entry->mAddressOrigin = aUnicastAddress.GetOrigin();
    entry->mEvent         = (aEvent == Ip6::Netif::kAddressAdded) ? kAddressAdded : kAddressRemoved;
    entry->mScope         = (aUnicastAddress.GetScope() & 0xf);
    entry->mPreferred     = aUnicastAddress.mPreferred;
    entry->mValid         = aUnicastAddress.mValid;
    entry->mRloc          = aUnicastAddress.mRloc;

exit:
    return;
}

void HistoryTracker::RecordAddressEvent(Ip6::Netif::AddressEvent            aEvent,
                                        const Ip6::Netif::MulticastAddress &aMulticastAddress,
                                        Ip6::Netif::AddressOrigin           aAddressOrigin)
{
    MulticastAddressInfo *entry = mMulticastAddressHistory.AddNewEntry();

    VerifyOrExit(entry != nullptr);

    entry->mAddress       = aMulticastAddress.GetAddress();
    entry->mAddressOrigin = aAddressOrigin;
    entry->mEvent         = (aEvent == Ip6::Netif::kAddressAdded) ? kAddressAdded : kAddressRemoved;

exit:
    return;
}

void HistoryTracker::HandleNotifierEvents(Events aEvents)
{
    if (aEvents.ContainsAny(kEventThreadRoleChanged | kEventThreadRlocAdded | kEventThreadRlocRemoved |
                            kEventThreadPartitionIdChanged))
    {
        RecordNetworkInfo();
    }
}

void HistoryTracker::HandleTimer(Timer &aTimer)
{
    aTimer.Get<HistoryTracker>().HandleTimer();
}

void HistoryTracker::HandleTimer(void)
{
    mNetInfoHistory.UpdateAgedEntries();
    mUnicastAddressHistory.UpdateAgedEntries();
    mMulticastAddressHistory.UpdateAgedEntries();
    mRxHistory.UpdateAgedEntries();
    mTxHistory.UpdateAgedEntries();
    mNeighborHistory.UpdateAgedEntries();

    mTimer.Start(kAgeCheckPeriod);
}

void HistoryTracker::EntryAgeToString(uint32_t aEntryAge, char *aBuffer, uint16_t aSize)
{
    StringWriter writer(aBuffer, aSize);

    if (aEntryAge >= kMaxAge)
    {
        writer.Append("more than %u days", kMaxAge / kOneDayInMsec);
    }
    else
    {
        uint32_t days = aEntryAge / kOneDayInMsec;

        if (days > 0)
        {
            writer.Append("%u day%s ", days, (days == 1) ? "" : "s");
            aEntryAge -= days * kOneDayInMsec;
        }

        writer.Append("%02u:%02u:%02u.%03u", (aEntryAge / kOneHourInMsec),
                      (aEntryAge % kOneHourInMsec) / kOneMinuteInMsec,
                      (aEntryAge % kOneMinuteInMsec) / kOneSecondInMsec, (aEntryAge % kOneSecondInMsec));
    }
}

//---------------------------------------------------------------------------------------------------------------------
// HistoryTracker::Timestamp

void HistoryTracker::Timestamp::SetToNow(void)
{
    mTime = TimerMilli::GetNow();

    // If the current time happens to be the special value which we
    // use to indicate "distant past", decrement the time by one.

    if (mTime.GetValue() == kDistantPast)
    {
        mTime.SetValue(mTime.GetValue() - 1);
    }
}

uint32_t HistoryTracker::Timestamp::GetDurationTill(TimeMilli aTime) const
{
    return IsDistantPast() ? kMaxAge : OT_MIN(aTime - mTime, kMaxAge);
}

//---------------------------------------------------------------------------------------------------------------------
// HistoryTracker::List

HistoryTracker::List::List(void)
    : mStartIndex(0)
    , mSize(0)
{
}

void HistoryTracker::List::Clear(void)
{
    mStartIndex = 0;
    mSize       = 0;
}

uint16_t HistoryTracker::List::Add(uint16_t aMaxSize, Timestamp aTimestamps[])
{
    // Add a new entry and return its list index. Overwrites the
    // oldest entry if list is full.
    //
    // Entries are saved in the order they are added such that
    // `mStartIndex` is the newest entry and the entries after up
    // to `mSize` are the previously added entries.

    mStartIndex = (mStartIndex == 0) ? aMaxSize - 1 : mStartIndex - 1;
    mSize += (mSize == aMaxSize) ? 0 : 1;

    aTimestamps[mStartIndex].SetToNow();

    return mStartIndex;
}

Error HistoryTracker::List::Iterate(uint16_t        aMaxSize,
                                    const Timestamp aTimestamps[],
                                    Iterator &      aIterator,
                                    uint16_t &      aListIndex,
                                    uint32_t &      aEntryAge) const
{
    Error error = kErrorNone;

    VerifyOrExit(aIterator.GetEntryNumber() < mSize, error = kErrorNotFound);

    aListIndex = MapEntryNumberToListIndex(aIterator.GetEntryNumber(), aMaxSize);
    aEntryAge  = aTimestamps[aListIndex].GetDurationTill(aIterator.GetInitTime());

    aIterator.IncrementEntryNumber();

exit:
    return error;
}

uint16_t HistoryTracker::List::MapEntryNumberToListIndex(uint16_t aEntryNumber, uint16_t aMaxSize) const
{
    // Map the `aEntryNumber` to the list index. `aEntryNumber` value
    // of zero corresponds to the newest (the most recently added)
    // entry and value one to next one and so on. List index
    // warps at the end of array to start of array. Caller MUST
    // ensure `aEntryNumber` is smaller than `mSize`.

    uint32_t index;

    OT_ASSERT(aEntryNumber < mSize);

    index = static_cast<uint32_t>(aEntryNumber) + mStartIndex;
    index -= (index >= aMaxSize) ? aMaxSize : 0;

    return static_cast<uint16_t>(index);
}

void HistoryTracker::List::UpdateAgedEntries(uint16_t aMaxSize, Timestamp aTimestamps[])
{
    TimeMilli now = TimerMilli::GetNow();

    // We go through the entries in reverse (starting with the oldest
    // entry) and check if the entry's age is larger than `kMaxAge`
    // and if so mark it as "distant past". We can stop as soon as we
    // get to an entry with age smaller than max.
    //
    // The `for()` loop condition is `(entryNumber < mSize)` which
    // ensures that we go through the loop body for `entryNumber`
    // value of zero and then in the next iteration (when the
    // `entryNumber` rolls over) we stop.

    for (uint16_t entryNumber = mSize - 1; entryNumber < mSize; entryNumber--)
    {
        uint16_t index = MapEntryNumberToListIndex(entryNumber, aMaxSize);

        if (aTimestamps[index].GetDurationTill(now) < kMaxAge)
        {
            break;
        }

        aTimestamps[index].MarkAsDistantPast();
    }
}

} // namespace Utils
} // namespace ot

#endif // #if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
