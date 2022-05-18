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
 *   This file implements the Network Data Publisher.
 *
 */

#include "network_data_publisher.hpp"

#if OPENTHREAD_CONFIG_NETDATA_PUBLISHER_ENABLE

#include "common/code_utils.hpp"
#include "common/const_cast.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/random.hpp"
#include "thread/network_data_local.hpp"
#include "thread/network_data_service.hpp"

namespace ot {
namespace NetworkData {

//---------------------------------------------------------------------------------------------------------------------
// Publisher

Publisher::Publisher(Instance &aInstance)
    : InstanceLocator(aInstance)
#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    , mDnsSrpServiceEntry(aInstance)
#endif
#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
    , mPrefixCallback(nullptr)
    , mPrefixCallbackContext(nullptr)
#endif
    , mTimer(aInstance, Publisher::HandleTimer)
{
#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
    // Since the `PrefixEntry` type is used in an array,
    // we cannot use a constructor with an argument (e.g.,
    // we cannot use `InstacneLocator`) so we use
    // `IntanceLocatorInit`  and `Init()` the entries one
    // by one.

    for (PrefixEntry &entry : mPrefixEntries)
    {
        entry.Init(aInstance);
    }
#endif
}

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

void Publisher::SetPrefixCallback(PrefixCallback aCallback, void *aContext)
{
    mPrefixCallback        = aCallback;
    mPrefixCallbackContext = aContext;
}

Error Publisher::PublishOnMeshPrefix(const OnMeshPrefixConfig &aConfig)
{
    Error        error;
    PrefixEntry *entry;

    VerifyOrExit(aConfig.IsValid(GetInstance()), error = kErrorInvalidArgs);
    VerifyOrExit(aConfig.mStable, error = kErrorInvalidArgs);

    SuccessOrExit(error = AllocatePrefixEntry(aConfig.GetPrefix(), entry));
    entry->Publish(aConfig);

exit:
    return error;
}

Error Publisher::PublishExternalRoute(const ExternalRouteConfig &aConfig)
{
    Error        error;
    PrefixEntry *entry;

    VerifyOrExit(aConfig.IsValid(GetInstance()), error = kErrorInvalidArgs);
    VerifyOrExit(aConfig.mStable, error = kErrorInvalidArgs);

    SuccessOrExit(error = AllocatePrefixEntry(aConfig.GetPrefix(), entry));
    entry->Publish(aConfig);

exit:
    return error;
}

bool Publisher::IsPrefixAdded(const Ip6::Prefix &aPrefix) const
{
    bool               isAdded = false;
    const PrefixEntry *entry;

    entry = FindMatchingPrefixEntry(aPrefix);
    VerifyOrExit(entry != nullptr);

    isAdded = entry->IsAdded();

exit:
    return isAdded;
}

Error Publisher::UnpublishPrefix(const Ip6::Prefix &aPrefix)
{
    Error        error = kErrorNone;
    PrefixEntry *entry;

    entry = FindMatchingPrefixEntry(aPrefix);
    VerifyOrExit(entry != nullptr, error = kErrorNotFound);

    entry->Unpublish();

exit:
    return error;
}

Error Publisher::AllocatePrefixEntry(const Ip6::Prefix &aPrefix, PrefixEntry *&aEntry)
{
    Error error = kErrorNoBufs;

    VerifyOrExit(FindMatchingPrefixEntry(aPrefix) == nullptr, error = kErrorAlready);

    for (PrefixEntry &entry : mPrefixEntries)
    {
        if (!entry.IsInUse())
        {
            aEntry = &entry;
            ExitNow(error = kErrorNone);
        }
    }

exit:
    return error;
}

Publisher::PrefixEntry *Publisher::FindMatchingPrefixEntry(const Ip6::Prefix &aPrefix)
{
    return AsNonConst(AsConst(this)->FindMatchingPrefixEntry(aPrefix));
}

const Publisher::PrefixEntry *Publisher::FindMatchingPrefixEntry(const Ip6::Prefix &aPrefix) const
{
    const PrefixEntry *prefixEntry = nullptr;

    for (const PrefixEntry &entry : mPrefixEntries)
    {
        if (entry.IsInUse() && entry.Matches(aPrefix))
        {
            prefixEntry = &entry;
            break;
        }
    }

    return prefixEntry;
}

bool Publisher::IsAPrefixEntry(const Entry &aEntry) const
{
    return (&mPrefixEntries[0] <= &aEntry) && (&aEntry < OT_ARRAY_END(mPrefixEntries));
}

void Publisher::NotifyPrefixEntryChange(Event aEvent, const Ip6::Prefix &aPrefix) const
{
    if (mPrefixCallback != nullptr)
    {
        mPrefixCallback(static_cast<otNetDataPublisherEvent>(aEvent), &aPrefix, mPrefixCallbackContext);
    }
}

#endif // OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

void Publisher::HandleNotifierEvents(Events aEvents)
{
    OT_UNUSED_VARIABLE(aEvents);

    bool registerWithLeader = false;

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    if (mDnsSrpServiceEntry.HandleNotifierEvents(aEvents))
    {
        registerWithLeader = true;
    }
#endif

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
    for (PrefixEntry &entry : mPrefixEntries)
    {
        entry.HandleNotifierEvents(aEvents);
    }
#endif

    if (registerWithLeader)
    {
        Get<Notifier>().HandleServerDataUpdated();
    }
}

void Publisher::HandleTimer(Timer &aTimer)
{
    aTimer.Get<Publisher>().HandleTimer();
}

void Publisher::HandleTimer(void)
{
    bool registerWithLeader = false;

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    if (mDnsSrpServiceEntry.HandleTimer())
    {
        registerWithLeader = true;
    }
#endif

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
    for (PrefixEntry &entry : mPrefixEntries)
    {
        if (entry.HandleTimer())
        {
            registerWithLeader = true;
        }
    }
#endif

    if (registerWithLeader)
    {
        Get<Notifier>().HandleServerDataUpdated();
    }
}

//---------------------------------------------------------------------------------------------------------------------
// Publisher::Entry

void Publisher::Entry::SetState(State aState)
{
    VerifyOrExit(mState != aState);

    otLogInfoNetData("Publisher: %s - State: %s -> %s", ToString(/* aIncludeState */ false).AsCString(),
                     StateToString(mState), StateToString(aState));
    mState = aState;

exit:
    return;
}

bool Publisher::Entry::IsPreferred(uint16_t aRloc16) const
{
    // Indicates whether or not an entry from `aRloc16` is preferred
    // over our entry (based on our RLOC). We prefer an entry from a
    // router over an entry from an end-device (e.g., a REED). If both
    // are the same type, then the one with smaller RLOC16 is preferred.

    bool isOtherRouter = Mle::Mle::IsActiveRouter(aRloc16);

    return (Get<Mle::Mle>().IsRouterOrLeader() == isOtherRouter) ? (aRloc16 < Get<Mle::Mle>().GetRloc16())
                                                                 : isOtherRouter;
}

void Publisher::Entry::UpdateState(uint8_t aNumEntries, uint8_t aNumPreferredEntries, uint8_t aDesiredNumEntries)
{
    // This method uses the info about number existing entries (total
    // and preferred) in Network Data along with the desired number of
    // entries we aim to have in the Network Data to decide whether or
    // not to take any action (add or remove our entry).

    otLogInfoNetData("Publisher: %s in netdata - total:%d, preferred:%d, desired:%d", ToString().AsCString(),
                     aNumEntries, aNumPreferredEntries, aDesiredNumEntries);

    switch (GetState())
    {
    case kNoEntry:
        break;

    case kToAdd:
        // Our entry is ready to be added. If there are too few existing
        // entries, we start adding our entry (start the timer with a
        // random delay before adding the entry).

        if (aNumEntries < aDesiredNumEntries)
        {
            mUpdateTime = TimerMilli::GetNow() + Random::NonCrypto::GetUint32InRange(1, kMaxDelayToAdd);
            SetState(kAdding);
            Get<Publisher>().GetTimer().FireAtIfEarlier(mUpdateTime);
            LogUpdateTime();
        }
        break;

    case kAdding:
        // Our entry is being added (waiting time before we add). If we
        // now see that there are enough entries, we stop adding the
        // entry.

        if (aNumEntries >= aDesiredNumEntries)
        {
            SetState(kToAdd);
        }
        break;

    case kAdded:
        // Our entry is already added in the Network Data. If there are
        // enough entries, do nothing and keep monitoring. If we see now
        // that there are too many entries, we start removing our entry
        // after a random delay time. If our entry itself is preferred
        // over other entries (indicated by `aNumPreferredEntries <
        // aDesiredNumEntries`) we add an extra delay before removing
        // the entry. This gives higher chance for a non-preferred
        // entry from another device to be removed before our entry.

        if (aNumEntries > aDesiredNumEntries)
        {
            mUpdateTime = TimerMilli::GetNow() + Random::NonCrypto::GetUint32InRange(1, kMaxDelayToRemove);

            if (aNumPreferredEntries < aDesiredNumEntries)
            {
                mUpdateTime += kExtraDelayToRemovePeferred;
            }

            SetState(kRemoving);
            Get<Publisher>().GetTimer().FireAtIfEarlier(mUpdateTime);
            LogUpdateTime();
        }
        break;

    case kRemoving:
        // Our entry is being removed (wait time before remove). If we
        // now see that there are enough or too few entries, we stop
        // removing our entry.

        if (aNumEntries <= aDesiredNumEntries)
        {
            SetState(kAdded);
        }
        break;
    }
}

bool Publisher::Entry::HandleTimer(void)
{
    bool registerWithLeader = false;

    // Timer is used to delay adding/removing the entry. If we have
    // reached `mUpdateTime` add or remove the entry. Otherwise,
    // restart the timer (note that timer can be shared between
    // different published entries). This method returns a `bool`
    // indicating whether or not anything in local Network Data got
    // changed so to notify the leader and register the changes.

    VerifyOrExit((GetState() == kAdding) || (GetState() == kRemoving));

    if (mUpdateTime <= TimerMilli::GetNow())
    {
        registerWithLeader = (GetState() == kAdding) ? Add() : Remove(/* aNextState */ kToAdd);
    }
    else
    {
        Get<Publisher>().GetTimer().FireAtIfEarlier(mUpdateTime);
    }

exit:
    return registerWithLeader;
}

bool Publisher::Entry::Add(void)
{
    bool registerWithLeader = false;

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    if (Get<Publisher>().IsADnsSrpServiceEntry(*this))
    {
        registerWithLeader = static_cast<DnsSrpServiceEntry *>(this)->Add();
        ExitNow();
    }
#endif

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
    if (Get<Publisher>().IsAPrefixEntry(*this))
    {
        registerWithLeader = static_cast<PrefixEntry *>(this)->Add();
        ExitNow();
    }
#endif

exit:
    return registerWithLeader;
}

bool Publisher::Entry::Remove(State aNextState)
{
    bool registerWithLeader = false;

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    if (Get<Publisher>().IsADnsSrpServiceEntry(*this))
    {
        registerWithLeader = static_cast<DnsSrpServiceEntry *>(this)->Remove(aNextState);
        ExitNow();
    }
#endif

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
    if (Get<Publisher>().IsAPrefixEntry(*this))
    {
        registerWithLeader = static_cast<PrefixEntry *>(this)->Remove(aNextState);
        ExitNow();
    }
#endif

exit:
    return registerWithLeader;
}

Publisher::Entry::InfoString Publisher::Entry::ToString(bool aIncludeState) const
{
    InfoString string;

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    if (Get<Publisher>().IsADnsSrpServiceEntry(*this))
    {
        string.Append("DNS/SRP service");
        ExitNow();
    }
#endif

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
    if (Get<Publisher>().IsAPrefixEntry(*this))
    {
        const PrefixEntry &prefixEntry = *static_cast<const PrefixEntry *>(this);

        switch (prefixEntry.mType)
        {
        case PrefixEntry::kTypeOnMeshPrefix:
            string.Append("OnMeshPrefix ");
            break;

        case PrefixEntry::kTypeExternalRoute:
            string.Append("ExternalRoute ");
            break;
        }

        string.Append(prefixEntry.mPrefix.ToString().AsCString());
        ExitNow();
    }
#endif

exit:
    if (aIncludeState)
    {
        string.Append(" (state:%s)", StateToString(GetState()));
    }

    return string;
}

void Publisher::Entry::LogUpdateTime(void) const
{
    otLogInfoNetData("Publisher: %s - update in %u msec", ToString().AsCString(), mUpdateTime - TimerMilli::GetNow());
}

const char *Publisher::Entry::StateToString(State aState)
{
    static const char *const kStateStrings[] = {
        "NoEntry",  // (0) kNoEntry
        "ToAdd",    // (1) kToAdd
        "Adding",   // (2) kAdding
        "Added",    // (3) kAdded
        "Removing", // (4) kRemoving
    };

    static_assert(0 == kNoEntry, "kNoEntry value is not correct");
    static_assert(1 == kToAdd, "kToAdd value is not correct");
    static_assert(2 == kAdding, "kAdding value is not correct");
    static_assert(3 == kAdded, "kAdded value is not correct");
    static_assert(4 == kRemoving, "kRemoving value is not correct");

    return kStateStrings[aState];
}

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE

//---------------------------------------------------------------------------------------------------------------------
// Publisher::DnsSrpServiceEntry

Publisher::DnsSrpServiceEntry::DnsSrpServiceEntry(Instance &aInstance)
    : mCallback(nullptr)
    , mCallbackContext(nullptr)
{
    Init(aInstance);
}

void Publisher::DnsSrpServiceEntry::SetCallback(DnsSrpServiceCallback aCallback, void *aContext)
{
    mCallback        = aCallback;
    mCallbackContext = aContext;
}

void Publisher::DnsSrpServiceEntry::PublishAnycast(uint8_t aSequenceNumber)
{
    otLogInfoNetData("Publisher: Publishing DNS/SRP service anycast (seq-num:%d)", aSequenceNumber);
    Publish(Info::InfoAnycast(aSequenceNumber));
}

void Publisher::DnsSrpServiceEntry::PublishUnicast(const Ip6::Address &aAddress, uint16_t aPort)
{
    otLogInfoNetData("Publisher: Publishing DNS/SRP service unicast (%s, port:%d)", aAddress.ToString().AsCString(),
                     aPort);
    Publish(Info::InfoUnicast(kTypeUnicast, aAddress, aPort));
}

void Publisher::DnsSrpServiceEntry::PublishUnicast(uint16_t aPort)
{
    otLogInfoNetData("Publisher: Publishing DNS/SRP service unicast (ml-eid, port:%d)", aPort);
    Publish(Info::InfoUnicast(kTypeUnicastMeshLocalEid, Get<Mle::Mle>().GetMeshLocal64(), aPort));
}

void Publisher::DnsSrpServiceEntry::Publish(const Info &aInfo)
{
    bool registerWithLeader = false;

    if (GetState() != kNoEntry)
    {
        if (aInfo == mInfo)
        {
            otLogInfoNetData("Publisher: %s is already being published", ToString().AsCString());
            ExitNow();
        }

        registerWithLeader = Remove(/* aNextState */ kNoEntry);
    }

    mInfo = aInfo;
    SetState(kToAdd);

    Process();

    if (registerWithLeader)
    {
        Get<Notifier>().HandleServerDataUpdated();
    }

exit:
    return;
}

void Publisher::DnsSrpServiceEntry::Unpublish(void)
{
    bool registerWithLeader;

    otLogInfoNetData("Publisher: Unpublishing DNS/SRP service");

    registerWithLeader = Remove(/* aNextState */ kNoEntry);

    if (registerWithLeader)
    {
        Get<Notifier>().HandleServerDataUpdated();
    }
}

bool Publisher::DnsSrpServiceEntry::HandleNotifierEvents(Events aEvents)
{
    bool registerWithLeader = false;

    if ((GetType() == kTypeUnicastMeshLocalEid) && aEvents.Contains(kEventThreadMeshLocalAddrChanged))
    {
        mInfo.SetAddress(Get<Mle::Mle>().GetMeshLocal64());

        if (GetState() == kAdded)
        {
            // If the entry is already added, we need to update it
            // so we remove it and add it back immediately with
            // the new mesh-local address.

            Remove(/* aNextState */ kAdding);
            Add();
            registerWithLeader = true;
        }
    }

    if (aEvents.ContainsAny(kEventThreadNetdataChanged | kEventThreadRoleChanged))
    {
        Process();
    }

    return registerWithLeader;
}

bool Publisher::DnsSrpServiceEntry::Add(void)
{
    // Adds the service entry to the network data.

    bool registerWithLeader = false;

    switch (GetType())
    {
    case kTypeAnycast:
        SuccessOrExit(Get<Service::Manager>().Add<Service::DnsSrpAnycast>(
            Service::DnsSrpAnycast::ServiceData(mInfo.GetSequenceNumber())));
        break;

    case kTypeUnicast:
        SuccessOrExit(Get<Service::Manager>().Add<Service::DnsSrpUnicast>(
            Service::DnsSrpUnicast::ServiceData(mInfo.GetAddress(), mInfo.GetPort())));
        break;

    case kTypeUnicastMeshLocalEid:
        SuccessOrExit(Get<Service::Manager>().Add<Service::DnsSrpUnicast>(
            Service::DnsSrpUnicast::ServerData(mInfo.GetAddress(), mInfo.GetPort())));
        break;
    }

    registerWithLeader = true;
    SetState(kAdded);
    Notify(kEventEntryAdded);

exit:
    return registerWithLeader;
}

bool Publisher::DnsSrpServiceEntry::Remove(State aNextState)
{
    // Removes the service entry from network data (if it was added).

    bool registerWithLeader = false;

    VerifyOrExit((GetState() == kAdded) || (GetState() == kRemoving));

    switch (GetType())
    {
    case kTypeAnycast:
        SuccessOrExit(Get<Service::Manager>().Remove<Service::DnsSrpAnycast>(
            Service::DnsSrpAnycast::ServiceData(mInfo.GetSequenceNumber())));
        break;

    case kTypeUnicast:
        SuccessOrExit(Get<Service::Manager>().Remove<Service::DnsSrpUnicast>(
            Service::DnsSrpUnicast::ServiceData(mInfo.GetAddress(), mInfo.GetPort())));
        break;

    case kTypeUnicastMeshLocalEid:
        SuccessOrExit(Get<Service::Manager>().Remove<Service::DnsSrpUnicast>());
        break;
    }

    registerWithLeader = true;
    Notify(kEventEntryRemoved);

exit:
    SetState(aNextState);
    return registerWithLeader;
}

void Publisher::DnsSrpServiceEntry::Notify(Event aEvent) const
{
#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
    Get<Srp::Server>().HandleNetDataPublisherEvent(aEvent);
#endif

    if (mCallback != nullptr)
    {
        mCallback(static_cast<otNetDataPublisherEvent>(aEvent), mCallbackContext);
    }
}

void Publisher::DnsSrpServiceEntry::Process(void)
{
    // This method checks the entries currently present in Network Data
    // based on which it then decides whether or not take action
    // (add/remove or keep monitoring).

    uint8_t numEntries          = 0;
    uint8_t numPreferredEntries = 0;
    uint8_t desiredNumEntries   = 0;

    // Do not make any changes if device is not attached, and wait
    // for role change event.
    VerifyOrExit(Get<Mle::Mle>().IsAttached());

    VerifyOrExit(GetState() != kNoEntry);

    switch (GetType())
    {
    case kTypeAnycast:
        CountAnycastEntries(numEntries, numPreferredEntries);
        desiredNumEntries = kDesiredNumAnycast;
        break;

    case kTypeUnicast:
    case kTypeUnicastMeshLocalEid:
        CountUnicastEntries(numEntries, numPreferredEntries);
        desiredNumEntries = kDesiredNumUnicast;
        break;
    }

    UpdateState(numEntries, numPreferredEntries, desiredNumEntries);

exit:
    return;
}

void Publisher::DnsSrpServiceEntry::CountAnycastEntries(uint8_t &aNumEntries, uint8_t &aNumPreferredEntries) const
{
    // Count the number of matching "DNS/SRP Anycast" service entries
    // in the Network Data (the match requires the entry to use same
    // "sequence number" value). We prefer the entries associated with
    // smaller RLCO16.

    Service::DnsSrpAnycast::ServiceData serviceData(mInfo.GetSequenceNumber());
    const ServiceTlv *                  serviceTlv = nullptr;
    ServiceData                         data;

    data.Init(&serviceData, serviceData.GetLength());

    while ((serviceTlv = Get<Leader>().FindNextThreadService(serviceTlv, data, NetworkData::kServicePrefixMatch)) !=
           nullptr)
    {
        TlvIterator      subTlvIterator(*serviceTlv);
        const ServerTlv *serverSubTlv;

        while ((serverSubTlv = subTlvIterator.Iterate<ServerTlv>()) != nullptr)
        {
            aNumEntries++;

            if (IsPreferred(serverSubTlv->GetServer16()))
            {
                aNumPreferredEntries++;
            }
        }
    }
}

void Publisher::DnsSrpServiceEntry::CountUnicastEntries(uint8_t &aNumEntries, uint8_t &aNumPreferredEntries) const
{
    // Count the number of "DNS/SRP Unicast" service entries in
    // the Network Data.

    const ServiceTlv *serviceTlv = nullptr;
    ServiceData       data;

    data.InitFrom(Service::DnsSrpUnicast::kServiceData);

    while ((serviceTlv = Get<Leader>().FindNextThreadService(serviceTlv, data, NetworkData::kServicePrefixMatch)) !=
           nullptr)
    {
        TlvIterator      subTlvIterator(*serviceTlv);
        const ServerTlv *serverSubTlv;

        while (((serverSubTlv = subTlvIterator.Iterate<ServerTlv>())) != nullptr)
        {
            if (serviceTlv->GetServiceDataLength() >= sizeof(Service::DnsSrpUnicast::ServiceData))
            {
                aNumEntries++;

                // Generally, we prefer entries where the SRP/DNS server
                // address/port info is included in the service TLV data
                // over the ones where the info is included in the
                // server TLV data (i.e., we prefer infra-provided
                // SRP/DNS entry over a BR local one using ML-EID). If
                // our entry itself uses the service TLV data, then we
                // prefer based on the associated RLOC16.

                if (GetType() == kTypeUnicast)
                {
                    if (IsPreferred(serverSubTlv->GetServer16()))
                    {
                        aNumPreferredEntries++;
                    }
                }
                else
                {
                    aNumPreferredEntries++;
                }
            }

            if (serverSubTlv->GetServerDataLength() >= sizeof(Service::DnsSrpUnicast::ServerData))
            {
                aNumEntries++;

                // If our entry also uses the server TLV data (with
                // ML-EID address), then the we prefer based on the
                // associated RLOC16.

                if ((GetType() == kTypeUnicastMeshLocalEid) && IsPreferred(serverSubTlv->GetServer16()))
                {
                    aNumPreferredEntries++;
                }
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
// Publisher::DnsSrpServiceEntry::Info

Publisher::DnsSrpServiceEntry::Info::Info(Type aType, uint16_t aPortOrSeqNumber, const Ip6::Address *aAddress)
    : mPortOrSeqNumber(aPortOrSeqNumber)
    , mType(aType)
{
    // It is important to `Clear()` the object since we compare all
    // bytes using overload of operator `==`.

    Clear();

    mType            = aType;
    mPortOrSeqNumber = aPortOrSeqNumber;

    if (aAddress != nullptr)
    {
        mAddress = *aAddress;
    }
}

#endif // OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE

//---------------------------------------------------------------------------------------------------------------------
// Publisher::PrefixEntry

void Publisher::PrefixEntry::Publish(const OnMeshPrefixConfig &aConfig)
{
    otLogInfoNetData("Publisher: Publishing OnMeshPrefix %s", aConfig.GetPrefix().ToString().AsCString());

    mType   = kTypeOnMeshPrefix;
    mPrefix = aConfig.GetPrefix();
    mFlags  = aConfig.ConvertToTlvFlags();

    SetState(kToAdd);

    Process();
}

void Publisher::PrefixEntry::Publish(const ExternalRouteConfig &aConfig)
{
    otLogInfoNetData("Publisher: Publishing ExternalRoute %s", aConfig.GetPrefix().ToString().AsCString());

    mType   = kTypeExternalRoute;
    mPrefix = aConfig.GetPrefix();
    mFlags  = aConfig.ConvertToTlvFlags();

    SetState(kToAdd);

    Process();
}

void Publisher::PrefixEntry::Unpublish(void)
{
    bool registerWithLeader = false;

    otLogInfoNetData("Publisher: Unpublishing %s", mPrefix.ToString().AsCString());

    registerWithLeader = Remove(/* aNextState */ kNoEntry);

    if (registerWithLeader)
    {
        Get<Notifier>().HandleServerDataUpdated();
    }
}

void Publisher::PrefixEntry::HandleNotifierEvents(Events aEvents)
{
    if (aEvents.ContainsAny(kEventThreadNetdataChanged | kEventThreadRoleChanged))
    {
        Process();
    }
}

bool Publisher::PrefixEntry::Add(void)
{
    // Adds the prefix entry to the network data.

    bool registerWithLeader = false;

    switch (mType)
    {
    case kTypeOnMeshPrefix:
        SuccessOrExit(AddOnMeshPrefix());
        break;

    case kTypeExternalRoute:
        SuccessOrExit(AddExternalRoute());
        break;
    }

    registerWithLeader = true;
    SetState(kAdded);
    Get<Publisher>().NotifyPrefixEntryChange(kEventEntryAdded, mPrefix);

exit:
    return registerWithLeader;
}

Error Publisher::PrefixEntry::AddOnMeshPrefix(void)
{
    OnMeshPrefixConfig config;

    config.mPrefix = mPrefix;
    config.mStable = true;
    config.SetFromTlvFlags(mFlags);

    return Get<Local>().AddOnMeshPrefix(config);
}

Error Publisher::PrefixEntry::AddExternalRoute(void)
{
    ExternalRouteConfig config;

    config.mPrefix = mPrefix;
    config.mStable = true;
    config.SetFromTlvFlags(static_cast<uint8_t>(mFlags));

    return Get<Local>().AddHasRoutePrefix(config);
}

bool Publisher::PrefixEntry::Remove(State aNextState)
{
    // Remove the prefix entry from the network data.

    bool registerWithLeader = false;

    VerifyOrExit((GetState() == kAdded) || (GetState() == kRemoving));

    switch (mType)
    {
    case kTypeOnMeshPrefix:
        IgnoreError(Get<Local>().RemoveOnMeshPrefix(mPrefix));
        break;

    case kTypeExternalRoute:
        IgnoreError(Get<Local>().RemoveHasRoutePrefix(mPrefix));
        break;
    }

    registerWithLeader = true;
    Get<Publisher>().NotifyPrefixEntryChange(kEventEntryRemoved, mPrefix);

exit:
    SetState(aNextState);
    return registerWithLeader;
}

void Publisher::PrefixEntry::Process(void)
{
    // This method checks the entries currently present in Network Data
    // based on which it then decides whether or not take action
    // (add/remove or keep monitoring).

    uint8_t numEntries          = 0;
    uint8_t numPreferredEntries = 0;
    uint8_t desiredNumEntries   = 0;

    // Do not make any changes if device is not attached, and wait
    // for role change event.
    VerifyOrExit(Get<Mle::Mle>().IsAttached());

    VerifyOrExit(GetState() != kNoEntry);

    switch (mType)
    {
    case kTypeOnMeshPrefix:
        CountOnMeshPrefixEntries(numEntries, numPreferredEntries);
        desiredNumEntries = kDesiredNumOnMeshPrefix;
        break;
    case kTypeExternalRoute:
        CountExternalRouteEntries(numEntries, numPreferredEntries);
        desiredNumEntries = kDesiredNumExternalRoute;
        break;
    }

    UpdateState(numEntries, numPreferredEntries, desiredNumEntries);

exit:
    return;
}

void Publisher::PrefixEntry::CountOnMeshPrefixEntries(uint8_t &aNumEntries, uint8_t &aNumPreferredEntries) const
{
    const PrefixTlv *      prefixTlv;
    const BorderRouterTlv *brSubTlv;
    int8_t                 preference             = BorderRouterEntry::PreferenceFromFlags(mFlags);
    uint16_t               flagsWithoutPreference = BorderRouterEntry::FlagsWithoutPreference(mFlags);

    prefixTlv = Get<Leader>().FindPrefix(mPrefix);
    VerifyOrExit(prefixTlv != nullptr);

    brSubTlv = prefixTlv->FindSubTlv<BorderRouterTlv>(/* aStable */ true);
    VerifyOrExit(brSubTlv != nullptr);

    for (const BorderRouterEntry *entry = brSubTlv->GetFirstEntry(); entry <= brSubTlv->GetLastEntry();
         entry                          = entry->GetNext())
    {
        uint16_t entryFlags      = entry->GetFlags();
        int8_t   entryPreference = BorderRouterEntry::PreferenceFromFlags(entryFlags);

        // Count an existing entry in the network data if its flags
        // match ours and and its preference is same or higher than our
        // preference. We do not count matching entries at a lower
        // preference than ours. This ensures that a device with higher
        // preference entry publishes its entry even when there are many
        // lower preference similar entries in the network data
        // (potentially causing a lower preference entry to be removed).

        if ((BorderRouterEntry::FlagsWithoutPreference(entryFlags) == flagsWithoutPreference) &&
            (entryPreference >= preference))
        {
            aNumEntries++;

            // We prefer an entry if it has strictly higher preference
            // than ours or if it has same preference we use the associated
            // RLOC16.

            if ((entryPreference > preference) || IsPreferred(entry->GetRloc()))
            {
                aNumPreferredEntries++;
            }
        }
    }

exit:
    return;
}

void Publisher::PrefixEntry::CountExternalRouteEntries(uint8_t &aNumEntries, uint8_t &aNumPreferredEntries) const
{
    const PrefixTlv *  prefixTlv;
    const HasRouteTlv *hrSubTlv;
    int8_t             preference             = HasRouteEntry::PreferenceFromFlags(static_cast<uint8_t>(mFlags));
    uint8_t            flagsWithoutPreference = HasRouteEntry::FlagsWithoutPreference(static_cast<uint8_t>(mFlags));

    prefixTlv = Get<Leader>().FindPrefix(mPrefix);
    VerifyOrExit(prefixTlv != nullptr);

    hrSubTlv = prefixTlv->FindSubTlv<HasRouteTlv>(/* aStable */ true);
    VerifyOrExit(hrSubTlv != nullptr);

    for (const HasRouteEntry *entry = hrSubTlv->GetFirstEntry(); entry <= hrSubTlv->GetLastEntry();
         entry                      = entry->GetNext())
    {
        uint8_t entryFlags      = entry->GetFlags();
        int8_t  entryPreference = HasRouteEntry::PreferenceFromFlags(entryFlags);

        // Count an existing entry in the network data if its flags
        // match ours and and its preference is same or higher than our
        // preference. We do not count matching entries at a lower
        // preference than ours. This ensures that a device with higher
        // preference entry publishes its entry even when there are many
        // lower preference similar entries in the network data
        // (potentially causing a lower preference entry to be removed).

        if ((HasRouteEntry::FlagsWithoutPreference(entryFlags) == flagsWithoutPreference) &&
            (entryPreference >= preference))
        {
            aNumEntries++;

            // We prefer an entry if it has strictly higher preference
            // than ours or if it has same preference with a smaller
            // RLOC16.

            if ((entryPreference > preference) || IsPreferred(entry->GetRloc()))
            {
                aNumPreferredEntries++;
            }
        }
    }

exit:
    return;
}

} // namespace NetworkData
} // namespace ot

#endif // OPENTHREAD_CONFIG_NETDATA_PUBLISHER_ENABLE
