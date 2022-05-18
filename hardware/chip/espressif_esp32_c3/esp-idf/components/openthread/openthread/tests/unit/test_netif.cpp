/*
 *  Copyright (c) 2019, The OpenThread Authors.
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

#include <stdarg.h>

#include "test_platform.h"

#include <openthread/config.h>

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "net/netif.hpp"

#include "test_util.h"

namespace ot {

class TestNetif : public Ip6::Netif
{
public:
    explicit TestNetif(Instance &aInstance)
        : Ip6::Netif(aInstance)
    {
    }

    // Provide `protected` methods in `Netif` as `public` from `TestNetif`
    // so that we can verify their behavior in this test
    void SubscribeAllNodesMulticast(void) { Ip6::Netif::SubscribeAllNodesMulticast(); }
    void UnsubscribeAllNodesMulticast(void) { Ip6::Netif::UnsubscribeAllNodesMulticast(); }
};

// This function verifies the multicast addresses on Netif matches the list of given addresses.
void VerifyMulticastAddressList(const Ip6::Netif &aNetif, Ip6::Address aAddresses[], uint8_t aLength)
{
    uint8_t count = 0;

    for (uint8_t i = 0; i < aLength; i++)
    {
        VerifyOrQuit(aNetif.IsMulticastSubscribed(aAddresses[i]));
    }

    for (const Ip6::Netif::MulticastAddress &addr : aNetif.GetMulticastAddresses())
    {
        bool didFind = false;

        for (uint8_t i = 0; i < aLength; i++)
        {
            if (addr.GetAddress() == aAddresses[i])
            {
                didFind = true;
                break;
            }
        }

        VerifyOrQuit(didFind, "Netif multicast address is missing from expected address list");
        count++;
    }

    VerifyOrQuit(count == aLength, "Expected address is missing from Netif address list");
}

void TestNetifMulticastAddresses(void)
{
    const uint8_t kMaxAddresses = 8;

    Instance *   instance = testInitInstance();
    TestNetif    netif(*instance);
    Ip6::Address addresses[kMaxAddresses];

    Ip6::Address                 address;
    Ip6::Netif::MulticastAddress netifAddress;

    const char *kLinkLocalAllNodes    = "ff02::01";
    const char *kRealmLocalAllNodes   = "ff03::01";
    const char *kRealmLocalAllMpl     = "ff03::fc";
    const char *kLinkLocalAllRouters  = "ff02::02";
    const char *kRealmLocalAllRouters = "ff03::02";
    const char *kTestAddress1         = "ff02::114";
    const char *kTestAddress2         = "ff03::114";
    const char *kTestAddress3         = "ff04::114";

    IgnoreError(addresses[0].FromString(kLinkLocalAllRouters));
    IgnoreError(addresses[1].FromString(kRealmLocalAllRouters));
    IgnoreError(addresses[2].FromString(kLinkLocalAllNodes));
    IgnoreError(addresses[3].FromString(kRealmLocalAllNodes));
    IgnoreError(addresses[4].FromString(kRealmLocalAllMpl));
    IgnoreError(addresses[5].FromString(kTestAddress1));
    IgnoreError(addresses[6].FromString(kTestAddress2));
    IgnoreError(addresses[7].FromString(kTestAddress3));

    VerifyMulticastAddressList(netif, addresses, 0);

    netif.SubscribeAllNodesMulticast();
    VerifyMulticastAddressList(netif, &addresses[2], 3);

    netif.SubscribeAllNodesMulticast();
    VerifyMulticastAddressList(netif, &addresses[2], 3);

    netif.SubscribeAllRoutersMulticast();
    VerifyMulticastAddressList(netif, &addresses[0], 5);

    netif.SubscribeAllRoutersMulticast();
    VerifyMulticastAddressList(netif, &addresses[0], 5);

    netif.UnsubscribeAllRoutersMulticast();
    VerifyMulticastAddressList(netif, &addresses[2], 3);

    netif.UnsubscribeAllRoutersMulticast();
    VerifyMulticastAddressList(netif, &addresses[2], 3);

    IgnoreError(netifAddress.GetAddress().FromString(kTestAddress1));
    netif.SubscribeMulticast(netifAddress);
    VerifyMulticastAddressList(netif, &addresses[2], 4);

    netif.SubscribeMulticast(netifAddress);
    VerifyMulticastAddressList(netif, &addresses[2], 4);

    netif.UnsubscribeAllNodesMulticast();
    VerifyMulticastAddressList(netif, &addresses[5], 1);

    netif.UnsubscribeAllNodesMulticast();
    VerifyMulticastAddressList(netif, &addresses[5], 1);

    IgnoreError(address.FromString(kTestAddress2));
    SuccessOrQuit(netif.SubscribeExternalMulticast(address));
    VerifyMulticastAddressList(netif, &addresses[5], 2);

    netif.SubscribeAllNodesMulticast();
    VerifyMulticastAddressList(netif, &addresses[2], 5);

    netif.SubscribeAllNodesMulticast();
    VerifyMulticastAddressList(netif, &addresses[2], 5);

    netif.SubscribeAllRoutersMulticast();
    VerifyMulticastAddressList(netif, &addresses[0], 7);

    netif.SubscribeAllRoutersMulticast();
    VerifyMulticastAddressList(netif, &addresses[0], 7);

    IgnoreError(address.FromString(kTestAddress3));
    SuccessOrQuit(netif.SubscribeExternalMulticast(address));
    VerifyMulticastAddressList(netif, &addresses[0], 8);

    IgnoreError(address.FromString(kTestAddress1)); // same as netifAddress (internal)
    VerifyOrQuit(netif.UnsubscribeExternalMulticast(address) == kErrorInvalidArgs,
                 "UnsubscribeExternalMulticast() did not fail when address was not external");

    IgnoreError(address.FromString(kRealmLocalAllMpl));
    VerifyOrQuit(netif.UnsubscribeExternalMulticast(address) == kErrorInvalidArgs,
                 "UnsubscribeExternalMulticast() did not fail when address was fixed address");

    netif.UnsubscribeAllRoutersMulticast();
    VerifyMulticastAddressList(netif, &addresses[2], 6);

    netif.UnsubscribeAllRoutersMulticast();
    VerifyMulticastAddressList(netif, &addresses[2], 6);

    netif.UnsubscribeAllExternalMulticastAddresses();
    VerifyMulticastAddressList(netif, &addresses[2], 4);

    netif.UnsubscribeMulticast(netifAddress);
    VerifyMulticastAddressList(netif, &addresses[2], 3);

    netif.UnsubscribeMulticast(netifAddress);
    VerifyMulticastAddressList(netif, &addresses[2], 3);

    netif.UnsubscribeAllNodesMulticast();
    VerifyMulticastAddressList(netif, nullptr, 0);

    // The first five elements in `addresses[]` are the default/fixed addresses:
    // kLinkLocalAllRouters, kRealmLocalAllRouters, kLinkLocalAllNodes,
    // kRealmLocalAllNodes, and kRealmLocalAllMpl. Verify that we cannot add
    // any of them as an external multicast address.

    for (uint8_t i = 0; i < 5; i++)
    {
        VerifyOrQuit(netif.SubscribeExternalMulticast(addresses[i]) == kErrorInvalidArgs,
                     "SubscribeExternalMulticast() did not fail when address was a default/fixed address");
    }
}

} // namespace ot

int main(void)
{
    ot::TestNetifMulticastAddresses();
    printf("All tests passed\n");
    return 0;
}
