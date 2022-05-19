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

/**
 * @file
 *   This file implements MLR management.
 */

#include "mlr_manager.hpp"

#if OPENTHREAD_CONFIG_MLR_ENABLE || (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE)

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "net/ip6_address.hpp"
#include "thread/thread_netif.hpp"
#include "thread/uri_paths.hpp"
#include "utils/slaac_address.hpp"

namespace ot {

MlrManager::MlrManager(Instance &aInstance)
    : InstanceLocator(aInstance)
#if (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE) && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
    , mRegisterMulticastListenersCallback(nullptr)
    , mRegisterMulticastListenersContext(nullptr)
#endif
    , mReregistrationDelay(0)
    , mSendDelay(0)
    , mMlrPending(false)
#if (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE) && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
    , mRegisterMulticastListenersPending(false)
#endif
{
}

void MlrManager::HandleNotifierEvents(Events aEvents)
{
#if OPENTHREAD_CONFIG_MLR_ENABLE
    if (aEvents.Contains(kEventIp6MulticastSubscribed))
    {
        UpdateLocalSubscriptions();
    }
#endif

    if (aEvents.Contains(kEventThreadRoleChanged) && Get<Mle::MleRouter>().IsChild())
    {
        // Reregistration after re-attach
        UpdateReregistrationDelay(true);
    }
}

void MlrManager::HandleBackboneRouterPrimaryUpdate(BackboneRouter::Leader::State               aState,
                                                   const BackboneRouter::BackboneRouterConfig &aConfig)
{
    OT_UNUSED_VARIABLE(aConfig);

    bool needRereg =
        aState == BackboneRouter::Leader::kStateAdded || aState == BackboneRouter::Leader::kStateToTriggerRereg;

    UpdateReregistrationDelay(needRereg);
}

#if OPENTHREAD_CONFIG_MLR_ENABLE
void MlrManager::UpdateLocalSubscriptions(void)
{
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE
    // Check multicast addresses are newly listened against Children
    for (Ip6::Netif::ExternalMulticastAddress &addr :
         Get<ThreadNetif>().IterateExternalMulticastAddresses(Ip6::Address::kTypeMulticastLargerThanRealmLocal))
    {
        if (addr.GetMlrState() == kMlrStateToRegister && IsAddressMlrRegisteredByAnyChild(addr.GetAddress()))
        {
            addr.SetMlrState(kMlrStateRegistered);
        }
    }
#endif

    CheckInvariants();
    ScheduleSend(0);
}

bool MlrManager::IsAddressMlrRegisteredByNetif(const Ip6::Address &aAddress) const
{
    bool ret = false;

    OT_ASSERT(aAddress.IsMulticastLargerThanRealmLocal());

    for (const Ip6::Netif::ExternalMulticastAddress &addr : Get<ThreadNetif>().IterateExternalMulticastAddresses())
    {
        if (addr.GetAddress() == aAddress && addr.GetMlrState() == kMlrStateRegistered)
        {
            ExitNow(ret = true);
        }
    }

exit:
    return ret;
}

#endif // OPENTHREAD_CONFIG_MLR_ENABLE

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE

bool MlrManager::IsAddressMlrRegisteredByAnyChildExcept(const Ip6::Address &aAddress, const Child *aExceptChild) const
{
    bool ret = false;

    OT_ASSERT(aAddress.IsMulticastLargerThanRealmLocal());

    for (Child &child : Get<ChildTable>().Iterate(Child::kInStateValid))
    {
        if (&child != aExceptChild && child.HasMlrRegisteredAddress(aAddress))
        {
            ExitNow(ret = true);
        }
    }

exit:
    return ret;
}

void MlrManager::UpdateProxiedSubscriptions(Child &             aChild,
                                            const Ip6::Address *aOldMlrRegisteredAddresses,
                                            uint16_t            aOldMlrRegisteredAddressNum)
{
    VerifyOrExit(aChild.IsStateValid());

    // Search the new multicast addresses and set its flag accordingly
    for (const Ip6::Address &address : aChild.IterateIp6Addresses(Ip6::Address::kTypeMulticastLargerThanRealmLocal))
    {
        bool isMlrRegistered = false;

        // Check if it's a new multicast address against old addresses
        for (size_t i = 0; i < aOldMlrRegisteredAddressNum; i++)
        {
            if (aOldMlrRegisteredAddresses[i] == address)
            {
                isMlrRegistered = true;
                break;
            }
        }

#if OPENTHREAD_CONFIG_MLR_ENABLE
        // Check if it's a new multicast address against parent Netif
        isMlrRegistered = isMlrRegistered || IsAddressMlrRegisteredByNetif(address);
#endif
        // Check if it's a new multicast address against other Children
        isMlrRegistered = isMlrRegistered || IsAddressMlrRegisteredByAnyChildExcept(address, &aChild);

        aChild.SetAddressMlrState(address, isMlrRegistered ? kMlrStateRegistered : kMlrStateToRegister);
    }

exit:
    LogMulticastAddresses();
    CheckInvariants();

    if (aChild.HasAnyMlrToRegisterAddress())
    {
        ScheduleSend(Random::NonCrypto::GetUint16InRange(1, Mle::kParentAggregateDelay));
    }
}

#endif // OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE

void MlrManager::ScheduleSend(uint16_t aDelay)
{
    OT_ASSERT(!mMlrPending || mSendDelay == 0);

    VerifyOrExit(!mMlrPending);

    if (aDelay == 0)
    {
        mSendDelay = 0;
        SendMulticastListenerRegistration();
    }
    else if (mSendDelay == 0 || mSendDelay > aDelay)
    {
        mSendDelay = aDelay;
    }

    UpdateTimeTickerRegistration();
exit:
    return;
}

void MlrManager::UpdateTimeTickerRegistration(void)
{
    if (mSendDelay == 0 && mReregistrationDelay == 0)
    {
        Get<TimeTicker>().UnregisterReceiver(TimeTicker::kMlrManager);
    }
    else
    {
        Get<TimeTicker>().RegisterReceiver(TimeTicker::kMlrManager);
    }
}

void MlrManager::SendMulticastListenerRegistration(void)
{
    Error           error;
    Mle::MleRouter &mle = Get<Mle::MleRouter>();
    Ip6::Address    addresses[kIp6AddressesNumMax];
    uint8_t         addressesNum = 0;

    VerifyOrExit(!mMlrPending, error = kErrorBusy);
    VerifyOrExit(mle.IsAttached(), error = kErrorInvalidState);
    VerifyOrExit(mle.IsFullThreadDevice() || mle.GetParent().IsThreadVersion1p1(), error = kErrorInvalidState);
    VerifyOrExit(Get<BackboneRouter::Leader>().HasPrimary(), error = kErrorInvalidState);

#if OPENTHREAD_CONFIG_MLR_ENABLE
    // Append Netif multicast addresses
    for (Ip6::Netif::ExternalMulticastAddress &addr :
         Get<ThreadNetif>().IterateExternalMulticastAddresses(Ip6::Address::kTypeMulticastLargerThanRealmLocal))
    {
        if (addressesNum >= kIp6AddressesNumMax)
        {
            break;
        }

        if (addr.GetMlrState() == kMlrStateToRegister)
        {
            AppendToUniqueAddressList(addresses, addressesNum, addr.GetAddress());
            addr.SetMlrState(kMlrStateRegistering);
        }
    }
#endif

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE
    // Append Child multicast addresses
    for (Child &child : Get<ChildTable>().Iterate(Child::kInStateValid))
    {
        if (addressesNum >= kIp6AddressesNumMax)
        {
            break;
        }

        if (!child.HasAnyMlrToRegisterAddress())
        {
            continue;
        }

        for (const Ip6::Address &address : child.IterateIp6Addresses(Ip6::Address::kTypeMulticastLargerThanRealmLocal))
        {
            if (addressesNum >= kIp6AddressesNumMax)
            {
                break;
            }

            if (child.GetAddressMlrState(address) == kMlrStateToRegister)
            {
                AppendToUniqueAddressList(addresses, addressesNum, address);
                child.SetAddressMlrState(address, kMlrStateRegistering);
            }
        }
    }
#endif

    VerifyOrExit(addressesNum > 0, error = kErrorNotFound);
    SuccessOrExit(
        error = SendMulticastListenerRegistrationMessage(
            addresses, addressesNum, nullptr, &MlrManager::HandleMulticastListenerRegistrationResponse, this));

    mMlrPending = true;

    // Generally Thread 1.2 Router would send MLR.req on bebelf for MA (scope >=4) subscribed by its MTD child.
    // When Thread 1.2 MTD attaches to Thread 1.1 parent, 1.2 MTD should send MLR.req to PBBR itself.
    // In this case, Thread 1.2 sleepy end device relies on fast data poll to fetch the response timely.
    if (!Get<Mle::Mle>().IsRxOnWhenIdle())
    {
        Get<DataPollSender>().SendFastPolls();
    }

exit:
    if (error != kErrorNone)
    {
        SetMulticastAddressMlrState(kMlrStateRegistering, kMlrStateToRegister);

        if (error == kErrorNoBufs)
        {
            ScheduleSend(1);
        }
    }

    LogMulticastAddresses();
    CheckInvariants();
}

#if (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE) && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
Error MlrManager::RegisterMulticastListeners(const otIp6Address *                    aAddresses,
                                             uint8_t                                 aAddressNum,
                                             const uint32_t *                        aTimeout,
                                             otIp6RegisterMulticastListenersCallback aCallback,
                                             void *                                  aContext)
{
    Error error;

    VerifyOrExit(aAddresses != nullptr, error = kErrorInvalidArgs);
    VerifyOrExit(aAddressNum > 0 && aAddressNum <= kIp6AddressesNumMax, error = kErrorInvalidArgs);
    VerifyOrExit(aContext == nullptr || aCallback != nullptr, error = kErrorInvalidArgs);
#if !OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    VerifyOrExit(Get<MeshCoP::Commissioner>().IsActive(), error = kErrorInvalidState);
#else
    if (!Get<MeshCoP::Commissioner>().IsActive())
    {
        otLogWarnMlr("MLR.req without active commissioner session for test.");
    }
#endif

    // Only allow one outstanding registration if callback is specified.
    VerifyOrExit(!mRegisterMulticastListenersPending, error = kErrorBusy);

    SuccessOrExit(error = SendMulticastListenerRegistrationMessage(
                      aAddresses, aAddressNum, aTimeout, &MlrManager::HandleRegisterMulticastListenersResponse, this));

    mRegisterMulticastListenersPending  = true;
    mRegisterMulticastListenersCallback = aCallback;
    mRegisterMulticastListenersContext  = aContext;

exit:
    return error;
}

void MlrManager::HandleRegisterMulticastListenersResponse(void *               aContext,
                                                          otMessage *          aMessage,
                                                          const otMessageInfo *aMessageInfo,
                                                          Error                aResult)
{
    static_cast<MlrManager *>(aContext)->HandleRegisterMulticastListenersResponse(
        static_cast<Coap::Message *>(aMessage), static_cast<const Ip6::MessageInfo *>(aMessageInfo), aResult);
}

void MlrManager::HandleRegisterMulticastListenersResponse(otMessage *          aMessage,
                                                          const otMessageInfo *aMessageInfo,
                                                          Error                aResult)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    uint8_t                                 status;
    Error                                   error;
    Ip6::Address                            failedAddresses[kIp6AddressesNumMax];
    uint8_t                                 failedAddressNum = 0;
    otIp6RegisterMulticastListenersCallback callback         = mRegisterMulticastListenersCallback;
    void *                                  context          = mRegisterMulticastListenersContext;

    mRegisterMulticastListenersPending  = false;
    mRegisterMulticastListenersCallback = nullptr;
    mRegisterMulticastListenersContext  = nullptr;

    error = ParseMulticastListenerRegistrationResponse(aResult, static_cast<Coap::Message *>(aMessage), status,
                                                       failedAddresses, failedAddressNum);

    if (callback != nullptr)
    {
        callback(context, error, status, failedAddresses, failedAddressNum);
    }
}

#endif // (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE) && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE

Error MlrManager::SendMulticastListenerRegistrationMessage(const otIp6Address *  aAddresses,
                                                           uint8_t               aAddressNum,
                                                           const uint32_t *      aTimeout,
                                                           Coap::ResponseHandler aResponseHandler,
                                                           void *                aResponseContext)
{
    OT_UNUSED_VARIABLE(aTimeout);

    Error            error   = kErrorNone;
    Mle::MleRouter & mle     = Get<Mle::MleRouter>();
    Coap::Message *  message = nullptr;
    Ip6::MessageInfo messageInfo;
    Ip6AddressesTlv  addressesTlv;

    VerifyOrExit(Get<BackboneRouter::Leader>().HasPrimary(), error = kErrorInvalidState);

    VerifyOrExit((message = Get<Tmf::Agent>().NewMessage()) != nullptr, error = kErrorNoBufs);

    message->InitAsConfirmablePost();
    SuccessOrExit(message->GenerateRandomToken(Coap::Message::kDefaultTokenLength));
    SuccessOrExit(message->AppendUriPathOptions(UriPath::kMlr));
    SuccessOrExit(message->SetPayloadMarker());

    addressesTlv.Init();
    addressesTlv.SetLength(sizeof(Ip6::Address) * aAddressNum);
    SuccessOrExit(error = message->Append(addressesTlv));
    SuccessOrExit(error = message->AppendBytes(aAddresses, sizeof(Ip6::Address) * aAddressNum));

#if (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE) && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
    if (Get<MeshCoP::Commissioner>().IsActive())
    {
        SuccessOrExit(
            error = Tlv::Append<ThreadCommissionerSessionIdTlv>(*message, Get<MeshCoP::Commissioner>().GetSessionId()));
    }

    if (aTimeout != nullptr)
    {
        SuccessOrExit(error = Tlv::Append<ThreadTimeoutTlv>(*message, *aTimeout));
    }
#else
    OT_ASSERT(aTimeout == nullptr);
#endif

    if (!mle.IsFullThreadDevice() && mle.GetParent().IsThreadVersion1p1())
    {
        uint8_t pbbrServiceId;

        SuccessOrExit(error = Get<BackboneRouter::Leader>().GetServiceId(pbbrServiceId));
        SuccessOrExit(error = mle.GetServiceAloc(pbbrServiceId, messageInfo.GetPeerAddr()));
    }
    else
    {
        messageInfo.GetPeerAddr().SetToRoutingLocator(mle.GetMeshLocalPrefix(),
                                                      Get<BackboneRouter::Leader>().GetServer16());
    }

    messageInfo.SetPeerPort(Tmf::kUdpPort);
    messageInfo.SetSockAddr(mle.GetMeshLocal16());

    error = Get<Tmf::Agent>().SendMessage(*message, messageInfo, aResponseHandler, aResponseContext);

    otLogInfoMlr("Sent MLR.req: addressNum=%d", aAddressNum);

exit:
    otLogInfoMlr("SendMulticastListenerRegistrationMessage(): %s", ErrorToString(error));
    FreeMessageOnError(message, error);
    return error;
}

void MlrManager::HandleMulticastListenerRegistrationResponse(void *               aContext,
                                                             otMessage *          aMessage,
                                                             const otMessageInfo *aMessageInfo,
                                                             Error                aResult)
{
    static_cast<MlrManager *>(aContext)->HandleMulticastListenerRegistrationResponse(
        static_cast<Coap::Message *>(aMessage), static_cast<const Ip6::MessageInfo *>(aMessageInfo), aResult);
}

void MlrManager::HandleMulticastListenerRegistrationResponse(Coap::Message *         aMessage,
                                                             const Ip6::MessageInfo *aMessageInfo,
                                                             Error                   aResult)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    uint8_t      status;
    Error        error;
    Ip6::Address failedAddresses[kIp6AddressesNumMax];
    uint8_t      failedAddressNum = 0;

    error = ParseMulticastListenerRegistrationResponse(aResult, aMessage, status, failedAddresses, failedAddressNum);

    FinishMulticastListenerRegistration(error == kErrorNone && status == ThreadStatusTlv::MlrStatus::kMlrSuccess,
                                        failedAddresses, failedAddressNum);

    if (error == kErrorNone && status == ThreadStatusTlv::MlrStatus::kMlrSuccess)
    {
        // keep sending until all multicast addresses are registered.
        ScheduleSend(0);
    }
    else
    {
        otBackboneRouterConfig config;
        uint16_t               reregDelay;

        // The Device has just attempted a Multicast Listener Registration which failed, and it retries the same
        // registration with a random time delay chosen in the interval [0, Reregistration Delay].
        // This is required by Thread 1.2 Specification 5.24.2.3
        if (Get<BackboneRouter::Leader>().GetConfig(config) == kErrorNone)
        {
            reregDelay = config.mReregistrationDelay > 1
                             ? Random::NonCrypto::GetUint16InRange(1, config.mReregistrationDelay)
                             : 1;
            ScheduleSend(reregDelay);
        }
    }
}

Error MlrManager::ParseMulticastListenerRegistrationResponse(Error          aResult,
                                                             Coap::Message *aMessage,
                                                             uint8_t &      aStatus,
                                                             Ip6::Address * aFailedAddresses,
                                                             uint8_t &      aFailedAddressNum)
{
    Error    error;
    uint16_t addressesOffset, addressesLength;

    aStatus = ThreadStatusTlv::MlrStatus::kMlrGeneralFailure;

    VerifyOrExit(aResult == kErrorNone && aMessage != nullptr, error = kErrorParse);
    VerifyOrExit(aMessage->GetCode() == Coap::kCodeChanged, error = kErrorParse);

    SuccessOrExit(error = Tlv::Find<ThreadStatusTlv>(*aMessage, aStatus));

    if (ThreadTlv::FindTlvValueOffset(*aMessage, Ip6AddressesTlv::kIp6Addresses, addressesOffset, addressesLength) ==
        kErrorNone)
    {
        VerifyOrExit(addressesLength % sizeof(Ip6::Address) == 0, error = kErrorParse);
        VerifyOrExit(addressesLength / sizeof(Ip6::Address) <= kIp6AddressesNumMax, error = kErrorParse);

        for (uint16_t offset = 0; offset < addressesLength; offset += sizeof(Ip6::Address))
        {
            IgnoreError(aMessage->Read(addressesOffset + offset, aFailedAddresses[aFailedAddressNum]));
            aFailedAddressNum++;
        }
    }

    VerifyOrExit(aFailedAddressNum == 0 || aStatus != ThreadStatusTlv::MlrStatus::kMlrSuccess, error = kErrorParse);

exit:
    LogMlrResponse(aResult, error, aStatus, aFailedAddresses, aFailedAddressNum);
    return aResult != kErrorNone ? aResult : error;
}

void MlrManager::SetMulticastAddressMlrState(MlrState aFromState, MlrState aToState)
{
#if OPENTHREAD_CONFIG_MLR_ENABLE
    for (Ip6::Netif::ExternalMulticastAddress &addr :
         Get<ThreadNetif>().IterateExternalMulticastAddresses(Ip6::Address::kTypeMulticastLargerThanRealmLocal))
    {
        if (addr.GetMlrState() == aFromState)
        {
            addr.SetMlrState(aToState);
        }
    }
#endif
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE
    for (Child &child : Get<ChildTable>().Iterate(Child::kInStateValid))
    {
        for (const Ip6::Address &address : child.IterateIp6Addresses(Ip6::Address::kTypeMulticastLargerThanRealmLocal))
        {
            if (child.GetAddressMlrState(address) == aFromState)
            {
                child.SetAddressMlrState(address, aToState);
            }
        }
    }
#endif
}

void MlrManager::FinishMulticastListenerRegistration(bool                aSuccess,
                                                     const Ip6::Address *aFailedAddresses,
                                                     uint8_t             aFailedAddressNum)
{
    OT_ASSERT(mMlrPending);

    mMlrPending = false;

#if OPENTHREAD_CONFIG_MLR_ENABLE
    for (Ip6::Netif::ExternalMulticastAddress &addr :
         Get<ThreadNetif>().IterateExternalMulticastAddresses(Ip6::Address::kTypeMulticastLargerThanRealmLocal))
    {
        if (addr.GetMlrState() == kMlrStateRegistering)
        {
            bool success = aSuccess || !AddressListContains(aFailedAddresses, aFailedAddressNum, addr.GetAddress());

            addr.SetMlrState(success ? kMlrStateRegistered : kMlrStateToRegister);
        }
    }
#endif
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE
    for (Child &child : Get<ChildTable>().Iterate(Child::kInStateValid))
    {
        for (const Ip6::Address &address : child.IterateIp6Addresses(Ip6::Address::kTypeMulticastLargerThanRealmLocal))
        {
            if (child.GetAddressMlrState(address) == kMlrStateRegistering)
            {
                bool success = aSuccess || !AddressListContains(aFailedAddresses, aFailedAddressNum, address);

                child.SetAddressMlrState(address, success ? kMlrStateRegistered : kMlrStateToRegister);
            }
        }
    }
#endif

    LogMulticastAddresses();
    CheckInvariants();
}

void MlrManager::HandleTimeTick(void)
{
    if (mSendDelay > 0 && --mSendDelay == 0)
    {
        SendMulticastListenerRegistration();
    }

    if (mReregistrationDelay > 0 && --mReregistrationDelay == 0)
    {
        Reregister();
    }

    UpdateTimeTickerRegistration();
}

void MlrManager::Reregister(void)
{
    otLogInfoMlr("MLR Reregister!");

    SetMulticastAddressMlrState(kMlrStateRegistered, kMlrStateToRegister);
    CheckInvariants();

    ScheduleSend(0);

    // Schedule for the next renewing.
    UpdateReregistrationDelay(false);
}

void MlrManager::UpdateReregistrationDelay(bool aRereg)
{
    Mle::MleRouter &mle = Get<Mle::MleRouter>();

    bool needSendMlr = (mle.IsFullThreadDevice() || mle.GetParent().IsThreadVersion1p1()) &&
                       Get<BackboneRouter::Leader>().HasPrimary();

    if (!needSendMlr)
    {
        mReregistrationDelay = 0;
    }
    else
    {
        BackboneRouter::BackboneRouterConfig config;
        uint32_t                             reregDelay;
        uint32_t                             effectiveMlrTimeout;

        IgnoreError(Get<BackboneRouter::Leader>().GetConfig(config));

        if (aRereg)
        {
            reregDelay = config.mReregistrationDelay > 1
                             ? Random::NonCrypto::GetUint16InRange(1, config.mReregistrationDelay)
                             : 1;
        }
        else
        {
            // Calculate renewing period according to Thread Spec. 5.24.2.3.2
            // The random time t SHOULD be chosen such that (0.5* MLR-Timeout) < t < (MLR-Timeout – 9 seconds).
            effectiveMlrTimeout = config.mMlrTimeout > Mle::kMlrTimeoutMin ? config.mMlrTimeout
                                                                           : static_cast<uint32_t>(Mle::kMlrTimeoutMin);
            reregDelay = Random::NonCrypto::GetUint32InRange((effectiveMlrTimeout >> 1u) + 1, effectiveMlrTimeout - 9);
        }

        if (mReregistrationDelay == 0 || mReregistrationDelay > reregDelay)
        {
            mReregistrationDelay = reregDelay;
        }
    }

    UpdateTimeTickerRegistration();

    otLogDebgMlr("MlrManager::UpdateReregistrationDelay: rereg=%d, needSendMlr=%d, ReregDelay=%lu", aRereg, needSendMlr,
                 mReregistrationDelay);
}

void MlrManager::LogMulticastAddresses(void)
{
#if OPENTHREAD_CONFIG_LOG_MLR && OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_DEBG
    otLogDebgMlr("-------- Multicast Addresses --------");

#if OPENTHREAD_CONFIG_MLR_ENABLE
    for (const Ip6::Netif::ExternalMulticastAddress &addr : Get<ThreadNetif>().IterateExternalMulticastAddresses())
    {
        otLogDebgMlr("%-32s%c", addr.GetAddress().ToString().AsCString(), "-rR"[addr.GetMlrState()]);
    }
#endif

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE
    for (Child &child : Get<ChildTable>().Iterate(Child::kInStateValid))
    {
        for (const Ip6::Address &address : child.IterateIp6Addresses(Ip6::Address::kTypeMulticastLargerThanRealmLocal))
        {
            otLogDebgMlr("%-32s%c %04x", address.ToString().AsCString(), "-rR"[child.GetAddressMlrState(address)],
                         child.GetRloc16());
        }
    }
#endif

#endif // OPENTHREAD_CONFIG_LOG_MLR && OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_DEBG
}

void MlrManager::AppendToUniqueAddressList(Ip6::Address (&aAddresses)[kIp6AddressesNumMax],
                                           uint8_t &           aAddressNum,
                                           const Ip6::Address &aAddress)
{
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE
    for (uint8_t i = 0; i < aAddressNum; i++)
    {
        if (aAddresses[i] == aAddress)
        {
            ExitNow();
        }
    }
#endif

    aAddresses[aAddressNum++] = aAddress;

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE
exit:
#endif
    return;
}

bool MlrManager::AddressListContains(const Ip6::Address *aAddressList,
                                     uint8_t             aAddressListSize,
                                     const Ip6::Address &aAddress)
{
    bool contains = false;

    // An empty address list is treated as if it contains all failed addresses.
    VerifyOrExit(aAddressListSize > 0, contains = true);

    for (uint8_t i = 0; i < aAddressListSize; i++)
    {
        if (aAddressList[i] == aAddress)
        {
            ExitNow(contains = true);
        }
    }

exit:
    return contains;
}

void MlrManager::LogMlrResponse(Error               aResult,
                                Error               aError,
                                uint8_t             aStatus,
                                const Ip6::Address *aFailedAddresses,
                                uint8_t             aFailedAddressNum)
{
    OT_UNUSED_VARIABLE(aResult);
    OT_UNUSED_VARIABLE(aError);
    OT_UNUSED_VARIABLE(aStatus);
    OT_UNUSED_VARIABLE(aFailedAddresses);
    OT_UNUSED_VARIABLE(aFailedAddressNum);

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_WARN) && (OPENTHREAD_CONFIG_LOG_MLR == 1)
    if (aResult == kErrorNone && aError == kErrorNone && aStatus == ThreadStatusTlv::MlrStatus::kMlrSuccess)
    {
        otLogInfoMlr("Receive MLR.rsp OK");
    }
    else
    {
        otLogWarnMlr("Receive MLR.rsp: result=%s, error=%s, status=%d, failedAddressNum=%d", ErrorToString(aResult),
                     ErrorToString(aError), aStatus, aFailedAddressNum);

        for (uint8_t i = 0; i < aFailedAddressNum; i++)
        {
            otLogWarnMlr("MA failed: %s", aFailedAddresses[i].ToString().AsCString());
        }
    }
#endif
}

void MlrManager::CheckInvariants(void) const
{
#if OPENTHREAD_EXAMPLES_SIMULATION
    uint16_t registeringNum = 0;

    OT_ASSERT(!mMlrPending || mSendDelay == 0);

#if OPENTHREAD_CONFIG_MLR_ENABLE
    for (Ip6::Netif::ExternalMulticastAddress &addr :
         Get<ThreadNetif>().IterateExternalMulticastAddresses(Ip6::Address::kTypeMulticastLargerThanRealmLocal))
    {
        registeringNum += (addr.GetMlrState() == kMlrStateRegistering);
    }
#endif
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE
    for (Child &child : Get<ChildTable>().Iterate(Child::kInStateValid))
    {
        for (const Ip6::Address &address : child.IterateIp6Addresses(Ip6::Address::kTypeMulticastLargerThanRealmLocal))
        {
            registeringNum += (child.GetAddressMlrState(address) == kMlrStateRegistering);
        }
    }
#endif

    OT_ASSERT(registeringNum == 0 || mMlrPending);
#endif // OPENTHREAD_EXAMPLES_SIMULATION
}

} // namespace ot

#endif // OPENTHREAD_CONFIG_MLR_ENABLE
