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
 *   This file includes helper functions to convert between public OT C structs and corresponding core C++ classes.
 */

#ifndef AS_CORE_TYPE_HPP_
#define AS_CORE_TYPE_HPP_

#include "openthread-core-config.h"

#include "common/instance.hpp"
#include "crypto/sha256.hpp"
#include "mac/mac_filter.hpp"
#include "net/dns_types.hpp"

namespace ot {

/**
 * This structure relates a given public OT type to its corresponding core C++ class/type.
 *
 * @tparam FromType  The public OT type.
 *
 * Specializations of this template struct are provided for different `FromType` which include a member type definition
 * named `Type` to provide the corresponding core class/type related to `FromType.
 *
 * For example, `CoreType<otIp6Address>::Type` is defined as `Ip6::Address`.
 *
 */
template <typename FromType> struct CoreType;

/**
 * This function converts a pointer to public OT type (C struct) to the corresponding core C++ type reference.
 *
 * @tparam Type   The public OT type to convert.
 *
 * @param[in] aObject   A pointer to the object to convert.
 *
 * @returns A reference of the corresponding C++ type matching @p aObject.
 *
 */
template <typename Type> typename CoreType<Type>::Type &AsCoreType(Type *aObject)
{
    return *static_cast<typename CoreType<Type>::Type *>(aObject);
}

/**
 * This function converts a const pointer to public OT type (C struct) to the corresponding core C++ type reference.
 *
 * @tparam Type   The public OT type to convert.
 *
 * @param[in] aObject   A const pointer to the object to convert.
 *
 * @returns A const reference of the corresponding C++ type matching @p aObject.
 *
 */
template <typename Type> const typename CoreType<Type>::Type &AsCoreType(const Type *aObject)
{
    return *static_cast<const typename CoreType<Type>::Type *>(aObject);
}

/**
 * This function converts a pointer to public OT type (C struct) to the corresponding core C++ type pointer.
 *
 * @tparam Type   The public OT type to convert.
 *
 * @param[in] aObject   A pointer to the object to convert.
 *
 * @returns A pointer of the corresponding C++ type matching @p aObject.
 *
 */
template <typename Type> typename CoreType<Type>::Type *AsCoreTypePtr(Type *aObject)
{
    return static_cast<typename CoreType<Type>::Type *>(aObject);
}

/**
 * This function converts a const pointer to public OT type (C struct) to the corresponding core C++ type pointer.
 *
 * @tparam Type   The public OT type to convert.
 *
 * @param[in] aObject   A pointer to the object to convert.
 *
 * @returns A const pointer of the corresponding C++ type matching @p aObject.
 *
 */
template <typename Type> const typename CoreType<Type>::Type *AsCoreTypePtr(const Type *aObject)
{
    return static_cast<const typename CoreType<Type>::Type *>(aObject);
}

#if OPENTHREAD_FTD || OPENTHREAD_MTD

/**
 * This method casts an `otMessage` pointer to a `Coap::Message` reference.
 *
 * @param[in] aMessage   A pointer to an `otMessage`.
 *
 * @returns A reference to `Coap::Message` matching @p aMessage.
 *
 */
inline Coap::Message &AsCoapMessage(otMessage *aMessage)
{
    return *static_cast<Coap::Message *>(aMessage);
}

/**
 * This method casts an `otMessage` pointer to a `Coap::Message` reference.
 *
 * @param[in] aMessage   A pointer to an `otMessage`.
 *
 * @returns A reference to `Coap::Message` matching @p aMessage.
 *
 */
inline Coap::Message *AsCoapMessagePtr(otMessage *aMessage)
{
    return static_cast<Coap::Message *>(aMessage);
}

/**
 * This method casts an `otMessage` pointer to a `Coap::Message` pointer.
 *
 * @param[in] aMessage   A pointer to an `otMessage`.
 *
 * @returns A pointer to `Coap::Message` matching @p aMessage.
 *
 */
inline const Coap::Message &AsCoapMessage(const otMessage *aMessage)
{
    return *static_cast<const Coap::Message *>(aMessage);
}

/**
 * This method casts an `otMessage` pointer to a `Coap::Message` reference.
 *
 * @param[in] aMessage   A pointer to an `otMessage`.
 *
 * @returns A pointer to `Coap::Message` matching @p aMessage.
 *
 */
inline const Coap::Message *AsCoapMessagePtr(const otMessage *aMessage)
{
    return static_cast<const Coap::Message *>(aMessage);
}

#endif // #if OPENTHREAD_FTD || OPENTHREAD_MTD

/**
 * This structure maps two enumeration types.
 *
 * @tparam FromEnumType  The enum type.
 *
 * Specializations of this template struct are provided which include a member type definition named `Type` to provide
 * the related `enum` type mapped with `FromEnumType`.
 *
 * For example, `MappedEnum<otMacFilterAddressMode>::Type` is defined as `Mac::Filter::Mode`.
 *
 */
template <typename FromEnumType> struct MappedEnum;

/**
 * This function convert an enumeration type value to a related enumeration type value.
 *
 * @param[in] aValue   The enumeration value to convert
 *
 * @returns The matching enumeration value.
 *
 */
template <typename EnumType> const typename MappedEnum<EnumType>::Type MapEnum(EnumType aValue)
{
    return static_cast<typename MappedEnum<EnumType>::Type>(aValue);
}

//---------------------------------------------------------------------------------------------------------------------
// Private macros and definitions

// Helper macro to define specialization of `CoreType` struct relating `BaseType` with `SubType`.
#define DefineCoreType(BaseType, SubType) \
    template <> struct CoreType<BaseType> \
    {                                     \
        using Type = SubType;             \
    }

// Helper macro to map two related enumeration types.
#define DefineMapEnum(FirstEnumType, SecondEnumType) \
    template <> struct MappedEnum<FirstEnumType>     \
    {                                                \
        using Type = SecondEnumType;                 \
    };                                               \
                                                     \
    template <> struct MappedEnum<SecondEnumType>    \
    {                                                \
        using Type = FirstEnumType;                  \
    }

DefineCoreType(otInstance, Instance);

// Message
DefineCoreType(otMessage, Message);
DefineCoreType(otMessageSettings, Message::Settings);
DefineCoreType(otMessageBuffer, Buffer);
DefineCoreType(otMessageQueue, MessageQueue);

// Mac
DefineCoreType(otRadioFrame, Mac::Frame);
DefineCoreType(otExtAddress, Mac::ExtAddress);
DefineCoreType(otMacKey, Mac::Key);
DefineCoreType(otExtendedPanId, Mac::ExtendedPanId);
DefineCoreType(otNetworkName, Mac::NetworkName);
#if OPENTHREAD_CONFIG_MAC_FILTER_ENABLE
DefineMapEnum(otMacFilterAddressMode, Mac::Filter::Mode);
#endif

DefineCoreType(otCryptoKey, Crypto::Key);
DefineCoreType(otCryptoSha256Hash, Crypto::Sha256::Hash);

#if OPENTHREAD_FTD || OPENTHREAD_MTD

// Thread
DefineCoreType(otThreadLinkInfo, ThreadLinkInfo);
DefineCoreType(otSecurityPolicy, SecurityPolicy);
DefineCoreType(otNetworkKey, NetworkKey);
DefineCoreType(otPskc, Pskc);

DefineCoreType(otNeighborInfo, Neighbor::Info);
DefineCoreType(otRouterInfo, Router::Info);
#if OPENTHREAD_FTD
DefineCoreType(otChildInfo, Child::Info);
#endif

// Ip6
DefineCoreType(otIp6Prefix, Ip6::Prefix);
DefineCoreType(otIp6InterfaceIdentifier, Ip6::InterfaceIdentifier);
DefineCoreType(otIp6Address, Ip6::Address);
DefineCoreType(otMessageInfo, Ip6::MessageInfo);
DefineCoreType(otSockAddr, Ip6::SockAddr);
DefineCoreType(otNetifAddress, Ip6::Netif::UnicastAddress);
DefineCoreType(otNetifMulticastAddress, Ip6::Netif::MulticastAddress);
DefineCoreType(otIcmp6Header, Ip6::Icmp::Header);
DefineCoreType(otIcmp6Handler, Ip6::Icmp::Handler);
DefineCoreType(otUdpSocket, Ip6::Udp::SocketHandle);
DefineCoreType(otUdpReceiver, Ip6::Udp::Receiver);
DefineCoreType(otTcpEndpoint, Ip6::Tcp::Endpoint);
DefineCoreType(otTcpListener, Ip6::Tcp::Listener);

// Mle
DefineCoreType(otMeshLocalPrefix, Mle::MeshLocalPrefix);
DefineCoreType(otLeaderData, Mle::LeaderData);
DefineMapEnum(otDeviceRole, Mle::DeviceRole);

// LinkMetrics
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
DefineCoreType(otLinkMetrics, LinkMetrics::Metrics);
DefineCoreType(otLinkMetricsValues, LinkMetrics::MetricsValues);
DefineMapEnum(otLinkMetricsEnhAckFlags, LinkMetrics::EnhAckFlags);
#endif

// NetworkData
DefineCoreType(otBorderRouterConfig, NetworkData::OnMeshPrefixConfig);
DefineCoreType(otExternalRouteConfig, NetworkData::ExternalRouteConfig);
DefineCoreType(otServiceConfig, NetworkData::ServiceConfig);
DefineCoreType(otServerConfig, NetworkData::ServiceConfig::ServerConfig);

// MeshCoP
DefineCoreType(otOperationalDatasetComponents, MeshCoP::Dataset::Components);
DefineCoreType(otOperationalDataset, MeshCoP::Dataset::Info);
DefineCoreType(otJoinerPskd, MeshCoP::JoinerPskd);
DefineCoreType(otJoinerDiscerner, MeshCoP::JoinerDiscerner);
DefineCoreType(otSteeringData, MeshCoP::SteeringData);
#if OPENTHREAD_CONFIG_JOINER_ENABLE
DefineMapEnum(otJoinerState, MeshCoP::Joiner::State);
#endif
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
DefineMapEnum(otCommissionerState, MeshCoP::Commissioner::State);
#endif
#if OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
DefineMapEnum(otBorderAgentState, MeshCoP::BorderAgent::State);
#endif

// Coap
DefineCoreType(otCoapTxParameters, Coap::TxParameters);
DefineCoreType(otCoapResource, Coap::Resource);
DefineCoreType(otCoapOption, Coap::Option);
DefineCoreType(otCoapOptionIterator, Coap::Option::Iterator);
DefineMapEnum(otCoapType, Coap::Type);
DefineMapEnum(otCoapCode, Coap::Code);
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
DefineCoreType(otCoapBlockwiseResource, Coap::ResourceBlockWise);
#endif

// Dns
DefineCoreType(otDnsTxtEntry, Dns::TxtEntry);
DefineCoreType(otDnsTxtEntryIterator, Dns::TxtEntry::Iterator);
#if OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
DefineCoreType(otDnsQueryConfig, Dns::Client::QueryConfig);
DefineCoreType(otDnsAddressResponse, Dns::Client::AddressResponse);
#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE
DefineCoreType(otDnsBrowseResponse, Dns::Client::BrowseResponse);
DefineCoreType(otDnsServiceResponse, Dns::Client::ServiceResponse);
DefineCoreType(otDnsServiceInfo, Dns::Client::ServiceInfo);
#endif
#endif
#if OPENTHREAD_CONFIG_DNSSD_SERVER_ENABLE
DefineMapEnum(otDnssdQueryType, Dns::ServiceDiscovery::Server::DnsQueryType);
#endif

// Srp
#if OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
DefineCoreType(otSrpClientHostInfo, Srp::Client::HostInfo);
DefineCoreType(otSrpClientService, Srp::Client::Service);
DefineMapEnum(otSrpClientItemState, Srp::Client::ItemState);
#endif
#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
DefineCoreType(otSrpServerLeaseConfig, Srp::Server::LeaseConfig);
DefineCoreType(otSrpServerHost, Srp::Server::Host);
DefineCoreType(otSrpServerService, Srp::Server::Service);
DefineMapEnum(otSrpServerState, Srp::Server::State);
DefineMapEnum(otSrpServerAddressMode, Srp::Server::AddressMode);
#endif

// Utils
#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
DefineCoreType(otHistoryTrackerIterator, Utils::HistoryTracker::Iterator);
DefineCoreType(otHistoryTrackerNetworkInfo, Utils::HistoryTracker::NetworkInfo);
DefineCoreType(otHistoryTrackerMessageInfo, Utils::HistoryTracker::MessageInfo);
DefineCoreType(otHistoryTrackerNeighborInfo, Utils::HistoryTracker::NeighborInfo);
#endif
#if OPENTHREAD_CONFIG_PING_SENDER_ENABLE
DefineCoreType(otPingSenderReply, Utils::PingSender::Reply);
DefineCoreType(otPingSenderConfig, Utils::PingSender::Config);
DefineCoreType(otPingSenderStatistics, Utils::PingSender::Statistics);
#endif
#if OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_ENABLE
DefineCoreType(otSrpClientBuffersServiceEntry, Utils::SrpClientBuffers::ServiceEntry);
#endif

#endif // #if OPENTHREAD_FTD || OPENTHREAD_MTD

} // namespace ot

#endif // AS_CORE_TYPE_HPP_
