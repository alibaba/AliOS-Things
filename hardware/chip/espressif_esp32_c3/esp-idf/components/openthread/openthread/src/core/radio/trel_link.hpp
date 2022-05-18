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
 *   This file includes definitions for Thread Radio Encapsulation Link (TREL).
 */

#ifndef TREL_LINK_HPP_
#define TREL_LINK_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE

#include "common/encoding.hpp"
#include "common/locator.hpp"
#include "common/tasklet.hpp"
#include "common/timer.hpp"
#include "mac/mac_frame.hpp"
#include "mac/mac_types.hpp"
#include "radio/trel_interface.hpp"
#include "radio/trel_packet.hpp"

namespace ot {

class Neighbor;

namespace Trel {

/**
 * @addtogroup core-trel
 *
 * @brief
 *   This module includes definitions for Thread Radio Encapsulation Link (TREL)
 *
 * @{
 *
 */

/**
 * This class represents a Thread Radio Encapsulation Link (TREL).
 *
 */
class Link : public InstanceLocator
{
    friend class ot::Instance;
    friend class Interface;

public:
    static constexpr uint16_t kMtuSize = 1280 - 48 - sizeof(Header); ///< MTU size for TREL frame.
    static constexpr uint8_t  kFcsSize = 0;                          ///< FCS size for TREL frame.

    /**
     * This constructor initializes the `Link` object.
     *
     * @param[in]  aInstance  A reference to the OpenThread instance.
     *
     */
    explicit Link(Instance &aInstance);

    /**
     * This method sets the PAN Identifier.
     *
     * @param[in] aPanId   A PAN Identifier.
     *
     */
    void SetPanId(Mac::PanId aPanId) { mPanId = aPanId; }

    /**
     * This method notifies TREL radio link that device's extended MAC address has changed for it to update any
     * internal address/state.
     *
     */
    void HandleExtAddressChange(void) { mInterface.HandleExtAddressChange(); }

    /**
     * This method enables the TREL radio link.
     *
     */
    void Enable(void);

    /**
     * This method disables the TREL radio link.
     *
     */
    void Disable(void);

    /**
     * This method requests TREL radio link to transition to Sleep mode
     *
     */
    void Sleep(void);

    /**
     * This method requests TREL radio link to transition to Receive mode on a given channel.
     *
     * `Mac::HandleReceivedFrame()` is used to notify MAC layer upon receiving a frame.
     *
     * @param[in] aChannel   The channel to receive on.
     *
     */
    void Receive(uint8_t aChannel);

    /**
     * This method gets the radio transmit frame for TREL radio link.
     *
     * @returns The transmit frame.
     *
     */
    Mac::TxFrame &GetTransmitFrame(void) { return mTxFrame; }

    /**
     * This method requests a frame to be sent over TREL radio link.
     *
     * The frame should be already placed in `GetTransmitFrame()` frame.
     *
     * `Mac::RecordFrameTransmitStatus()` and `Mac::HandleTransmitDone()` are used to notify the success or error status
     * of frame transmission upon completion of send.
     *
     */
    void Send(void);

private:
    static constexpr uint16_t kMaxHeaderSize   = sizeof(Header);
    static constexpr uint16_t k154AckFrameSize = 3 + kFcsSize;
    static constexpr int8_t   kRxRssi          = -20; // The RSSI value used for received frames on TREL radio link.
    static constexpr uint32_t kAckWaitWindow   = 750; // (in msec)

    enum State : uint8_t
    {
        kStateDisabled,
        kStateSleep,
        kStateReceive,
        kStateTransmit,
    };

    void AfterInit(void);
    void SetState(State aState);
    void BeginTransmit(void);
    void InvokeSendDone(Error aError) { InvokeSendDone(aError, nullptr); }
    void InvokeSendDone(Error aError, Mac::RxFrame *aAckFrame);
    void ProcessReceivedPacket(Packet &aPacket);
    void HandleAck(Packet &aAckPacket);
    void SendAck(Packet &aRxPacket);
    void ReportDeferredAckStatus(Neighbor &aNeighbor, Error aError);
    void HandleTimer(Neighbor &aNeighbor);

    static void HandleTxTasklet(Tasklet &aTasklet);
    void        HandleTxTasklet(void);

    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);

    static const char *StateToString(State aState);

    State        mState;
    uint8_t      mRxChannel;
    Mac::PanId   mPanId;
    uint32_t     mTxPacketNumber;
    Tasklet      mTxTasklet;
    TimerMilli   mTimer;
    Interface    mInterface;
    Mac::RxFrame mRxFrame;
    Mac::TxFrame mTxFrame;
    uint8_t      mTxPacketBuffer[kMaxHeaderSize + kMtuSize];
    uint8_t      mAckPacketBuffer[kMaxHeaderSize];
    uint8_t      mAckFrameBuffer[k154AckFrameSize];
};

/**
 * This class defines all the neighbor info required for TREL link.
 *
 * `Neighbor` class publicly inherits from this class.
 *
 */
class NeighborInfo
{
    friend class Link;

private:
    uint32_t GetPendingTrelAckCount(void) const { return (mTrelPreviousPendingAcks + mTrelCurrentPendingAcks); }

    void DecrementPendingTrelAckCount(void)
    {
        if (mTrelPreviousPendingAcks != 0)
        {
            mTrelPreviousPendingAcks--;
        }
        else if (mTrelCurrentPendingAcks != 0)
        {
            mTrelCurrentPendingAcks--;
        }
    }

    uint32_t GetExpectedTrelAckNumber(void) const { return mTrelTxPacketNumber - GetPendingTrelAckCount(); }

    bool IsRxAckNumberValid(uint32_t aAckNumber) const
    {
        // Note that calculating the difference between `aAckNumber`
        // and `GetExpectedTrelAckNumber` will correctly handle the
        // roll-over of packet number value.

        return (GetPendingTrelAckCount() != 0) && (aAckNumber - GetExpectedTrelAckNumber() < GetPendingTrelAckCount());
    }

    uint32_t mTrelTxPacketNumber;      // Next packet number to use for tx
    uint16_t mTrelCurrentPendingAcks;  // Number of pending acks for current interval.
    uint16_t mTrelPreviousPendingAcks; // Number of pending acks for previous interval.
};

/**
 * @}
 *
 */

} // namespace Trel
} // namespace ot

#endif // #if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE

#endif // TREL_LINK_HPP_
