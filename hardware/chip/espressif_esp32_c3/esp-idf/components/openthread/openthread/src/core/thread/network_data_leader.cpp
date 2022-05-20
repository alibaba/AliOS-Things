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
 *   This file implements the Thread Network Data managed by the Thread Leader.
 */

#include "network_data_leader.hpp"

#include "coap/coap_message.hpp"
#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/encoding.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/message.hpp"
#include "common/random.hpp"
#include "common/timer.hpp"
#include "mac/mac_types.hpp"
#include "thread/lowpan.hpp"
#include "thread/mle_router.hpp"
#include "thread/thread_netif.hpp"
#include "thread/thread_tlvs.hpp"
#include "thread/uri_paths.hpp"

namespace ot {
namespace NetworkData {

void LeaderBase::Reset(void)
{
    mVersion       = Random::NonCrypto::GetUint8();
    mStableVersion = Random::NonCrypto::GetUint8();
    SetLength(0);
    Get<ot::Notifier>().Signal(kEventThreadNetdataChanged);
}

Error LeaderBase::GetServiceId(uint32_t           aEnterpriseNumber,
                               const ServiceData &aServiceData,
                               bool               aServerStable,
                               uint8_t &          aServiceId) const
{
    Error         error    = kErrorNotFound;
    Iterator      iterator = kIteratorInit;
    ServiceConfig serviceConfig;
    ServiceData   serviceData;

    while (GetNextService(iterator, serviceConfig) == kErrorNone)
    {
        serviceConfig.GetServiceData(serviceData);

        if (aEnterpriseNumber == serviceConfig.mEnterpriseNumber && aServiceData == serviceData &&
            aServerStable == serviceConfig.mServerConfig.mStable)
        {
            aServiceId = serviceConfig.mServiceId;
            ExitNow(error = kErrorNone);
        }
    }

exit:
    return error;
}

const PrefixTlv *LeaderBase::FindNextMatchingPrefix(const Ip6::Address &aAddress, const PrefixTlv *aPrevTlv) const
{
    const PrefixTlv *prefixTlv;
    TlvIterator      tlvIterator((aPrevTlv == nullptr) ? GetTlvsStart() : aPrevTlv->GetNext(), GetTlvsEnd());

    while ((prefixTlv = tlvIterator.Iterate<PrefixTlv>()) != nullptr)
    {
        if (aAddress.MatchesPrefix(prefixTlv->GetPrefix(), prefixTlv->GetPrefixLength()))
        {
            break;
        }
    }

    return prefixTlv;
}

Error LeaderBase::GetContext(const Ip6::Address &aAddress, Lowpan::Context &aContext) const
{
    const PrefixTlv * prefix = nullptr;
    const ContextTlv *contextTlv;

    aContext.mPrefix.SetLength(0);

    if (Get<Mle::MleRouter>().IsMeshLocalAddress(aAddress))
    {
        aContext.mPrefix.Set(Get<Mle::MleRouter>().GetMeshLocalPrefix());
        aContext.mContextId    = Mle::kMeshLocalPrefixContextId;
        aContext.mCompressFlag = true;
    }

    while ((prefix = FindNextMatchingPrefix(aAddress, prefix)) != nullptr)
    {
        contextTlv = prefix->FindSubTlv<ContextTlv>();

        if (contextTlv == nullptr)
        {
            continue;
        }

        if (prefix->GetPrefixLength() > aContext.mPrefix.GetLength())
        {
            aContext.mPrefix.Set(prefix->GetPrefix(), prefix->GetPrefixLength());
            aContext.mContextId    = contextTlv->GetContextId();
            aContext.mCompressFlag = contextTlv->IsCompress();
        }
    }

    return (aContext.mPrefix.GetLength() > 0) ? kErrorNone : kErrorNotFound;
}

Error LeaderBase::GetContext(uint8_t aContextId, Lowpan::Context &aContext) const
{
    Error            error = kErrorNotFound;
    TlvIterator      tlvIterator(GetTlvsStart(), GetTlvsEnd());
    const PrefixTlv *prefix;

    if (aContextId == Mle::kMeshLocalPrefixContextId)
    {
        aContext.mPrefix.Set(Get<Mle::MleRouter>().GetMeshLocalPrefix());
        aContext.mContextId    = Mle::kMeshLocalPrefixContextId;
        aContext.mCompressFlag = true;
        ExitNow(error = kErrorNone);
    }

    while ((prefix = tlvIterator.Iterate<PrefixTlv>()) != nullptr)
    {
        const ContextTlv *contextTlv = prefix->FindSubTlv<ContextTlv>();

        if ((contextTlv == nullptr) || (contextTlv->GetContextId() != aContextId))
        {
            continue;
        }

        aContext.mPrefix.Set(prefix->GetPrefix(), prefix->GetPrefixLength());
        aContext.mContextId    = contextTlv->GetContextId();
        aContext.mCompressFlag = contextTlv->IsCompress();
        ExitNow(error = kErrorNone);
    }

exit:
    return error;
}

bool LeaderBase::IsOnMesh(const Ip6::Address &aAddress) const
{
    const PrefixTlv *prefix = nullptr;
    bool             rval   = false;

    VerifyOrExit(!Get<Mle::MleRouter>().IsMeshLocalAddress(aAddress), rval = true);

    while ((prefix = FindNextMatchingPrefix(aAddress, prefix)) != nullptr)
    {
        // check both stable and temporary Border Router TLVs
        for (int i = 0; i < 2; i++)
        {
            const BorderRouterTlv *borderRouter = prefix->FindSubTlv<BorderRouterTlv>(/* aStable */ (i == 0));

            if (borderRouter == nullptr)
            {
                continue;
            }

            for (const BorderRouterEntry *entry = borderRouter->GetFirstEntry(); entry <= borderRouter->GetLastEntry();
                 entry                          = entry->GetNext())
            {
                if (entry->IsOnMesh())
                {
                    ExitNow(rval = true);
                }
            }
        }
    }

exit:
    return rval;
}

Error LeaderBase::RouteLookup(const Ip6::Address &aSource,
                              const Ip6::Address &aDestination,
                              uint8_t *           aPrefixMatchLength,
                              uint16_t *          aRloc16) const
{
    Error            error  = kErrorNoRoute;
    const PrefixTlv *prefix = nullptr;

    while ((prefix = FindNextMatchingPrefix(aSource, prefix)) != nullptr)
    {
        if (ExternalRouteLookup(prefix->GetDomainId(), aDestination, aPrefixMatchLength, aRloc16) == kErrorNone)
        {
            ExitNow(error = kErrorNone);
        }

        if (DefaultRouteLookup(*prefix, aRloc16) == kErrorNone)
        {
            if (aPrefixMatchLength)
            {
                *aPrefixMatchLength = 0;
            }

            ExitNow(error = kErrorNone);
        }
    }

exit:
    return error;
}

Error LeaderBase::ExternalRouteLookup(uint8_t             aDomainId,
                                      const Ip6::Address &aDestination,
                                      uint8_t *           aPrefixMatchLength,
                                      uint16_t *          aRloc16) const
{
    Error                error = kErrorNoRoute;
    TlvIterator          tlvIterator(GetTlvsStart(), GetTlvsEnd());
    const PrefixTlv *    prefixTlv;
    const HasRouteEntry *bestRouteEntry  = nullptr;
    uint8_t              bestMatchLength = 0;

    while ((prefixTlv = tlvIterator.Iterate<PrefixTlv>()) != nullptr)
    {
        const HasRouteTlv *hasRoute;
        uint8_t            prefixLength = prefixTlv->GetPrefixLength();
        TlvIterator        subTlvIterator(*prefixTlv);

        if (prefixTlv->GetDomainId() != aDomainId)
        {
            continue;
        }

        if (!aDestination.MatchesPrefix(prefixTlv->GetPrefix(), prefixLength))
        {
            continue;
        }

        if ((bestRouteEntry != nullptr) && (prefixLength <= bestMatchLength))
        {
            continue;
        }

        while ((hasRoute = subTlvIterator.Iterate<HasRouteTlv>()) != nullptr)
        {
            for (const HasRouteEntry *entry = hasRoute->GetFirstEntry(); entry <= hasRoute->GetLastEntry();
                 entry                      = entry->GetNext())
            {
                if (bestRouteEntry == nullptr || entry->GetPreference() > bestRouteEntry->GetPreference() ||
                    (entry->GetPreference() == bestRouteEntry->GetPreference() &&
                     (entry->GetRloc() == Get<Mle::MleRouter>().GetRloc16() ||
                      (bestRouteEntry->GetRloc() != Get<Mle::MleRouter>().GetRloc16() &&
                       Get<Mle::MleRouter>().GetCost(entry->GetRloc()) <
                           Get<Mle::MleRouter>().GetCost(bestRouteEntry->GetRloc())))))
                {
                    bestRouteEntry  = entry;
                    bestMatchLength = prefixLength;
                }
            }
        }
    }

    if (bestRouteEntry != nullptr)
    {
        if (aRloc16 != nullptr)
        {
            *aRloc16 = bestRouteEntry->GetRloc();
        }

        if (aPrefixMatchLength != nullptr)
        {
            *aPrefixMatchLength = bestMatchLength;
        }

        error = kErrorNone;
    }

    return error;
}

Error LeaderBase::DefaultRouteLookup(const PrefixTlv &aPrefix, uint16_t *aRloc16) const
{
    Error                    error = kErrorNoRoute;
    TlvIterator              subTlvIterator(aPrefix);
    const BorderRouterTlv *  borderRouter;
    const BorderRouterEntry *route = nullptr;

    while ((borderRouter = subTlvIterator.Iterate<BorderRouterTlv>()) != nullptr)
    {
        for (const BorderRouterEntry *entry = borderRouter->GetFirstEntry(); entry <= borderRouter->GetLastEntry();
             entry                          = entry->GetNext())
        {
            if (!entry->IsDefaultRoute())
            {
                continue;
            }

            if (route == nullptr || entry->GetPreference() > route->GetPreference() ||
                (entry->GetPreference() == route->GetPreference() &&
                 (entry->GetRloc() == Get<Mle::MleRouter>().GetRloc16() ||
                  (route->GetRloc() != Get<Mle::MleRouter>().GetRloc16() &&
                   Get<Mle::MleRouter>().GetCost(entry->GetRloc()) < Get<Mle::MleRouter>().GetCost(route->GetRloc())))))
            {
                route = entry;
            }
        }
    }

    if (route != nullptr)
    {
        if (aRloc16 != nullptr)
        {
            *aRloc16 = route->GetRloc();
        }

        error = kErrorNone;
    }

    return error;
}

Error LeaderBase::SetNetworkData(uint8_t        aVersion,
                                 uint8_t        aStableVersion,
                                 bool           aStableOnly,
                                 const Message &aMessage,
                                 uint16_t       aMessageOffset)
{
    Error    error = kErrorNone;
    Mle::Tlv tlv;
    uint16_t length;

    SuccessOrExit(error = aMessage.Read(aMessageOffset, tlv));

    length = aMessage.ReadBytes(aMessageOffset + sizeof(tlv), GetBytes(), tlv.GetLength());
    VerifyOrExit(length == tlv.GetLength(), error = kErrorParse);

    SetLength(tlv.GetLength());
    mVersion       = aVersion;
    mStableVersion = aStableVersion;

    if (aStableOnly)
    {
        RemoveTemporaryData();
    }

#if OPENTHREAD_FTD
    // Synchronize internal 6LoWPAN Context ID Set with recently obtained Network Data.
    if (Get<Mle::MleRouter>().IsLeader())
    {
        Get<Leader>().UpdateContextsAfterReset();
    }
#endif

    otDumpDebgNetData("SetNetworkData", GetBytes(), GetLength());

    Get<ot::Notifier>().Signal(kEventThreadNetdataChanged);

exit:
    return error;
}

Error LeaderBase::SetCommissioningData(const uint8_t *aValue, uint8_t aValueLength)
{
    Error                 error = kErrorNone;
    CommissioningDataTlv *commissioningDataTlv;

    RemoveCommissioningData();

    if (aValueLength > 0)
    {
        VerifyOrExit(aValueLength <= kMaxSize - sizeof(CommissioningDataTlv), error = kErrorNoBufs);
        commissioningDataTlv =
            static_cast<CommissioningDataTlv *>(AppendTlv(sizeof(CommissioningDataTlv) + aValueLength));
        VerifyOrExit(commissioningDataTlv != nullptr, error = kErrorNoBufs);

        commissioningDataTlv->Init();
        commissioningDataTlv->SetLength(aValueLength);
        memcpy(commissioningDataTlv->GetValue(), aValue, aValueLength);
    }

    mVersion++;
    Get<ot::Notifier>().Signal(kEventThreadNetdataChanged);

exit:
    return error;
}

const CommissioningDataTlv *LeaderBase::GetCommissioningData(void) const
{
    return NetworkDataTlv::Find<CommissioningDataTlv>(GetTlvsStart(), GetTlvsEnd());
}

const MeshCoP::Tlv *LeaderBase::GetCommissioningDataSubTlv(MeshCoP::Tlv::Type aType) const
{
    const MeshCoP::Tlv *  rval = nullptr;
    const NetworkDataTlv *commissioningDataTlv;

    commissioningDataTlv = GetCommissioningData();
    VerifyOrExit(commissioningDataTlv != nullptr);

    rval = MeshCoP::Tlv::FindTlv(commissioningDataTlv->GetValue(), commissioningDataTlv->GetLength(), aType);

exit:
    return rval;
}

bool LeaderBase::IsJoiningEnabled(void) const
{
    const MeshCoP::Tlv *steeringData;
    bool                rval = false;

    VerifyOrExit(GetCommissioningDataSubTlv(MeshCoP::Tlv::kBorderAgentLocator) != nullptr);

    steeringData = GetCommissioningDataSubTlv(MeshCoP::Tlv::kSteeringData);
    VerifyOrExit(steeringData != nullptr);

    for (int i = 0; i < steeringData->GetLength(); i++)
    {
        if (steeringData->GetValue()[i] != 0)
        {
            ExitNow(rval = true);
        }
    }

exit:
    return rval;
}

void LeaderBase::RemoveCommissioningData(void)
{
    CommissioningDataTlv *tlv = GetCommissioningData();

    VerifyOrExit(tlv != nullptr);
    RemoveTlv(tlv);

exit:
    return;
}

Error LeaderBase::SteeringDataCheck(const FilterIndexes &aFilterIndexes) const
{
    Error                 error = kErrorNone;
    const MeshCoP::Tlv *  steeringDataTlv;
    MeshCoP::SteeringData steeringData;

    steeringDataTlv = GetCommissioningDataSubTlv(MeshCoP::Tlv::kSteeringData);
    VerifyOrExit(steeringDataTlv != nullptr, error = kErrorInvalidState);

    static_cast<const MeshCoP::SteeringDataTlv *>(steeringDataTlv)->CopyTo(steeringData);

    VerifyOrExit(steeringData.Contains(aFilterIndexes), error = kErrorNotFound);

exit:
    return error;
}

Error LeaderBase::SteeringDataCheckJoiner(const Mac::ExtAddress &aEui64) const
{
    FilterIndexes   filterIndexes;
    Mac::ExtAddress joinerId;

    MeshCoP::ComputeJoinerId(aEui64, joinerId);
    MeshCoP::SteeringData::CalculateHashBitIndexes(joinerId, filterIndexes);

    return SteeringDataCheck(filterIndexes);
}

Error LeaderBase::SteeringDataCheckJoiner(const MeshCoP::JoinerDiscerner &aDiscerner) const
{
    FilterIndexes filterIndexes;

    MeshCoP::SteeringData::CalculateHashBitIndexes(aDiscerner, filterIndexes);

    return SteeringDataCheck(filterIndexes);
}

} // namespace NetworkData
} // namespace ot
