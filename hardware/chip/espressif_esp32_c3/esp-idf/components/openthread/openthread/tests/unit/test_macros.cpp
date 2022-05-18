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

#include "test_platform.h"

#include "common/arg_macros.hpp"

static constexpr uint8_t NumberOfArgs(void)
{
    return 0;
}

static constexpr uint8_t NumberOfArgs(uint8_t)
{
    return 1;
}

static constexpr uint8_t NumberOfArgs(uint8_t, uint8_t)
{
    return 2;
}

static constexpr uint8_t NumberOfArgs(uint8_t, uint8_t, uint8_t)
{
    return 3;
}

static constexpr uint8_t NumberOfArgs(uint8_t, uint8_t, uint8_t, uint8_t)
{
    return 4;
}

static constexpr uint8_t NumberOfArgs(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)
{
    return 5;
}

static constexpr uint8_t NumberOfArgs(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)
{
    return 6;
}

static constexpr uint8_t NumberOfArgs(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t)
{
    return 7;
}

int Sum(int aFirst)
{
    return aFirst;
}

template <typename... Args> int Sum(int aFirst, Args... aArgs)
{
    return aFirst + Sum(aArgs...);
}

void TestMacros(void)
{
    // Verify `OT_FIRST_ARG()` macro.

    VerifyOrQuit(OT_FIRST_ARG(1) == 1);
    VerifyOrQuit(OT_FIRST_ARG(1, 2, 3) == 1);
    VerifyOrQuit(OT_FIRST_ARG(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12) == 1);

    // Verify `OT_REST_ARGS()` macro.

    VerifyOrQuit(NumberOfArgs(OT_REST_ARGS(1)) == 0);

    VerifyOrQuit(NumberOfArgs(0 OT_REST_ARGS(1)) == 1);
    VerifyOrQuit(NumberOfArgs(0 OT_REST_ARGS(1, 2)) == 2);
    VerifyOrQuit(NumberOfArgs(0 OT_REST_ARGS(1, 2, 3)) == 3);
    VerifyOrQuit(NumberOfArgs(0 OT_REST_ARGS(1, 2, 3, 4)) == 4);
    VerifyOrQuit(NumberOfArgs(0 OT_REST_ARGS(1, 2, 3, 4, 5)) == 5);
    VerifyOrQuit(NumberOfArgs(0 OT_REST_ARGS(1, 2, 3, 4, 5, 6)) == 6);
    VerifyOrQuit(NumberOfArgs(0 OT_REST_ARGS(1, 2, 3, 4, 5, 6, 7)) == 7);

    VerifyOrQuit(Sum(100 OT_REST_ARGS(1)) == 100);
    VerifyOrQuit(Sum(100 OT_REST_ARGS(1, 2)) == 102);
    VerifyOrQuit(Sum(100 OT_REST_ARGS(1, 2, 3)) == 105);
    VerifyOrQuit(Sum(100 OT_REST_ARGS(1, 2, 3, 4)) == 109);
    VerifyOrQuit(Sum(100 OT_REST_ARGS(1, 2, 3, 4, 5)) == 114);
    VerifyOrQuit(Sum(100 OT_REST_ARGS(1, 2, 3, 4, 5, 6)) == 120);
    VerifyOrQuit(Sum(100 OT_REST_ARGS(1, 2, 3, 4, 5, 6, 7)) == 127);

    // Verify `OT_SECOND_ARG()` macro.

    VerifyOrQuit(NumberOfArgs(OT_SECOND_ARG(1)) == 0);
    VerifyOrQuit(NumberOfArgs(OT_SECOND_ARG(1, 2)) == 1);

    VerifyOrQuit(OT_SECOND_ARG(1, 2) == 2);
}

void TestVerifyOrExitSuccessNoAction(void)
{
    bool reachedEnd = false;

    VerifyOrExit(true);
    reachedEnd = true;

exit:
    VerifyOrQuit(reachedEnd, "VerifyOrExit() failed");
}

void TestVerifyOrExitFailureNoAction(void)
{
    bool reachedEnd = false;

    VerifyOrExit(false);
    reachedEnd = true;

exit:
    VerifyOrQuit(!reachedEnd, "VerifyOrExit() failed");
}

void TestVerifyOrExitSuccessWithAction(void)
{
    bool didAction  = false;
    bool reachedEnd = false;

    VerifyOrExit(true, didAction = true);
    reachedEnd = true;

exit:
    VerifyOrQuit(reachedEnd && !didAction, "VerifyOrExit() failed");
}

void TestVerifyOrExitFailureWithAction(void)
{
    bool didAction  = false;
    bool reachedEnd = false;

    VerifyOrExit(false, didAction = true);
    reachedEnd = true;

exit:
    VerifyOrQuit(!reachedEnd && didAction, "VerifyOrExit() failed");
}

int main(void)
{
    TestMacros();
    TestVerifyOrExitSuccessNoAction();
    TestVerifyOrExitFailureNoAction();
    TestVerifyOrExitSuccessWithAction();
    TestVerifyOrExitFailureWithAction();
    printf("All tests passed\n");
    return 0;
}
