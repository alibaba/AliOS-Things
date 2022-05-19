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

/**
 * @file
 *   This file implements the OpenThread UDP API.
 */

#include "openthread-core-config.h"

#include <openthread/udp.h>

#include "common/as_core_type.hpp"
#include "common/locator_getters.hpp"

using namespace ot;

otMessage *otUdpNewMessage(otInstance *aInstance, const otMessageSettings *aSettings)
{
    return AsCoreType(aInstance).Get<Ip6::Udp>().NewMessage(0, Message::Settings::From(aSettings));
}

otError otUdpOpen(otInstance *aInstance, otUdpSocket *aSocket, otUdpReceive aCallback, void *aContext)
{
    return AsCoreType(aInstance).Get<Ip6::Udp>().Open(AsCoreType(aSocket), aCallback, aContext);
}

bool otUdpIsOpen(otInstance *aInstance, const otUdpSocket *aSocket)
{
    return AsCoreType(aInstance).Get<Ip6::Udp>().IsOpen(AsCoreType(aSocket));
}

otError otUdpClose(otInstance *aInstance, otUdpSocket *aSocket)
{
    return AsCoreType(aInstance).Get<Ip6::Udp>().Close(AsCoreType(aSocket));
}

otError otUdpBind(otInstance *aInstance, otUdpSocket *aSocket, const otSockAddr *aSockName, otNetifIdentifier aNetif)
{
    return AsCoreType(aInstance).Get<Ip6::Udp>().Bind(AsCoreType(aSocket), AsCoreType(aSockName), aNetif);
}

otError otUdpConnect(otInstance *aInstance, otUdpSocket *aSocket, const otSockAddr *aSockName)
{
    return AsCoreType(aInstance).Get<Ip6::Udp>().Connect(AsCoreType(aSocket), AsCoreType(aSockName));
}

otError otUdpSend(otInstance *aInstance, otUdpSocket *aSocket, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    return AsCoreType(aInstance).Get<Ip6::Udp>().SendTo(AsCoreType(aSocket), AsCoreType(aMessage),
                                                        AsCoreType(aMessageInfo));
}

otUdpSocket *otUdpGetSockets(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Ip6::Udp>().GetUdpSockets();
}

#if OPENTHREAD_CONFIG_UDP_FORWARD_ENABLE
void otUdpForwardSetForwarder(otInstance *aInstance, otUdpForwarder aForwarder, void *aContext)
{
    AsCoreType(aInstance).Get<Ip6::Udp>().SetUdpForwarder(aForwarder, aContext);
}

void otUdpForwardReceive(otInstance *        aInstance,
                         otMessage *         aMessage,
                         uint16_t            aPeerPort,
                         const otIp6Address *aPeerAddr,
                         uint16_t            aSockPort)
{
    Ip6::MessageInfo messageInfo;

    OT_ASSERT(aMessage != nullptr && aPeerAddr != nullptr);

    messageInfo.SetSockAddr(AsCoreType(aInstance).Get<Mle::MleRouter>().GetMeshLocal16());
    messageInfo.SetSockPort(aSockPort);
    messageInfo.SetPeerAddr(AsCoreType(aPeerAddr));
    messageInfo.SetPeerPort(aPeerPort);
    messageInfo.SetIsHostInterface(true);

    AsCoreType(aInstance).Get<Ip6::Udp>().HandlePayload(AsCoreType(aMessage), messageInfo);

    AsCoreType(aMessage).Free();
}
#endif // OPENTHREAD_CONFIG_UDP_FORWARD_ENABLE

otError otUdpAddReceiver(otInstance *aInstance, otUdpReceiver *aUdpReceiver)
{
    return AsCoreType(aInstance).Get<Ip6::Udp>().AddReceiver(AsCoreType(aUdpReceiver));
}

otError otUdpRemoveReceiver(otInstance *aInstance, otUdpReceiver *aUdpReceiver)
{
    return AsCoreType(aInstance).Get<Ip6::Udp>().RemoveReceiver(AsCoreType(aUdpReceiver));
}

otError otUdpSendDatagram(otInstance *aInstance, otMessage *aMessage, otMessageInfo *aMessageInfo)
{
    return AsCoreType(aInstance).Get<Ip6::Udp>().SendDatagram(AsCoreType(aMessage), AsCoreType(aMessageInfo),
                                                              Ip6::kProtoUdp);
}

bool otUdpIsPortInUse(otInstance *aInstance, uint16_t port)
{
    return AsCoreType(aInstance).Get<Ip6::Udp>().IsPortInUse(port);
}
