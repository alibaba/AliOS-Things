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

#include "coap_secure.hpp"

#if OPENTHREAD_CONFIG_DTLS_ENABLE

#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/new.hpp"
#include "meshcop/dtls.hpp"
#include "thread/thread_netif.hpp"

/**
 * @file
 *   This file implements the secure CoAP agent.
 */

namespace ot {
namespace Coap {

CoapSecure::CoapSecure(Instance &aInstance, bool aLayerTwoSecurity)
    : CoapBase(aInstance, &CoapSecure::Send)
    , mDtls(aInstance, aLayerTwoSecurity)
    , mConnectedCallback(nullptr)
    , mConnectedContext(nullptr)
    , mTransmitTask(aInstance, CoapSecure::HandleTransmit, this)
{
}

Error CoapSecure::Start(uint16_t aPort)
{
    Error error = kErrorNone;

    mConnectedCallback = nullptr;
    mConnectedContext  = nullptr;

    SuccessOrExit(error = mDtls.Open(&CoapSecure::HandleDtlsReceive, &CoapSecure::HandleDtlsConnected, this));
    SuccessOrExit(error = mDtls.Bind(aPort));

exit:
    return error;
}

Error CoapSecure::Start(MeshCoP::Dtls::TransportCallback aCallback, void *aContext)
{
    Error error = kErrorNone;

    mConnectedCallback = nullptr;
    mConnectedContext  = nullptr;

    SuccessOrExit(error = mDtls.Open(&CoapSecure::HandleDtlsReceive, &CoapSecure::HandleDtlsConnected, this));
    SuccessOrExit(error = mDtls.Bind(aCallback, aContext));

exit:
    return error;
}

void CoapSecure::Stop(void)
{
    mDtls.Close();

    mTransmitQueue.DequeueAndFreeAll();
    ClearRequestsAndResponses();
}

Error CoapSecure::Connect(const Ip6::SockAddr &aSockAddr, ConnectedCallback aCallback, void *aContext)
{
    mConnectedCallback = aCallback;
    mConnectedContext  = aContext;

    return mDtls.Connect(aSockAddr);
}

void CoapSecure::SetPsk(const MeshCoP::JoinerPskd &aPskd)
{
    Error error;

    OT_UNUSED_VARIABLE(error);

    static_assert(static_cast<uint16_t>(MeshCoP::JoinerPskd::kMaxLength) <=
                      static_cast<uint16_t>(MeshCoP::Dtls::kPskMaxLength),
                  "The maximum length of DTLS PSK is smaller than joiner PSKd");

    error = mDtls.SetPsk(reinterpret_cast<const uint8_t *>(aPskd.GetAsCString()), aPskd.GetLength());

    OT_ASSERT(error == kErrorNone);
}

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
Error CoapSecure::SendMessage(Message &                   aMessage,
                              ResponseHandler             aHandler,
                              void *                      aContext,
                              otCoapBlockwiseTransmitHook aTransmitHook,
                              otCoapBlockwiseReceiveHook  aReceiveHook)
{
    Error error = kErrorNone;

    VerifyOrExit(IsConnected(), error = kErrorInvalidState);

    error = CoapBase::SendMessage(aMessage, mDtls.GetMessageInfo(), TxParameters::GetDefault(), aHandler, aContext,
                                  aTransmitHook, aReceiveHook);

exit:
    return error;
}

Error CoapSecure::SendMessage(Message &                   aMessage,
                              const Ip6::MessageInfo &    aMessageInfo,
                              ResponseHandler             aHandler,
                              void *                      aContext,
                              otCoapBlockwiseTransmitHook aTransmitHook,
                              otCoapBlockwiseReceiveHook  aReceiveHook)
{
    return CoapBase::SendMessage(aMessage, aMessageInfo, TxParameters::GetDefault(), aHandler, aContext, aTransmitHook,
                                 aReceiveHook);
}
#else  // OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
Error CoapSecure::SendMessage(Message &aMessage, ResponseHandler aHandler, void *aContext)
{
    Error error = kErrorNone;

    VerifyOrExit(IsConnected(), error = kErrorInvalidState);

    error = CoapBase::SendMessage(aMessage, mDtls.GetMessageInfo(), aHandler, aContext);

exit:
    return error;
}

Error CoapSecure::SendMessage(Message &               aMessage,
                              const Ip6::MessageInfo &aMessageInfo,
                              ResponseHandler         aHandler,
                              void *                  aContext)
{
    return CoapBase::SendMessage(aMessage, aMessageInfo, aHandler, aContext);
}
#endif // OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE

Error CoapSecure::Send(ot::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    mTransmitQueue.Enqueue(aMessage);
    mTransmitTask.Post();

    return kErrorNone;
}

void CoapSecure::HandleDtlsConnected(void *aContext, bool aConnected)
{
    return static_cast<CoapSecure *>(aContext)->HandleDtlsConnected(aConnected);
}

void CoapSecure::HandleDtlsConnected(bool aConnected)
{
    if (mConnectedCallback != nullptr)
    {
        mConnectedCallback(aConnected, mConnectedContext);
    }
}

void CoapSecure::HandleDtlsReceive(void *aContext, uint8_t *aBuf, uint16_t aLength)
{
    return static_cast<CoapSecure *>(aContext)->HandleDtlsReceive(aBuf, aLength);
}

void CoapSecure::HandleDtlsReceive(uint8_t *aBuf, uint16_t aLength)
{
    ot::Message *message = nullptr;

    VerifyOrExit((message = Get<MessagePool>().New(Message::kTypeIp6, Message::GetHelpDataReserved())) != nullptr);
    SuccessOrExit(message->AppendBytes(aBuf, aLength));

    CoapBase::Receive(*message, mDtls.GetMessageInfo());

exit:
    FreeMessage(message);
}

void CoapSecure::HandleTransmit(Tasklet &aTasklet)
{
    static_cast<CoapSecure *>(static_cast<TaskletContext &>(aTasklet).GetContext())->HandleTransmit();
}

void CoapSecure::HandleTransmit(void)
{
    Error        error   = kErrorNone;
    ot::Message *message = mTransmitQueue.GetHead();

    VerifyOrExit(message != nullptr);
    mTransmitQueue.Dequeue(*message);

    if (mTransmitQueue.GetHead() != nullptr)
    {
        mTransmitTask.Post();
    }

    SuccessOrExit(error = mDtls.Send(*message, message->GetLength()));

exit:
    if (error != kErrorNone)
    {
        otLogNoteMeshCoP("CoapSecure Transmit: %s", ErrorToString(error));
        message->Free();
    }
    else
    {
        otLogDebgMeshCoP("CoapSecure Transmit: %s", ErrorToString(error));
    }
}

} // namespace Coap
} // namespace ot

#endif // OPENTHREAD_CONFIG_DTLS_ENABLE
