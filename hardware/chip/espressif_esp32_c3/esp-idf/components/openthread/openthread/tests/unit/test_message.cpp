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

#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/message.hpp"
#include "common/random.hpp"

#include "test_platform.h"
#include "test_util.hpp"

namespace ot {

void TestMessage(void)
{
    enum : uint16_t
    {
        kMaxSize    = (kBufferSize * 3 + 24),
        kOffsetStep = 101,
        kLengthStep = 21,
    };

    Instance *   instance;
    MessagePool *messagePool;
    Message *    message;
    Message *    message2;
    uint8_t      writeBuffer[kMaxSize];
    uint8_t      readBuffer[kMaxSize];
    uint8_t      zeroBuffer[kMaxSize];

    memset(zeroBuffer, 0, sizeof(zeroBuffer));

    instance = static_cast<Instance *>(testInitInstance());
    VerifyOrQuit(instance != nullptr);

    messagePool = &instance->Get<MessagePool>();

    Random::NonCrypto::FillBuffer(writeBuffer, kMaxSize);

    VerifyOrQuit((message = messagePool->New(Message::kTypeIp6, 0)) != nullptr);
    SuccessOrQuit(message->SetLength(kMaxSize));
    message->WriteBytes(0, writeBuffer, kMaxSize);
    SuccessOrQuit(message->Read(0, readBuffer, kMaxSize));
    VerifyOrQuit(memcmp(writeBuffer, readBuffer, kMaxSize) == 0);
    VerifyOrQuit(message->CompareBytes(0, readBuffer, kMaxSize));
    VerifyOrQuit(message->Compare(0, readBuffer));
    VerifyOrQuit(message->GetLength() == kMaxSize);

    for (uint16_t offset = 0; offset < kMaxSize; offset++)
    {
        for (uint16_t length = 0; length <= kMaxSize - offset; length++)
        {
            for (uint16_t i = 0; i < length; i++)
            {
                writeBuffer[offset + i]++;
            }

            message->WriteBytes(offset, &writeBuffer[offset], length);

            SuccessOrQuit(message->Read(0, readBuffer, kMaxSize));
            VerifyOrQuit(memcmp(writeBuffer, readBuffer, kMaxSize) == 0);
            VerifyOrQuit(message->Compare(0, writeBuffer));

            memset(readBuffer, 0, sizeof(readBuffer));
            SuccessOrQuit(message->Read(offset, readBuffer, length));
            VerifyOrQuit(memcmp(readBuffer, &writeBuffer[offset], length) == 0);
            VerifyOrQuit(memcmp(&readBuffer[length], zeroBuffer, kMaxSize - length) == 0, "read after length");

            VerifyOrQuit(message->CompareBytes(offset, &writeBuffer[offset], length));

            if (length == 0)
            {
                continue;
            }

            // Change the first byte, and then last byte, and verify that
            // `CompareBytes()` correctly fails.

            writeBuffer[offset]++;
            VerifyOrQuit(!message->CompareBytes(offset, &writeBuffer[offset], length));
            writeBuffer[offset]--;

            writeBuffer[offset + length - 1]++;
            VerifyOrQuit(!message->CompareBytes(offset, &writeBuffer[offset], length));
            writeBuffer[offset + length - 1]--;
        }

        // Verify `ReadBytes()` behavior when requested read length goes beyond available bytes in the message.

        for (uint16_t length = kMaxSize - offset + 1; length <= kMaxSize + 1; length++)
        {
            uint16_t readLength;

            memset(readBuffer, 0, sizeof(readBuffer));
            readLength = message->ReadBytes(offset, readBuffer, length);

            VerifyOrQuit(readLength < length, "Message::ReadBytes() returned longer length");
            VerifyOrQuit(readLength == kMaxSize - offset);
            VerifyOrQuit(memcmp(readBuffer, &writeBuffer[offset], readLength) == 0);
            VerifyOrQuit(memcmp(&readBuffer[readLength], zeroBuffer, kMaxSize - readLength) == 0, "read after length");

            VerifyOrQuit(!message->CompareBytes(offset, readBuffer, length));
            VerifyOrQuit(message->CompareBytes(offset, readBuffer, readLength));
        }
    }

    VerifyOrQuit(message->GetLength() == kMaxSize);

    // Test `Message::CopyTo()` behavior.

    VerifyOrQuit((message2 = messagePool->New(Message::kTypeIp6, 0)) != nullptr);
    SuccessOrQuit(message2->SetLength(kMaxSize));

    for (uint16_t srcOffset = 0; srcOffset < kMaxSize; srcOffset += kOffsetStep)
    {
        for (uint16_t dstOffset = 0; dstOffset < kMaxSize; dstOffset += kOffsetStep)
        {
            for (uint16_t length = 0; length <= kMaxSize - dstOffset; length += kLengthStep)
            {
                uint16_t bytesCopied;

                message2->WriteBytes(0, zeroBuffer, kMaxSize);

                bytesCopied = message->CopyTo(srcOffset, dstOffset, length, *message2);

                if (srcOffset + length <= kMaxSize)
                {
                    VerifyOrQuit(bytesCopied == length, "CopyTo() failed");
                }
                else
                {
                    VerifyOrQuit(bytesCopied == kMaxSize - srcOffset, "CopyTo() failed");
                }

                SuccessOrQuit(message2->Read(0, readBuffer, kMaxSize));

                VerifyOrQuit(memcmp(&readBuffer[0], zeroBuffer, dstOffset) == 0, "read before length");
                VerifyOrQuit(memcmp(&readBuffer[dstOffset], &writeBuffer[srcOffset], bytesCopied) == 0);
                VerifyOrQuit(
                    memcmp(&readBuffer[dstOffset + bytesCopied], zeroBuffer, kMaxSize - bytesCopied - dstOffset) == 0,
                    "read after length");

                VerifyOrQuit(message->CompareBytes(srcOffset, *message2, dstOffset, bytesCopied));
                VerifyOrQuit(message2->CompareBytes(dstOffset, *message, srcOffset, bytesCopied));
            }
        }
    }

    // Verify `CopyTo()` with same source and destination message and a backward copy.

    for (uint16_t srcOffset = 0; srcOffset < kMaxSize; srcOffset++)
    {
        uint16_t bytesCopied;

        message->WriteBytes(0, writeBuffer, kMaxSize);

        bytesCopied = message->CopyTo(srcOffset, 0, kMaxSize, *message);
        VerifyOrQuit(bytesCopied == kMaxSize - srcOffset, "CopyTo() failed");

        SuccessOrQuit(message->Read(0, readBuffer, kMaxSize));

        VerifyOrQuit(memcmp(&readBuffer[0], &writeBuffer[srcOffset], bytesCopied) == 0,
                     "CopyTo() changed before srcOffset");
        VerifyOrQuit(memcmp(&readBuffer[bytesCopied], &writeBuffer[bytesCopied], kMaxSize - bytesCopied) == 0,
                     "CopyTo() write error");
    }

    // Verify `AppendBytesFromMessage()` with two different messages as source and destination.

    message->WriteBytes(0, writeBuffer, kMaxSize);

    for (uint16_t srcOffset = 0; srcOffset < kMaxSize; srcOffset += kOffsetStep)
    {
        for (uint16_t dstOffset = 0; dstOffset < kMaxSize; dstOffset += kOffsetStep)
        {
            for (uint16_t length = 0; length <= kMaxSize - srcOffset; length += kLengthStep)
            {
                IgnoreError(message2->SetLength(0));
                SuccessOrQuit(message2->AppendBytes(zeroBuffer, dstOffset));

                SuccessOrQuit(message2->AppendBytesFromMessage(*message, srcOffset, length));

                VerifyOrQuit(message2->CompareBytes(dstOffset, *message, srcOffset, length));
            }

            VerifyOrQuit(message2->AppendBytesFromMessage(*message, srcOffset, kMaxSize - srcOffset + 1) ==
                         kErrorParse);
        }
    }

    // Verify `AppendBytesFromMessage()` with the same message as source and destination.

    for (uint16_t srcOffset = 0; srcOffset < kMaxSize; srcOffset += kOffsetStep)
    {
        uint16_t size = kMaxSize;

        for (uint16_t length = 0; length <= kMaxSize - srcOffset; length++)
        {
            // Reset the `message` to its original size.
            IgnoreError(message->SetLength(size));

            SuccessOrQuit(message->AppendBytesFromMessage(*message, srcOffset, length));

            VerifyOrQuit(message->CompareBytes(size, *message, srcOffset, length));
        }
    }

    message->Free();
    message2->Free();

    testFreeInstance(instance);
}

} // namespace ot

int main(void)
{
    ot::TestMessage();
    printf("All tests passed\n");
    return 0;
}
