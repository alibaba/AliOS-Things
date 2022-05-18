/*
 *    Copyright (c) 2019, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 *    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements Thread Radio Encapsulation Link (TREL).
 */

#include "trel_link.hpp"

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"

namespace ot {
namespace Trel {

Link::Link(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mState(kStateDisabled)
    , mRxChannel(0)
    , mPanId(Mac::kPanIdBroadcast)
    , mTxPacketNumber(0)
    , mTxTasklet(aInstance, HandleTxTasklet)
    , mTimer(aInstance, HandleTimer)
    , mInterface(aInstance)
{
    memset(&mTxFrame, 0, sizeof(mTxFrame));
    memset(&mRxFrame, 0, sizeof(mRxFrame));
    memset(mAckFrameBuffer, 0, sizeof(mAckFrameBuffer));

    mTxFrame.mPsdu = &mTxPacketBuffer[kMaxHeaderSize];
    mTxFrame.SetLength(0);

#if OPENTHREAD_CONFIG_MULTI_RADIO
    mTxFrame.SetRadioType(Mac::kRadioTypeTrel);
    mRxFrame.SetRadioType(Mac::kRadioTypeTrel);
#endif

    mTimer.Start(kAckWaitWindow);
}

void Link::AfterInit(void)
{
    mInterface.Init();
}

void Link::Enable(void)
{
    if (mState == kStateDisabled)
    {
        SetState(kStateSleep);
    }
}

void Link::Disable(void)
{
    if (mState != kStateDisabled)
    {
        SetState(kStateDisabled);
    }
}

void Link::Sleep(void)
{
    OT_ASSERT(mState != kStateDisabled);
    SetState(kStateSleep);
}

void Link::Receive(uint8_t aChannel)
{
    OT_ASSERT(mState != kStateDisabled);
    mRxChannel = aChannel;
    SetState(kStateReceive);
}

void Link::Send(void)
{
    OT_ASSERT(mState != kStateDisabled);

    SetState(kStateTransmit);
    mTxTasklet.Post();
}

void Link::HandleTxTasklet(Tasklet &aTasklet)
{
    aTasklet.Get<Link>().HandleTxTasklet();
}

void Link::HandleTxTasklet(void)
{
    BeginTransmit();
}

void Link::BeginTransmit(void)
{
    Mac::Address  destAddr;
    Mac::PanId    destPanId;
    Header::Type  type;
    Packet        txPacket;
    Neighbor *    neighbor = nullptr;
    Mac::RxFrame *ackFrame = nullptr;

    VerifyOrExit(mState == kStateTransmit);

    // After sending a frame on a given channel we should
    // continue to rx on same channel
    mRxChannel = mTxFrame.GetChannel();

    VerifyOrExit(!mTxFrame.IsEmpty(), InvokeSendDone(kErrorAbort));

    IgnoreError(mTxFrame.GetDstAddr(destAddr));

    if (destAddr.IsNone() || destAddr.IsBroadcast())
    {
        type = Header::kTypeBroadcast;
    }
    else
    {
        type     = Header::kTypeUnicast;
        neighbor = Get<NeighborTable>().FindNeighbor(destAddr, Neighbor::kInStateAnyExceptInvalid);

        if (destAddr.IsShort())
        {
            if (neighbor != nullptr)
            {
                destAddr.SetExtended(neighbor->GetExtAddress());
            }
            else
            {
                // Send as a broadcast since we don't know the dest
                // ext address to include in the packet header.
                type = Header::kTypeBroadcast;
            }
        }
    }

    if (mTxFrame.GetDstPanId(destPanId) != kErrorNone)
    {
        destPanId = Mac::kPanIdBroadcast;
    }

    txPacket.Init(type, mTxFrame.GetPsdu(), mTxFrame.GetLength());

    if (neighbor == nullptr)
    {
        txPacket.GetHeader().SetAckMode(Header::kNoAck);
        txPacket.GetHeader().SetPacketNumber(mTxPacketNumber++);
    }
    else
    {
        txPacket.GetHeader().SetAckMode(Header::kAckRequested);
        txPacket.GetHeader().SetPacketNumber(neighbor->mTrelTxPacketNumber++);
        neighbor->mTrelCurrentPendingAcks++;
    }

    txPacket.GetHeader().SetChannel(mTxFrame.GetChannel());
    txPacket.GetHeader().SetPanId(destPanId);
    txPacket.GetHeader().SetSource(Get<Mac::Mac>().GetExtAddress());

    if (type == Header::kTypeUnicast)
    {
        OT_ASSERT(destAddr.IsExtended());
        txPacket.GetHeader().SetDestination(destAddr.GetExtended());
    }

    otLogDebgMac("Trel: BeginTransmit() [%s] plen:%d", txPacket.GetHeader().ToString().AsCString(),
                 txPacket.GetPayloadLength());

    VerifyOrExit(mInterface.Send(txPacket) == kErrorNone, InvokeSendDone(kErrorAbort));

    if (mTxFrame.GetAckRequest())
    {
        uint16_t fcf = Mac::Frame::kFcfFrameAck;

        if (!Get<Mle::MleRouter>().IsRxOnWhenIdle())
        {
            fcf |= Mac::Frame::kFcfFramePending;
        }

        // Prepare the ack frame (FCF followed by sequence number)
        Encoding::LittleEndian::WriteUint16(fcf, mAckFrameBuffer);
        mAckFrameBuffer[sizeof(fcf)] = mTxFrame.GetSequence();

        mRxFrame.mPsdu    = mAckFrameBuffer;
        mRxFrame.mLength  = k154AckFrameSize;
        mRxFrame.mChannel = mTxFrame.GetChannel();
#if OPENTHREAD_CONFIG_MULTI_RADIO
        mRxFrame.mRadioType = Mac::kRadioTypeTrel;
#endif
        mRxFrame.mInfo.mRxInfo.mTimestamp             = 0;
        mRxFrame.mInfo.mRxInfo.mRssi                  = OT_RADIO_RSSI_INVALID;
        mRxFrame.mInfo.mRxInfo.mLqi                   = OT_RADIO_LQI_NONE;
        mRxFrame.mInfo.mRxInfo.mAckedWithFramePending = false;

        ackFrame = &mRxFrame;
    }

    InvokeSendDone(kErrorNone, ackFrame);

exit:
    return;
}

void Link::InvokeSendDone(Error aError, Mac::RxFrame *aAckFrame)
{
    SetState(kStateReceive);

    Get<Mac::Mac>().RecordFrameTransmitStatus(mTxFrame, aAckFrame, aError, /* aRetryCount */ 0, /* aWillRetx */ false);
    Get<Mac::Mac>().HandleTransmitDone(mTxFrame, aAckFrame, aError);
}

void Link::HandleTimer(Timer &aTimer)
{
    aTimer.Get<Link>().HandleTimer();
}

void Link::HandleTimer(void)
{
    mTimer.Start(kAckWaitWindow);

#if OPENTHREAD_FTD
    for (Child &child : Get<ChildTable>().Iterate(Child::kInStateAnyExceptInvalid))
    {
        HandleTimer(child);
    }

    for (Router &router : Get<RouterTable>().Iterate())
    {
        HandleTimer(router);
    }
#endif

    // Parent and ParentCandidate should also be updated as neighbors.
    // Parent is considered only when the device is detached or child.
    // When device becomes a router/leader the parent entry is copied
    // into the router table but the MLE parent may still stay in
    // valid state. Note that "Parent Candidate" may be in use on a
    // router/leader during a partition merge, so it is always treated
    // as a neighbor.

    switch (Get<Mle::MleRouter>().GetRole())
    {
    case Mle::kRoleDisabled:
        break;

    case Mle::kRoleDetached:
    case Mle::kRoleChild:
        HandleTimer(Get<Mle::MleRouter>().GetParent());

        OT_FALL_THROUGH;

    case Mle::kRoleRouter:
    case Mle::kRoleLeader:
        HandleTimer(Get<Mle::MleRouter>().GetParentCandidate());
        break;
    }
}

void Link::HandleTimer(Neighbor &aNeighbor)
{
    VerifyOrExit(!aNeighbor.IsStateInvalid());

    // Any remaining previous pending ack has timed out.

    while (aNeighbor.mTrelPreviousPendingAcks > 0)
    {
        aNeighbor.mTrelPreviousPendingAcks--;

        ReportDeferredAckStatus(aNeighbor, kErrorNoAck);
        VerifyOrExit(!aNeighbor.IsStateInvalid());
    }

    aNeighbor.mTrelPreviousPendingAcks = aNeighbor.mTrelCurrentPendingAcks;
    aNeighbor.mTrelCurrentPendingAcks  = 0;

exit:
    return;
}

void Link::ProcessReceivedPacket(Packet &aPacket)
{
    Header::Type type;

    VerifyOrExit(aPacket.IsHeaderValid());

    type = aPacket.GetHeader().GetType();

    if (type != Header::kTypeAck)
    {
        // No need to check state or channel for a TREL ack packet.
        // Note that TREL ack may be received much later than the tx
        // and device can be on a different rx channel.

        VerifyOrExit((mState == kStateReceive) || (mState == kStateTransmit));
        VerifyOrExit(aPacket.GetHeader().GetChannel() == mRxChannel);
    }

    if (mPanId != Mac::kPanIdBroadcast)
    {
        Mac::PanId rxPanId = aPacket.GetHeader().GetPanId();

        VerifyOrExit((rxPanId == mPanId) || (rxPanId == Mac::kPanIdBroadcast));
    }

    // Drop packets originating from same device.
    VerifyOrExit(aPacket.GetHeader().GetSource() != Get<Mac::Mac>().GetExtAddress());

    if (type != Header::kTypeBroadcast)
    {
        VerifyOrExit(aPacket.GetHeader().GetDestination() == Get<Mac::Mac>().GetExtAddress());

        if (type == Header::kTypeAck)
        {
            HandleAck(aPacket);
            ExitNow();
        }
    }

    otLogDebgMac("Trel: ReceivedPacket() [%s] plen:%d", aPacket.GetHeader().ToString().AsCString(),
                 aPacket.GetPayloadLength());

    if (aPacket.GetHeader().GetAckMode() == Header::kAckRequested)
    {
        SendAck(aPacket);
    }

    mRxFrame.mPsdu    = aPacket.GetPayload();
    mRxFrame.mLength  = aPacket.GetPayloadLength();
    mRxFrame.mChannel = aPacket.GetHeader().GetChannel();
#if OPENTHREAD_CONFIG_MULTI_RADIO
    mRxFrame.mRadioType = Mac::kRadioTypeTrel;
#endif
    mRxFrame.mInfo.mRxInfo.mTimestamp             = 0;
    mRxFrame.mInfo.mRxInfo.mRssi                  = kRxRssi;
    mRxFrame.mInfo.mRxInfo.mLqi                   = OT_RADIO_LQI_NONE;
    mRxFrame.mInfo.mRxInfo.mAckedWithFramePending = true;

    Get<Mac::Mac>().HandleReceivedFrame(&mRxFrame, kErrorNone);

exit:
    return;
}

void Link::HandleAck(Packet &aAckPacket)
{
    Error        ackError;
    Mac::Address srcAddress;
    Neighbor *   neighbor;
    uint32_t     ackNumber;

    otLogDebgMac("Trel: HandleAck() [%s]", aAckPacket.GetHeader().ToString().AsCString());

    srcAddress.SetExtended(aAckPacket.GetHeader().GetSource());
    neighbor = Get<NeighborTable>().FindNeighbor(srcAddress, Neighbor::kInStateAnyExceptInvalid);
    VerifyOrExit(neighbor != nullptr);

    ackNumber = aAckPacket.GetHeader().GetPacketNumber();

    // Verify that neighbor is waiting for acks and the received ack
    // number is within the range of expected ack numbers.

    VerifyOrExit(neighbor->IsRxAckNumberValid(ackNumber));

    do
    {
        // Check whether the received ack number matches the next
        // expected one. If it does not, it indicates that some of
        // packets missed their acks.

        ackError = (ackNumber == neighbor->GetExpectedTrelAckNumber()) ? kErrorNone : kErrorNoAck;

        neighbor->DecrementPendingTrelAckCount();

        ReportDeferredAckStatus(*neighbor, ackError);
        VerifyOrExit(!neighbor->IsStateInvalid());

    } while (ackError == kErrorNoAck);

exit:
    return;
}

void Link::SendAck(Packet &aRxPacket)
{
    Packet ackPacket;

    ackPacket.Init(mAckPacketBuffer, sizeof(mAckPacketBuffer));

    ackPacket.GetHeader().Init(Header::kTypeAck);
    ackPacket.GetHeader().SetAckMode(Header::kNoAck);
    ackPacket.GetHeader().SetChannel(aRxPacket.GetHeader().GetChannel());
    ackPacket.GetHeader().SetPanId(aRxPacket.GetHeader().GetPanId());
    ackPacket.GetHeader().SetPacketNumber(aRxPacket.GetHeader().GetPacketNumber());
    ackPacket.GetHeader().SetSource(Get<Mac::Mac>().GetExtAddress());
    ackPacket.GetHeader().SetDestination(aRxPacket.GetHeader().GetSource());

    otLogDebgMac("Trel: SendAck [%s]", ackPacket.GetHeader().ToString().AsCString());

    IgnoreError(mInterface.Send(ackPacket));
}

void Link::ReportDeferredAckStatus(Neighbor &aNeighbor, Error aError)
{
    otLogDebgMac("Trel: ReportDeferredAckStatus(): %s for %s", aNeighbor.GetExtAddress().ToString().AsCString(),
                 ErrorToString(aError));

    Get<MeshForwarder>().HandleDeferredAck(aNeighbor, aError);
}

void Link::SetState(State aState)
{
    if (mState != aState)
    {
        otLogDebgMac("Trel: State: %s -> %s", StateToString(mState), StateToString(aState));
        mState = aState;
    }
}

// LCOV_EXCL_START

const char *Link::StateToString(State aState)
{
    static const char *kStateStrings[] = {
        "Disabled", // kStateDisabled
        "Sleep",    // kStateSleep
        "Receive",  // kStateReceive
        "Transmit", // kStateTransmit
    };

    return (static_cast<uint8_t>(aState) < OT_ARRAY_LENGTH(kStateStrings)) ? kStateStrings[aState] : "Unknown";
}

// LCOV_EXCL_STOP

} // namespace Trel
} // namespace ot

#endif // #if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
