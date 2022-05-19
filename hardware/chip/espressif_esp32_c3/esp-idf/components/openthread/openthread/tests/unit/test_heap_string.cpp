/*
 *  Copyright (c) 2021, The OpenThread Authors.
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

#include <string.h>

#include <openthread/config.h>

#include "test_util.hpp"
#include "common/code_utils.hpp"
#include "common/heap_string.hpp"

namespace ot {

void PrintString(const char *aName, const HeapString &aString)
{
    if (aString.IsNull())
    {
        printf("%s = (null)\n", aName);
    }
    else
    {
        printf("%s = [%zu] \"%s\"\n", aName, strlen(aString.AsCString()), aString.AsCString());
    }
}

void VerifyString(const char *aName, const HeapString &aString, const char *aExpectedString)
{
    PrintString(aName, aString);

    if (aExpectedString == nullptr)
    {
        VerifyOrQuit(aString.IsNull());
        VerifyOrQuit(aString.AsCString() == nullptr);
        VerifyOrQuit(aString != "something");
    }
    else
    {
        VerifyOrQuit(!aString.IsNull());
        VerifyOrQuit(aString.AsCString() != nullptr);
        VerifyOrQuit(strcmp(aString.AsCString(), aExpectedString) == 0, "String content is incorrect");
        VerifyOrQuit(aString != nullptr);
    }

    VerifyOrQuit(aString == aExpectedString);
}

// Function returning a `HeapString` by value.
HeapString GetName(void)
{
    HeapString name;

    SuccessOrQuit(name.Set("name"));

    return name;
}

void TestHeapString(void)
{
    HeapString  str1;
    HeapString  str2;
    const char *oldBuffer;

    printf("------------------------------------------------------------------------------------\n");
    printf("After constructor\n\n");
    VerifyString("str1", str1, nullptr);

    printf("------------------------------------------------------------------------------------\n");
    printf("Set(const char *aCstring)\n\n");
    SuccessOrQuit(str1.Set("hello"));
    VerifyString("str1", str1, "hello");
    oldBuffer = str1.AsCString();

    SuccessOrQuit(str1.Set("0123456789"));
    VerifyString("str1", str1, "0123456789");
    printf("\tDid reuse its old buffer: %s\n", str1.AsCString() == oldBuffer ? "yes" : "no");
    oldBuffer = str1.AsCString();

    SuccessOrQuit(str1.Set("9876543210"));
    VerifyString("str1", str1, "9876543210");
    printf("\tDid reuse its old buffer (same length): %s\n", str1.AsCString() == oldBuffer ? "yes" : "no");

    printf("------------------------------------------------------------------------------------\n");
    printf("Set(const HeapString &)\n\n");
    SuccessOrQuit(str2.Set(str1));
    VerifyString("str2", str2, str1.AsCString());

    SuccessOrQuit(str1.Set(nullptr));
    VerifyString("str1", str1, nullptr);

    SuccessOrQuit(str2.Set(str1));
    VerifyString("str2", str2, nullptr);

    printf("------------------------------------------------------------------------------------\n");
    printf("Free()\n\n");
    str1.Free();
    VerifyString("str1", str1, nullptr);

    SuccessOrQuit(str1.Set("hello again"));
    VerifyString("str1", str1, "hello again");

    str1.Free();
    VerifyString("str1", str1, nullptr);

    printf("------------------------------------------------------------------------------------\n");
    printf("Set() move semantics\n\n");
    SuccessOrQuit(str1.Set("old name"));
    PrintString("str1", str1);
    SuccessOrQuit(str1.Set(GetName()), "Set() with move semantics failed");
    VerifyString("str1", str1, "name");

    printf("------------------------------------------------------------------------------------\n");
    printf("operator==() with two null string\n\n");
    str1.Free();
    str2.Free();
    VerifyString("str1", str1, nullptr);
    VerifyString("str2", str2, nullptr);
    VerifyOrQuit(str1 == str2, "operator==() failed with two null strings");

    printf("\n -- PASS\n");
}

} // namespace ot

int main(void)
{
    ot::TestHeapString();
    printf("\nAll tests passed.\n");
    return 0;
}
