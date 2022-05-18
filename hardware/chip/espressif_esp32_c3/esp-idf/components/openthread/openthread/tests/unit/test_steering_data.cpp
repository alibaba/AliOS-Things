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

#include <openthread/config.h>

#include "test_util.hpp"
#include "meshcop/meshcop.hpp"

namespace ot {

void TestSteeringData(void)
{
    MeshCoP::SteeringData                 steeringData;
    MeshCoP::SteeringData::HashBitIndexes indexes;
    Mac::ExtAddress                       joinerId1;
    Mac::ExtAddress                       joinerId2;

    const uint8_t kAddress1[sizeof(Mac::ExtAddress)] = {0x10, 0x20, 0x03, 0x15, 0x10, 0x00, 0x60, 0x16};
    const uint8_t kAddress2[sizeof(Mac::ExtAddress)] = {0xbe, 0xef, 0xca, 0xfe, 0xde, 0xad, 0xba, 0xbe};

    joinerId1.Set(kAddress1);
    joinerId2.Set(kAddress2);

    MeshCoP::SteeringData::CalculateHashBitIndexes(joinerId2, indexes);

    steeringData.SetToPermitAllJoiners();

    DumpBuffer("After SetToPermitAllJoiners()", steeringData.GetData(), steeringData.GetLength());
    VerifyOrQuit(steeringData.GetLength() == 1, "after SetToPermitAllJoiners()");
    VerifyOrQuit(steeringData.PermitsAllJoiners(), "after SetToPermitAllJoiners()");
    VerifyOrQuit(!steeringData.IsEmpty(), "after SetToPermitAllJoiners()");
    VerifyOrQuit(steeringData.Contains(joinerId1), "after SetToPermitAllJoiners()");
    VerifyOrQuit(steeringData.Contains(joinerId2), "after SetToPermitAllJoiners()");
    VerifyOrQuit(steeringData.Contains(indexes), "after SetToPermitAllJoiners()");

    steeringData.Clear();

    DumpBuffer("After Clear()", steeringData.GetData(), steeringData.GetLength());
    VerifyOrQuit(steeringData.GetLength() == 1, "after Clear()");
    VerifyOrQuit(!steeringData.PermitsAllJoiners(), "after Clear()");
    VerifyOrQuit(steeringData.IsEmpty(), "after Clear()");
    VerifyOrQuit(!steeringData.Contains(joinerId1), "after Clear()");
    VerifyOrQuit(!steeringData.Contains(joinerId2), "after Clear()");
    VerifyOrQuit(!steeringData.Contains(indexes), "after Clear()");

    for (uint8_t len = 1; len <= MeshCoP::SteeringData::kMaxLength; len++)
    {
        printf("\n--------------------------------------------");

        steeringData.Init(len);

        VerifyOrQuit(steeringData.GetLength() == len, "after Init()");
        VerifyOrQuit(steeringData.IsEmpty(), "IsEmpy() failed after Init()");
        VerifyOrQuit(!steeringData.PermitsAllJoiners(), "after Init()");
        VerifyOrQuit(!steeringData.Contains(joinerId1), "after Init()");
        VerifyOrQuit(!steeringData.Contains(joinerId2), "after Init()");
        VerifyOrQuit(!steeringData.Contains(indexes), "after Init()");

        steeringData.UpdateBloomFilter(joinerId1);
        DumpBuffer("After UpdateBloomFilter(joinerId1)", steeringData.GetData(), steeringData.GetLength());
        VerifyOrQuit(steeringData.GetLength() == len, "after UpdateBloomFilter()");
        VerifyOrQuit(!steeringData.IsEmpty(), "IsEmpy() failed after UpdateBloomFilter()");
        VerifyOrQuit(!steeringData.PermitsAllJoiners(), "after UpdateBloomFilter");
        VerifyOrQuit(steeringData.Contains(joinerId1), "after UpdateBloomFilter");

        steeringData.UpdateBloomFilter(joinerId2);
        DumpBuffer("After UpdateBloomFilter(joinerId2)", steeringData.GetData(), steeringData.GetLength());
        VerifyOrQuit(steeringData.GetLength() == len, "after UpdateBloomFilter()");
        VerifyOrQuit(!steeringData.IsEmpty(), "IsEmpy() failed after UpdateBloomFilter()");
        VerifyOrQuit(!steeringData.PermitsAllJoiners(), "after UpdateBloomFilter");
        VerifyOrQuit(steeringData.Contains(joinerId1), "after UpdateBloomFilter");
        VerifyOrQuit(steeringData.Contains(joinerId2), "after UpdateBloomFilter");
        VerifyOrQuit(steeringData.Contains(indexes), "after UpdateBloomFilter");
    }

    steeringData.Init(0);

    VerifyOrQuit(steeringData.GetLength() == 0, "after Init()");
    VerifyOrQuit(steeringData.IsEmpty(), "IsEmpy() failed after Init()");
    VerifyOrQuit(!steeringData.PermitsAllJoiners(), "after Init()");
    VerifyOrQuit(!steeringData.Contains(joinerId1), "after Init()");
    VerifyOrQuit(!steeringData.Contains(joinerId2), "after Init()");
    VerifyOrQuit(!steeringData.Contains(indexes), "after Init()");
}

} // namespace ot

int main(void)
{
    ot::TestSteeringData();
    printf("\nAll tests passed.\n");
    return 0;
}
