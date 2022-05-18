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

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "lib/spinel/spinel_encoder.hpp"

#include "test_util.hpp"

namespace ot {
namespace Spinel {

enum
{
    kTestBufferSize = 800,
};

otError ReadFrame(Spinel::Buffer &aNcpBuffer, uint8_t *aFrame, uint16_t &aFrameLen)
{
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = aNcpBuffer.OutFrameBegin());
    aFrameLen = aNcpBuffer.OutFrameGetLength();
    VerifyOrExit(aNcpBuffer.OutFrameRead(aFrameLen, aFrame) == aFrameLen, error = OT_ERROR_FAILED);
    SuccessOrExit(error = aNcpBuffer.OutFrameRemove());

exit:
    return error;
}

void TestEncoder(void)
{
    uint8_t         buffer[kTestBufferSize];
    Spinel::Buffer  ncpBuffer(buffer, kTestBufferSize);
    Spinel::Encoder encoder(ncpBuffer);

    uint8_t        frame[kTestBufferSize];
    uint16_t       frameLen;
    spinel_ssize_t parsedLen;

    const bool         kBool_1 = true;
    const bool         kBool_2 = false;
    const uint8_t      kUint8  = 0x42;
    const int8_t       kInt8   = -73;
    const uint16_t     kUint16 = 0xabcd;
    const int16_t      kInt16  = -567;
    const uint32_t     kUint32 = 0xdeadbeef;
    const int32_t      kInt32  = -123455678L;
    const uint64_t     kUint64 = 0xfe10dc32ba549876ULL;
    const int64_t      kInt64  = -9197712039090021561LL;
    const unsigned int kUint_1 = 9;
    const unsigned int kUint_2 = 0xa3;
    const unsigned int kUint_3 = 0x8765;
    const unsigned int kUint_4 = SPINEL_MAX_UINT_PACKED - 1;

    const spinel_ipv6addr_t kIp6Addr = {
        {0x6B, 0x41, 0x65, 0x73, 0x42, 0x68, 0x61, 0x76, 0x54, 0x61, 0x72, 0x7A, 0x49, 0x69, 0x61, 0x4E}};

    const spinel_eui48_t kEui48 = {
        {4, 8, 15, 16, 23, 42} // "Lost" EUI48!
    };

    const spinel_eui64_t kEui64 = {
        {2, 3, 5, 7, 11, 13, 17, 19}, // "Prime" EUI64!
    };

    const char kString_1[] = "OpenThread";
    const char kString_2[] = "";

    const uint16_t kData[] = {10, 20, 3, 15, 1000, 60, 16}; // ... then comes 17,18,19,20  :)

    bool               b_1, b_2;
    uint8_t            u8;
    int8_t             i8;
    uint16_t           u16;
    int16_t            i16;
    uint32_t           u32;
    int32_t            i32;
    uint64_t           u64;
    int64_t            i64;
    unsigned int       u_1, u_2, u_3, u_4;
    spinel_ipv6addr_t *ip6Addr;
    spinel_eui48_t *   eui48;
    spinel_eui64_t *   eui64;
    const char *       utf_1;
    const char *       utf_2;
    const uint8_t *    dataPtr;
    spinel_size_t      dataLen;

    memset(buffer, 0, sizeof(buffer));

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 1: Encoding of simple types");

    SuccessOrQuit(encoder.BeginFrame(Spinel::Buffer::kPriorityLow));
    SuccessOrQuit(encoder.WriteBool(kBool_1));
    SuccessOrQuit(encoder.WriteBool(kBool_2));
    SuccessOrQuit(encoder.WriteUint8(kUint8));
    SuccessOrQuit(encoder.WriteInt8(kInt8));
    SuccessOrQuit(encoder.WriteUint16(kUint16));
    SuccessOrQuit(encoder.WriteInt16(kInt16));
    SuccessOrQuit(encoder.WriteUint32(kUint32));
    SuccessOrQuit(encoder.WriteInt32(kInt32));
    SuccessOrQuit(encoder.WriteUint64(kUint64));
    SuccessOrQuit(encoder.WriteInt64(kInt64));
    SuccessOrQuit(encoder.WriteUintPacked(kUint_1));
    SuccessOrQuit(encoder.WriteUintPacked(kUint_2));
    SuccessOrQuit(encoder.WriteUintPacked(kUint_3));
    SuccessOrQuit(encoder.WriteUintPacked(kUint_4));
    SuccessOrQuit(encoder.WriteIp6Address(kIp6Addr));
    SuccessOrQuit(encoder.WriteEui48(kEui48));
    SuccessOrQuit(encoder.WriteEui64(kEui64));
    SuccessOrQuit(encoder.WriteUtf8(kString_1));
    SuccessOrQuit(encoder.WriteUtf8(kString_2));
    SuccessOrQuit(encoder.WriteData((const uint8_t *)kData, sizeof(kData)));
    SuccessOrQuit(encoder.EndFrame());

    DumpBuffer("Buffer", buffer, 256);
    SuccessOrQuit(ReadFrame(ncpBuffer, frame, frameLen));
    DumpBuffer("Frame", frame, frameLen);

    parsedLen = spinel_datatype_unpack(
        frame, (spinel_size_t)frameLen,
        (SPINEL_DATATYPE_BOOL_S SPINEL_DATATYPE_BOOL_S SPINEL_DATATYPE_UINT8_S SPINEL_DATATYPE_INT8_S
             SPINEL_DATATYPE_UINT16_S SPINEL_DATATYPE_INT16_S SPINEL_DATATYPE_UINT32_S SPINEL_DATATYPE_INT32_S
                 SPINEL_DATATYPE_UINT64_S SPINEL_DATATYPE_INT64_S SPINEL_DATATYPE_UINT_PACKED_S
                     SPINEL_DATATYPE_UINT_PACKED_S SPINEL_DATATYPE_UINT_PACKED_S SPINEL_DATATYPE_UINT_PACKED_S
                         SPINEL_DATATYPE_IPv6ADDR_S SPINEL_DATATYPE_EUI48_S SPINEL_DATATYPE_EUI64_S
                             SPINEL_DATATYPE_UTF8_S SPINEL_DATATYPE_UTF8_S SPINEL_DATATYPE_DATA_S),
        &b_1, &b_2, &u8, &i8, &u16, &i16, &u32, &i32, &u64, &i64, &u_1, &u_2, &u_3, &u_4, &ip6Addr, &eui48, &eui64,
        &utf_1, &utf_2, &dataPtr, &dataLen);

    VerifyOrQuit(parsedLen == frameLen);
    VerifyOrQuit(b_1 == kBool_1);
    VerifyOrQuit(b_2 == kBool_2);
    VerifyOrQuit(u8 == kUint8);
    VerifyOrQuit(i8 == kInt8);
    VerifyOrQuit(u16 == kUint16);
    VerifyOrQuit(i16 == kInt16);
    VerifyOrQuit(u32 == kUint32);
    VerifyOrQuit(i32 == kInt32);
    VerifyOrQuit(u64 == kUint64);
    VerifyOrQuit(i64 == kInt64);
    VerifyOrQuit(u_1 == kUint_1);
    VerifyOrQuit(u_2 == kUint_2);
    VerifyOrQuit(u_3 == kUint_3);
    VerifyOrQuit(u_4 == kUint_4);
    VerifyOrQuit(memcmp(ip6Addr, &kIp6Addr, sizeof(spinel_ipv6addr_t)) == 0);
    VerifyOrQuit(memcmp(eui48, &kEui48, sizeof(spinel_eui48_t)) == 0);
    VerifyOrQuit(memcmp(eui64, &kEui64, sizeof(spinel_eui64_t)) == 0);
    VerifyOrQuit(memcmp(utf_1, kString_1, sizeof(kString_1)) == 0);
    VerifyOrQuit(memcmp(utf_2, kString_2, sizeof(kString_2)) == 0);
    VerifyOrQuit(dataLen == sizeof(kData));
    VerifyOrQuit(memcmp(dataPtr, &kData, sizeof(kData)) == 0);

    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 2: Test a single simple struct.");

    SuccessOrQuit(encoder.BeginFrame(Spinel::Buffer::kPriorityLow));
    SuccessOrQuit(encoder.WriteUint8(kUint8));
    SuccessOrQuit(encoder.OpenStruct());
    {
        SuccessOrQuit(encoder.WriteUint32(kUint32));
        SuccessOrQuit(encoder.WriteEui48(kEui48));
        SuccessOrQuit(encoder.WriteUintPacked(kUint_3));
    }
    SuccessOrQuit(encoder.CloseStruct());
    SuccessOrQuit(encoder.WriteInt16(kInt16));
    SuccessOrQuit(encoder.EndFrame());

    DumpBuffer("Buffer", buffer, 256);
    SuccessOrQuit(ReadFrame(ncpBuffer, frame, frameLen));
    DumpBuffer("Frame", frame, frameLen);

    parsedLen = spinel_datatype_unpack(
        frame, (spinel_size_t)frameLen,
        (SPINEL_DATATYPE_UINT8_S SPINEL_DATATYPE_STRUCT_S(
            SPINEL_DATATYPE_UINT32_S SPINEL_DATATYPE_EUI48_S SPINEL_DATATYPE_UINT_PACKED_S) SPINEL_DATATYPE_INT16_S

         ),
        &u8, &u32, &eui48, &u_3, &i16);

    VerifyOrQuit(parsedLen == frameLen);
    VerifyOrQuit(u8 == kUint8);
    VerifyOrQuit(i16 == kInt16);
    VerifyOrQuit(u32 == kUint32);
    VerifyOrQuit(u_3 == kUint_3);
    VerifyOrQuit(memcmp(eui48, &kEui48, sizeof(spinel_eui48_t)) == 0);

    // Parse the struct as a "data with len".
    parsedLen = spinel_datatype_unpack(frame, (spinel_size_t)frameLen,
                                       (SPINEL_DATATYPE_UINT8_S SPINEL_DATATYPE_DATA_WLEN_S SPINEL_DATATYPE_INT16_S

                                        ),
                                       &u8, &dataPtr, &dataLen, &i16);

    VerifyOrQuit(parsedLen == frameLen);
    VerifyOrQuit(u8 == kUint8);
    VerifyOrQuit(i16 == kInt16);

    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 3: Test multiple structs and struct within struct.");

    SuccessOrQuit(encoder.BeginFrame(Spinel::Buffer::kPriorityLow));
    SuccessOrQuit(encoder.OpenStruct());
    {
        SuccessOrQuit(encoder.WriteUint8(kUint8));
        SuccessOrQuit(encoder.WriteUtf8(kString_1));
        SuccessOrQuit(encoder.OpenStruct());
        {
            SuccessOrQuit(encoder.WriteBool(kBool_1));
            SuccessOrQuit(encoder.WriteIp6Address(kIp6Addr));
        }
        SuccessOrQuit(encoder.CloseStruct());
        SuccessOrQuit(encoder.WriteUint16(kUint16));
    }
    SuccessOrQuit(encoder.CloseStruct());
    SuccessOrQuit(encoder.WriteEui48(kEui48));
    SuccessOrQuit(encoder.OpenStruct());
    {
        SuccessOrQuit(encoder.WriteUint32(kUint32));
    }
    SuccessOrQuit(encoder.CloseStruct());
    SuccessOrQuit(encoder.WriteInt32(kInt32));
    SuccessOrQuit(encoder.EndFrame());

    DumpBuffer("Buffer", buffer, 256 + 100);

    SuccessOrQuit(ReadFrame(ncpBuffer, frame, frameLen));

    parsedLen = spinel_datatype_unpack(
        frame, (spinel_size_t)frameLen,
        (SPINEL_DATATYPE_STRUCT_S(SPINEL_DATATYPE_UINT8_S SPINEL_DATATYPE_UTF8_S SPINEL_DATATYPE_STRUCT_S(
            SPINEL_DATATYPE_BOOL_S SPINEL_DATATYPE_IPv6ADDR_S) SPINEL_DATATYPE_UINT16_S)
             SPINEL_DATATYPE_EUI48_S SPINEL_DATATYPE_STRUCT_S(SPINEL_DATATYPE_UINT32_S) SPINEL_DATATYPE_INT32_S),
        &u8, &utf_1, &b_1, &ip6Addr, &u16, &eui48, &u32, &i32);

    VerifyOrQuit(parsedLen == frameLen);
    VerifyOrQuit(b_1 == kBool_1);
    VerifyOrQuit(u8 == kUint8);
    VerifyOrQuit(u16 == kUint16);
    VerifyOrQuit(u32 == kUint32);
    VerifyOrQuit(i32 == kInt32);
    VerifyOrQuit(memcmp(ip6Addr, &kIp6Addr, sizeof(spinel_ipv6addr_t)) == 0);
    VerifyOrQuit(memcmp(eui48, &kEui48, sizeof(spinel_eui48_t)) == 0);
    VerifyOrQuit(memcmp(utf_1, kString_1, sizeof(kString_1)) == 0);

    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 4: Test unclosed struct.");

    SuccessOrQuit(encoder.BeginFrame(Spinel::Buffer::kPriorityLow));
    SuccessOrQuit(encoder.WriteUint8(kUint8));
    SuccessOrQuit(encoder.OpenStruct());
    {
        SuccessOrQuit(encoder.WriteUint32(kUint32));
        SuccessOrQuit(encoder.OpenStruct());
        {
            SuccessOrQuit(encoder.WriteEui48(kEui48));
            SuccessOrQuit(encoder.WriteUintPacked(kUint_3));
            // Do not close the structs expecting `EndFrame()` to close them.
        }
    }
    SuccessOrQuit(encoder.EndFrame());

    SuccessOrQuit(ReadFrame(ncpBuffer, frame, frameLen));

    parsedLen = spinel_datatype_unpack(
        frame, (spinel_size_t)frameLen,
        (SPINEL_DATATYPE_UINT8_S SPINEL_DATATYPE_STRUCT_S(
            SPINEL_DATATYPE_UINT32_S SPINEL_DATATYPE_STRUCT_S(SPINEL_DATATYPE_EUI48_S SPINEL_DATATYPE_UINT_PACKED_S))),
        &u8, &u32, &eui48, &u_3);

    VerifyOrQuit(parsedLen == frameLen);
    VerifyOrQuit(u8 == kUint8);
    VerifyOrQuit(u32 == kUint32);
    VerifyOrQuit(u_3 == kUint_3);
    VerifyOrQuit(memcmp(eui48, &kEui48, sizeof(spinel_eui48_t)) == 0);

    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 5: Test saving position and reseting back to a saved position");

    SuccessOrQuit(encoder.BeginFrame(Spinel::Buffer::kPriorityLow));
    SuccessOrQuit(encoder.WriteUint8(kUint8));
    SuccessOrQuit(encoder.OpenStruct());
    {
        SuccessOrQuit(encoder.WriteUint32(kUint32));

        // Save position in middle a first open struct.
        SuccessOrQuit(encoder.SavePosition());
        SuccessOrQuit(encoder.OpenStruct());
        {
            SuccessOrQuit(encoder.WriteEui48(kEui48));
            SuccessOrQuit(encoder.WriteUintPacked(kUint_3));
        }

        // Reset to saved position in middle of the second open struct which should be discarded.

        SuccessOrQuit(encoder.ResetToSaved());

        SuccessOrQuit(encoder.WriteIp6Address(kIp6Addr));
        SuccessOrQuit(encoder.WriteEui64(kEui64));
    }
    SuccessOrQuit(encoder.CloseStruct());
    SuccessOrQuit(encoder.WriteUtf8(kString_1));
    SuccessOrQuit(encoder.EndFrame());

    SuccessOrQuit(ReadFrame(ncpBuffer, frame, frameLen));

    parsedLen = spinel_datatype_unpack(
        frame, (spinel_size_t)frameLen,
        (SPINEL_DATATYPE_UINT8_S SPINEL_DATATYPE_STRUCT_S(
            SPINEL_DATATYPE_UINT32_S SPINEL_DATATYPE_IPv6ADDR_S SPINEL_DATATYPE_EUI64_S) SPINEL_DATATYPE_UTF8_S),
        &u8, &u32, &ip6Addr, &eui64, &utf_1);

    VerifyOrQuit(parsedLen == frameLen);

    VerifyOrQuit(u8 == kUint8);
    VerifyOrQuit(u32 == kUint32);
    VerifyOrQuit(i32 == kInt32);
    VerifyOrQuit(memcmp(ip6Addr, &kIp6Addr, sizeof(spinel_ipv6addr_t)) == 0);
    VerifyOrQuit(memcmp(eui64, &kEui64, sizeof(spinel_eui64_t)) == 0);
    VerifyOrQuit(memcmp(utf_1, kString_1, sizeof(kString_1)) == 0);

    printf(" -- PASS\n");
}

} // namespace Spinel
} // namespace ot

int main(void)
{
    ot::Spinel::TestEncoder();
    printf("\nAll tests passed.\n");
    return 0;
}
