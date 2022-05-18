/*
 *    Copyright (c) 2016, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 *    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements NCP frame buffer class.
 */

#include "spinel_buffer.hpp"

#include "common/code_utils.hpp"
#include "common/debug.hpp"

namespace ot {
namespace Spinel {

const Buffer::FrameTag Buffer::kInvalidTag = nullptr;

Buffer::Buffer(uint8_t *aBuffer, uint16_t aBufferLength)
    : mBuffer(aBuffer)
    , mBufferEnd(aBuffer + aBufferLength)
    , mBufferLength(aBufferLength)
{
#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
    for (otMessageQueue &queue : mMessageQueue)
    {
        otMessageQueueInit(&queue);
    }

    otMessageQueueInit(&mWriteFrameMessageQueue);
#endif

    SetFrameAddedCallback(nullptr, nullptr);
    SetFrameRemovedCallback(nullptr, nullptr);
    Clear();
}

void Buffer::Clear(void)
{
#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
    otMessage *message;
#endif

    // Write (InFrame) related variables
    mWriteFrameStart[kPriorityLow]  = mBuffer;
    mWriteFrameStart[kPriorityHigh] = GetUpdatedBufPtr(mBuffer, 1, kBackward);
    mWriteDirection                 = kUnknown;
    mWriteSegmentHead               = mBuffer;
    mWriteSegmentTail               = mBuffer;
    mWriteFrameTag                  = kInvalidTag;

    // Read (OutFrame) related variables
    mReadDirection   = kForward;
    mReadState       = kReadStateNotActive;
    mReadFrameLength = kUnknownFrameLength;

    mReadFrameStart[kPriorityLow]  = mBuffer;
    mReadFrameStart[kPriorityHigh] = GetUpdatedBufPtr(mBuffer, 1, kBackward);
    mReadSegmentHead               = mBuffer;
    mReadSegmentTail               = mBuffer;
    mReadPointer                   = mBuffer;

#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
    mReadMessage       = nullptr;
    mReadMessageOffset = 0;
    mReadMessageTail   = mMessageBuffer;

    // Free all messages in the queues.

    while ((message = otMessageQueueGetHead(&mWriteFrameMessageQueue)) != nullptr)
    {
        otMessageQueueDequeue(&mWriteFrameMessageQueue, message);

        // Note that messages associated with current (unfinished) input frame
        // are not yet owned by the `Buffer` and therefore should not
        // be freed.
    }

    for (otMessageQueue &queue : mMessageQueue)
    {
        while ((message = otMessageQueueGetHead(&queue)) != nullptr)
        {
            otMessageQueueDequeue(&queue, message);
            otMessageFree(message);
        }
    }
#endif
}

void Buffer::SetFrameAddedCallback(BufferCallback aFrameAddedCallback, void *aFrameAddedContext)
{
    mFrameAddedCallback = aFrameAddedCallback;
    mFrameAddedContext  = aFrameAddedContext;
}

void Buffer::SetFrameRemovedCallback(BufferCallback aFrameRemovedCallback, void *aFrameRemovedContext)
{
    mFrameRemovedCallback = aFrameRemovedCallback;
    mFrameRemovedContext  = aFrameRemovedContext;
}

// Returns an updated buffer pointer by moving forward/backward (based on `aDirection`) from `aBufPtr` by a given
// offset. The resulting buffer pointer is ensured to stay within the `mBuffer` boundaries.
uint8_t *Buffer::GetUpdatedBufPtr(uint8_t *aBufPtr, uint16_t aOffset, Direction aDirection) const
{
    uint8_t *ptr = aBufPtr;

    switch (aDirection)
    {
    case kForward:
        ptr += aOffset;

        while (ptr >= mBufferEnd)
        {
            ptr -= mBufferLength;
        }

        break;

    case kBackward:
        ptr -= aOffset;

        while (ptr < mBuffer)
        {
            ptr += mBufferLength;
        }

        break;

    case kUnknown:
        OT_ASSERT(false);
        OT_UNREACHABLE_CODE(break);
    }

    return ptr;
}

// Gets the distance between two buffer pointers (adjusts for the wrap-around) given a direction (forward or backward).
uint16_t Buffer::GetDistance(const uint8_t *aStartPtr, const uint8_t *aEndPtr, Direction aDirection) const
{
    size_t distance = 0;

    switch (aDirection)
    {
    case kForward:

        if (aEndPtr >= aStartPtr)
        {
            distance = static_cast<size_t>(aEndPtr - aStartPtr);
        }
        else
        {
            distance = static_cast<size_t>(mBufferEnd - aStartPtr);
            distance += static_cast<size_t>(aEndPtr - mBuffer);
        }

        break;

    case kBackward:

        if (aEndPtr <= aStartPtr)
        {
            distance = static_cast<size_t>(aStartPtr - aEndPtr);
        }
        else
        {
            distance = static_cast<size_t>(mBufferEnd - aEndPtr);
            distance += static_cast<size_t>(aStartPtr - mBuffer);
        }

        break;

    case kUnknown:
        OT_ASSERT(false);
        OT_UNREACHABLE_CODE(break);
    }

    return static_cast<uint16_t>(distance);
}

// Writes a uint16 value at the given buffer pointer (big-endian style).
void Buffer::WriteUint16At(uint8_t *aBufPtr, uint16_t aValue, Direction aDirection)
{
    *aBufPtr                                  = (aValue >> 8);
    *GetUpdatedBufPtr(aBufPtr, 1, aDirection) = (aValue & 0xff);
}

// Reads a uint16 value at the given buffer pointer (big-endian style).
uint16_t Buffer::ReadUint16At(uint8_t *aBufPtr, Direction aDirection)
{
    uint16_t value;

    value = static_cast<uint16_t>((*aBufPtr) << 8);
    value += *GetUpdatedBufPtr(aBufPtr, 1, aDirection);

    return value;
}

// Appends a byte at the write tail and updates the tail, discards the frame if buffer gets full.
otError Buffer::InFrameAppend(uint8_t aByte)
{
    otError  error = OT_ERROR_NONE;
    uint8_t *newTail;

    OT_ASSERT(mWriteDirection != kUnknown);

    newTail = GetUpdatedBufPtr(mWriteSegmentTail, 1, mWriteDirection);

    // Ensure the `newTail` has not reached the `mWriteFrameStart` for other direction (other priority level).
    if (newTail != mWriteFrameStart[(mWriteDirection == kForward) ? kBackward : kForward])
    {
        *mWriteSegmentTail = aByte;
        mWriteSegmentTail  = newTail;
    }
    else
    {
        error = OT_ERROR_NO_BUFS;
        InFrameDiscard();
    }

    return error;
}

// This method begins a new segment (if one is not already open).
otError Buffer::InFrameBeginSegment(void)
{
    otError  error       = OT_ERROR_NONE;
    uint16_t headerFlags = kSegmentHeaderNoFlag;

    // Verify that segment is not yet started (i.e., head and tail are the same).
    VerifyOrExit(mWriteSegmentHead == mWriteSegmentTail);

    // Check if this is the start of a new frame (i.e., frame start is same as segment head).
    if (mWriteFrameStart[mWriteDirection] == mWriteSegmentHead)
    {
        headerFlags |= kSegmentHeaderNewFrameFlag;
    }

    // Reserve space for the segment header.
    for (uint16_t i = kSegmentHeaderSize; i; i--)
    {
        SuccessOrExit(error = InFrameAppend(0));
    }

    // Write the flags at the segment head.
    WriteUint16At(mWriteSegmentHead, headerFlags, mWriteDirection);

exit:
    return error;
}

// This function closes/ends the current segment.
void Buffer::InFrameEndSegment(uint16_t aSegmentHeaderFlags)
{
    uint16_t segmentLength;
    uint16_t header;

    segmentLength = GetDistance(mWriteSegmentHead, mWriteSegmentTail, mWriteDirection);

    if (segmentLength >= kSegmentHeaderSize)
    {
        // Reduce the header size.
        segmentLength -= kSegmentHeaderSize;

        // Update the length and the flags in segment header (at segment head pointer).
        header = ReadUint16At(mWriteSegmentHead, mWriteDirection);
        header |= (segmentLength & kSegmentHeaderLengthMask);
        header |= aSegmentHeaderFlags;
        WriteUint16At(mWriteSegmentHead, header, mWriteDirection);

        // Move the segment head to current tail (to be ready for a possible next segment).
        mWriteSegmentHead = mWriteSegmentTail;
    }
    else
    {
        // Remove the current segment (move the tail back to head).
        mWriteSegmentTail = mWriteSegmentHead;
    }
}

// This method discards the current frame being written.
void Buffer::InFrameDiscard(void)
{
#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
    otMessage *message;
#endif

    VerifyOrExit(mWriteDirection != kUnknown);

    // Move the write segment head and tail pointers back to frame start.
    mWriteSegmentHead = mWriteSegmentTail = mWriteFrameStart[mWriteDirection];

#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
    while ((message = otMessageQueueGetHead(&mWriteFrameMessageQueue)) != nullptr)
    {
        otMessageQueueDequeue(&mWriteFrameMessageQueue, message);

        // Note that messages associated with current (unfinished) input frame
        // being discarded, are not yet owned by the `Buffer` and
        // therefore should not be freed.
    }
#endif

    mWriteDirection = kUnknown;

exit:
    UpdateReadWriteStartPointers();
}

// Returns `true` if in middle of writing a frame with given priority.
bool Buffer::InFrameIsWriting(Priority aPriority) const
{
    return (mWriteDirection == static_cast<Direction>(aPriority));
}

void Buffer::InFrameBegin(Priority aPriority)
{
    // Discard any previous unfinished frame.
    InFrameDiscard();

    switch (aPriority)
    {
    case kPriorityHigh:
        mWriteDirection = kBackward;
        break;

    case kPriorityLow:
        mWriteDirection = kForward;
        break;
    }

    // Set up the segment head and tail
    mWriteSegmentHead = mWriteSegmentTail = mWriteFrameStart[mWriteDirection];
}

otError Buffer::InFrameFeedByte(uint8_t aByte)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(mWriteDirection != kUnknown, error = OT_ERROR_INVALID_STATE);

    // Begin a new segment (if we are not in middle of segment already).
    SuccessOrExit(error = InFrameBeginSegment());

    error = InFrameAppend(aByte);

exit:
    return error;
}

otError Buffer::InFrameFeedData(const uint8_t *aDataBuffer, uint16_t aDataBufferLength)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(mWriteDirection != kUnknown, error = OT_ERROR_INVALID_STATE);

    // Begin a new segment (if we are not in middle of segment already).
    SuccessOrExit(error = InFrameBeginSegment());

    // Write the data buffer
    while (aDataBufferLength--)
    {
        SuccessOrExit(error = InFrameAppend(*aDataBuffer++));
    }

exit:
    return error;
}

#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
otError Buffer::InFrameFeedMessage(otMessage *aMessage)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(aMessage != nullptr, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(mWriteDirection != kUnknown, error = OT_ERROR_INVALID_STATE);

    // Begin a new segment (if we are not in middle of segment already).
    SuccessOrExit(error = InFrameBeginSegment());

    // Enqueue the message in the current write frame queue.
    otMessageQueueEnqueue(&mWriteFrameMessageQueue, aMessage);

    // End/Close the current segment marking the flag that it contains an associated message.
    InFrameEndSegment(kSegmentHeaderMessageIndicatorFlag);

exit:
    return error;
}
#endif

otError Buffer::InFrameGetPosition(WritePosition &aPosition)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(mWriteDirection != kUnknown, error = OT_ERROR_INVALID_STATE);

    // Begin a new segment (if we are not in middle of segment already).
    SuccessOrExit(error = InFrameBeginSegment());

    aPosition.mPosition    = mWriteSegmentTail;
    aPosition.mSegmentHead = mWriteSegmentHead;

exit:
    return error;
}

otError Buffer::InFrameOverwrite(const WritePosition &aPosition, const uint8_t *aDataBuffer, uint16_t aDataBufferLength)
{
    otError  error = OT_ERROR_NONE;
    uint8_t *bufPtr;
    uint16_t segmentLength;
    uint16_t distance;

    VerifyOrExit(mWriteDirection != kUnknown, error = OT_ERROR_INVALID_STATE);

    VerifyOrExit(aPosition.mSegmentHead == mWriteSegmentHead, error = OT_ERROR_INVALID_ARGS);

    // Ensure the overwrite does not go beyond current segment tail.
    segmentLength = GetDistance(mWriteSegmentHead, mWriteSegmentTail, mWriteDirection);
    distance      = GetDistance(mWriteSegmentHead, aPosition.mPosition, mWriteDirection);
    VerifyOrExit(distance + aDataBufferLength <= segmentLength, error = OT_ERROR_INVALID_ARGS);

    bufPtr = aPosition.mPosition;
    while (aDataBufferLength > 0)
    {
        *bufPtr = *aDataBuffer;

        aDataBuffer++;
        aDataBufferLength--;

        bufPtr = GetUpdatedBufPtr(bufPtr, 1, mWriteDirection);
    }

exit:
    return error;
}

uint16_t Buffer::InFrameGetDistance(const WritePosition &aPosition) const
{
    uint16_t distance = 0;
    uint16_t segmentLength;
    uint16_t offset;

    VerifyOrExit(mWriteDirection != kUnknown);
    VerifyOrExit(aPosition.mSegmentHead == mWriteSegmentHead);

    segmentLength = GetDistance(mWriteSegmentHead, mWriteSegmentTail, mWriteDirection);
    offset        = GetDistance(mWriteSegmentHead, aPosition.mPosition, mWriteDirection);
    VerifyOrExit(offset < segmentLength);

    distance = GetDistance(aPosition.mPosition, mWriteSegmentTail, mWriteDirection);

exit:
    return distance;
}

otError Buffer::InFrameReset(const WritePosition &aPosition)
{
    otError  error = OT_ERROR_NONE;
    uint16_t segmentLength;
    uint16_t offset;

    VerifyOrExit(mWriteDirection != kUnknown, error = OT_ERROR_INVALID_STATE);
    VerifyOrExit(aPosition.mSegmentHead == mWriteSegmentHead, error = OT_ERROR_INVALID_ARGS);

    segmentLength = GetDistance(mWriteSegmentHead, mWriteSegmentTail, mWriteDirection);
    offset        = GetDistance(mWriteSegmentHead, aPosition.mPosition, mWriteDirection);
    VerifyOrExit(offset < segmentLength, error = OT_ERROR_INVALID_ARGS);

    mWriteSegmentTail = aPosition.mPosition;

exit:
    return error;
}

otError Buffer::InFrameEnd(void)
{
#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
    otMessage *message;
#endif
    otError error = OT_ERROR_NONE;

    VerifyOrExit(mWriteDirection != kUnknown, error = OT_ERROR_INVALID_STATE);

    // End/Close the current segment (if any).
    InFrameEndSegment(kSegmentHeaderNoFlag);

    // Save and use the frame start pointer as the tag associated with the frame.
    mWriteFrameTag = mWriteFrameStart[mWriteDirection];

    // Update the frame start pointer to current segment head to be ready for next frame.
    mWriteFrameStart[mWriteDirection] = mWriteSegmentHead;

#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
    // Move all the messages from the frame queue to the main queue.
    while ((message = otMessageQueueGetHead(&mWriteFrameMessageQueue)) != nullptr)
    {
        otMessageQueueDequeue(&mWriteFrameMessageQueue, message);
        otMessageQueueEnqueue(&mMessageQueue[mWriteDirection], message);
    }
#endif

    if (mFrameAddedCallback != nullptr)
    {
        mFrameAddedCallback(mFrameAddedContext, mWriteFrameTag, static_cast<Priority>(mWriteDirection), this);
    }

    mWriteDirection = kUnknown;

exit:
    return error;
}

Buffer::FrameTag Buffer::InFrameGetLastTag(void) const
{
    return mWriteFrameTag;
}

bool Buffer::HasFrame(Priority aPriority) const
{
    return mReadFrameStart[aPriority] != mWriteFrameStart[aPriority];
}

bool Buffer::IsEmpty(void) const
{
    return !HasFrame(kPriorityHigh) && !HasFrame(kPriorityLow);
}

void Buffer::OutFrameSelectReadDirection(void)
{
    if (mReadState == kReadStateNotActive)
    {
        mReadDirection = HasFrame(kPriorityHigh) ? kBackward : kForward;
    }
}

// Start/Prepare a new segment for reading.
otError Buffer::OutFramePrepareSegment(void)
{
    otError  error = OT_ERROR_NONE;
    uint16_t header;

    while (true)
    {
        // Go to the next segment (set the segment head to current segment's end/tail).
        mReadSegmentHead = mReadSegmentTail;

        // Ensure there is something to read (i.e. segment head is not at start of frame being written).
        VerifyOrExit(mReadSegmentHead != mWriteFrameStart[mReadDirection], error = OT_ERROR_NOT_FOUND);

        // Read the segment header.
        header = ReadUint16At(mReadSegmentHead, mReadDirection);

        // Check if this segment is the start of a frame.
        if (header & kSegmentHeaderNewFrameFlag)
        {
            // Ensure that this segment is start of current frame, otherwise the current frame is finished.
            VerifyOrExit(mReadSegmentHead == mReadFrameStart[mReadDirection], error = OT_ERROR_NOT_FOUND);
        }

        // Find tail/end of current segment.
        mReadSegmentTail = GetUpdatedBufPtr(mReadSegmentHead, kSegmentHeaderSize + (header & kSegmentHeaderLengthMask),
                                            mReadDirection);

        // Update the current read pointer to skip the segment header.
        mReadPointer = GetUpdatedBufPtr(mReadSegmentHead, kSegmentHeaderSize, mReadDirection);

        // Check if there are data bytes to be read in this segment (i.e. read pointer not at the tail).
        if (mReadPointer != mReadSegmentTail)
        {
            // Update the state to `InSegment` and return.
            mReadState = kReadStateInSegment;

            ExitNow();
        }

#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
        // No data in this segment,  prepare any appended/associated message of this segment.
        if (OutFramePrepareMessage() == OT_ERROR_NONE)
        {
            ExitNow();
        }

        // If there is no message (`PrepareMessage()` returned an error), loop back to prepare the next segment.
#endif
    }

exit:

    if (error != OT_ERROR_NONE)
    {
        mReadState = kReadStateDone;
    }

    return error;
}

#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
// This method prepares an associated message in current segment and fills the message buffer. It returns
// ThreadError_NotFound if there is no message or if the message has no content.
otError Buffer::OutFramePrepareMessage(void)
{
    otError  error = OT_ERROR_NONE;
    uint16_t header;

    // Read the segment header
    header = ReadUint16At(mReadSegmentHead, mReadDirection);

    // Ensure that the segment header indicates that there is an associated message or return `NotFound` error.
    VerifyOrExit((header & kSegmentHeaderMessageIndicatorFlag) != 0, error = OT_ERROR_NOT_FOUND);

    // Update the current message from the queue.
    mReadMessage = (mReadMessage == nullptr) ? otMessageQueueGetHead(&mMessageQueue[mReadDirection])
                                             : otMessageQueueGetNext(&mMessageQueue[mReadDirection], mReadMessage);

    VerifyOrExit(mReadMessage != nullptr, error = OT_ERROR_NOT_FOUND);

    // Reset the offset for reading the message.
    mReadMessageOffset = 0;

    // Fill the content from current message into the message buffer.
    SuccessOrExit(error = OutFrameFillMessageBuffer());

    // If all successful, set the state to `InMessage`.
    mReadState = kReadStateInMessage;

exit:
    return error;
}

// This method fills content from current message into the message buffer. It returns OT_ERROR_NOT_FOUND if no more
// content in the current message.
otError Buffer::OutFrameFillMessageBuffer(void)
{
    otError error = OT_ERROR_NONE;
    int     readLength;

    VerifyOrExit(mReadMessage != nullptr, error = OT_ERROR_NOT_FOUND);

    VerifyOrExit(mReadMessageOffset < otMessageGetLength(mReadMessage), error = OT_ERROR_NOT_FOUND);

    // Read portion of current message from the offset into message buffer.
    readLength = otMessageRead(mReadMessage, mReadMessageOffset, mMessageBuffer, sizeof(mMessageBuffer));

    VerifyOrExit(readLength > 0, error = OT_ERROR_NOT_FOUND);

    // Update the message offset, set up the message tail, and set read pointer to start of message buffer.

    mReadMessageOffset += readLength;

    mReadMessageTail = mMessageBuffer + readLength;

    mReadPointer = mMessageBuffer;

exit:
    return error;
}
#endif // #if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE

otError Buffer::OutFrameBegin(void)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(!IsEmpty(), error = OT_ERROR_NOT_FOUND);

    OutFrameSelectReadDirection();

    // Move the segment head and tail to start of frame.
    mReadSegmentHead = mReadSegmentTail = mReadFrameStart[mReadDirection];

#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
    mReadMessage = nullptr;
#endif

    // Prepare the current segment for reading.
    error = OutFramePrepareSegment();

exit:
    return error;
}

bool Buffer::OutFrameHasEnded(void)
{
    return (mReadState == kReadStateDone) || (mReadState == kReadStateNotActive);
}

uint8_t Buffer::OutFrameReadByte(void)
{
    otError error;
    uint8_t retval = kReadByteAfterFrameHasEnded;

    switch (mReadState)
    {
    case kReadStateNotActive:
        OT_FALL_THROUGH;

    case kReadStateDone:

        retval = kReadByteAfterFrameHasEnded;

        break;

    case kReadStateInSegment:

        // Read a byte from current read pointer and move the read pointer by 1 byte in the read direction.
        retval       = *mReadPointer;
        mReadPointer = GetUpdatedBufPtr(mReadPointer, 1, mReadDirection);

        // Check if at end of current segment.
        if (mReadPointer == mReadSegmentTail)
        {
#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
            // Prepare any message associated with this segment.
            error = OutFramePrepareMessage();
#else
            error = OT_ERROR_NOT_FOUND;
#endif

            // If there is no message, move to next segment (if any).
            if (error != OT_ERROR_NONE)
            {
                IgnoreError(OutFramePrepareSegment());
            }
        }

        break;

    case kReadStateInMessage:
#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
        // Read a byte from current read pointer and move the read pointer by 1 byte.
        retval = *mReadPointer;
        mReadPointer++;

        // Check if at the end of content in message buffer.
        if (mReadPointer == mReadMessageTail)
        {
            // Fill more bytes from current message into message buffer.
            error = OutFrameFillMessageBuffer();

            // If no more bytes in the message, move to next segment (if any).
            if (error != OT_ERROR_NONE)
            {
                IgnoreError(OutFramePrepareSegment());
            }
        }
#endif
        break;
    }

    return retval;
}

uint16_t Buffer::OutFrameRead(uint16_t aReadLength, uint8_t *aDataBuffer)
{
    uint16_t bytesRead = 0;

    for (bytesRead = 0; (bytesRead < aReadLength) && !OutFrameHasEnded(); bytesRead++)
    {
        *aDataBuffer++ = OutFrameReadByte();
    }

    return bytesRead;
}

otError Buffer::OutFrameRemove(void)
{
    otError  error = OT_ERROR_NONE;
    uint8_t *bufPtr;
    uint16_t header;
    uint8_t  numSegments;
    FrameTag tag;

    VerifyOrExit(!IsEmpty(), error = OT_ERROR_NOT_FOUND);

    OutFrameSelectReadDirection();

    // Save the frame start pointer as the tag associated with the frame being removed.
    tag = mReadFrameStart[mReadDirection];

    // Begin at the start of current frame and move through all segments.

    bufPtr      = mReadFrameStart[mReadDirection];
    numSegments = 0;

    while (bufPtr != mWriteFrameStart[mReadDirection])
    {
        // Read the segment header
        header = ReadUint16At(bufPtr, mReadDirection);

        // If the current segment defines a new frame, and it is not the start of current frame, then we have reached
        // end of current frame.
        if (header & kSegmentHeaderNewFrameFlag)
        {
            if (bufPtr != mReadFrameStart[mReadDirection])
            {
                break;
            }
        }

#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
        // If current segment has an appended message, remove it from message queue and free it.
        if (header & kSegmentHeaderMessageIndicatorFlag)
        {
            otMessage *message;

            if ((message = otMessageQueueGetHead(&mMessageQueue[mReadDirection])) != nullptr)
            {
                otMessageQueueDequeue(&mMessageQueue[mReadDirection], message);
                otMessageFree(message);
            }
        }
#endif

        // Move the pointer to next segment.
        bufPtr = GetUpdatedBufPtr(bufPtr, kSegmentHeaderSize + (header & kSegmentHeaderLengthMask), mReadDirection);

        numSegments++;

        // If this assert fails, it is a likely indicator that the internal structure of the NCP buffer has been
        // corrupted.
        OT_ASSERT(numSegments <= kMaxSegments);
    }

    mReadFrameStart[mReadDirection] = bufPtr;

    UpdateReadWriteStartPointers();

    mReadState       = kReadStateNotActive;
    mReadFrameLength = kUnknownFrameLength;

    if (mFrameRemovedCallback != nullptr)
    {
        mFrameRemovedCallback(mFrameRemovedContext, tag, static_cast<Priority>(mReadDirection), this);
    }

exit:
    return error;
}

void Buffer::UpdateReadWriteStartPointers(void)
{
    // If there is no fully written high priority frame, and not in middle of writing a new frame either.
    if (!HasFrame(kPriorityHigh) && !InFrameIsWriting(kPriorityHigh))
    {
        // Move the high priority pointers to be right behind the low priority start.
        mWriteFrameStart[kPriorityHigh] = GetUpdatedBufPtr(mReadFrameStart[kPriorityLow], 1, kBackward);
        mReadFrameStart[kPriorityHigh]  = mWriteFrameStart[kPriorityHigh];
        ExitNow();
    }

    // If there is no fully written low priority frame, and not in middle of writing a new frame either.
    if (!HasFrame(kPriorityLow) && !InFrameIsWriting(kPriorityLow))
    {
        // Move the low priority pointers to be 1 byte after the high priority start.
        mWriteFrameStart[kPriorityLow] = GetUpdatedBufPtr(mReadFrameStart[kPriorityHigh], 1, kForward);
        mReadFrameStart[kPriorityLow]  = mWriteFrameStart[kPriorityLow];
    }

exit:
    return;
}

uint16_t Buffer::OutFrameGetLength(void)
{
    uint16_t frameLength = 0;
    uint16_t header;
    uint8_t *bufPtr;
    uint8_t  numSegments;
#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
    otMessage *message = nullptr;
#endif

    // If the frame length was calculated before, return the previously calculated length.
    VerifyOrExit(mReadFrameLength == kUnknownFrameLength, frameLength = mReadFrameLength);

    VerifyOrExit(!IsEmpty(), frameLength = 0);

    OutFrameSelectReadDirection();

    // Calculate frame length by adding length of all segments and messages within the current frame.

    bufPtr      = mReadFrameStart[mReadDirection];
    numSegments = 0;

    while (bufPtr != mWriteFrameStart[mReadDirection])
    {
        // Read the segment header
        header = ReadUint16At(bufPtr, mReadDirection);

        // If the current segment defines a new frame, and it is not the start of current frame, then we have reached
        // end of current frame.
        if (header & kSegmentHeaderNewFrameFlag)
        {
            if (bufPtr != mReadFrameStart[mReadDirection])
            {
                break;
            }
        }

#if OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE
        // If current segment has an associated message, add its length to frame length.
        if (header & kSegmentHeaderMessageIndicatorFlag)
        {
            message = (message == nullptr) ? otMessageQueueGetHead(&mMessageQueue[mReadDirection])
                                           : otMessageQueueGetNext(&mMessageQueue[mReadDirection], message);

            if (message != nullptr)
            {
                frameLength += otMessageGetLength(message);
            }
        }
#endif

        // Add the length of current segment to the frame length.
        frameLength += (header & kSegmentHeaderLengthMask);

        // Move the pointer to next segment.
        bufPtr = GetUpdatedBufPtr(bufPtr, kSegmentHeaderSize + (header & kSegmentHeaderLengthMask), mReadDirection);

        numSegments++;

        // If this assert fails, it is a likely indicator that the internal structure of the NCP buffer has been
        // corrupted.
        OT_ASSERT(numSegments <= kMaxSegments);
    }

    // Remember the calculated frame length for current active frame.
    if (mReadState != kReadStateNotActive)
    {
        mReadFrameLength = frameLength;
    }

exit:
    return frameLength;
}

Buffer::FrameTag Buffer::OutFrameGetTag(void)
{
    OutFrameSelectReadDirection();

    // If buffer is empty use `kInvalidTag`, otherwise use the frame start pointer as the tag associated with
    // current out frame being read

    return IsEmpty() ? kInvalidTag : mReadFrameStart[mReadDirection];
}

} // namespace Spinel
} // namespace ot
