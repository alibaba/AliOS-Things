/*
 *  Copyright (c) 2019, The OpenThread Authors.
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
 *   This file includes definitions for MAC radio links.
 */

#ifndef MAC_LINKS_HPP_
#define MAC_LINKS_HPP_

#include "openthread-core-config.h"

#include "common/debug.hpp"
#include "common/locator.hpp"
#include "mac/mac_frame.hpp"
#include "mac/mac_types.hpp"
#include "mac/sub_mac.hpp"
#include "radio/trel_link.hpp"

namespace ot {
namespace Mac {

/**
 * @addtogroup core-mac
 *
 * @brief
 *   This module includes definitions for MAC radio links (multi radio).
 *
 * @{
 *
 */

/**
 * This class represents tx frames for different radio link types.
 *
 */
class TxFrames : InstanceLocator
{
    friend class Links;

public:
#if OPENTHREAD_CONFIG_MULTI_RADIO
    /**
     * This method gets the `TxFrame` for a given radio link type.
     *
     * This method also updates the selected radio types (from `GetSelectedRadioTypes()`) to include the @p aRadioType.
     *
     * @param[in] aRadioType   A radio link type.
     *
     * @returns A reference to the `TxFrame` for the given radio link type.
     *
     */
    TxFrame &GetTxFrame(RadioType aRadioType);

    /**
     * This method gets the `TxFrame` with the smallest MTU size among a given set of radio types.
     *
     * This method also updates the selected radio types (from `GetSelectedRadioTypes()`) to include the set
     * @p aRadioTypes.
     *
     * @param[in] aRadioTypes   A set of radio link types.
     *
     * @returns A reference to the `TxFrame` with the smallest MTU size among the set of @p aRadioTypes.
     *
     */
    TxFrame &GetTxFrame(RadioTypes aRadioTypes);

    /**
     * This method gets the `TxFrame` for sending a broadcast frame.
     *
     * This method also updates the selected radio type (from `GetSelectedRadioTypes()`) to include all radio types
     * (supported by device).
     *
     * The broadcast frame is the `TxFrame` with the smallest MTU size among all radio types.
     *
     * @returns A reference to a `TxFrame` for broadcast.
     *
     */
    TxFrame &GetBroadcastTxFrame(void);

    /**
     * This method gets the selected radio types.
     *
     * This set specifies the radio links the frame should be sent over (in parallel). The set starts a empty after
     * method `Clear()` is called. It gets updated through calls to methods `GetTxFrame(aType)`,
     * `GetTxFrame(aRadioTypes)`, or `GetBroadcastTxFrame()`.
     *
     * @returns The selected radio types.
     *
     */
    RadioTypes GetSelectedRadioTypes(void) const { return mSelectedRadioTypes; }

    /**
     * This method gets the required radio types.
     *
     * This set specifies the radio links for which we expect the frame tx to be successful to consider the overall tx
     * successful. If the set is empty, successful tx over any radio link is sufficient for overall tx to be considered
     * successful. The required radio type set is expected to be a subset of selected radio types.
     *
     * The set starts as empty after `Clear()` call. It can be updated through `SetRequiredRadioTypes()` method
     *
     * @returns The required radio types.
     *
     */
    RadioTypes GetRequiredRadioTypes(void) const { return mRequiredRadioTypes; }

    /**
     * This method sets the required types.
     *
     * Please see `GetRequiredRadioTypes()` for more details on how this set is used during tx.
     *
     * @param[in] aRadioTypes   A set of radio link types.
     *
     */
    void SetRequiredRadioTypes(RadioTypes aRadioTypes) { mRequiredRadioTypes = aRadioTypes; }

#else // #if OPENTHREAD_CONFIG_MULTI_RADIO

#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
    /**
     * This method gets the tx frame.
     *
     * @returns A reference to `TxFrame`.
     *
     */
    TxFrame &GetTxFrame(void) { return mTxFrame802154; }
#elif OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
    /**
     * This method gets the tx frame.
     *
     * @returns A reference to `TxFrame`.
     *
     */
    TxFrame &GetTxFrame(void) { return mTxFrameTrel; }
#endif
    /**
     * This method gets a tx frame for sending a broadcast frame.
     *
     * @returns A reference to a `TxFrame` for broadcast.
     *
     */
    TxFrame &GetBroadcastTxFrame(void) { return GetTxFrame(); }

#endif // #if OPENTHREAD_CONFIG_MULTI_RADIO

    /**
     * This method clears all supported radio tx frames (sets the PSDU length to zero and clears flags).
     *
     */
    void Clear(void)
    {
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
        mTxFrame802154.SetLength(0);
        mTxFrame802154.SetIsARetransmission(false);
        mTxFrame802154.SetIsSecurityProcessed(false);
        mTxFrame802154.SetCsmaCaEnabled(true); // Set to true by default, only set to `false` for CSL transmission
        mTxFrame802154.SetIsHeaderUpdated(false);
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
        mTxFrame802154.SetTxDelay(0);
        mTxFrame802154.SetTxDelayBaseTime(0);
#endif
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
        mTxFrameTrel.SetLength(0);
        mTxFrameTrel.SetIsARetransmission(false);
        mTxFrameTrel.SetIsSecurityProcessed(false);
        mTxFrameTrel.SetCsmaCaEnabled(true);
        mTxFrameTrel.SetIsHeaderUpdated(false);
#endif

#if OPENTHREAD_CONFIG_MULTI_RADIO
        mSelectedRadioTypes.Clear();
        mRequiredRadioTypes.Clear();
#endif
    }

    /**
     * This method sets the channel on all supported radio tx frames.
     *
     * @param[in] aChannel  A channel.
     *
     */
    void SetChannel(uint8_t aChannel)
    {
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
        mTxFrame802154.SetChannel(aChannel);
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
        mTxFrameTrel.SetChannel(aChannel);
#endif
    }

    /**
     * This method sets the Sequence Number value on all supported radio tx frames.
     *
     * @param[in]  aSequence  The Sequence Number value.
     *
     */
    void SetSequence(uint8_t aSequence)
    {
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
        mTxFrame802154.SetSequence(aSequence);
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
        mTxFrameTrel.SetSequence(aSequence);
#endif
    }

    /**
     * This method sets the maximum number of the CSMA-CA backoffs on all supported radio tx
     * frames.
     *
     * @param[in]  aMaxCsmaBackoffs  The maximum number of CSMA-CA backoffs.
     *
     */
    void SetMaxCsmaBackoffs(uint8_t aMaxCsmaBackoffs)
    {
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
        mTxFrame802154.SetMaxCsmaBackoffs(aMaxCsmaBackoffs);
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
        mTxFrameTrel.SetMaxCsmaBackoffs(aMaxCsmaBackoffs);
#endif
    }

    /**
     * This method sets the maximum number of retries allowed after a transmission failure on all supported radio tx
     * frames.
     *
     * @param[in]  aMaxFrameRetries  The maximum number of retries allowed after a transmission failure.
     *
     */
    void SetMaxFrameRetries(uint8_t aMaxFrameRetries)
    {
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
        mTxFrame802154.SetMaxFrameRetries(aMaxFrameRetries);
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
        mTxFrameTrel.SetMaxFrameRetries(aMaxFrameRetries);
#endif
    }

private:
    explicit TxFrames(Instance &aInstance);

#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
    TxFrame &mTxFrame802154;
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
    TxFrame &mTxFrameTrel;
#endif

#if OPENTHREAD_CONFIG_MULTI_RADIO
    RadioTypes mSelectedRadioTypes;
    RadioTypes mRequiredRadioTypes;
#endif
};

/**
 * This class represents MAC radio links (multi radio).
 *
 */
class Links : public InstanceLocator
{
    friend class ot::Instance;

public:
    static const int8_t kInvalidRssiValue = SubMac::kInvalidRssiValue; ///< Invalid RSSI value.

    /**
     * This constructor initializes the `Links` object.
     *
     * @param[in]  aInstance  A reference to the OpenThread instance.
     *
     */
    explicit Links(Instance &aInstance);

    /**
     * This method sets the PAN ID.
     *
     * @param[in] aPanId  The PAN ID.
     *
     */
    void SetPanId(PanId aPanId)
    {
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
        mSubMac.SetPanId(aPanId);
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
        mTrel.SetPanId(aPanId);
#endif
    }

    /**
     * This method gets the MAC Short Address.
     *
     * @returns The MAC Short Address.
     *
     */
    ShortAddress GetShortAddress(void) const
    {
        return
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
            mSubMac.GetShortAddress();
#else
            mShortAddress;
#endif
    }

    /**
     * This method sets the MAC Short Address.
     *
     * @param[in] aShortAddress   A MAC Short Address.
     *
     */
    void SetShortAddress(ShortAddress aShortAddress)
    {
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
        mSubMac.SetShortAddress(aShortAddress);
#else
        mShortAddress = aShortAddress;
#endif
    }

    /**
     * This method gets the MAC Extended Address.
     *
     * @returns The MAC Extended Address.
     *
     */
    const ExtAddress &GetExtAddress(void) const
    {
        return
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
            mSubMac.GetExtAddress();
#else
            mExtAddress;
#endif
    }

    /**
     * This method sets the MAC Extended Address.
     *
     * @param[in] aExtAddress  A MAC Extended Address.
     *
     */
    void SetExtAddress(const ExtAddress &aExtAddress)
    {
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
        mSubMac.SetExtAddress(aExtAddress);
#else
        mExtAddress = aExtAddress;
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
        mTrel.HandleExtAddressChange();
#endif
    }

    /**
     * This method registers a callback to provide received packet capture for IEEE 802.15.4 frames.
     *
     * @param[in]  aPcapCallback     A pointer to a function that is called when receiving an IEEE 802.15.4 link frame
     *                               or nullptr to disable the callback.
     * @param[in]  aCallbackContext  A pointer to application-specific context.
     *
     */
    void SetPcapCallback(otLinkPcapCallback aPcapCallback, void *aCallbackContext)
    {
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
        mSubMac.SetPcapCallback(aPcapCallback, aCallbackContext);
#endif
        OT_UNUSED_VARIABLE(aPcapCallback);
        OT_UNUSED_VARIABLE(aCallbackContext);
    }

    /**
     * This method indicates whether radio should stay in Receive or Sleep during CSMA backoff.
     *
     * @param[in]  aRxOnWhenBackoff  TRUE to keep radio in Receive, FALSE to put to Sleep during CSMA backoff.
     *
     */
    void SetRxOnWhenBackoff(bool aRxOnWhenBackoff)
    {
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
        mSubMac.SetRxOnWhenBackoff(aRxOnWhenBackoff);
#endif
        OT_UNUSED_VARIABLE(aRxOnWhenBackoff);
    }

    /**
     * This method enables all radio links.
     *
     */
    void Enable(void)
    {
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
        IgnoreError(mSubMac.Enable());
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
        mTrel.Enable();
#endif
    }

    /**
     * This method disables all radio links.
     *
     */
    void Disable(void)
    {
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
        IgnoreError(mSubMac.Disable());
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
        mTrel.Disable();
#endif
    }

    /**
     * This method transitions all radio links to Sleep.
     *
     */
    void Sleep(void)
    {
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
        IgnoreError(mSubMac.Sleep());
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
        mTrel.Sleep();
#endif
    }

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    /**
     * This method transitions all radios link to CSL sample state.
     *
     * CSL sample state is only applicable and used for 15.4 radio link. Other link are transitioned to sleep state.
     *
     * @param[in]  aPanChannel  The current phy channel used by the device. This param will only take effect when CSL
     *                          channel hasn't been explicitly specified.
     */
    void CslSample(uint8_t aPanChannel)
    {
        OT_UNUSED_VARIABLE(aPanChannel);
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
        IgnoreError(mSubMac.CslSample(aPanChannel));
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
        mTrel.Sleep();
#endif
    }
#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

    /**
     * This method transitions all radio links to Receive.
     *
     * @param[in]  aChannel   The channel to use for receiving.
     *
     */
    void Receive(uint8_t aChannel)
    {
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
        IgnoreError(mSubMac.Receive(aChannel));
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
        mTrel.Receive(aChannel);
#endif
    }

    /**
     * This method gets the radio transmit frames.
     *
     * @returns The transmit frames.
     *
     */
    TxFrames &GetTxFrames(void) { return mTxFrames; }

#if !OPENTHREAD_CONFIG_MULTI_RADIO

    /**
     * This method sends a prepared frame.
     *
     * The prepared frame is from `GetTxFrames()`. This method is available only in single radio link mode.
     *
     */
    void Send(void)
    {
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
        {
            Error error = mSubMac.Send();
            OT_ASSERT(error == kErrorNone);
            OT_UNUSED_VARIABLE(error);
        }
#endif
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
        mTrel.Send();
#endif
    }

#else // #if !OPENTHREAD_CONFIG_MULTI_RADIO

    /**
     * This method sends prepared frames over a given set of radio links.
     *
     * The prepared frame must be from `GetTxFrames()`. This method is available only in multi radio link mode.
     *
     * @param[in] aFrame       A reference to a prepared frame.
     * @param[in] aRadioTypes  A set of radio types to send on.
     *
     */
    void Send(TxFrame &aFrame, RadioTypes aRadioTypes);

#endif // !OPENTHREAD_CONFIG_MULTI_RADIO

    /**
     * This method gets the number of transmit retries for the last transmitted frame.
     *
     * @returns Number of transmit retries.
     *
     */
    uint8_t GetTransmitRetries(void) const
    {
        return
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
            mSubMac.GetTransmitRetries();
#else
            0;
#endif
    }

    /**
     * This method gets the most recent RSSI measurement from radio link.
     *
     * @returns The RSSI in dBm when it is valid. `kInvalidRssiValue` when RSSI is invalid.
     *
     */
    int8_t GetRssi(void) const
    {
        return
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
            mSubMac.GetRssi();
#else
            kInvalidRssiValue;
#endif
    }

    /**
     * This method begins energy scan.
     *
     * @param[in] aScanChannel   The channel to perform the energy scan on.
     * @param[in] aScanDuration  The duration, in milliseconds, for the channel to be scanned.
     *
     * @retval kErrorNone            Successfully started scanning the channel.
     * @retval kErrorInvalidState    The radio was disabled or transmitting.
     * @retval kErrorNotImplemented  Energy scan is not supported by radio link.
     *
     */
    Error EnergyScan(uint8_t aScanChannel, uint16_t aScanDuration)
    {
        OT_UNUSED_VARIABLE(aScanChannel);
        OT_UNUSED_VARIABLE(aScanDuration);

        return
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
            mSubMac.EnergyScan(aScanChannel, aScanDuration);
#else
            kErrorNotImplemented;
#endif
    }

    /**
     * This method returns the noise floor value (currently use the radio receive sensitivity value).
     *
     * @returns The noise floor value in dBm.
     *
     */
    int8_t GetNoiseFloor(void)
    {
        return
#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
            mSubMac.GetNoiseFloor();
#else
            kDefaultNoiseFloor;
#endif
    }

    /**
     * This methods gets a reference to the `SubMac` instance.
     *
     * @returns A reference to the `SubMac` instance.
     *
     */
    SubMac &GetSubMac(void) { return mSubMac; }

    /**
     * This methods gets a reference to the `SubMac` instance.
     *
     * @returns A reference to the `SubMac` instance.
     *
     */
    const SubMac &GetSubMac(void) const { return mSubMac; }

    /**
     * This method returns a reference to the current MAC key (for Key Mode 1) for a given Frame.
     *
     * @param[in] aFrame    The frame for which to get the MAC key.
     *
     * @returns A reference to the current MAC key.
     *
     */
    const KeyMaterial *GetCurrentMacKey(const Frame &aFrame) const;

    /**
     * This method returns a reference to the temporary MAC key (for Key Mode 1) for a given Frame based on a given
     * Key Sequence.
     *
     * @param[in] aFrame        The frame for which to get the MAC key.
     * @param[in] aKeySequence  The Key Sequence number (MUST be one off (+1 or -1) from current key sequence number).
     *
     * @returns A reference to the temporary MAC key.
     *
     */
    const KeyMaterial *GetTemporaryMacKey(const Frame &aFrame, uint32_t aKeySequence) const;

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
    /**
     * This method sets the current MAC frame counter value from the value from a `TxFrame`.
     *
     * @param[in] TxFrame  The `TxFrame` from which to get the counter value.
     *
     * @retval kErrorNone            If successful.
     * @retval kErrorInvalidState    If the raw link-layer isn't enabled.
     *
     */
    void SetMacFrameCounter(TxFrame &aFrame);
#endif

private:
    static constexpr int8_t kDefaultNoiseFloor = -100;

    SubMac mSubMac;
#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
    Trel::Link mTrel;
#endif

    // `TxFrames` member definition should be after `mSubMac`, `mTrel`
    // definitions to allow it to use their methods from its
    // constructor.
    TxFrames mTxFrames;

#if !OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE
    ShortAddress mShortAddress;
    ExtAddress   mExtAddress;
#endif
};

/**
 * @}
 *
 */

} // namespace Mac
} // namespace ot

#endif // MAC_LINKS_HPP_
