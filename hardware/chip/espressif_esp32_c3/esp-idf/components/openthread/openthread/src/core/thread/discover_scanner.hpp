/*
 *  Copyright (c) 2016-2020, The OpenThread Authors.
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
 *   This file includes definitions for MLE Discover Scan process.
 */

#ifndef DISCOVER_SCANNER_HPP_
#define DISCOVER_SCANNER_HPP_

#include "openthread-core-config.h"

#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/timer.hpp"
#include "mac/channel_mask.hpp"
#include "mac/mac.hpp"
#include "mac/mac_types.hpp"
#include "meshcop/meshcop.hpp"
#include "thread/mle.hpp"

namespace ot {

class MeshForwarder;

namespace Mle {

/**
 * This class implements MLE Discover Scan.
 *
 */
class DiscoverScanner : public InstanceLocator, private NonCopyable
{
    friend class ot::Instance;
    friend class ot::MeshForwarder;
    friend class Mle;

public:
    /**
     * Default scan duration (per channel), in milliseconds.
     *
     */
    static constexpr uint32_t kDefaultScanDuration = Mac::kScanDurationDefault;

    /**
     * This type represents Discover Scan result.
     *
     */
    typedef otActiveScanResult ScanResult;

    /**
     * This type represents the handler function pointer called with any Discover Scan result or when the scan
     * completes.
     *
     * The handler function format is `void (*oHandler)(ScanResult *aResult, void *aContext);`. End of scan is
     * indicated by `aResult` pointer being set to nullptr.
     *
     */
    typedef otHandleActiveScanResult Handler;

    /**
     * This type represents the filter indexes, i.e., hash bit index values for the bloom filter (calculated from a
     * Joiner ID).
     *
     * This is used when filtering is enabled during Discover Scan, i.e., received MLE Discovery Responses with steering
     * data (bloom filter) not containing the given indexes are filtered.
     *
     */
    typedef MeshCoP::SteeringData::HashBitIndexes FilterIndexes;

    /**
     * This constructor initializes the object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit DiscoverScanner(Instance &aInstance);

    /**
     * This method starts a Thread Discovery Scan.
     *
     * @param[in]  aScanChannels      Channel mask listing channels to scan (if empty, use all supported channels).
     * @param[in]  aPanId             The PAN ID filter (set to Broadcast PAN to disable filter).
     * @param[in]  aJoiner            Value of the Joiner Flag in the Discovery Request TLV.
     * @param[in]  aEnableFiltering   Enable filtering MLE Discovery Responses with steering data not containing a
     *                                given filter indexes.
     * @param[in]  aFilterIndexes     A pointer to `FilterIndexes` to use for filtering (when enabled).
     *                                If set to nullptr, filter indexes are derived from hash of factory-assigned EUI64.
     * @param[in]  aHandler           A pointer to a function that is called on receiving an MLE Discovery Response.
     * @param[in]  aContext           A pointer to arbitrary context information.
     *
     * @retval kErrorNone           Successfully started a Thread Discovery Scan.
     * @retval kErrorInvalidState   The IPv6 interface is not enabled (netif is not up).
     * @retval kErrorNoBufs         Could not allocate message for Discovery Request.
     * @retval kErrorBusy           Thread Discovery Scan is already in progress.
     *
     */
    Error Discover(const Mac::ChannelMask &aScanChannels,
                   Mac::PanId              aPanId,
                   bool                    aJoiner,
                   bool                    aEnableFiltering,
                   const FilterIndexes *   aFilterIndexes,
                   Handler                 aCallback,
                   void *                  aContext);

    /**
     * This method indicates whether or not an MLE Thread Discovery Scan is currently in progress.
     *
     * @returns true if an MLE Thread Discovery Scan is in progress, false otherwise.
     *
     */
    bool IsInProgress(void) const { return (mState != kStateIdle); }

    /**
     * This method sets Joiner Advertisement.
     *
     * @param[in]  aOui             The Vendor OUI for Joiner Advertisement.
     * @param[in]  aAdvData         A pointer to AdvData for Joiner Advertisement.
     * @param[in]  aAdvDataLength   The length of AdvData.
     *
     * @retval kErrorNone           Successfully set Joiner Advertisement.
     * @retval kErrorInvalidArgs    Invalid AdvData.
     *
     */
    Error SetJoinerAdvertisement(uint32_t aOui, const uint8_t *aAdvData, uint8_t aAdvDataLength);

private:
    enum State : uint8_t
    {
        kStateIdle,
        kStateScanning,
        kStateScanDone,
    };

    static constexpr uint32_t kMaxOui = 0xffffff;

    // Methods used by `MeshForwarder`
    Mac::TxFrame *PrepareDiscoveryRequestFrame(Mac::TxFrame &aFrame);
    void          HandleDiscoveryRequestFrameTxDone(Message &aMessage);
    void          Stop(void) { HandleDiscoverComplete(); }

    // Methods used from `Mle`
    void HandleDiscoveryResponse(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo) const;

    void        HandleDiscoverComplete(void);
    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);

    Handler          mHandler;
    void *           mHandlerContext;
    TimerMilli       mTimer;
    FilterIndexes    mFilterIndexes;
    Mac::ChannelMask mScanChannels;
    State            mState;
    uint32_t         mOui;
    uint8_t          mScanChannel;
    uint8_t          mAdvDataLength;
    uint8_t          mAdvData[MeshCoP::JoinerAdvertisementTlv::kAdvDataMaxLength];
    bool             mEnableFiltering : 1;
    bool             mShouldRestorePanId : 1;
};

} // namespace Mle
} // namespace ot

#endif // DISCOVER_SCANNER_HPP_
