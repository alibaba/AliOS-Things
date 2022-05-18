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

#ifndef CSL_TX_SCHEDULER_HPP_
#define CSL_TX_SCHEDULER_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE

#include "common/locator.hpp"
#include "common/message.hpp"
#include "common/non_copyable.hpp"
#include "common/time.hpp"
#include "mac/mac.hpp"
#include "mac/mac_frame.hpp"
#include "thread/indirect_sender_frame_context.hpp"

namespace ot {

/**
 * @addtogroup core-mesh-forwarding
 *
 * @brief
 *   This module includes definitions for CSL transmission scheduling.
 *
 * @{
 */

class Child;

/**
 * This class implements CSL tx scheduling functionality.
 *
 */
class CslTxScheduler : public InstanceLocator, private NonCopyable
{
    friend class Mac::Mac;
    friend class IndirectSender;

public:
    static constexpr uint8_t kMaxCslTriggeredTxAttempts = OPENTHREAD_CONFIG_MAC_MAX_TX_ATTEMPTS_INDIRECT_POLLS;

    /**
     * This class defines all the child info required for scheduling CSL transmissions.
     *
     * `Child` class publicly inherits from this class.
     *
     */
    class ChildInfo
    {
    public:
        uint8_t GetCslTxAttempts(void) const { return mCslTxAttempts; }
        void    IncrementCslTxAttempts(void) { mCslTxAttempts++; }
        void    ResetCslTxAttempts(void) { mCslTxAttempts = 0; }

        bool IsCslSynchronized(void) const { return mCslSynchronized && mCslPeriod > 0; }
        void SetCslSynchronized(bool aCslSynchronized) { mCslSynchronized = aCslSynchronized; }

        uint8_t GetCslChannel(void) const { return mCslChannel; }
        void    SetCslChannel(uint8_t aChannel) { mCslChannel = aChannel; }

        uint32_t GetCslTimeout(void) const { return mCslTimeout; }
        void     SetCslTimeout(uint32_t aTimeout) { mCslTimeout = aTimeout; }

        uint16_t GetCslPeriod(void) const { return mCslPeriod; }
        void     SetCslPeriod(uint16_t aPeriod) { mCslPeriod = aPeriod; }

        uint16_t GetCslPhase(void) const { return mCslPhase; }
        void     SetCslPhase(uint16_t aPhase) { mCslPhase = aPhase; }

        TimeMilli GetCslLastHeard(void) const { return mCslLastHeard; }
        void      SetCslLastHeard(TimeMilli aCslLastHeard) { mCslLastHeard = aCslLastHeard; }

        uint64_t GetLastRxTimestamp(void) const { return mLastRxTimstamp; }
        void     SetLastRxTimestamp(uint64_t aLastRxTimestamp) { mLastRxTimstamp = aLastRxTimestamp; }

    private:
        uint8_t   mCslTxAttempts : 7;   ///< Number of CSL triggered tx attempts.
        bool      mCslSynchronized : 1; ///< Indicates whether or not the child is CSL synchronized.
        uint8_t   mCslChannel;          ///< The channel the device will listen on.
        uint32_t  mCslTimeout;          ///< The sync timeout, in seconds.
        uint16_t  mCslPeriod;           ///< CSL sampled listening period in units of 10 symbols (160 microseconds).
        uint16_t  mCslPhase;            ///< The time when the next CSL sample will start.
        TimeMilli mCslLastHeard;        ///< Time when last frame containing CSL IE was heard.
        uint64_t  mLastRxTimstamp;      ///< Time when last frame containing CSL IE was received, in microseconds.

        static_assert(kMaxCslTriggeredTxAttempts < (1 << 7), "mCslTxAttempts cannot fit max!");
    };

    /**
     * This class defines the callbacks used by the `CslTxScheduler`.
     *
     */
    class Callbacks : public InstanceLocator
    {
        friend class CslTxScheduler;

    private:
        typedef IndirectSenderBase::FrameContext FrameContext;

        /**
         * This constructor initializes the callbacks object.
         *
         * @param[in]  aInstance   A reference to the OpenThread instance.
         *
         */
        explicit Callbacks(Instance &aInstance);

        /**
         * This callback method requests a frame to be prepared for CSL transmission to a given SSED.
         *
         * @param[out] aFrame    A reference to a MAC frame where the new frame would be placed.
         * @param[out] aContext  A reference to a `FrameContext` where the context for the new frame would be placed.
         * @param[in]  aChild    The child for which to prepare the frame.
         *
         * @retval kErrorNone   Frame was prepared successfully.
         * @retval kErrorAbort  CSL transmission should be aborted (no frame for the child).
         *
         */
        Error PrepareFrameForChild(Mac::TxFrame &aFrame, FrameContext &aContext, Child &aChild);

        /**
         * This callback method notifies the end of CSL frame transmission to a child.
         *
         * @param[in]  aFrame     The transmitted frame.
         * @param[in]  aContext   The context associated with the frame when it was prepared.
         * @param[in]  aError     kErrorNone when the frame was transmitted successfully,
         *                        kErrorNoAck when the frame was transmitted but no ACK was received,
         *                        kErrorChannelAccessFailure tx failed due to activity on the channel,
         *                        kErrorAbort when transmission was aborted for other reasons.
         * @param[in]  aChild     The child to which the frame was transmitted.
         *
         */
        void HandleSentFrameToChild(const Mac::TxFrame &aFrame,
                                    const FrameContext &aContext,
                                    Error               aError,
                                    Child &             aChild);
    };
    /**
     * This constructor initializes the CSL tx scheduler object.
     *
     * @param[in]  aInstance   A reference to the OpenThread instance.
     *
     */
    explicit CslTxScheduler(Instance &aInstance);

    /**
     * This method updates the next CSL transmission (finds the nearest child).
     *
     * It would then request the `Mac` to do the CSL tx. If the last CSL tx has been fired at `Mac` but hasn't been
     * done yet, and it's aborted, this method would set `mCslTxChild` to `nullptr` to notify the `HandleTransmitDone`
     * that the operation has been aborted.
     *
     */
    void Update(void);

    /**
     * This method clears all the states inside `CslTxScheduler` and the related states in each child.
     *
     */
    void Clear(void);

private:
    void InitFrameRequestAhead(void);
    void RescheduleCslTx(void);

    uint32_t GetNextCslTransmissionDelay(const Child &aChild, uint32_t &aDelayFromLastRx) const;

    // Callbacks from `Mac`
    Mac::TxFrame *HandleFrameRequest(Mac::TxFrames &aTxFrames);
    void          HandleSentFrame(const Mac::TxFrame &aFrame, Error aError);

    void HandleSentFrame(const Mac::TxFrame &aFrame, Error aError, Child &aChild);

    uint32_t                mCslFrameRequestAheadUs;
    Child *                 mCslTxChild;
    Message *               mCslTxMessage;
    Callbacks::FrameContext mFrameContext;
    Callbacks               mCallbacks;
};

/**
 * @}
 *
 */

} // namespace ot

#endif // OPENTHREAD_FTD && OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE

#endif // CSL_TX_SCHEDULER_HPP_
