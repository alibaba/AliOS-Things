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
 *   This file implements IPv6 network interfaces.
 */

#include "netif.hpp"

#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/message.hpp"
#include "net/ip6.hpp"

namespace ot {
namespace Ip6 {

class AddressInfo : public otIp6AddressInfo
{
public:
    explicit AddressInfo(const Netif::UnicastAddress &aAddress)
    {
        mAddress      = &aAddress.mAddress;
        mPrefixLength = aAddress.mPrefixLength;
        mScope        = aAddress.GetScope();
        mPreferred    = aAddress.mPreferred;
    }

    explicit AddressInfo(const Netif::MulticastAddress &aAddress)
    {
        mAddress      = &aAddress.GetAddress();
        mPrefixLength = kMulticastPrefixLength;
        mScope        = aAddress.GetAddress().GetScope();
        mPreferred    = false;
    }

private:
    static constexpr uint8_t kMulticastPrefixLength =
        128; ///< Multicast prefix length used to notify internal address changes.
};

/*
 * Certain fixed multicast addresses are defined as a set of chained (linked-list) constant `otNetifMulticastAddress`
 * entries:
 *
 * LinkLocalAllRouters -> RealmLocalAllRouters -> LinkLocalAll -> RealmLocalAll -> RealmLocalAllMplForwarders -> nullptr
 *
 * All or a portion of the chain is appended to the end of `mMulticastAddresses` linked-list. If the interface is
 * subscribed to all-routers multicast addresses (using `SubscribeAllRoutersMulticast()`) then all the five entries
 * are appended. Otherwise only the last three are appended.
 *
 */

// "ff03::fc"
const otNetifMulticastAddress Netif::kRealmLocalAllMplForwardersMulticastAddress = {
    {{{0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc}}},
    nullptr};

// "ff03::01"
const otNetifMulticastAddress Netif::kRealmLocalAllNodesMulticastAddress = {
    {{{0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}}},
    &Netif::kRealmLocalAllMplForwardersMulticastAddress};

// "ff02::01"
const otNetifMulticastAddress Netif::kLinkLocalAllNodesMulticastAddress = {
    {{{0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}}},
    &Netif::kRealmLocalAllNodesMulticastAddress};

// "ff03::02"
const otNetifMulticastAddress Netif::kRealmLocalAllRoutersMulticastAddress = {
    {{{0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02}}},
    &Netif::kLinkLocalAllNodesMulticastAddress};

// "ff02::02"
const otNetifMulticastAddress Netif::kLinkLocalAllRoutersMulticastAddress = {
    {{{0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02}}},
    &Netif::kRealmLocalAllRoutersMulticastAddress};

//---------------------------------------------------------------------------------------------------------------------
// Netif

Netif::Netif(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mMulticastPromiscuous(false)
    , mAddressCallback(nullptr)
    , mAddressCallbackContext(nullptr)
{
}

bool Netif::IsMulticastSubscribed(const Address &aAddress) const
{
    return mMulticastAddresses.ContainsMatching(aAddress);
}

void Netif::SubscribeAllNodesMulticast(void)
{
    MulticastAddress *tail;
    MulticastAddress &linkLocalAllNodesAddress =
        static_cast<MulticastAddress &>(AsNonConst(kLinkLocalAllNodesMulticastAddress));

    VerifyOrExit(!mMulticastAddresses.Contains(linkLocalAllNodesAddress));

    // Append the fixed chain of three multicast addresses to the
    // tail of the list:
    //
    //    LinkLocalAll -> RealmLocalAll -> RealmLocalAllMpl.

    tail = mMulticastAddresses.GetTail();

    if (tail == nullptr)
    {
        mMulticastAddresses.SetHead(&linkLocalAllNodesAddress);
    }
    else
    {
        tail->SetNext(&linkLocalAllNodesAddress);
    }

    Get<Notifier>().Signal(kEventIp6MulticastSubscribed);

#if !OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
    VerifyOrExit(mAddressCallback != nullptr);
#endif

    for (const MulticastAddress *entry = &linkLocalAllNodesAddress; entry; entry = entry->GetNext())
    {
#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
        Get<Utils::HistoryTracker>().RecordAddressEvent(kAddressAdded, *entry, kOriginThread);

        if (mAddressCallback != nullptr)
#endif
        {
            AddressInfo addressInfo(*entry);

            mAddressCallback(&addressInfo, kAddressAdded, mAddressCallbackContext);
        }
    }

exit:
    return;
}

void Netif::UnsubscribeAllNodesMulticast(void)
{
    MulticastAddress *      prev;
    const MulticastAddress &linkLocalAllNodesAddress =
        static_cast<MulticastAddress &>(AsNonConst(kLinkLocalAllNodesMulticastAddress));

    // The tail of multicast address linked list contains the
    // fixed addresses. Search if LinkLocalAll is present
    // in the list and find entry before it.
    //
    //    LinkLocalAll -> RealmLocalAll -> RealmLocalAllMpl.

    SuccessOrExit(mMulticastAddresses.Find(linkLocalAllNodesAddress, prev));

    // This method MUST be called after `UnsubscribeAllRoutersMulticast().
    // Verify this by checking the chain at the end of the list only
    // contains three entries and not the five fixed addresses (check that
    // `prev` entry before `LinkLocalAll` is not `RealmLocalRouters`):
    //
    //    LinkLocalAllRouters -> RealmLocalAllRouters -> LinkLocalAll
    //         -> RealmLocalAll -> RealmLocalAllMpl.

    OT_ASSERT(prev != static_cast<MulticastAddress *>(AsNonConst(&kRealmLocalAllRoutersMulticastAddress)));

    if (prev == nullptr)
    {
        mMulticastAddresses.Clear();
    }
    else
    {
        prev->SetNext(nullptr);
    }

    Get<Notifier>().Signal(kEventIp6MulticastUnsubscribed);

#if !OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
    VerifyOrExit(mAddressCallback != nullptr);
#endif

    for (const MulticastAddress *entry = &linkLocalAllNodesAddress; entry; entry = entry->GetNext())
    {
#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
        Get<Utils::HistoryTracker>().RecordAddressEvent(kAddressRemoved, *entry, kOriginThread);

        if (mAddressCallback != nullptr)
#endif
        {
            AddressInfo addressInfo(*entry);

            mAddressCallback(&addressInfo, kAddressRemoved, mAddressCallbackContext);
        }
    }

exit:
    return;
}

void Netif::SubscribeAllRoutersMulticast(void)
{
    Error             error = kErrorNone;
    MulticastAddress *prev  = nullptr;
    MulticastAddress &linkLocalAllRoutersAddress =
        static_cast<MulticastAddress &>(AsNonConst(kLinkLocalAllRoutersMulticastAddress));
    MulticastAddress &linkLocalAllNodesAddress =
        static_cast<MulticastAddress &>(AsNonConst(kLinkLocalAllNodesMulticastAddress));
    MulticastAddress &realmLocalAllRoutersAddress =
        static_cast<MulticastAddress &>(AsNonConst(kRealmLocalAllRoutersMulticastAddress));

    error = mMulticastAddresses.Find(linkLocalAllNodesAddress, prev);

    // This method MUST be called after `SubscribeAllNodesMulticast()`
    // Ensure that the `LinkLocalAll` was found on the list.

    OT_ASSERT(error == kErrorNone);
    OT_UNUSED_VARIABLE(error);

    // The tail of multicast address linked list contains the
    // fixed addresses. We either have a chain of five addresses
    //
    //    LinkLocalAllRouters -> RealmLocalAllRouters ->
    //        LinkLocalAll -> RealmLocalAll -> RealmLocalAllMpl.
    //
    // or just the last three addresses
    //
    //    LinkLocalAll -> RealmLocalAll -> RealmLocalAllMpl.
    //
    // If the previous entry behind `LinkLocalAll` is
    // `RealmLocalAllRouters` then all five addresses are on
    // the list already.

    VerifyOrExit(prev != &realmLocalAllRoutersAddress);

    if (prev == nullptr)
    {
        mMulticastAddresses.SetHead(&linkLocalAllRoutersAddress);
    }
    else
    {
        prev->SetNext(&linkLocalAllRoutersAddress);
    }

    Get<Notifier>().Signal(kEventIp6MulticastSubscribed);

#if !OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
    VerifyOrExit(mAddressCallback != nullptr);
#endif

    for (const MulticastAddress *entry = &linkLocalAllRoutersAddress; entry != &linkLocalAllNodesAddress;
         entry                         = entry->GetNext())
    {
#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
        Get<Utils::HistoryTracker>().RecordAddressEvent(kAddressAdded, *entry, kOriginThread);

        if (mAddressCallback != nullptr)
#endif
        {
            AddressInfo addressInfo(*entry);

            mAddressCallback(&addressInfo, kAddressAdded, mAddressCallbackContext);
        }
    }

exit:
    return;
}

void Netif::UnsubscribeAllRoutersMulticast(void)
{
    MulticastAddress *prev;
    MulticastAddress &linkLocalAllRoutersAddress =
        static_cast<MulticastAddress &>(AsNonConst(kLinkLocalAllRoutersMulticastAddress));
    MulticastAddress &linkLocalAllNodesAddress =
        static_cast<MulticastAddress &>(AsNonConst(kLinkLocalAllNodesMulticastAddress));

    // The tail of multicast address linked list contains the
    // fixed addresses. We check for the chain of five addresses:
    //
    //    LinkLocalAllRouters -> RealmLocalAllRouters ->
    //        LinkLocalAll -> RealmLocalAll -> RealmLocalAllMpl.
    //
    // If found, we then replace the entry behind `LinkLocalAllRouters`
    // to point to `LinkLocalAll` instead (so that tail contains the
    // three fixed addresses at end of the chain).

    SuccessOrExit(mMulticastAddresses.Find(linkLocalAllRoutersAddress, prev));

    if (prev == nullptr)
    {
        mMulticastAddresses.SetHead(&linkLocalAllNodesAddress);
    }
    else
    {
        prev->SetNext(&linkLocalAllNodesAddress);
    }

    Get<Notifier>().Signal(kEventIp6MulticastUnsubscribed);

#if !OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
    VerifyOrExit(mAddressCallback != nullptr);
#endif

    for (const MulticastAddress *entry = &linkLocalAllRoutersAddress; entry != &linkLocalAllNodesAddress;
         entry                         = entry->GetNext())
    {
#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
        Get<Utils::HistoryTracker>().RecordAddressEvent(kAddressRemoved, *entry, kOriginThread);

        if (mAddressCallback != nullptr)
#endif
        {
            AddressInfo addressInfo(*entry);

            mAddressCallback(&addressInfo, kAddressRemoved, mAddressCallbackContext);
        }
    }

exit:
    return;
}

bool Netif::IsMulticastAddressExternal(const MulticastAddress &aAddress) const
{
    return mExtMulticastAddressPool.IsPoolEntry(static_cast<const ExternalMulticastAddress &>(aAddress));
}

void Netif::SubscribeMulticast(MulticastAddress &aAddress)
{
    SuccessOrExit(mMulticastAddresses.Add(aAddress));

    Get<Notifier>().Signal(kEventIp6MulticastSubscribed);

#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
    Get<Utils::HistoryTracker>().RecordAddressEvent(kAddressAdded, aAddress, kOriginThread);
#endif

    if (mAddressCallback != nullptr)
    {
        AddressInfo addressInfo(aAddress);

        mAddressCallback(&addressInfo, kAddressAdded, mAddressCallbackContext);
    }

exit:
    return;
}

void Netif::UnsubscribeMulticast(const MulticastAddress &aAddress)
{
    SuccessOrExit(mMulticastAddresses.Remove(aAddress));

    Get<Notifier>().Signal(kEventIp6MulticastUnsubscribed);

#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
    Get<Utils::HistoryTracker>().RecordAddressEvent(kAddressRemoved, aAddress, kOriginThread);
#endif

    if (mAddressCallback != nullptr)
    {
        AddressInfo addressInfo(aAddress);

        mAddressCallback(&addressInfo, kAddressRemoved, mAddressCallbackContext);
    }

exit:
    return;
}

Error Netif::SubscribeExternalMulticast(const Address &aAddress)
{
    Error             error = kErrorNone;
    MulticastAddress &linkLocalAllRoutersAddress =
        static_cast<MulticastAddress &>(AsNonConst(kLinkLocalAllRoutersMulticastAddress));
    ExternalMulticastAddress *entry;

    VerifyOrExit(aAddress.IsMulticast(), error = kErrorInvalidArgs);
    VerifyOrExit(!IsMulticastSubscribed(aAddress), error = kErrorAlready);

    // Check that the address is not one of the fixed addresses:
    // LinkLocalAllRouters -> RealmLocalAllRouters -> LinkLocalAllNodes
    // -> RealmLocalAllNodes -> RealmLocalAllMpl.

    for (const MulticastAddress *cur = &linkLocalAllRoutersAddress; cur; cur = cur->GetNext())
    {
        VerifyOrExit(cur->GetAddress() != aAddress, error = kErrorInvalidArgs);
    }

    entry = mExtMulticastAddressPool.Allocate();
    VerifyOrExit(entry != nullptr, error = kErrorNoBufs);

    entry->mAddress = aAddress;
#if OPENTHREAD_CONFIG_MLR_ENABLE
    entry->mMlrState = kMlrStateToRegister;
#endif
    mMulticastAddresses.Push(*entry);

#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
    Get<Utils::HistoryTracker>().RecordAddressEvent(kAddressAdded, *entry, kOriginManual);
#endif

    Get<Notifier>().Signal(kEventIp6MulticastSubscribed);

exit:
    return error;
}

Error Netif::UnsubscribeExternalMulticast(const Address &aAddress)
{
    Error             error = kErrorNone;
    MulticastAddress *entry;
    MulticastAddress *prev;

    entry = mMulticastAddresses.FindMatching(aAddress, prev);
    VerifyOrExit(entry != nullptr, error = kErrorNotFound);

    VerifyOrExit(IsMulticastAddressExternal(*entry), error = kErrorInvalidArgs);

    mMulticastAddresses.PopAfter(prev);

#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
    Get<Utils::HistoryTracker>().RecordAddressEvent(kAddressRemoved, *entry, kOriginManual);
#endif

    mExtMulticastAddressPool.Free(static_cast<ExternalMulticastAddress &>(*entry));

    Get<Notifier>().Signal(kEventIp6MulticastUnsubscribed);

exit:
    return error;
}

void Netif::UnsubscribeAllExternalMulticastAddresses(void)
{
    MulticastAddress *next;

    for (MulticastAddress *entry = mMulticastAddresses.GetHead(); entry != nullptr; entry = next)
    {
        next = entry->GetNext();

        if (IsMulticastAddressExternal(*entry))
        {
            IgnoreError(UnsubscribeExternalMulticast(entry->GetAddress()));
        }
    }
}

void Netif::SetAddressCallback(otIp6AddressCallback aCallback, void *aCallbackContext)
{
    mAddressCallback        = aCallback;
    mAddressCallbackContext = aCallbackContext;
}

void Netif::AddUnicastAddress(UnicastAddress &aAddress)
{
    SuccessOrExit(mUnicastAddresses.Add(aAddress));

    Get<Notifier>().Signal(aAddress.mRloc ? kEventThreadRlocAdded : kEventIp6AddressAdded);

#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
    Get<Utils::HistoryTracker>().RecordAddressEvent(kAddressAdded, aAddress);
#endif

    if (mAddressCallback != nullptr)
    {
        AddressInfo addressInfo(aAddress);

        mAddressCallback(&addressInfo, kAddressAdded, mAddressCallbackContext);
    }

exit:
    return;
}

void Netif::RemoveUnicastAddress(const UnicastAddress &aAddress)
{
    SuccessOrExit(mUnicastAddresses.Remove(aAddress));

    Get<Notifier>().Signal(aAddress.mRloc ? kEventThreadRlocRemoved : kEventIp6AddressRemoved);

#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
    Get<Utils::HistoryTracker>().RecordAddressEvent(kAddressRemoved, aAddress);
#endif

    if (mAddressCallback != nullptr)
    {
        AddressInfo addressInfo(aAddress);

        mAddressCallback(&addressInfo, kAddressRemoved, mAddressCallbackContext);
    }

exit:
    return;
}

Error Netif::AddExternalUnicastAddress(const UnicastAddress &aAddress)
{
    Error           error = kErrorNone;
    UnicastAddress *entry;

    VerifyOrExit(!aAddress.GetAddress().IsMulticast(), error = kErrorInvalidArgs);

    entry = mUnicastAddresses.FindMatching(aAddress.GetAddress());

    if (entry != nullptr)
    {
        VerifyOrExit(IsUnicastAddressExternal(*entry), error = kErrorAlready);

        entry->mPrefixLength  = aAddress.mPrefixLength;
        entry->mAddressOrigin = aAddress.mAddressOrigin;
        entry->mPreferred     = aAddress.mPreferred;
        entry->mValid         = aAddress.mValid;
        ExitNow();
    }

    VerifyOrExit(!aAddress.GetAddress().IsLinkLocal(), error = kErrorInvalidArgs);

    entry = mExtUnicastAddressPool.Allocate();
    VerifyOrExit(entry != nullptr, error = kErrorNoBufs);

    *entry = aAddress;
    mUnicastAddresses.Push(*entry);

#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
    Get<Utils::HistoryTracker>().RecordAddressEvent(kAddressAdded, *entry);
#endif

    Get<Notifier>().Signal(kEventIp6AddressAdded);

exit:
    return error;
}

Error Netif::RemoveExternalUnicastAddress(const Address &aAddress)
{
    Error           error = kErrorNone;
    UnicastAddress *entry;
    UnicastAddress *prev;

    entry = mUnicastAddresses.FindMatching(aAddress, prev);
    VerifyOrExit(entry != nullptr, error = kErrorNotFound);

    VerifyOrExit(IsUnicastAddressExternal(*entry), error = kErrorInvalidArgs);

    mUnicastAddresses.PopAfter(prev);

#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
    Get<Utils::HistoryTracker>().RecordAddressEvent(kAddressRemoved, *entry);
#endif

    mExtUnicastAddressPool.Free(*entry);
    Get<Notifier>().Signal(kEventIp6AddressRemoved);

exit:
    return error;
}

void Netif::RemoveAllExternalUnicastAddresses(void)
{
    UnicastAddress *next;

    for (UnicastAddress *entry = mUnicastAddresses.GetHead(); entry != nullptr; entry = next)
    {
        next = entry->GetNext();

        if (IsUnicastAddressExternal(*entry))
        {
            IgnoreError(RemoveExternalUnicastAddress(entry->GetAddress()));
        }
    }
}

bool Netif::HasUnicastAddress(const Address &aAddress) const
{
    return mUnicastAddresses.ContainsMatching(aAddress);
}

bool Netif::IsUnicastAddressExternal(const UnicastAddress &aAddress) const
{
    return mExtUnicastAddressPool.IsPoolEntry(aAddress);
}

//---------------------------------------------------------------------------------------------------------------------
// Netif::UnicastAddress

void Netif::UnicastAddress::InitAsThreadOrigin(bool aPreferred)
{
    Clear();
    mPrefixLength  = NetworkPrefix::kLength;
    mAddressOrigin = kOriginThread;
    mPreferred     = aPreferred;
    mValid         = true;
}

void Netif::UnicastAddress::InitAsThreadOriginRealmLocalScope(void)
{
    InitAsThreadOrigin();
    SetScopeOverride(Address::kRealmLocalScope);
}

void Netif::UnicastAddress::InitAsThreadOriginGlobalScope(void)
{
    Clear();
    mAddressOrigin = kOriginThread;
    mValid         = true;
    SetScopeOverride(Address::kGlobalScope);
}

void Netif::UnicastAddress::InitAsSlaacOrigin(uint8_t aPrefixLength, bool aPreferred)
{
    Clear();
    mPrefixLength  = aPrefixLength;
    mAddressOrigin = kOriginSlaac;
    mPreferred     = aPreferred;
    mValid         = true;
}

//---------------------------------------------------------------------------------------------------------------------
// Netif::ExternalMulticastAddress::Iterator

Netif::ExternalMulticastAddress::Iterator::Iterator(const Netif &aNetif, Address::TypeFilter aFilter)
    : ItemPtrIterator(nullptr)
    , mNetif(aNetif)
    , mFilter(aFilter)
{
    AdvanceFrom(mNetif.GetMulticastAddresses().GetHead());
}

void Netif::ExternalMulticastAddress::Iterator::AdvanceFrom(const MulticastAddress *aAddr)
{
    while (aAddr != nullptr &&
           !(mNetif.IsMulticastAddressExternal(*aAddr) && aAddr->GetAddress().MatchesFilter(mFilter)))
    {
        aAddr = aAddr->GetNext();
    }

    mItem = AsNonConst(static_cast<const ExternalMulticastAddress *>(aAddr));
}

} // namespace Ip6
} // namespace ot
