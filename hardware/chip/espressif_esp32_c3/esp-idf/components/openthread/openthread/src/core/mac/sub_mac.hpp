/*
 *  Copyright (c) 2016-2018, The OpenThread Authors.
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
 *   This file includes definitions for the IEEE 802.15.4 MAC layer (sub-MAC).
 */

#ifndef SUB_MAC_HPP_
#define SUB_MAC_HPP_

#include "openthread-core-config.h"

#include <openthread/link.h>

#include <openthread/platform/crypto.h>

#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/timer.hpp"
#include "mac/mac_frame.hpp"
#include "radio/radio.hpp"

namespace ot {

/**
 * @addtogroup core-mac
 *
 * @brief
 *   This module includes definitions for the IEEE 802.15.4 MAC (sub-MAC).
 *
 * @{
 *
 */

namespace Mac {

#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE && (OPENTHREAD_CONFIG_THREAD_VERSION < OT_THREAD_VERSION_1_2)
#error "Thread 1.2 or higher version is required for OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE."
#endif

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

#if (OPENTHREAD_CONFIG_THREAD_VERSION < OT_THREAD_VERSION_1_2)
#error "Thread 1.2 or higher version is required for OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE."
#endif

#if !OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
#error "Microsecond timer OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE is required for "\
    "OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE"
#endif

#endif

#if OPENTHREAD_CONFIG_MAC_CSL_DEBUG_ENABLE && !OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
#error "OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE is required for OPENTHREAD_CONFIG_MAC_CSL_DEBUG_ENABLE."
#endif

#if OPENTHREAD_RADIO || OPENTHREAD_CONFIG_LINK_RAW_ENABLE
class LinkRaw;
#endif

/**
 * This class implements the IEEE 802.15.4 MAC (sub-MAC).
 *
 * Sub-MAC layer implements a subset of IEEE802.15.4 MAC primitives which are shared by both MAC layer (in FTD/MTD
 * modes) and Raw Link (Radio only mode).

 * The sub-MAC layer handles the following (if not provided by radio platform):
 *
 *    - Ack timeout for frame transmission,
 *    - CSMA backoff logic,
 *    - Frame re-transmissions,
 *    - Energy scan on a single channel and RSSI sampling.
 *
 * It also act as the interface (to radio platform) for setting/getting radio configurations such as short or extended
 * addresses and PAN Id.
 *
 */
class SubMac : public InstanceLocator, private NonCopyable
{
    friend class Radio::Callbacks;
    friend class LinkRaw;

public:
    static constexpr int8_t kInvalidRssiValue = 127; ///< Invalid Received Signal Strength Indicator (RSSI) value.

    /**
     * This class defines the callbacks notifying `SubMac` user of changes and events.
     *
     */
    class Callbacks : public InstanceLocator
    {
    public:
        /**
         * This constructor initializes the `Callbacks` object.
         *
         * @param[in]  aInstance  A reference to the OpenThread instance.
         *
         */
        explicit Callbacks(Instance &aInstance);

        /**
         * This method notifies user of `SubMac` of a received frame.
         *
         * @param[in]  aFrame    A pointer to the received frame or nullptr if the receive operation failed.
         * @param[in]  aError    kErrorNone when successfully received a frame,
         *                       kErrorAbort when reception was aborted and a frame was not received,
         *                       kErrorNoBufs when a frame could not be received due to lack of rx buffer space.
         *
         */
        void ReceiveDone(RxFrame *aFrame, Error aError);

        /**
         * This method notifies user of `SubMac` of CCA status (success/failure) for a frame transmission attempt.
         *
         * This is intended for updating counters, logging, and/or tracking CCA failure rate statistics.
         *
         * @param[in] aCcaSuccess   TRUE if the CCA succeeded, FALSE otherwise.
         * @param[in] aChannel      The channel on which CCA was performed.
         *
         */
        void RecordCcaStatus(bool aCcaSuccess, uint8_t aChannel);

        /**
         * This method notifies user of `SubMac` of the status of a frame transmission attempt.
         *
         * This is intended for updating counters, logging, and/or collecting statistics.
         *
         * @note Unlike `TransmitDone` which is invoked after all re-transmission attempts to indicate the final status
         * of a frame transmission, this method is invoked on all frame transmission attempts.
         *
         * @param[in] aFrame      The transmitted frame.
         * @param[in] aAckFrame   A pointer to the ACK frame, or nullptr if no ACK was received.
         * @param[in] aError      kErrorNone when the frame was transmitted successfully,
         *                        kErrorNoAck when the frame was transmitted but no ACK was received,
         *                        kErrorChannelAccessFailure tx failed due to activity on the channel,
         *                        kErrorAbort when transmission was aborted for other reasons.
         * @param[in] aRetryCount Current retry count. This is valid only when sub-mac handles frame re-transmissions.
         * @param[in] aWillRetx   Indicates whether frame will be retransmitted or not. This is applicable only
         *                        when there was an error in current transmission attempt.
         *
         */
        void RecordFrameTransmitStatus(const TxFrame &aFrame,
                                       const RxFrame *aAckFrame,
                                       Error          aError,
                                       uint8_t        aRetryCount,
                                       bool           aWillRetx);

        /**
         * The method notifies user of `SubMac` that the transmit operation has completed, providing, if applicable,
         * the received ACK frame.
         *
         * @param[in]  aFrame     The transmitted frame.
         * @param[in]  aAckFrame  A pointer to the ACK frame, nullptr if no ACK was received.
         * @param[in]  aError     kErrorNone when the frame was transmitted,
         *                        kErrorNoAck when the frame was transmitted but no ACK was received,
         *                        kErrorChannelAccessFailure tx failed due to activity on the channel,
         *                        kErrorAbort when transmission was aborted for other reasons.
         *
         */
        void TransmitDone(TxFrame &aFrame, RxFrame *aAckFrame, Error aError);

        /**
         * This method notifies user of `SubMac` that energy scan is complete.
         *
         * @param[in]  aMaxRssi  Maximum RSSI seen on the channel, or `SubMac::kInvalidRssiValue` if failed.
         *
         */
        void EnergyScanDone(int8_t aMaxRssi);

        /**
         * This method notifies user of `SubMac` that a specific MAC frame counter is used for transmission.
         *
         * It is possible that this callback is invoked out of order in terms of counter values (i.e., called for a
         * smaller counter value after a call for a larger counter value).
         *
         * @param[in]  aFrameCounter  The MAC frame counter value which was used.
         *
         */
        void FrameCounterUsed(uint32_t aFrameCounter);
    };

    /**
     * This constructor initializes the `SubMac` object.
     *
     * @param[in]  aInstance  A reference to the OpenThread instance.
     *
     */
    explicit SubMac(Instance &aInstance);

    /**
     * This method gets the capabilities provided by platform radio.
     *
     * @returns The capability bit vector (see `OT_RADIO_CAP_*` definitions).
     *
     */
    otRadioCaps GetRadioCaps(void) const { return mRadioCaps; }

    /**
     * This method gets the capabilities provided by `SubMac` layer.
     *
     * @returns The capability bit vector (see `OT_RADIO_CAP_*` definitions).
     *
     */
    otRadioCaps GetCaps(void) const;

    /**
     * This method sets the PAN ID.
     *
     * @param[in] aPanId  The PAN ID.
     *
     */
    void SetPanId(PanId aPanId);

    /**
     * This method gets the short address.
     *
     * @returns The short address.
     *
     */
    ShortAddress GetShortAddress(void) const { return mShortAddress; }

    /**
     * This method sets the short address.
     *
     * @param[in] aShortAddress   The short address.
     *
     */
    void SetShortAddress(ShortAddress aShortAddress);

    /**
     * This function gets the extended address.
     *
     * @returns A reference to the extended address.
     *
     */
    const ExtAddress &GetExtAddress(void) const { return mExtAddress; }

    /**
     * This method sets extended address.
     *
     * @param[in] aExtAddress  The extended address.
     *
     */
    void SetExtAddress(const ExtAddress &aExtAddress);

    /**
     * This method registers a callback to provide received packet capture for IEEE 802.15.4 frames.
     *
     * @param[in]  aPcapCallback     A pointer to a function that is called when receiving an IEEE 802.15.4 link frame
     *                                or nullptr to disable the callback.
     * @param[in]  aCallbackContext  A pointer to application-specific context.
     *
     */
    void SetPcapCallback(otLinkPcapCallback aPcapCallback, void *aCallbackContext);

    /**
     * This method indicates whether radio should stay in Receive or Sleep during CSMA backoff.
     *
     * @param[in]  aRxOnWhenBackoff  TRUE to keep radio in Receive, FALSE to put to Sleep during CSMA backoff.
     *
     */
    void SetRxOnWhenBackoff(bool aRxOnWhenBackoff) { mRxOnWhenBackoff = aRxOnWhenBackoff; }

    /**
     * This method enables the radio.
     *
     * @retval kErrorNone     Successfully enabled.
     * @retval kErrorFailed   The radio could not be enabled.
     *
     */
    Error Enable(void);

    /**
     * This method disables the radio.
     *
     * @retval kErrorNone     Successfully disabled the radio.
     *
     */
    Error Disable(void);

    /**
     * This method transitions the radio to Sleep.
     *
     * @retval kErrorNone          Successfully transitioned to Sleep.
     * @retval kErrorBusy          The radio was transmitting.
     * @retval kErrorInvalidState  The radio was disabled.
     *
     */
    Error Sleep(void);

    /**
     * This method indicates whether the sub-mac is busy transmitting or scanning.
     *
     * @retval TRUE if the sub-mac is busy transmitting or scanning.
     * @retval FALSE if the sub-mac is not busy transmitting or scanning.
     *
     */
    bool IsTransmittingOrScanning(void) const { return (mState == kStateTransmit) || (mState == kStateEnergyScan); }

    /**
     * This method transitions the radio to Receive.
     *
     * @param[in]  aChannel   The channel to use for receiving.
     *
     * @retval kErrorNone          Successfully transitioned to Receive.
     * @retval kErrorInvalidState  The radio was disabled or transmitting.
     *
     */
    Error Receive(uint8_t aChannel);

    /**
     * This method gets the radio transmit frame.
     *
     * @returns The transmit frame.
     *
     */
    TxFrame &GetTransmitFrame(void) { return mTransmitFrame; }

    /**
     * This method sends a prepared frame.
     *
     * The frame should be placed in `GetTransmitFrame()` frame.
     *
     * The `SubMac` layer handles Ack timeout, CSMA backoff, and frame retransmission.
     *
     * @retval kErrorNone          Successfully started the frame transmission
     * @retval kErrorInvalidState  The radio was disabled or transmitting.
     *
     */
    Error Send(void);

    /**
     * This method gets the number of transmit retries of last transmitted frame.
     *
     * @returns Number of transmit retries.
     *
     */
    uint8_t GetTransmitRetries(void) const { return mTransmitRetries; }

    /**
     * This method gets the most recent RSSI measurement.
     *
     * @returns The RSSI in dBm when it is valid. `kInvalidRssiValue` when RSSI is invalid.
     *
     */
    int8_t GetRssi(void) const;

    /**
     * This method begins energy scan.
     *
     * @param[in] aScanChannel   The channel to perform the energy scan on.
     * @param[in] aScanDuration  The duration, in milliseconds, for the channel to be scanned.
     *
     * @retval kErrorNone            Successfully started scanning the channel.
     * @retval kErrorInvalidState    The radio was disabled or transmitting.
     * @retval kErrorNotImplemented  Energy scan is not supported (applicable in link-raw/radio mode only).
     *
     */
    Error EnergyScan(uint8_t aScanChannel, uint16_t aScanDuration);

    /**
     * This method returns the noise floor value (currently use the radio receive sensitivity value).
     *
     * @returns The noise floor value in dBm.
     *
     */
    int8_t GetNoiseFloor(void);

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

    /**
     * This method lets `SubMac` start CSL sample.
     *
     * `SubMac` would switch the radio state between `Receive` and `Sleep` according the CSL timer. When CslSample is
     * started, `mState` will become `kStateCslSample`. But it could be doing `Sleep` or `Receive` at this moment
     * (depending on `mCslState`).
     *
     * @param[in]  aPanChannel  The current phy channel used by the device. This param will only take effect when CSL
     *                          channel hasn't been explicitly specified.
     *
     * @retval kErrorNone          Successfully entered CSL operation (sleep or receive according to CSL timer).
     * @retval kErrorBusy          The radio was transmitting.
     * @retval kErrorInvalidState  The radio was disabled.
     *
     */
    Error CslSample(uint8_t aPanChannel);

    /**
     * This method gets the CSL channel.
     *
     * @returns CSL channel.
     *
     */
    uint8_t GetCslChannel(void) const { return mCslChannel; }

    /**
     * This method sets the CSL channel.
     *
     * @param[in]  aChannel  The CSL channel. `0` to set CSL Channel unspecified.
     *
     */
    void SetCslChannel(uint8_t aChannel);

    /**
     * This method indicates if CSL channel has been explicitly specified by the upper layer.
     *
     * @returns If CSL channel has been specified.
     *
     */
    bool IsCslChannelSpecified(void) const { return mIsCslChannelSpecified; }

    /**
     * This method sets the flag representing if CSL channel has been specified.
     *
     */
    void SetCslChannelSpecified(bool aIsSpecified) { mIsCslChannelSpecified = aIsSpecified; }

    /**
     * This method gets the CSL period.
     *
     * @returns CSL period.
     *
     */
    uint16_t GetCslPeriod(void) const { return mCslPeriod; }

    /**
     * This method sets the CSL period.
     *
     * @param[in]  aPeriod  The CSL period in 10 symbols.
     *
     */
    void SetCslPeriod(uint16_t aPeriod);

    /**
     * This method returns CSL parent clock accuracy, in ± ppm.
     *
     * @retval CSL parent clock accuracy.
     *
     */
    uint8_t GetCslParentClockAccuracy(void) const { return mCslParentAccuracy; }

    /**
     * This method sets CSL parent clock accuracy, in ± ppm.
     *
     * @param[in] aCslParentAccuracy CSL parent clock accuracy, in ± ppm.
     *
     */
    void SetCslParentClockAccuracy(uint8_t aCslParentAccuracy) { mCslParentAccuracy = aCslParentAccuracy; }

    /**
     * This method sets CSL parent uncertainty, in ±10 us units.
     *
     * @retval CSL parent uncertainty, in ±10 us units.
     *
     */
    uint8_t GetCslParentUncertainty(void) const { return mCslParentUncert; }

    /**
     * This method returns CSL parent uncertainty, in ±10 us units.
     *
     * @param[in] aCslParentUncert  CSL parent uncertainty, in ±10 us units.
     *
     */
    void SetCslParentUncertainty(uint8_t aCslParentUncert) { mCslParentUncert = aCslParentUncert; }

#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

    /**
     * This method sets MAC keys and key index.
     *
     * @param[in] aKeyIdMode  MAC key ID mode.
     * @param[in] aKeyId      The key ID.
     * @param[in] aPrevKey    The previous MAC key.
     * @param[in] aCurrKey    The current MAC key.
     * @param[in] aNextKey    The next MAC key.
     *
     */
    void SetMacKey(uint8_t            aKeyIdMode,
                   uint8_t            aKeyId,
                   const KeyMaterial &aPrevKey,
                   const KeyMaterial &aCurrKey,
                   const KeyMaterial &aNextKey);

    /**
     * This method returns a reference to the current MAC key.
     *
     * @returns A reference to the current MAC key.
     *
     */
    const KeyMaterial &GetCurrentMacKey(void) const { return mCurrKey; }

    /**
     * This method returns a reference to the previous MAC key.
     *
     * @returns A reference to the previous MAC key.
     *
     */
    const KeyMaterial &GetPreviousMacKey(void) const { return mPrevKey; }

    /**
     * This method returns a reference to the next MAC key.
     *
     * @returns A reference to the next MAC key.
     *
     */
    const KeyMaterial &GetNextMacKey(void) const { return mNextKey; }

    /**
     * This method returns the current MAC frame counter value.
     *
     * @returns The current MAC frame counter value.
     *
     */
    uint32_t GetFrameCounter(void) const { return mFrameCounter; }

    /**
     * This method sets the current MAC Frame Counter value.
     *
     * @param[in] aFrameCounter  The MAC Frame Counter value.
     *
     */
    void SetFrameCounter(uint32_t aFrameCounter);

private:
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    static void HandleCslTimer(Timer &aTimer);
    void        HandleCslTimer(void);
    void        GetCslWindowEdges(uint32_t &ahead, uint32_t &after);
#endif

    static constexpr uint8_t  kMinBE             = 3;   // macMinBE (IEEE 802.15.4-2006).
    static constexpr uint8_t  kMaxBE             = 5;   // macMaxBE (IEEE 802.15.4-2006).
    static constexpr uint32_t kUnitBackoffPeriod = 20;  // Number of symbols (IEEE 802.15.4-2006).
    static constexpr uint32_t kAckTimeout        = 16;  // Timeout for waiting on an ACK (in msec).
    static constexpr uint32_t kCcaSampleInterval = 128; // CCA sample interval, 128 usec.

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    static constexpr uint32_t kEnergyScanRssiSampleInterval = 128; // RSSI sample interval for energy scan, 128 usec
#else
    static constexpr uint32_t kEnergyScanRssiSampleInterval = 1; // RSSI sample interval during energy scan, 1 msec
#endif

    enum State : uint8_t
    {
        kStateDisabled,    // Radio is disabled.
        kStateSleep,       // Radio is in sleep.
        kStateReceive,     // Radio in in receive.
        kStateCsmaBackoff, // CSMA backoff before transmission.
        kStateTransmit,    // Radio is transmitting.
        kStateEnergyScan,  // Energy scan.
#if !OPENTHREAD_MTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
        kStateCslTransmit, // CSL transmission.
#endif
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
        kStateCslSample, // CSL receive.
#endif
    };

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    // CSL receive window for the longest possible frame and
    // ack duration.
    static constexpr uint32_t kMinCslWindow = OPENTHREAD_CONFIG_CSL_MIN_RECEIVE_ON;

    // CSL receivers would wake up `kCslReceiveTimeAhead` earlier
    // than expected sample window. The value is in usec.
    static constexpr uint32_t kCslReceiveTimeAhead = OPENTHREAD_CONFIG_CSL_RECEIVE_TIME_AHEAD;

    enum CslState : uint8_t{
        kCslIdle,   // CSL receiver is not started.
        kCslSample, // Sampling CSL channel.
        kCslSleep,  // Radio in sleep.
    };
#endif

    /**
     * This method initializes the states of the sub-MAC layer.
     *
     */
    void Init(void);

    bool RadioSupportsCsmaBackoff(void) const
    {
        return ((mRadioCaps & (OT_RADIO_CAPS_CSMA_BACKOFF | OT_RADIO_CAPS_TRANSMIT_RETRIES)) != 0);
    }

    bool RadioSupportsTransmitSecurity(void) const { return ((mRadioCaps & OT_RADIO_CAPS_TRANSMIT_SEC) != 0); }
    bool RadioSupportsRetries(void) const { return ((mRadioCaps & OT_RADIO_CAPS_TRANSMIT_RETRIES) != 0); }
    bool RadioSupportsAckTimeout(void) const { return ((mRadioCaps & OT_RADIO_CAPS_ACK_TIMEOUT) != 0); }
    bool RadioSupportsEnergyScan(void) const { return ((mRadioCaps & OT_RADIO_CAPS_ENERGY_SCAN) != 0); }
    bool RadioSupportsTransmitTiming(void) const { return ((mRadioCaps & OT_RADIO_CAPS_TRANSMIT_TIMING) != 0); }
    bool RadioSupportsReceiveTiming(void) const { return ((mRadioCaps & OT_RADIO_CAPS_RECEIVE_TIMING) != 0); }

    bool ShouldHandleTransmitSecurity(void) const;
    bool ShouldHandleCsmaBackOff(void) const;
    bool ShouldHandleAckTimeout(void) const;
    bool ShouldHandleRetries(void) const;
    bool ShouldHandleEnergyScan(void) const;
    bool ShouldHandleTransmitTargetTime(void) const;

    void ProcessTransmitSecurity(void);
    void SignalFrameCounterUsed(uint32_t aFrameCounter);
    void StartCsmaBackoff(void);
    void BeginTransmit(void);
    void SampleRssi(void);

    void HandleReceiveDone(RxFrame *aFrame, Error aError);
    void HandleTransmitStarted(TxFrame &aFrame);
    void HandleTransmitDone(TxFrame &aFrame, RxFrame *aAckFrame, Error aError);
    void SignalFrameCounterUsedOnTxDone(const TxFrame &aFrame);
    void HandleEnergyScanDone(int8_t aMaxRssi);

    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);

    void               SetState(State aState);
    static const char *StateToString(State aState);
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    static const char *CslStateToString(CslState aCslState);
#endif

    otRadioCaps        mRadioCaps;
    State              mState;
    uint8_t            mCsmaBackoffs;
    uint8_t            mTransmitRetries;
    ShortAddress       mShortAddress;
    ExtAddress         mExtAddress;
    bool               mRxOnWhenBackoff;
    int8_t             mEnergyScanMaxRssi;
    TimeMilli          mEnergyScanEndTime;
    TxFrame &          mTransmitFrame;
    Callbacks          mCallbacks;
    otLinkPcapCallback mPcapCallback;
    void *             mPcapCallbackContext;
    KeyMaterial        mPrevKey;
    KeyMaterial        mCurrKey;
    KeyMaterial        mNextKey;
    uint32_t           mFrameCounter;
    uint8_t            mKeyId;
#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    TimerMicro mTimer;
#else
    TimerMilli                mTimer;
#endif

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    uint16_t   mCslPeriod;                 // The CSL sample period, in units of 10 symbols (160 microseconds).
    uint8_t    mCslChannel : 7;            // The CSL sample channel (only when `mIsCslChannelSpecified` is `true`).
    uint8_t    mIsCslChannelSpecified : 1; // Whether the CSL channel was explicitly set
    TimeMicro  mCslSampleTime;             // The CSL sample time of the current period.
    TimeMicro  mCslLastSync;               // The timestamp of the last successful CSL synchronization.
    uint8_t    mCslParentAccuracy;         // Drift of timer used for scheduling CSL tx by the parent, in ± ppm.
    uint8_t    mCslParentUncert;           // Uncertainty of the scheduling CSL of tx by the parent, in ±10 us units.
    CslState   mCslState;
    TimerMicro mCslTimer;
#endif
};

/**
 * @}
 *
 */

} // namespace Mac
} // namespace ot

#endif // SUB_MAC_HPP_
