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

#include "test_platform.h"

#include <openthread/config.h>

#include "test_util.h"
#include "common/code_utils.hpp"
#include "common/string.hpp"

namespace ot {

enum
{
    kStringSize = 10,
};

template <uint16_t kSize> void PrintString(const char *aName, const String<kSize> aString)
{
    printf("\t%s = [%zu] \"%s\"\n", aName, strlen(aString.AsCString()), aString.AsCString());
}

void TestStringWriter(void)
{
    String<kStringSize> str;
    constexpr char      kLongString[] = "abcdefghijklmnopqratuvwxyzabcdefghijklmnopqratuvwxyz";

    printf("\nTest 1: StringWriter constructor\n");

    VerifyOrQuit(str.GetSize() == kStringSize);
    VerifyOrQuit(str.GetLength() == 0, "failed for empty string");

    VerifyOrQuit(strcmp(str.AsCString(), "") == 0);

    PrintString("str", str);

    printf(" -- PASS\n");

    printf("\nTest 2: StringWriter::Append() method\n");

    str.Append("Hi");
    VerifyOrQuit(str.GetLength() == 2);
    VerifyOrQuit(strcmp(str.AsCString(), "Hi") == 0);
    PrintString("str", str);

    str.Append("%s%d", "!", 12);
    VerifyOrQuit(str.GetLength() == 5);
    VerifyOrQuit(strcmp(str.AsCString(), "Hi!12") == 0);
    PrintString("str", str);

    str.Append(kLongString);
    VerifyOrQuit(str.IsTruncated() && str.GetLength() == 5 + sizeof(kLongString) - 1,
                 "String::Append() did not handle overflow buffer correctly");
    PrintString("str", str);

    printf("\nTest 3: StringWriter::Clear() method\n");

    str.Clear();
    str.Append("Hello");
    VerifyOrQuit(str.GetLength() == 5);
    VerifyOrQuit(strcmp(str.AsCString(), "Hello") == 0);
    PrintString("str", str);

    str.Clear();
    VerifyOrQuit(str.GetLength() == 0, "failed after Clear()");
    VerifyOrQuit(strcmp(str.AsCString(), "") == 0);

    str.Append("%d", 12);
    VerifyOrQuit(str.GetLength() == 2);
    VerifyOrQuit(strcmp(str.AsCString(), "12") == 0);
    PrintString("str", str);

    str.Clear();
    str.Append(kLongString);
    VerifyOrQuit(str.IsTruncated() && str.GetLength() == sizeof(kLongString) - 1,
                 "String::Clear() + String::Append() did not handle overflow buffer correctly");
    PrintString("str", str);

    printf(" -- PASS\n");
}

void TestStringLength(void)
{
    char string_a[5] = "\0foo";
    char string_b[8] = "foo\0bar";

    printf("\nTest 4: String::StringLength() method\n");

    VerifyOrQuit(StringLength(string_a, 0) == 0);
    VerifyOrQuit(StringLength(string_a, 1) == 0);
    VerifyOrQuit(StringLength(string_a, 2) == 0);

    VerifyOrQuit(StringLength(string_b, 0) == 0);
    VerifyOrQuit(StringLength(string_b, 1) == 1);
    VerifyOrQuit(StringLength(string_b, 2) == 2);
    VerifyOrQuit(StringLength(string_b, 3) == 3);
    VerifyOrQuit(StringLength(string_b, 4) == 3);
    VerifyOrQuit(StringLength(string_b, 5) == 3);
    VerifyOrQuit(StringLength(string_b, 6) == 3);

    printf(" -- PASS\n");
}

void TestUtf8(void)
{
    printf("\nTest 5: IsValidUtf8String() function\n");

    VerifyOrQuit(IsValidUtf8String("An ASCII string"));
    VerifyOrQuit(IsValidUtf8String(u8"Строка UTF-8"));
    VerifyOrQuit(!IsValidUtf8String("\xbf"));
    VerifyOrQuit(!IsValidUtf8String("\xdf"));
    VerifyOrQuit(!IsValidUtf8String("\xef\x80"));
    VerifyOrQuit(!IsValidUtf8String("\xf7\x80\x80"));
    VerifyOrQuit(!IsValidUtf8String("\xff"));
    VerifyOrQuit(!IsValidUtf8String("NUL\x00NUL", 7)); // UTF-8 NUL
    VerifyOrQuit(!IsValidUtf8String("abcde\x11"));     // control character

    printf(" -- PASS\n");
}

void TestStringFind(void)
{
    char emptyString[1] = {'\0'};
    char testString[]   = "foo.bar.bar\\.";
    char testString2[]  = "abcabcabcdabc";

    printf("\nTest 6: StringFind() function\n");

    VerifyOrQuit(StringFind(testString, 'f') == testString);
    VerifyOrQuit(StringFind(testString, 'o') == &testString[1]);
    VerifyOrQuit(StringFind(testString, '.') == &testString[3]);
    VerifyOrQuit(StringFind(testString, 'r') == &testString[6]);
    VerifyOrQuit(StringFind(testString, '\\') == &testString[11]);
    VerifyOrQuit(StringFind(testString, 'x') == nullptr);
    VerifyOrQuit(StringFind(testString, ',') == nullptr);

    VerifyOrQuit(StringFind(emptyString, 'f') == nullptr);
    VerifyOrQuit(StringFind(emptyString, '.') == nullptr);

    VerifyOrQuit(StringFind(testString, "foo") == &testString[0]);
    VerifyOrQuit(StringFind(testString, "oo") == &testString[1]);
    VerifyOrQuit(StringFind(testString, "bar") == &testString[4]);
    VerifyOrQuit(StringFind(testString, "bar\\") == &testString[8]);
    VerifyOrQuit(StringFind(testString, "\\.") == &testString[11]);
    VerifyOrQuit(StringFind(testString, testString) == testString);
    VerifyOrQuit(StringFind(testString, "fooo") == nullptr);
    VerifyOrQuit(StringFind(testString, "far") == nullptr);
    VerifyOrQuit(StringFind(testString, "bar\\..") == nullptr);
    VerifyOrQuit(StringFind(testString, "") == &testString[0]);

    VerifyOrQuit(StringFind(emptyString, "foo") == nullptr);
    VerifyOrQuit(StringFind(emptyString, "bar") == nullptr);
    VerifyOrQuit(StringFind(emptyString, "") == &emptyString[0]);

    // Verify when sub-string has repeated patterns
    VerifyOrQuit(StringFind(testString2, "abcabc") == &testString2[0]);
    VerifyOrQuit(StringFind(testString2, "abcabcd") == &testString2[3]);

    printf(" -- PASS\n");
}

void TestStringEndsWith(void)
{
    printf("\nTest 7: StringEndsWith() function\n");

    VerifyOrQuit(StringEndsWith("foobar", 'r'));
    VerifyOrQuit(!StringEndsWith("foobar", 'a'));
    VerifyOrQuit(!StringEndsWith("foobar", '\0'));
    VerifyOrQuit(StringEndsWith("a", 'a'));
    VerifyOrQuit(!StringEndsWith("a", 'b'));

    VerifyOrQuit(StringEndsWith("foobar", "bar"));
    VerifyOrQuit(!StringEndsWith("foobar", "ba"));
    VerifyOrQuit(StringEndsWith("foobar", "foobar"));
    VerifyOrQuit(!StringEndsWith("foobar", "foobarr"));

    VerifyOrQuit(!StringEndsWith("", 'a'));
    VerifyOrQuit(!StringEndsWith("", "foo"));

    printf(" -- PASS\n");
}

} // namespace ot

int main(void)
{
    ot::TestStringWriter();
    ot::TestStringLength();
    ot::TestUtf8();
    ot::TestStringFind();
    ot::TestStringEndsWith();
    printf("\nAll tests passed.\n");
    return 0;
}
