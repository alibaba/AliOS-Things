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

/**
 * @file
 *   This file implements platform for TREL using IPv6/UDP socket under POSIX.
 */

#include "openthread-posix-config.h"

#include "platform-posix.h"

#if OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET && !defined(__linux__)
#error "netlink socket use is only supported on linux platform"
#endif

#include <arpa/inet.h>
#include <assert.h>
#include <fcntl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#if OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET
#include <linux/if_tun.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#endif

#include <openthread/platform/trel-udp6.h>

#include "common/code_utils.hpp"
#include "common/logging.hpp"
#include "posix/platform/radio_url.hpp"

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE

#define TREL_MAX_PACKET_SIZE 1400
#define TREL_PACKET_POOL_SIZE 5

#define USEC_PER_MSEC 1000u
#define TREL_SOCKET_BIND_MAX_WAIT_TIME_MSEC 4000u

#define TREL_UNICAST_ADDRESS_PREFIX_LEN 64
#define TREL_UNICAST_ADDRESS_SCOPE 2 // The unicast address is link-local

typedef struct TxPacket
{
    struct TxPacket *mNext;
    uint8_t          mBuffer[TREL_MAX_PACKET_SIZE];
    uint16_t         mLength;
    otIp6Address     mDestAddress;
} TxPacket;

static uint8_t      sRxPacketBuffer[TREL_MAX_PACKET_SIZE];
static uint16_t     sRxPacketLength;
static TxPacket     sTxPacketPool[TREL_PACKET_POOL_SIZE];
static TxPacket *   sFreeTxPacketHead;  // A singly linked list of free/available `TxPacket` from pool.
static TxPacket *   sTxPacketQueueTail; // A circular linked list for queued tx packets.
static char         sInterfaceName[IFNAMSIZ + 1];
static bool         sInitialized     = false;
static bool         sEnabled         = false;
static int          sInterfaceIndex  = -1;
static int          sMulticastSocket = -1;
static int          sSocket          = -1;
static uint16_t     sUdpPort         = 0;
static otIp6Address sInterfaceAddress;

#if OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET
constexpr static uint64_t kAddInterfaceAddressTimeoutSecs = 10;

static int sNetlinkSocket                = -1;
uint64_t   sAddInterfaceAddressRetryTime = 0;
#endif

#if OPENTHREAD_CONFIG_LOG_PLATFORM
#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_CRIT)
static const char *Ip6AddrToString(const void *aAddress)
{
    static char string[INET6_ADDRSTRLEN];
    return inet_ntop(AF_INET6, aAddress, string, sizeof(string));
}
#endif // #if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_CRIT)

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_DEBG)
static const char *BufferToString(const uint8_t *aBuffer, uint16_t aLength)
{
    const uint16_t kMaxWrite = 16;
    static char    string[1600];

    uint16_t num = 0;
    char *   cur = &string[0];
    char *   end = &string[sizeof(string) - 1];

    cur += snprintf(cur, (uint16_t)(end - cur), "[(len:%d) ", aLength);
    VerifyOrExit(cur < end);

    while (aLength-- && (num < kMaxWrite))
    {
        cur += snprintf(cur, (uint16_t)(end - cur), "%02x ", *aBuffer++);
        VerifyOrExit(cur < end);

        num++;
    }

    if (aLength != 0)
    {
        cur += snprintf(cur, (uint16_t)(end - cur), "... ");
        VerifyOrExit(cur < end);
    }

    *cur++ = ']';
    VerifyOrExit(cur < end);

    *cur++ = '\0';

exit:
    *end = '\0';
    return string;
}
#endif // #if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_DEBG)
#endif // OPENTHREAD_CONFIG_LOG_PLATFORM

#if OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET

static void UpdateUnicastAddress(const otIp6Address *aUnicastAddress, bool aToAdd)
{
    int            ret;
    struct rtattr *rta;

    struct
    {
        struct nlmsghdr  nh;
        struct ifaddrmsg ifa;
        char             buf[64];
    } request;

    memset(&request, 0, sizeof(request));

    request.nh.nlmsg_len   = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
    request.nh.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_CREATE | NLM_F_EXCL;
    request.nh.nlmsg_type  = aToAdd ? RTM_NEWADDR : RTM_DELADDR;
    request.nh.nlmsg_pid   = 0;
    request.nh.nlmsg_seq   = 0;

    request.ifa.ifa_family    = AF_INET6;
    request.ifa.ifa_prefixlen = TREL_UNICAST_ADDRESS_PREFIX_LEN;
    request.ifa.ifa_flags     = IFA_F_NODAD;
    request.ifa.ifa_scope     = TREL_UNICAST_ADDRESS_SCOPE;
    request.ifa.ifa_index     = (unsigned int)(sInterfaceIndex);

    rta = reinterpret_cast<struct rtattr *>((reinterpret_cast<char *>(&request)) + NLMSG_ALIGN(request.nh.nlmsg_len));
    rta->rta_type = IFA_LOCAL;
    rta->rta_len  = RTA_LENGTH(sizeof(otIp6Address));

    memcpy(RTA_DATA(rta), aUnicastAddress, sizeof(otIp6Address));

    request.nh.nlmsg_len = NLMSG_ALIGN(request.nh.nlmsg_len) + rta->rta_len;

    ret = send(sNetlinkSocket, &request, request.nh.nlmsg_len, 0);
    VerifyOrDie(ret != -1, OT_EXIT_ERROR_ERRNO);
}

static void AddUnicastAddress(const otIp6Address *aUnicastAddress)
{
    otLogDebgPlat("[trel] AddUnicastAddress(%s)", Ip6AddrToString(aUnicastAddress));
    UpdateUnicastAddress(aUnicastAddress, /* aToAdd */ true);
    sAddInterfaceAddressRetryTime = otPlatTimeGet() + kAddInterfaceAddressTimeoutSecs * US_PER_S;
}

static void RemoveUnicastAddress(const otIp6Address *aUnicastAddress)
{
    otLogDebgPlat("[trel] RemoveUnicastAddress(%s)", Ip6AddrToString(aUnicastAddress));
    UpdateUnicastAddress(aUnicastAddress, /* aToAdd */ false);
}

static void HandleAddInterfaceAddressTimeout(void)
{
    otLogWarnPlat("[trel] Failed to add TREL interface address after %d seconds. Trying to add again.",
                  kAddInterfaceAddressTimeoutSecs);
    RemoveUnicastAddress(&sInterfaceAddress);
    AddUnicastAddress(&sInterfaceAddress);
    sAddInterfaceAddressRetryTime = otPlatTimeGet() + kAddInterfaceAddressTimeoutSecs * US_PER_S;
}

static void ProcessNetifAddrEvent(struct nlmsghdr *aNetlinkMessage)
{
    struct ifaddrmsg *ifaddr = reinterpret_cast<struct ifaddrmsg *>(NLMSG_DATA(aNetlinkMessage));
    size_t            rtaLength;

    VerifyOrExit(ifaddr->ifa_index == static_cast<unsigned int>(sInterfaceIndex) && ifaddr->ifa_family == AF_INET6);

    rtaLength = IFA_PAYLOAD(aNetlinkMessage);

    for (struct rtattr *rta = reinterpret_cast<struct rtattr *>(IFA_RTA(ifaddr)); RTA_OK(rta, rtaLength);
         rta                = RTA_NEXT(rta, rtaLength))
    {
        if (rta->rta_type != IFA_ADDRESS)
        {
            continue;
        }

        if (memcmp(&sInterfaceAddress, RTA_DATA(rta), sizeof(sInterfaceAddress)) != 0)
        {
            continue;
        }

        if (aNetlinkMessage->nlmsg_type == RTM_NEWADDR)
        {
            otLogInfoPlat("[trel] Interface address added successfully.");
            sAddInterfaceAddressRetryTime = 0;
        }
        else if (aNetlinkMessage->nlmsg_type == RTM_DELADDR)
        {
            if (sAddInterfaceAddressRetryTime == 0)
            {
                otLogWarnPlat("[trel] Interface address removed unexpectedly.");
                sAddInterfaceAddressRetryTime = otPlatTimeGet() + kAddInterfaceAddressTimeoutSecs * US_PER_S;
            }
        }
    }

exit:
    return;
}

static void ReceiveNetlinkMessage(void)
{
    const size_t kMaxNetLinkBufSize = 8192;
    ssize_t      len;
    union
    {
        nlmsghdr mHeader;
        uint8_t  mBuffer[kMaxNetLinkBufSize];
    } msgBuffer;

    len = recv(sNetlinkSocket, msgBuffer.mBuffer, sizeof(msgBuffer.mBuffer), 0);
    if (len < 0)
    {
        otLogCritPlat("failed to receive netlink message: %s", strerror(errno));
        ExitNow();
    }

    for (struct nlmsghdr *header = &msgBuffer.mHeader; NLMSG_OK(header, static_cast<size_t>(len));
         header                  = NLMSG_NEXT(header, len))
    {
        switch (header->nlmsg_type)
        {
        case RTM_NEWADDR:
        case RTM_DELADDR:
            ProcessNetifAddrEvent(header);
            break;
        case NLMSG_ERROR:
        {
            struct nlmsgerr *errMsg = reinterpret_cast<struct nlmsgerr *>(NLMSG_DATA(header));

            if (errMsg->error != 0)
            {
                otLogWarnPlat("netlink NLMSG_ERROR response: seq=%u, error=%d", header->nlmsg_seq, errMsg->error);
            }
        }
        default:
            break;
        }
    }

exit:
    return;
}

#else // OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET

static void AddUnicastAddress(const otIp6Address *aUnicastAddress)
{
    int mgmtFd;
    int ret;
    struct in6_ifreq
    {
        struct in6_addr ifr6_addr;
        uint32_t        ifr6_prefixlen;
        int             ifr6_ifindex;
    } ifr6;

    otLogDebgPlat("[trel] AddUnicastAddress(%s)", Ip6AddrToString(aUnicastAddress));

    mgmtFd = SocketWithCloseExec(AF_INET6, SOCK_DGRAM, IPPROTO_IP, kSocketNonBlock);
    VerifyOrDie(mgmtFd >= 0, OT_EXIT_ERROR_ERRNO);

    memcpy(&ifr6.ifr6_addr, aUnicastAddress, sizeof(otIp6Address));
    ifr6.ifr6_prefixlen = 64;
    ifr6.ifr6_ifindex   = sInterfaceIndex;

    ret = ioctl(mgmtFd, SIOCSIFADDR, &ifr6);

    if (!(ret == 0 || errno == EALREADY || errno == EEXIST))
    {
        otLogCritPlat("[trel] Failed to add unicast address %s on TREL netif \"%s\"", Ip6AddrToString(aUnicastAddress),
                      sInterfaceName);
        DieNow(OT_EXIT_ERROR_ERRNO);
    }

    close(mgmtFd);
}

static void RemoveUnicastAddress(const otIp6Address *aUnicastAddress)
{
    int mgmtFd;
    int ret;
    struct in6_ifreq
    {
        struct in6_addr ifr6_addr;
        uint32_t        ifr6_prefixlen;
        int             ifr6_ifindex;
    } ifr6;

    otLogDebgPlat("[trel] RemoveUnicastAddress(%s)", Ip6AddrToString(aUnicastAddress));

    mgmtFd = SocketWithCloseExec(AF_INET6, SOCK_DGRAM, IPPROTO_IP, kSocketNonBlock);
    VerifyOrDie(mgmtFd >= 0, OT_EXIT_ERROR_ERRNO);

    memcpy(&ifr6.ifr6_addr, aUnicastAddress, sizeof(otIp6Address));
    ifr6.ifr6_prefixlen = 64;
    ifr6.ifr6_ifindex   = sInterfaceIndex;

    ret = ioctl(mgmtFd, SIOCDIFADDR, &ifr6);

    VerifyOrDie(ret == 0, OT_EXIT_ERROR_ERRNO);

    close(mgmtFd);
}

#endif // OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET

static void PrepareSocket(void)
{
    int                 val;
    struct sockaddr_in6 sockAddr;
    uint64_t            startTime;
    bool                isSocketBound = false;

    otLogDebgPlat("[trel] PrepareSocket()");

    sSocket = SocketWithCloseExec(AF_INET6, SOCK_DGRAM, 0, kSocketNonBlock);
    VerifyOrDie(sSocket >= 0, OT_EXIT_ERROR_ERRNO);

    // Set the multicast interface index (for tx), disable loop back
    // of multicast tx and set the multicast hop limit to 1 to reach
    // a single sub-net.

    val = sInterfaceIndex;
    VerifyOrDie(setsockopt(sSocket, IPPROTO_IPV6, IPV6_MULTICAST_IF, &val, sizeof(val)) == 0, OT_EXIT_ERROR_ERRNO);

    val = 0;
    VerifyOrDie(setsockopt(sSocket, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &val, sizeof(val)) == 0, OT_EXIT_ERROR_ERRNO);

    val = 1;
    VerifyOrDie(setsockopt(sSocket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == 0, OT_EXIT_ERROR_ERRNO);
    VerifyOrDie(setsockopt(sSocket, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val)) == 0, OT_EXIT_ERROR_ERRNO);

    val = 1;
    VerifyOrDie(setsockopt(sSocket, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &val, sizeof(val)) == 0, OT_EXIT_ERROR_ERRNO);

    // Make the socket non-blocking to allow immediate tx attempt.
    val = fcntl(sSocket, F_GETFL, 0);
    VerifyOrDie(val != -1, OT_EXIT_ERROR_ERRNO);
    val = val | O_NONBLOCK;
    VerifyOrDie(fcntl(sSocket, F_SETFL, val) == 0, OT_EXIT_ERROR_ERRNO);

    // Bind the socket. The address to which we want to bind the
    // socket, is itself added earlier above. The address therefore
    // may not be immediately available/ready on the interface and the
    // socket `bind()` call may fail with `EADDRNOTAVAIL` error. In
    // such a case, we keep trying up to a maximum wait time.

    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin6_family = AF_INET6;
    sockAddr.sin6_port   = htons(sUdpPort);
    memcpy(&sockAddr.sin6_addr, &sInterfaceAddress, sizeof(otIp6Address));
    sockAddr.sin6_scope_id = (uint32_t)sInterfaceIndex;

    startTime = otPlatTimeGet();

    while (otPlatTimeGet() - startTime < TREL_SOCKET_BIND_MAX_WAIT_TIME_MSEC * USEC_PER_MSEC)
    {
        if (bind(sSocket, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) != -1)
        {
            isSocketBound = true;
            break;
        }

        if (errno == EADDRNOTAVAIL)
        {
            continue;
        }

        otLogCritPlat("[trel] Failed to bind socket to %s (port %d) on TREL netif \"%s\"",
                      Ip6AddrToString(&sInterfaceAddress), sUdpPort, sInterfaceName);
        DieNow(OT_EXIT_ERROR_ERRNO);
    }

    if (!isSocketBound)
    {
        otLogCritPlat("[trel] Timed out waiting for address %s to become available for binding on TREL "
                      "netif \"%s\" - timeout %lu (ms)",
                      Ip6AddrToString(&sInterfaceAddress), sInterfaceName, TREL_SOCKET_BIND_MAX_WAIT_TIME_MSEC);
        DieNow(OT_EXIT_ERROR_ERRNO);
    }
}

static otError SendPacket(const uint8_t *aBuffer, uint16_t aLength, const otIp6Address *aDestAddress)
{
    otError             error = OT_ERROR_NONE;
    struct sockaddr_in6 sockAddr;
    ssize_t             ret;

    VerifyOrExit(sSocket >= 0, error = OT_ERROR_INVALID_STATE);

    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin6_family = AF_INET6;
    sockAddr.sin6_port   = htons(sUdpPort);
    memcpy(&sockAddr.sin6_addr, aDestAddress, sizeof(otIp6Address));

    ret = sendto(sSocket, aBuffer, aLength, 0, (struct sockaddr *)&sockAddr, sizeof(sockAddr));

    if (ret != aLength)
    {
        otLogDebgPlat("[trel] SendPacket() -- sendto() failed errno %d", errno);

        switch (errno)
        {
        case ENETUNREACH:
        case ENETDOWN:
        case EHOSTUNREACH:
            error = OT_ERROR_ABORT;
            break;

        default:
            error = OT_ERROR_INVALID_STATE;
        }
    }

exit:
    otLogDebgPlat("[trel] SendPacket(%s) err:%s pkt:%s", Ip6AddrToString(aDestAddress), otThreadErrorToString(error),
                  BufferToString(aBuffer, aLength));

    return error;
}

static void ReceivePacket(int aSocket, otInstance *aInstance)
{
    struct sockaddr_in6 sockAddr;
    socklen_t           sockAddrLen = sizeof(sockAddr);
    ssize_t             ret;

    memset(&sockAddr, 0, sizeof(sockAddr));

    ret = recvfrom(aSocket, (char *)sRxPacketBuffer, sizeof(sRxPacketBuffer), 0, (struct sockaddr *)&sockAddr,
                   &sockAddrLen);
    VerifyOrDie(ret >= 0, OT_EXIT_ERROR_ERRNO);

    sRxPacketLength = (uint16_t)(ret);

    if (sRxPacketLength > sizeof(sRxPacketBuffer))
    {
        sRxPacketLength = sizeof(sRxPacketLength);
    }

    otLogDebgPlat("[trel] ReceivePacket() - received from %s port:%d, id:%d, pkt:%s",
                  Ip6AddrToString(&sockAddr.sin6_addr), ntohs(sockAddr.sin6_port), sockAddr.sin6_scope_id,
                  BufferToString(sRxPacketBuffer, sRxPacketLength));

    if (sEnabled)
    {
        otPlatTrelUdp6HandleReceived(aInstance, sRxPacketBuffer, sRxPacketLength);
    }
}

static void InitPacketQueue(void)
{
    sTxPacketQueueTail = NULL;

    // Chain all the packets in pool in the free linked list.
    sFreeTxPacketHead = NULL;

    for (uint16_t index = 0; index < OT_ARRAY_LENGTH(sTxPacketPool); index++)
    {
        TxPacket *packet = &sTxPacketPool[index];

        packet->mNext     = sFreeTxPacketHead;
        sFreeTxPacketHead = packet;
    }
}

static void SendQueuedPackets(void)
{
    while (sTxPacketQueueTail != NULL)
    {
        TxPacket *packet = sTxPacketQueueTail->mNext; // tail->mNext is the head of the list.

        if (SendPacket(packet->mBuffer, packet->mLength, &packet->mDestAddress) == OT_ERROR_INVALID_STATE)
        {
            otLogDebgPlat("[trel] SendQueuedPackets() - SendPacket() would block");
            break;
        }

        // Remove the `packet` from the packet queue (circular
        // linked list).

        if (packet == sTxPacketQueueTail)
        {
            sTxPacketQueueTail = NULL;
        }
        else
        {
            sTxPacketQueueTail->mNext = packet->mNext;
        }

        // Add the `packet` to the free packet singly linked list.

        packet->mNext     = sFreeTxPacketHead;
        sFreeTxPacketHead = packet;
    }
}

static otError EnqueuePacket(const uint8_t *aBuffer, uint16_t aLength, const otIp6Address *aDestAddress)
{
    otError   error = OT_ERROR_NONE;
    TxPacket *packet;

    // Allocate an available packet entry (from the free packet list)
    // and copy the packet content into it.

    VerifyOrExit(sFreeTxPacketHead != NULL, error = OT_ERROR_NO_BUFS);
    packet            = sFreeTxPacketHead;
    sFreeTxPacketHead = sFreeTxPacketHead->mNext;

    memcpy(packet->mBuffer, aBuffer, aLength);
    packet->mLength      = aLength;
    packet->mDestAddress = *aDestAddress;

    // Add packet to the tail of TxPacketQueue circular linked-list.

    if (sTxPacketQueueTail == NULL)
    {
        packet->mNext      = packet;
        sTxPacketQueueTail = packet;
    }
    else
    {
        packet->mNext             = sTxPacketQueueTail->mNext;
        sTxPacketQueueTail->mNext = packet;
        sTxPacketQueueTail        = packet;
    }

    otLogDebgPlat("[trel] EnqueuePacket(%s) - %s", Ip6AddrToString(aDestAddress), BufferToString(aBuffer, aLength));

exit:
    return error;
}

//---------------------------------------------------------------------------------------------------------------------
// otPlatTrelUdp6

void otPlatTrelUdp6Init(otInstance *aInstance, const otIp6Address *aUnicastAddress, uint16_t aUdpPort)
{
    OT_UNUSED_VARIABLE(aInstance);

    int                 val;
    struct sockaddr_in6 sockAddr;

    VerifyOrExit(sEnabled);

    otLogDebgPlat("[trel] otPlatTrelUdp6Init(%s, port:%d)", Ip6AddrToString(aUnicastAddress), aUdpPort);

    sUdpPort          = aUdpPort;
    sInterfaceAddress = *aUnicastAddress;
    sInterfaceIndex   = (int)if_nametoindex(sInterfaceName);

    if (sInterfaceIndex <= 0)
    {
        otLogCritPlat("[trel] Failed to find index of TREL netif \"%s\"", sInterfaceName);
        DieNow(OT_EXIT_ERROR_ERRNO);
    }

#if OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET
    {
        struct sockaddr_nl addr;

        sNetlinkSocket = SocketWithCloseExec(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE, kSocketNonBlock);
        VerifyOrDie(sNetlinkSocket >= 0, OT_EXIT_ERROR_ERRNO);

        memset(&addr, 0, sizeof(addr));
        addr.nl_family = AF_NETLINK;
        addr.nl_groups = RTMGRP_IPV6_IFADDR;

        VerifyOrDie(bind(sNetlinkSocket, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) == 0,
                    OT_EXIT_ERROR_ERRNO);
    }
#endif

    RemoveUnicastAddress(aUnicastAddress);
    AddUnicastAddress(aUnicastAddress);

    sMulticastSocket = socket(AF_INET6, SOCK_DGRAM, 0);
    VerifyOrDie(sMulticastSocket >= 0, OT_EXIT_ERROR_ERRNO);

    val = 1;
    VerifyOrDie(setsockopt(sMulticastSocket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == 0, OT_EXIT_ERROR_ERRNO);
    VerifyOrDie(setsockopt(sMulticastSocket, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val)) == 0, OT_EXIT_ERROR_ERRNO);

    // To receive from multicast addresses, the socket need to be
    // bound to `in6addr_any` address.
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin6_family   = AF_INET6;
    sockAddr.sin6_port     = htons(sUdpPort);
    sockAddr.sin6_addr     = in6addr_any;
    sockAddr.sin6_scope_id = (uint32_t)sInterfaceIndex;

    if (bind(sMulticastSocket, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) == -1)
    {
        otLogCritPlat("[trel] Failed to bind multicast socket to any address on TREL netif \"%s\"", sInterfaceName);
        DieNow(OT_EXIT_ERROR_ERRNO);
    }

    PrepareSocket();

exit:
    return;
}

void otPlatTrelUdp6UpdateAddress(otInstance *aInstance, const otIp6Address *aUnicastAddress)
{
    OT_UNUSED_VARIABLE(aInstance);

    VerifyOrExit(sEnabled);

    assert(sSocket >= 0);

    otLogDebgPlat("[trel] otPlatTrelUdp6UpdateAddress(%s)", Ip6AddrToString(aUnicastAddress));

    VerifyOrExit(memcmp(aUnicastAddress, &sInterfaceAddress, sizeof(otIp6Address)) != 0);

    close(sSocket);
    RemoveUnicastAddress(&sInterfaceAddress);

    sInterfaceAddress = *aUnicastAddress;
    AddUnicastAddress(aUnicastAddress);

    PrepareSocket();

exit:
    return;
}

void otPlatTrelUdp6SubscribeMulticastAddress(otInstance *aInstance, const otIp6Address *aMulticastAddress)
{
    OT_UNUSED_VARIABLE(aInstance);

    struct ipv6_mreq mr;

    VerifyOrExit(sEnabled);

    assert(sMulticastSocket != -1);

    memcpy(&mr.ipv6mr_multiaddr, aMulticastAddress, sizeof(otIp6Address));
    mr.ipv6mr_interface = (unsigned int)sInterfaceIndex;
    VerifyOrDie(setsockopt(sMulticastSocket, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mr, sizeof(mr)) == 0, OT_EXIT_ERROR_ERRNO);

    otLogDebgPlat("[trel] otPlatTrelUdp6SubscribeMulticastAddress(%s)", Ip6AddrToString(aMulticastAddress));

exit:
    return;
}

otError otPlatTrelUdp6SendTo(otInstance *        aInstance,
                             const uint8_t *     aBuffer,
                             uint16_t            aLength,
                             const otIp6Address *aDestAddress)
{
    OT_UNUSED_VARIABLE(aInstance);

    otError error = OT_ERROR_NONE;

    VerifyOrExit(sEnabled);

    assert(aLength <= TREL_MAX_PACKET_SIZE);

    otLogDebgPlat("[trel] otPlatTrelUdp6SendTo(%s) %s", Ip6AddrToString(aDestAddress),
                  BufferToString(aBuffer, aLength));

    // We try to send the packet immediately. If it fails (e.g.,
    // network is down) `SendPacket()` returns `OT_ERROR_ABORT`. If
    // the send operation would block (e.g., socket is not yet ready
    // or is out of buffer) we get `OT_ERROR_INVALID_STATE`. In that
    // case we enqueue the packet to send it later when socket becomes
    // ready.

    error = SendPacket(aBuffer, aLength, aDestAddress);

    if (error == OT_ERROR_INVALID_STATE)
    {
        error = EnqueuePacket(aBuffer, aLength, aDestAddress);

        if (error != OT_ERROR_NONE)
        {
            error = OT_ERROR_ABORT;
        }
    }

exit:
    return error;
}

otError otPlatTrelUdp6SetTestMode(otInstance *aInstance, bool aEnable)
{
    OT_UNUSED_VARIABLE(aInstance);

    otError error = OT_ERROR_NONE;

    VerifyOrExit(aEnable != sEnabled);

    if (aEnable)
    {
        VerifyOrExit(sInitialized, error = OT_ERROR_FAILED);
    }

    sEnabled = aEnable;

    if (!sEnabled)
    {
        InitPacketQueue();
    }

exit:
    return error;
}

//---------------------------------------------------------------------------------------------------------------------
// platformTrel system

void platformTrelInit(const char *aTrelUrl)
{
    assert(!sInitialized);

    if (aTrelUrl != NULL)
    {
        ot::Posix::RadioUrl url(aTrelUrl);

        strncpy(sInterfaceName, url.GetPath(), sizeof(sInterfaceName) - 1);
    }
    else
    {
        strncpy(sInterfaceName, OPENTHREAD_CONFIG_POSIX_APP_TREL_INTERFACE_NAME, sizeof(sInterfaceName) - 1);
    }

    sInterfaceName[sizeof(sInterfaceName) - 1] = 0;
    otLogDebgPlat("[trel] platformTrelInit(InterfaceName:\"%s\")", sInterfaceName);

    InitPacketQueue();

    // Disable trel platform when interface name is empty.
    sInitialized = (sInterfaceName[0] != '\0');
    sEnabled     = sInitialized;
}

void platformTrelDeinit(void)
{
    VerifyOrExit(sInitialized);

    if (sSocket != -1)
    {
        close(sSocket);
    }

    if (sMulticastSocket != -1)
    {
        close(sMulticastSocket);
    }

    if (!otIp6IsAddressUnspecified(&sInterfaceAddress))
    {
        RemoveUnicastAddress(&sInterfaceAddress);
    }

#if OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET
    if (sNetlinkSocket != -1)
    {
        close(sNetlinkSocket);
        sNetlinkSocket = -1;
    }
#endif

    sInitialized = false;
    sEnabled     = false;

    otLogDebgPlat("[trel] platformTrelDeinit()");
exit:
    return;
}

void platformTrelUpdateFdSet(fd_set *aReadFdSet, fd_set *aWriteFdSet, int *aMaxFd, struct timeval *aTimeout)
{
    assert((aReadFdSet != NULL) && (aWriteFdSet != NULL) && (aMaxFd != NULL) && (aTimeout != NULL));
    VerifyOrExit((sSocket >= 0) && (sMulticastSocket >= 0));
#if OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET
    VerifyOrExit(sNetlinkSocket >= 0);
#endif

    FD_SET(sMulticastSocket, aReadFdSet);
    FD_SET(sSocket, aReadFdSet);
#if OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET
    FD_SET(sNetlinkSocket, aReadFdSet);
#endif

    if (sTxPacketQueueTail != NULL)
    {
        FD_SET(sSocket, aWriteFdSet);
    }

    if (*aMaxFd < sMulticastSocket)
    {
        *aMaxFd = sMulticastSocket;
    }

    if (*aMaxFd < sSocket)
    {
        *aMaxFd = sSocket;
    }

#if OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET
    if (*aMaxFd < sNetlinkSocket)
    {
        *aMaxFd = sNetlinkSocket;
    }

    if (sAddInterfaceAddressRetryTime > 0)
    {
        uint64_t now = otPlatTimeGet();

        if (sAddInterfaceAddressRetryTime > now)
        {
            uint64_t remain = sAddInterfaceAddressRetryTime - now;

            if (remain <
                (static_cast<uint64_t>(aTimeout->tv_sec) * US_PER_S + static_cast<uint64_t>(aTimeout->tv_usec)))
            {
                aTimeout->tv_sec  = static_cast<time_t>(remain / US_PER_S);
                aTimeout->tv_usec = static_cast<suseconds_t>(remain % US_PER_S);
            }
        }
        else
        {
            aTimeout->tv_sec  = 0;
            aTimeout->tv_usec = 0;
        }
    }
#endif

exit:
    return;
}

void platformTrelProcess(otInstance *aInstance, const fd_set *aReadFdSet, const fd_set *aWriteFdSet)
{
    VerifyOrExit((sSocket >= 0) && (sMulticastSocket >= 0));
#if OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET
    VerifyOrExit(sNetlinkSocket >= 0);
#endif

    if (FD_ISSET(sSocket, aWriteFdSet))
    {
        SendQueuedPackets();
    }

    if (FD_ISSET(sSocket, aReadFdSet))
    {
        ReceivePacket(sSocket, aInstance);
    }

    if (FD_ISSET(sMulticastSocket, aReadFdSet))
    {
        ReceivePacket(sMulticastSocket, aInstance);
    }

#if OPENTHREAD_CONFIG_POSIX_TREL_USE_NETLINK_SOCKET
    if (FD_ISSET(sNetlinkSocket, aReadFdSet))
    {
        ReceiveNetlinkMessage();
    }

    if (sAddInterfaceAddressRetryTime > 0)
    {
        uint64_t now = otPlatTimeGet();

        if (sAddInterfaceAddressRetryTime <= now)
        {
            sAddInterfaceAddressRetryTime = 0;

            HandleAddInterfaceAddressTimeout();
        }
    }
#endif

exit:
    return;
}

#endif // #if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
