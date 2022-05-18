/*
 *  Copyright (c) 2017, The OpenThread Authors.
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
 *   This file implements a simple CLI for the CoAP service.
 */

#include "cli_coap.hpp"

#if OPENTHREAD_CONFIG_COAP_API_ENABLE

#include <openthread/random_noncrypto.h>

#include <ctype.h>

#include "cli/cli.hpp"

namespace ot {
namespace Cli {

constexpr Coap::Command Coap::sCommands[];

Coap::Coap(Output &aOutput)
    : OutputWrapper(aOutput)
    , mUseDefaultRequestTxParameters(true)
    , mUseDefaultResponseTxParameters(true)
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
    , mObserveSerial(0)
    , mRequestTokenLength(0)
    , mSubscriberTokenLength(0)
    , mSubscriberConfirmableNotifications(false)
#endif
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    , mBlockCount(1)
#endif
{
    memset(&mResource, 0, sizeof(mResource));
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
    memset(&mRequestAddr, 0, sizeof(mRequestAddr));
    memset(&mSubscriberSock, 0, sizeof(mSubscriberSock));
    memset(&mRequestToken, 0, sizeof(mRequestToken));
    memset(&mSubscriberToken, 0, sizeof(mSubscriberToken));
    memset(&mRequestUri, 0, sizeof(mRequestUri));
#endif
    memset(&mUriPath, 0, sizeof(mUriPath));
    strncpy(mResourceContent, "0", sizeof(mResourceContent));
    mResourceContent[sizeof(mResourceContent) - 1] = '\0';
}

#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
otError Coap::CancelResourceSubscription(void)
{
    otError       error   = OT_ERROR_NONE;
    otMessage *   message = nullptr;
    otMessageInfo messageInfo;

    memset(&messageInfo, 0, sizeof(messageInfo));
    messageInfo.mPeerAddr = mRequestAddr;
    messageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

    VerifyOrExit(mRequestTokenLength != 0, error = OT_ERROR_INVALID_STATE);

    message = otCoapNewMessage(GetInstancePtr(), nullptr);
    VerifyOrExit(message != nullptr, error = OT_ERROR_NO_BUFS);

    otCoapMessageInit(message, OT_COAP_TYPE_CONFIRMABLE, OT_COAP_CODE_GET);

    SuccessOrExit(error = otCoapMessageSetToken(message, mRequestToken, mRequestTokenLength));
    SuccessOrExit(error = otCoapMessageAppendObserveOption(message, 1));
    SuccessOrExit(error = otCoapMessageAppendUriPathOptions(message, mRequestUri));
    SuccessOrExit(error = otCoapSendRequest(GetInstancePtr(), message, &messageInfo, &Coap::HandleResponse, this));

    memset(&mRequestAddr, 0, sizeof(mRequestAddr));
    memset(&mRequestUri, 0, sizeof(mRequestUri));
    mRequestTokenLength = 0;

exit:

    if ((error != OT_ERROR_NONE) && (message != nullptr))
    {
        otMessageFree(message);
    }

    return error;
}

void Coap::CancelSubscriber(void)
{
    memset(&mSubscriberSock, 0, sizeof(mSubscriberSock));
    mSubscriberTokenLength = 0;
}
#endif // OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE

void Coap::PrintPayload(otMessage *aMessage)
{
    uint8_t  buf[kMaxBufferSize];
    uint16_t bytesToPrint;
    uint16_t bytesPrinted = 0;
    uint16_t length       = otMessageGetLength(aMessage) - otMessageGetOffset(aMessage);

    if (length > 0)
    {
        OutputFormat(" with payload: ");

        while (length > 0)
        {
            bytesToPrint = (length < sizeof(buf)) ? length : sizeof(buf);
            otMessageRead(aMessage, otMessageGetOffset(aMessage) + bytesPrinted, buf, bytesToPrint);

            OutputBytes(buf, static_cast<uint8_t>(bytesToPrint));

            length -= bytesToPrint;
            bytesPrinted += bytesToPrint;
        }
    }

    OutputLine("");
}

#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
otError Coap::ProcessCancel(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    return CancelResourceSubscription();
}
#endif

otError Coap::ProcessHelp(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    for (const Command &command : sCommands)
    {
        OutputLine(command.mName);
    }

    return OT_ERROR_NONE;
}

otError Coap::ProcessResource(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (!aArgs[0].IsEmpty())
    {
        VerifyOrExit(aArgs[0].GetLength() < kMaxUriLength, error = OT_ERROR_INVALID_ARGS);

        mResource.mUriPath = mUriPath;
        mResource.mContext = this;
        mResource.mHandler = &Coap::HandleRequest;

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        mResource.mReceiveHook  = &Coap::BlockwiseReceiveHook;
        mResource.mTransmitHook = &Coap::BlockwiseTransmitHook;

        if (!aArgs[1].IsEmpty())
        {
            SuccessOrExit(error = aArgs[1].ParseAsUint32(mBlockCount));
        }
#endif

        strncpy(mUriPath, aArgs[0].GetCString(), sizeof(mUriPath) - 1);

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        otCoapAddBlockWiseResource(GetInstancePtr(), &mResource);
#else
        otCoapAddResource(GetInstancePtr(), &mResource);
#endif
    }
    else
    {
        OutputLine("%s", mResource.mUriPath != nullptr ? mResource.mUriPath : "");
    }

exit:
    return error;
}

otError Coap::ProcessSet(Arg aArgs[])
{
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
    otMessage *   notificationMessage = nullptr;
    otMessageInfo messageInfo;
#endif
    otError error = OT_ERROR_NONE;

    if (!aArgs[0].IsEmpty())
    {
        VerifyOrExit(aArgs[0].GetLength() < sizeof(mResourceContent), error = OT_ERROR_INVALID_ARGS);
        strncpy(mResourceContent, aArgs[0].GetCString(), sizeof(mResourceContent));
        mResourceContent[sizeof(mResourceContent) - 1] = '\0';

#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
        if (mSubscriberTokenLength > 0)
        {
            // Notify the subscriber
            memset(&messageInfo, 0, sizeof(messageInfo));
            messageInfo.mPeerAddr = mSubscriberSock.mAddress;
            messageInfo.mPeerPort = mSubscriberSock.mPort;

            OutputFormat("sending coap notification to ");
            OutputIp6AddressLine(mSubscriberSock.mAddress);

            notificationMessage = otCoapNewMessage(GetInstancePtr(), nullptr);
            VerifyOrExit(notificationMessage != nullptr, error = OT_ERROR_NO_BUFS);

            otCoapMessageInit(
                notificationMessage,
                ((mSubscriberConfirmableNotifications) ? OT_COAP_TYPE_CONFIRMABLE : OT_COAP_TYPE_NON_CONFIRMABLE),
                OT_COAP_CODE_CONTENT);

            SuccessOrExit(error = otCoapMessageSetToken(notificationMessage, mSubscriberToken, mSubscriberTokenLength));
            SuccessOrExit(error = otCoapMessageAppendObserveOption(notificationMessage, mObserveSerial++));
            SuccessOrExit(error = otCoapMessageSetPayloadMarker(notificationMessage));
            SuccessOrExit(error = otMessageAppend(notificationMessage, mResourceContent,
                                                  static_cast<uint16_t>(strlen(mResourceContent))));

            SuccessOrExit(error = otCoapSendRequest(GetInstancePtr(), notificationMessage, &messageInfo,
                                                    &Coap::HandleNotificationResponse, this));
        }
#endif // OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
    }
    else
    {
        OutputLine("%s", mResourceContent);
    }

exit:

#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
    if ((error != OT_ERROR_NONE) && (notificationMessage != nullptr))
    {
        otMessageFree(notificationMessage);
    }
#endif

    return error;
}

otError Coap::ProcessStart(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    return otCoapStart(GetInstancePtr(), OT_DEFAULT_COAP_PORT);
}

otError Coap::ProcessStop(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    otCoapRemoveBlockWiseResource(GetInstancePtr(), &mResource);
#else
    otCoapRemoveResource(GetInstancePtr(), &mResource);
#endif

    return otCoapStop(GetInstancePtr());
}

otError Coap::ProcessParameters(Arg aArgs[])
{
    otError             error = OT_ERROR_NONE;
    bool *              defaultTxParameters;
    otCoapTxParameters *txParameters;

    if (aArgs[0] == "request")
    {
        txParameters        = &mRequestTxParameters;
        defaultTxParameters = &mUseDefaultRequestTxParameters;
    }
    else if (aArgs[0] == "response")
    {
        txParameters        = &mResponseTxParameters;
        defaultTxParameters = &mUseDefaultResponseTxParameters;
    }
    else
    {
        ExitNow(error = OT_ERROR_INVALID_ARGS);
    }

    if (!aArgs[1].IsEmpty())
    {
        if (aArgs[1] == "default")
        {
            *defaultTxParameters = true;
        }
        else
        {
            SuccessOrExit(error = aArgs[1].ParseAsUint32(txParameters->mAckTimeout));
            SuccessOrExit(error = aArgs[2].ParseAsUint8(txParameters->mAckRandomFactorNumerator));
            SuccessOrExit(error = aArgs[3].ParseAsUint8(txParameters->mAckRandomFactorDenominator));
            SuccessOrExit(error = aArgs[4].ParseAsUint8(txParameters->mMaxRetransmit));

            VerifyOrExit(txParameters->mAckRandomFactorNumerator > txParameters->mAckRandomFactorDenominator,
                         error = OT_ERROR_INVALID_ARGS);

            *defaultTxParameters = false;
        }
    }

    OutputLine("Transmission parameters for %s:", aArgs[0].GetCString());

    if (*defaultTxParameters)
    {
        OutputLine("default");
    }
    else
    {
        OutputLine("ACK_TIMEOUT=%u ms, ACK_RANDOM_FACTOR=%u/%u, MAX_RETRANSMIT=%u", txParameters->mAckTimeout,
                   txParameters->mAckRandomFactorNumerator, txParameters->mAckRandomFactorDenominator,
                   txParameters->mMaxRetransmit);
    }

exit:
    return error;
}

otError Coap::ProcessGet(Arg aArgs[])
{
    return ProcessRequest(aArgs, OT_COAP_CODE_GET);
}

otError Coap::ProcessPost(Arg aArgs[])
{
    return ProcessRequest(aArgs, OT_COAP_CODE_POST);
}

otError Coap::ProcessPut(Arg aArgs[])
{
    return ProcessRequest(aArgs, OT_COAP_CODE_PUT);
}

otError Coap::ProcessDelete(Arg aArgs[])
{
    return ProcessRequest(aArgs, OT_COAP_CODE_DELETE);
}

#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
otError Coap::ProcessObserve(Arg aArgs[])
{
    return ProcessRequest(aArgs, OT_COAP_CODE_GET, /* aCoapObserve */ true);
}
#endif

#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
otError Coap::ProcessRequest(Arg aArgs[], otCoapCode aCoapCode, bool aCoapObserve)
#else
otError Coap::ProcessRequest(Arg aArgs[], otCoapCode aCoapCode)
#endif
{
    otError       error   = OT_ERROR_NONE;
    otMessage *   message = nullptr;
    otMessageInfo messageInfo;
    uint16_t      payloadLength = 0;

    // Default parameters
    char         coapUri[kMaxUriLength] = "test";
    otCoapType   coapType               = OT_COAP_TYPE_NON_CONFIRMABLE;
    otIp6Address coapDestinationIp;
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    bool           coapBlock     = false;
    otCoapBlockSzx coapBlockSize = OT_COAP_OPTION_BLOCK_SZX_16;
    BlockType      coapBlockType = (aCoapCode == OT_COAP_CODE_GET) ? kBlockType2 : kBlockType1;
#endif

#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE && OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    if (aCoapObserve)
    {
        coapBlockType = kBlockType1;
    }
#endif

    SuccessOrExit(error = aArgs[0].ParseAsIp6Address(coapDestinationIp));

    VerifyOrExit(!aArgs[1].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(aArgs[1].GetLength() < sizeof(coapUri), error = OT_ERROR_INVALID_ARGS);
    strcpy(coapUri, aArgs[1].GetCString());

    // CoAP-Type
    if (!aArgs[2].IsEmpty())
    {
        if (aArgs[2] == "con")
        {
            coapType = OT_COAP_TYPE_CONFIRMABLE;
        }
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        else if (aArgs[2] == "block-16")
        {
            coapType      = OT_COAP_TYPE_CONFIRMABLE;
            coapBlock     = true;
            coapBlockSize = OT_COAP_OPTION_BLOCK_SZX_16;
        }
        else if (aArgs[2] == "block-32")
        {
            coapType      = OT_COAP_TYPE_CONFIRMABLE;
            coapBlock     = true;
            coapBlockSize = OT_COAP_OPTION_BLOCK_SZX_32;
        }
        else if (aArgs[2] == "block-64")
        {
            coapType      = OT_COAP_TYPE_CONFIRMABLE;
            coapBlock     = true;
            coapBlockSize = OT_COAP_OPTION_BLOCK_SZX_64;
        }
        else if (aArgs[2] == "block-128")
        {
            coapType      = OT_COAP_TYPE_CONFIRMABLE;
            coapBlock     = true;
            coapBlockSize = OT_COAP_OPTION_BLOCK_SZX_128;
        }
        else if (aArgs[2] == "block-256")
        {
            coapType      = OT_COAP_TYPE_CONFIRMABLE;
            coapBlock     = true;
            coapBlockSize = OT_COAP_OPTION_BLOCK_SZX_256;
        }
        else if (aArgs[2] == "block-512")
        {
            coapType      = OT_COAP_TYPE_CONFIRMABLE;
            coapBlock     = true;
            coapBlockSize = OT_COAP_OPTION_BLOCK_SZX_512;
        }
        else if (aArgs[2] == "block-1024")
        {
            coapType      = OT_COAP_TYPE_CONFIRMABLE;
            coapBlock     = true;
            coapBlockSize = OT_COAP_OPTION_BLOCK_SZX_1024;
        }
#endif // OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    }

#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
    if (aCoapObserve && mRequestTokenLength)
    {
        // New observe request, cancel any existing observation
        SuccessOrExit(error = CancelResourceSubscription());
    }
#endif

    message = otCoapNewMessage(GetInstancePtr(), nullptr);
    VerifyOrExit(message != nullptr, error = OT_ERROR_NO_BUFS);

    otCoapMessageInit(message, coapType, aCoapCode);
    otCoapMessageGenerateToken(message, OT_COAP_DEFAULT_TOKEN_LENGTH);

#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
    if (aCoapObserve)
    {
        SuccessOrExit(error = otCoapMessageAppendObserveOption(message, 0));
    }
#endif

    SuccessOrExit(error = otCoapMessageAppendUriPathOptions(message, coapUri));

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    if (coapBlock)
    {
        if (coapBlockType == kBlockType1)
        {
            SuccessOrExit(error = otCoapMessageAppendBlock1Option(message, 0, true, coapBlockSize));
        }
        else
        {
            SuccessOrExit(error = otCoapMessageAppendBlock2Option(message, 0, false, coapBlockSize));
        }
    }
#endif

    if (!aArgs[3].IsEmpty())
    {
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        if (coapBlock)
        {
            SuccessOrExit(error = aArgs[3].ParseAsUint32(mBlockCount));
        }
        else
        {
#endif
            payloadLength = aArgs[3].GetLength();

            if (payloadLength > 0)
            {
                SuccessOrExit(error = otCoapMessageSetPayloadMarker(message));
            }
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        }
#endif
    }

    // Embed content into message if given
    if (payloadLength > 0)
    {
        SuccessOrExit(error = otMessageAppend(message, aArgs[3].GetCString(), payloadLength));
    }

    memset(&messageInfo, 0, sizeof(messageInfo));
    messageInfo.mPeerAddr = coapDestinationIp;
    messageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
    if (aCoapObserve)
    {
        // Make a note of the message details for later so we can cancel it later.
        memcpy(&mRequestAddr, &coapDestinationIp, sizeof(mRequestAddr));
        mRequestTokenLength = otCoapMessageGetTokenLength(message);
        memcpy(mRequestToken, otCoapMessageGetToken(message), mRequestTokenLength);
        // Use `memcpy` instead of `strncpy` here because GCC will give warnings for `strncpy` when the dest's length is
        // not bigger than the src's length.
        memcpy(mRequestUri, coapUri, sizeof(mRequestUri) - 1);
    }
#endif

    if ((coapType == OT_COAP_TYPE_CONFIRMABLE) || (aCoapCode == OT_COAP_CODE_GET))
    {
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        if (coapBlock)
        {
            if (aCoapCode == OT_COAP_CODE_PUT || aCoapCode == OT_COAP_CODE_POST)
            {
                SuccessOrExit(error = otCoapMessageSetPayloadMarker(message));
            }
            error = otCoapSendRequestBlockWiseWithParameters(GetInstancePtr(), message, &messageInfo,
                                                             &Coap::HandleResponse, this, GetRequestTxParameters(),
                                                             Coap::BlockwiseTransmitHook, Coap::BlockwiseReceiveHook);
        }
        else
        {
#endif
            error = otCoapSendRequestWithParameters(GetInstancePtr(), message, &messageInfo, &Coap::HandleResponse,
                                                    this, GetRequestTxParameters());
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        }
#endif
    }
    else
    {
        error = otCoapSendRequestWithParameters(GetInstancePtr(), message, &messageInfo, nullptr, nullptr,
                                                GetResponseTxParameters());
    }

exit:

    if ((error != OT_ERROR_NONE) && (message != nullptr))
    {
        otMessageFree(message);
    }

    return error;
}

otError Coap::Process(Arg aArgs[])
{
    otError        error = OT_ERROR_INVALID_ARGS;
    const Command *command;

    if (aArgs[0].IsEmpty())
    {
        IgnoreError(ProcessHelp(aArgs));
        ExitNow();
    }

    command = Utils::LookupTable::Find(aArgs[0].GetCString(), sCommands);
    VerifyOrExit(command != nullptr, error = OT_ERROR_INVALID_COMMAND);

    error = (this->*command->mHandler)(aArgs + 1);

exit:
    return error;
}

void Coap::HandleRequest(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<Coap *>(aContext)->HandleRequest(aMessage, aMessageInfo);
}

void Coap::HandleRequest(otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    otError    error           = OT_ERROR_NONE;
    otMessage *responseMessage = nullptr;
    otCoapCode responseCode    = OT_COAP_CODE_EMPTY;
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
    uint64_t observe        = 0;
    bool     observePresent = false;
#endif
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    uint64_t blockValue   = 0;
    bool     blockPresent = false;
#endif
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE || OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
    otCoapOptionIterator iterator;
#endif

    OutputFormat("coap request from ");
    OutputIp6Address(aMessageInfo->mPeerAddr);
    OutputFormat(" ");

    switch (otCoapMessageGetCode(aMessage))
    {
    case OT_COAP_CODE_GET:
        OutputFormat("GET");
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE || OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
        SuccessOrExit(error = otCoapOptionIteratorInit(&iterator, aMessage));
#endif
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
        if (otCoapOptionIteratorGetFirstOptionMatching(&iterator, OT_COAP_OPTION_OBSERVE) != nullptr)
        {
            SuccessOrExit(error = otCoapOptionIteratorGetOptionUintValue(&iterator, &observe));
            observePresent = true;

            OutputFormat(" OBS=%lu", static_cast<uint32_t>(observe));
        }
#endif
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        if (otCoapOptionIteratorGetFirstOptionMatching(&iterator, OT_COAP_OPTION_BLOCK2) != nullptr)
        {
            SuccessOrExit(error = otCoapOptionIteratorGetOptionUintValue(&iterator, &blockValue));
            blockPresent = true;
        }
#endif
        break;

    case OT_COAP_CODE_DELETE:
        OutputFormat("DELETE");
        break;

    case OT_COAP_CODE_PUT:
        OutputFormat("PUT");
        break;

    case OT_COAP_CODE_POST:
        OutputFormat("POST");
        break;

    default:
        OutputLine("Undefined");
        ExitNow(error = OT_ERROR_PARSE);
    }

    PrintPayload(aMessage);

    if (otCoapMessageGetType(aMessage) == OT_COAP_TYPE_CONFIRMABLE ||
        otCoapMessageGetCode(aMessage) == OT_COAP_CODE_GET)
    {
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
        if (observePresent && (mSubscriberTokenLength > 0) && (observe == 0))
        {
            // There is already a subscriber
            responseCode = OT_COAP_CODE_SERVICE_UNAVAILABLE;
        }
        else
#endif
            if (otCoapMessageGetCode(aMessage) == OT_COAP_CODE_GET)
        {
            responseCode = OT_COAP_CODE_CONTENT;
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
            if (observePresent)
            {
                if (observe == 0)
                {
                    // New subscriber
                    OutputLine("Subscribing client");
                    mSubscriberSock.mAddress = aMessageInfo->mPeerAddr;
                    mSubscriberSock.mPort    = aMessageInfo->mPeerPort;
                    mSubscriberTokenLength   = otCoapMessageGetTokenLength(aMessage);
                    memcpy(mSubscriberToken, otCoapMessageGetToken(aMessage), mSubscriberTokenLength);

                    /*
                     * Implementer note.
                     *
                     * Here, we try to match a confirmable GET request with confirmable
                     * notifications, however this is not a requirement of RFC7641:
                     * the server can send notifications of either type regardless of
                     * what the client used to subscribe initially.
                     */
                    mSubscriberConfirmableNotifications = (otCoapMessageGetType(aMessage) == OT_COAP_TYPE_CONFIRMABLE);
                }
                else if (observe == 1)
                {
                    // See if it matches our subscriber token
                    if ((otCoapMessageGetTokenLength(aMessage) == mSubscriberTokenLength) &&
                        (memcmp(otCoapMessageGetToken(aMessage), mSubscriberToken, mSubscriberTokenLength) == 0))
                    {
                        // Unsubscribe request
                        CancelSubscriber();
                    }
                }
            }
#endif // OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
        }
        else
        {
            responseCode = OT_COAP_CODE_VALID;
        }

        responseMessage = otCoapNewMessage(GetInstancePtr(), nullptr);
        VerifyOrExit(responseMessage != nullptr, error = OT_ERROR_NO_BUFS);

        SuccessOrExit(
            error = otCoapMessageInitResponse(responseMessage, aMessage, OT_COAP_TYPE_ACKNOWLEDGMENT, responseCode));

        if (responseCode == OT_COAP_CODE_CONTENT)
        {
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
            if (observePresent && (observe == 0))
            {
                SuccessOrExit(error = otCoapMessageAppendObserveOption(responseMessage, mObserveSerial++));
            }
#endif
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
            if (blockPresent)
            {
                SuccessOrExit(error = otCoapMessageAppendBlock2Option(responseMessage,
                                                                      static_cast<uint32_t>(blockValue >> 4), true,
                                                                      static_cast<otCoapBlockSzx>(blockValue & 0x7)));
                SuccessOrExit(error = otCoapMessageSetPayloadMarker(responseMessage));
            }
            else
            {
#endif
                SuccessOrExit(error = otCoapMessageSetPayloadMarker(responseMessage));
                SuccessOrExit(error = otMessageAppend(responseMessage, mResourceContent,
                                                      static_cast<uint16_t>(strlen(mResourceContent))));
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
            }
#endif
        }

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        if (blockPresent)
        {
            SuccessOrExit(error = otCoapSendResponseBlockWiseWithParameters(GetInstancePtr(), responseMessage,
                                                                            aMessageInfo, GetResponseTxParameters(),
                                                                            this, mResource.mTransmitHook));
        }
        else
        {
#endif
            SuccessOrExit(error = otCoapSendResponseWithParameters(GetInstancePtr(), responseMessage, aMessageInfo,
                                                                   GetResponseTxParameters()));
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        }
#endif
    }

exit:

    if (error != OT_ERROR_NONE)
    {
        if (responseMessage != nullptr)
        {
            OutputLine("coap send response error %d: %s", error, otThreadErrorToString(error));
            otMessageFree(responseMessage);
        }
    }
    else if (responseCode >= OT_COAP_CODE_RESPONSE_MIN)
    {
        OutputLine("coap response sent");
    }
}

#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
void Coap::HandleNotificationResponse(void *               aContext,
                                      otMessage *          aMessage,
                                      const otMessageInfo *aMessageInfo,
                                      otError              aError)
{
    static_cast<Coap *>(aContext)->HandleNotificationResponse(aMessage, aMessageInfo, aError);
}

void Coap::HandleNotificationResponse(otMessage *aMessage, const otMessageInfo *aMessageInfo, otError aError)
{
    OT_UNUSED_VARIABLE(aMessage);

    switch (aError)
    {
    case OT_ERROR_NONE:
        if (aMessageInfo != nullptr)
        {
            OutputFormat("Received ACK in reply to notification from ");
            OutputIp6AddressLine(aMessageInfo->mPeerAddr);
        }
        break;

    default:
        OutputLine("coap receive notification response error %d: %s", aError, otThreadErrorToString(aError));
        CancelSubscriber();
        break;
    }
}
#endif // OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE

void Coap::HandleResponse(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo, otError aError)
{
    static_cast<Coap *>(aContext)->HandleResponse(aMessage, aMessageInfo, aError);
}

void Coap::HandleResponse(otMessage *aMessage, const otMessageInfo *aMessageInfo, otError aError)
{
    if (aError != OT_ERROR_NONE)
    {
        OutputLine("coap receive response error %d: %s", aError, otThreadErrorToString(aError));
    }
    else if ((aMessageInfo != nullptr) && (aMessage != nullptr))
    {
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
        otCoapOptionIterator iterator;
#endif

        OutputFormat("coap response from ");
        OutputIp6Address(aMessageInfo->mPeerAddr);

#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
        if (otCoapOptionIteratorInit(&iterator, aMessage) == OT_ERROR_NONE)
        {
            const otCoapOption *observeOpt =
                otCoapOptionIteratorGetFirstOptionMatching(&iterator, OT_COAP_OPTION_OBSERVE);

            if (observeOpt != nullptr)
            {
                uint64_t observeVal = 0;
                otError  error      = otCoapOptionIteratorGetOptionUintValue(&iterator, &observeVal);

                if (error == OT_ERROR_NONE)
                {
                    OutputFormat(" OBS=%u", observeVal);
                }
            }
        }
#endif
        PrintPayload(aMessage);
    }
}

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
otError Coap::BlockwiseReceiveHook(void *         aContext,
                                   const uint8_t *aBlock,
                                   uint32_t       aPosition,
                                   uint16_t       aBlockLength,
                                   bool           aMore,
                                   uint32_t       aTotalLength)
{
    return static_cast<Coap *>(aContext)->BlockwiseReceiveHook(aBlock, aPosition, aBlockLength, aMore, aTotalLength);
}

otError Coap::BlockwiseReceiveHook(const uint8_t *aBlock,
                                   uint32_t       aPosition,
                                   uint16_t       aBlockLength,
                                   bool           aMore,
                                   uint32_t       aTotalLength)
{
    OT_UNUSED_VARIABLE(aMore);
    OT_UNUSED_VARIABLE(aTotalLength);

    OutputLine("received block: Num %i Len %i", aPosition / aBlockLength, aBlockLength);

    for (uint16_t i = 0; i < aBlockLength / 16; i++)
    {
        OutputBytesLine(&aBlock[i * 16], 16);
    }

    return OT_ERROR_NONE;
}

otError Coap::BlockwiseTransmitHook(void *    aContext,
                                    uint8_t * aBlock,
                                    uint32_t  aPosition,
                                    uint16_t *aBlockLength,
                                    bool *    aMore)
{
    return static_cast<Coap *>(aContext)->BlockwiseTransmitHook(aBlock, aPosition, aBlockLength, aMore);
}

otError Coap::BlockwiseTransmitHook(uint8_t *aBlock, uint32_t aPosition, uint16_t *aBlockLength, bool *aMore)
{
    static uint32_t blockCount = 0;
    OT_UNUSED_VARIABLE(aPosition);

    // Send a random payload
    otRandomNonCryptoFillBuffer(aBlock, *aBlockLength);

    OutputLine("send block: Num %i Len %i", blockCount, *aBlockLength);

    for (uint16_t i = 0; i < *aBlockLength / 16; i++)
    {
        OutputBytesLine(&aBlock[i * 16], 16);
    }

    if (blockCount == mBlockCount - 1)
    {
        blockCount = 0;
        *aMore     = false;
    }
    else
    {
        *aMore = true;
        blockCount++;
    }

    return OT_ERROR_NONE;
}
#endif // OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE

} // namespace Cli
} // namespace ot

#endif // OPENTHREAD_CONFIG_COAP_API_ENABLE
