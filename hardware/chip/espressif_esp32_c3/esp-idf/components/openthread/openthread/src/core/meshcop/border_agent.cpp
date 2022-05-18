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
 *   This file implements the BorderAgent service.
 */

#include "border_agent.hpp"

#if OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE

#include "coap/coap_message.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "meshcop/meshcop.hpp"
#include "meshcop/meshcop_tlvs.hpp"
#include "thread/thread_netif.hpp"
#include "thread/thread_tlvs.hpp"
#include "thread/uri_paths.hpp"

namespace ot {
namespace MeshCoP {

namespace {
constexpr uint16_t kBorderAgentUdpPort = OPENTHREAD_CONFIG_BORDER_AGENT_UDP_PORT; ///< UDP port of border agent service.
}

void BorderAgent::ForwardContext::Init(Instance &           aInstance,
                                       const Coap::Message &aMessage,
                                       bool                 aPetition,
                                       bool                 aSeparate)
{
    InstanceLocatorInit::Init(aInstance);
    mMessageId   = aMessage.GetMessageId();
    mPetition    = aPetition;
    mSeparate    = aSeparate;
    mType        = aMessage.GetType();
    mTokenLength = aMessage.GetTokenLength();
    memcpy(mToken, aMessage.GetToken(), mTokenLength);
}

Error BorderAgent::ForwardContext::ToHeader(Coap::Message &aMessage, uint8_t aCode)
{
    if ((mType == Coap::kTypeNonConfirmable) || mSeparate)
    {
        aMessage.Init(Coap::kTypeNonConfirmable, static_cast<Coap::Code>(aCode));
    }
    else
    {
        aMessage.Init(Coap::kTypeAck, static_cast<Coap::Code>(aCode));
    }

    if (!mSeparate)
    {
        aMessage.SetMessageId(mMessageId);
    }

    return aMessage.SetToken(mToken, mTokenLength);
}

Coap::Message::Code BorderAgent::CoapCodeFromError(Error aError)
{
    Coap::Message::Code code;

    switch (aError)
    {
    case kErrorNone:
        code = Coap::kCodeChanged;
        break;

    case kErrorParse:
        code = Coap::kCodeBadRequest;
        break;

    default:
        code = Coap::kCodeInternalError;
        break;
    }

    return code;
}

void BorderAgent::SendErrorMessage(ForwardContext &aForwardContext, Error aError)
{
    Error             error   = kErrorNone;
    Coap::CoapSecure &coaps   = Get<Coap::CoapSecure>();
    Coap::Message *   message = nullptr;

    VerifyOrExit((message = NewMeshCoPMessage(coaps)) != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = aForwardContext.ToHeader(*message, CoapCodeFromError(aError)));
    SuccessOrExit(error = coaps.SendMessage(*message, coaps.GetMessageInfo()));

exit:
    FreeMessageOnError(message, error);
    LogError("send error CoAP message", error);
}

void BorderAgent::SendErrorMessage(const Coap::Message &aRequest, bool aSeparate, Error aError)
{
    Error             error   = kErrorNone;
    Coap::CoapSecure &coaps   = Get<Coap::CoapSecure>();
    Coap::Message *   message = nullptr;

    VerifyOrExit((message = NewMeshCoPMessage(coaps)) != nullptr, error = kErrorNoBufs);

    if (aRequest.IsNonConfirmable() || aSeparate)
    {
        message->Init(Coap::kTypeNonConfirmable, CoapCodeFromError(aError));
    }
    else
    {
        message->Init(Coap::kTypeAck, CoapCodeFromError(aError));
    }

    if (!aSeparate)
    {
        message->SetMessageId(aRequest.GetMessageId());
    }

    SuccessOrExit(error = message->SetTokenFromMessage(aRequest));

    SuccessOrExit(error = coaps.SendMessage(*message, coaps.GetMessageInfo()));

exit:
    FreeMessageOnError(message, error);
    LogError("send error CoAP message", error);
}

void BorderAgent::HandleCoapResponse(void *               aContext,
                                     otMessage *          aMessage,
                                     const otMessageInfo *aMessageInfo,
                                     Error                aResult)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    ForwardContext &forwardContext = *static_cast<ForwardContext *>(aContext);

    forwardContext.Get<BorderAgent>().HandleCoapResponse(forwardContext, static_cast<const Coap::Message *>(aMessage),
                                                         aResult);
}

void BorderAgent::HandleCoapResponse(ForwardContext &aForwardContext, const Coap::Message *aResponse, Error aResult)
{
    Coap::Message *message = nullptr;
    Error          error;

    SuccessOrExit(error = aResult);
    VerifyOrExit((message = NewMeshCoPMessage(Get<Coap::CoapSecure>())) != nullptr, error = kErrorNoBufs);

    if (aForwardContext.IsPetition() && aResponse->GetCode() == Coap::kCodeChanged)
    {
        uint8_t state;

        SuccessOrExit(error = Tlv::Find<StateTlv>(*aResponse, state));

        if (state == StateTlv::kAccept)
        {
            uint16_t sessionId;

            SuccessOrExit(error = Tlv::Find<CommissionerSessionIdTlv>(*aResponse, sessionId));

            IgnoreError(Get<Mle::MleRouter>().GetCommissionerAloc(mCommissionerAloc.GetAddress(), sessionId));
            Get<ThreadNetif>().AddUnicastAddress(mCommissionerAloc);
            IgnoreError(Get<Ip6::Udp>().AddReceiver(mUdpReceiver));

            otLogInfoMeshCoP("commissioner accepted: session ID=%d, ALOC=%s", sessionId,
                             mCommissionerAloc.GetAddress().ToString().AsCString());
        }
    }

    SuccessOrExit(error = aForwardContext.ToHeader(*message, aResponse->GetCode()));

    if (aResponse->GetLength() > aResponse->GetOffset())
    {
        SuccessOrExit(error = message->SetPayloadMarker());
    }

    SuccessOrExit(error = ForwardToCommissioner(*message, *aResponse));

exit:

    if (error != kErrorNone)
    {
        FreeMessage(message);

        otLogWarnMeshCoP("Commissioner request[%hu] failed: %s", aForwardContext.GetMessageId(), ErrorToString(error));

        SendErrorMessage(aForwardContext, error);
    }

    Instance::HeapFree(&aForwardContext);
}

template <Coap::Resource BorderAgent::*aResource>
void BorderAgent::HandleRequest(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    IgnoreError(static_cast<BorderAgent *>(aContext)->ForwardToLeader(
        *static_cast<Coap::Message *>(aMessage), *static_cast<const Ip6::MessageInfo *>(aMessageInfo),
        (static_cast<BorderAgent *>(aContext)->*aResource).GetUriPath(), false, false));
}

template <>
void BorderAgent::HandleRequest<&BorderAgent::mCommissionerPetition>(void *               aContext,
                                                                     otMessage *          aMessage,
                                                                     const otMessageInfo *aMessageInfo)
{
    IgnoreError(static_cast<BorderAgent *>(aContext)->ForwardToLeader(
        *static_cast<Coap::Message *>(aMessage), *static_cast<const Ip6::MessageInfo *>(aMessageInfo),
        UriPath::kLeaderPetition, true, true));
}

template <>
void BorderAgent::HandleRequest<&BorderAgent::mCommissionerKeepAlive>(void *               aContext,
                                                                      otMessage *          aMessage,
                                                                      const otMessageInfo *aMessageInfo)
{
    static_cast<BorderAgent *>(aContext)->HandleKeepAlive(*static_cast<Coap::Message *>(aMessage),
                                                          *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

template <>
void BorderAgent::HandleRequest<&BorderAgent::mRelayTransmit>(void *               aContext,
                                                              otMessage *          aMessage,
                                                              const otMessageInfo *aMessageInfo)
{
    OT_UNUSED_VARIABLE(aMessageInfo);
    static_cast<BorderAgent *>(aContext)->HandleRelayTransmit(*static_cast<Coap::Message *>(aMessage));
}

template <>
void BorderAgent::HandleRequest<&BorderAgent::mRelayReceive>(void *               aContext,
                                                             otMessage *          aMessage,
                                                             const otMessageInfo *aMessageInfo)
{
    OT_UNUSED_VARIABLE(aMessageInfo);
    static_cast<BorderAgent *>(aContext)->HandleRelayReceive(*static_cast<Coap::Message *>(aMessage));
}

template <>
void BorderAgent::HandleRequest<&BorderAgent::mProxyTransmit>(void *               aContext,
                                                              otMessage *          aMessage,
                                                              const otMessageInfo *aMessageInfo)
{
    OT_UNUSED_VARIABLE(aMessageInfo);
    static_cast<BorderAgent *>(aContext)->HandleProxyTransmit(*static_cast<Coap::Message *>(aMessage));
}

BorderAgent::BorderAgent(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mCommissionerPetition(UriPath::kCommissionerPetition,
                            BorderAgent::HandleRequest<&BorderAgent::mCommissionerPetition>,
                            this)
    , mCommissionerKeepAlive(UriPath::kCommissionerKeepAlive,
                             BorderAgent::HandleRequest<&BorderAgent::mCommissionerKeepAlive>,
                             this)
    , mRelayTransmit(UriPath::kRelayTx, BorderAgent::HandleRequest<&BorderAgent::mRelayTransmit>, this)
    , mRelayReceive(UriPath::kRelayRx, BorderAgent::HandleRequest<&BorderAgent::mRelayReceive>, this)
    , mCommissionerGet(UriPath::kCommissionerGet, BorderAgent::HandleRequest<&BorderAgent::mCommissionerGet>, this)
    , mCommissionerSet(UriPath::kCommissionerSet, BorderAgent::HandleRequest<&BorderAgent::mCommissionerSet>, this)
    , mActiveGet(UriPath::kActiveGet, BorderAgent::HandleRequest<&BorderAgent::mActiveGet>, this)
    , mActiveSet(UriPath::kActiveSet, BorderAgent::HandleRequest<&BorderAgent::mActiveSet>, this)
    , mPendingGet(UriPath::kPendingGet, BorderAgent::HandleRequest<&BorderAgent::mPendingGet>, this)
    , mPendingSet(UriPath::kPendingSet, BorderAgent::HandleRequest<&BorderAgent::mPendingSet>, this)
    , mProxyTransmit(UriPath::kProxyTx, BorderAgent::HandleRequest<&BorderAgent::mProxyTransmit>, this)
    , mUdpReceiver(BorderAgent::HandleUdpReceive, this)
    , mTimer(aInstance, HandleTimeout)
    , mState(kStateStopped)
    , mUdpProxyPort(0)
{
    mCommissionerAloc.InitAsThreadOriginRealmLocalScope();
}

void BorderAgent::HandleNotifierEvents(Events aEvents)
{
    VerifyOrExit(aEvents.ContainsAny(kEventThreadRoleChanged | kEventCommissionerStateChanged));

#if OPENTHREAD_CONFIG_COMMISSIONER_ENABLE && OPENTHREAD_FTD
    VerifyOrExit(Get<MeshCoP::Commissioner>().IsDisabled());
#endif

    if (Get<Mle::MleRouter>().IsAttached())
    {
        Start();
    }
    else
    {
        Stop();
    }

exit:
    return;
}

void BorderAgent::HandleProxyTransmit(const Coap::Message &aMessage)
{
    Message *           message = nullptr;
    Ip6::MessageInfo    messageInfo;
    uint16_t            offset;
    Error               error;
    UdpEncapsulationTlv tlv;

    SuccessOrExit(error = Tlv::FindTlvOffset(aMessage, Tlv::kUdpEncapsulation, offset));
    SuccessOrExit(error = aMessage.Read(offset, tlv));

    VerifyOrExit((message = Get<Ip6::Udp>().NewMessage(0)) != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = message->SetLength(tlv.GetUdpLength()));
    aMessage.CopyTo(offset + sizeof(tlv), 0, tlv.GetUdpLength(), *message);

    VerifyOrExit(tlv.GetSourcePort() > 0 && tlv.GetDestinationPort() > 0, error = kErrorDrop);

    messageInfo.SetSockPort(tlv.GetSourcePort());
    messageInfo.SetSockAddr(mCommissionerAloc.GetAddress());
    messageInfo.SetPeerPort(tlv.GetDestinationPort());

    SuccessOrExit(error = Tlv::Find<Ip6AddressTlv>(aMessage, messageInfo.GetPeerAddr()));

    SuccessOrExit(error = Get<Ip6::Udp>().SendDatagram(*message, messageInfo, Ip6::kProtoUdp));
    mUdpProxyPort = tlv.GetSourcePort();

    otLogInfoMeshCoP("Proxy transmit sent to %s", messageInfo.GetPeerAddr().ToString().AsCString());

exit:
    FreeMessageOnError(message, error);
    LogError("send proxy stream", error);
}

bool BorderAgent::HandleUdpReceive(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    Error          error;
    Coap::Message *message = nullptr;

    VerifyOrExit(aMessageInfo.GetSockAddr() == mCommissionerAloc.GetAddress(),
                 error = kErrorDestinationAddressFiltered);

    VerifyOrExit(aMessage.GetLength() > 0, error = kErrorNone);

    VerifyOrExit((message = NewMeshCoPMessage(Get<Coap::CoapSecure>())) != nullptr, error = kErrorNoBufs);

    message->InitAsNonConfirmablePost();
    SuccessOrExit(error = message->AppendUriPathOptions(UriPath::kProxyRx));
    SuccessOrExit(error = message->SetPayloadMarker());

    {
        UdpEncapsulationTlv tlv;
        uint16_t            offset;
        uint16_t            udpLength = aMessage.GetLength() - aMessage.GetOffset();

        tlv.Init();
        tlv.SetSourcePort(aMessageInfo.GetPeerPort());
        tlv.SetDestinationPort(aMessageInfo.GetSockPort());
        tlv.SetUdpLength(udpLength);
        SuccessOrExit(error = message->Append(tlv));

        offset = message->GetLength();
        SuccessOrExit(error = message->SetLength(offset + udpLength));
        aMessage.CopyTo(aMessage.GetOffset(), offset, udpLength, *message);
    }

    SuccessOrExit(error = Tlv::Append<Ip6AddressTlv>(*message, aMessageInfo.GetPeerAddr()));

    SuccessOrExit(error = Get<Coap::CoapSecure>().SendMessage(*message, Get<Coap::CoapSecure>().GetMessageInfo()));

    otLogInfoMeshCoP("Sent to commissioner on %s", UriPath::kProxyRx);

exit:
    FreeMessageOnError(message, error);
    LogError("notify commissioner on ProxyRx (c/ur)", error);

    return error != kErrorDestinationAddressFiltered;
}

void BorderAgent::HandleRelayReceive(const Coap::Message &aMessage)
{
    Coap::Message *message = nullptr;
    Error          error;

    VerifyOrExit(aMessage.IsNonConfirmablePostRequest(), error = kErrorDrop);
    VerifyOrExit((message = NewMeshCoPMessage(Get<Coap::CoapSecure>())) != nullptr, error = kErrorNoBufs);

    message->InitAsNonConfirmablePost();
    SuccessOrExit(error = message->AppendUriPathOptions(UriPath::kRelayRx));

    if (aMessage.GetLength() > aMessage.GetOffset())
    {
        SuccessOrExit(error = message->SetPayloadMarker());
    }

    SuccessOrExit(error = ForwardToCommissioner(*message, aMessage));
    otLogInfoMeshCoP("Sent to commissioner on %s", UriPath::kRelayRx);

exit:
    FreeMessageOnError(message, error);
}

Error BorderAgent::ForwardToCommissioner(Coap::Message &aForwardMessage, const Message &aMessage)
{
    Error    error  = kErrorNone;
    uint16_t offset = 0;

    offset = aForwardMessage.GetLength();
    SuccessOrExit(error = aForwardMessage.SetLength(offset + aMessage.GetLength() - aMessage.GetOffset()));
    aMessage.CopyTo(aMessage.GetOffset(), offset, aMessage.GetLength() - aMessage.GetOffset(), aForwardMessage);

    SuccessOrExit(error =
                      Get<Coap::CoapSecure>().SendMessage(aForwardMessage, Get<Coap::CoapSecure>().GetMessageInfo()));

    otLogInfoMeshCoP("Sent to commissioner");

exit:
    LogError("send to commissioner", error);
    return error;
}

void BorderAgent::HandleKeepAlive(const Coap::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    Error error;

    error = ForwardToLeader(aMessage, aMessageInfo, UriPath::kLeaderKeepAlive, false, true);

    if (error == kErrorNone)
    {
        mTimer.Start(kKeepAliveTimeout);
    }
}

void BorderAgent::HandleRelayTransmit(const Coap::Message &aMessage)
{
    Error            error = kErrorNone;
    uint16_t         joinerRouterRloc;
    Coap::Message *  message = nullptr;
    Ip6::MessageInfo messageInfo;
    uint16_t         offset = 0;

    VerifyOrExit(aMessage.IsNonConfirmablePostRequest());

    SuccessOrExit(error = Tlv::Find<JoinerRouterLocatorTlv>(aMessage, joinerRouterRloc));

    VerifyOrExit((message = NewMeshCoPMessage(Get<Tmf::Agent>())) != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = message->InitAsNonConfirmablePost(UriPath::kRelayTx));
    SuccessOrExit(error = message->SetPayloadMarker());

    offset = message->GetLength();
    SuccessOrExit(error = message->SetLength(offset + aMessage.GetLength() - aMessage.GetOffset()));
    aMessage.CopyTo(aMessage.GetOffset(), offset, aMessage.GetLength() - aMessage.GetOffset(), *message);

    messageInfo.SetSockPort(Tmf::kUdpPort);
    messageInfo.SetSockAddr(Get<Mle::MleRouter>().GetMeshLocal16());
    messageInfo.SetPeerPort(Tmf::kUdpPort);
    messageInfo.SetPeerAddr(Get<Mle::MleRouter>().GetMeshLocal16());
    messageInfo.GetPeerAddr().GetIid().SetLocator(joinerRouterRloc);

    SuccessOrExit(error = Get<Tmf::Agent>().SendMessage(*message, messageInfo));

    otLogInfoMeshCoP("Sent to joiner router request on %s", UriPath::kRelayTx);

exit:
    FreeMessageOnError(message, error);
    LogError("send to joiner router request RelayTx (c/tx)", error);
}

Error BorderAgent::ForwardToLeader(const Coap::Message &   aMessage,
                                   const Ip6::MessageInfo &aMessageInfo,
                                   const char *            aPath,
                                   bool                    aPetition,
                                   bool                    aSeparate)
{
    Error            error          = kErrorNone;
    ForwardContext * forwardContext = nullptr;
    Ip6::MessageInfo messageInfo;
    Coap::Message *  message = nullptr;
    uint16_t         offset  = 0;

    VerifyOrExit((message = NewMeshCoPMessage(Get<Tmf::Agent>())) != nullptr, error = kErrorNoBufs);

    if (aSeparate)
    {
        SuccessOrExit(error = Get<Coap::CoapSecure>().SendAck(aMessage, aMessageInfo));
    }

    forwardContext = static_cast<ForwardContext *>(Instance::HeapCAlloc(1, sizeof(ForwardContext)));
    VerifyOrExit(forwardContext != nullptr, error = kErrorNoBufs);

    forwardContext->Init(GetInstance(), aMessage, aPetition, aSeparate);

    SuccessOrExit(error = message->InitAsConfirmablePost(aPath));

    // Payload of c/cg may be empty
    if (aMessage.GetLength() - aMessage.GetOffset() > 0)
    {
        SuccessOrExit(error = message->SetPayloadMarker());
    }

    offset = message->GetLength();
    SuccessOrExit(error = message->SetLength(offset + aMessage.GetLength() - aMessage.GetOffset()));
    aMessage.CopyTo(aMessage.GetOffset(), offset, aMessage.GetLength() - aMessage.GetOffset(), *message);

    SuccessOrExit(error = Get<Mle::MleRouter>().GetLeaderAloc(messageInfo.GetPeerAddr()));
    messageInfo.SetPeerPort(Tmf::kUdpPort);
    messageInfo.SetSockAddr(Get<Mle::MleRouter>().GetMeshLocal16());
    messageInfo.SetSockPort(Tmf::kUdpPort);

    SuccessOrExit(error = Get<Tmf::Agent>().SendMessage(*message, messageInfo, HandleCoapResponse, forwardContext));

    // HandleCoapResponse is responsible to free this forward context.
    forwardContext = nullptr;

    otLogInfoMeshCoP("Forwarded request to leader on %s", aPath);

exit:
    LogError("forward to leader", error);

    if (error != kErrorNone)
    {
        if (forwardContext != nullptr)
        {
            Instance::HeapFree(forwardContext);
        }

        FreeMessage(message);
        SendErrorMessage(aMessage, aSeparate, error);
    }

    return error;
}

void BorderAgent::HandleConnected(bool aConnected, void *aContext)
{
    static_cast<BorderAgent *>(aContext)->HandleConnected(aConnected);
}

void BorderAgent::HandleConnected(bool aConnected)
{
    if (aConnected)
    {
        otLogInfoMeshCoP("Commissioner connected");
        mState = kStateActive;
        mTimer.Start(kKeepAliveTimeout);
    }
    else
    {
        otLogInfoMeshCoP("Commissioner disconnected");
        IgnoreError(Get<Ip6::Udp>().RemoveReceiver(mUdpReceiver));
        Get<ThreadNetif>().RemoveUnicastAddress(mCommissionerAloc);
        mState        = kStateStarted;
        mUdpProxyPort = 0;
    }
}

uint16_t BorderAgent::GetUdpPort(void) const
{
    return Get<Coap::CoapSecure>().GetUdpPort();
}

void BorderAgent::Start(void)
{
    Error             error;
    Coap::CoapSecure &coaps = Get<Coap::CoapSecure>();
    Pskc              pskc;

    VerifyOrExit(mState == kStateStopped, error = kErrorAlready);

    Get<KeyManager>().GetPskc(pskc);
    SuccessOrExit(error = coaps.Start(kBorderAgentUdpPort));
    SuccessOrExit(error = coaps.SetPsk(pskc.m8, Pskc::kSize));

    pskc.Clear();
    coaps.SetConnectedCallback(HandleConnected, this);

    coaps.AddResource(mActiveGet);
    coaps.AddResource(mActiveSet);
    coaps.AddResource(mPendingGet);
    coaps.AddResource(mPendingSet);
    coaps.AddResource(mCommissionerPetition);
    coaps.AddResource(mCommissionerKeepAlive);
    coaps.AddResource(mCommissionerSet);
    coaps.AddResource(mCommissionerGet);
    coaps.AddResource(mProxyTransmit);
    coaps.AddResource(mRelayTransmit);

    Get<Tmf::Agent>().AddResource(mRelayReceive);

    mState        = kStateStarted;
    mUdpProxyPort = 0;

    otLogInfoMeshCoP("Border Agent start listening on port %d", kBorderAgentUdpPort);

exit:
    if (error != kErrorNone)
    {
        otLogWarnMeshCoP("failed to start Border Agent on port %d: %s", kBorderAgentUdpPort, ErrorToString(error));
    }
}

void BorderAgent::HandleTimeout(Timer &aTimer)
{
    aTimer.Get<BorderAgent>().HandleTimeout();
}

void BorderAgent::HandleTimeout(void)
{
    if (Get<Coap::CoapSecure>().IsConnected())
    {
        Get<Coap::CoapSecure>().Disconnect();
        otLogWarnMeshCoP("Reset commissioner session");
    }
}

void BorderAgent::Stop(void)
{
    Coap::CoapSecure &coaps = Get<Coap::CoapSecure>();

    VerifyOrExit(mState != kStateStopped);

    mTimer.Stop();

    coaps.RemoveResource(mCommissionerPetition);
    coaps.RemoveResource(mCommissionerKeepAlive);
    coaps.RemoveResource(mCommissionerSet);
    coaps.RemoveResource(mCommissionerGet);
    coaps.RemoveResource(mActiveGet);
    coaps.RemoveResource(mActiveSet);
    coaps.RemoveResource(mPendingGet);
    coaps.RemoveResource(mPendingSet);
    coaps.RemoveResource(mProxyTransmit);
    coaps.RemoveResource(mRelayTransmit);

    Get<Tmf::Agent>().RemoveResource(mRelayReceive);

    coaps.Stop();

    mState        = kStateStopped;
    mUdpProxyPort = 0;

    otLogInfoMeshCoP("Border Agent stopped");

exit:
    return;
}

void BorderAgent::ApplyMeshLocalPrefix(void)
{
    VerifyOrExit(mState == kStateActive);

    if (Get<ThreadNetif>().HasUnicastAddress(mCommissionerAloc))
    {
        Get<ThreadNetif>().RemoveUnicastAddress(mCommissionerAloc);
        mCommissionerAloc.GetAddress().SetPrefix(Get<Mle::MleRouter>().GetMeshLocalPrefix());
        Get<ThreadNetif>().AddUnicastAddress(mCommissionerAloc);
    }

exit:
    return;
}

} // namespace MeshCoP
} // namespace ot

#endif // OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
