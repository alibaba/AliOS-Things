/*
 *  Copyright (c) 2018, The OpenThread Authors.
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
 *   This file implements FTD-specific mesh forwarding of IPv6/6LoWPAN messages.
 */

#include "mesh_forwarder.hpp"

#if OPENTHREAD_FTD

#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "meshcop/meshcop.hpp"
#include "net/ip6.hpp"
#include "net/tcp6.hpp"
#include "net/udp6.hpp"

namespace ot {

Error MeshForwarder::SendMessage(Message &aMessage)
{
    Mle::MleRouter &mle   = Get<Mle::MleRouter>();
    Error           error = kErrorNone;
    Neighbor *      neighbor;

    aMessage.SetOffset(0);
    aMessage.SetDatagramTag(0);
    mSendQueue.Enqueue(aMessage);

    switch (aMessage.GetType())
    {
    case Message::kTypeIp6:
    {
        Ip6::Header ip6Header;

        IgnoreError(aMessage.Read(0, ip6Header));

        if (ip6Header.GetDestination().IsMulticast())
        {
            // For traffic destined to multicast address larger than realm local, generally it uses IP-in-IP
            // encapsulation (RFC2473), with outer destination as ALL_MPL_FORWARDERS. So here if the destination
            // is multicast address larger than realm local, it should be for indirection transmission for the
            // device's sleepy child, thus there should be no direct transmission.
            if (!ip6Header.GetDestination().IsMulticastLargerThanRealmLocal())
            {
                // schedule direct transmission
                aMessage.SetDirectTransmission();
            }

            if (aMessage.GetSubType() != Message::kSubTypeMplRetransmission)
            {
                if (ip6Header.GetDestination() == mle.GetLinkLocalAllThreadNodesAddress() ||
                    ip6Header.GetDestination() == mle.GetRealmLocalAllThreadNodesAddress())
                {
                    // destined for all sleepy children
                    for (Child &child : Get<ChildTable>().Iterate(Child::kInStateValidOrRestoring))
                    {
                        if (!child.IsRxOnWhenIdle())
                        {
                            mIndirectSender.AddMessageForSleepyChild(aMessage, child);
                        }
                    }
                }
                else
                {
                    // destined for some sleepy children which subscribed the multicast address.
                    for (Child &child : Get<ChildTable>().Iterate(Child::kInStateValidOrRestoring))
                    {
                        if (!child.IsRxOnWhenIdle() && child.HasIp6Address(ip6Header.GetDestination()))
                        {
                            mIndirectSender.AddMessageForSleepyChild(aMessage, child);
                        }
                    }
                }
            }
        }
        else if ((neighbor = Get<NeighborTable>().FindNeighbor(ip6Header.GetDestination())) != nullptr &&
                 !neighbor->IsRxOnWhenIdle() && !aMessage.GetDirectTransmission())
        {
            // destined for a sleepy child
            Child &child = *static_cast<Child *>(neighbor);
            mIndirectSender.AddMessageForSleepyChild(aMessage, child);
        }
        else
        {
            // schedule direct transmission
            aMessage.SetDirectTransmission();
        }

        break;
    }

#if OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE
    case Message::kTypeSupervision:
    {
        Child *child = Get<Utils::ChildSupervisor>().GetDestination(aMessage);
        OT_ASSERT((child != nullptr) && !child->IsRxOnWhenIdle());
        mIndirectSender.AddMessageForSleepyChild(aMessage, *child);
        break;
    }
#endif

    default:
        aMessage.SetDirectTransmission();
        break;
    }

    mScheduleTransmissionTask.Post();

    return error;
}

void MeshForwarder::HandleResolved(const Ip6::Address &aEid, Error aError)
{
    Message *    cur, *next;
    Ip6::Address ip6Dst;
    bool         enqueuedMessage = false;

    for (cur = mResolvingQueue.GetHead(); cur; cur = next)
    {
        next = cur->GetNext();

        if (cur->GetType() != Message::kTypeIp6)
        {
            continue;
        }

        IgnoreError(cur->Read(Ip6::Header::kDestinationFieldOffset, ip6Dst));

        if (ip6Dst == aEid)
        {
            mResolvingQueue.Dequeue(*cur);

            if (aError == kErrorNone)
            {
                mSendQueue.Enqueue(*cur);
                enqueuedMessage = true;
            }
            else
            {
                LogMessage(kMessageDrop, *cur, nullptr, aError);
                cur->Free();
            }
        }
    }

    if (enqueuedMessage)
    {
        mScheduleTransmissionTask.Post();
    }
}

Error MeshForwarder::EvictMessage(Message::Priority aPriority)
{
    Error          error    = kErrorNotFound;
    PriorityQueue *queues[] = {&mResolvingQueue, &mSendQueue};
    Message *      evict    = nullptr;

    // search for a lower priority message to evict (choose lowest priority message among all queues)
    for (PriorityQueue *queue : queues)
    {
        for (uint8_t priority = 0; priority < aPriority; priority++)
        {
            for (Message *message = queue->GetHeadForPriority(static_cast<Message::Priority>(priority)); message;
                 message          = message->GetNext())
            {
                if (message->GetPriority() != priority)
                {
                    break;
                }

                if (message->GetDoNotEvict())
                {
                    continue;
                }

                evict     = message;
                aPriority = static_cast<Message::Priority>(priority);
                break;
            }
        }
    }

    if (evict != nullptr)
    {
        ExitNow(error = kErrorNone);
    }

    for (uint8_t priority = aPriority; priority < Message::kNumPriorities; priority++)
    {
        // search for an equal or higher priority indirect message to evict
        for (Message *message = mSendQueue.GetHeadForPriority(aPriority); message; message = message->GetNext())
        {
            if (message->GetPriority() != priority)
            {
                break;
            }

            if (message->GetDoNotEvict())
            {
                continue;
            }

            if (message->IsChildPending())
            {
                evict = message;
                ExitNow(error = kErrorNone);
            }
        }
    }

exit:

    if (error == kErrorNone)
    {
        RemoveMessage(*evict);
    }

    return error;
}

void MeshForwarder::RemoveMessages(Child &aChild, Message::SubType aSubType)
{
    Message *nextMessage;

    for (Message *message = mSendQueue.GetHead(); message; message = nextMessage)
    {
        nextMessage = message->GetNext();

        if ((aSubType != Message::kSubTypeNone) && (aSubType != message->GetSubType()))
        {
            continue;
        }

        if (mIndirectSender.RemoveMessageFromSleepyChild(*message, aChild) != kErrorNone)
        {
            switch (message->GetType())
            {
            case Message::kTypeIp6:
            {
                Ip6::Header ip6header;

                IgnoreError(message->Read(0, ip6header));

                if (&aChild == static_cast<Child *>(Get<NeighborTable>().FindNeighbor(ip6header.GetDestination())))
                {
                    message->ClearDirectTransmission();
                }

                break;
            }

            case Message::kType6lowpan:
            {
                Lowpan::MeshHeader meshHeader;

                IgnoreError(meshHeader.ParseFrom(*message));

                if (&aChild == static_cast<Child *>(Get<NeighborTable>().FindNeighbor(meshHeader.GetDestination())))
                {
                    message->ClearDirectTransmission();
                }

                break;
            }

            default:
                break;
            }
        }

        RemoveMessageIfNoPendingTx(*message);
    }
}

void MeshForwarder::RemoveDataResponseMessages(void)
{
    Ip6::Header ip6Header;
    Message *   next;

    for (Message *message = mSendQueue.GetHead(); message != nullptr; message = next)
    {
        next = message->GetNext();

        if (message->GetSubType() != Message::kSubTypeMleDataResponse)
        {
            continue;
        }

        IgnoreError(message->Read(0, ip6Header));

        if (!(ip6Header.GetDestination().IsMulticast()))
        {
            for (Child &child : Get<ChildTable>().Iterate(Child::kInStateAnyExceptInvalid))
            {
                IgnoreError(mIndirectSender.RemoveMessageFromSleepyChild(*message, child));
            }
        }

        if (mSendMessage == message)
        {
            mSendMessage = nullptr;
        }

        LogMessage(kMessageDrop, *message, nullptr, kErrorNone);
        mSendQueue.DequeueAndFree(*message);
    }
}

void MeshForwarder::SendMesh(Message &aMessage, Mac::TxFrame &aFrame)
{
    uint16_t fcf;
    bool     iePresent = CalcIePresent(&aMessage);

    // initialize MAC header
    fcf = Mac::Frame::kFcfFrameData | Mac::Frame::kFcfPanidCompression | Mac::Frame::kFcfDstAddrShort |
          Mac::Frame::kFcfSrcAddrShort | Mac::Frame::kFcfAckRequest | Mac::Frame::kFcfSecurityEnabled;

    if (iePresent)
    {
        fcf |= Mac::Frame::kFcfIePresent;
    }

    fcf |= CalcFrameVersion(Get<NeighborTable>().FindNeighbor(mMacDest), iePresent);

    aFrame.InitMacHeader(fcf, Mac::Frame::kKeyIdMode1 | Mac::Frame::kSecEncMic32);
    aFrame.SetDstPanId(Get<Mac::Mac>().GetPanId());
    aFrame.SetDstAddr(mMacDest.GetShort());
    aFrame.SetSrcAddr(mMacSource.GetShort());

#if OPENTHREAD_CONFIG_MAC_HEADER_IE_SUPPORT
    if (iePresent)
    {
        AppendHeaderIe(&aMessage, aFrame);
    }
#endif

    // write payload
    OT_ASSERT(aMessage.GetLength() <= aFrame.GetMaxPayloadLength());
    aMessage.ReadBytes(0, aFrame.GetPayload(), aMessage.GetLength());
    aFrame.SetPayloadLength(aMessage.GetLength());

    mMessageNextOffset = aMessage.GetLength();
}

Error MeshForwarder::UpdateMeshRoute(Message &aMessage)
{
    Error              error = kErrorNone;
    Lowpan::MeshHeader meshHeader;
    Neighbor *         neighbor;
    uint16_t           nextHop;

    IgnoreError(meshHeader.ParseFrom(aMessage));

    nextHop = Get<Mle::MleRouter>().GetNextHop(meshHeader.GetDestination());

    if (nextHop != Mac::kShortAddrInvalid)
    {
        neighbor = Get<NeighborTable>().FindNeighbor(nextHop);
    }
    else
    {
        neighbor = Get<NeighborTable>().FindNeighbor(meshHeader.GetDestination());
    }

    if (neighbor == nullptr)
    {
        ExitNow(error = kErrorDrop);
    }

    mMacDest.SetShort(neighbor->GetRloc16());
    mMacSource.SetShort(Get<Mac::Mac>().GetShortAddress());

    mAddMeshHeader = true;
    mMeshDest      = meshHeader.GetDestination();
    mMeshSource    = meshHeader.GetSource();

exit:
    return error;
}

void MeshForwarder::EvaluateRoutingCost(uint16_t aDest, uint8_t &aBestCost, uint16_t &aBestDest) const
{
    const Neighbor *neighbor;
    uint8_t         curCost = 0x00;

    // Path cost
    curCost = Get<Mle::MleRouter>().GetCost(aDest);

    if (!Mle::MleRouter::IsActiveRouter(aDest))
    {
        // Assume best link between remote child server and its parent.
        curCost += 1;
    }

    // Cost if the server is direct neighbor.
    neighbor = Get<NeighborTable>().FindNeighbor(aDest);

    if (neighbor != nullptr && neighbor->IsStateValid())
    {
        uint8_t cost;

        if (!Mle::MleRouter::IsActiveRouter(aDest))
        {
            // Cost calculated only from Link Quality In as the parent only maintains
            // one-direction link info.
            cost = Mle::MleRouter::LinkQualityToCost(neighbor->GetLinkInfo().GetLinkQuality());
        }
        else
        {
            cost = Get<Mle::MleRouter>().GetLinkCost(Mle::Mle::RouterIdFromRloc16(aDest));
        }

        // Choose the minimum cost
        curCost = OT_MIN(curCost, cost);
    }

    if ((aBestDest == Mac::kShortAddrInvalid) || (curCost < aBestCost))
    {
        aBestDest = aDest;
        aBestCost = curCost;
    }
}

Error MeshForwarder::AnycastRouteLookup(uint8_t aServiceId, AnycastType aType, uint16_t &aMeshDest) const
{
    NetworkData::Iterator iterator = NetworkData::kIteratorInit;
    uint8_t               bestCost = Mle::kMaxRouteCost;
    uint16_t              bestDest = Mac::kShortAddrInvalid;
    uint8_t               routerId;

    switch (aType)
    {
    case kAnycastDhcp6Agent:
    case kAnycastNeighborDiscoveryAgent:
    {
        NetworkData::OnMeshPrefixConfig config;
        Lowpan::Context                 context;

        SuccessOrExit(Get<NetworkData::Leader>().GetContext(aServiceId, context));

        while (Get<NetworkData::Leader>().GetNextOnMeshPrefix(iterator, config) == kErrorNone)
        {
            if (config.GetPrefix() != context.mPrefix)
            {
                continue;
            }

            switch (aType)
            {
            case kAnycastDhcp6Agent:
                if (!(config.mDhcp || config.mConfigure))
                {
                    continue;
                }
                break;
            case kAnycastNeighborDiscoveryAgent:
                if (!config.mNdDns)
                {
                    continue;
                }
                break;
            default:
                OT_ASSERT(false);
                break;
            }

            EvaluateRoutingCost(config.mRloc16, bestCost, bestDest);
        }

        break;
    }
    case kAnycastService:
    {
        NetworkData::ServiceConfig config;

        while (Get<NetworkData::Leader>().GetNextService(iterator, config) == kErrorNone)
        {
            if (config.mServiceId != aServiceId)
            {
                continue;
            }

            EvaluateRoutingCost(config.mServerConfig.mRloc16, bestCost, bestDest);
        }

        break;
    }
    }

    routerId = Mle::Mle::RouterIdFromRloc16(bestDest);

    if (!(Mle::Mle::IsActiveRouter(bestDest) ||
          Mle::Mle::Rloc16FromRouterId(routerId) == Get<Mle::MleRouter>().GetRloc16()))
    {
        // if agent is neither active router nor child of this device
        // use the parent of the ED Agent as Dest
        bestDest = Mle::Mle::Rloc16FromRouterId(routerId);
    }

    aMeshDest = bestDest;

exit:
    return (bestDest != Mac::kShortAddrInvalid) ? kErrorNone : kErrorNoRoute;
}

Error MeshForwarder::UpdateIp6RouteFtd(Ip6::Header &ip6Header, Message &aMessage)
{
    Mle::MleRouter &mle   = Get<Mle::MleRouter>();
    Error           error = kErrorNone;
    Neighbor *      neighbor;

    if (aMessage.GetOffset() > 0)
    {
        mMeshDest = aMessage.GetMeshDest();
    }
    else if (mle.IsRoutingLocator(ip6Header.GetDestination()))
    {
        uint16_t rloc16 = ip6Header.GetDestination().GetIid().GetLocator();
        VerifyOrExit(mle.IsRouterIdValid(Mle::Mle::RouterIdFromRloc16(rloc16)), error = kErrorDrop);
        mMeshDest = rloc16;
    }
    else if (mle.IsAnycastLocator(ip6Header.GetDestination()))
    {
        uint16_t aloc16 = ip6Header.GetDestination().GetIid().GetLocator();

        if (aloc16 == Mle::kAloc16Leader)
        {
            mMeshDest = Mle::Mle::Rloc16FromRouterId(mle.GetLeaderId());
        }
        else if (aloc16 <= Mle::kAloc16DhcpAgentEnd)
        {
            uint8_t contextId = static_cast<uint8_t>(aloc16 - Mle::kAloc16DhcpAgentStart + 1);
            SuccessOrExit(error = AnycastRouteLookup(contextId, kAnycastDhcp6Agent, mMeshDest));
        }
        else if (aloc16 <= Mle::kAloc16ServiceEnd)
        {
            uint8_t serviceId = static_cast<uint8_t>(aloc16 - Mle::kAloc16ServiceStart);
            SuccessOrExit(error = AnycastRouteLookup(serviceId, kAnycastService, mMeshDest));
        }
        else if (aloc16 <= Mle::kAloc16CommissionerEnd)
        {
            SuccessOrExit(error = MeshCoP::GetBorderAgentRloc(Get<ThreadNetif>(), mMeshDest));
        }

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
        else if (aloc16 == Mle::kAloc16BackboneRouterPrimary)
        {
            VerifyOrExit(Get<BackboneRouter::Leader>().HasPrimary(), error = kErrorDrop);
            mMeshDest = Get<BackboneRouter::Leader>().GetServer16();
        }
#endif
        else if ((aloc16 >= Mle::kAloc16NeighborDiscoveryAgentStart) &&
                 (aloc16 <= Mle::kAloc16NeighborDiscoveryAgentEnd))
        {
            uint8_t contextId = static_cast<uint8_t>(aloc16 - Mle::kAloc16NeighborDiscoveryAgentStart + 1);
            SuccessOrExit(error = AnycastRouteLookup(contextId, kAnycastNeighborDiscoveryAgent, mMeshDest));
        }
        else
        {
            ExitNow(error = kErrorDrop);
        }
    }
    else if ((neighbor = Get<NeighborTable>().FindNeighbor(ip6Header.GetDestination())) != nullptr)
    {
        mMeshDest = neighbor->GetRloc16();
    }
    else if (Get<NetworkData::Leader>().IsOnMesh(ip6Header.GetDestination()))
    {
        SuccessOrExit(error = Get<AddressResolver>().Resolve(ip6Header.GetDestination(), mMeshDest));
    }
    else
    {
        IgnoreError(Get<NetworkData::Leader>().RouteLookup(ip6Header.GetSource(), ip6Header.GetDestination(), nullptr,
                                                           &mMeshDest));
    }

    VerifyOrExit(mMeshDest != Mac::kShortAddrInvalid, error = kErrorDrop);

    mMeshSource = Get<Mac::Mac>().GetShortAddress();

    SuccessOrExit(error = mle.CheckReachability(mMeshDest, ip6Header));
    aMessage.SetMeshDest(mMeshDest);
    mMacDest.SetShort(mle.GetNextHop(mMeshDest));

    if (mMacDest.GetShort() != mMeshDest)
    {
        // destination is not neighbor
        mMacSource.SetShort(mMeshSource);
        mAddMeshHeader = true;
    }

exit:
    return error;
}

Error MeshForwarder::GetIp6Header(const uint8_t *     aFrame,
                                  uint16_t            aFrameLength,
                                  const Mac::Address &aMacSource,
                                  const Mac::Address &aMacDest,
                                  Ip6::Header &       aIp6Header)
{
    uint8_t headerLength;
    bool    nextHeaderCompressed;

    return DecompressIp6Header(aFrame, aFrameLength, aMacSource, aMacDest, aIp6Header, headerLength,
                               nextHeaderCompressed);
}

void MeshForwarder::SendIcmpErrorIfDstUnreach(const Message &     aMessage,
                                              const Mac::Address &aMacSource,
                                              const Mac::Address &aMacDest)
{
    Error       error;
    Ip6::Header ip6header;
    Child *     child;

    VerifyOrExit(aMacSource.IsShort() && aMacDest.IsShort());

    child = Get<ChildTable>().FindChild(aMacSource.GetShort(), Child::kInStateAnyExceptInvalid);
    VerifyOrExit((child == nullptr) || child->IsFullThreadDevice());

    IgnoreError(aMessage.Read(0, ip6header));
    VerifyOrExit(!ip6header.GetDestination().IsMulticast() &&
                 Get<NetworkData::Leader>().IsOnMesh(ip6header.GetDestination()));

    error = Get<Mle::MleRouter>().CheckReachability(aMacDest.GetShort(), ip6header);

    if (error == kErrorNoRoute)
    {
        SendDestinationUnreachable(aMacSource.GetShort(), aMessage);
    }

exit:
    return;
}

Error MeshForwarder::CheckReachability(const uint8_t *     aFrame,
                                       uint16_t            aFrameLength,
                                       const Mac::Address &aMeshSource,
                                       const Mac::Address &aMeshDest)
{
    Error                  error = kErrorNone;
    Ip6::Header            ip6Header;
    Message *              message = nullptr;
    Lowpan::FragmentHeader fragmentHeader;
    uint16_t               fragmentHeaderLength;
    uint16_t               datagramSize = 0;

    if (fragmentHeader.ParseFrom(aFrame, aFrameLength, fragmentHeaderLength) == kErrorNone)
    {
        // Only the first fragment header is followed by a LOWPAN_IPHC header
        VerifyOrExit(fragmentHeader.GetDatagramOffset() == 0, error = kErrorNotFound);
        aFrame += fragmentHeaderLength;
        aFrameLength -= fragmentHeaderLength;

        datagramSize = fragmentHeader.GetDatagramSize();
    }

    VerifyOrExit(aFrameLength >= 1 && Lowpan::Lowpan::IsLowpanHc(aFrame), error = kErrorNotFound);

    error = FrameToMessage(aFrame, aFrameLength, datagramSize, aMeshSource, aMeshDest, message);
    SuccessOrExit(error);

    IgnoreError(message->Read(0, ip6Header));
    error = Get<Mle::MleRouter>().CheckReachability(aMeshDest.GetShort(), ip6Header);

exit:
    if (error == kErrorNotFound)
    {
        // the message may not contain an IPv6 header
        error = kErrorNone;
    }
    else if (error == kErrorNoRoute)
    {
        SendDestinationUnreachable(aMeshSource.GetShort(), *message);
    }

    FreeMessage(message);

    return error;
}

void MeshForwarder::SendDestinationUnreachable(uint16_t aMeshSource, const Message &aMessage)
{
    Ip6::MessageInfo messageInfo;

    messageInfo.GetPeerAddr() = Get<Mle::MleRouter>().GetMeshLocal16();
    messageInfo.GetPeerAddr().GetIid().SetLocator(aMeshSource);

    IgnoreError(Get<Ip6::Icmp>().SendError(Ip6::Icmp::Header::kTypeDstUnreach,
                                           Ip6::Icmp::Header::kCodeDstUnreachNoRoute, messageInfo, aMessage));
}

void MeshForwarder::HandleMesh(uint8_t *             aFrame,
                               uint16_t              aFrameLength,
                               const Mac::Address &  aMacSource,
                               const ThreadLinkInfo &aLinkInfo)
{
    Error              error   = kErrorNone;
    Message *          message = nullptr;
    Mac::Address       meshDest;
    Mac::Address       meshSource;
    Lowpan::MeshHeader meshHeader;
    uint16_t           headerLength;

    // Security Check: only process Mesh Header frames that had security enabled.
    VerifyOrExit(aLinkInfo.IsLinkSecurityEnabled(), error = kErrorSecurity);

    SuccessOrExit(error = meshHeader.ParseFrom(aFrame, aFrameLength, headerLength));

    meshSource.SetShort(meshHeader.GetSource());
    meshDest.SetShort(meshHeader.GetDestination());

    aFrame += headerLength;
    aFrameLength -= headerLength;

    UpdateRoutes(aFrame, aFrameLength, meshSource, meshDest);

    if (meshDest.GetShort() == Get<Mac::Mac>().GetShortAddress() ||
        Get<Mle::MleRouter>().IsMinimalChild(meshDest.GetShort()))
    {
        if (Lowpan::FragmentHeader::IsFragmentHeader(aFrame, aFrameLength))
        {
            HandleFragment(aFrame, aFrameLength, meshSource, meshDest, aLinkInfo);
        }
        else if (Lowpan::Lowpan::IsLowpanHc(aFrame))
        {
            HandleLowpanHC(aFrame, aFrameLength, meshSource, meshDest, aLinkInfo);
        }
        else
        {
            ExitNow(error = kErrorParse);
        }
    }
    else if (meshHeader.GetHopsLeft() > 0)
    {
        Message::Priority priority = Message::kPriorityNormal;
        uint16_t          offset   = 0;

        Get<Mle::MleRouter>().ResolveRoutingLoops(aMacSource.GetShort(), meshDest.GetShort());

        SuccessOrExit(error = CheckReachability(aFrame, aFrameLength, meshSource, meshDest));

        meshHeader.DecrementHopsLeft();

        GetForwardFramePriority(aFrame, aFrameLength, meshSource, meshDest, priority);
        message = Get<MessagePool>().New(Message::kType6lowpan, priority);
        VerifyOrExit(message != nullptr, error = kErrorNoBufs);

        SuccessOrExit(error = message->SetLength(meshHeader.GetHeaderLength() + aFrameLength));
        offset += meshHeader.WriteTo(*message, offset);
        message->WriteBytes(offset, aFrame, aFrameLength);
        message->SetLinkInfo(aLinkInfo);

#if OPENTHREAD_CONFIG_MULTI_RADIO
        // We set the received radio type on the message in order for it
        // to be logged correctly from LogMessage().
        message->SetRadioType(static_cast<Mac::RadioType>(aLinkInfo.mRadioType));
#endif

        LogMessage(kMessageReceive, *message, &aMacSource, kErrorNone);

#if OPENTHREAD_CONFIG_MULTI_RADIO
        // Since the message will be forwarded, we clear the radio
        // type on the message to allow the radio type for tx to be
        // selected later (based on the radios supported by the next
        // hop).
        message->ClearRadioType();
#endif

        IgnoreError(SendMessage(*message));
    }

exit:

    if (error != kErrorNone)
    {
        otLogInfoMac("Dropping rx mesh frame, error:%s, len:%d, src:%s, sec:%s", ErrorToString(error), aFrameLength,
                     aMacSource.ToString().AsCString(), aLinkInfo.IsLinkSecurityEnabled() ? "yes" : "no");
        FreeMessage(message);
    }
}

void MeshForwarder::UpdateRoutes(const uint8_t *     aFrame,
                                 uint16_t            aFrameLength,
                                 const Mac::Address &aMeshSource,
                                 const Mac::Address &aMeshDest)
{
    Ip6::Header ip6Header;
    Neighbor *  neighbor;

    VerifyOrExit(!aMeshDest.IsBroadcast() && aMeshSource.IsShort());
    SuccessOrExit(GetIp6Header(aFrame, aFrameLength, aMeshSource, aMeshDest, ip6Header));

    if (!ip6Header.GetSource().GetIid().IsLocator() &&
        Get<NetworkData::Leader>().IsOnMesh(ip6Header.GetSource()) /* only for on mesh address which may require AQ */)
    {
        // FTDs MAY add/update EID-to-RLOC Map Cache entries by
        // inspecting packets being received.

        Get<AddressResolver>().UpdateSnoopedCacheEntry(ip6Header.GetSource(), aMeshSource.GetShort(),
                                                       aMeshDest.GetShort());
    }

    neighbor = Get<NeighborTable>().FindNeighbor(ip6Header.GetSource());
    VerifyOrExit(neighbor != nullptr && !neighbor->IsFullThreadDevice());

    if (!Mle::Mle::RouterIdMatch(aMeshSource.GetShort(), Get<Mac::Mac>().GetShortAddress()))
    {
        Get<Mle::MleRouter>().RemoveNeighbor(*neighbor);
    }

exit:
    return;
}

bool MeshForwarder::FragmentPriorityList::UpdateOnTimeTick(void)
{
    bool contineRxingTicks = false;

    for (Entry &entry : mEntries)
    {
        if (!entry.IsExpired())
        {
            entry.DecrementLifetime();

            if (!entry.IsExpired())
            {
                contineRxingTicks = true;
            }
        }
    }

    return contineRxingTicks;
}

void MeshForwarder::UpdateFragmentPriority(Lowpan::FragmentHeader &aFragmentHeader,
                                           uint16_t                aFragmentLength,
                                           uint16_t                aSrcRloc16,
                                           Message::Priority       aPriority)
{
    FragmentPriorityList::Entry *entry;

    entry = mFragmentPriorityList.FindEntry(aSrcRloc16, aFragmentHeader.GetDatagramTag());

    if (entry == nullptr)
    {
        VerifyOrExit(aFragmentHeader.GetDatagramOffset() == 0);

        mFragmentPriorityList.AllocateEntry(aSrcRloc16, aFragmentHeader.GetDatagramTag(), aPriority);
        Get<TimeTicker>().RegisterReceiver(TimeTicker::kMeshForwarder);
        ExitNow();
    }

    if (aFragmentHeader.GetDatagramOffset() + aFragmentLength >= aFragmentHeader.GetDatagramSize())
    {
        entry->Clear();
    }
    else
    {
        entry->ResetLifetime();
    }

exit:
    return;
}

MeshForwarder::FragmentPriorityList::Entry *MeshForwarder::FragmentPriorityList::FindEntry(uint16_t aSrcRloc16,
                                                                                           uint16_t aTag)
{
    Entry *rval = nullptr;

    for (Entry &entry : mEntries)
    {
        if (!entry.IsExpired() && entry.Matches(aSrcRloc16, aTag))
        {
            rval = &entry;
            break;
        }
    }

    return rval;
}

MeshForwarder::FragmentPriorityList::Entry *MeshForwarder::FragmentPriorityList::AllocateEntry(
    uint16_t          aSrcRloc16,
    uint16_t          aTag,
    Message::Priority aPriority)
{
    Entry *newEntry = nullptr;

    for (Entry &entry : mEntries)
    {
        if (entry.IsExpired())
        {
            entry.mSrcRloc16   = aSrcRloc16;
            entry.mDatagramTag = aTag;
            entry.mPriority    = aPriority;
            entry.ResetLifetime();
            newEntry = &entry;
            break;
        }
    }

    return newEntry;
}

Error MeshForwarder::GetFragmentPriority(Lowpan::FragmentHeader &aFragmentHeader,
                                         uint16_t                aSrcRloc16,
                                         Message::Priority &     aPriority)
{
    Error                        error = kErrorNone;
    FragmentPriorityList::Entry *entry;

    entry = mFragmentPriorityList.FindEntry(aSrcRloc16, aFragmentHeader.GetDatagramTag());
    VerifyOrExit(entry != nullptr, error = kErrorNotFound);
    aPriority = entry->GetPriority();

exit:
    return error;
}

void MeshForwarder::GetForwardFramePriority(const uint8_t *     aFrame,
                                            uint16_t            aFrameLength,
                                            const Mac::Address &aMeshSource,
                                            const Mac::Address &aMeshDest,
                                            Message::Priority & aPriority)
{
    Error                  error      = kErrorNone;
    bool                   isFragment = false;
    Lowpan::FragmentHeader fragmentHeader;
    uint16_t               fragmentHeaderLength;

    if (fragmentHeader.ParseFrom(aFrame, aFrameLength, fragmentHeaderLength) == kErrorNone)
    {
        isFragment = true;
        aFrame += fragmentHeaderLength;
        aFrameLength -= fragmentHeaderLength;

        if (fragmentHeader.GetDatagramOffset() > 0)
        {
            // Get priority from the pre-buffered info
            ExitNow(error = GetFragmentPriority(fragmentHeader, aMeshSource.GetShort(), aPriority));
        }
    }

    // Get priority from IPv6 header or UDP destination port directly
    error = GetFramePriority(aFrame, aFrameLength, aMeshSource, aMeshDest, aPriority);

exit:
    if (error != kErrorNone)
    {
        otLogNoteMac("Failed to get forwarded frame priority, error:%s, len:%d, src:%d, dst:%s", ErrorToString(error),
                     aFrameLength, aMeshSource.ToString().AsCString(), aMeshDest.ToString().AsCString());
    }
    else if (isFragment)
    {
        UpdateFragmentPriority(fragmentHeader, aFrameLength, aMeshSource.GetShort(), aPriority);
    }

    return;
}

// LCOV_EXCL_START

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_NOTE) && (OPENTHREAD_CONFIG_LOG_MAC == 1)

Error MeshForwarder::LogMeshFragmentHeader(MessageAction       aAction,
                                           const Message &     aMessage,
                                           const Mac::Address *aMacAddress,
                                           Error               aError,
                                           uint16_t &          aOffset,
                                           Mac::Address &      aMeshSource,
                                           Mac::Address &      aMeshDest,
                                           otLogLevel          aLogLevel)
{
    Error                  error             = kErrorFailed;
    bool                   hasFragmentHeader = false;
    bool                   shouldLogRss;
    Lowpan::MeshHeader     meshHeader;
    Lowpan::FragmentHeader fragmentHeader;
    uint16_t               headerLength;
    bool                   shouldLogRadio = false;
    const char *           radioString    = "";

    SuccessOrExit(meshHeader.ParseFrom(aMessage, headerLength));

    aMeshSource.SetShort(meshHeader.GetSource());
    aMeshDest.SetShort(meshHeader.GetDestination());

    aOffset = headerLength;

    if (fragmentHeader.ParseFrom(aMessage, aOffset, headerLength) == kErrorNone)
    {
        hasFragmentHeader = true;
        aOffset += headerLength;
    }

    shouldLogRss = (aAction == kMessageReceive) || (aAction == kMessageReassemblyDrop);

#if OPENTHREAD_CONFIG_MULTI_RADIO
    shouldLogRadio = true;
    radioString    = aMessage.IsRadioTypeSet() ? RadioTypeToString(aMessage.GetRadioType()) : "all";
#endif

    otLogMac(aLogLevel, "%s mesh frame, len:%d%s%s, msrc:%s, mdst:%s, hops:%d, frag:%s, sec:%s%s%s%s%s%s%s",
             MessageActionToString(aAction, aError), aMessage.GetLength(),
             (aMacAddress == nullptr) ? "" : ((aAction == kMessageReceive) ? ", from:" : ", to:"),
             (aMacAddress == nullptr) ? "" : aMacAddress->ToString().AsCString(), aMeshSource.ToString().AsCString(),
             aMeshDest.ToString().AsCString(), meshHeader.GetHopsLeft() + ((aAction == kMessageReceive) ? 1 : 0),
             hasFragmentHeader ? "yes" : "no", aMessage.IsLinkSecurityEnabled() ? "yes" : "no",
             (aError == kErrorNone) ? "" : ", error:", (aError == kErrorNone) ? "" : ErrorToString(aError),
             shouldLogRss ? ", rss:" : "", shouldLogRss ? aMessage.GetRssAverager().ToString().AsCString() : "",
             shouldLogRadio ? ", radio:" : "", radioString);

    if (hasFragmentHeader)
    {
        otLogMac(aLogLevel, "    Frag tag:%04x, offset:%d, size:%d", fragmentHeader.GetDatagramTag(),
                 fragmentHeader.GetDatagramOffset(), fragmentHeader.GetDatagramSize());

        VerifyOrExit(fragmentHeader.GetDatagramOffset() == 0);
    }

    error = kErrorNone;

exit:
    return error;
}

Error MeshForwarder::DecompressIp6UdpTcpHeader(const Message &     aMessage,
                                               uint16_t            aOffset,
                                               const Mac::Address &aMeshSource,
                                               const Mac::Address &aMeshDest,
                                               Ip6::Header &       aIp6Header,
                                               uint16_t &          aChecksum,
                                               uint16_t &          aSourcePort,
                                               uint16_t &          aDestPort)
{
    Error    error = kErrorParse;
    int      headerLength;
    bool     nextHeaderCompressed;
    uint8_t  frameBuffer[sizeof(Ip6::Header)];
    uint16_t frameLength;
    union
    {
        Ip6::Udp::Header udp;
        Ip6::Tcp::Header tcp;
    } header;

    aChecksum   = 0;
    aSourcePort = 0;
    aDestPort   = 0;

    // Read and decompress the IPv6 header

    frameLength = aMessage.ReadBytes(aOffset, frameBuffer, sizeof(frameBuffer));

    headerLength = Get<Lowpan::Lowpan>().DecompressBaseHeader(aIp6Header, nextHeaderCompressed, aMeshSource, aMeshDest,
                                                              frameBuffer, frameLength);
    VerifyOrExit(headerLength >= 0);

    aOffset += headerLength;

    // Read and decompress UDP or TCP header

    switch (aIp6Header.GetNextHeader())
    {
    case Ip6::kProtoUdp:
        if (nextHeaderCompressed)
        {
            frameLength  = aMessage.ReadBytes(aOffset, frameBuffer, sizeof(Ip6::Udp::Header));
            headerLength = Get<Lowpan::Lowpan>().DecompressUdpHeader(header.udp, frameBuffer, frameLength);
            VerifyOrExit(headerLength >= 0);
        }
        else
        {
            SuccessOrExit(aMessage.Read(aOffset, header.udp));
        }

        aChecksum   = header.udp.GetChecksum();
        aSourcePort = header.udp.GetSourcePort();
        aDestPort   = header.udp.GetDestinationPort();
        break;

    case Ip6::kProtoTcp:
        SuccessOrExit(aMessage.Read(aOffset, header.tcp));
        aChecksum   = header.tcp.GetChecksum();
        aSourcePort = header.tcp.GetSourcePort();
        aDestPort   = header.tcp.GetDestinationPort();
        break;

    default:
        break;
    }

    error = kErrorNone;

exit:
    return error;
}

void MeshForwarder::LogMeshIpHeader(const Message &     aMessage,
                                    uint16_t            aOffset,
                                    const Mac::Address &aMeshSource,
                                    const Mac::Address &aMeshDest,
                                    otLogLevel          aLogLevel)
{
    uint16_t    checksum;
    uint16_t    sourcePort;
    uint16_t    destPort;
    Ip6::Header ip6Header;

    SuccessOrExit(DecompressIp6UdpTcpHeader(aMessage, aOffset, aMeshSource, aMeshDest, ip6Header, checksum, sourcePort,
                                            destPort));

    otLogMac(aLogLevel, "    IPv6 %s msg, chksum:%04x, prio:%s", Ip6::Ip6::IpProtoToString(ip6Header.GetNextHeader()),
             checksum, MessagePriorityToString(aMessage));

    LogIp6SourceDestAddresses(ip6Header, sourcePort, destPort, aLogLevel);

exit:
    return;
}

void MeshForwarder::LogMeshMessage(MessageAction       aAction,
                                   const Message &     aMessage,
                                   const Mac::Address *aMacAddress,
                                   Error               aError,
                                   otLogLevel          aLogLevel)
{
    uint16_t     offset;
    Mac::Address meshSource;
    Mac::Address meshDest;

    SuccessOrExit(
        LogMeshFragmentHeader(aAction, aMessage, aMacAddress, aError, offset, meshSource, meshDest, aLogLevel));

    // When log action is `kMessageTransmit` we do not include
    // the IPv6 header info in the logs, as the same info is
    // logged when the same Mesh Header message was received
    // and info about it was logged.

    VerifyOrExit(aAction != kMessageTransmit);

    LogMeshIpHeader(aMessage, offset, meshSource, meshDest, aLogLevel);

exit:
    return;
}

#endif // #if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_NOTE) && (OPENTHREAD_CONFIG_LOG_MAC == 1)

// LCOV_EXCL_STOP

} // namespace ot

#endif // OPENTHREAD_FTD
