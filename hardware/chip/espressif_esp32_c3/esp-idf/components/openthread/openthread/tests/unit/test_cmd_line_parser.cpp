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

#include <string.h>

#include "test_platform.h"

#include <openthread/config.h>

#include "common/instance.hpp"
#include "utils/parse_cmdline.hpp"

#include "test_util.hpp"

using ot::Utils::CmdLineParser::ParseAsBool;
using ot::Utils::CmdLineParser::ParseAsHexString;
using ot::Utils::CmdLineParser::ParseAsHexStringSegment;
using ot::Utils::CmdLineParser::ParseAsInt16;
using ot::Utils::CmdLineParser::ParseAsInt32;
using ot::Utils::CmdLineParser::ParseAsInt8;
using ot::Utils::CmdLineParser::ParseAsUint16;
using ot::Utils::CmdLineParser::ParseAsUint32;
using ot::Utils::CmdLineParser::ParseAsUint64;
using ot::Utils::CmdLineParser::ParseAsUint8;

template <typename ValueType> struct TestCase
{
    const char *mString;
    otError     mError;
    ValueType   mValue;
};

template <typename ValueType, otError (&Parser)(const char *aString, ValueType &aValue)>
void VerifyParser(const TestCase<ValueType> *aTestCases, const char *aParserName, const char *aPrintFormat)
{
    const TestCase<ValueType> *testCase = aTestCases;
    ValueType                  value;
    otError                    error;

    printf("----------------------------------------------------------\n");

    while (true)
    {
        printf("%s(\"%s\") -> ", aParserName, testCase->mString);

        if (testCase->mError != OT_ERROR_NONE)
        {
            printf("error:%s", otThreadErrorToString(testCase->mError));
        }
        else
        {
            printf(aPrintFormat, testCase->mValue);
        }

        printf("\n");

        error = Parser(testCase->mString, value);

        VerifyOrQuit(error == testCase->mError, "Parser did not return the expected error");

        if (error == OT_ERROR_NONE)
        {
            VerifyOrQuit(value == testCase->mValue, "Parser failed");
        }

        if (testCase->mString[0] == '\0')
        {
            break;
        }

        testCase++;
    }
}

void TestParsingInts(void)
{
    TestCase<bool> kBoolTestCases[] = {
        {"0", OT_ERROR_NONE, false},          // Zero as false value
        {"1", OT_ERROR_NONE, true},           // Non-zero as true value
        {"0x0", OT_ERROR_NONE, false},        // Zero as false value
        {"0x1", OT_ERROR_NONE, true},         // Non-zero (in hex) as true value
        {"10", OT_ERROR_NONE, true},          // Non-zero as true value
        {"a", OT_ERROR_INVALID_ARGS, false},  // Error case: Incorrect char
        {"-1", OT_ERROR_INVALID_ARGS, false}, // Error case: Negative value
        {"", OT_ERROR_INVALID_ARGS, false},   // Empty string indicate end of the list
    };

    TestCase<uint8_t> kUint8TestCases[] = {
        {"0", OT_ERROR_NONE, 0},
        {"1", OT_ERROR_NONE, 1},
        {"74", OT_ERROR_NONE, 74},
        {"255", OT_ERROR_NONE, 255}, // Max `uint8` value (decimal format)
        {"0xa", OT_ERROR_NONE, 0xa},
        {"0x04", OT_ERROR_NONE, 4},
        {"0x7e", OT_ERROR_NONE, 0x7e},
        {"0xcd", OT_ERROR_NONE, 0xcd},
        {"0x0", OT_ERROR_NONE, 0},
        {"0xff", OT_ERROR_NONE, 0xff},     // Max `uint8` value (hex format)
        {"0x0000ff", OT_ERROR_NONE, 0xff}, // Hex format (extra zeros)
        {"0xB", OT_ERROR_NONE, 0xb},
        {"0X04", OT_ERROR_NONE, 4},
        {"0X7E", OT_ERROR_NONE, 0x7e},
        {"0XCD", OT_ERROR_NONE, 0xcd},
        {"0X0", OT_ERROR_NONE, 0},
        {"0XFF", OT_ERROR_NONE, 0xff},
        {"00", OT_ERROR_NONE, 0},
        {"-5", OT_ERROR_INVALID_ARGS, 0},    // Error case: Negative value.
        {"0y", OT_ERROR_INVALID_ARGS, 0},    // Error case: Incorrect prefix.
        {"0x7g", OT_ERROR_INVALID_ARGS, 0},  // Error case: Bad hex char.
        {"0xaaa", OT_ERROR_INVALID_ARGS, 0}, // Error case: Out or range.
        {"256", OT_ERROR_INVALID_ARGS, 0},   // Error case: Out of range (max value + 1)
        {"12e", OT_ERROR_INVALID_ARGS, 0},   // Error case: Extra char.
        {"", OT_ERROR_INVALID_ARGS, 0}       // Empty string indicates end of the list
    };

    TestCase<uint16_t> kUint16TestCases[] = {
        {"0", OT_ERROR_NONE, 0},
        {"1245", OT_ERROR_NONE, 1245},
        {"0xa", OT_ERROR_NONE, 0xa},
        {"0xab7d", OT_ERROR_NONE, 0xab7d},
        {"0X1AE", OT_ERROR_NONE, 0x1ae},
        {"0X7E", OT_ERROR_NONE, 0x7e},
        {"65535", OT_ERROR_NONE, 65535},       // Max `uint16` value (decimal format)
        {"0xffff", OT_ERROR_NONE, 0xffff},     // Max `uint16` value (hex format)
        {"-1", OT_ERROR_INVALID_ARGS, 0},      // Error case: Negative value
        {"0y", OT_ERROR_INVALID_ARGS, 0},      // Error case: Incorrect prefix
        {"0xq", OT_ERROR_INVALID_ARGS, 0},     // Error case: Bad hex char.
        {"0x12345", OT_ERROR_INVALID_ARGS, 0}, // Error case: Out of range.
        {"65536", OT_ERROR_INVALID_ARGS, 0},   // Error case: Out of range (max value + 1)
        {"", OT_ERROR_INVALID_ARGS, 0}         // Empty string indicates end of the list
    };

    TestCase<uint32_t> kUint32TestCases[] = {
        {"0", OT_ERROR_NONE, 0},
        {"1234567", OT_ERROR_NONE, 1234567},
        {"0xc", OT_ERROR_NONE, 0xc},
        {"0x01234567", OT_ERROR_NONE, 0x1234567},
        {"0XABCDEF09", OT_ERROR_NONE, 0xabcdef09},
        {"0X54321", OT_ERROR_NONE, 0x54321},
        {"4294967295", OT_ERROR_NONE, 4294967295}, // Max `uint32` value (decimal format)
        {"0xffffffff", OT_ERROR_NONE, 0xffffffff}, // Max `uint32` value (hex format)
        {"-1", OT_ERROR_INVALID_ARGS, 0},
        {"0y", OT_ERROR_INVALID_ARGS, 0},
        {"0x1234zz", OT_ERROR_INVALID_ARGS, 0},    // Error case: Bad hex char
        {"0x123456789", OT_ERROR_INVALID_ARGS, 0}, // Error case: Out of range
        {"4294967296", OT_ERROR_INVALID_ARGS, 0},  // Error case: Out of range (max value + 1)
        {"", OT_ERROR_INVALID_ARGS, 0}             // Empty string indicates end of the list.
    };

    TestCase<uint64_t> kUint64TestCases[] = {
        {"0", OT_ERROR_NONE, 0},
        {"123456789087654321", OT_ERROR_NONE, 123456789087654321},
        {"0xb", OT_ERROR_NONE, 0xb},
        {"0x1234567890acbdef", OT_ERROR_NONE, 0x1234567890acbdef},
        {"0XFEDCBA9876543210", OT_ERROR_NONE, 0xfedcba9876543210},
        {"0xffffffffffffffff", OT_ERROR_NONE, 0xffffffffffffffff},        // Max `uint64` value (hex format)
        {"18446744073709551615", OT_ERROR_NONE, 18446744073709551615ull}, // Max `uint64` value (decimal format)
        {"-1", OT_ERROR_INVALID_ARGS, 0},
        {"0x1234567890acbdef0", OT_ERROR_INVALID_ARGS, 0},  // Error case: Out of range
        {"18446744073709551616", OT_ERROR_INVALID_ARGS, 0}, // Error case: Out or range (max value + 1)
        {"", OT_ERROR_INVALID_ARGS, 0}                      // Empty string indicates end of the list.
    };

    TestCase<int8_t> kInt8TestCases[] = {
        {"0", OT_ERROR_NONE, 0},
        {"-1", OT_ERROR_NONE, -1},
        {"+74", OT_ERROR_NONE, 74},
        {"-0x12", OT_ERROR_NONE, -0x12},
        {"-0XB", OT_ERROR_NONE, -11},
        {"127", OT_ERROR_NONE, 127},        // Max `int8` value
        {"-128", OT_ERROR_NONE, -128},      // Min `int8` value
        {"128", OT_ERROR_INVALID_ARGS, 0},  // Error case: Out of range (max value + 1)
        {"-129", OT_ERROR_INVALID_ARGS, 0}, // Error case: Out of range (min value - 1)
        {"--1", OT_ERROR_INVALID_ARGS, 0},  // Error case: Extra sign
        {"+-2", OT_ERROR_INVALID_ARGS, 0},  // Error case: Extra sign
        {"++1", OT_ERROR_INVALID_ARGS, 0},  // Error case: Extra sign
        {"", OT_ERROR_INVALID_ARGS, 0}      // Empty string indicates end of the list.
    };

    TestCase<int16_t> kInt16TestCases[] = {
        {"-1", OT_ERROR_NONE, -1},
        {"+0x1234", OT_ERROR_NONE, 0x1234},
        {"-0X6E8", OT_ERROR_NONE, -0x6E8},
        {"32767", OT_ERROR_NONE, 32767},       // Max `int16` value
        {"0X7FFF", OT_ERROR_NONE, 0x7fff},     // Max `int16` value (hex value)
        {"-32768", OT_ERROR_NONE, -32768},     // Min `int16` value
        {"-0x8000", OT_ERROR_NONE, -0x8000},   // Min `int16` value (hex value)
        {"32768", OT_ERROR_INVALID_ARGS, 0},   // Error case: Out of range (max + 1)
        {"0X8000", OT_ERROR_INVALID_ARGS, 0},  // Error case: Out of range (max + 1)
        {"-32769", OT_ERROR_INVALID_ARGS, 0},  // Error case: Out of range (min - 1)
        {"-0x8001", OT_ERROR_INVALID_ARGS, 0}, // Error case: Out of range (min - 1)
        {"", OT_ERROR_INVALID_ARGS, 0}         // Empty string indicates end of the list.
    };

    TestCase<int32_t> kInt32TestCases[] = {
        {"-256", OT_ERROR_NONE, -256},
        {"+0x12345678", OT_ERROR_NONE, 0x12345678},
        {"-0X6677aB", OT_ERROR_NONE, -0X6677aB},
        {"2147483647", OT_ERROR_NONE, 2147483647},
        {"0x7fffFFFF", OT_ERROR_NONE, 0x7fffffff},   // Max `int32` value
        {"-2147483648", OT_ERROR_NONE, -2147483648}, // Min `int32` value
        {"2147483648", OT_ERROR_INVALID_ARGS, 0},    // Error case: Out of range (max + 1)
        {"0X80000000", OT_ERROR_INVALID_ARGS, 0},    // Error case: Out of range (max + 1)
        {"-2147483649", OT_ERROR_INVALID_ARGS, 0},   // Error case: Out of range (min - 1)
        {"-0x80000001", OT_ERROR_INVALID_ARGS, 0},   // Error case: Out of range (min - 1)
        {"", OT_ERROR_INVALID_ARGS, 0}               // Empty string indicates end of the list
    };

    VerifyParser<bool, ParseAsBool>(kBoolTestCases, "ParseAsBool", "%d");
    VerifyParser<uint8_t, ParseAsUint8>(kUint8TestCases, "ParseAsUint8", "0x%02x");
    VerifyParser<uint16_t, ParseAsUint16>(kUint16TestCases, "ParseAsUint16", "0x%04x");
    VerifyParser<uint32_t, ParseAsUint32>(kUint32TestCases, "ParseAsUint32", "0x%08x");
    VerifyParser<uint64_t, ParseAsUint64>(kUint64TestCases, "ParseAsUint64", "0x%016llx");
    VerifyParser<int8_t, ParseAsInt8>(kInt8TestCases, "ParseAsInt8", "%d");
    VerifyParser<int16_t, ParseAsInt16>(kInt16TestCases, "ParseAsInt16", "%d");
    VerifyParser<int32_t, ParseAsInt32>(kInt32TestCases, "ParseAsInt32", "%d");
}

void TestParsingHexStrings(void)
{
    const char    kEvenHexString[]   = "DeadBeefCafeBabe";
    const uint8_t kEvenParsedArray[] = {0xde, 0xad, 0xbe, 0xef, 0xca, 0xfe, 0xba, 0xbe};

    const char    kOddHexString[]   = "abcdef9876543";
    const uint8_t kOddParsedArray[] = {0xa, 0xbc, 0xde, 0xf9, 0x87, 0x65, 0x43};

    uint8_t        buffer[sizeof(kEvenParsedArray)];
    uint8_t        buf3[3];
    uint16_t       len;
    const char *   string;
    const uint8_t *bufPtr;

    // Verify `ParseAsHexString(const char *aString, uint8_t *aBuffer, uint16_t aSize)`

    buffer[0] = 0xff;
    SuccessOrQuit(ParseAsHexString("0", buffer, 1));
    VerifyOrQuit(buffer[0] == 0, "ParseAsHexString() parsed incorrectly");

    buffer[0] = 0;
    SuccessOrQuit(ParseAsHexString("7e", buffer, 1));
    VerifyOrQuit(buffer[0] == 0x7e, "ParseAsHexString() parsed incorrectly");

    VerifyOrQuit(ParseAsHexString("123", buffer, 1) != OT_ERROR_NONE, "ParseAsHexString() passed with bad input");
    SuccessOrQuit(ParseAsHexString("123", buffer, 2));
    VerifyOrQuit(buffer[0] == 1 && buffer[1] == 0x23, "ParseAsHexString() parsed incorrectly");

    VerifyOrQuit(ParseAsHexString("123x", buffer, 2) != OT_ERROR_NONE, "ParseAsHexString() passed with bad input");
    VerifyOrQuit(ParseAsHexString(" 123", buffer, 2) != OT_ERROR_NONE, "ParseAsHexString() passed with bad input");

    // Verify `ParseAsHexString<kMaxSize>()`

    VerifyOrQuit(ParseAsHexString("1122", buf3) != OT_ERROR_NONE, "ParseAsHexString() passed with bad input");
    VerifyOrQuit(ParseAsHexString("1122334", buf3) != OT_ERROR_NONE, "ParseAsHexString() passed with bad input");
    VerifyOrQuit(ParseAsHexString("11223344", buf3) != OT_ERROR_NONE, "ParseAsHexString() passed with bad input");
    SuccessOrQuit(ParseAsHexString("abbade", buf3));

    VerifyOrQuit(buf3[0] == 0xab && buf3[1] == 0xba && buf3[2] == 0xde, "ParseAsHexString() parsed incorrectly");
    SuccessOrQuit(ParseAsHexString("012345", buf3));
    VerifyOrQuit(buf3[0] == 0x01 && buf3[1] == 0x23 && buf3[2] == 0x45, "ParseAsHexString() parsed incorrectly");
    SuccessOrQuit(ParseAsHexString("12345", buf3), "ParseAsHexString() failed with odd length");
    VerifyOrQuit(buf3[0] == 0x01 && buf3[1] == 0x23 && buf3[2] == 0x45, "ParseAsHexString() parsed incorrectly");

    SuccessOrQuit(ParseAsHexString(kEvenHexString, buffer), "ParseAsHexString failed");
    VerifyOrQuit(memcmp(buffer, kEvenParsedArray, sizeof(buffer)) == 0, "ParseAsHexString() parsed incorrectly");

    // Verify `ParseAsHexString(const char *aString, uint16_t &aSize, uint8_t *aBuffer)`

    printf("----------------------------------------------------------\n");
    len = sizeof(buffer);

    SuccessOrQuit(ParseAsHexString(kEvenHexString, len, buffer));
    VerifyOrQuit(len == sizeof(kEvenParsedArray), "ParseAsHexString() parsed incorrectly");
    VerifyOrQuit(memcmp(buffer, kEvenParsedArray, len) == 0, "ParseAsHexString() parsed incorrectly");
    DumpBuffer(kEvenHexString, buffer, len);

    SuccessOrQuit(ParseAsHexString(kOddHexString, len, buffer));
    VerifyOrQuit(len == sizeof(kOddParsedArray), "ParseAsHexString() parsed incorrectly");
    VerifyOrQuit(memcmp(buffer, kOddParsedArray, len) == 0, "ParseAsHexString() parsed incorrectly");
    DumpBuffer(kOddHexString, buffer, len);

    // Verify `ParseAsHexStringSegement()`

    printf("----------------------------------------------------------\n");

    for (uint8_t testIter = 0; testIter <= 1; testIter++)
    {
        for (uint8_t segmentLen = 1; segmentLen <= sizeof(buffer); segmentLen++)
        {
            if (testIter == 0)
            {
                string = kEvenHexString;
                bufPtr = kEvenParsedArray;
            }
            else
            {
                string = kOddHexString;
                bufPtr = kOddParsedArray;
            }

            len = segmentLen;

            printf("\"%s\" segLen:%d -> ", string, segmentLen);

            while (true)
            {
                otError error = ParseAsHexStringSegment(string, len, buffer);

                printf("%d (\"%s\") ", len, string);

                if (error == OT_ERROR_NONE)
                {
                    VerifyOrQuit(len <= segmentLen, "ParseAsHexStringSegment() parsed incorrectly");
                    VerifyOrQuit(memcmp(buffer, bufPtr, len) == 0, "ParseAsHexStringSegment() parsed incorrectly");
                    VerifyOrQuit(*string == '\0',
                                 "ParseAsHexStringSegment() failed to update string pointer correctly");
                    break;
                }

                VerifyOrQuit(error == OT_ERROR_PENDING, "ParseAsHexStringSegment() failed");
                VerifyOrQuit(len == segmentLen, "ParseAsHexStringSegment() parsed incorrectly");
                VerifyOrQuit(memcmp(buffer, bufPtr, len) == 0, "ParseAsHexStringSegment() parsed incorrectly");
                bufPtr += len;
            }

            printf("\n");
        }
    }
}

int main(void)
{
    TestParsingInts();
    TestParsingHexStrings();

    printf("All tests passed\n");
    return 0;
}
