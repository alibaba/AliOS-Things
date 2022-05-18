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
 *   This file includes definitions for Thread Link Metrics query and management.
 */

#ifndef LINK_METRICS_HPP_
#define LINK_METRICS_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE

#if (OPENTHREAD_CONFIG_THREAD_VERSION < OT_THREAD_VERSION_1_2)
#error "Thread 1.2 or higher version is required for OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE" \
       "and OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE."
#endif

#include <openthread/link.h>

#include "common/clearable.hpp"
#include "common/locator.hpp"
#include "common/message.hpp"
#include "common/non_copyable.hpp"
#include "common/pool.hpp"
#include "net/ip6_address.hpp"
#include "thread/link_metrics_tlvs.hpp"
#include "thread/link_quality.hpp"

namespace ot {
class Neighbor;

namespace LinkMetrics {

/**
 * @addtogroup core-link-metrics
 *
 * @brief
 *   This module includes definitions for Thread Link Metrics query and management.
 *
 * @{
 */

/**
 * This type represents the results (values) for a set of metrics.
 *
 * @sa otLinkMetricsValues.
 *
 */
class MetricsValues : public otLinkMetricsValues, public Clearable<MetricsValues>
{
public:
    /**
     * This method gets the metrics flags.
     *
     * @returns The metrics flags.
     *
     */
    Metrics &GetMetrics(void) { return static_cast<Metrics &>(mMetrics); }

    /**
     * This method gets the metrics flags.
     *
     * @returns The metrics flags.
     *
     */
    const Metrics &GetMetrics(void) const { return static_cast<const Metrics &>(mMetrics); }

    /**
     * This method set the metrics flags.
     *
     * @param[in] aMetrics  The metrics flags to set from.
     *
     */
    void SetMetrics(const Metrics &aMetrics) { mMetrics = aMetrics; }
};

/**
 * This class represents one Series that is being tracked by the Subject.
 *
 * When an Initiator successfully configured a Forward Tracking Series, the Subject would use an instance of this class
 * to track the information of the Series. The Subject has a `Pool` of `SeriesInfo`. It would allocate one when a new
 * Series comes, and free it when a Series finishes.
 *
 * This class inherits `LinkedListEntry` and each `Neighbor` has a list of `SeriesInfo` so that the Subject could track
 * per Series initiated by neighbors as long as it has available resources.
 *
 */
class SeriesInfo : public LinkedListEntry<SeriesInfo>
{
    friend class LinkedList<SeriesInfo>;
    friend class LinkedListEntry<SeriesInfo>;

public:
    /**
     * This constant represents Link Probe when filtering frames to be accounted using Series Flag. There's
     * already `kFcfFrameData`, `kFcfFrameAck` and `kFcfFrameMacCmd`. This item is added so that we can
     * filter a Link Probe for series in the same way as other frames.
     *
     */
    static constexpr uint8_t kSeriesTypeLinkProbe = 0;

    /**
     * This method initializes the SeriesInfo object.
     *
     * @param[in]  aSeriesId      The Series ID.
     * @param[in]  aSeriesFlags   The Series Flags which specify what types of frames are to be accounted.
     * @param[in]  aMetrics       Metrics to query.
     *
     */
    void Init(uint8_t aSeriesId, const SeriesFlags &aSeriesFlags, const Metrics &aMetrics);

    /**
     * This method gets the Series ID.
     *
     * @returns  The Series ID.
     *
     */
    uint8_t GetSeriesId(void) const { return mSeriesId; }

    /**
     * This method gets the PDU count.
     *
     * @returns  The PDU count.
     *
     */
    uint32_t GetPduCount(void) const { return mPduCount; }

    /**
     * This method gets the average LQI.
     *
     * @returns  The average LQI.
     *
     */
    uint8_t GetAverageLqi(void) const { return mLqiAverager.GetAverage(); }

    /**
     * This method gets the average RSS.
     *
     * @returns  The average RSS.
     *
     */
    int8_t GetAverageRss(void) const { return mRssAverager.GetAverage(); }

    /**
     * This method aggregates the Link Metrics data of a frame into this series.
     *
     * @param[in]  aFrameType    The type of the frame.
     * @param[in]  aLqi          The LQI value.
     * @param[in]  aRss          The RSS value.
     *
     */
    void AggregateLinkMetrics(uint8_t aFrameType, uint8_t aLqi, int8_t aRss);

    /**
     * This methods gets the metrics.
     *
     * @returns  The metrics associated with `SeriesInfo`.
     *
     */
    const Metrics &GetLinkMetrics(void) const { return mMetrics; }

private:
    bool Matches(const uint8_t &aSeriesId) const { return mSeriesId == aSeriesId; }
    bool IsFrameTypeMatch(uint8_t aFrameType) const;

    SeriesInfo *mNext;
    uint8_t     mSeriesId;
    SeriesFlags mSeriesFlags;
    Metrics     mMetrics;
    RssAverager mRssAverager;
    LqiAverager mLqiAverager;
    uint32_t    mPduCount;
};

/**
 * This enumeration type represent Link Metrics Status.
 *
 */
enum Status : uint8_t
{
    kStatusSuccess                   = OT_LINK_METRICS_STATUS_SUCCESS,
    kStatusCannotSupportNewSeries    = OT_LINK_METRICS_STATUS_CANNOT_SUPPORT_NEW_SERIES,
    kStatusSeriesIdAlreadyRegistered = OT_LINK_METRICS_STATUS_SERIESID_ALREADY_REGISTERED,
    kStatusSeriesIdNotRecognized     = OT_LINK_METRICS_STATUS_SERIESID_NOT_RECOGNIZED,
    kStatusNoMatchingFramesReceived  = OT_LINK_METRICS_STATUS_NO_MATCHING_FRAMES_RECEIVED,
    kStatusOtherError                = OT_LINK_METRICS_STATUS_OTHER_ERROR,
};

/**
 * This class implements Thread Link Metrics query and management.
 *
 */
class LinkMetrics : public InstanceLocator, private NonCopyable
{
    friend class ot::Neighbor;

public:
    typedef otLinkMetricsReportCallback                ReportCallback;
    typedef otLinkMetricsMgmtResponseCallback          MgmtResponseCallback;
    typedef otLinkMetricsEnhAckProbingIeReportCallback EnhAckProbingIeReportCallback;

    /**
     * This constructor initializes an instance of the LinkMetrics class.
     *
     * @param[in]  aInstance  A reference to the OpenThread interface.
     *
     */
    explicit LinkMetrics(Instance &aInstance);

    /**
     * This method sends an MLE Data Request containing Link Metrics Query TLV to query Link Metrics data.
     *
     * It could be either a Single Probe or a Forward Tracking Series.
     *
     * @param[in]  aDestination       A reference to the IPv6 address of the destination.
     * @param[in]  aSeriesId          The Series ID to query, 0 for single probe.
     * @param[in]  aMetrics           A pointer to metrics to query.
     *
     * @retval kErrorNone             Successfully sent a Link Metrics query message.
     * @retval kErrorNoBufs           Insufficient buffers to generate the MLE Data Request message.
     * @retval kErrorInvalidArgs      TypeIdFlags are not valid or exceed the count limit.
     * @retval kErrorUnknownNeighbor  @p aDestination is not link-local or the neighbor is not found.
     *
     */
    Error Query(const Ip6::Address &aDestination, uint8_t aSeriesId, const Metrics *aMetrics);

    /**
     * This method sends an MLE Link Metrics Management Request to configure/clear a Forward Tracking Series.
     *
     * @param[in] aDestination       A reference to the IPv6 address of the destination.
     * @param[in] aSeriesId          The Series ID.
     * @param[in] aSeriesFlags       The Series Flags info which specify what types of frames are to be accounted.
     * @param[in] aMetrics           A pointer to flags specifying what metrics to query.
     *
     * @retval kErrorNone             Successfully sent a Link Metrics Management Request message.
     * @retval kErrorNoBufs           Insufficient buffers to generate the MLE Link Metrics Management Request message.
     * @retval kErrorInvalidArgs      @p aSeriesId is not within the valid range.
     * @retval kErrorUnknownNeighbor  @p aDestination is not link-local or the neighbor is not found.
     *
     */
    Error SendMgmtRequestForwardTrackingSeries(const Ip6::Address &     aDestination,
                                               uint8_t                  aSeriesId,
                                               const SeriesFlags::Info &aSeriesFlags,
                                               const Metrics *          aMetrics);

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
    /**
     * This method sends an MLE Link Metrics Management Request to configure/clear a Enhanced-ACK Based Probing.
     *
     * @param[in] aDestination       A reference to the IPv6 address of the destination.
     * @param[in] aEnhAckFlags       Enh-ACK Flags to indicate whether to register or clear the probing. `0` to clear
     *                               and `1` to register. Other values are reserved.
     * @param[in] aMetrics           A pointer to flags specifying what metrics to query. Should be `nullptr` when
     *                               `aEnhAckFlags` is `0`.
     *
     * @retval kErrorNone             Successfully sent a Link Metrics Management Request message.
     * @retval kErrorNoBufs           Insufficient buffers to generate the MLE Link Metrics Management Request message.
     * @retval kErrorInvalidArgs      @p aEnhAckFlags is not a valid value or @p aMetrics isn't correct.
     * @retval kErrorUnknownNeighbor  @p aDestination is not link-local or the neighbor is not found.
     *
     */
    Error SendMgmtRequestEnhAckProbing(const Ip6::Address &aDestination,
                                       EnhAckFlags         aEnhAckFlags,
                                       const Metrics *     aMetrics);

    /**
     * This method sends an MLE Link Probe message.
     *
     * @param[in] aDestination    A reference to the IPv6 address of the destination.
     * @param[in] aSeriesId       The Series ID which the Probe message targets at.
     * @param[in] aLength         The length of the data payload in Link Probe TLV, [0, 64].
     *
     * @retval kErrorNone             Successfully sent a Link Probe message.
     * @retval kErrorNoBufs           Insufficient buffers to generate the MLE Link Probe message.
     * @retval kErrorInvalidArgs      @p aSeriesId or @p aLength is not within the valid range.
     * @retval kErrorUnknownNeighbor  @p aDestination is not link-local or the neighbor is not found.
     *
     */
    Error SendLinkProbe(const Ip6::Address &aDestination, uint8_t aSeriesId, uint8_t aLength);
#endif

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    /**
     * This method appends a Link Metrics Report to a message according to the Link Metrics query.
     *
     * @param[out]  aMessage           A reference to the message to append report.
     * @param[in]   aRequestMessage    A reference to the message of the Data Request.
     * @param[in]   aNeighbor          A reference to the neighbor who queries the report.
     *
     * @retval kErrorNone         Successfully appended the Thread Discovery TLV.
     * @retval kErrorParse        Cannot parse query sub TLV successfully.
     * @retval kErrorInvalidArgs  QueryId is invalid or any Type ID is invalid.
     *
     */
    Error AppendReport(Message &aMessage, const Message &aRequestMessage, Neighbor &aNeighbor);
#endif
    /**
     * This method handles the received Link Metrics Management Request contained in @p aMessage and return a status.
     *
     * @param[in]   aMessage     A reference to the message that contains the Link Metrics Management Request.
     * @param[in]   aNeighbor    A reference to the neighbor who sends the request.
     * @param[out]  aStatus      A reference to the status which indicates the handling result.
     *
     * @retval kErrorNone     Successfully handled the Link Metrics Management Request.
     * @retval kErrorParse    Cannot parse sub-TLVs from @p aMessage successfully.
     *
     */
    Error HandleManagementRequest(const Message &aMessage, Neighbor &aNeighbor, Status &aStatus);

    /**
     * This method handles the received Link Metrics Management Response contained in @p aMessage.
     *
     * @param[in]  aMessage    A reference to the message that contains the Link Metrics Management Response.
     * @param[in]  aAddress    A reference to the source address of the message.
     *
     * @retval kErrorNone     Successfully handled the Link Metrics Management Response.
     * @retval kErrorParse    Cannot parse sub-TLVs from @p aMessage successfully.
     *
     */
    Error HandleManagementResponse(const Message &aMessage, const Ip6::Address &aAddress);

    /**
     * This method handles the received Link Metrics report contained in @p aMessage.
     *
     * @param[in]  aMessage      A reference to the message.
     * @param[in]  aOffset       The offset in bytes where the metrics report sub-TLVs start.
     * @param[in]  aLength       The length of the metrics report sub-TLVs in bytes.
     * @param[in]  aAddress      A reference to the source address of the message.
     *
     */
    void HandleReport(const Message &aMessage, uint16_t aOffset, uint16_t aLength, const Ip6::Address &aAddress);

    /**
     * This method handles the Link Probe contained in @p aMessage.
     *
     * @param[in]   aMessage     A reference to the message that contains the Link Probe Message.
     * @param[out]  aSeriesId    A reference to Series ID that parsed from the message.
     *
     * @retval kErrorNone     Successfully handled the Link Metrics Management Response.
     * @retval kErrorParse    Cannot parse sub-TLVs from @p aMessage successfully.
     *
     */
    Error HandleLinkProbe(const Message &aMessage, uint8_t &aSeriesId);

    /**
     * This method registers a callback to handle Link Metrics report received.
     *
     * @param[in]  aCallback  A pointer to a function that is called when a Link Metrics report is received.
     * @param[in]  aContext   A pointer to application-specific context.
     *
     */
    void SetReportCallback(ReportCallback aCallback, void *aContext);

    /**
     * This method registers a callback to handle Link Metrics Management Response received.
     *
     * @param[in]  aCallback A pointer to a function that is called when a Link Metrics Management Response is received.
     * @param[in]  aContext  A pointer to application-specific context.
     *
     */
    void SetMgmtResponseCallback(MgmtResponseCallback aCallback, void *aContext);

    /**
     * This method registers a callback to handle Link Metrics when Enh-ACK Probing IE is received.
     *
     * @param[in]  aCallback A pointer to a function that is called when Enh-ACK Probing IE is received is received.
     * @param[in]  aContext  A pointer to application-specific context.
     *
     */
    void SetEnhAckProbingCallback(EnhAckProbingIeReportCallback aCallback, void *aContext);

    /**
     * This method processes received Enh-ACK Probing IE data.
     *
     * @param[in] aData      A pointer to buffer containing the Enh-ACK Probing IE data.
     * @param[in] aLen       The length of @p aData.
     * @param[in] aNeighbor  The neighbor from which the Enh-ACK Probing IE was received.
     *
     */
    void ProcessEnhAckIeData(const uint8_t *aData, uint8_t aLength, const Neighbor &aNeighbor);

private:
    static constexpr uint8_t kMaxTypeIdFlags = 4;

    // Max number of SeriesInfo that could be allocated by the pool.
    static constexpr uint16_t kMaxSeriesSupported = OPENTHREAD_CONFIG_MLE_LINK_METRICS_MAX_SERIES_SUPPORTED;

    static constexpr uint8_t kQueryIdSingleProbe = 0;   // This query ID represents Single Probe.
    static constexpr uint8_t kSeriesIdAllSeries  = 255; // This series ID represents all series.
    static constexpr uint8_t kLinkProbeMaxLen    = 64;  // Max length of data payload in Link Probe TLV.

    Error SendLinkMetricsQuery(const Ip6::Address &aDestination,
                               uint8_t             aSeriesId,
                               const TypeIdFlags * aTypeIdFlags,
                               uint8_t             aTypeIdFlagsCount);

    Status ConfigureForwardTrackingSeries(uint8_t            aSeriesId,
                                          const SeriesFlags &aSeriesFlags,
                                          const Metrics &    aMetrics,
                                          Neighbor &         aNeighbor);

    Status ConfigureEnhAckProbing(EnhAckFlags aEnhAckFlags, const Metrics &aMetrics, Neighbor &aNeighbor);

    Neighbor *GetNeighborFromLinkLocalAddr(const Ip6::Address &aDestination);

    static Error ReadTypeIdFlagsFromMessage(const Message &aMessage,
                                            uint8_t        aStartPos,
                                            uint8_t        aEndPos,
                                            Metrics &      aMetrics);
    static Error AppendReportSubTlvToMessage(Message &aMessage, uint8_t &aLength, const MetricsValues &aValues);
    static Error AppendStatusSubTlvToMessage(Message &aMessage, uint8_t &aLength, Status aStatus);

    ReportCallback                mReportCallback;
    void *                        mReportCallbackContext;
    MgmtResponseCallback          mMgmtResponseCallback;
    void *                        mMgmtResponseCallbackContext;
    EnhAckProbingIeReportCallback mEnhAckProbingIeReportCallback;
    void *                        mEnhAckProbingIeReportCallbackContext;

    Pool<SeriesInfo, kMaxSeriesSupported> mSeriesInfoPool;
};

/**
 * @}
 */

} // namespace LinkMetrics
} // namespace ot

#endif // OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE

#endif // LINK_METRICS_HPP
