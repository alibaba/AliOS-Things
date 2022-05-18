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
 *   This file implements a simple CLI for the CoAP Secure service.
 */

#include "cli_coap_secure.hpp"

#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE

#include <mbedtls/debug.h>
#include <openthread/random_noncrypto.h>

#include "cli/cli.hpp"

// header for place your x509 certificate and private key
#include "x509_cert_key.hpp"

namespace ot {
namespace Cli {

constexpr CoapSecure::Command CoapSecure::sCommands[];

CoapSecure::CoapSecure(Output &aOutput)
    : OutputWrapper(aOutput)
    , mShutdownFlag(false)
    , mUseCertificate(false)
    , mPskLength(0)
    , mPskIdLength(0)
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    , mBlockCount(1)
#endif
{
    memset(&mResource, 0, sizeof(mResource));
    memset(&mPsk, 0, sizeof(mPsk));
    memset(&mPskId, 0, sizeof(mPskId));
    memset(&mUriPath, 0, sizeof(mUriPath));
    strncpy(mResourceContent, "0", sizeof(mResourceContent));
    mResourceContent[sizeof(mResourceContent) - 1] = '\0';
}

void CoapSecure::PrintPayload(otMessage *aMessage)
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

otError CoapSecure::ProcessHelp(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    for (const Command &command : sCommands)
    {
        OutputLine(command.mName);
    }

    return OT_ERROR_NONE;
}

otError CoapSecure::ProcessResource(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (!aArgs[0].IsEmpty())
    {
        VerifyOrExit(aArgs[0].GetLength() < kMaxUriLength, error = OT_ERROR_INVALID_ARGS);

        mResource.mUriPath = mUriPath;
        mResource.mContext = this;
        mResource.mHandler = &CoapSecure::HandleRequest;

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        mResource.mReceiveHook  = &CoapSecure::BlockwiseReceiveHook;
        mResource.mTransmitHook = &CoapSecure::BlockwiseTransmitHook;

        if (!aArgs[1].IsEmpty())
        {
            SuccessOrExit(error = aArgs[1].ParseAsUint32(mBlockCount));
        }
#endif

        strncpy(mUriPath, aArgs[0].GetCString(), sizeof(mUriPath) - 1);
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        otCoapSecureAddBlockWiseResource(GetInstancePtr(), &mResource);
#else
        otCoapSecureAddResource(GetInstancePtr(), &mResource);
#endif
    }
    else
    {
        OutputLine("%s", mResource.mUriPath != nullptr ? mResource.mUriPath : "");
    }

exit:
    return error;
}

otError CoapSecure::ProcessSet(Arg aArgs[])
{
    otError error = OT_ERROR_NONE;

    if (!aArgs[0].IsEmpty())
    {
        VerifyOrExit(aArgs[0].GetLength() < sizeof(mResourceContent), error = OT_ERROR_INVALID_ARGS);
        strncpy(mResourceContent, aArgs[0].GetCString(), sizeof(mResourceContent));
        mResourceContent[sizeof(mResourceContent) - 1] = '\0';
    }
    else
    {
        OutputLine("%s", mResourceContent);
    }

exit:
    return error;
}

otError CoapSecure::ProcessStart(Arg aArgs[])
{
    otError error          = OT_ERROR_NONE;
    bool    verifyPeerCert = true;

    if (!aArgs[0].IsEmpty())
    {
        if (aArgs[0] == "false")
        {
            verifyPeerCert = false;
        }
        else
        {
            VerifyOrExit(aArgs[0] == "true", error = OT_ERROR_INVALID_ARGS);
        }
    }

    otCoapSecureSetSslAuthMode(GetInstancePtr(), verifyPeerCert);
    otCoapSecureSetClientConnectedCallback(GetInstancePtr(), &CoapSecure::HandleConnected, this);

#if CLI_COAP_SECURE_USE_COAP_DEFAULT_HANDLER
    otCoapSecureSetDefaultHandler(GetInstancePtr(), &CoapSecure::DefaultHandler, this);
#endif

    error = otCoapSecureStart(GetInstancePtr(), OT_DEFAULT_COAP_SECURE_PORT);

exit:
    return error;
}

otError CoapSecure::ProcessStop(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    otCoapRemoveBlockWiseResource(GetInstancePtr(), &mResource);
#else
    otCoapRemoveResource(GetInstancePtr(), &mResource);
#endif

    if (otCoapSecureIsConnectionActive(GetInstancePtr()))
    {
        otCoapSecureDisconnect(GetInstancePtr());
        mShutdownFlag = true;
    }
    else
    {
        Stop();
    }

    return OT_ERROR_NONE;
}

otError CoapSecure::ProcessGet(Arg aArgs[])
{
    return ProcessRequest(aArgs, OT_COAP_CODE_GET);
}

otError CoapSecure::ProcessPost(Arg aArgs[])
{
    return ProcessRequest(aArgs, OT_COAP_CODE_POST);
}

otError CoapSecure::ProcessPut(Arg aArgs[])
{
    return ProcessRequest(aArgs, OT_COAP_CODE_PUT);
}

otError CoapSecure::ProcessDelete(Arg aArgs[])
{
    return ProcessRequest(aArgs, OT_COAP_CODE_DELETE);
}

otError CoapSecure::ProcessRequest(Arg aArgs[], otCoapCode aCoapCode)
{
    otError    error         = OT_ERROR_NONE;
    otMessage *message       = nullptr;
    uint16_t   payloadLength = 0;

    // Default parameters
    char       coapUri[kMaxUriLength];
    otCoapType coapType = OT_COAP_TYPE_NON_CONFIRMABLE;
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    bool           coapBlock     = false;
    otCoapBlockSzx coapBlockSize = OT_COAP_OPTION_BLOCK_SZX_16;
    BlockType      coapBlockType = (aCoapCode == OT_COAP_CODE_GET) ? kBlockType2 : kBlockType1;
#endif

    VerifyOrExit(!aArgs[0].IsEmpty(), error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(aArgs[0].GetLength() < sizeof(coapUri), error = OT_ERROR_INVALID_ARGS);
    strcpy(coapUri, aArgs[0].GetCString());

    if (!aArgs[1].IsEmpty())
    {
        if (aArgs[1] == "con")
        {
            coapType = OT_COAP_TYPE_CONFIRMABLE;
        }
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        else if (aArgs[1] == "block-16")
        {
            coapType      = OT_COAP_TYPE_CONFIRMABLE;
            coapBlock     = true;
            coapBlockSize = OT_COAP_OPTION_BLOCK_SZX_16;
        }
        else if (aArgs[1] == "block-32")
        {
            coapType      = OT_COAP_TYPE_CONFIRMABLE;
            coapBlock     = true;
            coapBlockSize = OT_COAP_OPTION_BLOCK_SZX_32;
        }
        else if (aArgs[1] == "block-64")
        {
            coapType      = OT_COAP_TYPE_CONFIRMABLE;
            coapBlock     = true;
            coapBlockSize = OT_COAP_OPTION_BLOCK_SZX_64;
        }
        else if (aArgs[1] == "block-128")
        {
            coapType      = OT_COAP_TYPE_CONFIRMABLE;
            coapBlock     = true;
            coapBlockSize = OT_COAP_OPTION_BLOCK_SZX_128;
        }
        else if (aArgs[1] == "block-256")
        {
            coapType      = OT_COAP_TYPE_CONFIRMABLE;
            coapBlock     = true;
            coapBlockSize = OT_COAP_OPTION_BLOCK_SZX_256;
        }
        else if (aArgs[1] == "block-512")
        {
            coapType      = OT_COAP_TYPE_CONFIRMABLE;
            coapBlock     = true;
            coapBlockSize = OT_COAP_OPTION_BLOCK_SZX_512;
        }
        else if (aArgs[1] == "block-1024")
        {
            coapType      = OT_COAP_TYPE_CONFIRMABLE;
            coapBlock     = true;
            coapBlockSize = OT_COAP_OPTION_BLOCK_SZX_1024;
        }
#endif // OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    }

    message = otCoapNewMessage(GetInstancePtr(), nullptr);
    VerifyOrExit(message != nullptr, error = OT_ERROR_NO_BUFS);

    otCoapMessageInit(message, coapType, aCoapCode);
    otCoapMessageGenerateToken(message, OT_COAP_DEFAULT_TOKEN_LENGTH);
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

    if (!aArgs[2].IsEmpty())
    {
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        if (coapBlock)
        {
            SuccessOrExit(error = aArgs[2].ParseAsUint32(mBlockCount));
        }
        else
        {
#endif
            payloadLength = aArgs[2].GetLength();

            if (payloadLength > 0)
            {
                SuccessOrExit(error = otCoapMessageSetPayloadMarker(message));
            }
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        }
#endif
    }

    if (payloadLength > 0)
    {
        SuccessOrExit(error = otMessageAppend(message, aArgs[2].GetCString(), payloadLength));
    }

    if ((coapType == OT_COAP_TYPE_CONFIRMABLE) || (aCoapCode == OT_COAP_CODE_GET))
    {
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        if (coapBlock)
        {
            error =
                otCoapSecureSendRequestBlockWise(GetInstancePtr(), message, &CoapSecure::HandleResponse, this,
                                                 &CoapSecure::BlockwiseTransmitHook, &CoapSecure::BlockwiseReceiveHook);
        }
        else
        {
#endif
            error = otCoapSecureSendRequest(GetInstancePtr(), message, &CoapSecure::HandleResponse, this);
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        }
#endif
    }
    else
    {
        error = otCoapSecureSendRequest(GetInstancePtr(), message, nullptr, nullptr);
    }

exit:

    if ((error != OT_ERROR_NONE) && (message != nullptr))
    {
        otMessageFree(message);
    }

    return error;
}

otError CoapSecure::ProcessConnect(Arg aArgs[])
{
    otError    error;
    otSockAddr sockaddr;

    memset(&sockaddr, 0, sizeof(sockaddr));
    SuccessOrExit(error = aArgs[0].ParseAsIp6Address(sockaddr.mAddress));
    sockaddr.mPort = OT_DEFAULT_COAP_SECURE_PORT;

    if (!aArgs[1].IsEmpty())
    {
        SuccessOrExit(error = aArgs[1].ParseAsUint16(sockaddr.mPort));
    }

    SuccessOrExit(error = otCoapSecureConnect(GetInstancePtr(), &sockaddr, &CoapSecure::HandleConnected, this));

exit:
    return error;
}

otError CoapSecure::ProcessDisconnect(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    otCoapSecureDisconnect(GetInstancePtr());

    return OT_ERROR_NONE;
}

#ifdef MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
otError CoapSecure::ProcessPsk(Arg aArgs[])
{
    otError  error = OT_ERROR_NONE;
    uint16_t length;

    VerifyOrExit(!aArgs[1].IsEmpty(), error = OT_ERROR_INVALID_ARGS);

    length = aArgs[0].GetLength();
    VerifyOrExit(length <= sizeof(mPsk), error = OT_ERROR_INVALID_ARGS);
    mPskLength = static_cast<uint8_t>(length);
    memcpy(mPsk, aArgs[0].GetCString(), mPskLength);

    length = aArgs[1].GetLength();
    VerifyOrExit(length <= sizeof(mPskId), error = OT_ERROR_INVALID_ARGS);
    mPskIdLength = static_cast<uint8_t>(length);
    memcpy(mPskId, aArgs[1].GetCString(), mPskIdLength);

    otCoapSecureSetPsk(GetInstancePtr(), mPsk, mPskLength, mPskId, mPskIdLength);
    mUseCertificate = false;

exit:
    return error;
}
#endif // MBEDTLS_KEY_EXCHANGE_PSK_ENABLED

#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
otError CoapSecure::ProcessX509(Arg aArgs[])
{
    OT_UNUSED_VARIABLE(aArgs);

    otCoapSecureSetCertificate(GetInstancePtr(), reinterpret_cast<const uint8_t *>(OT_CLI_COAPS_X509_CERT),
                               sizeof(OT_CLI_COAPS_X509_CERT), reinterpret_cast<const uint8_t *>(OT_CLI_COAPS_PRIV_KEY),
                               sizeof(OT_CLI_COAPS_PRIV_KEY));

    otCoapSecureSetCaCertificateChain(GetInstancePtr(),
                                      reinterpret_cast<const uint8_t *>(OT_CLI_COAPS_TRUSTED_ROOT_CERTIFICATE),
                                      sizeof(OT_CLI_COAPS_TRUSTED_ROOT_CERTIFICATE));
    mUseCertificate = true;

    return OT_ERROR_NONE;
}
#endif

otError CoapSecure::Process(Arg aArgs[])
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

void CoapSecure::Stop(void)
{
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    otCoapRemoveBlockWiseResource(GetInstancePtr(), &mResource);
#else
    otCoapRemoveResource(GetInstancePtr(), &mResource);
#endif
    otCoapSecureStop(GetInstancePtr());
}

void CoapSecure::HandleConnected(bool aConnected, void *aContext)
{
    static_cast<CoapSecure *>(aContext)->HandleConnected(aConnected);
}

void CoapSecure::HandleConnected(bool aConnected)
{
    if (aConnected)
    {
        OutputLine("coaps connected");
    }
    else
    {
        OutputLine("coaps disconnected");

        if (mShutdownFlag)
        {
            Stop();
            mShutdownFlag = false;
        }
    }
}

void CoapSecure::HandleRequest(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<CoapSecure *>(aContext)->HandleRequest(aMessage, aMessageInfo);
}

void CoapSecure::HandleRequest(otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    otError    error           = OT_ERROR_NONE;
    otMessage *responseMessage = nullptr;
    otCoapCode responseCode    = OT_COAP_CODE_EMPTY;
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    uint64_t             blockValue   = 0;
    bool                 blockPresent = false;
    otCoapOptionIterator iterator;
#endif

    OutputFormat("coaps request from ");
    OutputIp6Address(aMessageInfo->mPeerAddr);
    OutputFormat(" ");

    switch (otCoapMessageGetCode(aMessage))
    {
    case OT_COAP_CODE_GET:
        OutputFormat("GET");
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        SuccessOrExit(error = otCoapOptionIteratorInit(&iterator, aMessage));
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
        return;
    }

    PrintPayload(aMessage);

    if ((otCoapMessageGetType(aMessage) == OT_COAP_TYPE_CONFIRMABLE) ||
        (otCoapMessageGetCode(aMessage) == OT_COAP_CODE_GET))
    {
        if (otCoapMessageGetCode(aMessage) == OT_COAP_CODE_GET)
        {
            responseCode = OT_COAP_CODE_CONTENT;
        }
        else
        {
            responseCode = OT_COAP_CODE_VALID;
        }

        responseMessage = otCoapNewMessage(GetInstancePtr(), nullptr);
        VerifyOrExit(responseMessage != nullptr, error = OT_ERROR_NO_BUFS);

        SuccessOrExit(
            error = otCoapMessageInitResponse(responseMessage, aMessage, OT_COAP_TYPE_ACKNOWLEDGMENT, responseCode));

        if (otCoapMessageGetCode(aMessage) == OT_COAP_CODE_GET)
        {
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
            if (blockPresent)
            {
                SuccessOrExit(error = otCoapMessageAppendBlock2Option(responseMessage,
                                                                      static_cast<uint32_t>(blockValue >> 4), true,
                                                                      static_cast<otCoapBlockSzx>(blockValue & 0x7)));
            }
#endif
            SuccessOrExit(error = otCoapMessageSetPayloadMarker(responseMessage));
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
            if (!blockPresent)
            {
#endif
                SuccessOrExit(error = otMessageAppend(responseMessage, &mResourceContent,
                                                      static_cast<uint16_t>(strlen(mResourceContent))));
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
            }
#endif
        }

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        if (blockPresent)
        {
            SuccessOrExit(error = otCoapSecureSendResponseBlockWise(GetInstancePtr(), responseMessage, aMessageInfo,
                                                                    this, mResource.mTransmitHook));
        }
        else
        {
#endif
            SuccessOrExit(error = otCoapSecureSendResponse(GetInstancePtr(), responseMessage, aMessageInfo));
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        }
#endif
    }

exit:

    if (error != OT_ERROR_NONE)
    {
        if (responseMessage != nullptr)
        {
            OutputLine("coaps send response error %d: %s", error, otThreadErrorToString(error));
            otMessageFree(responseMessage);
        }
    }
    else if (responseCode >= OT_COAP_CODE_RESPONSE_MIN)
    {
        OutputLine("coaps response sent");
    }
}

void CoapSecure::HandleResponse(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo, otError aError)
{
    static_cast<CoapSecure *>(aContext)->HandleResponse(aMessage, aMessageInfo, aError);
}

void CoapSecure::HandleResponse(otMessage *aMessage, const otMessageInfo *aMessageInfo, otError aError)
{
    OT_UNUSED_VARIABLE(aMessageInfo);

    if (aError != OT_ERROR_NONE)
    {
        OutputLine("coaps receive response error %d: %s", aError, otThreadErrorToString(aError));
    }
    else
    {
        OutputFormat("coaps response from ");
        OutputIp6Address(aMessageInfo->mPeerAddr);

        PrintPayload(aMessage);
    }
}

#if CLI_COAP_SECURE_USE_COAP_DEFAULT_HANDLER
void CoapSecure::DefaultHandler(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<CoapSecure *>(aContext)->DefaultHandler(aMessage, aMessageInfo);
}

void CoapSecure::DefaultHandler(otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    otError    error           = OT_ERROR_NONE;
    otMessage *responseMessage = nullptr;

    if ((otCoapMessageGetType(aMessage) == OT_COAP_TYPE_CONFIRMABLE) ||
        (otCoapMessageGetCode(aMessage) == OT_COAP_CODE_GET))
    {
        responseMessage = otCoapNewMessage(GetInstancePtr(), nullptr);
        VerifyOrExit(responseMessage != nullptr, error = OT_ERROR_NO_BUFS);

        SuccessOrExit(error = otCoapMessageInitResponse(responseMessage, aMessage, OT_COAP_TYPE_NON_CONFIRMABLE,
                                                        OT_COAP_CODE_NOT_FOUND));

        SuccessOrExit(error = otCoapSecureSendResponse(GetInstancePtr(), responseMessage, aMessageInfo));
    }

exit:
    if (error != OT_ERROR_NONE && responseMessage != nullptr)
    {
        otMessageFree(responseMessage);
    }
}
#endif // CLI_COAP_SECURE_USE_COAP_DEFAULT_HANDLER

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
otError CoapSecure::BlockwiseReceiveHook(void *         aContext,
                                         const uint8_t *aBlock,
                                         uint32_t       aPosition,
                                         uint16_t       aBlockLength,
                                         bool           aMore,
                                         uint32_t       aTotalLength)
{
    return static_cast<CoapSecure *>(aContext)->BlockwiseReceiveHook(aBlock, aPosition, aBlockLength, aMore,
                                                                     aTotalLength);
}

otError CoapSecure::BlockwiseReceiveHook(const uint8_t *aBlock,
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

otError CoapSecure::BlockwiseTransmitHook(void *    aContext,
                                          uint8_t * aBlock,
                                          uint32_t  aPosition,
                                          uint16_t *aBlockLength,
                                          bool *    aMore)
{
    return static_cast<CoapSecure *>(aContext)->BlockwiseTransmitHook(aBlock, aPosition, aBlockLength, aMore);
}

otError CoapSecure::BlockwiseTransmitHook(uint8_t *aBlock, uint32_t aPosition, uint16_t *aBlockLength, bool *aMore)
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

#endif // OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
