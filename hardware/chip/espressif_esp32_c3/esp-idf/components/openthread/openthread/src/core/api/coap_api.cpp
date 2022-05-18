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
 *   This file implements the OpenThread CoAP API.
 */

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_COAP_API_ENABLE

#include <openthread/coap.h>

#include "coap/coap_message.hpp"
#include "common/as_core_type.hpp"
#include "common/locator_getters.hpp"

using namespace ot;

otMessage *otCoapNewMessage(otInstance *aInstance, const otMessageSettings *aSettings)
{
    return AsCoreType(aInstance).GetApplicationCoap().NewMessage(Message::Settings::From(aSettings));
}

void otCoapMessageInit(otMessage *aMessage, otCoapType aType, otCoapCode aCode)
{
    AsCoapMessage(aMessage).Init(MapEnum(aType), MapEnum(aCode));
}

otError otCoapMessageInitResponse(otMessage *aResponse, const otMessage *aRequest, otCoapType aType, otCoapCode aCode)
{
    Coap::Message &      response = AsCoapMessage(aResponse);
    const Coap::Message &request  = AsCoapMessage(aRequest);

    response.Init(MapEnum(aType), MapEnum(aCode));
    response.SetMessageId(request.GetMessageId());

    return response.SetTokenFromMessage(request);
}

otError otCoapMessageSetToken(otMessage *aMessage, const uint8_t *aToken, uint8_t aTokenLength)
{
    return AsCoapMessage(aMessage).SetToken(aToken, aTokenLength);
}

void otCoapMessageGenerateToken(otMessage *aMessage, uint8_t aTokenLength)
{
    IgnoreError(AsCoapMessage(aMessage).GenerateRandomToken(aTokenLength));
}

otError otCoapMessageAppendContentFormatOption(otMessage *aMessage, otCoapOptionContentFormat aContentFormat)
{
    return AsCoapMessage(aMessage).AppendContentFormatOption(aContentFormat);
}

otError otCoapMessageAppendOption(otMessage *aMessage, uint16_t aNumber, uint16_t aLength, const void *aValue)
{
    return AsCoapMessage(aMessage).AppendOption(aNumber, aLength, aValue);
}

otError otCoapMessageAppendUintOption(otMessage *aMessage, uint16_t aNumber, uint32_t aValue)
{
    return AsCoapMessage(aMessage).AppendUintOption(aNumber, aValue);
}

otError otCoapMessageAppendObserveOption(otMessage *aMessage, uint32_t aObserve)
{
    return AsCoapMessage(aMessage).AppendObserveOption(aObserve);
}

otError otCoapMessageAppendUriPathOptions(otMessage *aMessage, const char *aUriPath)
{
    return AsCoapMessage(aMessage).AppendUriPathOptions(aUriPath);
}

uint16_t otCoapBlockSizeFromExponent(otCoapBlockSzx aSize)
{
    return static_cast<uint16_t>(1 << (static_cast<uint8_t>(aSize) + Coap::Message::kBlockSzxBase));
}

otError otCoapMessageAppendBlock2Option(otMessage *aMessage, uint32_t aNum, bool aMore, otCoapBlockSzx aSize)
{
    return AsCoapMessage(aMessage).AppendBlockOption(Coap::Message::kBlockType2, aNum, aMore, aSize);
}

otError otCoapMessageAppendBlock1Option(otMessage *aMessage, uint32_t aNum, bool aMore, otCoapBlockSzx aSize)
{
    return AsCoapMessage(aMessage).AppendBlockOption(Coap::Message::kBlockType1, aNum, aMore, aSize);
}

otError otCoapMessageAppendProxyUriOption(otMessage *aMessage, const char *aUriPath)
{
    return AsCoapMessage(aMessage).AppendProxyUriOption(aUriPath);
}

otError otCoapMessageAppendMaxAgeOption(otMessage *aMessage, uint32_t aMaxAge)
{
    return AsCoapMessage(aMessage).AppendMaxAgeOption(aMaxAge);
}

otError otCoapMessageAppendUriQueryOption(otMessage *aMessage, const char *aUriQuery)
{
    return AsCoapMessage(aMessage).AppendUriQueryOption(aUriQuery);
}

otError otCoapMessageSetPayloadMarker(otMessage *aMessage)
{
    return AsCoapMessage(aMessage).SetPayloadMarker();
}

otCoapType otCoapMessageGetType(const otMessage *aMessage)
{
    return static_cast<otCoapType>(AsCoapMessage(aMessage).GetType());
}

otCoapCode otCoapMessageGetCode(const otMessage *aMessage)
{
    return static_cast<otCoapCode>(AsCoapMessage(aMessage).GetCode());
}

const char *otCoapMessageCodeToString(const otMessage *aMessage)
{
    return AsCoapMessage(aMessage).CodeToString();
}

uint16_t otCoapMessageGetMessageId(const otMessage *aMessage)
{
    return AsCoapMessage(aMessage).GetMessageId();
}

uint8_t otCoapMessageGetTokenLength(const otMessage *aMessage)
{
    return AsCoapMessage(aMessage).GetTokenLength();
}

const uint8_t *otCoapMessageGetToken(const otMessage *aMessage)
{
    return AsCoapMessage(aMessage).GetToken();
}

otError otCoapOptionIteratorInit(otCoapOptionIterator *aIterator, const otMessage *aMessage)
{
    return AsCoreType(aIterator).Init(AsCoapMessage(aMessage));
}

const otCoapOption *otCoapOptionIteratorGetFirstOptionMatching(otCoapOptionIterator *aIterator, uint16_t aOption)
{
    Coap::Option::Iterator &iterator = AsCoreType(aIterator);

    IgnoreError(iterator.Init(iterator.GetMessage(), aOption));
    return iterator.GetOption();
}

const otCoapOption *otCoapOptionIteratorGetFirstOption(otCoapOptionIterator *aIterator)
{
    Coap::Option::Iterator &iterator = AsCoreType(aIterator);

    IgnoreError(iterator.Init(iterator.GetMessage()));
    return iterator.GetOption();
}

const otCoapOption *otCoapOptionIteratorGetNextOptionMatching(otCoapOptionIterator *aIterator, uint16_t aOption)
{
    Coap::Option::Iterator &iterator = AsCoreType(aIterator);

    IgnoreError(iterator.Advance(aOption));
    return iterator.GetOption();
}

const otCoapOption *otCoapOptionIteratorGetNextOption(otCoapOptionIterator *aIterator)
{
    Coap::Option::Iterator &iterator = AsCoreType(aIterator);

    IgnoreError(iterator.Advance());
    return iterator.GetOption();
}

otError otCoapOptionIteratorGetOptionUintValue(otCoapOptionIterator *aIterator, uint64_t *aValue)
{
    return AsCoreType(aIterator).ReadOptionValue(*aValue);
}

otError otCoapOptionIteratorGetOptionValue(otCoapOptionIterator *aIterator, void *aValue)
{
    return AsCoreType(aIterator).ReadOptionValue(aValue);
}

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
otError otCoapSendRequestBlockWiseWithParameters(otInstance *                aInstance,
                                                 otMessage *                 aMessage,
                                                 const otMessageInfo *       aMessageInfo,
                                                 otCoapResponseHandler       aHandler,
                                                 void *                      aContext,
                                                 const otCoapTxParameters *  aTxParameters,
                                                 otCoapBlockwiseTransmitHook aTransmitHook,
                                                 otCoapBlockwiseReceiveHook  aReceiveHook)
{
    Error                     error;
    const Coap::TxParameters &txParameters = Coap::TxParameters::From(aTxParameters);

    if (aTxParameters != nullptr)
    {
        VerifyOrExit(txParameters.IsValid(), error = kErrorInvalidArgs);
    }

    error = AsCoreType(aInstance).GetApplicationCoap().SendMessage(AsCoapMessage(aMessage), AsCoreType(aMessageInfo),
                                                                   txParameters, aHandler, aContext, aTransmitHook,
                                                                   aReceiveHook);

exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE

otError otCoapSendRequestWithParameters(otInstance *              aInstance,
                                        otMessage *               aMessage,
                                        const otMessageInfo *     aMessageInfo,
                                        otCoapResponseHandler     aHandler,
                                        void *                    aContext,
                                        const otCoapTxParameters *aTxParameters)
{
    Error error;

    const Coap::TxParameters &txParameters = Coap::TxParameters::From(aTxParameters);

    if (aTxParameters != nullptr)
    {
        VerifyOrExit(txParameters.IsValid(), error = kErrorInvalidArgs);
    }

    error = AsCoreType(aInstance).GetApplicationCoap().SendMessage(AsCoapMessage(aMessage), AsCoreType(aMessageInfo),
                                                                   txParameters, aHandler, aContext);

exit:
    return error;
}

otError otCoapStart(otInstance *aInstance, uint16_t aPort)
{
    return AsCoreType(aInstance).GetApplicationCoap().Start(aPort);
}

otError otCoapStop(otInstance *aInstance)
{
    return AsCoreType(aInstance).GetApplicationCoap().Stop();
}

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
void otCoapAddBlockWiseResource(otInstance *aInstance, otCoapBlockwiseResource *aResource)
{
    AsCoreType(aInstance).GetApplicationCoap().AddBlockWiseResource(AsCoreType(aResource));
}

void otCoapRemoveBlockWiseResource(otInstance *aInstance, otCoapBlockwiseResource *aResource)
{
    AsCoreType(aInstance).GetApplicationCoap().RemoveBlockWiseResource(AsCoreType(aResource));
}
#endif

void otCoapAddResource(otInstance *aInstance, otCoapResource *aResource)
{
    AsCoreType(aInstance).GetApplicationCoap().AddResource(AsCoreType(aResource));
}

void otCoapRemoveResource(otInstance *aInstance, otCoapResource *aResource)
{
    AsCoreType(aInstance).GetApplicationCoap().RemoveResource(AsCoreType(aResource));
}

void otCoapSetDefaultHandler(otInstance *aInstance, otCoapRequestHandler aHandler, void *aContext)
{
    AsCoreType(aInstance).GetApplicationCoap().SetDefaultHandler(aHandler, aContext);
}

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
otError otCoapSendResponseBlockWiseWithParameters(otInstance *                aInstance,
                                                  otMessage *                 aMessage,
                                                  const otMessageInfo *       aMessageInfo,
                                                  const otCoapTxParameters *  aTxParameters,
                                                  void *                      aContext,
                                                  otCoapBlockwiseTransmitHook aTransmitHook)
{
    return AsCoreType(aInstance).GetApplicationCoap().SendMessage(AsCoapMessage(aMessage), AsCoreType(aMessageInfo),
                                                                  Coap::TxParameters::From(aTxParameters), nullptr,
                                                                  aContext, aTransmitHook, nullptr);
}
#endif

otError otCoapSendResponseWithParameters(otInstance *              aInstance,
                                         otMessage *               aMessage,
                                         const otMessageInfo *     aMessageInfo,
                                         const otCoapTxParameters *aTxParameters)
{
    return AsCoreType(aInstance).GetApplicationCoap().SendMessage(
        AsCoapMessage(aMessage), AsCoreType(aMessageInfo), Coap::TxParameters::From(aTxParameters), nullptr, nullptr);
}

#endif // OPENTHREAD_CONFIG_COAP_API_ENABLE
