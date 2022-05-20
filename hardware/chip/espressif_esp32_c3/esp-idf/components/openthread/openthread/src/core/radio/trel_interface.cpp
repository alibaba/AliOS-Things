/*
 *    Copyright (c) 2019, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 *    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements Thread Radio Encapsulation Link (TREL) interface.
 */

#include "trel_interface.hpp"

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE

#include <openthread/platform/trel-udp6.h>

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"

namespace ot {
namespace Trel {

Interface::Interface(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mInitialized(false)
{
}

void Interface::Init(void)
{
    Ip6::Address ip6Address;

    OT_ASSERT(!mInitialized);

    ip6Address.SetToLinkLocalAddress(Get<Mac::Mac>().GetExtAddress());
    otPlatTrelUdp6Init(&GetInstance(), &ip6Address, kUdpPort);

    CreateMulticastIp6Address(ip6Address);
    otPlatTrelUdp6SubscribeMulticastAddress(&GetInstance(), &ip6Address);

    mInitialized = true;
}

void Interface::HandleExtAddressChange(void)
{
    Ip6::Address ip6Address;

    VerifyOrExit(mInitialized);

    ip6Address.SetToLinkLocalAddress(Get<Mac::Mac>().GetExtAddress());
    otPlatTrelUdp6UpdateAddress(&GetInstance(), &ip6Address);

exit:
    return;
}

Error Interface::Send(const Packet &aPacket)
{
    Ip6::Address destIp6Address;

    switch (aPacket.GetHeader().GetType())
    {
    case Header::kTypeBroadcast:
        CreateMulticastIp6Address(destIp6Address);
        break;

    case Header::kTypeUnicast:
    case Header::kTypeAck:
        destIp6Address.SetToLinkLocalAddress(aPacket.GetHeader().GetDestination());
        break;
    }

    return otPlatTrelUdp6SendTo(&GetInstance(), aPacket.GetBuffer(), aPacket.GetLength(), &destIp6Address);
}

void Interface::HandleReceived(uint8_t *aBuffer, uint16_t aLength)
{
    mRxPacket.Init(aBuffer, aLength);
    Get<Link>().ProcessReceivedPacket(mRxPacket);
}

void Interface::CreateMulticastIp6Address(Ip6::Address &aIp6Address)
{
    // Use ff02::1 (Link-local All Nodes multicast address).
    aIp6Address.SetToLinkLocalAllNodesMulticast();
}

} // namespace Trel
} // namespace ot

extern "C" void otPlatTrelUdp6HandleReceived(otInstance *aInstance, uint8_t *aBuffer, uint16_t aLength)
{
    ot::Instance &instance = *static_cast<ot::Instance *>(aInstance);

    VerifyOrExit(instance.IsInitialized());
    instance.Get<ot::Trel::Interface>().HandleReceived(aBuffer, aLength);

exit:
    return;
}

extern "C" OT_TOOL_WEAK otError otPlatTrelUdp6SetTestMode(otInstance *aInstance, bool aEnable)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aEnable);

    return ot::kErrorNotImplemented;
}

#endif // #if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
