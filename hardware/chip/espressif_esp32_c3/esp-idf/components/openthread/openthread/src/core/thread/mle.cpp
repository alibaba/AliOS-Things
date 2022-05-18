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
 *   This file implements MLE functionality required for the Thread Child, Router and Leader roles.
 */

#include "mle.hpp"

#include <openthread/platform/radio.h>
#include <openthread/platform/time.h>

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/encoding.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/random.hpp"
#include "common/settings.hpp"
#include "crypto/aes_ccm.hpp"
#include "meshcop/meshcop.hpp"
#include "meshcop/meshcop_tlvs.hpp"
#include "net/netif.hpp"
#include "net/udp6.hpp"
#include "thread/address_resolver.hpp"
#include "thread/key_manager.hpp"
#include "thread/link_metrics.hpp"
#include "thread/mle_router.hpp"
#include "thread/thread_netif.hpp"
#include "thread/time_sync_service.hpp"

using ot::Encoding::BigEndian::HostSwap16;

namespace ot {
namespace Mle {

Mle::Mle(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mRetrieveNewNetworkData(false)
    , mRole(kRoleDisabled)
    , mNeighborTable(aInstance)
    , mDeviceMode(DeviceMode::kModeRxOnWhenIdle)
    , mAttachState(kAttachStateIdle)
    , mReattachState(kReattachStop)
    , mAttachCounter(0)
    , mAnnounceDelay(kAnnounceTimeout)
    , mAttachTimer(aInstance, Mle::HandleAttachTimer)
    , mDelayedResponseTimer(aInstance, Mle::HandleDelayedResponseTimer)
    , mMessageTransmissionTimer(aInstance, Mle::HandleMessageTransmissionTimer)
    , mParentLeaderCost(0)
    , mParentRequestMode(kAttachAny)
    , mParentPriority(0)
    , mParentLinkQuality3(0)
    , mParentLinkQuality2(0)
    , mParentLinkQuality1(0)
    , mParentSedBufferSize(0)
    , mParentSedDatagramCount(0)
    , mChildUpdateAttempts(0)
    , mChildUpdateRequestState(kChildUpdateRequestNone)
    , mDataRequestAttempts(0)
    , mDataRequestState(kDataRequestNone)
    , mAddressRegistrationMode(kAppendAllAddresses)
    , mHasRestored(false)
    , mParentLinkMargin(0)
    , mParentIsSingleton(false)
    , mReceivedResponseFromParent(false)
    , mSocket(aInstance)
    , mTimeout(kMleEndDeviceTimeout)
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    , mCslTimeout(OPENTHREAD_CONFIG_CSL_TIMEOUT)
#endif
#if OPENTHREAD_CONFIG_MLE_INFORM_PREVIOUS_PARENT_ON_REATTACH
    , mPreviousParentRloc(Mac::kShortAddrInvalid)
#endif
#if OPENTHREAD_CONFIG_PARENT_SEARCH_ENABLE
    , mParentSearchIsInBackoff(false)
    , mParentSearchBackoffWasCanceled(false)
    , mParentSearchRecentlyDetached(false)
    , mParentSearchBackoffCancelTime(0)
    , mParentSearchTimer(aInstance, Mle::HandleParentSearchTimer)
#endif
    , mAnnounceChannel(0)
    , mAlternateChannel(0)
    , mAlternatePanId(Mac::kPanIdBroadcast)
    , mAlternateTimestamp(0)
    , mParentResponseCb(nullptr)
    , mParentResponseCbContext(nullptr)
{
    MeshLocalPrefix meshLocalPrefix;

    mParent.Init(aInstance);
    mParentCandidate.Init(aInstance);

    mLeaderData.Clear();
    mParentLeaderData.Clear();
    mParent.Clear();
    mParentCandidate.Clear();
    ResetCounters();

    mLinkLocal64.InitAsThreadOrigin(/* aPreferred */ true);
    mLinkLocal64.GetAddress().SetToLinkLocalAddress(Get<Mac::Mac>().GetExtAddress());

    mLeaderAloc.InitAsThreadOriginRealmLocalScope();

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    for (Ip6::Netif::UnicastAddress &serviceAloc : mServiceAlocs)
    {
        serviceAloc.InitAsThreadOriginRealmLocalScope();
        serviceAloc.GetAddress().GetIid().SetLocator(Mac::kShortAddrInvalid);
    }
#endif

    meshLocalPrefix.SetFromExtendedPanId(Get<Mac::Mac>().GetExtendedPanId());

    mMeshLocal64.InitAsThreadOriginRealmLocalScope();
    mMeshLocal64.GetAddress().GetIid().GenerateRandom();

    mMeshLocal16.InitAsThreadOriginRealmLocalScope();
    mMeshLocal16.GetAddress().GetIid().SetToLocator(0);
    mMeshLocal16.mRloc = true;

    // Store RLOC address reference in MPL module.
    Get<Ip6::Mpl>().SetMatchingAddress(mMeshLocal16.GetAddress());

    mLinkLocalAllThreadNodes.Clear();
    mLinkLocalAllThreadNodes.GetAddress().mFields.m16[0] = HostSwap16(0xff32);
    mLinkLocalAllThreadNodes.GetAddress().mFields.m16[7] = HostSwap16(0x0001);

    mRealmLocalAllThreadNodes.Clear();
    mRealmLocalAllThreadNodes.GetAddress().mFields.m16[0] = HostSwap16(0xff33);
    mRealmLocalAllThreadNodes.GetAddress().mFields.m16[7] = HostSwap16(0x0001);

    SetMeshLocalPrefix(meshLocalPrefix);

    // `SetMeshLocalPrefix()` also adds the Mesh-Local EID and subscribes
    // to the Link- and Realm-Local All Thread Nodes multicast addresses.
}

Error Mle::Enable(void)
{
    Error error = kErrorNone;

    UpdateLinkLocalAddress();
    SuccessOrExit(error = mSocket.Open(&Mle::HandleUdpReceive, this));
    SuccessOrExit(error = mSocket.Bind(kUdpPort));

#if OPENTHREAD_CONFIG_PARENT_SEARCH_ENABLE
    StartParentSearchTimer();
#endif
exit:
    return error;
}

void Mle::ScheduleChildUpdateRequest(void)
{
    mChildUpdateRequestState = kChildUpdateRequestPending;
    ScheduleMessageTransmissionTimer();
}

Error Mle::Disable(void)
{
    Error error = kErrorNone;

    Stop(kKeepNetworkDatasets);
    SuccessOrExit(error = mSocket.Close());
    Get<ThreadNetif>().RemoveUnicastAddress(mLinkLocal64);

exit:
    return error;
}

Error Mle::Start(StartMode aMode)
{
    Error error = kErrorNone;

    // cannot bring up the interface if IEEE 802.15.4 promiscuous mode is enabled
    VerifyOrExit(!Get<Radio>().GetPromiscuous(), error = kErrorInvalidState);
    VerifyOrExit(Get<ThreadNetif>().IsUp(), error = kErrorInvalidState);

    if (Get<Mac::Mac>().GetPanId() == Mac::kPanIdBroadcast)
    {
        Get<Mac::Mac>().SetPanId(Mac::GenerateRandomPanId());
    }

    SetStateDetached();

    ApplyMeshLocalPrefix();
    SetRloc16(GetRloc16());

    mAttachCounter = 0;

    Get<KeyManager>().Start();

    if (aMode == kNormalAttach)
    {
        mReattachState = kReattachStart;
    }

    if ((aMode == kAnnounceAttach) || (GetRloc16() == Mac::kShortAddrInvalid))
    {
        IgnoreError(BecomeChild(kAttachAny));
    }
#if OPENTHREAD_FTD
    else if (IsActiveRouter(GetRloc16()))
    {
        if (Get<MleRouter>().BecomeRouter(ThreadStatusTlv::kTooFewRouters) != kErrorNone)
        {
            IgnoreError(BecomeChild(kAttachAny));
        }
    }
#endif
    else
    {
        mChildUpdateAttempts = 0;
        IgnoreError(SendChildUpdateRequest());
    }

exit:
    return error;
}

void Mle::Stop(StopMode aMode)
{
    if (aMode == kUpdateNetworkDatasets)
    {
        Get<MeshCoP::ActiveDataset>().HandleDetach();
        Get<MeshCoP::PendingDataset>().HandleDetach();
    }

    VerifyOrExit(!IsDisabled());

    Get<KeyManager>().Stop();
    SetStateDetached();
    Get<ThreadNetif>().UnsubscribeMulticast(mRealmLocalAllThreadNodes);
    Get<ThreadNetif>().UnsubscribeMulticast(mLinkLocalAllThreadNodes);
    Get<ThreadNetif>().RemoveUnicastAddress(mMeshLocal16);
    Get<ThreadNetif>().RemoveUnicastAddress(mMeshLocal64);

    SetRole(kRoleDisabled);

exit:
    return;
}

void Mle::SetRole(DeviceRole aRole)
{
    DeviceRole oldRole = mRole;

    SuccessOrExit(Get<Notifier>().Update(mRole, aRole, kEventThreadRoleChanged));

    otLogNoteMle("Role %s -> %s", RoleToString(oldRole), RoleToString(mRole));

    switch (mRole)
    {
    case kRoleDisabled:
        mCounters.mDisabledRole++;
        break;
    case kRoleDetached:
        mCounters.mDetachedRole++;
        break;
    case kRoleChild:
        mCounters.mChildRole++;
        break;
    case kRoleRouter:
        mCounters.mRouterRole++;
        break;
    case kRoleLeader:
        mCounters.mLeaderRole++;
        break;
    }

    // If the previous state is disabled, the parent can be in kStateRestored.
    if (!IsChild() && oldRole != kRoleDisabled)
    {
        mParent.SetState(Neighbor::kStateInvalid);
    }

exit:
    OT_UNUSED_VARIABLE(oldRole);
}

void Mle::SetAttachState(AttachState aState)
{
    VerifyOrExit(aState != mAttachState);
    otLogInfoMle("AttachState %s -> %s", AttachStateToString(mAttachState), AttachStateToString(aState));
    mAttachState = aState;

exit:
    return;
}

Error Mle::Restore(void)
{
    Error                 error = kErrorNone;
    Settings::NetworkInfo networkInfo;
    Settings::ParentInfo  parentInfo;

    IgnoreError(Get<MeshCoP::ActiveDataset>().Restore());
    IgnoreError(Get<MeshCoP::PendingDataset>().Restore());

#if OPENTHREAD_CONFIG_DUA_ENABLE
    Get<DuaManager>().Restore();
#endif

    SuccessOrExit(error = Get<Settings>().Read(networkInfo));

    Get<KeyManager>().SetCurrentKeySequence(networkInfo.GetKeySequence());
    Get<KeyManager>().SetMleFrameCounter(networkInfo.GetMleFrameCounter());
    Get<KeyManager>().SetAllMacFrameCounters(networkInfo.GetMacFrameCounter());
    mDeviceMode.Set(networkInfo.GetDeviceMode());

    // force re-attach when version mismatch.
    VerifyOrExit(networkInfo.GetVersion() == kThreadVersion);

    switch (networkInfo.GetRole())
    {
    case kRoleChild:
    case kRoleRouter:
    case kRoleLeader:
        break;

    default:
        ExitNow();
    }

    Get<Mac::Mac>().SetShortAddress(networkInfo.GetRloc16());
    Get<Mac::Mac>().SetExtAddress(networkInfo.GetExtAddress());

    mMeshLocal64.GetAddress().SetIid(networkInfo.GetMeshLocalIid());

    if (networkInfo.GetRloc16() == Mac::kShortAddrInvalid)
    {
        ExitNow();
    }

    if (!IsActiveRouter(networkInfo.GetRloc16()))
    {
        error = Get<Settings>().Read(parentInfo);

        if (error != kErrorNone)
        {
            // If the restored RLOC16 corresponds to an end-device, it
            // is expected that the `ParentInfo` settings to be valid
            // as well. The device can still recover from such an invalid
            // setting by skipping the re-attach ("Child Update Request"
            // exchange) and going through the full attach process.

            otLogWarnMle("Invalid settings - no saved parent info with valid end-device RLOC16 0x%04x",
                         networkInfo.GetRloc16());
            ExitNow();
        }

        mParent.Clear();
        mParent.SetExtAddress(parentInfo.GetExtAddress());
        mParent.SetVersion(static_cast<uint8_t>(parentInfo.GetVersion()));
        mParent.SetDeviceMode(DeviceMode(DeviceMode::kModeFullThreadDevice | DeviceMode::kModeRxOnWhenIdle |
                                         DeviceMode::kModeFullNetworkData));
        mParent.SetRloc16(Rloc16FromRouterId(RouterIdFromRloc16(networkInfo.GetRloc16())));
        mParent.SetState(Neighbor::kStateRestored);

#if OPENTHREAD_CONFIG_MLE_INFORM_PREVIOUS_PARENT_ON_REATTACH
        mPreviousParentRloc = mParent.GetRloc16();
#endif
    }
#if OPENTHREAD_FTD
    else
    {
        Get<MleRouter>().SetRouterId(RouterIdFromRloc16(GetRloc16()));
        Get<MleRouter>().SetPreviousPartitionId(networkInfo.GetPreviousPartitionId());
        Get<ChildTable>().Restore();
    }
#endif

    // Sucessfully restored the network information from non-volatile settings after boot.
    mHasRestored = true;

exit:
    return error;
}

Error Mle::Store(void)
{
    Error                 error = kErrorNone;
    Settings::NetworkInfo networkInfo;

    networkInfo.Init();

    if (IsAttached())
    {
        // Only update network information while we are attached to
        // avoid losing/overwriting previous information when a reboot
        // occurs after a message is sent but before attaching.

        networkInfo.SetRole(mRole);
        networkInfo.SetRloc16(GetRloc16());
        networkInfo.SetPreviousPartitionId(mLeaderData.GetPartitionId());
        networkInfo.SetExtAddress(Get<Mac::Mac>().GetExtAddress());
        networkInfo.SetMeshLocalIid(mMeshLocal64.GetAddress().GetIid());
        networkInfo.SetVersion(kThreadVersion);

        if (IsChild())
        {
            Settings::ParentInfo parentInfo;

            parentInfo.Init();
            parentInfo.SetExtAddress(mParent.GetExtAddress());
            parentInfo.SetVersion(mParent.GetVersion());

            SuccessOrExit(error = Get<Settings>().Save(parentInfo));
        }
    }
    else
    {
        // When not attached, read out any previous saved `NetworkInfo`.
        // If there is none, it indicates that device was never attached
        // before. In that case, no need to save any info (note that on
        // a device reset the MLE/MAC frame counters would reset but
        // device also starts with a new randomly generated extended
        // address. If there is a previously saved `NetworkInfo`, we
        // just update the key sequence and MAC and MLE frame counters.

        SuccessOrExit(Get<Settings>().Read(networkInfo));
    }

    networkInfo.SetKeySequence(Get<KeyManager>().GetCurrentKeySequence());
    networkInfo.SetMleFrameCounter(Get<KeyManager>().GetMleFrameCounter() +
                                   OPENTHREAD_CONFIG_STORE_FRAME_COUNTER_AHEAD);
    networkInfo.SetMacFrameCounter(Get<KeyManager>().GetMaximumMacFrameCounter() +
                                   OPENTHREAD_CONFIG_STORE_FRAME_COUNTER_AHEAD);
    networkInfo.SetDeviceMode(mDeviceMode.Get());

    SuccessOrExit(error = Get<Settings>().Save(networkInfo));

    Get<KeyManager>().SetStoredMleFrameCounter(networkInfo.GetMleFrameCounter());
    Get<KeyManager>().SetStoredMacFrameCounter(networkInfo.GetMacFrameCounter());

    otLogDebgMle("Store Network Information");

exit:
    return error;
}

Error Mle::BecomeDetached(void)
{
    Error error = kErrorNone;

    VerifyOrExit(!IsDisabled(), error = kErrorInvalidState);

    // In case role is already detached and attach state is `kAttachStateStart`
    // (i.e., waiting to start an attach attempt), there is no need to make any
    // changes.

    VerifyOrExit(!IsDetached() || mAttachState != kAttachStateStart);

    // not in reattach stage after reset
    if (mReattachState == kReattachStop)
    {
        Get<MeshCoP::PendingDataset>().HandleDetach();
    }

#if OPENTHREAD_CONFIG_PARENT_SEARCH_ENABLE
    mParentSearchRecentlyDetached = true;
#endif

    SetStateDetached();
    mParent.SetState(Neighbor::kStateInvalid);
    SetRloc16(Mac::kShortAddrInvalid);
    IgnoreError(BecomeChild(kAttachAny));

exit:
    return error;
}

Error Mle::BecomeChild(AttachMode aMode)
{
    Error error = kErrorNone;

    VerifyOrExit(!IsDisabled(), error = kErrorInvalidState);
    VerifyOrExit(!IsAttaching(), error = kErrorBusy);

    if (!IsDetached())
    {
        mAttachCounter = 0;
    }

    if (mReattachState == kReattachStart)
    {
        if (Get<MeshCoP::ActiveDataset>().Restore() == kErrorNone)
        {
            mReattachState = kReattachActive;
        }
        else
        {
            mReattachState = kReattachStop;
        }
    }

    mParentCandidate.Clear();
    SetAttachState(kAttachStateStart);
    mParentRequestMode = aMode;

    if (aMode != kAttachBetter)
    {
#if OPENTHREAD_FTD
        if (IsFullThreadDevice())
        {
            Get<MleRouter>().StopAdvertiseTrickleTimer();
        }
#endif
    }
    else
    {
        mCounters.mBetterPartitionAttachAttempts++;
    }

    mAttachTimer.Start(GetAttachStartDelay());

    if (IsDetached())
    {
        mAttachCounter++;

        if (mAttachCounter == 0)
        {
            mAttachCounter--;
        }

        mCounters.mAttachAttempts++;

        if (!IsRxOnWhenIdle())
        {
            Get<Mac::Mac>().SetRxOnWhenIdle(false);
        }
    }

exit:
    return error;
}

uint32_t Mle::GetAttachStartDelay(void) const
{
    uint32_t delay = 1;
    uint32_t jitter;

    VerifyOrExit(IsDetached());

    if (mAttachCounter == 0)
    {
        delay = 1 + Random::NonCrypto::GetUint32InRange(0, kParentRequestRouterTimeout);
        ExitNow();
    }
#if OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_ENABLE
    else
    {
        uint16_t       counter = mAttachCounter - 1;
        const uint32_t ratio   = kAttachBackoffMaxInterval / kAttachBackoffMinInterval;

        if ((counter < sizeof(ratio) * CHAR_BIT) && ((1UL << counter) <= ratio))
        {
            delay = kAttachBackoffMinInterval;
            delay <<= counter;
        }
        else
        {
            delay = Random::NonCrypto::AddJitter(kAttachBackoffMaxInterval, kAttachBackoffJitter);
        }
    }
#endif // OPENTHREAD_CONFIG_MLE_ATTACH_BACKOFF_ENABLE

    jitter = Random::NonCrypto::GetUint32InRange(0, kAttachStartJitter);

    if (jitter + delay > delay) // check for overflow
    {
        delay += jitter;
    }

    otLogNoteMle("Attach attempt %d unsuccessful, will try again in %u.%03u seconds", mAttachCounter, delay / 1000,
                 delay % 1000);

exit:
    return delay;
}

bool Mle::IsAttached(void) const
{
    return (IsChild() || IsRouter() || IsLeader());
}

bool Mle::IsRouterOrLeader(void) const
{
    return (IsRouter() || IsLeader());
}

void Mle::SetStateDetached(void)
{
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    if (Get<Mac::Mac>().IsCslEnabled())
    {
        IgnoreError(Get<Radio>().EnableCsl(0, GetParent().GetRloc16(), &GetParent().GetExtAddress()));
    }
#endif
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    Get<BackboneRouter::Local>().Reset();
#endif
#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
    Get<BackboneRouter::Leader>().Reset();
#endif

    if (IsLeader())
    {
        Get<ThreadNetif>().RemoveUnicastAddress(mLeaderAloc);
    }

    SetRole(kRoleDetached);
    SetAttachState(kAttachStateIdle);
    mAttachTimer.Stop();
    mMessageTransmissionTimer.Stop();
    mChildUpdateRequestState = kChildUpdateRequestNone;
    mChildUpdateAttempts     = 0;
    mDataRequestState        = kDataRequestNone;
    mDataRequestAttempts     = 0;
    Get<MeshForwarder>().SetRxOnWhenIdle(true);
    Get<Mac::Mac>().SetBeaconEnabled(false);
#if OPENTHREAD_FTD
    Get<MleRouter>().HandleDetachStart();
#endif
    Get<Ip6::Ip6>().SetForwardingEnabled(false);
#if OPENTHREAD_FTD
    Get<Ip6::Mpl>().SetTimerExpirations(0);
#endif
}

void Mle::SetStateChild(uint16_t aRloc16)
{
    if (IsLeader())
    {
        Get<ThreadNetif>().RemoveUnicastAddress(mLeaderAloc);
    }

    SetRloc16(aRloc16);
    SetRole(kRoleChild);
    SetAttachState(kAttachStateIdle);
    mAttachTimer.Start(kAttachBackoffDelayToResetCounter);
    mReattachState       = kReattachStop;
    mChildUpdateAttempts = 0;
    mDataRequestAttempts = 0;
    Get<Mac::Mac>().SetBeaconEnabled(false);
    ScheduleMessageTransmissionTimer();

#if OPENTHREAD_FTD
    if (IsFullThreadDevice())
    {
        Get<MleRouter>().HandleChildStart(mParentRequestMode);
    }
#endif

    Get<Ip6::Ip6>().SetForwardingEnabled(false);
#if OPENTHREAD_FTD
    Get<Ip6::Mpl>().SetTimerExpirations(kMplChildDataMessageTimerExpirations);
#endif

    // send announce after attached if needed
    InformPreviousChannel();

#if OPENTHREAD_CONFIG_PARENT_SEARCH_ENABLE
    UpdateParentSearchState();
#endif

#if OPENTHREAD_CONFIG_MLE_INFORM_PREVIOUS_PARENT_ON_REATTACH
    InformPreviousParent();
    mPreviousParentRloc = mParent.GetRloc16();
#endif

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    if (Get<Mac::Mac>().IsCslCapable())
    {
        uint32_t period = IsRxOnWhenIdle() ? 0 : Get<Mac::Mac>().GetCslPeriod();
        IgnoreError(Get<Radio>().EnableCsl(period, GetParent().GetRloc16(), &GetParent().GetExtAddress()));
        ScheduleChildUpdateRequest();
    }
#endif
}

void Mle::InformPreviousChannel(void)
{
    VerifyOrExit(mAlternatePanId != Mac::kPanIdBroadcast);
    VerifyOrExit(IsChild() || IsRouter());

#if OPENTHREAD_FTD
    VerifyOrExit(!IsFullThreadDevice() || IsRouter() || Get<MleRouter>().GetRouterSelectionJitterTimeout() == 0);
#endif

    mAlternatePanId = Mac::kPanIdBroadcast;
    Get<AnnounceBeginServer>().SendAnnounce(1 << mAlternateChannel);

exit:
    return;
}

void Mle::SetTimeout(uint32_t aTimeout)
{
    VerifyOrExit(mTimeout != aTimeout);

    if (aTimeout < kMinTimeout)
    {
        aTimeout = kMinTimeout;
    }

    mTimeout = aTimeout;

    Get<DataPollSender>().RecalculatePollPeriod();

    if (IsChild())
    {
        IgnoreError(SendChildUpdateRequest());
    }

exit:
    return;
}

Error Mle::SetDeviceMode(DeviceMode aDeviceMode)
{
    Error      error   = kErrorNone;
    DeviceMode oldMode = mDeviceMode;

    VerifyOrExit(aDeviceMode.IsValid(), error = kErrorInvalidArgs);
    VerifyOrExit(mDeviceMode != aDeviceMode);
    mDeviceMode = aDeviceMode;

#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
    Get<Utils::HistoryTracker>().RecordNetworkInfo();
#endif

#if OPENTHREAD_CONFIG_OTNS_ENABLE
    Get<Utils::Otns>().EmitDeviceMode(mDeviceMode);
#endif

    otLogNoteMle("Mode 0x%02x -> 0x%02x [%s]", oldMode.Get(), mDeviceMode.Get(), mDeviceMode.ToString().AsCString());

    IgnoreError(Store());

    switch (mRole)
    {
    case kRoleDisabled:
        break;

    case kRoleDetached:
        mAttachCounter = 0;
        SetStateDetached();
        IgnoreError(BecomeChild(kAttachAny));
        break;

    case kRoleChild:
        SetStateChild(GetRloc16());
        IgnoreError(SendChildUpdateRequest());
        break;

    case kRoleRouter:
    case kRoleLeader:
        if (oldMode.IsFullThreadDevice() && !mDeviceMode.IsFullThreadDevice())
        {
            IgnoreError(BecomeDetached());
        }

        break;
    }

exit:
    return error;
}

void Mle::UpdateLinkLocalAddress(void)
{
    Get<ThreadNetif>().RemoveUnicastAddress(mLinkLocal64);
    mLinkLocal64.GetAddress().GetIid().SetFromExtAddress(Get<Mac::Mac>().GetExtAddress());
    Get<ThreadNetif>().AddUnicastAddress(mLinkLocal64);

    Get<Notifier>().Signal(kEventThreadLinkLocalAddrChanged);
}

void Mle::SetMeshLocalPrefix(const MeshLocalPrefix &aMeshLocalPrefix)
{
    VerifyOrExit(GetMeshLocalPrefix() != aMeshLocalPrefix,
                 Get<Notifier>().SignalIfFirst(kEventThreadMeshLocalAddrChanged));

    if (Get<ThreadNetif>().IsUp())
    {
        Get<ThreadNetif>().RemoveUnicastAddress(mLeaderAloc);

        // We must remove the old addresses before adding the new ones.
        Get<ThreadNetif>().RemoveUnicastAddress(mMeshLocal64);
        Get<ThreadNetif>().RemoveUnicastAddress(mMeshLocal16);
        Get<ThreadNetif>().UnsubscribeMulticast(mLinkLocalAllThreadNodes);
        Get<ThreadNetif>().UnsubscribeMulticast(mRealmLocalAllThreadNodes);
    }

    mMeshLocal64.GetAddress().SetPrefix(aMeshLocalPrefix);
    mMeshLocal16.GetAddress().SetPrefix(aMeshLocalPrefix);
    mLeaderAloc.GetAddress().SetPrefix(aMeshLocalPrefix);

    // Just keep mesh local prefix if network interface is down
    VerifyOrExit(Get<ThreadNetif>().IsUp());

    ApplyMeshLocalPrefix();

exit:
    return;
}

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
Error Mle::SetMeshLocalIid(const Ip6::InterfaceIdentifier &aMlIid)
{
    Error error = kErrorNone;

    VerifyOrExit(!Get<ThreadNetif>().HasUnicastAddress(mMeshLocal64), error = kErrorInvalidState);

    mMeshLocal64.GetAddress().SetIid(aMlIid);
exit:
    return error;
}
#endif

void Mle::ApplyMeshLocalPrefix(void)
{
    mLinkLocalAllThreadNodes.GetAddress().SetMulticastNetworkPrefix(GetMeshLocalPrefix());
    mRealmLocalAllThreadNodes.GetAddress().SetMulticastNetworkPrefix(GetMeshLocalPrefix());

    VerifyOrExit(!IsDisabled());

    // Add the addresses back into the table.
    Get<ThreadNetif>().AddUnicastAddress(mMeshLocal64);
    Get<ThreadNetif>().SubscribeMulticast(mLinkLocalAllThreadNodes);
    Get<ThreadNetif>().SubscribeMulticast(mRealmLocalAllThreadNodes);

    if (IsAttached())
    {
        Get<ThreadNetif>().AddUnicastAddress(mMeshLocal16);
    }

    // update Leader ALOC
    if (IsLeader())
    {
        Get<ThreadNetif>().AddUnicastAddress(mLeaderAloc);
    }

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
    Get<MeshCoP::Commissioner>().ApplyMeshLocalPrefix();
#endif

#if OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
    Get<MeshCoP::BorderAgent>().ApplyMeshLocalPrefix();
#endif

#if OPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE
    Get<Dhcp6::Server>().ApplyMeshLocalPrefix();
#endif

#if OPENTHREAD_CONFIG_NEIGHBOR_DISCOVERY_AGENT_ENABLE
    Get<NeighborDiscovery::Agent>().ApplyMeshLocalPrefix();
#endif

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE

    for (Ip6::Netif::UnicastAddress &serviceAloc : mServiceAlocs)
    {
        if (serviceAloc.GetAddress().GetIid().GetLocator() != Mac::kShortAddrInvalid)
        {
            Get<ThreadNetif>().RemoveUnicastAddress(serviceAloc);
            serviceAloc.GetAddress().SetPrefix(GetMeshLocalPrefix());
            Get<ThreadNetif>().AddUnicastAddress(serviceAloc);
        }
    }

#endif

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    Get<BackboneRouter::Local>().ApplyMeshLocalPrefix();
#endif

exit:
    // Changing the prefix also causes the mesh local address to be different.
    Get<Notifier>().Signal(kEventThreadMeshLocalAddrChanged);
}

uint16_t Mle::GetRloc16(void) const
{
    return Get<Mac::Mac>().GetShortAddress();
}

void Mle::SetRloc16(uint16_t aRloc16)
{
    uint16_t oldRloc16 = GetRloc16();

    if (aRloc16 != oldRloc16)
    {
        otLogNoteMle("RLOC16 %04x -> %04x", oldRloc16, aRloc16);
    }

    if (Get<ThreadNetif>().HasUnicastAddress(mMeshLocal16) &&
        (mMeshLocal16.GetAddress().GetIid().GetLocator() != aRloc16))
    {
        Get<ThreadNetif>().RemoveUnicastAddress(mMeshLocal16);
        Get<Tmf::Agent>().ClearRequests(mMeshLocal16.GetAddress());
    }

    Get<Mac::Mac>().SetShortAddress(aRloc16);
    Get<Ip6::Mpl>().SetSeedId(aRloc16);

    if (aRloc16 != Mac::kShortAddrInvalid)
    {
        // We can always call `AddUnicastAddress(mMeshLocat16)` and if
        // the address is already added, it will perform no action.

        mMeshLocal16.GetAddress().GetIid().SetLocator(aRloc16);
        Get<ThreadNetif>().AddUnicastAddress(mMeshLocal16);
#if OPENTHREAD_FTD
        Get<AddressResolver>().RestartAddressQueries();
#endif
    }
}

void Mle::SetLeaderData(uint32_t aPartitionId, uint8_t aWeighting, uint8_t aLeaderRouterId)
{
    if (mLeaderData.GetPartitionId() != aPartitionId)
    {
#if OPENTHREAD_FTD
        Get<MleRouter>().HandlePartitionChange();
#endif
        Get<Notifier>().Signal(kEventThreadPartitionIdChanged);
        mCounters.mPartitionIdChanges++;
    }
    else
    {
        Get<Notifier>().SignalIfFirst(kEventThreadPartitionIdChanged);
    }

    mLeaderData.SetPartitionId(aPartitionId);
    mLeaderData.SetWeighting(aWeighting);
    mLeaderData.SetLeaderRouterId(aLeaderRouterId);
}

Error Mle::GetLeaderAddress(Ip6::Address &aAddress) const
{
    Error error = kErrorNone;

    VerifyOrExit(GetRloc16() != Mac::kShortAddrInvalid, error = kErrorDetached);

    aAddress.SetToRoutingLocator(GetMeshLocalPrefix(), Rloc16FromRouterId(mLeaderData.GetLeaderRouterId()));

exit:
    return error;
}

Error Mle::GetLocatorAddress(Ip6::Address &aAddress, uint16_t aLocator) const
{
    Error error = kErrorNone;

    VerifyOrExit(GetRloc16() != Mac::kShortAddrInvalid, error = kErrorDetached);

    memcpy(&aAddress, &mMeshLocal16.GetAddress(), 14);
    aAddress.GetIid().SetLocator(aLocator);

exit:
    return error;
}

Error Mle::GetServiceAloc(uint8_t aServiceId, Ip6::Address &aAddress) const
{
    Error error = kErrorNone;

    VerifyOrExit(GetRloc16() != Mac::kShortAddrInvalid, error = kErrorDetached);
    aAddress.SetToAnycastLocator(GetMeshLocalPrefix(), ServiceAlocFromId(aServiceId));

exit:
    return error;
}

const LeaderData &Mle::GetLeaderData(void)
{
    mLeaderData.SetDataVersion(Get<NetworkData::Leader>().GetVersion());
    mLeaderData.SetStableDataVersion(Get<NetworkData::Leader>().GetStableVersion());

    return mLeaderData;
}

Message *Mle::NewMleMessage(void)
{
    Message *         message;
    Message::Settings settings(Message::kNoLinkSecurity, Message::kPriorityNet);

    message = mSocket.NewMessage(0, settings);
    VerifyOrExit(message != nullptr);

    message->SetSubType(Message::kSubTypeMleGeneral);

exit:
    return message;
}

Error Mle::AppendHeader(Message &aMessage, Command aCommand)
{
    Error  error = kErrorNone;
    Header header;

    header.Init();

    if (aCommand == kCommandDiscoveryRequest || aCommand == kCommandDiscoveryResponse)
    {
        header.SetSecuritySuite(Header::kNoSecurity);
    }
    else
    {
        header.SetKeyIdMode2();
    }

    header.SetCommand(aCommand);

    SuccessOrExit(error = aMessage.AppendBytes(&header, header.GetLength()));

exit:
    return error;
}

Error Mle::AppendSourceAddress(Message &aMessage) const
{
    return Tlv::Append<SourceAddressTlv>(aMessage, GetRloc16());
}

Error Mle::AppendStatus(Message &aMessage, StatusTlv::Status aStatus)
{
    return Tlv::Append<StatusTlv>(aMessage, aStatus);
}

Error Mle::AppendMode(Message &aMessage, DeviceMode aMode)
{
    return Tlv::Append<ModeTlv>(aMessage, aMode.Get());
}

Error Mle::AppendTimeout(Message &aMessage, uint32_t aTimeout)
{
    return Tlv::Append<TimeoutTlv>(aMessage, aTimeout);
}

Error Mle::AppendChallenge(Message &aMessage, const Challenge &aChallenge)
{
    return Tlv::Append<ChallengeTlv>(aMessage, aChallenge.mBuffer, aChallenge.mLength);
}

Error Mle::AppendChallenge(Message &aMessage, const uint8_t *aChallenge, uint8_t aChallengeLength)
{
    return Tlv::Append<ChallengeTlv>(aMessage, aChallenge, aChallengeLength);
}

Error Mle::AppendResponse(Message &aMessage, const Challenge &aResponse)
{
    return Tlv::Append<ResponseTlv>(aMessage, aResponse.mBuffer, aResponse.mLength);
}

Error Mle::ReadChallengeOrResponse(const Message &aMessage, uint8_t aTlvType, Challenge &aBuffer)
{
    Error    error;
    uint16_t offset;
    uint16_t length;

    SuccessOrExit(error = Tlv::FindTlvValueOffset(aMessage, aTlvType, offset, length));
    VerifyOrExit(length >= kMinChallengeSize, error = kErrorParse);

    if (length > kMaxChallengeSize)
    {
        length = kMaxChallengeSize;
    }

    aMessage.ReadBytes(offset, aBuffer.mBuffer, length);
    aBuffer.mLength = static_cast<uint8_t>(length);

exit:
    return error;
}

Error Mle::ReadChallenge(const Message &aMessage, Challenge &aChallenge)
{
    return ReadChallengeOrResponse(aMessage, Tlv::kChallenge, aChallenge);
}

Error Mle::ReadResponse(const Message &aMessage, Challenge &aResponse)
{
    return ReadChallengeOrResponse(aMessage, Tlv::kResponse, aResponse);
}

Error Mle::AppendLinkFrameCounter(Message &aMessage)
{
    uint32_t counter;

    // When including Link-layer Frame Counter TLV in an MLE message
    // the value is set to the maximum MAC frame counter on all
    // supported radio links. All radio links must also start using
    // the same counter value as the value included in the TLV.

    counter = Get<KeyManager>().GetMaximumMacFrameCounter();

#if OPENTHREAD_CONFIG_MULTI_RADIO
    Get<KeyManager>().SetAllMacFrameCounters(counter);
#endif

    return Tlv::Append<LinkFrameCounterTlv>(aMessage, counter);
}

Error Mle::AppendMleFrameCounter(Message &aMessage)
{
    return Tlv::Append<MleFrameCounterTlv>(aMessage, Get<KeyManager>().GetMleFrameCounter());
}

Error Mle::ReadFrameCounters(const Message &aMessage, uint32_t &aLinkFrameCounter, uint32_t &aMleFrameCounter) const
{
    Error error;

    SuccessOrExit(error = Tlv::Find<LinkFrameCounterTlv>(aMessage, aLinkFrameCounter));

    switch (Tlv::Find<MleFrameCounterTlv>(aMessage, aMleFrameCounter))
    {
    case kErrorNone:
        break;
    case kErrorNotFound:
        aMleFrameCounter = aLinkFrameCounter;
        break;
    default:
        error = kErrorParse;
        break;
    }

exit:
    return error;
}

Error Mle::AppendAddress16(Message &aMessage, uint16_t aRloc16)
{
    return Tlv::Append<Address16Tlv>(aMessage, aRloc16);
}

Error Mle::AppendLeaderData(Message &aMessage)
{
    LeaderDataTlv leaderDataTlv;

    mLeaderData.SetDataVersion(Get<NetworkData::Leader>().GetVersion());
    mLeaderData.SetStableDataVersion(Get<NetworkData::Leader>().GetStableVersion());

    leaderDataTlv.Init();
    leaderDataTlv.Set(mLeaderData);

    return leaderDataTlv.AppendTo(aMessage);
}

Error Mle::ReadLeaderData(const Message &aMessage, LeaderData &aLeaderData)
{
    Error         error;
    LeaderDataTlv leaderDataTlv;

    SuccessOrExit(error = Tlv::FindTlv(aMessage, leaderDataTlv));
    VerifyOrExit(leaderDataTlv.IsValid(), error = kErrorParse);
    leaderDataTlv.Get(aLeaderData);

exit:
    return error;
}

Error Mle::AppendNetworkData(Message &aMessage, bool aStableOnly)
{
    Error   error = kErrorNone;
    uint8_t networkData[NetworkData::NetworkData::kMaxSize];
    uint8_t length;

    VerifyOrExit(!mRetrieveNewNetworkData, error = kErrorInvalidState);

    length = sizeof(networkData);
    IgnoreError(Get<NetworkData::Leader>().CopyNetworkData(aStableOnly, networkData, length));

    error = Tlv::Append<NetworkDataTlv>(aMessage, networkData, length);

exit:
    return error;
}

Error Mle::AppendTlvRequest(Message &aMessage, const uint8_t *aTlvs, uint8_t aTlvsLength)
{
    return Tlv::Append<TlvRequestTlv>(aMessage, aTlvs, aTlvsLength);
}

Error Mle::FindTlvRequest(const Message &aMessage, RequestedTlvs &aRequestedTlvs)
{
    Error    error;
    uint16_t offset;
    uint16_t length;

    SuccessOrExit(error = Tlv::FindTlvValueOffset(aMessage, Tlv::kTlvRequest, offset, length));

    if (length > sizeof(aRequestedTlvs.mTlvs))
    {
        length = sizeof(aRequestedTlvs.mTlvs);
    }

    aMessage.ReadBytes(offset, aRequestedTlvs.mTlvs, length);
    aRequestedTlvs.mNumTlvs = static_cast<uint8_t>(length);

exit:
    return error;
}

Error Mle::AppendScanMask(Message &aMessage, uint8_t aScanMask)
{
    return Tlv::Append<ScanMaskTlv>(aMessage, aScanMask);
}

Error Mle::AppendLinkMargin(Message &aMessage, uint8_t aLinkMargin)
{
    return Tlv::Append<LinkMarginTlv>(aMessage, aLinkMargin);
}

Error Mle::AppendVersion(Message &aMessage)
{
    return Tlv::Append<VersionTlv>(aMessage, kThreadVersion);
}

bool Mle::HasUnregisteredAddress(void)
{
    bool retval = false;

    // Checks whether there are any addresses in addition to the mesh-local
    // address that need to be registered.

    for (const Ip6::Netif::UnicastAddress &addr : Get<ThreadNetif>().GetUnicastAddresses())
    {
        if (!addr.GetAddress().IsLinkLocal() && !IsRoutingLocator(addr.GetAddress()) &&
            !IsAnycastLocator(addr.GetAddress()) && addr.GetAddress() != GetMeshLocal64())
        {
            ExitNow(retval = true);
        }
    }

    if (!IsRxOnWhenIdle())
    {
        // For sleepy end-device, we register any external multicast
        // addresses.

        retval = Get<ThreadNetif>().HasAnyExternalMulticastAddress();
    }

exit:
    return retval;
}

Error Mle::AppendAddressRegistration(Message &aMessage, AddressRegistrationMode aMode)
{
    Error                    error = kErrorNone;
    Tlv                      tlv;
    AddressRegistrationEntry entry;
    Lowpan::Context          context;
    uint8_t                  length      = 0;
    uint8_t                  counter     = 0;
    uint16_t                 startOffset = aMessage.GetLength();
#if OPENTHREAD_CONFIG_DUA_ENABLE
    Ip6::Address domainUnicastAddress;
#endif

    tlv.SetType(Tlv::kAddressRegistration);
    SuccessOrExit(error = aMessage.Append(tlv));

    // Prioritize ML-EID
    entry.SetContextId(kMeshLocalPrefixContextId);
    entry.SetIid(GetMeshLocal64().GetIid());
    SuccessOrExit(error = aMessage.AppendBytes(&entry, entry.GetLength()));
    length += entry.GetLength();

    // Continue to append the other addresses if not `kAppendMeshLocalOnly` mode
    VerifyOrExit(aMode != kAppendMeshLocalOnly);
    counter++;

#if OPENTHREAD_CONFIG_DUA_ENABLE
    // Cache Domain Unicast Address.
    domainUnicastAddress = Get<DuaManager>().GetDomainUnicastAddress();

    if (Get<ThreadNetif>().HasUnicastAddress(domainUnicastAddress))
    {
        error = Get<NetworkData::Leader>().GetContext(domainUnicastAddress, context);

        OT_ASSERT(error == kErrorNone);

        // Prioritize DUA, compressed entry
        entry.SetContextId(context.mContextId);
        entry.SetIid(domainUnicastAddress.GetIid());
        SuccessOrExit(error = aMessage.AppendBytes(&entry, entry.GetLength()));
        length += entry.GetLength();
        counter++;
    }
#endif // OPENTHREAD_CONFIG_DUA_ENABLE

    for (const Ip6::Netif::UnicastAddress &addr : Get<ThreadNetif>().GetUnicastAddresses())
    {
        if (addr.GetAddress().IsLinkLocal() || IsRoutingLocator(addr.GetAddress()) ||
            IsAnycastLocator(addr.GetAddress()) || addr.GetAddress() == GetMeshLocal64())
        {
            continue;
        }

#if OPENTHREAD_CONFIG_DUA_ENABLE
        // Skip DUA that was already appended above.
        if (addr.GetAddress() == domainUnicastAddress)
        {
            continue;
        }
#endif

        if (Get<NetworkData::Leader>().GetContext(addr.GetAddress(), context) == kErrorNone)
        {
            // compressed entry
            entry.SetContextId(context.mContextId);
            entry.SetIid(addr.GetAddress().GetIid());
        }
        else
        {
            // uncompressed entry
            entry.SetUncompressed();
            entry.SetIp6Address(addr.GetAddress());
        }

        SuccessOrExit(error = aMessage.AppendBytes(&entry, entry.GetLength()));
        length += entry.GetLength();
        counter++;
        // only continue to append if there is available entry.
        VerifyOrExit(counter < OPENTHREAD_CONFIG_MLE_IP_ADDRS_TO_REGISTER);
    }

    // Append external multicast addresses.  For sleepy end device,
    // register all external multicast addresses with the parent for
    // indirect transmission. Since Thread 1.2, non-sleepy MED should
    // also register external multicast addresses of scope larger than
    // realm with a 1.2 or higher parent.
    if (!IsRxOnWhenIdle()
#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
        || !GetParent().IsThreadVersion1p1()
#endif
    )
    {
        for (const Ip6::Netif::MulticastAddress &addr : Get<ThreadNetif>().IterateExternalMulticastAddresses())
        {
#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
            // For Thread 1.2 MED, skip multicast address with scope not
            // larger than realm local when registering.
            if (IsRxOnWhenIdle() && !addr.GetAddress().IsMulticastLargerThanRealmLocal())
            {
                continue;
            }
#endif

            entry.SetUncompressed();
            entry.SetIp6Address(addr.GetAddress());
            SuccessOrExit(error = aMessage.AppendBytes(&entry, entry.GetLength()));
            length += entry.GetLength();

            counter++;
            // only continue to append if there is available entry.
            VerifyOrExit(counter < OPENTHREAD_CONFIG_MLE_IP_ADDRS_TO_REGISTER);
        }
    }

exit:

    if (error == kErrorNone && length > 0)
    {
        tlv.SetLength(length);
        aMessage.Write(startOffset, tlv);
    }

    return error;
}

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
Error Mle::AppendTimeRequest(Message &aMessage)
{
    // `TimeRequestTlv` has no value.
    return Tlv::Append<TimeRequestTlv>(aMessage, nullptr, 0);
}

Error Mle::AppendTimeParameter(Message &aMessage)
{
    TimeParameterTlv tlv;

    tlv.Init();
    tlv.SetTimeSyncPeriod(Get<TimeSync>().GetTimeSyncPeriod());
    tlv.SetXtalThreshold(Get<TimeSync>().GetXtalThreshold());

    return tlv.AppendTo(aMessage);
}

Error Mle::AppendXtalAccuracy(Message &aMessage)
{
    return Tlv::Append<XtalAccuracyTlv>(aMessage, otPlatTimeGetXtalAccuracy());
}
#endif // OPENTHREAD_CONFIG_TIME_SYNC_ENABLE

Error Mle::AppendActiveTimestamp(Message &aMessage)
{
    Error                     error     = kErrorNone;
    const MeshCoP::Timestamp *timestamp = Get<MeshCoP::ActiveDataset>().GetTimestamp();

    VerifyOrExit(timestamp != nullptr);
    error = Tlv::Append<ActiveTimestampTlv>(aMessage, *timestamp);

exit:
    return error;
}

Error Mle::AppendPendingTimestamp(Message &aMessage)
{
    Error                     error     = kErrorNone;
    const MeshCoP::Timestamp *timestamp = Get<MeshCoP::PendingDataset>().GetTimestamp();

    VerifyOrExit(timestamp != nullptr && timestamp->GetSeconds() != 0);
    error = Tlv::Append<PendingTimestampTlv>(aMessage, *timestamp);

exit:
    return error;
}

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
Error Mle::AppendCslChannel(Message &aMessage)
{
    Error         error = kErrorNone;
    CslChannelTlv cslChannel;

    // In current implementation, it's allowed to set CSL Channel unspecified. As `0` is not valid for Channel value
    // in CSL Channel TLV, if CSL channel is not specified, we don't append CSL Channel TLV.
    // And on transmitter side, it would also set CSL Channel for the child to `0` if it doesn't find a CSL Channel
    // TLV.
    VerifyOrExit(Get<Mac::Mac>().IsCslChannelSpecified());

    cslChannel.Init();
    cslChannel.SetChannelPage(0);
    cslChannel.SetChannel(Get<Mac::Mac>().GetCslChannel());

    SuccessOrExit(error = aMessage.Append(cslChannel));

exit:
    return error;
}

Error Mle::AppendCslTimeout(Message &aMessage)
{
    OT_ASSERT(Get<Mac::Mac>().IsCslEnabled());
    return Tlv::Append<CslTimeoutTlv>(aMessage, mCslTimeout == 0 ? mTimeout : mCslTimeout);
}

void Mle::SetCslTimeout(uint32_t aTimeout)
{
    VerifyOrExit(mCslTimeout != aTimeout);

    mCslTimeout = aTimeout;

    Get<DataPollSender>().RecalculatePollPeriod();

    if (Get<Mac::Mac>().IsCslEnabled())
    {
        ScheduleChildUpdateRequest();
    }

exit:
    return;
}
#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
Error Mle::AppendCslClockAccuracy(Message &aMessage)
{
    Error               error = kErrorNone;
    CslClockAccuracyTlv cslClockAccuracy;

    cslClockAccuracy.Init();

    cslClockAccuracy.SetCslClockAccuracy(Get<Radio>().GetCslAccuracy());
    cslClockAccuracy.SetCslUncertainty(Get<Radio>().GetCslClockUncertainty());

    SuccessOrExit(error = aMessage.Append(cslClockAccuracy));

exit:
    return error;
}
#endif

void Mle::HandleNotifierEvents(Events aEvents)
{
    VerifyOrExit(!IsDisabled());

    if (aEvents.Contains(kEventThreadRoleChanged))
    {
        if (IsChild() && !IsFullThreadDevice() && mAddressRegistrationMode == kAppendMeshLocalOnly)
        {
            // If only mesh-local address was registered in the "Child
            // ID Request" message, after device is attached, trigger a
            // "Child Update Request" to register the remaining
            // addresses.

            mAddressRegistrationMode = kAppendAllAddresses;
            mChildUpdateRequestState = kChildUpdateRequestPending;
            ScheduleMessageTransmissionTimer();
        }
    }

    if (aEvents.ContainsAny(kEventIp6AddressAdded | kEventIp6AddressRemoved))
    {
        if (!Get<ThreadNetif>().HasUnicastAddress(mMeshLocal64.GetAddress()))
        {
            // Mesh Local EID was removed, choose a new one and add it back
            mMeshLocal64.GetAddress().GetIid().GenerateRandom();

            Get<ThreadNetif>().AddUnicastAddress(mMeshLocal64);
            Get<Notifier>().Signal(kEventThreadMeshLocalAddrChanged);
        }

        if (IsChild() && !IsFullThreadDevice())
        {
            mChildUpdateRequestState = kChildUpdateRequestPending;
            ScheduleMessageTransmissionTimer();
        }
    }

    if (aEvents.ContainsAny(kEventIp6MulticastSubscribed | kEventIp6MulticastUnsubscribed))
    {
        // When multicast subscription changes, SED always notifies its parent as it depends on its
        // parent for indirect transmission. Since Thread 1.2, MED MAY also notify its parent of 1.2
        // or higher version as it could depend on its parent to perform Multicast Listener Report.
        if (IsChild() && !IsFullThreadDevice() &&
            (!IsRxOnWhenIdle()
#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
             || !GetParent().IsThreadVersion1p1()
#endif
                 ))

        {
            mChildUpdateRequestState = kChildUpdateRequestPending;
            ScheduleMessageTransmissionTimer();
        }
    }

    if (aEvents.Contains(kEventThreadNetdataChanged))
    {
#if OPENTHREAD_FTD
        if (IsFullThreadDevice())
        {
            Get<MleRouter>().HandleNetworkDataUpdateRouter();
        }
        else
#endif
        {
            if (!aEvents.Contains(kEventThreadRoleChanged))
            {
                mChildUpdateRequestState = kChildUpdateRequestPending;
                ScheduleMessageTransmissionTimer();
            }
        }

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
        Get<BackboneRouter::Leader>().Update();
#endif
#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
        UpdateServiceAlocs();
#endif

#if OPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE
        IgnoreError(Get<Dhcp6::Server>().UpdateService());
#endif

#if OPENTHREAD_CONFIG_NEIGHBOR_DISCOVERY_AGENT_ENABLE
        Get<NeighborDiscovery::Agent>().UpdateService();
#endif

#if OPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE
        Get<Dhcp6::Client>().UpdateAddresses();
#endif
    }

    if (aEvents.ContainsAny(kEventThreadRoleChanged | kEventThreadKeySeqCounterChanged))
    {
        // Store the settings on a key seq change, or when role changes and device
        // is attached (i.e., skip `Store()` on role change to detached).

        if (aEvents.Contains(kEventThreadKeySeqCounterChanged) || IsAttached())
        {
            IgnoreError(Store());
        }
    }

exit:
    return;
}

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
void Mle::UpdateServiceAlocs(void)
{
    uint16_t              rloc               = GetRloc16();
    uint16_t              serviceAloc        = 0;
    uint8_t               serviceId          = 0;
    NetworkData::Iterator serviceIterator    = NetworkData::kIteratorInit;
    size_t                serviceAlocsLength = OT_ARRAY_LENGTH(mServiceAlocs);
    size_t                i                  = 0;

    VerifyOrExit(!IsDisabled());

    // First remove all alocs which are no longer necessary, to free up space in mServiceAlocs
    for (i = 0; i < serviceAlocsLength; i++)
    {
        serviceAloc = mServiceAlocs[i].GetAddress().GetIid().GetLocator();

        if ((serviceAloc != Mac::kShortAddrInvalid) &&
            (!Get<NetworkData::Leader>().ContainsService(Mle::ServiceIdFromAloc(serviceAloc), rloc)))
        {
            Get<ThreadNetif>().RemoveUnicastAddress(mServiceAlocs[i]);
            mServiceAlocs[i].GetAddress().GetIid().SetLocator(Mac::kShortAddrInvalid);
        }
    }

    // Now add any missing service alocs which should be there, if there is enough space in mServiceAlocs
    while (Get<NetworkData::Leader>().GetNextServiceId(serviceIterator, rloc, serviceId) == kErrorNone)
    {
        for (i = 0; i < serviceAlocsLength; i++)
        {
            serviceAloc = mServiceAlocs[i].GetAddress().GetIid().GetLocator();

            if ((serviceAloc != Mac::kShortAddrInvalid) && (Mle::ServiceIdFromAloc(serviceAloc) == serviceId))
            {
                break;
            }
        }

        if (i >= serviceAlocsLength)
        {
            // Service Aloc is not there, but it should be. Lets add it into first empty space
            for (i = 0; i < serviceAlocsLength; i++)
            {
                serviceAloc = mServiceAlocs[i].GetAddress().GetIid().GetLocator();

                if (serviceAloc == Mac::kShortAddrInvalid)
                {
                    SuccessOrExit(GetServiceAloc(serviceId, mServiceAlocs[i].GetAddress()));
                    Get<ThreadNetif>().AddUnicastAddress(mServiceAlocs[i]);
                    break;
                }
            }
        }
    }

exit:
    return;
}
#endif

void Mle::HandleAttachTimer(Timer &aTimer)
{
    aTimer.Get<Mle>().HandleAttachTimer();
}

void Mle::HandleAttachTimer(void)
{
    uint32_t delay          = 0;
    bool     shouldAnnounce = true;

    if (mAttachState == kAttachStateParentRequestRouter || mAttachState == kAttachStateParentRequestReed ||
        (mAttachState == kAttachStateAnnounce && !HasMoreChannelsToAnnouce()))
    {
        uint8_t linkQuality;

        linkQuality = mParentCandidate.GetLinkInfo().GetLinkQuality();

        if (linkQuality > mParentCandidate.GetLinkQualityOut())
        {
            linkQuality = mParentCandidate.GetLinkQualityOut();
        }

        // If already attached, accept the parent candidate if
        // we are trying to attach to a better partition or if a
        // Parent Response was also received from the current parent
        // to which the device is attached. This ensures that the
        // new parent candidate is compared with the current parent
        // and that it is indeed preferred over the current one.
        // If we are in kAttachStateParentRequestRouter and cannot
        // find a parent with best link quality(3), we will keep
        // the candidate and forward to REED stage to find a better
        // parent.

        if ((linkQuality == 3 || mAttachState != kAttachStateParentRequestRouter) &&
            mParentCandidate.IsStateParentResponse() &&
            (!IsChild() || mReceivedResponseFromParent || mParentRequestMode == kAttachBetter) &&
            SendChildIdRequest() == kErrorNone)
        {
            SetAttachState(kAttachStateChildIdRequest);
            delay = kParentRequestReedTimeout;
            ExitNow();
        }
    }

    switch (mAttachState)
    {
    case kAttachStateIdle:
        mAttachCounter = 0;
        break;

    case kAttachStateProcessAnnounce:
        ProcessAnnounce();
        break;

    case kAttachStateStart:
        if (mAttachCounter > 0)
        {
            otLogNoteMle("Attempt to attach - attempt %d, %s %s", mAttachCounter,
                         AttachModeToString(mParentRequestMode), ReattachStateToString(mReattachState));
        }
        else
        {
            otLogNoteMle("Attempt to attach - %s %s", AttachModeToString(mParentRequestMode),
                         ReattachStateToString(mReattachState));
        }

        SetAttachState(kAttachStateParentRequestRouter);
        mParentCandidate.SetState(Neighbor::kStateInvalid);
        mReceivedResponseFromParent = false;
        Get<MeshForwarder>().SetRxOnWhenIdle(true);

        // initial MLE Parent Request has both E and R flags set in Scan Mask TLV
        // during reattach when losing connectivity.
        if (mParentRequestMode == kAttachSame1 || mParentRequestMode == kAttachSame2)
        {
            IgnoreError(SendParentRequest(kParentRequestTypeRoutersAndReeds));
            delay = kParentRequestReedTimeout;
        }
        // initial MLE Parent Request has only R flag set in Scan Mask TLV for
        // during initial attach or downgrade process
        else
        {
            IgnoreError(SendParentRequest(kParentRequestTypeRouters));
            delay = kParentRequestRouterTimeout;
        }

        break;

    case kAttachStateParentRequestRouter:
        SetAttachState(kAttachStateParentRequestReed);
        IgnoreError(SendParentRequest(kParentRequestTypeRoutersAndReeds));
        delay = kParentRequestReedTimeout;
        break;

    case kAttachStateParentRequestReed:
        shouldAnnounce = PrepareAnnounceState();

        if (shouldAnnounce)
        {
            // We send an extra "Parent Request" as we switch to
            // `kAttachStateAnnounce` and start sending Announce on
            // all channels. This gives an additional chance to find
            // a parent during this phase. Note that we can stay in
            // `kAttachStateAnnounce` for multiple iterations, each
            // time sending an Announce on a different channel
            // (with `mAnnounceDelay` wait between them).

            SetAttachState(kAttachStateAnnounce);
            IgnoreError(SendParentRequest(kParentRequestTypeRoutersAndReeds));
            mAnnounceChannel = Mac::ChannelMask::kChannelIteratorFirst;
            delay            = mAnnounceDelay;
            break;
        }

        OT_FALL_THROUGH;

    case kAttachStateAnnounce:
        if (shouldAnnounce && (GetNextAnnouceChannel(mAnnounceChannel) == kErrorNone))
        {
            SendAnnounce(mAnnounceChannel, kOrphanAnnounce);
            delay = mAnnounceDelay;
            break;
        }

        OT_FALL_THROUGH;

    case kAttachStateChildIdRequest:
        SetAttachState(kAttachStateIdle);
        mParentCandidate.Clear();
        delay = Reattach();
        break;
    }

exit:

    if (delay != 0)
    {
        mAttachTimer.Start(delay);
    }
}

bool Mle::PrepareAnnounceState(void)
{
    bool             shouldAnnounce = false;
    Mac::ChannelMask channelMask;

    VerifyOrExit(!IsChild() && (mReattachState == kReattachStop) &&
                 (Get<MeshCoP::ActiveDataset>().IsPartiallyComplete() || !IsFullThreadDevice()));

    if (Get<MeshCoP::ActiveDataset>().GetChannelMask(channelMask) != kErrorNone)
    {
        channelMask = Get<Mac::Mac>().GetSupportedChannelMask();
    }

    mAnnounceDelay = kAnnounceTimeout / (channelMask.GetNumberOfChannels() + 1);

    if (mAnnounceDelay < kMinAnnounceDelay)
    {
        mAnnounceDelay = kMinAnnounceDelay;
    }

    shouldAnnounce = true;

exit:
    return shouldAnnounce;
}

uint32_t Mle::Reattach(void)
{
    uint32_t delay = 0;

    if (mReattachState == kReattachActive)
    {
        if (Get<MeshCoP::PendingDataset>().Restore() == kErrorNone)
        {
            IgnoreError(Get<MeshCoP::PendingDataset>().ApplyConfiguration());
            mReattachState = kReattachPending;
            SetAttachState(kAttachStateStart);
            delay = 1 + Random::NonCrypto::GetUint32InRange(0, kAttachStartJitter);
        }
        else
        {
            mReattachState = kReattachStop;
        }
    }
    else if (mReattachState == kReattachPending)
    {
        mReattachState = kReattachStop;
        IgnoreError(Get<MeshCoP::ActiveDataset>().Restore());
    }

    VerifyOrExit(mReattachState == kReattachStop);

    switch (mParentRequestMode)
    {
    case kAttachAny:
        if (!IsChild())
        {
            if (mAlternatePanId != Mac::kPanIdBroadcast)
            {
                IgnoreError(Get<Mac::Mac>().SetPanChannel(mAlternateChannel));
                Get<Mac::Mac>().SetPanId(mAlternatePanId);
                mAlternatePanId = Mac::kPanIdBroadcast;
                IgnoreError(BecomeDetached());
            }
#if OPENTHREAD_FTD
            else if (IsFullThreadDevice() && Get<MleRouter>().BecomeLeader() == kErrorNone)
            {
                // do nothing
            }
#endif
            else
            {
                IgnoreError(BecomeDetached());
            }
        }
        else if (!IsRxOnWhenIdle())
        {
            // return to sleepy operation
            Get<DataPollSender>().SetAttachMode(false);
            Get<MeshForwarder>().SetRxOnWhenIdle(false);
        }

        break;

    case kAttachSame1:
        IgnoreError(BecomeChild(kAttachSame2));
        break;

    case kAttachSame2:
    case kAttachSameDowngrade:
        IgnoreError(BecomeChild(kAttachAny));
        break;

    case kAttachBetter:
        break;
    }

exit:
    return delay;
}

void Mle::HandleDelayedResponseTimer(Timer &aTimer)
{
    aTimer.Get<Mle>().HandleDelayedResponseTimer();
}

void Mle::HandleDelayedResponseTimer(void)
{
    DelayedResponseMetadata metadata;
    TimeMilli               now          = TimerMilli::GetNow();
    TimeMilli               nextSendTime = now.GetDistantFuture();
    Message *               message;
    Message *               nextMessage;

    for (message = mDelayedResponses.GetHead(); message != nullptr; message = nextMessage)
    {
        nextMessage = message->GetNext();

        metadata.ReadFrom(*message);

        if (now < metadata.mSendTime)
        {
            if (nextSendTime > metadata.mSendTime)
            {
                nextSendTime = metadata.mSendTime;
            }
        }
        else
        {
            Error error = kErrorNone;

            mDelayedResponses.Dequeue(*message);
            metadata.RemoveFrom(*message);

            if (message->GetSubType() == Message::kSubTypeMleDataRequest)
            {
                error = AppendActiveTimestamp(*message);
                error = (error == kErrorNone) ? AppendPendingTimestamp(*message) : error;
            }

            error = (error == kErrorNone) ? SendMessage(*message, metadata.mDestination) : error;

            if (error == kErrorNone)
            {
                Log(kMessageSend, kTypeGenericDelayed, metadata.mDestination);

                // Here enters fast poll mode, as for Rx-Off-when-idle device, the enqueued msg should
                // be Mle Data Request.
                // Note: Finer-grade check (e.g. message subtype) might be required when deciding whether
                // or not enters fast poll mode fast poll mode if there are other type of delayed message
                // for Rx-Off-when-idle device.
                if (!IsRxOnWhenIdle())
                {
                    Get<DataPollSender>().SendFastPolls(DataPollSender::kDefaultFastPolls);
                }
            }
            else
            {
                message->Free();
            }
        }
    }

    if (nextSendTime < now.GetDistantFuture())
    {
        mDelayedResponseTimer.FireAt(nextSendTime);
    }
}

void Mle::RemoveDelayedDataResponseMessage(void)
{
    Message *               message = mDelayedResponses.GetHead();
    DelayedResponseMetadata metadata;

    while (message != nullptr)
    {
        metadata.ReadFrom(*message);

        if (message->GetSubType() == Message::kSubTypeMleDataResponse)
        {
            mDelayedResponses.DequeueAndFree(*message);
            Log(kMessageRemoveDelayed, kTypeDataResponse, metadata.mDestination);

            // no more than one multicast MLE Data Response in Delayed Message Queue.
            break;
        }

        message = message->GetNext();
    }
}

void Mle::RemoveDelayedDataRequestMessage(const Ip6::Address &aDestination)
{
    for (Message *message = mDelayedResponses.GetHead(); message != nullptr; message = message->GetNext())
    {
        DelayedResponseMetadata metadata;

        metadata.ReadFrom(*message);

        if (message->GetSubType() == Message::kSubTypeMleDataRequest && metadata.mDestination == aDestination)
        {
            mDelayedResponses.DequeueAndFree(*message);
            Log(kMessageRemoveDelayed, kTypeDataRequest, metadata.mDestination);

            // no more than one MLE Data Request for the destination in Delayed Message Queue.
            break;
        }
    }
}

Error Mle::SendParentRequest(ParentRequestType aType)
{
    Error        error = kErrorNone;
    Message *    message;
    uint8_t      scanMask = 0;
    Ip6::Address destination;

    mParentRequestChallenge.GenerateRandom();

    switch (aType)
    {
    case kParentRequestTypeRouters:
        scanMask = ScanMaskTlv::kRouterFlag;
        break;

    case kParentRequestTypeRoutersAndReeds:
        scanMask = ScanMaskTlv::kRouterFlag | ScanMaskTlv::kEndDeviceFlag;
        break;
    }

    VerifyOrExit((message = NewMleMessage()) != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = AppendHeader(*message, kCommandParentRequest));
    SuccessOrExit(error = AppendMode(*message, mDeviceMode));
    SuccessOrExit(error = AppendChallenge(*message, mParentRequestChallenge));
    SuccessOrExit(error = AppendScanMask(*message, scanMask));
    SuccessOrExit(error = AppendVersion(*message));
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    SuccessOrExit(error = AppendTimeRequest(*message));
#endif

    destination.SetToLinkLocalAllRoutersMulticast();
    SuccessOrExit(error = SendMessage(*message, destination));

    switch (aType)
    {
    case kParentRequestTypeRouters:
        Log(kMessageSend, kTypeParentRequestToRouters, destination);
        break;

    case kParentRequestTypeRoutersAndReeds:
        Log(kMessageSend, kTypeParentRequestToRoutersReeds, destination);
        break;
    }

exit:
    FreeMessageOnError(message, error);
    return error;
}

void Mle::RequestShorterChildIdRequest(void)
{
    if (mAttachState == kAttachStateChildIdRequest)
    {
        mAddressRegistrationMode = kAppendMeshLocalOnly;
        IgnoreError(SendChildIdRequest());
    }
}

Error Mle::SendChildIdRequest(void)
{
    Error        error   = kErrorNone;
    uint8_t      tlvs[]  = {Tlv::kAddress16, Tlv::kNetworkData, Tlv::kRoute};
    uint8_t      tlvsLen = sizeof(tlvs);
    Message *    message = nullptr;
    Ip6::Address destination;

    if (mParent.GetExtAddress() == mParentCandidate.GetExtAddress())
    {
        if (IsChild())
        {
            otLogInfoMle("Already attached to candidate parent");
            ExitNow(error = kErrorAlready);
        }
        else
        {
            // Invalidate stale parent state.
            //
            // Parent state is not normally invalidated after becoming a Router/Leader (see #1875).  When trying to
            // attach to a better partition, invalidating old parent state (especially when in kStateRestored) ensures
            // that FindNeighbor() returns mParentCandidate when processing the Child ID Response.
            mParent.SetState(Neighbor::kStateInvalid);
        }
    }

    VerifyOrExit((message = NewMleMessage()) != nullptr, error = kErrorNoBufs);
    message->SetSubType(Message::kSubTypeMleChildIdRequest);
    SuccessOrExit(error = AppendHeader(*message, kCommandChildIdRequest));
    SuccessOrExit(error = AppendResponse(*message, mParentCandidateChallenge));
    SuccessOrExit(error = AppendLinkFrameCounter(*message));
    SuccessOrExit(error = AppendMleFrameCounter(*message));
    SuccessOrExit(error = AppendMode(*message, mDeviceMode));
    SuccessOrExit(error = AppendTimeout(*message, mTimeout));
    SuccessOrExit(error = AppendVersion(*message));

    if (!IsFullThreadDevice())
    {
        SuccessOrExit(error = AppendAddressRegistration(*message, mAddressRegistrationMode));

        // no need to request the last Route64 TLV for MTD
        tlvsLen -= 1;
    }

    SuccessOrExit(error = AppendTlvRequest(*message, tlvs, tlvsLen));
    SuccessOrExit(error = AppendActiveTimestamp(*message));
    SuccessOrExit(error = AppendPendingTimestamp(*message));

    mParentCandidate.SetState(Neighbor::kStateValid);

    destination.SetToLinkLocalAddress(mParentCandidate.GetExtAddress());
    SuccessOrExit(error = SendMessage(*message, destination));

    Log(kMessageSend,
        (mAddressRegistrationMode == kAppendMeshLocalOnly) ? kTypeChildIdRequestShort : kTypeChildIdRequest,
        destination);

    if (!IsRxOnWhenIdle())
    {
        Get<DataPollSender>().SetAttachMode(true);
        Get<MeshForwarder>().SetRxOnWhenIdle(false);
    }

exit:
    FreeMessageOnError(message, error);
    return error;
}

Error Mle::SendDataRequest(const Ip6::Address &aDestination,
                           const uint8_t *     aTlvs,
                           uint8_t             aTlvsLength,
                           uint16_t            aDelay,
                           const uint8_t *     aExtraTlvs,
                           uint8_t             aExtraTlvsLength)
{
    OT_UNUSED_VARIABLE(aExtraTlvs);
    OT_UNUSED_VARIABLE(aExtraTlvsLength);

    Error    error = kErrorNone;
    Message *message;

    RemoveDelayedDataRequestMessage(aDestination);

    VerifyOrExit((message = NewMleMessage()) != nullptr, error = kErrorNoBufs);
    message->SetSubType(Message::kSubTypeMleDataRequest);
    SuccessOrExit(error = AppendHeader(*message, kCommandDataRequest));
    SuccessOrExit(error = AppendTlvRequest(*message, aTlvs, aTlvsLength));

    if (aExtraTlvs != nullptr && aExtraTlvsLength > 0)
    {
        SuccessOrExit(error = message->AppendBytes(aExtraTlvs, aExtraTlvsLength));
    }

    if (aDelay)
    {
        SuccessOrExit(error = AddDelayedResponse(*message, aDestination, aDelay));
        Log(kMessageDelay, kTypeDataRequest, aDestination);
    }
    else
    {
        SuccessOrExit(error = AppendActiveTimestamp(*message));
        SuccessOrExit(error = AppendPendingTimestamp(*message));

        SuccessOrExit(error = SendMessage(*message, aDestination));
        Log(kMessageSend, kTypeDataRequest, aDestination);

        if (!IsRxOnWhenIdle())
        {
            Get<DataPollSender>().SendFastPolls(DataPollSender::kDefaultFastPolls);
        }
    }

exit:
    FreeMessageOnError(message, error);

    if (IsChild() && !IsRxOnWhenIdle())
    {
        mDataRequestState = kDataRequestActive;

        if (mChildUpdateRequestState == kChildUpdateRequestNone)
        {
            ScheduleMessageTransmissionTimer();
        }
    }

    return error;
}

void Mle::ScheduleMessageTransmissionTimer(void)
{
    uint32_t interval = 0;

    switch (mChildUpdateRequestState)
    {
    case kChildUpdateRequestNone:
        break;

    case kChildUpdateRequestPending:
        ExitNow(interval = kChildUpdateRequestPendingDelay);

    case kChildUpdateRequestActive:
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
        // CSL transmitter may respond in next CSL cycle.
        // This condition IsCslCapable() && !IsRxOnWhenIdle() is used instead of
        // IsCslEnabled because during transitions SSED -> MED and MED -> SSED
        // there is a delay in synchronisation of IsRxOnWhenIdle residing in MAC
        // and in MLE, which causes below datapoll interval to be calculated incorrectly.
        if (Get<Mac::Mac>().IsCslCapable() && !IsRxOnWhenIdle())
        {
            ExitNow(interval = Get<Mac::Mac>().GetCslPeriod() * kUsPerTenSymbols / 1000 +
                               static_cast<uint32_t>(kUnicastRetransmissionDelay));
        }
        else
#endif
        {
            ExitNow(interval = kUnicastRetransmissionDelay);
        }
    }

    switch (mDataRequestState)
    {
    case kDataRequestNone:
        break;

    case kDataRequestActive:
        ExitNow(interval = kUnicastRetransmissionDelay);
    }

    if (IsChild() && IsRxOnWhenIdle())
    {
        interval =
            Time::SecToMsec(mTimeout) - static_cast<uint32_t>(kUnicastRetransmissionDelay) * kMaxChildKeepAliveAttempts;
    }

exit:
    if (interval != 0)
    {
        mMessageTransmissionTimer.Start(interval);
    }
    else
    {
        mMessageTransmissionTimer.Stop();
    }
}

void Mle::HandleMessageTransmissionTimer(Timer &aTimer)
{
    aTimer.Get<Mle>().HandleMessageTransmissionTimer();
}

void Mle::HandleMessageTransmissionTimer(void)
{
    // The `mMessageTransmissionTimer` is used for:
    //
    //  - Delaying kEvent notification triggered "Child Update Request" transmission (to allow aggregation),
    //  - Retransmission of "Child Update Request",
    //  - Retransmission of "Data Request" on a child,
    //  - Sending periodic keep-alive "Child Update Request" messages on a non-sleepy (rx-on) child.

    switch (mChildUpdateRequestState)
    {
    case kChildUpdateRequestNone:
        if (mDataRequestState == kDataRequestActive)
        {
            static const uint8_t tlvs[] = {Tlv::kNetworkData};
            Ip6::Address         destination;

            VerifyOrExit(mDataRequestAttempts < kMaxChildKeepAliveAttempts, IgnoreError(BecomeDetached()));

            destination.SetToLinkLocalAddress(mParent.GetExtAddress());

            if (SendDataRequest(destination, tlvs, sizeof(tlvs), 0) == kErrorNone)
            {
                mDataRequestAttempts++;
            }

            ExitNow();
        }

        // Keep-alive "Child Update Request" only on a non-sleepy child
        VerifyOrExit(IsChild() && IsRxOnWhenIdle());
        break;

    case kChildUpdateRequestPending:
        if (Get<Notifier>().IsPending())
        {
            // Here intentionally delay another kChildUpdateRequestPendingDelay
            // cycle to ensure we only send a Child Update Request after we
            // know there are no more pending changes.
            ScheduleMessageTransmissionTimer();
            ExitNow();
        }

        mChildUpdateAttempts = 0;
        break;

    case kChildUpdateRequestActive:
        break;
    }

    VerifyOrExit(mChildUpdateAttempts < kMaxChildKeepAliveAttempts, IgnoreError(BecomeDetached()));

    if (SendChildUpdateRequest() == kErrorNone)
    {
        mChildUpdateAttempts++;
    }

exit:
    return;
}

Error Mle::SendChildUpdateRequest(void)
{
    Error                   error = kErrorNone;
    Ip6::Address            destination;
    Message *               message = nullptr;
    AddressRegistrationMode mode    = kAppendAllAddresses;

    if (!mParent.IsStateValidOrRestoring())
    {
        otLogWarnMle("No valid parent when sending Child Update Request");
        IgnoreError(BecomeDetached());
        ExitNow();
    }

    mChildUpdateRequestState = kChildUpdateRequestActive;
    ScheduleMessageTransmissionTimer();

    VerifyOrExit((message = NewMleMessage()) != nullptr, error = kErrorNoBufs);
    message->SetSubType(Message::kSubTypeMleChildUpdateRequest);
    SuccessOrExit(error = AppendHeader(*message, kCommandChildUpdateRequest));
    SuccessOrExit(error = AppendMode(*message, mDeviceMode));

    switch (mRole)
    {
    case kRoleDetached:
        mParentRequestChallenge.GenerateRandom();
        SuccessOrExit(error = AppendChallenge(*message, mParentRequestChallenge));
        mode = kAppendMeshLocalOnly;
        break;

    case kRoleChild:
        SuccessOrExit(error = AppendSourceAddress(*message));
        SuccessOrExit(error = AppendLeaderData(*message));
        SuccessOrExit(error = AppendTimeout(*message, mTimeout));
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
        if (Get<Mac::Mac>().IsCslEnabled())
        {
            SuccessOrExit(error = AppendCslChannel(*message));
            SuccessOrExit(error = AppendCslTimeout(*message));
        }
#endif
        break;

    case kRoleDisabled:
    case kRoleRouter:
    case kRoleLeader:
        OT_ASSERT(false);
        OT_UNREACHABLE_CODE(break);
    }

    if (!IsFullThreadDevice())
    {
        SuccessOrExit(error = AppendAddressRegistration(*message, mode));
    }

    destination.SetToLinkLocalAddress(mParent.GetExtAddress());
    SuccessOrExit(error = SendMessage(*message, destination));

    Log(kMessageSend, kTypeChildUpdateRequestOfParent, destination);

    if (!IsRxOnWhenIdle())
    {
        Get<MeshForwarder>().SetRxOnWhenIdle(false);
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
        Get<DataPollSender>().SetAttachMode(!Get<Mac::Mac>().IsCslEnabled());
#else
        Get<DataPollSender>().SetAttachMode(true);
#endif
    }
    else
    {
        Get<MeshForwarder>().SetRxOnWhenIdle(true);
    }

exit:
    FreeMessageOnError(message, error);
    return error;
}

Error Mle::SendChildUpdateResponse(const uint8_t *aTlvs, uint8_t aNumTlvs, const Challenge &aChallenge)
{
    Error        error = kErrorNone;
    Ip6::Address destination;
    Message *    message;
    bool         checkAddress = false;

    VerifyOrExit((message = NewMleMessage()) != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = AppendHeader(*message, kCommandChildUpdateResponse));
    SuccessOrExit(error = AppendSourceAddress(*message));
    SuccessOrExit(error = AppendLeaderData(*message));

    for (int i = 0; i < aNumTlvs; i++)
    {
        switch (aTlvs[i])
        {
        case Tlv::kTimeout:
            SuccessOrExit(error = AppendTimeout(*message, mTimeout));
            break;

        case Tlv::kStatus:
            SuccessOrExit(error = AppendStatus(*message, StatusTlv::kError));
            break;

        case Tlv::kAddressRegistration:
            if (!IsFullThreadDevice())
            {
                // We only register the mesh-local address in the "Child
                // Update Response" message and if there are additional
                // addresses to register we follow up with a "Child Update
                // Request".

                SuccessOrExit(error = AppendAddressRegistration(*message, kAppendMeshLocalOnly));
                checkAddress = true;
            }

            break;

        case Tlv::kResponse:
            SuccessOrExit(error = AppendResponse(*message, aChallenge));
            break;

        case Tlv::kLinkFrameCounter:
            SuccessOrExit(error = AppendLinkFrameCounter(*message));
            break;

        case Tlv::kMleFrameCounter:
            SuccessOrExit(error = AppendMleFrameCounter(*message));
            break;
        }
    }

    destination.SetToLinkLocalAddress(mParent.GetExtAddress());
    SuccessOrExit(error = SendMessage(*message, destination));

    Log(kMessageSend, kTypeChildUpdateResponseOfParent, destination);

    if (checkAddress && HasUnregisteredAddress())
    {
        IgnoreError(SendChildUpdateRequest());
    }

exit:
    FreeMessageOnError(message, error);
    return error;
}

void Mle::SendAnnounce(uint8_t aChannel, AnnounceMode aMode)
{
    Ip6::Address destination;

    destination.SetToLinkLocalAllNodesMulticast();

    SendAnnounce(aChannel, destination, aMode);
}

void Mle::SendAnnounce(uint8_t aChannel, const Ip6::Address &aDestination, AnnounceMode aMode)
{
    Error              error = kErrorNone;
    ChannelTlv         channel;
    MeshCoP::Timestamp activeTimestamp;
    Message *          message = nullptr;

    VerifyOrExit(Get<Mac::Mac>().GetSupportedChannelMask().ContainsChannel(aChannel), error = kErrorInvalidArgs);
    VerifyOrExit((message = NewMleMessage()) != nullptr, error = kErrorNoBufs);
    message->SetLinkSecurityEnabled(true);
    message->SetSubType(Message::kSubTypeMleAnnounce);
    message->SetChannel(aChannel);
    SuccessOrExit(error = AppendHeader(*message, kCommandAnnounce));

    channel.Init();
    channel.SetChannel(Get<Mac::Mac>().GetPanChannel());
    SuccessOrExit(error = channel.AppendTo(*message));

    switch (aMode)
    {
    case kOrphanAnnounce:
        activeTimestamp.Clear();
        activeTimestamp.SetAuthoritative(true);
        SuccessOrExit(error = Tlv::Append<ActiveTimestampTlv>(*message, activeTimestamp));
        break;

    case kNormalAnnounce:
        SuccessOrExit(error = AppendActiveTimestamp(*message));
        break;
    }

    SuccessOrExit(error = Tlv::Append<PanIdTlv>(*message, Get<Mac::Mac>().GetPanId()));

    SuccessOrExit(error = SendMessage(*message, aDestination));

    otLogInfoMle("Send Announce on channel %d", aChannel);

exit:
    FreeMessageOnError(message, error);
}

Error Mle::GetNextAnnouceChannel(uint8_t &aChannel) const
{
    // This method gets the next channel to send announce on after
    // `aChannel`. Returns `kErrorNotFound` if no more channel in the
    // channel mask after `aChannel`.

    Mac::ChannelMask channelMask;

    if (Get<MeshCoP::ActiveDataset>().GetChannelMask(channelMask) != kErrorNone)
    {
        channelMask = Get<Mac::Mac>().GetSupportedChannelMask();
    }

    return channelMask.GetNextChannel(aChannel);
}

bool Mle::HasMoreChannelsToAnnouce(void) const
{
    uint8_t channel = mAnnounceChannel;

    return GetNextAnnouceChannel(channel) == kErrorNone;
}

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
Error Mle::SendLinkMetricsManagementResponse(const Ip6::Address &aDestination, LinkMetrics::Status aStatus)
{
    Error    error = kErrorNone;
    Message *message;
    Tlv      tlv;
    ot::Tlv  statusSubTlv;

    VerifyOrExit((message = NewMleMessage()) != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = AppendHeader(*message, kCommandLinkMetricsManagementResponse));

    tlv.SetType(Tlv::kLinkMetricsManagement);
    statusSubTlv.SetType(LinkMetrics::SubTlv::kStatus);
    statusSubTlv.SetLength(sizeof(aStatus));
    tlv.SetLength(statusSubTlv.GetSize());

    SuccessOrExit(error = message->Append(tlv));
    SuccessOrExit(error = message->Append(statusSubTlv));
    SuccessOrExit(error = message->Append(aStatus));

    SuccessOrExit(error = SendMessage(*message, aDestination));

exit:
    FreeMessageOnError(message, error);
    return error;
}
#endif

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
Error Mle::SendLinkProbe(const Ip6::Address &aDestination, uint8_t aSeriesId, uint8_t *aBuf, uint8_t aLength)
{
    Error    error = kErrorNone;
    Message *message;
    Tlv      tlv;

    VerifyOrExit((message = NewMleMessage()) != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = AppendHeader(*message, kCommandLinkProbe));

    tlv.SetType(Tlv::kLinkProbe);
    tlv.SetLength(sizeof(aSeriesId) + aLength);

    SuccessOrExit(error = message->Append(tlv));
    SuccessOrExit(error = message->Append(aSeriesId));
    SuccessOrExit(error = message->AppendBytes(aBuf, aLength));

    SuccessOrExit(error = SendMessage(*message, aDestination));

exit:
    FreeMessageOnError(message, error);
    return error;
}
#endif

Error Mle::SendMessage(Message &aMessage, const Ip6::Address &aDestination)
{
    Error            error = kErrorNone;
    Header           header;
    uint32_t         keySequence;
    uint8_t          nonce[Crypto::AesCcm::kNonceSize];
    uint8_t          tag[kMleSecurityTagSize];
    Crypto::AesCcm   aesCcm;
    uint8_t          buf[64];
    uint16_t         length;
    Ip6::MessageInfo messageInfo;

    IgnoreError(aMessage.Read(0, header));

    if (header.GetSecuritySuite() == Header::k154Security)
    {
        header.SetFrameCounter(Get<KeyManager>().GetMleFrameCounter());

        keySequence = Get<KeyManager>().GetCurrentKeySequence();
        header.SetKeyId(keySequence);

        aMessage.WriteBytes(0, &header, header.GetLength());

        Crypto::AesCcm::GenerateNonce(Get<Mac::Mac>().GetExtAddress(), Get<KeyManager>().GetMleFrameCounter(),
                                      Mac::Frame::kSecEncMic32, nonce);

        aesCcm.SetKey(Get<KeyManager>().GetCurrentMleKey());
        aesCcm.Init(16 + 16 + header.GetHeaderLength(), aMessage.GetLength() - (header.GetLength() - 1), sizeof(tag),
                    nonce, sizeof(nonce));

        aesCcm.Header(&mLinkLocal64.GetAddress(), sizeof(mLinkLocal64.GetAddress()));
        aesCcm.Header(&aDestination, sizeof(aDestination));
        aesCcm.Header(header.GetBytes() + 1, header.GetHeaderLength());

        aMessage.SetOffset(header.GetLength() - 1);

        while (aMessage.GetOffset() < aMessage.GetLength())
        {
            length = aMessage.ReadBytes(aMessage.GetOffset(), buf, sizeof(buf));
            aesCcm.Payload(buf, buf, length, Crypto::AesCcm::kEncrypt);
            aMessage.WriteBytes(aMessage.GetOffset(), buf, length);
            aMessage.MoveOffset(length);
        }

        aesCcm.Finalize(tag);
        SuccessOrExit(error = aMessage.AppendBytes(tag, sizeof(tag)));

        Get<KeyManager>().IncrementMleFrameCounter();
    }

    messageInfo.SetPeerAddr(aDestination);
    messageInfo.SetSockAddr(mLinkLocal64.GetAddress());
    messageInfo.SetPeerPort(kUdpPort);
    messageInfo.SetHopLimit(kMleHopLimit);

    SuccessOrExit(error = mSocket.SendTo(aMessage, messageInfo));

exit:
    return error;
}

Error Mle::AddDelayedResponse(Message &aMessage, const Ip6::Address &aDestination, uint16_t aDelay)
{
    Error                   error = kErrorNone;
    DelayedResponseMetadata metadata;

    metadata.mSendTime    = TimerMilli::GetNow() + aDelay;
    metadata.mDestination = aDestination;

    SuccessOrExit(error = metadata.AppendTo(aMessage));
    mDelayedResponses.Enqueue(aMessage);

    mDelayedResponseTimer.FireAtIfEarlier(metadata.mSendTime);

exit:
    return error;
}

void Mle::HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<Mle *>(aContext)->HandleUdpReceive(*static_cast<Message *>(aMessage),
                                                   *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

void Mle::HandleUdpReceive(Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    Error              error = kErrorNone;
    Header             header;
    uint32_t           keySequence;
    const KeyMaterial *mleKey;
    uint32_t           frameCounter;
    uint8_t            messageTag[kMleSecurityTagSize];
    uint8_t            nonce[Crypto::AesCcm::kNonceSize];
    Mac::ExtAddress    extAddr;
    Crypto::AesCcm     aesCcm;
    uint16_t           mleOffset;
    uint8_t            buf[64];
    uint16_t           length;
    uint8_t            tag[kMleSecurityTagSize];
    uint8_t            command;
    Neighbor *         neighbor;

    otLogDebgMle("Receive UDP message");

    VerifyOrExit(aMessageInfo.GetLinkInfo() != nullptr);
    VerifyOrExit(aMessageInfo.GetHopLimit() == kMleHopLimit, error = kErrorParse);

    length = aMessage.ReadBytes(aMessage.GetOffset(), &header, sizeof(header));
    VerifyOrExit(header.IsValid() && header.GetLength() <= length, error = kErrorParse);

    if (header.GetSecuritySuite() == Header::kNoSecurity)
    {
        aMessage.MoveOffset(header.GetLength());

        switch (header.GetCommand())
        {
#if OPENTHREAD_FTD
        case kCommandDiscoveryRequest:
            Get<MleRouter>().HandleDiscoveryRequest(aMessage, aMessageInfo);
            break;
#endif

        case kCommandDiscoveryResponse:
            Get<DiscoverScanner>().HandleDiscoveryResponse(aMessage, aMessageInfo);
            break;

        default:
            break;
        }

        ExitNow();
    }

    VerifyOrExit(!IsDisabled(), error = kErrorInvalidState);
    VerifyOrExit(header.GetSecuritySuite() == Header::k154Security, error = kErrorParse);

    keySequence = header.GetKeyId();

    if (keySequence == Get<KeyManager>().GetCurrentKeySequence())
    {
        mleKey = &Get<KeyManager>().GetCurrentMleKey();
    }
    else
    {
        mleKey = &Get<KeyManager>().GetTemporaryMleKey(keySequence);
    }

    VerifyOrExit(aMessage.GetOffset() + header.GetLength() + sizeof(messageTag) <= aMessage.GetLength(),
                 error = kErrorParse);
    aMessage.MoveOffset(header.GetLength() - 1);

    IgnoreError(aMessage.Read(aMessage.GetLength() - sizeof(messageTag), messageTag));
    SuccessOrExit(error = aMessage.SetLength(aMessage.GetLength() - sizeof(messageTag)));

    aMessageInfo.GetPeerAddr().GetIid().ConvertToExtAddress(extAddr);

    frameCounter = header.GetFrameCounter();
    Crypto::AesCcm::GenerateNonce(extAddr, frameCounter, Mac::Frame::kSecEncMic32, nonce);

    aesCcm.SetKey(*mleKey);
    aesCcm.Init(sizeof(aMessageInfo.GetPeerAddr()) + sizeof(aMessageInfo.GetSockAddr()) + header.GetHeaderLength(),
                aMessage.GetLength() - aMessage.GetOffset(), sizeof(messageTag), nonce, sizeof(nonce));

    aesCcm.Header(&aMessageInfo.GetPeerAddr(), sizeof(aMessageInfo.GetPeerAddr()));
    aesCcm.Header(&aMessageInfo.GetSockAddr(), sizeof(aMessageInfo.GetSockAddr()));
    aesCcm.Header(header.GetBytes() + 1, header.GetHeaderLength());

    mleOffset = aMessage.GetOffset();

    while (aMessage.GetOffset() < aMessage.GetLength())
    {
        length = aMessage.ReadBytes(aMessage.GetOffset(), buf, sizeof(buf));
        aesCcm.Payload(buf, buf, length, Crypto::AesCcm::kDecrypt);
#ifndef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
        aMessage.WriteBytes(aMessage.GetOffset(), buf, length);
#endif
        aMessage.MoveOffset(length);
    }

    aesCcm.Finalize(tag);
#ifndef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
    VerifyOrExit(memcmp(messageTag, tag, sizeof(tag)) == 0, error = kErrorSecurity);
#endif

    if (keySequence > Get<KeyManager>().GetCurrentKeySequence())
    {
        Get<KeyManager>().SetCurrentKeySequence(keySequence);
    }

    aMessage.SetOffset(mleOffset);

    IgnoreError(aMessage.Read(aMessage.GetOffset(), command));
    aMessage.MoveOffset(sizeof(command));

    neighbor = (command == kCommandChildIdResponse) ? mNeighborTable.FindParent(extAddr)
                                                    : mNeighborTable.FindNeighbor(extAddr);

    if (neighbor != nullptr && neighbor->IsStateValid())
    {
        if (keySequence == neighbor->GetKeySequence())
        {
#if OPENTHREAD_CONFIG_MULTI_RADIO
            // Only when counter is exactly one off, we allow it to be
            // used for updating radio link info (by `RadioSelector`)
            // before message is dropped as a duplicate. This handles
            // the common case where a broadcast MLE message (such as
            // Link Advertisement) is received over multiple radio
            // links.

            if ((frameCounter + 1) == neighbor->GetMleFrameCounter())
            {
                OT_ASSERT(aMessage.IsRadioTypeSet());
                Get<RadioSelector>().UpdateOnReceive(*neighbor, aMessage.GetRadioType(), /* IsDuplicate */ true);

                // We intentionally exit without setting the error to
                // skip logging "Failed to process UDP" at the exit
                // label. Note that in multi-radio mode, receiving
                // duplicate MLE message (with one-off counter) would
                // be common and ok for broadcast MLE messages (e.g.
                // MLE Link Advertisements).
                ExitNow();
            }
#endif
            VerifyOrExit(frameCounter >= neighbor->GetMleFrameCounter(), error = kErrorDuplicated);
        }
        else
        {
            VerifyOrExit(keySequence > neighbor->GetKeySequence(), error = kErrorDuplicated);
            neighbor->SetKeySequence(keySequence);
            neighbor->GetLinkFrameCounters().Reset();
            neighbor->SetLinkAckFrameCounter(0);
        }

        neighbor->SetMleFrameCounter(frameCounter + 1);
    }

#if OPENTHREAD_CONFIG_MULTI_RADIO
    if (neighbor != nullptr)
    {
        OT_ASSERT(aMessage.IsRadioTypeSet());
        Get<RadioSelector>().UpdateOnReceive(*neighbor, aMessage.GetRadioType(), /* IsDuplicate */ false);
    }
#endif

    switch (command)
    {
    case kCommandAdvertisement:
        HandleAdvertisement(aMessage, aMessageInfo, neighbor);
        break;

    case kCommandDataResponse:
        HandleDataResponse(aMessage, aMessageInfo, neighbor);
        break;

    case kCommandParentResponse:
        HandleParentResponse(aMessage, aMessageInfo, keySequence);
        break;

    case kCommandChildIdResponse:
        HandleChildIdResponse(aMessage, aMessageInfo, neighbor);
        break;

    case kCommandAnnounce:
        HandleAnnounce(aMessage, aMessageInfo);
        break;

    case kCommandChildUpdateRequest:
#if OPENTHREAD_FTD
        if (IsRouterOrLeader())
        {
            Get<MleRouter>().HandleChildUpdateRequest(aMessage, aMessageInfo);
        }
        else
#endif
        {
            HandleChildUpdateRequest(aMessage, aMessageInfo, neighbor);
        }

        break;

    case kCommandChildUpdateResponse:
#if OPENTHREAD_FTD
        if (IsRouterOrLeader())
        {
            Get<MleRouter>().HandleChildUpdateResponse(aMessage, aMessageInfo, keySequence, neighbor);
        }
        else
#endif
        {
            HandleChildUpdateResponse(aMessage, aMessageInfo, neighbor);
        }

        break;

#if OPENTHREAD_FTD
    case kCommandLinkRequest:
        Get<MleRouter>().HandleLinkRequest(aMessage, aMessageInfo, neighbor);
        break;

    case kCommandLinkAccept:
        Get<MleRouter>().HandleLinkAccept(aMessage, aMessageInfo, keySequence, neighbor);
        break;

    case kCommandLinkAcceptAndRequest:
        Get<MleRouter>().HandleLinkAcceptAndRequest(aMessage, aMessageInfo, keySequence, neighbor);
        break;

    case kCommandDataRequest:
        Get<MleRouter>().HandleDataRequest(aMessage, aMessageInfo, neighbor);
        break;

    case kCommandParentRequest:
        Get<MleRouter>().HandleParentRequest(aMessage, aMessageInfo);
        break;

    case kCommandChildIdRequest:
        Get<MleRouter>().HandleChildIdRequest(aMessage, aMessageInfo, keySequence);
        break;

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    case kCommandTimeSync:
        Get<MleRouter>().HandleTimeSync(aMessage, aMessageInfo, neighbor);
        break;
#endif
#endif // OPENTHREAD_FTD

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    case kCommandLinkMetricsManagementRequest:
        HandleLinkMetricsManagementRequest(aMessage, aMessageInfo, neighbor);
        break;
#endif

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
    case kCommandLinkMetricsManagementResponse:
        HandleLinkMetricsManagementResponse(aMessage, aMessageInfo, neighbor);
        break;
#endif

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    case kCommandLinkProbe:
        HandleLinkProbe(aMessage, aMessageInfo, neighbor);
        break;
#endif

    default:
        ExitNow(error = kErrorDrop);
    }

#if OPENTHREAD_CONFIG_MULTI_RADIO
    // If we could not find a neighbor matching the MAC address of the
    // received MLE messages, or if the neighbor is now invalid, we
    // check again after the message is handled with a relaxed neighbor
    // state filer. The processing of the received MLE message may
    // create a new neighbor or change the neighbor table (e.g.,
    // receiving a "Parent Request" from a new child, or processing a
    // "Link Request" from a previous child which is being promoted to a
    // router).

    if ((neighbor == nullptr) || neighbor->IsStateInvalid())
    {
        neighbor = Get<NeighborTable>().FindNeighbor(extAddr, Neighbor::kInStateAnyExceptInvalid);

        if (neighbor != nullptr)
        {
            Get<RadioSelector>().UpdateOnReceive(*neighbor, aMessage.GetRadioType(), /* aIsDuplicate */ false);
        }
    }
#endif

exit:
    LogProcessError(kTypeGenericUdp, error);
}

void Mle::HandleAdvertisement(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, Neighbor *aNeighbor)
{
    Error      error = kErrorNone;
    uint16_t   sourceAddress;
    LeaderData leaderData;
    uint8_t    tlvs[] = {Tlv::kNetworkData};
    uint16_t   delay;

    // Source Address
    SuccessOrExit(error = Tlv::Find<SourceAddressTlv>(aMessage, sourceAddress));

    Log(kMessageReceive, kTypeAdvertisement, aMessageInfo.GetPeerAddr(), sourceAddress);

    // Leader Data
    SuccessOrExit(error = ReadLeaderData(aMessage, leaderData));

    if (!IsDetached())
    {
#if OPENTHREAD_FTD
        if (IsFullThreadDevice())
        {
            SuccessOrExit(error = Get<MleRouter>().HandleAdvertisement(aMessage, aMessageInfo, aNeighbor));
        }
        else
#endif
        {
            if ((aNeighbor == &mParent) && (mParent.GetRloc16() != sourceAddress))
            {
                // Remove stale parent.
                IgnoreError(BecomeDetached());
            }
        }
    }

    switch (mRole)
    {
    case kRoleDisabled:
    case kRoleDetached:
        ExitNow();

    case kRoleChild:
        VerifyOrExit(aNeighbor == &mParent);

        if ((mParent.GetRloc16() == sourceAddress) && (leaderData.GetPartitionId() != mLeaderData.GetPartitionId() ||
                                                       leaderData.GetLeaderRouterId() != GetLeaderId()))
        {
            SetLeaderData(leaderData.GetPartitionId(), leaderData.GetWeighting(), leaderData.GetLeaderRouterId());

#if OPENTHREAD_FTD
            if (IsFullThreadDevice())
            {
                RouteTlv route;

                if ((Tlv::FindTlv(aMessage, route) == kErrorNone) && route.IsValid())
                {
                    // Overwrite Route Data
                    IgnoreError(Get<MleRouter>().ProcessRouteTlv(route));
                }
            }
#endif

            mRetrieveNewNetworkData = true;
        }

        mParent.SetLastHeard(TimerMilli::GetNow());
        break;

    case kRoleRouter:
    case kRoleLeader:
        VerifyOrExit(aNeighbor && aNeighbor->IsStateValid());
        break;
    }

    if (mRetrieveNewNetworkData || IsNetworkDataNewer(leaderData))
    {
        delay = Random::NonCrypto::GetUint16InRange(0, kMleMaxResponseDelay);
        IgnoreError(SendDataRequest(aMessageInfo.GetPeerAddr(), tlvs, sizeof(tlvs), delay));
    }

exit:
    LogProcessError(kTypeAdvertisement, error);
}

void Mle::HandleDataResponse(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, const Neighbor *aNeighbor)
{
    Error error;
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
    uint16_t metricsReportValueOffset;
    uint16_t length;
#endif

    Log(kMessageReceive, kTypeDataResponse, aMessageInfo.GetPeerAddr());

    VerifyOrExit(aNeighbor && aNeighbor->IsStateValid(), error = kErrorSecurity);

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
    if (Tlv::FindTlvValueOffset(aMessage, Tlv::kLinkMetricsReport, metricsReportValueOffset, length) == kErrorNone)
    {
        Get<LinkMetrics::LinkMetrics>().HandleReport(aMessage, metricsReportValueOffset, length,
                                                     aMessageInfo.GetPeerAddr());
    }
#endif

    error = HandleLeaderData(aMessage, aMessageInfo);

    if (mDataRequestState == kDataRequestNone && !IsRxOnWhenIdle())
    {
        // Here simply stops fast data poll request by Mle Data Request.
        // Note that in some cases fast data poll may continue after below stop operation until
        // running out the specified number. E.g. other component also trigger fast poll, and
        // is waiting for response; or the corner case where multiple Mle Data Request attempts
        // happened due to the retransmission mechanism.
        Get<DataPollSender>().StopFastPolls();
    }

exit:
    LogProcessError(kTypeDataResponse, error);
}

bool Mle::IsNetworkDataNewer(const LeaderData &aLeaderData)
{
    int8_t diff;

    if (IsFullNetworkData())
    {
        diff = static_cast<int8_t>(aLeaderData.GetDataVersion() - Get<NetworkData::Leader>().GetVersion());
    }
    else
    {
        diff = static_cast<int8_t>(aLeaderData.GetStableDataVersion() - Get<NetworkData::Leader>().GetStableVersion());
    }

    return (diff > 0);
}

Error Mle::HandleLeaderData(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    Error                     error = kErrorNone;
    LeaderData                leaderData;
    MeshCoP::Timestamp        activeTimestamp;
    MeshCoP::Timestamp        pendingTimestamp;
    const MeshCoP::Timestamp *timestamp;
    bool                      hasActiveTimestamp   = false;
    bool                      hasPendingTimestamp  = false;
    uint16_t                  networkDataOffset    = 0;
    uint16_t                  activeDatasetOffset  = 0;
    uint16_t                  pendingDatasetOffset = 0;
    bool                      dataRequest          = false;
    Tlv                       tlv;

    // Leader Data
    SuccessOrExit(error = ReadLeaderData(aMessage, leaderData));

    if ((leaderData.GetPartitionId() != mLeaderData.GetPartitionId()) ||
        (leaderData.GetWeighting() != mLeaderData.GetWeighting()) || (leaderData.GetLeaderRouterId() != GetLeaderId()))
    {
        if (IsChild())
        {
            SetLeaderData(leaderData.GetPartitionId(), leaderData.GetWeighting(), leaderData.GetLeaderRouterId());
            mRetrieveNewNetworkData = true;
        }
        else
        {
            ExitNow(error = kErrorDrop);
        }
    }
    else if (!mRetrieveNewNetworkData)
    {
        VerifyOrExit(IsNetworkDataNewer(leaderData));
    }

    // Active Timestamp
    switch (Tlv::Find<ActiveTimestampTlv>(aMessage, activeTimestamp))
    {
    case kErrorNone:
        hasActiveTimestamp = true;

        timestamp = Get<MeshCoP::ActiveDataset>().GetTimestamp();

        // if received timestamp does not match the local value and message does not contain the dataset,
        // send MLE Data Request
        if (!IsLeader() && ((timestamp == nullptr) || (timestamp->Compare(activeTimestamp) != 0)) &&
            (Tlv::FindTlvOffset(aMessage, Tlv::kActiveDataset, activeDatasetOffset) != kErrorNone))
        {
            ExitNow(dataRequest = true);
        }

        break;

    case kErrorNotFound:
        break;

    default:
        ExitNow(error = kErrorParse);
    }

    // Pending Timestamp
    switch (Tlv::Find<PendingTimestampTlv>(aMessage, pendingTimestamp))
    {
    case kErrorNone:
        hasPendingTimestamp = true;

        timestamp = Get<MeshCoP::PendingDataset>().GetTimestamp();

        // if received timestamp does not match the local value and message does not contain the dataset,
        // send MLE Data Request
        if (!IsLeader() && ((timestamp == nullptr) || (timestamp->Compare(pendingTimestamp) != 0)) &&
            (Tlv::FindTlvOffset(aMessage, Tlv::kPendingDataset, pendingDatasetOffset) != kErrorNone))
        {
            ExitNow(dataRequest = true);
        }

        break;

    case kErrorNotFound:
        break;

    default:
        ExitNow(error = kErrorParse);
    }

    if (Tlv::FindTlvOffset(aMessage, Tlv::kNetworkData, networkDataOffset) == kErrorNone)
    {
        error =
            Get<NetworkData::Leader>().SetNetworkData(leaderData.GetDataVersion(), leaderData.GetStableDataVersion(),
                                                      !IsFullNetworkData(), aMessage, networkDataOffset);
        SuccessOrExit(error);
    }
    else
    {
        ExitNow(dataRequest = true);
    }

#if OPENTHREAD_FTD
    if (IsLeader())
    {
        Get<NetworkData::Leader>().IncrementVersionAndStableVersion();
    }
    else
#endif
    {
        // Active Dataset
        if (hasActiveTimestamp)
        {
            if (activeDatasetOffset > 0)
            {
                IgnoreError(aMessage.Read(activeDatasetOffset, tlv));
                IgnoreError(Get<MeshCoP::ActiveDataset>().Save(activeTimestamp, aMessage,
                                                               activeDatasetOffset + sizeof(tlv), tlv.GetLength()));
            }
        }

        // Pending Dataset
        if (hasPendingTimestamp)
        {
            if (pendingDatasetOffset > 0)
            {
                IgnoreError(aMessage.Read(pendingDatasetOffset, tlv));
                IgnoreError(Get<MeshCoP::PendingDataset>().Save(pendingTimestamp, aMessage,
                                                                pendingDatasetOffset + sizeof(tlv), tlv.GetLength()));
            }
        }
    }

    mRetrieveNewNetworkData = false;

exit:

    if (dataRequest)
    {
        static const uint8_t tlvs[] = {Tlv::kNetworkData};
        uint16_t             delay;

        if (aMessageInfo.GetSockAddr().IsMulticast())
        {
            delay = Random::NonCrypto::GetUint16InRange(0, kMleMaxResponseDelay);
        }
        else
        {
            // This method may have been called from an MLE request
            // handler.  We add a minimum delay here so that the MLE
            // response is enqueued before the MLE Data Request.
            delay = 10;
        }

        IgnoreError(SendDataRequest(aMessageInfo.GetPeerAddr(), tlvs, sizeof(tlvs), delay));
    }
    else if (error == kErrorNone)
    {
        mDataRequestAttempts = 0;
        mDataRequestState    = kDataRequestNone;

        // Here the `mMessageTransmissionTimer` is intentionally not canceled
        // so that when it fires from its callback a "Child Update" is sent
        // if the device is a rx-on child. This way, even when the timer is
        // reused for retransmission of "Data Request" messages, it is ensured
        // that keep-alive "Child Update Request" messages are send within the
        // child's timeout.
    }

    return error;
}

bool Mle::IsBetterParent(uint16_t               aRloc16,
                         uint8_t                aLinkQuality,
                         uint8_t                aLinkMargin,
                         const ConnectivityTlv &aConnectivityTlv,
                         uint8_t                aVersion,
                         uint8_t                aCslClockAccuracy,
                         uint8_t                aCslUncertainty)
{
    bool rval = false;

    uint8_t candidateLinkQualityIn     = mParentCandidate.GetLinkInfo().GetLinkQuality();
    uint8_t candidateTwoWayLinkQuality = (candidateLinkQualityIn < mParentCandidate.GetLinkQualityOut())
                                             ? candidateLinkQualityIn
                                             : mParentCandidate.GetLinkQualityOut();
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    uint64_t candidateCslMetric = 0;
    uint64_t cslMetric          = 0;
#else
    OT_UNUSED_VARIABLE(aCslClockAccuracy);
    OT_UNUSED_VARIABLE(aCslUncertainty);
#endif

    // Mesh Impacting Criteria
    if (aLinkQuality != candidateTwoWayLinkQuality)
    {
        ExitNow(rval = (aLinkQuality > candidateTwoWayLinkQuality));
    }

    if (IsActiveRouter(aRloc16) != IsActiveRouter(mParentCandidate.GetRloc16()))
    {
        ExitNow(rval = IsActiveRouter(aRloc16));
    }

    if (aConnectivityTlv.GetParentPriority() != mParentPriority)
    {
        ExitNow(rval = (aConnectivityTlv.GetParentPriority() > mParentPriority));
    }

    // Prefer the parent with highest quality links (Link Quality 3 field in Connectivity TLV) to neighbors
    if (aConnectivityTlv.GetLinkQuality3() != mParentLinkQuality3)
    {
        ExitNow(rval = (aConnectivityTlv.GetLinkQuality3() > mParentLinkQuality3));
    }

    // Thread 1.2 Specification 4.5.2.1.2 Child Impacting Criteria
    if (aVersion != mParentCandidate.GetVersion())
    {
        ExitNow(rval = (aVersion > mParentCandidate.GetVersion()));
    }

    if (aConnectivityTlv.GetSedBufferSize() != mParentSedBufferSize)
    {
        ExitNow(rval = (aConnectivityTlv.GetSedBufferSize() > mParentSedBufferSize));
    }

    if (aConnectivityTlv.GetSedDatagramCount() != mParentSedDatagramCount)
    {
        ExitNow(rval = (aConnectivityTlv.GetSedDatagramCount() > mParentSedDatagramCount));
    }

    // Extra rules
    if (aConnectivityTlv.GetLinkQuality2() != mParentLinkQuality2)
    {
        ExitNow(rval = (aConnectivityTlv.GetLinkQuality2() > mParentLinkQuality2));
    }

    if (aConnectivityTlv.GetLinkQuality1() != mParentLinkQuality1)
    {
        ExitNow(rval = (aConnectivityTlv.GetLinkQuality1() > mParentLinkQuality1));
    }

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    // CSL metric
    if (!IsRxOnWhenIdle())
    {
        cslMetric = CalcParentCslMetric(aCslClockAccuracy, aCslUncertainty);
        candidateCslMetric =
            CalcParentCslMetric(mParentCandidate.GetCslClockAccuracy(), mParentCandidate.GetCslClockUncertainty());
        if (candidateCslMetric != cslMetric)
        {
            ExitNow(rval = (cslMetric < candidateCslMetric));
        }
    }
#endif

    rval = (aLinkMargin > mParentLinkMargin);

exit:
    return rval;
}

void Mle::HandleParentResponse(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, uint32_t aKeySequence)
{
    Error                 error    = kErrorNone;
    const ThreadLinkInfo *linkInfo = aMessageInfo.GetThreadLinkInfo();
    Challenge             response;
    uint16_t              version;
    uint16_t              sourceAddress;
    LeaderData            leaderData;
    uint8_t               linkMarginFromTlv;
    uint8_t               linkMargin;
    uint8_t               linkQuality;
    ConnectivityTlv       connectivity;
    uint32_t              linkFrameCounter;
    uint32_t              mleFrameCounter;
    Mac::ExtAddress       extAddress;
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    TimeParameterTlv timeParameter;
#endif
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    CslClockAccuracyTlv clockAccuracy;
#endif

    // Source Address
    SuccessOrExit(error = Tlv::Find<SourceAddressTlv>(aMessage, sourceAddress));

    Log(kMessageReceive, kTypeParentResponse, aMessageInfo.GetPeerAddr(), sourceAddress);

    // Version
    SuccessOrExit(error = Tlv::Find<VersionTlv>(aMessage, version));
    VerifyOrExit(version >= OT_THREAD_VERSION_1_1, error = kErrorParse);

    // Response
    SuccessOrExit(error = ReadResponse(aMessage, response));
    VerifyOrExit(response == mParentRequestChallenge, error = kErrorParse);

    aMessageInfo.GetPeerAddr().GetIid().ConvertToExtAddress(extAddress);

    if (IsChild() && mParent.GetExtAddress() == extAddress)
    {
        mReceivedResponseFromParent = true;
    }

    // Leader Data
    SuccessOrExit(error = ReadLeaderData(aMessage, leaderData));

    // Link Margin
    SuccessOrExit(error = Tlv::Find<LinkMarginTlv>(aMessage, linkMarginFromTlv));

    linkMargin = LinkQualityInfo::ConvertRssToLinkMargin(Get<Mac::Mac>().GetNoiseFloor(), linkInfo->GetRss());

    if (linkMargin > linkMarginFromTlv)
    {
        linkMargin = linkMarginFromTlv;
    }

    linkQuality = LinkQualityInfo::ConvertLinkMarginToLinkQuality(linkMargin);

    // Connectivity
    SuccessOrExit(error = Tlv::FindTlv(aMessage, connectivity));
    VerifyOrExit(connectivity.IsValid(), error = kErrorParse);

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    // CSL Accuracy
    if (Tlv::FindTlv(aMessage, clockAccuracy) != kErrorNone)
    {
        clockAccuracy.SetCslClockAccuracy(kCslWorstCrystalPpm);
        clockAccuracy.SetCslUncertainty(kCslWorstUncertainty);
    }
#endif

    // Share data with application, if requested.
    if (mParentResponseCb)
    {
        otThreadParentResponseInfo parentinfo;

        parentinfo.mExtAddr      = extAddress;
        parentinfo.mRloc16       = sourceAddress;
        parentinfo.mRssi         = linkInfo->GetRss();
        parentinfo.mPriority     = connectivity.GetParentPriority();
        parentinfo.mLinkQuality3 = connectivity.GetLinkQuality3();
        parentinfo.mLinkQuality2 = connectivity.GetLinkQuality2();
        parentinfo.mLinkQuality1 = connectivity.GetLinkQuality1();
        parentinfo.mIsAttached   = IsAttached();

        mParentResponseCb(&parentinfo, mParentResponseCbContext);
    }

#if OPENTHREAD_FTD
    if (IsFullThreadDevice() && !IsDetached())
    {
        int8_t diff = static_cast<int8_t>(connectivity.GetIdSequence() - Get<RouterTable>().GetRouterIdSequence());

        switch (mParentRequestMode)
        {
        case kAttachAny:
            VerifyOrExit(leaderData.GetPartitionId() != mLeaderData.GetPartitionId() || diff > 0);
            break;

        case kAttachSame1:
        case kAttachSame2:
            VerifyOrExit(leaderData.GetPartitionId() == mLeaderData.GetPartitionId());
            VerifyOrExit(diff > 0);
            break;

        case kAttachSameDowngrade:
            VerifyOrExit(leaderData.GetPartitionId() == mLeaderData.GetPartitionId());
            VerifyOrExit(diff >= 0);
            break;

        case kAttachBetter:
            VerifyOrExit(leaderData.GetPartitionId() != mLeaderData.GetPartitionId());

            VerifyOrExit(MleRouter::ComparePartitions(connectivity.GetActiveRouters() <= 1, leaderData,
                                                      Get<MleRouter>().IsSingleton(), mLeaderData) > 0);
            break;
        }
    }
#endif

    // Continue to process the "ParentResponse" if it is from current
    // parent candidate to update the challenge and frame counters.

    if (mParentCandidate.IsStateParentResponse() && (mParentCandidate.GetExtAddress() != extAddress))
    {
        // if already have a candidate parent, only seek a better parent

        int compare = 0;

#if OPENTHREAD_FTD
        if (IsFullThreadDevice())
        {
            compare = MleRouter::ComparePartitions(connectivity.GetActiveRouters() <= 1, leaderData, mParentIsSingleton,
                                                   mParentLeaderData);
        }

        // only consider partitions that are the same or better
        VerifyOrExit(compare >= 0);
#endif

        // only consider better parents if the partitions are the same
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
        VerifyOrExit(compare != 0 ||
                     IsBetterParent(sourceAddress, linkQuality, linkMargin, connectivity, static_cast<uint8_t>(version),
                                    clockAccuracy.GetCslClockAccuracy(), clockAccuracy.GetCslUncertainty()));
#else
        VerifyOrExit(compare != 0 || IsBetterParent(sourceAddress, linkQuality, linkMargin, connectivity,
                                                    static_cast<uint8_t>(version), 0, 0));
#endif
    }

    // Link/MLE Frame Counters
    SuccessOrExit(error = ReadFrameCounters(aMessage, linkFrameCounter, mleFrameCounter));

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE

    // Time Parameter
    if (Tlv::FindTlv(aMessage, timeParameter) == kErrorNone)
    {
        VerifyOrExit(timeParameter.IsValid());

        Get<TimeSync>().SetTimeSyncPeriod(timeParameter.GetTimeSyncPeriod());
        Get<TimeSync>().SetXtalThreshold(timeParameter.GetXtalThreshold());
    }

#if OPENTHREAD_CONFIG_TIME_SYNC_REQUIRED
    else
    {
        // If the time sync feature is required, don't choose the parent which doesn't support it.
        ExitNow();
    }

#endif // OPENTHREAD_CONFIG_TIME_SYNC_REQUIRED
#endif // OPENTHREAD_CONFIG_TIME_SYNC_ENABLE

    // Challenge
    SuccessOrExit(error = ReadChallenge(aMessage, mParentCandidateChallenge));

    mParentCandidate.SetExtAddress(extAddress);
    mParentCandidate.SetRloc16(sourceAddress);
    mParentCandidate.GetLinkFrameCounters().SetAll(linkFrameCounter);
    mParentCandidate.SetLinkAckFrameCounter(linkFrameCounter);
    mParentCandidate.SetMleFrameCounter(mleFrameCounter);
    mParentCandidate.SetVersion(static_cast<uint8_t>(version));
    mParentCandidate.SetDeviceMode(DeviceMode(DeviceMode::kModeFullThreadDevice | DeviceMode::kModeRxOnWhenIdle |
                                              DeviceMode::kModeFullNetworkData));
    mParentCandidate.GetLinkInfo().Clear();
    mParentCandidate.GetLinkInfo().AddRss(linkInfo->GetRss());
    mParentCandidate.ResetLinkFailures();
    mParentCandidate.SetLinkQualityOut(LinkQualityInfo::ConvertLinkMarginToLinkQuality(linkMarginFromTlv));
    mParentCandidate.SetState(Neighbor::kStateParentResponse);
    mParentCandidate.SetKeySequence(aKeySequence);
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    mParentCandidate.SetCslClockAccuracy(clockAccuracy.GetCslClockAccuracy());
    mParentCandidate.SetCslClockUncertainty(clockAccuracy.GetCslUncertainty());
#endif

    mParentPriority         = connectivity.GetParentPriority();
    mParentLinkQuality3     = connectivity.GetLinkQuality3();
    mParentLinkQuality2     = connectivity.GetLinkQuality2();
    mParentLinkQuality1     = connectivity.GetLinkQuality1();
    mParentLeaderCost       = connectivity.GetLeaderCost();
    mParentSedBufferSize    = connectivity.GetSedBufferSize();
    mParentSedDatagramCount = connectivity.GetSedDatagramCount();
    mParentLeaderData       = leaderData;
    mParentIsSingleton      = connectivity.GetActiveRouters() <= 1;
    mParentLinkMargin       = linkMargin;

exit:
    LogProcessError(kTypeParentResponse, error);
}

void Mle::HandleChildIdResponse(const Message &         aMessage,
                                const Ip6::MessageInfo &aMessageInfo,
                                const Neighbor *        aNeighbor)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    Error              error = kErrorNone;
    LeaderData         leaderData;
    uint16_t           sourceAddress;
    uint16_t           shortAddress;
    MeshCoP::Timestamp timestamp;
    Tlv                tlv;
    uint16_t           networkDataOffset;
    uint16_t           offset;

    // Source Address
    SuccessOrExit(error = Tlv::Find<SourceAddressTlv>(aMessage, sourceAddress));

    Log(kMessageReceive, kTypeChildIdResponse, aMessageInfo.GetPeerAddr(), sourceAddress);

    VerifyOrExit(aNeighbor && aNeighbor->IsStateValid(), error = kErrorSecurity);

    VerifyOrExit(mAttachState == kAttachStateChildIdRequest);

    // Leader Data
    SuccessOrExit(error = ReadLeaderData(aMessage, leaderData));

    // ShortAddress
    SuccessOrExit(error = Tlv::Find<Address16Tlv>(aMessage, shortAddress));

    // Network Data
    error = Tlv::FindTlvOffset(aMessage, Tlv::kNetworkData, networkDataOffset);
    SuccessOrExit(error);

    // Active Timestamp
    switch (Tlv::Find<ActiveTimestampTlv>(aMessage, timestamp))
    {
    case kErrorNone:
        // Active Dataset
        if (Tlv::FindTlvOffset(aMessage, Tlv::kActiveDataset, offset) == kErrorNone)
        {
            IgnoreError(aMessage.Read(offset, tlv));
            IgnoreError(Get<MeshCoP::ActiveDataset>().Save(timestamp, aMessage, offset + sizeof(tlv), tlv.GetLength()));
        }
        break;

    case kErrorNotFound:
        break;

    default:
        ExitNow(error = kErrorParse);
    }

    // clear Pending Dataset if device succeed to reattach using stored Pending Dataset
    if (mReattachState == kReattachPending)
    {
        Get<MeshCoP::PendingDataset>().Clear();
    }

    // Pending Timestamp
    switch (Tlv::Find<PendingTimestampTlv>(aMessage, timestamp))
    {
    case kErrorNone:
        // Pending Dataset
        if (Tlv::FindTlvOffset(aMessage, Tlv::kPendingDataset, offset) == kErrorNone)
        {
            IgnoreError(aMessage.Read(offset, tlv));
            IgnoreError(
                Get<MeshCoP::PendingDataset>().Save(timestamp, aMessage, offset + sizeof(tlv), tlv.GetLength()));
        }
        break;

    case kErrorNotFound:
        Get<MeshCoP::PendingDataset>().ClearNetwork();
        break;

    default:
        ExitNow(error = kErrorParse);
    }

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    // Sync to Thread network time
    if (aMessage.GetTimeSyncSeq() != OT_TIME_SYNC_INVALID_SEQ)
    {
        Get<TimeSync>().HandleTimeSyncMessage(aMessage);
    }
#endif

    // Parent Attach Success

    SetStateDetached();

    SetLeaderData(leaderData.GetPartitionId(), leaderData.GetWeighting(), leaderData.GetLeaderRouterId());

    if (!IsRxOnWhenIdle())
    {
        Get<DataPollSender>().SetAttachMode(false);
        Get<MeshForwarder>().SetRxOnWhenIdle(false);
    }
    else
    {
        Get<MeshForwarder>().SetRxOnWhenIdle(true);
    }

#if OPENTHREAD_FTD
    if (IsFullThreadDevice())
    {
        RouteTlv route;

        if (Tlv::FindTlv(aMessage, route) == kErrorNone)
        {
            SuccessOrExit(error = Get<MleRouter>().ProcessRouteTlv(route));
        }
    }
#endif

    mParent = mParentCandidate;
    mParentCandidate.Clear();

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    Get<Mac::Mac>().SetCslParentUncertainty(mParent.GetCslClockUncertainty());
    Get<Mac::Mac>().SetCslParentClockAccuracy(mParent.GetCslClockAccuracy());
#endif

    mParent.SetRloc16(sourceAddress);

    IgnoreError(Get<NetworkData::Leader>().SetNetworkData(leaderData.GetDataVersion(),
                                                          leaderData.GetStableDataVersion(), !IsFullNetworkData(),
                                                          aMessage, networkDataOffset));

    SetStateChild(shortAddress);

exit:
    LogProcessError(kTypeChildIdResponse, error);
}

void Mle::HandleChildUpdateRequest(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, Neighbor *aNeighbor)
{
    static const uint8_t kMaxResponseTlvs = 6;

    Error         error = kErrorNone;
    uint16_t      sourceAddress;
    Challenge     challenge;
    RequestedTlvs requestedTlvs;
    uint8_t       tlvs[kMaxResponseTlvs] = {};
    uint8_t       numTlvs                = 0;

    // Source Address
    SuccessOrExit(error = Tlv::Find<SourceAddressTlv>(aMessage, sourceAddress));

    Log(kMessageReceive, kTypeChildUpdateRequestOfParent, aMessageInfo.GetPeerAddr(), sourceAddress);

    // Challenge
    switch (ReadChallenge(aMessage, challenge))
    {
    case kErrorNone:
        tlvs[numTlvs++] = Tlv::kResponse;
        tlvs[numTlvs++] = Tlv::kMleFrameCounter;
        tlvs[numTlvs++] = Tlv::kLinkFrameCounter;
        break;
    case kErrorNotFound:
        challenge.mLength = 0;
        break;
    default:
        ExitNow(error = kErrorParse);
    }

    if (aNeighbor == &mParent)
    {
        uint8_t status;

        switch (Tlv::Find<StatusTlv>(aMessage, status))
        {
        case kErrorNone:
            VerifyOrExit(status != StatusTlv::kError, IgnoreError(BecomeDetached()));
            break;
        case kErrorNotFound:
            break;
        default:
            ExitNow(error = kErrorParse);
        }

        if (mParent.GetRloc16() != sourceAddress)
        {
            IgnoreError(BecomeDetached());
            ExitNow();
        }

        // Leader Data, Network Data, Active Timestamp, Pending Timestamp
        SuccessOrExit(error = HandleLeaderData(aMessage, aMessageInfo));
    }
    else
    {
        // this device is not a child of the Child Update Request source
        tlvs[numTlvs++] = Tlv::kStatus;
    }

    // TLV Request
    switch (FindTlvRequest(aMessage, requestedTlvs))
    {
    case kErrorNone:
        for (uint8_t i = 0; i < requestedTlvs.mNumTlvs; i++)
        {
            if (numTlvs >= sizeof(tlvs))
            {
                otLogWarnMle("Failed to respond with TLVs: %d of %d", i, requestedTlvs.mNumTlvs);
                break;
            }

            tlvs[numTlvs++] = requestedTlvs.mTlvs[i];
        }
        break;
    case kErrorNotFound:
        break;
    default:
        ExitNow(error = kErrorParse);
    }

#if OPENTHREAD_CONFIG_MULTI_RADIO
    if ((aNeighbor != nullptr) && (challenge.mLength != 0))
    {
        aNeighbor->ClearLastRxFragmentTag();
    }
#endif

    SuccessOrExit(error = SendChildUpdateResponse(tlvs, numTlvs, challenge));

exit:
    LogProcessError(kTypeChildUpdateRequestOfParent, error);
}

void Mle::HandleChildUpdateResponse(const Message &         aMessage,
                                    const Ip6::MessageInfo &aMessageInfo,
                                    const Neighbor *        aNeighbor)
{
    Error     error = kErrorNone;
    uint8_t   status;
    uint8_t   mode;
    Challenge response;
    uint32_t  linkFrameCounter;
    uint32_t  mleFrameCounter;
    uint16_t  sourceAddress;
    uint32_t  timeout;

    Log(kMessageReceive, kTypeChildUpdateResponseOfParent, aMessageInfo.GetPeerAddr());

    switch (mRole)
    {
    case kRoleDetached:
        SuccessOrExit(error = ReadResponse(aMessage, response));
        VerifyOrExit(response == mParentRequestChallenge, error = kErrorSecurity);
        break;

    case kRoleChild:
        VerifyOrExit((aNeighbor == &mParent) && mParent.IsStateValid(), error = kErrorSecurity);
        break;

    default:
        OT_ASSERT(false);
        OT_UNREACHABLE_CODE(break);
    }

    // Status
    if (Tlv::Find<StatusTlv>(aMessage, status) == kErrorNone)
    {
        IgnoreError(BecomeDetached());
        ExitNow();
    }

    // Mode
    SuccessOrExit(error = Tlv::Find<ModeTlv>(aMessage, mode));
    VerifyOrExit(DeviceMode(mode) == mDeviceMode, error = kErrorDrop);

    switch (mRole)
    {
    case kRoleDetached:
        SuccessOrExit(error = ReadFrameCounters(aMessage, linkFrameCounter, mleFrameCounter));

        mParent.GetLinkFrameCounters().SetAll(linkFrameCounter);
        mParent.SetLinkAckFrameCounter(linkFrameCounter);
        mParent.SetMleFrameCounter(mleFrameCounter);

        mParent.SetState(Neighbor::kStateValid);
        SetStateChild(GetRloc16());

        mRetrieveNewNetworkData = true;

        OT_FALL_THROUGH;

    case kRoleChild:
        // Source Address
        SuccessOrExit(error = Tlv::Find<SourceAddressTlv>(aMessage, sourceAddress));

        if (RouterIdFromRloc16(sourceAddress) != RouterIdFromRloc16(GetRloc16()))
        {
            IgnoreError(BecomeDetached());
            ExitNow();
        }

        // Leader Data, Network Data, Active Timestamp, Pending Timestamp
        SuccessOrExit(error = HandleLeaderData(aMessage, aMessageInfo));

        // Timeout optional
        switch (Tlv::Find<TimeoutTlv>(aMessage, timeout))
        {
        case kErrorNone:
            mTimeout = timeout;
            break;
        case kErrorNotFound:
            break;
        default:
            ExitNow(error = kErrorParse);
        }

        if (!IsRxOnWhenIdle())
        {
            Get<DataPollSender>().SetAttachMode(false);
            Get<MeshForwarder>().SetRxOnWhenIdle(false);
        }
        else
        {
            Get<MeshForwarder>().SetRxOnWhenIdle(true);
        }

        break;

    default:
        OT_ASSERT(false);
        OT_UNREACHABLE_CODE(break);
    }

exit:

    if (error == kErrorNone)
    {
        if (mChildUpdateRequestState == kChildUpdateRequestActive)
        {
            mChildUpdateAttempts     = 0;
            mChildUpdateRequestState = kChildUpdateRequestNone;
            ScheduleMessageTransmissionTimer();
        }
    }

    LogProcessError(kTypeChildUpdateResponseOfParent, error);
}

void Mle::HandleAnnounce(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    Error                     error = kErrorNone;
    ChannelTlv                channelTlv;
    MeshCoP::Timestamp        timestamp;
    const MeshCoP::Timestamp *localTimestamp;
    uint8_t                   channel;
    uint16_t                  panId;

    Log(kMessageReceive, kTypeAnnounce, aMessageInfo.GetPeerAddr());

    SuccessOrExit(error = Tlv::FindTlv(aMessage, channelTlv));
    VerifyOrExit(channelTlv.IsValid(), error = kErrorParse);

    channel = static_cast<uint8_t>(channelTlv.GetChannel());

    SuccessOrExit(error = Tlv::Find<ActiveTimestampTlv>(aMessage, timestamp));
    SuccessOrExit(error = Tlv::Find<PanIdTlv>(aMessage, panId));

    localTimestamp = Get<MeshCoP::ActiveDataset>().GetTimestamp();

    if (localTimestamp == nullptr || localTimestamp->Compare(timestamp) > 0)
    {
        // No action is required if device is detached, and current
        // channel and pan-id match the values from the received MLE
        // Announce message.

        VerifyOrExit(!IsDetached() || (Get<Mac::Mac>().GetPanChannel() != channel) ||
                     (Get<Mac::Mac>().GetPanId() != panId));

        if (mAttachState == kAttachStateProcessAnnounce)
        {
            VerifyOrExit(mAlternateTimestamp < timestamp.GetSeconds());
        }

        mAlternateTimestamp = timestamp.GetSeconds();
        mAlternateChannel   = channel;
        mAlternatePanId     = panId;
        SetAttachState(kAttachStateProcessAnnounce);
        mAttachTimer.Start(kAnnounceProcessTimeout);
        mAttachCounter = 0;

        otLogNoteMle("Delay processing Announce - channel %d, panid 0x%02x", channel, panId);
    }
    else if (localTimestamp->Compare(timestamp) < 0)
    {
        SendAnnounce(channel);

#if OPENTHREAD_CONFIG_MLE_SEND_UNICAST_ANNOUNCE_RESPONSE
        SendAnnounce(channel, aMessageInfo.GetPeerAddr());
#endif
    }
    else
    {
        // Timestamps are equal.

#if OPENTHREAD_CONFIG_ANNOUNCE_SENDER_ENABLE
        // Notify `AnnounceSender` of the received Announce
        // message so it can update its state to determine
        // whether to send Announce or not.
        Get<AnnounceSender>().UpdateOnReceivedAnnounce();
#endif
    }

exit:
    LogProcessError(kTypeAnnounce, error);
}

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
void Mle::HandleLinkMetricsManagementRequest(const Message &         aMessage,
                                             const Ip6::MessageInfo &aMessageInfo,
                                             Neighbor *              aNeighbor)
{
    Error               error = kErrorNone;
    LinkMetrics::Status status;

    Log(kMessageReceive, kTypeLinkMetricsManagementRequest, aMessageInfo.GetPeerAddr());

    VerifyOrExit(aNeighbor != nullptr, error = kErrorInvalidState);

    SuccessOrExit(error = Get<LinkMetrics::LinkMetrics>().HandleManagementRequest(aMessage, *aNeighbor, status));
    error = SendLinkMetricsManagementResponse(aMessageInfo.GetPeerAddr(), status);

exit:
    LogProcessError(kTypeLinkMetricsManagementRequest, error);
}

#endif // OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
void Mle::HandleLinkMetricsManagementResponse(const Message &         aMessage,
                                              const Ip6::MessageInfo &aMessageInfo,
                                              Neighbor *              aNeighbor)
{
    Error error = kErrorNone;

    Log(kMessageReceive, kTypeLinkMetricsManagementResponse, aMessageInfo.GetPeerAddr());

    VerifyOrExit(aNeighbor != nullptr, error = kErrorInvalidState);

    error = Get<LinkMetrics::LinkMetrics>().HandleManagementResponse(aMessage, aMessageInfo.GetPeerAddr());

exit:
    LogProcessError(kTypeLinkMetricsManagementResponse, error);
}
#endif // OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
void Mle::HandleLinkProbe(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, Neighbor *aNeighbor)
{
    Error   error = kErrorNone;
    uint8_t seriesId;

    Log(kMessageReceive, kTypeLinkProbe, aMessageInfo.GetPeerAddr());

    SuccessOrExit(error = Get<LinkMetrics::LinkMetrics>().HandleLinkProbe(aMessage, seriesId));
    aNeighbor->AggregateLinkMetrics(seriesId, LinkMetrics::SeriesInfo::kSeriesTypeLinkProbe, aMessage.GetAverageLqi(),
                                    aMessage.GetAverageRss());

exit:
    LogProcessError(kTypeLinkProbe, error);
}
#endif // OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE

void Mle::ProcessAnnounce(void)
{
    uint8_t  newChannel = mAlternateChannel;
    uint16_t newPanId   = mAlternatePanId;

    OT_ASSERT(mAttachState == kAttachStateProcessAnnounce);

    otLogNoteMle("Processing Announce - channel %d, panid 0x%02x", newChannel, newPanId);

    Stop(kKeepNetworkDatasets);

    // Save the current/previous channel and pan-id
    mAlternateChannel   = Get<Mac::Mac>().GetPanChannel();
    mAlternatePanId     = Get<Mac::Mac>().GetPanId();
    mAlternateTimestamp = 0;

    IgnoreError(Get<Mac::Mac>().SetPanChannel(newChannel));
    Get<Mac::Mac>().SetPanId(newPanId);

    IgnoreError(Start(kAnnounceAttach));
}

uint16_t Mle::GetNextHop(uint16_t aDestination) const
{
    OT_UNUSED_VARIABLE(aDestination);
    return (mParent.IsStateValid()) ? mParent.GetRloc16() : static_cast<uint16_t>(Mac::kShortAddrInvalid);
}

bool Mle::IsRoutingLocator(const Ip6::Address &aAddress) const
{
    return IsMeshLocalAddress(aAddress) && aAddress.GetIid().IsRoutingLocator();
}

bool Mle::IsAnycastLocator(const Ip6::Address &aAddress) const
{
    return IsMeshLocalAddress(aAddress) && aAddress.GetIid().IsAnycastLocator();
}

bool Mle::IsMeshLocalAddress(const Ip6::Address &aAddress) const
{
    return (aAddress.GetPrefix() == GetMeshLocalPrefix());
}

Error Mle::CheckReachability(uint16_t aMeshDest, Ip6::Header &aIp6Header)
{
    Error error;

    if ((aMeshDest != GetRloc16()) || Get<ThreadNetif>().HasUnicastAddress(aIp6Header.GetDestination()))
    {
        error = kErrorNone;
    }
    else
    {
        error = kErrorNoRoute;
    }

    return error;
}

#if OPENTHREAD_CONFIG_MLE_INFORM_PREVIOUS_PARENT_ON_REATTACH
void Mle::InformPreviousParent(void)
{
    Error            error   = kErrorNone;
    Message *        message = nullptr;
    Ip6::MessageInfo messageInfo;

    VerifyOrExit((mPreviousParentRloc != Mac::kShortAddrInvalid) && (mPreviousParentRloc != mParent.GetRloc16()));

    mCounters.mParentChanges++;

    VerifyOrExit((message = Get<Ip6::Ip6>().NewMessage(0)) != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = message->SetLength(0));

    messageInfo.SetSockAddr(GetMeshLocal64());
    messageInfo.SetPeerAddr(GetMeshLocal16());
    messageInfo.GetPeerAddr().GetIid().SetLocator(mPreviousParentRloc);

    SuccessOrExit(error = Get<Ip6::Ip6>().SendDatagram(*message, messageInfo, Ip6::kProtoNone));

    otLogNoteMle("Sending message to inform previous parent 0x%04x", mPreviousParentRloc);

exit:

    if (error != kErrorNone)
    {
        otLogWarnMle("Failed to inform previous parent: %s", ErrorToString(error));

        FreeMessage(message);
    }
}
#endif // OPENTHREAD_CONFIG_MLE_INFORM_PREVIOUS_PARENT_ON_REATTACH

#if OPENTHREAD_CONFIG_PARENT_SEARCH_ENABLE
void Mle::HandleParentSearchTimer(Timer &aTimer)
{
    aTimer.Get<Mle>().HandleParentSearchTimer();
}

void Mle::HandleParentSearchTimer(void)
{
    int8_t parentRss;

    otLogInfoMle("PeriodicParentSearch: %s interval passed", mParentSearchIsInBackoff ? "Backoff" : "Check");

    if (mParentSearchBackoffWasCanceled)
    {
        // Backoff can be canceled if the device switches to a new parent.
        // from `UpdateParentSearchState()`. We want to limit this to happen
        // only once within a backoff interval.

        if (TimerMilli::GetNow() - mParentSearchBackoffCancelTime >= kParentSearchBackoffInterval)
        {
            mParentSearchBackoffWasCanceled = false;
            otLogInfoMle("PeriodicParentSearch: Backoff cancellation is allowed on parent switch");
        }
    }

    mParentSearchIsInBackoff = false;

    VerifyOrExit(IsChild());

    parentRss = GetParent().GetLinkInfo().GetAverageRss();
    otLogInfoMle("PeriodicParentSearch: Parent RSS %d", parentRss);
    VerifyOrExit(parentRss != OT_RADIO_RSSI_INVALID);

    if (parentRss < kParentSearchRssThreadhold)
    {
        otLogInfoMle("PeriodicParentSearch: Parent RSS less than %d, searching for new parents",
                     kParentSearchRssThreadhold);
        mParentSearchIsInBackoff = true;
        IgnoreError(BecomeChild(kAttachAny));
    }

exit:
    StartParentSearchTimer();
}

void Mle::StartParentSearchTimer(void)
{
    uint32_t interval;

    interval = Random::NonCrypto::GetUint32InRange(0, kParentSearchJitterInterval);

    if (mParentSearchIsInBackoff)
    {
        interval += kParentSearchBackoffInterval;
    }
    else
    {
        interval += kParentSearchCheckInterval;
    }

    mParentSearchTimer.Start(interval);

    otLogInfoMle("PeriodicParentSearch: (Re)starting timer for %s interval",
                 mParentSearchIsInBackoff ? "backoff" : "check");
}

void Mle::UpdateParentSearchState(void)
{
#if OPENTHREAD_CONFIG_MLE_INFORM_PREVIOUS_PARENT_ON_REATTACH

    // If we are in middle of backoff and backoff was not canceled
    // recently and we recently detached from a previous parent,
    // then we check if the new parent is different from the previous
    // one, and if so, we cancel the backoff mode and also remember
    // the backoff cancel time. This way the canceling of backoff
    // is allowed only once within a backoff window.
    //
    // The reason behind the canceling of the backoff is to handle
    // the scenario where a previous parent is not available for a
    // short duration (e.g., it is going through a software update)
    // and the child switches to a less desirable parent. With this
    // model the child will check for other parents sooner and have
    // the chance to switch back to the original (and possibly
    // preferred) parent more quickly.

    if (mParentSearchIsInBackoff && !mParentSearchBackoffWasCanceled && mParentSearchRecentlyDetached)
    {
        if ((mPreviousParentRloc != Mac::kShortAddrInvalid) && (mPreviousParentRloc != mParent.GetRloc16()))
        {
            mParentSearchIsInBackoff        = false;
            mParentSearchBackoffWasCanceled = true;
            mParentSearchBackoffCancelTime  = TimerMilli::GetNow();
            otLogInfoMle("PeriodicParentSearch: Canceling backoff on switching to a new parent");
        }
    }

#endif // OPENTHREAD_CONFIG_MLE_INFORM_PREVIOUS_PARENT_ON_REATTACH

    mParentSearchRecentlyDetached = false;

    if (!mParentSearchIsInBackoff)
    {
        StartParentSearchTimer();
    }
}
#endif // OPENTHREAD_CONFIG_PARENT_SEARCH_ENABLE

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO) && (OPENTHREAD_CONFIG_LOG_MLE == 1)
void Mle::Log(MessageAction aAction, MessageType aType, const Ip6::Address &aAddress)
{
    Log(aAction, aType, aAddress, Mac::kShortAddrInvalid);
}

void Mle::Log(MessageAction aAction, MessageType aType, const Ip6::Address &aAddress, uint16_t aRloc)
{
    enum : uint8_t
    {
        kRlocStringSize = 17,
    };

    String<kRlocStringSize> rlocString;

    if (aRloc != Mac::kShortAddrInvalid)
    {
        rlocString.Append(",0x%04x", aRloc);
    }

    otLogInfoMle("%s %s%s (%s%s)", MessageActionToString(aAction), MessageTypeToString(aType),
                 MessageTypeActionToSuffixString(aType, aAction), aAddress.ToString().AsCString(),
                 rlocString.AsCString());
}
#endif

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_WARN) && (OPENTHREAD_CONFIG_LOG_MLE == 1)
void Mle::LogProcessError(MessageType aType, Error aError)
{
    LogError(kMessageReceive, aType, aError);
}

void Mle::LogSendError(MessageType aType, Error aError)
{
    LogError(kMessageSend, aType, aError);
}

void Mle::LogError(MessageAction aAction, MessageType aType, Error aError)
{
    if (aError != kErrorNone)
    {
        otLogWarnMle("Failed to %s %s%s: %s", aAction == kMessageSend ? "send" : "process", MessageTypeToString(aType),
                     MessageTypeActionToSuffixString(aType, aAction), ErrorToString(aError));
    }
}

const char *Mle::MessageActionToString(MessageAction aAction)
{
    static const char *const kMessageActionStrings[] = {
        "Send",           // (0) kMessageSend
        "Receive",        // (1) kMessageReceive
        "Delay",          // (2) kMessageDelay
        "Remove Delayed", // (3) kMessageRemoveDelayed
    };

    static_assert(kMessageSend == 0, "kMessageSend value is incorrect");
    static_assert(kMessageReceive == 1, "kMessageReceive value is incorrect");
    static_assert(kMessageDelay == 2, "kMessageDelay value is incorrect");
    static_assert(kMessageRemoveDelayed == 3, "kMessageRemoveDelayed value is incorrect");

    return kMessageActionStrings[aAction];
}

const char *Mle::MessageTypeToString(MessageType aType)
{
    static const char *const kMessageTypeStrings[] = {
        "Advertisement",         // (0)  kTypeAdvertisement
        "Announce",              // (1)  kTypeAnnounce
        "Child ID Request",      // (2)  kTypeChildIdRequest
        "Child ID Request",      // (3)  kTypeChildIdRequestShort
        "Child ID Response",     // (4)  kTypeChildIdResponse
        "Child Update Request",  // (5)  kTypeChildUpdateRequestOfParent
        "Child Update Response", // (6)  kTypeChildUpdateResponseOfParent
        "Data Request",          // (7)  kTypeDataRequest
        "Data Response",         // (8)  kTypeDataResponse
        "Discovery Request",     // (9)  kTypeDiscoveryRequest
        "Discovery Response",    // (10) kTypeDiscoveryResponse
        "delayed message",       // (11) kTypeGenericDelayed
        "UDP",                   // (12) kTypeGenericUdp
        "Parent Request",        // (13) kTypeParentRequestToRouters
        "Parent Request",        // (14) kTypeParentRequestToRoutersReeds
        "Parent Response",       // (15) kTypeParentResponse
#if OPENTHREAD_FTD
        "Address Release",         // (16) kTypeAddressRelease
        "Address Release Reply",   // (17) kTypeAddressReleaseReply
        "Address Reply",           // (18) kTypeAddressReply
        "Address Solicit",         // (19) kTypeAddressSolicit
        "Child Update Request",    // (20) kTypeChildUpdateRequestOfChild
        "Child Update Response",   // (21) kTypeChildUpdateResponseOfChild
        "Child Update Response",   // (22) kTypeChildUpdateResponseOfUnknownChild
        "Link Accept",             // (23) kTypeLinkAccept
        "Link Accept and Request", // (24) kTypeLinkAcceptAndRequest
        "Link Reject",             // (25) kTypeLinkReject
        "Link Request",            // (26) kTypeLinkRequest
        "Parent Request",          // (27) kTypeParentRequest
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
        "Time Sync", // (28) kTypeTimeSync
#endif
#endif
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
        "Link Metrics Management Request",  // (29) kTypeLinkMetricsManagementRequest
        "Link Metrics Management Response", // (30) kTypeLinkMetricsManagementResponse
        "Link Probe",                       // (31) kTypeLinkProbe
#endif
    };

    static_assert(kTypeAdvertisement == 0, "kTypeAdvertisement value is incorrect");
    static_assert(kTypeAnnounce == 1, "kTypeAnnounce value is incorrect");
    static_assert(kTypeChildIdRequest == 2, "kTypeChildIdRequest value is incorrect");
    static_assert(kTypeChildIdRequestShort == 3, "kTypeChildIdRequestShort value is incorrect");
    static_assert(kTypeChildIdResponse == 4, "kTypeChildIdResponse value is incorrect");
    static_assert(kTypeChildUpdateRequestOfParent == 5, "kTypeChildUpdateRequestOfParent value is incorrect");
    static_assert(kTypeChildUpdateResponseOfParent == 6, "kTypeChildUpdateResponseOfParent value is incorrect");
    static_assert(kTypeDataRequest == 7, "kTypeDataRequest value is incorrect");
    static_assert(kTypeDataResponse == 8, "kTypeDataResponse value is incorrect");
    static_assert(kTypeDiscoveryRequest == 9, "kTypeDiscoveryRequest value is incorrect");
    static_assert(kTypeDiscoveryResponse == 10, "kTypeDiscoveryResponse value is incorrect");
    static_assert(kTypeGenericDelayed == 11, "kTypeGenericDelayed value is incorrect");
    static_assert(kTypeGenericUdp == 12, "kTypeGenericUdp value is incorrect");
    static_assert(kTypeParentRequestToRouters == 13, "kTypeParentRequestToRouters value is incorrect");
    static_assert(kTypeParentRequestToRoutersReeds == 14, "kTypeParentRequestToRoutersReeds value is incorrect");
    static_assert(kTypeParentResponse == 15, "kTypeParentResponse value is incorrect");
#if OPENTHREAD_FTD
    static_assert(kTypeAddressRelease == 16, "kTypeAddressRelease value is incorrect");
    static_assert(kTypeAddressReleaseReply == 17, "kTypeAddressReleaseReply value is incorrect");
    static_assert(kTypeAddressReply == 18, "kTypeAddressReply value is incorrect");
    static_assert(kTypeAddressSolicit == 19, "kTypeAddressSolicit value is incorrect");
    static_assert(kTypeChildUpdateRequestOfChild == 20, "kTypeChildUpdateRequestOfChild value is incorrect");
    static_assert(kTypeChildUpdateResponseOfChild == 21, "kTypeChildUpdateResponseOfChild value is incorrect");
    static_assert(kTypeChildUpdateResponseOfUnknownChild == 22, "kTypeChildUpdateResponseOfUnknownChild is incorrect");
    static_assert(kTypeLinkAccept == 23, "kTypeLinkAccept value is incorrect");
    static_assert(kTypeLinkAcceptAndRequest == 24, "kTypeLinkAcceptAndRequest value is incorrect");
    static_assert(kTypeLinkReject == 25, "kTypeLinkReject value is incorrect");
    static_assert(kTypeLinkRequest == 26, "kTypeLinkRequest value is incorrect");
    static_assert(kTypeParentRequest == 27, "kTypeParentRequest value is incorrect");
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    static_assert(kTypeTimeSync == 28, "kTypeTimeSync value is incorrect");
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    static_assert(kTypeLinkMetricsManagementRequest == 29, "kTypeLinkMetricsManagementRequest value is incorrect)");
    static_assert(kTypeLinkMetricsManagementResponse == 30, "kTypeLinkMetricsManagementResponse value is incorrect)");
    static_assert(kTypeLinkProbe == 31, "kTypeLinkProbe value is incorrect)");
#endif
#else // OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    static_assert(kTypeLinkMetricsManagementRequest == 28, "kTypeLinkMetricsManagementRequest value is incorrect)");
    static_assert(kTypeLinkMetricsManagementResponse == 29, "kTypeLinkMetricsManagementResponse value is incorrect)");
    static_assert(kTypeLinkProbe == 30, "kTypeLinkProbe value is incorrect)");
#endif
#endif // OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
#else  // OPENTHREAD_FTD
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    static_assert(kTypeLinkMetricsManagementRequest == 16, "kTypeLinkMetricsManagementRequest value is incorrect)");
    static_assert(kTypeLinkMetricsManagementResponse == 17, "kTypeLinkMetricsManagementResponse value is incorrect)");
    static_assert(kTypeLinkProbe == 18, "kTypeLinkProbe value is incorrect)");
#endif
#endif // OPENTHREAD_FTD

    return kMessageTypeStrings[aType];
}

const char *Mle::MessageTypeActionToSuffixString(MessageType aType, MessageAction aAction)
{
    const char *str = "";

    switch (aType)
    {
    case kTypeChildIdRequestShort:
        str = " - short";
        break;
    case kTypeChildUpdateRequestOfParent:
    case kTypeChildUpdateResponseOfParent:
        str = (aAction == kMessageReceive) ? " from parent" : " to parent";
        break;
    case kTypeParentRequestToRouters:
        str = " to routers";
        break;
    case kTypeParentRequestToRoutersReeds:
        str = " to routers and REEDs";
        break;
#if OPENTHREAD_FTD
    case kTypeChildUpdateRequestOfChild:
    case kTypeChildUpdateResponseOfChild:
        str = (aAction == kMessageReceive) ? " from child" : " to child";
        break;
    case kTypeChildUpdateResponseOfUnknownChild:
        str = (aAction == kMessageReceive) ? " from unknown child" : " to child";
        break;
#endif // OPENTHREAD_FTD
    default:
        break;
    }

    return str;
}

#endif // #if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_WARN) && (OPENTHREAD_CONFIG_LOG_MLE == 1)

const char *Mle::RoleToString(DeviceRole aRole)
{
    static const char *const kRoleStrings[] = {
        "disabled", // (0) kRoleDisabled
        "detached", // (1) kRoleDetached
        "child",    // (2) kRoleChild
        "router",   // (3) kRoleRouter
        "leader",   // (4) kRoleLeader
    };

    static_assert(kRoleDisabled == 0, "kRoleDisabled value is incorrect");
    static_assert(kRoleDetached == 1, "kRoleDetached value is incorrect");
    static_assert(kRoleChild == 2, "kRoleChild value is incorrect");
    static_assert(kRoleRouter == 3, "kRoleRouter value is incorrect");
    static_assert(kRoleLeader == 4, "kRoleLeader value is incorrect");

    return (aRole <= OT_ARRAY_LENGTH(kRoleStrings)) ? kRoleStrings[aRole] : "invalid";
}

// LCOV_EXCL_START

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_NOTE) && (OPENTHREAD_CONFIG_LOG_MLE == 1)

const char *Mle::AttachModeToString(AttachMode aMode)
{
    static const char *const kAttachModeStrings[] = {
        "any-partition",            // (0) kAttachAny
        "same-partition-try-1",     // (1) kAttachSame1
        "same-partition-try-2",     // (2) kAttachSame2
        "better-partition",         // (3) kAttachBetter
        "same-partition-downgrade", // (4) kAttachSameDowngrade
    };

    static_assert(kAttachAny == 0, "kAttachAny value is incorrect");
    static_assert(kAttachSame1 == 1, "kAttachSame1 value is incorrect");
    static_assert(kAttachSame2 == 2, "kAttachSame2 value is incorrect");
    static_assert(kAttachBetter == 3, "kAttachBetter value is incorrect");
    static_assert(kAttachSameDowngrade == 4, "kAttachSameDowngrade value is incorrect");

    return kAttachModeStrings[aMode];
}

const char *Mle::AttachStateToString(AttachState aState)
{
    static const char *const kAttachStateStrings[] = {
        "Idle",             // (0) kAttachStateIdle
        "ProcessAnnounce",  // (1) kAttachStateProcessAnnounce
        "Start",            // (2) kAttachStateStart
        "ParentReqRouters", // (3) kAttachStateParentRequestRouter
        "ParentReqReeds",   // (4) kAttachStateParentRequestReed
        "Announce",         // (5) kAttachStateAnnounce
        "ChildIdReq",       // (6) kAttachStateChildIdRequest
    };

    static_assert(kAttachStateIdle == 0, "kAttachStateIdle value is incorrect");
    static_assert(kAttachStateProcessAnnounce == 1, "kAttachStateProcessAnnounce value is incorrect");
    static_assert(kAttachStateStart == 2, "kAttachStateStart value is incorrect");
    static_assert(kAttachStateParentRequestRouter == 3, "kAttachStateParentRequestRouter value is incorrect");
    static_assert(kAttachStateParentRequestReed == 4, "kAttachStateParentRequestReed value is incorrect");
    static_assert(kAttachStateAnnounce == 5, "kAttachStateAnnounce value is incorrect");
    static_assert(kAttachStateChildIdRequest == 6, "kAttachStateChildIdRequest value is incorrect");

    return kAttachStateStrings[aState];
}

const char *Mle::ReattachStateToString(ReattachState aState)
{
    static const char *const kReattachStateStrings[] = {
        "",                                 // (0) kReattachStop
        "reattaching",                      // (1) kReattachStart
        "reattaching with Active Dataset",  // (2) kReattachActive
        "reattaching with Pending Dataset", // (3) kReattachPending
    };

    static_assert(kReattachStop == 0, "kReattachStop value is incorrect");
    static_assert(kReattachStart == 1, "kReattachStart value is incorrect");
    static_assert(kReattachActive == 2, "kReattachActive value is incorrect");
    static_assert(kReattachPending == 3, "kReattachPending value is incorrect");

    return kReattachStateStrings[aState];
}

#endif // (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_NOTE) && (OPENTHREAD_CONFIG_LOG_MLE == 1)

// LCOV_EXCL_STOP

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
Error Mle::SendLinkMetricsManagementRequest(const Ip6::Address &aDestination, const uint8_t *aSubTlvs, uint8_t aLength)
{
    Error    error = kErrorNone;
    Message *message;
    Tlv      tlv;

    VerifyOrExit((message = NewMleMessage()) != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = AppendHeader(*message, kCommandLinkMetricsManagementRequest));

    // Link Metrics Management TLV
    tlv.SetType(Tlv::kLinkMetricsManagement);
    tlv.SetLength(aLength);

    SuccessOrExit(error = message->AppendBytes(&tlv, sizeof(tlv)));
    SuccessOrExit(error = message->AppendBytes(aSubTlvs, aLength));

    SuccessOrExit(error = SendMessage(*message, aDestination));

exit:
    FreeMessageOnError(message, error);
    return error;
}
#endif

void Mle::RegisterParentResponseStatsCallback(otThreadParentResponseCallback aCallback, void *aContext)
{
    mParentResponseCb        = aCallback;
    mParentResponseCbContext = aContext;
}

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
uint64_t Mle::CalcParentCslMetric(uint8_t aCslClockAccuracy, uint8_t aCslUncertainty)
{
    /*
     * This function calculates the overall time that device will operate on battery
     * by summming sequence of "ON quants" over a period of time.
     */
    const uint64_t usInSecond   = 1000000;
    uint64_t       cslPeriodUs  = kMinCslPeriod * kUsPerTenSymbols;
    uint64_t       cslTimeoutUs = GetCslTimeout() * usInSecond;
    uint64_t       k            = cslTimeoutUs / cslPeriodUs;

    return k * (k + 1) * cslPeriodUs / usInSecond * aCslClockAccuracy + aCslUncertainty * k * kUsPerUncertUnit;
}
#endif

void Mle::Challenge::GenerateRandom(void)
{
    mLength = kMaxChallengeSize;
    IgnoreError(Random::Crypto::FillBuffer(mBuffer, mLength));
}

bool Mle::Challenge::Matches(const uint8_t *aBuffer, uint8_t aLength) const
{
    return (mLength == aLength) && (memcmp(mBuffer, aBuffer, aLength) == 0);
}

void Mle::DelayedResponseMetadata::ReadFrom(const Message &aMessage)
{
    uint16_t length = aMessage.GetLength();

    OT_ASSERT(length >= sizeof(*this));
    IgnoreError(aMessage.Read(length - sizeof(*this), *this));
}

void Mle::DelayedResponseMetadata::RemoveFrom(Message &aMessage) const
{
    Error error = aMessage.SetLength(aMessage.GetLength() - sizeof(*this));
    OT_ASSERT(error == kErrorNone);
    OT_UNUSED_VARIABLE(error);
}

} // namespace Mle
} // namespace ot
