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
 *   This file includes definitions for Thread Link Metrics.
 */

#include "link_metrics.hpp"

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE

#include "common/code_utils.hpp"
#include "common/encoding.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "mac/mac.hpp"
#include "thread/link_metrics_tlvs.hpp"
#include "thread/neighbor_table.hpp"

namespace ot {
namespace LinkMetrics {

using ot::Encoding::BigEndian::HostSwap32;

void SeriesInfo::Init(uint8_t aSeriesId, const SeriesFlags &aSeriesFlags, const Metrics &aMetrics)
{
    mSeriesId    = aSeriesId;
    mSeriesFlags = aSeriesFlags;
    mMetrics     = aMetrics;
    mRssAverager.Clear();
    mLqiAverager.Clear();
    mPduCount = 0;
}

void SeriesInfo::AggregateLinkMetrics(uint8_t aFrameType, uint8_t aLqi, int8_t aRss)
{
    if (IsFrameTypeMatch(aFrameType))
    {
        mPduCount++;
        mLqiAverager.Add(aLqi);
        IgnoreError(mRssAverager.Add(aRss));
    }
}

bool SeriesInfo::IsFrameTypeMatch(uint8_t aFrameType) const
{
    bool match = false;

    switch (aFrameType)
    {
    case kSeriesTypeLinkProbe:
        VerifyOrExit(!mSeriesFlags.IsMacDataFlagSet()); // Ignore this when Mac Data is accounted
        match = mSeriesFlags.IsLinkProbeFlagSet();
        break;
    case Mac::Frame::kFcfFrameData:
        match = mSeriesFlags.IsMacDataFlagSet();
        break;
    case Mac::Frame::kFcfFrameMacCmd:
        match = mSeriesFlags.IsMacDataRequestFlagSet();
        break;
    case Mac::Frame::kFcfFrameAck:
        match = mSeriesFlags.IsMacAckFlagSet();
        break;
    default:
        break;
    }

exit:
    return match;
}

LinkMetrics::LinkMetrics(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mReportCallback(nullptr)
    , mReportCallbackContext(nullptr)
    , mMgmtResponseCallback(nullptr)
    , mMgmtResponseCallbackContext(nullptr)
    , mEnhAckProbingIeReportCallback(nullptr)
    , mEnhAckProbingIeReportCallbackContext(nullptr)
{
}

Error LinkMetrics::Query(const Ip6::Address &aDestination, uint8_t aSeriesId, const Metrics *aMetrics)
{
    Error       error;
    TypeIdFlags typeIdFlags[kMaxTypeIdFlags];
    uint8_t     typeIdFlagsCount = 0;
    Neighbor *  neighbor         = GetNeighborFromLinkLocalAddr(aDestination);

    VerifyOrExit(neighbor != nullptr, error = kErrorUnknownNeighbor);
    VerifyOrExit(neighbor->IsThreadVersion1p2(), error = kErrorNotCapable);

    if (aMetrics != nullptr)
    {
        typeIdFlagsCount = TypeIdFlagsFromMetrics(typeIdFlags, *aMetrics);
    }

    if (aSeriesId != 0)
    {
        VerifyOrExit(typeIdFlagsCount == 0, error = kErrorInvalidArgs);
    }

    error = SendLinkMetricsQuery(aDestination, aSeriesId, typeIdFlags, typeIdFlagsCount);

exit:
    return error;
}

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
Error LinkMetrics::SendMgmtRequestForwardTrackingSeries(const Ip6::Address &     aDestination,
                                                        uint8_t                  aSeriesId,
                                                        const SeriesFlags::Info &aSeriesFlags,
                                                        const Metrics *          aMetrics)
{
    Error        error = kErrorNone;
    uint8_t      subTlvs[sizeof(Tlv) + sizeof(uint8_t) * 2 + sizeof(TypeIdFlags) * kMaxTypeIdFlags];
    Tlv *        fwdProbingSubTlv  = reinterpret_cast<Tlv *>(subTlvs);
    SeriesFlags *seriesFlags       = reinterpret_cast<SeriesFlags *>(subTlvs + sizeof(Tlv) + sizeof(aSeriesId));
    uint8_t      typeIdFlagsOffset = sizeof(Tlv) + sizeof(uint8_t) * 2;
    uint8_t      typeIdFlagsCount  = 0;
    Neighbor *   neighbor          = GetNeighborFromLinkLocalAddr(aDestination);

    VerifyOrExit(neighbor != nullptr, error = kErrorUnknownNeighbor);
    VerifyOrExit(neighbor->IsThreadVersion1p2(), error = kErrorNotCapable);

    // Directly transform `aMetrics` into TypeIdFlags and put them into `subTlvs`
    if (aMetrics != nullptr)
    {
        typeIdFlagsCount =
            TypeIdFlagsFromMetrics(reinterpret_cast<TypeIdFlags *>(subTlvs + typeIdFlagsOffset), *aMetrics);
    }

    VerifyOrExit(aSeriesId > kQueryIdSingleProbe, error = kErrorInvalidArgs);

    fwdProbingSubTlv->SetType(SubTlv::kFwdProbingReg);

    // SeriesId + SeriesFlags + typeIdFlagsCount * TypeIdFlags
    fwdProbingSubTlv->SetLength(sizeof(uint8_t) * 2 + sizeof(TypeIdFlags) * typeIdFlagsCount);

    memcpy(subTlvs + sizeof(Tlv), &aSeriesId, sizeof(aSeriesId));

    seriesFlags->SetFrom(aSeriesFlags);

    error = Get<Mle::MleRouter>().SendLinkMetricsManagementRequest(aDestination, subTlvs, fwdProbingSubTlv->GetSize());

exit:
    otLogDebgMle("SendMgmtRequestForwardTrackingSeries, error:%s, Series ID:%u", ErrorToString(error), aSeriesId);
    return error;
}

Error LinkMetrics::SendMgmtRequestEnhAckProbing(const Ip6::Address &aDestination,
                                                const EnhAckFlags   aEnhAckFlags,
                                                const Metrics *     aMetrics)
{
    Error              error = kErrorNone;
    EnhAckConfigSubTlv enhAckConfigSubTlv;
    Mac::Address       macAddress;
    Neighbor *         neighbor = GetNeighborFromLinkLocalAddr(aDestination);

    VerifyOrExit(neighbor != nullptr, error = kErrorUnknownNeighbor);
    VerifyOrExit(neighbor->IsThreadVersion1p2(), error = kErrorNotCapable);

    if (aEnhAckFlags == kEnhAckClear)
    {
        VerifyOrExit(aMetrics == nullptr, error = kErrorInvalidArgs);
    }

    enhAckConfigSubTlv.SetEnhAckFlags(aEnhAckFlags);

    if (aMetrics != nullptr)
    {
        enhAckConfigSubTlv.SetTypeIdFlags(*aMetrics);
    }

    error = Get<Mle::MleRouter>().SendLinkMetricsManagementRequest(
        aDestination, reinterpret_cast<const uint8_t *>(&enhAckConfigSubTlv), enhAckConfigSubTlv.GetSize());

    if (aMetrics != nullptr)
    {
        neighbor->SetEnhAckProbingMetrics(*aMetrics);
    }
    else
    {
        Metrics metrics;

        metrics.Clear();
        neighbor->SetEnhAckProbingMetrics(metrics);
    }

exit:
    return error;
}

Error LinkMetrics::SendLinkProbe(const Ip6::Address &aDestination, uint8_t aSeriesId, uint8_t aLength)
{
    Error     error = kErrorNone;
    uint8_t   buf[kLinkProbeMaxLen];
    Neighbor *neighbor = GetNeighborFromLinkLocalAddr(aDestination);

    VerifyOrExit(neighbor != nullptr, error = kErrorUnknownNeighbor);
    VerifyOrExit(neighbor->IsThreadVersion1p2(), error = kErrorNotCapable);

    VerifyOrExit(aLength <= LinkMetrics::kLinkProbeMaxLen && aSeriesId != kQueryIdSingleProbe &&
                     aSeriesId != kSeriesIdAllSeries,
                 error = kErrorInvalidArgs);

    error = Get<Mle::MleRouter>().SendLinkProbe(aDestination, aSeriesId, buf, aLength);
exit:
    otLogDebgMle("SendLinkProbe, error:%s, Series ID:%u", ErrorToString(error), aSeriesId);
    return error;
}
#endif // OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
Error LinkMetrics::AppendReport(Message &aMessage, const Message &aRequestMessage, Neighbor &aNeighbor)
{
    Error         error = kErrorNone;
    Tlv           tlv;
    uint8_t       queryId;
    bool          hasQueryId  = false;
    uint8_t       length      = 0;
    uint16_t      startOffset = aMessage.GetLength();
    uint16_t      offset;
    uint16_t      endOffset;
    MetricsValues values;

    values.Clear();

    SuccessOrExit(error = Tlv::FindTlvValueOffset(aRequestMessage, Mle::Tlv::Type::kLinkMetricsQuery, offset,
                                                  endOffset)); // `endOffset` is used to store tlv length here

    endOffset = offset + endOffset;

    while (offset < endOffset)
    {
        SuccessOrExit(error = aRequestMessage.Read(offset, tlv));

        switch (tlv.GetType())
        {
        case SubTlv::kQueryId:
            SuccessOrExit(error = Tlv::Read<QueryIdSubTlv>(aRequestMessage, offset, queryId));
            hasQueryId = true;
            break;

        case SubTlv::kQueryOptions:
            SuccessOrExit(error = ReadTypeIdFlagsFromMessage(aRequestMessage, offset + sizeof(tlv),
                                                             static_cast<uint16_t>(offset + tlv.GetSize()),
                                                             values.GetMetrics()));
            break;

        default:
            break;
        }

        offset += tlv.GetSize();
    }

    VerifyOrExit(hasQueryId, error = kErrorParse);

    // Link Metrics Report TLV
    tlv.SetType(Mle::Tlv::kLinkMetricsReport);
    SuccessOrExit(error = aMessage.Append(tlv));

    if (queryId == kQueryIdSingleProbe)
    {
        values.mPduCountValue = HostSwap32(aRequestMessage.GetPsduCount());
        values.mLqiValue      = aRequestMessage.GetAverageLqi();
        // Linearly scale Link Margin from [0, 130] to [0, 255]
        values.mLinkMarginValue =
            LinkQualityInfo::ConvertRssToLinkMargin(Get<Mac::Mac>().GetNoiseFloor(), aRequestMessage.GetAverageRss()) *
            255 / 130;
        // Linearly scale rss from [-130, 0] to [0, 255]
        values.mRssiValue = (aRequestMessage.GetAverageRss() + 130) * 255 / 130;

        SuccessOrExit(error = AppendReportSubTlvToMessage(aMessage, length, values));
    }
    else
    {
        SeriesInfo *seriesInfo = aNeighbor.GetForwardTrackingSeriesInfo(queryId);

        if (seriesInfo == nullptr)
        {
            SuccessOrExit(error = AppendStatusSubTlvToMessage(aMessage, length, kStatusSeriesIdNotRecognized));
        }
        else if (seriesInfo->GetPduCount() == 0)
        {
            SuccessOrExit(error = AppendStatusSubTlvToMessage(aMessage, length, kStatusNoMatchingFramesReceived));
        }
        else
        {
            values.SetMetrics(seriesInfo->GetLinkMetrics());
            values.mPduCountValue = HostSwap32(seriesInfo->GetPduCount());
            values.mLqiValue      = seriesInfo->GetAverageLqi();
            // Linearly scale Link Margin from [0, 130] to [0, 255]
            values.mLinkMarginValue =
                LinkQualityInfo::ConvertRssToLinkMargin(Get<Mac::Mac>().GetNoiseFloor(), seriesInfo->GetAverageRss()) *
                255 / 130;
            // Linearly scale RSSI from [-130, 0] to [0, 255]
            values.mRssiValue = (seriesInfo->GetAverageRss() + 130) * 255 / 130;
            SuccessOrExit(error = AppendReportSubTlvToMessage(aMessage, length, values));
        }
    }

    tlv.SetLength(length);
    aMessage.Write(startOffset, tlv);

exit:
    otLogDebgMle("AppendReport, error:%s", ErrorToString(error));
    return error;
}

Error LinkMetrics::HandleManagementRequest(const Message &aMessage, Neighbor &aNeighbor, Status &aStatus)
{
    Error       error = kErrorNone;
    Tlv         tlv;
    uint8_t     seriesId;
    SeriesFlags seriesFlags;
    EnhAckFlags enhAckFlags;
    Metrics     metrics;
    bool        hasForwardProbingRegistrationTlv = false;
    bool        hasEnhAckProbingTlv              = false;
    uint16_t    offset;
    uint16_t    length;
    uint16_t    index = 0;

    SuccessOrExit(error = Tlv::FindTlvValueOffset(aMessage, Mle::Tlv::Type::kLinkMetricsManagement, offset, length));

    while (index < length)
    {
        uint16_t pos = offset + index;

        SuccessOrExit(aMessage.Read(pos, tlv));

        pos += sizeof(tlv);

        switch (tlv.GetType())
        {
        case SubTlv::kFwdProbingReg:
            VerifyOrExit(!hasForwardProbingRegistrationTlv && !hasEnhAckProbingTlv, error = kErrorParse);
            VerifyOrExit(tlv.GetLength() >= sizeof(seriesId) + sizeof(seriesFlags), error = kErrorParse);
            SuccessOrExit(aMessage.Read(pos, seriesId));
            pos += sizeof(seriesId);
            SuccessOrExit(aMessage.Read(pos, seriesFlags));
            pos += sizeof(seriesFlags);
            SuccessOrExit(error = ReadTypeIdFlagsFromMessage(
                              aMessage, pos, static_cast<uint16_t>(offset + index + tlv.GetSize()), metrics));
            hasForwardProbingRegistrationTlv = true;
            break;

        case SubTlv::kEnhAckConfig:
            VerifyOrExit(!hasForwardProbingRegistrationTlv && !hasEnhAckProbingTlv, error = kErrorParse);
            VerifyOrExit(tlv.GetLength() >= sizeof(EnhAckFlags), error = kErrorParse);
            SuccessOrExit(aMessage.Read(pos, enhAckFlags));
            pos += sizeof(enhAckFlags);
            SuccessOrExit(error = ReadTypeIdFlagsFromMessage(
                              aMessage, pos, static_cast<uint16_t>(offset + index + tlv.GetSize()), metrics));
            hasEnhAckProbingTlv = true;
            break;

        default:
            break;
        }

        index += tlv.GetSize();
    }

    if (hasForwardProbingRegistrationTlv)
    {
        aStatus = ConfigureForwardTrackingSeries(seriesId, seriesFlags, metrics, aNeighbor);
    }
    else if (hasEnhAckProbingTlv)
    {
        aStatus = ConfigureEnhAckProbing(enhAckFlags, metrics, aNeighbor);
    }

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE

Error LinkMetrics::HandleManagementResponse(const Message &aMessage, const Ip6::Address &aAddress)
{
    Error    error = kErrorNone;
    Tlv      tlv;
    uint16_t offset;
    uint16_t length;
    uint16_t index = 0;
    Status   status;
    bool     hasStatus = false;

    VerifyOrExit(mMgmtResponseCallback != nullptr);

    SuccessOrExit(error = Tlv::FindTlvValueOffset(aMessage, Mle::Tlv::Type::kLinkMetricsManagement, offset, length));

    while (index < length)
    {
        SuccessOrExit(aMessage.Read(offset + index, tlv));

        switch (tlv.GetType())
        {
        case SubTlv::kStatus:
            VerifyOrExit(!hasStatus, error = kErrorParse);
            VerifyOrExit(tlv.GetLength() == sizeof(status), error = kErrorParse);
            SuccessOrExit(aMessage.Read(offset + index + sizeof(tlv), status));
            hasStatus = true;
            break;

        default:
            break;
        }

        index += tlv.GetSize();
    }

    VerifyOrExit(hasStatus, error = kErrorParse);

    mMgmtResponseCallback(&aAddress, status, mMgmtResponseCallbackContext);

exit:
    return error;
}

void LinkMetrics::HandleReport(const Message &     aMessage,
                               uint16_t            aOffset,
                               uint16_t            aLength,
                               const Ip6::Address &aAddress)
{
    Error         error = kErrorNone;
    MetricsValues values;
    uint8_t       rawValue;
    uint16_t      pos    = aOffset;
    uint16_t      endPos = aOffset + aLength;
    Tlv           tlv;
    TypeIdFlags   typeIdFlags;
    bool          hasStatus = false;
    bool          hasReport = false;
    Status        status;

    OT_UNUSED_VARIABLE(error);

    VerifyOrExit(mReportCallback != nullptr);

    values.Clear();

    while (pos < endPos)
    {
        SuccessOrExit(aMessage.Read(pos, tlv));
        VerifyOrExit(tlv.GetType() == SubTlv::kReport);
        pos += sizeof(Tlv);

        VerifyOrExit(pos + tlv.GetLength() <= endPos, error = kErrorParse);

        switch (tlv.GetType())
        {
        case SubTlv::kStatus:
            // There should be either: one Status TLV or some Report-Sub TLVs
            VerifyOrExit(!hasStatus && !hasReport, error = kErrorDrop);
            VerifyOrExit(tlv.GetLength() == sizeof(status), error = kErrorParse);
            SuccessOrExit(aMessage.Read(pos, status));
            hasStatus = true;
            pos += sizeof(status);
            break;

        case SubTlv::kReport:
            // There shouldn't be any Report-Sub TLV when there's a Status TLV
            VerifyOrExit(!hasStatus, error = kErrorDrop);
            VerifyOrExit(tlv.GetLength() > sizeof(typeIdFlags), error = kErrorParse);
            SuccessOrExit(aMessage.Read(pos, typeIdFlags));

            if (typeIdFlags.IsExtendedFlagSet())
            {
                pos += tlv.GetLength(); // Skip the whole sub-TLV if `E` flag is set
                continue;
            }

            hasReport = true;
            pos += sizeof(TypeIdFlags);

            switch (typeIdFlags.GetRawValue())
            {
            case TypeIdFlags::kPdu:
                values.GetMetrics().mPduCount = true;
                SuccessOrExit(aMessage.Read(pos, values.mPduCountValue));
                values.mPduCountValue = HostSwap32(values.mPduCountValue);
                pos += sizeof(uint32_t);
                otLogDebgMle(" - PDU Counter: %d (Count/Summation)", values.mPduCountValue);
                break;

            case TypeIdFlags::kLqi:
                values.GetMetrics().mLqi = true;
                SuccessOrExit(aMessage.Read(pos, values.mLqiValue));
                pos += sizeof(uint8_t);
                otLogDebgMle(" - LQI: %d (Exponential Moving Average)", values.mLqiValue);
                break;

            case TypeIdFlags::kLinkMargin:
                values.GetMetrics().mLinkMargin = true;
                SuccessOrExit(aMessage.Read(pos, rawValue));
                // Reverse operation for linear scale, map from [0, 255] to [0, 130]
                values.mLinkMarginValue = rawValue * 130 / 255;
                pos += sizeof(uint8_t);
                otLogDebgMle(" - Margin: %d (dB) (Exponential Moving Average)", values.mLinkMarginValue);
                break;

            case TypeIdFlags::kRssi:
                values.GetMetrics().mRssi = true;
                SuccessOrExit(aMessage.Read(pos, rawValue));
                // Reverse operation for linear scale, map from [0, 255] to [-130, 0]
                values.mRssiValue = rawValue * 130 / 255 - 130;
                pos += sizeof(uint8_t);
                otLogDebgMle(" - RSSI: %d (dBm) (Exponential Moving Average)", values.mRssiValue);
                break;

            default:
                break;
            }
            break;
        }
    }

    if (hasStatus)
    {
        mReportCallback(&aAddress, nullptr, status, mReportCallbackContext);
    }
    else if (hasReport)
    {
        mReportCallback(&aAddress, &values, OT_LINK_METRICS_STATUS_SUCCESS, mReportCallbackContext);
    }

exit:
    otLogDebgMle("HandleReport, error:%s", ErrorToString(error));
    return;
}

Error LinkMetrics::HandleLinkProbe(const Message &aMessage, uint8_t &aSeriesId)
{
    Error    error = kErrorNone;
    uint16_t offset;
    uint16_t length;

    SuccessOrExit(error = Tlv::FindTlvValueOffset(aMessage, Mle::Tlv::Type::kLinkProbe, offset, length));
    VerifyOrExit(length >= sizeof(aSeriesId), error = kErrorParse);
    error = aMessage.Read(offset, aSeriesId);

exit:
    return error;
}

void LinkMetrics::SetReportCallback(ReportCallback aCallback, void *aContext)
{
    mReportCallback        = aCallback;
    mReportCallbackContext = aContext;
}

void LinkMetrics::SetMgmtResponseCallback(MgmtResponseCallback aCallback, void *aContext)
{
    mMgmtResponseCallback        = aCallback;
    mMgmtResponseCallbackContext = aContext;
}

void LinkMetrics::SetEnhAckProbingCallback(EnhAckProbingIeReportCallback aCallback, void *aContext)
{
    mEnhAckProbingIeReportCallback        = aCallback;
    mEnhAckProbingIeReportCallbackContext = aContext;
}

void LinkMetrics::ProcessEnhAckIeData(const uint8_t *aData, uint8_t aLength, const Neighbor &aNeighbor)
{
    MetricsValues values;
    uint8_t       idx = 0;

    VerifyOrExit(mEnhAckProbingIeReportCallback != nullptr);

    values.SetMetrics(aNeighbor.GetEnhAckProbingMetrics());

    if (values.GetMetrics().mLqi && idx < aLength)
    {
        values.mLqiValue = aData[idx++];
    }
    if (values.GetMetrics().mLinkMargin && idx < aLength)
    {
        // Reverse operation for linear scale, map from [0, 255] to [0, 130]
        values.mLinkMarginValue = aData[idx++] * 130 / 255;
    }
    if (values.GetMetrics().mRssi && idx < aLength)
    {
        // Reverse operation for linear scale, map from [0, 255] to [-130, 0]
        values.mRssiValue = aData[idx++] * 130 / 255 - 130;
    }

    mEnhAckProbingIeReportCallback(aNeighbor.GetRloc16(), &aNeighbor.GetExtAddress(), &values,
                                   mEnhAckProbingIeReportCallbackContext);

exit:
    return;
}

Error LinkMetrics::SendLinkMetricsQuery(const Ip6::Address &aDestination,
                                        uint8_t             aSeriesId,
                                        const TypeIdFlags * aTypeIdFlags,
                                        uint8_t             aTypeIdFlagsCount)
{
    // LinkMetricsQuery Tlv + LinkMetricsQueryId sub-TLV (value-length: 1 byte) +
    // LinkMetricsQueryOptions sub-TLV (value-length: `kMaxTypeIdFlags` bytes)
    constexpr uint16_t kBufferSize = sizeof(Tlv) * 3 + sizeof(uint8_t) + sizeof(TypeIdFlags) * kMaxTypeIdFlags;

    Error                error = kErrorNone;
    QueryOptionsSubTlv   queryOptionsTlv;
    uint8_t              length = 0;
    static const uint8_t tlvs[] = {Mle::Tlv::kLinkMetricsReport};
    uint8_t              buf[kBufferSize];
    Tlv *                tlv = reinterpret_cast<Tlv *>(buf);
    Tlv                  subTlv;

    // Link Metrics Query TLV
    tlv->SetType(Mle::Tlv::kLinkMetricsQuery);
    length += sizeof(Tlv);

    // Link Metrics Query ID sub-TLV
    subTlv.SetType(SubTlv::kQueryId);
    subTlv.SetLength(sizeof(uint8_t));
    memcpy(buf + length, &subTlv, sizeof(subTlv));
    length += sizeof(subTlv);
    memcpy(buf + length, &aSeriesId, sizeof(aSeriesId));
    length += sizeof(aSeriesId);

    // Link Metrics Query Options sub-TLV
    if (aTypeIdFlagsCount > 0)
    {
        queryOptionsTlv.Init();
        queryOptionsTlv.SetLength(aTypeIdFlagsCount * sizeof(TypeIdFlags));

        memcpy(buf + length, &queryOptionsTlv, sizeof(queryOptionsTlv));
        length += sizeof(queryOptionsTlv);
        memcpy(buf + length, aTypeIdFlags, queryOptionsTlv.GetLength());
        length += queryOptionsTlv.GetLength();
    }

    // Set Length for Link Metrics Report TLV
    tlv->SetLength(length - sizeof(Tlv));

    SuccessOrExit(error = Get<Mle::MleRouter>().SendDataRequest(aDestination, tlvs, sizeof(tlvs), 0, buf, length));

exit:
    return error;
}

Status LinkMetrics::ConfigureForwardTrackingSeries(uint8_t            aSeriesId,
                                                   const SeriesFlags &aSeriesFlags,
                                                   const Metrics &    aMetrics,
                                                   Neighbor &         aNeighbor)
{
    Status status = kStatusSuccess;

    VerifyOrExit(0 < aSeriesId, status = kStatusOtherError);
    if (aSeriesFlags.GetRawValue() == 0) // Remove the series
    {
        if (aSeriesId == kSeriesIdAllSeries) // Remove all
        {
            aNeighbor.RemoveAllForwardTrackingSeriesInfo();
        }
        else
        {
            SeriesInfo *seriesInfo = aNeighbor.RemoveForwardTrackingSeriesInfo(aSeriesId);
            VerifyOrExit(seriesInfo != nullptr, status = kStatusSeriesIdNotRecognized);
            mSeriesInfoPool.Free(*seriesInfo);
        }
    }
    else // Add a new series
    {
        SeriesInfo *seriesInfo = aNeighbor.GetForwardTrackingSeriesInfo(aSeriesId);
        VerifyOrExit(seriesInfo == nullptr, status = kStatusSeriesIdAlreadyRegistered);
        seriesInfo = mSeriesInfoPool.Allocate();
        VerifyOrExit(seriesInfo != nullptr, status = kStatusCannotSupportNewSeries);

        seriesInfo->Init(aSeriesId, aSeriesFlags, aMetrics);

        aNeighbor.AddForwardTrackingSeriesInfo(*seriesInfo);
    }

exit:
    return status;
}

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
Status LinkMetrics::ConfigureEnhAckProbing(EnhAckFlags aEnhAckFlags, const Metrics &aMetrics, Neighbor &aNeighbor)
{
    Status status = kStatusSuccess;
    Error  error  = kErrorNone;

    VerifyOrExit(!aMetrics.mReserved, status = kStatusOtherError);

    if (aEnhAckFlags == kEnhAckRegister)
    {
        VerifyOrExit(!aMetrics.mPduCount, status = kStatusOtherError);
        VerifyOrExit(aMetrics.mLqi || aMetrics.mLinkMargin || aMetrics.mRssi, status = kStatusOtherError);
        VerifyOrExit(!(aMetrics.mLqi && aMetrics.mLinkMargin && aMetrics.mRssi), status = kStatusOtherError);

        error = Get<Radio>().ConfigureEnhAckProbing(aMetrics, aNeighbor.GetRloc16(), aNeighbor.GetExtAddress());
    }
    else if (aEnhAckFlags == kEnhAckClear)
    {
        VerifyOrExit(!aMetrics.mLqi && !aMetrics.mLinkMargin && !aMetrics.mRssi, status = kStatusOtherError);
        error = Get<Radio>().ConfigureEnhAckProbing(aMetrics, aNeighbor.GetRloc16(), aNeighbor.GetExtAddress());
    }
    else
    {
        status = kStatusOtherError;
    }

    VerifyOrExit(error == kErrorNone, status = kStatusOtherError);

exit:
    return status;
}
#endif // OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE

Neighbor *LinkMetrics::GetNeighborFromLinkLocalAddr(const Ip6::Address &aDestination)
{
    Neighbor *   neighbor = nullptr;
    Mac::Address macAddress;

    VerifyOrExit(aDestination.IsLinkLocal());
    aDestination.GetIid().ConvertToMacAddress(macAddress);
    neighbor = Get<NeighborTable>().FindNeighbor(macAddress);

exit:
    return neighbor;
}

Error LinkMetrics::ReadTypeIdFlagsFromMessage(const Message &aMessage,
                                              uint8_t        aStartPos,
                                              uint8_t        aEndPos,
                                              Metrics &      aMetrics)
{
    Error error = kErrorNone;

    memset(&aMetrics, 0, sizeof(aMetrics));

    for (uint16_t pos = aStartPos; pos < aEndPos; pos += sizeof(TypeIdFlags))
    {
        TypeIdFlags typeIdFlags;

        SuccessOrExit(aMessage.Read(pos, typeIdFlags));

        switch (typeIdFlags.GetRawValue())
        {
        case TypeIdFlags::kPdu:
            VerifyOrExit(!aMetrics.mPduCount, error = kErrorParse);
            aMetrics.mPduCount = true;
            break;

        case TypeIdFlags::kLqi:
            VerifyOrExit(!aMetrics.mLqi, error = kErrorParse);
            aMetrics.mLqi = true;
            break;

        case TypeIdFlags::kLinkMargin:
            VerifyOrExit(!aMetrics.mLinkMargin, error = kErrorParse);
            aMetrics.mLinkMargin = true;
            break;

        case TypeIdFlags::kRssi:
            VerifyOrExit(!aMetrics.mRssi, error = kErrorParse);
            aMetrics.mRssi = true;
            break;

        default:
            if (typeIdFlags.IsExtendedFlagSet())
            {
                pos += sizeof(uint8_t); // Skip the additional second flags byte.
            }
            else
            {
                aMetrics.mReserved = true;
            }
            break;
        }
    }

exit:
    return error;
}

Error LinkMetrics::AppendReportSubTlvToMessage(Message &aMessage, uint8_t &aLength, const MetricsValues &aValues)
{
    Error        error = kErrorNone;
    ReportSubTlv metric;

    aLength = 0;

    // Link Metrics Report sub-TLVs
    if (aValues.mMetrics.mPduCount)
    {
        metric.Init();
        metric.SetMetricsTypeId(TypeIdFlags(TypeIdFlags::kPdu));
        metric.SetMetricsValue32(aValues.mPduCountValue);
        SuccessOrExit(error = aMessage.AppendBytes(&metric, metric.GetSize()));
        aLength += metric.GetSize();
    }

    if (aValues.mMetrics.mLqi)
    {
        metric.Init();
        metric.SetMetricsTypeId(TypeIdFlags(TypeIdFlags::kLqi));
        metric.SetMetricsValue8(aValues.mLqiValue);
        SuccessOrExit(error = aMessage.AppendBytes(&metric, metric.GetSize()));
        aLength += metric.GetSize();
    }

    if (aValues.mMetrics.mLinkMargin)
    {
        metric.Init();
        metric.SetMetricsTypeId(TypeIdFlags(TypeIdFlags::kLinkMargin));
        metric.SetMetricsValue8(aValues.mLinkMarginValue);
        SuccessOrExit(error = aMessage.AppendBytes(&metric, metric.GetSize()));
        aLength += metric.GetSize();
    }

    if (aValues.mMetrics.mRssi)
    {
        metric.Init();
        metric.SetMetricsTypeId(TypeIdFlags(TypeIdFlags::kRssi));
        metric.SetMetricsValue8(aValues.mRssiValue);
        SuccessOrExit(error = aMessage.AppendBytes(&metric, metric.GetSize()));
        aLength += metric.GetSize();
    }

exit:
    return error;
}

Error LinkMetrics::AppendStatusSubTlvToMessage(Message &aMessage, uint8_t &aLength, Status aStatus)
{
    Error error = kErrorNone;
    Tlv   statusTlv;

    statusTlv.SetType(SubTlv::kStatus);
    statusTlv.SetLength(sizeof(uint8_t));
    SuccessOrExit(error = aMessage.AppendBytes(&statusTlv, sizeof(statusTlv)));
    SuccessOrExit(error = aMessage.AppendBytes(&aStatus, sizeof(aStatus)));
    aLength += statusTlv.GetSize();

exit:
    return error;
}

} // namespace LinkMetrics
} // namespace ot

#endif // OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
