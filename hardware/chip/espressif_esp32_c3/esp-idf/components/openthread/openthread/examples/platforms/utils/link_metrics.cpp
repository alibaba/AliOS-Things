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

#include "link_metrics.h"

#include <openthread/link_metrics.h>

#include "common/clearable.hpp"
#include "common/linked_list.hpp"
#include "common/pool.hpp"
#include "thread/link_quality.hpp"

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE

using namespace ot;

static int8_t sNoiseFloor; ///< The noise floor used by Link Metrics. It should be set to the platform's
                           ///< noise floor (measured noise floor, receiver sensitivity or a constant).

class LinkMetricsDataInfo : public LinkedListEntry<LinkMetricsDataInfo>, public Clearable<LinkMetricsDataInfo>
{
    friend class LinkedList<LinkMetricsDataInfo>;
    friend class LinkedListEntry<LinkMetricsDataInfo>;

public:
    /**
     * Construtor.
     *
     */
    LinkMetricsDataInfo(void) { Clear(); };

    /**
     * Set the information for this object.
     *
     * @param[in]  aLinkMetrics     Flags specifying what metrics to query.
     * @param[in]  aShortAddress    Short Address of the Probing Initiator tracked by this object.
     * @param[in]  aExtAddress      A reference to the Extended Address of the Probing Initiator tracked by this
     *                              object.
     *
     */
    void Set(otLinkMetrics aLinkMetrics, otShortAddress aShortAddress, const otExtAddress &aExtAddress)
    {
        mLinkMetrics  = aLinkMetrics;
        mShortAddress = aShortAddress;
        memcpy(mExtAddress.m8, aExtAddress.m8, sizeof(aExtAddress));
    }

    /**
     * This method gets Link Metrics data stored in this object.
     *
     * TODO: Currently the order of Link Metircs data is fixed. Will update it to follow the order specified in TLV.
     *
     * @param[in]   aLqi     LQI value of the acknowledeged frame.
     * @param[in]   aRssi    RSSI value of the acknowledged frame.
     * @param[out]  aData    A pointer to the output buffer. @p aData MUST NOT be `nullptr`. The buffer must have
     *                       at least 2 bytes (per spec 4.11.3.4.4.6). Otherwise the behavior would be undefined.
     *
     * @returns  The number of bytes written. `0` on failure.
     *
     */
    uint8_t GetEnhAckData(uint8_t aLqi, int8_t aRssi, uint8_t *aData) const
    {
        enum
        {
            kEnhAckProbingDataMaxLen = 2,
        };

        uint8_t bytes = 0;

        VerifyOrExit(aData != nullptr);

        if (mLinkMetrics.mLqi)
        {
            aData[bytes++] = aLqi;
        }
        if (mLinkMetrics.mLinkMargin)
        {
            aData[bytes++] = static_cast<uint8_t>(GetLinkMargin(aRssi) * 255 /
                                                  130); // Linear scale Link Margin from [0, 130] to [0, 255]
        }
        if (bytes < kEnhAckProbingDataMaxLen && mLinkMetrics.mRssi)
        {
            aData[bytes++] =
                static_cast<uint8_t>((aRssi + 130) * 255 / 130); // Linear scale RSSI from [-130, 0] to [0, 255]
        }

    exit:
        return bytes;
    }

    /**
     * This method gets the length of Link Metrics Data.
     *
     * @returns  The number of bytes for the data.
     *
     */
    uint8_t GetEnhAckDataLen() const
    {
        return static_cast<uint8_t>(mLinkMetrics.mLqi) + static_cast<uint8_t>(mLinkMetrics.mLinkMargin) +
               static_cast<uint8_t>(mLinkMetrics.mRssi);
    }

    /**
     * This method gets the metrics configured for the Enhanced-ACK Based Probing.
     *
     * @returns  The metrics configured.
     *
     */
    otLinkMetrics GetLinkMetrics(void) const { return mLinkMetrics; }

private:
    uint8_t GetLinkMargin(int8_t aRssi) const { return LinkQualityInfo::ConvertRssToLinkMargin(sNoiseFloor, aRssi); }

    bool Matches(const otShortAddress &aShortAddress) const { return mShortAddress == aShortAddress; };

    bool Matches(const otExtAddress &aExtAddress) const
    {
        return memcmp(&mExtAddress, &aExtAddress, sizeof(otExtAddress)) == 0;
    };

    LinkMetricsDataInfo *mNext;

    otLinkMetrics mLinkMetrics;

    otShortAddress mShortAddress;
    otExtAddress   mExtAddress;
};

enum
{
    kMaxEnhAckProbingInitiator = 10,
};

typedef Pool<LinkMetricsDataInfo, kMaxEnhAckProbingInitiator> LinkMetricsDataInfoPool;

typedef LinkedList<LinkMetricsDataInfo> LinkMetricsDataInfoList;

static LinkMetricsDataInfoPool &GetLinkMetricsDataInfoPool(void)
{
    static LinkMetricsDataInfoPool sDataInfoPool;
    return sDataInfoPool;
}

static LinkMetricsDataInfoList &GetLinkMetricsDataInfoActiveList(void)
{
    static LinkMetricsDataInfoList sDataInfoActiveList;
    return sDataInfoActiveList;
}

static inline bool IsLinkMetricsClear(otLinkMetrics aLinkMetrics)
{
    return !aLinkMetrics.mPduCount && !aLinkMetrics.mLqi && !aLinkMetrics.mLinkMargin && !aLinkMetrics.mRssi;
}

void otLinkMetricsInit(int8_t aNoiseFloor)
{
    sNoiseFloor = aNoiseFloor;
}

otError otLinkMetricsConfigureEnhAckProbing(otShortAddress      aShortAddress,
                                            const otExtAddress *aExtAddress,
                                            otLinkMetrics       aLinkMetrics)
{
    otError              error    = OT_ERROR_NONE;
    LinkMetricsDataInfo *dataInfo = nullptr;

    VerifyOrExit(aExtAddress != nullptr, error = OT_ERROR_INVALID_ARGS);

    if (IsLinkMetricsClear(aLinkMetrics)) ///< Remove the entry
    {
        dataInfo = GetLinkMetricsDataInfoActiveList().RemoveMatching(aShortAddress);
        VerifyOrExit(dataInfo != nullptr, error = OT_ERROR_NOT_FOUND);
        GetLinkMetricsDataInfoPool().Free(*dataInfo);
    }
    else
    {
        dataInfo = GetLinkMetricsDataInfoActiveList().FindMatching(aShortAddress);

        if (dataInfo == nullptr)
        {
            dataInfo = GetLinkMetricsDataInfoPool().Allocate();
            VerifyOrExit(dataInfo != nullptr, error = OT_ERROR_NO_BUFS);
            dataInfo->Clear();
            GetLinkMetricsDataInfoActiveList().Push(*dataInfo);
        }

        // Overwrite the previous configuration if it already existed.
        dataInfo->Set(aLinkMetrics, aShortAddress, *aExtAddress);
    }

exit:
    return error;
}

LinkMetricsDataInfo *GetLinkMetricsInfoByMacAddress(const otMacAddress *aMacAddress)
{
    LinkMetricsDataInfo *dataInfo = nullptr;

    VerifyOrExit(aMacAddress != nullptr);

    if (aMacAddress->mType == OT_MAC_ADDRESS_TYPE_SHORT)
    {
        dataInfo = GetLinkMetricsDataInfoActiveList().FindMatching(aMacAddress->mAddress.mShortAddress);
    }
    else if (aMacAddress->mType == OT_MAC_ADDRESS_TYPE_EXTENDED)
    {
        dataInfo = GetLinkMetricsDataInfoActiveList().FindMatching(aMacAddress->mAddress.mExtAddress);
    }

exit:
    return dataInfo;
}

uint8_t otLinkMetricsEnhAckGenData(const otMacAddress *aMacAddress, uint8_t aLqi, int8_t aRssi, uint8_t *aData)
{
    uint8_t              bytes    = 0;
    LinkMetricsDataInfo *dataInfo = GetLinkMetricsInfoByMacAddress(aMacAddress);

    VerifyOrExit(dataInfo != nullptr);

    bytes = dataInfo->GetEnhAckData(aLqi, aRssi, aData);

exit:
    return bytes;
}

uint8_t otLinkMetricsEnhAckGetDataLen(const otMacAddress *aMacAddress)
{
    uint8_t              len      = 0;
    LinkMetricsDataInfo *dataInfo = GetLinkMetricsInfoByMacAddress(aMacAddress);

    VerifyOrExit(dataInfo != nullptr);
    len = dataInfo->GetEnhAckDataLen();

exit:
    return len;
}
#endif // OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
