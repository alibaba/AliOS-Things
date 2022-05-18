/*
 *  Copyright (c) 2020, The OpenThread Authors.
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

#include <openthread/platform/flash.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "utils/flash.hpp"

#include "test_platform.h"
#include "test_util.h"

namespace ot {

void TestFlash(void)
{
#if OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
    uint8_t readBuffer[256];
    uint8_t writeBuffer[256];

    Instance *instance = testInitInstance();
    Flash     flash(*instance);

    for (uint32_t i = 0; i < sizeof(readBuffer); i++)
    {
        readBuffer[i] = i & 0xff;
    }

    flash.Init();

    // No records in settings

    VerifyOrQuit(flash.Delete(0, 0) == kErrorNotFound);
    VerifyOrQuit(flash.Get(0, 0, nullptr, nullptr) == kErrorNotFound);

    // Multiple records with different keys

    for (uint16_t key = 0; key < 16; key++)
    {
        uint16_t length = key;

        SuccessOrQuit(flash.Add(key, writeBuffer, length));
    }

    for (uint16_t key = 0; key < 16; key++)
    {
        uint16_t length = key;

        SuccessOrQuit(flash.Get(key, 0, readBuffer, &length));
        VerifyOrQuit(length == key, "Get() did not return expected length");
        VerifyOrQuit(memcmp(readBuffer, writeBuffer, length) == 0, "Get() did not return expected value");
    }

    for (uint16_t key = 0; key < 16; key++)
    {
        SuccessOrQuit(flash.Delete(key, 0));
    }

    for (uint16_t key = 0; key < 16; key++)
    {
        VerifyOrQuit(flash.Delete(key, 0) == kErrorNotFound);
        VerifyOrQuit(flash.Get(key, 0, nullptr, nullptr) == kErrorNotFound);
    }

    // Multiple records with the same key

    for (uint16_t index = 0; index < 16; index++)
    {
        uint16_t length = index;

        SuccessOrQuit(flash.Add(0, writeBuffer, length));
    }

    for (uint16_t index = 0; index < 16; index++)
    {
        uint16_t length = index;

        SuccessOrQuit(flash.Get(0, index, readBuffer, &length));
        VerifyOrQuit(length == index, "Get() did not return expected length");
        VerifyOrQuit(memcmp(readBuffer, writeBuffer, length) == 0, "Get() did not return expected value");
    }

    for (uint16_t index = 0; index < 16; index++)
    {
        SuccessOrQuit(flash.Delete(0, 0));
    }

    VerifyOrQuit(flash.Delete(0, 0) == kErrorNotFound);
    VerifyOrQuit(flash.Get(0, 0, nullptr, nullptr) == kErrorNotFound);

    // Multiple records with the same key

    for (uint16_t index = 0; index < 16; index++)
    {
        uint16_t length = index;

        if ((index % 4) == 0)
        {
            SuccessOrQuit(flash.Set(0, writeBuffer, length));
        }
        else
        {
            SuccessOrQuit(flash.Add(0, writeBuffer, length));
        }
    }

    for (uint16_t index = 0; index < 4; index++)
    {
        uint16_t length = index + 12;

        SuccessOrQuit(flash.Get(0, index, readBuffer, &length));
        VerifyOrQuit(length == (index + 12), "Get() did not return expected length");
        VerifyOrQuit(memcmp(readBuffer, writeBuffer, length) == 0, "Get() did not return expected value");
    }

    for (uint16_t index = 0; index < 4; index++)
    {
        SuccessOrQuit(flash.Delete(0, 0));
    }

    VerifyOrQuit(flash.Delete(0, 0) == kErrorNotFound);
    VerifyOrQuit(flash.Get(0, 0, nullptr, nullptr) == kErrorNotFound);

    // Wipe()

    for (uint16_t key = 0; key < 16; key++)
    {
        uint16_t length = key;

        SuccessOrQuit(flash.Add(key, writeBuffer, length));
    }

    flash.Wipe();

    for (uint16_t key = 0; key < 16; key++)
    {
        VerifyOrQuit(flash.Delete(key, 0) == kErrorNotFound);
        VerifyOrQuit(flash.Get(key, 0, nullptr, nullptr) == kErrorNotFound);
    }

    // Test swap

    for (uint16_t index = 0; index < 4096; index++)
    {
        uint16_t key    = index & 0xf;
        uint16_t length = index & 0xf;

        SuccessOrQuit(flash.Set(key, writeBuffer, length));
    }

    for (uint16_t key = 0; key < 16; key++)
    {
        uint16_t length = key;

        SuccessOrQuit(flash.Get(key, 0, readBuffer, &length));
        VerifyOrQuit(length == key, "Get() did not return expected length");
        VerifyOrQuit(memcmp(readBuffer, writeBuffer, length) == 0, "Get() did not return expected value");
    }
#endif // OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
}

} // namespace ot

int main(void)
{
    ot::TestFlash();
    printf("All tests passed\n");
    return 0;
}
