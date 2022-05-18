/*
 *  Copyright (c) 2018, The OpenThread Authors.
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
 * @brief
 *   This file includes the platform UDP driver.
 */

#ifdef __APPLE__
#define __APPLE_USE_RFC_3542
#endif

#include "openthread-posix-config.h"
#include "platform-posix.h"

#include <arpa/inet.h>
#include <assert.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#include <openthread/udp.h>
#include <openthread/platform/udp.h>

#include "common/code_utils.hpp"

#if OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE

#include "posix/platform/ip6_utils.hpp"
#include "posix/platform/mainloop.hpp"
#include "posix/platform/udp.hpp"

using namespace ot::Posix::Ip6Utils;

namespace {

constexpr size_t kMaxUdpSize = 1280;

void *FdToHandle(int aFd)
{
    return reinterpret_cast<void *>(aFd);
}

int FdFromHandle(void *aHandle)
{
    return static_cast<int>(reinterpret_cast<long>(aHandle));
}

bool IsLinkLocal(const struct in6_addr &aAddress)
{
    return aAddress.s6_addr[0] == 0xfe && aAddress.s6_addr[1] == 0x80;
}

bool IsMulticast(const otIp6Address &aAddress)
{
    return aAddress.mFields.m8[0] == 0xff;
}

otError transmitPacket(int aFd, uint8_t *aPayload, uint16_t aLength, const otMessageInfo &aMessageInfo)
{
#ifdef __APPLE__
    // use fixed value for CMSG_SPACE is not a constant expression on macOS
    constexpr size_t kBufferSize = 128;
#else
    constexpr size_t kBufferSize = CMSG_SPACE(sizeof(struct in6_pktinfo)) + CMSG_SPACE(sizeof(int));
#endif
    struct sockaddr_in6 peerAddr;
    uint8_t             control[kBufferSize];
    size_t              controlLength = 0;
    struct iovec        iov;
    struct msghdr       msg;
    struct cmsghdr *    cmsg;
    ssize_t             rval;
    otError             error = OT_ERROR_NONE;

    memset(&peerAddr, 0, sizeof(peerAddr));
    peerAddr.sin6_port   = htons(aMessageInfo.mPeerPort);
    peerAddr.sin6_family = AF_INET6;
    memcpy(&peerAddr.sin6_addr, &aMessageInfo.mPeerAddr, sizeof(peerAddr.sin6_addr));

    if (IsLinkLocal(peerAddr.sin6_addr) && !aMessageInfo.mIsHostInterface)
    {
        // sin6_scope_id only works for link local destinations
        peerAddr.sin6_scope_id = gNetifIndex;
    }

    memset(control, 0, sizeof(control));

    iov.iov_base = aPayload;
    iov.iov_len  = aLength;

    msg.msg_name       = &peerAddr;
    msg.msg_namelen    = sizeof(peerAddr);
    msg.msg_control    = control;
    msg.msg_controllen = static_cast<decltype(msg.msg_controllen)>(sizeof(control));
    msg.msg_iov        = &iov;
    msg.msg_iovlen     = 1;
    msg.msg_flags      = 0;

    {
        int hopLimit = (aMessageInfo.mHopLimit ? aMessageInfo.mHopLimit : OPENTHREAD_CONFIG_IP6_HOP_LIMIT_DEFAULT);

        cmsg             = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = IPPROTO_IPV6;
        cmsg->cmsg_type  = IPV6_HOPLIMIT;
        cmsg->cmsg_len   = CMSG_LEN(sizeof(int));

        memcpy(CMSG_DATA(cmsg), &hopLimit, sizeof(int));

        controlLength += CMSG_SPACE(sizeof(int));
    }

    if (!IsMulticast(aMessageInfo.mSockAddr) &&
        memcmp(&aMessageInfo.mSockAddr, &in6addr_any, sizeof(aMessageInfo.mSockAddr)))
    {
        struct in6_pktinfo pktinfo;

        cmsg             = CMSG_NXTHDR(&msg, cmsg);
        cmsg->cmsg_level = IPPROTO_IPV6;
        cmsg->cmsg_type  = IPV6_PKTINFO;
        cmsg->cmsg_len   = CMSG_LEN(sizeof(pktinfo));

        pktinfo.ipi6_ifindex = aMessageInfo.mIsHostInterface ? 0 : gNetifIndex;

        memcpy(&pktinfo.ipi6_addr, &aMessageInfo.mSockAddr, sizeof(pktinfo.ipi6_addr));
        memcpy(CMSG_DATA(cmsg), &pktinfo, sizeof(pktinfo));

        controlLength += CMSG_SPACE(sizeof(pktinfo));
    }

#ifdef __APPLE__
    msg.msg_controllen = static_cast<socklen_t>(controlLength);
#else
    msg.msg_controllen           = controlLength;
#endif

    rval = sendmsg(aFd, &msg, 0);
    VerifyOrExit(rval > 0, perror("sendmsg"));

exit:
    // EINVAL happens when we shift from child to router and the
    // interface address changes. Ask callers to try again later.
    if (rval == -1)
    {
        error = (errno == EINVAL) ? OT_ERROR_INVALID_STATE : OT_ERROR_FAILED;
    }

    return error;
}

otError receivePacket(int aFd, uint8_t *aPayload, uint16_t &aLength, otMessageInfo &aMessageInfo)
{
    struct sockaddr_in6 peerAddr;
    uint8_t             control[kMaxUdpSize];
    struct iovec        iov;
    struct msghdr       msg;
    ssize_t             rval;

    iov.iov_base = aPayload;
    iov.iov_len  = aLength;

    msg.msg_name       = &peerAddr;
    msg.msg_namelen    = sizeof(peerAddr);
    msg.msg_control    = control;
    msg.msg_controllen = sizeof(control);
    msg.msg_iov        = &iov;
    msg.msg_iovlen     = 1;
    msg.msg_flags      = 0;

    rval = recvmsg(aFd, &msg, 0);
    VerifyOrExit(rval > 0, perror("recvmsg"));
    aLength = static_cast<uint16_t>(rval);

    for (struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg != nullptr; cmsg = CMSG_NXTHDR(&msg, cmsg))
    {
        if (cmsg->cmsg_level == IPPROTO_IPV6)
        {
            if (cmsg->cmsg_type == IPV6_HOPLIMIT)
            {
                int hoplimit;

                memcpy(&hoplimit, CMSG_DATA(cmsg), sizeof(hoplimit));
                aMessageInfo.mHopLimit = static_cast<uint8_t>(hoplimit);
            }
            else if (cmsg->cmsg_type == IPV6_PKTINFO)
            {
                struct in6_pktinfo pktinfo;

                memcpy(&pktinfo, CMSG_DATA(cmsg), sizeof(pktinfo));

                aMessageInfo.mIsHostInterface = (pktinfo.ipi6_ifindex != gNetifIndex);
                memcpy(&aMessageInfo.mSockAddr, &pktinfo.ipi6_addr, sizeof(aMessageInfo.mSockAddr));
            }
        }
    }

    aMessageInfo.mPeerPort = ntohs(peerAddr.sin6_port);
    memcpy(&aMessageInfo.mPeerAddr, &peerAddr.sin6_addr, sizeof(aMessageInfo.mPeerAddr));

exit:
    return rval > 0 ? OT_ERROR_NONE : OT_ERROR_FAILED;
}

} // namespace

otError otPlatUdpSocket(otUdpSocket *aUdpSocket)
{
    otError error = OT_ERROR_NONE;
    int     fd;

    assert(aUdpSocket->mHandle == nullptr);

    fd = SocketWithCloseExec(AF_INET6, SOCK_DGRAM, IPPROTO_UDP, kSocketNonBlock);
    VerifyOrExit(fd >= 0, error = OT_ERROR_FAILED);

    aUdpSocket->mHandle = FdToHandle(fd);

exit:
    return error;
}

otError otPlatUdpClose(otUdpSocket *aUdpSocket)
{
    otError error = OT_ERROR_NONE;
    int     fd;

    // Only call `close()` on platform UDP sockets.
    // Platform UDP sockets always have valid `mHandle` upon creation.
    VerifyOrExit(aUdpSocket->mHandle != nullptr);

    fd = FdFromHandle(aUdpSocket->mHandle);
    VerifyOrExit(0 == close(fd), error = OT_ERROR_FAILED);

    aUdpSocket->mHandle = nullptr;

exit:
    return error;
}

otError otPlatUdpBind(otUdpSocket *aUdpSocket)
{
    otError error = OT_ERROR_NONE;
    int     fd;

    assert(gNetifIndex != 0);
    assert(aUdpSocket->mHandle != nullptr);
    VerifyOrExit(aUdpSocket->mSockName.mPort != 0, error = OT_ERROR_INVALID_ARGS);
    fd = FdFromHandle(aUdpSocket->mHandle);

    {
        struct sockaddr_in6 sin6;

        memset(&sin6, 0, sizeof(struct sockaddr_in6));
        sin6.sin6_port   = htons(aUdpSocket->mSockName.mPort);
        sin6.sin6_family = AF_INET6;
        memcpy(&sin6.sin6_addr, &aUdpSocket->mSockName.mAddress, sizeof(sin6.sin6_addr));
        VerifyOrExit(0 == bind(fd, reinterpret_cast<struct sockaddr *>(&sin6), sizeof(sin6)), error = OT_ERROR_FAILED);
    }

    {
        int on = 1;
        VerifyOrExit(0 == setsockopt(fd, IPPROTO_IPV6, IPV6_RECVHOPLIMIT, &on, sizeof(on)), error = OT_ERROR_FAILED);
        VerifyOrExit(0 == setsockopt(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, &on, sizeof(on)), error = OT_ERROR_FAILED);
    }

exit:
    if (error == OT_ERROR_FAILED)
    {
        otLogCritPlat("Failed to bind UDP socket: %s", strerror(errno));
    }

    return error;
}

otError otPlatUdpBindToNetif(otUdpSocket *aUdpSocket, otNetifIdentifier aNetifIdentifier)
{
    otError error = OT_ERROR_NONE;
    int     fd    = FdFromHandle(aUdpSocket->mHandle);
    int     one   = 1;
    int     zero  = 0;

    switch (aNetifIdentifier)
    {
    case OT_NETIF_UNSPECIFIED:
    {
#ifdef __linux__
        VerifyOrExit(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, nullptr, 0) == 0, error = OT_ERROR_FAILED);
#else  // __NetBSD__ || __FreeBSD__ || __APPLE__
        unsigned int netifIndex = 0;
        VerifyOrExit(setsockopt(fd, IPPROTO_IPV6, IPV6_BOUND_IF, &netifIndex, sizeof(netifIndex)) == 0,
                     error = OT_ERROR_FAILED);
#endif // __linux__
        break;
    }
    case OT_NETIF_THREAD:
    {
#ifdef __linux__
        VerifyOrExit(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, &gNetifName, strlen(gNetifName)) == 0,
                     error = OT_ERROR_FAILED);
#else  // __NetBSD__ || __FreeBSD__ || __APPLE__
        VerifyOrExit(setsockopt(fd, IPPROTO_IPV6, IPV6_BOUND_IF, &gNetifIndex, sizeof(gNetifIndex)) == 0,
                     error = OT_ERROR_FAILED);
#endif // __linux__
        break;
    }
    case OT_NETIF_BACKBONE:
    {
#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
#if __linux__
        VerifyOrExit(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, gBackboneNetifName, strlen(gBackboneNetifName)) == 0,
                     error = OT_ERROR_FAILED);
#else  // __NetBSD__ || __FreeBSD__ || __APPLE__
        VerifyOrExit(setsockopt(fd, IPPROTO_IPV6, IPV6_BOUND_IF, &gBackboneNetifIndex, sizeof(gBackboneNetifIndex)) ==
                         0,
                     error = OT_ERROR_FAILED);
#endif // __linux__
#else
        ExitNow(error = OT_ERROR_NOT_IMPLEMENTED);
#endif
        VerifyOrExit(setsockopt(fd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (char *)&one, sizeof(one)) == 0,
                     error = OT_ERROR_FAILED);

        break;
    }
    }

    VerifyOrExit(setsockopt(fd, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &zero, sizeof(zero)) == 0, error = OT_ERROR_FAILED);

exit:
    return error;
}

otError otPlatUdpConnect(otUdpSocket *aUdpSocket)
{
    otError             error = OT_ERROR_NONE;
    struct sockaddr_in6 sin6;
    int                 fd;
    bool isDisconnect = memcmp(&aUdpSocket->mPeerName.mAddress, &in6addr_any, sizeof(in6addr_any)) == 0 &&
                        aUdpSocket->mPeerName.mPort == 0;

    VerifyOrExit(aUdpSocket->mHandle != nullptr, error = OT_ERROR_INVALID_ARGS);

    fd = FdFromHandle(aUdpSocket->mHandle);

    memset(&sin6, 0, sizeof(struct sockaddr_in6));
    sin6.sin6_port = htons(aUdpSocket->mPeerName.mPort);
    if (!isDisconnect)
    {
        sin6.sin6_family = AF_INET6;
        memcpy(&sin6.sin6_addr, &aUdpSocket->mPeerName.mAddress, sizeof(sin6.sin6_addr));
    }
    else
    {
#ifdef __APPLE__
        sin6.sin6_family = AF_UNSPEC;
#else
        char      netifName[IFNAMSIZ];
        socklen_t len = sizeof(netifName);

        if (getsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, &netifName, &len) != 0)
        {
            otLogWarnPlat("Failed to read socket bound device: %s", strerror(errno));
            len = 0;
        }

        // There is a bug in linux that connecting to AF_UNSPEC does not disconnect.
        // We create new socket to disconnect.
        SuccessOrExit(error = otPlatUdpClose(aUdpSocket));
        SuccessOrExit(error = otPlatUdpSocket(aUdpSocket));
        SuccessOrExit(error = otPlatUdpBind(aUdpSocket));

        if (len > 0 && netifName[0] != '\0')
        {
            fd = FdFromHandle(aUdpSocket->mHandle);
            VerifyOrExit(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, &netifName, len) == 0, {
                otLogWarnPlat("Failed to bind to device: %s", strerror(errno));
                error = OT_ERROR_FAILED;
            });
        }

        ExitNow();
#endif
    }

    if (connect(fd, reinterpret_cast<struct sockaddr *>(&sin6), sizeof(sin6)) != 0)
    {
#ifdef __APPLE__
        VerifyOrExit(errno == EAFNOSUPPORT && isDisconnect);
#endif
        otLogWarnPlat("Failed to connect to [%s]:%u: %s", Ip6AddressString(&aUdpSocket->mPeerName.mAddress).AsCString(),
                      aUdpSocket->mPeerName.mPort, strerror(errno));
        error = OT_ERROR_FAILED;
    }

exit:
    return error;
}

otError otPlatUdpSend(otUdpSocket *aUdpSocket, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    otError  error = OT_ERROR_NONE;
    int      fd;
    uint16_t len;
    uint8_t  payload[kMaxUdpSize];

    VerifyOrExit(aUdpSocket->mHandle != nullptr, error = OT_ERROR_INVALID_ARGS);
    fd = FdFromHandle(aUdpSocket->mHandle);

    len = otMessageGetLength(aMessage);
    VerifyOrExit(len == otMessageRead(aMessage, 0, payload, len), error = OT_ERROR_INVALID_ARGS);

    if (aMessageInfo->mMulticastLoop)
    {
        int value = 1;

        VerifyOrDie(setsockopt(fd, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &value, sizeof(value)) == 0, OT_EXIT_ERROR_ERRNO);
    }

    error = transmitPacket(fd, payload, len, *aMessageInfo);

    if (aMessageInfo->mMulticastLoop)
    {
        int value = 0;

        VerifyOrDie(setsockopt(fd, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &value, sizeof(value)) == 0, OT_EXIT_ERROR_ERRNO);
    }

exit:
    if (error == OT_ERROR_NONE)
    {
        otMessageFree(aMessage);
    }

    return error;
}

otError otPlatUdpJoinMulticastGroup(otUdpSocket *       aUdpSocket,
                                    otNetifIdentifier   aNetifIdentifier,
                                    const otIp6Address *aAddress)
{
    otError          error = OT_ERROR_NONE;
    struct ipv6_mreq mreq;
    int              fd;

    VerifyOrExit(aUdpSocket->mHandle != nullptr, error = OT_ERROR_INVALID_ARGS);
    fd = FdFromHandle(aUdpSocket->mHandle);

    memcpy(&mreq.ipv6mr_multiaddr, aAddress->mFields.m8, sizeof(mreq.ipv6mr_multiaddr));

    switch (aNetifIdentifier)
    {
    case OT_NETIF_UNSPECIFIED:
        break;
    case OT_NETIF_THREAD:
        mreq.ipv6mr_interface = gNetifIndex;
        break;
    case OT_NETIF_BACKBONE:
#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
        mreq.ipv6mr_interface = gBackboneNetifIndex;
#else
        ExitNow(error = OT_ERROR_NOT_IMPLEMENTED);
#endif
        break;
    }

    VerifyOrExit(setsockopt(fd, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq, sizeof(mreq)) == 0 || errno == EADDRINUSE,
                 error = OT_ERROR_FAILED);

exit:
    if (error != OT_ERROR_NONE)
    {
        otLogCritPlat("IPV6_JOIN_GROUP failed: %s", strerror(errno));
    }
    return error;
}

otError otPlatUdpLeaveMulticastGroup(otUdpSocket *       aUdpSocket,
                                     otNetifIdentifier   aNetifIdentifier,
                                     const otIp6Address *aAddress)
{
    otError          error = OT_ERROR_NONE;
    struct ipv6_mreq mreq;
    int              fd;

    VerifyOrExit(aUdpSocket->mHandle != nullptr, error = OT_ERROR_INVALID_ARGS);
    fd = FdFromHandle(aUdpSocket->mHandle);

    memcpy(&mreq.ipv6mr_multiaddr, aAddress->mFields.m8, sizeof(mreq.ipv6mr_multiaddr));

    switch (aNetifIdentifier)
    {
    case OT_NETIF_UNSPECIFIED:
        break;
    case OT_NETIF_THREAD:
        mreq.ipv6mr_interface = gNetifIndex;
        break;
    case OT_NETIF_BACKBONE:
#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
        mreq.ipv6mr_interface = gBackboneNetifIndex;
#else
        ExitNow(error = OT_ERROR_NOT_IMPLEMENTED);
#endif
        break;
    }

    VerifyOrExit(setsockopt(fd, IPPROTO_IPV6, IPV6_LEAVE_GROUP, &mreq, sizeof(mreq)) == 0 || errno == EADDRINUSE,
                 error = OT_ERROR_FAILED);

exit:
    if (error != OT_ERROR_NONE)
    {
        otLogCritPlat("IPV6_LEAVE_GROUP failed: %s", strerror(errno));
    }
    return error;
}

namespace ot {
namespace Posix {

void Udp::Update(otSysMainloopContext &aContext)
{
    VerifyOrExit(gNetifIndex != 0);

    for (otUdpSocket *socket = otUdpGetSockets(gInstance); socket != nullptr; socket = socket->mNext)
    {
        int fd;

        if (socket->mHandle == nullptr)
        {
            continue;
        }

        fd = FdFromHandle(socket->mHandle);
        FD_SET(fd, &aContext.mReadFdSet);

        if (aContext.mMaxFd < fd)
        {
            aContext.mMaxFd = fd;
        }
    }

exit:
    return;
}

void Udp::Init(const char *aIfName)
{
    if (aIfName == nullptr)
    {
        DieNow(OT_EXIT_INVALID_ARGUMENTS);
    }

    if (aIfName != gNetifName)
    {
        VerifyOrDie(strlen(aIfName) < sizeof(gNetifName) - 1, OT_EXIT_INVALID_ARGUMENTS);
        assert(gNetifIndex == 0);
        strcpy(gNetifName, aIfName);
        gNetifIndex = if_nametoindex(gNetifName);
        VerifyOrDie(gNetifIndex != 0, OT_EXIT_ERROR_ERRNO);
    }

    assert(gNetifIndex != 0);
}

void Udp::SetUp(void)
{
    Mainloop::Manager::Get().Add(*this);
}

void Udp::TearDown(void)
{
    Mainloop::Manager::Get().Remove(*this);
}

void Udp::Deinit(void)
{
    // TODO All platform sockets should be closed
}

Udp &Udp::Get(void)
{
    static Udp sInstance;

    return sInstance;
}

void Udp::Process(const otSysMainloopContext &aContext)
{
    otMessageSettings msgSettings = {false, OT_MESSAGE_PRIORITY_NORMAL};

    for (otUdpSocket *socket = otUdpGetSockets(gInstance); socket != nullptr; socket = socket->mNext)
    {
        int fd = FdFromHandle(socket->mHandle);

        if (fd > 0 && FD_ISSET(fd, &aContext.mReadFdSet))
        {
            otMessageInfo messageInfo;
            otMessage *   message = nullptr;
            uint8_t       payload[kMaxUdpSize];
            uint16_t      length = sizeof(payload);

            memset(&messageInfo, 0, sizeof(messageInfo));
            messageInfo.mSockPort = socket->mSockName.mPort;

            if (OT_ERROR_NONE != receivePacket(fd, payload, length, messageInfo))
            {
                continue;
            }

            message = otUdpNewMessage(gInstance, &msgSettings);

            if (message == nullptr)
            {
                continue;
            }

            if (otMessageAppend(message, payload, length) != OT_ERROR_NONE)
            {
                otMessageFree(message);
                continue;
            }

            socket->mHandler(socket->mContext, message, &messageInfo);
            otMessageFree(message);
            // only process one socket a time
            break;
        }
    }

    return;
}

} // namespace Posix
} // namespace ot
#endif // #if OPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE
