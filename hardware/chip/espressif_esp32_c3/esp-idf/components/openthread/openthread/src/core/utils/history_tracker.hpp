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
 *   This file includes definitions to support History Tracker module.
 */

#ifndef HISTORY_TRACKER_HPP_
#define HISTORY_TRACKER_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE

#include <openthread/history_tracker.h>
#include <openthread/platform/radio.h>

#include "common/clearable.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/notifier.hpp"
#include "common/timer.hpp"
#include "net/netif.hpp"
#include "net/socket.hpp"
#include "thread/mesh_forwarder.hpp"
#include "thread/mle.hpp"
#include "thread/mle_types.hpp"
#include "thread/neighbor_table.hpp"

namespace ot {
namespace Utils {

/**
 * This class implements History Tracker.
 *
 */
class HistoryTracker : public InstanceLocator, private NonCopyable
{
    friend class ot::MeshForwarder;
    friend class ot::Notifier;
    friend class ot::Mle::Mle;
    friend class ot::NeighborTable;
    friend class ot::Ip6::Netif;

public:
    /**
     * This constant specifies the maximum age of entries which is 49 days (value in msec).
     *
     * Entries older than the max age will give this value as their age.
     *
     */
    static constexpr uint32_t kMaxAge = OT_HISTORY_TRACKER_MAX_AGE;

    /**
     * This constant specifies the recommend string size to represent an entry age
     *
     */
    static constexpr uint16_t kEntryAgeStringSize = OT_HISTORY_TRACKER_ENTRY_AGE_STRING_SIZE;

    /**
     * This type represents an iterator to iterate through a history list.
     *
     */
    class Iterator : public otHistoryTrackerIterator
    {
        friend class HistoryTracker;

    public:
        /**
         * This method initializes an `Iterator`
         *
         * An iterator MUST be initialized before it is used. An iterator can be initialized again to start from
         * the beginning of the list.
         *
         */
        void Init(void) { ResetEntryNumber(), SetInitTime(); }

    private:
        uint16_t  GetEntryNumber(void) const { return mData16; }
        void      ResetEntryNumber(void) { mData16 = 0; }
        void      IncrementEntryNumber(void) { mData16++; }
        TimeMilli GetInitTime(void) const { return TimeMilli(mData32); }
        void      SetInitTime(void) { mData32 = TimerMilli::GetNow().GetValue(); }
    };

    /**
     * This type represents Thread network info.
     *
     */
    typedef otHistoryTrackerNetworkInfo NetworkInfo;

    /**
     * This structure represents a unicast IPv6 address info.
     *
     */
    typedef otHistoryTrackerUnicastAddressInfo UnicastAddressInfo;

    /**
     * This structure represent a multicast IPv6 address info.
     *
     */
    typedef otHistoryTrackerMulticastAddressInfo MulticastAddressInfo;

    /**
     * This type represents a RX/TX IPv6 message info.
     *
     */
    typedef otHistoryTrackerMessageInfo MessageInfo;

    /**
     * This type represents a neighbor info.
     *
     */
    typedef otHistoryTrackerNeighborInfo NeighborInfo;

    /**
     * This constructor initializes the `HistoryTracker`.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit HistoryTracker(Instance &aInstance);

    /**
     * This method iterates over the entries in the network info history list.
     *
     * @param[inout] aIterator  An iterator. MUST be initialized.
     * @param[out]   aEntryAge  A reference to a variable to output the entry's age.
     *                          Age is provided as the duration (in milliseconds) from when entry was recorded to
     *                          @p aIterator initialization time. It is set to `kMaxAge` for entries older than max age.
     *
     * @returns A pointer to `NetworkInfo` entry or `nullptr` if no more entries in the list.
     *
     */
    const NetworkInfo *IterateNetInfoHistory(Iterator &aIterator, uint32_t &aEntryAge) const
    {
        return mNetInfoHistory.Iterate(aIterator, aEntryAge);
    }

    /**
     * This method iterates over the entries in the unicast address history list.
     *
     * @param[inout] aIterator  An iterator. MUST be initialized.
     * @param[out]   aEntryAge  A reference to a variable to output the entry's age.
     *                          Age is provided as the duration (in milliseconds) from when entry was recorded to
     *                          @p aIterator initialization time. It is set to `kMaxAge` for entries older than max age.
     *
     * @returns A pointer to `UnicastAddress` entry or `nullptr` if no more entries in the list.
     *
     */
    const UnicastAddressInfo *IterateUnicastAddressHistory(Iterator &aIterator, uint32_t &aEntryAge) const
    {
        return mUnicastAddressHistory.Iterate(aIterator, aEntryAge);
    }

    /**
     * This method iterates over the entries in the multicast address history list.
     *
     * @param[inout] aIterator  An iterator. MUST be initialized.
     * @param[out]   aEntryAge  A reference to a variable to output the entry's age.
     *                          Age is provided as the duration (in milliseconds) from when entry was recorded to
     *                          @p aIterator initialization time. It is set to `kMaxAge` for entries older than max age.
     *
     * @returns A pointer to `MulticastAddress` entry or `nullptr` if no more entries in the list.
     *
     */
    const MulticastAddressInfo *IterateMulticastAddressHistory(Iterator &aIterator, uint32_t &aEntryAge) const
    {
        return mMulticastAddressHistory.Iterate(aIterator, aEntryAge);
    }

    /**
     * This method iterates over the entries in the RX history list.
     *
     * @param[inout] aIterator  An iterator. MUST be initialized.
     * @param[out]   aEntryAge  A reference to a variable to output the entry's age.
     *                          Age is provided as the duration (in milliseconds) from when entry was recorded to
     *                          @p aIterator initialization time. It is set to `kMaxAge` for entries older than max age.
     *
     * @returns A pointer to `MessageInfo` entry or `nullptr` if no more entries in the list.
     *
     */
    const MessageInfo *IterateRxHistory(Iterator &aIterator, uint32_t &aEntryAge) const
    {
        return mRxHistory.Iterate(aIterator, aEntryAge);
    }

    /**
     * This method iterates over the entries in the TX history list.
     *
     * @param[inout] aIterator  An iterator. MUST be initialized.
     * @param[out]   aEntryAge  A reference to a variable to output the entry's age.
     *                          Age is provided as the duration (in milliseconds) from when entry was recorded to
     *                          @p aIterator initialization time. It is set to `kMaxAge` for entries older than max age.
     *
     * @returns A pointer to `MessageInfo` entry or `nullptr` if no more entries in the list.
     *
     */
    const MessageInfo *IterateTxHistory(Iterator &aIterator, uint32_t &aEntryAge) const
    {
        return mTxHistory.Iterate(aIterator, aEntryAge);
    }

    const NeighborInfo *IterateNeighborHistory(Iterator &aIterator, uint32_t &aEntryAge) const
    {
        return mNeighborHistory.Iterate(aIterator, aEntryAge);
    }

    /**
     * This static method converts a given entry age to a human-readable string.
     *
     * The entry age string follows the format "<hh>:<mm>:<ss>.<mmmm>" for hours, minutes, seconds and millisecond
     * (if shorter than one day) or "<dd> days <hh>:<mm>:<ss>.<mmmm>" (if longer than one day).
     *
     * If the resulting string does not fit in @p aBuffer (within its @p aSize characters), the string will be
     * truncated but the outputted string is always null-terminated.
     *
     * @param[in]  aEntryAge The entry age (duration in msec).
     * @param[out] aBuffer   A pointer to a char array to output the string (MUST NOT be NULL).
     * @param[in]  aSize     The size of @p aBuffer (in bytes). Recommended to use `OT_IP6_ADDRESS_STRING_SIZE`.
     *
     */
    static void EntryAgeToString(uint32_t aEntryAge, char *aBuffer, uint16_t aSize);

private:
    static constexpr uint32_t kOneSecondInMsec = 1000;
    static constexpr uint32_t kOneMinuteInMsec = 60 * kOneSecondInMsec;
    static constexpr uint32_t kOneHourInMsec   = 60 * kOneMinuteInMsec;
    static constexpr uint32_t kOneDayInMsec    = 24 * kOneHourInMsec;

    // `Timestamp` uses `uint32_t` value. `2^32` msec is 49 days, 17
    // hours, 2 minutes and 47 seconds and 296 msec. We use 49 days
    // as `kMaxAge` and check for aged entries every 16 hours.

    static constexpr uint32_t kAgeCheckPeriod = 16 * kOneHourInMsec;

    static constexpr uint16_t kNetInfoListSize       = OPENTHREAD_CONFIG_HISTORY_TRACKER_NET_INFO_LIST_SIZE;
    static constexpr uint16_t kUnicastAddrListSize   = OPENTHREAD_CONFIG_HISTORY_TRACKER_UNICAST_ADDRESS_LIST_SIZE;
    static constexpr uint16_t kMulticastAddrListSize = OPENTHREAD_CONFIG_HISTORY_TRACKER_MULTICAST_ADDRESS_LIST_SIZE;
    static constexpr uint16_t kRxListSize            = OPENTHREAD_CONFIG_HISTORY_TRACKER_RX_LIST_SIZE;
    static constexpr uint16_t kTxListSize            = OPENTHREAD_CONFIG_HISTORY_TRACKER_TX_LIST_SIZE;
    static constexpr uint16_t kNeighborListSize      = OPENTHREAD_CONFIG_HISTORY_TRACKER_NEIGHBOR_LIST_SIZE;

    typedef otHistoryTrackerAddressEvent AddressEvent;

    static constexpr AddressEvent kAddressAdded   = OT_HISTORY_TRACKER_ADDRESS_EVENT_ADDED;
    static constexpr AddressEvent kAddressRemoved = OT_HISTORY_TRACKER_ADDRESS_EVENT_REMOVED;

    static constexpr int8_t   kInvalidRss    = OT_RADIO_RSSI_INVALID;
    static constexpr uint16_t kInvalidRloc16 = Mac::kShortAddrInvalid;

    typedef otHistoryTrackerNeighborEvent NeighborEvent;

    static constexpr NeighborEvent kNeighborAdded     = OT_HISTORY_TRACKER_NEIGHBOR_EVENT_ADDED;
    static constexpr NeighborEvent kNeighborRemoved   = OT_HISTORY_TRACKER_NEIGHBOR_EVENT_REMOVED;
    static constexpr NeighborEvent kNeighborChanged   = OT_HISTORY_TRACKER_NEIGHBOR_EVENT_CHANGED;
    static constexpr NeighborEvent kNeighborRestoring = OT_HISTORY_TRACKER_NEIGHBOR_EVENT_RESTORING;

    class Timestamp
    {
    public:
        void     SetToNow(void);
        uint32_t GetDurationTill(TimeMilli aTime) const;
        bool     IsDistantPast(void) const { return (mTime.GetValue() == kDistantPast); }
        void     MarkAsDistantPast(void) { return mTime.SetValue(kDistantPast); }

    private:
        static constexpr uint32_t kDistantPast = 0;

        TimeMilli mTime;
    };

    // An ordered list of timestamped items (base class of `EntryList<Entry, kSize>`).
    class List : private NonCopyable
    {
    public:
        void     Clear(void);
        uint16_t GetSize(void) const { return mSize; }

    protected:
        List(void);
        uint16_t Add(uint16_t aMaxSize, Timestamp aTimestamps[]);
        void     UpdateAgedEntries(uint16_t aMaxSize, Timestamp aTimestamps[]);
        uint16_t MapEntryNumberToListIndex(uint16_t aEntryNumber, uint16_t aMaxSize) const;
        Error    Iterate(uint16_t        aMaxSize,
                         const Timestamp aTimestamps[],
                         Iterator &      aIterator,
                         uint16_t &      aListIndex,
                         uint32_t &      aEntryAge) const;

    private:
        uint16_t mStartIndex;
        uint16_t mSize;
    };

    // A history list (with given max size) of timestamped `Entry` items.
    template <typename Entry, uint16_t kMaxSize> class EntryList : public List
    {
    public:
        // Adds a new entry to the list or overwrites the oldest entry
        // if list is full. First version returns a pointer to the
        // new `Entry` (for caller to populate). Second version copies
        // the given `aEntry`.
        Entry *AddNewEntry(void) { return &mEntries[Add(kMaxSize, mTimestamps)]; }
        void   AddNewEntry(const Entry &aEntry) { mEntries[Add(kMaxSize, mTimestamps)] = aEntry; }

        void UpdateAgedEntries(void) { List::UpdateAgedEntries(kMaxSize, mTimestamps); }

        const Entry *Iterate(Iterator &aIterator, uint32_t &aEntryAge) const
        {
            uint16_t index;

            return (List::Iterate(kMaxSize, mTimestamps, aIterator, index, aEntryAge) == kErrorNone) ? &mEntries[index]
                                                                                                     : nullptr;
        }

    private:
        Timestamp mTimestamps[kMaxSize];
        Entry     mEntries[kMaxSize];
    };

    // Partial specialization for `kMaxSize` zero.
    template <typename Entry> class EntryList<Entry, 0> : private NonCopyable
    {
    public:
        void         Clear(void) {}
        uint16_t     GetSize(void) const { return 0; }
        Entry *      AddNewEntry(void) { return nullptr; }
        void         AddNewEntry(const Entry &) {}
        const Entry *Iterate(Iterator &, uint32_t &) const { return nullptr; }
        void         RemoveAgedEntries(void) {}
    };

    enum MessageType : uint8_t
    {
        kRxMessage,
        kTxMessage,
    };

    void RecordRxMessage(const Message &aMessage, const Mac::Address &aMacSource)
    {
        RecordMessage(aMessage, aMacSource, kRxMessage);
    }

    void RecordTxMessage(const Message &aMessage, const Mac::Address &aMacDest)
    {
        RecordMessage(aMessage, aMacDest, kTxMessage);
    }

    void        RecordNetworkInfo(void);
    void        RecordMessage(const Message &aMessage, const Mac::Address &aMacAddress, MessageType aType);
    void        RecordNeighborEvent(NeighborTable::Event aEvent, const NeighborTable::EntryInfo &aInfo);
    void        RecordAddressEvent(Ip6::Netif::AddressEvent aEvent, const Ip6::Netif::UnicastAddress &aUnicastAddress);
    void        RecordAddressEvent(Ip6::Netif::AddressEvent            aEvent,
                                   const Ip6::Netif::MulticastAddress &aMulticastAddress,
                                   Ip6::Netif::AddressOrigin           aAddressOrigin);
    void        HandleNotifierEvents(Events aEvents);
    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);

    EntryList<NetworkInfo, kNetInfoListSize>                mNetInfoHistory;
    EntryList<UnicastAddressInfo, kUnicastAddrListSize>     mUnicastAddressHistory;
    EntryList<MulticastAddressInfo, kMulticastAddrListSize> mMulticastAddressHistory;
    EntryList<MessageInfo, kRxListSize>                     mRxHistory;
    EntryList<MessageInfo, kTxListSize>                     mTxHistory;
    EntryList<NeighborInfo, kNeighborListSize>              mNeighborHistory;

    TimerMilli mTimer;
};

} // namespace Utils
} // namespace ot

#endif // OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE

#endif // HISTORY_TRACKER_HPP_
