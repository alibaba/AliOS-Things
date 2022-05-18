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

#include <ctype.h>

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "common/message.hpp"
#include "common/random.hpp"
#include "lib/spinel/spinel_buffer.hpp"

#include "test_platform.h"
#include "test_util.hpp"

namespace ot {
namespace Spinel {

// This module implements unit-test for Spinel::Buffer class.

// Test related constants:
enum
{
    kTestBufferSize       = 800,
    kTestIterationAttemps = 10000,
    kTagArraySize         = 1000,
};

//  Messages used for building frames...
static const uint8_t sOpenThreadText[] = "OpenThread Rocks";
static const uint8_t sHelloText[]      = "Hello there!";
static const uint8_t sMottoText[]      = "Think good thoughts, say good words, do good deeds!";
static const uint8_t sMysteryText[]    = "4871(\\):|(3$}{4|/4/2%14(\\)";
static const uint8_t sHexText[]        = "0123456789abcdef";

static ot::Instance *sInstance;
static MessagePool * sMessagePool;

struct CallbackContext
{
    uint32_t mFrameAddedCount;   // Number of times FrameAddedCallback is invoked.
    uint32_t mFrameRemovedCount; // Number of times FrameRemovedCallback is invoked.
};

CallbackContext sContext;

enum
{
    kNumPrios = 2, // Number of priority levels.

    kTestFrame1Size = sizeof(sMottoText) + sizeof(sMysteryText) + sizeof(sMottoText) + sizeof(sHelloText),
    kTestFrame2Size = sizeof(sMysteryText) + sizeof(sHelloText) + sizeof(sOpenThreadText),
    kTestFrame3Size = sizeof(sMysteryText),
    kTestFrame4Size = sizeof(sOpenThreadText),
};

Spinel::Buffer::FrameTag sTagHistoryArray[kNumPrios][kTagArraySize];
uint32_t                 sTagHistoryHead[kNumPrios] = {0};
uint32_t                 sTagHistoryTail[kNumPrios] = {0};
Spinel::Buffer::FrameTag sExpectedRemovedTag        = Spinel::Buffer::kInvalidTag;

void ClearTagHistory(void)
{
    for (uint8_t priority = 0; priority < kNumPrios; priority++)
    {
        sTagHistoryHead[priority] = sTagHistoryTail[priority];
    }
}

void AddTagToHistory(Spinel::Buffer::FrameTag aTag, Spinel::Buffer::Priority aPriority)
{
    uint8_t priority = static_cast<uint8_t>(aPriority);

    sTagHistoryArray[priority][sTagHistoryTail[priority]] = aTag;

    if (++sTagHistoryTail[priority] == kTagArraySize)
    {
        sTagHistoryTail[priority] = 0;
    }

    VerifyOrQuit(sTagHistoryTail[priority] != sTagHistoryHead[priority],
                 "Ran out of space in `TagHistoryArray`, increase its size.");
}

void VerifyAndRemoveTagFromHistory(Spinel::Buffer::FrameTag aTag, Spinel::Buffer::Priority aPriority)
{
    uint8_t priority = static_cast<uint8_t>(aPriority);

    VerifyOrQuit(sTagHistoryHead[priority] != sTagHistoryTail[priority], "Tag history is empty,");
    VerifyOrQuit(aTag == sTagHistoryArray[priority][sTagHistoryHead[priority]],
                 "Removed tag does not match the added one");

    if (++sTagHistoryHead[priority] == kTagArraySize)
    {
        sTagHistoryHead[priority] = 0;
    }

    if (sExpectedRemovedTag != Spinel::Buffer::kInvalidTag)
    {
        VerifyOrQuit(sExpectedRemovedTag == aTag, "Removed tag does match the previous OutFrameGetTag()");
        sExpectedRemovedTag = Spinel::Buffer::kInvalidTag;
    }
}

void FrameAddedCallback(void *                   aContext,
                        Spinel::Buffer::FrameTag aTag,
                        Spinel::Buffer::Priority aPriority,
                        Spinel::Buffer *         aNcpBuffer)
{
    CallbackContext *callbackContext = reinterpret_cast<CallbackContext *>(aContext);

    VerifyOrQuit(aNcpBuffer != nullptr, "Null Spinel::Buffer in the callback");
    VerifyOrQuit(callbackContext != nullptr, "Null context in the callback");
    VerifyOrQuit(aTag != Spinel::Buffer::kInvalidTag, "Invalid tag in the callback");
    VerifyOrQuit(aTag == aNcpBuffer->InFrameGetLastTag(), "InFrameGetLastTag() does not match the tag from callback");

    AddTagToHistory(aTag, aPriority);

    callbackContext->mFrameAddedCount++;
}

void FrameRemovedCallback(void *                   aContext,
                          Spinel::Buffer::FrameTag aTag,
                          Spinel::Buffer::Priority aPriority,
                          Spinel::Buffer *         aNcpBuffer)
{
    CallbackContext *callbackContext = reinterpret_cast<CallbackContext *>(aContext);

    VerifyOrQuit(aNcpBuffer != nullptr, "Null Spinel::Buffer in the callback");
    VerifyOrQuit(callbackContext != nullptr, "Null context in the callback");
    VerifyOrQuit(aTag != Spinel::Buffer::kInvalidTag, "Invalid tag in the callback");

    VerifyAndRemoveTagFromHistory(aTag, aPriority);

    callbackContext->mFrameRemovedCount++;
}

// Reads bytes from the ncp buffer, and verifies that it matches with the given content buffer.
void ReadAndVerifyContent(Spinel::Buffer &aNcpBuffer, const uint8_t *aContentBuffer, uint16_t aBufferLength)
{
    while (aBufferLength--)
    {
        VerifyOrQuit(aNcpBuffer.OutFrameHasEnded() == false, "Out frame ended before end of expected content.");

        VerifyOrQuit(aNcpBuffer.OutFrameReadByte() == *aContentBuffer++,
                     "Out frame read byte does not match expected content");
    }
}

void WriteTestFrame1(Spinel::Buffer &aNcpBuffer, Spinel::Buffer::Priority aPriority)
{
    Message *       message;
    CallbackContext oldContext;

    message = sMessagePool->New(Message::kTypeIp6, 0);
    VerifyOrQuit(message != nullptr, "Null Message");
    SuccessOrQuit(message->SetLength(sizeof(sMottoText)));
    message->Write(0, sMottoText);

    oldContext = sContext;
    aNcpBuffer.InFrameBegin(aPriority);
    SuccessOrQuit(aNcpBuffer.InFrameFeedData(sMottoText, sizeof(sMottoText)));
    SuccessOrQuit(aNcpBuffer.InFrameFeedData(sMysteryText, sizeof(sMysteryText)));
    SuccessOrQuit(aNcpBuffer.InFrameFeedMessage(message));
    SuccessOrQuit(aNcpBuffer.InFrameFeedData(sHelloText, sizeof(sHelloText)));
    SuccessOrQuit(aNcpBuffer.InFrameEnd());
    VerifyOrQuit(oldContext.mFrameAddedCount + 1 == sContext.mFrameAddedCount, "FrameAddedCallback failed.");
    VerifyOrQuit(oldContext.mFrameRemovedCount == sContext.mFrameRemovedCount, "FrameRemovedCallback failed.");
}

void VerifyAndRemoveFrame1(Spinel::Buffer &aNcpBuffer)
{
    CallbackContext oldContext = sContext;

    sExpectedRemovedTag = aNcpBuffer.OutFrameGetTag();
    VerifyOrQuit(aNcpBuffer.OutFrameGetLength() == kTestFrame1Size);
    SuccessOrQuit(aNcpBuffer.OutFrameBegin());
    VerifyOrQuit(sExpectedRemovedTag == aNcpBuffer.OutFrameGetTag());
    VerifyOrQuit(aNcpBuffer.OutFrameGetLength() == kTestFrame1Size);
    ReadAndVerifyContent(aNcpBuffer, sMottoText, sizeof(sMottoText));
    ReadAndVerifyContent(aNcpBuffer, sMysteryText, sizeof(sMysteryText));
    ReadAndVerifyContent(aNcpBuffer, sMottoText, sizeof(sMottoText));
    ReadAndVerifyContent(aNcpBuffer, sHelloText, sizeof(sHelloText));
    VerifyOrQuit(aNcpBuffer.OutFrameHasEnded(), "Frame longer than expected.");
    VerifyOrQuit(aNcpBuffer.OutFrameReadByte() == 0, "ReadByte() returned non-zero after end of frame.");
    VerifyOrQuit(sExpectedRemovedTag == aNcpBuffer.OutFrameGetTag());
    VerifyOrQuit(aNcpBuffer.OutFrameGetLength() == kTestFrame1Size);
    SuccessOrQuit(aNcpBuffer.OutFrameRemove());
    VerifyOrQuit(oldContext.mFrameAddedCount == sContext.mFrameAddedCount, "FrameAddedCallback failed.");
    VerifyOrQuit(oldContext.mFrameRemovedCount + 1 == sContext.mFrameRemovedCount, "FrameRemovedCallback failed.");
}

void WriteTestFrame2(Spinel::Buffer &aNcpBuffer, Spinel::Buffer::Priority aPriority)
{
    Message *       message1;
    Message *       message2;
    CallbackContext oldContext = sContext;

    message1 = sMessagePool->New(Message::kTypeIp6, 0);
    VerifyOrQuit(message1 != nullptr, "Null Message");
    SuccessOrQuit(message1->SetLength(sizeof(sMysteryText)));
    message1->Write(0, sMysteryText);

    message2 = sMessagePool->New(Message::kTypeIp6, 0);
    VerifyOrQuit(message2 != nullptr, "Null Message");
    SuccessOrQuit(message2->SetLength(sizeof(sHelloText)));
    message2->Write(0, sHelloText);

    aNcpBuffer.InFrameBegin(aPriority);
    SuccessOrQuit(aNcpBuffer.InFrameFeedMessage(message1));
    SuccessOrQuit(aNcpBuffer.InFrameFeedData(sOpenThreadText, sizeof(sOpenThreadText)));
    SuccessOrQuit(aNcpBuffer.InFrameFeedMessage(message2));
    SuccessOrQuit(aNcpBuffer.InFrameEnd());

    VerifyOrQuit(oldContext.mFrameAddedCount + 1 == sContext.mFrameAddedCount, "FrameAddedCallback failed.");
    VerifyOrQuit(oldContext.mFrameRemovedCount == sContext.mFrameRemovedCount, "FrameRemovedCallback failed.");
}

void VerifyAndRemoveFrame2(Spinel::Buffer &aNcpBuffer)
{
    CallbackContext oldContext = sContext;

    VerifyOrQuit(aNcpBuffer.OutFrameGetLength() == kTestFrame2Size);
    SuccessOrQuit(aNcpBuffer.OutFrameBegin());
    VerifyOrQuit(aNcpBuffer.OutFrameGetLength() == kTestFrame2Size);
    ReadAndVerifyContent(aNcpBuffer, sMysteryText, sizeof(sMysteryText));
    ReadAndVerifyContent(aNcpBuffer, sOpenThreadText, sizeof(sOpenThreadText));
    ReadAndVerifyContent(aNcpBuffer, sHelloText, sizeof(sHelloText));
    VerifyOrQuit(aNcpBuffer.OutFrameHasEnded(), "Frame longer than expected.");
    VerifyOrQuit(aNcpBuffer.OutFrameReadByte() == 0, "ReadByte() returned non-zero after end of frame.");
    sExpectedRemovedTag = aNcpBuffer.OutFrameGetTag();
    VerifyOrQuit(aNcpBuffer.OutFrameGetLength() == kTestFrame2Size);
    SuccessOrQuit(aNcpBuffer.OutFrameRemove());

    VerifyOrQuit(oldContext.mFrameAddedCount == sContext.mFrameAddedCount, "FrameAddedCallback failed.");
    VerifyOrQuit(oldContext.mFrameRemovedCount + 1 == sContext.mFrameRemovedCount, "FrameRemovedCallback failed.");
}

void WriteTestFrame3(Spinel::Buffer &aNcpBuffer, Spinel::Buffer::Priority aPriority)
{
    Message *       message1;
    CallbackContext oldContext = sContext;

    message1 = sMessagePool->New(Message::kTypeIp6, 0);
    VerifyOrQuit(message1 != nullptr, "Null Message");

    // An empty message with no content.
    SuccessOrQuit(message1->SetLength(0));

    aNcpBuffer.InFrameBegin(aPriority);
    SuccessOrQuit(aNcpBuffer.InFrameFeedMessage(message1));
    SuccessOrQuit(aNcpBuffer.InFrameFeedData(sMysteryText, sizeof(sMysteryText)));
    SuccessOrQuit(aNcpBuffer.InFrameEnd());

    VerifyOrQuit(oldContext.mFrameAddedCount + 1 == sContext.mFrameAddedCount, "FrameAddedCallback failed.");
    VerifyOrQuit(oldContext.mFrameRemovedCount == sContext.mFrameRemovedCount, "FrameRemovedCallback failed.");
}

void VerifyAndRemoveFrame3(Spinel::Buffer &aNcpBuffer)
{
    CallbackContext oldContext = sContext;

    VerifyOrQuit(aNcpBuffer.OutFrameGetLength() == sizeof(sMysteryText));
    SuccessOrQuit(aNcpBuffer.OutFrameBegin());
    VerifyOrQuit(aNcpBuffer.OutFrameGetLength() == sizeof(sMysteryText));
    ReadAndVerifyContent(aNcpBuffer, sMysteryText, sizeof(sMysteryText));
    VerifyOrQuit(aNcpBuffer.OutFrameHasEnded(), "Frame longer than expected.");
    VerifyOrQuit(aNcpBuffer.OutFrameReadByte() == 0, "ReadByte() returned non-zero after end of frame.");
    sExpectedRemovedTag = aNcpBuffer.OutFrameGetTag();
    VerifyOrQuit(aNcpBuffer.OutFrameGetLength() == sizeof(sMysteryText));
    SuccessOrQuit(aNcpBuffer.OutFrameRemove());

    VerifyOrQuit(oldContext.mFrameAddedCount == sContext.mFrameAddedCount, "FrameAddedCallback failed.");
    VerifyOrQuit(oldContext.mFrameRemovedCount + 1 == sContext.mFrameRemovedCount, "FrameRemovedCallback failed.");
}

void WriteTestFrame4(Spinel::Buffer &aNcpBuffer, Spinel::Buffer::Priority aPriority)
{
    CallbackContext oldContext = sContext;

    aNcpBuffer.InFrameBegin(aPriority);
    SuccessOrQuit(aNcpBuffer.InFrameFeedData(sOpenThreadText, sizeof(sOpenThreadText)));
    SuccessOrQuit(aNcpBuffer.InFrameEnd());

    VerifyOrQuit(oldContext.mFrameAddedCount + 1 == sContext.mFrameAddedCount, "FrameAddedCallback failed.");
    VerifyOrQuit(oldContext.mFrameRemovedCount == sContext.mFrameRemovedCount, "FrameRemovedCallback failed.");
}

void VerifyAndRemoveFrame4(Spinel::Buffer &aNcpBuffer)
{
    CallbackContext oldContext = sContext;

    VerifyOrQuit(aNcpBuffer.OutFrameGetLength() == sizeof(sOpenThreadText));
    SuccessOrQuit(aNcpBuffer.OutFrameBegin());
    VerifyOrQuit(aNcpBuffer.OutFrameGetLength() == sizeof(sOpenThreadText));
    ReadAndVerifyContent(aNcpBuffer, sOpenThreadText, sizeof(sOpenThreadText));
    VerifyOrQuit(aNcpBuffer.OutFrameHasEnded(), "Frame longer than expected.");
    VerifyOrQuit(aNcpBuffer.OutFrameReadByte() == 0, "ReadByte() returned non-zero after end of frame.");
    sExpectedRemovedTag = aNcpBuffer.OutFrameGetTag();
    VerifyOrQuit(aNcpBuffer.OutFrameGetLength() == sizeof(sOpenThreadText));
    SuccessOrQuit(aNcpBuffer.OutFrameRemove());

    VerifyOrQuit(oldContext.mFrameAddedCount == sContext.mFrameAddedCount, "FrameAddedCallback failed.");
    VerifyOrQuit(oldContext.mFrameRemovedCount + 1 == sContext.mFrameRemovedCount, "FrameRemovedCallback failed.");
}

// This function implements the Spinel::Buffer tests
void TestBuffer(void)
{
    unsigned       i, j;
    uint8_t        buffer[kTestBufferSize];
    Spinel::Buffer ncpBuffer(buffer, kTestBufferSize);

    Message *                     message;
    uint8_t                       readBuffer[16];
    uint16_t                      readLen, readOffset;
    Spinel::Buffer::WritePosition pos1, pos2;

    sInstance    = testInitInstance();
    sMessagePool = &sInstance->Get<MessagePool>();

    for (i = 0; i < sizeof(buffer); i++)
    {
        buffer[i] = 0;
    }

    sContext.mFrameAddedCount   = 0;
    sContext.mFrameRemovedCount = 0;
    ClearTagHistory();

    // Set the callbacks.
    ncpBuffer.SetFrameAddedCallback(FrameAddedCallback, &sContext);
    ncpBuffer.SetFrameRemovedCallback(FrameRemovedCallback, &sContext);

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 1: Check initial buffer state");

    VerifyOrQuit(ncpBuffer.IsEmpty(), "Not empty after init.");
    VerifyOrQuit(ncpBuffer.InFrameGetLastTag() == Spinel::Buffer::kInvalidTag, "Incorrect tag after init.");
    VerifyOrQuit(ncpBuffer.OutFrameGetTag() == Spinel::Buffer::kInvalidTag, "Incorrect OutFrameTag after init.");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 2: Write and read a single frame");

    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityLow);
    DumpBuffer("\nBuffer after frame1 (low priority)", buffer, kTestBufferSize);
    printf("\nFrameLen is %u", ncpBuffer.OutFrameGetLength());
    VerifyAndRemoveFrame1(ncpBuffer);

    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    DumpBuffer("\nBuffer after frame1 (high priority)", buffer, kTestBufferSize);
    printf("\nFrameLen is %u", ncpBuffer.OutFrameGetLength());
    VerifyAndRemoveFrame1(ncpBuffer);

    printf("\nIterations: ");

    // Always add as low priority.
    for (j = 0; j < kTestIterationAttemps; j++)
    {
        printf("*");
        WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityLow);
        VerifyOrQuit(ncpBuffer.IsEmpty() == false, "IsEmpty() is incorrect when buffer is non-empty");

        VerifyAndRemoveFrame1(ncpBuffer);
        VerifyOrQuit(ncpBuffer.IsEmpty(), "IsEmpty() is incorrect when buffer is empty.");
    }

    // Always add as high priority.
    for (j = 0; j < kTestIterationAttemps; j++)
    {
        printf("*");
        WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityHigh);
        VerifyOrQuit(ncpBuffer.IsEmpty() == false, "IsEmpty() is incorrect when buffer is non-empty");

        VerifyAndRemoveFrame1(ncpBuffer);
        VerifyOrQuit(ncpBuffer.IsEmpty(), "IsEmpty() is incorrect when buffer is empty.");
    }

    // Every 5th add as high priority.
    for (j = 0; j < kTestIterationAttemps; j++)
    {
        printf("*");
        WriteTestFrame1(ncpBuffer, ((j % 5) == 0) ? Spinel::Buffer::kPriorityHigh : Spinel::Buffer::kPriorityLow);
        VerifyOrQuit(ncpBuffer.IsEmpty() == false, "IsEmpty() is incorrect when buffer is non-empty");

        VerifyAndRemoveFrame1(ncpBuffer);
        VerifyOrQuit(ncpBuffer.IsEmpty(), "IsEmpty() is incorrect when buffer is empty.");
    }

    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 3: Multiple frames write and read (same priority)");

    WriteTestFrame2(ncpBuffer, Spinel::Buffer::kPriorityLow);
    WriteTestFrame3(ncpBuffer, Spinel::Buffer::kPriorityLow);
    WriteTestFrame2(ncpBuffer, Spinel::Buffer::kPriorityLow);
    WriteTestFrame2(ncpBuffer, Spinel::Buffer::kPriorityLow);

    DumpBuffer("\nBuffer after multiple frames", buffer, kTestBufferSize);

    VerifyAndRemoveFrame2(ncpBuffer);
    VerifyAndRemoveFrame3(ncpBuffer);
    VerifyAndRemoveFrame2(ncpBuffer);
    VerifyAndRemoveFrame2(ncpBuffer);

    printf("\nIterations: ");

    // Repeat this multiple times.
    for (j = 0; j < kTestIterationAttemps; j++)
    {
        printf("*");

        WriteTestFrame2(ncpBuffer, Spinel::Buffer::kPriorityLow);
        WriteTestFrame3(ncpBuffer, Spinel::Buffer::kPriorityLow);
        WriteTestFrame2(ncpBuffer, Spinel::Buffer::kPriorityLow);

        VerifyAndRemoveFrame2(ncpBuffer);
        VerifyAndRemoveFrame3(ncpBuffer);

        WriteTestFrame2(ncpBuffer, Spinel::Buffer::kPriorityLow);
        WriteTestFrame3(ncpBuffer, Spinel::Buffer::kPriorityLow);

        VerifyAndRemoveFrame2(ncpBuffer);
        VerifyAndRemoveFrame2(ncpBuffer);
        VerifyAndRemoveFrame3(ncpBuffer);

        VerifyOrQuit(ncpBuffer.IsEmpty(), "IsEmpty() is incorrect when buffer is empty.");
    }

    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 4: Multiple frames write and read (mixed priority)");

    WriteTestFrame2(ncpBuffer, Spinel::Buffer::kPriorityLow);
    WriteTestFrame3(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    VerifyAndRemoveFrame3(ncpBuffer);
    VerifyAndRemoveFrame2(ncpBuffer);

    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityLow);
    WriteTestFrame2(ncpBuffer, Spinel::Buffer::kPriorityLow);
    WriteTestFrame3(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    WriteTestFrame4(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    VerifyAndRemoveFrame3(ncpBuffer);
    VerifyAndRemoveFrame4(ncpBuffer);
    VerifyAndRemoveFrame1(ncpBuffer);
    VerifyAndRemoveFrame2(ncpBuffer);

    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityLow);
    WriteTestFrame2(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    WriteTestFrame3(ncpBuffer, Spinel::Buffer::kPriorityLow);
    WriteTestFrame4(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    VerifyAndRemoveFrame2(ncpBuffer);
    VerifyAndRemoveFrame4(ncpBuffer);
    VerifyAndRemoveFrame1(ncpBuffer);
    VerifyAndRemoveFrame3(ncpBuffer);

    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityLow);
    WriteTestFrame2(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    WriteTestFrame3(ncpBuffer, Spinel::Buffer::kPriorityLow);
    WriteTestFrame4(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    VerifyAndRemoveFrame2(ncpBuffer);
    VerifyAndRemoveFrame4(ncpBuffer);
    VerifyAndRemoveFrame1(ncpBuffer);
    VerifyAndRemoveFrame3(ncpBuffer);

    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    WriteTestFrame2(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    WriteTestFrame3(ncpBuffer, Spinel::Buffer::kPriorityLow);
    WriteTestFrame4(ncpBuffer, Spinel::Buffer::kPriorityLow);
    VerifyAndRemoveFrame1(ncpBuffer);
    VerifyAndRemoveFrame2(ncpBuffer);
    VerifyAndRemoveFrame3(ncpBuffer);
    VerifyAndRemoveFrame4(ncpBuffer);

    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityLow);
    WriteTestFrame2(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    WriteTestFrame3(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    VerifyAndRemoveFrame2(ncpBuffer);
    WriteTestFrame4(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    VerifyAndRemoveFrame3(ncpBuffer);
    VerifyAndRemoveFrame4(ncpBuffer);
    VerifyAndRemoveFrame1(ncpBuffer);

    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 5: Frame discard when buffer full and partial read restart");

    printf("\nIterations: ");

    for (j = 0; j < kTestIterationAttemps; j++)
    {
        bool frame1IsHighPriority = ((j % 3) == 0);

        printf("*");

        WriteTestFrame2(ncpBuffer, Spinel::Buffer::kPriorityLow);
        WriteTestFrame3(ncpBuffer, Spinel::Buffer::kPriorityHigh);

        ncpBuffer.InFrameBegin((j % 2) == 0 ? Spinel::Buffer::kPriorityHigh : Spinel::Buffer::kPriorityLow);
        SuccessOrQuit(ncpBuffer.InFrameFeedData(sHelloText, sizeof(sHelloText)));

        message = sMessagePool->New(Message::kTypeIp6, 0);
        VerifyOrQuit(message != nullptr, "Null Message");
        SuccessOrQuit(message->SetLength(sizeof(sMysteryText)));
        message->Write(0, sMysteryText);

        SuccessOrQuit(ncpBuffer.InFrameFeedMessage(message));

        // Start writing a new frame in middle of an unfinished frame. Ensure the first one is discarded.
        WriteTestFrame1(ncpBuffer, frame1IsHighPriority ? Spinel::Buffer::kPriorityHigh : Spinel::Buffer::kPriorityLow);

        // Note that message will not be freed by the NCP buffer since the frame associated with it was discarded and
        // not yet finished/ended.
        otMessageFree(message);

        VerifyAndRemoveFrame3(ncpBuffer);

        // Start reading few bytes from the frame
        SuccessOrQuit(ncpBuffer.OutFrameBegin());
        ncpBuffer.OutFrameReadByte();
        ncpBuffer.OutFrameReadByte();
        ncpBuffer.OutFrameReadByte();

        // Now reset the read pointer and read/verify the frame from start.
        if (frame1IsHighPriority)
        {
            VerifyAndRemoveFrame1(ncpBuffer);
            VerifyAndRemoveFrame2(ncpBuffer);
        }
        else
        {
            VerifyAndRemoveFrame2(ncpBuffer);
            VerifyAndRemoveFrame1(ncpBuffer);
        }

        VerifyOrQuit(ncpBuffer.IsEmpty(), "IsEmpty() is incorrect when buffer is empty.");
    }

    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 6: Clear() and empty buffer method tests");

    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityLow);

    ncpBuffer.Clear();
    ClearTagHistory();

    VerifyOrQuit(ncpBuffer.InFrameGetLastTag() == Spinel::Buffer::kInvalidTag, "Incorrect last tag after Clear().");
    VerifyOrQuit(ncpBuffer.OutFrameGetTag() == Spinel::Buffer::kInvalidTag, "Incorrect OutFrameTag after Clear().");
    VerifyOrQuit(ncpBuffer.IsEmpty(), "IsEmpty() is incorrect when buffer is empty.");
    VerifyOrQuit(ncpBuffer.OutFrameHasEnded(), "OutFrameHasEnded() is incorrect when no data in buffer.");
    VerifyOrQuit(ncpBuffer.OutFrameRemove() == OT_ERROR_NOT_FOUND,
                 "Remove() returned incorrect error status when buffer is empty.");
    VerifyOrQuit(ncpBuffer.OutFrameGetLength() == 0,
                 "OutFrameGetLength() returned non-zero length when buffer is empty.");

    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityLow);
    VerifyAndRemoveFrame1(ncpBuffer);

    VerifyOrQuit(ncpBuffer.IsEmpty(), "IsEmpty() is incorrect when buffer is empty.");
    VerifyOrQuit(ncpBuffer.OutFrameHasEnded(), "OutFrameHasEnded() is incorrect when no data in buffer.");
    VerifyOrQuit(ncpBuffer.OutFrameRemove() == OT_ERROR_NOT_FOUND,
                 "Remove() returned incorrect error status when buffer is empty.");
    VerifyOrQuit(ncpBuffer.OutFrameGetLength() == 0,
                 "OutFrameGetLength() returned non-zero length when buffer is empty.");

    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 7: OutFrameRead() in parts\n");

    ncpBuffer.InFrameBegin(Spinel::Buffer::kPriorityLow);
    SuccessOrQuit(ncpBuffer.InFrameFeedData(sMottoText, sizeof(sMottoText)));
    SuccessOrQuit(ncpBuffer.InFrameEnd());

    SuccessOrQuit(ncpBuffer.OutFrameBegin());
    readOffset = 0;

    while ((readLen = ncpBuffer.OutFrameRead(sizeof(readBuffer), readBuffer)) != 0)
    {
        DumpBuffer("Read() returned", readBuffer, readLen);

        VerifyOrQuit(memcmp(readBuffer, sMottoText + readOffset, readLen) == 0,
                     "Read() does not match expected content.");

        readOffset += readLen;
    }

    VerifyOrQuit(readOffset == sizeof(sMottoText), "Read len does not match expected length.");

    SuccessOrQuit(ncpBuffer.OutFrameRemove());

    printf("\n -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 8: Remove a frame without reading it first");

    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityLow);
    WriteTestFrame2(ncpBuffer, Spinel::Buffer::kPriorityLow);
    VerifyOrQuit(ncpBuffer.OutFrameGetLength() == kTestFrame1Size);
    SuccessOrQuit(ncpBuffer.OutFrameRemove());
    VerifyAndRemoveFrame2(ncpBuffer);
    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 9: Check length when front frame gets changed (a higher priority frame is added)");
    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityLow);
    VerifyOrQuit(ncpBuffer.OutFrameGetLength() == kTestFrame1Size);
    WriteTestFrame3(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    VerifyOrQuit(ncpBuffer.OutFrameGetLength() == kTestFrame3Size);
    VerifyAndRemoveFrame3(ncpBuffer);
    VerifyAndRemoveFrame1(ncpBuffer);
    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 10: Active out frame remaining unchanged when a higher priority frame is written while reading it");
    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityLow);
    VerifyOrQuit(ncpBuffer.OutFrameGetLength() == kTestFrame1Size);
    SuccessOrQuit(ncpBuffer.OutFrameBegin());
    VerifyOrQuit(ncpBuffer.OutFrameGetLength() == kTestFrame1Size);
    ReadAndVerifyContent(ncpBuffer, sMottoText, sizeof(sMottoText));
    WriteTestFrame2(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    VerifyOrQuit(ncpBuffer.OutFrameGetLength() == kTestFrame1Size);
    ReadAndVerifyContent(ncpBuffer, sMysteryText, sizeof(sMysteryText));
    SuccessOrQuit(ncpBuffer.OutFrameBegin());
    VerifyOrQuit(ncpBuffer.OutFrameGetLength() == kTestFrame1Size);
    ReadAndVerifyContent(ncpBuffer, sMottoText, sizeof(sMottoText));
    ReadAndVerifyContent(ncpBuffer, sMysteryText, sizeof(sMysteryText));
    ReadAndVerifyContent(ncpBuffer, sMottoText, sizeof(sMottoText));
    ReadAndVerifyContent(ncpBuffer, sHelloText, sizeof(sHelloText));
    VerifyOrQuit(ncpBuffer.OutFrameHasEnded(), "Frame longer than expected.");
    WriteTestFrame3(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    WriteTestFrame4(ncpBuffer, Spinel::Buffer::kPriorityLow);
    VerifyOrQuit(ncpBuffer.OutFrameGetLength() == kTestFrame1Size);
    VerifyAndRemoveFrame1(ncpBuffer);
    VerifyAndRemoveFrame2(ncpBuffer);
    VerifyAndRemoveFrame3(ncpBuffer);
    VerifyAndRemoveFrame4(ncpBuffer);
    // Repeat test reversing frame priority orders.
    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    VerifyOrQuit(ncpBuffer.OutFrameGetLength() == kTestFrame1Size);
    SuccessOrQuit(ncpBuffer.OutFrameBegin());
    VerifyOrQuit(ncpBuffer.OutFrameGetLength() == kTestFrame1Size);
    ReadAndVerifyContent(ncpBuffer, sMottoText, sizeof(sMottoText));
    WriteTestFrame2(ncpBuffer, Spinel::Buffer::kPriorityLow);
    VerifyOrQuit(ncpBuffer.OutFrameGetLength() == kTestFrame1Size);
    ReadAndVerifyContent(ncpBuffer, sMysteryText, sizeof(sMysteryText));
    SuccessOrQuit(ncpBuffer.OutFrameBegin());
    VerifyOrQuit(ncpBuffer.OutFrameGetLength() == kTestFrame1Size);
    ReadAndVerifyContent(ncpBuffer, sMottoText, sizeof(sMottoText));
    ReadAndVerifyContent(ncpBuffer, sMysteryText, sizeof(sMysteryText));
    ReadAndVerifyContent(ncpBuffer, sMottoText, sizeof(sMottoText));
    ReadAndVerifyContent(ncpBuffer, sHelloText, sizeof(sHelloText));
    VerifyOrQuit(ncpBuffer.OutFrameHasEnded(), "Frame longer than expected.");
    WriteTestFrame3(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    WriteTestFrame4(ncpBuffer, Spinel::Buffer::kPriorityLow);
    VerifyOrQuit(ncpBuffer.OutFrameGetLength() == kTestFrame1Size);
    VerifyAndRemoveFrame1(ncpBuffer);
    VerifyAndRemoveFrame3(ncpBuffer);
    VerifyAndRemoveFrame2(ncpBuffer);
    VerifyAndRemoveFrame4(ncpBuffer);
    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\n Test 11: Read and remove in middle of an active input frame write");
    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityLow);
    ncpBuffer.InFrameBegin(Spinel::Buffer::kPriorityHigh);
    SuccessOrQuit(ncpBuffer.InFrameFeedData(sOpenThreadText, sizeof(sOpenThreadText)));
    VerifyAndRemoveFrame1(ncpBuffer);
    VerifyOrQuit(ncpBuffer.IsEmpty());
    SuccessOrQuit(ncpBuffer.InFrameEnd());
    VerifyAndRemoveFrame4(ncpBuffer);
    // Repeat the test reversing priorities
    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    ncpBuffer.InFrameBegin(Spinel::Buffer::kPriorityLow);
    SuccessOrQuit(ncpBuffer.InFrameFeedData(sOpenThreadText, sizeof(sOpenThreadText)));
    VerifyAndRemoveFrame1(ncpBuffer);
    VerifyOrQuit(ncpBuffer.IsEmpty());
    SuccessOrQuit(ncpBuffer.InFrameEnd());
    VerifyAndRemoveFrame4(ncpBuffer);
    // Repeat the test with same priorities
    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    ncpBuffer.InFrameBegin(Spinel::Buffer::kPriorityHigh);
    SuccessOrQuit(ncpBuffer.InFrameFeedData(sOpenThreadText, sizeof(sOpenThreadText)));
    VerifyAndRemoveFrame1(ncpBuffer);
    VerifyOrQuit(ncpBuffer.IsEmpty());
    SuccessOrQuit(ncpBuffer.InFrameEnd());
    VerifyAndRemoveFrame4(ncpBuffer);
    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\n Test 12: Check returned error status");
    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityLow);
    ncpBuffer.InFrameBegin(Spinel::Buffer::kPriorityHigh);
    VerifyOrQuit(ncpBuffer.InFrameFeedData(buffer, sizeof(buffer)) == OT_ERROR_NO_BUFS);
    VerifyAndRemoveFrame1(ncpBuffer);
    VerifyOrQuit(ncpBuffer.IsEmpty());

    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityLow);
    WriteTestFrame2(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    // Ensure writes with starting `InFrameBegin()` fail
    VerifyOrQuit(ncpBuffer.InFrameFeedData(sOpenThreadText, 1) == OT_ERROR_INVALID_STATE);
    VerifyOrQuit(ncpBuffer.InFrameFeedData(sOpenThreadText, 0) == OT_ERROR_INVALID_STATE);
    VerifyOrQuit(ncpBuffer.InFrameFeedData(sOpenThreadText, 0) == OT_ERROR_INVALID_STATE);
    VerifyOrQuit(ncpBuffer.InFrameEnd() == OT_ERROR_INVALID_STATE);
    message = sMessagePool->New(Message::kTypeIp6, 0);
    VerifyOrQuit(message != nullptr, "Null Message");
    SuccessOrQuit(message->SetLength(sizeof(sMysteryText)));
    message->Write(0, sMysteryText);
    VerifyOrQuit(ncpBuffer.InFrameFeedMessage(message) == OT_ERROR_INVALID_STATE);
    message->Free();
    VerifyOrQuit(ncpBuffer.InFrameEnd() == OT_ERROR_INVALID_STATE);
    VerifyAndRemoveFrame2(ncpBuffer);
    VerifyAndRemoveFrame1(ncpBuffer);
    VerifyOrQuit(ncpBuffer.IsEmpty());
    VerifyOrQuit(ncpBuffer.OutFrameBegin() == OT_ERROR_NOT_FOUND, "OutFrameBegin() failed on empty queue");
    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    VerifyAndRemoveFrame1(ncpBuffer);
    VerifyOrQuit(ncpBuffer.IsEmpty());
    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\n Test 13: Ensure we can utilize the full buffer size when frames removed during write");
    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    WriteTestFrame2(ncpBuffer, Spinel::Buffer::kPriorityLow);
    ncpBuffer.InFrameBegin(Spinel::Buffer::kPriorityHigh);
    VerifyAndRemoveFrame1(ncpBuffer);
    VerifyAndRemoveFrame2(ncpBuffer);
    SuccessOrQuit(ncpBuffer.InFrameFeedData(buffer, sizeof(buffer) - 4));
    SuccessOrQuit(ncpBuffer.InFrameEnd());
    SuccessOrQuit(ncpBuffer.OutFrameRemove());
    // Repeat the test with a low priority buffer write
    WriteTestFrame1(ncpBuffer, Spinel::Buffer::kPriorityHigh);
    WriteTestFrame2(ncpBuffer, Spinel::Buffer::kPriorityLow);
    ncpBuffer.InFrameBegin(Spinel::Buffer::kPriorityLow);
    VerifyAndRemoveFrame1(ncpBuffer);
    VerifyAndRemoveFrame2(ncpBuffer);
    SuccessOrQuit(ncpBuffer.InFrameFeedData(buffer, sizeof(buffer) - 4));
    SuccessOrQuit(ncpBuffer.InFrameEnd());
    SuccessOrQuit(ncpBuffer.OutFrameRemove());
    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\n Test 14: Test InFrameOverwrite ");
    printf("\nIterations: ");

    for (j = 0; j < kTestIterationAttemps; j++)
    {
        uint16_t                 index;
        bool                     addExtra = ((j % 7) != 0);
        Spinel::Buffer::Priority priority;

        printf("*");
        priority = ((j % 3) == 0) ? Spinel::Buffer::kPriorityHigh : Spinel::Buffer::kPriorityLow;
        index    = static_cast<uint16_t>(j % sizeof(sHexText));
        ncpBuffer.InFrameBegin(priority);
        SuccessOrQuit(ncpBuffer.InFrameFeedData(sHexText, index));
        SuccessOrQuit(ncpBuffer.InFrameGetPosition(pos1));
        SuccessOrQuit(ncpBuffer.InFrameFeedData(sMysteryText, sizeof(sHexText) - index));
        VerifyOrQuit(ncpBuffer.InFrameGetDistance(pos1) == sizeof(sHexText) - index);

        if (addExtra)
        {
            SuccessOrQuit(ncpBuffer.InFrameFeedData(sHelloText, sizeof(sHelloText)));
        }

        SuccessOrQuit(ncpBuffer.InFrameOverwrite(pos1, sHexText + index, sizeof(sHexText) - index),
                      "InFrameOverwrite() failed.");
        VerifyOrQuit(ncpBuffer.InFrameGetDistance(pos1) ==
                         sizeof(sHexText) - index + (addExtra ? sizeof(sHelloText) : 0),
                     "InFrameGetDistance() failed");
        SuccessOrQuit(ncpBuffer.InFrameEnd());
        VerifyOrQuit(ncpBuffer.InFrameGetPosition(pos2) == OT_ERROR_INVALID_STATE);
        VerifyOrQuit(ncpBuffer.InFrameOverwrite(pos1, sHexText, 0) != OT_ERROR_NONE);
        SuccessOrQuit(ncpBuffer.OutFrameBegin());
        ReadAndVerifyContent(ncpBuffer, sHexText, sizeof(sHexText));

        if (addExtra)
        {
            ReadAndVerifyContent(ncpBuffer, sHelloText, sizeof(sHelloText));
        }

        SuccessOrQuit(ncpBuffer.OutFrameRemove());
        VerifyOrQuit(ncpBuffer.InFrameGetPosition(pos2) == OT_ERROR_INVALID_STATE);
    }

    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\n Test 15: Test InFrameReset()");
    printf("\nIterations: ");

    for (j = 0; j < kTestIterationAttemps; j++)
    {
        uint16_t                 index;
        bool                     addExtra = ((j % 7) != 0);
        Spinel::Buffer::Priority priority;

        printf("*");
        priority = ((j % 3) == 0) ? Spinel::Buffer::kPriorityHigh : Spinel::Buffer::kPriorityLow;
        index    = static_cast<uint16_t>(j % sizeof(sHexText));
        ncpBuffer.InFrameBegin(priority);
        SuccessOrQuit(ncpBuffer.InFrameFeedData(sHexText, index));
        SuccessOrQuit(ncpBuffer.InFrameGetPosition(pos1));
        SuccessOrQuit(ncpBuffer.InFrameFeedData(sMysteryText, sizeof(sHexText) - index));
        VerifyOrQuit(ncpBuffer.InFrameGetDistance(pos1) == sizeof(sHexText) - index);

        if (addExtra)
        {
            SuccessOrQuit(ncpBuffer.InFrameFeedData(sHelloText, sizeof(sHelloText)));
        }

        SuccessOrQuit(ncpBuffer.InFrameReset(pos1));
        SuccessOrQuit(ncpBuffer.InFrameFeedData(sHexText + index, sizeof(sHexText) - index));

        if (addExtra)
        {
            SuccessOrQuit(ncpBuffer.InFrameReset(pos1));
            SuccessOrQuit(ncpBuffer.InFrameFeedData(sHexText + index, sizeof(sHexText) - index));
        }

        VerifyOrQuit(ncpBuffer.InFrameGetDistance(pos1) == sizeof(sHexText) - index);
        SuccessOrQuit(ncpBuffer.InFrameEnd());
        SuccessOrQuit(ncpBuffer.OutFrameBegin());
        ReadAndVerifyContent(ncpBuffer, sHexText, sizeof(sHexText));
        SuccessOrQuit(ncpBuffer.OutFrameRemove());
    }

    printf(" -- PASS\n");

    testFreeInstance(sInstance);
}

/**
 * NCP Buffer Fuzz testing
 *
 * Randomly decide if to read or write a frame to the NCP buffer (use `kReadProbability` in percent to control the
 * behavior).
 *
 * When writing a frame, use a random length (1 up to `kMaxFrameLen`) and generate random byte sequences.
 * When reading a frame ensure the length and the content matches what was written earlier.
 * Handle the cases where buffer gets full or empty.
 *
 */

enum
{
    kFuzTestBufferSize            = 2000,   // Size of the buffer used during fuzz testing
    kFuzTestIterationAttempts     = 500000, // Number of iterations  to run
    kLensArraySize                = 500,    // Size of "Lengths" array.
    kMaxFrameLen                  = 400,    // Maximum frame length
    kReadProbability              = 50,     // Probability (in percent) to randomly choose to read vs write frame
    kHighPriorityProbability      = 20,     // Probability (in percent) to write a high priority frame
    kUseTrueRandomNumberGenerator = 1,      // To use true random number generator or not.
};

uint8_t  sFrameBuffer[kNumPrios][kFuzTestBufferSize];
uint32_t sFrameBufferTailIndex[kNumPrios] = {0};

uint32_t GetRandom(uint32_t max)
{
    uint32_t value;

    if (kUseTrueRandomNumberGenerator)
    {
        SuccessOrQuit(Random::Crypto::FillBuffer(reinterpret_cast<uint8_t *>(&value), sizeof(value)));
    }
    else
    {
        value = Random::NonCrypto::GetUint32();
    }

    return value % max;
}

otError WriteRandomFrame(uint32_t aLength, Spinel::Buffer &aNcpBuffer, Spinel::Buffer::Priority aPriority)
{
    otError         error;
    uint8_t         byte;
    uint8_t         priority   = static_cast<uint8_t>(aPriority);
    CallbackContext oldContext = sContext;
    uint32_t        tail       = sFrameBufferTailIndex[priority];

    aNcpBuffer.InFrameBegin(aPriority);

    while (aLength--)
    {
        byte = static_cast<uint8_t>(GetRandom(256));
        SuccessOrExit(error = aNcpBuffer.InFrameFeedData(&byte, sizeof(byte)));
        sFrameBuffer[priority][tail++] = byte;
    }

    SuccessOrExit(error = aNcpBuffer.InFrameEnd());

    sFrameBufferTailIndex[priority] = tail;

    // check the callbacks
    VerifyOrQuit(oldContext.mFrameAddedCount + 1 == sContext.mFrameAddedCount, "FrameAddedCallback failed.");
    VerifyOrQuit(oldContext.mFrameRemovedCount == sContext.mFrameRemovedCount, "FrameRemovedCallback failed.");

exit:
    return error;
}

otError ReadRandomFrame(uint32_t aLength, Spinel::Buffer &aNcpBuffer, uint8_t priority)
{
    CallbackContext oldContext = sContext;

    SuccessOrQuit(aNcpBuffer.OutFrameBegin());
    VerifyOrQuit(aNcpBuffer.OutFrameGetLength() == aLength);

    // Read and verify that the content is same as sFrameBuffer values...
    ReadAndVerifyContent(aNcpBuffer, sFrameBuffer[priority], static_cast<uint16_t>(aLength));
    sExpectedRemovedTag = aNcpBuffer.OutFrameGetTag();

    SuccessOrQuit(aNcpBuffer.OutFrameRemove());

    sFrameBufferTailIndex[priority] -= aLength;
    memmove(sFrameBuffer[priority], sFrameBuffer[priority] + aLength, sFrameBufferTailIndex[priority]);

    // If successful check the callbacks
    VerifyOrQuit(oldContext.mFrameAddedCount == sContext.mFrameAddedCount, "FrameAddedCallback failed.");
    VerifyOrQuit(oldContext.mFrameRemovedCount + 1 == sContext.mFrameRemovedCount, "FrameRemovedCallback failed.");

    return OT_ERROR_NONE;
}

// This runs a fuzz test of NCP buffer
void TestFuzzBuffer(void)
{
    uint8_t        buffer[kFuzTestBufferSize];
    Spinel::Buffer ncpBuffer(buffer, kFuzTestBufferSize);

    uint32_t lensArray[kNumPrios][kLensArraySize]; // Keeps track of length of written frames so far
    uint32_t lensArrayStart[kNumPrios];
    uint32_t lensArrayCount[kNumPrios];

    sInstance    = testInitInstance();
    sMessagePool = &sInstance->Get<MessagePool>();

    memset(buffer, 0, sizeof(buffer));

    memset(lensArray, 0, sizeof(lensArray));
    memset(lensArrayStart, 0, sizeof(lensArrayStart));
    memset(lensArrayCount, 0, sizeof(lensArrayCount));

    sContext.mFrameAddedCount   = 0;
    sContext.mFrameRemovedCount = 0;
    ClearTagHistory();

    ncpBuffer.SetFrameAddedCallback(FrameAddedCallback, &sContext);
    ncpBuffer.SetFrameRemovedCallback(FrameRemovedCallback, &sContext);

    for (uint32_t iter = 0; iter < kFuzTestIterationAttempts; iter++)
    {
        bool shouldRead;

        if (lensArrayCount[0] == 0 && lensArrayCount[1] == 0)
        {
            shouldRead = false;
        }
        else if (lensArrayCount[0] == kLensArraySize - 1 || lensArrayCount[1] == kLensArraySize - 1)
        {
            shouldRead = true;
        }
        else
        {
            // Randomly decide to read or write.
            shouldRead = (GetRandom(100) < kReadProbability);
        }

        if (shouldRead)
        {
            uint32_t len;
            uint8_t  priority;

            priority = (lensArrayCount[Spinel::Buffer::kPriorityHigh] != 0) ? Spinel::Buffer::kPriorityHigh
                                                                            : Spinel::Buffer::kPriorityLow;

            len                      = lensArray[priority][lensArrayStart[priority]];
            lensArrayStart[priority] = (lensArrayStart[priority] + 1) % kLensArraySize;
            lensArrayCount[priority]--;

            printf("R%c%d ", priority == Spinel::Buffer::kPriorityHigh ? 'H' : 'L', len);

            SuccessOrQuit(ReadRandomFrame(len, ncpBuffer, priority), "Failed to read random frame.");
        }
        else
        {
            uint32_t                 len = GetRandom(kMaxFrameLen) + 1;
            Spinel::Buffer::Priority priority;

            if (GetRandom(100) < kHighPriorityProbability)
            {
                priority = Spinel::Buffer::kPriorityHigh;
            }
            else
            {
                priority = Spinel::Buffer::kPriorityLow;
            }

            if (WriteRandomFrame(len, ncpBuffer, priority) == OT_ERROR_NONE)
            {
                lensArray[priority][(lensArrayStart[priority] + lensArrayCount[priority]) % kLensArraySize] = len;
                lensArrayCount[priority]++;

                printf("W%c%d ", priority == Spinel::Buffer::kPriorityHigh ? 'H' : 'L', len);
            }
            else
            {
                printf("FULL ");
            }
        }

        if (lensArrayCount[0] == 0 && lensArrayCount[1] == 0)
        {
            VerifyOrQuit(ncpBuffer.IsEmpty());
            printf("EMPTY ");
        }
    }

    printf("\n -- PASS\n");

    testFreeInstance(sInstance);
}

} // namespace Spinel
} // namespace ot

int main(void)
{
    ot::Spinel::TestBuffer();
    ot::Spinel::TestFuzzBuffer();
    printf("\nAll tests passed.\n");
    return 0;
}
