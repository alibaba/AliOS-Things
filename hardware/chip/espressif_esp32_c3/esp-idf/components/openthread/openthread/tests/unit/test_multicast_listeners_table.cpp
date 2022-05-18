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

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE

#include "test_platform.h"

#include <openthread/config.h>
#include <openthread/ip6.h>

#include "test_util.h"
#include "backbone_router/multicast_listeners_table.hpp"
#include "common/code_utils.hpp"
#include "common/instance.hpp"

namespace ot {

static ot::Instance *sInstance;

using namespace ot::BackboneRouter;

static const otIp6Address MA201 = {
    {{0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}}};
static const otIp6Address MA301 = {
    {{0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}}};
static const otIp6Address MA401 = {
    {{0xff, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}}};
static const otIp6Address MA501 = {
    {{0xff, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}}};

uint32_t sNow;

extern "C" uint32_t otPlatAlarmMilliGetNow(void)
{
    return sNow;
}

void testMulticastListenersTableAPIs(Instance *aInstance);

void TestMulticastListenersTable(void)
{
    sInstance = testInitInstance();
    VerifyOrQuit(sInstance != nullptr);

    MulticastListenersTable &table = sInstance->Get<MulticastListenersTable>();

    for (MulticastListenersTable::Listener &listener : table.Iterate())
    {
        VerifyOrQuit(false, "MulticastListenersTable should be empty when created");
    }

    // Removing from empty table should be OK
    table.Remove(static_cast<const Ip6::Address &>(MA401));

    sNow = 1;
    // Add valid MAs should succeed
    SuccessOrQuit(table.Add(static_cast<const Ip6::Address &>(MA401), TimerMilli::GetNow()));
    SuccessOrQuit(table.Add(static_cast<const Ip6::Address &>(MA501), TimerMilli::GetNow()));
    VerifyOrQuit(table.Count() == 2, "Table count is wrong");

    // Add invalid MAs should fail with kErrorInvalidArgs
    VerifyOrQuit(table.Add(static_cast<const Ip6::Address &>(MA201), TimerMilli::GetNow()) == kErrorInvalidArgs,
                 "failed to detect bad arg");
    VerifyOrQuit(table.Add(static_cast<const Ip6::Address &>(MA301), TimerMilli::GetNow()) == kErrorInvalidArgs,
                 "failed to detect bad arg");

    // Expire should expire outdated Listeners
    sNow = 2;
    table.Expire();
    VerifyOrQuit(table.Count() == 0, "Table count is wrong");

    // Add different addresses until the table is full
    sNow = 10;
    for (uint16_t i = 0; i < OPENTHREAD_CONFIG_MAX_MULTICAST_LISTENERS; i++)
    {
        Ip6::Address address;

        address                = static_cast<const Ip6::Address &>(MA401);
        address.mFields.m16[7] = HostSwap16(i);

        SuccessOrQuit(table.Add(address, TimerMilli::GetNow() + i));
        VerifyOrQuit(table.Count() == i + 1, "Table count is wrong");
    }

    // Now the table is full, we can't add more addresses
    VerifyOrQuit(table.Add(static_cast<const Ip6::Address &>(MA501), TimerMilli::GetNow()) == kErrorNoBufs,
                 "succeeded when table is full");

    // Expire one Listener at a time
    for (uint16_t i = 0; i < OPENTHREAD_CONFIG_MAX_MULTICAST_LISTENERS; i++)
    {
        table.Expire();
        VerifyOrQuit(table.Count() == OPENTHREAD_CONFIG_MAX_MULTICAST_LISTENERS - i - 1, "Table count is wrong");

        sNow += 1;
    }

    // Now the table should be empty
    VerifyOrQuit(table.Count() == 0, "Table count is wrong");

    // Now test the APIs
    testMulticastListenersTableAPIs(sInstance);

    // Do some fuzzy test
    for (uint16_t i = 0; i < 10000; i++)
    {
        Ip6::Address address;
        sNow += 10;

        table.Expire();

        for (MulticastListenersTable::Listener &listener : table.Iterate())
        {
            OT_ASSERT(listener.GetAddress().IsMulticastLargerThanRealmLocal());
            OT_ASSERT(listener.GetExpireTime() > TimerMilli::GetNow());
        }

        address = static_cast<const Ip6::Address &>(MA401);

        address.mFields.m16[7] = Random::NonCrypto::GetUint16InRange(1, 1000);
        IgnoreError(table.Add(address, TimerMilli::GetNow() + Random::NonCrypto::GetUint32InRange(1, 100)));

        address.mFields.m16[7] = Random::NonCrypto::GetUint16InRange(1, 1000);
        if (Random::NonCrypto::GetUint16InRange(0, 2) == 0)
        {
            table.Remove(address);
        }
    }
}

void testMulticastListenersTableAPIs(Instance *aInstance)
{
#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    otBackboneRouterMulticastListenerIterator iter = OT_BACKBONE_ROUTER_MULTICAST_LISTENER_ITERATOR_INIT;
    otBackboneRouterMulticastListenerInfo     info;
    size_t                                    table_size = 0;

    while (otBackboneRouterMulticastListenerGetNext(aInstance, &iter, &info) == kErrorNone)
    {
        VerifyOrQuit(false, "Table should be empty");
    }

    SuccessOrQuit(otBackboneRouterMulticastListenerAdd(aInstance, &MA401, 30));

    table_size = 0, iter = OT_BACKBONE_ROUTER_MULTICAST_LISTENER_ITERATOR_INIT;
    while (otBackboneRouterMulticastListenerGetNext(aInstance, &iter, &info) == kErrorNone)
    {
        table_size++;

        VerifyOrQuit(memcmp(&info.mAddress, &MA401, sizeof(otIp6Address)) == 0, "bad address");
        VerifyOrQuit(info.mTimeout == 30, "bad timeout");
    }
    VerifyOrQuit(table_size == 1, "Table size is wrong");

    otBackboneRouterMulticastListenerClear(aInstance);

    iter = OT_BACKBONE_ROUTER_MULTICAST_LISTENER_ITERATOR_INIT;
    while (otBackboneRouterMulticastListenerGetNext(aInstance, &iter, &info) == kErrorNone)
    {
        VerifyOrQuit(false, "Table should be empty");
    }
#endif
}

} // namespace ot

int main(void)
{
    ot::TestMulticastListenersTable();
    printf("\nAll tests passed.\n");
    return 0;
}

#else
int main(void)
{
    return 0;
}
#endif // OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE
