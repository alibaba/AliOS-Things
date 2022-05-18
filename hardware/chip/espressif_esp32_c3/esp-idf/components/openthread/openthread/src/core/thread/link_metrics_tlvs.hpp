/*
 *  Copyright (c) 2020, The OpenThread Authors.
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
 *   This file includes definitions for generating and processing Link Metrics TLVs.
 *
 */

#ifndef LINK_METRICS_TLVS_HPP_
#define LINK_METRICS_TLVS_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE

#include <openthread/link_metrics.h>

#include "common/clearable.hpp"
#include "common/encoding.hpp"
#include "common/message.hpp"
#include "common/tlvs.hpp"

namespace ot {
namespace LinkMetrics {

/**
 * This type represents Link Metric Flags indicating a set of metrics.
 *
 * @sa otLinkMetrics
 *
 */
class Metrics : public otLinkMetrics, public Clearable<Metrics>
{
};

/**
 * This class defines constants related to Link Metrics Sub-TLVs.
 *
 */
class SubTlv
{
public:
    /**
     * Link Metrics Sub-TLV types.
     *
     */
    enum Type : uint8_t
    {
        kReport        = 0, ///< Report Sub-TLV
        kQueryId       = 1, ///< Query ID Sub-TLV
        kQueryOptions  = 2, ///< Query Options Sub-TLV
        kFwdProbingReg = 3, ///< Forward Probing Registration Sub-TLV
        kStatus        = 5, ///< Status Sub-TLV
        kEnhAckConfig  = 7, ///< Enhanced ACK Configuration Sub-TLV
    };
};

/**
 * This class defines Link Metrics Query ID Sub-TLV constants and types.
 *
 */
typedef UintTlvInfo<SubTlv::kQueryId, uint8_t> QueryIdSubTlv;

/**
 * This class implements Link Metrics Type ID Flags generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN class TypeIdFlags
{
    static constexpr uint8_t kExtendedFlag     = 1 << 7;
    static constexpr uint8_t kLengthOffset     = 6;
    static constexpr uint8_t kLengthFlag       = 1 << kLengthOffset;
    static constexpr uint8_t kTypeEnumOffset   = 3;
    static constexpr uint8_t kTypeEnumMask     = 7 << kTypeEnumOffset;
    static constexpr uint8_t kMetricEnumOffset = 0;
    static constexpr uint8_t kMetricEnumMask   = 7 << kMetricEnumOffset;

public:
    /**
     * This enumeration specifies the Length field in Type ID Flags.
     *
     */
    enum Length
    {
        kShortLength    = 0, ///< Short value length (1 byte value)
        kExtendedLength = 1, ///< Extended value length (4 bytes value)
    };

    /**
     * This enumeration specifies the Type values in Type ID Flags.
     *
     */
    enum TypeEnum : uint8_t
    {
        kTypeCountSummation   = 0, ///< Count or summation
        kTypeExpMovingAverage = 1, ///< Exponential moving average.
        kTypeReserved         = 2, ///< Reserved for future use.
    };

    /**
     * This enumeration specifies the Metric values in Type ID Flag.
     *
     */
    enum MetricEnum : uint8_t
    {
        kMetricPdusReceived = 0, ///< Number of PDUs received.
        kMetricLqi          = 1, ///< Link Quality Indicator.
        kMetricLinkMargin   = 2, ///< Link Margin.
        kMetricRssi         = 3, ///< RSSI in dbm.
    };

    /**
     * This constant defines the raw value for Type ID Flag for PDU.
     *
     */
    static constexpr uint8_t kPdu = (kExtendedLength << kLengthOffset) | (kTypeCountSummation << kTypeEnumOffset) |
                                    (kMetricPdusReceived << kMetricEnumOffset);

    /**
     * This constant defines the raw value for Type ID Flag for LQI.
     *
     */
    static constexpr uint8_t kLqi = (kShortLength << kLengthOffset) | (kTypeExpMovingAverage << kTypeEnumOffset) |
                                    (kMetricLqi << kMetricEnumOffset);

    /**
     * This constant defines the raw value for Type ID Flag for Link Margin.
     *
     */
    static constexpr uint8_t kLinkMargin = (kShortLength << kLengthOffset) |
                                           (kTypeExpMovingAverage << kTypeEnumOffset) |
                                           (kMetricLinkMargin << kMetricEnumOffset);

    /**
     * This constant defines the raw value for Type ID Flag for RSSI
     *
     */
    static constexpr uint8_t kRssi = (kShortLength << kLengthOffset) | (kTypeExpMovingAverage << kTypeEnumOffset) |
                                     (kMetricRssi << kMetricEnumOffset);

    /**
     * Default constructor.
     *
     */
    TypeIdFlags(void) = default;

    /**
     * Constructor to initialize from raw value.
     *
     * @param[in] aFlags  The raw flags value.
     *
     */
    explicit TypeIdFlags(uint8_t aFlags)
        : mFlags(aFlags)
    {
    }

    /**
     * This method initializes the Type ID value
     *
     */
    void Init(void) { mFlags = 0; }

    /**
     * This method clears the Extended flag.
     *
     */
    void ClearExtendedFlag(void) { mFlags &= ~kExtendedFlag; }

    /**
     * This method sets the Extended flag, indicating an additional second flags byte after the current 1-byte flags.
     * MUST NOT set in Thread 1.2.1.
     *
     */
    void SetExtendedFlag(void) { mFlags |= kExtendedFlag; }

    /**
     * This method indicates whether or not the Extended flag is set.
     *
     * @retval true   The Extended flag is set.
     * @retval false  The Extended flag is not set.
     *
     */
    bool IsExtendedFlagSet(void) const { return (mFlags & kExtendedFlag) != 0; }

    /**
     * This method clears value length flag.
     *
     */
    void ClearLengthFlag(void) { mFlags &= ~kLengthFlag; }

    /**
     * This method sets the value length flag.
     *
     */
    void SetLengthFlag(void) { mFlags |= kLengthFlag; }

    /**
     * This method indicates whether or not the value length flag is set.
     *
     * @retval true   The value length flag is set, extended value length (4 bytes)
     * @retval false  The value length flag is not set, short value length (1 byte)
     *
     */
    bool IsLengthFlagSet(void) const { return (mFlags & kLengthFlag) != 0; }

    /**
     * This method sets the Type/Average Enum.
     *
     * @param[in]  aTypeEnum  Type/Average Enum.
     *
     */
    void SetTypeEnum(TypeEnum aTypeEnum)
    {
        mFlags = (mFlags & ~kTypeEnumMask) | ((aTypeEnum << kTypeEnumOffset) & kTypeEnumMask);
    }

    /**
     * This method returns the Type/Average Enum.
     *
     * @returns The Type/Average Enum.
     *
     */
    TypeEnum GetTypeEnum(void) const { return static_cast<TypeEnum>((mFlags & kTypeEnumMask) >> kTypeEnumOffset); }

    /**
     * This method sets the Metric Enum.
     *
     * @param[in]  aMetricEnum  Metric Enum.
     *
     */
    void SetMetricEnum(MetricEnum aMetricEnum)
    {
        mFlags = (mFlags & ~kMetricEnumMask) | ((aMetricEnum << kMetricEnumOffset) & kMetricEnumMask);
    }

    /**
     * This method returns the Metric Enum.
     *
     * @returns The Metric Enum.
     *
     */
    MetricEnum GetMetricEnum(void) const
    {
        return static_cast<MetricEnum>((mFlags & kMetricEnumMask) >> kMetricEnumOffset);
    }

    /**
     * This method returns the raw value of the entire TypeIdFlags.
     *
     * @returns The raw value of TypeIdFlags.
     *
     */
    uint8_t GetRawValue(void) const { return mFlags; }

    /**
     * This method sets the raw value of the entire TypeIdFlags.
     *
     * @param[in]  aFlags  The raw flags value.
     *
     */
    void SetRawValue(uint8_t aFlags) { mFlags = aFlags; }

private:
    uint8_t mFlags;
} OT_TOOL_PACKED_END;

/**
 * This class implements Link Metrics Report Sub-TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class ReportSubTlv : public Tlv, public TlvInfo<SubTlv::kReport>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(SubTlv::kReport);
        SetLength(sizeof(*this) - sizeof(Tlv));
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval true   The TLV appears to be well-formed.
     * @retval false  The TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(TypeIdFlags) + sizeof(uint8_t); }

    /**
     * This method returns the Link Metrics Type ID.
     *
     * @returns The Link Metrics Type ID.
     *
     */
    TypeIdFlags GetMetricsTypeId(void) const { return mMetricsTypeId; }

    /**
     * This method sets the Link Metrics Type ID.
     *
     * @param[in]  aMetricsTypeID  The Link Metrics Type ID to set.
     *
     */
    void SetMetricsTypeId(TypeIdFlags aMetricsTypeId)
    {
        mMetricsTypeId = aMetricsTypeId;

        if (!aMetricsTypeId.IsLengthFlagSet())
        {
            SetLength(sizeof(*this) - sizeof(Tlv) - sizeof(uint32_t) + sizeof(uint8_t)); // The value is 1 byte long
        }
    }

    /**
     * This method returns the metric value in 8 bits.
     *
     * @returns The metric value.
     *
     */
    uint8_t GetMetricsValue8(void) const { return mMetricsValue.m8; }

    /**
     * This method returns the metric value in 32 bits.
     *
     * @returns The metric value.
     *
     */
    uint32_t GetMetricsValue32(void) const { return mMetricsValue.m32; }

    /**
     * This method sets the metric value (8 bits).
     *
     * @param[in]  aMetricsValue  Metrics value.
     *
     */
    void SetMetricsValue8(uint8_t aMetricsValue) { mMetricsValue.m8 = aMetricsValue; }

    /**
     * This method sets the metric value (32 bits).
     *
     * @param[in]  aMetricsValue  Metrics value.
     *
     */
    void SetMetricsValue32(uint32_t aMetricsValue) { mMetricsValue.m32 = aMetricsValue; }

private:
    TypeIdFlags mMetricsTypeId;
    union
    {
        uint8_t  m8;
        uint32_t m32;
    } mMetricsValue;
} OT_TOOL_PACKED_END;

/**
 * This class implements Link Metrics Query Options Sub-TLV generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class QueryOptionsSubTlv : public Tlv, public TlvInfo<SubTlv::kQueryOptions>
{
public:
    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(SubTlv::kQueryOptions);
        SetLength(0);
    }

    /**
     * This method indicates whether or not the TLV appears to be well-formed.
     *
     * @retval TRUE   If the TLV appears to be well-formed.
     * @retval FALSE  If the TLV does not appear to be well-formed.
     *
     */
    bool IsValid(void) const { return GetLength() >= sizeof(TypeIdFlags); }

} OT_TOOL_PACKED_END;

/**
 * This class implements Series Flags for Forward Tracking Series.
 *
 */
OT_TOOL_PACKED_BEGIN
class SeriesFlags
{
public:
    /**
     * This type represents which frames to be accounted in a Forward Tracking Series.
     *
     * @sa otLinkMetricsSeriesFlags
     *
     */
    typedef otLinkMetricsSeriesFlags Info;

    /**
     * Default constructor.
     *
     */
    SeriesFlags(void)
        : mFlags(0)
    {
    }

    /**
     * This method sets the values from an `Info` object.
     *
     * @param[in]  aSeriesFlags  The `Info` object.
     *
     */
    void SetFrom(const Info &aSeriesFlags)
    {
        Clear();

        if (aSeriesFlags.mLinkProbe)
        {
            SetLinkProbeFlag();
        }

        if (aSeriesFlags.mMacData)
        {
            SetMacDataFlag();
        }

        if (aSeriesFlags.mMacDataRequest)
        {
            SetMacDataRequestFlag();
        }

        if (aSeriesFlags.mMacAck)
        {
            SetMacAckFlag();
        }
    }

    /**
     * This method clears the Link Probe flag.
     *
     */
    void ClearLinkProbeFlag(void) { mFlags &= ~kLinkProbeFlag; }

    /**
     * This method sets the Link Probe flag.
     *
     */
    void SetLinkProbeFlag(void) { mFlags |= kLinkProbeFlag; }

    /**
     * This method indicates whether or not the Link Probe flag is set.
     *
     * @retval true   The Link Probe flag is set.
     * @retval false  The Link Probe flag is not set.
     *
     */
    bool IsLinkProbeFlagSet(void) const { return (mFlags & kLinkProbeFlag) != 0; }

    /**
     * This method clears the MAC Data flag.
     *
     */
    void ClearMacDataFlag(void) { mFlags &= ~kMacDataFlag; }

    /**
     * This method sets the MAC Data flag.
     *
     */
    void SetMacDataFlag(void) { mFlags |= kMacDataFlag; }

    /**
     * This method indicates whether or not the MAC Data flag is set.
     *
     * @retval true   The MAC Data flag is set.
     * @retval false  The MAC Data flag is not set.
     *
     */
    bool IsMacDataFlagSet(void) const { return (mFlags & kMacDataFlag) != 0; }

    /**
     * This method clears the MAC Data Request flag.
     *
     */
    void ClearMacDataRequestFlag(void) { mFlags &= ~kMacDataRequestFlag; }

    /**
     * This method sets the MAC Data Request flag.
     *
     */
    void SetMacDataRequestFlag(void) { mFlags |= kMacDataRequestFlag; }

    /**
     * This method indicates whether or not the MAC Data Request flag is set.
     *
     * @retval true   The MAC Data Request flag is set.
     * @retval false  The MAC Data Request flag is not set.
     *
     */
    bool IsMacDataRequestFlagSet(void) const { return (mFlags & kMacDataRequestFlag) != 0; }

    /**
     * This method clears the Mac Ack flag.
     *
     */
    void ClearMacAckFlag(void) { mFlags &= ~kMacAckFlag; }

    /**
     * This method sets the Mac Ack flag.
     *
     */
    void SetMacAckFlag(void) { mFlags |= kMacAckFlag; }

    /**
     * This method indicates whether or not the Mac Ack flag is set.
     *
     * @retval true   The Mac Ack flag is set.
     * @retval false  The Mac Ack flag is not set.
     *
     */
    bool IsMacAckFlagSet(void) const { return (mFlags & kMacAckFlag) != 0; }

    /**
     * This method returns the raw value of flags.
     *
     */
    uint8_t GetRawValue(void) const { return mFlags; }

    /**
     * This method clears the all the flags.
     *
     */
    void Clear(void) { mFlags = 0; }

private:
    static constexpr uint8_t kLinkProbeFlag      = 1 << 0;
    static constexpr uint8_t kMacDataFlag        = 1 << 1;
    static constexpr uint8_t kMacDataRequestFlag = 1 << 2;
    static constexpr uint8_t kMacAckFlag         = 1 << 3;

    uint8_t mFlags;
} OT_TOOL_PACKED_END;

/**
 * This enumeration type represent Enhanced-ACK Flags.
 *
 */
enum EnhAckFlags : uint8_t
{
    kEnhAckClear    = OT_LINK_METRICS_ENH_ACK_CLEAR,    ///< Clear.
    kEnhAckRegister = OT_LINK_METRICS_ENH_ACK_REGISTER, ///< Register.
};

static uint8_t TypeIdFlagsFromMetrics(TypeIdFlags aTypeIdFlags[], const Metrics &aMetrics)
{
    uint8_t count = 0;

    if (aMetrics.mPduCount)
    {
        aTypeIdFlags[count++].SetRawValue(TypeIdFlags::kPdu);
    }

    if (aMetrics.mLqi)
    {
        aTypeIdFlags[count++].SetRawValue(TypeIdFlags::kLqi);
    }

    if (aMetrics.mLinkMargin)
    {
        aTypeIdFlags[count++].SetRawValue(TypeIdFlags::kLinkMargin);
    }

    if (aMetrics.mRssi)
    {
        aTypeIdFlags[count++].SetRawValue(TypeIdFlags::kRssi);
    }

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    if (aMetrics.mReserved)
    {
        for (uint8_t i = 0; i < count; i++)
        {
            aTypeIdFlags[i].SetTypeEnum(TypeIdFlags::kTypeReserved);
        }
    }
#endif

    return count;
}

OT_TOOL_PACKED_BEGIN
class EnhAckConfigSubTlv : public Tlv, public TlvInfo<SubTlv::kEnhAckConfig>
{
public:
    /**
     * Default constructor
     *
     */
    EnhAckConfigSubTlv(void) { Init(); }

    /**
     * This method initializes the TLV.
     *
     */
    void Init(void)
    {
        SetType(SubTlv::kEnhAckConfig);
        SetLength(sizeof(EnhAckFlags));
    }

    /**
     * This method sets Enhanced ACK Flags.
     *
     * @param[in] aEnhAckFlags  The value of Enhanced ACK Flags.
     *
     */
    void SetEnhAckFlags(EnhAckFlags aEnhAckFlags)
    {
        memcpy(mSubTlvs + kEnhAckFlagsOffset, &aEnhAckFlags, sizeof(aEnhAckFlags));
    }

    /**
     * This method sets Type ID Flags.
     *
     * @param[in] aMetrics  A metrics flags to indicate the Type ID Flags.
     *
     */
    void SetTypeIdFlags(const Metrics &aMetrics)
    {
        uint8_t count;

        count = TypeIdFlagsFromMetrics(reinterpret_cast<TypeIdFlags *>(mSubTlvs + kTypeIdFlagsOffset), aMetrics);

        OT_ASSERT(count <= kMaxTypeIdFlagsEnhAck);

        SetLength(sizeof(EnhAckFlags) + sizeof(TypeIdFlags) * count);
    }

private:
    static constexpr uint8_t  kMaxTypeIdFlagsEnhAck = 3;
    static constexpr uint8_t  kEnhAckFlagsOffset    = 0;
    static constexpr uint16_t kTypeIdFlagsOffset    = sizeof(TypeIdFlags);

    uint8_t mSubTlvs[sizeof(EnhAckFlags) + sizeof(TypeIdFlags) * kMaxTypeIdFlagsEnhAck];
} OT_TOOL_PACKED_END;

} // namespace LinkMetrics
} // namespace ot

#endif // OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE

#endif // LINK_METRICS_TLVS_HPP_
