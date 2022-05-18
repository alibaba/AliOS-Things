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
 *   This file includes definitions for generating and processing MLE TLVs.
 */

#ifndef NETWORK_DIAGNOSTIC_TLVS_HPP_
#define NETWORK_DIAGNOSTIC_TLVS_HPP_

#include "openthread-core-config.h"

#include <openthread/thread.h>

#include "common/encoding.hpp"
#include "common/message.hpp"
#include "common/tlvs.hpp"
#include "net/ip6_address.hpp"
#include "radio/radio.hpp"
#include "thread/mle_types.hpp"

namespace ot {

namespace NetworkDiagnostic {

using ot::Encoding::BigEndian::HostSwap16;
using ot::Encoding::BigEndian::HostSwap32;

/**
 * @addtogroup core-mle-tlvs
 *
 * @brief
 *   This module includes definitions for generating and processing MLE TLVs.
 *
 * @{
 *
 */

/**
 * This class implements MLE TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class NetworkDiagnosticTlv : public ot::Tlv
{
public:
    /**
     * MLE TLV Types.
     *
     */
    enum Type : uint8_t
    {
        kExtMacAddress   = OT_NETWORK_DIAGNOSTIC_TLV_EXT_ADDRESS,
        kAddress16       = OT_NETWORK_DIAGNOSTIC_TLV_SHORT_ADDRESS,
        kMode            = OT_NETWORK_DIAGNOSTIC_TLV_MODE,
        kTimeout         = OT_NETWORK_DIAGNOSTIC_TLV_TIMEOUT,
        kConnectivity    = OT_NETWORK_DIAGNOSTIC_TLV_CONNECTIVITY,
        kRoute           = OT_NETWORK_DIAGNOSTIC_TLV_ROUTE,
        kLeaderData      = OT_NETWORK_DIAGNOSTIC_TLV_LEADER_DATA,
        kNetworkData     = OT_NETWORK_DIAGNOSTIC_TLV_NETWORK_DATA,
        kIp6AddressList  = OT_NETWORK_DIAGNOSTIC_TLV_IP6_ADDR_LIST,
        kMacCounters     = OT_NETWORK_DIAGNOSTIC_TLV_MAC_COUNTERS,
        kBatteryLevel    = OT_NETWORK_DIAGNOSTIC_TLV_BATTERY_LEVEL,
        kSupplyVoltage   = OT_NETWORK_DIAGNOSTIC_TLV_SUPPLY_VOLTAGE,
        kChildTable      = OT_NETWORK_DIAGNOSTIC_TLV_CHILD_TABLE,
        kChannelPages    = OT_NETWORK_DIAGNOSTIC_TLV_CHANNEL_PAGES,
        kTypeList        = OT_NETWORK_DIAGNOSTIC_TLV_TYPE_LIST,
        kMaxChildTimeout = OT_NETWORK_DIAGNOSTIC_TLV_MAX_CHILD_TIMEOUT,
    };

    /**
     * This method returns the Type value.
     *
     * @returns The Type value.
     *
     */
    Type GetType(void) const { return static_cast<Type>(ot::Tlv::GetType()); }

    /**
     * This method sets the Type value.
     *
     * @param[in]  aType  The Type value.
     *
     */
    void SetType(Type aType) { ot::Tlv::SetType(static_cast<uint8_t>(aType)); }

} OT_TOOL_PACKED_END;

/**
 * This class defines Extended MAC Address TLV constants and types.
 *
 */
typedef SimpleTlvInfo<NetworkDiagnosticTlv::kExtMacAddress, Mac::ExtAddress> ExtMacAddressTlv;

/**
 * This class defines Address16 TLV constants and types.
 *
 */
typedef UintTlvInfo<NetworkDiagnosticTlv::kAddress16, uint16_t> Address16Tlv;

/**
 * This class defines Mode TLV constants and types.
 *
 */
typedef UintTlvInfo<NetworkDiagnosticTlv::kMode, uint8_t> ModeTlv;

/**
 * This class defines Timeout TLV constants and types.
 *
 */
typedef UintTlvInfo<NetworkDiagnosticTlv::kTimeout, uint32_t> TimeoutTlv;

/**
 * This class defines Battery Level TLV constants and types.
 *
 */
typedef UintTlvInfo<NetworkDiagnosticTlv::kBatteryLevel, uint8_t> BatteryLevelTlv;

/**
 * This class defines Supply Voltage TLV constants and types.
 *
 */
typedef UintTlvInfo<NetworkDiagnosticTlv::kSupplyVoltage, uint16_t> SupplyVoltageTlv;

/**
 * This class defines Max Child Timeout TLV constants and types.
 *
 */
typedef UintTlvInfo<NetworkDiagnosticTlv::kMaxChildTimeout, uint32_t> MaxChildTimeoutTlv;

/**
 * This class implements Connectivity TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class ConnectivityTlv : public NetworkDiagnosticTlv, public TlvInfo<NetworkDiagnosticTlv::kConnectivity>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kConnectivity);
        SetLength(sizeof(*this) - sizeof(NetworkDiagnosticTlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const
    {
        return IsSedBufferingIncluded() || (GetLength() == sizeof(*this) - sizeof(NetworkDiagnosticTlv) -
                                                               sizeof(mSedBufferSize) - sizeof(mSedDatagramCount));
    }

    /**
     * This method indicates whether or not the sed buffer size and datagram count are included.
     *
     * @retval TRUE   If the sed buffer size and datagram count are included.
     * @retval FALSE  If the sed buffer size and datagram count are not included.
     *
     */
    bool IsSedBufferingIncluded(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * This method returns the Parent Priority value.
     *
     * @returns The Parent Priority value.
     *
     */
    int8_t GetParentPriority(void) const { return (mParentPriority & kParentPriorityMask) >> kParentPriorityOffset; }

    /**
     * This method sets the Parent Priority value.
     *
     * @param[in] aParentPriority  The Parent Priority value.
     *
     */
    void SetParentPriority(int8_t aParentPriority)
    {
        mParentPriority = (aParentPriority << kParentPriorityOffset) & kParentPriorityMask;
    }

    /**
     * This method returns the Link Quality 3 value.
     *
     * @returns The Link Quality 3 value.
     *
     */
    uint8_t GetLinkQuality3(void) const { return mLinkQuality3; }

    /**
     * This method sets the Link Quality 3 value.
     *
     * @param[in]  aLinkQuality  The Link Quality 3 value.
     *
     */
    void SetLinkQuality3(uint8_t aLinkQuality) { mLinkQuality3 = aLinkQuality; }

    /**
     * This method returns the Link Quality 2 value.
     *
     * @returns The Link Quality 2 value.
     *
     */
    uint8_t GetLinkQuality2(void) const { return mLinkQuality2; }

    /**
     * This method sets the Link Quality 2 value.
     *
     * @param[in]  aLinkQuality  The Link Quality 2 value.
     *
     */
    void SetLinkQuality2(uint8_t aLinkQuality) { mLinkQuality2 = aLinkQuality; }

    /**
     * This method sets the Link Quality 1 value.
     *
     * @returns The Link Quality 1 value.
     *
     */
    uint8_t GetLinkQuality1(void) const { return mLinkQuality1; }

    /**
     * This method sets the Link Quality 1 value.
     *
     * @param[in]  aLinkQuality  The Link Quality 1 value.
     *
     */
    void SetLinkQuality1(uint8_t aLinkQuality) { mLinkQuality1 = aLinkQuality; }

    /**
     * This method sets the Active Routers value.
     *
     * @returns The Active Routers value.
     *
     */
    uint8_t GetActiveRouters(void) const { return mActiveRouters; }

    /**
     * This method sets the Active Routers value.
     *
     * @param[in]  aActiveRouters  The Active Routers value.
     *
     */
    void SetActiveRouters(uint8_t aActiveRouters) { mActiveRouters = aActiveRouters; }

    /**
     * This method returns the Leader Cost value.
     *
     * @returns The Leader Cost value.
     *
     */
    uint8_t GetLeaderCost(void) const { return mLeaderCost; }

    /**
     * This method sets the Leader Cost value.
     *
     * @param[in]  aCost  The Leader Cost value.
     *
     */
    void SetLeaderCost(uint8_t aCost) { mLeaderCost = aCost; }

    /**
     * This method returns the ID Sequence value.
     *
     * @returns The ID Sequence value.
     *
     */
    uint8_t GetIdSequence(void) const { return mIdSequence; }

    /**
     * This method sets the ID Sequence value.
     *
     * @param[in]  aSequence  The ID Sequence value.
     *
     */
    void SetIdSequence(uint8_t aSequence) { mIdSequence = aSequence; }

    /**
     * This method returns the SED Buffer Size value.
     *
     * @returns The SED Buffer Size value.
     *
     */
    uint16_t GetSedBufferSize(void) const
    {
        uint16_t buffersize = OPENTHREAD_CONFIG_DEFAULT_SED_BUFFER_SIZE;

        if (IsSedBufferingIncluded())
        {
            buffersize = HostSwap16(mSedBufferSize);
        }
        return buffersize;
    }

    /**
     * This method sets the SED Buffer Size value.
     *
     * @param[in]  aSedBufferSize  The SED Buffer Size value.
     *
     */
    void SetSedBufferSize(uint16_t aSedBufferSize) { mSedBufferSize = HostSwap16(aSedBufferSize); }

    /**
     * This method returns the SED Datagram Count value.
     *
     * @returns The SED Datagram Count value.
     *
     */
    uint8_t GetSedDatagramCount(void) const
    {
        uint8_t count = OPENTHREAD_CONFIG_DEFAULT_SED_DATAGRAM_COUNT;

        if (IsSedBufferingIncluded())
        {
            count = mSedDatagramCount;
        }
        return count;
    }

    /**
     * This method sets the SED Datagram Count value.
     *
     * @param[in]  aSedDatagramCount  The SED Datagram Count value.
     *
     */
    void SetSedDatagramCount(uint8_t aSedDatagramCount) { mSedDatagramCount = aSedDatagramCount; }

private:
    static constexpr uint8_t kParentPriorityOffset = 6;
    static constexpr uint8_t kParentPriorityMask   = 3 << kParentPriorityOffset;

    uint8_t  mParentPriority;
    uint8_t  mLinkQuality3;
    uint8_t  mLinkQuality2;
    uint8_t  mLinkQuality1;
    uint8_t  mLeaderCost;
    uint8_t  mIdSequence;
    uint8_t  mActiveRouters;
    uint16_t mSedBufferSize;
    uint8_t  mSedDatagramCount;
} OT_TOOL_PACKED_END;

/**
 * This class implements Route TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class RouteTlv : public NetworkDiagnosticTlv, public TlvInfo<NetworkDiagnosticTlv::kRoute>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kRoute);
        SetLength(sizeof(*this) - sizeof(NetworkDiagnosticTlv));
        mRouterIdMask.Clear();
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(mRouterIdSequence) + sizeof(mRouterIdMask); }

    /**
     * This method returns the Router ID Sequence value.
     *
     * @returns The Router ID Sequence value.
     *
     */
    uint8_t GetRouterIdSequence(void) const { return mRouterIdSequence; }

    /**
     * This method sets the Router ID Sequence value.
     *
     * @param[in]  aSequence  The Router ID Sequence value.
     *
     */
    void SetRouterIdSequence(uint8_t aSequence) { mRouterIdSequence = aSequence; }

    /**
     * This method indicates whether or not a Router ID bit is set.
     *
     * @param[in]  aRouterId  The Router ID.
     *
     * @retval TRUE   If the Router ID bit is set.
     * @retval FALSE  If the Router ID bit is not set.
     *
     */
    bool IsRouterIdSet(uint8_t aRouterId) const { return mRouterIdMask.Contains(aRouterId); }

    /**
     * This method sets the Router ID bit.
     *
     * @param[in]  aRouterId  The Router ID bit to set.
     *
     */
    void SetRouterId(uint8_t aRouterId) { mRouterIdMask.Add(aRouterId); }

    /**
     * This method returns the Route Data Length value.
     *
     * @returns The Route Data Length value.
     *
     */
    uint8_t GetRouteDataLength(void) const { return GetLength() - sizeof(mRouterIdSequence) - sizeof(mRouterIdMask); }

    /**
     * This method sets the Route Data Length value.
     *
     * @param[in]  aLength  The Route Data Length value.
     *
     */
    void SetRouteDataLength(uint8_t aLength) { SetLength(sizeof(mRouterIdSequence) + sizeof(mRouterIdMask) + aLength); }

    /**
     * This method returns the Route Cost value for a given Router index.
     *
     * @param[in]  aRouterIndex  The Router index.
     *
     * @returns The Route Cost value for a given Router index.
     *
     */
    uint8_t GetRouteCost(uint8_t aRouterIndex) const { return mRouteData[aRouterIndex] & kRouteCostMask; }

    /**
     * This method sets the Route Cost value for a given Router index.
     *
     * @param[in]  aRouterIndex  The Router index.
     * @param[in]  aRouteCost    The Route Cost value.
     *
     */
    void SetRouteCost(uint8_t aRouterIndex, uint8_t aRouteCost)
    {
        mRouteData[aRouterIndex] = (mRouteData[aRouterIndex] & ~kRouteCostMask) | aRouteCost;
    }

    /**
     * This method returns the Link Quality In value for a given Router index.
     *
     * @param[in]  aRouterIndex  The Router index.
     *
     * @returns The Link Quality In value for a given Router index.
     *
     */
    uint8_t GetLinkQualityIn(uint8_t aRouterIndex) const
    {
        return (mRouteData[aRouterIndex] & kLinkQualityInMask) >> kLinkQualityInOffset;
    }

    /**
     * This method sets the Link Quality In value for a given Router index.
     *
     * @param[in]  aRouterIndex  The Router index.
     * @param[in]  aLinkQuality  The Link Quality In value for a given Router index.
     *
     */
    void SetLinkQualityIn(uint8_t aRouterIndex, uint8_t aLinkQuality)
    {
        mRouteData[aRouterIndex] = (mRouteData[aRouterIndex] & ~kLinkQualityInMask) |
                                   ((aLinkQuality << kLinkQualityInOffset) & kLinkQualityInMask);
    }

    /**
     * This method returns the Link Quality Out value for a given Router index.
     *
     * @param[in]  aRouterIndex  The Router index.
     *
     * @returns The Link Quality Out value for a given Router index.
     *
     */
    uint8_t GetLinkQualityOut(uint8_t aRouterIndex) const
    {
        return (mRouteData[aRouterIndex] & kLinkQualityOutMask) >> kLinkQualityOutOffset;
    }

    /**
     * This method sets the Link Quality Out value for a given Router index.
     *
     * @param[in]  aRouterIndex  The Router index.
     * @param[in]  aLinkQuality  The Link Quality Out value for a given Router index.
     *
     */
    void SetLinkQualityOut(uint8_t aRouterIndex, uint8_t aLinkQuality)
    {
        mRouteData[aRouterIndex] = (mRouteData[aRouterIndex] & ~kLinkQualityOutMask) |
                                   ((aLinkQuality << kLinkQualityOutOffset) & kLinkQualityOutMask);
    }

private:
    static constexpr uint8_t kLinkQualityOutOffset = 6;
    static constexpr uint8_t kLinkQualityOutMask   = 3 << kLinkQualityOutOffset;
    static constexpr uint8_t kLinkQualityInOffset  = 4;
    static constexpr uint8_t kLinkQualityInMask    = 3 << kLinkQualityInOffset;
    static constexpr uint8_t kRouteCostOffset      = 0;
    static constexpr uint8_t kRouteCostMask        = 0xf << kRouteCostOffset;

    uint8_t          mRouterIdSequence;
    Mle::RouterIdSet mRouterIdMask;
    uint8_t          mRouteData[Mle::kMaxRouterId + 1];
} OT_TOOL_PACKED_END;

/**
 * This class implements Leader Data TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class LeaderDataTlv : public NetworkDiagnosticTlv, public TlvInfo<NetworkDiagnosticTlv::kLeaderData>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kLeaderData);
        SetLength(sizeof(*this) - sizeof(NetworkDiagnosticTlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(NetworkDiagnosticTlv); }

    /**
     * This method returns the Partition ID value.
     *
     * @returns The Partition ID value.
     *
     */
    uint32_t GetPartitionId(void) const { return HostSwap32(mPartitionId); }

    /**
     * This method sets the Partition ID value.
     *
     * @param[in]  aPartitionId  The Partition ID value.
     *
     */
    void SetPartitionId(uint32_t aPartitionId) { mPartitionId = HostSwap32(aPartitionId); }

    /**
     * This method returns the Weighting value.
     *
     * @returns The Weighting value.
     *
     */
    uint8_t GetWeighting(void) const { return mWeighting; }

    /**
     * This method sets the Weighting value.
     *
     * @param[in]  aWeighting  The Weighting value.
     *
     */
    void SetWeighting(uint8_t aWeighting) { mWeighting = aWeighting; }

    /**
     * This method returns the Data Version value.
     *
     * @returns The Data Version value.
     *
     */
    uint8_t GetDataVersion(void) const { return mDataVersion; }

    /**
     * This method sets the Data Version value.
     *
     * @param[in]  aVersion  The Data Version value.
     *
     */
    void SetDataVersion(uint8_t aVersion) { mDataVersion = aVersion; }

    /**
     * This method returns the Stable Data Version value.
     *
     * @returns The Stable Data Version value.
     *
     */
    uint8_t GetStableDataVersion(void) const { return mStableDataVersion; }

    /**
     * This method sets the Stable Data Version value.
     *
     * @param[in]  aVersion  The Stable Data Version value.
     *
     */
    void SetStableDataVersion(uint8_t aVersion) { mStableDataVersion = aVersion; }

    /**
     * This method returns the Leader Router ID value.
     *
     * @returns The Leader Router ID value.
     *
     */
    uint8_t GetLeaderRouterId(void) const { return mLeaderRouterId; }

    /**
     * This method sets the Leader Router ID value.
     *
     * @param[in]  aRouterId  The Leader Router ID value.
     *
     */
    void SetLeaderRouterId(uint8_t aRouterId) { mLeaderRouterId = aRouterId; }

private:
    uint32_t mPartitionId;
    uint8_t  mWeighting;
    uint8_t  mDataVersion;
    uint8_t  mStableDataVersion;
    uint8_t  mLeaderRouterId;
} OT_TOOL_PACKED_END;

/**
 * This class implements Network Data TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class NetworkDataTlv : public NetworkDiagnosticTlv, public TlvInfo<NetworkDiagnosticTlv::kNetworkData>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kNetworkData);
        SetLength(sizeof(*this) - sizeof(NetworkDiagnosticTlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() < sizeof(*this) - sizeof(NetworkDiagnosticTlv); }

    /**
     * This method returns a pointer to the Network Data.
     *
     * @returns A pointer to the Network Data.
     *
     */
    uint8_t *GetNetworkData(void) { return mNetworkData; }

    /**
     * This method sets the Network Data.
     *
     * @param[in]  aNetworkData  A pointer to the Network Data.
     *
     */
    void SetNetworkData(const uint8_t *aNetworkData) { memcpy(mNetworkData, aNetworkData, GetLength()); }

private:
    uint8_t mNetworkData[255];
} OT_TOOL_PACKED_END;

/**
 * This class implements IPv6 Address List TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class Ip6AddressListTlv : public NetworkDiagnosticTlv, public TlvInfo<NetworkDiagnosticTlv::kIp6AddressList>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kIp6AddressList);
        SetLength(sizeof(*this) - sizeof(NetworkDiagnosticTlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return !IsExtended() && (GetLength() % sizeof(Ip6::Address) == 0); }

    /**
     * This method returns a pointer to the IPv6 address entry.
     *
     * @param[in]  aIndex  The index into the IPv6 address list.
     *
     * @returns A reference to the IPv6 address.
     *
     */
    const Ip6::Address &GetIp6Address(uint8_t aIndex) const
    {
        return *reinterpret_cast<const Ip6::Address *>(GetValue() + (aIndex * sizeof(Ip6::Address)));
    }

} OT_TOOL_PACKED_END;

/**
 * This class implements Mac Counters TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class MacCountersTlv : public NetworkDiagnosticTlv, public TlvInfo<NetworkDiagnosticTlv::kMacCounters>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kMacCounters);
        SetLength(sizeof(*this) - sizeof(NetworkDiagnosticTlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(NetworkDiagnosticTlv); }

    /**
     * This method returns the IfInUnknownProtos counter.
     *
     * @returns The IfInUnknownProtos counter
     *
     */
    uint32_t GetIfInUnknownProtos(void) const { return HostSwap32(mIfInUnknownProtos); }

    /**
     * This method sets the IfInUnknownProtos counter.
     *
     * @param[in]  aIfInUnknownProtos The IfInUnknownProtos counter
     *
     */
    void SetIfInUnknownProtos(const uint32_t aIfInUnknownProtos)
    {
        mIfInUnknownProtos = HostSwap32(aIfInUnknownProtos);
    }

    /**
     * This method returns the IfInErrors counter.
     *
     * @returns The IfInErrors counter
     *
     */
    uint32_t GetIfInErrors(void) const { return HostSwap32(mIfInErrors); }

    /**
     * This method sets the IfInErrors counter.
     *
     * @param[in]  aIfInErrors The IfInErrors counter
     *
     */
    void SetIfInErrors(const uint32_t aIfInErrors) { mIfInErrors = HostSwap32(aIfInErrors); }

    /**
     * This method returns the IfOutErrors counter.
     *
     * @returns The IfOutErrors counter
     *
     */
    uint32_t GetIfOutErrors(void) const { return HostSwap32(mIfOutErrors); }

    /**
     * This method sets the IfOutErrors counter.
     *
     * @param[in]  aIfOutErrors The IfOutErrors counter.
     *
     */
    void SetIfOutErrors(const uint32_t aIfOutErrors) { mIfOutErrors = HostSwap32(aIfOutErrors); }

    /**
     * This method returns the IfInUcastPkts counter.
     *
     * @returns The IfInUcastPkts counter
     *
     */
    uint32_t GetIfInUcastPkts(void) const { return HostSwap32(mIfInUcastPkts); }

    /**
     * This method sets the IfInUcastPkts counter.
     *
     * @param[in]  aIfInUcastPkts The IfInUcastPkts counter.
     *
     */
    void SetIfInUcastPkts(const uint32_t aIfInUcastPkts) { mIfInUcastPkts = HostSwap32(aIfInUcastPkts); }
    /**
     * This method returns the IfInBroadcastPkts counter.
     *
     * @returns The IfInBroadcastPkts counter
     *
     */
    uint32_t GetIfInBroadcastPkts(void) const { return HostSwap32(mIfInBroadcastPkts); }

    /**
     * This method sets the IfInBroadcastPkts counter.
     *
     * @param[in]  aIfInBroadcastPkts The IfInBroadcastPkts counter.
     *
     */
    void SetIfInBroadcastPkts(const uint32_t aIfInBroadcastPkts)
    {
        mIfInBroadcastPkts = HostSwap32(aIfInBroadcastPkts);
    }

    /**
     * This method returns the IfInDiscards counter.
     *
     * @returns The IfInDiscards counter
     *
     */
    uint32_t GetIfInDiscards(void) const { return HostSwap32(mIfInDiscards); }

    /**
     * This method sets the IfInDiscards counter.
     *
     * @param[in]  aIfInDiscards The IfInDiscards counter.
     *
     */
    void SetIfInDiscards(const uint32_t aIfInDiscards) { mIfInDiscards = HostSwap32(aIfInDiscards); }

    /**
     * This method returns the IfOutUcastPkts counter.
     *
     * @returns The IfOutUcastPkts counter
     *
     */
    uint32_t GetIfOutUcastPkts(void) const { return HostSwap32(mIfOutUcastPkts); }

    /**
     * This method sets the IfOutUcastPkts counter.
     *
     * @param[in]  aIfOutUcastPkts The IfOutUcastPkts counter.
     *
     */
    void SetIfOutUcastPkts(const uint32_t aIfOutUcastPkts) { mIfOutUcastPkts = HostSwap32(aIfOutUcastPkts); }

    /**
     * This method returns the IfOutBroadcastPkts counter.
     *
     * @returns The IfOutBroadcastPkts counter
     *
     */
    uint32_t GetIfOutBroadcastPkts(void) const { return HostSwap32(mIfOutBroadcastPkts); }

    /**
     * This method sets the IfOutBroadcastPkts counter.
     *
     * @param[in]  aIfOutBroadcastPkts The IfOutBroadcastPkts counter.
     *
     */
    void SetIfOutBroadcastPkts(const uint32_t aIfOutBroadcastPkts)
    {
        mIfOutBroadcastPkts = HostSwap32(aIfOutBroadcastPkts);
    }

    /**
     * This method returns the IfOutDiscards counter.
     *
     * @returns The IfOutDiscards counter
     *
     */
    uint32_t GetIfOutDiscards(void) const { return HostSwap32(mIfOutDiscards); }

    /**
     * This method sets the IfOutDiscards counter.
     *
     * @param[in]  aIfOutDiscards The IfOutDiscards counter.
     *
     */
    void SetIfOutDiscards(const uint32_t aIfOutDiscards) { mIfOutDiscards = HostSwap32(aIfOutDiscards); }

private:
    uint32_t mIfInUnknownProtos;
    uint32_t mIfInErrors;
    uint32_t mIfOutErrors;
    uint32_t mIfInUcastPkts;
    uint32_t mIfInBroadcastPkts;
    uint32_t mIfInDiscards;
    uint32_t mIfOutUcastPkts;
    uint32_t mIfOutBroadcastPkts;
    uint32_t mIfOutDiscards;
} OT_TOOL_PACKED_END;

/**
 * This class implements Child Table Entry generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class ChildTableEntry
{
public:
    /**
     * Default constructor.
     *
     */
    ChildTableEntry(void)
        : mTimeoutRsvChildId(0)
        , mMode(0)
    {
    }

    /**
     * This method returns the Timeout value.
     *
     * @returns The Timeout value.
     *
     */
    uint8_t GetTimeout(void) const { return (HostSwap16(mTimeoutRsvChildId) & kTimeoutMask) >> kTimeoutOffset; }

    /**
     * This method sets the Timeout value.
     *
     * @param[in]  aTimeout  The Timeout value.
     *
     */
    void SetTimeout(uint8_t aTimeout)
    {
        mTimeoutRsvChildId = HostSwap16((HostSwap16(mTimeoutRsvChildId) & ~kTimeoutMask) |
                                        ((aTimeout << kTimeoutOffset) & kTimeoutMask));
    }

    /**
     * This method returns the Child ID value.
     *
     * @returns The Child ID value.
     *
     */
    uint16_t GetChildId(void) const { return HostSwap16(mTimeoutRsvChildId) & kChildIdMask; }

    /**
     * This method sets the Child ID value.
     *
     * @param[in]  aChildId  The Child ID value.
     *
     */
    void SetChildId(uint16_t aChildId)
    {
        mTimeoutRsvChildId = HostSwap16((HostSwap16(mTimeoutRsvChildId) & ~kChildIdMask) | (aChildId & kChildIdMask));
    }

    /**
     * This method returns the Device Mode
     *
     * @returns The Device Mode
     *
     */
    Mle::DeviceMode GetMode(void) const { return Mle::DeviceMode(mMode); }

    /**
     * This method sets the Device Mode.
     *
     * @param[in]  aMode  The Device Mode.
     *
     */
    void SetMode(Mle::DeviceMode aMode) { mMode = aMode.Get(); }

    /**
     * This method returns the Reserved value.
     *
     * @returns The Reserved value.
     *
     */
    uint8_t GetReserved(void) const { return (HostSwap16(mTimeoutRsvChildId) & kReservedMask) >> kReservedOffset; }

    /**
     * This method sets the Reserved value.
     *
     * @param[in]  aReserved  The Reserved value.
     *
     */
    void SetReserved(uint8_t aReserved)
    {
        mTimeoutRsvChildId = HostSwap16((HostSwap16(mTimeoutRsvChildId) & ~kReservedMask) |
                                        ((aReserved << kReservedOffset) & kReservedMask));
    }

private:
    static constexpr uint8_t  kTimeoutOffset  = 11;
    static constexpr uint8_t  kReservedOffset = 9;
    static constexpr uint16_t kTimeoutMask    = 0xf800;
    static constexpr uint16_t kReservedMask   = 0x0600;
    static constexpr uint16_t kChildIdMask    = 0x1f;

    uint16_t mTimeoutRsvChildId;
    uint8_t  mMode;
} OT_TOOL_PACKED_END;

/**
 * This class implements Child Table TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class ChildTableTlv : public NetworkDiagnosticTlv, public TlvInfo<NetworkDiagnosticTlv::kChildTable>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kChildTable);
        SetLength(sizeof(*this) - sizeof(NetworkDiagnosticTlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return (GetLength() % sizeof(ChildTableEntry)) == 0; }

    /**
     * This method returns the number of Child Table entries.
     *
     * @returns The number of Child Table entries.
     *
     */
    uint8_t GetNumEntries(void) const { return GetLength() / sizeof(ChildTableEntry); }

    /**
     * This method returns the Child Table entry at @p aIndex.
     *
     * @param[in]  aIndex  The index into the Child Table list.
     *
     * @returns  A reference to the Child Table entry.
     *
     */
    ChildTableEntry &GetEntry(uint16_t aIndex)
    {
        return *reinterpret_cast<ChildTableEntry *>(GetValue() + (aIndex * sizeof(ChildTableEntry)));
    }

    /**
     * This method reads the Child Table entry at @p aIndex.
     *
     * @param[out]  aEntry      A reference to a ChildTableEntry.
     * @param[in]   aMessage    A reference to the message.
     * @param[in]   aOffset     The offset of the ChildTableTLV in aMessage.
     * @param[in]   aIndex      The index into the Child Table list.
     *
     * @retval  kErrorNotFound   No such entry is found.
     * @retval  kErrorNone       Successfully read the entry.
     *
     */
    Error ReadEntry(ChildTableEntry &aEntry, const Message &aMessage, uint16_t aOffset, uint8_t aIndex) const
    {
        return ((aIndex < GetNumEntries()) &&
                (aMessage.Read(aOffset + sizeof(ChildTableTlv) + (aIndex * sizeof(ChildTableEntry)), aEntry) ==
                 kErrorNone))
                   ? kErrorNone
                   : kErrorInvalidArgs;
    }

} OT_TOOL_PACKED_END;

/**
 * This class implements Channel Pages TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class ChannelPagesTlv : public NetworkDiagnosticTlv, public TlvInfo<NetworkDiagnosticTlv::kChannelPages>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kChannelPages);
        SetLength(sizeof(*this) - sizeof(NetworkDiagnosticTlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const
    {
        // At least one channel page must be included.
        return GetLength() >= 1;
    }

    /**
     * This method returns a pointer to the list of Channel Pages.
     *
     * @returns A pointer to the list of Channel Pages.
     *
     */
    uint8_t *GetChannelPages(void) { return mChannelPages; }

private:
    uint8_t mChannelPages[Radio::kNumChannelPages];
} OT_TOOL_PACKED_END;

/**
 * This class implements IPv6 Address List TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class TypeListTlv : public NetworkDiagnosticTlv, public TlvInfo<NetworkDiagnosticTlv::kTypeList>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kTypeList);
        SetLength(sizeof(*this) - sizeof(NetworkDiagnosticTlv));
    }
} OT_TOOL_PACKED_END;

/**
 * @}
 *
 */

} // namespace NetworkDiagnostic

} // namespace ot

#endif // NETWORK_DIAGNOSTIC_TLVS_HPP_
