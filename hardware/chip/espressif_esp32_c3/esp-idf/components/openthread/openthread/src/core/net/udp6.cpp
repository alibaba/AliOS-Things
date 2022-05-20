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
 *   This file implements UDP/IPv6 sockets.
 */

#include "udp6.hpp"

#include <stdio.h>

#include <openthread/platform/udp.h>

#include "common/code_utils.hpp"
#include "common/encoding.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "net/checksum.hpp"
#include "net/ip6.hpp"

namespace ot {
namespace Ip6 {

bool Udp::SocketHandle::Matches(const MessageInfo &aMessageInfo) const
{
    bool matches = false;

    VerifyOrExit(GetSockName().mPort == aMessageInfo.GetSockPort());

    VerifyOrExit(aMessageInfo.GetSockAddr().IsMulticast() || GetSockName().GetAddress().IsUnspecified() ||
                 GetSockName().GetAddress() == aMessageInfo.GetSockAddr());

    // Verify source if connected socket
    if (GetPeerName().mPort != 0)
    {
        VerifyOrExit(GetPeerName().mPort == aMessageInfo.GetPeerPort());

        VerifyOrExit(GetPeerName().GetAddress().IsUnspecified() ||
                     GetPeerName().GetAddress() == aMessageInfo.GetPeerAddr());
    }

    matches = true;

exit:
    return matches;
}

Udp::Socket::Socket(Instance &aInstance)
    : InstanceLocator(aInstance)
{
    Clear();
}

Message *Udp::Socket::NewMessage(uint16_t aReserved, const Message::Settings &aSettings)
{
    return Get<Udp>().NewMessage(aReserved, aSettings);
}

Error Udp::Socket::Open(otUdpReceive aHandler, void *aContext)
{
    return Get<Udp>().Open(*this, aHandler, aContext);
}

bool Udp::Socket::IsOpen(void) const
{
    return Get<Udp>().IsOpen(*this);
}

Error Udp::Socket::Bind(const SockAddr &aSockAddr, otNetifIdentifier aNetifIdentifier)
{
    return Get<Udp>().Bind(*this, aSockAddr, aNetifIdentifier);
}

Error Udp::Socket::Bind(uint16_t aPort, otNetifIdentifier aNetifIdentifier)
{
    return Bind(SockAddr(aPort), aNetifIdentifier);
}

Error Udp::Socket::Connect(const SockAddr &aSockAddr)
{
    return Get<Udp>().Connect(*this, aSockAddr);
}

Error Udp::Socket::Connect(uint16_t aPort)
{
    return Connect(SockAddr(aPort));
}

Error Udp::Socket::Close(void)
{
    return Get<Udp>().Close(*this);
}

Error Udp::Socket::SendTo(Message &aMessage, const MessageInfo &aMessageInfo)
{
    return Get<Udp>().SendTo(*this, aMessage, aMessageInfo);
}

#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
Error Udp::Socket::JoinNetifMulticastGroup(otNetifIdentifier aNetifIdentifier, const Address &aAddress)
{
    OT_UNUSED_VARIABLE(aNetifIdentifier);
    OT_UNUSED_VARIABLE(aAddress);

    Error error = kErrorNotImplemented;

    VerifyOrExit(aAddress.IsMulticast(), error = kErrorInvalidArgs);

#if OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE
    error = otPlatUdpJoinMulticastGroup(this, aNetifIdentifier, &aAddress);
#endif

exit:
    return error;
}

Error Udp::Socket::LeaveNetifMulticastGroup(otNetifIdentifier aNetifIdentifier, const Address &aAddress)
{
    OT_UNUSED_VARIABLE(aNetifIdentifier);
    OT_UNUSED_VARIABLE(aAddress);

    Error error = kErrorNotImplemented;

    VerifyOrExit(aAddress.IsMulticast(), error = kErrorInvalidArgs);

#if OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE
    error = otPlatUdpLeaveMulticastGroup(this, aNetifIdentifier, &aAddress);
#endif

exit:
    return error;
}
#endif

Udp::Udp(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mEphemeralPort(kDynamicPortMin)
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    , mPrevBackboneSockets(nullptr)
#endif
#if OPENTHREAD_CONFIG_UDP_FORWARD_ENABLE
    , mUdpForwarderContext(nullptr)
    , mUdpForwarder(nullptr)
#endif
{
}

Error Udp::AddReceiver(Receiver &aReceiver)
{
    return mReceivers.Add(aReceiver);
}

Error Udp::RemoveReceiver(Receiver &aReceiver)
{
    Error error;

    SuccessOrExit(error = mReceivers.Remove(aReceiver));
    aReceiver.SetNext(nullptr);

exit:
    return error;
}

Error Udp::Open(SocketHandle &aSocket, otUdpReceive aHandler, void *aContext)
{
    Error error = kErrorNone;

    OT_ASSERT(!IsOpen(aSocket));

    aSocket.GetSockName().Clear();
    aSocket.GetPeerName().Clear();
    aSocket.mHandler = aHandler;
    aSocket.mContext = aContext;

#if OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE
    error = otPlatUdpSocket(&aSocket);
#endif
    SuccessOrExit(error);

    AddSocket(aSocket);

exit:
    return error;
}

Error Udp::Bind(SocketHandle &aSocket, const SockAddr &aSockAddr, otNetifIdentifier aNetifIdentifier)
{
    OT_UNUSED_VARIABLE(aNetifIdentifier);

    Error error = kErrorNone;

#if OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE
    SuccessOrExit(error = otPlatUdpBindToNetif(&aSocket, aNetifIdentifier));
#endif

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    if (aNetifIdentifier == OT_NETIF_BACKBONE)
    {
        SetBackboneSocket(aSocket);
    }
#endif

    VerifyOrExit(aSockAddr.GetAddress().IsUnspecified() || Get<ThreadNetif>().HasUnicastAddress(aSockAddr.GetAddress()),
                 error = kErrorInvalidArgs);

    aSocket.mSockName = aSockAddr;

    if (!aSocket.IsBound())
    {
        do
        {
            aSocket.mSockName.mPort = GetEphemeralPort();
#if OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE
            error = otPlatUdpBind(&aSocket);
#endif
        } while (error != kErrorNone);
    }
#if OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE
    else if (ShouldUsePlatformUdp(aSocket))
    {
        error = otPlatUdpBind(&aSocket);
    }
#endif

exit:
    return error;
}

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
void Udp::SetBackboneSocket(SocketHandle &aSocket)
{
    RemoveSocket(aSocket);

    if (mPrevBackboneSockets != nullptr)
    {
        mSockets.PushAfter(aSocket, *mPrevBackboneSockets);
    }
    else
    {
        mSockets.Push(aSocket);
    }
}

const Udp::SocketHandle *Udp::GetBackboneSockets(void) const
{
    return mPrevBackboneSockets != nullptr ? mPrevBackboneSockets->GetNext() : mSockets.GetHead();
}

bool Udp::IsBackboneSocket(const SocketHandle &aSocket) const
{
    bool retval = false;

    for (const SocketHandle *sock = GetBackboneSockets(); sock != nullptr; sock = sock->GetNext())
    {
        if (sock == &aSocket)
        {
            ExitNow(retval = true);
        }
    }

exit:
    return retval;
}
#endif

Error Udp::Connect(SocketHandle &aSocket, const SockAddr &aSockAddr)
{
    Error error = kErrorNone;

    aSocket.mPeerName = aSockAddr;

    if (!aSocket.IsBound())
    {
        SuccessOrExit(error = Bind(aSocket, aSocket.GetSockName(), OT_NETIF_THREAD));
    }

#if OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE
    if (ShouldUsePlatformUdp(aSocket))
    {
        error = otPlatUdpConnect(&aSocket);
    }
#endif

exit:
    return error;
}

Error Udp::Close(SocketHandle &aSocket)
{
    Error error = kErrorNone;

    VerifyOrExit(IsOpen(aSocket));

#if OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE
    error = otPlatUdpClose(&aSocket);
#endif
    SuccessOrExit(error);

    RemoveSocket(aSocket);
    aSocket.GetSockName().Clear();
    aSocket.GetPeerName().Clear();

exit:
    return error;
}

Error Udp::SendTo(SocketHandle &aSocket, Message &aMessage, const MessageInfo &aMessageInfo)
{
    Error       error = kErrorNone;
    MessageInfo messageInfoLocal;

    VerifyOrExit((aMessageInfo.GetSockPort() == 0) || (aSocket.GetSockName().mPort == aMessageInfo.GetSockPort()),
                 error = kErrorInvalidArgs);

    messageInfoLocal = aMessageInfo;

    if (messageInfoLocal.GetPeerAddr().IsUnspecified())
    {
        VerifyOrExit(!aSocket.GetPeerName().GetAddress().IsUnspecified(), error = kErrorInvalidArgs);

        messageInfoLocal.SetPeerAddr(aSocket.GetPeerName().GetAddress());
    }

    if (messageInfoLocal.mPeerPort == 0)
    {
        VerifyOrExit(aSocket.GetPeerName().mPort != 0, error = kErrorInvalidArgs);
        messageInfoLocal.mPeerPort = aSocket.GetPeerName().mPort;
    }

    if (messageInfoLocal.GetSockAddr().IsUnspecified())
    {
        messageInfoLocal.SetSockAddr(aSocket.GetSockName().GetAddress());
    }

    if (!aSocket.IsBound())
    {
        SuccessOrExit(error = Bind(aSocket, aSocket.GetSockName(), OT_NETIF_THREAD));
    }

    messageInfoLocal.SetSockPort(aSocket.GetSockName().mPort);

#if OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE
    if (ShouldUsePlatformUdp(aSocket))
    {
        SuccessOrExit(error = otPlatUdpSend(&aSocket, &aMessage, &messageInfoLocal));
    }
    else
#endif
    {
        SuccessOrExit(error = SendDatagram(aMessage, messageInfoLocal, kProtoUdp));
    }

exit:
    return error;
}

bool Udp::IsPortReserved(uint16_t aPort)
{
    return aPort == Tmf::kUdpPort || (kSrpServerPortMin <= aPort && aPort <= kSrpServerPortMax);
}

void Udp::AddSocket(SocketHandle &aSocket)
{
    SuccessOrExit(mSockets.Add(aSocket));

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    if (mPrevBackboneSockets == nullptr)
    {
        mPrevBackboneSockets = &aSocket;
    }
#endif
exit:
    return;
}

void Udp::RemoveSocket(SocketHandle &aSocket)
{
    SocketHandle *prev;

    SuccessOrExit(mSockets.Find(aSocket, prev));

    mSockets.PopAfter(prev);
    aSocket.SetNext(nullptr);

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    if (&aSocket == mPrevBackboneSockets)
    {
        mPrevBackboneSockets = prev;
    }
#endif

exit:
    return;
}

uint16_t Udp::GetEphemeralPort(void)
{
    do
    {
        if (mEphemeralPort < kDynamicPortMax)
        {
            mEphemeralPort++;
        }
        else
        {
            mEphemeralPort = kDynamicPortMin;
        }
    } while (IsPortReserved(mEphemeralPort));

    return mEphemeralPort;
}

Message *Udp::NewMessage(uint16_t aReserved, const Message::Settings &aSettings)
{
    return Get<Ip6>().NewMessage(sizeof(Header) + aReserved, aSettings);
}

Error Udp::SendDatagram(Message &aMessage, MessageInfo &aMessageInfo, uint8_t aIpProto)
{
    Error error = kErrorNone;

#if OPENTHREAD_CONFIG_UDP_FORWARD_ENABLE
    if (aMessageInfo.IsHostInterface())
    {
        VerifyOrExit(mUdpForwarder != nullptr, error = kErrorNoRoute);
        mUdpForwarder(&aMessage, aMessageInfo.mPeerPort, &aMessageInfo.GetPeerAddr(), aMessageInfo.mSockPort,
                      mUdpForwarderContext);
        // message is consumed by the callback
    }
    else
#endif
    {
        Header udpHeader;

        udpHeader.SetSourcePort(aMessageInfo.mSockPort);
        udpHeader.SetDestinationPort(aMessageInfo.mPeerPort);
        udpHeader.SetLength(sizeof(udpHeader) + aMessage.GetLength());
        udpHeader.SetChecksum(0);

        SuccessOrExit(error = aMessage.Prepend(udpHeader));
        aMessage.SetOffset(0);

        error = Get<Ip6>().SendDatagram(aMessage, aMessageInfo, aIpProto);
    }

exit:
    return error;
}

Error Udp::HandleMessage(Message &aMessage, MessageInfo &aMessageInfo)
{
    Error  error = kErrorNone;
    Header udpHeader;

    SuccessOrExit(error = aMessage.Read(aMessage.GetOffset(), udpHeader));

#ifndef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
    SuccessOrExit(error = Checksum::VerifyMessageChecksum(aMessage, aMessageInfo, kProtoUdp));
#endif

    aMessage.MoveOffset(sizeof(udpHeader));
    aMessageInfo.mPeerPort = udpHeader.GetSourcePort();
    aMessageInfo.mSockPort = udpHeader.GetDestinationPort();

#if OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE
    VerifyOrExit(!ShouldUsePlatformUdp(aMessageInfo.mSockPort) || IsPortInUse(aMessageInfo.mSockPort));
#endif

    for (Receiver &receiver : mReceivers)
    {
        VerifyOrExit(!receiver.HandleMessage(aMessage, aMessageInfo));
    }

    HandlePayload(aMessage, aMessageInfo);

exit:
    return error;
}

void Udp::HandlePayload(Message &aMessage, MessageInfo &aMessageInfo)
{
    SocketHandle *socket;
    SocketHandle *prev;

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    {
        const SocketHandle *socketsBegin, *socketsEnd;

        if (!aMessageInfo.IsHostInterface())
        {
            socketsBegin = mSockets.GetHead();
            socketsEnd   = GetBackboneSockets();
        }
        else
        {
            socketsBegin = GetBackboneSockets();
            socketsEnd   = nullptr;
        }

        socket = mSockets.FindMatching(socketsBegin, socketsEnd, aMessageInfo, prev);
    }
#else
    socket = mSockets.FindMatching(aMessageInfo, prev);
#endif

    VerifyOrExit(socket != nullptr);

    aMessage.RemoveHeader(aMessage.GetOffset());
    OT_ASSERT(aMessage.GetOffset() == 0);
    socket->HandleUdpReceive(aMessage, aMessageInfo);

exit:
    return;
}

bool Udp::IsPortInUse(uint16_t aPort) const
{
    bool found = false;

    for (const SocketHandle &socket : mSockets)
    {
        if (socket.GetSockName().GetPort() == aPort)
        {
            found = true;
            break;
        }
    }

    return found;
}

bool Udp::ShouldUsePlatformUdp(uint16_t aPort) const
{
    return (aPort != Mle::kUdpPort && aPort != Tmf::kUdpPort
#if OPENTHREAD_CONFIG_DNSSD_SERVER_ENABLE && !OPENTHREAD_CONFIG_DNSSD_SERVER_BIND_UNSPECIFIED_NETIF
            && aPort != Dns::ServiceDiscovery::Server::kPort
#endif
#if OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
            && aPort != Get<MeshCoP::BorderAgent>().GetUdpProxyPort()
#endif
#if OPENTHREAD_FTD
            && aPort != Get<MeshCoP::JoinerRouter>().GetJoinerUdpPort()
#endif
    );
}

#if OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE
bool Udp::ShouldUsePlatformUdp(const Udp::SocketHandle &aSocket) const
{
    return (ShouldUsePlatformUdp(aSocket.mSockName.mPort)
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
            || IsBackboneSocket(aSocket)
#endif
    );
}
#endif // OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE

} // namespace Ip6
} // namespace ot
