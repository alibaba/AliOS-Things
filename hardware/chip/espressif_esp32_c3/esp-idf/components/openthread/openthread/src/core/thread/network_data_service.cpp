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
 *   This file implements function for managing Thread Network Data service/server entries.
 *
 */

#include "network_data_service.hpp"

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "thread/network_data_local.hpp"

namespace ot {
namespace NetworkData {
namespace Service {

// Definitions of static const class member variables to allow ODR-use
// (One Definition Rule), e.g., to get address of `kServiceData`.

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
const uint8_t BackboneRouter::kServiceData;
#endif
const uint8_t DnsSrpAnycast::kServiceData;
const uint8_t DnsSrpUnicast::kServiceData;

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE

Error Manager::AddService(const void *aServiceData,
                          uint8_t     aServiceDataLength,
                          bool        aServerStable,
                          const void *aServerData,
                          uint8_t     aServerDataLength)
{
    ServiceData serviceData;
    ServerData  serverData;

    serviceData.Init(aServiceData, aServiceDataLength);
    serverData.Init(aServerData, aServerDataLength);

    return Get<Local>().AddService(kThreadEnterpriseNumber, serviceData, aServerStable, serverData);
}

Error Manager::RemoveService(const void *aServiceData, uint8_t aServiceDataLength)
{
    ServiceData serviceData;

    serviceData.Init(aServiceData, aServiceDataLength);

    return Get<Local>().RemoveService(kThreadEnterpriseNumber, serviceData);
}

#endif // OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE

Error Manager::GetServiceId(const void *aServiceData,
                            uint8_t     aServiceDataLength,
                            bool        aServerStable,
                            uint8_t &   aServiceId) const
{
    ServiceData serviceData;

    serviceData.Init(aServiceData, aServiceDataLength);

    return Get<Leader>().GetServiceId(kThreadEnterpriseNumber, serviceData, aServerStable, aServiceId);
}

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

void Manager::GetBackboneRouterPrimary(ot::BackboneRouter::BackboneRouterConfig &aConfig) const
{
    const ServerTlv *                 rvalServerTlv  = nullptr;
    const BackboneRouter::ServerData *rvalServerData = nullptr;
    const ServiceTlv *                serviceTlv     = nullptr;
    ServiceData                       serviceData;

    serviceData.Init(&BackboneRouter::kServiceData, BackboneRouter::kServiceDataMinSize);

    aConfig.mServer16 = Mac::kShortAddrInvalid;

    while ((serviceTlv = Get<Leader>().FindNextThreadService(serviceTlv, serviceData,
                                                             NetworkData::kServicePrefixMatch)) != nullptr)
    {
        Iterator iterator;

        iterator.mServiceTlv = serviceTlv;

        while (IterateToNextServer(iterator) == kErrorNone)
        {
            ServerData                        data;
            const BackboneRouter::ServerData *serverData;

            iterator.mServerSubTlv->GetServerData(data);

            if (data.GetLength() < sizeof(BackboneRouter::ServerData))
            {
                continue;
            }

            serverData = reinterpret_cast<const BackboneRouter::ServerData *>(data.GetBytes());

            if (rvalServerTlv == nullptr ||
                IsBackboneRouterPreferredTo(*iterator.mServerSubTlv, *serverData, *rvalServerTlv, *rvalServerData))
            {
                rvalServerTlv  = iterator.mServerSubTlv;
                rvalServerData = serverData;
            }
        }
    }

    VerifyOrExit(rvalServerTlv != nullptr);

    aConfig.mServer16            = rvalServerTlv->GetServer16();
    aConfig.mSequenceNumber      = rvalServerData->GetSequenceNumber();
    aConfig.mReregistrationDelay = rvalServerData->GetReregistrationDelay();
    aConfig.mMlrTimeout          = rvalServerData->GetMlrTimeout();

exit:
    return;
}

bool Manager::IsBackboneRouterPreferredTo(const ServerTlv &                 aServerTlv,
                                          const BackboneRouter::ServerData &aServerData,
                                          const ServerTlv &                 aOtherServerTlv,
                                          const BackboneRouter::ServerData &aOtherServerData) const
{
    bool     isPreferred;
    uint16_t leaderRloc16 = Mle::Mle::Rloc16FromRouterId(Get<Mle::MleRouter>().GetLeaderId());

    VerifyOrExit(aServerTlv.GetServer16() != leaderRloc16, isPreferred = true);
    VerifyOrExit(aOtherServerTlv.GetServer16() != leaderRloc16, isPreferred = false);

    isPreferred = aServerData.GetSequenceNumber() > aOtherServerData.GetSequenceNumber() ||
                  (aServerData.GetSequenceNumber() == aOtherServerData.GetSequenceNumber() &&
                   aServerTlv.GetServer16() > aOtherServerTlv.GetServer16());
exit:
    return isPreferred;
}

#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

Error Manager::GetNextDnsSrpAnycastInfo(Iterator &aIterator, DnsSrpAnycast::Info &aInfo) const
{
    Error             error = kErrorNone;
    ServiceData       serviceData;
    const ServiceTlv *tlv = aIterator.mServiceTlv;

    serviceData.InitFrom(DnsSrpAnycast::kServiceData);

    do
    {
        tlv = Get<Leader>().FindNextThreadService(tlv, serviceData, NetworkData::kServicePrefixMatch);
        VerifyOrExit(tlv != nullptr, error = kErrorNotFound);

    } while (tlv->GetServiceDataLength() < sizeof(DnsSrpAnycast::ServiceData));

    tlv->GetServiceData(serviceData);
    aInfo.mAnycastAddress.SetToAnycastLocator(Get<Mle::Mle>().GetMeshLocalPrefix(),
                                              Mle::Mle::ServiceAlocFromId(tlv->GetServiceId()));
    aInfo.mSequenceNumber =
        reinterpret_cast<const DnsSrpAnycast::ServiceData *>(serviceData.GetBytes())->GetSequenceNumber();

    aIterator.mServiceTlv = tlv;

exit:
    return error;
}

Error Manager::FindPreferredDnsSrpAnycastInfo(DnsSrpAnycast::Info &aInfo) const
{
    Iterator            iterator;
    DnsSrpAnycast::Info info;
    bool                found = false;

    while (GetNextDnsSrpAnycastInfo(iterator, info) == kErrorNone)
    {
        if (!found || info.IsSequenceNumberAheadOf(aInfo))
        {
            aInfo = info;
            found = true;
        }
    }

    return found ? kErrorNone : kErrorNotFound;
}

Error Manager::GetNextDnsSrpUnicastInfo(Iterator &aIterator, DnsSrpUnicast::Info &aInfo) const
{
    Error       error = kErrorNone;
    ServiceData serviceData;

    serviceData.InitFrom(DnsSrpUnicast::kServiceData);

    while (true)
    {
        // Process Server sub-TLVs in the current Service TLV.

        while (IterateToNextServer(aIterator) == kErrorNone)
        {
            ServerData data;

            // Server sub-TLV can contain address and port info
            // (then we parse and return the info), or it can be
            // empty (then we skip over it).

            aIterator.mServerSubTlv->GetServerData(data);

            if (data.GetLength() >= sizeof(DnsSrpUnicast::ServerData))
            {
                const DnsSrpUnicast::ServerData *serverData =
                    reinterpret_cast<const DnsSrpUnicast::ServerData *>(data.GetBytes());

                aInfo.mSockAddr.SetAddress(serverData->GetAddress());
                aInfo.mSockAddr.SetPort(serverData->GetPort());
                ExitNow();
            }

            if (data.GetLength() == sizeof(uint16_t))
            {
                // Handle the case where the server TLV data only
                // contains a port number and use the RLOC as the
                // IPv6 address.
                aInfo.mSockAddr.GetAddress().SetToRoutingLocator(Get<Mle::Mle>().GetMeshLocalPrefix(),
                                                                 aIterator.mServerSubTlv->GetServer16());
                aInfo.mSockAddr.SetPort(Encoding::BigEndian::ReadUint16(data.GetBytes()));
                ExitNow();
            }
        }

        // Find the next matching Service TLV.

        aIterator.mServiceTlv =
            Get<Leader>().FindNextThreadService(aIterator.mServiceTlv, serviceData, NetworkData::kServicePrefixMatch);

        VerifyOrExit(aIterator.mServiceTlv != nullptr, error = kErrorNotFound);

        if (aIterator.mServiceTlv->GetServiceDataLength() >= sizeof(DnsSrpUnicast::ServiceData))
        {
            // The Service TLV data contains the address and port info.

            const DnsSrpUnicast::ServiceData *dnsServiceData;

            aIterator.mServiceTlv->GetServiceData(serviceData);
            dnsServiceData = reinterpret_cast<const DnsSrpUnicast::ServiceData *>(serviceData.GetBytes());
            aInfo.mSockAddr.SetAddress(dnsServiceData->GetAddress());
            aInfo.mSockAddr.SetPort(dnsServiceData->GetPort());
            ExitNow();
        }

        // Go back to the start of `while (true)` loop to
        // process the Server sub-TLVs in the new Service TLV.
    }

exit:
    return error;
}

Error Manager::IterateToNextServer(Iterator &aIterator) const
{
    Error error = kErrorNotFound;

    VerifyOrExit(aIterator.mServiceTlv != nullptr);

    aIterator.mServerSubTlv = NetworkDataTlv::Find<ServerTlv>(
        /* aStart */ (aIterator.mServerSubTlv != nullptr) ? aIterator.mServerSubTlv->GetNext()
                                                          : aIterator.mServiceTlv->GetSubTlvs(),
        /* aEnd */ aIterator.mServiceTlv->GetNext());

    if (aIterator.mServerSubTlv != nullptr)
    {
        error = kErrorNone;
    }

exit:
    return error;
}

} // namespace Service
} // namespace NetworkData
} // namespace ot
