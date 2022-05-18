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
 * @brief
 *   This file defines the OpenThread Link Metrics API.
 */

#ifndef LINK_METRICS_H_
#define LINK_METRICS_H_

#include "openthread-core-config.h"

#include <openthread/ip6.h>
#include <openthread/message.h>
#include <openthread/platform/radio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-link-metrics
 *
 * @brief
 *   This module includes functions that control the Link Metrics protocol.
 *
 * @{
 *
 */

/**
 * This structure represents the result (value) for a Link Metrics query.
 *
 */
typedef struct otLinkMetricsValues
{
    otLinkMetrics mMetrics; ///< Specifies which metrics values are present/included.

    uint32_t mPduCountValue;   ///< The value of Pdu Count.
    uint8_t  mLqiValue;        ///< The value LQI.
    uint8_t  mLinkMarginValue; ///< The value of Link Margin.
    int8_t   mRssiValue;       ///< The value of Rssi.
} otLinkMetricsValues;

/**
 * This structure represents which frames are accounted in a Forward Tracking Series.
 *
 */
typedef struct otLinkMetricsSeriesFlags
{
    bool mLinkProbe : 1;      ///< MLE Link Probe.
    bool mMacData : 1;        ///< MAC Data frame.
    bool mMacDataRequest : 1; ///< MAC Data Request.
    bool mMacAck : 1;         ///< MAC Ack.
} otLinkMetricsSeriesFlags;

/**
 * Enhanced-ACK Flags.
 *
 * These are used in Enhanced-ACK Based Probing to indicate whether to register or clear the probing.
 *
 */
typedef enum otLinkMetricsEnhAckFlags
{
    OT_LINK_METRICS_ENH_ACK_CLEAR    = 0, ///< Clear.
    OT_LINK_METRICS_ENH_ACK_REGISTER = 1, ///< Register.
} otLinkMetricsEnhAckFlags;

/**
 * Link Metrics Status values.
 *
 */
typedef enum otLinkMetricsStatus
{
    OT_LINK_METRICS_STATUS_SUCCESS                     = 0,
    OT_LINK_METRICS_STATUS_CANNOT_SUPPORT_NEW_SERIES   = 1,
    OT_LINK_METRICS_STATUS_SERIESID_ALREADY_REGISTERED = 2,
    OT_LINK_METRICS_STATUS_SERIESID_NOT_RECOGNIZED     = 3,
    OT_LINK_METRICS_STATUS_NO_MATCHING_FRAMES_RECEIVED = 4,
    OT_LINK_METRICS_STATUS_OTHER_ERROR                 = 254,
} otLinkMetricsStatus;

/**
 * This function pointer is called when a Link Metrics report is received.
 *
 * @param[in]  aSource         A pointer to the source address.
 * @param[in]  aMetricsValues  A pointer to the Link Metrics values (the query result).
 * @param[in]  aStatus         The status code in the report (only useful when @p aMetricsValues is NULL).
 * @param[in]  aContext        A pointer to application-specific context.
 *
 */
typedef void (*otLinkMetricsReportCallback)(const otIp6Address *       aSource,
                                            const otLinkMetricsValues *aMetricsValues,
                                            uint8_t                    aStatus,
                                            void *                     aContext);
/**
 * This function pointer is called when a Link Metrics Management Response is received.
 *
 * @param[in]  aSource         A pointer to the source address.
 * @param[in]  aStatus         The status code in the response.
 * @param[in]  aContext        A pointer to application-specific context.
 *
 */
typedef void (*otLinkMetricsMgmtResponseCallback)(const otIp6Address *aSource, uint8_t aStatus, void *aContext);

/**
 * This function pointer is called when Enh-ACK Probing IE is received.
 *
 * @param[in] aShortAddress     The Mac short address of the Probing Subject.
 * @param[in] aExtAddress       A pointer to the Mac extended address of the Probing Subject.
 * @param[in] aMetricsValues    A pointer to the Link Metrics values obtained from the IE.
 * @param[in] aContext          A pointer to application-specific context.
 *
 */
typedef void (*otLinkMetricsEnhAckProbingIeReportCallback)(otShortAddress             aShortAddress,
                                                           const otExtAddress *       aExtAddress,
                                                           const otLinkMetricsValues *aMetricsValues,
                                                           void *                     aContext);

/**
 * This function sends an MLE Data Request to query Link Metrics.
 *
 * It could be either Single Probe or Forward Tracking Series.
 *
 * @param[in]  aInstance            A pointer to an OpenThread instance.
 * @param[in]  aDestination         A pointer to the destination address.
 * @param[in]  aSeriesId            The Series ID to query about, 0 for Single Probe.
 * @param[in]  aLinkMetricsFlags    A pointer to flags specifying what metrics to query.
 * @param[in]  aCallback            A pointer to a function that is called when Link Metrics report is received.
 * @param[in]  aCallbackContext     A pointer to application-specific context.
 *
 * @retval OT_ERROR_NONE              Successfully sent a Link Metrics query message.
 * @retval OT_ERROR_NO_BUFS           Insufficient buffers to generate the MLE Data Request message.
 * @retval OT_ERROR_UNKNOWN_NEIGHBOR  @p aDestination is not link-local or the neighbor is not found.
 * @retval OT_ERROR_NOT_CAPABLE       The neighbor is not a Thread 1.2 device and does not support Link Metrics.
 *
 */
otError otLinkMetricsQuery(otInstance *                aInstance,
                           const otIp6Address *        aDestination,
                           uint8_t                     aSeriesId,
                           const otLinkMetrics *       aLinkMetricsFlags,
                           otLinkMetricsReportCallback aCallback,
                           void *                      aCallbackContext);

/**
 * This function sends an MLE Link Metrics Management Request to configure/clear a Forward Tracking Series.
 *
 * @param[in] aInstance          A pointer to an OpenThread instance.
 * @param[in] aDestination       A pointer to the destination address.
 * @param[in] aSeriesId          The Series ID to operate with.
 * @param[in] aSeriesFlags       The Series Flags that specifies which frames are to be accounted.
 * @param[in] aLinkMetricsFlags  A pointer to flags specifying what metrics to query. Should be `NULL` when
 *                               `aSeriesFlags` is `0`.
 * @param[in]  aCallback         A pointer to a function that is called when Link Metrics Management Response is
 *                               received.
 * @param[in]  aCallbackContext  A pointer to application-specific context.
 *
 * @retval OT_ERROR_NONE              Successfully sent a Link Metrics Management Request message.
 * @retval OT_ERROR_NO_BUFS           Insufficient buffers to generate the MLE Link Metrics Management Request message.
 * @retval OT_ERROR_INVALID_ARGS      @p aSeriesId is not within the valid range.
 * @retval OT_ERROR_UNKNOWN_NEIGHBOR  @p aDestination is not link-local or the neighbor is not found.
 * @retval OT_ERROR_NOT_CAPABLE       The neighbor is not a Thread 1.2 device and does not support Link Metrics.
 *
 */
otError otLinkMetricsConfigForwardTrackingSeries(otInstance *                      aInstance,
                                                 const otIp6Address *              aDestination,
                                                 uint8_t                           aSeriesId,
                                                 otLinkMetricsSeriesFlags          aSeriesFlags,
                                                 const otLinkMetrics *             aLinkMetricsFlags,
                                                 otLinkMetricsMgmtResponseCallback aCallback,
                                                 void *                            aCallbackContext);

/**
 * This function sends an MLE Link Metrics Management Request to configure/clear an Enhanced-ACK Based Probing.
 * This functionality requires OT_LINK_METRICS_INITIATOR feature enabled.
 *
 * @param[in] aInstance          A pointer to an OpenThread instance.
 * @param[in] aDestination       A pointer to the destination address.
 * @param[in] aEnhAckFlags       Enh-ACK Flags to indicate whether to register or clear the probing. `0` to clear and
 *                               `1` to register. Other values are reserved.
 * @param[in] aLinkMetricsFlags  A pointer to flags specifying what metrics to query. Should be `NULL` when
 *                               `aEnhAckFlags` is `0`.
 * @param[in] aCallback          A pointer to a function that is called when an Enhanced Ack with Link Metrics is
 *                               received.
 * @param[in] aCallbackContext   A pointer to application-specific context.
 *
 * @retval OT_ERROR_NONE              Successfully sent a Link Metrics Management Request message.
 * @retval OT_ERROR_NO_BUFS           Insufficient buffers to generate the MLE Link Metrics Management Request message.
 * @retval OT_ERROR_INVALID_ARGS      @p aEnhAckFlags is not a valid value or @p aLinkMetricsFlags isn't correct.
 * @retval OT_ERROR_UNKNOWN_NEIGHBOR  @p aDestination is not link-local or the neighbor is not found.
 * @retval OT_ERROR_NOT_CAPABLE       The neighbor is not a Thread 1.2 device and does not support Link Metrics.
 *
 */
otError otLinkMetricsConfigEnhAckProbing(otInstance *                               aInstance,
                                         const otIp6Address *                       aDestination,
                                         otLinkMetricsEnhAckFlags                   aEnhAckFlags,
                                         const otLinkMetrics *                      aLinkMetricsFlags,
                                         otLinkMetricsMgmtResponseCallback          aCallback,
                                         void *                                     aCallbackContext,
                                         otLinkMetricsEnhAckProbingIeReportCallback aEnhAckCallback,
                                         void *                                     aEnhAckCallbackContext);

/**
 * This function sends an MLE Link Probe message.
 *
 * @param[in] aInstance       A pointer to an OpenThread instance.
 * @param[in] aDestination    A pointer to the destination address.
 * @param[in] aSeriesId       The Series ID [1, 254] which the Probe message aims at.
 * @param[in] aLength         The length of the data payload in Link Probe TLV, [0, 64] (per Thread 1.2 spec, 4.4.37).
 *
 * @retval OT_ERROR_NONE              Successfully sent a Link Probe message.
 * @retval OT_ERROR_NO_BUFS           Insufficient buffers to generate the MLE Link Probe message.
 * @retval OT_ERROR_INVALID_ARGS      @p aSeriesId or @p aLength is not within the valid range.
 * @retval OT_ERROR_UNKNOWN_NEIGHBOR  @p aDestination is not link-local or the neighbor is not found.
 * @retval OT_ERROR_NOT_CAPABLE       The neighbor is not a Thread 1.2 device and does not support Link Metrics.
 *
 */
otError otLinkMetricsSendLinkProbe(otInstance *        aInstance,
                                   const otIp6Address *aDestination,
                                   uint8_t             aSeriesId,
                                   uint8_t             aLength);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // LINK_METRICS_H_
