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

#define MAX_ITERATIONS 100

#include <stdlib.h>
#include <string.h>

#include <openthread/instance.h>
#include <openthread/ip6.h>
#include <openthread/link.h>
#include <openthread/ncp.h>
#include <openthread/tasklet.h>
#include <openthread/thread.h>
#include <openthread/thread_ftd.h>

#include "fuzzer_platform.h"
#include "common/code_utils.hpp"

static int HdlcSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    OT_UNUSED_VARIABLE(aBuf);
    OT_UNUSED_VARIABLE(aBufLength);

    return aBufLength;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    const otPanId panId = 0xdead;

    otInstance *instance = nullptr;
    uint8_t *   buf      = nullptr;

    VerifyOrExit(size <= 65536);

    FuzzerPlatformInit();

    instance = otInstanceInitSingle();
    otNcpHdlcInit(instance, HdlcSend);
    IgnoreError(otLinkSetPanId(instance, panId));
    IgnoreError(otIp6SetEnabled(instance, true));
    IgnoreError(otThreadSetEnabled(instance, true));
    IgnoreError(otThreadBecomeLeader(instance));

    buf = static_cast<uint8_t *>(malloc(size));

    memcpy(buf, data, size);

    otNcpHdlcReceive(buf, static_cast<uint16_t>(size));

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

    if (buf != nullptr)
    {
        free(buf);
    }

    if (instance != nullptr)
    {
        otInstanceFinalize(instance);
    }

    return 0;
}
