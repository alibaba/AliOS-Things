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

#include "posix/platform/multicast_routing.hpp"

#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE

#include <assert.h>
#include <net/if.h>
#include <netinet/icmp6.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#if __linux__
#include <linux/mroute6.h>
#else
#error "Multicast Routing feature is not ported to non-Linux platforms yet."
#endif

#include <openthread/backbone_router_ftd.h>

#include "core/common/debug.hpp"
#include "core/common/logging.hpp"

namespace ot {
namespace Posix {

void MulticastRoutingManager::SetUp(void)
{
    OT_ASSERT(gInstance != nullptr);

    otBackboneRouterSetMulticastListenerCallback(gInstance,
                                                 &MulticastRoutingManager::HandleBackboneMulticastListenerEvent, this);
    Mainloop::Manager::Get().Add(*this);
}

void MulticastRoutingManager::TearDown(void)
{
    OT_ASSERT(gInstance != nullptr);

    otBackboneRouterSetMulticastListenerCallback(gInstance, nullptr, nullptr);
    Mainloop::Manager::Get().Remove(*this);
}

void MulticastRoutingManager::HandleBackboneMulticastListenerEvent(void *                                 aContext,
                                                                   otBackboneRouterMulticastListenerEvent aEvent,
                                                                   const otIp6Address *                   aAddress)
{
    static_cast<MulticastRoutingManager *>(aContext)->HandleBackboneMulticastListenerEvent(
        aEvent, static_cast<const Ip6::Address &>(*aAddress));
}

void MulticastRoutingManager::HandleBackboneMulticastListenerEvent(otBackboneRouterMulticastListenerEvent aEvent,
                                                                   const Ip6::Address &                   aAddress)
{
    switch (aEvent)
    {
    case OT_BACKBONE_ROUTER_MULTICAST_LISTENER_ADDED:
        Add(aAddress);
        break;
    case OT_BACKBONE_ROUTER_MULTICAST_LISTENER_REMOVED:
        Remove(aAddress);
        break;
    }
}

void MulticastRoutingManager::Enable(void)
{
    VerifyOrExit(!IsEnabled());

    InitMulticastRouterSock();

    otLogResultPlat(OT_ERROR_NONE, "MulticastRoutingManager: %s", __FUNCTION__);
exit:
    return;
}

void MulticastRoutingManager::Disable(void)
{
    FinalizeMulticastRouterSock();

    otLogResultPlat(OT_ERROR_NONE, "MulticastRoutingManager: %s", __FUNCTION__);
}

void MulticastRoutingManager::Add(const Ip6::Address &aAddress)
{
    VerifyOrExit(IsEnabled());

    UnblockInboundMulticastForwardingCache(aAddress);
    otLogResultPlat(OT_ERROR_NONE, "MulticastRoutingManager: %s: %s", __FUNCTION__, aAddress.ToString().AsCString());

exit:
    return;
}

void MulticastRoutingManager::Remove(const Ip6::Address &aAddress)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(IsEnabled());

    RemoveInboundMulticastForwardingCache(aAddress);
    otLogResultPlat(error, "MulticastRoutingManager: %s: %s", __FUNCTION__, aAddress.ToString().AsCString());

exit:
    return;
}

bool MulticastRoutingManager::HasMulticastListener(const Ip6::Address &aAddress) const
{
    bool                                      found = false;
    otBackboneRouterMulticastListenerIterator iter  = OT_BACKBONE_ROUTER_MULTICAST_LISTENER_ITERATOR_INIT;
    otBackboneRouterMulticastListenerInfo     listenerInfo;

    while (otBackboneRouterMulticastListenerGetNext(gInstance, &iter, &listenerInfo) == OT_ERROR_NONE)
    {
        VerifyOrExit(static_cast<const Ip6::Address &>(listenerInfo.mAddress) != aAddress, found = true);
    }

exit:
    return found;
}

void MulticastRoutingManager::Update(otSysMainloopContext &aContext)
{
    VerifyOrExit(IsEnabled());

    FD_SET(mMulticastRouterSock, &aContext.mReadFdSet);
    aContext.mMaxFd = OT_MAX(aContext.mMaxFd, mMulticastRouterSock);

exit:
    return;
}

void MulticastRoutingManager::Process(const otSysMainloopContext &aContext)
{
    VerifyOrExit(IsEnabled());

    ExpireMulticastForwardingCache();

    if (FD_ISSET(mMulticastRouterSock, &aContext.mReadFdSet))
    {
        ProcessMulticastRouterMessages();
    }

exit:
    return;
}

void MulticastRoutingManager::InitMulticastRouterSock(void)
{
    int                 one = 1;
    struct icmp6_filter filter;
    struct mif6ctl      mif6ctl;

    // Create a Multicast Routing socket
    mMulticastRouterSock = SocketWithCloseExec(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6, kSocketBlock);
    VerifyOrDie(mMulticastRouterSock != -1, OT_EXIT_ERROR_ERRNO);

    // Enable Multicast Forwarding in Kernel
    VerifyOrDie(0 == setsockopt(mMulticastRouterSock, IPPROTO_IPV6, MRT6_INIT, &one, sizeof(one)), OT_EXIT_ERROR_ERRNO);

    // Filter all ICMPv6 messages
    ICMP6_FILTER_SETBLOCKALL(&filter);
    VerifyOrDie(0 == setsockopt(mMulticastRouterSock, IPPROTO_ICMPV6, ICMP6_FILTER, (void *)&filter, sizeof(filter)),
                OT_EXIT_ERROR_ERRNO);

    memset(&mif6ctl, 0, sizeof(mif6ctl));
    mif6ctl.mif6c_flags     = 0;
    mif6ctl.vifc_threshold  = 1;
    mif6ctl.vifc_rate_limit = 0;

    // Add Thread network interface to MIF
    mif6ctl.mif6c_mifi = kMifIndexThread;
    mif6ctl.mif6c_pifi = if_nametoindex(gNetifName);
    VerifyOrDie(mif6ctl.mif6c_pifi > 0, OT_EXIT_ERROR_ERRNO);
    VerifyOrDie(0 == setsockopt(mMulticastRouterSock, IPPROTO_IPV6, MRT6_ADD_MIF, &mif6ctl, sizeof(mif6ctl)),
                OT_EXIT_ERROR_ERRNO);

    // Add Backbone network interface to MIF
    mif6ctl.mif6c_mifi = kMifIndexBackbone;
    mif6ctl.mif6c_pifi = if_nametoindex(gBackboneNetifName);
    VerifyOrDie(mif6ctl.mif6c_pifi > 0, OT_EXIT_ERROR_ERRNO);
    VerifyOrDie(0 == setsockopt(mMulticastRouterSock, IPPROTO_IPV6, MRT6_ADD_MIF, &mif6ctl, sizeof(mif6ctl)),
                OT_EXIT_ERROR_ERRNO);
}

void MulticastRoutingManager::FinalizeMulticastRouterSock(void)
{
    VerifyOrExit(IsEnabled());

    close(mMulticastRouterSock);
    mMulticastRouterSock = -1;

exit:
    return;
}

void MulticastRoutingManager::ProcessMulticastRouterMessages(void)
{
    otError         error = OT_ERROR_NONE;
    char            buf[sizeof(struct mrt6msg)];
    int             nr;
    struct mrt6msg *mrt6msg;
    Ip6::Address    src, dst;

    nr = read(mMulticastRouterSock, buf, sizeof(buf));

    VerifyOrExit(nr >= static_cast<int>(sizeof(struct mrt6msg)), error = OT_ERROR_FAILED);

    mrt6msg = reinterpret_cast<struct mrt6msg *>(buf);

    VerifyOrExit(mrt6msg->im6_mbz == 0);
    VerifyOrExit(mrt6msg->im6_msgtype == MRT6MSG_NOCACHE);

    src.SetBytes(mrt6msg->im6_src.s6_addr);
    dst.SetBytes(mrt6msg->im6_dst.s6_addr);

    error = AddMulticastForwardingCache(src, dst, static_cast<MifIndex>(mrt6msg->im6_mif));

exit:
    otLogResultPlat(error, "MulticastRoutingManager: %s", __FUNCTION__);
}

otError MulticastRoutingManager::AddMulticastForwardingCache(const Ip6::Address &aSrcAddr,
                                                             const Ip6::Address &aGroupAddr,
                                                             MifIndex            aIif)
{
    otError        error = OT_ERROR_NONE;
    struct mf6cctl mf6cctl;
    MifIndex       forwardMif = kMifIndexNone;

    VerifyOrExit(aIif == kMifIndexThread || aIif == kMifIndexBackbone, error = OT_ERROR_INVALID_ARGS);

    ExpireMulticastForwardingCache();

    if (aIif == kMifIndexBackbone)
    {
        // Forward multicast traffic from Backbone to Thread if the group address is subscribed by any Thread device via
        // MLR.
        if (HasMulticastListener(aGroupAddr))
        {
            forwardMif = kMifIndexThread;
        }
    }
    else
    {
        // Forward multicast traffic from Thread to Backbone if multicast scope > kRealmLocalScope
        // TODO: (MLR) allow scope configuration of outbound multicast routing
        if (aGroupAddr.GetScope() > Ip6::Address::kRealmLocalScope)
        {
            forwardMif = kMifIndexBackbone;
        }
    }

    memset(&mf6cctl, 0, sizeof(mf6cctl));

    memcpy(mf6cctl.mf6cc_origin.sin6_addr.s6_addr, aSrcAddr.GetBytes(), sizeof(mf6cctl.mf6cc_origin.sin6_addr.s6_addr));
    memcpy(mf6cctl.mf6cc_mcastgrp.sin6_addr.s6_addr, aGroupAddr.GetBytes(),
           sizeof(mf6cctl.mf6cc_mcastgrp.sin6_addr.s6_addr));
    mf6cctl.mf6cc_parent = aIif;

    if (forwardMif != kMifIndexNone)
    {
        IF_SET(forwardMif, &mf6cctl.mf6cc_ifset);
    }

    // Note that kernel reports repetitive `MRT6MSG_NOCACHE` upcalls with a rate limit (e.g. once per 10s for Linux).
    // Because of it, we need to add a "blocking" MFC even if there is no forwarding for this group address.
    // When a  Multicast Listener is later added, the "blocking" MFC will be altered to be a "forwarding" MFC so that
    // corresponding multicast traffic can be forwarded instantly.
    VerifyOrExit(0 == setsockopt(mMulticastRouterSock, IPPROTO_IPV6, MRT6_ADD_MFC, &mf6cctl, sizeof(mf6cctl)),
                 error = OT_ERROR_FAILED);

    SaveMulticastForwardingCache(aSrcAddr, aGroupAddr, aIif, forwardMif);
exit:
    otLogResultPlat(error, "MulticastRoutingManager: %s: add dynamic route: %s %s => %s %s", __FUNCTION__,
                    MifIndexToString(aIif), aSrcAddr.ToString().AsCString(), aGroupAddr.ToString().AsCString(),
                    MifIndexToString(forwardMif));

    return error;
}

void MulticastRoutingManager::UnblockInboundMulticastForwardingCache(const Ip6::Address &aGroupAddr)
{
    struct mf6cctl mf6cctl;

    memset(&mf6cctl, 0, sizeof(mf6cctl));
    memcpy(mf6cctl.mf6cc_mcastgrp.sin6_addr.s6_addr, aGroupAddr.GetBytes(),
           sizeof(mf6cctl.mf6cc_mcastgrp.sin6_addr.s6_addr));
    mf6cctl.mf6cc_parent = kMifIndexBackbone;
    IF_SET(kMifIndexThread, &mf6cctl.mf6cc_ifset);

    for (MulticastForwardingCache &mfc : mMulticastForwardingCacheTable)
    {
        otError error;

        if (!mfc.IsValid() || mfc.mIif != kMifIndexBackbone || mfc.mOif == kMifIndexThread ||
            mfc.mGroupAddr != aGroupAddr)
        {
            continue;
        }

        // Unblock this inbound route
        memcpy(mf6cctl.mf6cc_origin.sin6_addr.s6_addr, mfc.mSrcAddr.GetBytes(),
               sizeof(mf6cctl.mf6cc_origin.sin6_addr.s6_addr));

        error = (0 == setsockopt(mMulticastRouterSock, IPPROTO_IPV6, MRT6_ADD_MFC, &mf6cctl, sizeof(mf6cctl)))
                    ? OT_ERROR_NONE
                    : OT_ERROR_FAILED;

        mfc.Set(kMifIndexBackbone, kMifIndexThread);

        otLogResultPlat(error, "MulticastRoutingManager: %s: %s %s => %s %s", __FUNCTION__, MifIndexToString(mfc.mIif),
                        mfc.mSrcAddr.ToString().AsCString(), mfc.mGroupAddr.ToString().AsCString(),
                        MifIndexToString(kMifIndexThread));
    }
}

void MulticastRoutingManager::RemoveInboundMulticastForwardingCache(const Ip6::Address &aGroupAddr)
{
    for (MulticastForwardingCache &mfc : mMulticastForwardingCacheTable)
    {
        if (mfc.IsValid() && mfc.mIif == kMifIndexBackbone && mfc.mGroupAddr == aGroupAddr)
        {
            RemoveMulticastForwardingCache(mfc);
        }
    }
}

void MulticastRoutingManager::ExpireMulticastForwardingCache(void)
{
    struct sioc_sg_req6 sioc_sg_req6;
    uint64_t            now = otPlatTimeGet();
    struct mf6cctl      mf6cctl;

    VerifyOrExit(now >= mLastExpireTime + kMulticastForwardingCacheExpiringInterval * US_PER_S);

    mLastExpireTime = now;

    memset(&mf6cctl, 0, sizeof(mf6cctl));
    memset(&sioc_sg_req6, 0, sizeof(sioc_sg_req6));

    for (MulticastForwardingCache &mfc : mMulticastForwardingCacheTable)
    {
        if (mfc.IsValid() && mfc.mLastUseTime + kMulticastForwardingCacheExpireTimeout * US_PER_S < now)
        {
            if (!UpdateMulticastRouteInfo(mfc))
            {
                // The multicast route is expired
                RemoveMulticastForwardingCache(mfc);
            }
        }
    }

    DumpMulticastForwardingCache();

exit:
    return;
}

bool MulticastRoutingManager::UpdateMulticastRouteInfo(MulticastForwardingCache &aMfc) const
{
    bool                updated = false;
    struct sioc_sg_req6 sioc_sg_req6;

    memset(&sioc_sg_req6, 0, sizeof(sioc_sg_req6));

    memcpy(sioc_sg_req6.src.sin6_addr.s6_addr, aMfc.mSrcAddr.GetBytes(), sizeof(sioc_sg_req6.src.sin6_addr.s6_addr));
    memcpy(sioc_sg_req6.grp.sin6_addr.s6_addr, aMfc.mGroupAddr.GetBytes(), sizeof(sioc_sg_req6.grp.sin6_addr.s6_addr));

    if (ioctl(mMulticastRouterSock, SIOCGETSGCNT_IN6, &sioc_sg_req6) != -1)
    {
        unsigned long validPktCnt;

        otLogDebgPlat("MulticastRoutingManager: %s: SIOCGETSGCNT_IN6 %s => %s: bytecnt=%lu, pktcnt=%lu, wrong_if=%lu",
                      __FUNCTION__, aMfc.mSrcAddr.ToString().AsCString(), aMfc.mGroupAddr.ToString().AsCString(),
                      sioc_sg_req6.bytecnt, sioc_sg_req6.pktcnt, sioc_sg_req6.wrong_if);

        validPktCnt = sioc_sg_req6.pktcnt - sioc_sg_req6.wrong_if;
        if (validPktCnt != aMfc.mValidPktCnt)
        {
            aMfc.SetValidPktCnt(validPktCnt);

            updated = true;
        }
    }
    else
    {
        otLogWarnPlat("MulticastRoutingManager: %s: SIOCGETSGCNT_IN6 %s => %s failed: %s", __FUNCTION__,
                      aMfc.mSrcAddr.ToString().AsCString(), aMfc.mGroupAddr.ToString().AsCString(), strerror(errno));
    }

    return updated;
}

const char *MulticastRoutingManager::MifIndexToString(MifIndex aMif)
{
    const char *string = "Unknown";

    switch (aMif)
    {
    case kMifIndexNone:
        string = "None";
        break;
    case kMifIndexThread:
        string = "Thread";
        break;
    case kMifIndexBackbone:
        string = "Backbone";
        break;
    }

    return string;
}

void MulticastRoutingManager::DumpMulticastForwardingCache(void) const
{
#if OPENTHREAD_CONFIG_LOG_PLATFORM && OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_DEBG
    otLogDebgPlat("MulticastRoutingManager: ==================== MFC ENTRIES ====================");

    for (const MulticastForwardingCache &mfc : mMulticastForwardingCacheTable)
    {
        if (mfc.IsValid())
        {
            otLogDebgPlat("MulticastRoutingManager: %s %s => %s %s", MifIndexToString(mfc.mIif),
                          mfc.mSrcAddr.ToString().AsCString(), mfc.mGroupAddr.ToString().AsCString(),
                          MifIndexToString(mfc.mOif));
        }
    }

    otLogDebgPlat("MulticastRoutingManager: =====================================================");
#endif
}

void MulticastRoutingManager::HandleStateChange(otInstance *aInstance, otChangedFlags aFlags)
{
    if (aFlags & OT_CHANGED_THREAD_BACKBONE_ROUTER_STATE)
    {
        otBackboneRouterState state = otBackboneRouterGetState(aInstance);

        switch (state)
        {
        case OT_BACKBONE_ROUTER_STATE_DISABLED:
        case OT_BACKBONE_ROUTER_STATE_SECONDARY:
            Disable();
            break;
        case OT_BACKBONE_ROUTER_STATE_PRIMARY:
            Enable();
            break;
        }
    }
}

void MulticastRoutingManager::MulticastForwardingCache::Set(MulticastRoutingManager::MifIndex aIif,
                                                            MulticastRoutingManager::MifIndex aOif)
{
    mIif         = aIif;
    mOif         = aOif;
    mValidPktCnt = 0;
    mLastUseTime = otPlatTimeGet();
}

void MulticastRoutingManager::MulticastForwardingCache::Set(const Ip6::Address &aSrcAddr,
                                                            const Ip6::Address &aGroupAddr,
                                                            MifIndex            aIif,
                                                            MifIndex            aOif)
{
    mSrcAddr   = aSrcAddr;
    mGroupAddr = aGroupAddr;
    Set(aIif, aOif);
}

void MulticastRoutingManager::MulticastForwardingCache::SetValidPktCnt(unsigned long aValidPktCnt)
{
    mValidPktCnt = aValidPktCnt;
    mLastUseTime = otPlatTimeGet();
}

void MulticastRoutingManager::SaveMulticastForwardingCache(const Ip6::Address &              aSrcAddr,
                                                           const Ip6::Address &              aGroupAddr,
                                                           MulticastRoutingManager::MifIndex aIif,
                                                           MulticastRoutingManager::MifIndex aOif)
{
    MulticastForwardingCache *invalid = nullptr;
    MulticastForwardingCache *oldest  = nullptr;

    for (MulticastForwardingCache &mfc : mMulticastForwardingCacheTable)
    {
        if (mfc.IsValid())
        {
            if (mfc.mSrcAddr == aSrcAddr && mfc.mGroupAddr == aGroupAddr)
            {
                mfc.Set(aIif, aOif);
                ExitNow();
            }

            if (oldest == nullptr || mfc.mLastUseTime < oldest->mLastUseTime)
            {
                oldest = &mfc;
            }
        }
        else if (invalid == nullptr)
        {
            invalid = &mfc;
        }
    }

    if (invalid != nullptr)
    {
        invalid->Set(aSrcAddr, aGroupAddr, aIif, aOif);
    }
    else
    {
        RemoveMulticastForwardingCache(*oldest);
        oldest->Set(aSrcAddr, aGroupAddr, aIif, aOif);
    }

exit:
    return;
}

void MulticastRoutingManager::RemoveMulticastForwardingCache(
    MulticastRoutingManager::MulticastForwardingCache &aMfc) const
{
    otError        error;
    struct mf6cctl mf6cctl;

    memset(&mf6cctl, 0, sizeof(mf6cctl));

    memcpy(mf6cctl.mf6cc_origin.sin6_addr.s6_addr, aMfc.mSrcAddr.GetBytes(),
           sizeof(mf6cctl.mf6cc_origin.sin6_addr.s6_addr));
    memcpy(mf6cctl.mf6cc_mcastgrp.sin6_addr.s6_addr, aMfc.mGroupAddr.GetBytes(),
           sizeof(mf6cctl.mf6cc_mcastgrp.sin6_addr.s6_addr));

    mf6cctl.mf6cc_parent = aMfc.mIif;

    error = (0 == setsockopt(mMulticastRouterSock, IPPROTO_IPV6, MRT6_DEL_MFC, &mf6cctl, sizeof(mf6cctl)))
                ? OT_ERROR_NONE
                : OT_ERROR_FAILED;

    otLogResultPlat(error, "MulticastRoutingManager: %s: %s %s => %s %s", __FUNCTION__, MifIndexToString(aMfc.mIif),
                    aMfc.mSrcAddr.ToString().AsCString(), aMfc.mGroupAddr.ToString().AsCString(),
                    MifIndexToString(aMfc.mOif));

    aMfc.Erase();
}

} // namespace Posix
} // namespace ot

#endif // OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE
