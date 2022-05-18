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

#include "platform-simulation.h"

#include <openthread/random_noncrypto.h>
#include <openthread/platform/trel-udp6.h>

#include "utils/code_utils.h"

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE

// Change DEBUG_LOG to all extra logging
#define DEBUG_LOG 0

// The IPv4 group for receiving
#define TREL_SIM_GROUP "224.0.0.116"
#define TREL_SIM_PORT 9200

#define TREL_MAX_PACKET_SIZE 1800

#define TREL_MAX_PENDING_TX 5

OT_TOOL_PACKED_BEGIN
struct PacketHeader
{
    otIp6Address mSrcIp6Address;
    otIp6Address mDestIp6Address;
    uint16_t     mPort;
} OT_TOOL_PACKED_END;

typedef struct PacketHeader PacketHeader;

OT_TOOL_PACKED_BEGIN
struct Packet
{
    PacketHeader mHeader;
    uint8_t      mPayload[TREL_MAX_PACKET_SIZE];
    uint16_t     mLength; // Total packet length including the header.
} OT_TOOL_PACKED_END;

typedef struct Packet Packet;

static uint8_t sNumPendingTx = 0;
static Packet  sPendingTx[TREL_MAX_PENDING_TX];

static int          sTxFd       = -1;
static int          sRxFd       = -1;
static uint16_t     sPortOffset = 0;
static bool         sEnabled    = true;
static otIp6Address sUnicastAddress;
static otIp6Address sMulticastAddress;
static uint16_t     sUdpPort;

#if DEBUG_LOG
static void dumpBuffer(const void *aBuffer, uint16_t aLength)
{
    const uint8_t *buffer = (const uint8_t *)aBuffer;
    fprintf(stderr, "[ (len:%d) ", aLength);

    while (aLength--)
    {
        fprintf(stderr, "%02x ", *buffer++);
    }

    fprintf(stderr, "]");
}

static const char *ip6AddrToString(const void *aAddress)
{
    static char string[INET6_ADDRSTRLEN];

    return inet_ntop(AF_INET6, aAddress, string, sizeof(string));
}
#endif

static bool ip6AddrsAreEqual(const otIp6Address *aFirst, const otIp6Address *aSecond)
{
    return (memcmp(aFirst, aSecond, sizeof(otIp6Address)) == 0);
}

static void initFds(void)
{
    int                fd;
    int                one = 1;
    struct sockaddr_in sockaddr;
    struct ip_mreqn    mreq;

    memset(&sockaddr, 0, sizeof(sockaddr));

    otEXPECT_ACTION((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) != -1, perror("socket(sTxFd)"));

    sockaddr.sin_family      = AF_INET;
    sockaddr.sin_port        = htons((uint16_t)(TREL_SIM_PORT + sPortOffset + gNodeId));
    sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    otEXPECT_ACTION(setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &sockaddr.sin_addr, sizeof(sockaddr.sin_addr)) != -1,
                    perror("setsockopt(sTxFd, IP_MULTICAST_IF)"));

    otEXPECT_ACTION(setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, &one, sizeof(one)) != -1,
                    perror("setsockopt(sTxFd, IP_MULTICAST_LOOP)"));

    otEXPECT_ACTION(bind(fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) != -1, perror("bind(sTxFd)"));

    // Tx fd is successfully initialized.
    sTxFd = fd;

    otEXPECT_ACTION((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) != -1, perror("socket(sRxFd)"));

    otEXPECT_ACTION(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) != -1,
                    perror("setsockopt(sRxFd, SO_REUSEADDR)"));
    otEXPECT_ACTION(setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one)) != -1,
                    perror("setsockopt(sRxFd, SO_REUSEPORT)"));

    memset(&mreq, 0, sizeof(mreq));
    inet_pton(AF_INET, TREL_SIM_GROUP, &mreq.imr_multiaddr);

    // Always use loopback device to send simulation packets.
    mreq.imr_address.s_addr = inet_addr("127.0.0.1");

    otEXPECT_ACTION(setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &mreq.imr_address, sizeof(mreq.imr_address)) != -1,
                    perror("setsockopt(sRxFd, IP_MULTICAST_IF)"));
    otEXPECT_ACTION(setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) != -1,
                    perror("setsockopt(sRxFd, IP_ADD_MEMBERSHIP)"));

    sockaddr.sin_family      = AF_INET;
    sockaddr.sin_port        = htons((uint16_t)(TREL_SIM_PORT + sPortOffset));
    sockaddr.sin_addr.s_addr = inet_addr(TREL_SIM_GROUP);

    otEXPECT_ACTION(bind(fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) != -1, perror("bind(sRxFd)"));

    // Rx fd is successfully initialized.
    sRxFd = fd;

exit:
    if (sRxFd == -1 || sTxFd == -1)
    {
        exit(EXIT_FAILURE);
    }
}

static void deinitFds(void)
{
    if (sRxFd != -1)
    {
        close(sRxFd);
    }

    if (sTxFd != -1)
    {
        close(sTxFd);
    }
}

static void sendPendingTxPackets(void)
{
    ssize_t            rval;
    struct sockaddr_in sockaddr;

    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    inet_pton(AF_INET, TREL_SIM_GROUP, &sockaddr.sin_addr);

    sockaddr.sin_port = htons((uint16_t)(TREL_SIM_PORT + sPortOffset));

    for (uint8_t i = 0; i < sNumPendingTx; i++)
    {
#if DEBUG_LOG
        fprintf(stderr, "\n[trel-udp] Sending packet (num:%d)", i);
        dumpBuffer(&sPendingTx[i], sPendingTx[i].mLength);
        fprintf(stderr, "\n");
#endif

        rval = sendto(sTxFd, &sPendingTx[i], sPendingTx[i].mLength, 0, (struct sockaddr *)&sockaddr, sizeof(sockaddr));

        if (rval < 0)
        {
            perror("sendto(sTxFd)");
            exit(EXIT_FAILURE);
        }
    }

    sNumPendingTx = 0;
}

//---------------------------------------------------------------------------------------------------------------------
// otPlatTrel

void otPlatTrelUdp6Init(otInstance *aInstance, const otIp6Address *aUnicastAddress, uint16_t aUdpPort)
{
    OT_UNUSED_VARIABLE(aInstance);

    sUnicastAddress = *aUnicastAddress;
    sUdpPort        = aUdpPort;

#if DEBUG_LOG
    fprintf(stderr, "\n[trel-udp6] otPlatTrelUdp6Init(aUnicastAddress:%s, aUdpPort:%d)\n",
            ip6AddrToString(aUnicastAddress), aUdpPort);
#endif
}

void otPlatTrelUdp6UpdateAddress(otInstance *aInstance, const otIp6Address *aUnicastAddress)
{
    OT_UNUSED_VARIABLE(aInstance);

    sUnicastAddress = *aUnicastAddress;

#if DEBUG_LOG
    fprintf(stderr, "\n[trel-udp6] otPlatTrelUdp6UpdateAddress(aUnicastAddress:%s)\n",
            ip6AddrToString(aUnicastAddress));
#endif
}

void otPlatTrelUdp6SubscribeMulticastAddress(otInstance *aInstance, const otIp6Address *aMulticastAddress)
{
    OT_UNUSED_VARIABLE(aInstance);

    sMulticastAddress = *aMulticastAddress;

#if DEBUG_LOG
    fprintf(stderr, "\n[trel-udp6] otPlatTrelUdp6SubscribeMulticastAddress(aMulticastAddress:%s)\n",
            ip6AddrToString(aMulticastAddress));
#endif
}

otError otPlatTrelUdp6SendTo(otInstance *        aInstance,
                             const uint8_t *     aBuffer,
                             uint16_t            aLength,
                             const otIp6Address *aDestAddress)
{
    otError error = OT_ERROR_NONE;
    Packet *packet;

    OT_UNUSED_VARIABLE(aInstance);

    otEXPECT(sEnabled);
    otEXPECT_ACTION(sNumPendingTx < TREL_MAX_PENDING_TX, error = OT_ERROR_ABORT);
    otEXPECT_ACTION(aLength <= TREL_MAX_PACKET_SIZE, error = OT_ERROR_ABORT);

    packet                          = &sPendingTx[sNumPendingTx++];
    packet->mHeader.mSrcIp6Address  = sUnicastAddress;
    packet->mHeader.mDestIp6Address = *aDestAddress;
    packet->mHeader.mPort           = sUdpPort;
    packet->mLength                 = aLength + sizeof(PacketHeader);
    memcpy(packet->mPayload, aBuffer, aLength);

exit:
    return error;
}

otError otPlatTrelUdp6SetTestMode(otInstance *aInstance, bool aEnable)
{
    OT_UNUSED_VARIABLE(aInstance);

    sEnabled = aEnable;

    if (!sEnabled)
    {
        sNumPendingTx = 0;
    }

    return OT_ERROR_NONE;
}

//---------------------------------------------------------------------------------------------------------------------
// platformTrel system

void platformTrelInit(uint32_t aSpeedUpFactor)
{
    char *str;

    str = getenv("PORT_OFFSET");

    if (str != NULL)
    {
        char *endptr;

        sPortOffset = (uint16_t)strtol(str, &endptr, 0);

        if (*endptr != '\0')
        {
            fprintf(stderr, "\nInvalid PORT_OFFSET: %s\n", str);
            exit(EXIT_FAILURE);
        }

        sPortOffset *= (MAX_NETWORK_SIZE + 1);
    }

    initFds();

    OT_UNUSED_VARIABLE(aSpeedUpFactor);
}

void platformTrelDeinit(void)
{
    deinitFds();
}

void platformTrelUpdateFdSet(fd_set *aReadFdSet, fd_set *aWriteFdSet, struct timeval *aTimeout, int *aMaxFd)
{
    OT_UNUSED_VARIABLE(aTimeout);

    // Always ready to receive
    if (aReadFdSet != NULL)
    {
        FD_SET(sRxFd, aReadFdSet);

        if (aMaxFd != NULL && *aMaxFd < sRxFd)
        {
            *aMaxFd = sRxFd;
        }
    }

    if ((aWriteFdSet != NULL) && (sNumPendingTx > 0))
    {
        FD_SET(sTxFd, aWriteFdSet);

        if (aMaxFd != NULL && *aMaxFd < sTxFd)
        {
            *aMaxFd = sTxFd;
        }
    }
}

void platformTrelProcess(otInstance *aInstance, const fd_set *aReadFdSet, const fd_set *aWriteFdSet)
{
    if (FD_ISSET(sTxFd, aWriteFdSet) && (sNumPendingTx > 0))
    {
        sendPendingTxPackets();
    }

    if (FD_ISSET(sRxFd, aReadFdSet))
    {
        Packet  rxPacket;
        ssize_t rval;

        rval = recvfrom(sRxFd, (char *)&rxPacket, sizeof(rxPacket) - sizeof(rxPacket.mLength), 0, NULL, NULL);

        if (rval < 0)
        {
            perror("recvfrom(sRxFd)");
            exit(EXIT_FAILURE);
        }

        rxPacket.mLength = (uint16_t)(rval);

#if DEBUG_LOG
        fprintf(stderr, "\n[trel-udp6] recvdPacket()");
        fprintf(stderr, " src:%s", ip6AddrToString(&rxPacket.mHeader.mSrcIp6Address));
        fprintf(stderr, " dst:%s", ip6AddrToString(&rxPacket.mHeader.mDestIp6Address));
        fprintf(stderr, " port:%u ", rxPacket.mHeader.mPort);
        dumpBuffer(&rxPacket, rxPacket.mLength);
        fprintf(stderr, "\n");
#endif
        if (sEnabled && (rxPacket.mHeader.mPort == sUdpPort) &&
            (ip6AddrsAreEqual(&rxPacket.mHeader.mDestIp6Address, &sUnicastAddress) ||
             ip6AddrsAreEqual(&rxPacket.mHeader.mDestIp6Address, &sMulticastAddress)))
        {
            otPlatTrelUdp6HandleReceived(aInstance, rxPacket.mPayload, rxPacket.mLength - sizeof(PacketHeader));
        }
    }
}

// This is added for RCP build to be built ok
OT_TOOL_WEAK void otPlatTrelUdp6HandleReceived(otInstance *aInstance, uint8_t *aBuffer, uint16_t aLength)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aBuffer);
    OT_UNUSED_VARIABLE(aLength);

    assert(false);
}

#endif // OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
