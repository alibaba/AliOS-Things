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

#include <openthread/config.h>

#include "test_platform.h"
#include "test_util.hpp"

#include "common/data.hpp"

namespace ot {

template <DataLengthType kDataLengthType> void TestData(void)
{
    typedef Data<kDataLengthType> Data;

    const uint8_t kData[]     = {0x12, 0x03, 0x19, 0x77};
    const uint8_t kDataCopy[] = {0x12, 0x03, 0x19, 0x77};

    Data     data;
    Data     data2;
    uint8_t  buffer[sizeof(kData) + 1];
    uint8_t  u8;
    uint16_t u16;

    data.Clear();
    data2.Clear();
    VerifyOrQuit(data.GetLength() == 0);
    VerifyOrQuit(data.GetBytes() == nullptr);
    VerifyOrQuit(data == data2);
    VerifyOrQuit(data.StartsWith(data));
    VerifyOrQuit(data2.StartsWith(data));

    data.Init(kData, sizeof(kData));
    VerifyOrQuit(data.GetLength() == sizeof(kData));
    VerifyOrQuit(data.GetBytes() == &kData[0]);
    VerifyOrQuit(data == data);
    VerifyOrQuit(data.StartsWith(data));

    memset(buffer, 0, sizeof(buffer));
    data.CopyBytesTo(buffer);
    VerifyOrQuit(memcmp(buffer, kData, sizeof(kData)) == 0);
    VerifyOrQuit(buffer[sizeof(kData)] == 0);
    VerifyOrQuit(data.MatchesBytesIn(buffer));

    data2.InitFrom(kDataCopy);
    VerifyOrQuit(data2.GetLength() == sizeof(kDataCopy));
    VerifyOrQuit(data2.GetBytes() == &kDataCopy[0]);
    VerifyOrQuit(data == data2);
    VerifyOrQuit(data.StartsWith(data2));
    VerifyOrQuit(data2.StartsWith(data));

    data2.SetLength(sizeof(kDataCopy) - 1);
    VerifyOrQuit(data2.GetLength() == sizeof(kDataCopy) - 1);
    VerifyOrQuit(data2.GetBytes() == &kDataCopy[0]);
    VerifyOrQuit(data2.MatchesBytesIn(kDataCopy));
    VerifyOrQuit(data != data2);
    VerifyOrQuit(data.StartsWith(data2));
    VerifyOrQuit(!data2.StartsWith(data));

    data2.InitFromRange(&kDataCopy[0], &kDataCopy[2]);
    VerifyOrQuit(data2.GetLength() == 2);
    VerifyOrQuit(data2.GetBytes() == &kDataCopy[0]);
    VerifyOrQuit(data != data2);
    VerifyOrQuit(data.StartsWith(data2));
    VerifyOrQuit(!data2.StartsWith(data));

    data2 = data;
    VerifyOrQuit(data2 == data);

    data.Clear();
    VerifyOrQuit(data.GetLength() == 0);
    VerifyOrQuit(data.GetBytes() == nullptr);
    VerifyOrQuit(data != data2);
    VerifyOrQuit(!data.StartsWith(data2));
    VerifyOrQuit(data2.StartsWith(data));

    memset(buffer, 0xaa, sizeof(buffer));
    data.CopyBytesTo(buffer);
    VerifyOrQuit(buffer[0] == 0xaa);

    data.InitFrom(u8);
    VerifyOrQuit(data.GetLength() == sizeof(u8));
    VerifyOrQuit(data.GetBytes() == &u8);

    data.InitFrom(u16);
    VerifyOrQuit(data.GetLength() == sizeof(u16));
    VerifyOrQuit(data.GetBytes() == reinterpret_cast<uint8_t *>(&u16));

    printf("- TestData<%s> passed\n", kDataLengthType == kWithUint8Length ? "kWithUint8Length" : "kWithUint16Length");
}

template <DataLengthType kDataLengthType> void TestMutableData(void)
{
    typedef Data<kDataLengthType>        Data;
    typedef MutableData<kDataLengthType> MutableData;

    constexpr uint8_t kMaxSize = 20;

    const uint8_t kData[]  = {10, 20, 3, 15, 10, 00, 60, 16};
    const uint8_t kData2[] = {0xab, 0xbc, 0xcd, 0xde, 0xef};

    MutableData mutableData;
    Data        data;
    uint8_t     buffer[kMaxSize];
    uint8_t     u8;
    uint16_t    u16;

    data.Init(kData, sizeof(kData));

    mutableData.Clear();
    VerifyOrQuit(mutableData.GetLength() == 0);
    VerifyOrQuit(mutableData.GetBytes() == nullptr);

    mutableData.Init(buffer, sizeof(buffer));
    VerifyOrQuit(mutableData.GetLength() == sizeof(buffer));
    VerifyOrQuit(mutableData.GetBytes() == &buffer[0]);

    SuccessOrQuit(mutableData.CopyBytesFrom(kData, sizeof(kData)));
    VerifyOrQuit(mutableData.GetLength() == sizeof(kData));
    VerifyOrQuit(mutableData.GetBytes() == &buffer[0]);
    VerifyOrQuit(mutableData == data);

    SuccessOrQuit(mutableData.CopyBytesFrom(kData2, sizeof(kData2)));
    VerifyOrQuit(mutableData.GetLength() == sizeof(kData2));
    VerifyOrQuit(mutableData.GetBytes() == &buffer[0]);
    VerifyOrQuit(memcmp(mutableData.GetBytes(), kData2, sizeof(kData2)) == 0);

    memset(buffer, 0, sizeof(buffer));
    mutableData.InitFrom(buffer);
    SuccessOrQuit(mutableData.CopyBytesFrom(kData, sizeof(kData)));
    VerifyOrQuit(mutableData == data);

    memset(buffer, 0, sizeof(buffer));
    SuccessOrQuit(mutableData.CopyBytesFrom(data));
    VerifyOrQuit(mutableData == data);

    memset(buffer, 0, sizeof(buffer));
    mutableData.InitFromRange(&buffer[0], &buffer[2]);
    VerifyOrQuit(mutableData.GetLength() == 2);
    VerifyOrQuit(mutableData.GetBytes() == &buffer[0]);

    VerifyOrQuit(mutableData.CopyBytesFrom(kData, sizeof(kData)) == kErrorNoBufs);
    VerifyOrQuit(mutableData.GetLength() == 2);
    VerifyOrQuit(mutableData.GetBytes() == &buffer[0]);
    VerifyOrQuit(memcmp(mutableData.GetBytes(), kData, 2) == 0);

    VerifyOrQuit(mutableData.CopyBytesFrom(data) == kErrorNoBufs);
    VerifyOrQuit(mutableData.GetLength() == 2);
    VerifyOrQuit(mutableData.GetBytes() == &buffer[0]);
    VerifyOrQuit(memcmp(mutableData.GetBytes(), kData, 2) == 0);

    memset(buffer, 0xff, sizeof(buffer));
    mutableData.InitFrom(buffer);
    VerifyOrQuit(mutableData.GetLength() == sizeof(buffer));
    VerifyOrQuit(mutableData.GetBytes() == &buffer[0]);

    u8 = 0xaa;
    mutableData.InitFrom(u8);
    VerifyOrQuit(mutableData.GetLength() == sizeof(u8));
    VerifyOrQuit(mutableData.GetBytes() == &u8);
    mutableData.ClearBytes();
    VerifyOrQuit(u8 == 0);

    u16 = 0x1234;
    mutableData.InitFrom(u16);
    VerifyOrQuit(mutableData.GetLength() == sizeof(u16));
    VerifyOrQuit(mutableData.GetBytes() == reinterpret_cast<uint8_t *>(&u16));
    mutableData.ClearBytes();
    VerifyOrQuit(u16 == 0);

    printf("- TestMutableData<%s> passed\n",
           kDataLengthType == kWithUint8Length ? "kWithUint8Length" : "kWithUint16Length");
}

} // namespace ot

int main(void)
{
    ot::TestData<ot::kWithUint8Length>();
    ot::TestData<ot::kWithUint16Length>();
    ot::TestMutableData<ot::kWithUint8Length>();
    ot::TestMutableData<ot::kWithUint16Length>();

    printf("All tests passed\n");
    return 0;
}
