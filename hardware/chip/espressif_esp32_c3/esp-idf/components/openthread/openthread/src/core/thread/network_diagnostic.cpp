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
 *   This file implements Thread's Network Diagnostic processing.
 */

#include "network_diagnostic.hpp"

#if OPENTHREAD_FTD || OPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE

#include "coap/coap_message.hpp"
#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/encoding.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "mac/mac.hpp"
#include "net/netif.hpp"
#include "thread/mesh_forwarder.hpp"
#include "thread/mle_router.hpp"
#include "thread/thread_netif.hpp"
#include "thread/thread_tlvs.hpp"
#include "thread/uri_paths.hpp"

namespace ot {

namespace NetworkDiagnostic {

NetworkDiagnostic::NetworkDiagnostic(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mDiagnosticGetRequest(UriPath::kDiagnosticGetRequest, &NetworkDiagnostic::HandleDiagnosticGetRequest, this)
    , mDiagnosticGetQuery(UriPath::kDiagnosticGetQuery, &NetworkDiagnostic::HandleDiagnosticGetQuery, this)
    , mDiagnosticGetAnswer(UriPath::kDiagnosticGetAnswer, &NetworkDiagnostic::HandleDiagnosticGetAnswer, this)
    , mDiagnosticReset(UriPath::kDiagnosticReset, &NetworkDiagnostic::HandleDiagnosticReset, this)
    , mReceiveDiagnosticGetCallback(nullptr)
    , mReceiveDiagnosticGetCallbackContext(nullptr)
{
    Get<Tmf::Agent>().AddResource(mDiagnosticGetRequest);
    Get<Tmf::Agent>().AddResource(mDiagnosticGetQuery);
    Get<Tmf::Agent>().AddResource(mDiagnosticGetAnswer);
    Get<Tmf::Agent>().AddResource(mDiagnosticReset);
}

Error NetworkDiagnostic::SendDiagnosticGet(const Ip6::Address &           aDestination,
                                           const uint8_t                  aTlvTypes[],
                                           uint8_t                        aCount,
                                           otReceiveDiagnosticGetCallback aCallback,
                                           void *                         aCallbackContext)
{
    Error                 error;
    Coap::Message *       message = nullptr;
    Ip6::MessageInfo      messageInfo;
    otCoapResponseHandler handler = nullptr;

    VerifyOrExit((message = Get<Tmf::Agent>().NewMessage()) != nullptr, error = kErrorNoBufs);

    if (aDestination.IsMulticast())
    {
        SuccessOrExit(error = message->InitAsNonConfirmablePost(UriPath::kDiagnosticGetQuery));
        messageInfo.SetMulticastLoop(true);
    }
    else
    {
        handler = &NetworkDiagnostic::HandleDiagnosticGetResponse;
        SuccessOrExit(error = message->InitAsConfirmablePost(UriPath::kDiagnosticGetRequest));
    }

    if (aCount > 0)
    {
        SuccessOrExit(error = message->SetPayloadMarker());
    }

    if (aCount > 0)
    {
        SuccessOrExit(error = Tlv::Append<TypeListTlv>(*message, aTlvTypes, aCount));
    }

    if (aDestination.IsLinkLocal() || aDestination.IsLinkLocalMulticast())
    {
        messageInfo.SetSockAddr(Get<Mle::MleRouter>().GetLinkLocalAddress());
    }
    else
    {
        messageInfo.SetSockAddr(Get<Mle::MleRouter>().GetMeshLocal16());
    }

    messageInfo.SetPeerAddr(aDestination);
    messageInfo.SetPeerPort(Tmf::kUdpPort);

    SuccessOrExit(error = Get<Tmf::Agent>().SendMessage(*message, messageInfo, handler, this));

    mReceiveDiagnosticGetCallback        = aCallback;
    mReceiveDiagnosticGetCallbackContext = aCallbackContext;

    otLogInfoNetDiag("Sent diagnostic get");

exit:
    FreeMessageOnError(message, error);
    return error;
}

void NetworkDiagnostic::HandleDiagnosticGetResponse(void *               aContext,
                                                    otMessage *          aMessage,
                                                    const otMessageInfo *aMessageInfo,
                                                    Error                aResult)
{
    static_cast<NetworkDiagnostic *>(aContext)->HandleDiagnosticGetResponse(
        static_cast<Coap::Message *>(aMessage), static_cast<const Ip6::MessageInfo *>(aMessageInfo), aResult);
}

void NetworkDiagnostic::HandleDiagnosticGetResponse(Coap::Message *         aMessage,
                                                    const Ip6::MessageInfo *aMessageInfo,
                                                    Error                   aResult)
{
    SuccessOrExit(aResult);
    VerifyOrExit(aMessage->GetCode() == Coap::kCodeChanged, aResult = kErrorFailed);

exit:
    if (mReceiveDiagnosticGetCallback)
    {
        mReceiveDiagnosticGetCallback(aResult, aMessage, aMessageInfo, mReceiveDiagnosticGetCallbackContext);
    }
    else
    {
        otLogDebgNetDiag("Received diagnostic get response, error = %s", ErrorToString(aResult));
    }
    return;
}

void NetworkDiagnostic::HandleDiagnosticGetAnswer(void *               aContext,
                                                  otMessage *          aMessage,
                                                  const otMessageInfo *aMessageInfo)
{
    static_cast<NetworkDiagnostic *>(aContext)->HandleDiagnosticGetAnswer(
        *static_cast<Coap::Message *>(aMessage), *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

void NetworkDiagnostic::HandleDiagnosticGetAnswer(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    VerifyOrExit(aMessage.IsConfirmablePostRequest());

    otLogInfoNetDiag("Diagnostic get answer received");

    if (mReceiveDiagnosticGetCallback)
    {
        mReceiveDiagnosticGetCallback(kErrorNone, &aMessage, &aMessageInfo, mReceiveDiagnosticGetCallbackContext);
    }

    SuccessOrExit(Get<Tmf::Agent>().SendEmptyAck(aMessage, aMessageInfo));

    otLogInfoNetDiag("Sent diagnostic answer acknowledgment");

exit:
    return;
}

Error NetworkDiagnostic::AppendIp6AddressList(Message &aMessage)
{
    Error             error = kErrorNone;
    Ip6AddressListTlv tlv;
    uint8_t           count = 0;

    tlv.Init();

    for (const Ip6::Netif::UnicastAddress &addr : Get<ThreadNetif>().GetUnicastAddresses())
    {
        OT_UNUSED_VARIABLE(addr);
        count++;
    }

    tlv.SetLength(count * sizeof(Ip6::Address));
    SuccessOrExit(error = aMessage.Append(tlv));

    for (const Ip6::Netif::UnicastAddress &addr : Get<ThreadNetif>().GetUnicastAddresses())
    {
        SuccessOrExit(error = aMessage.Append(addr.GetAddress()));
    }

exit:

    return error;
}

#if OPENTHREAD_FTD
Error NetworkDiagnostic::AppendChildTable(Message &aMessage)
{
    Error           error   = kErrorNone;
    uint16_t        count   = 0;
    uint8_t         timeout = 0;
    ChildTableTlv   tlv;
    ChildTableEntry entry;

    tlv.Init();

    count = Get<ChildTable>().GetNumChildren(Child::kInStateValid);

    // The length of the Child Table TLV may exceed the outgoing link's MTU (1280B).
    // As a workaround we limit the number of entries in the Child Table TLV,
    // also to avoid using extended TLV format. The issue is processed by the
    // Thread Group (SPEC-894).
    if (count > (Tlv::kBaseTlvMaxLength / sizeof(ChildTableEntry)))
    {
        count = Tlv::kBaseTlvMaxLength / sizeof(ChildTableEntry);
    }

    tlv.SetLength(static_cast<uint8_t>(count * sizeof(ChildTableEntry)));

    SuccessOrExit(error = aMessage.Append(tlv));

    for (Child &child : Get<ChildTable>().Iterate(Child::kInStateValid))
    {
        VerifyOrExit(count--);

        timeout = 0;

        while (static_cast<uint32_t>(1 << timeout) < child.GetTimeout())
        {
            timeout++;
        }

        entry.SetReserved(0);
        entry.SetTimeout(timeout + 4);

        entry.SetChildId(Mle::Mle::ChildIdFromRloc16(child.GetRloc16()));
        entry.SetMode(child.GetDeviceMode());

        SuccessOrExit(error = aMessage.Append(entry));
    }

exit:

    return error;
}
#endif // OPENTHREAD_FTD

void NetworkDiagnostic::FillMacCountersTlv(MacCountersTlv &aMacCountersTlv)
{
    const otMacCounters &macCounters = Get<Mac::Mac>().GetCounters();

    aMacCountersTlv.SetIfInUnknownProtos(macCounters.mRxOther);
    aMacCountersTlv.SetIfInErrors(macCounters.mRxErrNoFrame + macCounters.mRxErrUnknownNeighbor +
                                  macCounters.mRxErrInvalidSrcAddr + macCounters.mRxErrSec + macCounters.mRxErrFcs +
                                  macCounters.mRxErrOther);
    aMacCountersTlv.SetIfOutErrors(macCounters.mTxErrCca);
    aMacCountersTlv.SetIfInUcastPkts(macCounters.mRxUnicast);
    aMacCountersTlv.SetIfInBroadcastPkts(macCounters.mRxBroadcast);
    aMacCountersTlv.SetIfInDiscards(macCounters.mRxAddressFiltered + macCounters.mRxDestAddrFiltered +
                                    macCounters.mRxDuplicated);
    aMacCountersTlv.SetIfOutUcastPkts(macCounters.mTxUnicast);
    aMacCountersTlv.SetIfOutBroadcastPkts(macCounters.mTxBroadcast);
    aMacCountersTlv.SetIfOutDiscards(macCounters.mTxErrBusyChannel);
}

Error NetworkDiagnostic::FillRequestedTlvs(const Message &       aRequest,
                                           Message &             aResponse,
                                           NetworkDiagnosticTlv &aNetworkDiagnosticTlv)
{
    Error    error  = kErrorNone;
    uint16_t offset = 0;
    uint8_t  type;

    offset = aRequest.GetOffset() + sizeof(NetworkDiagnosticTlv);

    for (uint32_t i = 0; i < aNetworkDiagnosticTlv.GetLength(); i++)
    {
        SuccessOrExit(error = aRequest.Read(offset, type));

        otLogInfoNetDiag("Type %d", type);

        switch (type)
        {
        case NetworkDiagnosticTlv::kExtMacAddress:
            SuccessOrExit(error = Tlv::Append<ExtMacAddressTlv>(aResponse, Get<Mac::Mac>().GetExtAddress()));
            break;

        case NetworkDiagnosticTlv::kAddress16:
            SuccessOrExit(error = Tlv::Append<Address16Tlv>(aResponse, Get<Mle::MleRouter>().GetRloc16()));
            break;

        case NetworkDiagnosticTlv::kMode:
            SuccessOrExit(error = Tlv::Append<ModeTlv>(aResponse, Get<Mle::MleRouter>().GetDeviceMode().Get()));
            break;

        case NetworkDiagnosticTlv::kTimeout:
            if (!Get<Mle::MleRouter>().IsRxOnWhenIdle())
            {
                SuccessOrExit(error = Tlv::Append<TimeoutTlv>(aResponse, Get<Mle::MleRouter>().GetTimeout()));
            }

            break;

#if OPENTHREAD_FTD
        case NetworkDiagnosticTlv::kConnectivity:
        {
            ConnectivityTlv tlv;
            tlv.Init();
            Get<Mle::MleRouter>().FillConnectivityTlv(reinterpret_cast<Mle::ConnectivityTlv &>(tlv));
            SuccessOrExit(error = tlv.AppendTo(aResponse));
            break;
        }

        case NetworkDiagnosticTlv::kRoute:
        {
            RouteTlv tlv;
            tlv.Init();
            Get<Mle::MleRouter>().FillRouteTlv(reinterpret_cast<Mle::RouteTlv &>(tlv));
            SuccessOrExit(error = tlv.AppendTo(aResponse));
            break;
        }
#endif

        case NetworkDiagnosticTlv::kLeaderData:
        {
            LeaderDataTlv          tlv;
            const Mle::LeaderData &leaderData = Get<Mle::MleRouter>().GetLeaderData();

            tlv.Init();
            tlv.SetPartitionId(leaderData.GetPartitionId());
            tlv.SetWeighting(leaderData.GetWeighting());
            tlv.SetDataVersion(leaderData.GetDataVersion());
            tlv.SetStableDataVersion(leaderData.GetStableDataVersion());
            tlv.SetLeaderRouterId(leaderData.GetLeaderRouterId());

            SuccessOrExit(error = tlv.AppendTo(aResponse));
            break;
        }

        case NetworkDiagnosticTlv::kNetworkData:
        {
            NetworkData::NetworkData &netData = Get<NetworkData::Leader>();

            SuccessOrExit(error = Tlv::Append<NetworkDataTlv>(aResponse, netData.GetBytes(), netData.GetLength()));
            break;
        }

        case NetworkDiagnosticTlv::kIp6AddressList:
        {
            SuccessOrExit(error = AppendIp6AddressList(aResponse));
            break;
        }

        case NetworkDiagnosticTlv::kMacCounters:
        {
            MacCountersTlv tlv;
            memset(&tlv, 0, sizeof(tlv));
            tlv.Init();
            FillMacCountersTlv(tlv);
            SuccessOrExit(error = tlv.AppendTo(aResponse));
            break;
        }

        case NetworkDiagnosticTlv::kBatteryLevel:
        {
            // Thread 1.1.1 Specification Section 10.11.4.2:
            // Omitted if the battery level is not measured, is unknown or the device does not
            // operate on battery power.
            break;
        }

        case NetworkDiagnosticTlv::kSupplyVoltage:
        {
            // Thread 1.1.1 Specification Section 10.11.4.3:
            // Omitted if the supply voltage is not measured, is unknown.
            break;
        }

#if OPENTHREAD_FTD
        case NetworkDiagnosticTlv::kChildTable:
        {
            // Thread 1.1.1 Specification Section 10.11.2.2:
            // If a Thread device is unable to supply a specific Diagnostic TLV, that TLV is omitted.
            // Here only Leader or Router may have children.
            if (Get<Mle::MleRouter>().IsRouterOrLeader())
            {
                SuccessOrExit(error = AppendChildTable(aResponse));
            }
            break;
        }
#endif

        case NetworkDiagnosticTlv::kChannelPages:
        {
            uint8_t         length   = 0;
            uint32_t        pageMask = Radio::kSupportedChannelPages;
            ChannelPagesTlv tlv;

            tlv.Init();
            for (uint8_t page = 0; page < sizeof(pageMask) * 8; page++)
            {
                if (pageMask & (1 << page))
                {
                    tlv.GetChannelPages()[length++] = page;
                }
            }

            tlv.SetLength(length);
            SuccessOrExit(error = tlv.AppendTo(aResponse));
            break;
        }

#if OPENTHREAD_FTD
        case NetworkDiagnosticTlv::kMaxChildTimeout:
        {
            uint32_t maxTimeout;

            if (Get<Mle::MleRouter>().GetMaxChildTimeout(maxTimeout) == kErrorNone)
            {
                SuccessOrExit(error = Tlv::Append<MaxChildTimeoutTlv>(aResponse, maxTimeout));
            }

            break;
        }
#endif

        default:
            // Skip unrecognized TLV type.
            break;
        }

        offset += sizeof(type);
    }

exit:
    return error;
}

void NetworkDiagnostic::HandleDiagnosticGetQuery(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<NetworkDiagnostic *>(aContext)->HandleDiagnosticGetQuery(
        *static_cast<Coap::Message *>(aMessage), *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

void NetworkDiagnostic::HandleDiagnosticGetQuery(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    Error                error   = kErrorNone;
    Coap::Message *      message = nullptr;
    NetworkDiagnosticTlv networkDiagnosticTlv;
    Ip6::MessageInfo     messageInfo;

    VerifyOrExit(aMessage.IsPostRequest(), error = kErrorDrop);

    otLogInfoNetDiag("Received diagnostic get query");

    SuccessOrExit(error = aMessage.Read(aMessage.GetOffset(), networkDiagnosticTlv));

    VerifyOrExit(networkDiagnosticTlv.GetType() == NetworkDiagnosticTlv::kTypeList, error = kErrorParse);

    // DIAG_GET.qry may be sent as a confirmable message.
    if (aMessage.IsConfirmable())
    {
        if (Get<Tmf::Agent>().SendEmptyAck(aMessage, aMessageInfo) == kErrorNone)
        {
            otLogInfoNetDiag("Sent diagnostic get query acknowledgment");
        }
    }

    VerifyOrExit((message = Get<Tmf::Agent>().NewMessage()) != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = message->InitAsConfirmablePost(UriPath::kDiagnosticGetAnswer));

    if (networkDiagnosticTlv.GetLength() > 0)
    {
        SuccessOrExit(error = message->SetPayloadMarker());
    }

    if (aMessageInfo.GetSockAddr().IsLinkLocal() || aMessageInfo.GetSockAddr().IsLinkLocalMulticast())
    {
        messageInfo.SetSockAddr(Get<Mle::MleRouter>().GetLinkLocalAddress());
    }
    else
    {
        messageInfo.SetSockAddr(Get<Mle::MleRouter>().GetMeshLocal16());
    }

    messageInfo.SetPeerAddr(aMessageInfo.GetPeerAddr());
    messageInfo.SetPeerPort(Tmf::kUdpPort);

    SuccessOrExit(error = FillRequestedTlvs(aMessage, *message, networkDiagnosticTlv));

    SuccessOrExit(error = Get<Tmf::Agent>().SendMessage(*message, messageInfo, nullptr, this));

    otLogInfoNetDiag("Sent diagnostic get answer");

exit:
    FreeMessageOnError(message, error);
}

void NetworkDiagnostic::HandleDiagnosticGetRequest(void *               aContext,
                                                   otMessage *          aMessage,
                                                   const otMessageInfo *aMessageInfo)
{
    static_cast<NetworkDiagnostic *>(aContext)->HandleDiagnosticGetRequest(
        *static_cast<Coap::Message *>(aMessage), *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

void NetworkDiagnostic::HandleDiagnosticGetRequest(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    Error                error   = kErrorNone;
    Coap::Message *      message = nullptr;
    NetworkDiagnosticTlv networkDiagnosticTlv;
    Ip6::MessageInfo     messageInfo(aMessageInfo);

    VerifyOrExit(aMessage.IsConfirmablePostRequest(), error = kErrorDrop);

    otLogInfoNetDiag("Received diagnostic get request");

    SuccessOrExit(error = aMessage.Read(aMessage.GetOffset(), networkDiagnosticTlv));

    VerifyOrExit(networkDiagnosticTlv.GetType() == NetworkDiagnosticTlv::kTypeList, error = kErrorParse);

    VerifyOrExit((message = Get<Tmf::Agent>().NewMessage()) != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = message->SetDefaultResponseHeader(aMessage));
    SuccessOrExit(error = message->SetPayloadMarker());

    SuccessOrExit(error = FillRequestedTlvs(aMessage, *message, networkDiagnosticTlv));

    SuccessOrExit(error = Get<Tmf::Agent>().SendMessage(*message, messageInfo));

    otLogInfoNetDiag("Sent diagnostic get response");

exit:
    FreeMessageOnError(message, error);
}

Error NetworkDiagnostic::SendDiagnosticReset(const Ip6::Address &aDestination,
                                             const uint8_t       aTlvTypes[],
                                             uint8_t             aCount)
{
    Error            error;
    Coap::Message *  message = nullptr;
    Ip6::MessageInfo messageInfo;

    VerifyOrExit((message = Get<Tmf::Agent>().NewMessage()) != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = message->InitAsConfirmablePost(UriPath::kDiagnosticReset));

    if (aCount > 0)
    {
        SuccessOrExit(error = message->SetPayloadMarker());
    }

    if (aCount > 0)
    {
        SuccessOrExit(error = Tlv::Append<TypeListTlv>(*message, aTlvTypes, aCount));
    }

    if (aDestination.IsLinkLocal() || aDestination.IsLinkLocalMulticast())
    {
        messageInfo.SetSockAddr(Get<Mle::MleRouter>().GetLinkLocalAddress());
    }
    else
    {
        messageInfo.SetSockAddr(Get<Mle::MleRouter>().GetMeshLocal16());
    }

    messageInfo.SetPeerAddr(aDestination);
    messageInfo.SetPeerPort(Tmf::kUdpPort);

    SuccessOrExit(error = Get<Tmf::Agent>().SendMessage(*message, messageInfo));

    otLogInfoNetDiag("Sent network diagnostic reset");

exit:
    FreeMessageOnError(message, error);
    return error;
}

void NetworkDiagnostic::HandleDiagnosticReset(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<NetworkDiagnostic *>(aContext)->HandleDiagnosticReset(
        *static_cast<Coap::Message *>(aMessage), *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

void NetworkDiagnostic::HandleDiagnosticReset(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    uint16_t             offset = 0;
    uint8_t              type;
    NetworkDiagnosticTlv tlv;

    otLogInfoNetDiag("Received diagnostic reset request");

    VerifyOrExit(aMessage.IsConfirmablePostRequest());

    SuccessOrExit(aMessage.Read(aMessage.GetOffset(), tlv));

    VerifyOrExit(tlv.GetType() == NetworkDiagnosticTlv::kTypeList);

    offset = aMessage.GetOffset() + sizeof(NetworkDiagnosticTlv);

    for (uint8_t i = 0; i < tlv.GetLength(); i++)
    {
        SuccessOrExit(aMessage.Read(offset + i, type));

        switch (type)
        {
        case NetworkDiagnosticTlv::kMacCounters:
            Get<Mac::Mac>().ResetCounters();
            otLogInfoNetDiag("Received diagnostic reset type kMacCounters(9)");
            break;

        default:
            otLogInfoNetDiag("Received diagnostic reset other type %d not resetable", type);
            break;
        }
    }

    SuccessOrExit(Get<Tmf::Agent>().SendEmptyAck(aMessage, aMessageInfo));

    otLogInfoNetDiag("Sent diagnostic reset acknowledgment");

exit:
    return;
}

static inline void ParseMode(const Mle::DeviceMode &aMode, otLinkModeConfig &aLinkModeConfig)
{
    aLinkModeConfig.mRxOnWhenIdle = aMode.IsRxOnWhenIdle();
    aLinkModeConfig.mDeviceType   = aMode.IsFullThreadDevice();
    aLinkModeConfig.mNetworkData  = aMode.IsFullNetworkData();
}

static inline void ParseConnectivity(const ConnectivityTlv &    aConnectivityTlv,
                                     otNetworkDiagConnectivity &aNetworkDiagConnectivity)
{
    aNetworkDiagConnectivity.mParentPriority   = aConnectivityTlv.GetParentPriority();
    aNetworkDiagConnectivity.mLinkQuality3     = aConnectivityTlv.GetLinkQuality3();
    aNetworkDiagConnectivity.mLinkQuality2     = aConnectivityTlv.GetLinkQuality2();
    aNetworkDiagConnectivity.mLinkQuality1     = aConnectivityTlv.GetLinkQuality1();
    aNetworkDiagConnectivity.mLeaderCost       = aConnectivityTlv.GetLeaderCost();
    aNetworkDiagConnectivity.mIdSequence       = aConnectivityTlv.GetIdSequence();
    aNetworkDiagConnectivity.mActiveRouters    = aConnectivityTlv.GetActiveRouters();
    aNetworkDiagConnectivity.mSedBufferSize    = aConnectivityTlv.GetSedBufferSize();
    aNetworkDiagConnectivity.mSedDatagramCount = aConnectivityTlv.GetSedDatagramCount();
}

static void ParseRoute(const RouteTlv &aRouteTlv, otNetworkDiagRoute &aNetworkDiagRoute)
{
    uint8_t routeCount = 0;

    for (uint8_t i = 0; i <= Mle::kMaxRouterId; ++i)
    {
        if (!aRouteTlv.IsRouterIdSet(i))
        {
            continue;
        }
        aNetworkDiagRoute.mRouteData[routeCount].mRouterId       = i;
        aNetworkDiagRoute.mRouteData[routeCount].mRouteCost      = aRouteTlv.GetRouteCost(routeCount);
        aNetworkDiagRoute.mRouteData[routeCount].mLinkQualityIn  = aRouteTlv.GetLinkQualityIn(routeCount);
        aNetworkDiagRoute.mRouteData[routeCount].mLinkQualityOut = aRouteTlv.GetLinkQualityOut(routeCount);
        ++routeCount;
    }
    aNetworkDiagRoute.mRouteCount = routeCount;
    aNetworkDiagRoute.mIdSequence = aRouteTlv.GetRouterIdSequence();
}

static inline void ParseLeaderData(const LeaderDataTlv &aLeaderDataTlv, otLeaderData &aLeaderData)
{
    aLeaderData.mPartitionId       = aLeaderDataTlv.GetPartitionId();
    aLeaderData.mWeighting         = aLeaderDataTlv.GetWeighting();
    aLeaderData.mDataVersion       = aLeaderDataTlv.GetDataVersion();
    aLeaderData.mStableDataVersion = aLeaderDataTlv.GetStableDataVersion();
    aLeaderData.mLeaderRouterId    = aLeaderDataTlv.GetLeaderRouterId();
}

static inline void ParseMacCounters(const MacCountersTlv &aMacCountersTlv, otNetworkDiagMacCounters &aMacCounters)
{
    aMacCounters.mIfInUnknownProtos  = aMacCountersTlv.GetIfInUnknownProtos();
    aMacCounters.mIfInErrors         = aMacCountersTlv.GetIfInErrors();
    aMacCounters.mIfOutErrors        = aMacCountersTlv.GetIfOutErrors();
    aMacCounters.mIfInUcastPkts      = aMacCountersTlv.GetIfInUcastPkts();
    aMacCounters.mIfInBroadcastPkts  = aMacCountersTlv.GetIfInBroadcastPkts();
    aMacCounters.mIfInDiscards       = aMacCountersTlv.GetIfInDiscards();
    aMacCounters.mIfOutUcastPkts     = aMacCountersTlv.GetIfOutUcastPkts();
    aMacCounters.mIfOutBroadcastPkts = aMacCountersTlv.GetIfOutBroadcastPkts();
    aMacCounters.mIfOutDiscards      = aMacCountersTlv.GetIfOutDiscards();
}

static inline void ParseChildEntry(const ChildTableEntry &aChildTableTlvEntry, otNetworkDiagChildEntry &aChildEntry)
{
    aChildEntry.mTimeout = aChildTableTlvEntry.GetTimeout();
    aChildEntry.mChildId = aChildTableTlvEntry.GetChildId();
    ParseMode(aChildTableTlvEntry.GetMode(), aChildEntry.mMode);
}

Error NetworkDiagnostic::GetNextDiagTlv(const Coap::Message &aMessage,
                                        Iterator &           aIterator,
                                        otNetworkDiagTlv &   aNetworkDiagTlv)
{
    Error                error  = kErrorNone;
    uint16_t             offset = aMessage.GetOffset() + aIterator;
    NetworkDiagnosticTlv tlv;

    while (true)
    {
        uint16_t tlvTotalLength;

        VerifyOrExit(aMessage.Read(offset, tlv) == kErrorNone, error = kErrorNotFound);

        switch (tlv.GetType())
        {
        case NetworkDiagnosticTlv::kExtMacAddress:
            SuccessOrExit(error = Tlv::Read<ExtMacAddressTlv>(
                              aMessage, offset, static_cast<Mac::ExtAddress &>(aNetworkDiagTlv.mData.mExtAddress)));
            break;

        case NetworkDiagnosticTlv::kAddress16:
            SuccessOrExit(error = Tlv::Read<Address16Tlv>(aMessage, offset, aNetworkDiagTlv.mData.mAddr16));
            break;

        case NetworkDiagnosticTlv::kMode:
        {
            uint8_t mode;

            SuccessOrExit(error = Tlv::Read<ModeTlv>(aMessage, offset, mode));
            ParseMode(Mle::DeviceMode(mode), aNetworkDiagTlv.mData.mMode);
            break;
        }

        case NetworkDiagnosticTlv::kTimeout:
            SuccessOrExit(error = Tlv::Read<TimeoutTlv>(aMessage, offset, aNetworkDiagTlv.mData.mTimeout));
            break;

        case NetworkDiagnosticTlv::kConnectivity:
        {
            ConnectivityTlv connectivity;

            SuccessOrExit(error = aMessage.Read(offset, connectivity));
            VerifyOrExit(connectivity.IsValid(), error = kErrorParse);

            ParseConnectivity(connectivity, aNetworkDiagTlv.mData.mConnectivity);
            break;
        }

        case NetworkDiagnosticTlv::kRoute:
        {
            RouteTlv route;

            tlvTotalLength = sizeof(tlv) + tlv.GetLength();
            VerifyOrExit(tlvTotalLength <= sizeof(route), error = kErrorParse);
            SuccessOrExit(error = aMessage.Read(offset, &route, tlvTotalLength));
            VerifyOrExit(route.IsValid(), error = kErrorParse);

            ParseRoute(route, aNetworkDiagTlv.mData.mRoute);
            break;
        }

        case NetworkDiagnosticTlv::kLeaderData:
        {
            LeaderDataTlv leaderData;

            SuccessOrExit(error = aMessage.Read(offset, leaderData));
            VerifyOrExit(leaderData.IsValid(), error = kErrorParse);

            ParseLeaderData(leaderData, aNetworkDiagTlv.mData.mLeaderData);
            break;
        }

        case NetworkDiagnosticTlv::kNetworkData:
        {
            NetworkDataTlv networkData;

            tlvTotalLength = sizeof(tlv) + tlv.GetLength();
            VerifyOrExit(tlvTotalLength <= sizeof(networkData), error = kErrorParse);
            SuccessOrExit(error = aMessage.Read(offset, &networkData, tlvTotalLength));
            VerifyOrExit(networkData.IsValid(), error = kErrorParse);
            VerifyOrExit(sizeof(aNetworkDiagTlv.mData.mNetworkData.m8) >= networkData.GetLength(), error = kErrorParse);

            memcpy(aNetworkDiagTlv.mData.mNetworkData.m8, networkData.GetNetworkData(), networkData.GetLength());
            aNetworkDiagTlv.mData.mNetworkData.mCount = networkData.GetLength();
            break;
        }

        case NetworkDiagnosticTlv::kIp6AddressList:
        {
            Ip6AddressListTlv &ip6AddrList = static_cast<Ip6AddressListTlv &>(tlv);

            VerifyOrExit(ip6AddrList.IsValid(), error = kErrorParse);
            VerifyOrExit(sizeof(aNetworkDiagTlv.mData.mIp6AddrList.mList) >= ip6AddrList.GetLength(),
                         error = kErrorParse);
            SuccessOrExit(error = aMessage.Read(offset + sizeof(ip6AddrList), aNetworkDiagTlv.mData.mIp6AddrList.mList,
                                                ip6AddrList.GetLength()));
            aNetworkDiagTlv.mData.mIp6AddrList.mCount = ip6AddrList.GetLength() / OT_IP6_ADDRESS_SIZE;
            break;
        }

        case NetworkDiagnosticTlv::kMacCounters:
        {
            MacCountersTlv macCounters;

            SuccessOrExit(error = aMessage.Read(offset, macCounters));
            VerifyOrExit(macCounters.IsValid(), error = kErrorParse);

            ParseMacCounters(macCounters, aNetworkDiagTlv.mData.mMacCounters);
            break;
        }

        case NetworkDiagnosticTlv::kBatteryLevel:
            SuccessOrExit(error = Tlv::Read<BatteryLevelTlv>(aMessage, offset, aNetworkDiagTlv.mData.mBatteryLevel));
            break;

        case NetworkDiagnosticTlv::kSupplyVoltage:
            SuccessOrExit(error = Tlv::Read<SupplyVoltageTlv>(aMessage, offset, aNetworkDiagTlv.mData.mSupplyVoltage));
            break;

        case NetworkDiagnosticTlv::kChildTable:
        {
            ChildTableTlv &childTable = static_cast<ChildTableTlv &>(tlv);

            VerifyOrExit(childTable.IsValid(), error = kErrorParse);
            VerifyOrExit(childTable.GetNumEntries() <= OT_ARRAY_LENGTH(aNetworkDiagTlv.mData.mChildTable.mTable),
                         error = kErrorParse);

            for (uint8_t i = 0; i < childTable.GetNumEntries(); ++i)
            {
                ChildTableEntry childEntry;
                VerifyOrExit(childTable.ReadEntry(childEntry, aMessage, offset, i) == kErrorNone, error = kErrorParse);
                ParseChildEntry(childEntry, aNetworkDiagTlv.mData.mChildTable.mTable[i]);
            }
            aNetworkDiagTlv.mData.mChildTable.mCount = childTable.GetNumEntries();
            break;
        }

        case NetworkDiagnosticTlv::kChannelPages:
        {
            VerifyOrExit(sizeof(aNetworkDiagTlv.mData.mChannelPages.m8) >= tlv.GetLength(), error = kErrorParse);
            SuccessOrExit(
                error = aMessage.Read(offset + sizeof(tlv), aNetworkDiagTlv.mData.mChannelPages.m8, tlv.GetLength()));
            aNetworkDiagTlv.mData.mChannelPages.mCount = tlv.GetLength();
            break;
        }

        case NetworkDiagnosticTlv::kMaxChildTimeout:
            SuccessOrExit(error =
                              Tlv::Read<MaxChildTimeoutTlv>(aMessage, offset, aNetworkDiagTlv.mData.mMaxChildTimeout));
            break;

        default:
            // Ignore unrecognized Network Diagnostic TLV silently and
            // continue to top of the `while(true)` loop.
            offset += tlv.GetSize();
            continue;
        }

        // Exit if a TLV is recognized and parsed successfully.
        aNetworkDiagTlv.mType = tlv.GetType();
        aIterator             = static_cast<uint16_t>(offset - aMessage.GetOffset() + tlv.GetSize());
        ExitNow();
    }

exit:
    return error;
}

} // namespace NetworkDiagnostic

} // namespace ot

#endif // OPENTHREAD_FTD || OPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE
