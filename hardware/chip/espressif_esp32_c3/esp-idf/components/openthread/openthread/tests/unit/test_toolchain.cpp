/*
 *  Copyright (c) 2016, The OpenThread Authors.
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

#include <stdint.h>
#include <stdio.h>

#include <openthread/ip6.h>
#include <openthread/platform/toolchain.h>

#include "test_util.h"
#include "thread/topology.hpp"

extern "C" {
uint32_t       otNetifAddress_Size_c();
uint32_t       otNetifAddress_offset_mNext_c();
otNetifAddress CreateNetif_c();
}

void test_packed1(void)
{
    OT_TOOL_PACKED_BEGIN
    struct packed_t
    {
        uint8_t  mByte;
        uint32_t mWord;
        uint16_t mShort;
    } OT_TOOL_PACKED_END;

    static_assert(sizeof(packed_t) == 7, "packed_t should be packed to 7 bytes");

    VerifyOrQuit(sizeof(packed_t) == 7, "OT_TOOL_PACKED failed 1");
}

void test_packed2(void)
{
    OT_TOOL_PACKED_BEGIN
    struct packed_t
    {
        uint8_t mBytes[3];
        uint8_t mByte;
    } OT_TOOL_PACKED_END;

    static_assert(sizeof(packed_t) == 4, "packed_t should be packed to 4 bytes");

    VerifyOrQuit(sizeof(packed_t) == 4, "OT_TOOL_PACKED failed 2");
}

void test_packed_union(void)
{
    typedef struct
    {
        uint16_t mField;
    } nested_t;

    OT_TOOL_PACKED_BEGIN
    struct packed_t
    {
        uint8_t mBytes[3];
        union
        {
            nested_t mNestedStruct;
            uint8_t  mByte;
        } OT_TOOL_PACKED_FIELD;
    } OT_TOOL_PACKED_END;

    static_assert(sizeof(packed_t) == 5, "packed_t should be packed to 5 bytes");

    VerifyOrQuit(sizeof(packed_t) == 5, "OT_TOOL_PACKED failed 3");
}

void test_packed_enum(void)
{
    ot::Neighbor neighbor;
    neighbor.SetState(ot::Neighbor::kStateValid);

    // Make sure that when we read the 3 bit field it is read as unsigned, so it return '4'
    VerifyOrQuit(neighbor.GetState() == ot::Neighbor::kStateValid, "OT_TOOL_PACKED failed 4");
}

void test_addr_sizes(void)
{
    VerifyOrQuit(offsetof(otNetifAddress, mNext) == otNetifAddress_offset_mNext_c(),
                 "mNext should offset the same in C & C++");
    VerifyOrQuit(sizeof(otNetifAddress) == otNetifAddress_Size_c(), "otNetifAddress should the same in C & C++");
}

void test_addr_bitfield(void)
{
    VerifyOrQuit(CreateNetif_c().mScopeOverrideValid == true, "test_addr_size_cpp");
}

void test_packed_alignment(void)
{
    OT_TOOL_PACKED_BEGIN
    struct PackedStruct
    {
        uint32_t mUint32;
        uint8_t  mByte;
        uint16_t mUint16;
    } OT_TOOL_PACKED_END;

    PackedStruct   packedStruct;
    PackedStruct   packedStructCopy;
    const uint8_t *packedStructBytes = reinterpret_cast<const uint8_t *>(&packedStruct);
    uint8_t        buffer[sizeof(PackedStruct) * 2 + 1];

    VerifyOrQuit(sizeof(PackedStruct) == 7, "OT_TOOL_PACKED failed");

    packedStruct.mUint32 = 0x12345678;
    packedStruct.mByte   = 0xfe;
    packedStruct.mUint16 = 0xabcd;

    for (uint16_t start = 0; start < sizeof(PackedStruct); start++)
    {
        uint8_t *ptr = &buffer[start];

        memset(buffer, 0, sizeof(buffer));

        *reinterpret_cast<PackedStruct *>(ptr) = packedStruct;

        for (uint16_t i = 0; i < start; i++)
        {
            VerifyOrQuit(buffer[i] == 0, "OT_TOOL_PACKED alignment failed - pre-size write");
        }

        VerifyOrQuit(memcmp(ptr, packedStructBytes, sizeof(PackedStruct)) == 0, "OT_TOOL_PACKED alignment failed");

        for (uint16_t i = start + sizeof(packedStruct); i < sizeof(buffer); i++)
        {
            VerifyOrQuit(buffer[i] == 0, "OT_TOOL_PACKED alignment failed - post-size write");
        }

        memset(&packedStructCopy, 0, sizeof(PackedStruct));
        packedStructCopy = *reinterpret_cast<PackedStruct *>(ptr);

        VerifyOrQuit(memcmp(&packedStructCopy, &packedStruct, sizeof(PackedStruct)) == 0,
                     "OT_TOOL_PACKED failed - read error");
    }
}

void TestToolchain(void)
{
    test_packed1();
    test_packed2();
    test_packed_union();
    test_packed_enum();
    test_addr_sizes();
    test_addr_bitfield();
    test_packed_alignment();
}

int main(void)
{
    TestToolchain();
    printf("All tests passed\n");
    return 0;
}
