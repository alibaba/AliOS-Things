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
 *   This file implements the platform network on Linux.
 */

#include "openthread-posix-config.h"
#include "platform-posix.h"

#if defined(__APPLE__)

//	NOTE: on mac OS, the utun driver is present on the system and "works" --
//	but in a limited way.  In particular, the mac OS "utun" driver is marked IFF_POINTTOPOINT,
//	and you cannot clear that flag with SIOCSIFFLAGS (it's part of the IFF_CANTCHANGE definition
//	in xnu's net/if.h [but removed from the mac OS SDK net/if.h]).  And unfortunately, mac OS's
//	build of mDNSResponder won't allow for mDNS over an interface marked IFF_POINTTOPOINT
//	(see comments near definition of MulticastInterface in mDNSMacOSX.c for the bogus reasoning).
//
//	There is an alternative.  An open-source tuntap kernel extension is available here:
//
//		<http://tuntaposx.sourceforge.net>
//		<https://sourceforge.net/p/tuntaposx/code/ci/master/tree/>
//
//	and can be installed via homebrew here:
//
//		<https://formulae.brew.sh/cask/tuntap>
//
//	Building from source and installing isn't trivial, and it's
//	not getting easier (https://forums.developer.apple.com/thread/79590).
//
//	If you want mDNS support, then you can't use Apple utun.  I use the non-Apple driver
//	pretty much exclusively, because mDNS is a requirement.

#if !(defined(OPENTHREAD_POSIX_CONFIG_MACOS_TUN_OPTION) &&                         \
      ((OPENTHREAD_POSIX_CONFIG_MACOS_TUN_OPTION == OT_POSIX_CONFIG_MACOS_UTUN) || \
       (OPENTHREAD_POSIX_CONFIG_MACOS_TUN_OPTION == OT_POSIX_CONFIG_MACOS_TUN)))
#error "Unexpected tunnel driver selection"
#endif

#endif // defined(__APPLE__)

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <ifaddrs.h>
#ifdef __linux__
#include <linux/if_tun.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#endif // __linux__
#include <net/if.h>
#include <net/if_arp.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#if defined(__APPLE__) || defined(__NetBSD__) || defined(__FreeBSD__)
#include <netinet/in.h>
#if defined(__APPLE__) || defined(__FreeBSD__)
#include <net/if_var.h>
#endif // defined(__APPLE__) || defined(__FreeBSD__)
#include <net/route.h>
#include <netinet6/in6_var.h>
#if defined(__APPLE__) || defined(__FreeBSD__)
// the prf_ra structure is defined inside another structure (in6_prflags), and C++
//   treats that as out of scope if another structure tries to use it -- this (slightly gross)
//   workaround makes us dependent on our definition remaining in sync (at least the size of it),
//   so we add a compile-time check that will fail if the SDK ever changes
//
// our definition of the struct:
struct prf_ra
{
    u_char onlink : 1;
    u_char autonomous : 1;
    u_char reserved : 6;
} prf_ra;
// object that contains the SDK's version of the structure:
struct in6_prflags compile_time_check_prflags;
// compile time check to make sure they're the same size:
extern int
    compile_time_check_struct_prf_ra[(sizeof(struct prf_ra) == sizeof(compile_time_check_prflags.prf_ra)) ? 1 : -1];
#endif
#include <net/if_dl.h>    // struct sockaddr_dl
#include <netinet6/nd6.h> // ND6_INFINITE_LIFETIME

#ifdef __APPLE__
#if OPENTHREAD_POSIX_CONFIG_MACOS_TUN_OPTION == OT_POSIX_CONFIG_MACOS_UTUN
#include <net/if_utun.h>
#endif

#if OPENTHREAD_POSIX_CONFIG_MACOS_TUN_OPTION == OT_POSIX_CONFIG_MACOS_TUN
#include <sys/ioccom.h>
// FIX ME: include the tun_ioctl.h file (challenging, as it's location depends on where the developer puts it)
#define TUNSIFHEAD _IOW('t', 96, int)
#define TUNGIFHEAD _IOR('t', 97, int)
#endif

#include <sys/kern_control.h>
#endif // defined(__APPLE__)

#if defined(__NetBSD__) || defined(__FreeBSD__)
#include <net/if_tun.h>
#endif // defined(__NetBSD__) || defined(__FreeBSD__)

#endif // defined(__APPLE__) || defined(__NetBSD__) || defined(__FreeBSD__)

#include <openthread/icmp6.h>
#include <openthread/instance.h>
#include <openthread/ip6.h>
#include <openthread/message.h>
#include <openthread/netdata.h>
#include <openthread/platform/misc.h>

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/logging.hpp"
#include "net/ip6_address.hpp"

unsigned int gNetifIndex = 0;
char         gNetifName[IFNAMSIZ];

const char *otSysGetThreadNetifName(void)
{
    return gNetifName;
}

unsigned int otSysGetThreadNetifIndex(void)
{
    return gNetifIndex;
}

#if OPENTHREAD_CONFIG_PLATFORM_NETIF_ENABLE
#include "posix/platform/ip6_utils.hpp"

using namespace ot::Posix::Ip6Utils;

#ifndef OPENTHREAD_POSIX_TUN_DEVICE

#ifdef __linux__
#define OPENTHREAD_POSIX_TUN_DEVICE "/dev/net/tun"
#elif defined(__NetBSD__) || defined(__FreeBSD__)
#define OPENTHREAD_POSIX_TUN_DEVICE "/dev/tun0"
#elif defined(__APPLE__)
#if OPENTHREAD_POSIX_CONFIG_MACOS_TUN_OPTION == OT_POSIX_CONFIG_MACOS_UTUN
#define OPENTHREAD_POSIX_TUN_DEVICE // not used - calculated dynamically
#elif OPENTHREAD_POSIX_CONFIG_MACOS_TUN_OPTION == OT_POSIX_CONFIG_MACOS_TUN
#define OPENTHREAD_POSIX_TUN_DEVICE "/dev/tun0"
#endif
#else
// good luck -- untested platform...
#define OPENTHREAD_POSIX_TUN_DEVICE "/dev/net/tun"
#endif

#endif // OPENTHREAD_TUN_DEVICE

#if defined(__linux__)
static uint32_t sNetlinkSequence = 0; ///< Netlink message sequence.
#endif

#if OPENTHREAD_POSIX_CONFIG_INSTALL_EXTERNAL_ROUTES_ENABLE
#if defined(__linux__)
static constexpr uint32_t kExternalRoutePriority  = OPENTHREAD_POSIX_CONFIG_EXTERNAL_ROUTE_PRIORITY;
static constexpr uint8_t  kMaxExternalRoutesNum   = OPENTHREAD_POSIX_CONFIG_MAX_EXTERNAL_ROUTE_NUM;
static uint8_t            sAddedExternalRoutesNum = 0;
static otIp6Prefix        sAddedExternalRoutes[kMaxExternalRoutesNum];
#else
#error "OPENTHREAD_POSIX_CONFIG_INSTALL_EXTERNAL_ROUTES_ENABLE only works on Linux platform"
#endif // defined(__linux__)
#endif

#if defined(RTM_NEWMADDR) || defined(__NetBSD__)
// on some BSDs (mac OS, FreeBSD), we get RTM_NEWMADDR/RTM_DELMADDR messages, so we don't need to monitor using MLD
// on NetBSD, MLD monitoring simply doesn't work
#define OPENTHREAD_POSIX_USE_MLD_MONITOR 0
#else
// on some platforms (Linux, but others might be made to work), we do not get information about multicast
// group joining via AF_NETLINK or AF_ROUTE sockets.  on those platform, we must listen for IPv6 ICMP
// MLDv2 messages to know when mulicast memberships change
// 		https://stackoverflow.com/questions/37346289/using-netlink-is-it-possible-to-listen-whenever-multicast-group-membership-is-ch
#define OPENTHREAD_POSIX_USE_MLD_MONITOR 1
#endif // defined(RTM_NEWMADDR) || defined(__NetBSD__)

// some platforms (like NetBSD) do not have RTM_NEWMADDR/RTM_DELMADDR messages, and they ALSO lack
// working MLDv2 support.  for those platforms, we must tell the OpenThread interface to
// pass ALL multicast packets up; the kernel's IPv6 will filter and drop those that have no listeners
#define OPENTHREAD_POSIX_MULTICAST_PROMISCUOUS_REQUIRED 0
#if !OPENTHREAD_POSIX_USE_MLD_MONITOR
#if defined(__NetBSD__)
#undef OPENTHREAD_POSIX_MULTICAST_PROMISCUOUS_REQUIRED
#define OPENTHREAD_POSIX_MULTICAST_PROMISCUOUS_REQUIRED 1
#endif
#endif

#if defined(__NetBSD__) || defined(__FreeBSD__)
static otError destroyTunnel(void);
#endif

static int sTunFd     = -1; ///< Used to exchange IPv6 packets.
static int sIpFd      = -1; ///< Used to manage IPv6 stack on Thread interface.
static int sNetlinkFd = -1; ///< Used to receive netlink events.
#if OPENTHREAD_POSIX_USE_MLD_MONITOR
static int sMLDMonitorFd = -1; ///< Used to receive MLD events.
#endif
#if OPENTHREAD_POSIX_USE_MLD_MONITOR
// ff02::16
static const otIp6Address kMLDv2MulticastAddress = {
    {{0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16}}};

OT_TOOL_PACKED_BEGIN
struct MLDv2Header
{
    uint8_t  mType;
    uint8_t  _rsv0;
    uint16_t mChecksum;
    uint16_t _rsv1;
    uint16_t mNumRecords;
} OT_TOOL_PACKED_END;

OT_TOOL_PACKED_BEGIN
struct MLDv2Record
{
    uint8_t         mRecordType;
    uint8_t         mAuxDataLen;
    uint16_t        mNumSources;
    struct in6_addr mMulticastAddress;
} OT_TOOL_PACKED_END;

enum
{
    kICMPv6MLDv2Type                      = 143,
    kICMPv6MLDv2RecordChangeToExcludeType = 3,
    kICMPv6MLDv2RecordChangeToIncludeType = 4,
};
#endif

static constexpr size_t kMaxIp6Size = OPENTHREAD_CONFIG_IP6_MAX_DATAGRAM_LENGTH;
#if defined(RTM_NEWLINK) && defined(RTM_DELLINK)
static bool sIsSyncingState = false;
#endif

#define OPENTHREAD_POSIX_LOG_TUN_PACKETS 0

#if !defined(__linux__)
static bool UnicastAddressIsSubscribed(otInstance *aInstance, const otNetifAddress *netAddr)
{
    const otNetifAddress *address = otIp6GetUnicastAddresses(aInstance);

    while (address != nullptr)
    {
        if (memcmp(address->mAddress.mFields.m8, netAddr->mAddress.mFields.m8, sizeof(address->mAddress.mFields.m8)) ==
            0)
        {
            return true;
        }

        address = address->mNext;
    }

    return false;
}
#endif

#if defined(__APPLE__) || defined(__NetBSD__) || defined(__FreeBSD__)
static const uint8_t allOnes[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static void InitNetaskWithPrefixLength(struct in6_addr *address, uint8_t prefixLen)
{
#define MAX_PREFIX_LENGTH (OT_IP6_ADDRESS_SIZE * CHAR_BIT)
    if (prefixLen > MAX_PREFIX_LENGTH)
    {
        prefixLen = MAX_PREFIX_LENGTH;
    }

    ot::Ip6::Address addr;

    addr.Clear();
    addr.SetPrefix(allOnes, prefixLen);
    memcpy(address, addr.mFields.m8, sizeof(addr.mFields.m8));
}

static uint8_t NetmaskToPrefixLength(const struct sockaddr_in6 *netmask)
{
    return otIp6PrefixMatch(reinterpret_cast<const otIp6Address *>(netmask->sin6_addr.s6_addr),
                            reinterpret_cast<const otIp6Address *>(allOnes));
}
#endif

#if defined(__linux__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"

static void UpdateUnicastLinux(const otIp6AddressInfo &aAddressInfo, bool aIsAdded)
{
    struct rtattr *rta;

    struct
    {
        struct nlmsghdr  nh;
        struct ifaddrmsg ifa;
        char             buf[512];
    } req;

    memset(&req, 0, sizeof(req));

    req.nh.nlmsg_len   = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
    req.nh.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_CREATE | NLM_F_EXCL;
    req.nh.nlmsg_type  = aIsAdded ? RTM_NEWADDR : RTM_DELADDR;
    req.nh.nlmsg_pid   = 0;
    req.nh.nlmsg_seq   = ++sNetlinkSequence;

    req.ifa.ifa_family    = AF_INET6;
    req.ifa.ifa_prefixlen = aAddressInfo.mPrefixLength;
    req.ifa.ifa_flags     = IFA_F_NODAD;
    req.ifa.ifa_scope     = aAddressInfo.mScope;
    req.ifa.ifa_index     = gNetifIndex;

    rta           = reinterpret_cast<struct rtattr *>((reinterpret_cast<char *>(&req)) + NLMSG_ALIGN(req.nh.nlmsg_len));
    rta->rta_type = IFA_LOCAL;
    rta->rta_len  = RTA_LENGTH(sizeof(*aAddressInfo.mAddress));

    memcpy(RTA_DATA(rta), aAddressInfo.mAddress, sizeof(*aAddressInfo.mAddress));

    req.nh.nlmsg_len = NLMSG_ALIGN(req.nh.nlmsg_len) + rta->rta_len;

    if (!aAddressInfo.mPreferred)
    {
        struct ifa_cacheinfo cacheinfo;

        rta           = reinterpret_cast<struct rtattr *>((reinterpret_cast<char *>(rta)) + rta->rta_len);
        rta->rta_type = IFA_CACHEINFO;
        rta->rta_len  = RTA_LENGTH(sizeof(cacheinfo));

        memset(&cacheinfo, 0, sizeof(cacheinfo));
        cacheinfo.ifa_valid = UINT32_MAX;

        memcpy(RTA_DATA(rta), &cacheinfo, sizeof(cacheinfo));

        req.nh.nlmsg_len += rta->rta_len;
    }

    if (send(sNetlinkFd, &req, req.nh.nlmsg_len, 0) != -1)
    {
        otLogInfoPlat("[netif] Sent request#%u to %s %s/%u", sNetlinkSequence, (aIsAdded ? "add" : "remove"),
                      Ip6AddressString(aAddressInfo.mAddress).AsCString(), aAddressInfo.mPrefixLength);
    }
    else
    {
        otLogWarnPlat("[netif] Failed to send request#%u to %s %s/%u", sNetlinkSequence, (aIsAdded ? "add" : "remove"),
                      Ip6AddressString(aAddressInfo.mAddress).AsCString(), aAddressInfo.mPrefixLength);
    }
}

#pragma GCC diagnostic pop
#endif // defined(__linux__)

static void UpdateUnicast(otInstance *aInstance, const otIp6AddressInfo &aAddressInfo, bool aIsAdded)
{
    OT_UNUSED_VARIABLE(aInstance);

    assert(gInstance == aInstance);
    assert(sIpFd >= 0);

#if defined(__linux__)
    UpdateUnicastLinux(aAddressInfo, aIsAdded);
#elif defined(__APPLE__) || defined(__NetBSD__) || defined(__FreeBSD__)
    {
        int                 rval;
        struct in6_aliasreq ifr6;

        memset(&ifr6, 0, sizeof(ifr6));
        strlcpy(ifr6.ifra_name, gNetifName, sizeof(ifr6.ifra_name));
        ifr6.ifra_addr.sin6_family = AF_INET6;
        ifr6.ifra_addr.sin6_len    = sizeof(ifr6.ifra_addr);
        memcpy(&ifr6.ifra_addr.sin6_addr, aAddressInfo.mAddress, sizeof(struct in6_addr));
        ifr6.ifra_prefixmask.sin6_family = AF_INET6;
        ifr6.ifra_prefixmask.sin6_len    = sizeof(ifr6.ifra_prefixmask);
        InitNetaskWithPrefixLength(&ifr6.ifra_prefixmask.sin6_addr, aAddressInfo.mPrefixLength);
        ifr6.ifra_lifetime.ia6t_vltime    = ND6_INFINITE_LIFETIME;
        ifr6.ifra_lifetime.ia6t_pltime    = ND6_INFINITE_LIFETIME;

#if defined(__APPLE__)
        ifr6.ifra_lifetime.ia6t_expire    = ND6_INFINITE_LIFETIME;
        ifr6.ifra_lifetime.ia6t_preferred = (aAddressInfo.mPreferred ? ND6_INFINITE_LIFETIME : 0);
#endif

        rval = ioctl(sIpFd, aIsAdded ? SIOCAIFADDR_IN6 : SIOCDIFADDR_IN6, &ifr6);
        if (rval == 0)
        {
            otLogInfoPlat("[netif] %s %s/%u", (aIsAdded ? "Added" : "Removed"),
                          Ip6AddressString(aAddressInfo.mAddress).AsCString(), aAddressInfo.mPrefixLength);
        }
        else if (errno != EALREADY)
        {
            otLogWarnPlat("[netif] Failed to %s %s/%u: %s", (aIsAdded ? "add" : "remove"),
                          Ip6AddressString(aAddressInfo.mAddress).AsCString(), aAddressInfo.mPrefixLength,
                          strerror(errno));
        }
    }
#endif
}

static void UpdateMulticast(otInstance *aInstance, const otIp6Address &aAddress, bool aIsAdded)
{
    OT_UNUSED_VARIABLE(aInstance);

    struct ipv6_mreq mreq;
    otError          error = OT_ERROR_NONE;
    int              err;

    assert(gInstance == aInstance);

    VerifyOrExit(sIpFd >= 0);
    memcpy(&mreq.ipv6mr_multiaddr, &aAddress, sizeof(mreq.ipv6mr_multiaddr));
    mreq.ipv6mr_interface = gNetifIndex;

    err = setsockopt(sIpFd, IPPROTO_IPV6, (aIsAdded ? IPV6_JOIN_GROUP : IPV6_LEAVE_GROUP), &mreq, sizeof(mreq));

#if defined(__APPLE__) || defined(__FreeBSD__)
    if ((err != 0) && (errno == EINVAL) && (IN6_IS_ADDR_MC_LINKLOCAL(&mreq.ipv6mr_multiaddr)))
    {
        // FIX ME
        // on mac OS (and FreeBSD), the first time we run (but not subsequently), we get a failure on this
        // particular join. do we need to bring up the interface at least once prior to joining? we need to figure
        // out why so we can get rid of this workaround
        char addressString[INET6_ADDRSTRLEN + 1];

        inet_ntop(AF_INET6, mreq.ipv6mr_multiaddr.s6_addr, addressString, sizeof(addressString));
        otLogWarnPlat("[netif] Ignoring %s failure (EINVAL) for MC LINKLOCAL address (%s)",
                      aIsAdded ? "IPV6_JOIN_GROUP" : "IPV6_LEAVE_GROUP", addressString);
        err = 0;
    }
#endif

    if (err != 0)
    {
        otLogWarnPlat("[netif] %s failure (%d)", aIsAdded ? "IPV6_JOIN_GROUP" : "IPV6_LEAVE_GROUP", errno);
        error = OT_ERROR_FAILED;
        ExitNow();
    }

    otLogInfoPlat("[netif] %s multicast address %s", aIsAdded ? "Added" : "Removed",
                  Ip6AddressString(&aAddress).AsCString());

exit:
    SuccessOrDie(error);
}

static void UpdateLink(otInstance *aInstance)
{
    otError      error = OT_ERROR_NONE;
    struct ifreq ifr;
    bool         ifState = false;
    bool         otState = false;

    assert(gInstance == aInstance);

    VerifyOrExit(sIpFd >= 0);
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, gNetifName, sizeof(ifr.ifr_name));
    VerifyOrExit(ioctl(sIpFd, SIOCGIFFLAGS, &ifr) == 0, perror("ioctl"); error = OT_ERROR_FAILED);

    ifState = ((ifr.ifr_flags & IFF_UP) == IFF_UP) ? true : false;
    otState = otIp6IsEnabled(aInstance);

    otLogNotePlat("[netif] Changing interface state to %s%s.", otState ? "up" : "down",
                  (ifState == otState) ? " (already done, ignoring)" : "");

    if (ifState != otState)
    {
        ifr.ifr_flags = otState ? (ifr.ifr_flags | IFF_UP) : (ifr.ifr_flags & ~IFF_UP);
        VerifyOrExit(ioctl(sIpFd, SIOCSIFFLAGS, &ifr) == 0, perror("ioctl"); error = OT_ERROR_FAILED);
#if defined(RTM_NEWLINK) && defined(RTM_DELLINK)
        // wait for RTM_NEWLINK event before processing notification from kernel to avoid infinite loop
        sIsSyncingState = true;
#endif
    }

exit:
    if (error != OT_ERROR_NONE)
    {
        otLogWarnPlat("[netif] Failed to update state %s", otThreadErrorToString(error));
    }
}

#if OPENTHREAD_POSIX_CONFIG_INSTALL_EXTERNAL_ROUTES_ENABLE
void AddRtAttr(struct nlmsghdr *aHeader, uint32_t aMaxLen, uint8_t aType, const void *aData, uint8_t aLen)
{
    uint8_t        len = RTA_LENGTH(aLen);
    struct rtattr *rta;

    assert(NLMSG_ALIGN(aHeader->nlmsg_len) + RTA_ALIGN(len) <= aMaxLen);
    OT_UNUSED_VARIABLE(aMaxLen);

    rta           = (struct rtattr *)((char *)(aHeader) + NLMSG_ALIGN((aHeader)->nlmsg_len));
    rta->rta_type = aType;
    rta->rta_len  = len;
    if (aLen)
    {
        memcpy(RTA_DATA(rta), aData, aLen);
    }
    aHeader->nlmsg_len = NLMSG_ALIGN(aHeader->nlmsg_len) + RTA_ALIGN(len);
}

void AddRtAttrUint32(struct nlmsghdr *aHeader, uint32_t aMaxLen, uint8_t aType, uint32_t aData)
{
    AddRtAttr(aHeader, aMaxLen, aType, &aData, sizeof(aData));
}

static otError AddExternalRoute(const otIp6Prefix &aPrefix)
{
    constexpr unsigned int kBufSize = 128;
    struct
    {
        struct nlmsghdr header;
        struct rtmsg    msg;
        char            buf[kBufSize];
    } req{};
    unsigned char data[sizeof(in6_addr)];
    char          addrBuf[OT_IP6_ADDRESS_STRING_SIZE];
    unsigned int  netifIdx = otSysGetThreadNetifIndex();
    otError       error    = OT_ERROR_NONE;

    VerifyOrExit(netifIdx > 0, error = OT_ERROR_INVALID_STATE);
    VerifyOrExit(sNetlinkFd >= 0, error = OT_ERROR_INVALID_STATE);
    VerifyOrExit(sAddedExternalRoutesNum < kMaxExternalRoutesNum, error = OT_ERROR_NO_BUFS);

    req.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_CREATE | NLM_F_EXCL;

    req.header.nlmsg_len  = NLMSG_LENGTH(sizeof(rtmsg));
    req.header.nlmsg_type = RTM_NEWROUTE;
    req.header.nlmsg_pid  = 0;
    req.header.nlmsg_seq  = ++sNetlinkSequence;

    req.msg.rtm_family   = AF_INET6;
    req.msg.rtm_src_len  = 0;
    req.msg.rtm_dst_len  = aPrefix.mLength;
    req.msg.rtm_tos      = 0;
    req.msg.rtm_scope    = RT_SCOPE_UNIVERSE;
    req.msg.rtm_type     = RTN_UNICAST;
    req.msg.rtm_table    = RT_TABLE_MAIN;
    req.msg.rtm_protocol = RTPROT_BOOT;
    req.msg.rtm_flags    = 0;

    otIp6AddressToString(&aPrefix.mPrefix, addrBuf, OT_IP6_ADDRESS_STRING_SIZE);
    inet_pton(AF_INET6, addrBuf, data);
    AddRtAttr(reinterpret_cast<nlmsghdr *>(&req), sizeof(req), RTA_DST, data, sizeof(data));
    AddRtAttrUint32(&req.header, sizeof(req), RTA_PRIORITY, kExternalRoutePriority);
    AddRtAttrUint32(&req.header, sizeof(req), RTA_OIF, netifIdx);

    if (send(sNetlinkFd, &req, sizeof(req), 0) < 0)
    {
        VerifyOrExit(errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK, error = OT_ERROR_BUSY);
        DieNow(OT_EXIT_ERROR_ERRNO);
    }
exit:
    return error;
}

static otError DeleteExternalRoute(const otIp6Prefix &aPrefix)
{
    constexpr unsigned int kBufSize = 512;
    struct
    {
        struct nlmsghdr header;
        struct rtmsg    msg;
        char            buf[kBufSize];
    } req{};
    unsigned char data[sizeof(in6_addr)];
    char          addrBuf[OT_IP6_ADDRESS_STRING_SIZE];
    unsigned int  netifIdx = otSysGetThreadNetifIndex();
    otError       error    = OT_ERROR_NONE;

    VerifyOrExit(netifIdx > 0, error = OT_ERROR_INVALID_STATE);
    VerifyOrExit(sNetlinkFd >= 0, error = OT_ERROR_INVALID_STATE);

    req.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_NONREC;

    req.header.nlmsg_len  = NLMSG_LENGTH(sizeof(rtmsg));
    req.header.nlmsg_type = RTM_DELROUTE;
    req.header.nlmsg_pid  = 0;
    req.header.nlmsg_seq  = ++sNetlinkSequence;

    req.msg.rtm_family   = AF_INET6;
    req.msg.rtm_src_len  = 0;
    req.msg.rtm_dst_len  = aPrefix.mLength;
    req.msg.rtm_tos      = 0;
    req.msg.rtm_scope    = RT_SCOPE_UNIVERSE;
    req.msg.rtm_type     = RTN_UNICAST;
    req.msg.rtm_table    = RT_TABLE_MAIN;
    req.msg.rtm_protocol = RTPROT_BOOT;
    req.msg.rtm_flags    = 0;

    otIp6AddressToString(&aPrefix.mPrefix, addrBuf, OT_IP6_ADDRESS_STRING_SIZE);
    inet_pton(AF_INET6, addrBuf, data);
    AddRtAttr(reinterpret_cast<nlmsghdr *>(&req), sizeof(req), RTA_DST, data, sizeof(data));
    AddRtAttrUint32(&req.header, sizeof(req), RTA_OIF, netifIdx);

    if (send(sNetlinkFd, &req, sizeof(req), 0) < 0)
    {
        VerifyOrExit(errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK, error = OT_ERROR_BUSY);
        DieNow(OT_EXIT_ERROR_ERRNO);
    }

exit:
    return error;
}

bool HasExternalRouteInNetData(otInstance *aInstance, const otIp6Prefix &aExternalRoute)
{
    otNetworkDataIterator iterator = OT_NETWORK_DATA_ITERATOR_INIT;
    otExternalRouteConfig config;
    bool                  found = false;

    while (otNetDataGetNextRoute(aInstance, &iterator, &config) == OT_ERROR_NONE)
    {
        if (otIp6ArePrefixesEqual(&config.mPrefix, &aExternalRoute))
        {
            found = true;
            break;
        }
    }
    return found;
}

bool HasAddedExternalRoute(const otIp6Prefix &aExternalRoute)
{
    bool found = false;

    for (uint8_t i = 0; i < sAddedExternalRoutesNum; ++i)
    {
        if (otIp6ArePrefixesEqual(&sAddedExternalRoutes[i], &aExternalRoute))
        {
            found = true;
            break;
        }
    }
    return found;
}

static void UpdateExternalRoutes(otInstance *aInstance)
{
    otError               error;
    otNetworkDataIterator iterator = OT_NETWORK_DATA_ITERATOR_INIT;
    otExternalRouteConfig config;
    char                  prefixString[OT_IP6_PREFIX_STRING_SIZE];

    for (int i = 0; i < static_cast<int>(sAddedExternalRoutesNum); ++i)
    {
        if (HasExternalRouteInNetData(aInstance, sAddedExternalRoutes[i]))
        {
            continue;
        }
        if ((error = DeleteExternalRoute(sAddedExternalRoutes[i])) != OT_ERROR_NONE)
        {
            otIp6PrefixToString(&sAddedExternalRoutes[i], prefixString, sizeof(prefixString));
            otLogWarnPlat("[netif] Failed to delete an external route %s in kernel: %s", prefixString,
                          otThreadErrorToString(error));
        }
        else
        {
            sAddedExternalRoutes[i] = sAddedExternalRoutes[sAddedExternalRoutesNum - 1];
            --sAddedExternalRoutesNum;
            --i;
        }
    }

    while (otNetDataGetNextRoute(aInstance, &iterator, &config) == OT_ERROR_NONE)
    {
        if (config.mRloc16 == otThreadGetRloc16(aInstance) || HasAddedExternalRoute(config.mPrefix))
        {
            continue;
        }
        VerifyOrExit(sAddedExternalRoutesNum < kMaxExternalRoutesNum,
                     otLogWarnPlat("[netif] No buffer to add more external routes in kernel"));
        if ((error = AddExternalRoute(config.mPrefix)) != OT_ERROR_NONE)
        {
            otIp6PrefixToString(&config.mPrefix, prefixString, sizeof(prefixString));
            otLogWarnPlat("[netif] Failed to add an external route %s in kernel: %s", prefixString,
                          otThreadErrorToString(error));
        }
        else
        {
            sAddedExternalRoutes[sAddedExternalRoutesNum++] = config.mPrefix;
        }
    }
exit:
    return;
}
#endif // OPENTHREAD_POSIX_CONFIG_INSTALL_EXTERNAL_ROUTES_ENABLE

static void processAddressChange(const otIp6AddressInfo *aAddressInfo, bool aIsAdded, void *aContext)
{
    if (aAddressInfo->mAddress->mFields.m8[0] == 0xff)
    {
        UpdateMulticast(static_cast<otInstance *>(aContext), *aAddressInfo->mAddress, aIsAdded);
    }
    else
    {
        UpdateUnicast(static_cast<otInstance *>(aContext), *aAddressInfo, aIsAdded);
    }
}

void platformNetifStateChange(otInstance *aInstance, otChangedFlags aFlags)
{
    if (OT_CHANGED_THREAD_NETIF_STATE & aFlags)
    {
        UpdateLink(aInstance);
    }
#if OPENTHREAD_POSIX_CONFIG_INSTALL_EXTERNAL_ROUTES_ENABLE
    if (OT_CHANGED_THREAD_NETDATA & aFlags)
    {
        UpdateExternalRoutes(aInstance);
    }
#endif
}

static void processReceive(otMessage *aMessage, void *aContext)
{
    OT_UNUSED_VARIABLE(aContext);

    char     packet[kMaxIp6Size + 4];
    otError  error     = OT_ERROR_NONE;
    uint16_t length    = otMessageGetLength(aMessage);
    size_t   offset    = 0;
    uint16_t maxLength = sizeof(packet) - 4;
#if defined(__APPLE__) || defined(__NetBSD__) || defined(__FreeBSD__)
    // BSD tunnel drivers use (for legacy reasons) a 4-byte header to determine the address family of the packet
    offset += 4;
#endif

    assert(gInstance == aContext);
    assert(length <= kMaxIp6Size);

    VerifyOrExit(sTunFd > 0);

    VerifyOrExit(otMessageRead(aMessage, 0, &packet[offset], maxLength) == length, error = OT_ERROR_NO_BUFS);

#if OPENTHREAD_POSIX_LOG_TUN_PACKETS
    otLogInfoPlat("[netif] Packet from NCP (%u bytes)", static_cast<uint16_t>(length));
    otDumpInfo(OT_LOG_REGION_PLATFORM, "", &packet[offset], length);
#endif

#if defined(__APPLE__) || defined(__NetBSD__) || defined(__FreeBSD__)
    packet[0] = 0;
    packet[1] = 0;
    packet[2] = (PF_INET6 << 8) & 0xFF;
    packet[3] = (PF_INET6 << 0) & 0xFF;
    length += 4;
#endif

    VerifyOrExit(write(sTunFd, packet, length) == length, perror("write"); error = OT_ERROR_FAILED);

exit:
    otMessageFree(aMessage);

    if (error != OT_ERROR_NONE)
    {
        otLogWarnPlat("[netif] Failed to receive, error:%s", otThreadErrorToString(error));
    }
}

static void processTransmit(otInstance *aInstance)
{
    otMessage *message = nullptr;
    ssize_t    rval;
    char       packet[kMaxIp6Size];
    otError    error  = OT_ERROR_NONE;
    size_t     offset = 0;

    assert(gInstance == aInstance);

    rval = read(sTunFd, packet, sizeof(packet));
    VerifyOrExit(rval > 0, error = OT_ERROR_FAILED);

    {
        otMessageSettings settings;

        settings.mLinkSecurityEnabled = (otThreadGetDeviceRole(aInstance) != OT_DEVICE_ROLE_DISABLED);
        settings.mPriority            = OT_MESSAGE_PRIORITY_LOW;
        message                       = otIp6NewMessage(aInstance, &settings);
        VerifyOrExit(message != nullptr, error = OT_ERROR_NO_BUFS);
    }

#if defined(__APPLE__) || defined(__NetBSD__) || defined(__FreeBSD__)
    // BSD tunnel drivers have (for legacy reasons), may have a 4-byte header on them
    if ((rval >= 4) && (packet[0] == 0) && (packet[1] == 0))
    {
        rval -= 4;
        offset = 4;
    }
#endif

#if OPENTHREAD_POSIX_LOG_TUN_PACKETS
    otLogInfoPlat("[netif] Packet to NCP (%hu bytes)", static_cast<uint16_t>(rval));
    otDumpInfo(OT_LOG_REGION_PLATFORM, "", &packet[offset], static_cast<size_t>(rval));
#endif

    SuccessOrExit(error = otMessageAppend(message, &packet[offset], static_cast<uint16_t>(rval)));

    error   = otIp6Send(aInstance, message);
    message = nullptr;

exit:
    if (message != nullptr)
    {
        otMessageFree(message);
    }

    if (error != OT_ERROR_NONE)
    {
        otLogWarnPlat("[netif] Failed to transmit, error:%s", otThreadErrorToString(error));
    }
}

static void logAddrEvent(bool isAdd, bool isUnicast, struct sockaddr_in6 &addr6, otError error)
{
    char addressString[INET6_ADDRSTRLEN + 1];

    // these parameters may not be used if logging is disabled at compile time
    OT_UNUSED_VARIABLE(isUnicast);
    OT_UNUSED_VARIABLE(addr6);
    OT_UNUSED_VARIABLE(addressString);

    if ((error == OT_ERROR_NONE) || ((isAdd) && (error == OT_ERROR_ALREADY)) ||
        ((!isAdd) && (error == OT_ERROR_NOT_FOUND)))
    {
        otLogNotePlat("[netif] %s [%s] %s%s", isAdd ? "ADD" : "DEL", isUnicast ? "U" : "M",
                      inet_ntop(AF_INET6, addr6.sin6_addr.s6_addr, addressString, sizeof(addressString)),
                      error == OT_ERROR_ALREADY ? " (already subscribed, ignored)"
                                                : error == OT_ERROR_NOT_FOUND ? " (not found, ignored)" : "");
    }
    else
    {
        otLogWarnPlat("[netif] %s [%s] %s failed (%s)", isAdd ? "ADD" : "DEL", isUnicast ? "U" : "M",
                      inet_ntop(AF_INET6, addr6.sin6_addr.s6_addr, addressString, sizeof(addressString)),
                      otThreadErrorToString(error));
    }
}

#if defined(__linux__)

static void processNetifAddrEvent(otInstance *aInstance, struct nlmsghdr *aNetlinkMessage)
{
    struct ifaddrmsg *  ifaddr = reinterpret_cast<struct ifaddrmsg *>(NLMSG_DATA(aNetlinkMessage));
    size_t              rtaLength;
    otError             error = OT_ERROR_NONE;
    struct sockaddr_in6 addr6;

    VerifyOrExit(ifaddr->ifa_index == static_cast<unsigned int>(gNetifIndex) && ifaddr->ifa_family == AF_INET6);

    rtaLength = IFA_PAYLOAD(aNetlinkMessage);

    for (struct rtattr *rta = reinterpret_cast<struct rtattr *>(IFA_RTA(ifaddr)); RTA_OK(rta, rtaLength);
         rta                = RTA_NEXT(rta, rtaLength))
    {
        switch (rta->rta_type)
        {
        case IFA_ADDRESS:
        case IFA_LOCAL:
        case IFA_BROADCAST:
        case IFA_ANYCAST:
        case IFA_MULTICAST:
        {
            ot::Ip6::Address addr;
            memcpy(&addr, RTA_DATA(rta), sizeof(addr));

            memset(&addr6, 0, sizeof(addr6));
            addr6.sin6_family = AF_INET6;
            memcpy(&addr6.sin6_addr, RTA_DATA(rta), sizeof(addr6.sin6_addr));

            if (aNetlinkMessage->nlmsg_type == RTM_NEWADDR)
            {
                if (!addr.IsMulticast())
                {
                    otNetifAddress netAddr;

                    netAddr.mAddress      = addr;
                    netAddr.mPrefixLength = ifaddr->ifa_prefixlen;

                    error = otIp6AddUnicastAddress(aInstance, &netAddr);
                }
                else
                {
                    otNetifMulticastAddress netAddr;

                    netAddr.mAddress = addr;

                    error = otIp6SubscribeMulticastAddress(aInstance, &addr);
                }

                logAddrEvent(/* isAdd */ true, !addr.IsMulticast(), addr6, error);
                if (error == OT_ERROR_ALREADY)
                {
                    error = OT_ERROR_NONE;
                }

                SuccessOrExit(error);
            }
            else if (aNetlinkMessage->nlmsg_type == RTM_DELADDR)
            {
                if (!addr.IsMulticast())
                {
                    error = otIp6RemoveUnicastAddress(aInstance, &addr);
                }
                else
                {
                    error = otIp6UnsubscribeMulticastAddress(aInstance, &addr);
                }

                logAddrEvent(/* isAdd */ false, !addr.IsMulticast(), addr6, error);
                if (error == OT_ERROR_NOT_FOUND)
                {
                    error = OT_ERROR_NONE;
                }

                SuccessOrExit(error);
            }
            else
            {
                continue;
            }
            break;
        }

        default:
            otLogWarnPlat("[netif] Unexpected address type (%d).", (int)rta->rta_type);
            break;
        }
    }

exit:
    if (error != OT_ERROR_NONE)
    {
        otLogWarnPlat("[netif] Failed to process event, error:%s", otThreadErrorToString(error));
    }
}

static void processNetifLinkEvent(otInstance *aInstance, struct nlmsghdr *aNetlinkMessage)
{
    struct ifinfomsg *ifinfo = reinterpret_cast<struct ifinfomsg *>(NLMSG_DATA(aNetlinkMessage));
    otError           error  = OT_ERROR_NONE;
    bool              isUp;

    VerifyOrExit(ifinfo->ifi_index == static_cast<int>(gNetifIndex) && (ifinfo->ifi_change & IFF_UP));

    isUp = ((ifinfo->ifi_flags & IFF_UP) != 0);

    otLogInfoPlat("[netif] Host netif is %s", isUp ? "up" : "down");

#if defined(RTM_NEWLINK) && defined(RTM_DELLINK)
    if (sIsSyncingState)
    {
        VerifyOrExit(isUp == otIp6IsEnabled(aInstance),
                     otLogWarnPlat("[netif] Host netif state notification is unexpected (ignore)"));
        sIsSyncingState = false;
    }
    else
#endif
        if (isUp != otIp6IsEnabled(aInstance))
    {
        SuccessOrExit(error = otIp6SetEnabled(aInstance, isUp));
        otLogInfoPlat("[netif] Succeeded to sync netif state with host");
    }

exit:
    if (error != OT_ERROR_NONE)
    {
        otLogWarnPlat("[netif] Failed to sync netif state with host: %s", otThreadErrorToString(error));
    }
}
#endif

#if defined(__APPLE__) || defined(__NetBSD__) || defined(__FreeBSD__)

#if defined(__FreeBSD__)
#define ROUNDUP(a) ((a) > 0 ? (1 + (((a)-1) | (sizeof(uint32_t) - 1))) : sizeof(uint32_t))
#endif

#if defined(__APPLE__)
#define ROUNDUP(a) ((a) > 0 ? (1 + (((a)-1) | (sizeof(uint32_t) - 1))) : sizeof(uint32_t))
#define DARWIN_SA_SIZE(sa) ROUNDUP(sa->sa_len)
#define SA_SIZE(sa) DARWIN_SA_SIZE(sa)
#endif

#if defined(__NetBSD__)
#define RT_ROUNDUP2(a, n) ((a) > 0 ? (1 + (((a)-1U) | ((n)-1))) : (n))
#define RT_ROUNDUP(a) RT_ROUNDUP2((a), sizeof(uint64_t))
#define SA_SIZE(sa) RT_ROUNDUP(sa->sa_len)
#endif

static void processNetifAddrEvent(otInstance *aInstance, struct rt_msghdr *rtm)
{
    otError            error;
    struct ifa_msghdr *ifam;
#ifdef RTM_NEWMADDR
    struct ifma_msghdr *ifmam;
#endif
    struct sockaddr_in6 addr6;
    struct sockaddr_in6 netmask;
    uint8_t *           addrbuf;
    unsigned int        addrlen;
    unsigned int        addrmask = 0;
    unsigned int        i;
    struct sockaddr *   sa;
    bool                is_link_local;
    size_t              buffer_len = rtm->rtm_msglen;

    addr6.sin6_family   = 0;
    netmask.sin6_family = 0;

    if ((rtm->rtm_type == RTM_NEWADDR) || (rtm->rtm_type == RTM_DELADDR))
    {
        ifam = reinterpret_cast<struct ifa_msghdr *>(rtm);

        VerifyOrExit(ifam->ifam_index == static_cast<unsigned int>(gNetifIndex));

        addrbuf  = (uint8_t *)&ifam[1];
        addrmask = (unsigned int)ifam->ifam_addrs;
    }
#ifdef RTM_NEWMADDR
    else if ((rtm->rtm_type == RTM_NEWMADDR) || (rtm->rtm_type == RTM_DELMADDR))
    {
        ifmam = reinterpret_cast<struct ifma_msghdr *>(rtm);

        VerifyOrExit(ifmam->ifmam_index == static_cast<unsigned int>(gNetifIndex));

        addrbuf  = (uint8_t *)&ifmam[1];
        addrmask = (unsigned int)ifmam->ifmam_addrs;
    }
#endif
    addrlen = (unsigned int)buffer_len;

    if (addrmask != 0)
    {
        for (i = 0; i < RTAX_MAX; i++)
        {
            unsigned int mask = (addrmask & (1 << i));
            if (mask)
            {
                sa = (struct sockaddr *)addrbuf;

                if (sa->sa_family == AF_INET6)
                {
                    if (i == RTAX_IFA)
                        memcpy(&addr6, sa, sizeof(sockaddr_in6));
                    if (i == RTAX_NETMASK)
                        memcpy(&netmask, sa, sizeof(sockaddr_in6));
                }
                addrlen -= SA_SIZE(sa);
                addrbuf += SA_SIZE(sa);
            }
        }
    }

    if (addr6.sin6_family == AF_INET6)
    {
        is_link_local = false;
        if (IN6_IS_ADDR_LINKLOCAL(&addr6.sin6_addr))
        {
            is_link_local = true;
            // clear the scope -- Mac OS X sends this to us (bozos!)
            addr6.sin6_addr.s6_addr[3] = 0;
        }
        else if (IN6_IS_ADDR_MC_LINKLOCAL(&addr6.sin6_addr))
        {
            addr6.sin6_addr.s6_addr[3] = 0;
        }

        ot::Ip6::Address addr;
        memcpy(&addr, &addr6.sin6_addr, sizeof(addr));

        if (rtm->rtm_type == RTM_NEWADDR
#ifdef RTM_NEWMADDR
            || rtm->rtm_type == RTM_NEWMADDR
#endif
        )
        {
            if (!addr.IsMulticast())
            {
                otNetifAddress netAddr;
                bool           subscribed;

                netAddr.mAddress      = addr;
                netAddr.mPrefixLength = NetmaskToPrefixLength(&netmask);

                subscribed = UnicastAddressIsSubscribed(aInstance, &netAddr);

                if (subscribed)
                {
                    logAddrEvent(/* isAdd */ true, /* isUnicast */ true, addr6, OT_ERROR_ALREADY);
                    error = OT_ERROR_NONE;
                }
                else
                {
                    if (is_link_local)
                    {
                        // remove the stack-added link-local address

                        int                 err;
                        struct in6_aliasreq ifr6;
                        char                addressString[INET6_ADDRSTRLEN + 1];

                        OT_UNUSED_VARIABLE(addressString); // if otLog*Plat is disabled, we'll get a warning

                        memset(&ifr6, 0, sizeof(ifr6));
                        strlcpy(ifr6.ifra_name, gNetifName, sizeof(ifr6.ifra_name));
                        ifr6.ifra_addr.sin6_family = AF_INET6;
                        ifr6.ifra_addr.sin6_len    = sizeof(ifr6.ifra_addr);
                        memcpy(&ifr6.ifra_addr.sin6_addr, &addr6.sin6_addr, sizeof(struct in6_addr));
                        ifr6.ifra_prefixmask.sin6_family = AF_INET6;
                        ifr6.ifra_prefixmask.sin6_len    = sizeof(ifr6.ifra_prefixmask);
                        InitNetaskWithPrefixLength(&ifr6.ifra_prefixmask.sin6_addr, netAddr.mPrefixLength);
                        ifr6.ifra_lifetime.ia6t_vltime = ND6_INFINITE_LIFETIME;
                        ifr6.ifra_lifetime.ia6t_pltime = ND6_INFINITE_LIFETIME;

#if defined(__APPLE__)
                        ifr6.ifra_lifetime.ia6t_expire    = ND6_INFINITE_LIFETIME;
                        ifr6.ifra_lifetime.ia6t_preferred = ND6_INFINITE_LIFETIME;
#endif

                        err = ioctl(sIpFd, SIOCDIFADDR_IN6, &ifr6);
                        if (err != 0)
                        {
                            otLogWarnPlat(
                                "[netif] Error (%d) removing stack-addded link-local address %s", errno,
                                inet_ntop(AF_INET6, addr6.sin6_addr.s6_addr, addressString, sizeof(addressString)));
                            error = OT_ERROR_FAILED;
                        }
                        else
                        {
                            otLogNotePlat(
                                "[netif]        %s (removed stack-added link-local)",
                                inet_ntop(AF_INET6, addr6.sin6_addr.s6_addr, addressString, sizeof(addressString)));
                            error = OT_ERROR_NONE;
                        }
                    }
                    else
                    {
                        error = otIp6AddUnicastAddress(aInstance, &netAddr);
                        logAddrEvent(/* isAdd */ true, /* isUnicast */ true, addr6, error);
                        if (error == OT_ERROR_ALREADY)
                        {
                            error = OT_ERROR_NONE;
                        }
                    }
                }
                SuccessOrExit(error);
            }
            else
            {
                otNetifMulticastAddress netAddr;
                netAddr.mAddress = addr;

                error = otIp6SubscribeMulticastAddress(aInstance, &addr);
                logAddrEvent(/* isAdd */ true, /* isUnicast */ false, addr6, error);
                if (error == OT_ERROR_ALREADY)
                {
                    error = OT_ERROR_NONE;
                }
                SuccessOrExit(error);
            }
        }
        else if (rtm->rtm_type == RTM_DELADDR
#ifdef RTM_DELMADDR
                 || rtm->rtm_type == RTM_DELMADDR
#endif
        )
        {
            if (!addr.IsMulticast())
            {
                error = otIp6RemoveUnicastAddress(aInstance, &addr);
                logAddrEvent(/* isAdd */ false, /* isUnicast */ true, addr6, error);
                if (error == OT_ERROR_NOT_FOUND)
                {
                    error = OT_ERROR_NONE;
                }
            }
            else
            {
                error = otIp6UnsubscribeMulticastAddress(aInstance, &addr);
                logAddrEvent(/* isAdd */ false, /* isUnicast */ false, addr6, error);
                if (error == OT_ERROR_NOT_FOUND)
                {
                    error = OT_ERROR_NONE;
                }
            }
            SuccessOrExit(error);
        }
    }

exit:;
}

static void processNetifInfoEvent(otInstance *aInstance, struct rt_msghdr *rtm)
{
    struct if_msghdr *ifm   = reinterpret_cast<struct if_msghdr *>(rtm);
    otError           error = OT_ERROR_NONE;

    VerifyOrExit(ifm->ifm_index == static_cast<int>(gNetifIndex));

    UpdateLink(aInstance);

exit:
    if (error != OT_ERROR_NONE)
    {
        otLogWarnPlat("[netif] Failed to process info event: %s", otThreadErrorToString(error));
    }
}

#endif

static void processNetlinkEvent(otInstance *aInstance)
{
    const size_t kMaxNetifEvent = 8192;
    ssize_t      length;

    union
    {
#if defined(__linux__)
        nlmsghdr nlMsg;
#else
        rt_msghdr rtMsg;
#endif
        char buffer[kMaxNetifEvent];
    } msgBuffer;

    length = recv(sNetlinkFd, msgBuffer.buffer, sizeof(msgBuffer.buffer), 0);

    VerifyOrExit(length > 0);

#if defined(__linux__)
    for (struct nlmsghdr *msg = &msgBuffer.nlMsg; NLMSG_OK(msg, static_cast<size_t>(length));
         msg                  = NLMSG_NEXT(msg, length))
    {
#else
    {
        // BSD sends one message per read to routing socket (see route.c, monitor command)
        struct rt_msghdr *msg;

        msg = &msgBuffer.rtMsg;

#define nlmsg_type rtm_type

#endif
        switch (msg->nlmsg_type)
        {
        case RTM_NEWADDR:
        case RTM_DELADDR:
            processNetifAddrEvent(aInstance, msg);
            break;

#if defined(RTM_NEWLINK) && defined(RTM_DELLINK)
        case RTM_NEWLINK:
        case RTM_DELLINK:
            processNetifLinkEvent(aInstance, msg);
            break;
#endif

#if defined(RTM_NEWMADDR) && defined(RTM_DELMADDR)
        case RTM_NEWMADDR:
        case RTM_DELMADDR:
            processNetifAddrEvent(aInstance, msg);
            break;
#endif

#if !defined(__linux__)
        case RTM_IFINFO:
            processNetifInfoEvent(aInstance, msg);
            break;

#else
        case NLMSG_ERROR:
        {
            const struct nlmsgerr *err = reinterpret_cast<const nlmsgerr *>(NLMSG_DATA(msg));

            if (err->error == 0)
            {
                otLogInfoPlat("[netif] Succeeded to process request#%u", err->msg.nlmsg_seq);
            }
            else
            {
                otLogWarnPlat("[netif] Failed to process request#%u: %s", err->msg.nlmsg_seq, strerror(err->error));
            }

            break;
        }
#endif

#if defined(ROUTE_FILTER) || defined(RO_MSGFILTER) || defined(__linux__)
        default:
            otLogWarnPlat("[netif] Unhandled/Unexpected netlink/route message (%d).", (int)msg->nlmsg_type);
            break;
#else
        // this platform doesn't support filtering, so we expect messages of other types...we just ignore them
#endif
        }
    }

exit:
    return;
}

#if OPENTHREAD_POSIX_USE_MLD_MONITOR
static void mldListenerInit(void)
{
    struct ipv6_mreq mreq6;

    sMLDMonitorFd          = SocketWithCloseExec(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6, kSocketNonBlock);
    mreq6.ipv6mr_interface = gNetifIndex;
    memcpy(&mreq6.ipv6mr_multiaddr, kMLDv2MulticastAddress.mFields.m8, sizeof(kMLDv2MulticastAddress.mFields.m8));

    VerifyOrDie(setsockopt(sMLDMonitorFd, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq6, sizeof(mreq6)) == 0, OT_EXIT_FAILURE);
#if defined(__linux__)
    VerifyOrDie(setsockopt(sMLDMonitorFd, SOL_SOCKET, SO_BINDTODEVICE, gNetifName,
                           static_cast<socklen_t>(strnlen(gNetifName, IFNAMSIZ))) == 0,
                OT_EXIT_FAILURE);
#endif
}

static void processMLDEvent(otInstance *aInstance)
{
    const size_t        kMaxMLDEvent = 8192;
    uint8_t             buffer[kMaxMLDEvent];
    ssize_t             bufferLen = -1;
    struct sockaddr_in6 srcAddr;
    socklen_t           addrLen  = sizeof(srcAddr);
    bool                fromSelf = false;
    MLDv2Header *       hdr      = reinterpret_cast<MLDv2Header *>(buffer);
    size_t              offset;
    uint8_t             type;
    struct ifaddrs *    ifAddrs = nullptr;
    char                addressString[INET6_ADDRSTRLEN + 1];

    bufferLen = recvfrom(sMLDMonitorFd, buffer, sizeof(buffer), 0, reinterpret_cast<sockaddr *>(&srcAddr), &addrLen);
    VerifyOrExit(bufferLen > 0);

    type = buffer[0];
    VerifyOrExit(type == kICMPv6MLDv2Type && bufferLen >= static_cast<ssize_t>(sizeof(MLDv2Header)));

    // Check whether it is sent by self
    VerifyOrExit(getifaddrs(&ifAddrs) == 0);
    for (struct ifaddrs *ifAddr = ifAddrs; ifAddr != nullptr; ifAddr = ifAddr->ifa_next)
    {
        if (ifAddr->ifa_addr != nullptr && ifAddr->ifa_addr->sa_family == AF_INET6 &&
            strncmp(gNetifName, ifAddr->ifa_name, IFNAMSIZ) == 0)
        {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
            struct sockaddr_in6 *addr6 = reinterpret_cast<struct sockaddr_in6 *>(ifAddr->ifa_addr);
#pragma GCC diagnostic pop

            if (memcmp(&addr6->sin6_addr, &srcAddr.sin6_addr, sizeof(in6_addr)) == 0)
            {
                fromSelf = true;
                break;
            }
        }
    }
    VerifyOrExit(fromSelf);

    hdr    = reinterpret_cast<MLDv2Header *>(buffer);
    offset = sizeof(MLDv2Header);

    for (size_t i = 0; i < ntohs(hdr->mNumRecords) && offset < static_cast<size_t>(bufferLen); i++)
    {
        if (static_cast<size_t>(bufferLen) >= (sizeof(MLDv2Record) + offset))
        {
            MLDv2Record *record = reinterpret_cast<MLDv2Record *>(&buffer[offset]);

            otError      err;
            otIp6Address address;

            memcpy(&address.mFields.m8, &record->mMulticastAddress, sizeof(address.mFields.m8));
            inet_ntop(AF_INET6, &record->mMulticastAddress, addressString, sizeof(addressString));
            if (record->mRecordType == kICMPv6MLDv2RecordChangeToIncludeType)
            {
                err = otIp6SubscribeMulticastAddress(aInstance, &address);
                if (err == OT_ERROR_ALREADY)
                {
                    otLogNotePlat(
                        "[netif] Will not subscribe duplicate multicast address %s",
                        inet_ntop(AF_INET6, &record->mMulticastAddress, addressString, sizeof(addressString)));
                }
                else if (err != OT_ERROR_NONE)
                {
                    otLogWarnPlat("[netif] Failed to subscribe multicast address %s: %s", addressString,
                                  otThreadErrorToString(err));
                }
                else
                {
                    otLogDebgPlat("[netif] Subscribed multicast address %s", addressString);
                }
            }
            else if (record->mRecordType == kICMPv6MLDv2RecordChangeToExcludeType)
            {
                err = otIp6UnsubscribeMulticastAddress(aInstance, &address);
                if (err != OT_ERROR_NONE)
                {
                    otLogWarnPlat("[netif] Failed to unsubscribe multicast address %s: %s", addressString,
                                  otThreadErrorToString(err));
                }
                else
                {
                    otLogDebgPlat("[netif] Unsubscribed multicast address %s", addressString);
                }
            }

            offset += sizeof(MLDv2Record) + sizeof(in6_addr) * ntohs(record->mNumSources);
        }
    }

exit:
    if (ifAddrs)
    {
        freeifaddrs(ifAddrs);
    }

    return;
}
#endif

#if defined(__linux__)
// set up the tun device
static void platformConfigureTunDevice(const char *aInterfaceName, char *deviceName, size_t deviceNameLen)
{
    struct ifreq ifr;

    sTunFd = open(OPENTHREAD_POSIX_TUN_DEVICE, O_RDWR | O_CLOEXEC | O_NONBLOCK);
    VerifyOrDie(sTunFd >= 0, OT_EXIT_ERROR_ERRNO);

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI | static_cast<short>(IFF_TUN_EXCL);

    if (aInterfaceName)
    {
        VerifyOrDie(strlen(aInterfaceName) < IFNAMSIZ, OT_EXIT_INVALID_ARGUMENTS);

        strncpy(ifr.ifr_name, aInterfaceName, IFNAMSIZ);
    }
    else
    {
        strncpy(ifr.ifr_name, "wpan%d", IFNAMSIZ);
    }

    VerifyOrDie(ioctl(sTunFd, TUNSETIFF, static_cast<void *>(&ifr)) == 0, OT_EXIT_ERROR_ERRNO);
    VerifyOrDie(ioctl(sTunFd, TUNSETLINK, ARPHRD_VOID) == 0, OT_EXIT_ERROR_ERRNO);

    strncpy(deviceName, ifr.ifr_name, deviceNameLen);

    ifr.ifr_mtu = static_cast<int>(kMaxIp6Size);
    VerifyOrDie(ioctl(sIpFd, SIOCSIFMTU, static_cast<void *>(&ifr)) == 0, OT_EXIT_ERROR_ERRNO);
}
#endif

#if defined(__APPLE__) && (OPENTHREAD_POSIX_CONFIG_MACOS_TUN_OPTION == OT_POSIX_CONFIG_MACOS_UTUN)
static void platformConfigureTunDevice(const char *aInterfaceName, char *deviceName, size_t deviceNameLen)
{
    (void)aInterfaceName;
    int                 err = 0;
    struct sockaddr_ctl addr;
    struct ctl_info     info;

    sTunFd = SocketWithCloseExec(PF_SYSTEM, SOCK_DGRAM, SYSPROTO_CONTROL, kSocketNonBlock);
    VerifyOrDie(sTunFd >= 0, OT_EXIT_ERROR_ERRNO);

    memset(&info, 0, sizeof(info));
    strncpy(info.ctl_name, UTUN_CONTROL_NAME, strlen(UTUN_CONTROL_NAME));
    err = ioctl(sTunFd, CTLIOCGINFO, &info);
    VerifyOrDie(err == 0, OT_EXIT_ERROR_ERRNO);

    addr.sc_id      = info.ctl_id;
    addr.sc_len     = sizeof(addr);
    addr.sc_family  = AF_SYSTEM;
    addr.ss_sysaddr = AF_SYS_CONTROL;

    addr.sc_unit = 0;
    err          = connect(sTunFd, (struct sockaddr *)&addr, sizeof(addr));
    VerifyOrDie(err == 0, OT_EXIT_ERROR_ERRNO);

    socklen_t devNameLen;
    devNameLen = (socklen_t)deviceNameLen;
    err        = getsockopt(sTunFd, SYSPROTO_CONTROL, UTUN_OPT_IFNAME, deviceName, &devNameLen);
    VerifyOrDie(err == 0, OT_EXIT_ERROR_ERRNO);

    otLogInfoPlat("[netif] Tunnel device name = '%s'", deviceName);
}
#endif

#if defined(__NetBSD__) || defined(__FreeBSD__)
static otError destroyTunnel(void)
{
    otError      error;
    struct ifreq ifr;

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, gNetifName, sizeof(ifr.ifr_name));
    VerifyOrExit(ioctl(sIpFd, SIOCIFDESTROY, &ifr) == 0, perror("ioctl"); error = OT_ERROR_FAILED);
    error = OT_ERROR_NONE;

exit:
    return error;
}
#endif

#if defined(__NetBSD__) ||                                                                             \
    (defined(__APPLE__) && (OPENTHREAD_POSIX_CONFIG_MACOS_TUN_OPTION == OT_POSIX_CONFIG_MACOS_TUN)) || \
    defined(__FreeBSD__)
static void platformConfigureTunDevice(const char *aInterfaceName, char *deviceName, size_t deviceNameLen)
{
    int         flags = IFF_BROADCAST | IFF_MULTICAST;
    int         err;
    const char *last_slash;
    const char *path;

    (void)aInterfaceName;

    path = OPENTHREAD_POSIX_TUN_DEVICE;

    sTunFd = open(path, O_RDWR | O_NONBLOCK);
    VerifyOrDie(sTunFd >= 0, OT_EXIT_ERROR_ERRNO);

#if defined(__NetBSD__) || defined(__FreeBSD__)
    err = ioctl(sTunFd, TUNSIFMODE, &flags);
    VerifyOrDie(err == 0, OT_EXIT_ERROR_ERRNO);
#endif

    flags = 1;
    err   = ioctl(sTunFd, TUNSIFHEAD, &flags);
    VerifyOrDie(err == 0, OT_EXIT_ERROR_ERRNO);

    last_slash = strrchr(OPENTHREAD_POSIX_TUN_DEVICE, '/');
    VerifyOrDie(last_slash != nullptr, OT_EXIT_ERROR_ERRNO);
    last_slash++;

    strncpy(deviceName, last_slash, deviceNameLen);
}
#endif

static void platformConfigureNetLink(void)
{
#if defined(__linux__)
    sNetlinkFd = SocketWithCloseExec(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE, kSocketNonBlock);
#elif defined(__APPLE__) || defined(__NetBSD__) || defined(__FreeBSD__)
    sNetlinkFd = SocketWithCloseExec(PF_ROUTE, SOCK_RAW, 0, kSocketNonBlock);
#else
#error "!! Unknown platform !!"
#endif
    VerifyOrDie(sNetlinkFd >= 0, OT_EXIT_ERROR_ERRNO);

#if defined(__linux__)
    {
        struct sockaddr_nl sa;

        memset(&sa, 0, sizeof(sa));
        sa.nl_family = AF_NETLINK;
        sa.nl_groups = RTMGRP_LINK | RTMGRP_IPV6_IFADDR;
        VerifyOrDie(bind(sNetlinkFd, reinterpret_cast<struct sockaddr *>(&sa), sizeof(sa)) == 0, OT_EXIT_ERROR_ERRNO);
    }
#endif

#if defined(__APPLE__) || defined(__NetBSD__) || defined(__FreeBSD__)
    {
        int status;
#ifdef ROUTE_FILTER
        unsigned int msgfilter = ROUTE_FILTER(RTM_IFINFO) | ROUTE_FILTER(RTM_NEWADDR) | ROUTE_FILTER(RTM_DELADDR) |
                                 ROUTE_FILTER(RTM_NEWMADDR) | ROUTE_FILTER(RTM_DELMADDR);
#define FILTER_CMD ROUTE_MSGFILTER
#define FILTER_ARG msgfilter
#define FILTER_ARG_SZ sizeof(msgfilter)
#endif
#ifdef RO_MSGFILTER
        uint8_t msgfilter[] = {RTM_IFINFO, RTM_NEWADDR, RTM_DELADDR};
#define FILTER_CMD RO_MSGFILTER
#define FILTER_ARG msgfilter
#define FILTER_ARG_SZ sizeof(msgfilter)
#endif
#if defined(ROUTE_FILTER) || defined(RO_MSGFILTER)
        status = setsockopt(sNetlinkFd, AF_ROUTE, FILTER_CMD, FILTER_ARG, FILTER_ARG_SZ);
        VerifyOrDie(status == 0, OT_EXIT_ERROR_ERRNO);
#endif
        status = fcntl(sNetlinkFd, F_SETFL, O_NONBLOCK);
        VerifyOrDie(status == 0, OT_EXIT_ERROR_ERRNO);
    }
#endif // defined(__APPLE__) || defined(__NetBSD__) || defined(__FreeBSD__)
}

void platformNetifInit(const char *aInterfaceName)
{
    sIpFd = SocketWithCloseExec(AF_INET6, SOCK_DGRAM, IPPROTO_IP, kSocketNonBlock);
    VerifyOrDie(sIpFd >= 0, OT_EXIT_ERROR_ERRNO);

    platformConfigureNetLink();
    platformConfigureTunDevice(aInterfaceName, gNetifName, sizeof(gNetifName));

    gNetifIndex = if_nametoindex(gNetifName);
    VerifyOrDie(gNetifIndex > 0, OT_EXIT_FAILURE);

#if OPENTHREAD_POSIX_USE_MLD_MONITOR
    mldListenerInit();
#endif
}

void platformNetifSetUp(void)
{
    OT_ASSERT(gInstance != nullptr);

    otIp6SetReceiveFilterEnabled(gInstance, true);
    otIcmp6SetEchoMode(gInstance, OT_ICMP6_ECHO_HANDLER_DISABLED);
    otIp6SetReceiveCallback(gInstance, processReceive, gInstance);
    otIp6SetAddressCallback(gInstance, processAddressChange, gInstance);
#if OPENTHREAD_POSIX_MULTICAST_PROMISCUOUS_REQUIRED
    otIp6SetMulticastPromiscuousEnabled(aInstance, true);
#endif
}

void platformNetifTearDown(void)
{
}

void platformNetifDeinit(void)
{
    if (sTunFd != -1)
    {
        close(sTunFd);
        sTunFd = -1;

#if defined(__NetBSD__) || defined(__FreeBSD__)
        destroyTunnel();
#endif
    }

    if (sIpFd != -1)
    {
        close(sIpFd);
        sIpFd = -1;
    }

    if (sNetlinkFd != -1)
    {
        close(sNetlinkFd);
        sNetlinkFd = -1;
    }

#if OPENTHREAD_POSIX_USE_MLD_MONITOR
    if (sMLDMonitorFd != -1)
    {
        close(sMLDMonitorFd);
        sMLDMonitorFd = -1;
    }
#endif

    gNetifIndex = 0;
}

void platformNetifUpdateFdSet(fd_set *aReadFdSet, fd_set *aWriteFdSet, fd_set *aErrorFdSet, int *aMaxFd)
{
    OT_UNUSED_VARIABLE(aWriteFdSet);

    VerifyOrExit(gNetifIndex > 0);

    assert(sTunFd >= 0);
    assert(sNetlinkFd >= 0);
    assert(sIpFd >= 0);

    FD_SET(sTunFd, aReadFdSet);
    FD_SET(sTunFd, aErrorFdSet);
    FD_SET(sNetlinkFd, aReadFdSet);
    FD_SET(sNetlinkFd, aErrorFdSet);
#if OPENTHREAD_POSIX_USE_MLD_MONITOR
    FD_SET(sMLDMonitorFd, aReadFdSet);
    FD_SET(sMLDMonitorFd, aErrorFdSet);
#endif

    if (sTunFd > *aMaxFd)
    {
        *aMaxFd = sTunFd;
    }

    if (sNetlinkFd > *aMaxFd)
    {
        *aMaxFd = sNetlinkFd;
    }

#if OPENTHREAD_POSIX_USE_MLD_MONITOR
    if (sMLDMonitorFd > *aMaxFd)
    {
        *aMaxFd = sMLDMonitorFd;
    }
#endif
exit:
    return;
}

void platformNetifProcess(const fd_set *aReadFdSet, const fd_set *aWriteFdSet, const fd_set *aErrorFdSet)
{
    OT_UNUSED_VARIABLE(aWriteFdSet);
    VerifyOrExit(gNetifIndex > 0);

    if (FD_ISSET(sTunFd, aErrorFdSet))
    {
        close(sTunFd);
        DieNow(OT_EXIT_FAILURE);
    }

    if (FD_ISSET(sNetlinkFd, aErrorFdSet))
    {
        close(sNetlinkFd);
        DieNow(OT_EXIT_FAILURE);
    }

#if OPENTHREAD_POSIX_USE_MLD_MONITOR
    if (FD_ISSET(sMLDMonitorFd, aErrorFdSet))
    {
        close(sMLDMonitorFd);
        DieNow(OT_EXIT_FAILURE);
    }
#endif

    if (FD_ISSET(sTunFd, aReadFdSet))
    {
        processTransmit(gInstance);
    }

    if (FD_ISSET(sNetlinkFd, aReadFdSet))
    {
        processNetlinkEvent(gInstance);
    }

#if OPENTHREAD_POSIX_USE_MLD_MONITOR
    if (FD_ISSET(sMLDMonitorFd, aReadFdSet))
    {
        processMLDEvent(gInstance);
    }
#endif

exit:
    return;
}

#endif // OPENTHREAD_CONFIG_PLATFORM_NETIF_ENABLE
