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
 *   This file includes definitions for the IEEE 802.15.4 MAC.
 */

#ifndef MAC_HPP_
#define MAC_HPP_

#include "openthread-core-config.h"

#include <openthread/platform/radio.h>
#include <openthread/platform/time.h>

#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/tasklet.hpp"
#include "common/time.hpp"
#include "common/timer.hpp"
#include "mac/channel_mask.hpp"
#include "mac/mac_filter.hpp"
#include "mac/mac_frame.hpp"
#include "mac/mac_links.hpp"
#include "mac/mac_types.hpp"
#include "mac/sub_mac.hpp"
#include "radio/trel_link.hpp"
#include "thread/key_manager.hpp"
#include "thread/link_quality.hpp"

namespace ot {

class Neighbor;

/**
 * @addtogroup core-mac
 *
 * @brief
 *   This module includes definitions for the IEEE 802.15.4 MAC
 *
 * @{
 *
 */

namespace Mac {

constexpr uint32_t kDataPollTimeout = 100; ///< Timeout for receiving Data Frame (in msec).
constexpr uint32_t kSleepDelay      = 300; ///< Max sleep delay when frame is pending (in msec).

constexpr uint16_t kScanDurationDefault = OPENTHREAD_CONFIG_MAC_SCAN_DURATION; ///< Duration per channel (in msec).

constexpr uint8_t kMaxCsmaBackoffsDirect   = OPENTHREAD_CONFIG_MAC_MAX_CSMA_BACKOFFS_DIRECT;
constexpr uint8_t kMaxCsmaBackoffsIndirect = OPENTHREAD_CONFIG_MAC_MAX_CSMA_BACKOFFS_INDIRECT;
constexpr uint8_t kMaxCsmaBackoffsCsl      = 0;

constexpr uint8_t kDefaultMaxFrameRetriesDirect   = OPENTHREAD_CONFIG_MAC_DEFAULT_MAX_FRAME_RETRIES_DIRECT;
constexpr uint8_t kDefaultMaxFrameRetriesIndirect = OPENTHREAD_CONFIG_MAC_DEFAULT_MAX_FRAME_RETRIES_INDIRECT;
constexpr uint8_t kMaxFrameRetriesCsl             = 0;

constexpr uint8_t kTxNumBcast = OPENTHREAD_CONFIG_MAC_TX_NUM_BCAST; ///< Num of times broadcast frame is tx.

/**
 * This type defines the function pointer called on receiving an IEEE 802.15.4 Beacon during an Active Scan.
 *
 */
typedef otHandleActiveScanResult ActiveScanHandler;

/**
 * This type defines an Active Scan result.
 *
 */
typedef otActiveScanResult ActiveScanResult;

/**
 * This type defines the function pointer which is called during an Energy Scan when the scan result for a channel is
 * ready or when the scan completes.
 *
 */
typedef otHandleEnergyScanResult EnergyScanHandler;

/**
 * This type defines an Energy Scan result.
 *
 */
typedef otEnergyScanResult EnergyScanResult;

/**
 * This class implements the IEEE 802.15.4 MAC.
 *
 */
class Mac : public InstanceLocator, private NonCopyable
{
    friend class ot::Instance;

public:
    /**
     * This constructor initializes the MAC object.
     *
     * @param[in]  aInstance  A reference to the OpenThread instance.
     *
     */
    explicit Mac(Instance &aInstance);

    /**
     * This method starts an IEEE 802.15.4 Active Scan.
     *
     * @param[in]  aScanChannels  A bit vector indicating which channels to scan. Zero is mapped to all channels.
     * @param[in]  aScanDuration  The time in milliseconds to spend scanning each channel. Zero duration maps to
     *                            default value `kScanDurationDefault` = 300 ms.
     * @param[in]  aHandler       A pointer to a function that is called on receiving an IEEE 802.15.4 Beacon.
     * @param[in]  aContext       A pointer to an arbitrary context (used when invoking `aHandler` callback).
     *
     * @retval kErrorNone  Successfully scheduled the Active Scan request.
     * @retval kErrorBusy  Could not schedule the scan (a scan is ongoing or scheduled).
     *
     */
    Error ActiveScan(uint32_t aScanChannels, uint16_t aScanDuration, ActiveScanHandler aHandler, void *aContext);

    /**
     * This method starts an IEEE 802.15.4 Energy Scan.
     *
     * @param[in]  aScanChannels     A bit vector indicating on which channels to scan. Zero is mapped to all channels.
     * @param[in]  aScanDuration     The time in milliseconds to spend scanning each channel. If the duration is set to
     *                               zero, a single RSSI sample will be taken per channel.
     * @param[in]  aHandler          A pointer to a function called to pass on scan result or indicate scan completion.
     * @param[in]  aContext          A pointer to an arbitrary context (used when invoking @p aHandler callback).
     *
     * @retval kErrorNone  Accepted the Energy Scan request.
     * @retval kErrorBusy  Could not start the energy scan.
     *
     */
    Error EnergyScan(uint32_t aScanChannels, uint16_t aScanDuration, EnergyScanHandler aHandler, void *aContext);

    /**
     * This method indicates the energy scan for the current channel is complete.
     *
     * @param[in]  aEnergyScanMaxRssi  The maximum RSSI encountered on the scanned channel.
     *
     */
    void EnergyScanDone(int8_t aEnergyScanMaxRssi);

    /**
     * This method indicates whether or not IEEE 802.15.4 Beacon transmissions are enabled.
     *
     * @retval TRUE   If IEEE 802.15.4 Beacon transmissions are enabled.
     * @retval FALSE  If IEEE 802.15.4 Beacon transmissions are not enabled.
     *
     */
    bool IsBeaconEnabled(void) const { return mBeaconsEnabled; }

    /**
     * This method enables/disables IEEE 802.15.4 Beacon transmissions.
     *
     * @param[in]  aEnabled  TRUE to enable IEEE 802.15.4 Beacon transmissions, FALSE otherwise.
     *
     */
    void SetBeaconEnabled(bool aEnabled) { mBeaconsEnabled = aEnabled; }

    /**
     * This method indicates whether or not rx-on-when-idle is enabled.
     *
     * @retval TRUE   If rx-on-when-idle is enabled.
     * @retval FALSE  If rx-on-when-idle is not enabled.
     */
    bool GetRxOnWhenIdle(void) const { return mRxOnWhenIdle; }

    /**
     * This method sets the rx-on-when-idle mode.
     *
     * @param[in]  aRxOnWhenIdle  The rx-on-when-idle mode.
     *
     */
    void SetRxOnWhenIdle(bool aRxOnWhenIdle);

    /**
     * This method requests a direct data frame transmission.
     *
     */
    void RequestDirectFrameTransmission(void);

#if OPENTHREAD_FTD
    /**
     * This method requests an indirect data frame transmission.
     *
     */
    void RequestIndirectFrameTransmission(void);

#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    /**
     * This method requests `Mac` to start a CSL tx operation after a delay of @p aDelay time.
     *
     * @param[in]  aDelay  Delay time for `Mac` to start a CSL tx, in units of milliseconds.
     *
     */
    void RequestCslFrameTransmission(uint32_t aDelay);
#endif

#endif

    /**
     * This method requests transmission of a data poll (MAC Data Request) frame.
     *
     * @retval kErrorNone          Data poll transmission request is scheduled successfully.
     * @retval kErrorAlready       MAC is busy sending earlier poll transmission request.
     * @retval kErrorInvalidState  The MAC layer is not enabled.
     *
     */
    Error RequestDataPollTransmission(void);

    /**
     * This method returns a reference to the IEEE 802.15.4 Extended Address.
     *
     * @returns A pointer to the IEEE 802.15.4 Extended Address.
     *
     */
    const ExtAddress &GetExtAddress(void) const { return mLinks.GetExtAddress(); }

    /**
     * This method sets the IEEE 802.15.4 Extended Address.
     *
     * @param[in]  aExtAddress  A reference to the IEEE 802.15.4 Extended Address.
     *
     */
    void SetExtAddress(const ExtAddress &aExtAddress) { mLinks.SetExtAddress(aExtAddress); }

    /**
     * This method returns the IEEE 802.15.4 Short Address.
     *
     * @returns The IEEE 802.15.4 Short Address.
     *
     */
    ShortAddress GetShortAddress(void) const { return mLinks.GetShortAddress(); }

    /**
     * This method sets the IEEE 802.15.4 Short Address.
     *
     * @param[in]  aShortAddress  The IEEE 802.15.4 Short Address.
     *
     */
    void SetShortAddress(ShortAddress aShortAddress) { mLinks.SetShortAddress(aShortAddress); }

    /**
     * This method returns the IEEE 802.15.4 PAN Channel.
     *
     * @returns The IEEE 802.15.4 PAN Channel.
     *
     */
    uint8_t GetPanChannel(void) const { return mPanChannel; }

    /**
     * This method sets the IEEE 802.15.4 PAN Channel.
     *
     * @param[in]  aChannel  The IEEE 802.15.4 PAN Channel.
     *
     * @retval kErrorNone          Successfully set the IEEE 802.15.4 PAN Channel.
     * @retval kErrorInvalidArgs   The @p aChannel is not in the supported channel mask.
     *
     */
    Error SetPanChannel(uint8_t aChannel);

    /**
     * This method sets the temporary IEEE 802.15.4 radio channel.
     *
     * This method allows user to temporarily change the radio channel and use a different channel (during receive)
     * instead of the PAN channel (from `SetPanChannel()`). A call to `ClearTemporaryChannel()` would clear the
     * temporary channel and adopt the PAN channel again. The `SetTemporaryChannel()` can be used multiple times in row
     * (before a call to `ClearTemporaryChannel()`) to change the temporary channel.
     *
     * @param[in]  aChannel            A IEEE 802.15.4 channel.
     *
     * @retval kErrorNone          Successfully set the temporary channel
     * @retval kErrorInvalidArgs   The @p aChannel is not in the supported channel mask.
     *
     */
    Error SetTemporaryChannel(uint8_t aChannel);

    /**
     * This method clears the use of a previously set temporary channel and adopts the PAN channel.
     *
     */
    void ClearTemporaryChannel(void);

    /**
     * This method returns the supported channel mask.
     *
     * @returns The supported channel mask.
     *
     */
    const ChannelMask &GetSupportedChannelMask(void) const { return mSupportedChannelMask; }

    /**
     * This method sets the supported channel mask
     *
     * @param[in] aMask   The supported channel mask.
     *
     */
    void SetSupportedChannelMask(const ChannelMask &aMask);

    /**
     * This method returns the IEEE 802.15.4 Network Name.
     *
     * @returns The IEEE 802.15.4 Network Name.
     *
     */
    const NetworkName &GetNetworkName(void) const { return mNetworkName; }

    /**
     * This method sets the IEEE 802.15.4 Network Name.
     *
     * @param[in]  aNameString   A pointer to a string character array. Must be null terminated.
     *
     * @retval kErrorNone          Successfully set the IEEE 802.15.4 Network Name.
     * @retval kErrorInvalidArgs   Given name is too long.
     *
     */
    Error SetNetworkName(const char *aNameString);

    /**
     * This method sets the IEEE 802.15.4 Network Name.
     *
     * @param[in]  aNameData     A name data (pointer to char buffer and length).
     *
     * @retval kErrorNone          Successfully set the IEEE 802.15.4 Network Name.
     * @retval kErrorInvalidArgs   Given name is too long.
     *
     */
    Error SetNetworkName(const NameData &aNameData);

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    /**
     * This method returns the Thread Domain Name.
     *
     * @returns The Thread Domain Name.
     *
     */
    const DomainName &GetDomainName(void) const { return mDomainName; }

    /**
     * This method sets the Thread Domain Name.
     *
     * @param[in]  aNameString   A pointer to a string character array. Must be null terminated.
     *
     * @retval kErrorNone          Successfully set the Thread Domain Name.
     * @retval kErrorInvalidArgs   Given name is too long.
     *
     */
    Error SetDomainName(const char *aNameString);

    /**
     * This method sets the Thread Domain Name.
     *
     * @param[in]  aNameData     A name data (pointer to char buffer and length).
     *
     * @retval kErrorNone          Successfully set the Thread Domain Name.
     * @retval kErrorInvalidArgs   Given name is too long.
     *
     */
    Error SetDomainName(const NameData &aNameData);
#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

    /**
     * This method returns the IEEE 802.15.4 PAN ID.
     *
     * @returns The IEEE 802.15.4 PAN ID.
     *
     */
    PanId GetPanId(void) const { return mPanId; }

    /**
     * This method sets the IEEE 802.15.4 PAN ID.
     *
     * @param[in]  aPanId  The IEEE 802.15.4 PAN ID.
     *
     */
    void SetPanId(PanId aPanId);

    /**
     * This method returns the IEEE 802.15.4 Extended PAN Identifier.
     *
     * @returns The IEEE 802.15.4 Extended PAN Identifier.
     *
     */
    const ExtendedPanId &GetExtendedPanId(void) const { return mExtendedPanId; }

    /**
     * This method sets the IEEE 802.15.4 Extended PAN Identifier.
     *
     * @param[in]  aExtendedPanId  The IEEE 802.15.4 Extended PAN Identifier.
     *
     */
    void SetExtendedPanId(const ExtendedPanId &aExtendedPanId);

    /**
     * This method returns the maximum number of frame retries during direct transmission.
     *
     * @returns The maximum number of retries during direct transmission.
     *
     */
    uint8_t GetMaxFrameRetriesDirect(void) const { return mMaxFrameRetriesDirect; }

    /**
     * This method sets the maximum number of frame retries during direct transmission.
     *
     * @param[in]  aMaxFrameRetriesDirect  The maximum number of retries during direct transmission.
     *
     */
    void SetMaxFrameRetriesDirect(uint8_t aMaxFrameRetriesDirect) { mMaxFrameRetriesDirect = aMaxFrameRetriesDirect; }

#if OPENTHREAD_FTD
    /**
     * This method returns the maximum number of frame retries during indirect transmission.
     *
     * @returns The maximum number of retries during indirect transmission.
     *
     */
    uint8_t GetMaxFrameRetriesIndirect(void) const { return mMaxFrameRetriesIndirect; }

    /**
     * This method sets the maximum number of frame retries during indirect transmission.
     *
     * @param[in]  aMaxFrameRetriesIndirect  The maximum number of retries during indirect transmission.
     *
     */
    void SetMaxFrameRetriesIndirect(uint8_t aMaxFrameRetriesIndirect)
    {
        mMaxFrameRetriesIndirect = aMaxFrameRetriesIndirect;
    }
#endif

    /**
     * This method is called to handle a received frame.
     *
     * @param[in]  aFrame  A pointer to the received frame, or nullptr if the receive operation was aborted.
     * @param[in]  aError  kErrorNone when successfully received a frame,
     *                     kErrorAbort when reception was aborted and a frame was not received.
     *
     */
    void HandleReceivedFrame(RxFrame *aFrame, Error aError);

    /**
     * This method records CCA status (success/failure) for a frame transmission attempt.
     *
     * @param[in] aCcaSuccess   TRUE if the CCA succeeded, FALSE otherwise.
     * @param[in] aChannel      The channel on which CCA was performed.
     *
     */
    void RecordCcaStatus(bool aCcaSuccess, uint8_t aChannel);

    /**
     * This method records the status of a frame transmission attempt, updating MAC counters.
     *
     * Unlike `HandleTransmitDone` which is called after all transmission attempts of frame to indicate final status
     * of a frame transmission request, this method is invoked on all frame transmission attempts.
     *
     * @param[in] aFrame      The transmitted frame.
     * @param[in] aAckFrame   A pointer to the ACK frame, or nullptr if no ACK was received.
     * @param[in] aError      kErrorNone when the frame was transmitted successfully,
     *                        kErrorNoAck when the frame was transmitted but no ACK was received,
     *                        kErrorChannelAccessFailure tx failed due to activity on the channel,
     *                        kErrorAbort when transmission was aborted for other reasons.
     * @param[in] aRetryCount Indicates number of transmission retries for this frame.
     * @param[in] aWillRetx   Indicates whether frame will be retransmitted or not. This is applicable only
     *                        when there was an error in transmission (i.e., `aError` is not NONE).
     *
     */
    void RecordFrameTransmitStatus(const TxFrame &aFrame,
                                   const RxFrame *aAckFrame,
                                   Error          aError,
                                   uint8_t        aRetryCount,
                                   bool           aWillRetx);

    /**
     * This method is called to handle transmit events.
     *
     * @param[in]  aFrame      The frame that was transmitted.
     * @param[in]  aAckFrame   A pointer to the ACK frame, nullptr if no ACK was received.
     * @param[in]  aError      kErrorNone when the frame was transmitted successfully,
     *                         kErrorNoAck when the frame was transmitted but no ACK was received,
     *                         kErrorChannelAccessFailure when the tx failed due to activity on the channel,
     *                         kErrorAbort when transmission was aborted for other reasons.
     *
     */
    void HandleTransmitDone(TxFrame &aFrame, RxFrame *aAckFrame, Error aError);

    /**
     * This method returns if an active scan is in progress.
     *
     */
    bool IsActiveScanInProgress(void) const { return (mOperation == kOperationActiveScan) || (mPendingActiveScan); }

    /**
     * This method returns if an energy scan is in progress.
     *
     */
    bool IsEnergyScanInProgress(void) const { return (mOperation == kOperationEnergyScan) || (mPendingEnergyScan); }

#if OPENTHREAD_FTD
    /**
     * This method indicates whether the MAC layer is performing an indirect transmission (in middle of a tx).
     *
     * @returns TRUE if in middle of an indirect transmission, FALSE otherwise.
     *
     */
    bool IsPerformingIndirectTransmit(void) const { return (mOperation == kOperationTransmitDataIndirect); }
#endif

    /**
     * This method returns if the MAC layer is in transmit state.
     *
     * The MAC layer is in transmit state during CSMA/CA, CCA, transmission of Data, Beacon or Data Request frames and
     * receiving of ACK frames. The MAC layer is not in transmit state during transmission of ACK frames or Beacon
     * Requests.
     *
     */
    bool IsInTransmitState(void) const;

    /**
     * This method registers a callback to provide received raw IEEE 802.15.4 frames.
     *
     * @param[in]  aPcapCallback     A pointer to a function that is called when receiving an IEEE 802.15.4 link frame
     * or nullptr to disable the callback.
     * @param[in]  aCallbackContext  A pointer to application-specific context.
     *
     */
    void SetPcapCallback(otLinkPcapCallback aPcapCallback, void *aCallbackContext)
    {
        mLinks.SetPcapCallback(aPcapCallback, aCallbackContext);
    }

    /**
     * This method indicates whether or not promiscuous mode is enabled at the link layer.
     *
     * @retval true   Promiscuous mode is enabled.
     * @retval false  Promiscuous mode is not enabled.
     *
     */
    bool IsPromiscuous(void) const { return mPromiscuous; }

    /**
     * This method enables or disables the link layer promiscuous mode.
     *
     * Promiscuous mode keeps the receiver enabled, overriding the value of mRxOnWhenIdle.
     *
     * @param[in]  aPromiscuous  true to enable promiscuous mode, or false otherwise.
     *
     */
    void SetPromiscuous(bool aPromiscuous);

    /**
     * This method resets mac counters
     *
     */
    void ResetCounters(void) { memset(&mCounters, 0, sizeof(mCounters)); }

    /**
     * This method returns the MAC counter.
     *
     * @returns A reference to the MAC counter.
     *
     */
    otMacCounters &GetCounters(void) { return mCounters; }

#if OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_ENABLE
    /**
     * This method returns the MAC retry histogram for direct transmission.
     *
     * @param[out]  aNumberOfEntries    A reference to where the size of returned histogram array is placed.
     *
     * @returns     A pointer to the histogram of retries (in a form of an array).
     *              The n-th element indicates that the packet has been sent with n-th retry.
     *
     */
    const uint32_t *GetDirectRetrySuccessHistogram(uint8_t &aNumberOfEntries);

#if OPENTHREAD_FTD
    /**
     * This method returns the MAC retry histogram for indirect transmission.
     *
     * @param[out]  aNumberOfEntries    A reference to where the size of returned histogram array is placed.
     *
     * @returns     A pointer to the histogram of retries (in a form of an array).
     *              The n-th element indicates that the packet has been sent with n-th retry.
     *
     */
    const uint32_t *GetIndirectRetrySuccessHistogram(uint8_t &aNumberOfEntries);
#endif

    /**
     * This method resets MAC retry histogram.
     *
     */
    void ResetRetrySuccessHistogram(void);
#endif // OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_ENABLE

    /**
     * This method returns the noise floor value (currently use the radio receive sensitivity value).
     *
     * @returns The noise floor value in dBm.
     *
     */
    int8_t GetNoiseFloor(void) { return mLinks.GetNoiseFloor(); }

    /**
     * This method returns the current CCA (Clear Channel Assessment) failure rate.
     *
     * The rate is maintained over a window of (roughly) last `OPENTHREAD_CONFIG_CCA_FAILURE_RATE_AVERAGING_WINDOW`
     * frame transmissions.
     *
     * @returns The CCA failure rate with maximum value `0xffff` corresponding to 100% failure rate.
     *
     */
    uint16_t GetCcaFailureRate(void) const { return mCcaSuccessRateTracker.GetFailureRate(); }

    /**
     * This method Starts/Stops the Link layer. It may only be used when the Netif Interface is down.
     *
     * @param[in]  aEnable The requested State for the MAC layer. true - Start, false - Stop.
     *
     */
    void SetEnabled(bool aEnable) { mEnabled = aEnable; }

    /**
     * This method indicates whether or not the link layer is enabled.
     *
     * @retval true   Link layer is enabled.
     * @retval false  Link layer is not enabled.
     *
     */
    bool IsEnabled(void) const { return mEnabled; }

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    /**
     * This method gets the CSL channel.
     *
     * @returns CSL channel.
     *
     */
    uint8_t GetCslChannel(void) const { return mLinks.GetSubMac().GetCslChannel(); }

    /**
     * This method sets the CSL channel.
     *
     * @param[in]  aChannel  The CSL channel.
     *
     */
    void SetCslChannel(uint8_t aChannel);

    /**
     * This method indicates if CSL channel has been explicitly specified by the upper layer.
     *
     * @returns If CSL channel has been specified.
     *
     */
    bool IsCslChannelSpecified(void) const { return mLinks.GetSubMac().IsCslChannelSpecified(); }

    /**
     * This method gets the CSL period.
     *
     * @returns CSL period in units of 10 symbols.
     *
     */
    uint16_t GetCslPeriod(void) const { return mLinks.GetSubMac().GetCslPeriod(); }

    /**
     * This method sets the CSL period.
     *
     * @param[in]  aPeriod  The CSL period in 10 symbols.
     *
     */
    void SetCslPeriod(uint16_t aPeriod);

    /**
     * This method indicates whether CSL is started at the moment.
     *
     * @retval TRUE   If CSL is enabled.
     * @retval FALSE  If CSL is not enabled.
     *
     */
    bool IsCslEnabled(void) const;

    /**
     * This method indicates whether Link is capable of starting CSL.
     *
     * @retval TRUE   If Link is capable of starting CSL.
     * @retval FALSE  If link is not capable of starting CSL.
     *
     */
    bool IsCslCapable(void) const;

    /**
     * This method returns CSL parent clock accuracy, in ± ppm.
     *
     * @retval CSL parent clock accuracy, in ± ppm.
     *
     */
    uint8_t GetCslParentClockAccuracy(void) const { return mLinks.GetSubMac().GetCslParentClockAccuracy(); }

    /**
     * This method sets CSL parent clock accuracy, in ± ppm.
     *
     * @param[in] aCslParentAccuracy CSL parent clock accuracy, in ± ppm.
     *
     */
    void SetCslParentClockAccuracy(uint8_t aCslParentAccuracy)
    {
        mLinks.GetSubMac().SetCslParentClockAccuracy(aCslParentAccuracy);
    }

    /**
     * This method returns CSL parent uncertainty, in ±10 us units.
     *
     * @retval CSL parent uncertainty, in ±10 us units.
     *
     */
    uint8_t GetCslParentUncertainty(void) const { return mLinks.GetSubMac().GetCslParentUncertainty(); }

    /**
     * This method returns CSL parent uncertainty, in ±10 us units.
     *
     * @param[in] aCslParentUncert  CSL parent uncertainty, in ±10 us units.
     *
     */
    void SetCslParentUncertainty(uint8_t aCslParentUncert)
    {
        mLinks.GetSubMac().SetCslParentUncertainty(aCslParentUncert);
    }
#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

private:
    static constexpr int8_t   kInvalidRssiValue  = SubMac::kInvalidRssiValue;
    static constexpr uint16_t kMaxCcaSampleCount = OPENTHREAD_CONFIG_CCA_FAILURE_RATE_AVERAGING_WINDOW;

    enum Operation : uint8_t
    {
        kOperationIdle = 0,
        kOperationActiveScan,
        kOperationEnergyScan,
        kOperationTransmitBeacon,
        kOperationTransmitDataDirect,
        kOperationTransmitPoll,
        kOperationWaitingForData,
#if OPENTHREAD_FTD
        kOperationTransmitDataIndirect,
#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
        kOperationTransmitDataCsl,
#endif
#endif
    };

#if OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_ENABLE
    struct RetryHistogram
    {
        /**
         * Histogram of number of retries for a single direct packet until success
         * [0 retry: packet count, 1 retry: packet count, 2 retry : packet count ...
         *  until max retry limit: packet count]
         *
         *  The size of the array is OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_MAX_SIZE_COUNT_DIRECT.
         */
        uint32_t mTxDirectRetrySuccess[OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_MAX_SIZE_COUNT_DIRECT];

        /**
         * Histogram of number of retries for a single indirect packet until success
         * [0 retry: packet count, 1 retry: packet count, 2 retry : packet count ...
         *  until max retry limit: packet count]
         *
         *  The size of the array is OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_MAX_SIZE_COUNT_INDIRECT.
         */
        uint32_t mTxIndirectRetrySuccess[OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_MAX_SIZE_COUNT_INDIRECT];
    };
#endif // OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_ENABLE

    Error ProcessReceiveSecurity(RxFrame &aFrame, const Address &aSrcAddr, Neighbor *aNeighbor);
    void  ProcessTransmitSecurity(TxFrame &aFrame);
#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
    Error ProcessEnhAckSecurity(TxFrame &aTxFrame, RxFrame &aAckFrame);
#endif

    void     UpdateIdleMode(void);
    void     StartOperation(Operation aOperation);
    void     FinishOperation(void);
    void     PerformNextOperation(void);
    TxFrame *PrepareBeaconRequest(void);
    TxFrame *PrepareBeacon(void);
    bool     ShouldSendBeacon(void) const;
    bool     IsJoinable(void) const;
    void     BeginTransmit(void);
    bool     HandleMacCommand(RxFrame &aFrame);

    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);
    static void HandleOperationTask(Tasklet &aTasklet);

    void  Scan(Operation aScanOperation, uint32_t aScanChannels, uint16_t aScanDuration);
    Error UpdateScanChannel(void);
    void  PerformActiveScan(void);
    void  ReportActiveScanResult(const RxFrame *aBeaconFrame);
    Error ConvertBeaconToActiveScanResult(const RxFrame *aBeaconFrame, ActiveScanResult &aResult);
    void  PerformEnergyScan(void);
    void  ReportEnergyScanResult(int8_t aRssi);
    Error SignalNetworkNameChange(Error aError);

    void LogFrameRxFailure(const RxFrame *aFrame, Error aError) const;
    void LogFrameTxFailure(const TxFrame &aFrame, Error aError, uint8_t aRetryCount, bool aWillRetx) const;
    void LogBeacon(const char *aActionText, const BeaconPayload &aBeaconPayload) const;

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    uint8_t GetTimeIeOffset(const Frame &aFrame);
#endif

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    void ProcessCsl(const RxFrame &aFrame, const Address &aSrcAddr);
#endif
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    void ProcessEnhAckProbing(const RxFrame &aFrame, const Neighbor &aNeighbor);
#endif
    static const char *OperationToString(Operation aOperation);

    static const otExtAddress    sMode2ExtAddress;
    static const otExtendedPanId sExtendedPanidInit;
    static const char            sNetworkNameInit[];
    static const char            sDomainNameInit[];

    bool mEnabled : 1;
    bool mPendingActiveScan : 1;
    bool mPendingEnergyScan : 1;
    bool mPendingTransmitBeacon : 1;
    bool mPendingTransmitDataDirect : 1;
#if OPENTHREAD_FTD
    bool mPendingTransmitDataIndirect : 1;
#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    bool mPendingTransmitDataCsl : 1;
#endif
#endif
    bool mPendingTransmitPoll : 1;
    bool mPendingWaitingForData : 1;
    bool mShouldTxPollBeforeData : 1;
    bool mRxOnWhenIdle : 1;
    bool mPromiscuous : 1;
    bool mBeaconsEnabled : 1;
    bool mUsingTemporaryChannel : 1;
#if OPENTHREAD_CONFIG_MAC_STAY_AWAKE_BETWEEN_FRAGMENTS
    bool mShouldDelaySleep : 1;
    bool mDelayingSleep : 1;
#endif

    Operation     mOperation;
    uint8_t       mBeaconSequence;
    uint8_t       mDataSequence;
    uint8_t       mBroadcastTransmitCount;
    PanId         mPanId;
    uint8_t       mPanChannel;
    uint8_t       mRadioChannel;
    ChannelMask   mSupportedChannelMask;
    ExtendedPanId mExtendedPanId;
    NetworkName   mNetworkName;
#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    DomainName mDomainName;
#endif
    uint8_t     mScanChannel;
    uint16_t    mScanDuration;
    ChannelMask mScanChannelMask;
    uint8_t     mMaxFrameRetriesDirect;
#if OPENTHREAD_FTD
    uint8_t mMaxFrameRetriesIndirect;
#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    TimeMilli mCslTxFireTime;
#endif
#endif

    union
    {
        ActiveScanHandler mActiveScanHandler;
        EnergyScanHandler mEnergyScanHandler;
    };

    void *mScanHandlerContext;

    Links              mLinks;
    Tasklet            mOperationTask;
    TimerMilli         mTimer;
    otMacCounters      mCounters;
    uint32_t           mKeyIdMode2FrameCounter;
    SuccessRateTracker mCcaSuccessRateTracker;
    uint16_t           mCcaSampleCount;
#if OPENTHREAD_CONFIG_MAC_RETRY_SUCCESS_HISTOGRAM_ENABLE
    RetryHistogram mRetryHistogram;
#endif

#if OPENTHREAD_CONFIG_MULTI_RADIO
    RadioTypes mTxPendingRadioLinks;
    RadioTypes mTxBeaconRadioLinks;
    Error      mTxError;
#endif

#if OPENTHREAD_CONFIG_MAC_FILTER_ENABLE
    Filter mFilter;
#endif // OPENTHREAD_CONFIG_MAC_FILTER_ENABLE

    KeyMaterial mMode2KeyMaterial;
};

/**
 * @}
 *
 */

} // namespace Mac
} // namespace ot

#endif // MAC_HPP_
