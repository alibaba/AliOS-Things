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
 *   This file implements the Thread network interface.
 */

#include "thread_netif.hpp"

#include "common/code_utils.hpp"
#include "common/encoding.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/message.hpp"
#include "net/ip6.hpp"
#include "net/netif.hpp"
#include "net/udp6.hpp"
#include "thread/mle.hpp"
#include "thread/thread_tlvs.hpp"
#include "thread/uri_paths.hpp"

namespace ot {

ThreadNetif::ThreadNetif(Instance &aInstance)
    : Netif(aInstance)
    , mTmfAgent(aInstance)
#if OPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE
    , mDhcp6Client(aInstance)
#endif
#if OPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE
    , mDhcp6Server(aInstance)
#endif
#if OPENTHREAD_CONFIG_NEIGHBOR_DISCOVERY_AGENT_ENABLE
    , mNeighborDiscoveryAgent(aInstance)
#endif
#if OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE
    , mSlaac(aInstance)
#endif
#if OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
    , mDnsClient(aInstance)
#endif
#if OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
    , mSrpClient(aInstance)
#endif
#if OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_ENABLE
    , mSrpClientBuffers(aInstance)
#endif
#if OPENTHREAD_CONFIG_DNSSD_SERVER_ENABLE
    , mDnssdServer(aInstance)
#endif
#if OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE
    , mSntpClient(aInstance)
#endif
    , mActiveDataset(aInstance)
    , mPendingDataset(aInstance)
    , mIp6Filter(aInstance)
    , mKeyManager(aInstance)
    , mLowpan(aInstance)
    , mMac(aInstance)
    , mMeshForwarder(aInstance)
    , mMleRouter(aInstance)
    , mDiscoverScanner(aInstance)
#if OPENTHREAD_CONFIG_MULTI_RADIO
    , mRadioSelector(aInstance)
#endif
#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE || OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    , mNetworkDataLocal(aInstance)
#endif
    , mNetworkDataLeader(aInstance)
#if OPENTHREAD_FTD || OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE || OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    , mNetworkDataNotifier(aInstance)
#endif
#if OPENTHREAD_CONFIG_NETDATA_PUBLISHER_ENABLE
    , mNetworkDataPublisher(aInstance)
#endif
    , mNetworkDataServiceManager(aInstance)
#if OPENTHREAD_FTD || OPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE
    , mNetworkDiagnostic(aInstance)
#endif
    , mIsUp(false)
#if OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
    , mBorderAgent(aInstance)
#endif
#if OPENTHREAD_CONFIG_COMMISSIONER_ENABLE && OPENTHREAD_FTD
    , mCommissioner(aInstance)
#endif
#if OPENTHREAD_CONFIG_DTLS_ENABLE
    , mCoapSecure(aInstance)
#endif
#if OPENTHREAD_CONFIG_JOINER_ENABLE
    , mJoiner(aInstance)
#endif
#if OPENTHREAD_CONFIG_JAM_DETECTION_ENABLE
    , mJamDetector(aInstance)
#endif
#if OPENTHREAD_FTD
    , mJoinerRouter(aInstance)
    , mLeader(aInstance)
    , mAddressResolver(aInstance)
#endif
#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    , mBackboneRouterLeader(aInstance)
#endif
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    , mBackboneRouterLocal(aInstance)
    , mBackboneRouterManager(aInstance)
#endif
#if OPENTHREAD_CONFIG_MLR_ENABLE || (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE)
    , mMlrManager(aInstance)
#endif

#if OPENTHREAD_CONFIG_DUA_ENABLE || (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_DUA_ENABLE)
    , mDuaManager(aInstance)
#endif
#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
    , mSrpServer(aInstance)
#endif

#if OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE
#if OPENTHREAD_FTD
    , mChildSupervisor(aInstance)
#endif
    , mSupervisionListener(aInstance)
#endif
    , mAnnounceBegin(aInstance)
    , mPanIdQuery(aInstance)
    , mEnergyScan(aInstance)
#if OPENTHREAD_CONFIG_TMF_ANYCAST_LOCATOR_ENABLE
    , mAnycastLocator(aInstance)
#endif
#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    , mTimeSync(aInstance)
#endif
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    , mLinkMetrics(aInstance)
#endif
{
}

void ThreadNetif::Up(void)
{
    VerifyOrExit(!mIsUp);

    // Enable the MAC just in case it was disabled while the Interface was down.
    Get<Mac::Mac>().SetEnabled(true);
#if OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
    IgnoreError(Get<Utils::ChannelMonitor>().Start());
#endif
    Get<MeshForwarder>().Start();

    mIsUp = true;

    SubscribeAllNodesMulticast();
    IgnoreError(Get<Mle::MleRouter>().Enable());
    IgnoreError(Get<Tmf::Agent>().Start());
#if OPENTHREAD_CONFIG_DNSSD_SERVER_ENABLE
    IgnoreError(Get<Dns::ServiceDiscovery::Server>().Start());
#endif
#if OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
    IgnoreError(Get<Dns::Client>().Start());
#endif
#if OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE
    IgnoreError(Get<Sntp::Client>().Start());
#endif
    Get<Notifier>().Signal(kEventThreadNetifStateChanged);

exit:
    return;
}

void ThreadNetif::Down(void)
{
    VerifyOrExit(mIsUp);

#if OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
    Get<Dns::Client>().Stop();
#endif
#if OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE
    IgnoreError(Get<Sntp::Client>().Stop());
#endif
#if OPENTHREAD_CONFIG_DNSSD_SERVER_ENABLE
    Get<Dns::ServiceDiscovery::Server>().Stop();
#endif
#if OPENTHREAD_CONFIG_DTLS_ENABLE
    Get<Coap::CoapSecure>().Stop();
#endif
    IgnoreError(Get<Tmf::Agent>().Stop());
    IgnoreError(Get<Mle::MleRouter>().Disable());
    RemoveAllExternalUnicastAddresses();
    UnsubscribeAllExternalMulticastAddresses();
    UnsubscribeAllRoutersMulticast();
    UnsubscribeAllNodesMulticast();

    mIsUp = false;
    Get<MeshForwarder>().Stop();
#if OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
    IgnoreError(Get<Utils::ChannelMonitor>().Stop());
#endif
    Get<Notifier>().Signal(kEventThreadNetifStateChanged);

exit:
    return;
}

Error ThreadNetif::RouteLookup(const Ip6::Address &aSource, const Ip6::Address &aDestination, uint8_t *aPrefixMatch)
{
    Error    error;
    uint16_t rloc;

    SuccessOrExit(error = Get<NetworkData::Leader>().RouteLookup(aSource, aDestination, aPrefixMatch, &rloc));

    if (rloc == Get<Mle::MleRouter>().GetRloc16())
    {
        error = kErrorNoRoute;
    }

exit:
    return error;
}

bool ThreadNetif::IsOnMesh(const Ip6::Address &aAddress) const
{
    return Get<NetworkData::Leader>().IsOnMesh(aAddress);
}

} // namespace ot
