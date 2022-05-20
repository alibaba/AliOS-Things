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

#ifndef MLE_TLVS_HPP_
#define MLE_TLVS_HPP_

#include "openthread-core-config.h"

#include "common/encoding.hpp"
#include "common/message.hpp"
#include "common/tlvs.hpp"
#include "meshcop/timestamp.hpp"
#include "net/ip6_address.hpp"
#include "thread/link_metrics_tlvs.hpp"
#include "thread/mle_types.hpp"

namespace ot {

namespace Mle {

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
class Tlv : public ot::Tlv
{
public:
    /**
     * MLE TLV Types.
     *
     */
    enum Type : uint8_t
    {
        kSourceAddress         = 0,  ///< Source Address TLV
        kMode                  = 1,  ///< Mode TLV
        kTimeout               = 2,  ///< Timeout TLV
        kChallenge             = 3,  ///< Challenge TLV
        kResponse              = 4,  ///< Response TLV
        kLinkFrameCounter      = 5,  ///< Link-Layer Frame Counter TLV
        kLinkQuality           = 6,  ///< Link Quality TLV
        kNetworkParameter      = 7,  ///< Network Parameter TLV
        kMleFrameCounter       = 8,  ///< MLE Frame Counter TLV
        kRoute                 = 9,  ///< Route64 TLV
        kAddress16             = 10, ///< Address16 TLV
        kLeaderData            = 11, ///< Leader Data TLV
        kNetworkData           = 12, ///< Network Data TLV
        kTlvRequest            = 13, ///< TLV Request TLV
        kScanMask              = 14, ///< Scan Mask TLV
        kConnectivity          = 15, ///< Connectivity TLV
        kLinkMargin            = 16, ///< Link Margin TLV
        kStatus                = 17, ///< Status TLV
        kVersion               = 18, ///< Version TLV
        kAddressRegistration   = 19, ///< Address Registration TLV
        kChannel               = 20, ///< Channel TLV
        kPanId                 = 21, ///< PAN ID TLV
        kActiveTimestamp       = 22, ///< Active Timestamp TLV
        kPendingTimestamp      = 23, ///< Pending Timestamp TLV
        kActiveDataset         = 24, ///< Active Operational Dataset TLV
        kPendingDataset        = 25, ///< Pending Operational Dataset TLV
        kDiscovery             = 26, ///< Thread Discovery TLV
        kCslChannel            = 80, ///< CSL Channel TLV
        kCslTimeout            = 85, ///< CSL Timeout TLV
        kCslClockAccuracy      = 86, ///< CSL Clock Accuracy TLV
        kLinkMetricsQuery      = 87, ///< Link Metrics Query TLV
        kLinkMetricsManagement = 88, ///< Link Metrics Management TLV
        kLinkMetricsReport     = 89, ///< Link Metrics Report TLV
        kLinkProbe             = 90, ///< Link Probe TLV

        /**
         * Applicable/Required only when time synchronization service
         * (`OPENTHREAD_CONFIG_TIME_SYNC_ENABLE`) is enabled.
         *
         */
        kTimeRequest   = 252, ///< Time Request TLV
        kTimeParameter = 253, ///< Time Parameter TLV
        kXtalAccuracy  = 254, ///< XTAL Accuracy TLV

        kInvalid = 255,
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
 * This class defines Source Address TLV constants and types.
 *
 */
typedef UintTlvInfo<Tlv::kSourceAddress, uint16_t> SourceAddressTlv;

/**
 * This class defines Mode TLV constants and types.
 *
 */
typedef UintTlvInfo<Tlv::kMode, uint8_t> ModeTlv;

/**
 * This class defines Timeout TLV constants and types.
 *
 */
typedef UintTlvInfo<Tlv::kTimeout, uint32_t> TimeoutTlv;

/**
 * This class defines Challenge TLV constants and types.
 *
 */
typedef TlvInfo<Tlv::kChallenge> ChallengeTlv;

/**
 * This class defines Response TLV constants and types.
 *
 */
typedef TlvInfo<Tlv::kResponse> ResponseTlv;

/**
 * This class defines Link Frame Counter TLV constants and types.
 *
 */
typedef UintTlvInfo<Tlv::kLinkFrameCounter, uint32_t> LinkFrameCounterTlv;

/**
 * This class defines MLE Frame Counter TLV constants and types.
 *
 */
typedef UintTlvInfo<Tlv::kMleFrameCounter, uint32_t> MleFrameCounterTlv;

/**
 * This class defines Address16 TLV constants and types.
 *
 */
typedef UintTlvInfo<Tlv::kAddress16, uint16_t> Address16Tlv;

/**
 * This class defines Network Data TLV constants and types.
 *
 */
typedef TlvInfo<Tlv::kNetworkData> NetworkDataTlv;

/**
 * This class defines TLV Request TLV constants and types.
 *
 */
typedef TlvInfo<Tlv::kTlvRequest> TlvRequestTlv;

/**
 * This class defines Link Margin TLV constants and types.
 *
 */
typedef UintTlvInfo<Tlv::kLinkMargin, uint8_t> LinkMarginTlv;

/**
 * This class defines Version TLV constants and types.
 *
 */
typedef UintTlvInfo<Tlv::kVersion, uint16_t> VersionTlv;

/**
 * This class defines PAN ID TLV constants and types.
 *
 */
typedef UintTlvInfo<Tlv::kPanId, uint16_t> PanIdTlv;

/**
 * This class defines Active Timestamp TLV constants and types.
 *
 */
typedef SimpleTlvInfo<Tlv::kActiveTimestamp, MeshCoP::Timestamp> ActiveTimestampTlv;

/**
 * This class defines Pending Timestamp TLV constants and types.
 *
 */
typedef SimpleTlvInfo<Tlv::kPendingTimestamp, MeshCoP::Timestamp> PendingTimestampTlv;

/**
 * This class defines CSL Timeout TLV constants and types.
 *
 */
typedef UintTlvInfo<Tlv::kCslTimeout, uint32_t> CslTimeoutTlv;

/**
 * This class defines XTAL Accuracy TLV constants and types.
 *
 */
typedef UintTlvInfo<Tlv::kXtalAccuracy, uint16_t> XtalAccuracyTlv;

#if !OPENTHREAD_CONFIG_MLE_LONG_ROUTES_ENABLE

/**
 * This class implements Route TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class RouteTlv : public Tlv, public TlvInfo<Tlv::kRoute>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kRoute);
        SetLength(sizeof(*this) - sizeof(Tlv));
        mRouterIdMask.Clear();
        memset(mRouteData, 0, sizeof(mRouteData));
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
     * This method gets the Router ID Mask.
     *
     */
    const RouterIdSet &GetRouterIdMask(void) const { return mRouterIdMask; }

    /**
     * This method sets the Router ID Mask.
     *
     * @param[in]  aRouterIdSet The Router ID Mask to set.
     *
     */
    void SetRouterIdMask(const RouterIdSet &aRouterIdSet) { mRouterIdMask = aRouterIdSet; }

    /**
     * This method indicates whether or not a Router ID bit is set.
     *
     * @param[in]  aRouterId  The Router ID bit.
     *
     * @retval TRUE   If the Router ID bit is set.
     * @retval FALSE  If the Router ID bit is not set.
     *
     */
    bool IsRouterIdSet(uint8_t aRouterId) const { return mRouterIdMask.Contains(aRouterId); }

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

    uint8_t     mRouterIdSequence;
    RouterIdSet mRouterIdMask;
    uint8_t     mRouteData[kMaxRouterId + 1];
} OT_TOOL_PACKED_END;

#else // OPENTHREAD_CONFIG_MLE_LONG_ROUTES_ENABLE

/**
 * This class implements Route TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class RouteTlv : public Tlv, public TlvInfo<Tlv::kRoute>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kRoute);
        SetLength(sizeof(*this) - sizeof(Tlv));
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
     * This method gets the Router ID Mask.
     *
     */
    const RouterIdSet &GetRouterIdMask(void) const { return mRouterIdMask; }

    /**
     * This method sets the Router ID Mask.
     *
     * @param[in]  aRouterIdSet The Router ID Mask to set.
     *
     */
    void SetRouterIdMask(const RouterIdSet &aRouterIdSet) { mRouterIdMask = aRouterIdSet; }

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
     * @returns The Route Data Length value in bytes
     *
     */
    uint8_t GetRouteDataLength(void) const { return GetLength() - sizeof(mRouterIdSequence) - sizeof(mRouterIdMask); }

    /**
     * This method sets the Route Data Length value.
     *
     * @param[in]  aLength  The Route Data Length value in number of router entries
     *
     */
    void SetRouteDataLength(uint8_t aLength)
    {
        SetLength(sizeof(mRouterIdSequence) + sizeof(mRouterIdMask) + aLength + (aLength + 1) / 2);
    }

    /**
     * This method returns the Route Cost value for a given Router index.
     *
     * @param[in]  aRouterIndex  The Router index.
     *
     * @returns The Route Cost value for a given Router index.
     *
     */
    uint8_t GetRouteCost(uint8_t aRouterIndex) const
    {
        if (aRouterIndex & 1)
        {
            return mRouteData[aRouterIndex + aRouterIndex / 2 + 1];
        }
        else
        {
            return static_cast<uint8_t>((mRouteData[aRouterIndex + aRouterIndex / 2] & kRouteCostMask)
                                        << kOddEntryOffset) |
                   ((mRouteData[aRouterIndex + aRouterIndex / 2 + 1] &
                     static_cast<uint8_t>(kRouteCostMask << kOddEntryOffset)) >>
                    kOddEntryOffset);
        }
    }

    /**
     * This method sets the Route Cost value for a given Router index.
     *
     * @param[in]  aRouterIndex  The Router index.
     * @param[in]  aRouteCost    The Route Cost value.
     *
     */
    void SetRouteCost(uint8_t aRouterIndex, uint8_t aRouteCost)
    {
        if (aRouterIndex & 1)
        {
            mRouteData[aRouterIndex + aRouterIndex / 2 + 1] = aRouteCost;
        }
        else
        {
            mRouteData[aRouterIndex + aRouterIndex / 2] =
                (mRouteData[aRouterIndex + aRouterIndex / 2] & ~kRouteCostMask) |
                ((aRouteCost >> kOddEntryOffset) & kRouteCostMask);
            mRouteData[aRouterIndex + aRouterIndex / 2 + 1] = static_cast<uint8_t>(
                (mRouteData[aRouterIndex + aRouterIndex / 2 + 1] & ~(kRouteCostMask << kOddEntryOffset)) |
                ((aRouteCost & kRouteCostMask) << kOddEntryOffset));
        }
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
        int offset = ((aRouterIndex & 1) ? kOddEntryOffset : 0);
        return (mRouteData[aRouterIndex + aRouterIndex / 2] & (kLinkQualityInMask >> offset)) >>
               (kLinkQualityInOffset - offset);
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
        int offset = ((aRouterIndex & 1) ? kOddEntryOffset : 0);
        mRouteData[aRouterIndex + aRouterIndex / 2] =
            (mRouteData[aRouterIndex + aRouterIndex / 2] & ~(kLinkQualityInMask >> offset)) |
            ((aLinkQuality << (kLinkQualityInOffset - offset)) & (kLinkQualityInMask >> offset));
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
        int offset = ((aRouterIndex & 1) ? kOddEntryOffset : 0);
        return (mRouteData[aRouterIndex + aRouterIndex / 2] & (kLinkQualityOutMask >> offset)) >>
               (kLinkQualityOutOffset - offset);
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
        int offset = ((aRouterIndex & 1) ? kOddEntryOffset : 0);
        mRouteData[aRouterIndex + aRouterIndex / 2] =
            (mRouteData[aRouterIndex + aRouterIndex / 2] & ~(kLinkQualityOutMask >> offset)) |
            ((aLinkQuality << (kLinkQualityOutOffset - offset)) & (kLinkQualityOutMask >> offset));
    }

private:
    static constexpr uint8_t kLinkQualityOutOffset = 6;
    static constexpr uint8_t kLinkQualityOutMask   = 3 << kLinkQualityOutOffset;
    static constexpr uint8_t kLinkQualityInOffset  = 4;
    static constexpr uint8_t kLinkQualityInMask    = 3 << kLinkQualityInOffset;
    static constexpr uint8_t kRouteCostOffset      = 0;
    static constexpr uint8_t kRouteCostMask        = 0xf << kRouteCostOffset;
    static constexpr uint8_t kOddEntryOffset       = 4;

    uint8_t     mRouterIdSequence;
    RouterIdSet mRouterIdMask;
    // Since we do hold 12 (compressible to 11) bits of data per router, each entry occupies 1.5 bytes,
    // consecutively. First 4 bits are link qualities, remaining 8 bits are route cost.
    uint8_t mRouteData[kMaxRouterId + 1 + kMaxRouterId / 2 + 1];
} OT_TOOL_PACKED_END;

#endif // OPENTHREAD_CONFIG_MLE_LONG_ROUTES_ENABLE

/**
 * This class implements Leader Data TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class LeaderDataTlv : public Tlv, public TlvInfo<Tlv::kLeaderData>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kLeaderData);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * This method gets the Leader Data info from TLV.
     *
     * @param[out] aLeaderData   A reference to output Leader Data info.
     *
     */
    void Get(LeaderData &aLeaderData) const
    {
        aLeaderData.SetPartitionId(HostSwap32(mPartitionId));
        aLeaderData.SetWeighting(mWeighting);
        aLeaderData.SetDataVersion(mDataVersion);
        aLeaderData.SetStableDataVersion(mStableDataVersion);
        aLeaderData.SetLeaderRouterId(mLeaderRouterId);
    }

    /**
     * This method sets the Leader Data.
     *
     * @param[in] aLeaderData   A Leader Data.
     *
     */
    void Set(const LeaderData &aLeaderData)
    {
        mPartitionId       = HostSwap32(aLeaderData.GetPartitionId());
        mWeighting         = aLeaderData.GetWeighting();
        mDataVersion       = aLeaderData.GetDataVersion();
        mStableDataVersion = aLeaderData.GetStableDataVersion();
        mLeaderRouterId    = aLeaderData.GetLeaderRouterId();
    }

private:
    uint32_t mPartitionId;
    uint8_t  mWeighting;
    uint8_t  mDataVersion;
    uint8_t  mStableDataVersion;
    uint8_t  mLeaderRouterId;
} OT_TOOL_PACKED_END;

/**
 * This class implements Scan Mask TLV generation and parsing.
 *
 */
class ScanMaskTlv : public UintTlvInfo<Tlv::kScanMask, uint8_t>
{
public:
    static constexpr uint8_t kRouterFlag    = 1 << 7; ///< Scan Mask Router Flag.
    static constexpr uint8_t kEndDeviceFlag = 1 << 6; ///< Scan Mask End Device Flag.

    /**
     * This method indicates whether or not the Router flag is set.
     *
     * @param[in] aMask   A scan mask value.
     *
     * @retval TRUE   If the Router flag is set.
     * @retval FALSE  If the Router flag is not set.
     */
    static bool IsRouterFlagSet(uint8_t aMask) { return (aMask & kRouterFlag) != 0; }

    /**
     * This method indicates whether or not the End Device flag is set.
     *
     * @param[in] aMask   A scan mask value.
     *
     * @retval TRUE   If the End Device flag is set.
     * @retval FALSE  If the End Device flag is not set.
     */
    static bool IsEndDeviceFlagSet(uint8_t aMask) { return (aMask & kEndDeviceFlag) != 0; }
};

/**
 * This class implements Connectivity TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class ConnectivityTlv : public Tlv, public TlvInfo<Tlv::kConnectivity>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kConnectivity);
        SetLength(sizeof(*this) - sizeof(Tlv));
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
        return IsSedBufferingIncluded() ||
               (GetLength() == sizeof(*this) - sizeof(Tlv) - sizeof(mSedBufferSize) - sizeof(mSedDatagramCount));
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
    int8_t GetParentPriority(void) const
    {
        return (static_cast<int8_t>(mParentPriority & kParentPriorityMask)) >> kParentPriorityOffset;
    }

    /**
     * This method sets the Parent Priority value.
     *
     * @param[in] aParentPriority  The Parent Priority value.
     *
     */
    void SetParentPriority(int8_t aParentPriority)
    {
        mParentPriority = (static_cast<uint8_t>(aParentPriority) << kParentPriorityOffset) & kParentPriorityMask;
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
 * This class specifies Status TLV status values.
 *
 */
struct StatusTlv : public UintTlvInfo<Tlv::kStatus, uint8_t>
{
    /**
     * Status values.
     */
    enum Status : uint8_t
    {
        kError = 1, ///< Error.
    };
};

/**
 * This class implements Source Address TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class AddressRegistrationEntry
{
public:
    /**
     * This method returns the IPv6 address or IID length.
     *
     * @returns The IPv6 address length if the Compressed bit is clear, or the IID length if the Compressed bit is
     *          set.
     *
     */
    uint8_t GetLength(void) const { return sizeof(mControl) + (IsCompressed() ? sizeof(mIid) : sizeof(mIp6Address)); }

    /**
     * This method indicates whether or not the Compressed flag is set.
     *
     * @retval TRUE   If the Compressed flag is set.
     * @retval FALSE  If the Compressed flag is not set.
     *
     */
    bool IsCompressed(void) const { return (mControl & kCompressed) != 0; }

    /**
     * This method sets the Uncompressed flag.
     *
     */
    void SetUncompressed(void) { mControl = 0; }

    /**
     * This method returns the Context ID for the compressed form.
     *
     * @returns The Context ID value.
     *
     */
    uint8_t GetContextId(void) const { return mControl & kCidMask; }

    /**
     * This method sets the Context ID value.
     *
     * @param[in]  aContextId  The Context ID value.
     *
     */
    void SetContextId(uint8_t aContextId) { mControl = kCompressed | aContextId; }

    /**
     * This method returns the IID value.
     *
     * @returns The IID value.
     *
     */
    const Ip6::InterfaceIdentifier &GetIid(void) const { return mIid; }

    /**
     * This method sets the IID value.
     *
     * @param[in]  aIid  The IID value.
     *
     */
    void SetIid(const Ip6::InterfaceIdentifier &aIid) { mIid = aIid; }

    /**
     * This method returns the IPv6 Address value.
     *
     * @returns The IPv6 Address value.
     *
     */
    const Ip6::Address &GetIp6Address(void) const { return mIp6Address; }

    /**
     * This method sets the IPv6 Address value.
     *
     * @param[in]  aAddress  A reference to the IPv6 Address value.
     *
     */
    void SetIp6Address(const Ip6::Address &aAddress) { mIp6Address = aAddress; }

private:
    static constexpr uint8_t kCompressed = 1 << 7;
    static constexpr uint8_t kCidMask    = 0xf;

    uint8_t mControl;
    union
    {
        Ip6::InterfaceIdentifier mIid;
        Ip6::Address             mIp6Address;
    } OT_TOOL_PACKED_FIELD;
} OT_TOOL_PACKED_END;

/**
 * This class implements Channel TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class ChannelTlv : public Tlv, public TlvInfo<Tlv::kChannel>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kChannel);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * This method returns the Channel Page value.
     *
     * @returns The Channel Page value.
     *
     */
    uint8_t GetChannelPage(void) const { return mChannelPage; }

    /**
     * This method sets the Channel Page value.
     *
     * @param[in]  aChannelPage  The Channel Page value.
     *
     */
    void SetChannelPage(uint8_t aChannelPage) { mChannelPage = aChannelPage; }

    /**
     * This method returns the Channel value.
     *
     * @returns The Channel value.
     *
     */
    uint16_t GetChannel(void) const { return HostSwap16(mChannel); }

    /**
     * This method sets the Channel value.
     *
     * @param[in]  aChannel  The Channel value.
     *
     */
    void SetChannel(uint16_t aChannel) { mChannel = HostSwap16(aChannel); }

private:
    uint8_t  mChannelPage;
    uint16_t mChannel;
} OT_TOOL_PACKED_END;

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
/**
 * This class defines Time Request TLV constants and types.
 *
 */
typedef TlvInfo<Tlv::kTimeRequest> TimeRequestTlv;

/**
 * This class implements Time Parameter TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class TimeParameterTlv : public Tlv, public TlvInfo<Tlv::kTimeParameter>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kTimeParameter);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(*this) - sizeof(Tlv); }

    /**
     * This method returns the time sync period.
     *
     * @returns The time sync period.
     *
     */
    uint16_t GetTimeSyncPeriod(void) const { return HostSwap16(mTimeSyncPeriod); }

    /**
     * This method sets the time sync period.
     *
     * @param[in]  aTimeSyncPeriod  The time sync period.
     *
     */
    void SetTimeSyncPeriod(uint16_t aTimeSyncPeriod) { mTimeSyncPeriod = HostSwap16(aTimeSyncPeriod); }

    /**
     * This method returns the XTAL accuracy threshold.
     *
     * @returns The XTAL accuracy threshold.
     *
     */
    uint16_t GetXtalThreshold(void) const { return HostSwap16(mXtalThreshold); }

    /**
     * This method sets the XTAL accuracy threshold.
     *
     * @param[in]  aXTALThreshold  The XTAL accuracy threshold.
     *
     */
    void SetXtalThreshold(uint16_t aXtalThreshold) { mXtalThreshold = HostSwap16(aXtalThreshold); }

private:
    uint16_t mTimeSyncPeriod;
    uint16_t mXtalThreshold;
} OT_TOOL_PACKED_END;

#endif // OPENTHREAD_CONFIG_TIME_SYNC_ENABLE

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE || (OPENTHREAD_FTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE)
/**
 * This class implements CSL Channel TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class CslChannelTlv : public Tlv, public TlvInfo<Tlv::kCslChannel>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kCslChannel);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() == sizeof(*this) - sizeof(Tlv); }

    /**
     * This method returns the Channel Page value.
     *
     * @returns The Channel Page value.
     *
     */
    uint8_t GetChannelPage(void) const { return mChannelPage; }

    /**
     * This method sets the Channel Page value.
     *
     * @param[in]  aChannelPage  The Channel Page value.
     *
     */
    void SetChannelPage(uint8_t aChannelPage) { mChannelPage = aChannelPage; }

    /**
     * This method returns the Channel value.
     *
     * @returns The Channel value.
     *
     */
    uint16_t GetChannel(void) const { return HostSwap16(mChannel); }

    /**
     * This method sets the Channel value.
     *
     * @param[in]  aChannel  The Channel value.
     *
     */
    void SetChannel(uint16_t aChannel) { mChannel = HostSwap16(aChannel); }

private:
    uint8_t  mChannelPage;
    uint16_t mChannel;
} OT_TOOL_PACKED_END;

#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE || (OPENTHREAD_FTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE)

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE || OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
/**
 * This class implements CSL Clock Accuracy TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class CslClockAccuracyTlv : public Tlv, public TlvInfo<Tlv::kCslClockAccuracy>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(kCslClockAccuracy);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method returns the CSL Clock Accuracy value.
     *
     * @returns The CSL Clock Accuracy value.
     *
     */
    uint8_t GetCslClockAccuracy(void) { return mCslClockAccuracy; }

    /**
     * This method sets the CSL Clock Accuracy value.
     *
     * @param[in]  aCslClockAccuracy  The CSL Clock Accuracy value.
     *
     */
    void SetCslClockAccuracy(uint8_t aCslClockAccuracy) { mCslClockAccuracy = aCslClockAccuracy; }

    /**
     * This method returns the Clock Accuracy value.
     *
     * @returns The Clock Accuracy value.
     *
     */
    uint8_t GetCslUncertainty(void) { return mCslUncertainty; }

    /**
     * This method sets the CSL Uncertainty value.
     *
     * @param[in]  aCslUncertainty  The CSL Uncertainty value.
     *
     */
    void SetCslUncertainty(uint8_t aCslUncertainty) { mCslUncertainty = aCslUncertainty; }

private:
    uint8_t mCslClockAccuracy;
    uint8_t mCslUncertainty;
} OT_TOOL_PACKED_END;

#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE || OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
/**
 * @}
 *
 */

} // namespace Mle

} // namespace ot

#endif // MLE_TLVS_HPP_
