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
 *   This file implements the message buffer pool and message buffers.
 */

#include "message.hpp"

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "net/checksum.hpp"
#include "net/ip6.hpp"

#if OPENTHREAD_MTD || OPENTHREAD_FTD

#if OPENTHREAD_CONFIG_MESSAGE_USE_HEAP_ENABLE && OPENTHREAD_CONFIG_PLATFORM_MESSAGE_MANAGEMENT
#error "OPENTHREAD_CONFIG_MESSAGE_USE_HEAP_ENABLE conflicts with OPENTHREAD_CONFIG_PLATFORM_MESSAGE_MANAGEMENT."
#endif

#if OPENTHREAD_CONFIG_MESSAGE_USE_HEAP_ENABLE && !OPENTHREAD_CONFIG_DTLS_ENABLE
#error "OPENTHREAD_CONFIG_MESSAGE_USE_HEAP_ENABLE is strongly discouraged when OPENTHREAD_CONFIG_DTLS_ENABLE is off."
#endif

namespace ot {

//---------------------------------------------------------------------------------------------------------------------
// MessagePool

MessagePool::MessagePool(Instance &aInstance)
    : InstanceLocator(aInstance)
#if !OPENTHREAD_CONFIG_PLATFORM_MESSAGE_MANAGEMENT && !OPENTHREAD_CONFIG_MESSAGE_USE_HEAP_ENABLE
    , mNumFreeBuffers(kNumBuffers)
#endif
{
#if OPENTHREAD_CONFIG_PLATFORM_MESSAGE_MANAGEMENT
    otPlatMessagePoolInit(&GetInstance(), kNumBuffers, sizeof(Buffer));
#endif
}

Message *MessagePool::New(Message::Type aType, uint16_t aReserveHeader, Message::Priority aPriority)
{
    Error    error = kErrorNone;
    Message *message;

    VerifyOrExit((message = static_cast<Message *>(NewBuffer(aPriority))) != nullptr);

    memset(message, 0, sizeof(*message));
    message->SetMessagePool(this);
    message->SetType(aType);
    message->SetReserved(aReserveHeader);
    message->SetLinkSecurityEnabled(true);

    SuccessOrExit(error = message->SetPriority(aPriority));
    SuccessOrExit(error = message->SetLength(0));

exit:
    if (error != kErrorNone)
    {
        Free(message);
        message = nullptr;
    }

    return message;
}

Message *MessagePool::New(Message::Type aType, uint16_t aReserveHeader, const Message::Settings &aSettings)
{
    Message *message = New(aType, aReserveHeader, aSettings.GetPriority());

    if (message)
    {
        message->SetLinkSecurityEnabled(aSettings.IsLinkSecurityEnabled());
    }

    return message;
}

void MessagePool::Free(Message *aMessage)
{
    OT_ASSERT(aMessage->Next() == nullptr && aMessage->Prev() == nullptr);

    FreeBuffers(static_cast<Buffer *>(aMessage));
}

Buffer *MessagePool::NewBuffer(Message::Priority aPriority)
{
    Buffer *buffer = nullptr;

    while ((
#if OPENTHREAD_CONFIG_MESSAGE_USE_HEAP_ENABLE
               buffer = static_cast<Buffer *>(Instance::HeapCAlloc(1, sizeof(Buffer)))
#elif OPENTHREAD_CONFIG_PLATFORM_MESSAGE_MANAGEMENT
               buffer = static_cast<Buffer *>(otPlatMessagePoolNew(&GetInstance()))
#else
               buffer = mBufferPool.Allocate()
#endif
                   ) == nullptr)
    {
        SuccessOrExit(ReclaimBuffers(aPriority));
    }

#if !OPENTHREAD_CONFIG_PLATFORM_MESSAGE_MANAGEMENT && !OPENTHREAD_CONFIG_MESSAGE_USE_HEAP_ENABLE
    mNumFreeBuffers--;
#endif

    buffer->SetNextBuffer(nullptr);

exit:
    if (buffer == nullptr)
    {
        otLogInfoMem("No available message buffer");
    }

    return buffer;
}

void MessagePool::FreeBuffers(Buffer *aBuffer)
{
    while (aBuffer != nullptr)
    {
        Buffer *next = aBuffer->GetNextBuffer();
#if OPENTHREAD_CONFIG_MESSAGE_USE_HEAP_ENABLE
        Instance::HeapFree(aBuffer);
#elif OPENTHREAD_CONFIG_PLATFORM_MESSAGE_MANAGEMENT
        otPlatMessagePoolFree(&GetInstance(), aBuffer);
#else
        mBufferPool.Free(*aBuffer);
        mNumFreeBuffers++;
#endif
        aBuffer = next;
    }
}

Error MessagePool::ReclaimBuffers(Message::Priority aPriority)
{
    return Get<MeshForwarder>().EvictMessage(aPriority);
}

uint16_t MessagePool::GetFreeBufferCount(void) const
{
    uint16_t rval;

#if OPENTHREAD_CONFIG_MESSAGE_USE_HEAP_ENABLE
    rval = static_cast<uint16_t>(GetInstance().GetHeap().GetFreeSize() / sizeof(Buffer));
#elif OPENTHREAD_CONFIG_PLATFORM_MESSAGE_MANAGEMENT
    rval = otPlatMessagePoolNumFreeBuffers(&GetInstance());
#else
    rval = mNumFreeBuffers;
#endif

    return rval;
}

uint16_t MessagePool::GetTotalBufferCount(void) const
{
#if OPENTHREAD_CONFIG_MESSAGE_USE_HEAP_ENABLE
    return static_cast<uint16_t>(GetInstance().GetHeap().GetCapacity() / sizeof(Buffer));
#else
    return OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS;
#endif
}

//---------------------------------------------------------------------------------------------------------------------
// Message::Settings

const otMessageSettings Message::Settings::kDefault = {kWithLinkSecurity, kPriorityNormal};

Message::Settings::Settings(LinkSecurityMode aSecurityMode, Priority aPriority)
{
    mLinkSecurityEnabled = aSecurityMode;
    mPriority            = aPriority;
}

const Message::Settings &Message::Settings::From(const otMessageSettings *aSettings)
{
    return (aSettings == nullptr) ? GetDefault() : *static_cast<const Settings *>(aSettings);
}

//---------------------------------------------------------------------------------------------------------------------
// Message

Error Message::ResizeMessage(uint16_t aLength)
{
    // This method adds or frees message buffers to meet the
    // requested length.

    Error    error     = kErrorNone;
    Buffer * curBuffer = this;
    Buffer * lastBuffer;
    uint16_t curLength = kHeadBufferDataSize;

    while (curLength < aLength)
    {
        if (curBuffer->GetNextBuffer() == nullptr)
        {
            curBuffer->SetNextBuffer(GetMessagePool()->NewBuffer(GetPriority()));
            VerifyOrExit(curBuffer->GetNextBuffer() != nullptr, error = kErrorNoBufs);
        }

        curBuffer = curBuffer->GetNextBuffer();
        curLength += kBufferDataSize;
    }

    lastBuffer = curBuffer;
    curBuffer  = curBuffer->GetNextBuffer();
    lastBuffer->SetNextBuffer(nullptr);

    GetMessagePool()->FreeBuffers(curBuffer);

exit:
    return error;
}

void Message::Free(void)
{
    GetMessagePool()->Free(this);
}

Message *Message::GetNext(void) const
{
    Message *next;
    Message *tail;

    if (GetMetadata().mInPriorityQ)
    {
        PriorityQueue *priorityQueue = GetPriorityQueue();
        VerifyOrExit(priorityQueue != nullptr, next = nullptr);
        tail = priorityQueue->GetTail();
    }
    else
    {
        MessageQueue *messageQueue = GetMessageQueue();
        VerifyOrExit(messageQueue != nullptr, next = nullptr);
        tail = messageQueue->GetTail();
    }

    next = (this == tail) ? nullptr : Next();

exit:
    return next;
}

Error Message::SetLength(uint16_t aLength)
{
    Error    error              = kErrorNone;
    uint16_t totalLengthRequest = GetReserved() + aLength;

    VerifyOrExit(totalLengthRequest >= GetReserved(), error = kErrorInvalidArgs);

    SuccessOrExit(error = ResizeMessage(totalLengthRequest));
    GetMetadata().mLength = aLength;

    // Correct the offset in case shorter length is set.
    if (GetOffset() > aLength)
    {
        SetOffset(aLength);
    }

exit:
    return error;
}

uint8_t Message::GetBufferCount(void) const
{
    uint8_t rval = 1;

    for (const Buffer *curBuffer = GetNextBuffer(); curBuffer; curBuffer = curBuffer->GetNextBuffer())
    {
        rval++;
    }

    return rval;
}

void Message::MoveOffset(int aDelta)
{
    OT_ASSERT(GetOffset() + aDelta <= GetLength());
    GetMetadata().mOffset += static_cast<int16_t>(aDelta);
    OT_ASSERT(GetMetadata().mOffset <= GetLength());
}

void Message::SetOffset(uint16_t aOffset)
{
    OT_ASSERT(aOffset <= GetLength());
    GetMetadata().mOffset = aOffset;
}

bool Message::IsSubTypeMle(void) const
{
    bool rval;

    switch (GetMetadata().mSubType)
    {
    case kSubTypeMleGeneral:
    case kSubTypeMleAnnounce:
    case kSubTypeMleDiscoverRequest:
    case kSubTypeMleDiscoverResponse:
    case kSubTypeMleChildUpdateRequest:
    case kSubTypeMleDataResponse:
    case kSubTypeMleChildIdRequest:
        rval = true;
        break;

    default:
        rval = false;
        break;
    }

    return rval;
}

Error Message::SetPriority(Priority aPriority)
{
    Error          error    = kErrorNone;
    uint8_t        priority = static_cast<uint8_t>(aPriority);
    PriorityQueue *priorityQueue;

    VerifyOrExit(priority < kNumPriorities, error = kErrorInvalidArgs);

    VerifyOrExit(IsInAQueue(), GetMetadata().mPriority = priority);
    VerifyOrExit(GetMetadata().mPriority != priority);

    priorityQueue = GetPriorityQueue();

    if (priorityQueue != nullptr)
    {
        priorityQueue->Dequeue(*this);
    }

    GetMetadata().mPriority = priority;

    if (priorityQueue != nullptr)
    {
        priorityQueue->Enqueue(*this);
    }

exit:
    return error;
}

const char *Message::PriorityToString(Priority aPriority)
{
    static const char *const kPriorityStrings[] = {
        "low",    // (0) kPriorityLow
        "normal", // (1) kPriorityNormal
        "high",   // (2) kPriorityHigh
        "net",    // (3) kPriorityNet
    };

    static_assert(kPriorityLow == 0, "kPriorityLow value is incorrect");
    static_assert(kPriorityNormal == 1, "kPriorityNormal value is incorrect");
    static_assert(kPriorityHigh == 2, "kPriorityHigh value is incorrect");
    static_assert(kPriorityNet == 3, "kPriorityNet value is incorrect");

    return kPriorityStrings[aPriority];
}

Error Message::AppendBytes(const void *aBuf, uint16_t aLength)
{
    Error    error     = kErrorNone;
    uint16_t oldLength = GetLength();

    SuccessOrExit(error = SetLength(GetLength() + aLength));
    WriteBytes(oldLength, aBuf, aLength);

exit:
    return error;
}

Error Message::AppendBytesFromMessage(const Message &aMessage, uint16_t aOffset, uint16_t aLength)
{
    Error    error       = kErrorNone;
    uint16_t writeOffset = GetLength();
    Chunk    chunk;

    VerifyOrExit(aMessage.GetLength() >= aOffset + aLength, error = kErrorParse);
    SuccessOrExit(error = SetLength(GetLength() + aLength));

    aMessage.GetFirstChunk(aOffset, aLength, chunk);

    while (chunk.GetLength() > 0)
    {
        WriteBytes(writeOffset, chunk.GetBytes(), chunk.GetLength());
        writeOffset += chunk.GetLength();
        aMessage.GetNextChunk(aLength, chunk);
    }

exit:
    return error;
}

Error Message::PrependBytes(const void *aBuf, uint16_t aLength)
{
    Error   error     = kErrorNone;
    Buffer *newBuffer = nullptr;

    while (aLength > GetReserved())
    {
        VerifyOrExit((newBuffer = GetMessagePool()->NewBuffer(GetPriority())) != nullptr, error = kErrorNoBufs);

        newBuffer->SetNextBuffer(GetNextBuffer());
        SetNextBuffer(newBuffer);

        if (GetReserved() < sizeof(mBuffer.mHead.mData))
        {
            // Copy payload from the first buffer.
            memcpy(newBuffer->mBuffer.mHead.mData + GetReserved(), mBuffer.mHead.mData + GetReserved(),
                   sizeof(mBuffer.mHead.mData) - GetReserved());
        }

        SetReserved(GetReserved() + kBufferDataSize);
    }

    SetReserved(GetReserved() - aLength);
    GetMetadata().mLength += aLength;
    SetOffset(GetOffset() + aLength);

    if (aBuf != nullptr)
    {
        WriteBytes(0, aBuf, aLength);
    }

exit:
    return error;
}

void Message::RemoveHeader(uint16_t aLength)
{
    OT_ASSERT(aLength <= GetMetadata().mLength);

    GetMetadata().mReserved += aLength;
    GetMetadata().mLength -= aLength;

    if (GetMetadata().mOffset > aLength)
    {
        GetMetadata().mOffset -= aLength;
    }
    else
    {
        GetMetadata().mOffset = 0;
    }
}

void Message::GetFirstChunk(uint16_t aOffset, uint16_t &aLength, Chunk &aChunk) const
{
    // This method gets the first message chunk (contiguous data
    // buffer) corresponding to a given offset and length. On exit
    // `aChunk` is updated such that `aChunk.GetBytes()` gives the
    // pointer to the start of chunk and `aChunk.GetLength()` gives
    // its length. The `aLength` is also decreased by the chunk
    // length.

    VerifyOrExit(aOffset < GetLength(), aChunk.SetLength(0));

    if (aOffset + aLength >= GetLength())
    {
        aLength = GetLength() - aOffset;
    }

    aOffset += GetReserved();

    aChunk.SetBuffer(this);

    // Special case for the first buffer

    if (aOffset < kHeadBufferDataSize)
    {
        aChunk.Init(GetFirstData() + aOffset, kHeadBufferDataSize - aOffset);
        ExitNow();
    }

    aOffset -= kHeadBufferDataSize;

    // Find the `Buffer` matching the offset

    while (true)
    {
        aChunk.SetBuffer(aChunk.GetBuffer()->GetNextBuffer());

        OT_ASSERT(aChunk.GetBuffer() != nullptr);

        if (aOffset < kBufferDataSize)
        {
            aChunk.Init(aChunk.GetBuffer()->GetData() + aOffset, kBufferDataSize - aOffset);
            ExitNow();
        }

        aOffset -= kBufferDataSize;
    }

exit:
    if (aChunk.GetLength() > aLength)
    {
        aChunk.SetLength(aLength);
    }

    aLength -= aChunk.GetLength();
}

void Message::GetNextChunk(uint16_t &aLength, Chunk &aChunk) const
{
    // This method gets the next message chunk. On input, the
    // `aChunk` should be the previous chunk. On exit, it is
    // updated to provide info about next chunk, and `aLength`
    // is decreased by the chunk length. If there is no more
    // chunk, `aChunk.GetLength()` would be zero.

    VerifyOrExit(aLength > 0, aChunk.SetLength(0));

    aChunk.SetBuffer(aChunk.GetBuffer()->GetNextBuffer());

    OT_ASSERT(aChunk.GetBuffer() != nullptr);

    aChunk.Init(aChunk.GetBuffer()->GetData(), kBufferDataSize);

    if (aChunk.GetLength() > aLength)
    {
        aChunk.SetLength(aLength);
    }

    aLength -= aChunk.GetLength();

exit:
    return;
}

uint16_t Message::ReadBytes(uint16_t aOffset, void *aBuf, uint16_t aLength) const
{
    uint8_t *bufPtr = reinterpret_cast<uint8_t *>(aBuf);
    Chunk    chunk;

    GetFirstChunk(aOffset, aLength, chunk);

    while (chunk.GetLength() > 0)
    {
        chunk.CopyBytesTo(bufPtr);
        bufPtr += chunk.GetLength();
        GetNextChunk(aLength, chunk);
    }

    return static_cast<uint16_t>(bufPtr - reinterpret_cast<uint8_t *>(aBuf));
}

Error Message::Read(uint16_t aOffset, void *aBuf, uint16_t aLength) const
{
    return (ReadBytes(aOffset, aBuf, aLength) == aLength) ? kErrorNone : kErrorParse;
}

bool Message::CompareBytes(uint16_t aOffset, const void *aBuf, uint16_t aLength) const
{
    uint16_t       bytesToCompare = aLength;
    const uint8_t *bufPtr         = reinterpret_cast<const uint8_t *>(aBuf);
    Chunk          chunk;

    GetFirstChunk(aOffset, aLength, chunk);

    while (chunk.GetLength() > 0)
    {
        VerifyOrExit(chunk.MatchesBytesIn(bufPtr));
        bufPtr += chunk.GetLength();
        bytesToCompare -= chunk.GetLength();
        GetNextChunk(aLength, chunk);
    }

exit:
    return (bytesToCompare == 0);
}

bool Message::CompareBytes(uint16_t       aOffset,
                           const Message &aOtherMessage,
                           uint16_t       aOtherOffset,
                           uint16_t       aLength) const
{
    uint16_t bytesToCompare = aLength;
    Chunk    chunk;

    GetFirstChunk(aOffset, aLength, chunk);

    while (chunk.GetLength() > 0)
    {
        VerifyOrExit(aOtherMessage.CompareBytes(aOtherOffset, chunk.GetBytes(), chunk.GetLength()));
        aOtherOffset += chunk.GetLength();
        bytesToCompare -= chunk.GetLength();
        GetNextChunk(aLength, chunk);
    }

exit:
    return (bytesToCompare == 0);
}

void Message::WriteBytes(uint16_t aOffset, const void *aBuf, uint16_t aLength)
{
    const uint8_t *bufPtr = reinterpret_cast<const uint8_t *>(aBuf);
    MutableChunk   chunk;

    OT_ASSERT(aOffset + aLength <= GetLength());

    GetFirstChunk(aOffset, aLength, chunk);

    while (chunk.GetLength() > 0)
    {
        memmove(chunk.GetBytes(), bufPtr, chunk.GetLength());
        bufPtr += chunk.GetLength();
        GetNextChunk(aLength, chunk);
    }
}

uint16_t Message::CopyTo(uint16_t aSourceOffset, uint16_t aDestinationOffset, uint16_t aLength, Message &aMessage) const
{
    uint16_t bytesCopied = 0;
    Chunk    chunk;

    // This implementing can potentially overwrite the data when bytes are
    // being copied forward within the same message, i.e., source and
    // destination messages are the same, and source offset is smaller than
    // the destination offset. We assert not allowing such a use.

    OT_ASSERT((&aMessage != this) || (aSourceOffset >= aDestinationOffset));

    GetFirstChunk(aSourceOffset, aLength, chunk);

    while (chunk.GetLength() > 0)
    {
        aMessage.WriteBytes(aDestinationOffset, chunk.GetBytes(), chunk.GetLength());
        aDestinationOffset += chunk.GetLength();
        bytesCopied += chunk.GetLength();
        GetNextChunk(aLength, chunk);
    }

    return bytesCopied;
}

Message *Message::Clone(uint16_t aLength) const
{
    Error    error = kErrorNone;
    Message *messageCopy;
    uint16_t offset;

    VerifyOrExit((messageCopy = GetMessagePool()->New(GetType(), GetReserved(), GetPriority())) != nullptr,
                 error = kErrorNoBufs);
    SuccessOrExit(error = messageCopy->SetLength(aLength));
    CopyTo(0, 0, aLength, *messageCopy);

    // Copy selected message information.
    offset = GetOffset() < aLength ? GetOffset() : aLength;
    messageCopy->SetOffset(offset);

    messageCopy->SetSubType(GetSubType());
    messageCopy->SetLinkSecurityEnabled(IsLinkSecurityEnabled());
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    messageCopy->SetTimeSync(IsTimeSync());
#endif

exit:
    FreeAndNullMessageOnError(messageCopy, error);
    return messageCopy;
}

bool Message::GetChildMask(uint16_t aChildIndex) const
{
    return GetMetadata().mChildMask.Get(aChildIndex);
}

void Message::ClearChildMask(uint16_t aChildIndex)
{
    GetMetadata().mChildMask.Set(aChildIndex, false);
}

void Message::SetChildMask(uint16_t aChildIndex)
{
    GetMetadata().mChildMask.Set(aChildIndex, true);
}

bool Message::IsChildPending(void) const
{
    return GetMetadata().mChildMask.HasAny();
}

void Message::SetLinkInfo(const ThreadLinkInfo &aLinkInfo)
{
    SetLinkSecurityEnabled(aLinkInfo.mLinkSecurity);
    SetPanId(aLinkInfo.mPanId);
    AddRss(aLinkInfo.mRss);
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    AddLqi(aLinkInfo.mLqi);
#endif
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    SetTimeSyncSeq(aLinkInfo.mTimeSyncSeq);
    SetNetworkTimeOffset(aLinkInfo.mNetworkTimeOffset);
#endif
#if OPENTHREAD_CONFIG_MULTI_RADIO
    SetRadioType(static_cast<Mac::RadioType>(aLinkInfo.mRadioType));
#endif
}

bool Message::IsTimeSync(void) const
{
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    return GetMetadata().mTimeSync;
#else
    return false;
#endif
}

void Message::SetMessageQueue(MessageQueue *aMessageQueue)
{
    GetMetadata().mQueue       = aMessageQueue;
    GetMetadata().mInPriorityQ = false;
}

void Message::SetPriorityQueue(PriorityQueue *aPriorityQueue)
{
    GetMetadata().mQueue       = aPriorityQueue;
    GetMetadata().mInPriorityQ = true;
}

//---------------------------------------------------------------------------------------------------------------------
// MessageQueue

MessageQueue::MessageQueue(void)
{
    SetTail(nullptr);
}

Message *MessageQueue::GetHead(void) const
{
    return (GetTail() == nullptr) ? nullptr : GetTail()->Next();
}

void MessageQueue::Enqueue(Message &aMessage, QueuePosition aPosition)
{
    OT_ASSERT(!aMessage.IsInAQueue());
    OT_ASSERT((aMessage.Next() == nullptr) && (aMessage.Prev() == nullptr));

    aMessage.SetMessageQueue(this);

    if (GetTail() == nullptr)
    {
        aMessage.Next() = &aMessage;
        aMessage.Prev() = &aMessage;

        SetTail(&aMessage);
    }
    else
    {
        Message *head = GetTail()->Next();

        aMessage.Next() = head;
        aMessage.Prev() = GetTail();

        head->Prev()      = &aMessage;
        GetTail()->Next() = &aMessage;

        if (aPosition == kQueuePositionTail)
        {
            SetTail(&aMessage);
        }
    }
}

void MessageQueue::Dequeue(Message &aMessage)
{
    OT_ASSERT(aMessage.GetMessageQueue() == this);
    OT_ASSERT((aMessage.Next() != nullptr) && (aMessage.Prev() != nullptr));

    if (&aMessage == GetTail())
    {
        SetTail(GetTail()->Prev());

        if (&aMessage == GetTail())
        {
            SetTail(nullptr);
        }
    }

    aMessage.Prev()->Next() = aMessage.Next();
    aMessage.Next()->Prev() = aMessage.Prev();

    aMessage.Prev() = nullptr;
    aMessage.Next() = nullptr;

    aMessage.SetMessageQueue(nullptr);
}

void MessageQueue::DequeueAndFree(Message &aMessage)
{
    Dequeue(aMessage);
    aMessage.Free();
}

void MessageQueue::DequeueAndFreeAll(void)
{
    Message *message;

    while ((message = GetHead()) != nullptr)
    {
        DequeueAndFree(*message);
    }
}

void MessageQueue::GetInfo(uint16_t &aMessageCount, uint16_t &aBufferCount) const
{
    aMessageCount = 0;
    aBufferCount  = 0;

    for (const Message *message = GetHead(); message != nullptr; message = message->GetNext())
    {
        aMessageCount++;
        aBufferCount += message->GetBufferCount();
    }
}

//---------------------------------------------------------------------------------------------------------------------
// PriorityQueue

PriorityQueue::PriorityQueue(void)
{
    for (Message *&tail : mTails)
    {
        tail = nullptr;
    }
}

Message *PriorityQueue::FindFirstNonNullTail(Message::Priority aStartPriorityLevel) const
{
    // Find the first non-nullptr tail starting from the given priority
    // level and moving forward (wrapping from priority value
    // `kNumPriorities` -1 back to 0).

    Message *tail = nullptr;
    uint8_t  priority;

    priority = static_cast<uint8_t>(aStartPriorityLevel);

    do
    {
        if (mTails[priority] != nullptr)
        {
            tail = mTails[priority];
            break;
        }

        priority = PrevPriority(priority);
    } while (priority != aStartPriorityLevel);

    return tail;
}

Message *PriorityQueue::GetHead(void) const
{
    Message *tail;

    tail = FindFirstNonNullTail(Message::kPriorityLow);

    return (tail == nullptr) ? nullptr : tail->Next();
}

Message *PriorityQueue::GetHeadForPriority(Message::Priority aPriority) const
{
    Message *head;
    Message *previousTail;

    if (mTails[aPriority] != nullptr)
    {
        previousTail = FindFirstNonNullTail(static_cast<Message::Priority>(PrevPriority(aPriority)));

        OT_ASSERT(previousTail != nullptr);

        head = previousTail->Next();
    }
    else
    {
        head = nullptr;
    }

    return head;
}

Message *PriorityQueue::GetTail(void) const
{
    return FindFirstNonNullTail(Message::kPriorityLow);
}

void PriorityQueue::Enqueue(Message &aMessage)
{
    Message::Priority priority;
    Message *         tail;
    Message *         next;

    OT_ASSERT(!aMessage.IsInAQueue());

    aMessage.SetPriorityQueue(this);

    priority = aMessage.GetPriority();

    tail = FindFirstNonNullTail(priority);

    if (tail != nullptr)
    {
        next = tail->Next();

        aMessage.Next() = next;
        aMessage.Prev() = tail;
        next->Prev()    = &aMessage;
        tail->Next()    = &aMessage;
    }
    else
    {
        aMessage.Next() = &aMessage;
        aMessage.Prev() = &aMessage;
    }

    mTails[priority] = &aMessage;
}

void PriorityQueue::Dequeue(Message &aMessage)
{
    Message::Priority priority;
    Message *         tail;

    OT_ASSERT(aMessage.GetPriorityQueue() == this);

    priority = aMessage.GetPriority();

    tail = mTails[priority];

    if (&aMessage == tail)
    {
        tail = tail->Prev();

        if ((&aMessage == tail) || (tail->GetPriority() != priority))
        {
            tail = nullptr;
        }

        mTails[priority] = tail;
    }

    aMessage.Next()->Prev() = aMessage.Prev();
    aMessage.Prev()->Next() = aMessage.Next();
    aMessage.Next()         = nullptr;
    aMessage.Prev()         = nullptr;

    aMessage.SetPriorityQueue(nullptr);
}

void PriorityQueue::DequeueAndFree(Message &aMessage)
{
    Dequeue(aMessage);
    aMessage.Free();
}

void PriorityQueue::DequeueAndFreeAll(void)
{
    Message *message;

    while ((message = GetHead()) != nullptr)
    {
        DequeueAndFree(*message);
    }
}

void PriorityQueue::GetInfo(uint16_t &aMessageCount, uint16_t &aBufferCount) const
{
    aMessageCount = 0;
    aBufferCount  = 0;

    for (const Message *message = GetHead(); message != nullptr; message = message->GetNext())
    {
        aMessageCount++;
        aBufferCount += message->GetBufferCount();
    }
}

} // namespace ot
#endif // OPENTHREAD_MTD || OPENTHREAD_FTD
