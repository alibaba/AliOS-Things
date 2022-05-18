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
 *   This file includes definitions for radio selector (for multi radio links).
 */

#ifndef RADIO_SELECTOR_HPP_
#define RADIO_SELECTOR_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_MULTI_RADIO

#include <openthread/multi_radio.h>

#include "common/locator.hpp"
#include "common/message.hpp"
#include "mac/mac_frame.hpp"
#include "mac/mac_links.hpp"
#include "mac/mac_types.hpp"

namespace ot {

/**
 * @addtogroup core-radio-selector
 *
 * @brief
 *   This module includes definition for radio selector (for multi radio links).
 *
 * @{
 *
 */

class Neighbor;

class RadioSelector : InstanceLocator
{
public:
    /**
     * This class defines all the neighbor info required for multi radio link and radio selection.
     *
     * `Neighbor` class publicly inherits from this class.
     *
     */
    class NeighborInfo
    {
        friend class RadioSelector;

    public:
        /**
         * This type represents multi radio information associated with a neighbor.
         *
         */
        typedef otMultiRadioNeighborInfo MultiRadioInfo;

        /**
         * This method returns the supported radio types by the neighbor.
         *
         * @returns The supported radio types set.
         *
         */
        Mac::RadioTypes GetSupportedRadioTypes(void) const { return mSupportedRadioTypes; }

        /**
         * This method retrieves the multi radio information `otMultiRadioNeighborInfo` associated with the neighbor.
         *
         * @param[out] aInfo  A reference to `MultiRadioInfo` to populate with neighbor info.
         *
         */
        void PopulateMultiRadioInfo(MultiRadioInfo &aInfo);

    private:
        void AddSupportedRadioType(Mac::RadioType aType) { mSupportedRadioTypes.Add(aType); }
        void RemoveSupportedRadioType(Mac::RadioType aType) { mSupportedRadioTypes.Remove(aType); }
        void ClearSupportedRadioType(void) { mSupportedRadioTypes.Clear(); }

        uint8_t GetRadioPreference(Mac::RadioType aType) const { return mRadioPreference[aType]; }
        void    SetRadioPreference(Mac::RadioType aType, uint8_t aValue) { mRadioPreference[aType] = aValue; }

        Mac::RadioTypes mSupportedRadioTypes;
        uint8_t         mRadioPreference[Mac::kNumRadioTypes];
    };

    /**
     * This constructor initializes the RadioSelector object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit RadioSelector(Instance &aInstance);

    /**
     * This method updates the neighbor info (for multi radio support) on a received frame event.
     *
     * This method notifies `RadioSelector` of a received secure frame/message on a radio link type for neighbor. If
     * the frame/message happens to be received earlier on another radio link, the `aIsDuplicate` is set to `true`.
     * A duplicate frame/message should have passed the security check (i.e., tag/MIC should be valid).
     *
     * @param[in] aNeighbor     The neighbor for which a frame/message was received.
     * @param[in] aRadioType    The radio link type on which the frame/message was received.
     * @param[in] aIsDuplicate  Indicates whether the received frame/message is a duplicate or not.
     *
     */
    void UpdateOnReceive(Neighbor &aNeighbor, Mac::RadioType aRadioType, bool aIsDuplicate);

    /**
     * This method updates the neighbor info (for multi radio support) on a send done event.
     *
     * This method notifies `RadioSelector` the status of frame transmission on a radio link type. The radio link
     * type is provided by the `aFrame` itself.
     *
     * @param[in] aFrame     A transmitted frame.
     * @param[in] aTxError   The transmission error.
     *
     */
    void UpdateOnSendDone(Mac::TxFrame &aFrame, Error aTxError);

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
    /**
     * This method updates the neighbor info (for multi radio support) on a deferred ack event.
     *
     * The deferred ack model is used by TREL radio link.
     *
     * @param[in]  aNeighbor              The neighbor from which ack was expected
     * @param[in]  aError                 The deferred ack status (`kErrorNone` indicates ack was received).
     * @param[out] aAllowNeighborRemove   Boolean variable to output whether the neighbor is allowed to be removed.
     *
     */
    void UpdateOnDeferredAck(Neighbor &aNeighbor, Error aTxError, bool &aAllowNeighborRemove);
#endif

    /**
     * This method selects the radio link type for sending a data poll frame to a given parent neighbor.
     *
     * @param[in] aParent  The parent to which the data poll frame will be sent.
     *
     * @returns The radio type on which the data poll frame should be sent.
     *
     */
    Mac::RadioType SelectPollFrameRadio(const Neighbor &aParent);

    /**
     * This method selects the radio link for sending a given message to a specified MAC destination.
     *
     * The `aMessage` will be updated to store the selected radio type (please see `Message::GetRadioType()`).
     * The `aTxFrames` will also be updated to indicate which radio links are to be used.
     *
     * @param[inout] aMessage   The message to send.
     * @param[in]    aMacDest   The MAC destination address.
     * @param[inout] aTxFrames  The set of TxFrames for all radio links.
     *
     * @returns  A reference to `mTxFrame` to use when preparing the frame for tx.
     *
     */
    Mac::TxFrame &SelectRadio(Message &aMessage, const Mac::Address &aMacDest, Mac::TxFrames &aTxFrames);

private:
    static constexpr int16_t kPreferenceChangeOnTxError            = -35;  // Change on a tx error on a radio link.
    static constexpr int16_t kPreferenceChangeOnTxSuccess          = 25;   // Change on tx success on a radio link.
    static constexpr int16_t kPreferenceChangeOnDeferredAckSuccess = 25;   // Change on deferred ack success.
    static constexpr int16_t kPreferenceChangeOnDeferredAckTimeout = -100; // Change on deferred ack timeout.
    static constexpr int16_t kPreferenceChangeOnRx                 = 15;   // Change on new (secure) rx.
    static constexpr int16_t kPreferenceChangeOnRxDuplicate        = 15;   // Change on new (secure) duplicate rx.

    static constexpr uint8_t kMinPreference  = 0;   // Minimum preference value.
    static constexpr uint8_t kMaxPreference  = 255; // Maximum preference value.
    static constexpr uint8_t kInitPreference = 200; // Initial preference value
    static constexpr uint8_t kHighPreference = 220; // High preference.

    static constexpr uint8_t kTrelProbeProbability = 25; // Probability percentage to probe on TREL link

    static constexpr uint16_t kRadioPreferenceStringSize = 75;

    otLogLevel     UpdatePreference(Neighbor &aNeighbor, Mac::RadioType aRadioType, int16_t aDifference);
    Mac::RadioType Select(Mac::RadioTypes aRadioOptions, const Neighbor &aNeighbor);
    void           Log(otLogLevel aLogLevel, const char *aActionText, Mac::RadioType aType, const Neighbor &aNeighbor);

    static const Mac::RadioType sRadioSelectionOrder[Mac::kNumRadioTypes];
};

/**
 * @}
 *
 */

} // namespace ot

#endif // #if OPENTHREAD_CONFIG_MULTI_RADIO

#endif // RADIO_SELECTOR_HPP_
