/*
 *  Copyright (c) 2019, The OpenThread Authors.
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
 *   This file implements an entropy source based on /dev/urandom or pseudo-random generator.
 *
 */

#include "openthread-posix-config.h"
#include "platform-posix.h"

#include <assert.h>
#include <stdio.h>

#include <openthread/error.h>
#include <openthread/platform/entropy.h>

#include "common/code_utils.hpp"

#ifndef __SANITIZE_ADDRESS__
#define __SANITIZE_ADDRESS__ 0
#endif

#if __SANITIZE_ADDRESS__ != 0

static uint32_t sState = 1;

#endif // __SANITIZE_ADDRESS__

void platformRandomInit(void)
{
#if __SANITIZE_ADDRESS__ != 0

    // Multiplying gNodeId assures that no two nodes gets the same seed within an hour.
    sState = (uint32_t)time(nullptr) + (3600 * gNodeId);

#endif // __SANITIZE_ADDRESS__
}

#if __SANITIZE_ADDRESS__ != 0

uint32_t randomUint32Get(void)
{
    uint32_t mlcg, p, q;
    uint64_t tmpstate;

    tmpstate = (uint64_t)33614 * (uint64_t)sState;
    q        = tmpstate & 0xffffffff;
    q        = q >> 1;
    p        = tmpstate >> 32;
    mlcg     = p + q;

    if (mlcg & 0x80000000)
    {
        mlcg &= 0x7fffffff;
        mlcg++;
    }

    sState = mlcg;

    return mlcg;
}

#endif // __SANITIZE_ADDRESS__

otError otPlatEntropyGet(uint8_t *aOutput, uint16_t aOutputLength)
{
    otError error = OT_ERROR_NONE;

#if __SANITIZE_ADDRESS__ == 0

    FILE * file = nullptr;
    size_t readLength;

    VerifyOrExit(aOutput && aOutputLength, error = OT_ERROR_INVALID_ARGS);

    file = fopen("/dev/urandom", "rb");
    VerifyOrExit(file != nullptr, error = OT_ERROR_FAILED);

    readLength = fread(aOutput, 1, aOutputLength, file);
    VerifyOrExit(readLength == aOutputLength, error = OT_ERROR_FAILED);

exit:

    if (file != nullptr)
    {
        fclose(file);
    }

#else // __SANITIZE_ADDRESS__

    /*
     * THE IMPLEMENTATION BELOW IS NOT COMPLIANT WITH THE THREAD SPECIFICATION.
     *
     * Address Sanitizer triggers test failures when reading random
     * values from /dev/urandom.  The pseudo-random number generator
     * implementation below is only used to enable continuous
     * integration checks with Address Sanitizer enabled.
     */
    VerifyOrExit(aOutput && aOutputLength, error = OT_ERROR_INVALID_ARGS);

    for (uint16_t length = 0; length < aOutputLength; length++)
    {
        aOutput[length] = (uint8_t)randomUint32Get();
    }

exit:

#endif // __SANITIZE_ADDRESS__

    return error;
}
