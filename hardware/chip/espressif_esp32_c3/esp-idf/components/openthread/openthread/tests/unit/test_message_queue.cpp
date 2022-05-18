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

#include <stdarg.h>

#include "test_platform.h"

#include <openthread/config.h>

#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/message.hpp"

#include "test_util.h"

#define kNumTestMessages 5

static ot::Instance *   sInstance;
static ot::MessagePool *sMessagePool;

// This function verifies the content of the message queue to match the passed in messages
void VerifyMessageQueueContent(ot::MessageQueue &aMessageQueue, int aExpectedLength, ...)
{
    va_list      args;
    ot::Message *message;
    ot::Message *msgArg;

    va_start(args, aExpectedLength);

    if (aExpectedLength == 0)
    {
        message = aMessageQueue.GetHead();
        VerifyOrQuit(message == nullptr, "not empty when expected len is zero.");
    }
    else
    {
        for (message = aMessageQueue.GetHead(); message != nullptr; message = message->GetNext())
        {
            VerifyOrQuit(aExpectedLength != 0, "contains more entries than expected");

            msgArg = va_arg(args, ot::Message *);
            VerifyOrQuit(msgArg == message, "content does not match what is expected.");

            aExpectedLength--;
        }

        VerifyOrQuit(aExpectedLength == 0, "less entries than expected");
    }

    va_end(args);
}

void TestMessageQueue(void)
{
    ot::MessageQueue messageQueue;
    ot::Message *    messages[kNumTestMessages];
    uint16_t         msgCount, bufferCount;

    sInstance = testInitInstance();
    VerifyOrQuit(sInstance != nullptr);

    sMessagePool = &sInstance->Get<ot::MessagePool>();

    for (ot::Message *&msg : messages)
    {
        msg = sMessagePool->New(ot::Message::kTypeIp6, 0);
        VerifyOrQuit(msg != nullptr, "Message::New() failed");
    }

    VerifyMessageQueueContent(messageQueue, 0);

    // Enqueue 1 message and remove it
    messageQueue.Enqueue(*messages[0]);
    VerifyMessageQueueContent(messageQueue, 1, messages[0]);
    messageQueue.Dequeue(*messages[0]);
    VerifyMessageQueueContent(messageQueue, 0);

    // Enqueue 1 message at head and remove it
    messageQueue.Enqueue(*messages[0], ot::MessageQueue::kQueuePositionHead);
    VerifyMessageQueueContent(messageQueue, 1, messages[0]);
    messageQueue.Dequeue(*messages[0]);
    VerifyMessageQueueContent(messageQueue, 0);

    // Enqueue 5 messages
    messageQueue.Enqueue(*messages[0]);
    VerifyMessageQueueContent(messageQueue, 1, messages[0]);
    messageQueue.Enqueue(*messages[1]);
    VerifyMessageQueueContent(messageQueue, 2, messages[0], messages[1]);
    messageQueue.Enqueue(*messages[2]);
    VerifyMessageQueueContent(messageQueue, 3, messages[0], messages[1], messages[2]);
    messageQueue.Enqueue(*messages[3]);
    VerifyMessageQueueContent(messageQueue, 4, messages[0], messages[1], messages[2], messages[3]);
    messageQueue.Enqueue(*messages[4]);
    VerifyMessageQueueContent(messageQueue, 5, messages[0], messages[1], messages[2], messages[3], messages[4]);

    // Check the GetInfo()
    messageQueue.GetInfo(msgCount, bufferCount);
    VerifyOrQuit(msgCount == 5, "MessageQueue::GetInfo() failed.");

    // Remove from head
    messageQueue.Dequeue(*messages[0]);
    VerifyMessageQueueContent(messageQueue, 4, messages[1], messages[2], messages[3], messages[4]);

    // Remove a message in middle
    messageQueue.Dequeue(*messages[3]);
    VerifyMessageQueueContent(messageQueue, 3, messages[1], messages[2], messages[4]);

    // Remove from tail
    messageQueue.Dequeue(*messages[4]);
    VerifyMessageQueueContent(messageQueue, 2, messages[1], messages[2]);

    // Add after remove
    messageQueue.Enqueue(*messages[0]);
    VerifyMessageQueueContent(messageQueue, 3, messages[1], messages[2], messages[0]);
    messageQueue.Enqueue(*messages[3]);
    VerifyMessageQueueContent(messageQueue, 4, messages[1], messages[2], messages[0], messages[3]);

    // Remove from middle
    messageQueue.Dequeue(*messages[2]);
    VerifyMessageQueueContent(messageQueue, 3, messages[1], messages[0], messages[3]);

    // Add to head
    messageQueue.Enqueue(*messages[2], ot::MessageQueue::kQueuePositionHead);
    VerifyMessageQueueContent(messageQueue, 4, messages[2], messages[1], messages[0], messages[3]);

    // Remove from head
    messageQueue.Dequeue(*messages[2]);
    VerifyMessageQueueContent(messageQueue, 3, messages[1], messages[0], messages[3]);

    // Remove from head
    messageQueue.Dequeue(*messages[1]);
    VerifyMessageQueueContent(messageQueue, 2, messages[0], messages[3]);

    // Add to head
    messageQueue.Enqueue(*messages[1], ot::MessageQueue::kQueuePositionHead);
    VerifyMessageQueueContent(messageQueue, 3, messages[1], messages[0], messages[3]);

    // Add to tail
    messageQueue.Enqueue(*messages[2], ot::MessageQueue::kQueuePositionTail);
    VerifyMessageQueueContent(messageQueue, 4, messages[1], messages[0], messages[3], messages[2]);

    // Remove all messages.
    messageQueue.Dequeue(*messages[3]);
    VerifyMessageQueueContent(messageQueue, 3, messages[1], messages[0], messages[2]);
    messageQueue.Dequeue(*messages[1]);
    VerifyMessageQueueContent(messageQueue, 2, messages[0], messages[2]);
    messageQueue.Dequeue(*messages[2]);
    VerifyMessageQueueContent(messageQueue, 1, messages[0]);
    messageQueue.Dequeue(*messages[0]);
    VerifyMessageQueueContent(messageQueue, 0);

    testFreeInstance(sInstance);
}

// This function verifies the content of the message queue to match the passed in messages
void VerifyMessageQueueContentUsingOtApi(otMessageQueue *aQueue, int aExpectedLength, ...)
{
    va_list    args;
    otMessage *message;
    otMessage *msgArg;

    va_start(args, aExpectedLength);

    if (aExpectedLength == 0)
    {
        message = otMessageQueueGetHead(aQueue);
        VerifyOrQuit(message == nullptr, "not empty when expected len is zero.");
    }
    else
    {
        for (message = otMessageQueueGetHead(aQueue); message != nullptr;
             message = otMessageQueueGetNext(aQueue, message))
        {
            VerifyOrQuit(aExpectedLength != 0, "more entries than expected");

            msgArg = va_arg(args, otMessage *);
            VerifyOrQuit(msgArg == message, "does not match what is expected.");

            aExpectedLength--;
        }

        VerifyOrQuit(aExpectedLength == 0, "less entries than expected");
    }

    va_end(args);
}

// This test checks all the OpenThread C APIs for `otMessageQueue`
void TestMessageQueueOtApis(void)
{
    otMessage *    messages[kNumTestMessages];
    otMessage *    message;
    otMessageQueue queue, queue2;

    sInstance = testInitInstance();
    VerifyOrQuit(sInstance != nullptr);

    for (otMessage *&msg : messages)
    {
        msg = otIp6NewMessage(sInstance, nullptr);
        VerifyOrQuit(msg != nullptr, "otIp6NewMessage() failed.");
    }

    otMessageQueueInit(&queue);
    otMessageQueueInit(&queue2);

    // Check an empty queue.
    VerifyMessageQueueContentUsingOtApi(&queue, 0);

    // Add message to the queue and check the content
    otMessageQueueEnqueue(&queue, messages[0]);
    VerifyMessageQueueContentUsingOtApi(&queue, 1, messages[0]);
    otMessageQueueEnqueue(&queue, messages[1]);
    VerifyMessageQueueContentUsingOtApi(&queue, 2, messages[0], messages[1]);
    otMessageQueueEnqueueAtHead(&queue, messages[2]);
    VerifyMessageQueueContentUsingOtApi(&queue, 3, messages[2], messages[0], messages[1]);
    otMessageQueueEnqueue(&queue, messages[3]);
    VerifyMessageQueueContentUsingOtApi(&queue, 4, messages[2], messages[0], messages[1], messages[3]);

    // Remove elements and check the content
    otMessageQueueDequeue(&queue, messages[1]);
    VerifyMessageQueueContentUsingOtApi(&queue, 3, messages[2], messages[0], messages[3]);
    otMessageQueueDequeue(&queue, messages[0]);
    VerifyMessageQueueContentUsingOtApi(&queue, 2, messages[2], messages[3]);
    otMessageQueueDequeue(&queue, messages[3]);
    VerifyMessageQueueContentUsingOtApi(&queue, 1, messages[2]);

    // Check the failure cases for otMessageQueueGetNext()
    message = otMessageQueueGetNext(&queue, nullptr);
    VerifyOrQuit(message == nullptr, "otMessageQueueGetNext(queue, nullptr) did not return nullptr.");
    message = otMessageQueueGetNext(&queue, messages[1]);
    VerifyOrQuit(message == nullptr, "otMessageQueueGetNext() did not return nullptr for a message not in the queue.");

    // Check the failure case when attempting to do otMessageQueueGetNext() but passing in a wrong queue pointer.
    otMessageQueueEnqueue(&queue2, messages[0]);
    VerifyMessageQueueContentUsingOtApi(&queue2, 1, messages[0]);
    otMessageQueueEnqueue(&queue2, messages[1]);
    VerifyMessageQueueContentUsingOtApi(&queue2, 2, messages[0], messages[1]);

    message = otMessageQueueGetNext(&queue2, messages[0]);
    VerifyOrQuit(message == messages[1], "otMessageQueueGetNext() failed");
    message = otMessageQueueGetNext(&queue, messages[0]);
    VerifyOrQuit(message == nullptr, "otMessageQueueGetNext() did not return nullptr for message not in  the queue.");

    // Remove all element and make sure queue is empty
    otMessageQueueDequeue(&queue, messages[2]);
    VerifyMessageQueueContentUsingOtApi(&queue, 0);

    testFreeInstance(sInstance);
}

int main(void)
{
    TestMessageQueue();
    TestMessageQueueOtApis();
    printf("All tests passed\n");
    return 0;
}
