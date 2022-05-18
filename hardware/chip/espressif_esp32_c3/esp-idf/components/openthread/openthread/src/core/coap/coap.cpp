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

#include "coap.hpp"

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/random.hpp"
#include "net/ip6.hpp"
#include "net/udp6.hpp"
#include "thread/thread_netif.hpp"

/**
 * @file
 *   This file contains common code base for CoAP client and server.
 */

namespace ot {
namespace Coap {

CoapBase::CoapBase(Instance &aInstance, Sender aSender)
    : InstanceLocator(aInstance)
    , mMessageId(Random::NonCrypto::GetUint16())
    , mRetransmissionTimer(aInstance, Coap::HandleRetransmissionTimer, this)
    , mContext(nullptr)
    , mInterceptor(nullptr)
    , mResponsesQueue(aInstance)
    , mDefaultHandler(nullptr)
    , mDefaultHandlerContext(nullptr)
    , mSender(aSender)
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    , mLastResponse(nullptr)
#endif
{
}

void CoapBase::ClearRequestsAndResponses(void)
{
    ClearRequests(nullptr); // Clear requests matching any address.
    mResponsesQueue.DequeueAllResponses();
}

void CoapBase::ClearRequests(const Ip6::Address &aAddress)
{
    ClearRequests(&aAddress);
}

void CoapBase::ClearRequests(const Ip6::Address *aAddress)
{
    Message *nextMessage;

    for (Message *message = mPendingRequests.GetHead(); message != nullptr; message = nextMessage)
    {
        Metadata metadata;

        nextMessage = message->GetNextCoapMessage();
        metadata.ReadFrom(*message);

        if ((aAddress == nullptr) || (metadata.mSourceAddress == *aAddress))
        {
            FinalizeCoapTransaction(*message, metadata, nullptr, nullptr, kErrorAbort);
        }
    }
}

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
void CoapBase::AddBlockWiseResource(ResourceBlockWise &aResource)
{
    IgnoreError(mBlockWiseResources.Add(aResource));
}

void CoapBase::RemoveBlockWiseResource(ResourceBlockWise &aResource)
{
    IgnoreError(mBlockWiseResources.Remove(aResource));
    aResource.SetNext(nullptr);
}
#endif

void CoapBase::AddResource(Resource &aResource)
{
    IgnoreError(mResources.Add(aResource));
}

void CoapBase::RemoveResource(Resource &aResource)
{
    IgnoreError(mResources.Remove(aResource));
    aResource.SetNext(nullptr);
}

void CoapBase::SetDefaultHandler(RequestHandler aHandler, void *aContext)
{
    mDefaultHandler        = aHandler;
    mDefaultHandlerContext = aContext;
}

void CoapBase::SetInterceptor(Interceptor aInterceptor, void *aContext)
{
    mInterceptor = aInterceptor;
    mContext     = aContext;
}

Message *CoapBase::NewMessage(const Message::Settings &aSettings)
{
    Message *message = nullptr;

    VerifyOrExit((message = static_cast<Message *>(Get<Ip6::Udp>().NewMessage(0, aSettings))) != nullptr);
    message->SetOffset(0);

exit:
    return message;
}

Error CoapBase::Send(ot::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    Error error;

#if OPENTHREAD_CONFIG_OTNS_ENABLE
    Get<Utils::Otns>().EmitCoapSend(static_cast<Message &>(aMessage), aMessageInfo);
#endif

    error = mSender(*this, aMessage, aMessageInfo);

#if OPENTHREAD_CONFIG_OTNS_ENABLE
    if (error != kErrorNone)
    {
        Get<Utils::Otns>().EmitCoapSendFailure(error, static_cast<Message &>(aMessage), aMessageInfo);
    }
#endif
    return error;
}

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
Error CoapBase::SendMessage(Message &                   aMessage,
                            const Ip6::MessageInfo &    aMessageInfo,
                            const TxParameters &        aTxParameters,
                            ResponseHandler             aHandler,
                            void *                      aContext,
                            otCoapBlockwiseTransmitHook aTransmitHook,
                            otCoapBlockwiseReceiveHook  aReceiveHook)
#else
Error CoapBase::SendMessage(Message &               aMessage,
                            const Ip6::MessageInfo &aMessageInfo,
                            const TxParameters &    aTxParameters,
                            ResponseHandler         aHandler,
                            void *                  aContext)
#endif
{
    Error    error;
    Message *storedCopy = nullptr;
    uint16_t copyLength = 0;
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    uint8_t  buf[kMaxBlockLength] = {0};
    uint16_t bufLen               = kMaxBlockLength;
    bool     moreBlocks           = false;
#endif

    switch (aMessage.GetType())
    {
    case kTypeAck:
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        // Check for block-wise transfer
        if ((aTransmitHook != nullptr) && (aMessage.ReadBlockOptionValues(kOptionBlock2) == kErrorNone) &&
            (aMessage.GetBlockWiseBlockNumber() == 0))
        {
            // Set payload for first block of the transfer
            VerifyOrExit((bufLen = otCoapBlockSizeFromExponent(aMessage.GetBlockWiseBlockSize())) <= kMaxBlockLength,
                         error = kErrorNoBufs);
            SuccessOrExit(error = aTransmitHook(aContext, buf, aMessage.GetBlockWiseBlockNumber() * bufLen, &bufLen,
                                                &moreBlocks));
            SuccessOrExit(error = aMessage.AppendBytes(buf, bufLen));

            SuccessOrExit(error = CacheLastBlockResponse(&aMessage));
        }
#endif

        mResponsesQueue.EnqueueResponse(aMessage, aMessageInfo, aTxParameters);
        break;
    case kTypeReset:
        OT_ASSERT(aMessage.GetCode() == kCodeEmpty);
        break;
    default:
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        // Check for block-wise transfer
        if ((aTransmitHook != nullptr) && (aMessage.ReadBlockOptionValues(kOptionBlock1) == kErrorNone) &&
            (aMessage.GetBlockWiseBlockNumber() == 0))
        {
            // Set payload for first block of the transfer
            VerifyOrExit((bufLen = otCoapBlockSizeFromExponent(aMessage.GetBlockWiseBlockSize())) <= kMaxBlockLength,
                         error = kErrorNoBufs);
            SuccessOrExit(error = aTransmitHook(aContext, buf, aMessage.GetBlockWiseBlockNumber() * bufLen, &bufLen,
                                                &moreBlocks));
            SuccessOrExit(error = aMessage.AppendBytes(buf, bufLen));

            // Block-Wise messages always have to be confirmable
            if (aMessage.IsNonConfirmable())
            {
                aMessage.SetType(kTypeConfirmable);
            }
        }
#endif

        aMessage.SetMessageId(mMessageId++);
        break;
    }

    aMessage.Finish();

    if (aMessage.IsConfirmable())
    {
        copyLength = aMessage.GetLength();
    }
    else if (aMessage.IsNonConfirmable() && (aHandler != nullptr))
    {
        // As we do not retransmit non confirmable messages, create a
        // copy of header only, for token information.
        copyLength = aMessage.GetOptionStart();
    }

    if (copyLength > 0)
    {
        Metadata metadata;

#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
        // Whether or not to turn on special "Observe" handling.
        Option::Iterator iterator;
        bool             observe;

        SuccessOrExit(error = iterator.Init(aMessage, kOptionObserve));
        observe = !iterator.IsDone();

        // Special case, if we're sending a GET with Observe=1, that is a cancellation.
        if (observe && aMessage.IsGetRequest())
        {
            uint64_t observeVal = 0;

            SuccessOrExit(error = iterator.ReadOptionValue(observeVal));

            if (observeVal == 1)
            {
                Metadata handlerMetadata;

                // We're cancelling our subscription, so disable special-case handling on this request.
                observe = false;

                // If we can find the previous handler context, cancel that too.  Peer address
                // and tokens, etc should all match.
                Message *origRequest = FindRelatedRequest(aMessage, aMessageInfo, handlerMetadata);
                if (origRequest != nullptr)
                {
                    FinalizeCoapTransaction(*origRequest, handlerMetadata, nullptr, nullptr, kErrorNone);
                }
            }
        }
#endif // OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE

        metadata.mSourceAddress            = aMessageInfo.GetSockAddr();
        metadata.mDestinationPort          = aMessageInfo.GetPeerPort();
        metadata.mDestinationAddress       = aMessageInfo.GetPeerAddr();
        metadata.mMulticastLoop            = aMessageInfo.GetMulticastLoop();
        metadata.mResponseHandler          = aHandler;
        metadata.mResponseContext          = aContext;
        metadata.mRetransmissionsRemaining = aTxParameters.mMaxRetransmit;
        metadata.mRetransmissionTimeout    = aTxParameters.CalculateInitialRetransmissionTimeout();
        metadata.mAcknowledged             = false;
        metadata.mConfirmable              = aMessage.IsConfirmable();
#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
        metadata.mHopLimit        = aMessageInfo.GetHopLimit();
        metadata.mIsHostInterface = aMessageInfo.IsHostInterface();
#endif
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        metadata.mBlockwiseReceiveHook  = aReceiveHook;
        metadata.mBlockwiseTransmitHook = aTransmitHook;
#endif
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
        metadata.mObserve = observe;
#endif
        metadata.mNextTimerShot =
            TimerMilli::GetNow() +
            (metadata.mConfirmable ? metadata.mRetransmissionTimeout : aTxParameters.CalculateMaxTransmitWait());

        storedCopy = CopyAndEnqueueMessage(aMessage, copyLength, metadata);
        VerifyOrExit(storedCopy != nullptr, error = kErrorNoBufs);
    }

    SuccessOrExit(error = Send(aMessage, aMessageInfo));

exit:

    if (error != kErrorNone && storedCopy != nullptr)
    {
        DequeueMessage(*storedCopy);
    }

    return error;
}

Error CoapBase::SendMessage(Message &               aMessage,
                            const Ip6::MessageInfo &aMessageInfo,
                            ResponseHandler         aHandler,
                            void *                  aContext)
{
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    return SendMessage(aMessage, aMessageInfo, TxParameters::GetDefault(), aHandler, aContext, nullptr, nullptr);
#else
    return SendMessage(aMessage, aMessageInfo, TxParameters::GetDefault(), aHandler, aContext);
#endif
}

Error CoapBase::SendReset(Message &aRequest, const Ip6::MessageInfo &aMessageInfo)
{
    return SendEmptyMessage(kTypeReset, aRequest, aMessageInfo);
}

Error CoapBase::SendAck(const Message &aRequest, const Ip6::MessageInfo &aMessageInfo)
{
    return SendEmptyMessage(kTypeAck, aRequest, aMessageInfo);
}

Error CoapBase::SendEmptyAck(const Message &aRequest, const Ip6::MessageInfo &aMessageInfo, Code aCode)
{
    return (aRequest.IsConfirmable() ? SendHeaderResponse(aCode, aRequest, aMessageInfo) : kErrorInvalidArgs);
}

Error CoapBase::SendNotFound(const Message &aRequest, const Ip6::MessageInfo &aMessageInfo)
{
    return SendHeaderResponse(kCodeNotFound, aRequest, aMessageInfo);
}

Error CoapBase::SendEmptyMessage(Type aType, const Message &aRequest, const Ip6::MessageInfo &aMessageInfo)
{
    Error    error   = kErrorNone;
    Message *message = nullptr;

    VerifyOrExit(aRequest.IsConfirmable(), error = kErrorInvalidArgs);

    VerifyOrExit((message = NewMessage()) != nullptr, error = kErrorNoBufs);

    message->Init(aType, kCodeEmpty);
    message->SetMessageId(aRequest.GetMessageId());

    message->Finish();
    SuccessOrExit(error = Send(*message, aMessageInfo));

exit:
    FreeMessageOnError(message, error);
    return error;
}

Error CoapBase::SendHeaderResponse(Message::Code aCode, const Message &aRequest, const Ip6::MessageInfo &aMessageInfo)
{
    Error    error   = kErrorNone;
    Message *message = nullptr;

    VerifyOrExit(aRequest.IsRequest(), error = kErrorInvalidArgs);
    VerifyOrExit((message = NewMessage()) != nullptr, error = kErrorNoBufs);

    switch (aRequest.GetType())
    {
    case kTypeConfirmable:
        message->Init(kTypeAck, aCode);
        message->SetMessageId(aRequest.GetMessageId());
        break;

    case kTypeNonConfirmable:
        message->Init(kTypeNonConfirmable, aCode);
        break;

    default:
        ExitNow(error = kErrorInvalidArgs);
        OT_UNREACHABLE_CODE(break);
    }

    SuccessOrExit(error = message->SetTokenFromMessage(aRequest));

    SuccessOrExit(error = SendMessage(*message, aMessageInfo));

exit:
    FreeMessageOnError(message, error);
    return error;
}

void CoapBase::HandleRetransmissionTimer(Timer &aTimer)
{
    static_cast<Coap *>(static_cast<TimerMilliContext &>(aTimer).GetContext())->HandleRetransmissionTimer();
}

void CoapBase::HandleRetransmissionTimer(void)
{
    TimeMilli        now      = TimerMilli::GetNow();
    TimeMilli        nextTime = now.GetDistantFuture();
    Metadata         metadata;
    Message *        nextMessage;
    Ip6::MessageInfo messageInfo;

    for (Message *message = mPendingRequests.GetHead(); message != nullptr; message = nextMessage)
    {
        nextMessage = message->GetNextCoapMessage();

        metadata.ReadFrom(*message);

        if (now >= metadata.mNextTimerShot)
        {
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
            if (message->IsRequest() && metadata.mObserve && metadata.mAcknowledged)
            {
                // This is a RFC7641 subscription.  Do not time out.
                continue;
            }
#endif

            if (!metadata.mConfirmable || (metadata.mRetransmissionsRemaining == 0))
            {
                // No expected response or acknowledgment.
                FinalizeCoapTransaction(*message, metadata, nullptr, nullptr, kErrorResponseTimeout);
                continue;
            }

            // Increment retransmission counter and timer.
            metadata.mRetransmissionsRemaining--;
            metadata.mRetransmissionTimeout *= 2;
            metadata.mNextTimerShot = now + metadata.mRetransmissionTimeout;
            metadata.UpdateIn(*message);

            // Retransmit
            if (!metadata.mAcknowledged)
            {
                messageInfo.SetPeerAddr(metadata.mDestinationAddress);
                messageInfo.SetPeerPort(metadata.mDestinationPort);
                messageInfo.SetSockAddr(metadata.mSourceAddress);
#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
                messageInfo.SetHopLimit(metadata.mHopLimit);
                messageInfo.SetIsHostInterface(metadata.mIsHostInterface);
#endif
                messageInfo.SetMulticastLoop(metadata.mMulticastLoop);

                SendCopy(*message, messageInfo);
            }
        }

        if (nextTime > metadata.mNextTimerShot)
        {
            nextTime = metadata.mNextTimerShot;
        }
    }

    if (nextTime < now.GetDistantFuture())
    {
        mRetransmissionTimer.FireAt(nextTime);
    }
}

void CoapBase::FinalizeCoapTransaction(Message &               aRequest,
                                       const Metadata &        aMetadata,
                                       Message *               aResponse,
                                       const Ip6::MessageInfo *aMessageInfo,
                                       Error                   aResult)
{
    DequeueMessage(aRequest);

    if (aMetadata.mResponseHandler != nullptr)
    {
        aMetadata.mResponseHandler(aMetadata.mResponseContext, aResponse, aMessageInfo, aResult);
    }
}

Error CoapBase::AbortTransaction(ResponseHandler aHandler, void *aContext)
{
    Error    error = kErrorNotFound;
    Message *nextMessage;
    Metadata metadata;

    for (Message *message = mPendingRequests.GetHead(); message != nullptr; message = nextMessage)
    {
        nextMessage = message->GetNextCoapMessage();
        metadata.ReadFrom(*message);

        if (metadata.mResponseHandler == aHandler && metadata.mResponseContext == aContext)
        {
            FinalizeCoapTransaction(*message, metadata, nullptr, nullptr, kErrorAbort);
            error = kErrorNone;
        }
    }

    return error;
}

Message *CoapBase::CopyAndEnqueueMessage(const Message &aMessage, uint16_t aCopyLength, const Metadata &aMetadata)
{
    Error    error       = kErrorNone;
    Message *messageCopy = nullptr;

    VerifyOrExit((messageCopy = aMessage.Clone(aCopyLength)) != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = aMetadata.AppendTo(*messageCopy));

    mRetransmissionTimer.FireAtIfEarlier(aMetadata.mNextTimerShot);

    mPendingRequests.Enqueue(*messageCopy);

exit:
    FreeAndNullMessageOnError(messageCopy, error);
    return messageCopy;
}

void CoapBase::DequeueMessage(Message &aMessage)
{
    mPendingRequests.Dequeue(aMessage);

    if (mRetransmissionTimer.IsRunning() && (mPendingRequests.GetHead() == nullptr))
    {
        mRetransmissionTimer.Stop();
    }

    aMessage.Free();

    // No need to worry that the earliest pending message was removed -
    // the timer would just shoot earlier and then it'd be setup again.
}

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
void CoapBase::FreeLastBlockResponse(void)
{
    if (mLastResponse != nullptr)
    {
        mLastResponse->Free();
        mLastResponse = nullptr;
    }
}

Error CoapBase::CacheLastBlockResponse(Message *aResponse)
{
    Error error = kErrorNone;
    // Save last response for block-wise transfer
    FreeLastBlockResponse();

    if ((mLastResponse = aResponse->Clone()) == nullptr)
    {
        error = kErrorNoBufs;
    }

    return error;
}

Error CoapBase::PrepareNextBlockRequest(Message::BlockType aType,
                                        bool               aMoreBlocks,
                                        Message &          aRequestOld,
                                        Message &          aRequest,
                                        Message &          aMessage)
{
    Error            error       = kErrorNone;
    bool             isOptionSet = false;
    uint64_t         optionBuf   = 0;
    uint16_t         blockOption = 0;
    Option::Iterator iterator;

    blockOption = (aType == Message::kBlockType1) ? kOptionBlock1 : kOptionBlock2;

    aRequest.Init(kTypeConfirmable, static_cast<ot::Coap::Code>(aRequestOld.GetCode()));
    SuccessOrExit(error = iterator.Init(aRequestOld));

    // Copy options from last response to next message
    for (; !iterator.IsDone() && iterator.GetOption()->GetLength() != 0; error = iterator.Advance())
    {
        uint16_t optionNumber = iterator.GetOption()->GetNumber();

        SuccessOrExit(error);

        // Check if option to copy next is higher than or equal to Block1 option
        if (optionNumber >= blockOption && !isOptionSet)
        {
            // Write Block1 option to next message
            SuccessOrExit(error = aRequest.AppendBlockOption(aType, aMessage.GetBlockWiseBlockNumber() + 1, aMoreBlocks,
                                                             aMessage.GetBlockWiseBlockSize()));
            aRequest.SetBlockWiseBlockNumber(aMessage.GetBlockWiseBlockNumber() + 1);
            aRequest.SetBlockWiseBlockSize(aMessage.GetBlockWiseBlockSize());
            aRequest.SetMoreBlocksFlag(aMoreBlocks);

            isOptionSet = true;

            // If option to copy next is Block1 or Block2 option, option is not copied
            if (optionNumber == kOptionBlock1 || optionNumber == kOptionBlock2)
            {
                continue;
            }
        }

        // Copy option
        SuccessOrExit(error = iterator.ReadOptionValue(&optionBuf));
        SuccessOrExit(error = aRequest.AppendOption(optionNumber, iterator.GetOption()->GetLength(), &optionBuf));
    }

    if (!isOptionSet)
    {
        // Write Block1 option to next message
        SuccessOrExit(error = aRequest.AppendBlockOption(aType, aMessage.GetBlockWiseBlockNumber() + 1, aMoreBlocks,
                                                         aMessage.GetBlockWiseBlockSize()));
        aRequest.SetBlockWiseBlockNumber(aMessage.GetBlockWiseBlockNumber() + 1);
        aRequest.SetBlockWiseBlockSize(aMessage.GetBlockWiseBlockSize());
        aRequest.SetMoreBlocksFlag(aMoreBlocks);
    }

exit:
    return error;
}

Error CoapBase::SendNextBlock1Request(Message &               aRequest,
                                      Message &               aMessage,
                                      const Ip6::MessageInfo &aMessageInfo,
                                      const Metadata &        aCoapMetadata)
{
    Error    error                = kErrorNone;
    Message *request              = nullptr;
    bool     moreBlocks           = false;
    uint8_t  buf[kMaxBlockLength] = {0};
    uint16_t bufLen               = kMaxBlockLength;

    SuccessOrExit(error = aRequest.ReadBlockOptionValues(kOptionBlock1));
    SuccessOrExit(error = aMessage.ReadBlockOptionValues(kOptionBlock1));

    // Conclude block-wise transfer if last block has been received
    if (!aRequest.IsMoreBlocksFlagSet())
    {
        FinalizeCoapTransaction(aRequest, aCoapMetadata, &aMessage, &aMessageInfo, kErrorNone);
        ExitNow();
    }

    // Get next block
    VerifyOrExit((bufLen = otCoapBlockSizeFromExponent(aMessage.GetBlockWiseBlockSize())) <= kMaxBlockLength,
                 error = kErrorNoBufs);

    SuccessOrExit(
        error = aCoapMetadata.mBlockwiseTransmitHook(aCoapMetadata.mResponseContext, buf,
                                                     otCoapBlockSizeFromExponent(aMessage.GetBlockWiseBlockSize()) *
                                                         (aMessage.GetBlockWiseBlockNumber() + 1),
                                                     &bufLen, &moreBlocks));

    // Check if block length is valid
    VerifyOrExit(bufLen <= otCoapBlockSizeFromExponent(aMessage.GetBlockWiseBlockSize()), error = kErrorInvalidArgs);

    // Init request for next block
    VerifyOrExit((request = NewMessage()) != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = PrepareNextBlockRequest(Message::kBlockType1, moreBlocks, aRequest, *request, aMessage));

    SuccessOrExit(error = request->SetPayloadMarker());

    SuccessOrExit(error = request->AppendBytes(buf, bufLen));

    DequeueMessage(aRequest);

    otLogInfoCoap("Send Block1 Nr. %d, Size: %d bytes, More Blocks Flag: %d", request->GetBlockWiseBlockNumber(),
                  otCoapBlockSizeFromExponent(request->GetBlockWiseBlockSize()), request->IsMoreBlocksFlagSet());

    SuccessOrExit(error = SendMessage(*request, aMessageInfo, TxParameters::GetDefault(),
                                      aCoapMetadata.mResponseHandler, aCoapMetadata.mResponseContext,
                                      aCoapMetadata.mBlockwiseTransmitHook, aCoapMetadata.mBlockwiseReceiveHook));

exit:
    FreeMessageOnError(request, error);

    return error;
}

Error CoapBase::SendNextBlock2Request(Message &               aRequest,
                                      Message &               aMessage,
                                      const Ip6::MessageInfo &aMessageInfo,
                                      const Metadata &        aCoapMetadata,
                                      uint32_t                aTotalLength,
                                      bool                    aBeginBlock1Transfer)
{
    Error    error                = kErrorNone;
    Message *request              = nullptr;
    uint8_t  buf[kMaxBlockLength] = {0};
    uint16_t bufLen               = kMaxBlockLength;

    SuccessOrExit(error = aMessage.ReadBlockOptionValues(kOptionBlock2));

    // Check payload and block length
    VerifyOrExit((aMessage.GetLength() - aMessage.GetOffset()) <=
                         otCoapBlockSizeFromExponent(aMessage.GetBlockWiseBlockSize()) &&
                     (aMessage.GetLength() - aMessage.GetOffset()) <= kMaxBlockLength,
                 error = kErrorNoBufs);

    // Read and then forward payload to receive hook function
    bufLen = aMessage.ReadBytes(aMessage.GetOffset(), buf, aMessage.GetLength() - aMessage.GetOffset());
    SuccessOrExit(
        error = aCoapMetadata.mBlockwiseReceiveHook(aCoapMetadata.mResponseContext, buf,
                                                    otCoapBlockSizeFromExponent(aMessage.GetBlockWiseBlockSize()) *
                                                        aMessage.GetBlockWiseBlockNumber(),
                                                    bufLen, aMessage.IsMoreBlocksFlagSet(), aTotalLength));

    // CoAP Block-Wise Transfer continues
    otLogInfoCoap("Received Block2 Nr. %d , Size: %d bytes, More Blocks Flag: %d", aMessage.GetBlockWiseBlockNumber(),
                  otCoapBlockSizeFromExponent(aMessage.GetBlockWiseBlockSize()), aMessage.IsMoreBlocksFlagSet());

    // Conclude block-wise transfer if last block has been received
    if (!aMessage.IsMoreBlocksFlagSet())
    {
        FinalizeCoapTransaction(aRequest, aCoapMetadata, &aMessage, &aMessageInfo, kErrorNone);
        ExitNow();
    }

    // Init request for next block
    VerifyOrExit((request = NewMessage()) != nullptr, error = kErrorNoBufs);
    SuccessOrExit(error = PrepareNextBlockRequest(Message::kBlockType2, aMessage.IsMoreBlocksFlagSet(), aRequest,
                                                  *request, aMessage));

    if (!aBeginBlock1Transfer)
    {
        DequeueMessage(aRequest);
    }

    otLogInfoCoap("Request Block2 Nr. %d, Size: %d bytes", request->GetBlockWiseBlockNumber(),
                  otCoapBlockSizeFromExponent(request->GetBlockWiseBlockSize()));

    SuccessOrExit(error =
                      SendMessage(*request, aMessageInfo, TxParameters::GetDefault(), aCoapMetadata.mResponseHandler,
                                  aCoapMetadata.mResponseContext, nullptr, aCoapMetadata.mBlockwiseReceiveHook));

exit:
    FreeMessageOnError(request, error);

    return error;
}

Error CoapBase::ProcessBlock1Request(Message &                aMessage,
                                     const Ip6::MessageInfo & aMessageInfo,
                                     const ResourceBlockWise &aResource,
                                     uint32_t                 aTotalLength)
{
    Error    error                = kErrorNone;
    Message *response             = nullptr;
    uint8_t  buf[kMaxBlockLength] = {0};
    uint16_t bufLen               = kMaxBlockLength;

    SuccessOrExit(error = aMessage.ReadBlockOptionValues(kOptionBlock1));

    // Read and then forward payload to receive hook function
    VerifyOrExit((aMessage.GetLength() - aMessage.GetOffset()) <= kMaxBlockLength, error = kErrorNoBufs);
    bufLen = aMessage.ReadBytes(aMessage.GetOffset(), buf, aMessage.GetLength() - aMessage.GetOffset());
    SuccessOrExit(error = aResource.HandleBlockReceive(buf,
                                                       otCoapBlockSizeFromExponent(aMessage.GetBlockWiseBlockSize()) *
                                                           aMessage.GetBlockWiseBlockNumber(),
                                                       bufLen, aMessage.IsMoreBlocksFlagSet(), aTotalLength));

    if (aMessage.IsMoreBlocksFlagSet())
    {
        // Set up next response
        VerifyOrExit((response = NewMessage()) != nullptr, error = kErrorFailed);
        response->Init(kTypeAck, kCodeContinue);
        response->SetMessageId(aMessage.GetMessageId());
        IgnoreReturnValue(
            response->SetToken(static_cast<const Message &>(aMessage).GetToken(), aMessage.GetTokenLength()));

        response->SetBlockWiseBlockNumber(aMessage.GetBlockWiseBlockNumber());
        response->SetMoreBlocksFlag(aMessage.IsMoreBlocksFlagSet());
        response->SetBlockWiseBlockSize(aMessage.GetBlockWiseBlockSize());

        SuccessOrExit(error = response->AppendBlockOption(Message::kBlockType1, response->GetBlockWiseBlockNumber(),
                                                          response->IsMoreBlocksFlagSet(),
                                                          response->GetBlockWiseBlockSize()));

        SuccessOrExit(error = CacheLastBlockResponse(response));

        otLogInfoCoap("Acknowledge Block1 Nr. %d, Size: %d bytes", response->GetBlockWiseBlockNumber(),
                      otCoapBlockSizeFromExponent(response->GetBlockWiseBlockSize()));

        SuccessOrExit(error = SendMessage(*response, aMessageInfo));

        error = kErrorBusy;
    }
    else
    {
        // Conclude block-wise transfer if last block has been received
        FreeLastBlockResponse();
        error = kErrorNone;
    }

exit:
    if (error != kErrorNone && error != kErrorBusy && response != nullptr)
    {
        response->Free();
    }

    return error;
}

Error CoapBase::ProcessBlock2Request(Message &                aMessage,
                                     const Ip6::MessageInfo & aMessageInfo,
                                     const ResourceBlockWise &aResource)
{
    Error            error                = kErrorNone;
    Message *        response             = nullptr;
    uint8_t          buf[kMaxBlockLength] = {0};
    uint16_t         bufLen               = kMaxBlockLength;
    bool             moreBlocks           = false;
    uint64_t         optionBuf            = 0;
    Option::Iterator iterator;

    SuccessOrExit(error = aMessage.ReadBlockOptionValues(kOptionBlock2));

    otLogInfoCoap("Request for Block2 Nr. %d, Size: %d bytes received", aMessage.GetBlockWiseBlockNumber(),
                  otCoapBlockSizeFromExponent(aMessage.GetBlockWiseBlockSize()));

    if (aMessage.GetBlockWiseBlockNumber() == 0)
    {
        aResource.HandleRequest(aMessage, aMessageInfo);
        ExitNow();
    }

    // Set up next response
    VerifyOrExit((response = NewMessage()) != nullptr, error = kErrorNoBufs);
    response->Init(kTypeAck, kCodeContent);
    response->SetMessageId(aMessage.GetMessageId());

    VerifyOrExit((bufLen = otCoapBlockSizeFromExponent(aMessage.GetBlockWiseBlockSize())) <= kMaxBlockLength,
                 error = kErrorNoBufs);
    SuccessOrExit(error = aResource.HandleBlockTransmit(buf,
                                                        otCoapBlockSizeFromExponent(aMessage.GetBlockWiseBlockSize()) *
                                                            aMessage.GetBlockWiseBlockNumber(),
                                                        &bufLen, &moreBlocks));

    response->SetMoreBlocksFlag(moreBlocks);
    if (moreBlocks)
    {
        switch (bufLen)
        {
        case 1024:
            response->SetBlockWiseBlockSize(OT_COAP_OPTION_BLOCK_SZX_1024);
            break;
        case 512:
            response->SetBlockWiseBlockSize(OT_COAP_OPTION_BLOCK_SZX_512);
            break;
        case 256:
            response->SetBlockWiseBlockSize(OT_COAP_OPTION_BLOCK_SZX_256);
            break;
        case 128:
            response->SetBlockWiseBlockSize(OT_COAP_OPTION_BLOCK_SZX_128);
            break;
        case 64:
            response->SetBlockWiseBlockSize(OT_COAP_OPTION_BLOCK_SZX_64);
            break;
        case 32:
            response->SetBlockWiseBlockSize(OT_COAP_OPTION_BLOCK_SZX_32);
            break;
        case 16:
            response->SetBlockWiseBlockSize(OT_COAP_OPTION_BLOCK_SZX_16);
            break;
        default:
            error = kErrorInvalidArgs;
            ExitNow();
            break;
        }
    }
    else
    {
        // Verify that buffer length is not larger than requested block size
        VerifyOrExit(bufLen <= otCoapBlockSizeFromExponent(aMessage.GetBlockWiseBlockSize()),
                     error = kErrorInvalidArgs);
        response->SetBlockWiseBlockSize(aMessage.GetBlockWiseBlockSize());
    }

    response->SetBlockWiseBlockNumber(
        (otCoapBlockSizeFromExponent(aMessage.GetBlockWiseBlockSize()) * aMessage.GetBlockWiseBlockNumber()) /
        (otCoapBlockSizeFromExponent(response->GetBlockWiseBlockSize())));

    // Copy options from last response
    SuccessOrExit(error = iterator.Init(*mLastResponse));

    while (!iterator.IsDone())
    {
        uint16_t optionNumber = iterator.GetOption()->GetNumber();

        if (optionNumber == kOptionBlock2)
        {
            SuccessOrExit(error = response->AppendBlockOption(Message::kBlockType2, response->GetBlockWiseBlockNumber(),
                                                              response->IsMoreBlocksFlagSet(),
                                                              response->GetBlockWiseBlockSize()));
        }
        else if (optionNumber == kOptionBlock1)
        {
            SuccessOrExit(error = iterator.ReadOptionValue(&optionBuf));
            SuccessOrExit(error = response->AppendOption(optionNumber, iterator.GetOption()->GetLength(), &optionBuf));
        }

        SuccessOrExit(error = iterator.Advance());
    }

    SuccessOrExit(error = response->SetPayloadMarker());
    SuccessOrExit(error = response->AppendBytes(buf, bufLen));

    if (response->IsMoreBlocksFlagSet())
    {
        SuccessOrExit(error = CacheLastBlockResponse(response));
    }
    else
    {
        // Conclude block-wise transfer if last block has been received
        FreeLastBlockResponse();
    }

    otLogInfoCoap("Send Block2 Nr. %d, Size: %d bytes, More Blocks Flag %d", response->GetBlockWiseBlockNumber(),
                  otCoapBlockSizeFromExponent(response->GetBlockWiseBlockSize()), response->IsMoreBlocksFlagSet());

    SuccessOrExit(error = SendMessage(*response, aMessageInfo));

exit:
    FreeMessageOnError(response, error);

    return error;
}
#endif // OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE

void CoapBase::SendCopy(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    Error    error;
    Message *messageCopy = nullptr;

    // Create a message copy for lower layers.
    messageCopy = aMessage.Clone(aMessage.GetLength() - sizeof(Metadata));
    VerifyOrExit(messageCopy != nullptr, error = kErrorNoBufs);

    SuccessOrExit(error = Send(*messageCopy, aMessageInfo));

exit:

    if (error != kErrorNone)
    {
        otLogWarnCoap("Failed to send copy: %s", ErrorToString(error));
        FreeMessage(messageCopy);
    }
}

Message *CoapBase::FindRelatedRequest(const Message &         aResponse,
                                      const Ip6::MessageInfo &aMessageInfo,
                                      Metadata &              aMetadata)
{
    Message *message;

    for (message = mPendingRequests.GetHead(); message != nullptr; message = message->GetNextCoapMessage())
    {
        aMetadata.ReadFrom(*message);

        if (((aMetadata.mDestinationAddress == aMessageInfo.GetPeerAddr()) ||
             aMetadata.mDestinationAddress.IsMulticast() ||
             aMetadata.mDestinationAddress.GetIid().IsAnycastLocator()) &&
            (aMetadata.mDestinationPort == aMessageInfo.GetPeerPort()))
        {
            switch (aResponse.GetType())
            {
            case kTypeReset:
            case kTypeAck:
                if (aResponse.GetMessageId() == message->GetMessageId())
                {
                    ExitNow();
                }

                break;

            case kTypeConfirmable:
            case kTypeNonConfirmable:
                if (aResponse.IsTokenEqual(*message))
                {
                    ExitNow();
                }

                break;
            }
        }
    }

exit:
    return message;
}

void CoapBase::Receive(ot::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    Message &message = static_cast<Message &>(aMessage);

    if (message.ParseHeader() != kErrorNone)
    {
        otLogDebgCoap("Failed to parse CoAP header");

        if (!aMessageInfo.GetSockAddr().IsMulticast() && message.IsConfirmable())
        {
            IgnoreError(SendReset(message, aMessageInfo));
        }
    }
    else if (message.IsRequest())
    {
        ProcessReceivedRequest(message, aMessageInfo);
    }
    else
    {
        ProcessReceivedResponse(message, aMessageInfo);
    }

#if OPENTHREAD_CONFIG_OTNS_ENABLE
    Get<Utils::Otns>().EmitCoapReceive(message, aMessageInfo);
#endif
}

void CoapBase::ProcessReceivedResponse(Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    Metadata metadata;
    Message *request = nullptr;
    Error    error   = kErrorNone;
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
    bool responseObserve = false;
#endif
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    uint8_t  blockOptionType    = 0;
    uint32_t totalTransfereSize = 0;
#endif

    request = FindRelatedRequest(aMessage, aMessageInfo, metadata);
    VerifyOrExit(request != nullptr);

#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
    if (metadata.mObserve && request->IsRequest())
    {
        // We sent Observe in our request, see if we received Observe in the response too.
        Option::Iterator iterator;

        SuccessOrExit(error = iterator.Init(aMessage, kOptionObserve));
        responseObserve = !iterator.IsDone();
    }
#endif

    switch (aMessage.GetType())
    {
    case kTypeReset:
        if (aMessage.IsEmpty())
        {
            FinalizeCoapTransaction(*request, metadata, nullptr, nullptr, kErrorAbort);
        }

        // Silently ignore non-empty reset messages (RFC 7252, p. 4.2).
        break;

    case kTypeAck:
        if (aMessage.IsEmpty())
        {
            // Empty acknowledgment.
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
            if (metadata.mObserve && !request->IsRequest())
            {
                // This is the ACK to our RFC7641 notification.  There will be no
                // "separate" response so pass it back as if it were a piggy-backed
                // response so we can stop re-sending and the application can move on.
                FinalizeCoapTransaction(*request, metadata, &aMessage, &aMessageInfo, kErrorNone);
            }
            else
#endif
            {
                // This is not related to RFC7641 or the outgoing "request" was not a
                // notification.
                if (metadata.mConfirmable)
                {
                    metadata.mAcknowledged = true;
                    metadata.UpdateIn(*request);
                }

                // Remove the message if response is not expected, otherwise await
                // response.
                if (metadata.mResponseHandler == nullptr)
                {
                    DequeueMessage(*request);
                }
            }
        }
        else if (aMessage.IsResponse() && aMessage.IsTokenEqual(*request))
        {
            // Piggybacked response.  If there's an Observe option present in both
            // request and response, and we have a response handler; then we're
            // dealing with RFC7641 rules here.
            // (If there is no response handler, then we're wasting our time!)
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
            if (metadata.mObserve && responseObserve && (metadata.mResponseHandler != nullptr))
            {
                // This is a RFC7641 notification.  The request is *not* done!
                metadata.mResponseHandler(metadata.mResponseContext, &aMessage, &aMessageInfo, kErrorNone);

                // Consider the message acknowledged at this point.
                metadata.mAcknowledged = true;
                metadata.UpdateIn(*request);
            }
            else
#endif
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
            {
                if (metadata.mBlockwiseTransmitHook != nullptr || metadata.mBlockwiseReceiveHook != nullptr)
                {
                    // Search for CoAP Block-Wise Option [RFC7959]
                    Option::Iterator iterator;

                    SuccessOrExit(error = iterator.Init(aMessage));
                    while (!iterator.IsDone())
                    {
                        switch (iterator.GetOption()->GetNumber())
                        {
                        case kOptionBlock1:
                            blockOptionType += 1;
                            break;

                        case kOptionBlock2:
                            blockOptionType += 2;
                            break;

                        case kOptionSize2:
                            // ToDo: wait for method to read uint option values
                            totalTransfereSize = 0;
                            break;

                        default:
                            break;
                        }

                        SuccessOrExit(error = iterator.Advance());
                    }
                }
                switch (blockOptionType)
                {
                case 0:
                    // Piggybacked response.
                    FinalizeCoapTransaction(*request, metadata, &aMessage, &aMessageInfo, kErrorNone);
                    break;
                case 1: // Block1 option
                    if (aMessage.GetCode() == kCodeContinue && metadata.mBlockwiseTransmitHook != nullptr)
                    {
                        error = SendNextBlock1Request(*request, aMessage, aMessageInfo, metadata);
                    }

                    if (aMessage.GetCode() != kCodeContinue || metadata.mBlockwiseTransmitHook == nullptr ||
                        error != kErrorNone)
                    {
                        FinalizeCoapTransaction(*request, metadata, &aMessage, &aMessageInfo, error);
                    }
                    break;
                case 2: // Block2 option
                    if (aMessage.GetCode() < kCodeBadRequest && metadata.mBlockwiseReceiveHook != nullptr)
                    {
                        error = SendNextBlock2Request(*request, aMessage, aMessageInfo, metadata, totalTransfereSize,
                                                      false);
                    }

                    if (aMessage.GetCode() >= kCodeBadRequest || metadata.mBlockwiseReceiveHook == nullptr ||
                        error != kErrorNone)
                    {
                        FinalizeCoapTransaction(*request, metadata, &aMessage, &aMessageInfo, error);
                    }
                    break;
                case 3: // Block1 & Block2 option
                    if (aMessage.GetCode() < kCodeBadRequest && metadata.mBlockwiseReceiveHook != nullptr)
                    {
                        error =
                            SendNextBlock2Request(*request, aMessage, aMessageInfo, metadata, totalTransfereSize, true);
                    }

                    FinalizeCoapTransaction(*request, metadata, &aMessage, &aMessageInfo, error);
                    break;
                default:
                    error = kErrorAbort;
                    FinalizeCoapTransaction(*request, metadata, &aMessage, &aMessageInfo, error);
                    break;
                }
            }
#else  // OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
            {
                FinalizeCoapTransaction(*request, metadata, &aMessage, &aMessageInfo, kErrorNone);
            }
#endif // OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
        }

        // Silently ignore acknowledgments carrying requests (RFC 7252, p. 4.2)
        // or with no token match (RFC 7252, p. 5.3.2)
        break;

    case kTypeConfirmable:
        // Send empty ACK if it is a CON message.
        IgnoreError(SendAck(aMessage, aMessageInfo));

        OT_FALL_THROUGH;
        // Handling of RFC7641 and multicast is below.
    case kTypeNonConfirmable:
        // Separate response or observation notification.  If the request was to a multicast
        // address, OR both the request and response carry Observe options, then this is NOT
        // the final message, we may see multiples.
        if ((metadata.mResponseHandler != nullptr) && (metadata.mDestinationAddress.IsMulticast()
#if OPENTHREAD_CONFIG_COAP_OBSERVE_API_ENABLE
                                                       || (metadata.mObserve && responseObserve)
#endif
                                                           ))
        {
            // If multicast non-confirmable request, allow multiple responses
            metadata.mResponseHandler(metadata.mResponseContext, &aMessage, &aMessageInfo, kErrorNone);
        }
        else
        {
            FinalizeCoapTransaction(*request, metadata, &aMessage, &aMessageInfo, kErrorNone);
        }

        break;
    }

exit:

    if (error == kErrorNone && request == nullptr)
    {
        if (aMessage.IsConfirmable() || aMessage.IsNonConfirmable())
        {
            // Successfully parsed a header but no matching request was
            // found - reject the message by sending reset.
            IgnoreError(SendReset(aMessage, aMessageInfo));
        }
    }
}

void CoapBase::ProcessReceivedRequest(Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    char     uriPath[Message::kMaxReceivedUriPath + 1];
    Message *cachedResponse = nullptr;
    Error    error          = kErrorNotFound;
#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    Option::Iterator iterator;
    char *           curUriPath         = uriPath;
    uint8_t          blockOptionType    = 0;
    uint32_t         totalTransfereSize = 0;
#endif

    if (mInterceptor != nullptr)
    {
        SuccessOrExit(error = mInterceptor(aMessage, aMessageInfo, mContext));
    }

    switch (mResponsesQueue.GetMatchedResponseCopy(aMessage, aMessageInfo, &cachedResponse))
    {
    case kErrorNone:
        cachedResponse->Finish();
        error = Send(*cachedResponse, aMessageInfo);

        OT_FALL_THROUGH;

    case kErrorNoBufs:
        ExitNow();

    case kErrorNotFound:
    default:
        break;
    }

#if OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE
    SuccessOrExit(error = iterator.Init(aMessage));

    while (!iterator.IsDone())
    {
        switch (iterator.GetOption()->GetNumber())
        {
        case kOptionUriPath:
            if (curUriPath != uriPath)
            {
                *curUriPath++ = '/';
            }

            VerifyOrExit(curUriPath + iterator.GetOption()->GetLength() < OT_ARRAY_END(uriPath), error = kErrorParse);

            IgnoreError(iterator.ReadOptionValue(curUriPath));
            curUriPath += iterator.GetOption()->GetLength();
            break;

        case kOptionBlock1:
            blockOptionType += 1;
            break;

        case kOptionBlock2:
            blockOptionType += 2;
            break;

        case kOptionSize1:
            // ToDo: wait for method to read uint option values
            totalTransfereSize = 0;
            break;

        default:
            break;
        }

        SuccessOrExit(error = iterator.Advance());
    }

    curUriPath[0] = '\0';

    for (const ResourceBlockWise &resource : mBlockWiseResources)
    {
        if (strcmp(resource.GetUriPath(), uriPath) != 0)
        {
            continue;
        }

        if ((resource.mReceiveHook != nullptr || resource.mTransmitHook != nullptr) && blockOptionType != 0)
        {
            switch (blockOptionType)
            {
            case 1:
                if (resource.mReceiveHook != nullptr)
                {
                    switch (ProcessBlock1Request(aMessage, aMessageInfo, resource, totalTransfereSize))
                    {
                    case kErrorNone:
                        resource.HandleRequest(aMessage, aMessageInfo);
                        // Fall through
                    case kErrorBusy:
                        error = kErrorNone;
                        break;
                    case kErrorNoBufs:
                        IgnoreReturnValue(SendHeaderResponse(kCodeRequestTooLarge, aMessage, aMessageInfo));
                        error = kErrorDrop;
                        break;
                    case kErrorNoFrameReceived:
                        IgnoreReturnValue(SendHeaderResponse(kCodeRequestIncomplete, aMessage, aMessageInfo));
                        error = kErrorDrop;
                        break;
                    default:
                        IgnoreReturnValue(SendHeaderResponse(kCodeInternalError, aMessage, aMessageInfo));
                        error = kErrorDrop;
                        break;
                    }
                }
                break;
            case 2:
                if (resource.mTransmitHook != nullptr)
                {
                    if ((error = ProcessBlock2Request(aMessage, aMessageInfo, resource)) != kErrorNone)
                    {
                        IgnoreReturnValue(SendHeaderResponse(kCodeInternalError, aMessage, aMessageInfo));
                        error = kErrorDrop;
                    }
                }
                break;
            }
            ExitNow();
        }
        else
        {
            resource.HandleRequest(aMessage, aMessageInfo);
            error = kErrorNone;
            ExitNow();
        }
    }
#else
    SuccessOrExit(error = aMessage.ReadUriPathOptions(uriPath));
#endif // OPENTHREAD_CONFIG_COAP_BLOCKWISE_TRANSFER_ENABLE

    for (const Resource &resource : mResources)
    {
        if (strcmp(resource.mUriPath, uriPath) == 0)
        {
            resource.HandleRequest(aMessage, aMessageInfo);
            error = kErrorNone;
            ExitNow();
        }
    }

    if (mDefaultHandler)
    {
        mDefaultHandler(mDefaultHandlerContext, &aMessage, &aMessageInfo);
        error = kErrorNone;
    }

exit:

    if (error != kErrorNone)
    {
        otLogInfoCoap("Failed to process request: %s", ErrorToString(error));

        if (error == kErrorNotFound && !aMessageInfo.GetSockAddr().IsMulticast())
        {
            IgnoreError(SendNotFound(aMessage, aMessageInfo));
        }

        FreeMessage(cachedResponse);
    }
}

void CoapBase::Metadata::ReadFrom(const Message &aMessage)
{
    uint16_t length = aMessage.GetLength();

    OT_ASSERT(length >= sizeof(*this));
    IgnoreError(aMessage.Read(length - sizeof(*this), *this));
}

void CoapBase::Metadata::UpdateIn(Message &aMessage) const
{
    aMessage.Write(aMessage.GetLength() - sizeof(*this), *this);
}

ResponsesQueue::ResponsesQueue(Instance &aInstance)
    : mTimer(aInstance, ResponsesQueue::HandleTimer, this)
{
}

Error ResponsesQueue::GetMatchedResponseCopy(const Message &         aRequest,
                                             const Ip6::MessageInfo &aMessageInfo,
                                             Message **              aResponse)
{
    Error          error = kErrorNone;
    const Message *cacheResponse;

    cacheResponse = FindMatchedResponse(aRequest, aMessageInfo);
    VerifyOrExit(cacheResponse != nullptr, error = kErrorNotFound);

    *aResponse = cacheResponse->Clone(cacheResponse->GetLength() - sizeof(ResponseMetadata));
    VerifyOrExit(*aResponse != nullptr, error = kErrorNoBufs);

exit:
    return error;
}

const Message *ResponsesQueue::FindMatchedResponse(const Message &aRequest, const Ip6::MessageInfo &aMessageInfo) const
{
    Message *message;

    for (message = mQueue.GetHead(); message != nullptr; message = message->GetNextCoapMessage())
    {
        if (message->GetMessageId() == aRequest.GetMessageId())
        {
            ResponseMetadata metadata;

            metadata.ReadFrom(*message);

            if ((metadata.mMessageInfo.GetPeerPort() == aMessageInfo.GetPeerPort()) &&
                (metadata.mMessageInfo.GetPeerAddr() == aMessageInfo.GetPeerAddr()))
            {
                break;
            }
        }
    }

    return message;
}

void ResponsesQueue::EnqueueResponse(Message &               aMessage,
                                     const Ip6::MessageInfo &aMessageInfo,
                                     const TxParameters &    aTxParameters)
{
    Message *        responseCopy;
    ResponseMetadata metadata;

    metadata.mDequeueTime = TimerMilli::GetNow() + aTxParameters.CalculateExchangeLifetime();
    metadata.mMessageInfo = aMessageInfo;

    VerifyOrExit(FindMatchedResponse(aMessage, aMessageInfo) == nullptr);

    UpdateQueue();

    VerifyOrExit((responseCopy = aMessage.Clone()) != nullptr);

    VerifyOrExit(metadata.AppendTo(*responseCopy) == kErrorNone, responseCopy->Free());

    mQueue.Enqueue(*responseCopy);

    mTimer.FireAtIfEarlier(metadata.mDequeueTime);

exit:
    return;
}

void ResponsesQueue::UpdateQueue(void)
{
    uint16_t  msgCount    = 0;
    Message * earliestMsg = nullptr;
    TimeMilli earliestDequeueTime(0);

    // Check the number of messages in the queue and if number is at
    // `kMaxCachedResponses` remove the one with earliest dequeue
    // time.

    for (Message *message = mQueue.GetHead(); message != nullptr; message = message->GetNextCoapMessage())
    {
        ResponseMetadata metadata;

        metadata.ReadFrom(*message);

        if ((earliestMsg == nullptr) || (metadata.mDequeueTime < earliestDequeueTime))
        {
            earliestMsg         = message;
            earliestDequeueTime = metadata.mDequeueTime;
        }

        msgCount++;
    }

    if (msgCount >= kMaxCachedResponses)
    {
        DequeueResponse(*earliestMsg);
    }
}

void ResponsesQueue::DequeueResponse(Message &aMessage)
{
    mQueue.DequeueAndFree(aMessage);
}

void ResponsesQueue::DequeueAllResponses(void)
{
    mQueue.DequeueAndFreeAll();
}

void ResponsesQueue::HandleTimer(Timer &aTimer)
{
    static_cast<ResponsesQueue *>(static_cast<TimerMilliContext &>(aTimer).GetContext())->HandleTimer();
}

void ResponsesQueue::HandleTimer(void)
{
    TimeMilli now             = TimerMilli::GetNow();
    TimeMilli nextDequeueTime = now.GetDistantFuture();
    Message * nextMessage;

    for (Message *message = mQueue.GetHead(); message != nullptr; message = nextMessage)
    {
        ResponseMetadata metadata;

        nextMessage = message->GetNextCoapMessage();

        metadata.ReadFrom(*message);

        if (now >= metadata.mDequeueTime)
        {
            DequeueResponse(*message);
            continue;
        }

        if (metadata.mDequeueTime < nextDequeueTime)
        {
            nextDequeueTime = metadata.mDequeueTime;
        }
    }

    if (nextDequeueTime < now.GetDistantFuture())
    {
        mTimer.FireAt(nextDequeueTime);
    }
}

void ResponsesQueue::ResponseMetadata::ReadFrom(const Message &aMessage)
{
    uint16_t length = aMessage.GetLength();

    OT_ASSERT(length >= sizeof(*this));
    IgnoreError(aMessage.Read(length - sizeof(*this), *this));
}

/// Return product of @p aValueA and @p aValueB if no overflow otherwise 0.
static uint32_t Multiply(uint32_t aValueA, uint32_t aValueB)
{
    uint32_t result = 0;

    VerifyOrExit(aValueA);

    result = aValueA * aValueB;
    result = (result / aValueA == aValueB) ? result : 0;

exit:
    return result;
}

bool TxParameters::IsValid(void) const
{
    bool rval = false;

    if ((mAckRandomFactorDenominator > 0) && (mAckRandomFactorNumerator >= mAckRandomFactorDenominator) &&
        (mAckTimeout >= OT_COAP_MIN_ACK_TIMEOUT) && (mMaxRetransmit <= OT_COAP_MAX_RETRANSMIT))
    {
        // Calulate exchange lifetime step by step and verify no overflow.
        uint32_t tmp = Multiply(mAckTimeout, (1U << (mMaxRetransmit + 1)) - 1);

        tmp = Multiply(tmp, mAckRandomFactorNumerator);
        tmp /= mAckRandomFactorDenominator;

        rval = (tmp != 0 && (tmp + mAckTimeout + 2 * kDefaultMaxLatency) > tmp);
    }

    return rval;
}

uint32_t TxParameters::CalculateInitialRetransmissionTimeout(void) const
{
    return Random::NonCrypto::GetUint32InRange(
        mAckTimeout, mAckTimeout * mAckRandomFactorNumerator / mAckRandomFactorDenominator + 1);
}

uint32_t TxParameters::CalculateExchangeLifetime(void) const
{
    // Final `mAckTimeout` is to account for processing delay.
    return CalculateSpan(mMaxRetransmit) + 2 * kDefaultMaxLatency + mAckTimeout;
}

uint32_t TxParameters::CalculateMaxTransmitWait(void) const
{
    return CalculateSpan(mMaxRetransmit + 1);
}

uint32_t TxParameters::CalculateSpan(uint8_t aMaxRetx) const
{
    return static_cast<uint32_t>(mAckTimeout * ((1U << aMaxRetx) - 1) / mAckRandomFactorDenominator *
                                 mAckRandomFactorNumerator);
}

const otCoapTxParameters TxParameters::kDefaultTxParameters = {
    kDefaultAckTimeout,
    kDefaultAckRandomFactorNumerator,
    kDefaultAckRandomFactorDenominator,
    kDefaultMaxRetransmit,
};

Coap::Coap(Instance &aInstance)
    : CoapBase(aInstance, &Coap::Send)
    , mSocket(aInstance)
{
}

Error Coap::Start(uint16_t aPort, otNetifIdentifier aNetifIdentifier)
{
    Error error        = kErrorNone;
    bool  socketOpened = false;

    VerifyOrExit(!mSocket.IsBound());

    SuccessOrExit(error = mSocket.Open(&Coap::HandleUdpReceive, this));
    socketOpened = true;

    SuccessOrExit(error = mSocket.Bind(aPort, aNetifIdentifier));

exit:
    if (error != kErrorNone && socketOpened)
    {
        IgnoreError(mSocket.Close());
    }

    return error;
}

Error Coap::Stop(void)
{
    Error error = kErrorNone;

    VerifyOrExit(mSocket.IsBound());

    SuccessOrExit(error = mSocket.Close());
    ClearRequestsAndResponses();

exit:
    return error;
}

void Coap::HandleUdpReceive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    static_cast<Coap *>(aContext)->Receive(*static_cast<Message *>(aMessage),
                                           *static_cast<const Ip6::MessageInfo *>(aMessageInfo));
}

Error Coap::Send(CoapBase &aCoapBase, ot::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    return static_cast<Coap &>(aCoapBase).Send(aMessage, aMessageInfo);
}

Error Coap::Send(ot::Message &aMessage, const Ip6::MessageInfo &aMessageInfo)
{
    return mSocket.IsBound() ? mSocket.SendTo(aMessage, aMessageInfo) : kErrorInvalidState;
}

} // namespace Coap
} // namespace ot
