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
 *   This file implements the Joiner role.
 */

#include "joiner.hpp"

#if OPENTHREAD_CONFIG_JOINER_ENABLE

#include <stdio.h>

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/encoding.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/string.hpp"
#include "meshcop/meshcop.hpp"
#include "radio/radio.hpp"
#include "thread/thread_netif.hpp"
#include "thread/uri_paths.hpp"
#include "utils/otns.hpp"

namespace ot {
namespace MeshCoP {

Joiner::Joiner(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mId()
    , mDiscerner()
    , mState(kStateIdle)
    , mCallback(nullptr)
    , mContext(nullptr)
    , mJoinerRouterIndex(0)
    , mFinalizeMessage(nullptr)
    , mTimer(aInstance, Joiner::HandleTimer)
    , mJoinerEntrust(UriPath::kJoinerEntrust, &Joiner::HandleJoinerEntrust, this)
{
    SetIdFromIeeeEui64();
    mDiscerner.Clear();
    memset(mJoinerRouters, 0, sizeof(mJoinerRouters));
    Get<Tmf::Agent>().AddResource(mJoinerEntrust);
}

void Joiner::SetIdFromIeeeEui64(void)
{
    Mac::ExtAddress eui64;

    Get<Radio>().GetIeeeEui64(eui64);
    ComputeJoinerId(eui64, mId);
}

const JoinerDiscerner *Joiner::GetDiscerner(void) const
{
    return mDiscerner.IsEmpty() ? nullptr : &mDiscerner;
}

Error Joiner::SetDiscerner(const JoinerDiscerner &aDiscerner)
{
    Error error = kErrorNone;

    VerifyOrExit(aDiscerner.IsValid(), error = kErrorInvalidArgs);
    VerifyOrExit(mState == kStateIdle, error = kErrorInvalidState);

    mDiscerner = aDiscerner;
    mDiscerner.GenerateJoinerId(mId);

exit:
    return error;
}

Error Joiner::ClearDiscerner(void)
{
    Error error = kErrorNone;

    VerifyOrExit(mState == kStateIdle, error = kErrorInvalidState);
    VerifyOrExit(!mDiscerner.IsEmpty());

    mDiscerner.Clear();
    SetIdFromIeeeEui64();

exit:
    return error;
}

void Joiner::SetState(State aState)
{
    State oldState = mState;
    OT_UNUSED_VARIABLE(oldState);

    SuccessOrExit(Get<Notifier>().Update(mState, aState, kEventJoinerStateChanged));

    otLogInfoMeshCoP("JoinerState: %s -> %s", StateToString(oldState), StateToString(aState));
exit:
    return;
}

Error Joiner::Start(const char *     aPskd,
                    const char *     aProvisioningUrl,
                    const char *     aVendorName,
                    const char *     aVendorModel,
                    const char *     aVendorSwVersion,
                    const char *     aVendorData,
                    otJoinerCallback aCallback,
                    void *           aContext)
{
    Error                        error;
    JoinerPskd                   joinerPskd;
    Mac::ExtAddress              randomAddress;
    SteeringData::HashBitIndexes filterIndexes;

    otLogInfoMeshCoP("Joiner starting");

    VerifyOrExit(aProvisioningUrl == nullptr || IsValidUtf8String(aProvisioningUrl), error = kErrorInvalidArgs);
    VerifyOrExit(aVendorName == nullptr || IsValidUtf8String(aVendorName), error = kErrorInvalidArgs);
    VerifyOrExit(aVendorSwVersion == nullptr || IsValidUtf8String(aVendorSwVersion), error = kErrorInvalidArgs);

    VerifyOrExit(mState == kStateIdle, error = kErrorBusy);
    VerifyOrExit(Get<ThreadNetif>().IsUp() && Get<Mle::Mle>().GetRole() == Mle::kRoleDisabled,
                 error = kErrorInvalidState);

    SuccessOrExit(error = joinerPskd.SetFrom(aPskd));

    // Use random-generated extended address.
    randomAddress.GenerateRandom();
    Get<Mac::Mac>().SetExtAddress(randomAddress);
    Get<Mle::MleRouter>().UpdateLinkLocalAddress();

    SuccessOrExit(error = Get<Coap::CoapSecure>().Start(kJoinerUdpPort));
    Get<Coap::CoapSecure>().SetPsk(joinerPskd);

    for (JoinerRouter &router : mJoinerRouters)
    {
        router.mPriority = 0; // Priority zero means entry is not in-use.
    }

    SuccessOrExit(error = PrepareJoinerFinalizeMessage(aProvisioningUrl, aVendorName, aVendorModel, aVendorSwVersion,
                                                       aVendorData));

    if (!mDiscerner.IsEmpty())
    {
        SteeringData::CalculateHashBitIndexes(mDiscerner, filterIndexes);
    }
    else
    {
        SteeringData::CalculateHashBitIndexes(mId, filterIndexes);
    }

    SuccessOrExit(error = Get<Mle::DiscoverScanner>().Discover(Mac::ChannelMask(0), Get<Mac::Mac>().GetPanId(),
                                                               /* aJoiner */ true, /* aEnableFiltering */ true,
                                                               &filterIndexes, HandleDiscoverResult, this));
    mCallback = aCallback;
    mContext  = aContext;

    SetState(kStateDiscover);

exit:
    if (error != kErrorNone)
    {
        FreeJoinerFinalizeMessage();
    }

    LogError("start joiner", error);
    return error;
}

void Joiner::Stop(void)
{
    otLogInfoMeshCoP("Joiner stopped");

    // Callback is set to `nullptr` to skip calling it from `Finish()`
    mCallback = nullptr;
    Finish(kErrorAbort);
}

void Joiner::Finish(Error aError)
{
    switch (mState)
    {
    case kStateIdle:
        ExitNow();

    case kStateConnect:
    case kStateConnected:
    case kStateEntrust:
    case kStateJoined:
        Get<Coap::CoapSecure>().Disconnect();
        IgnoreError(Get<Ip6::Filter>().RemoveUnsecurePort(kJoinerUdpPort));
        mTimer.Stop();

        OT_FALL_THROUGH;

    case kStateDiscover:
        Get<Coap::CoapSecure>().Stop();
        break;
    }

    SetState(kStateIdle);
    FreeJoinerFinalizeMessage();

    if (mCallback)
    {
        mCallback(aError, mContext);
    }

exit:
    return;
}

uint8_t Joiner::CalculatePriority(int8_t aRssi, bool aSteeringDataAllowsAny)
{
    int16_t priority;

    if (aRssi == OT_RADIO_RSSI_INVALID)
    {
        aRssi = -127;
    }

    // Limit the RSSI to range (-128, 0), i.e. -128 < aRssi < 0.

    if (aRssi <= -128)
    {
        priority = -127;
    }
    else if (aRssi >= 0)
    {
        priority = -1;
    }
    else
    {
        priority = aRssi;
    }

    // Assign higher priority to networks with an exact match of Joiner
    // ID in the Steering Data (128 < priority < 256) compared to ones
    // that allow all Joiners (0 < priority < 128). Sub-prioritize
    // based on signal strength. Priority 0 is reserved for unused
    // entry.

    priority += aSteeringDataAllowsAny ? 128 : 256;

    return static_cast<uint8_t>(priority);
}

void Joiner::HandleDiscoverResult(Mle::DiscoverScanner::ScanResult *aResult, void *aContext)
{
    static_cast<Joiner *>(aContext)->HandleDiscoverResult(aResult);
}

void Joiner::HandleDiscoverResult(Mle::DiscoverScanner::ScanResult *aResult)
{
    VerifyOrExit(mState == kStateDiscover);

    if (aResult != nullptr)
    {
        SaveDiscoveredJoinerRouter(*aResult);
    }
    else
    {
        Get<Mac::Mac>().SetExtAddress(mId);
        Get<Mle::MleRouter>().UpdateLinkLocalAddress();

        mJoinerRouterIndex = 0;
        TryNextJoinerRouter(kErrorNone);
    }

exit:
    return;
}

void Joiner::SaveDiscoveredJoinerRouter(const Mle::DiscoverScanner::ScanResult &aResult)
{
    uint8_t       priority;
    bool          doesAllowAny;
    JoinerRouter *end = OT_ARRAY_END(mJoinerRouters);
    JoinerRouter *entry;

    doesAllowAny = static_cast<const SteeringData &>(aResult.mSteeringData).PermitsAllJoiners();

    otLogInfoMeshCoP("Joiner discover network: %s, pan:0x%04x, port:%d, chan:%d, rssi:%d, allow-any:%s",
                     static_cast<const Mac::ExtAddress &>(aResult.mExtAddress).ToString().AsCString(), aResult.mPanId,
                     aResult.mJoinerUdpPort, aResult.mChannel, aResult.mRssi, doesAllowAny ? "yes" : "no");

    priority = CalculatePriority(aResult.mRssi, doesAllowAny);

    // We keep the list sorted based on priority. Find the place to
    // add the new result.

    for (entry = &mJoinerRouters[0]; entry < end; entry++)
    {
        if (priority > entry->mPriority)
        {
            break;
        }
    }

    VerifyOrExit(entry < end);

    // Shift elements in array to make room for the new one.
    memmove(entry + 1, entry,
            static_cast<size_t>(reinterpret_cast<uint8_t *>(end - 1) - reinterpret_cast<uint8_t *>(entry)));

    entry->mExtAddr       = static_cast<const Mac::ExtAddress &>(aResult.mExtAddress);
    entry->mPanId         = aResult.mPanId;
    entry->mJoinerUdpPort = aResult.mJoinerUdpPort;
    entry->mChannel       = aResult.mChannel;
    entry->mPriority      = priority;

exit:
    return;
}

void Joiner::TryNextJoinerRouter(Error aPrevError)
{
    for (; mJoinerRouterIndex < OT_ARRAY_LENGTH(mJoinerRouters); mJoinerRouterIndex++)
    {
        JoinerRouter &router = mJoinerRouters[mJoinerRouterIndex];
        Error         error;

        if (router.mPriority == 0)
        {
            break;
        }

        error = Connect(router);
        VerifyOrExit(error != kErrorNone, mJoinerRouterIndex++);

        // Save the error from `Connect` only if there is no previous
        // error from earlier attempts. This ensures that if there has
        // been a previous Joiner Router connect attempt where
        // `Connect()` call itself was successful, the error status
        // emitted from `Finish()` call corresponds to the error from
        // that attempt.

        if (aPrevError == kErrorNone)
        {
            aPrevError = error;
        }
    }

    if (aPrevError == kErrorNone)
    {
        aPrevError = kErrorNotFound;
    }

    Finish(aPrevError);

exit:
    return;
}

Error Joiner::Connect(JoinerRouter &aRouter)
{
    Error         error = kErrorNotFound;
    Ip6::SockAddr sockAddr(aRouter.mJoinerUdpPort);

    otLogInfoMeshCoP("Joiner connecting to %s, pan:0x%04x, chan:%d", aRouter.mExtAddr.ToString().AsCString(),
                     aRouter.mPanId, aRouter.mChannel);

    Get<Mac::Mac>().SetPanId(aRouter.mPanId);
    SuccessOrExit(error = Get<Mac::Mac>().SetPanChannel(aRouter.mChannel));
    SuccessOrExit(error = Get<Ip6::Filter>().AddUnsecurePort(kJoinerUdpPort));

    sockAddr.GetAddress().SetToLinkLocalAddress(aRouter.mExtAddr);

    SuccessOrExit(error = Get<Coap::CoapSecure>().Connect(sockAddr, Joiner::HandleSecureCoapClientConnect, this));

    SetState(kStateConnect);

exit:
    LogError("start secure joiner connection", error);
    return error;
}

void Joiner::HandleSecureCoapClientConnect(bool aConnected, void *aContext)
{
    static_cast<Joiner *>(aContext)->HandleSecureCoapClientConnect(aConnected);
}

void Joiner::HandleSecureCoapClientConnect(bool aConnected)
{
    VerifyOrExit(mState == kStateConnect);

    if (aConnected)
    {
        SetState(kStateConnected);
        SendJoinerFinalize();
        mTimer.Start(kReponseTimeout);
    }
    else
    {
        TryNextJoinerRouter(kErrorSecurity);
    }

exit:
    return;
}

Error Joiner::PrepareJoinerFinalizeMessage(const char *aProvisioningUrl,
                                           const char *aVendorName,
                                           const char *aVendorModel,
                                           const char *aVendorSwVersion,
                                           const char *aVendorData)
{
    Error                 error = kErrorNone;
    VendorNameTlv         vendorNameTlv;
    VendorModelTlv        vendorModelTlv;
    VendorSwVersionTlv    vendorSwVersionTlv;
    VendorStackVersionTlv vendorStackVersionTlv;
    ProvisioningUrlTlv    provisioningUrlTlv;

    VerifyOrExit((mFinalizeMessage = NewMeshCoPMessage(Get<Coap::CoapSecure>())) != nullptr, error = kErrorNoBufs);

    mFinalizeMessage->InitAsConfirmablePost();
    SuccessOrExit(error = mFinalizeMessage->AppendUriPathOptions(UriPath::kJoinerFinalize));
    SuccessOrExit(error = mFinalizeMessage->SetPayloadMarker());
    mFinalizeMessage->SetOffset(mFinalizeMessage->GetLength());

    SuccessOrExit(error = Tlv::Append<StateTlv>(*mFinalizeMessage, StateTlv::kAccept));

    vendorNameTlv.Init();
    vendorNameTlv.SetVendorName(aVendorName);
    SuccessOrExit(error = vendorNameTlv.AppendTo(*mFinalizeMessage));

    vendorModelTlv.Init();
    vendorModelTlv.SetVendorModel(aVendorModel);
    SuccessOrExit(error = vendorModelTlv.AppendTo(*mFinalizeMessage));

    vendorSwVersionTlv.Init();
    vendorSwVersionTlv.SetVendorSwVersion(aVendorSwVersion);
    SuccessOrExit(error = vendorSwVersionTlv.AppendTo(*mFinalizeMessage));

    vendorStackVersionTlv.Init();
    vendorStackVersionTlv.SetOui(OPENTHREAD_CONFIG_STACK_VENDOR_OUI);
    vendorStackVersionTlv.SetMajor(OPENTHREAD_CONFIG_STACK_VERSION_MAJOR);
    vendorStackVersionTlv.SetMinor(OPENTHREAD_CONFIG_STACK_VERSION_MINOR);
    vendorStackVersionTlv.SetRevision(OPENTHREAD_CONFIG_STACK_VERSION_REV);
    SuccessOrExit(error = vendorStackVersionTlv.AppendTo(*mFinalizeMessage));

    if (aVendorData != nullptr)
    {
        VendorDataTlv vendorDataTlv;
        vendorDataTlv.Init();
        vendorDataTlv.SetVendorData(aVendorData);
        SuccessOrExit(error = vendorDataTlv.AppendTo(*mFinalizeMessage));
    }

    provisioningUrlTlv.Init();
    provisioningUrlTlv.SetProvisioningUrl(aProvisioningUrl);

    if (provisioningUrlTlv.GetLength() > 0)
    {
        SuccessOrExit(error = provisioningUrlTlv.AppendTo(*mFinalizeMessage));
    }

exit:
    if (error != kErrorNone)
    {
        FreeJoinerFinalizeMessage();
    }

    return error;
}

void Joiner::FreeJoinerFinalizeMessage(void)
{
    VerifyOrExit(mState == kStateIdle && mFinalizeMessage != nullptr);

    mFinalizeMessage->Free();
    mFinalizeMessage = nullptr;

exit:
    return;
}

void Joiner::SendJoinerFinalize(void)
{
    OT_ASSERT(mFinalizeMessage != nullptr);

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    LogCertMessage("[THCI] direction=send | type=JOIN_FIN.req |", *mFinalizeMessage);
#endif

    SuccessOrExit(Get<Coap::CoapSecure>().SendMessage(*mFinalizeMessage, Joiner::HandleJoinerFinalizeResponse, this));
    mFinalizeMessage = nullptr;

    otLogInfoMeshCoP("Joiner sent finalize");

exit:
    return;
}

void Joiner::HandleJoinerFinalizeResponse(void *               aContext,
                                          otMessage *          aMessage,
                                          const otMessageInfo *aMessageInfo,
                                          Error                aResult)
{
    static_cast<Joiner *>(aContext)->HandleJoinerFinalizeResponse(
        static_cast<Coap::Message *>(aMessage), static_cast<const Ip6::MessageInfo *>(aMessageInfo), aResult);
}

void Joiner::HandleJoinerFinalizeResponse(Coap::Message *aMessage, const Ip6::MessageInfo *aMessageInfo, Error aResult)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    uint8_t state;

    VerifyOrExit(mState == kStateConnected && aResult == kErrorNone);
    OT_ASSERT(aMessage != nullptr);

    VerifyOrExit(aMessage->IsAck() && aMessage->GetCode() == Coap::kCodeChanged);

    SuccessOrExit(Tlv::Find<StateTlv>(*aMessage, state));

    SetState(kStateEntrust);
    mTimer.Start(kReponseTimeout);

    otLogInfoMeshCoP("Joiner received finalize response %d", state);

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    LogCertMessage("[THCI] direction=recv | type=JOIN_FIN.rsp |", *aMessage);
#endif

exit:
    Get<Coap::CoapSecure>().Disconnect();
    IgnoreError(Get<Ip6::Filter>().RemoveUnsecurePort(kJoinerUdpPort));
}

void Joiner::HandleJoinerEntrust(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<Joiner *>(aContext)->HandleJoinerEntrust(*static_cast<Coap::Message *>(aMessage),
                                                         *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

void Joiner::HandleJoinerEntrust(Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    Error         error;
    Dataset::Info datasetInfo;

    VerifyOrExit(mState == kStateEntrust && aMessage.IsConfirmablePostRequest(), error = kErrorDrop);

    otLogInfoMeshCoP("Joiner received entrust");
    otLogCertMeshCoP("[THCI] direction=recv | type=JOIN_ENT.ntf");

    datasetInfo.Clear();

    SuccessOrExit(error = Tlv::Find<NetworkKeyTlv>(aMessage, datasetInfo.UpdateNetworkKey()));

    datasetInfo.SetChannel(Get<Mac::Mac>().GetPanChannel());
    datasetInfo.SetPanId(Get<Mac::Mac>().GetPanId());

    IgnoreError(Get<MeshCoP::ActiveDataset>().Save(datasetInfo));

    otLogInfoMeshCoP("Joiner successful!");

    SendJoinerEntrustResponse(aMessage, aMessageInfo);

    // Delay extended address configuration to allow DTLS wrap up.
    mTimer.Start(kConfigExtAddressDelay);

exit:
    LogError("process joiner entrust", error);
}

void Joiner::SendJoinerEntrustResponse(const Coap::Message &aRequest, const Ip6::MessageInfo &aRequestInfo)
{
    Error            error = kErrorNone;
    Coap::Message *  message;
    Ip6::MessageInfo responseInfo(aRequestInfo);

    VerifyOrExit((message = NewMeshCoPMessage(Get<Tmf::Agent>())) != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = message->SetDefaultResponseHeader(aRequest));
    message->SetSubType(Message::kSubTypeJoinerEntrust);

    responseInfo.GetSockAddr().Clear();
    SuccessOrExit(error = Get<Tmf::Agent>().SendMessage(*message, responseInfo));

    SetState(kStateJoined);

    otLogInfoMeshCoP("Joiner sent entrust response");
    otLogCertMeshCoP("[THCI] direction=send | type=JOIN_ENT.rsp");

exit:
    FreeMessageOnError(message, error);
}

void Joiner::HandleTimer(Timer &aTimer)
{
    aTimer.Get<Joiner>().HandleTimer();
}

void Joiner::HandleTimer(void)
{
    Error error = kErrorNone;

    switch (mState)
    {
    case kStateIdle:
    case kStateDiscover:
    case kStateConnect:
        OT_ASSERT(false);
        OT_UNREACHABLE_CODE(break);

    case kStateConnected:
    case kStateEntrust:
        error = kErrorResponseTimeout;
        break;

    case kStateJoined:
        Mac::ExtAddress extAddress;

        extAddress.GenerateRandom();
        Get<Mac::Mac>().SetExtAddress(extAddress);
        Get<Mle::MleRouter>().UpdateLinkLocalAddress();

        error = kErrorNone;
        break;
    }

    Finish(error);
}

// LCOV_EXCL_START

const char *Joiner::StateToString(State aState)
{
    static const char *const kStateStrings[] = {
        "Idle",       // (0) kStateIdle
        "Discover",   // (1) kStateDiscover
        "Connecting", // (2) kStateConnect
        "Connected",  // (3) kStateConnected
        "Entrust",    // (4) kStateEntrust
        "Joined",     // (5) kStateJoined
    };

    static_assert(kStateIdle == 0, "kStateIdle value is incorrect");
    static_assert(kStateDiscover == 1, "kStateDiscover value is incorrect");
    static_assert(kStateConnect == 2, "kStateConnect value is incorrect");
    static_assert(kStateConnected == 3, "kStateConnected value is incorrect");
    static_assert(kStateEntrust == 4, "kStateEntrust value is incorrect");
    static_assert(kStateJoined == 5, "kStateJoined value is incorrect");

    return kStateStrings[aState];
}

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
void Joiner::LogCertMessage(const char *aText, const Coap::Message &aMessage) const
{
    uint8_t buf[OPENTHREAD_CONFIG_MESSAGE_BUFFER_SIZE];

    VerifyOrExit(aMessage.GetLength() <= sizeof(buf));
    aMessage.ReadBytes(aMessage.GetOffset(), buf, aMessage.GetLength() - aMessage.GetOffset());

    otDumpCertMeshCoP(aText, buf, aMessage.GetLength() - aMessage.GetOffset());

exit:
    return;
}
#endif

// LCOV_EXCL_STOP

} // namespace MeshCoP
} // namespace ot

#endif // OPENTHREAD_CONFIG_JOINER_ENABLE
