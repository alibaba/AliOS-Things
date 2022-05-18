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
#include "lib/spinel/spinel_decoder.hpp"

#include "test_util.hpp"

namespace ot {
namespace Spinel {

enum
{
    kTestBufferSize = 800,
};

void TestDecoder(void)
{
    uint8_t         buffer[kTestBufferSize];
    Spinel::Decoder decoder;

    spinel_ssize_t frameLen;

    const bool     kBool_1 = true;
    const bool     kBool_2 = false;
    const uint8_t  kUint8  = 0x42;
    const int8_t   kInt8   = -73;
    const uint16_t kUint16 = 0xabcd;
    const int16_t  kInt16  = -567;
    const uint32_t kUint32 = 0xdeadbeef;
    const int32_t  kInt32  = -123455678L;
    const uint64_t kUint64 = 0xfe10dc32ba549876ULL;
    const int64_t  kInt64  = -9197712039090021561LL;

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

    bool                     b_1, b_2;
    uint8_t                  u8;
    int8_t                   i8;
    uint16_t                 u16;
    int16_t                  i16;
    uint32_t                 u32;
    int32_t                  i32;
    uint64_t                 u64;
    int64_t                  i64;
    unsigned int             u_1, u_2, u_3, u_4;
    const spinel_ipv6addr_t *ip6Addr;
    const spinel_eui48_t *   eui48;
    const spinel_eui64_t *   eui64;
    const char *             utf_1;
    const char *             utf_2;
    const uint8_t *          dataPtr_1;
    const uint8_t *          dataPtr_2;
    uint16_t                 dataLen_1;
    uint16_t                 dataLen_2;

    memset(buffer, 0, sizeof(buffer));

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 1: Decoding simple types");

    frameLen = spinel_datatype_pack(
        buffer, sizeof(buffer),
        (SPINEL_DATATYPE_BOOL_S SPINEL_DATATYPE_BOOL_S SPINEL_DATATYPE_UINT8_S SPINEL_DATATYPE_INT8_S
             SPINEL_DATATYPE_UINT16_S SPINEL_DATATYPE_INT16_S SPINEL_DATATYPE_UINT32_S SPINEL_DATATYPE_INT32_S
                 SPINEL_DATATYPE_UINT64_S SPINEL_DATATYPE_INT64_S SPINEL_DATATYPE_UINT_PACKED_S
                     SPINEL_DATATYPE_UINT_PACKED_S SPINEL_DATATYPE_UINT_PACKED_S SPINEL_DATATYPE_UINT_PACKED_S
                         SPINEL_DATATYPE_IPv6ADDR_S SPINEL_DATATYPE_EUI48_S SPINEL_DATATYPE_EUI64_S
                             SPINEL_DATATYPE_UTF8_S SPINEL_DATATYPE_UTF8_S SPINEL_DATATYPE_DATA_WLEN_S
                                 SPINEL_DATATYPE_DATA_S),
        kBool_1, kBool_2, kUint8, kInt8, kUint16, kInt16, kUint32, kInt32, kUint64, kInt64, kUint_1, kUint_2, kUint_3,
        kUint_4, &kIp6Addr, &kEui48, &kEui64, kString_1, kString_2, kData, sizeof(kData), kData, sizeof(kData));

    DumpBuffer("Packed Spinel Frame", buffer, static_cast<uint16_t>(frameLen));

    decoder.Init(buffer, static_cast<uint16_t>(frameLen));

    VerifyOrQuit(decoder.GetFrame() == &buffer[0]);
    VerifyOrQuit(decoder.GetLength() == frameLen);

    VerifyOrQuit(decoder.GetReadLength() == 0);
    VerifyOrQuit(decoder.GetRemainingLength() == frameLen);
    VerifyOrQuit(decoder.IsAllRead() == false);

    SuccessOrQuit(decoder.ReadBool(b_1));
    SuccessOrQuit(decoder.ReadBool(b_2));
    SuccessOrQuit(decoder.ReadUint8(u8));
    SuccessOrQuit(decoder.ReadInt8(i8));
    SuccessOrQuit(decoder.ReadUint16(u16));
    SuccessOrQuit(decoder.ReadInt16(i16));
    SuccessOrQuit(decoder.ReadUint32(u32));
    SuccessOrQuit(decoder.ReadInt32(i32));
    SuccessOrQuit(decoder.ReadUint64(u64));
    SuccessOrQuit(decoder.ReadInt64(i64));

    // Check the state
    VerifyOrQuit(decoder.GetReadLength() != 0);
    VerifyOrQuit(decoder.GetRemainingLength() == frameLen - decoder.GetReadLength());
    VerifyOrQuit(decoder.IsAllRead() == false);

    SuccessOrQuit(decoder.ReadUintPacked(u_1));

    SuccessOrQuit(decoder.ReadUintPacked(u_2));
    SuccessOrQuit(decoder.ReadUintPacked(u_3));
    SuccessOrQuit(decoder.ReadUintPacked(u_4));
    SuccessOrQuit(decoder.ReadIp6Address(ip6Addr));
    SuccessOrQuit(decoder.ReadEui48(eui48));
    SuccessOrQuit(decoder.ReadEui64(eui64));
    SuccessOrQuit(decoder.ReadUtf8(utf_1));
    SuccessOrQuit(decoder.ReadUtf8(utf_2));
    SuccessOrQuit(decoder.ReadDataWithLen(dataPtr_1, dataLen_1));
    SuccessOrQuit(decoder.ReadData(dataPtr_2, dataLen_2));

    VerifyOrQuit(decoder.GetReadLength() == frameLen);
    VerifyOrQuit(decoder.GetRemainingLength() == 0);
    VerifyOrQuit(decoder.IsAllRead() == true);

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
    VerifyOrQuit(dataLen_1 == sizeof(kData));
    VerifyOrQuit(memcmp(dataPtr_1, &kData, sizeof(kData)) == 0);
    VerifyOrQuit(dataLen_2 == sizeof(kData));
    VerifyOrQuit(memcmp(dataPtr_2, &kData, sizeof(kData)) == 0);

    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 2: Test Reset(), SavePosition(), ResetToSaved()");

    // `ResetToSaved()` should fail if there is no saved position
    VerifyOrQuit(decoder.ResetToSaved() == OT_ERROR_INVALID_STATE);

    decoder.Reset();

    VerifyOrQuit(decoder.GetFrame() == &buffer[0]);
    VerifyOrQuit(decoder.GetLength() == frameLen);
    VerifyOrQuit(decoder.GetReadLength() == 0);
    VerifyOrQuit(decoder.GetRemainingLength() == frameLen);
    VerifyOrQuit(decoder.IsAllRead() == false);

    SuccessOrQuit(decoder.ReadBool(b_1));
    SuccessOrQuit(decoder.ReadBool(b_2));
    SuccessOrQuit(decoder.ReadUint8(u8));
    SuccessOrQuit(decoder.ReadInt8(i8));
    SuccessOrQuit(decoder.ReadUint16(u16));
    SuccessOrQuit(decoder.ReadInt16(i16));
    SuccessOrQuit(decoder.ReadUint32(u32));
    SuccessOrQuit(decoder.ReadInt32(i32));

    // `ResetToSaved()` should fail if there is no saved position
    VerifyOrQuit(decoder.ResetToSaved() == OT_ERROR_INVALID_STATE);

    // Save position
    decoder.SavePosition();

    SuccessOrQuit(decoder.ReadUint64(u64));
    SuccessOrQuit(decoder.ReadInt64(i64));
    SuccessOrQuit(decoder.ReadUintPacked(u_1));
    SuccessOrQuit(decoder.ReadUintPacked(u_2));
    SuccessOrQuit(decoder.ReadUintPacked(u_3));
    SuccessOrQuit(decoder.ReadUintPacked(u_4));
    SuccessOrQuit(decoder.ReadIp6Address(ip6Addr));

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

    SuccessOrQuit(decoder.ResetToSaved());

    SuccessOrQuit(decoder.ReadUint64(u64));
    SuccessOrQuit(decoder.ReadInt64(i64));
    SuccessOrQuit(decoder.ReadUintPacked(u_1));
    SuccessOrQuit(decoder.ReadUintPacked(u_2));
    SuccessOrQuit(decoder.ReadUintPacked(u_3));
    SuccessOrQuit(decoder.ReadUintPacked(u_4));
    SuccessOrQuit(decoder.ReadIp6Address(ip6Addr));

    VerifyOrQuit(u64 == kUint64);
    VerifyOrQuit(i64 == kInt64);
    VerifyOrQuit(u_1 == kUint_1);
    VerifyOrQuit(u_2 == kUint_2);
    VerifyOrQuit(u_3 == kUint_3);
    VerifyOrQuit(u_4 == kUint_4);
    VerifyOrQuit(memcmp(ip6Addr, &kIp6Addr, sizeof(spinel_ipv6addr_t)) == 0);

    // Go back to save position again.
    SuccessOrQuit(decoder.ResetToSaved());

    SuccessOrQuit(decoder.ReadUint64(u64));
    SuccessOrQuit(decoder.ReadInt64(i64));
    SuccessOrQuit(decoder.ReadUintPacked(u_1));
    SuccessOrQuit(decoder.ReadUintPacked(u_2));
    SuccessOrQuit(decoder.ReadUintPacked(u_3));
    SuccessOrQuit(decoder.ReadUintPacked(u_4));
    SuccessOrQuit(decoder.ReadIp6Address(ip6Addr));

    VerifyOrQuit(u64 == kUint64);
    VerifyOrQuit(i64 == kInt64);
    VerifyOrQuit(u_1 == kUint_1);
    VerifyOrQuit(u_2 == kUint_2);
    VerifyOrQuit(u_3 == kUint_3);
    VerifyOrQuit(u_4 == kUint_4);
    VerifyOrQuit(memcmp(ip6Addr, &kIp6Addr, sizeof(spinel_ipv6addr_t)) == 0);

    // Ensure saved position is cleared when decoder is reset or re-initialized.

    decoder.Reset();

    // `ResetToSaved()` should fail if there is no saved position
    VerifyOrQuit(decoder.ResetToSaved() == OT_ERROR_INVALID_STATE);

    decoder.SavePosition();
    SuccessOrQuit(decoder.ResetToSaved());

    decoder.Init(buffer, static_cast<uint16_t>(frameLen));
    VerifyOrQuit(decoder.ResetToSaved() == OT_ERROR_INVALID_STATE);

    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 3: Test decoding a single simple struct.");

    frameLen = spinel_datatype_pack(buffer, sizeof(buffer),
                                    (SPINEL_DATATYPE_UINT8_S SPINEL_DATATYPE_STRUCT_S(
                                        SPINEL_DATATYPE_UINT32_S SPINEL_DATATYPE_EUI48_S SPINEL_DATATYPE_UINT_PACKED_S)
                                         SPINEL_DATATYPE_INT16_S

                                     ),
                                    kUint8, kUint32, &kEui48, kUint_3, kInt16);

    DumpBuffer("Packed Spinel Frame (single struct)", buffer, static_cast<uint16_t>(frameLen));

    decoder.Init(buffer, static_cast<uint16_t>(frameLen));

    SuccessOrQuit(decoder.ReadUint8(u8));
    SuccessOrQuit(decoder.OpenStruct());
    {
        SuccessOrQuit(decoder.ReadUint32(u32));
        SuccessOrQuit(decoder.ReadEui48(eui48));
        SuccessOrQuit(decoder.ReadUintPacked(u_3));
    }
    SuccessOrQuit(decoder.CloseStruct());
    SuccessOrQuit(decoder.ReadInt16(i16));
    VerifyOrQuit(decoder.IsAllRead() == true);

    VerifyOrQuit(u8 == kUint8);
    VerifyOrQuit(i16 == kInt16);
    VerifyOrQuit(u32 == kUint32);
    VerifyOrQuit(u_3 == kUint_3);
    VerifyOrQuit(memcmp(eui48, &kEui48, sizeof(spinel_eui48_t)) == 0);

    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 4: Test partial struct read");

    // Re-use same frame as the previous test.

    decoder.Init(buffer, static_cast<uint16_t>(frameLen));

    SuccessOrQuit(decoder.ReadUint8(u8));
    SuccessOrQuit(decoder.OpenStruct());
    {
        SuccessOrQuit(decoder.ReadUint32(u32));
        // Skip the remaining fields in the struct
    }
    SuccessOrQuit(decoder.CloseStruct());
    SuccessOrQuit(decoder.ReadInt16(i16));

    VerifyOrQuit(u8 == kUint8);
    VerifyOrQuit(i16 == kInt16);

    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 5: Test `GetRemainingLengthInStruct()` and `IsAllReadInStruct`() in and out of an struct");

    // Re-use same frame as the previous test.

    decoder.Init(buffer, static_cast<uint16_t>(frameLen));

    VerifyOrQuit(decoder.GetFrame() == &buffer[0]);
    VerifyOrQuit(decoder.GetLength() == frameLen);

    VerifyOrQuit(decoder.GetReadLength() == 0);
    VerifyOrQuit(decoder.GetRemainingLength() == frameLen);
    VerifyOrQuit(decoder.IsAllRead() == false);

    // When not in an struct,  `etRemainingLengthInStruct()` should consider the whole frame.
    VerifyOrQuit(decoder.GetRemainingLengthInStruct() == frameLen);
    VerifyOrQuit(decoder.IsAllReadInStruct() == false);

    SuccessOrQuit(decoder.ReadUint8(u8));
    SuccessOrQuit(decoder.OpenStruct());
    {
        VerifyOrQuit(decoder.IsAllReadInStruct() == false);

        SuccessOrQuit(decoder.ReadUint32(u32));
        SuccessOrQuit(decoder.ReadEui48(eui48));
        SuccessOrQuit(decoder.ReadUintPacked(u_3));

        VerifyOrQuit(decoder.IsAllReadInStruct() == true);
        VerifyOrQuit(decoder.GetRemainingLengthInStruct() == 0);

        // Try reading beyond end of the struct and ensure it fails.
        VerifyOrQuit(decoder.ReadUint8(u8) == OT_ERROR_PARSE);

        // `ReadData()` at end of struct should still succeed but return zero as the data length.
        SuccessOrQuit(decoder.ReadData(dataPtr_1, dataLen_1));
        VerifyOrQuit(dataLen_1 == 0);
    }
    SuccessOrQuit(decoder.CloseStruct());

    VerifyOrQuit(decoder.IsAllReadInStruct() == false);
    SuccessOrQuit(decoder.ReadInt16(i16));
    VerifyOrQuit(decoder.IsAllRead() == true);

    VerifyOrQuit(decoder.GetRemainingLengthInStruct() == 0);
    VerifyOrQuit(decoder.IsAllReadInStruct() == true);

    // `ReadData()` at end of frame should still succeed but return zero as the data length.
    SuccessOrQuit(decoder.ReadData(dataPtr_1, dataLen_1));
    VerifyOrQuit(dataLen_1 == 0);

    VerifyOrQuit(u8 == kUint8);
    VerifyOrQuit(i16 == kInt16);
    VerifyOrQuit(u32 == kUint32);
    VerifyOrQuit(u_3 == kUint_3);
    VerifyOrQuit(memcmp(eui48, &kEui48, sizeof(spinel_eui48_t)) == 0);

    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 6: Test multiple nested structs");

    frameLen = spinel_datatype_pack(
        buffer, sizeof(buffer),
        (SPINEL_DATATYPE_STRUCT_S(SPINEL_DATATYPE_UINT8_S SPINEL_DATATYPE_UTF8_S SPINEL_DATATYPE_STRUCT_S(
            SPINEL_DATATYPE_BOOL_S SPINEL_DATATYPE_IPv6ADDR_S) SPINEL_DATATYPE_UINT16_S)
             SPINEL_DATATYPE_EUI48_S SPINEL_DATATYPE_STRUCT_S(SPINEL_DATATYPE_UINT32_S) SPINEL_DATATYPE_INT32_S),
        kUint8, kString_1, kBool_1, &kIp6Addr, kUint16, &kEui48, kUint32, kInt32);

    DumpBuffer("Packed Spinel Frame (multiple struct)", buffer, static_cast<uint16_t>(frameLen));

    decoder.Init(buffer, static_cast<uint16_t>(frameLen));

    SuccessOrQuit(decoder.OpenStruct());
    {
        SuccessOrQuit(decoder.ReadUint8(u8));
        SuccessOrQuit(decoder.ReadUtf8(utf_1));
        SuccessOrQuit(decoder.OpenStruct());
        {
            SuccessOrQuit(decoder.ReadBool(b_1));
            SuccessOrQuit(decoder.ReadIp6Address(ip6Addr));
        }
        SuccessOrQuit(decoder.CloseStruct());
        SuccessOrQuit(decoder.ReadUint16(u16));
    }
    SuccessOrQuit(decoder.CloseStruct());
    SuccessOrQuit(decoder.ReadEui48(eui48));
    SuccessOrQuit(decoder.OpenStruct());
    {
        SuccessOrQuit(decoder.ReadUint32(u32));
    }
    SuccessOrQuit(decoder.CloseStruct());
    SuccessOrQuit(decoder.ReadInt32(i32));

    VerifyOrQuit(decoder.GetReadLength() == frameLen);
    VerifyOrQuit(decoder.GetRemainingLength() == 0);
    VerifyOrQuit(decoder.IsAllRead() == true);

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
    printf("\nTest 7: Test `SavePosition()`, `ResetToSaved()` for nested structs");

    // Re-use same frame as the previous test.

    decoder.Init(buffer, static_cast<uint16_t>(frameLen));

    SuccessOrQuit(decoder.OpenStruct());
    {
        SuccessOrQuit(decoder.ReadUint8(u8));

        decoder.SavePosition();

        SuccessOrQuit(decoder.ReadUtf8(utf_1));
        SuccessOrQuit(decoder.OpenStruct());
        {
            SuccessOrQuit(decoder.ReadBool(b_1));
        }

        // Verify the read content so far.

        VerifyOrQuit(u8 == kUint8);
        VerifyOrQuit(b_1 == kBool_1);
        VerifyOrQuit(memcmp(utf_1, kString_1, sizeof(kString_1)) == 0);

        // Do not close the inner struct and jump to previously saved position and re-read the content.

        SuccessOrQuit(decoder.ResetToSaved());

        SuccessOrQuit(decoder.ReadUtf8(utf_1));
        SuccessOrQuit(decoder.OpenStruct());
        {
            SuccessOrQuit(decoder.ReadBool(b_1));
            SuccessOrQuit(decoder.ReadIp6Address(ip6Addr));
        }
        SuccessOrQuit(decoder.CloseStruct());
        SuccessOrQuit(decoder.ReadUint16(u16));
    }
    SuccessOrQuit(decoder.CloseStruct());
    SuccessOrQuit(decoder.ReadEui48(eui48));
    SuccessOrQuit(decoder.OpenStruct());
    {
        SuccessOrQuit(decoder.ReadUint32(u32));
    }
    SuccessOrQuit(decoder.CloseStruct());
    SuccessOrQuit(decoder.ReadInt32(i32));

    VerifyOrQuit(decoder.GetReadLength() == frameLen);
    VerifyOrQuit(decoder.GetRemainingLength() == 0);
    VerifyOrQuit(decoder.IsAllRead() == true);

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
    printf("\nTest 8: Test saving position at start of an open struct");

    // Re-use same frame as the previous test.

    decoder.Init(buffer, static_cast<uint16_t>(frameLen));

    SuccessOrQuit(decoder.OpenStruct());
    {
        SuccessOrQuit(decoder.ReadUint8(u8));
        SuccessOrQuit(decoder.ReadUtf8(utf_1));
        SuccessOrQuit(decoder.OpenStruct());
        {
            // Save position at start of the struct
            decoder.SavePosition();
            SuccessOrQuit(decoder.ReadBool(b_1));

            // Verify the read content so far.

            VerifyOrQuit(u8 == kUint8);
            VerifyOrQuit(memcmp(utf_1, kString_1, sizeof(kString_1)) == 0);
            VerifyOrQuit(b_1 == kBool_1);

            // Do not close the struct and jump to the previously saved position and re-read the content.

            SuccessOrQuit(decoder.ResetToSaved());
            SuccessOrQuit(decoder.ReadBool(b_1));
            SuccessOrQuit(decoder.ReadIp6Address(ip6Addr));
        }
        SuccessOrQuit(decoder.CloseStruct());
        SuccessOrQuit(decoder.ReadUint16(u16));
    }
    SuccessOrQuit(decoder.CloseStruct());
    SuccessOrQuit(decoder.ReadEui48(eui48));
    SuccessOrQuit(decoder.OpenStruct());
    {
        SuccessOrQuit(decoder.ReadUint32(u32));
    }
    SuccessOrQuit(decoder.CloseStruct());
    SuccessOrQuit(decoder.ReadInt32(i32));

    VerifyOrQuit(decoder.GetReadLength() == frameLen);
    VerifyOrQuit(decoder.GetRemainingLength() == 0);
    VerifyOrQuit(decoder.IsAllRead() == true);

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
    printf("\nTest 9: Test `ResetToSaved()` failure case (jumping back to a saved position closed struct).");

    // Re-use same frame as the previous test.

    decoder.Init(buffer, static_cast<uint16_t>(frameLen));

    SuccessOrQuit(decoder.OpenStruct());
    {
        SuccessOrQuit(decoder.ReadUint8(u8));
        SuccessOrQuit(decoder.ReadUtf8(utf_1));
        SuccessOrQuit(decoder.OpenStruct());
        {
            SuccessOrQuit(decoder.ReadBool(b_1));

            decoder.SavePosition();

            SuccessOrQuit(decoder.ReadIp6Address(ip6Addr));
        }
        SuccessOrQuit(decoder.CloseStruct());
        SuccessOrQuit(decoder.ReadUint16(u16));

        // `ResetToSaved()` should fail since the enclosing struct for the saved position is closed.
        VerifyOrQuit(decoder.ResetToSaved() == OT_ERROR_INVALID_STATE);
    }

    printf(" -- PASS\n");

    printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    printf("\nTest 10: Testing error cases and failures. (e.g., wrong struct length).");

    frameLen = spinel_datatype_pack(buffer, sizeof(buffer),
                                    (SPINEL_DATATYPE_UINT8_S SPINEL_DATATYPE_UINT16_S // Treat this as struct length
                                         SPINEL_DATATYPE_BOOL_S),
                                    kUint8, 10, kBool_1);

    DumpBuffer("Packed Spinel Frame (incorrect format)", buffer, static_cast<uint16_t>(frameLen));

    decoder.Init(buffer, static_cast<uint16_t>(frameLen));

    decoder.SavePosition();

    SuccessOrQuit(decoder.ReadUint8(u8));
    VerifyOrQuit(u8 == kUint8);

    // `OpenStruct()` should fail, since it expects a length 10 but there are not enough
    // bytes in the frame.
    VerifyOrQuit(decoder.OpenStruct() == OT_ERROR_PARSE);

    SuccessOrQuit(decoder.ResetToSaved());

    SuccessOrQuit(decoder.ReadUint8(u8));
    VerifyOrQuit(u8 == kUint8);
    VerifyOrQuit(decoder.ReadDataWithLen(dataPtr_1, dataLen_1) == OT_ERROR_PARSE);

    SuccessOrQuit(decoder.ResetToSaved());
    SuccessOrQuit(decoder.ReadUint8(u8));
    SuccessOrQuit(decoder.ReadUint16(u16));
    SuccessOrQuit(decoder.ReadBool(b_1));

    // Try reading beyond end of frame.
    VerifyOrQuit(decoder.ReadUint8(u8) == OT_ERROR_PARSE);

    printf(" -- PASS\n");
}

} // namespace Spinel
} // namespace ot

int main(void)
{
    ot::Spinel::Decoder();
    printf("\nAll tests passed.\n");
    return 0;
}
