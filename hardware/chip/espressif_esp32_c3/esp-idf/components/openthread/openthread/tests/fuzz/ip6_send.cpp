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

#define MAX_ITERATIONS 100

#include <stddef.h>

#include <openthread/instance.h>
#include <openthread/ip6.h>
#include <openthread/link.h>
#include <openthread/message.h>
#include <openthread/tasklet.h>
#include <openthread/thread.h>
#include <openthread/thread_ftd.h>

#include "fuzzer_platform.h"
#include "common/code_utils.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    const otPanId panId = 0xdead;

    otInstance *      instance = nullptr;
    otMessage *       message  = nullptr;
    otError           error    = OT_ERROR_NONE;
    otMessageSettings settings;

    VerifyOrExit(size > 0);

    FuzzerPlatformInit();

    instance = otInstanceInitSingle();
    IgnoreError(otLinkSetPanId(instance, panId));
    IgnoreError(otIp6SetEnabled(instance, true));
    IgnoreError(otThreadSetEnabled(instance, true));
    IgnoreError(otThreadBecomeLeader(instance));

    settings.mLinkSecurityEnabled = (data[0] & 0x1) != 0;
    settings.mPriority            = OT_MESSAGE_PRIORITY_NORMAL;

    message = otIp6NewMessage(instance, &settings);
    VerifyOrExit(message != nullptr, error = OT_ERROR_NO_BUFS);

    error = otMessageAppend(message, data + 1, static_cast<uint16_t>(size - 1));
    SuccessOrExit(error);

    error = otIp6Send(instance, message);

    message = nullptr;

    VerifyOrExit(!FuzzerPlatformResetWasRequested());

    for (int i = 0; i < MAX_ITERATIONS; i++)
    {
        while (otTaskletsArePending(instance))
        {
            otTaskletsProcess(instance);
        }

        FuzzerPlatformProcess(instance);
    }

exit:

    if (message != nullptr)
    {
        otMessageFree(message);
    }

    if (instance != nullptr)
    {
        otInstanceFinalize(instance);
    }

    return 0;
}
