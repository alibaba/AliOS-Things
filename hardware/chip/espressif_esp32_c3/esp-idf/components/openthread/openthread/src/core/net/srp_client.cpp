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

#include "srp_client.hpp"

#if OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/numeric_limits.hpp"
#include "common/random.hpp"
#include "common/settings.hpp"
#include "common/string.hpp"
#include "thread/network_data_service.hpp"

/**
 * @file
 *   This file implements the SRP client.
 */

namespace ot {
namespace Srp {

//---------------------------------------------------------------------
// Client::HostInfo

void Client::HostInfo::Init(void)
{
    Clearable<HostInfo>::Clear();

    // State is directly set on `mState` instead of using `SetState()`
    // to avoid logging.
    mState = OT_SRP_CLIENT_ITEM_STATE_REMOVED;
}

void Client::HostInfo::Clear(void)
{
    Clearable<HostInfo>::Clear();
    SetState(kRemoved);
}

void Client::HostInfo::SetState(ItemState aState)
{
    if (aState != GetState())
    {
        otLogInfoSrp("[client] HostInfo %s -> %s", ItemStateToString(GetState()), ItemStateToString(aState));
        mState = static_cast<otSrpClientItemState>(aState);
    }
}

void Client::HostInfo::SetAddresses(const Ip6::Address *aAddresses, uint8_t aNumAddresses)
{
    mAddresses    = aAddresses;
    mNumAddresses = aNumAddresses;

    otLogInfoSrp("[client] HostInfo set %d addrs", GetNumAddresses());

    for (uint8_t index = 0; index < GetNumAddresses(); index++)
    {
        otLogInfoSrp("[client] %s", GetAddress(index).ToString().AsCString());
    }
}

//---------------------------------------------------------------------
// Client::Service

Error Client::Service::Init(void)
{
    Error error = kErrorNone;

    VerifyOrExit((GetName() != nullptr) && (GetInstanceName() != nullptr), error = kErrorInvalidArgs);
    VerifyOrExit((GetTxtEntries() != nullptr) || (GetNumTxtEntries() == 0), error = kErrorInvalidArgs);

    // State is directly set on `mState` instead of using `SetState()`
    // to avoid logging.
    mState = OT_SRP_CLIENT_ITEM_STATE_REMOVED;

exit:
    return error;
}

void Client::Service::SetState(ItemState aState)
{
    VerifyOrExit(GetState() != aState);

    otLogInfoSrp("[client] Service %s -> %s, \"%s\" \"%s\"", ItemStateToString(GetState()), ItemStateToString(aState),
                 GetInstanceName(), GetName());

    if (aState == kToAdd)
    {
        constexpr uint16_t kSubTypeLabelStringSize = 80;

        String<kSubTypeLabelStringSize> string;

        // Log more details only when entering `kToAdd` state.

        if (HasSubType())
        {
            const char *label;

            for (uint16_t index = 0; (label = GetSubTypeLabelAt(index)) != nullptr; index++)
            {
                string.Append("%s\"%s\"", (index != 0) ? ", " : "", label);
            }
        }

        otLogInfoSrp("[client] subtypes:[%s] port:%d weight:%d prio:%d txts:%d", string.AsCString(), GetPort(),
                     GetWeight(), GetPriority(), GetNumTxtEntries());
    }

    mState = static_cast<otSrpClientItemState>(aState);

exit:
    return;
}

bool Client::Service::Matches(const Service &aOther) const
{
    // This method indicates whether or not two service entries match,
    // i.e., have the same service and instance names. This is intended
    // for use by `LinkedList::FindMatching()` to search within the
    // `mServices` list.

    return (strcmp(GetName(), aOther.GetName()) == 0) && (strcmp(GetInstanceName(), aOther.GetInstanceName()) == 0);
}

//---------------------------------------------------------------------
// Client

const char Client::kDefaultDomainName[] = "default.service.arpa";

Client::Client(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mState(kStateStopped)
    , mTxFailureRetryCount(0)
    , mShouldRemoveKeyLease(false)
#if OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE
    , mAutoStartModeEnabled(kAutoStartDefaultMode)
    , mAutoStartDidSelectServer(false)
    , mAutoStartIsUsingAnycastAddress(false)
#endif
#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    , mServiceKeyRecordEnabled(false)
#endif
    , mUpdateMessageId(0)
    , mRetryWaitInterval(kMinRetryWaitInterval)
    , mAcceptedLeaseInterval(0)
    , mLeaseInterval(kDefaultLease)
    , mKeyLeaseInterval(kDefaultKeyLease)
    , mSocket(aInstance)
    , mCallback(nullptr)
    , mCallbackContext(nullptr)
#if OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE
    , mAutoStartCallback(nullptr)
    , mAutoStartContext(nullptr)
    , mServerSequenceNumber(0)
#if OPENTHREAD_CONFIG_SRP_CLIENT_SWITCH_SERVER_ON_FAILURE
    , mTimoutFailureCount(0)
#endif
#endif
    , mDomainName(kDefaultDomainName)
    , mTimer(aInstance, Client::HandleTimer)
{
    mHostInfo.Init();

    // The `Client` implementation uses different constant array of
    // `ItemState` to define transitions between states in `Pause()`,
    // `Stop()`, `SendUpdate`, and `ProcessResponse()`, or to convert
    // an `ItemState` to string. Here, we assert that the enumeration
    // values are correct.

    static_assert(kToAdd == 0, "kToAdd value is not correct");
    static_assert(kAdding == 1, "kAdding value is not correct");
    static_assert(kToRefresh == 2, "kToRefresh value is not correct");
    static_assert(kRefreshing == 3, "kRefreshing value is not correct");
    static_assert(kToRemove == 4, "kToRemove value is not correct");
    static_assert(kRemoving == 5, "kRemoving value is not correct");
    static_assert(kRegistered == 6, "kRegistered value is not correct");
    static_assert(kRemoved == 7, "kRemoved value is not correct");
}

Error Client::Start(const Ip6::SockAddr &aServerSockAddr, Requester aRequester)
{
    Error error;

    VerifyOrExit(GetState() == kStateStopped,
                 error = (aServerSockAddr == GetServerAddress()) ? kErrorNone : kErrorBusy);

    SuccessOrExit(error = mSocket.Open(Client::HandleUdpReceive, this));
    SuccessOrExit(error = mSocket.Connect(aServerSockAddr));

    otLogInfoSrp("[client] %starting, server %s", (aRequester == kRequesterUser) ? "S" : "Auto-s",
                 aServerSockAddr.ToString().AsCString());

    Resume();

#if OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE
    mAutoStartDidSelectServer = (aRequester == kRequesterAuto);

    if (mAutoStartDidSelectServer)
    {
#if OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE && OPENTHREAD_CONFIG_DNS_CLIENT_DEFAULT_SERVER_ADDRESS_AUTO_SET_ENABLE
        Get<Dns::Client>().UpdateDefaultConfigAddress();
#endif

        if (mAutoStartCallback != nullptr)
        {
            mAutoStartCallback(&aServerSockAddr, mAutoStartContext);
        }
    }
#endif

exit:
    return error;
}

void Client::Stop(Requester aRequester, StopMode aMode)
{
    // Change the state of host info and services so that they are
    // added/removed again once the client is started back. In the
    // case of `kAdding`, we intentionally move to `kToRefresh`
    // instead of `kToAdd` since the server may receive our add
    // request and the item may be registered on the server. This
    // ensures that if we are later asked to remove the item, we do
    // notify server.

    static const ItemState kNewStateOnStop[]{
        /* (0) kToAdd      -> */ kToAdd,
        /* (1) kAdding     -> */ kToRefresh,
        /* (2) kToRefresh  -> */ kToRefresh,
        /* (3) kRefreshing -> */ kToRefresh,
        /* (4) kToRemove   -> */ kToRemove,
        /* (5) kRemoving   -> */ kToRemove,
        /* (6) kRegistered -> */ kToRefresh,
        /* (7) kRemoved    -> */ kRemoved,
    };

    VerifyOrExit(GetState() != kStateStopped);

    // State changes:
    //   kAdding     -> kToRefresh
    //   kRefreshing -> kToRefresh
    //   kRemoving   -> kToRemove
    //   kRegistered -> kToRefresh

    ChangeHostAndServiceStates(kNewStateOnStop);

    IgnoreError(mSocket.Close());

    mShouldRemoveKeyLease = false;
    mTxFailureRetryCount  = 0;

    if (aMode == kResetRetryInterval)
    {
        ResetRetryWaitInterval();
    }

    SetState(kStateStopped);

#if OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE
#if OPENTHREAD_CONFIG_SRP_CLIENT_SWITCH_SERVER_ON_FAILURE
    mTimoutFailureCount = 0;
#endif

    mAutoStartDidSelectServer = false;

    if ((aRequester == kRequesterAuto) && (mAutoStartCallback != nullptr))
    {
        mAutoStartCallback(nullptr, mAutoStartContext);
    }
#endif

exit:
#if OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE
    if (aRequester == kRequesterUser)
    {
        DisableAutoStartMode();
    }
#endif
}

void Client::SetCallback(Callback aCallback, void *aContext)
{
    mCallback        = aCallback;
    mCallbackContext = aContext;
}

void Client::Resume(void)
{
    SetState(kStateUpdated);
    UpdateState();
}

void Client::Pause(void)
{
    // Change the state of host info and services that are are being
    // added or removed so that they are added/removed again once the
    // client is resumed or started back.

    static const ItemState kNewStateOnPause[]{
        /* (0) kToAdd      -> */ kToAdd,
        /* (1) kAdding     -> */ kToRefresh,
        /* (2) kToRefresh  -> */ kToRefresh,
        /* (3) kRefreshing -> */ kToRefresh,
        /* (4) kToRemove   -> */ kToRemove,
        /* (5) kRemoving   -> */ kToRemove,
        /* (6) kRegistered -> */ kRegistered,
        /* (7) kRemoved    -> */ kRemoved,
    };

    // State changes:
    //   kAdding     -> kToRefresh
    //   kRefreshing -> kToRefresh
    //   kRemoving   -> kToRemove

    ChangeHostAndServiceStates(kNewStateOnPause);

    SetState(kStatePaused);
}

void Client::HandleNotifierEvents(Events aEvents)
{
    if (aEvents.Contains(kEventThreadRoleChanged))
    {
        HandleRoleChanged();
    }

#if OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE
    if (aEvents.ContainsAny(kEventThreadNetdataChanged | kEventThreadMeshLocalAddrChanged))
    {
        ProcessAutoStart();
    }
#endif
}

void Client::HandleRoleChanged(void)
{
    if (Get<Mle::Mle>().IsAttached())
    {
        VerifyOrExit(GetState() == kStatePaused);
        Resume();
    }
    else
    {
        VerifyOrExit(GetState() != kStateStopped);
        Pause();
    }

exit:
    return;
}

#if OPENTHREAD_CONFIG_SRP_CLIENT_DOMAIN_NAME_API_ENABLE
Error Client::SetDomainName(const char *aName)
{
    Error error = kErrorNone;

    VerifyOrExit((mHostInfo.GetState() == kToAdd) || (mHostInfo.GetState() == kRemoved), error = kErrorInvalidState);

    mDomainName = (aName != nullptr) ? aName : kDefaultDomainName;
    otLogInfoSrp("[client] Domain name \"%s\"", mDomainName);

exit:
    return error;
}
#endif

Error Client::SetHostName(const char *aName)
{
    Error error = kErrorNone;

    VerifyOrExit(aName != nullptr, error = kErrorInvalidArgs);

    VerifyOrExit((mHostInfo.GetState() == kToAdd) || (mHostInfo.GetState() == kRemoved), error = kErrorInvalidState);

    otLogInfoSrp("[client] Host name \"%s\"", aName);
    mHostInfo.SetName(aName);
    mHostInfo.SetState(kToAdd);
    UpdateState();

exit:
    return error;
}

Error Client::SetHostAddresses(const Ip6::Address *aAddresses, uint8_t aNumAddresses)
{
    Error error = kErrorNone;

    VerifyOrExit((aAddresses != nullptr) && (aNumAddresses > 0), error = kErrorInvalidArgs);

    VerifyOrExit((mHostInfo.GetState() != kToRemove) && (mHostInfo.GetState() != kRemoving),
                 error = kErrorInvalidState);

    if (mHostInfo.GetState() == kRemoved)
    {
        mHostInfo.SetState(kToAdd);
    }
    else if (mHostInfo.GetState() != kToAdd)
    {
        mHostInfo.SetState(kToRefresh);
    }

    mHostInfo.SetAddresses(aAddresses, aNumAddresses);
    UpdateState();

exit:
    return error;
}

Error Client::AddService(Service &aService)
{
    Error error;

    VerifyOrExit(mServices.FindMatching(aService) == nullptr, error = kErrorAlready);

    SuccessOrExit(error = aService.Init());
    mServices.Push(aService);

    aService.SetState(kToAdd);
    UpdateState();

exit:
    return error;
}

Error Client::RemoveService(Service &aService)
{
    Error               error = kErrorNone;
    LinkedList<Service> removedServices;

    VerifyOrExit(mServices.Contains(aService), error = kErrorNotFound);

    UpdateServiceStateToRemove(aService);

    // Check if the service was removed immediately, if so
    // invoke the callback to report the removed service.
    GetRemovedServices(removedServices);

    if (!removedServices.IsEmpty())
    {
        InvokeCallback(kErrorNone, mHostInfo, removedServices.GetHead());
    }

    UpdateState();

exit:
    return error;
}

void Client::UpdateServiceStateToRemove(Service &aService)
{
    if (aService.GetState() == kToAdd)
    {
        // If the service has not been added yet, we can remove it immediately.
        aService.SetState(kRemoved);
    }
    else if (aService.GetState() != kRemoving)
    {
        aService.SetState(kToRemove);
    }
}

Error Client::ClearService(Service &aService)
{
    Error error;

    SuccessOrExit(error = mServices.Remove(aService));
    aService.SetNext(nullptr);
    aService.SetState(kRemoved);
    UpdateState();

exit:
    return error;
}

Error Client::RemoveHostAndServices(bool aShouldRemoveKeyLease, bool aSendUnregToServer)
{
    Error error = kErrorNone;

    otLogInfoSrp("[client] Remove host & services");

    VerifyOrExit(mHostInfo.GetState() != kRemoved, error = kErrorAlready);

    if ((mHostInfo.GetState() == kToRemove) || (mHostInfo.GetState() == kRemoving))
    {
        // Host info remove is already ongoing, if "key lease" remove mode is
        // the same, there is no need to send a new update message.
        VerifyOrExit(mShouldRemoveKeyLease != aShouldRemoveKeyLease);
    }

    mShouldRemoveKeyLease = aShouldRemoveKeyLease;

    for (Service &service : mServices)
    {
        UpdateServiceStateToRemove(service);
    }

    if ((mHostInfo.GetState() == kToAdd) && !aSendUnregToServer)
    {
        // Host info is not added yet (not yet registered with
        // server), so we can remove it and all services immediately.
        mHostInfo.SetState(kRemoved);
        HandleUpdateDone();
        ExitNow();
    }

    mHostInfo.SetState(kToRemove);
    UpdateState();

exit:
    return error;
}

void Client::ClearHostAndServices(void)
{
    otLogInfoSrp("[client] Clear host & services");

    switch (GetState())
    {
    case kStateStopped:
    case kStatePaused:
        break;

    case kStateToUpdate:
    case kStateUpdating:
    case kStateUpdated:
    case kStateToRetry:
        SetState(kStateUpdated);
        break;
    }

    mTxFailureRetryCount = 0;
    ResetRetryWaitInterval();

    mServices.Clear();
    mHostInfo.Clear();
}

void Client::SetState(State aState)
{
    VerifyOrExit(aState != mState);

    otLogInfoSrp("[client] State %s -> %s", StateToString(mState), StateToString(aState));
    mState = aState;

    switch (mState)
    {
    case kStateStopped:
    case kStatePaused:
    case kStateUpdated:
        mTimer.Stop();
        break;

    case kStateToUpdate:
        mTimer.Start(kUpdateTxDelay);
        break;

    case kStateUpdating:
        mTimer.Start(GetRetryWaitInterval());
        break;

    case kStateToRetry:
        break;
    }
exit:
    return;
}

void Client::ChangeHostAndServiceStates(const ItemState *aNewStates)
{
#if OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE && OPENTHREAD_CONFIG_SRP_CLIENT_SAVE_SELECTED_SERVER_ENABLE
    ItemState oldHostState = mHostInfo.GetState();
#endif

    mHostInfo.SetState(aNewStates[mHostInfo.GetState()]);

    for (Service &service : mServices)
    {
        service.SetState(aNewStates[service.GetState()]);
    }

#if OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE && OPENTHREAD_CONFIG_SRP_CLIENT_SAVE_SELECTED_SERVER_ENABLE
    if (mAutoStartModeEnabled && mAutoStartDidSelectServer && (oldHostState != kRegistered) &&
        (mHostInfo.GetState() == kRegistered))
    {
        if (mAutoStartIsUsingAnycastAddress)
        {
            IgnoreError(Get<Settings>().Delete<Settings::SrpClientInfo>());
        }
        else
        {
            Settings::SrpClientInfo info;

            info.SetServerAddress(GetServerAddress().GetAddress());
            info.SetServerPort(GetServerAddress().GetPort());

            IgnoreError(Get<Settings>().Save(info));
        }
    }
#endif
}

void Client::InvokeCallback(Error aError) const
{
    InvokeCallback(aError, mHostInfo, nullptr);
}

void Client::InvokeCallback(Error aError, const HostInfo &aHostInfo, const Service *aRemovedServices) const
{
    VerifyOrExit(mCallback != nullptr);
    mCallback(aError, &aHostInfo, mServices.GetHead(), aRemovedServices, mCallbackContext);

exit:
    return;
}

void Client::SendUpdate(void)
{
    static const ItemState kNewStateOnMessageTx[]{
        /* (0) kToAdd      -> */ kAdding,
        /* (1) kAdding     -> */ kAdding,
        /* (2) kToRefresh  -> */ kRefreshing,
        /* (3) kRefreshing -> */ kRefreshing,
        /* (4) kToRemove   -> */ kRemoving,
        /* (5) kRemoving   -> */ kRemoving,
        /* (6) kRegistered -> */ kRegistered,
        /* (7) kRemoved    -> */ kRemoved,
    };

    Error    error   = kErrorNone;
    Message *message = mSocket.NewMessage(0);

    VerifyOrExit(message != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = PrepareUpdateMessage(*message));
    SuccessOrExit(error = mSocket.SendTo(*message, Ip6::MessageInfo()));

    otLogInfoSrp("[client] Send update");

    // State changes:
    //   kToAdd     -> kAdding
    //   kToRefresh -> kRefreshing
    //   kToRemove  -> kRemoving

    ChangeHostAndServiceStates(kNewStateOnMessageTx);

    // Remember the update message tx time to use later to determine the
    // lease renew time.
    mLeaseRenewTime      = TimerMilli::GetNow();
    mTxFailureRetryCount = 0;

    SetState(kStateUpdating);

    if (!Get<Mle::Mle>().IsRxOnWhenIdle())
    {
        // If device is sleepy send fast polls while waiting for
        // the response from server.
        Get<DataPollSender>().SendFastPolls(kFastPollsAfterUpdateTx);
    }

exit:
    if (error != kErrorNone)
    {
        // If there is an error in preparation or transmission of the
        // update message (e.g., no buffer to allocate message), up to
        // `kMaxTxFailureRetries` times, we wait for a short interval
        // `kTxFailureRetryInterval` and try again. After this, we
        // continue to retry using the `mRetryWaitInterval` (which keeps
        // growing on each failure).

        otLogInfoSrp("[client] Failed to send update: %s", ErrorToString(error));

        FreeMessage(message);

        SetState(kStateToRetry);

        if (mTxFailureRetryCount < kMaxTxFailureRetries)
        {
            uint32_t interval;

            mTxFailureRetryCount++;
            interval = Random::NonCrypto::AddJitter(kTxFailureRetryInterval, kTxFailureRetryJitter);
            mTimer.Start(interval);

            otLogInfoSrp("[client] Quick retry %d in %u msec", mTxFailureRetryCount, interval);

            // Do not report message preparation errors to user
            // until `kMaxTxFailureRetries` are exhausted.
        }
        else
        {
            LogRetryWaitInterval();
            mTimer.Start(Random::NonCrypto::AddJitter(GetRetryWaitInterval(), kRetryIntervalJitter));
            GrowRetryWaitInterval();
            InvokeCallback(error);
        }
    }
}

Error Client::PrepareUpdateMessage(Message &aMessage)
{
    constexpr uint16_t kHeaderOffset = 0;

    Error             error = kErrorNone;
    Dns::UpdateHeader header;
    Info              info;

    info.Clear();

    SuccessOrExit(error = ReadOrGenerateKey(info.mKeyPair));

    // Generate random Message ID and ensure it is different from last one
    do
    {
        SuccessOrExit(error = header.SetRandomMessageId());
    } while (header.GetMessageId() == mUpdateMessageId);

    mUpdateMessageId = header.GetMessageId();

    // SRP Update (DNS Update) message must have exactly one record in
    // Zone section, no records in Prerequisite Section, can have
    // multiple records in Update Section (tracked as they are added),
    // and two records in Additional Data Section (OPT and SIG records).
    // The SIG record itself should not be included in calculation of
    // SIG(0) signature, so the addition record count is set to one
    // here. After signature calculation and appending of SIG record,
    // the additional record count is updated to two and the header is
    // rewritten in the message.

    header.SetZoneRecordCount(1);
    header.SetAdditionalRecordCount(1);
    SuccessOrExit(error = aMessage.Append(header));

    // Prepare Zone section

    info.mDomainNameOffset = aMessage.GetLength();
    SuccessOrExit(error = Dns::Name::AppendName(mDomainName, aMessage));
    SuccessOrExit(error = aMessage.Append(Dns::Zone()));

    // Prepare Update section

    if (mHostInfo.GetState() != kToRemove)
    {
        for (Service &service : mServices)
        {
            SuccessOrExit(error = AppendServiceInstructions(service, aMessage, info));
        }
    }

    SuccessOrExit(error = AppendHostDescriptionInstruction(aMessage, info));

    header.SetUpdateRecordCount(info.mRecordCount);
    aMessage.Write(kHeaderOffset, header);

    // Prepare Additional Data section

    SuccessOrExit(error = AppendUpdateLeaseOptRecord(aMessage));
    SuccessOrExit(error = AppendSignature(aMessage, info));

    header.SetAdditionalRecordCount(2); // Lease OPT and SIG RRs
    aMessage.Write(kHeaderOffset, header);

exit:
    return error;
}

Error Client::ReadOrGenerateKey(Crypto::Ecdsa::P256::KeyPair &aKeyPair)
{
    Error error;

    error = Get<Settings>().Read<Settings::SrpEcdsaKey>(aKeyPair);

    if (error == kErrorNone)
    {
        Crypto::Ecdsa::P256::PublicKey publicKey;

        if (aKeyPair.GetPublicKey(publicKey) == kErrorNone)
        {
            ExitNow();
        }
    }

    SuccessOrExit(error = aKeyPair.Generate());
    IgnoreError(Get<Settings>().Save<Settings::SrpEcdsaKey>(aKeyPair));

exit:
    return error;
}

Error Client::AppendServiceInstructions(Service &aService, Message &aMessage, Info &aInfo)
{
    Error               error = kErrorNone;
    Dns::ResourceRecord rr;
    Dns::SrvRecord      srv;
    bool                removing;
    uint16_t            serviceNameOffset;
    uint16_t            instanceNameOffset;
    uint16_t            offset;

    if (aService.GetState() == kRegistered)
    {
        // If the lease needs to be renewed or if we are close to the
        // renewal time of a registered service, we refresh the service
        // early and include it in this update. This helps put more
        // services on the same lease refresh schedule.

        VerifyOrExit(ShouldRenewEarly(aService));
        aService.SetState(kToRefresh);
    }

    removing = ((aService.GetState() == kToRemove) || (aService.GetState() == kRemoving));

    //----------------------------------
    // Service Discovery Instruction

    // PTR record

    // "service name labels" + (pointer to) domain name.
    serviceNameOffset = aMessage.GetLength();
    SuccessOrExit(error = Dns::Name::AppendMultipleLabels(aService.GetName(), aMessage));
    SuccessOrExit(error = Dns::Name::AppendPointerLabel(aInfo.mDomainNameOffset, aMessage));

    // On remove, we use "Delete an RR from an RRSet" where class is set
    // to NONE and TTL to zero (RFC 2136 - section 2.5.4).

    rr.Init(Dns::ResourceRecord::kTypePtr, removing ? Dns::PtrRecord::kClassNone : Dns::PtrRecord::kClassInternet);
    rr.SetTtl(removing ? 0 : mLeaseInterval);
    offset = aMessage.GetLength();
    SuccessOrExit(error = aMessage.Append(rr));

    // "Instance name" + (pointer to) service name.
    instanceNameOffset = aMessage.GetLength();
    SuccessOrExit(error = Dns::Name::AppendLabel(aService.GetInstanceName(), aMessage));
    SuccessOrExit(error = Dns::Name::AppendPointerLabel(serviceNameOffset, aMessage));

    UpdateRecordLengthInMessage(rr, offset, aMessage);
    aInfo.mRecordCount++;

    if (aService.HasSubType())
    {
        const char *subTypeLabel;
        uint16_t    subServiceNameOffset = 0;

        for (uint16_t index = 0; (subTypeLabel = aService.GetSubTypeLabelAt(index)) != nullptr; ++index)
        {
            // subtype label + "_sub" label + (pointer to) service name.

            SuccessOrExit(error = Dns::Name::AppendLabel(subTypeLabel, aMessage));

            if (index == 0)
            {
                subServiceNameOffset = aMessage.GetLength();
                SuccessOrExit(error = Dns::Name::AppendLabel("_sub", aMessage));
                SuccessOrExit(error = Dns::Name::AppendPointerLabel(serviceNameOffset, aMessage));
            }
            else
            {
                SuccessOrExit(error = Dns::Name::AppendPointerLabel(subServiceNameOffset, aMessage));
            }

            // `rr` is already initialized as PTR (add or remove).
            offset = aMessage.GetLength();
            SuccessOrExit(error = aMessage.Append(rr));

            SuccessOrExit(error = Dns::Name::AppendPointerLabel(instanceNameOffset, aMessage));
            UpdateRecordLengthInMessage(rr, offset, aMessage);
            aInfo.mRecordCount++;
        }
    }

    //----------------------------------
    // Service Description Instruction

    // "Delete all RRsets from a name" for Instance Name.

    SuccessOrExit(error = Dns::Name::AppendPointerLabel(instanceNameOffset, aMessage));
    SuccessOrExit(error = AppendDeleteAllRrsets(aMessage));
    aInfo.mRecordCount++;

    VerifyOrExit(!removing);

    // SRV RR

    SuccessOrExit(error = Dns::Name::AppendPointerLabel(instanceNameOffset, aMessage));
    srv.Init();
    srv.SetTtl(mLeaseInterval);
    srv.SetPriority(aService.GetPriority());
    srv.SetWeight(aService.GetWeight());
    srv.SetPort(aService.GetPort());
    offset = aMessage.GetLength();
    SuccessOrExit(error = aMessage.Append(srv));
    SuccessOrExit(error = AppendHostName(aMessage, aInfo));
    UpdateRecordLengthInMessage(srv, offset, aMessage);
    aInfo.mRecordCount++;

    // TXT RR

    SuccessOrExit(error = Dns::Name::AppendPointerLabel(instanceNameOffset, aMessage));
    rr.Init(Dns::ResourceRecord::kTypeTxt);
    offset = aMessage.GetLength();
    SuccessOrExit(error = aMessage.Append(rr));
    SuccessOrExit(error =
                      Dns::TxtEntry::AppendEntries(aService.GetTxtEntries(), aService.GetNumTxtEntries(), aMessage));
    UpdateRecordLengthInMessage(rr, offset, aMessage);
    aInfo.mRecordCount++;

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    if (mServiceKeyRecordEnabled)
    {
        // KEY RR is optional in "Service Description Instruction". It
        // is added here under `REFERENCE_DEVICE` config and is intended
        // for testing only.

        SuccessOrExit(error = Dns::Name::AppendPointerLabel(instanceNameOffset, aMessage));
        SuccessOrExit(error = AppendKeyRecord(aMessage, aInfo));
    }
#endif

exit:
    return error;
}

Error Client::AppendHostDescriptionInstruction(Message &aMessage, Info &aInfo) const
{
    Error               error = kErrorNone;
    Dns::ResourceRecord rr;

    //----------------------------------
    // Host Description Instruction

    // "Delete all RRsets from a name" for Host Name.

    SuccessOrExit(error = AppendHostName(aMessage, aInfo));
    SuccessOrExit(error = AppendDeleteAllRrsets(aMessage));
    aInfo.mRecordCount++;

    // AAAA RRs

    rr.Init(Dns::ResourceRecord::kTypeAaaa);
    rr.SetTtl(mLeaseInterval);
    rr.SetLength(sizeof(Ip6::Address));

    for (uint8_t index = 0; index < mHostInfo.GetNumAddresses(); index++)
    {
        SuccessOrExit(error = AppendHostName(aMessage, aInfo));
        SuccessOrExit(error = aMessage.Append(rr));
        SuccessOrExit(error = aMessage.Append(mHostInfo.GetAddress(index)));
        aInfo.mRecordCount++;
    }

    // KEY RR

    SuccessOrExit(error = AppendHostName(aMessage, aInfo));
    SuccessOrExit(error = AppendKeyRecord(aMessage, aInfo));

exit:
    return error;
}

Error Client::AppendKeyRecord(Message &aMessage, Info &aInfo) const
{
    Error                          error;
    Dns::KeyRecord                 key;
    Crypto::Ecdsa::P256::PublicKey publicKey;

    key.Init();
    key.SetTtl(mLeaseInterval);
    key.SetFlags(Dns::KeyRecord::kAuthConfidPermitted, Dns::KeyRecord::kOwnerNonZone,
                 Dns::KeyRecord::kSignatoryFlagGeneral);
    key.SetProtocol(Dns::KeyRecord::kProtocolDnsSec);
    key.SetAlgorithm(Dns::KeyRecord::kAlgorithmEcdsaP256Sha256);
    key.SetLength(sizeof(Dns::KeyRecord) - sizeof(Dns::ResourceRecord) + sizeof(Crypto::Ecdsa::P256::PublicKey));
    SuccessOrExit(error = aMessage.Append(key));
    SuccessOrExit(error = aInfo.mKeyPair.GetPublicKey(publicKey));
    SuccessOrExit(error = aMessage.Append(publicKey));
    aInfo.mRecordCount++;

exit:
    return error;
}

Error Client::AppendDeleteAllRrsets(Message &aMessage) const
{
    // "Delete all RRsets from a name" (RFC 2136 - 2.5.3)
    // Name should be already appended in the message.

    Dns::ResourceRecord rr;

    rr.Init(Dns::ResourceRecord::kTypeAny, Dns::ResourceRecord::kClassAny);
    rr.SetTtl(0);
    rr.SetLength(0);

    return aMessage.Append(rr);
}

Error Client::AppendHostName(Message &aMessage, Info &aInfo, bool aDoNotCompress) const
{
    Error error;

    if (aDoNotCompress)
    {
        // Uncompressed (canonical form) of host name is used for SIG(0)
        // calculation.
        SuccessOrExit(error = Dns::Name::AppendMultipleLabels(mHostInfo.GetName(), aMessage));
        error = Dns::Name::AppendName(mDomainName, aMessage);
        ExitNow();
    }

    // If host name was previously added in the message, add it
    // compressed as pointer to the previous one. Otherwise,
    // append it and remember the offset.

    if (aInfo.mHostNameOffset != Info::kUnknownOffset)
    {
        ExitNow(error = Dns::Name::AppendPointerLabel(aInfo.mHostNameOffset, aMessage));
    }

    aInfo.mHostNameOffset = aMessage.GetLength();
    SuccessOrExit(error = Dns::Name::AppendMultipleLabels(mHostInfo.GetName(), aMessage));
    error = Dns::Name::AppendPointerLabel(aInfo.mDomainNameOffset, aMessage);

exit:
    return error;
}

Error Client::AppendUpdateLeaseOptRecord(Message &aMessage) const
{
    Error            error;
    Dns::OptRecord   optRecord;
    Dns::LeaseOption leaseOption;

    // Append empty (root domain) as OPT RR name.
    SuccessOrExit(error = Dns::Name::AppendTerminator(aMessage));

    // `Init()` sets the type and clears (set to zero) the extended
    // Response Code, version and all flags.
    optRecord.Init();
    optRecord.SetUdpPayloadSize(kUdpPayloadSize);
    optRecord.SetDnsSecurityFlag();
    optRecord.SetLength(sizeof(Dns::LeaseOption));

    SuccessOrExit(error = aMessage.Append(optRecord));

    leaseOption.Init();

    if (mHostInfo.GetState() == kToRemove)
    {
        leaseOption.SetLeaseInterval(0);
        leaseOption.SetKeyLeaseInterval(mShouldRemoveKeyLease ? 0 : mKeyLeaseInterval);
    }
    else
    {
        leaseOption.SetLeaseInterval(mLeaseInterval);
        leaseOption.SetKeyLeaseInterval(mKeyLeaseInterval);
    }

    error = aMessage.Append(leaseOption);

exit:
    return error;
}

Error Client::AppendSignature(Message &aMessage, Info &aInfo)
{
    Error                          error;
    Dns::SigRecord                 sig;
    Crypto::Sha256                 sha256;
    Crypto::Sha256::Hash           hash;
    Crypto::Ecdsa::P256::Signature signature;
    uint16_t                       offset;
    uint16_t                       len;

    // Prepare SIG RR: TTL, type covered, labels count should be set
    // to zero. Since we have no clock, inception and expiration time
    // are also set to zero. The RDATA length will be set later (not
    // yet known due to variably (and possible compression) of signer's
    // name.

    sig.Clear();
    sig.Init(Dns::ResourceRecord::kClassAny);
    sig.SetAlgorithm(Dns::KeyRecord::kAlgorithmEcdsaP256Sha256);

    // Append the SIG RR with full uncompressed form of the host name
    // as the signer's name. This is used for SIG(0) calculation only.
    // It will be overwritten with host name compressed.

    offset = aMessage.GetLength();
    SuccessOrExit(error = aMessage.Append(sig));
    SuccessOrExit(error = AppendHostName(aMessage, aInfo, /* aDoNotCompress */ true));

    // Calculate signature (RFC 2931): Calculated over "data" which is
    // concatenation of (1) the SIG RR RDATA wire format (including
    // the canonical form of the signer's name), entirely omitting the
    // signature subfield, (2) DNS query message, including DNS header
    // but not UDP/IP header before the header RR counts have been
    // adjusted for the inclusion of SIG(0).

    sha256.Start();

    // (1) SIG RR RDATA wire format
    len = aMessage.GetLength() - offset - sizeof(Dns::ResourceRecord);
    sha256.Update(aMessage, offset + sizeof(Dns::ResourceRecord), len);

    // (2) Message from DNS header before SIG
    sha256.Update(aMessage, 0, offset);

    sha256.Finish(hash);
    SuccessOrExit(error = aInfo.mKeyPair.Sign(hash, signature));

    // Move back in message and append SIG RR now with compressed host
    // name (as signer's name) along with the calculated signature.

    IgnoreError(aMessage.SetLength(offset));

    // SIG(0) uses owner name of root (single zero byte).
    SuccessOrExit(error = Dns::Name::AppendTerminator(aMessage));

    offset = aMessage.GetLength();
    SuccessOrExit(error = aMessage.Append(sig));
    SuccessOrExit(error = AppendHostName(aMessage, aInfo));
    SuccessOrExit(error = aMessage.Append(signature));
    UpdateRecordLengthInMessage(sig, offset, aMessage);

exit:
    return error;
}

void Client::UpdateRecordLengthInMessage(Dns::ResourceRecord &aRecord, uint16_t aOffset, Message &aMessage) const
{
    // This method is used to calculate an RR DATA length and update
    // (rewrite) it in a message. This should be called immediately
    // after all the fields in the record are written in the message.
    // `aOffset` gives the offset in the message to the start of the
    // record.

    aRecord.SetLength(aMessage.GetLength() - aOffset - sizeof(Dns::ResourceRecord));
    aMessage.Write(aOffset, aRecord);
}

void Client::HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    static_cast<Client *>(aContext)->ProcessResponse(*static_cast<Message *>(aMessage));
}

void Client::ProcessResponse(Message &aMessage)
{
    static const ItemState kNewStateOnUpdateDone[]{
        /* (0) kToAdd      -> */ kToAdd,
        /* (1) kAdding     -> */ kRegistered,
        /* (2) kToRefresh  -> */ kToRefresh,
        /* (3) kRefreshing -> */ kRegistered,
        /* (4) kToRemove   -> */ kToRemove,
        /* (5) kRemoving   -> */ kRemoved,
        /* (6) kRegistered -> */ kRegistered,
        /* (7) kRemoved    -> */ kRemoved,
    };

    Error               error = kErrorNone;
    Dns::UpdateHeader   header;
    uint16_t            offset = aMessage.GetOffset();
    uint16_t            recordCount;
    LinkedList<Service> removedServices;

    VerifyOrExit(GetState() == kStateUpdating);

    SuccessOrExit(error = aMessage.Read(offset, header));

    VerifyOrExit(header.GetType() == Dns::Header::kTypeResponse, error = kErrorParse);
    VerifyOrExit(header.GetQueryType() == Dns::Header::kQueryTypeUpdate, error = kErrorParse);
    VerifyOrExit(header.GetMessageId() == mUpdateMessageId, error = kErrorDrop);

    if (!Get<Mle::Mle>().IsRxOnWhenIdle())
    {
        Get<DataPollSender>().StopFastPolls();
    }

    // Response is for the earlier request message.

    otLogInfoSrp("[client] Received response");

#if OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE && OPENTHREAD_CONFIG_SRP_CLIENT_SWITCH_SERVER_ON_FAILURE
    mTimoutFailureCount = 0;
#endif

    error = Dns::Header::ResponseCodeToError(header.GetResponseCode());

    if (error != kErrorNone)
    {
        otLogInfoSrp("[client] Server rejected %s code:%d", ErrorToString(error), header.GetResponseCode());

        if (mHostInfo.GetState() == kAdding)
        {
            // Since server rejected the update message, we go back to
            // `kToAdd` state to allow user to give a new name using
            // `SetHostName()`.
            mHostInfo.SetState(kToAdd);
        }

        // Wait for the timer to expire to retry. Note that timer is
        // already scheduled for the current wait interval when state
        // was changed to `kStateUpdating`.

        LogRetryWaitInterval();
        GrowRetryWaitInterval();
        SetState(kStateToRetry);
        InvokeCallback(error);

#if OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE && OPENTHREAD_CONFIG_SRP_CLIENT_SWITCH_SERVER_ON_FAILURE
        if ((error == kErrorDuplicated) || (error == kErrorSecurity))
        {
            // If the server rejects the update with specific errors
            // (indicating duplicate name and/or security error), we
            // try to switch the server (we check if another can be
            // found in the Network Data).
            //
            // Note that this is done after invoking the callback and
            // notifying the user of the error from server. This works
            // correctly even if user makes changes from callback
            // (e.g., calls SRP client APIs like `Stop` or disables
            // auto-start), since we have a guard check at the top of
            // `SelectNextServer()` to verify that client is still
            // running and auto-start is enabled and selected the
            // server.

            SelectNextServer(/* aDisallowSwitchOnRegisteredHost */ true);
        }
#endif
        ExitNow(error = kErrorNone);
    }

    offset += sizeof(header);

    // Skip over all sections till Additional Data section
    // SPEC ENHANCEMENT: Server can echo the request back or not
    // include any of RRs. Would be good to explicitly require SRP server
    // to not echo back RRs.

    if (header.GetZoneRecordCount() != 0)
    {
        VerifyOrExit(header.GetZoneRecordCount() == 1, error = kErrorParse);
        SuccessOrExit(error = Dns::Name::ParseName(aMessage, offset));
        VerifyOrExit(offset + sizeof(Dns::Zone) <= aMessage.GetLength(), error = kErrorParse);
        offset += sizeof(Dns::Zone);
    }

    // Check for Update Lease OPT RR. This determines the lease
    // interval accepted by server. If not present, then use the
    // transmitted lease interval from the update request message.

    mAcceptedLeaseInterval = mLeaseInterval;
    recordCount =
        header.GetPrerequisiteRecordCount() + header.GetUpdateRecordCount() + header.GetAdditionalRecordCount();

    while (recordCount > 0)
    {
        uint16_t            startOffset = offset;
        Dns::ResourceRecord rr;

        SuccessOrExit(error = ReadResourceRecord(aMessage, offset, rr));
        recordCount--;

        if (rr.GetType() == Dns::ResourceRecord::kTypeOpt)
        {
            SuccessOrExit(error = ProcessOptRecord(aMessage, startOffset, static_cast<Dns::OptRecord &>(rr)));
        }
    }

    // Calculate the lease renew time based on update message tx time
    // and the lease time. `kLeaseRenewGuardInterval` is used to
    // ensure that we renew the lease before server expires it. In the
    // unlikely (but maybe useful for testing) case where the accepted
    // lease interval is too short (shorter than the guard time) we
    // just use half of the accepted lease interval.

    if (mAcceptedLeaseInterval > kLeaseRenewGuardInterval)
    {
        mLeaseRenewTime += Time::SecToMsec(mAcceptedLeaseInterval - kLeaseRenewGuardInterval);
    }
    else
    {
        mLeaseRenewTime += Time::SecToMsec(mAcceptedLeaseInterval) / 2;
    }

    for (Service &service : mServices)
    {
        if ((service.GetState() == kAdding) || (service.GetState() == kRefreshing))
        {
            service.SetLeaseRenewTime(mLeaseRenewTime);
        }
    }

    // State changes:
    //   kAdding     -> kRegistered
    //   kRefreshing -> kRegistered
    //   kRemoving   -> kRemoved

    ChangeHostAndServiceStates(kNewStateOnUpdateDone);

    HandleUpdateDone();
    UpdateState();

exit:
    if (error != kErrorNone)
    {
        otLogInfoSrp("[client] Failed to process response %s", ErrorToString(error));
    }
}

void Client::HandleUpdateDone(void)
{
    HostInfo            hostInfoCopy = mHostInfo;
    LinkedList<Service> removedServices;

    if (mHostInfo.GetState() == kRemoved)
    {
        mHostInfo.Clear();
    }

    ResetRetryWaitInterval();
    SetState(kStateUpdated);

    GetRemovedServices(removedServices);
    InvokeCallback(kErrorNone, hostInfoCopy, removedServices.GetHead());
}

void Client::GetRemovedServices(LinkedList<Service> &aRemovedServices)
{
    Service *service;
    Service *prev;
    Service *next;

    for (prev = nullptr, service = mServices.GetHead(); service != nullptr; service = next)
    {
        next = service->GetNext();

        if (service->GetState() == kRemoved)
        {
            mServices.PopAfter(prev);
            aRemovedServices.Push(*service);

            // When the service is removed from the list
            // we keep the `prev` pointer same as before.
        }
        else
        {
            prev = service;
        }
    }
}

Error Client::ReadResourceRecord(const Message &aMessage, uint16_t &aOffset, Dns::ResourceRecord &aRecord)
{
    // Reads and skips over a Resource Record (RR) from message at
    // given offset. On success, `aOffset` is updated to point to end
    // of RR.

    Error error;

    SuccessOrExit(error = Dns::Name::ParseName(aMessage, aOffset));
    SuccessOrExit(error = aMessage.Read(aOffset, aRecord));
    VerifyOrExit(aOffset + aRecord.GetSize() <= aMessage.GetLength(), error = kErrorParse);
    aOffset += static_cast<uint16_t>(aRecord.GetSize());

exit:
    return error;
}

Error Client::ProcessOptRecord(const Message &aMessage, uint16_t aOffset, const Dns::OptRecord &aOptRecord)
{
    // Read and process all options (in an OPT RR) from a message.
    // The `aOffset` points to beginning of record in `aMessage`.

    Error    error = kErrorNone;
    uint16_t len;

    IgnoreError(Dns::Name::ParseName(aMessage, aOffset));
    aOffset += sizeof(Dns::OptRecord);

    len = aOptRecord.GetLength();

    while (len > 0)
    {
        Dns::LeaseOption leaseOption;
        Dns::Option &    option = leaseOption;
        uint16_t         size;

        SuccessOrExit(error = aMessage.Read(aOffset, option));

        VerifyOrExit(aOffset + option.GetSize() <= aMessage.GetLength(), error = kErrorParse);

        if ((option.GetOptionCode() == Dns::Option::kUpdateLease) &&
            (option.GetOptionLength() >= Dns::LeaseOption::kOptionLength))
        {
            SuccessOrExit(error = aMessage.Read(aOffset, leaseOption));

            mAcceptedLeaseInterval = leaseOption.GetLeaseInterval();

            if (mAcceptedLeaseInterval > kMaxLease)
            {
                mAcceptedLeaseInterval = kMaxLease;
            }
        }

        size = static_cast<uint16_t>(option.GetSize());
        aOffset += size;
        len -= size;
    }

exit:
    return error;
}

void Client::UpdateState(void)
{
    TimeMilli now               = TimerMilli::GetNow();
    TimeMilli earliestRenewTime = now.GetDistantFuture();
    bool      shouldUpdate      = false;

    VerifyOrExit((GetState() != kStateStopped) && (GetState() != kStatePaused));
    VerifyOrExit(mHostInfo.GetName() != nullptr);

    // Go through the host info and all the services to check if there
    // are any new changes (i.e., anything new to add or remove). This
    // is used to determine whether to send an SRP update message or
    // not. Also keep track of the earliest renew time among the
    // previously registered services. This is used to schedule the
    // timer for next refresh.

    switch (mHostInfo.GetState())
    {
    case kAdding:
    case kRefreshing:
    case kRemoving:
        break;

    case kRegistered:
        if (now < mLeaseRenewTime)
        {
            break;
        }

        mHostInfo.SetState(kToRefresh);

        // Fall through

    case kToAdd:
    case kToRefresh:
        // Make sure we have at least one service and at least one
        // host address, otherwise no need to send SRP update message.
        // The exception is when removing host info where we allow
        // for empty service list.
        VerifyOrExit(!mServices.IsEmpty() && (mHostInfo.GetNumAddresses() > 0));

        // Fall through

    case kToRemove:
        shouldUpdate = true;
        break;

    case kRemoved:
        ExitNow();
    }

    // If host info is being removed, we skip over checking service list
    // for new adds (or removes). This handles the situation where while
    // remove is ongoing and before we get a response from the server,
    // user adds a new service to be registered. We wait for remove to
    // finish (receive response from server) before starting with a new
    // service adds.

    if (mHostInfo.GetState() != kRemoving)
    {
        for (Service &service : mServices)
        {
            switch (service.GetState())
            {
            case kToAdd:
            case kToRefresh:
            case kToRemove:
                shouldUpdate = true;
                break;

            case kRegistered:
                if (service.GetLeaseRenewTime() <= now)
                {
                    service.SetState(kToRefresh);
                    shouldUpdate = true;
                }
                else if (service.GetLeaseRenewTime() < earliestRenewTime)
                {
                    earliestRenewTime = service.GetLeaseRenewTime();
                }

                break;

            case kAdding:
            case kRefreshing:
            case kRemoving:
            case kRemoved:
                break;
            }
        }
    }

    if (shouldUpdate)
    {
        SetState(kStateToUpdate);
        ExitNow();
    }

    if ((GetState() == kStateUpdated) && (earliestRenewTime != now.GetDistantFuture()))
    {
        mTimer.FireAt(earliestRenewTime);
    }

exit:
    return;
}

void Client::GrowRetryWaitInterval(void)
{
    mRetryWaitInterval =
        mRetryWaitInterval / kRetryIntervalGrowthFactorDenominator * kRetryIntervalGrowthFactorNumerator;

    if (mRetryWaitInterval > kMaxRetryWaitInterval)
    {
        mRetryWaitInterval = kMaxRetryWaitInterval;
    }
}

uint32_t Client::GetBoundedLeaseInterval(uint32_t aInterval, uint32_t aDefaultInterval) const
{
    uint32_t boundedInterval = aDefaultInterval;

    if (aInterval != 0)
    {
        boundedInterval = OT_MIN(aInterval, static_cast<uint32_t>(kMaxLease));
    }

    return boundedInterval;
}

bool Client::ShouldRenewEarly(const Service &aService) const
{
    // Check if we reached the service renew time or close to it. The
    // "early renew interval" is used to allow early refresh. It is
    // calculated as a factor of the `mAcceptedLeaseInterval`. The
    // "early lease renew factor" is given as a fraction (numerator and
    // denominator). If the denominator is set to zero (i.e., factor is
    // set to infinity), then service is always included in all SRP
    // update messages.

    bool shouldRenew;

#if OPENTHREAD_CONFIG_SRP_CLIENT_EARLY_LEASE_RENEW_FACTOR_DENOMINATOR != 0
    uint32_t earlyRenewInterval =
        Time::SecToMsec(mAcceptedLeaseInterval) / kEarlyLeaseRenewFactorDenominator * kEarlyLeaseRenewFactorNumerator;

    shouldRenew = (aService.GetLeaseRenewTime() <= TimerMilli::GetNow() + earlyRenewInterval);
#else
    OT_UNUSED_VARIABLE(aService);
    shouldRenew = true;
#endif

    return shouldRenew;
}

void Client::HandleTimer(Timer &aTimer)
{
    aTimer.Get<Client>().HandleTimer();
}

void Client::HandleTimer(void)
{
    switch (GetState())
    {
    case kStateStopped:
    case kStatePaused:
        break;

    case kStateToUpdate:
    case kStateToRetry:
        SendUpdate();
        break;

    case kStateUpdating:
        LogRetryWaitInterval();
        otLogInfoSrp("[client] Timed out, no response");
        GrowRetryWaitInterval();
        SetState(kStateToUpdate);
        InvokeCallback(kErrorResponseTimeout);

#if OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE && OPENTHREAD_CONFIG_SRP_CLIENT_SWITCH_SERVER_ON_FAILURE

        // After certain number of back-to-back timeout failures, we try
        // to switch the server. This is again done after invoking the
        // callback. It works correctly due to the guard check at the
        // top of `SelectNextServer()`.

        if (mTimoutFailureCount < NumericLimits<uint8_t>::kMax)
        {
            mTimoutFailureCount++;
        }

        if (mTimoutFailureCount >= kMaxTimeoutFailuresToSwitchServer)
        {
            SelectNextServer(kDisallowSwitchOnRegisteredHost);
        }
#endif
        break;

    case kStateUpdated:
        UpdateState();
        break;
    }
}

#if OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE

void Client::EnableAutoStartMode(AutoStartCallback aCallback, void *aContext)
{
    mAutoStartCallback = aCallback;
    mAutoStartContext  = aContext;

    VerifyOrExit(!mAutoStartModeEnabled);
    mAutoStartModeEnabled = true;
    ProcessAutoStart();

exit:
    return;
}

void Client::ProcessAutoStart(void)
{
    Ip6::SockAddr                             serverSockAddr;
    bool                                      serverIsAnycast = false;
    NetworkData::Service::DnsSrpAnycast::Info anycastInfo;
#if OPENTHREAD_CONFIG_SRP_CLIENT_SAVE_SELECTED_SERVER_ENABLE
    Settings::SrpClientInfo savedInfo;
    bool                    hasSavedServerInfo = false;
#endif

    VerifyOrExit(mAutoStartModeEnabled);

    serverSockAddr.Clear();

    // If the SRP client is not running and auto start mode is
    // enabled, we check if we can find any SRP server info in the
    // Thread Network Data. If it is already running and the server
    // was chosen by the auto-start feature, then we ensure that the
    // selected server is still present in the Network Data.
    //
    // Two types of "DNS/SRP Service" entries can be present in
    // Network Data, "DNS/SRP Service Anycast Address" model and
    // "DNS/SRP Service Unicast" model. The Anycast entries are
    // preferred over the Unicast entries.

    VerifyOrExit(!IsRunning() || mAutoStartDidSelectServer);

    // Now `IsRunning()` implies `mAutoStartDidSelectServer`.

#if OPENTHREAD_CONFIG_SRP_CLIENT_SAVE_SELECTED_SERVER_ENABLE
    if (!IsRunning())
    {
        hasSavedServerInfo = (Get<Settings>().Read(savedInfo) == kErrorNone);
    }
#endif

    if (Get<NetworkData::Service::Manager>().FindPreferredDnsSrpAnycastInfo(anycastInfo) == kErrorNone)
    {
        if (IsRunning() && mAutoStartIsUsingAnycastAddress && (mServerSequenceNumber == anycastInfo.mSequenceNumber) &&
            (GetServerAddress().GetAddress() == anycastInfo.mAnycastAddress))
        {
            // Client is already using the same anycast address.
            ExitNow();
        }

        otLogInfoSrp("[client] Found anycast server %d", anycastInfo.mSequenceNumber);

        serverSockAddr.SetAddress(anycastInfo.mAnycastAddress);
        serverSockAddr.SetPort(kAnycastServerPort);
        mServerSequenceNumber = anycastInfo.mSequenceNumber;
        serverIsAnycast       = true;
    }
    else
    {
        uint16_t                                  numServers = 0;
        NetworkData::Service::DnsSrpUnicast::Info unicastInfo;
        NetworkData::Service::Manager::Iterator   iterator;

        while (Get<NetworkData::Service::Manager>().GetNextDnsSrpUnicastInfo(iterator, unicastInfo) == kErrorNone)
        {
            if (IsRunning() && !mAutoStartIsUsingAnycastAddress && (GetServerAddress() == unicastInfo.mSockAddr))
            {
                ExitNow();
            }

#if OPENTHREAD_CONFIG_SRP_CLIENT_SAVE_SELECTED_SERVER_ENABLE
            if (hasSavedServerInfo && (unicastInfo.mSockAddr.GetAddress() == savedInfo.GetServerAddress()) &&
                (unicastInfo.mSockAddr.GetPort() == savedInfo.GetServerPort()))
            {
                // Stop the search if we see a match for the previously
                // saved server info in the network data entries.

                serverSockAddr  = unicastInfo.mSockAddr;
                serverIsAnycast = false;
                break;
            }
#endif

            numServers++;

            // Choose a server randomly (with uniform distribution) from
            // the list of servers. As we iterate through server entries,
            // with probability `1/numServers`, we choose to switch the
            // current selected server with the new entry. This approach
            // results in a uniform/same probability of selection among
            // all server entries.

            if ((numServers == 1) || (Random::NonCrypto::GetUint16InRange(0, numServers) == 0))
            {
                serverSockAddr  = unicastInfo.mSockAddr;
                serverIsAnycast = false;
            }
        }
    }

    if (IsRunning())
    {
        Stop(kRequesterAuto, kResetRetryInterval);
    }

    VerifyOrExit(!serverSockAddr.GetAddress().IsUnspecified());

    mAutoStartIsUsingAnycastAddress = serverIsAnycast;
    IgnoreError(Start(serverSockAddr, kRequesterAuto));

exit:
    return;
}

#if OPENTHREAD_CONFIG_SRP_CLIENT_SWITCH_SERVER_ON_FAILURE
void Client::SelectNextServer(bool aDisallowSwitchOnRegisteredHost)
{
    // This method tries to find the next server info entry in the
    // Network Data after the current one selected. If found, it
    // restarts the client with the new server (keeping the retry wait
    // interval as before).

    Ip6::SockAddr serverSockAddr;
    bool          selectNext = false;

    serverSockAddr.Clear();

    // Ensure that client is running, auto-start is enabled and
    // auto-start selected the server.

    VerifyOrExit(IsRunning() && mAutoStartModeEnabled && mAutoStartDidSelectServer);

    if (aDisallowSwitchOnRegisteredHost)
    {
        // Ensure that host info is not yet registered (indicating that no
        // service has yet been registered either).
        VerifyOrExit((mHostInfo.GetState() == kAdding) || (mHostInfo.GetState() == kToAdd));
    }

    // We go through all entries to find the one matching the currently
    // selected one, then set `selectNext` to `true` so to select the
    // next one.

    do
    {
        NetworkData::Service::DnsSrpAnycast::Info anycastInfo;
        NetworkData::Service::DnsSrpUnicast::Info unicastInfo;
        NetworkData::Service::Manager::Iterator   iterator;

        while (Get<NetworkData::Service::Manager>().GetNextDnsSrpAnycastInfo(iterator, anycastInfo) == kErrorNone)
        {
            if (selectNext)
            {
                serverSockAddr.SetAddress(anycastInfo.mAnycastAddress);
                serverSockAddr.SetPort(kAnycastServerPort);
                mServerSequenceNumber           = anycastInfo.mSequenceNumber;
                mAutoStartIsUsingAnycastAddress = true;
                ExitNow();
            }

            if (mAutoStartIsUsingAnycastAddress && (GetServerAddress().GetAddress() == anycastInfo.mAnycastAddress) &&
                (GetServerAddress().GetPort() == kAnycastServerPort))
            {
                selectNext = true;
            }
        }

        iterator.Reset();

        while (Get<NetworkData::Service::Manager>().GetNextDnsSrpUnicastInfo(iterator, unicastInfo) == kErrorNone)
        {
            if (selectNext)
            {
                serverSockAddr                  = unicastInfo.mSockAddr;
                mAutoStartIsUsingAnycastAddress = false;
                ExitNow();
            }

            if (GetServerAddress() == unicastInfo.mSockAddr)
            {
                selectNext = true;
            }
        }

        // We loop back to handle the case where the current entry
        // is the last one.

    } while (selectNext);

    // If we reach here it indicates we could not find the entry
    // associated with currently selected server in the list. This
    // situation is rather unlikely but can still happen if Network
    // Data happens to be changed and the entry removed but
    // the "changed" event from `Notifier` may have not yet been
    // processed (note that events are emitted from their own
    // tasklet). In such a case we keep `serverSockAddr` as empty.

exit:
    if (!serverSockAddr.GetAddress().IsUnspecified() && (GetServerAddress() != serverSockAddr))
    {
        // We specifically update `mHostInfo` to `kToAdd` state. This
        // ensures that `Stop()` will keep it as kToAdd` and we detect
        // that the host info has not been registered yet and allow the
        // `SelectNextServer()` to happen again if the timeouts/failures
        // continue to happen with the new server.

        mHostInfo.SetState(kToAdd);
        Stop(kRequesterAuto, kKeepRetryInterval);
        IgnoreError(Start(serverSockAddr, kRequesterAuto));
    }
}
#endif // OPENTHREAD_CONFIG_SRP_CLIENT_SWITCH_SERVER_ON_FAILURE

#endif // OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE

const char *Client::ItemStateToString(ItemState aState)
{
    static const char *const kItemStateStrings[] = {
        "ToAdd",      // kToAdd      (0)
        "Adding",     // kAdding     (1)
        "ToRefresh",  // kToRefresh  (2)
        "Refreshing", // kRefreshing (3)
        "ToRemove",   // kToRemove   (4)
        "Removing",   // kRemoving   (5)
        "Registered", // kRegistered (6)
        "Removed",    // kRemoved    (7)
    };

    return kItemStateStrings[aState];
}

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO) && (OPENTHREAD_CONFIG_LOG_SRP == 1)

const char *Client::StateToString(State aState)
{
    static const char *const kStateStrings[] = {
        "Stopped",  // kStateStopped  (0)
        "Paused",   // kStatePaused   (1)
        "ToUpdate", // kStateToUpdate (2)
        "Updating", // kStateUpdating (3)
        "Updated",  // kStateUpdated  (4)
        "ToRetry",  // kStateToRetry  (5)
    };

    static_assert(kStateStopped == 0, "kStateStopped value is not correct");
    static_assert(kStatePaused == 1, "kStatePaused value is not correct");
    static_assert(kStateToUpdate == 2, "kStateToUpdate value is not correct");
    static_assert(kStateUpdating == 3, "kStateUpdating value is not correct");
    static_assert(kStateUpdated == 4, "kStateUpdated value is not correct");
    static_assert(kStateToRetry == 5, "kStateToRetry value is not correct");

    return kStateStrings[aState];
}

void Client::LogRetryWaitInterval(void) const
{
    constexpr uint16_t kLogInMsecLimit = 5000; // Max interval (in msec) to log the value in msec unit

    uint32_t interval = GetRetryWaitInterval();

    otLogInfoSrp("[client] Retry interval %u %s", (interval < kLogInMsecLimit) ? interval : Time::MsecToSec(interval),
                 (interval < kLogInMsecLimit) ? "ms" : "sec");
}

#endif // #if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO) && (OPENTHREAD_CONFIG_LOG_SRP == 1)

} // namespace Srp
} // namespace ot

#endif // OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
