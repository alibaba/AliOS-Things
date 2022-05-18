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

#include "openthread-core-config.h"

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE

#include "test_platform.h"

#include <openthread/config.h>
#include <openthread/ip6.h>

#include "test_util.h"
#include "backbone_router/ndproxy_table.hpp"
#include "common/code_utils.hpp"
#include "common/instance.hpp"

namespace ot {

static ot::Instance *sInstance;

using namespace ot::BackboneRouter;

Ip6::InterfaceIdentifier generateRandomIid(uint16_t aIndex)
{
    Ip6::InterfaceIdentifier iid;

    Random::NonCrypto::FillBuffer(iid.mFields.m8, sizeof(iid));
    iid.mFields.m16[3] = aIndex;

    return iid;
}

void TestNdProxyTable(void)
{
    Error error;

    sInstance = testInitInstance();
    VerifyOrQuit(sInstance != nullptr);

    BackboneRouter::NdProxyTable &table = sInstance->Get<BackboneRouter::NdProxyTable>();

    Ip6::InterfaceIdentifier existedAddressIid    = generateRandomIid(0);
    Ip6::InterfaceIdentifier existedMeshLocalIid  = generateRandomIid(0);
    Ip6::InterfaceIdentifier notExistAddressIid   = generateRandomIid(OPENTHREAD_CONFIG_NDPROXY_TABLE_ENTRY_NUM);
    Ip6::InterfaceIdentifier notExistMeshLocalIid = generateRandomIid(OPENTHREAD_CONFIG_NDPROXY_TABLE_ENTRY_NUM);

    // Reregister address IID when there are enough room should succeed.
    SuccessOrQuit(table.Register(existedAddressIid, existedMeshLocalIid, 0, nullptr));
    VerifyOrQuit(table.IsRegistered(existedAddressIid));
    VerifyOrQuit(!table.IsRegistered(notExistAddressIid));

    for (uint16_t i = 1; i < OPENTHREAD_CONFIG_NDPROXY_TABLE_ENTRY_NUM; i++)
    {
        Ip6::InterfaceIdentifier addressIid   = generateRandomIid(i);
        Ip6::InterfaceIdentifier meshLocalIid = generateRandomIid(i);

        // Reregister address IID when there are enough room should succeed.
        SuccessOrQuit(table.Register(addressIid, meshLocalIid, i, nullptr));

        VerifyOrQuit(table.IsRegistered(addressIid));

        // Reregister the same address IID should always succeed.
        SuccessOrQuit(table.Register(addressIid, meshLocalIid, i, nullptr));

        // Register the same address IID with a different ML-IID should fail.
        VerifyOrQuit(table.Register(addressIid, notExistMeshLocalIid, i, nullptr) == kErrorDuplicated);

        VerifyOrQuit(table.IsRegistered(addressIid));
    }

    // Now the table is full, registering another IID should fail.
    VerifyOrQuit(table.Register(notExistAddressIid, notExistMeshLocalIid, OPENTHREAD_CONFIG_NDPROXY_TABLE_ENTRY_NUM,
                                nullptr) == kErrorNoBufs);
    VerifyOrQuit(!table.IsRegistered(notExistAddressIid));
}

} // namespace ot

int main(void)
{
    ot::TestNdProxyTable();

    printf("\nAll tests passed.\n");
    return 0;
}

#else
int main(void)
{
    return 0;
}
#endif // OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE
