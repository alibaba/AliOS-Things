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
 *   This file implements a Commissioner role.
 */

#include "commissioner.hpp"

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE

#include <stdio.h>

#include "coap/coap_message.hpp"
#include "common/encoding.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/string.hpp"
#include "meshcop/joiner.hpp"
#include "meshcop/joiner_router.hpp"
#include "meshcop/meshcop.hpp"
#include "meshcop/meshcop_tlvs.hpp"
#include "thread/thread_netif.hpp"
#include "thread/thread_tlvs.hpp"
#include "thread/uri_paths.hpp"

namespace ot {
namespace MeshCoP {

Commissioner::Commissioner(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mActiveJoiner(nullptr)
    , mJoinerPort(0)
    , mJoinerRloc(0)
    , mSessionId(0)
    , mTransmitAttempts(0)
    , mJoinerExpirationTimer(aInstance, HandleJoinerExpirationTimer)
    , mTimer(aInstance, HandleTimer)
    , mRelayReceive(UriPath::kRelayRx, &Commissioner::HandleRelayReceive, this)
    , mDatasetChanged(UriPath::kDatasetChanged, &Commissioner::HandleDatasetChanged, this)
    , mJoinerFinalize(UriPath::kJoinerFinalize, &Commissioner::HandleJoinerFinalize, this)
    , mAnnounceBegin(aInstance)
    , mEnergyScan(aInstance)
    , mPanIdQuery(aInstance)
    , mState(kStateDisabled)
    , mStateCallback(nullptr)
    , mJoinerCallback(nullptr)
    , mCallbackContext(nullptr)
{
    memset(reinterpret_cast<void *>(mJoiners), 0, sizeof(mJoiners));

    mCommissionerAloc.Clear();
    mCommissionerAloc.mPrefixLength       = 64;
    mCommissionerAloc.mPreferred          = true;
    mCommissionerAloc.mValid              = true;
    mCommissionerAloc.mScopeOverride      = Ip6::Address::kRealmLocalScope;
    mCommissionerAloc.mScopeOverrideValid = true;

    mProvisioningUrl[0] = '\0';
}

void Commissioner::SetState(State aState)
{
    State oldState = mState;

    OT_UNUSED_VARIABLE(oldState);

    SuccessOrExit(Get<Notifier>().Update(mState, aState, kEventCommissionerStateChanged));

    otLogInfoMeshCoP("CommissionerState: %s -> %s", StateToString(oldState), StateToString(aState));

    if (mStateCallback)
    {
        mStateCallback(static_cast<otCommissionerState>(mState), mCallbackContext);
    }

exit:
    return;
}

void Commissioner::SignalJoinerEvent(JoinerEvent aEvent, const Joiner *aJoiner) const
{
    otJoinerInfo    joinerInfo;
    Mac::ExtAddress joinerId;
    bool            noJoinerId = false;

    VerifyOrExit((mJoinerCallback != nullptr) && (aJoiner != nullptr));

    aJoiner->CopyToJoinerInfo(joinerInfo);

    if (aJoiner->mType == Joiner::kTypeEui64)
    {
        ComputeJoinerId(aJoiner->mSharedId.mEui64, joinerId);
    }
    else if (aJoiner == mActiveJoiner)
    {
        mJoinerIid.ConvertToExtAddress(joinerId);
    }
    else
    {
        noJoinerId = true;
    }

    mJoinerCallback(static_cast<otCommissionerJoinerEvent>(aEvent), &joinerInfo, noJoinerId ? nullptr : &joinerId,
                    mCallbackContext);

exit:
    return;
}

void Commissioner::AddCoapResources(void)
{
    Get<Tmf::Agent>().AddResource(mRelayReceive);
    Get<Tmf::Agent>().AddResource(mDatasetChanged);
    Get<Coap::CoapSecure>().AddResource(mJoinerFinalize);
}

void Commissioner::RemoveCoapResources(void)
{
    Get<Tmf::Agent>().RemoveResource(mRelayReceive);
    Get<Tmf::Agent>().RemoveResource(mDatasetChanged);
    Get<Coap::CoapSecure>().RemoveResource(mJoinerFinalize);
}

void Commissioner::HandleCoapsConnected(bool aConnected, void *aContext)
{
    static_cast<Commissioner *>(aContext)->HandleCoapsConnected(aConnected);
}

void Commissioner::HandleCoapsConnected(bool aConnected)
{
    SignalJoinerEvent(aConnected ? kJoinerEventConnected : kJoinerEventEnd, mActiveJoiner);
}

Commissioner::Joiner *Commissioner::GetUnusedJoinerEntry(void)
{
    Joiner *rval = nullptr;

    for (Joiner &joiner : mJoiners)
    {
        if (joiner.mType == Joiner::kTypeUnused)
        {
            rval = &joiner;
            break;
        }
    }

    return rval;
}

Commissioner::Joiner *Commissioner::FindJoinerEntry(const Mac::ExtAddress *aEui64)
{
    Joiner *rval = nullptr;

    for (Joiner &joiner : mJoiners)
    {
        switch (joiner.mType)
        {
        case Joiner::kTypeUnused:
        case Joiner::kTypeDiscerner:
            break;

        case Joiner::kTypeAny:
            if (aEui64 == nullptr)
            {
                ExitNow(rval = &joiner);
            }
            break;

        case Joiner::kTypeEui64:
            if ((aEui64 != nullptr) && (joiner.mSharedId.mEui64 == *aEui64))
            {
                ExitNow(rval = &joiner);
            }
            break;
        }
    }

exit:
    return rval;
}

Commissioner::Joiner *Commissioner::FindJoinerEntry(const JoinerDiscerner &aDiscerner)
{
    Joiner *rval = nullptr;

    for (Joiner &joiner : mJoiners)
    {
        if ((joiner.mType == Joiner::kTypeDiscerner) && (aDiscerner == joiner.mSharedId.mDiscerner))
        {
            rval = &joiner;
            break;
        }
    }

    return rval;
}

Commissioner::Joiner *Commissioner::FindBestMatchingJoinerEntry(const Mac::ExtAddress &aReceivedJoinerId)
{
    Joiner *        best = nullptr;
    Mac::ExtAddress joinerId;

    // Prefer a full Joiner ID match, if not found use the entry
    // accepting any joiner.

    for (Joiner &joiner : mJoiners)
    {
        switch (joiner.mType)
        {
        case Joiner::kTypeUnused:
            break;

        case Joiner::kTypeAny:
            if (best == nullptr)
            {
                best = &joiner;
            }
            break;

        case Joiner::kTypeEui64:
            ComputeJoinerId(joiner.mSharedId.mEui64, joinerId);
            if (joinerId == aReceivedJoinerId)
            {
                ExitNow(best = &joiner);
            }
            break;

        case Joiner::kTypeDiscerner:
            if (joiner.mSharedId.mDiscerner.Matches(aReceivedJoinerId))
            {
                if ((best == nullptr) ||
                    ((best->mType == Joiner::kTypeDiscerner) &&
                     (best->mSharedId.mDiscerner.GetLength() < joiner.mSharedId.mDiscerner.GetLength())))
                {
                    best = &joiner;
                }
            }
            break;
        }
    }

exit:
    return best;
}

void Commissioner::RemoveJoinerEntry(Commissioner::Joiner &aJoiner)
{
    // Create a copy of `aJoiner` to use for signaling joiner event
    // and logging after the entry is removed. This ensures the joiner
    // event callback is invoked after all states are cleared.

    Joiner joinerCopy = aJoiner;

    aJoiner.mType = Joiner::kTypeUnused;

    if (&aJoiner == mActiveJoiner)
    {
        mActiveJoiner = nullptr;
    }

    UpdateJoinerExpirationTimer();

    SendCommissionerSet();

    LogJoinerEntry("Removed", joinerCopy);
    SignalJoinerEvent(kJoinerEventRemoved, &joinerCopy);
}

Error Commissioner::Start(otCommissionerStateCallback  aStateCallback,
                          otCommissionerJoinerCallback aJoinerCallback,
                          void *                       aCallbackContext)
{
    Error error = kErrorNone;

    VerifyOrExit(Get<Mle::MleRouter>().IsAttached(), error = kErrorInvalidState);
    VerifyOrExit(mState == kStateDisabled, error = kErrorAlready);

#if OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
    Get<MeshCoP::BorderAgent>().Stop();
#endif

    SuccessOrExit(error = Get<Coap::CoapSecure>().Start(SendRelayTransmit, this));
    Get<Coap::CoapSecure>().SetConnectedCallback(&Commissioner::HandleCoapsConnected, this);

    mStateCallback    = aStateCallback;
    mJoinerCallback   = aJoinerCallback;
    mCallbackContext  = aCallbackContext;
    mTransmitAttempts = 0;

    SuccessOrExit(error = SendPetition());
    SetState(kStatePetition);

exit:
    if ((error != kErrorNone) && (error != kErrorAlready))
    {
        Get<Coap::CoapSecure>().Stop();
    }

    LogError("start commissioner", error);
    return error;
}

Error Commissioner::Stop(bool aResign)
{
    Error error      = kErrorNone;
    bool  needResign = false;

    VerifyOrExit(mState != kStateDisabled, error = kErrorAlready);

    Get<Coap::CoapSecure>().Stop();

    if (mState == kStateActive)
    {
        Get<ThreadNetif>().RemoveUnicastAddress(mCommissionerAloc);
        RemoveCoapResources();
        ClearJoiners();
        needResign = true;
    }
    else if (mState == kStatePetition)
    {
        mTransmitAttempts = 0;
    }

    mTimer.Stop();

    SetState(kStateDisabled);

    if (needResign && aResign)
    {
        SendKeepAlive();
    }

#if OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
    Get<MeshCoP::BorderAgent>().Start();
#endif

exit:
    LogError("stop commissioner", error);
    return error;
}

void Commissioner::ComputeBloomFilter(SteeringData &aSteeringData) const
{
    Mac::ExtAddress joinerId;

    aSteeringData.Init();

    for (const Joiner &joiner : mJoiners)
    {
        switch (joiner.mType)
        {
        case Joiner::kTypeUnused:
            break;

        case Joiner::kTypeEui64:
            ComputeJoinerId(joiner.mSharedId.mEui64, joinerId);
            aSteeringData.UpdateBloomFilter(joinerId);
            break;

        case Joiner::kTypeDiscerner:
            aSteeringData.UpdateBloomFilter(joiner.mSharedId.mDiscerner);
            break;

        case Joiner::kTypeAny:
            aSteeringData.SetToPermitAllJoiners();
            ExitNow();
        }
    }

exit:
    return;
}

void Commissioner::SendCommissionerSet(void)
{
    Error                  error = kErrorNone;
    otCommissioningDataset dataset;

    VerifyOrExit(mState == kStateActive, error = kErrorInvalidState);

    memset(&dataset, 0, sizeof(dataset));

    dataset.mSessionId      = mSessionId;
    dataset.mIsSessionIdSet = true;

    ComputeBloomFilter(static_cast<SteeringData &>(dataset.mSteeringData));
    dataset.mIsSteeringDataSet = true;

    error = SendMgmtCommissionerSetRequest(dataset, nullptr, 0);

exit:
    LogError("send MGMT_COMMISSIONER_SET.req", error);
}

void Commissioner::ClearJoiners(void)
{
    for (Joiner &joiner : mJoiners)
    {
        joiner.mType = Joiner::kTypeUnused;
    }

    SendCommissionerSet();
}

Error Commissioner::AddJoiner(const Mac::ExtAddress *aEui64,
                              const JoinerDiscerner *aDiscerner,
                              const char *           aPskd,
                              uint32_t               aTimeout)
{
    Error   error = kErrorNone;
    Joiner *joiner;

    VerifyOrExit(mState == kStateActive, error = kErrorInvalidState);

    if (aDiscerner != nullptr)
    {
        VerifyOrExit(aDiscerner->IsValid(), error = kErrorInvalidArgs);
        joiner = FindJoinerEntry(*aDiscerner);
    }
    else
    {
        joiner = FindJoinerEntry(aEui64);
    }

    if (joiner == nullptr)
    {
        joiner = GetUnusedJoinerEntry();
    }

    VerifyOrExit(joiner != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = joiner->mPskd.SetFrom(aPskd));

    if (aDiscerner != nullptr)
    {
        joiner->mType                = Joiner::kTypeDiscerner;
        joiner->mSharedId.mDiscerner = *aDiscerner;
    }
    else if (aEui64 != nullptr)
    {
        joiner->mType            = Joiner::kTypeEui64;
        joiner->mSharedId.mEui64 = *aEui64;
    }
    else
    {
        joiner->mType = Joiner::kTypeAny;
    }

    joiner->mExpirationTime = TimerMilli::GetNow() + Time::SecToMsec(aTimeout);

    UpdateJoinerExpirationTimer();

    SendCommissionerSet();

    LogJoinerEntry("Added", *joiner);

exit:
    return error;
}

void Commissioner::Joiner::CopyToJoinerInfo(otJoinerInfo &aJoiner) const
{
    memset(&aJoiner, 0, sizeof(aJoiner));

    switch (mType)
    {
    case kTypeAny:
        aJoiner.mType = OT_JOINER_INFO_TYPE_ANY;
        break;

    case kTypeEui64:
        aJoiner.mType            = OT_JOINER_INFO_TYPE_EUI64;
        aJoiner.mSharedId.mEui64 = mSharedId.mEui64;
        break;

    case kTypeDiscerner:
        aJoiner.mType                = OT_JOINER_INFO_TYPE_DISCERNER;
        aJoiner.mSharedId.mDiscerner = mSharedId.mDiscerner;
        break;

    case kTypeUnused:
        ExitNow();
    }

    aJoiner.mPskd           = mPskd;
    aJoiner.mExpirationTime = mExpirationTime - TimerMilli::GetNow();

exit:
    return;
}

Error Commissioner::GetNextJoinerInfo(uint16_t &aIterator, otJoinerInfo &aJoinerInfo) const
{
    Error error = kErrorNone;

    while (aIterator < OT_ARRAY_LENGTH(mJoiners))
    {
        const Joiner &joiner = mJoiners[aIterator++];

        if (joiner.mType != Joiner::kTypeUnused)
        {
            joiner.CopyToJoinerInfo(aJoinerInfo);
            ExitNow();
        }
    }

    error = kErrorNotFound;

exit:
    return error;
}

Error Commissioner::RemoveJoiner(const Mac::ExtAddress *aEui64, const JoinerDiscerner *aDiscerner, uint32_t aDelay)
{
    Error   error = kErrorNone;
    Joiner *joiner;

    VerifyOrExit(mState == kStateActive, error = kErrorInvalidState);

    if (aDiscerner != nullptr)
    {
        VerifyOrExit(aDiscerner->IsValid(), error = kErrorInvalidArgs);
        joiner = FindJoinerEntry(*aDiscerner);
    }
    else
    {
        joiner = FindJoinerEntry(aEui64);
    }

    VerifyOrExit(joiner != nullptr, error = kErrorNotFound);

    RemoveJoiner(*joiner, aDelay);

exit:
    return error;
}

void Commissioner::RemoveJoiner(Joiner &aJoiner, uint32_t aDelay)
{
    if (aDelay > 0)
    {
        TimeMilli newExpirationTime = TimerMilli::GetNow() + Time::SecToMsec(aDelay);

        if (aJoiner.mExpirationTime > newExpirationTime)
        {
            aJoiner.mExpirationTime = newExpirationTime;
            UpdateJoinerExpirationTimer();
        }
    }
    else
    {
        RemoveJoinerEntry(aJoiner);
    }
}

Error Commissioner::SetProvisioningUrl(const char *aProvisioningUrl)
{
    Error   error = kErrorNone;
    uint8_t len;

    if (aProvisioningUrl == nullptr)
    {
        mProvisioningUrl[0] = '\0';
        ExitNow();
    }

    VerifyOrExit(IsValidUtf8String(aProvisioningUrl), error = kErrorInvalidArgs);

    len = static_cast<uint8_t>(StringLength(aProvisioningUrl, sizeof(mProvisioningUrl)));

    VerifyOrExit(len < sizeof(mProvisioningUrl), error = kErrorInvalidArgs);

    memcpy(mProvisioningUrl, aProvisioningUrl, len);
    mProvisioningUrl[len] = '\0';

exit:
    return error;
}

void Commissioner::HandleTimer(Timer &aTimer)
{
    aTimer.Get<Commissioner>().HandleTimer();
}

void Commissioner::HandleTimer(void)
{
    switch (mState)
    {
    case kStateDisabled:
        break;

    case kStatePetition:
        IgnoreError(SendPetition());
        break;

    case kStateActive:
        SendKeepAlive();
        break;
    }
}

void Commissioner::HandleJoinerExpirationTimer(Timer &aTimer)
{
    aTimer.Get<Commissioner>().HandleJoinerExpirationTimer();
}

void Commissioner::HandleJoinerExpirationTimer(void)
{
    TimeMilli now = TimerMilli::GetNow();

    for (Joiner &joiner : mJoiners)
    {
        if (joiner.mType == Joiner::kTypeUnused)
        {
            continue;
        }

        if (joiner.mExpirationTime <= now)
        {
            otLogDebgMeshCoP("removing joiner due to timeout or successfully joined");
            RemoveJoinerEntry(joiner);
        }
    }

    UpdateJoinerExpirationTimer();
}

void Commissioner::UpdateJoinerExpirationTimer(void)
{
    TimeMilli now  = TimerMilli::GetNow();
    TimeMilli next = now.GetDistantFuture();

    for (Joiner &joiner : mJoiners)
    {
        if (joiner.mType == Joiner::kTypeUnused)
        {
            continue;
        }

        if (joiner.mExpirationTime <= now)
        {
            next = now;
        }
        else if (joiner.mExpirationTime < next)
        {
            next = joiner.mExpirationTime;
        }
    }

    if (next < now.GetDistantFuture())
    {
        mJoinerExpirationTimer.FireAt(next);
    }
    else
    {
        mJoinerExpirationTimer.Stop();
    }
}

Error Commissioner::SendMgmtCommissionerGetRequest(const uint8_t *aTlvs, uint8_t aLength)
{
    Error            error = kErrorNone;
    Coap::Message *  message;
    Ip6::MessageInfo messageInfo;
    MeshCoP::Tlv     tlv;

    VerifyOrExit((message = NewMeshCoPMessage(Get<Tmf::Agent>())) != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = message->InitAsConfirmablePost(UriPath::kCommissionerGet));

    if (aLength > 0)
    {
        SuccessOrExit(error = message->SetPayloadMarker());
    }

    if (aLength > 0)
    {
        tlv.SetType(MeshCoP::Tlv::kGet);
        tlv.SetLength(aLength);
        SuccessOrExit(error = message->Append(tlv));
        SuccessOrExit(error = message->AppendBytes(aTlvs, aLength));
    }

    messageInfo.SetSockAddr(Get<Mle::MleRouter>().GetMeshLocal16());
    SuccessOrExit(error = Get<Mle::MleRouter>().GetLeaderAloc(messageInfo.GetPeerAddr()));
    messageInfo.SetPeerPort(Tmf::kUdpPort);
    SuccessOrExit(error = Get<Tmf::Agent>().SendMessage(*message, messageInfo,
                                                        Commissioner::HandleMgmtCommissionerGetResponse, this));

    otLogInfoMeshCoP("sent MGMT_COMMISSIONER_GET.req to leader");

exit:
    FreeMessageOnError(message, error);
    return error;
}

void Commissioner::HandleMgmtCommissionerGetResponse(void *               aContext,
                                                     otMessage *          aMessage,
                                                     const otMessageInfo *aMessageInfo,
                                                     Error                aResult)
{
    static_cast<Commissioner *>(aContext)->HandleMgmtCommissionerGetResponse(
        static_cast<Coap::Message *>(aMessage), static_cast<const Ip6::MessageInfo *>(aMessageInfo), aResult);
}

void Commissioner::HandleMgmtCommissionerGetResponse(Coap::Message *         aMessage,
                                                     const Ip6::MessageInfo *aMessageInfo,
                                                     Error                   aResult)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    VerifyOrExit(aResult == kErrorNone && aMessage->GetCode() == Coap::kCodeChanged);
    otLogInfoMeshCoP("received MGMT_COMMISSIONER_GET response");

exit:
    return;
}

Error Commissioner::SendMgmtCommissionerSetRequest(const otCommissioningDataset &aDataset,
                                                   const uint8_t *               aTlvs,
                                                   uint8_t                       aLength)
{
    Error            error = kErrorNone;
    Coap::Message *  message;
    Ip6::MessageInfo messageInfo;

    VerifyOrExit((message = NewMeshCoPMessage(Get<Tmf::Agent>())) != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = message->InitAsConfirmablePost(UriPath::kCommissionerSet));
    SuccessOrExit(error = message->SetPayloadMarker());

    if (aDataset.mIsLocatorSet)
    {
        SuccessOrExit(error = Tlv::Append<MeshCoP::BorderAgentLocatorTlv>(*message, aDataset.mLocator));
    }

    if (aDataset.mIsSessionIdSet)
    {
        SuccessOrExit(error = Tlv::Append<MeshCoP::CommissionerSessionIdTlv>(*message, aDataset.mSessionId));
    }

    if (aDataset.mIsSteeringDataSet)
    {
        SuccessOrExit(
            error = Tlv::Append<SteeringDataTlv>(*message, aDataset.mSteeringData.m8, aDataset.mSteeringData.mLength));
    }

    if (aDataset.mIsJoinerUdpPortSet)
    {
        SuccessOrExit(error = Tlv::Append<JoinerUdpPortTlv>(*message, aDataset.mJoinerUdpPort));
    }

    if (aLength > 0)
    {
        SuccessOrExit(error = message->AppendBytes(aTlvs, aLength));
    }

    messageInfo.SetSockAddr(Get<Mle::MleRouter>().GetMeshLocal16());
    SuccessOrExit(error = Get<Mle::MleRouter>().GetLeaderAloc(messageInfo.GetPeerAddr()));
    messageInfo.SetPeerPort(Tmf::kUdpPort);
    SuccessOrExit(error = Get<Tmf::Agent>().SendMessage(*message, messageInfo,
                                                        Commissioner::HandleMgmtCommissionerSetResponse, this));

    otLogInfoMeshCoP("sent MGMT_COMMISSIONER_SET.req to leader");

exit:
    FreeMessageOnError(message, error);
    return error;
}

void Commissioner::HandleMgmtCommissionerSetResponse(void *               aContext,
                                                     otMessage *          aMessage,
                                                     const otMessageInfo *aMessageInfo,
                                                     Error                aResult)
{
    static_cast<Commissioner *>(aContext)->HandleMgmtCommissionerSetResponse(
        static_cast<Coap::Message *>(aMessage), static_cast<const Ip6::MessageInfo *>(aMessageInfo), aResult);
}

void Commissioner::HandleMgmtCommissionerSetResponse(Coap::Message *         aMessage,
                                                     const Ip6::MessageInfo *aMessageInfo,
                                                     Error                   aResult)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    VerifyOrExit(aResult == kErrorNone && aMessage->GetCode() == Coap::kCodeChanged);
    otLogInfoMeshCoP("received MGMT_COMMISSIONER_SET response");

exit:
    return;
}

Error Commissioner::SendPetition(void)
{
    Error             error   = kErrorNone;
    Coap::Message *   message = nullptr;
    Ip6::MessageInfo  messageInfo;
    CommissionerIdTlv commissionerId;

    mTransmitAttempts++;

    VerifyOrExit((message = NewMeshCoPMessage(Get<Tmf::Agent>())) != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = message->InitAsConfirmablePost(UriPath::kLeaderPetition));
    SuccessOrExit(error = message->SetPayloadMarker());

    commissionerId.Init();
    commissionerId.SetCommissionerId("OpenThread Commissioner");

    SuccessOrExit(error = commissionerId.AppendTo(*message));

    SuccessOrExit(error = Get<Mle::MleRouter>().GetLeaderAloc(messageInfo.GetPeerAddr()));
    messageInfo.SetPeerPort(Tmf::kUdpPort);
    messageInfo.SetSockAddr(Get<Mle::MleRouter>().GetMeshLocal16());
    SuccessOrExit(
        error = Get<Tmf::Agent>().SendMessage(*message, messageInfo, Commissioner::HandleLeaderPetitionResponse, this));

    otLogInfoMeshCoP("sent petition");

exit:
    FreeMessageOnError(message, error);
    return error;
}

void Commissioner::HandleLeaderPetitionResponse(void *               aContext,
                                                otMessage *          aMessage,
                                                const otMessageInfo *aMessageInfo,
                                                Error                aResult)
{
    static_cast<Commissioner *>(aContext)->HandleLeaderPetitionResponse(
        static_cast<Coap::Message *>(aMessage), static_cast<const Ip6::MessageInfo *>(aMessageInfo), aResult);
}

void Commissioner::HandleLeaderPetitionResponse(Coap::Message *         aMessage,
                                                const Ip6::MessageInfo *aMessageInfo,
                                                Error                   aResult)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    uint8_t state;
    bool    retransmit = false;

    VerifyOrExit(mState != kStateActive);
    VerifyOrExit(aResult == kErrorNone && aMessage->GetCode() == Coap::kCodeChanged,
                 retransmit = (mState == kStatePetition));

    otLogInfoMeshCoP("received Leader Petition response");

    SuccessOrExit(Tlv::Find<StateTlv>(*aMessage, state));
    VerifyOrExit(state == StateTlv::kAccept, IgnoreError(Stop(/* aResign */ false)));

    SuccessOrExit(Tlv::Find<CommissionerSessionIdTlv>(*aMessage, mSessionId));

    // reject this session by sending KeepAlive reject if commissioner is in disabled state
    // this could happen if commissioner is stopped by API during petitioning
    if (mState == kStateDisabled)
    {
        SendKeepAlive(mSessionId);
        ExitNow();
    }

    IgnoreError(Get<Mle::MleRouter>().GetCommissionerAloc(mCommissionerAloc.GetAddress(), mSessionId));
    Get<ThreadNetif>().AddUnicastAddress(mCommissionerAloc);

    AddCoapResources();
    SetState(kStateActive);

    mTransmitAttempts = 0;
    mTimer.Start(Time::SecToMsec(kKeepAliveTimeout) / 2);

exit:

    if (retransmit)
    {
        if (mTransmitAttempts >= kPetitionRetryCount)
        {
            IgnoreError(Stop(/* aResign */ false));
        }
        else
        {
            mTimer.Start(Time::SecToMsec(kPetitionRetryDelay));
        }
    }
}

void Commissioner::SendKeepAlive(void)
{
    SendKeepAlive(mSessionId);
}

void Commissioner::SendKeepAlive(uint16_t aSessionId)
{
    Error            error   = kErrorNone;
    Coap::Message *  message = nullptr;
    Ip6::MessageInfo messageInfo;

    VerifyOrExit((message = NewMeshCoPMessage(Get<Tmf::Agent>())) != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = message->InitAsConfirmablePost(UriPath::kLeaderKeepAlive));
    SuccessOrExit(error = message->SetPayloadMarker());

    SuccessOrExit(
        error = Tlv::Append<StateTlv>(*message, (mState == kStateActive) ? StateTlv::kAccept : StateTlv::kReject));

    SuccessOrExit(error = Tlv::Append<CommissionerSessionIdTlv>(*message, aSessionId));

    messageInfo.SetSockAddr(Get<Mle::MleRouter>().GetMeshLocal16());
    SuccessOrExit(error = Get<Mle::MleRouter>().GetLeaderAloc(messageInfo.GetPeerAddr()));
    messageInfo.SetPeerPort(Tmf::kUdpPort);
    SuccessOrExit(error = Get<Tmf::Agent>().SendMessage(*message, messageInfo,
                                                        Commissioner::HandleLeaderKeepAliveResponse, this));

    otLogInfoMeshCoP("sent keep alive");

exit:
    FreeMessageOnError(message, error);
    LogError("send keep alive", error);
}

void Commissioner::HandleLeaderKeepAliveResponse(void *               aContext,
                                                 otMessage *          aMessage,
                                                 const otMessageInfo *aMessageInfo,
                                                 Error                aResult)
{
    static_cast<Commissioner *>(aContext)->HandleLeaderKeepAliveResponse(
        static_cast<Coap::Message *>(aMessage), static_cast<const Ip6::MessageInfo *>(aMessageInfo), aResult);
}

void Commissioner::HandleLeaderKeepAliveResponse(Coap::Message *         aMessage,
                                                 const Ip6::MessageInfo *aMessageInfo,
                                                 Error                   aResult)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    uint8_t state;

    VerifyOrExit(mState == kStateActive);
    VerifyOrExit(aResult == kErrorNone && aMessage->GetCode() == Coap::kCodeChanged,
                 IgnoreError(Stop(/* aResign */ false)));

    otLogInfoMeshCoP("received Leader keep-alive response");

    SuccessOrExit(Tlv::Find<StateTlv>(*aMessage, state));
    VerifyOrExit(state == StateTlv::kAccept, IgnoreError(Stop(/* aResign */ false)));

    mTimer.Start(Time::SecToMsec(kKeepAliveTimeout) / 2);

exit:
    return;
}

void Commissioner::HandleRelayReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<Commissioner *>(aContext)->HandleRelayReceive(*static_cast<Coap::Message *>(aMessage),
                                                              *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

void Commissioner::HandleRelayReceive(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    Error                    error;
    uint16_t                 joinerPort;
    Ip6::InterfaceIdentifier joinerIid;
    uint16_t                 joinerRloc;
    Ip6::MessageInfo         joinerMessageInfo;
    uint16_t                 offset;
    uint16_t                 length;

    VerifyOrExit(mState == kStateActive, error = kErrorInvalidState);

    VerifyOrExit(aMessage.IsNonConfirmablePostRequest());

    SuccessOrExit(error = Tlv::Find<JoinerUdpPortTlv>(aMessage, joinerPort));
    SuccessOrExit(error = Tlv::Find<JoinerIidTlv>(aMessage, joinerIid));
    SuccessOrExit(error = Tlv::Find<JoinerRouterLocatorTlv>(aMessage, joinerRloc));

    SuccessOrExit(error = Tlv::FindTlvValueOffset(aMessage, Tlv::kJoinerDtlsEncapsulation, offset, length));
    VerifyOrExit(length <= aMessage.GetLength() - offset, error = kErrorParse);

    if (!Get<Coap::CoapSecure>().IsConnectionActive())
    {
        Mac::ExtAddress receivedId;
        Joiner *        joiner;

        mJoinerIid = joinerIid;
        mJoinerIid.ConvertToExtAddress(receivedId);

        joiner = FindBestMatchingJoinerEntry(receivedId);
        VerifyOrExit(joiner != nullptr);

        Get<Coap::CoapSecure>().SetPsk(joiner->mPskd);
        mActiveJoiner = joiner;

        LogJoinerEntry("Starting new session with", *joiner);
        SignalJoinerEvent(kJoinerEventStart, joiner);
    }
    else
    {
        VerifyOrExit(mJoinerIid == joinerIid);
    }

    mJoinerPort = joinerPort;
    mJoinerRloc = joinerRloc;

    otLogInfoMeshCoP("Received Relay Receive (%s, 0x%04x)", mJoinerIid.ToString().AsCString(), mJoinerRloc);

    aMessage.SetOffset(offset);
    SuccessOrExit(error = aMessage.SetLength(offset + length));

    joinerMessageInfo.SetPeerAddr(Get<Mle::MleRouter>().GetMeshLocal64());
    joinerMessageInfo.GetPeerAddr().SetIid(mJoinerIid);
    joinerMessageInfo.SetPeerPort(mJoinerPort);

    Get<Coap::CoapSecure>().HandleUdpReceive(aMessage, joinerMessageInfo);

exit:
    return;
}

void Commissioner::HandleDatasetChanged(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<Commissioner *>(aContext)->HandleDatasetChanged(*static_cast<Coap::Message *>(aMessage),
                                                                *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

void Commissioner::HandleDatasetChanged(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    VerifyOrExit(aMessage.IsConfirmablePostRequest());

    otLogInfoMeshCoP("received dataset changed");

    SuccessOrExit(Get<Tmf::Agent>().SendEmptyAck(aMessage, aMessageInfo));

    otLogInfoMeshCoP("sent dataset changed acknowledgment");

exit:
    return;
}

void Commissioner::HandleJoinerFinalize(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<Commissioner *>(aContext)->HandleJoinerFinalize(*static_cast<Coap::Message *>(aMessage),
                                                                *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

void Commissioner::HandleJoinerFinalize(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    StateTlv::State    state = StateTlv::kAccept;
    ProvisioningUrlTlv provisioningUrl;

    otLogInfoMeshCoP("received joiner finalize");

    if (Tlv::FindTlv(aMessage, provisioningUrl) == kErrorNone)
    {
        uint8_t len = static_cast<uint8_t>(StringLength(mProvisioningUrl, sizeof(mProvisioningUrl)));

        if ((provisioningUrl.GetProvisioningUrlLength() != len) ||
            !memcmp(provisioningUrl.GetProvisioningUrl(), mProvisioningUrl, len))
        {
            state = StateTlv::kReject;
        }
    }

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    if (aMessage.GetLength() <= OPENTHREAD_CONFIG_MESSAGE_BUFFER_SIZE)
    {
        uint8_t buf[OPENTHREAD_CONFIG_MESSAGE_BUFFER_SIZE];

        aMessage.ReadBytes(aMessage.GetOffset(), buf, aMessage.GetLength() - aMessage.GetOffset());
        otDumpCertMeshCoP("[THCI] direction=recv | type=JOIN_FIN.req |", buf,
                          aMessage.GetLength() - aMessage.GetOffset());
    }
#endif

    SendJoinFinalizeResponse(aMessage, state);
}

void Commissioner::SendJoinFinalizeResponse(const Coap::Message &aRequest, StateTlv::State aState)
{
    Error            error = kErrorNone;
    Ip6::MessageInfo joinerMessageInfo;
    Coap::Message *  message;

    VerifyOrExit((message = NewMeshCoPMessage(Get<Coap::CoapSecure>())) != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = message->SetDefaultResponseHeader(aRequest));
    SuccessOrExit(error = message->SetPayloadMarker());
    message->SetOffset(message->GetLength());
    message->SetSubType(Message::kSubTypeJoinerFinalizeResponse);

    SuccessOrExit(error = Tlv::Append<StateTlv>(*message, aState));

    joinerMessageInfo.SetPeerAddr(Get<Mle::MleRouter>().GetMeshLocal64());
    joinerMessageInfo.GetPeerAddr().SetIid(mJoinerIid);
    joinerMessageInfo.SetPeerPort(mJoinerPort);

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    uint8_t buf[OPENTHREAD_CONFIG_MESSAGE_BUFFER_SIZE];

    VerifyOrExit(message->GetLength() <= sizeof(buf));
    message->ReadBytes(message->GetOffset(), buf, message->GetLength() - message->GetOffset());
    otDumpCertMeshCoP("[THCI] direction=send | type=JOIN_FIN.rsp |", buf, message->GetLength() - message->GetOffset());
#endif

    SuccessOrExit(error = Get<Coap::CoapSecure>().SendMessage(*message, joinerMessageInfo));

    SignalJoinerEvent(kJoinerEventFinalize, mActiveJoiner);

    if ((mActiveJoiner != nullptr) && (mActiveJoiner->mType != Joiner::kTypeAny))
    {
        // Remove after kRemoveJoinerDelay (seconds)
        RemoveJoiner(*mActiveJoiner, kRemoveJoinerDelay);
    }

    otLogInfoMeshCoP("sent joiner finalize response");

exit:
    FreeMessageOnError(message, error);
}

Error Commissioner::SendRelayTransmit(void *aContext, Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    return static_cast<Commissioner *>(aContext)->SendRelayTransmit(aMessage, aMessageInfo);
}

Error Commissioner::SendRelayTransmit(Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    Error            error = kErrorNone;
    ExtendedTlv      tlv;
    Coap::Message *  message;
    uint16_t         offset;
    Ip6::MessageInfo messageInfo;
    Kek              kek;

    VerifyOrExit((message = NewMeshCoPMessage(Get<Tmf::Agent>())) != nullptr, error = kErrorNoBufs);

    Get<KeyManager>().ExtractKek(kek);

    message->InitAsNonConfirmablePost();
    SuccessOrExit(error = message->AppendUriPathOptions(UriPath::kRelayTx));
    SuccessOrExit(error = message->SetPayloadMarker());

    SuccessOrExit(error = Tlv::Append<JoinerUdpPortTlv>(*message, mJoinerPort));
    SuccessOrExit(error = Tlv::Append<JoinerIidTlv>(*message, mJoinerIid));
    SuccessOrExit(error = Tlv::Append<JoinerRouterLocatorTlv>(*message, mJoinerRloc));

    if (aMessage.GetSubType() == Message::kSubTypeJoinerFinalizeResponse)
    {
        SuccessOrExit(error = Tlv::Append<JoinerRouterKekTlv>(*message, kek));
    }

    tlv.SetType(Tlv::kJoinerDtlsEncapsulation);
    tlv.SetLength(aMessage.GetLength());
    SuccessOrExit(error = message->Append(tlv));
    offset = message->GetLength();
    SuccessOrExit(error = message->SetLength(offset + aMessage.GetLength()));
    aMessage.CopyTo(0, offset, aMessage.GetLength(), *message);

    messageInfo.SetPeerAddr(Get<Mle::MleRouter>().GetMeshLocal16());
    messageInfo.GetPeerAddr().GetIid().SetLocator(mJoinerRloc);
    messageInfo.SetPeerPort(Tmf::kUdpPort);
    messageInfo.SetSockAddr(Get<Mle::MleRouter>().GetMeshLocal16());

    SuccessOrExit(error = Get<Tmf::Agent>().SendMessage(*message, messageInfo));

    aMessage.Free();

exit:
    FreeMessageOnError(message, error);
    return error;
}

void Commissioner::ApplyMeshLocalPrefix(void)
{
    VerifyOrExit(mState == kStateActive);

    Get<ThreadNetif>().RemoveUnicastAddress(mCommissionerAloc);
    mCommissionerAloc.GetAddress().SetPrefix(Get<Mle::MleRouter>().GetMeshLocalPrefix());
    Get<ThreadNetif>().AddUnicastAddress(mCommissionerAloc);

exit:
    return;
}

// LCOV_EXCL_START

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO) && (OPENTHREAD_CONFIG_LOG_MESHCOP == 1)

const char *Commissioner::StateToString(State aState)
{
    static const char *const kStateStrings[] = {
        "disabled", // (0) kStateDisabled
        "petition", // (1) kStatePetition
        "active",   // (2) kStateActive
    };

    static_assert(kStateDisabled == 0, "kStateDisabled value is incorrect");
    static_assert(kStatePetition == 1, "kStatePetition value is incorrect");
    static_assert(kStateActive == 2, "kStateActive value is incorrect");

    return kStateStrings[aState];
}

void Commissioner::LogJoinerEntry(const char *aAction, const Joiner &aJoiner) const
{
    switch (aJoiner.mType)
    {
    case Joiner::kTypeUnused:
        break;

    case Joiner::kTypeAny:
        otLogInfoMeshCoP("%s Joiner (any, %s)", aAction, aJoiner.mPskd.GetAsCString());
        break;

    case Joiner::kTypeEui64:
        otLogInfoMeshCoP("%s Joiner (eui64:%s, %s)", aAction, aJoiner.mSharedId.mEui64.ToString().AsCString(),
                         aJoiner.mPskd.GetAsCString());
        break;

    case Joiner::kTypeDiscerner:
        otLogInfoMeshCoP("%s Joiner (disc:%s, %s)", aAction, aJoiner.mSharedId.mDiscerner.ToString().AsCString(),
                         aJoiner.mPskd.GetAsCString());
        break;
    }
}

#else

void Commissioner::LogJoinerEntry(const char *, const Joiner &) const
{
}

#endif // (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO) && (OPENTHREAD_CONFIG_LOG_MESHCOP == 1)

// LCOV_EXCL_STOP

} // namespace MeshCoP
} // namespace ot

#endif // OPENTHREAD_FTD && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
