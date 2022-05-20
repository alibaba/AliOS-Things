/*
 *  Copyright (c) 2016, The OpenThread Authors.  All rights reserved.
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
 *   This file implements MLE functionality required for the Thread Router and Leader roles.
 */

#include "mle_router.hpp"

#if OPENTHREAD_FTD

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/encoding.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/random.hpp"
#include "common/settings.hpp"
#include "mac/mac_types.hpp"
#include "meshcop/meshcop.hpp"
#include "net/icmp6.hpp"
#include "thread/key_manager.hpp"
#include "thread/thread_netif.hpp"
#include "thread/thread_tlvs.hpp"
#include "thread/time_sync_service.hpp"
#include "thread/uri_paths.hpp"
#include "utils/otns.hpp"

namespace ot {
namespace Mle {

MleRouter::MleRouter(Instance &aInstance)
    : Mle(aInstance)
    , mAdvertiseTrickleTimer(aInstance, MleRouter::HandleAdvertiseTrickleTimer)
    , mAddressSolicit(UriPath::kAddressSolicit, &MleRouter::HandleAddressSolicit, this)
    , mAddressRelease(UriPath::kAddressRelease, &MleRouter::HandleAddressRelease, this)
    , mChildTable(aInstance)
    , mRouterTable(aInstance)
    , mChallengeTimeout(0)
    , mNextChildId(kMaxChildId)
    , mNetworkIdTimeout(kNetworkIdTimeout)
    , mRouterUpgradeThreshold(kRouterUpgradeThreshold)
    , mRouterDowngradeThreshold(kRouterDowngradeThreshold)
    , mLeaderWeight(kLeaderWeight)
#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    , mPreferredLeaderPartitionId(0)
#endif
    , mRouterEligible(true)
    , mAddressSolicitPending(false)
    , mAddressSolicitRejected(false)
    , mPreviousPartitionIdRouter(0)
    , mPreviousPartitionId(0)
    , mPreviousPartitionRouterIdSequence(0)
    , mPreviousPartitionIdTimeout(0)
    , mRouterSelectionJitter(kRouterSelectionJitter)
    , mRouterSelectionJitterTimeout(0)
    , mParentPriority(kParentPriorityUnspecified)
#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    , mBackboneRouterRegistrationDelay(0)
#endif
#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    , mMaxChildIpAddresses(0)
#endif
    , mDiscoveryRequestCallback(nullptr)
    , mDiscoveryRequestCallbackContext(nullptr)
{
    mDeviceMode.Set(mDeviceMode.Get() | DeviceMode::kModeFullThreadDevice | DeviceMode::kModeFullNetworkData);

    SetRouterId(kInvalidRouterId);

#if OPENTHREAD_CONFIG_MLE_STEERING_DATA_SET_OOB_ENABLE
    mSteeringData.Clear();
#endif
}

void MleRouter::HandlePartitionChange(void)
{
    mPreviousPartitionId               = mLeaderData.GetPartitionId();
    mPreviousPartitionRouterIdSequence = mRouterTable.GetRouterIdSequence();
    mPreviousPartitionIdTimeout        = GetNetworkIdTimeout();

    Get<AddressResolver>().Clear();
    IgnoreError(Get<Tmf::Agent>().AbortTransaction(&MleRouter::HandleAddressSolicitResponse, this));
    mRouterTable.Clear();
}

bool MleRouter::IsRouterEligible(void) const
{
    bool                  rval      = false;
    const SecurityPolicy &secPolicy = Get<KeyManager>().GetSecurityPolicy();

    VerifyOrExit(mRouterEligible && IsFullThreadDevice());

#if OPENTHREAD_CONFIG_THREAD_VERSION == OT_THREAD_VERSION_1_1
    VerifyOrExit(secPolicy.mRoutersEnabled);
#else
    if (secPolicy.mCommercialCommissioningEnabled)
    {
        VerifyOrExit(secPolicy.mNonCcmRoutersEnabled);
    }
    if (!secPolicy.mRoutersEnabled)
    {
        VerifyOrExit(secPolicy.mVersionThresholdForRouting + SecurityPolicy::kVersionThresholdOffsetVersion <=
                     kThreadVersion);
    }
#endif

    rval = true;

exit:
    return rval;
}

Error MleRouter::SetRouterEligible(bool aEligible)
{
    Error error = kErrorNone;

    VerifyOrExit(IsFullThreadDevice() || !aEligible, error = kErrorNotCapable);

    mRouterEligible = aEligible;

    switch (mRole)
    {
    case kRoleDisabled:
    case kRoleDetached:
        break;

    case kRoleChild:
        Get<Mac::Mac>().SetBeaconEnabled(mRouterEligible);
        break;

    case kRoleRouter:
    case kRoleLeader:
        if (!mRouterEligible)
        {
            IgnoreError(BecomeDetached());
        }

        break;
    }

exit:
    return error;
}

Error MleRouter::BecomeRouter(ThreadStatusTlv::Status aStatus)
{
    Error error = kErrorNone;

    VerifyOrExit(!IsDisabled(), error = kErrorInvalidState);
    VerifyOrExit(!IsRouter(), error = kErrorNone);
    VerifyOrExit(IsRouterEligible(), error = kErrorNotCapable);

    otLogInfoMle("Attempt to become router");

    Get<MeshForwarder>().SetRxOnWhenIdle(true);
    mRouterSelectionJitterTimeout = 0;

    switch (mRole)
    {
    case kRoleDetached:
        SuccessOrExit(error = SendLinkRequest(nullptr));
        Get<TimeTicker>().RegisterReceiver(TimeTicker::kMleRouter);
        break;

    case kRoleChild:
        SuccessOrExit(error = SendAddressSolicit(aStatus));
        break;

    default:
        OT_ASSERT(false);
        OT_UNREACHABLE_CODE(break);
    }

exit:
    return error;
}

Error MleRouter::BecomeLeader(void)
{
    Error    error = kErrorNone;
    Router * router;
    uint32_t partitionId;
    uint8_t  leaderId;

    VerifyOrExit(!Get<MeshCoP::ActiveDataset>().IsPartiallyComplete(), error = kErrorInvalidState);
    VerifyOrExit(!IsDisabled(), error = kErrorInvalidState);
    VerifyOrExit(!IsLeader(), error = kErrorNone);
    VerifyOrExit(IsRouterEligible(), error = kErrorNotCapable);

    mRouterTable.Clear();

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    partitionId = mPreferredLeaderPartitionId ? mPreferredLeaderPartitionId : Random::NonCrypto::GetUint32();
#else
    partitionId = Random::NonCrypto::GetUint32();
#endif

    leaderId = IsRouterIdValid(mPreviousRouterId) ? mPreviousRouterId
                                                  : Random::NonCrypto::GetUint8InRange(0, kMaxRouterId + 1);

    SetLeaderData(partitionId, mLeaderWeight, leaderId);

    router = mRouterTable.Allocate(leaderId);
    OT_ASSERT(router != nullptr);

    SetRouterId(leaderId);
    router->SetExtAddress(Get<Mac::Mac>().GetExtAddress());

    Get<NetworkData::Leader>().Reset();
    Get<MeshCoP::Leader>().SetEmptyCommissionerData();

    SetStateLeader(Rloc16FromRouterId(leaderId));

exit:
    return error;
}

void MleRouter::StopLeader(void)
{
    Get<Tmf::Agent>().RemoveResource(mAddressSolicit);
    Get<Tmf::Agent>().RemoveResource(mAddressRelease);
    Get<MeshCoP::ActiveDataset>().StopLeader();
    Get<MeshCoP::PendingDataset>().StopLeader();
    StopAdvertiseTrickleTimer();
    Get<NetworkData::Leader>().Stop();
    Get<ThreadNetif>().UnsubscribeAllRoutersMulticast();
}

void MleRouter::HandleDetachStart(void)
{
    mRouterTable.ClearNeighbors();
    StopLeader();
    Get<TimeTicker>().UnregisterReceiver(TimeTicker::kMleRouter);
}

void MleRouter::HandleChildStart(AttachMode aMode)
{
    // reset `rejected` flag whenever REED becomes child.
    mAddressSolicitRejected = false;

    mRouterSelectionJitterTimeout = 1 + Random::NonCrypto::GetUint8InRange(0, mRouterSelectionJitter);

    StopLeader();
    Get<TimeTicker>().RegisterReceiver(TimeTicker::kMleRouter);

    if (mRouterEligible)
    {
        Get<Mac::Mac>().SetBeaconEnabled(true);
    }

    Get<ThreadNetif>().SubscribeAllRoutersMulticast();

    VerifyOrExit(IsRouterIdValid(mPreviousRouterId));

    switch (aMode)
    {
    case kAttachSameDowngrade:
        SendAddressRelease();

        // reset children info if any
        if (HasChildren())
        {
            RemoveChildren();
        }

        // reset routerId info
        SetRouterId(kInvalidRouterId);
        break;

    case kAttachSame1:
    case kAttachSame2:
        if (HasChildren())
        {
            IgnoreError(BecomeRouter(ThreadStatusTlv::kHaveChildIdRequest));
        }

        break;

    case kAttachAny:
        // If attach was started due to receiving MLE Announce Messages, all rx-on-when-idle devices would
        // start attach immediately when receiving such Announce message as in Thread 1.1 specification,
        // Section 4.8.1,
        // "If the received value is newer and the channel and/or PAN ID in the Announce message differ
        //  from those currently in use, the receiving device attempts to attach using the channel and
        //  PAN ID received from the Announce message."
        //
        // That is, Parent-child relationship is highly unlikely to be kept in the new partition, so here
        // removes all children, leaving whether to become router according to the new partition status.
        if (IsAnnounceAttach() && HasChildren())
        {
            RemoveChildren();
        }

        OT_FALL_THROUGH;

    case kAttachBetter:
        if (HasChildren() && mPreviousPartitionIdRouter != mLeaderData.GetPartitionId())
        {
            IgnoreError(BecomeRouter(ThreadStatusTlv::kParentPartitionChange));
        }

        break;
    }

exit:

    if (mRouterTable.GetActiveRouterCount() >= mRouterUpgradeThreshold &&
        (!IsRouterIdValid(mPreviousRouterId) || !HasChildren()))
    {
        SetRouterId(kInvalidRouterId);
    }
}

void MleRouter::SetStateRouter(uint16_t aRloc16)
{
    SetRloc16(aRloc16);

    SetRole(kRoleRouter);
    SetAttachState(kAttachStateIdle);
    mAttachCounter = 0;
    mAttachTimer.Stop();
    mMessageTransmissionTimer.Stop();
    StopAdvertiseTrickleTimer();
    ResetAdvertiseInterval();

    Get<ThreadNetif>().SubscribeAllRoutersMulticast();
    mPreviousPartitionIdRouter = mLeaderData.GetPartitionId();
    Get<NetworkData::Leader>().Stop();
    Get<Ip6::Ip6>().SetForwardingEnabled(true);
    Get<Ip6::Mpl>().SetTimerExpirations(kMplRouterDataMessageTimerExpirations);
    Get<Mac::Mac>().SetBeaconEnabled(true);

    // remove children that do not have matching RLOC16
    for (Child &child : Get<ChildTable>().Iterate(Child::kInStateValidOrRestoring))
    {
        if (RouterIdFromRloc16(child.GetRloc16()) != mRouterId)
        {
            RemoveNeighbor(child);
        }
    }
}

void MleRouter::SetStateLeader(uint16_t aRloc16)
{
    IgnoreError(Get<MeshCoP::ActiveDataset>().Restore());
    IgnoreError(Get<MeshCoP::PendingDataset>().Restore());
    SetRloc16(aRloc16);

    SetRole(kRoleLeader);
    SetAttachState(kAttachStateIdle);
    mAttachCounter = 0;
    mAttachTimer.Stop();
    mMessageTransmissionTimer.Stop();
    StopAdvertiseTrickleTimer();
    ResetAdvertiseInterval();
    IgnoreError(GetLeaderAloc(mLeaderAloc.GetAddress()));
    Get<ThreadNetif>().AddUnicastAddress(mLeaderAloc);

    Get<ThreadNetif>().SubscribeAllRoutersMulticast();
    mPreviousPartitionIdRouter = mLeaderData.GetPartitionId();
    Get<TimeTicker>().RegisterReceiver(TimeTicker::kMleRouter);

    Get<NetworkData::Leader>().Start();
    Get<MeshCoP::ActiveDataset>().StartLeader();
    Get<MeshCoP::PendingDataset>().StartLeader();
    Get<Tmf::Agent>().AddResource(mAddressSolicit);
    Get<Tmf::Agent>().AddResource(mAddressRelease);
    Get<Ip6::Ip6>().SetForwardingEnabled(true);
    Get<Ip6::Mpl>().SetTimerExpirations(kMplRouterDataMessageTimerExpirations);
    Get<Mac::Mac>().SetBeaconEnabled(true);
    Get<AddressResolver>().Clear();

    // remove children that do not have matching RLOC16
    for (Child &child : Get<ChildTable>().Iterate(Child::kInStateValidOrRestoring))
    {
        if (RouterIdFromRloc16(child.GetRloc16()) != mRouterId)
        {
            RemoveNeighbor(child);
        }
    }

    otLogNoteMle("Leader partition id 0x%x", mLeaderData.GetPartitionId());
}

void MleRouter::HandleAdvertiseTrickleTimer(TrickleTimer &aTimer)
{
    aTimer.Get<MleRouter>().HandleAdvertiseTrickleTimer();
}

void MleRouter::HandleAdvertiseTrickleTimer(void)
{
    VerifyOrExit(IsRouterEligible(), mAdvertiseTrickleTimer.Stop());

    SendAdvertisement();

exit:
    return;
}

void MleRouter::StopAdvertiseTrickleTimer(void)
{
    mAdvertiseTrickleTimer.Stop();
}

void MleRouter::ResetAdvertiseInterval(void)
{
    VerifyOrExit(IsRouterOrLeader());

    if (!mAdvertiseTrickleTimer.IsRunning())
    {
        mAdvertiseTrickleTimer.Start(TrickleTimer::kModeTrickle, Time::SecToMsec(kAdvertiseIntervalMin),
                                     Time::SecToMsec(kAdvertiseIntervalMax));
    }

    mAdvertiseTrickleTimer.IndicateInconsistent();

exit:
    return;
}

void MleRouter::SendAdvertisement(void)
{
    Error        error = kErrorNone;
    Ip6::Address destination;
    Message *    message = nullptr;

    // Suppress MLE Advertisements when trying to attach to a better partition.
    //
    // Without this suppression, a device may send an MLE Advertisement before receiving the MLE Child ID Response.
    // The candidate parent then removes the attaching device because the Source Address TLV includes an RLOC16 that
    // indicates a Router role (i.e. a Child ID equal to zero).
    VerifyOrExit(!IsAttaching());

    // Suppress MLE Advertisements when transitioning to the router role.
    //
    // When trying to attach to a new partition, sending out advertisements as a REED can cause already-attached
    // children to detach.
    VerifyOrExit(!mAddressSolicitPending);

    VerifyOrExit((message = NewMleMessage()) != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = AppendHeader(*message, kCommandAdvertisement));
    SuccessOrExit(error = AppendSourceAddress(*message));
    SuccessOrExit(error = AppendLeaderData(*message));

    switch (mRole)
    {
    case kRoleDisabled:
    case kRoleDetached:
        OT_ASSERT(false);
        OT_UNREACHABLE_CODE(break);

    case kRoleChild:
        break;

    case kRoleRouter:
    case kRoleLeader:
        SuccessOrExit(error = AppendRoute(*message));
        break;
    }

    destination.SetToLinkLocalAllNodesMulticast();
    SuccessOrExit(error = SendMessage(*message, destination));

    Log(kMessageSend, kTypeAdvertisement, destination);

exit:
    FreeMessageOnError(message, error);
    LogSendError(kTypeAdvertisement, error);
}

Error MleRouter::SendLinkRequest(Neighbor *aNeighbor)
{
    static const uint8_t detachedTlvs[]      = {Tlv::kAddress16, Tlv::kRoute};
    static const uint8_t routerTlvs[]        = {Tlv::kLinkMargin};
    static const uint8_t validNeighborTlvs[] = {Tlv::kLinkMargin, Tlv::kRoute};
    Error                error               = kErrorNone;
    Message *            message;
    Ip6::Address         destination;

    destination.Clear();

    VerifyOrExit((message = NewMleMessage()) != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = AppendHeader(*message, kCommandLinkRequest));
    SuccessOrExit(error = AppendVersion(*message));

    switch (mRole)
    {
    case kRoleDisabled:
        OT_ASSERT(false);
        OT_UNREACHABLE_CODE(break);

    case kRoleDetached:
        SuccessOrExit(error = AppendTlvRequest(*message, detachedTlvs, sizeof(detachedTlvs)));
        break;

    case kRoleChild:
        SuccessOrExit(error = AppendSourceAddress(*message));
        SuccessOrExit(error = AppendLeaderData(*message));
        break;

    case kRoleRouter:
    case kRoleLeader:
        if (aNeighbor == nullptr || !aNeighbor->IsStateValid())
        {
            SuccessOrExit(error = AppendTlvRequest(*message, routerTlvs, sizeof(routerTlvs)));
        }
        else
        {
            SuccessOrExit(error = AppendTlvRequest(*message, validNeighborTlvs, sizeof(validNeighborTlvs)));
        }

        SuccessOrExit(error = AppendSourceAddress(*message));
        SuccessOrExit(error = AppendLeaderData(*message));
        break;
    }

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    SuccessOrExit(error = AppendTimeRequest(*message));
#endif

    if (aNeighbor == nullptr)
    {
        mChallenge.GenerateRandom();
        mChallengeTimeout = (((2 * kMaxResponseDelay) + kStateUpdatePeriod - 1) / kStateUpdatePeriod);

        SuccessOrExit(error = AppendChallenge(*message, mChallenge));
        destination.SetToLinkLocalAllRoutersMulticast();
    }
    else
    {
        if (!aNeighbor->IsStateValid())
        {
            aNeighbor->GenerateChallenge();
            SuccessOrExit(error = AppendChallenge(*message, aNeighbor->GetChallenge(), aNeighbor->GetChallengeSize()));
        }
        else
        {
            Challenge challenge;

            challenge.GenerateRandom();
            SuccessOrExit(error = AppendChallenge(*message, challenge));
        }

        destination.SetToLinkLocalAddress(aNeighbor->GetExtAddress());
    }

    SuccessOrExit(error = SendMessage(*message, destination));

    Log(kMessageSend, kTypeLinkRequest, destination);

exit:
    FreeMessageOnError(message, error);
    return error;
}

void MleRouter::HandleLinkRequest(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, Neighbor *aNeighbor)
{
    Error         error    = kErrorNone;
    Neighbor *    neighbor = nullptr;
    Challenge     challenge;
    uint16_t      version;
    LeaderData    leaderData;
    uint16_t      sourceAddress;
    RequestedTlvs requestedTlvs;

    Log(kMessageReceive, kTypeLinkRequest, aMessageInfo.GetPeerAddr());

    VerifyOrExit(IsRouterOrLeader(), error = kErrorInvalidState);

    VerifyOrExit(!IsAttaching(), error = kErrorInvalidState);

    // Challenge
    SuccessOrExit(error = ReadChallenge(aMessage, challenge));

    // Version
    SuccessOrExit(error = Tlv::Find<VersionTlv>(aMessage, version));
    VerifyOrExit(version >= OT_THREAD_VERSION_1_1, error = kErrorParse);

    // Leader Data
    switch (ReadLeaderData(aMessage, leaderData))
    {
    case kErrorNone:
        VerifyOrExit(leaderData.GetPartitionId() == mLeaderData.GetPartitionId(), error = kErrorInvalidState);
        break;
    case kErrorNotFound:
        break;
    default:
        ExitNow(error = kErrorParse);
    }

    // Source Address
    switch (Tlv::Find<SourceAddressTlv>(aMessage, sourceAddress))
    {
    case kErrorNone:
        if (IsActiveRouter(sourceAddress))
        {
            Mac::ExtAddress extAddr;

            aMessageInfo.GetPeerAddr().GetIid().ConvertToExtAddress(extAddr);

            neighbor = mRouterTable.GetRouter(RouterIdFromRloc16(sourceAddress));
            VerifyOrExit(neighbor != nullptr, error = kErrorParse);
            VerifyOrExit(!neighbor->IsStateLinkRequest(), error = kErrorAlready);

            if (!neighbor->IsStateValid())
            {
                neighbor->SetExtAddress(extAddr);
                neighbor->GetLinkInfo().Clear();
                neighbor->GetLinkInfo().AddRss(aMessageInfo.GetThreadLinkInfo()->GetRss());
                neighbor->ResetLinkFailures();
                neighbor->SetLastHeard(TimerMilli::GetNow());
                neighbor->SetState(Neighbor::kStateLinkRequest);
            }
            else
            {
                VerifyOrExit(neighbor->GetExtAddress() == extAddr);
            }
        }

        break;

    case kErrorNotFound:
        // lack of source address indicates router coming out of reset
        VerifyOrExit(aNeighbor && aNeighbor->IsStateValid() && IsActiveRouter(aNeighbor->GetRloc16()),
                     error = kErrorDrop);
        neighbor = aNeighbor;
        break;

    default:
        ExitNow(error = kErrorParse);
    }

    // TLV Request
    switch (FindTlvRequest(aMessage, requestedTlvs))
    {
    case kErrorNone:
        break;
    case kErrorNotFound:
        requestedTlvs.mNumTlvs = 0;
        break;
    default:
        ExitNow(error = kErrorParse);
    }

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    if (neighbor != nullptr)
    {
        neighbor->SetTimeSyncEnabled(Tlv::Find<TimeRequestTlv>(aMessage, nullptr, 0) == kErrorNone);
    }
#endif

#if OPENTHREAD_CONFIG_MULTI_RADIO
    if (neighbor != nullptr)
    {
        neighbor->ClearLastRxFragmentTag();
    }
#endif

    SuccessOrExit(error = SendLinkAccept(aMessageInfo, neighbor, requestedTlvs, challenge));

exit:
    LogProcessError(kTypeLinkRequest, error);
}

Error MleRouter::SendLinkAccept(const Ip6::MessageInfo &aMessageInfo,
                                Neighbor *              aNeighbor,
                                const RequestedTlvs &   aRequestedTlvs,
                                const Challenge &       aChallenge)
{
    Error                error        = kErrorNone;
    static const uint8_t routerTlvs[] = {Tlv::kLinkMargin};
    Message *            message;
    Command              command;
    uint8_t              linkMargin;

    command = (aNeighbor == nullptr || aNeighbor->IsStateValid()) ? kCommandLinkAccept : kCommandLinkAcceptAndRequest;

    VerifyOrExit((message = NewMleMessage()) != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = AppendHeader(*message, command));
    SuccessOrExit(error = AppendVersion(*message));
    SuccessOrExit(error = AppendSourceAddress(*message));
    SuccessOrExit(error = AppendResponse(*message, aChallenge));
    SuccessOrExit(error = AppendLinkFrameCounter(*message));
    SuccessOrExit(error = AppendMleFrameCounter(*message));

    // always append a link margin, regardless of whether or not it was requested
    linkMargin = LinkQualityInfo::ConvertRssToLinkMargin(Get<Mac::Mac>().GetNoiseFloor(),
                                                         aMessageInfo.GetThreadLinkInfo()->GetRss());

    SuccessOrExit(error = AppendLinkMargin(*message, linkMargin));

    if (aNeighbor != nullptr && IsActiveRouter(aNeighbor->GetRloc16()))
    {
        SuccessOrExit(error = AppendLeaderData(*message));
    }

    for (uint8_t i = 0; i < aRequestedTlvs.mNumTlvs; i++)
    {
        switch (aRequestedTlvs.mTlvs[i])
        {
        case Tlv::kRoute:
            SuccessOrExit(error = AppendRoute(*message, aNeighbor));
            break;

        case Tlv::kAddress16:
            VerifyOrExit(aNeighbor != nullptr, error = kErrorDrop);
            SuccessOrExit(error = AppendAddress16(*message, aNeighbor->GetRloc16()));
            break;

        case Tlv::kLinkMargin:
            break;

        default:
            ExitNow(error = kErrorDrop);
        }
    }

    if (aNeighbor != nullptr && !aNeighbor->IsStateValid())
    {
        aNeighbor->GenerateChallenge();

        SuccessOrExit(error = AppendChallenge(*message, aNeighbor->GetChallenge(), aNeighbor->GetChallengeSize()));
        SuccessOrExit(error = AppendTlvRequest(*message, routerTlvs, sizeof(routerTlvs)));
        aNeighbor->SetLastHeard(TimerMilli::GetNow());
        aNeighbor->SetState(Neighbor::kStateLinkRequest);
    }

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    if (aNeighbor != nullptr && aNeighbor->IsTimeSyncEnabled())
    {
        message->SetTimeSync(true);
    }
#endif

    if (aMessageInfo.GetSockAddr().IsMulticast())
    {
        SuccessOrExit(error = AddDelayedResponse(*message, aMessageInfo.GetPeerAddr(),
                                                 1 + Random::NonCrypto::GetUint16InRange(0, kMaxResponseDelay)));

        Log(kMessageDelay, kTypeLinkAccept, aMessageInfo.GetPeerAddr());
    }
    else
    {
        SuccessOrExit(error = SendMessage(*message, aMessageInfo.GetPeerAddr()));

        Log(kMessageSend, kTypeLinkAccept, aMessageInfo.GetPeerAddr());
    }

exit:
    FreeMessageOnError(message, error);
    return error;
}

void MleRouter::HandleLinkAccept(const Message &         aMessage,
                                 const Ip6::MessageInfo &aMessageInfo,
                                 uint32_t                aKeySequence,
                                 Neighbor *              aNeighbor)
{
    Error error = HandleLinkAccept(aMessage, aMessageInfo, aKeySequence, aNeighbor, false);

    LogProcessError(kTypeLinkAccept, error);
}

void MleRouter::HandleLinkAcceptAndRequest(const Message &         aMessage,
                                           const Ip6::MessageInfo &aMessageInfo,
                                           uint32_t                aKeySequence,
                                           Neighbor *              aNeighbor)
{
    Error error = HandleLinkAccept(aMessage, aMessageInfo, aKeySequence, aNeighbor, true);

    LogProcessError(kTypeLinkAcceptAndRequest, error);
}

Error MleRouter::HandleLinkAccept(const Message &         aMessage,
                                  const Ip6::MessageInfo &aMessageInfo,
                                  uint32_t                aKeySequence,
                                  Neighbor *              aNeighbor,
                                  bool                    aRequest)
{
    static const uint8_t dataRequestTlvs[] = {Tlv::kNetworkData};

    Error           error = kErrorNone;
    Router *        router;
    Neighbor::State neighborState;
    Mac::ExtAddress extAddr;
    uint16_t        version;
    Challenge       response;
    uint16_t        sourceAddress;
    uint32_t        linkFrameCounter;
    uint32_t        mleFrameCounter;
    uint8_t         routerId;
    uint16_t        address16;
    RouteTlv        route;
    LeaderData      leaderData;
    uint8_t         linkMargin;

    // Source Address
    SuccessOrExit(error = Tlv::Find<SourceAddressTlv>(aMessage, sourceAddress));

    Log(kMessageReceive, aRequest ? kTypeLinkAcceptAndRequest : kTypeLinkAccept, aMessageInfo.GetPeerAddr(),
        sourceAddress);

    VerifyOrExit(IsActiveRouter(sourceAddress), error = kErrorParse);

    routerId      = RouterIdFromRloc16(sourceAddress);
    router        = mRouterTable.GetRouter(routerId);
    neighborState = (router != nullptr) ? router->GetState() : Neighbor::kStateInvalid;

    // Response
    SuccessOrExit(error = ReadResponse(aMessage, response));

    // verify response
    switch (neighborState)
    {
    case Neighbor::kStateLinkRequest:
        VerifyOrExit(response.Matches(router->GetChallenge(), router->GetChallengeSize()), error = kErrorSecurity);
        break;

    case Neighbor::kStateInvalid:
        VerifyOrExit((mChallengeTimeout > 0) && (response == mChallenge), error = kErrorSecurity);

        OT_FALL_THROUGH;

    case Neighbor::kStateValid:
        break;

    default:
        ExitNow(error = kErrorSecurity);
    }

    // Remove stale neighbors
    if (aNeighbor && aNeighbor->GetRloc16() != sourceAddress)
    {
        RemoveNeighbor(*aNeighbor);
    }

    // Version
    SuccessOrExit(error = Tlv::Find<VersionTlv>(aMessage, version));
    VerifyOrExit(version >= OT_THREAD_VERSION_1_1, error = kErrorParse);

    // Link and MLE Frame Counters
    SuccessOrExit(error = ReadFrameCounters(aMessage, linkFrameCounter, mleFrameCounter));

    // Link Margin
    switch (Tlv::Find<LinkMarginTlv>(aMessage, linkMargin))
    {
    case kErrorNone:
        break;
    case kErrorNotFound:
        // Link Margin TLV may be skipped in Router Synchronization process after Reset
        VerifyOrExit(IsDetached(), error = kErrorNotFound);
        // Wait for an MLE Advertisement to establish a routing cost to the neighbor
        linkMargin = 0;
        break;
    default:
        ExitNow(error = kErrorParse);
    }

    switch (mRole)
    {
    case kRoleDisabled:
        OT_ASSERT(false);
        OT_UNREACHABLE_CODE(break);

    case kRoleDetached:
        // Address16
        SuccessOrExit(error = Tlv::Find<Address16Tlv>(aMessage, address16));
        VerifyOrExit(GetRloc16() == address16, error = kErrorDrop);

        // Leader Data
        SuccessOrExit(error = ReadLeaderData(aMessage, leaderData));
        SetLeaderData(leaderData.GetPartitionId(), leaderData.GetWeighting(), leaderData.GetLeaderRouterId());

        // Route
        SuccessOrExit(error = Tlv::FindTlv(aMessage, Tlv::kRoute, sizeof(route), route));
        VerifyOrExit(route.IsValid(), error = kErrorParse);
        mRouterTable.Clear();
        SuccessOrExit(error = ProcessRouteTlv(route));
        router = mRouterTable.GetRouter(routerId);
        VerifyOrExit(router != nullptr);

        if (mLeaderData.GetLeaderRouterId() == RouterIdFromRloc16(GetRloc16()))
        {
            SetStateLeader(GetRloc16());
        }
        else
        {
            SetStateRouter(GetRloc16());
        }

        mRetrieveNewNetworkData = true;
        IgnoreError(SendDataRequest(aMessageInfo.GetPeerAddr(), dataRequestTlvs, sizeof(dataRequestTlvs), 0));

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
        Get<TimeSync>().HandleTimeSyncMessage(aMessage);
#endif
        break;

    case kRoleChild:
        VerifyOrExit(router != nullptr);
        break;

    case kRoleRouter:
    case kRoleLeader:
        VerifyOrExit(router != nullptr);

        // Leader Data
        SuccessOrExit(error = ReadLeaderData(aMessage, leaderData));
        VerifyOrExit(leaderData.GetPartitionId() == mLeaderData.GetPartitionId());

        if (mRetrieveNewNetworkData ||
            (static_cast<int8_t>(leaderData.GetDataVersion() - Get<NetworkData::Leader>().GetVersion()) > 0))
        {
            IgnoreError(SendDataRequest(aMessageInfo.GetPeerAddr(), dataRequestTlvs, sizeof(dataRequestTlvs), 0));
        }

        // Route (optional)
        if (Tlv::FindTlv(aMessage, route) == kErrorNone)
        {
            VerifyOrExit(route.IsValid(), error = kErrorParse);
            SuccessOrExit(error = ProcessRouteTlv(route));
            UpdateRoutes(route, routerId);
            // need to update router after ProcessRouteTlv
            router = mRouterTable.GetRouter(routerId);
            OT_ASSERT(router != nullptr);
        }

        // update routing table
        if (routerId != mRouterId && !IsRouterIdValid(router->GetNextHop()))
        {
            ResetAdvertiseInterval();
        }

        break;
    }

    // finish link synchronization
    aMessageInfo.GetPeerAddr().GetIid().ConvertToExtAddress(extAddr);
    router->SetExtAddress(extAddr);
    router->SetRloc16(sourceAddress);
    router->GetLinkFrameCounters().SetAll(linkFrameCounter);
    router->SetLinkAckFrameCounter(linkFrameCounter);
    router->SetMleFrameCounter(mleFrameCounter);
    router->SetLastHeard(TimerMilli::GetNow());
    router->SetVersion(static_cast<uint8_t>(version));
    router->SetDeviceMode(DeviceMode(DeviceMode::kModeFullThreadDevice | DeviceMode::kModeRxOnWhenIdle |
                                     DeviceMode::kModeFullNetworkData));
    router->GetLinkInfo().Clear();
    router->GetLinkInfo().AddRss(aMessageInfo.GetThreadLinkInfo()->GetRss());
    router->SetLinkQualityOut(LinkQualityInfo::ConvertLinkMarginToLinkQuality(linkMargin));
    router->ResetLinkFailures();
    router->SetState(Neighbor::kStateValid);
    router->SetKeySequence(aKeySequence);

    mNeighborTable.Signal(NeighborTable::kRouterAdded, *router);

    if (aRequest)
    {
        Challenge     challenge;
        RequestedTlvs requestedTlvs;

        // Challenge
        SuccessOrExit(error = ReadChallenge(aMessage, challenge));

        // TLV Request
        switch (FindTlvRequest(aMessage, requestedTlvs))
        {
        case kErrorNone:
            break;
        case kErrorNotFound:
            requestedTlvs.mNumTlvs = 0;
            break;
        default:
            ExitNow(error = kErrorParse);
        }

        SuccessOrExit(error = SendLinkAccept(aMessageInfo, router, requestedTlvs, challenge));
    }

exit:
    return error;
}

uint8_t MleRouter::LinkQualityToCost(uint8_t aLinkQuality)
{
    uint8_t rval;

    switch (aLinkQuality)
    {
    case 1:
        rval = kLinkQuality1LinkCost;
        break;

    case 2:
        rval = kLinkQuality2LinkCost;
        break;

    case 3:
        rval = kLinkQuality3LinkCost;
        break;

    default:
        rval = kLinkQuality0LinkCost;
        break;
    }

    return rval;
}

uint8_t MleRouter::GetLinkCost(uint8_t aRouterId)
{
    uint8_t rval = kMaxRouteCost;
    Router *router;

    router = mRouterTable.GetRouter(aRouterId);

    // nullptr aRouterId indicates non-existing next hop, hence return kMaxRouteCost for it.
    VerifyOrExit(router != nullptr);

    rval = mRouterTable.GetLinkCost(*router);

exit:
    return rval;
}

Error MleRouter::SetRouterSelectionJitter(uint8_t aRouterJitter)
{
    Error error = kErrorNone;

    VerifyOrExit(aRouterJitter > 0, error = kErrorInvalidArgs);

    mRouterSelectionJitter = aRouterJitter;

exit:
    return error;
}

Error MleRouter::ProcessRouteTlv(const RouteTlv &aRoute)
{
    Error error = kErrorNone;

    mRouterTable.UpdateRouterIdSet(aRoute.GetRouterIdSequence(), aRoute.GetRouterIdMask());

    if (IsRouter() && !mRouterTable.IsAllocated(mRouterId))
    {
        IgnoreError(BecomeDetached());
        error = kErrorNoRoute;
    }

    return error;
}

bool MleRouter::IsSingleton(void)
{
    bool rval = true;

    if (IsAttached() && IsRouterEligible())
    {
        // not a singleton if any other routers exist
        if (mRouterTable.GetActiveRouterCount() > 1)
        {
            ExitNow(rval = false);
        }
    }

exit:
    return rval;
}

int MleRouter::ComparePartitions(bool              aSingletonA,
                                 const LeaderData &aLeaderDataA,
                                 bool              aSingletonB,
                                 const LeaderData &aLeaderDataB)
{
    int rval = 0;

    if (aLeaderDataA.GetWeighting() != aLeaderDataB.GetWeighting())
    {
        ExitNow(rval = aLeaderDataA.GetWeighting() > aLeaderDataB.GetWeighting() ? 1 : -1);
    }

    if (aSingletonA != aSingletonB)
    {
        ExitNow(rval = aSingletonB ? 1 : -1);
    }

    if (aLeaderDataA.GetPartitionId() != aLeaderDataB.GetPartitionId())
    {
        ExitNow(rval = aLeaderDataA.GetPartitionId() > aLeaderDataB.GetPartitionId() ? 1 : -1);
    }

exit:
    return rval;
}

bool MleRouter::IsSingleton(const RouteTlv &aRouteTlv)
{
    bool    rval  = true;
    uint8_t count = 0;

    // REEDs do not include a Route TLV and indicate not a singleton
    if (!aRouteTlv.IsValid())
    {
        ExitNow(rval = false);
    }

    // Check if 2 or more active routers
    for (uint8_t routerId = 0; routerId <= kMaxRouterId; routerId++)
    {
        if (aRouteTlv.IsRouterIdSet(routerId) && (++count >= 2))
        {
            ExitNow(rval = false);
        }
    }

exit:
    return rval;
}

Error MleRouter::HandleAdvertisement(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, Neighbor *aNeighbor)
{
    Error                 error    = kErrorNone;
    const ThreadLinkInfo *linkInfo = aMessageInfo.GetThreadLinkInfo();
    uint8_t linkMargin = LinkQualityInfo::ConvertRssToLinkMargin(Get<Mac::Mac>().GetNoiseFloor(), linkInfo->GetRss());
    Mac::ExtAddress extAddr;
    uint16_t        sourceAddress = Mac::kShortAddrInvalid;
    LeaderData      leaderData;
    RouteTlv        route;
    uint32_t        partitionId;
    Router *        router;
    uint8_t         routerId;
    uint8_t         routerCount;

    aMessageInfo.GetPeerAddr().GetIid().ConvertToExtAddress(extAddr);

    // Source Address
    SuccessOrExit(error = Tlv::Find<SourceAddressTlv>(aMessage, sourceAddress));

    // Leader Data
    SuccessOrExit(error = ReadLeaderData(aMessage, leaderData));

    // Route Data (optional)
    if (Tlv::FindTlv(aMessage, route) == kErrorNone)
    {
        VerifyOrExit(route.IsValid(), error = kErrorParse);
    }
    else
    {
        // mark that a Route TLV was not included
        route.SetLength(0);
    }

    partitionId = leaderData.GetPartitionId();

    if (partitionId != mLeaderData.GetPartitionId())
    {
        otLogNoteMle("Different partition (peer:%u, local:%u)", partitionId, mLeaderData.GetPartitionId());

        VerifyOrExit(linkMargin >= OPENTHREAD_CONFIG_MLE_PARTITION_MERGE_MARGIN_MIN, error = kErrorLinkMarginLow);

        if (route.IsValid() && IsFullThreadDevice() && (mPreviousPartitionIdTimeout > 0) &&
            (partitionId == mPreviousPartitionId))
        {
            VerifyOrExit((static_cast<int8_t>(route.GetRouterIdSequence() - mPreviousPartitionRouterIdSequence) > 0),
                         error = kErrorDrop);
        }

        if (IsChild() && (aNeighbor == &mParent || !IsFullThreadDevice()))
        {
            ExitNow();
        }

        if (ComparePartitions(IsSingleton(route), leaderData, IsSingleton(), mLeaderData) > 0
#if OPENTHREAD_CONFIG_TIME_SYNC_REQUIRED
            // if time sync is required, it will only migrate to a better network which also enables time sync.
            && aMessage.GetTimeSyncSeq() != OT_TIME_SYNC_INVALID_SEQ
#endif
        )
        {
            IgnoreError(BecomeChild(kAttachBetter));
        }

        ExitNow(error = kErrorDrop);
    }
    else if (leaderData.GetLeaderRouterId() != GetLeaderId())
    {
        VerifyOrExit(aNeighbor && aNeighbor->IsStateValid());

        if (!IsChild())
        {
            otLogInfoMle("Leader ID mismatch");
            IgnoreError(BecomeDetached());
            error = kErrorDrop;
        }

        ExitNow();
    }

    VerifyOrExit(IsActiveRouter(sourceAddress) && route.IsValid());
    routerId = RouterIdFromRloc16(sourceAddress);

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    Get<TimeSync>().HandleTimeSyncMessage(aMessage);
#endif

    if (IsFullThreadDevice() && (aNeighbor && aNeighbor->IsStateValid()) &&
        ((mRouterTable.GetActiveRouterCount() == 0) ||
         (static_cast<int8_t>(route.GetRouterIdSequence() - mRouterTable.GetRouterIdSequence()) > 0)))
    {
        bool processRouteTlv = false;

        switch (mRole)
        {
        case kRoleDisabled:
        case kRoleDetached:
            break;

        case kRoleChild:
            if (sourceAddress == mParent.GetRloc16())
            {
                processRouteTlv = true;
            }
            else
            {
                router = mRouterTable.GetRouter(routerId);

                if (router != nullptr && router->IsStateValid())
                {
                    processRouteTlv = true;
                }
            }

            break;

        case kRoleRouter:
        case kRoleLeader:
            processRouteTlv = true;
            break;
        }

        if (processRouteTlv)
        {
            SuccessOrExit(error = ProcessRouteTlv(route));
            if (Get<RouterTable>().Contains(*aNeighbor))
            {
                aNeighbor = nullptr; // aNeighbor is no longer valid after `ProcessRouteTlv`
            }
        }
    }

    switch (mRole)
    {
    case kRoleDisabled:
    case kRoleDetached:
        ExitNow();

    case kRoleChild:
        if (aNeighbor == &mParent)
        {
            // MLE Advertisement from parent
            router = &mParent;

            if (mParent.GetRloc16() != sourceAddress)
            {
                IgnoreError(BecomeDetached());
                ExitNow(error = kErrorNoRoute);
            }

            if (IsFullThreadDevice())
            {
                Router *leader;

                if ((mRouterSelectionJitterTimeout == 0) &&
                    (mRouterTable.GetActiveRouterCount() < mRouterUpgradeThreshold))
                {
                    mRouterSelectionJitterTimeout = 1 + Random::NonCrypto::GetUint8InRange(0, mRouterSelectionJitter);
                    ExitNow();
                }

                leader = mRouterTable.GetLeader();

                if (leader != nullptr)
                {
                    for (uint8_t id = 0, routeCount = 0; id <= kMaxRouterId; id++)
                    {
                        if (!route.IsRouterIdSet(id))
                        {
                            continue;
                        }

                        if (id != GetLeaderId())
                        {
                            routeCount++;
                            continue;
                        }

                        if (route.GetRouteCost(routeCount) > 0)
                        {
                            leader->SetNextHop(id);
                            leader->SetCost(route.GetRouteCost(routeCount));
                        }
                        else
                        {
                            leader->SetNextHop(kInvalidRouterId);
                            leader->SetCost(0);
                        }

                        break;
                    }
                }
            }
        }
        else
        {
            // MLE Advertisement not from parent, but from some other neighboring router
            router = mRouterTable.GetRouter(routerId);
            VerifyOrExit(router != nullptr);

            if (IsFullThreadDevice() && !router->IsStateValid() && !router->IsStateLinkRequest() &&
                (mRouterTable.GetActiveLinkCount() < OPENTHREAD_CONFIG_MLE_CHILD_ROUTER_LINKS))
            {
                router->SetExtAddress(extAddr);
                router->GetLinkInfo().Clear();
                router->GetLinkInfo().AddRss(linkInfo->GetRss());
                router->ResetLinkFailures();
                router->SetLastHeard(TimerMilli::GetNow());
                router->SetState(Neighbor::kStateLinkRequest);
                IgnoreError(SendLinkRequest(router));
                ExitNow(error = kErrorNoRoute);
            }
        }

        router->SetLastHeard(TimerMilli::GetNow());

        ExitNow();

    case kRoleRouter:
        router = mRouterTable.GetRouter(routerId);
        VerifyOrExit(router != nullptr);

        // check current active router number
        routerCount = 0;

        for (uint8_t id = 0; id <= kMaxRouterId; id++)
        {
            if (route.IsRouterIdSet(id))
            {
                routerCount++;
            }
        }

        if (routerCount > mRouterDowngradeThreshold && mRouterSelectionJitterTimeout == 0 &&
            HasMinDowngradeNeighborRouters() && HasSmallNumberOfChildren() &&
            HasOneNeighborWithComparableConnectivity(route, routerId))
        {
            mRouterSelectionJitterTimeout = 1 + Random::NonCrypto::GetUint8InRange(0, mRouterSelectionJitter);
        }

        OT_FALL_THROUGH;

    case kRoleLeader:
        router = mRouterTable.GetRouter(routerId);
        VerifyOrExit(router != nullptr);

        // Send unicast link request if no link to router and no unicast/multicast link request in progress
        if (!router->IsStateValid() && !router->IsStateLinkRequest() && (mChallengeTimeout == 0) &&
            (linkMargin >= OPENTHREAD_CONFIG_MLE_LINK_REQUEST_MARGIN_MIN))
        {
            router->SetExtAddress(extAddr);
            router->GetLinkInfo().Clear();
            router->GetLinkInfo().AddRss(linkInfo->GetRss());
            router->ResetLinkFailures();
            router->SetLastHeard(TimerMilli::GetNow());
            router->SetState(Neighbor::kStateLinkRequest);
            IgnoreError(SendLinkRequest(router));
            ExitNow(error = kErrorNoRoute);
        }

        router->SetLastHeard(TimerMilli::GetNow());
        break;
    }

    UpdateRoutes(route, routerId);

exit:
    if (aNeighbor && aNeighbor->GetRloc16() != sourceAddress)
    {
        // Remove stale neighbors
        RemoveNeighbor(*aNeighbor);
    }

    return error;
}

void MleRouter::UpdateRoutes(const RouteTlv &aRoute, uint8_t aRouterId)
{
    Router *neighbor;
    bool    resetAdvInterval = false;
    bool    changed          = false;

    neighbor = mRouterTable.GetRouter(aRouterId);
    VerifyOrExit(neighbor != nullptr);

    // update link quality out to neighbor
    changed = UpdateLinkQualityOut(aRoute, *neighbor, resetAdvInterval);

    // update routes
    for (uint8_t routerId = 0, routeCount = 0; routerId <= kMaxRouterId; routerId++)
    {
        Router *router;
        Router *nextHop;
        uint8_t oldNextHop;
        uint8_t cost;

        if (!aRoute.IsRouterIdSet(routerId))
        {
            continue;
        }

        router = mRouterTable.GetRouter(routerId);

        if (router == nullptr || router->GetRloc16() == GetRloc16() || router == neighbor)
        {
            routeCount++;
            continue;
        }

        oldNextHop = router->GetNextHop();
        nextHop    = mRouterTable.GetRouter(oldNextHop);

        cost = aRoute.GetRouteCost(routeCount);

        if (cost == 0)
        {
            cost = kMaxRouteCost;
        }

        if (nextHop == nullptr || nextHop == neighbor)
        {
            // router has no next hop or next hop is neighbor (sender)

            if (cost + mRouterTable.GetLinkCost(*neighbor) < kMaxRouteCost)
            {
                if (nextHop == nullptr && mRouterTable.GetLinkCost(*router) >= kMaxRouteCost)
                {
                    resetAdvInterval = true;
                }

                if (router->GetNextHop() != aRouterId)
                {
                    router->SetNextHop(aRouterId);
                    changed = true;
                }

                if (router->GetCost() != cost)
                {
                    router->SetCost(cost);
                    changed = true;
                }
            }
            else if (nextHop == neighbor)
            {
                if (mRouterTable.GetLinkCost(*router) >= kMaxRouteCost)
                {
                    resetAdvInterval = true;
                }

                router->SetNextHop(kInvalidRouterId);
                router->SetCost(0);
                router->SetLastHeard(TimerMilli::GetNow());
                changed = true;
            }
        }
        else
        {
            uint8_t curCost = router->GetCost() + mRouterTable.GetLinkCost(*nextHop);
            uint8_t newCost = cost + mRouterTable.GetLinkCost(*neighbor);

            if (newCost < curCost)
            {
                router->SetNextHop(aRouterId);
                router->SetCost(cost);
                changed = true;
            }
        }

        routeCount++;
    }

    if (resetAdvInterval)
    {
        ResetAdvertiseInterval();
    }

#if (OPENTHREAD_CONFIG_LOG_MLE && (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO))

    VerifyOrExit(changed);
    otLogInfoMle("Route table updated");

    for (Router &router : Get<RouterTable>().Iterate())
    {
        otLogInfoMle("    %04x -> %04x, cost:%d %d, lqin:%d, lqout:%d, link:%s", router.GetRloc16(),
                     (router.GetNextHop() == kInvalidRouterId) ? 0xffff : Rloc16FromRouterId(router.GetNextHop()),
                     router.GetCost(), mRouterTable.GetLinkCost(router), router.GetLinkInfo().GetLinkQuality(),
                     router.GetLinkQualityOut(),
                     router.GetRloc16() == GetRloc16() ? "device" : (router.IsStateValid() ? "yes" : "no"));
    }

#else
    OT_UNUSED_VARIABLE(changed);
#endif

exit:
    return;
}

bool MleRouter::UpdateLinkQualityOut(const RouteTlv &aRoute, Router &aNeighbor, bool &aResetAdvInterval)
{
    bool    changed = false;
    uint8_t linkQuality;
    uint8_t myRouterId;
    uint8_t myRouteCount;
    uint8_t oldLinkCost;
    Router *nextHop;

    myRouterId = RouterIdFromRloc16(GetRloc16());
    VerifyOrExit(aRoute.IsRouterIdSet(myRouterId));

    myRouteCount = 0;
    for (uint8_t routerId = 0; routerId < myRouterId; routerId++)
    {
        myRouteCount += aRoute.IsRouterIdSet(routerId);
    }

    linkQuality = aRoute.GetLinkQualityIn(myRouteCount);
    VerifyOrExit(aNeighbor.GetLinkQualityOut() != linkQuality);

    oldLinkCost = mRouterTable.GetLinkCost(aNeighbor);

    aNeighbor.SetLinkQualityOut(linkQuality);
    nextHop = mRouterTable.GetRouter(aNeighbor.GetNextHop());

    // reset MLE advertisement timer if neighbor route cost changed to or from infinite
    if (nextHop == nullptr && (oldLinkCost >= kMaxRouteCost) != (mRouterTable.GetLinkCost(aNeighbor) >= kMaxRouteCost))
    {
        aResetAdvInterval = true;
    }
    changed = true;

exit:
    return changed;
}

void MleRouter::HandleParentRequest(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    Error           error = kErrorNone;
    Mac::ExtAddress extAddr;
    uint16_t        version;
    uint8_t         scanMask;
    Challenge       challenge;
    Router *        leader;
    Child *         child;

    Log(kMessageReceive, kTypeParentRequest, aMessageInfo.GetPeerAddr());

    VerifyOrExit(IsRouterEligible(), error = kErrorInvalidState);

    // A Router/REED MUST NOT send an MLE Parent Response if:

    // 0. It is detached or attempting to another partition
    VerifyOrExit(!IsDetached() && !IsAttaching(), error = kErrorDrop);

    // 1. It has no available Child capacity (if Max Child Count minus
    // Child Count would be equal to zero)
    // ==> verified below when allocating a child entry

    // 2. It is disconnected from its Partition (that is, it has not
    // received an updated ID sequence number within LEADER_TIMEOUT
    // seconds)
    VerifyOrExit(mRouterTable.GetLeaderAge() < mNetworkIdTimeout, error = kErrorDrop);

    // 3. Its current routing path cost to the Leader is infinite.
    leader = mRouterTable.GetLeader();
    OT_ASSERT(leader != nullptr);

    VerifyOrExit(IsLeader() || GetLinkCost(GetLeaderId()) < kMaxRouteCost ||
                     (IsChild() && leader->GetCost() + 1 < kMaxRouteCost) ||
                     (leader->GetCost() + GetLinkCost(leader->GetNextHop()) < kMaxRouteCost),
                 error = kErrorDrop);

    // 4. It is a REED and there are already `kMaxRouters` active routers in
    // the network (because Leader would reject any further address solicit).
    // ==> Verified below when checking the scan mask.

    aMessageInfo.GetPeerAddr().GetIid().ConvertToExtAddress(extAddr);

    // Version
    SuccessOrExit(error = Tlv::Find<VersionTlv>(aMessage, version));
    VerifyOrExit(version >= OT_THREAD_VERSION_1_1, error = kErrorParse);

    // Scan Mask
    SuccessOrExit(error = Tlv::Find<ScanMaskTlv>(aMessage, scanMask));

    switch (mRole)
    {
    case kRoleDisabled:
    case kRoleDetached:
        ExitNow();

    case kRoleChild:
        VerifyOrExit(ScanMaskTlv::IsEndDeviceFlagSet(scanMask));
        VerifyOrExit(mRouterTable.GetActiveRouterCount() < kMaxRouters, error = kErrorDrop);
        break;

    case kRoleRouter:
    case kRoleLeader:
        VerifyOrExit(ScanMaskTlv::IsRouterFlagSet(scanMask));
        break;
    }

    // Challenge
    SuccessOrExit(error = ReadChallenge(aMessage, challenge));

    child = mChildTable.FindChild(extAddr, Child::kInStateAnyExceptInvalid);

    if (child == nullptr)
    {
        VerifyOrExit((child = mChildTable.GetNewChild()) != nullptr, error = kErrorNoBufs);

        // MAC Address
        child->SetExtAddress(extAddr);
        child->GetLinkInfo().Clear();
        child->GetLinkInfo().AddRss(aMessageInfo.GetThreadLinkInfo()->GetRss());
        child->ResetLinkFailures();
        child->SetState(Neighbor::kStateParentRequest);
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
        child->SetTimeSyncEnabled(Tlv::Find<TimeRequestTlv>(aMessage, nullptr, 0) == kErrorNone);
#endif
    }
    else if (TimerMilli::GetNow() - child->GetLastHeard() < kParentRequestRouterTimeout - kParentRequestDuplicateMargin)
    {
        ExitNow(error = kErrorDuplicated);
    }

    if (!child->IsStateValidOrRestoring())
    {
        child->SetLastHeard(TimerMilli::GetNow());
        child->SetTimeout(Time::MsecToSec(kMaxChildIdRequestTimeout));
    }

    SendParentResponse(child, challenge, !ScanMaskTlv::IsEndDeviceFlagSet(scanMask));

exit:
    LogProcessError(kTypeParentRequest, error);
}

void MleRouter::HandleTimeTick(void)
{
    bool routerStateUpdate = false;

    VerifyOrExit(IsFullThreadDevice(), Get<TimeTicker>().UnregisterReceiver(TimeTicker::kMleRouter));

    if (mChallengeTimeout > 0)
    {
        mChallengeTimeout--;
    }

    if (mPreviousPartitionIdTimeout > 0)
    {
        mPreviousPartitionIdTimeout--;
    }

    if (mRouterSelectionJitterTimeout > 0)
    {
        mRouterSelectionJitterTimeout--;

        if (mRouterSelectionJitterTimeout == 0)
        {
            routerStateUpdate = true;
        }
    }
#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    // Delay register only when `mRouterSelectionJitterTimeout` is 0,
    // that is, when the device has decided to stay as REED or Router.
    else if (mBackboneRouterRegistrationDelay > 0)
    {
        mBackboneRouterRegistrationDelay--;

        if (mBackboneRouterRegistrationDelay == 0)
        {
            // If no Backbone Router service after jitter, try to register its own Backbone Router Service.
            if (!Get<BackboneRouter::Leader>().HasPrimary())
            {
                if (Get<BackboneRouter::Local>().AddService() == kErrorNone)
                {
                    Get<NetworkData::Notifier>().HandleServerDataUpdated();
                }
            }
        }
    }
#endif

    switch (mRole)
    {
    case kRoleDisabled:
        OT_ASSERT(false);
        OT_UNREACHABLE_CODE(break);

    case kRoleDetached:
        if (mChallengeTimeout == 0)
        {
            IgnoreError(BecomeDetached());
            ExitNow();
        }

        break;

    case kRoleChild:
        if (routerStateUpdate)
        {
            if (mRouterTable.GetActiveRouterCount() < mRouterUpgradeThreshold)
            {
                // upgrade to Router
                IgnoreError(BecomeRouter(ThreadStatusTlv::kTooFewRouters));
            }
            else
            {
                // send announce after decided to stay in REED if needed
                InformPreviousChannel();
            }

            if (!mAdvertiseTrickleTimer.IsRunning())
            {
                SendAdvertisement();

                mAdvertiseTrickleTimer.Start(TrickleTimer::kModePlainTimer, Time::SecToMsec(kReedAdvertiseInterval),
                                             Time::SecToMsec(kReedAdvertiseInterval + kReedAdvertiseJitter));
            }

            ExitNow();
        }

        OT_FALL_THROUGH;

    case kRoleRouter:
        // verify path to leader
        otLogDebgMle("network id timeout = %d", mRouterTable.GetLeaderAge());

        if ((mRouterTable.GetActiveRouterCount() > 0) && (mRouterTable.GetLeaderAge() >= mNetworkIdTimeout))
        {
            otLogInfoMle("Router ID Sequence timeout");
            IgnoreError(BecomeChild(kAttachSame1));
        }

        if (routerStateUpdate && mRouterTable.GetActiveRouterCount() > mRouterDowngradeThreshold)
        {
            // downgrade to REED
            otLogNoteMle("Downgrade to REED");
            IgnoreError(BecomeChild(kAttachSameDowngrade));
        }

        break;

    case kRoleLeader:
        break;
    }

    // update children state
    for (Child &child : Get<ChildTable>().Iterate(Child::kInStateAnyExceptInvalid))
    {
        uint32_t timeout = 0;

        switch (child.GetState())
        {
        case Neighbor::kStateInvalid:
        case Neighbor::kStateChildIdRequest:
            continue;

        case Neighbor::kStateParentRequest:
        case Neighbor::kStateValid:
        case Neighbor::kStateRestored:
        case Neighbor::kStateChildUpdateRequest:
            timeout = Time::SecToMsec(child.GetTimeout());
            break;

        case Neighbor::kStateParentResponse:
        case Neighbor::kStateLinkRequest:
            OT_ASSERT(false);
            OT_UNREACHABLE_CODE(break);
        }

#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
        if (child.IsCslSynchronized() &&
            TimerMilli::GetNow() - child.GetCslLastHeard() >= Time::SecToMsec(child.GetCslTimeout()))
        {
            otLogInfoMle("Child CSL synchronization expired");
            child.SetCslSynchronized(false);
            Get<CslTxScheduler>().Update();
        }
#endif

        if (TimerMilli::GetNow() - child.GetLastHeard() >= timeout)
        {
            otLogInfoMle("Child timeout expired");
            RemoveNeighbor(child);
        }
        else if (IsRouterOrLeader() && child.IsStateRestored())
        {
            IgnoreError(SendChildUpdateRequest(child));
        }
    }

    // update router state
    for (Router &router : Get<RouterTable>().Iterate())
    {
        uint32_t age;

        if (router.GetRloc16() == GetRloc16())
        {
            router.SetLastHeard(TimerMilli::GetNow());
            continue;
        }

        age = TimerMilli::GetNow() - router.GetLastHeard();

        if (router.IsStateValid())
        {
#if OPENTHREAD_CONFIG_MLE_SEND_LINK_REQUEST_ON_ADV_TIMEOUT == 0

            if (age >= Time::SecToMsec(kMaxNeighborAge))
            {
                otLogInfoMle("Router timeout expired");
                RemoveNeighbor(router);
                continue;
            }

#else

            if (age >= Time::SecToMsec(kMaxNeighborAge))
            {
                if (age < Time::SecToMsec(kMaxNeighborAge) + kMaxTransmissionCount * kUnicastRetransmissionDelay)
                {
                    otLogInfoMle("Router timeout expired");
                    IgnoreError(SendLinkRequest(&router));
                }
                else
                {
                    RemoveNeighbor(router);
                    continue;
                }
            }

#endif
        }
        else if (router.IsStateLinkRequest())
        {
            if (age >= kMaxLinkRequestTimeout)
            {
                otLogInfoMle("Link Request timeout expired");
                RemoveNeighbor(router);
                continue;
            }
        }

        if (IsLeader())
        {
            if (mRouterTable.GetRouter(router.GetNextHop()) == nullptr &&
                mRouterTable.GetLinkCost(router) >= kMaxRouteCost && age >= Time::SecToMsec(kMaxLeaderToRouterTimeout))
            {
                otLogInfoMle("Router ID timeout expired (no route)");
                IgnoreError(mRouterTable.Release(router.GetRouterId()));
            }
        }
    }

    mRouterTable.HandleTimeTick();

    SynchronizeChildNetworkData();

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    if (IsRouterOrLeader())
    {
        Get<TimeSync>().ProcessTimeSync();
    }
#endif

exit:
    return;
}

void MleRouter::SendParentResponse(Child *aChild, const Challenge &aChallenge, bool aRoutersOnlyRequest)
{
    Error        error = kErrorNone;
    Ip6::Address destination;
    Message *    message;
    uint16_t     delay;

    VerifyOrExit((message = NewMleMessage()) != nullptr, error = kErrorNoBufs);
    message->SetDirectTransmission();

    SuccessOrExit(error = AppendHeader(*message, kCommandParentResponse));
    SuccessOrExit(error = AppendSourceAddress(*message));
    SuccessOrExit(error = AppendLeaderData(*message));
    SuccessOrExit(error = AppendLinkFrameCounter(*message));
    SuccessOrExit(error = AppendMleFrameCounter(*message));
    SuccessOrExit(error = AppendResponse(*message, aChallenge));
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    if (aChild->IsTimeSyncEnabled())
    {
        SuccessOrExit(error = AppendTimeParameter(*message));
    }
#endif
#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    if (!aChild->IsRxOnWhenIdle())
    {
        SuccessOrExit(error = AppendCslClockAccuracy(*message));
    }
#endif

    aChild->GenerateChallenge();

    SuccessOrExit(error = AppendChallenge(*message, aChild->GetChallenge(), aChild->GetChallengeSize()));
    error = AppendLinkMargin(*message, aChild->GetLinkInfo().GetLinkMargin());
    SuccessOrExit(error);

    SuccessOrExit(error = AppendConnectivity(*message));
    SuccessOrExit(error = AppendVersion(*message));

    destination.SetToLinkLocalAddress(aChild->GetExtAddress());

    if (aRoutersOnlyRequest)
    {
        delay = 1 + Random::NonCrypto::GetUint16InRange(0, kParentResponseMaxDelayRouters);
    }
    else
    {
        delay = 1 + Random::NonCrypto::GetUint16InRange(0, kParentResponseMaxDelayAll);
    }

    SuccessOrExit(error = AddDelayedResponse(*message, destination, delay));

    Log(kMessageDelay, kTypeParentResponse, destination);

exit:
    FreeMessageOnError(message, error);
    LogSendError(kTypeParentResponse, error);
}

uint8_t MleRouter::GetMaxChildIpAddresses(void) const
{
    uint8_t num = OPENTHREAD_CONFIG_MLE_IP_ADDRS_PER_CHILD;

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    if (mMaxChildIpAddresses != 0)
    {
        num = mMaxChildIpAddresses;
    }
#endif

    return num;
}

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
Error MleRouter::SetMaxChildIpAddresses(uint8_t aMaxIpAddresses)
{
    Error error = kErrorNone;

    VerifyOrExit(aMaxIpAddresses <= OPENTHREAD_CONFIG_MLE_IP_ADDRS_PER_CHILD, error = kErrorInvalidArgs);

    mMaxChildIpAddresses = aMaxIpAddresses;

exit:
    return error;
}
#endif

Error MleRouter::UpdateChildAddresses(const Message &aMessage, uint16_t aOffset, Child &aChild)
{
    Error                    error = kErrorNone;
    AddressRegistrationEntry entry;
    Ip6::Address             address;
    Lowpan::Context          context;
    Tlv                      tlv;
    uint8_t                  registeredCount = 0;
    uint8_t                  storedCount     = 0;
    uint16_t                 offset          = 0;
    uint16_t                 end             = 0;
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_DUA_ENABLE
    Ip6::Address        oldDua;
    const Ip6::Address *oldDuaPtr = nullptr;
    bool                hasDua    = false;
#endif

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE
    Ip6::Address oldMlrRegisteredAddresses[OPENTHREAD_CONFIG_MLE_IP_ADDRS_PER_CHILD - 1];
    uint16_t     oldMlrRegisteredAddressNum = 0;
#endif

    SuccessOrExit(error = aMessage.Read(aOffset, tlv));
    VerifyOrExit(tlv.GetLength() <= (aMessage.GetLength() - aOffset - sizeof(tlv)), error = kErrorParse);

    offset = aOffset + sizeof(tlv);
    end    = offset + tlv.GetLength();

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_DUA_ENABLE
    if ((oldDuaPtr = aChild.GetDomainUnicastAddress()) != nullptr)
    {
        oldDua = *oldDuaPtr;
    }
#endif

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE
    // Retrieve registered multicast addresses of the Child
    if (aChild.HasAnyMlrRegisteredAddress())
    {
        OT_ASSERT(aChild.IsStateValid());

        for (const Ip6::Address &childAddress :
             aChild.IterateIp6Addresses(Ip6::Address::kTypeMulticastLargerThanRealmLocal))
        {
            if (aChild.GetAddressMlrState(childAddress) == kMlrStateRegistered)
            {
                oldMlrRegisteredAddresses[oldMlrRegisteredAddressNum++] = childAddress;
            }
        }
    }
#endif

    aChild.ClearIp6Addresses();

    while (offset < end)
    {
        uint8_t len;

        // read out the control field
        SuccessOrExit(error = aMessage.Read(offset, &entry, sizeof(uint8_t)));

        len = entry.GetLength();

        SuccessOrExit(error = aMessage.Read(offset, &entry, len));

        offset += len;
        registeredCount++;

        if (entry.IsCompressed())
        {
            if (Get<NetworkData::Leader>().GetContext(entry.GetContextId(), context) != kErrorNone)
            {
                otLogWarnMle("Failed to get context %d for compressed address from child 0x%04x", entry.GetContextId(),
                             aChild.GetRloc16());
                continue;
            }

            address.Clear();
            address.SetPrefix(context.mPrefix);
            address.SetIid(entry.GetIid());

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_DUA_ENABLE
            if (Get<BackboneRouter::Leader>().IsDomainUnicast(address))
            {
                hasDua = true;

                if (oldDuaPtr != nullptr)
                {
                    Get<DuaManager>().UpdateChildDomainUnicastAddress(
                        aChild, oldDua != address ? ChildDuaState::kChanged : ChildDuaState::kUnchanged);
                }
                else
                {
                    Get<DuaManager>().UpdateChildDomainUnicastAddress(aChild, ChildDuaState::kAdded);
                }
            }
#endif
        }
        else
        {
            address = entry.GetIp6Address();
        }

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
        if (mMaxChildIpAddresses > 0 && storedCount >= mMaxChildIpAddresses)
        {
            // Skip remaining address registration entries but keep logging skipped addresses.
            error = kErrorNoBufs;
        }
        else
#endif
        {
            // We try to accept/add as many IPv6 addresses as possible.
            // "Child ID/Update Response" will indicate the accepted
            // addresses.
            error = aChild.AddIp6Address(address);
        }

        if (error == kErrorNone)
        {
            storedCount++;
            otLogInfoMle("Child 0x%04x IPv6 address[%d]=%s", aChild.GetRloc16(), storedCount,
                         address.ToString().AsCString());
        }
        else
        {
            otLogWarnMle("Error %s adding IPv6 address %s to child 0x%04x", ErrorToString(error),
                         address.ToString().AsCString(), aChild.GetRloc16());
        }

        if (address.IsMulticast())
        {
            continue;
        }

        // We check if the same address is in-use by another child, if so
        // remove it. This implements "last-in wins" duplicate address
        // resolution policy.
        //
        // Duplicate addresses can occur if a previously attached child
        // attaches to same parent again (after a reset, memory wipe) using
        // a new random extended address before the old entry in the child
        // table is timed out and then trying to register its globally unique
        // IPv6 address as the new child.

        for (Child &child : Get<ChildTable>().Iterate(Child::kInStateValidOrRestoring))
        {
            if (&child == &aChild)
            {
                continue;
            }

            IgnoreError(child.RemoveIp6Address(address));
        }

        // Clear EID-to-RLOC cache for the unicast address registered by the child.
        Get<AddressResolver>().Remove(address);
    }
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_DUA_ENABLE
    // Dua is removed
    if (oldDuaPtr != nullptr && !hasDua)
    {
        Get<DuaManager>().UpdateChildDomainUnicastAddress(aChild, ChildDuaState::kRemoved);
    }
#endif

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE
    Get<MlrManager>().UpdateProxiedSubscriptions(aChild, oldMlrRegisteredAddresses, oldMlrRegisteredAddressNum);
#endif

    if (registeredCount == 0)
    {
        otLogInfoMle("Child 0x%04x has no registered IPv6 address", aChild.GetRloc16());
    }
    else
    {
        otLogInfoMle("Child 0x%04x has %d registered IPv6 address%s, %d address%s stored", aChild.GetRloc16(),
                     registeredCount, (registeredCount == 1) ? "" : "es", storedCount, (storedCount == 1) ? "" : "es");
    }

    error = kErrorNone;

exit:
    return error;
}

void MleRouter::HandleChildIdRequest(const Message &         aMessage,
                                     const Ip6::MessageInfo &aMessageInfo,
                                     uint32_t                aKeySequence)
{
    Error              error = kErrorNone;
    Mac::ExtAddress    extAddr;
    uint16_t           version;
    Challenge          response;
    uint32_t           linkFrameCounter;
    uint32_t           mleFrameCounter;
    uint8_t            modeBitmask;
    DeviceMode         mode;
    uint32_t           timeout;
    RequestedTlvs      requestedTlvs;
    MeshCoP::Timestamp timestamp;
    bool               needsActiveDatasetTlv;
    bool               needsPendingDatasetTlv;
    Child *            child;
    Router *           router;
    uint8_t            numTlvs;
    uint16_t           addressRegistrationOffset = 0;

    Log(kMessageReceive, kTypeChildIdRequest, aMessageInfo.GetPeerAddr());

    VerifyOrExit(IsRouterEligible(), error = kErrorInvalidState);

    // only process message when operating as a child, router, or leader
    VerifyOrExit(IsAttached(), error = kErrorInvalidState);

    // Find Child
    aMessageInfo.GetPeerAddr().GetIid().ConvertToExtAddress(extAddr);

    child = mChildTable.FindChild(extAddr, Child::kInStateAnyExceptInvalid);
    VerifyOrExit(child != nullptr, error = kErrorAlready);

    // Version
    SuccessOrExit(error = Tlv::Find<VersionTlv>(aMessage, version));
    VerifyOrExit(version >= OT_THREAD_VERSION_1_1, error = kErrorParse);

    // Response
    SuccessOrExit(error = ReadResponse(aMessage, response));
    VerifyOrExit(response.Matches(child->GetChallenge(), child->GetChallengeSize()), error = kErrorSecurity);

    // Remove existing MLE messages
    Get<MeshForwarder>().RemoveMessages(*child, Message::kSubTypeMleGeneral);
    Get<MeshForwarder>().RemoveMessages(*child, Message::kSubTypeMleChildIdRequest);
    Get<MeshForwarder>().RemoveMessages(*child, Message::kSubTypeMleChildUpdateRequest);
    Get<MeshForwarder>().RemoveMessages(*child, Message::kSubTypeMleDataResponse);

    // Link-Layer and MLE Frame Counters
    SuccessOrExit(error = ReadFrameCounters(aMessage, linkFrameCounter, mleFrameCounter));

    // Mode
    SuccessOrExit(error = Tlv::Find<ModeTlv>(aMessage, modeBitmask));
    mode.Set(modeBitmask);

    // Timeout
    SuccessOrExit(error = Tlv::Find<TimeoutTlv>(aMessage, timeout));

    // TLV Request
    SuccessOrExit(error = FindTlvRequest(aMessage, requestedTlvs));
    VerifyOrExit(requestedTlvs.mNumTlvs <= Child::kMaxRequestTlvs, error = kErrorParse);

    // Active Timestamp
    needsActiveDatasetTlv = true;
    switch (Tlv::Find<ActiveTimestampTlv>(aMessage, timestamp))
    {
    case kErrorNone:
        needsActiveDatasetTlv = (Get<MeshCoP::ActiveDataset>().Compare(timestamp) != 0);
        break;
    case kErrorNotFound:
        break;
    default:
        ExitNow(error = kErrorParse);
    }

    // Pending Timestamp
    needsPendingDatasetTlv = true;
    switch (Tlv::Find<PendingTimestampTlv>(aMessage, timestamp))
    {
    case kErrorNone:
        needsPendingDatasetTlv = (Get<MeshCoP::PendingDataset>().Compare(timestamp) != 0);
        break;
    case kErrorNotFound:
        break;
    default:
        ExitNow(error = kErrorParse);
    }

    if (!mode.IsFullThreadDevice())
    {
        SuccessOrExit(error = Tlv::FindTlvOffset(aMessage, Tlv::kAddressRegistration, addressRegistrationOffset));
        SuccessOrExit(error = UpdateChildAddresses(aMessage, addressRegistrationOffset, *child));
    }

    // Remove from router table
    router = mRouterTable.GetRouter(extAddr);
    if (router != nullptr)
    {
        // The `router` here can be invalid
        RemoveNeighbor(*router);
    }

    if (!child->IsStateValid())
    {
        child->SetState(Neighbor::kStateChildIdRequest);
    }
    else
    {
        RemoveNeighbor(*child);
    }

    child->SetLastHeard(TimerMilli::GetNow());
    child->GetLinkFrameCounters().SetAll(linkFrameCounter);
    child->SetLinkAckFrameCounter(linkFrameCounter);
    child->SetMleFrameCounter(mleFrameCounter);
    child->SetKeySequence(aKeySequence);
    child->SetDeviceMode(mode);
    child->SetVersion(static_cast<uint8_t>(version));
    child->GetLinkInfo().AddRss(aMessageInfo.GetThreadLinkInfo()->GetRss());
    child->SetTimeout(timeout);
#if OPENTHREAD_CONFIG_MULTI_RADIO
    child->ClearLastRxFragmentTag();
#endif

    if (mode.IsFullNetworkData())
    {
        child->SetNetworkDataVersion(mLeaderData.GetDataVersion());
    }
    else
    {
        child->SetNetworkDataVersion(mLeaderData.GetStableDataVersion());
    }

    child->ClearRequestTlvs();

    for (numTlvs = 0; numTlvs < requestedTlvs.mNumTlvs; numTlvs++)
    {
        child->SetRequestTlv(numTlvs, requestedTlvs.mTlvs[numTlvs]);
    }

    if (needsActiveDatasetTlv)
    {
        child->SetRequestTlv(numTlvs++, Tlv::kActiveDataset);
    }

    if (needsPendingDatasetTlv)
    {
        child->SetRequestTlv(numTlvs++, Tlv::kPendingDataset);
    }

    switch (mRole)
    {
    case kRoleDisabled:
    case kRoleDetached:
        OT_ASSERT(false);
        OT_UNREACHABLE_CODE(break);

    case kRoleChild:
        child->SetState(Neighbor::kStateChildIdRequest);
        IgnoreError(BecomeRouter(ThreadStatusTlv::kHaveChildIdRequest));
        break;

    case kRoleRouter:
    case kRoleLeader:
        SuccessOrExit(error = SendChildIdResponse(*child));
        break;
    }

exit:
    LogProcessError(kTypeChildIdRequest, error);
}

void MleRouter::HandleChildUpdateRequest(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    static const uint8_t kMaxResponseTlvs = 10;

    Error           error = kErrorNone;
    Mac::ExtAddress extAddr;
    uint8_t         modeBitmask;
    DeviceMode      mode;
    Challenge       challenge;
    LeaderData      leaderData;
    uint32_t        timeout;
    Child *         child;
    DeviceMode      oldMode;
    RequestedTlvs   requestedTlvs;
    uint8_t         tlvs[kMaxResponseTlvs];
    uint8_t         tlvslength                = 0;
    uint16_t        addressRegistrationOffset = 0;
    bool            childDidChange            = false;

    Log(kMessageReceive, kTypeChildUpdateRequestOfChild, aMessageInfo.GetPeerAddr());

    // Mode
    SuccessOrExit(error = Tlv::Find<ModeTlv>(aMessage, modeBitmask));
    mode.Set(modeBitmask);

    // Challenge
    switch (ReadChallenge(aMessage, challenge))
    {
    case kErrorNone:
        tlvs[tlvslength++] = Tlv::kResponse;
        break;
    case kErrorNotFound:
        challenge.mLength = 0;
        break;
    default:
        ExitNow(error = kErrorParse);
    }

    tlvs[tlvslength++] = Tlv::kSourceAddress;

    aMessageInfo.GetPeerAddr().GetIid().ConvertToExtAddress(extAddr);
    child = mChildTable.FindChild(extAddr, Child::kInStateAnyExceptInvalid);

    if (child == nullptr)
    {
        // For invalid non-sleepy child, send Child Update Response with
        // Status TLV (error).
        if (mode.IsRxOnWhenIdle())
        {
            tlvs[tlvslength++] = Tlv::kStatus;
            SendChildUpdateResponse(nullptr, aMessageInfo, tlvs, tlvslength, challenge);
        }

        ExitNow();
    }

    // Ignore "Child Update Request" from a child that is present in the
    // child table but it is not yet in valid state. For example, a
    // child which is being restored (due to parent reset) or is in the
    // middle of the attach process (in `kStateParentRequest` or
    // `kStateChildIdRequest`).

    VerifyOrExit(child->IsStateValid());

    oldMode = child->GetDeviceMode();
    child->SetDeviceMode(mode);

    tlvs[tlvslength++] = Tlv::kMode;

    // Parent MUST include Leader Data TLV in Child Update Response
    tlvs[tlvslength++] = Tlv::kLeaderData;

    if (challenge.mLength != 0)
    {
        tlvs[tlvslength++] = Tlv::kMleFrameCounter;
        tlvs[tlvslength++] = Tlv::kLinkFrameCounter;
    }

    // IPv6 Address TLV
    if (Tlv::FindTlvOffset(aMessage, Tlv::kAddressRegistration, addressRegistrationOffset) == kErrorNone)
    {
        SuccessOrExit(error = UpdateChildAddresses(aMessage, addressRegistrationOffset, *child));
        tlvs[tlvslength++] = Tlv::kAddressRegistration;
    }

    // Leader Data
    switch (ReadLeaderData(aMessage, leaderData))
    {
    case kErrorNone:
    case kErrorNotFound:
        break;
    default:
        ExitNow(error = kErrorParse);
    }

    // Timeout
    switch (Tlv::Find<TimeoutTlv>(aMessage, timeout))
    {
    case kErrorNone:
        if (child->GetTimeout() != timeout)
        {
            child->SetTimeout(timeout);
            childDidChange = true;
        }

        tlvs[tlvslength++] = Tlv::kTimeout;
        break;

    case kErrorNotFound:
        break;

    default:
        ExitNow(error = kErrorParse);
    }

    // TLV Request
    switch (FindTlvRequest(aMessage, requestedTlvs))
    {
    case kErrorNone:
        VerifyOrExit(requestedTlvs.mNumTlvs <= (kMaxResponseTlvs - tlvslength), error = kErrorParse);
        for (uint8_t i = 0; i < requestedTlvs.mNumTlvs; i++)
        {
            // Skip LeaderDataTlv since it is already included by default.
            if (requestedTlvs.mTlvs[i] != Tlv::kLeaderData)
            {
                tlvs[tlvslength++] = requestedTlvs.mTlvs[i];
            }
        }
        break;
    case kErrorNotFound:
        break;
    default:
        ExitNow(error = kErrorParse);
    }

#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
    if (child->IsCslSynchronized())
    {
        CslChannelTlv cslChannel;
        uint32_t      cslTimeout;

        if (Tlv::Find<CslTimeoutTlv>(aMessage, cslTimeout) == kErrorNone)
        {
            child->SetCslTimeout(cslTimeout);
        }

        if (Tlv::FindTlv(aMessage, cslChannel) == kErrorNone)
        {
            child->SetCslChannel(static_cast<uint8_t>(cslChannel.GetChannel()));
        }
        else
        {
            // Set CSL Channel unspecified.
            child->SetCslChannel(0);
        }
    }
#endif // OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE

    child->SetLastHeard(TimerMilli::GetNow());

    if (oldMode != child->GetDeviceMode())
    {
        otLogNoteMle("Child 0x%04x mode change 0x%02x -> 0x%02x [%s]", child->GetRloc16(), oldMode.Get(),
                     child->GetDeviceMode().Get(), child->GetDeviceMode().ToString().AsCString());

        childDidChange = true;

#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
        if (child->IsRxOnWhenIdle())
        {
            // Clear CSL synchronization state
            child->SetCslSynchronized(false);
        }
#endif

        // The `IndirectSender::HandleChildModeChange()` needs to happen
        // after "Child Update" message is fully parsed to ensure that
        // any registered IPv6 addresses included in the "Child Update"
        // are added to the child.

        Get<IndirectSender>().HandleChildModeChange(*child, oldMode);
    }

    if (childDidChange)
    {
        IgnoreError(mChildTable.StoreChild(*child));
    }

#if OPENTHREAD_CONFIG_MULTI_RADIO
    // We clear the fragment tag only if the "Child Update Request" is
    // from a detached child trying to restore its link with its
    // parent which is indicated by the presence of Challenge TLV in
    // the message.
    if (challenge.mLength != 0)
    {
        child->ClearLastRxFragmentTag();
    }
#endif

    SendChildUpdateResponse(child, aMessageInfo, tlvs, tlvslength, challenge);

exit:
    LogProcessError(kTypeChildUpdateRequestOfChild, error);
}

void MleRouter::HandleChildUpdateResponse(const Message &         aMessage,
                                          const Ip6::MessageInfo &aMessageInfo,
                                          uint32_t                aKeySequence,
                                          Neighbor *              aNeighbor)
{
    Error      error = kErrorNone;
    uint16_t   sourceAddress;
    uint32_t   timeout;
    Challenge  response;
    uint8_t    status;
    uint32_t   linkFrameCounter;
    uint32_t   mleFrameCounter;
    LeaderData leaderData;
    Child *    child;
    uint16_t   addressRegistrationOffset = 0;

    if ((aNeighbor == nullptr) || IsActiveRouter(aNeighbor->GetRloc16()))
    {
        Log(kMessageReceive, kTypeChildUpdateResponseOfUnknownChild, aMessageInfo.GetPeerAddr());
        ExitNow(error = kErrorNotFound);
    }

    child = static_cast<Child *>(aNeighbor);

    // Response
    switch (ReadResponse(aMessage, response))
    {
    case kErrorNone:
        VerifyOrExit(response.Matches(child->GetChallenge(), child->GetChallengeSize()), error = kErrorSecurity);
        break;
    case kErrorNotFound:
        VerifyOrExit(child->IsStateValid(), error = kErrorSecurity);
        break;
    default:
        ExitNow(error = kErrorNone);
    }

    Log(kMessageReceive, kTypeChildUpdateResponseOfChild, aMessageInfo.GetPeerAddr(), child->GetRloc16());

    // Source Address
    switch (Tlv::Find<SourceAddressTlv>(aMessage, sourceAddress))
    {
    case kErrorNone:
        if (child->GetRloc16() != sourceAddress)
        {
            RemoveNeighbor(*child);
            ExitNow();
        }

        break;

    case kErrorNotFound:
        break;

    default:
        ExitNow(error = kErrorParse);
    }

    // Status
    switch (Tlv::Find<ThreadStatusTlv>(aMessage, status))
    {
    case kErrorNone:
        VerifyOrExit(status != StatusTlv::kError, RemoveNeighbor(*child));
        break;
    case kErrorNotFound:
        break;
    default:
        ExitNow(error = kErrorParse);
    }

    // Link-Layer Frame Counter

    switch (Tlv::Find<LinkFrameCounterTlv>(aMessage, linkFrameCounter))
    {
    case kErrorNone:
        child->GetLinkFrameCounters().SetAll(linkFrameCounter);
        child->SetLinkAckFrameCounter(linkFrameCounter);
        break;
    case kErrorNotFound:
        break;
    default:
        ExitNow(error = kErrorParse);
    }

    // MLE Frame Counter
    switch (Tlv::Find<MleFrameCounterTlv>(aMessage, mleFrameCounter))
    {
    case kErrorNone:
        child->SetMleFrameCounter(mleFrameCounter);
        break;
    case kErrorNotFound:
        break;
    default:
        ExitNow(error = kErrorNone);
    }

    // Timeout
    switch (Tlv::Find<TimeoutTlv>(aMessage, timeout))
    {
    case kErrorNone:
        child->SetTimeout(timeout);
        break;
    case kErrorNotFound:
        break;
    default:
        ExitNow(error = kErrorParse);
    }

    // IPv6 Address
    if (Tlv::FindTlvOffset(aMessage, Tlv::kAddressRegistration, addressRegistrationOffset) == kErrorNone)
    {
        SuccessOrExit(error = UpdateChildAddresses(aMessage, addressRegistrationOffset, *child));
    }

    // Leader Data
    switch (ReadLeaderData(aMessage, leaderData))
    {
    case kErrorNone:
        if (child->IsFullNetworkData())
        {
            child->SetNetworkDataVersion(leaderData.GetDataVersion());
        }
        else
        {
            child->SetNetworkDataVersion(leaderData.GetStableDataVersion());
        }
        break;
    case kErrorNotFound:
        break;
    default:
        ExitNow(error = kErrorParse);
    }

    SetChildStateToValid(*child);
    child->SetLastHeard(TimerMilli::GetNow());
    child->SetKeySequence(aKeySequence);
    child->GetLinkInfo().AddRss(aMessageInfo.GetThreadLinkInfo()->GetRss());

exit:
    LogProcessError(kTypeChildUpdateResponseOfChild, error);
}

void MleRouter::HandleDataRequest(const Message &         aMessage,
                                  const Ip6::MessageInfo &aMessageInfo,
                                  const Neighbor *        aNeighbor)
{
    Error              error = kErrorNone;
    RequestedTlvs      requestedTlvs;
    MeshCoP::Timestamp timestamp;
    uint8_t            tlvs[4];
    uint8_t            numTlvs;

    Log(kMessageReceive, kTypeDataRequest, aMessageInfo.GetPeerAddr());

    VerifyOrExit(aNeighbor && aNeighbor->IsStateValid(), error = kErrorSecurity);

    // TLV Request
    SuccessOrExit(error = FindTlvRequest(aMessage, requestedTlvs));
    VerifyOrExit(requestedTlvs.mNumTlvs <= sizeof(tlvs), error = kErrorParse);

    memset(tlvs, Tlv::kInvalid, sizeof(tlvs));
    memcpy(tlvs, requestedTlvs.mTlvs, requestedTlvs.mNumTlvs);
    numTlvs = requestedTlvs.mNumTlvs;

    // Active Timestamp
    switch (Tlv::Find<ActiveTimestampTlv>(aMessage, timestamp))
    {
    case kErrorNone:
        if (Get<MeshCoP::ActiveDataset>().Compare(timestamp) == 0)
        {
            break;
        }

        OT_FALL_THROUGH;

    case kErrorNotFound:
        tlvs[numTlvs++] = Tlv::kActiveDataset;
        break;

    default:
        ExitNow(error = kErrorParse);
    }

    // Pending Timestamp
    switch (Tlv::Find<PendingTimestampTlv>(aMessage, timestamp))
    {
    case kErrorNone:
        if (Get<MeshCoP::PendingDataset>().Compare(timestamp) == 0)
        {
            break;
        }

        OT_FALL_THROUGH;

    case kErrorNotFound:
        tlvs[numTlvs++] = Tlv::kPendingDataset;
        break;

    default:
        ExitNow(error = kErrorParse);
    }

    SendDataResponse(aMessageInfo.GetPeerAddr(), tlvs, numTlvs, 0, &aMessage);

exit:
    LogProcessError(kTypeDataRequest, error);
}

void MleRouter::HandleNetworkDataUpdateRouter(void)
{
    static const uint8_t tlvs[] = {Tlv::kNetworkData};
    Ip6::Address         destination;
    uint16_t             delay;

    VerifyOrExit(IsRouterOrLeader());

    destination.SetToLinkLocalAllNodesMulticast();

    delay = IsLeader() ? 0 : Random::NonCrypto::GetUint16InRange(0, kUnsolicitedDataResponseJitter);
    SendDataResponse(destination, tlvs, sizeof(tlvs), delay);

    SynchronizeChildNetworkData();

exit:
    return;
}

void MleRouter::SynchronizeChildNetworkData(void)
{
    VerifyOrExit(IsRouterOrLeader());

    for (Child &child : Get<ChildTable>().Iterate(Child::kInStateValid))
    {
        uint8_t version;

        if (child.IsRxOnWhenIdle())
        {
            continue;
        }

        if (child.IsFullNetworkData())
        {
            version = Get<NetworkData::Leader>().GetVersion();
        }
        else
        {
            version = Get<NetworkData::Leader>().GetStableVersion();
        }

        if (child.GetNetworkDataVersion() == version)
        {
            continue;
        }

        SuccessOrExit(SendChildUpdateRequest(child));
    }

exit:
    return;
}

#if OPENTHREAD_CONFIG_MLE_STEERING_DATA_SET_OOB_ENABLE
void MleRouter::SetSteeringData(const Mac::ExtAddress *aExtAddress)
{
    Mac::ExtAddress nullExtAddr;
    Mac::ExtAddress allowAnyExtAddr;

    nullExtAddr.Clear();
    allowAnyExtAddr.Fill(0xff);

    if ((aExtAddress == nullptr) || (*aExtAddress == nullExtAddr))
    {
        mSteeringData.Clear();
    }
    else if (*aExtAddress == allowAnyExtAddr)
    {
        mSteeringData.SetToPermitAllJoiners();
    }
    else
    {
        Mac::ExtAddress joinerId;

        mSteeringData.Init();
        MeshCoP::ComputeJoinerId(*aExtAddress, joinerId);
        mSteeringData.UpdateBloomFilter(joinerId);
    }
}
#endif // OPENTHREAD_CONFIG_MLE_STEERING_DATA_SET_OOB_ENABLE

void MleRouter::HandleDiscoveryRequest(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    Error                        error = kErrorNone;
    Tlv                          tlv;
    MeshCoP::Tlv                 meshcopTlv;
    MeshCoP::DiscoveryRequestTlv discoveryRequest;
    Mac::ExtendedPanId           extPanId;
    uint16_t                     offset;
    uint16_t                     end;

    Log(kMessageReceive, kTypeDiscoveryRequest, aMessageInfo.GetPeerAddr());

    discoveryRequest.SetLength(0);

    // only Routers and REEDs respond
    VerifyOrExit(IsRouterEligible(), error = kErrorInvalidState);

    // find MLE Discovery TLV
    VerifyOrExit(Tlv::FindTlvOffset(aMessage, Tlv::kDiscovery, offset) == kErrorNone, error = kErrorParse);
    IgnoreError(aMessage.Read(offset, tlv));

    offset += sizeof(tlv);
    end = offset + sizeof(tlv) + tlv.GetLength();

    while (offset < end)
    {
        IgnoreError(aMessage.Read(offset, meshcopTlv));

        switch (meshcopTlv.GetType())
        {
        case MeshCoP::Tlv::kDiscoveryRequest:
            IgnoreError(aMessage.Read(offset, discoveryRequest));
            VerifyOrExit(discoveryRequest.IsValid(), error = kErrorParse);

            break;

        case MeshCoP::Tlv::kExtendedPanId:
            SuccessOrExit(error = Tlv::Read<MeshCoP::ExtendedPanIdTlv>(aMessage, offset, extPanId));
            VerifyOrExit(Get<Mac::Mac>().GetExtendedPanId() != extPanId, error = kErrorDrop);

            break;

        default:
            break;
        }

        offset += sizeof(meshcopTlv) + meshcopTlv.GetLength();
    }

    if (discoveryRequest.IsValid())
    {
        if (mDiscoveryRequestCallback != nullptr)
        {
            otThreadDiscoveryRequestInfo info;

            aMessageInfo.GetPeerAddr().GetIid().ConvertToExtAddress(*static_cast<Mac::ExtAddress *>(&info.mExtAddress));
            info.mVersion  = discoveryRequest.GetVersion();
            info.mIsJoiner = discoveryRequest.IsJoiner();

            mDiscoveryRequestCallback(&info, mDiscoveryRequestCallbackContext);
        }

        if (discoveryRequest.IsJoiner())
        {
#if OPENTHREAD_CONFIG_MLE_STEERING_DATA_SET_OOB_ENABLE
            if (!mSteeringData.IsEmpty())
            {
            }
            else // if steering data is not set out of band, fall back to network data
#endif
            {
                VerifyOrExit(Get<NetworkData::Leader>().IsJoiningEnabled(), error = kErrorSecurity);
            }
        }
    }

    error = SendDiscoveryResponse(aMessageInfo.GetPeerAddr(), aMessage);

exit:
    LogProcessError(kTypeDiscoveryRequest, error);
}

Error MleRouter::SendDiscoveryResponse(const Ip6::Address &aDestination, const Message &aDiscoverRequestMessage)
{
    Error                         error = kErrorNone;
    Message *                     message;
    uint16_t                      startOffset;
    Tlv                           tlv;
    MeshCoP::DiscoveryResponseTlv discoveryResponse;
    MeshCoP::NetworkNameTlv       networkName;
    uint16_t                      delay;

    VerifyOrExit((message = NewMleMessage()) != nullptr, error = kErrorNoBufs);
    message->SetSubType(Message::kSubTypeMleDiscoverResponse);
    message->SetPanId(aDiscoverRequestMessage.GetPanId());
#if OPENTHREAD_CONFIG_MULTI_RADIO
    // Send the MLE Discovery Response message on same radio link
    // from which the "MLE Discover Request" message was received.
    message->SetRadioType(aDiscoverRequestMessage.GetRadioType());
#endif

    SuccessOrExit(error = AppendHeader(*message, kCommandDiscoveryResponse));

    // Discovery TLV
    tlv.SetType(Tlv::kDiscovery);
    SuccessOrExit(error = message->Append(tlv));

    startOffset = message->GetLength();

    // Discovery Response TLV
    discoveryResponse.Init();
    discoveryResponse.SetVersion(kThreadVersion);

#if OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
    if (Get<KeyManager>().GetSecurityPolicy().mNativeCommissioningEnabled)
    {
        SuccessOrExit(
            error = Tlv::Append<MeshCoP::CommissionerUdpPortTlv>(*message, Get<MeshCoP::BorderAgent>().GetUdpPort()));

        discoveryResponse.SetNativeCommissioner(true);
    }
    else
#endif
    {
        discoveryResponse.SetNativeCommissioner(false);
    }

    if (Get<KeyManager>().GetSecurityPolicy().mCommercialCommissioningEnabled)
    {
        discoveryResponse.SetCommercialCommissioningMode(true);
    }

    SuccessOrExit(error = discoveryResponse.AppendTo(*message));

    // Extended PAN ID TLV
    SuccessOrExit(error = Tlv::Append<MeshCoP::ExtendedPanIdTlv>(*message, Get<Mac::Mac>().GetExtendedPanId()));

    // Network Name TLV
    networkName.Init();
    networkName.SetNetworkName(Get<Mac::Mac>().GetNetworkName().GetAsData());
    SuccessOrExit(error = networkName.AppendTo(*message));

#if OPENTHREAD_CONFIG_MLE_STEERING_DATA_SET_OOB_ENABLE
    // If steering data is set out of band, use that value.
    // Otherwise use the one from commissioning data.
    if (!mSteeringData.IsEmpty())
    {
        SuccessOrExit(error = Tlv::Append<MeshCoP::SteeringDataTlv>(*message, mSteeringData.GetData(),
                                                                    mSteeringData.GetLength()));
    }
    else
#endif
    {
        const MeshCoP::Tlv *steeringData;

        steeringData = Get<NetworkData::Leader>().GetCommissioningDataSubTlv(MeshCoP::Tlv::kSteeringData);

        if (steeringData != nullptr)
        {
            SuccessOrExit(error = steeringData->AppendTo(*message));
        }
    }

    SuccessOrExit(
        error = Tlv::Append<MeshCoP::JoinerUdpPortTlv>(*message, Get<MeshCoP::JoinerRouter>().GetJoinerUdpPort()));

    tlv.SetLength(static_cast<uint8_t>(message->GetLength() - startOffset));
    message->Write(startOffset - sizeof(tlv), tlv);

    delay = Random::NonCrypto::GetUint16InRange(0, kDiscoveryMaxJitter + 1);

    SuccessOrExit(error = AddDelayedResponse(*message, aDestination, delay));

    Log(kMessageDelay, kTypeDiscoveryResponse, aDestination);

exit:
    FreeMessageOnError(message, error);
    LogProcessError(kTypeDiscoveryResponse, error);
    return error;
}

Error MleRouter::SendChildIdResponse(Child &aChild)
{
    Error        error = kErrorNone;
    Ip6::Address destination;
    Message *    message;

    VerifyOrExit((message = NewMleMessage()) != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = AppendHeader(*message, kCommandChildIdResponse));
    SuccessOrExit(error = AppendSourceAddress(*message));
    SuccessOrExit(error = AppendLeaderData(*message));
    SuccessOrExit(error = AppendActiveTimestamp(*message));
    SuccessOrExit(error = AppendPendingTimestamp(*message));

    if (aChild.GetRloc16() == 0)
    {
        uint16_t rloc16;

        // pick next Child ID that is not being used
        do
        {
            mNextChildId++;

            if (mNextChildId > kMaxChildId)
            {
                mNextChildId = kMinChildId;
            }

            rloc16 = Get<Mac::Mac>().GetShortAddress() | mNextChildId;

        } while (mChildTable.FindChild(rloc16, Child::kInStateAnyExceptInvalid) != nullptr);

        // allocate Child ID
        aChild.SetRloc16(rloc16);
    }

    SuccessOrExit(error = AppendAddress16(*message, aChild.GetRloc16()));

    for (uint8_t i = 0; i < Child::kMaxRequestTlvs; i++)
    {
        switch (aChild.GetRequestTlv(i))
        {
        case Tlv::kNetworkData:
            SuccessOrExit(error = AppendNetworkData(*message, !aChild.IsFullNetworkData()));
            break;

        case Tlv::kRoute:
            SuccessOrExit(error = AppendRoute(*message));
            break;

        case Tlv::kActiveDataset:
            SuccessOrExit(error = AppendActiveDataset(*message));
            break;

        case Tlv::kPendingDataset:
            SuccessOrExit(error = AppendPendingDataset(*message));
            break;

        default:
            break;
        }
    }

    if (!aChild.IsFullThreadDevice())
    {
        SuccessOrExit(error = AppendChildAddresses(*message, aChild));
    }

    SetChildStateToValid(aChild);

    if (!aChild.IsRxOnWhenIdle())
    {
        Get<IndirectSender>().SetChildUseShortAddress(aChild, false);
    }

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    if (aChild.IsTimeSyncEnabled())
    {
        message->SetTimeSync(true);
    }
#endif

    destination.SetToLinkLocalAddress(aChild.GetExtAddress());
    SuccessOrExit(error = SendMessage(*message, destination));

    Log(kMessageSend, kTypeChildIdResponse, destination, aChild.GetRloc16());

exit:
    FreeMessageOnError(message, error);
    return error;
}

Error MleRouter::SendChildUpdateRequest(Child &aChild)
{
    static const uint8_t tlvs[] = {Tlv::kTimeout, Tlv::kAddressRegistration};
    Error                error  = kErrorNone;
    Ip6::Address         destination;
    Message *            message;

    if (!aChild.IsRxOnWhenIdle())
    {
        uint16_t childIndex = Get<ChildTable>().GetChildIndex(aChild);

        for (message = Get<MeshForwarder>().GetSendQueue().GetHead(); message; message = message->GetNext())
        {
            if (message->GetChildMask(childIndex) && message->GetSubType() == Message::kSubTypeMleChildUpdateRequest)
            {
                // No need to send the resync "Child Update Request" to the sleepy child
                // if there is one already queued.
                if (aChild.IsStateRestoring())
                {
                    ExitNow();
                }

                // Remove queued outdated "Child Update Request" when there is newer Network Data is to send.
                Get<MeshForwarder>().RemoveMessages(aChild, Message::kSubTypeMleChildUpdateRequest);
                break;
            }
        }
    }

    VerifyOrExit((message = NewMleMessage()) != nullptr, error = kErrorNoBufs);
    message->SetSubType(Message::kSubTypeMleChildUpdateRequest);
    SuccessOrExit(error = AppendHeader(*message, kCommandChildUpdateRequest));
    SuccessOrExit(error = AppendSourceAddress(*message));
    SuccessOrExit(error = AppendLeaderData(*message));
    SuccessOrExit(error = AppendNetworkData(*message, !aChild.IsFullNetworkData()));
    SuccessOrExit(error = AppendActiveTimestamp(*message));
    SuccessOrExit(error = AppendPendingTimestamp(*message));

    if (!aChild.IsStateValid())
    {
        SuccessOrExit(error = AppendTlvRequest(*message, tlvs, sizeof(tlvs)));
        aChild.GenerateChallenge();
        SuccessOrExit(error = AppendChallenge(*message, aChild.GetChallenge(), aChild.GetChallengeSize()));
    }

    destination.SetToLinkLocalAddress(aChild.GetExtAddress());
    SuccessOrExit(error = SendMessage(*message, destination));

    if (aChild.IsRxOnWhenIdle())
    {
        // only try to send a single Child Update Request message to an rx-on-when-idle child
        aChild.SetState(Child::kStateChildUpdateRequest);
    }

    Log(kMessageSend, kTypeChildUpdateRequestOfChild, destination, aChild.GetRloc16());

exit:
    FreeMessageOnError(message, error);
    return error;
}

void MleRouter::SendChildUpdateResponse(Child *                 aChild,
                                        const Ip6::MessageInfo &aMessageInfo,
                                        const uint8_t *         aTlvs,
                                        uint8_t                 aTlvsLength,
                                        const Challenge &       aChallenge)
{
    Error    error = kErrorNone;
    Message *message;

    VerifyOrExit((message = NewMleMessage()) != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = AppendHeader(*message, kCommandChildUpdateResponse));

    for (int i = 0; i < aTlvsLength; i++)
    {
        switch (aTlvs[i])
        {
        case Tlv::kStatus:
            SuccessOrExit(error = AppendStatus(*message, StatusTlv::kError));
            break;

        case Tlv::kAddressRegistration:
            SuccessOrExit(error = AppendChildAddresses(*message, *aChild));
            break;

        case Tlv::kLeaderData:
            SuccessOrExit(error = AppendLeaderData(*message));
            break;

        case Tlv::kMode:
            SuccessOrExit(error = AppendMode(*message, aChild->GetDeviceMode()));
            break;

        case Tlv::kNetworkData:
            SuccessOrExit(error = AppendNetworkData(*message, !aChild->IsFullNetworkData()));
            SuccessOrExit(error = AppendActiveTimestamp(*message));
            SuccessOrExit(error = AppendPendingTimestamp(*message));
            break;

        case Tlv::kResponse:
            SuccessOrExit(error = AppendResponse(*message, aChallenge));
            break;

        case Tlv::kSourceAddress:
            SuccessOrExit(error = AppendSourceAddress(*message));
            break;

        case Tlv::kTimeout:
            SuccessOrExit(error = AppendTimeout(*message, aChild->GetTimeout()));
            break;

        case Tlv::kMleFrameCounter:
            SuccessOrExit(error = AppendMleFrameCounter(*message));
            break;

        case Tlv::kLinkFrameCounter:
            SuccessOrExit(error = AppendLinkFrameCounter(*message));
            break;
        }
    }

    SuccessOrExit(error = SendMessage(*message, aMessageInfo.GetPeerAddr()));

    if (aChild == nullptr)
    {
        Log(kMessageSend, kTypeChildUpdateResponseOfChild, aMessageInfo.GetPeerAddr());
    }
    else
    {
        Log(kMessageSend, kTypeChildUpdateResponseOfChild, aMessageInfo.GetPeerAddr(), aChild->GetRloc16());
    }

exit:
    FreeMessageOnError(message, error);
}

void MleRouter::SendDataResponse(const Ip6::Address &aDestination,
                                 const uint8_t *     aTlvs,
                                 uint8_t             aTlvsLength,
                                 uint16_t            aDelay,
                                 const Message *     aRequestMessage)
{
    OT_UNUSED_VARIABLE(aRequestMessage);

    Error     error   = kErrorNone;
    Message * message = nullptr;
    Neighbor *neighbor;
    bool      stableOnly;

    if (mRetrieveNewNetworkData)
    {
        otLogInfoMle("Suppressing Data Response - waiting for new network data");
        ExitNow();
    }

    VerifyOrExit((message = NewMleMessage()) != nullptr, error = kErrorNoBufs);
    message->SetSubType(Message::kSubTypeMleDataResponse);
    SuccessOrExit(error = AppendHeader(*message, kCommandDataResponse));
    SuccessOrExit(error = AppendSourceAddress(*message));
    SuccessOrExit(error = AppendLeaderData(*message));
    SuccessOrExit(error = AppendActiveTimestamp(*message));
    SuccessOrExit(error = AppendPendingTimestamp(*message));

    for (int i = 0; i < aTlvsLength; i++)
    {
        switch (aTlvs[i])
        {
        case Tlv::kNetworkData:
            neighbor   = mNeighborTable.FindNeighbor(aDestination);
            stableOnly = neighbor != nullptr ? !neighbor->IsFullNetworkData() : false;
            SuccessOrExit(error = AppendNetworkData(*message, stableOnly));
            break;

        case Tlv::kActiveDataset:
            SuccessOrExit(error = AppendActiveDataset(*message));
            break;

        case Tlv::kPendingDataset:
            SuccessOrExit(error = AppendPendingDataset(*message));
            break;

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
        case Tlv::kLinkMetricsReport:
            OT_ASSERT(aRequestMessage != nullptr);
            neighbor = mNeighborTable.FindNeighbor(aDestination);
            VerifyOrExit(neighbor != nullptr, error = kErrorInvalidState);
            SuccessOrExit(error = Get<LinkMetrics::LinkMetrics>().AppendReport(*message, *aRequestMessage, *neighbor));
            break;
#endif
        }
    }

    if (aDelay)
    {
        // Remove MLE Data Responses from Send Message Queue.
        Get<MeshForwarder>().RemoveDataResponseMessages();

        // Remove multicast MLE Data Response from Delayed Message Queue.
        RemoveDelayedDataResponseMessage();

        SuccessOrExit(error = AddDelayedResponse(*message, aDestination, aDelay));
        Log(kMessageDelay, kTypeDataResponse, aDestination);
    }
    else
    {
        SuccessOrExit(error = SendMessage(*message, aDestination));
        Log(kMessageSend, kTypeDataResponse, aDestination);
    }

exit:
    FreeMessageOnError(message, error);
    LogSendError(kTypeDataResponse, error);
}

bool MleRouter::IsMinimalChild(uint16_t aRloc16)
{
    bool rval = false;

    if (RouterIdFromRloc16(aRloc16) == RouterIdFromRloc16(Get<Mac::Mac>().GetShortAddress()))
    {
        Neighbor *neighbor;

        neighbor = mNeighborTable.FindNeighbor(aRloc16);

        rval = (neighbor != nullptr) && (!neighbor->IsFullThreadDevice());
    }

    return rval;
}

void MleRouter::RemoveRouterLink(Router &aRouter)
{
    switch (mRole)
    {
    case kRoleChild:
        if (&aRouter == &mParent)
        {
            IgnoreError(BecomeDetached());
        }
        break;

#if OPENTHREAD_FTD
    case kRoleRouter:
    case kRoleLeader:
        mRouterTable.RemoveRouterLink(aRouter);
        break;
#endif

    default:
        break;
    }
}

void MleRouter::RemoveNeighbor(Neighbor &aNeighbor)
{
    VerifyOrExit(!aNeighbor.IsStateInvalid());

    if (&aNeighbor == &mParent)
    {
        if (IsChild())
        {
            IgnoreError(BecomeDetached());
        }
    }
    else if (&aNeighbor == &mParentCandidate)
    {
        mParentCandidate.Clear();
    }
    else if (!IsActiveRouter(aNeighbor.GetRloc16()))
    {
        OT_ASSERT(mChildTable.GetChildIndex(static_cast<Child &>(aNeighbor)) < kMaxChildren);

        if (aNeighbor.IsStateValidOrRestoring())
        {
            mNeighborTable.Signal(NeighborTable::kChildRemoved, aNeighbor);
        }

        Get<IndirectSender>().ClearAllMessagesForSleepyChild(static_cast<Child &>(aNeighbor));

        if (aNeighbor.IsFullThreadDevice())
        {
            // Clear all EID-to-RLOC entries associated with the child.
            Get<AddressResolver>().Remove(aNeighbor.GetRloc16());
        }

        mChildTable.RemoveStoredChild(static_cast<Child &>(aNeighbor));
    }
    else if (aNeighbor.IsStateValid())
    {
        OT_ASSERT(mRouterTable.Contains(aNeighbor));

        mNeighborTable.Signal(NeighborTable::kRouterRemoved, aNeighbor);
        mRouterTable.RemoveRouterLink(static_cast<Router &>(aNeighbor));
    }

    aNeighbor.GetLinkInfo().Clear();
    aNeighbor.SetState(Neighbor::kStateInvalid);
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    aNeighbor.RemoveAllForwardTrackingSeriesInfo();
#endif

exit:
    return;
}

uint16_t MleRouter::GetNextHop(uint16_t aDestination)
{
    uint8_t       destinationId = RouterIdFromRloc16(aDestination);
    uint8_t       routeCost;
    uint8_t       linkCost;
    uint16_t      rval = Mac::kShortAddrInvalid;
    const Router *router;
    const Router *nextHop;

    if (IsChild())
    {
        ExitNow(rval = Mle::GetNextHop(aDestination));
    }

    // The frame is destined to a child
    if (destinationId == mRouterId)
    {
        ExitNow(rval = aDestination);
    }

    router = mRouterTable.GetRouter(destinationId);
    VerifyOrExit(router != nullptr);

    linkCost  = GetLinkCost(destinationId);
    routeCost = GetRouteCost(aDestination);

    if ((routeCost + GetLinkCost(router->GetNextHop())) < linkCost)
    {
        nextHop = mRouterTable.GetRouter(router->GetNextHop());
        VerifyOrExit(nextHop != nullptr && !nextHop->IsStateInvalid());

        rval = Rloc16FromRouterId(router->GetNextHop());
    }
    else if (linkCost < kMaxRouteCost)
    {
        rval = Rloc16FromRouterId(destinationId);
    }

exit:
    return rval;
}

uint8_t MleRouter::GetCost(uint16_t aRloc16)
{
    uint8_t routerId = RouterIdFromRloc16(aRloc16);
    uint8_t cost     = GetLinkCost(routerId);
    Router *router   = mRouterTable.GetRouter(routerId);
    uint8_t routeCost;

    VerifyOrExit(router != nullptr && mRouterTable.GetRouter(router->GetNextHop()) != nullptr);

    routeCost = GetRouteCost(aRloc16) + GetLinkCost(router->GetNextHop());

    if (cost > routeCost)
    {
        cost = routeCost;
    }

exit:
    return cost;
}

uint8_t MleRouter::GetRouteCost(uint16_t aRloc16) const
{
    uint8_t       rval = kMaxRouteCost;
    const Router *router;

    router = mRouterTable.GetRouter(RouterIdFromRloc16(aRloc16));
    VerifyOrExit(router != nullptr && mRouterTable.GetRouter(router->GetNextHop()) != nullptr);

    rval = router->GetCost();

exit:
    return rval;
}

Error MleRouter::SetPreferredRouterId(uint8_t aRouterId)
{
    Error error = kErrorNone;

    VerifyOrExit(IsDetached() || IsDisabled(), error = kErrorInvalidState);

    mPreviousRouterId = aRouterId;

exit:
    return error;
}

void MleRouter::SetRouterId(uint8_t aRouterId)
{
    mRouterId         = aRouterId;
    mPreviousRouterId = mRouterId;
}

void MleRouter::ResolveRoutingLoops(uint16_t aSourceMac, uint16_t aDestRloc16)
{
    Router *router;

    if (aSourceMac != GetNextHop(aDestRloc16))
    {
        ExitNow();
    }

    // loop exists
    router = mRouterTable.GetRouter(RouterIdFromRloc16(aDestRloc16));
    VerifyOrExit(router != nullptr);

    // invalidate next hop
    router->SetNextHop(kInvalidRouterId);
    ResetAdvertiseInterval();

exit:
    return;
}

Error MleRouter::CheckReachability(uint16_t aMeshDest, Ip6::Header &aIp6Header)
{
    Error error = kErrorNone;

    if (IsChild())
    {
        error = Mle::CheckReachability(aMeshDest, aIp6Header);
        ExitNow();
    }

    if (aMeshDest == Get<Mac::Mac>().GetShortAddress())
    {
        // mesh destination is this device
        if (Get<ThreadNetif>().HasUnicastAddress(aIp6Header.GetDestination()))
        {
            // IPv6 destination is this device
            ExitNow();
        }
        else if (mNeighborTable.FindNeighbor(aIp6Header.GetDestination()) != nullptr)
        {
            // IPv6 destination is an RFD child
            ExitNow();
        }
    }
    else if (RouterIdFromRloc16(aMeshDest) == mRouterId)
    {
        // mesh destination is a child of this device
        if (mChildTable.FindChild(aMeshDest, Child::kInStateValidOrRestoring))
        {
            ExitNow();
        }
    }
    else if (GetNextHop(aMeshDest) != Mac::kShortAddrInvalid)
    {
        // forwarding to another router and route is known
        ExitNow();
    }

    error = kErrorNoRoute;

exit:
    return error;
}

Error MleRouter::SendAddressSolicit(ThreadStatusTlv::Status aStatus)
{
    Error            error = kErrorNone;
    Ip6::MessageInfo messageInfo;
    Coap::Message *  message = nullptr;

    VerifyOrExit(!mAddressSolicitPending);

    VerifyOrExit((message = Get<Tmf::Agent>().NewPriorityMessage()) != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = message->InitAsConfirmablePost(UriPath::kAddressSolicit));
    SuccessOrExit(error = message->SetPayloadMarker());

    SuccessOrExit(error = Tlv::Append<ThreadExtMacAddressTlv>(*message, Get<Mac::Mac>().GetExtAddress()));

    if (IsRouterIdValid(mPreviousRouterId))
    {
        SuccessOrExit(error = Tlv::Append<ThreadRloc16Tlv>(*message, Rloc16FromRouterId(mPreviousRouterId)));
    }

    SuccessOrExit(error = Tlv::Append<ThreadStatusTlv>(*message, aStatus));

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    SuccessOrExit(error = AppendXtalAccuracy(*message));
#endif

    SuccessOrExit(error = GetLeaderAddress(messageInfo.GetPeerAddr()));
    messageInfo.SetSockAddr(GetMeshLocal16());
    messageInfo.SetPeerPort(Tmf::kUdpPort);

    SuccessOrExit(
        error = Get<Tmf::Agent>().SendMessage(*message, messageInfo, &MleRouter::HandleAddressSolicitResponse, this));
    mAddressSolicitPending = true;

    Log(kMessageSend, kTypeAddressSolicit, messageInfo.GetPeerAddr());

exit:
    FreeMessageOnError(message, error);
    return error;
}

void MleRouter::SendAddressRelease(void)
{
    Error            error = kErrorNone;
    Ip6::MessageInfo messageInfo;
    Coap::Message *  message;

    VerifyOrExit((message = Get<Tmf::Agent>().NewMessage()) != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = message->InitAsConfirmablePost(UriPath::kAddressRelease));
    SuccessOrExit(error = message->SetPayloadMarker());

    SuccessOrExit(error = Tlv::Append<ThreadRloc16Tlv>(*message, Rloc16FromRouterId(mRouterId)));

    SuccessOrExit(error = Tlv::Append<ThreadExtMacAddressTlv>(*message, Get<Mac::Mac>().GetExtAddress()));

    messageInfo.SetSockAddr(GetMeshLocal16());
    SuccessOrExit(error = GetLeaderAddress(messageInfo.GetPeerAddr()));
    messageInfo.SetPeerPort(Tmf::kUdpPort);
    SuccessOrExit(error = Get<Tmf::Agent>().SendMessage(*message, messageInfo));

    Log(kMessageSend, kTypeAddressRelease, messageInfo.GetPeerAddr());

exit:
    FreeMessageOnError(message, error);
    LogSendError(kTypeAddressRelease, error);
}

void MleRouter::HandleAddressSolicitResponse(void *               aContext,
                                             otMessage *          aMessage,
                                             const otMessageInfo *aMessageInfo,
                                             Error                aResult)
{
    static_cast<MleRouter *>(aContext)->HandleAddressSolicitResponse(
        static_cast<Coap::Message *>(aMessage), static_cast<const Ip6::MessageInfo *>(aMessageInfo), aResult);
}

void MleRouter::HandleAddressSolicitResponse(Coap::Message *         aMessage,
                                             const Ip6::MessageInfo *aMessageInfo,
                                             Error                   aResult)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    uint8_t             status;
    uint16_t            rloc16;
    ThreadRouterMaskTlv routerMaskTlv;
    uint8_t             routerId;
    Router *            router;
    Router *            leader;

    mAddressSolicitPending = false;

    VerifyOrExit(aResult == kErrorNone && aMessage != nullptr && aMessage != nullptr);

    VerifyOrExit(aMessage->GetCode() == Coap::kCodeChanged);

    Log(kMessageReceive, kTypeAddressReply, aMessageInfo->GetPeerAddr());

    SuccessOrExit(Tlv::Find<ThreadStatusTlv>(*aMessage, status));

    if (status != ThreadStatusTlv::kSuccess)
    {
        mAddressSolicitRejected = true;

        if (IsRouterIdValid(mPreviousRouterId))
        {
            if (HasChildren())
            {
                RemoveChildren();
            }

            SetRouterId(kInvalidRouterId);
        }

        ExitNow();
    }

    SuccessOrExit(Tlv::Find<ThreadRloc16Tlv>(*aMessage, rloc16));
    routerId = RouterIdFromRloc16(rloc16);

    SuccessOrExit(Tlv::FindTlv(*aMessage, routerMaskTlv));
    VerifyOrExit(routerMaskTlv.IsValid());

    // assign short address
    SetRouterId(routerId);

    SetStateRouter(Rloc16FromRouterId(mRouterId));
    mRouterTable.Clear();
    mRouterTable.UpdateRouterIdSet(routerMaskTlv.GetIdSequence(), routerMaskTlv.GetAssignedRouterIdMask());

    router = mRouterTable.GetRouter(routerId);
    VerifyOrExit(router != nullptr);

    router->SetExtAddress(Get<Mac::Mac>().GetExtAddress());
    router->SetCost(0);

    router = mRouterTable.GetRouter(mParent.GetRouterId());
    VerifyOrExit(router != nullptr);

    // Keep link to the parent in order to respond to Parent Requests before new link is established.
    *router = mParent;
    router->SetState(Neighbor::kStateValid);
    router->SetNextHop(kInvalidRouterId);
    router->SetCost(0);

    leader = mRouterTable.GetLeader();
    OT_ASSERT(leader != nullptr);

    if (leader != router)
    {
        // Keep route path to the Leader reported by the parent before it is updated.
        leader->SetCost(mParentLeaderCost);
        leader->SetNextHop(RouterIdFromRloc16(mParent.GetRloc16()));
    }

    // send link request
    IgnoreError(SendLinkRequest(nullptr));

    // send child id responses
    for (Child &child : Get<ChildTable>().Iterate(Child::kInStateChildIdRequest))
    {
        IgnoreError(SendChildIdResponse(child));
    }

exit:

    // send announce after received address solicit reply if needed
    InformPreviousChannel();
}

bool MleRouter::IsExpectedToBecomeRouterSoon(void) const
{
    static constexpr uint8_t kMaxDelay = 10;

    return IsRouterEligible() && IsChild() && !mAddressSolicitRejected &&
           ((GetRouterSelectionJitterTimeout() != 0 && GetRouterSelectionJitterTimeout() <= kMaxDelay) ||
            mAddressSolicitPending);
}

void MleRouter::HandleAddressSolicit(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<MleRouter *>(aContext)->HandleAddressSolicit(*static_cast<Coap::Message *>(aMessage),
                                                             *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

void MleRouter::HandleAddressSolicit(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    Error           error = kErrorNone;
    Mac::ExtAddress extAddress;
    uint16_t        rloc16;
    uint8_t         status;
    Router *        router = nullptr;
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    uint16_t xtalAccuracy;
#endif

    VerifyOrExit(aMessage.IsConfirmablePostRequest(), error = kErrorParse);

    Log(kMessageReceive, kTypeAddressSolicit, aMessageInfo.GetPeerAddr());

    SuccessOrExit(error = Tlv::Find<ThreadExtMacAddressTlv>(aMessage, extAddress));
    SuccessOrExit(error = Tlv::Find<ThreadStatusTlv>(aMessage, status));

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    // In a time sync enabled network, all routers' xtal accuracy must be less than the threshold.
    SuccessOrExit(Tlv::Find<XtalAccuracyTlv>(aMessage, xtalAccuracy));
    VerifyOrExit(xtalAccuracy <= Get<TimeSync>().GetXtalThreshold());
#endif

    // see if allocation already exists
    router = mRouterTable.GetRouter(extAddress);

    if (router != nullptr)
    {
        ExitNow();
    }

    // check the request reason
    switch (status)
    {
    case ThreadStatusTlv::kTooFewRouters:
        VerifyOrExit(mRouterTable.GetActiveRouterCount() < mRouterUpgradeThreshold);
        break;

    case ThreadStatusTlv::kHaveChildIdRequest:
    case ThreadStatusTlv::kParentPartitionChange:
        break;

    default:
        ExitNow(error = kErrorParse);
        OT_UNREACHABLE_CODE(break);
    }

    switch (Tlv::Find<ThreadRloc16Tlv>(aMessage, rloc16))
    {
    case kErrorNone:
        router = mRouterTable.Allocate(RouterIdFromRloc16(rloc16));
        break;
    case kErrorNotFound:
        break;
    default:
        ExitNow(error = kErrorParse);
    }

    // allocate new router id
    if (router == nullptr)
    {
        router = mRouterTable.Allocate();
    }
    else
    {
        otLogInfoMle("router id requested and provided!");
    }

    if (router != nullptr)
    {
        router->SetExtAddress(extAddress);
    }
    else
    {
        otLogInfoMle("router address unavailable!");
    }

exit:

    if (error == kErrorNone)
    {
        SendAddressSolicitResponse(aMessage, router, aMessageInfo);
    }
}

void MleRouter::SendAddressSolicitResponse(const Coap::Message &   aRequest,
                                           const Router *          aRouter,
                                           const Ip6::MessageInfo &aMessageInfo)
{
    Error               error = kErrorNone;
    ThreadRouterMaskTlv routerMaskTlv;
    Coap::Message *     message;

    VerifyOrExit((message = Get<Tmf::Agent>().NewPriorityMessage()) != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = message->SetDefaultResponseHeader(aRequest));
    SuccessOrExit(error = message->SetPayloadMarker());

    SuccessOrExit(error = Tlv::Append<ThreadStatusTlv>(
                      *message, aRouter == nullptr ? ThreadStatusTlv::kNoAddressAvailable : ThreadStatusTlv::kSuccess));

    if (aRouter != nullptr)
    {
        SuccessOrExit(error = Tlv::Append<ThreadRloc16Tlv>(*message, aRouter->GetRloc16()));

        routerMaskTlv.Init();
        routerMaskTlv.SetIdSequence(mRouterTable.GetRouterIdSequence());
        routerMaskTlv.SetAssignedRouterIdMask(mRouterTable.GetRouterIdSet());

        SuccessOrExit(error = routerMaskTlv.AppendTo(*message));
    }

    SuccessOrExit(error = Get<Tmf::Agent>().SendMessage(*message, aMessageInfo));

    Log(kMessageSend, kTypeAddressReply, aMessageInfo.GetPeerAddr());

exit:
    FreeMessageOnError(message, error);
}

void MleRouter::HandleAddressRelease(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<MleRouter *>(aContext)->HandleAddressRelease(*static_cast<Coap::Message *>(aMessage),
                                                             *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

void MleRouter::HandleAddressRelease(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    uint16_t        rloc16;
    Mac::ExtAddress extAddress;
    uint8_t         routerId;
    Router *        router;

    VerifyOrExit(aMessage.IsConfirmablePostRequest());

    Log(kMessageReceive, kTypeAddressRelease, aMessageInfo.GetPeerAddr());

    SuccessOrExit(Tlv::Find<ThreadRloc16Tlv>(aMessage, rloc16));
    SuccessOrExit(Tlv::Find<ThreadExtMacAddressTlv>(aMessage, extAddress));

    routerId = RouterIdFromRloc16(rloc16);
    router   = mRouterTable.GetRouter(routerId);

    VerifyOrExit((router != nullptr) && (router->GetExtAddress() == extAddress));

    IgnoreError(mRouterTable.Release(routerId));

    SuccessOrExit(Get<Tmf::Agent>().SendEmptyAck(aMessage, aMessageInfo));

    Log(kMessageSend, kTypeAddressReleaseReply, aMessageInfo.GetPeerAddr());

exit:
    return;
}

void MleRouter::FillConnectivityTlv(ConnectivityTlv &aTlv)
{
    Router *leader;
    uint8_t cost;
    int8_t  parentPriority = kParentPriorityMedium;

    if (mParentPriority != kParentPriorityUnspecified)
    {
        parentPriority = mParentPriority;
    }
    else
    {
        uint16_t numChildren = mChildTable.GetNumChildren(Child::kInStateValid);
        uint16_t maxAllowed  = mChildTable.GetMaxChildrenAllowed();

        if ((maxAllowed - numChildren) < (maxAllowed / 3))
        {
            parentPriority = kParentPriorityLow;
        }
        else
        {
            parentPriority = kParentPriorityMedium;
        }
    }

    aTlv.SetParentPriority(parentPriority);

    // compute leader cost and link qualities
    aTlv.SetLinkQuality1(0);
    aTlv.SetLinkQuality2(0);
    aTlv.SetLinkQuality3(0);

    leader = mRouterTable.GetLeader();
    cost   = (leader != nullptr) ? leader->GetCost() : static_cast<uint8_t>(kMaxRouteCost);

    switch (mRole)
    {
    case kRoleDisabled:
    case kRoleDetached:
        cost = static_cast<uint8_t>(kMaxRouteCost);
        break;

    case kRoleChild:
        switch (mParent.GetLinkInfo().GetLinkQuality())
        {
        case 1:
            aTlv.SetLinkQuality1(aTlv.GetLinkQuality1() + 1);
            break;

        case 2:
            aTlv.SetLinkQuality2(aTlv.GetLinkQuality2() + 1);
            break;

        case 3:
            aTlv.SetLinkQuality3(aTlv.GetLinkQuality3() + 1);
            break;
        }

        cost += LinkQualityToCost(mParent.GetLinkInfo().GetLinkQuality());
        break;

    case kRoleRouter:
        if (leader != nullptr)
        {
            cost += GetLinkCost(leader->GetNextHop());

            if (!IsRouterIdValid(leader->GetNextHop()) || GetLinkCost(GetLeaderId()) < cost)
            {
                cost = GetLinkCost(GetLeaderId());
            }
        }

        break;

    case kRoleLeader:
        cost = 0;
        break;
    }

    aTlv.SetActiveRouters(mRouterTable.GetActiveRouterCount());

    for (Router &router : Get<RouterTable>().Iterate())
    {
        uint8_t linkQuality;

        if (router.GetRloc16() == GetRloc16())
        {
            // skip self
            continue;
        }

        if (!router.IsStateValid())
        {
            // skip non-neighbor routers
            continue;
        }

        linkQuality = router.GetLinkInfo().GetLinkQuality();

        if (linkQuality > router.GetLinkQualityOut())
        {
            linkQuality = router.GetLinkQualityOut();
        }

        switch (linkQuality)
        {
        case 1:
            aTlv.SetLinkQuality1(aTlv.GetLinkQuality1() + 1);
            break;

        case 2:
            aTlv.SetLinkQuality2(aTlv.GetLinkQuality2() + 1);
            break;

        case 3:
            aTlv.SetLinkQuality3(aTlv.GetLinkQuality3() + 1);
            break;
        }
    }

    aTlv.SetLeaderCost((cost < kMaxRouteCost) ? cost : static_cast<uint8_t>(kMaxRouteCost));
    aTlv.SetIdSequence(mRouterTable.GetRouterIdSequence());
    aTlv.SetSedBufferSize(OPENTHREAD_CONFIG_DEFAULT_SED_BUFFER_SIZE);
    aTlv.SetSedDatagramCount(OPENTHREAD_CONFIG_DEFAULT_SED_DATAGRAM_COUNT);
}

Error MleRouter::AppendConnectivity(Message &aMessage)
{
    ConnectivityTlv tlv;

    tlv.Init();
    FillConnectivityTlv(tlv);

    return tlv.AppendTo(aMessage);
}

Error MleRouter::AppendChildAddresses(Message &aMessage, Child &aChild)
{
    Error                    error;
    Tlv                      tlv;
    AddressRegistrationEntry entry;
    Lowpan::Context          context;
    uint8_t                  length      = 0;
    uint16_t                 startOffset = aMessage.GetLength();

    tlv.SetType(Tlv::kAddressRegistration);
    SuccessOrExit(error = aMessage.Append(tlv));

    for (const Ip6::Address &address : aChild.IterateIp6Addresses())
    {
        if (address.IsMulticast() || Get<NetworkData::Leader>().GetContext(address, context) != kErrorNone)
        {
            // uncompressed entry
            entry.SetUncompressed();
            entry.SetIp6Address(address);
        }
        else if (context.mContextId != kMeshLocalPrefixContextId)
        {
            // compressed entry
            entry.SetContextId(context.mContextId);
            entry.SetIid(address.GetIid());
        }
        else
        {
            continue;
        }

        SuccessOrExit(error = aMessage.AppendBytes(&entry, entry.GetLength()));
        length += entry.GetLength();
    }

    tlv.SetLength(length);
    aMessage.Write(startOffset, tlv);

exit:
    return error;
}

void MleRouter::FillRouteTlv(RouteTlv &aTlv, Neighbor *aNeighbor)
{
    uint8_t     routerIdSequence = mRouterTable.GetRouterIdSequence();
    RouterIdSet routerIdSet      = mRouterTable.GetRouterIdSet();
    uint8_t     routerCount;

    if (aNeighbor && IsActiveRouter(aNeighbor->GetRloc16()))
    {
        // Sending a Link Accept message that may require truncation
        // of Route64 TLV

        routerCount = mRouterTable.GetActiveRouterCount();

        if (routerCount > kLinkAcceptMaxRouters)
        {
            for (uint8_t routerId = 0; routerId <= kMaxRouterId; routerId++)
            {
                if (routerCount <= kLinkAcceptMaxRouters)
                {
                    break;
                }

                if ((routerId == RouterIdFromRloc16(GetRloc16())) || (routerId == aNeighbor->GetRouterId()) ||
                    (routerId == GetLeaderId()))
                {
                    // Route64 TLV must contain this device and the
                    // neighboring router to ensure that at least this
                    // link can be established.
                    continue;
                }

                if (routerIdSet.Contains(routerId))
                {
                    routerIdSet.Remove(routerId);
                    routerCount--;
                }
            }

            // Ensure that the neighbor will process the current
            // Route64 TLV in a subsequent message exchange
            routerIdSequence -= kLinkAcceptSequenceRollback;
        }
    }

    aTlv.SetRouterIdSequence(routerIdSequence);
    aTlv.SetRouterIdMask(routerIdSet);

    routerCount = 0;

    for (Router &router : Get<RouterTable>().Iterate())
    {
        if (!routerIdSet.Contains(router.GetRouterId()))
        {
            continue;
        }

        if (router.GetRloc16() == GetRloc16())
        {
            aTlv.SetLinkQualityIn(routerCount, 0);
            aTlv.SetLinkQualityOut(routerCount, 0);
            aTlv.SetRouteCost(routerCount, 1);
        }
        else
        {
            Router *nextHop;
            uint8_t linkCost;
            uint8_t routeCost;

            linkCost = mRouterTable.GetLinkCost(router);
            nextHop  = mRouterTable.GetRouter(router.GetNextHop());

            if (nextHop == nullptr)
            {
                routeCost = linkCost;
            }
            else
            {
                routeCost = router.GetCost() + mRouterTable.GetLinkCost(*nextHop);

                if (linkCost < routeCost)
                {
                    routeCost = linkCost;
                }
            }

            if (routeCost >= kMaxRouteCost)
            {
                routeCost = 0;
            }

            aTlv.SetRouteCost(routerCount, routeCost);
            aTlv.SetLinkQualityOut(routerCount, router.GetLinkQualityOut());
            aTlv.SetLinkQualityIn(routerCount, router.GetLinkInfo().GetLinkQuality());
        }

        routerCount++;
    }

    aTlv.SetRouteDataLength(routerCount);
}

Error MleRouter::AppendRoute(Message &aMessage, Neighbor *aNeighbor)
{
    RouteTlv tlv;

    tlv.Init();
    FillRouteTlv(tlv, aNeighbor);

    return tlv.AppendTo(aMessage);
}

Error MleRouter::AppendActiveDataset(Message &aMessage)
{
    return Get<MeshCoP::ActiveDataset>().AppendMleDatasetTlv(aMessage);
}

Error MleRouter::AppendPendingDataset(Message &aMessage)
{
    return Get<MeshCoP::PendingDataset>().AppendMleDatasetTlv(aMessage);
}

bool MleRouter::HasMinDowngradeNeighborRouters(void)
{
    uint8_t linkQuality;
    uint8_t routerCount = 0;

    for (Router &router : Get<RouterTable>().Iterate())
    {
        if (!router.IsStateValid())
        {
            continue;
        }

        linkQuality = router.GetLinkInfo().GetLinkQuality();

        if (linkQuality > router.GetLinkQualityOut())
        {
            linkQuality = router.GetLinkQualityOut();
        }

        if (linkQuality >= 2)
        {
            routerCount++;
        }
    }

    return routerCount >= kMinDowngradeNeighbors;
}

bool MleRouter::HasOneNeighborWithComparableConnectivity(const RouteTlv &aRoute, uint8_t aRouterId)
{
    bool rval = true;

    // process local neighbor routers
    for (Router &router : Get<RouterTable>().Iterate())
    {
        uint8_t localLinkQuality = 0;
        uint8_t peerLinkQuality  = 0;
        uint8_t routerCount      = 0;

        if (router.GetRouterId() == mRouterId)
        {
            routerCount++;
            continue;
        }

        // check if neighbor is valid
        if (router.IsStateValid())
        {
            // if neighbor is just peer
            if (router.GetRouterId() == aRouterId)
            {
                routerCount++;
                continue;
            }

            localLinkQuality = router.GetLinkInfo().GetLinkQuality();

            if (localLinkQuality > router.GetLinkQualityOut())
            {
                localLinkQuality = router.GetLinkQualityOut();
            }

            if (localLinkQuality >= 2)
            {
                // check if this neighbor router is in peer Route64 TLV
                if (!aRoute.IsRouterIdSet(router.GetRouterId()))
                {
                    ExitNow(rval = false);
                }

                // get the peer's two-way link quality to this router
                peerLinkQuality = aRoute.GetLinkQualityIn(routerCount);

                if (peerLinkQuality > aRoute.GetLinkQualityOut(routerCount))
                {
                    peerLinkQuality = aRoute.GetLinkQualityOut(routerCount);
                }

                // compare local link quality to this router with peer's
                if (peerLinkQuality >= localLinkQuality)
                {
                    routerCount++;
                    continue;
                }

                ExitNow(rval = false);
            }
        }

        routerCount++;
    }

exit:
    return rval;
}

void MleRouter::SetChildStateToValid(Child &aChild)
{
    VerifyOrExit(!aChild.IsStateValid());

    aChild.SetState(Neighbor::kStateValid);
    IgnoreError(mChildTable.StoreChild(aChild));

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE
    Get<MlrManager>().UpdateProxiedSubscriptions(aChild, nullptr, 0);
#endif

    mNeighborTable.Signal(NeighborTable::kChildAdded, aChild);

exit:
    return;
}

bool MleRouter::HasChildren(void)
{
    return mChildTable.HasChildren(Child::kInStateValidOrAttaching);
}

void MleRouter::RemoveChildren(void)
{
    for (Child &child : Get<ChildTable>().Iterate(Child::kInStateValidOrRestoring))
    {
        RemoveNeighbor(child);
    }
}

bool MleRouter::HasSmallNumberOfChildren(void)
{
    uint16_t numChildren = 0;
    uint8_t  routerCount = mRouterTable.GetActiveRouterCount();

    VerifyOrExit(routerCount > mRouterDowngradeThreshold);

    numChildren = mChildTable.GetNumChildren(Child::kInStateValid);

    return numChildren < (routerCount - mRouterDowngradeThreshold) * 3;

exit:
    return false;
}

Error MleRouter::SetAssignParentPriority(int8_t aParentPriority)
{
    Error error = kErrorNone;

    VerifyOrExit(aParentPriority <= kParentPriorityHigh && aParentPriority >= kParentPriorityUnspecified,
                 error = kErrorInvalidArgs);

    mParentPriority = aParentPriority;

exit:
    return error;
}

Error MleRouter::GetMaxChildTimeout(uint32_t &aTimeout) const
{
    Error error = kErrorNotFound;

    aTimeout = 0;

    VerifyOrExit(IsRouterOrLeader(), error = kErrorInvalidState);

    for (Child &child : Get<ChildTable>().Iterate(Child::kInStateValid))
    {
        if (child.IsFullThreadDevice())
        {
            continue;
        }

        if (child.GetTimeout() > aTimeout)
        {
            aTimeout = child.GetTimeout();
        }

        error = kErrorNone;
    }

exit:
    return error;
}

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
void MleRouter::HandleTimeSync(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, const Neighbor *aNeighbor)
{
    Log(kMessageReceive, kTypeTimeSync, aMessageInfo.GetPeerAddr());

    VerifyOrExit(aNeighbor && aNeighbor->IsStateValid());

    Get<TimeSync>().HandleTimeSyncMessage(aMessage);

exit:
    return;
}

Error MleRouter::SendTimeSync(void)
{
    Error        error = kErrorNone;
    Ip6::Address destination;
    Message *    message = nullptr;

    VerifyOrExit((message = NewMleMessage()) != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = AppendHeader(*message, kCommandTimeSync));

    message->SetTimeSync(true);

    destination.SetToLinkLocalAllNodesMulticast();
    SuccessOrExit(error = SendMessage(*message, destination));

    Log(kMessageSend, kTypeTimeSync, destination);

exit:
    FreeMessageOnError(message, error);
    return error;
}
#endif // OPENTHREAD_CONFIG_TIME_SYNC_ENABLE

} // namespace Mle
} // namespace ot

#endif // OPENTHREAD_FTD
