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
 *   This file includes definitions for the Thread network interface.
 */

#ifndef THREAD_NETIF_HPP_
#define THREAD_NETIF_HPP_

#include "openthread-core-config.h"

#include "coap/coap_secure.hpp"
#include "mac/mac.hpp"
#include "thread/tmf.hpp"

#if OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
#include "meshcop/border_agent.hpp"
#endif
#if OPENTHREAD_CONFIG_COMMISSIONER_ENABLE && OPENTHREAD_FTD
#include "meshcop/commissioner.hpp"
#endif // OPENTHREAD_CONFIG_COMMISSIONER_ENABLE && OPENTHREAD_FTD

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
#include "backbone_router/bbr_leader.hpp"
#endif
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
#include "backbone_router/backbone_tmf.hpp"
#include "backbone_router/bbr_local.hpp"
#include "backbone_router/bbr_manager.hpp"
#endif

#if OPENTHREAD_CONFIG_MLR_ENABLE || (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE)
#include "thread/mlr_manager.hpp"
#endif

#if OPENTHREAD_CONFIG_DUA_ENABLE || (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_DUA_ENABLE)
#include "thread/dua_manager.hpp"
#endif

#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
#include "net/srp_server.hpp"
#endif

#include "meshcop/dataset_manager.hpp"

#if OPENTHREAD_CONFIG_JOINER_ENABLE
#include "meshcop/joiner.hpp"
#endif // OPENTHREAD_CONFIG_JOINER_ENABLE

#include "meshcop/joiner_router.hpp"
#include "meshcop/meshcop_leader.hpp"
#include "net/dhcp6.hpp"
#include "net/dhcp6_client.hpp"
#include "net/dhcp6_server.hpp"
#include "net/dns_client.hpp"
#include "net/dnssd_server.hpp"
#include "net/ip6_filter.hpp"
#include "net/nd_agent.hpp"
#include "net/netif.hpp"
#include "net/sntp_client.hpp"
#include "net/srp_client.hpp"
#include "thread/address_resolver.hpp"
#include "thread/announce_begin_server.hpp"
#include "thread/anycast_locator.hpp"
#include "thread/discover_scanner.hpp"
#include "thread/energy_scan_server.hpp"
#include "thread/key_manager.hpp"
#include "thread/link_metrics.hpp"
#include "thread/mesh_forwarder.hpp"
#include "thread/mle.hpp"
#include "thread/mle_router.hpp"
#include "thread/network_data_local.hpp"
#include "thread/network_data_notifier.hpp"
#include "thread/network_data_publisher.hpp"
#include "thread/network_data_service.hpp"
#include "thread/network_diagnostic.hpp"
#include "thread/panid_query_server.hpp"
#include "thread/radio_selector.hpp"
#include "thread/time_sync_service.hpp"
#include "utils/child_supervision.hpp"
#include "utils/srp_client_buffers.hpp"

#if OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE
#include "utils/slaac_address.hpp"
#endif

#if OPENTHREAD_CONFIG_JAM_DETECTION_ENABLE
#include "utils/jam_detector.hpp"
#endif // OPENTHREAD_CONFIG_JAM_DETECTION_ENABLE

namespace ot {

/**
 * @addtogroup core-netif
 *
 * @brief
 *   This module includes definitions for the Thread network interface.
 *
 * @{
 */

class ThreadNetif : public Ip6::Netif
{
    friend class Instance;

public:
    /**
     * This constructor initializes the Thread network interface.
     *
     * @param[in]  aInstance  A reference to the OpenThread instance.
     *
     */
    explicit ThreadNetif(Instance &aInstance);

    /**
     * This method enables the Thread network interface.
     *
     */
    void Up(void);

    /**
     * This method disables the Thread network interface.
     *
     */
    void Down(void);

    /**
     * This method indicates whether or not the Thread network interface is enabled.
     *
     * @retval TRUE   If the Thread network interface is enabled.
     * @retval FALSE  If the Thread network interface is not enabled.
     *
     */
    bool IsUp(void) const { return mIsUp; }

    /**
     * This method submits a message to the network interface.
     *
     * @param[in]  aMessage  A reference to the message.
     *
     * @retval kErrorNone  Successfully submitted the message to the interface.
     *
     */
    Error SendMessage(Message &aMessage) { return mMeshForwarder.SendMessage(aMessage); }

    /**
     * This method performs a route lookup.
     *
     * @param[in]   aSource       A reference to the IPv6 source address.
     * @param[in]   aDestination  A reference to the IPv6 destination address.
     * @param[out]  aPrefixMatch  A pointer where the number of prefix match bits for the chosen route is stored.
     *
     * @retval kErrorNone      Successfully found a route.
     * @retval kErrorNoRoute   Could not find a valid route.
     *
     */
    Error RouteLookup(const Ip6::Address &aSource, const Ip6::Address &aDestination, uint8_t *aPrefixMatch);

    /**
     * This method indicates whether @p aAddress matches an on-mesh prefix.
     *
     * @param[in]  aAddress  The IPv6 address.
     *
     * @retval TRUE   If @p aAddress matches an on-mesh prefix.
     * @retval FALSE  If @p aAddress does not match an on-mesh prefix.
     *
     */
    bool IsOnMesh(const Ip6::Address &aAddress) const;

private:
    Tmf::Agent mTmfAgent;
#if OPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE
    Dhcp6::Client mDhcp6Client;
#endif
#if OPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE
    Dhcp6::Server mDhcp6Server;
#endif
#if OPENTHREAD_CONFIG_NEIGHBOR_DISCOVERY_AGENT_ENABLE
    NeighborDiscovery::Agent mNeighborDiscoveryAgent;
#endif
#if OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE
    Utils::Slaac mSlaac;
#endif
#if OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
    Dns::Client mDnsClient;
#endif
#if OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
    Srp::Client mSrpClient;
#endif
#if OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_ENABLE
    Utils::SrpClientBuffers mSrpClientBuffers;
#endif
#if OPENTHREAD_CONFIG_DNSSD_SERVER_ENABLE
    Dns::ServiceDiscovery::Server mDnssdServer;
#endif
#if OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE
    Sntp::Client mSntpClient;
#endif
    MeshCoP::ActiveDataset  mActiveDataset;
    MeshCoP::PendingDataset mPendingDataset;
    Ip6::Filter             mIp6Filter;
    KeyManager              mKeyManager;
    Lowpan::Lowpan          mLowpan;
    Mac::Mac                mMac;
    MeshForwarder           mMeshForwarder;
    Mle::MleRouter          mMleRouter;
    Mle::DiscoverScanner    mDiscoverScanner;
#if OPENTHREAD_CONFIG_MULTI_RADIO
    RadioSelector mRadioSelector;
#endif
#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE || OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    NetworkData::Local mNetworkDataLocal;
#endif // OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE || OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    NetworkData::Leader mNetworkDataLeader;
#if OPENTHREAD_FTD || OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE || OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
    NetworkData::Notifier mNetworkDataNotifier;
#endif
#if OPENTHREAD_CONFIG_NETDATA_PUBLISHER_ENABLE
    NetworkData::Publisher mNetworkDataPublisher;
#endif
    NetworkData::Service::Manager mNetworkDataServiceManager;
#if OPENTHREAD_FTD || OPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE
    NetworkDiagnostic::NetworkDiagnostic mNetworkDiagnostic;
#endif // OPENTHREAD_FTD || OPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE
    bool mIsUp;

#if OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
    MeshCoP::BorderAgent mBorderAgent;
#endif
#if OPENTHREAD_CONFIG_COMMISSIONER_ENABLE && OPENTHREAD_FTD
    MeshCoP::Commissioner mCommissioner;
#endif // OPENTHREAD_CONFIG_COMMISSIONER_ENABLE

#if OPENTHREAD_CONFIG_DTLS_ENABLE
    Coap::CoapSecure mCoapSecure;
#endif // OPENTHREAD_CONFIG_DTLS_ENABLE

#if OPENTHREAD_CONFIG_JOINER_ENABLE
    MeshCoP::Joiner mJoiner;
#endif // OPENTHREAD_CONFIG_JOINER_ENABLE

#if OPENTHREAD_CONFIG_JAM_DETECTION_ENABLE
    Utils::JamDetector mJamDetector;
#endif // OPENTHREAD_CONFIG_JAM_DETECTION_ENABLE

#if OPENTHREAD_FTD
    MeshCoP::JoinerRouter mJoinerRouter;
    MeshCoP::Leader       mLeader;
    AddressResolver       mAddressResolver;
#endif // OPENTHREAD_FTD

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    BackboneRouter::Leader mBackboneRouterLeader;
#endif
#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    BackboneRouter::Local   mBackboneRouterLocal;
    BackboneRouter::Manager mBackboneRouterManager;
#endif
#if OPENTHREAD_CONFIG_MLR_ENABLE || (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE)
    MlrManager mMlrManager;
#endif
#if OPENTHREAD_CONFIG_DUA_ENABLE || (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_DUA_ENABLE)
    DuaManager mDuaManager;
#endif
#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
    Srp::Server mSrpServer;
#endif

#if OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE
#if OPENTHREAD_FTD
    Utils::ChildSupervisor mChildSupervisor;
#endif
    Utils::SupervisionListener mSupervisionListener;
#endif
    AnnounceBeginServer mAnnounceBegin;
    PanIdQueryServer    mPanIdQuery;
    EnergyScanServer    mEnergyScan;
#if OPENTHREAD_CONFIG_TMF_ANYCAST_LOCATOR_ENABLE
    AnycastLocator mAnycastLocator;
#endif

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    TimeSync mTimeSync;
#endif
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    LinkMetrics::LinkMetrics mLinkMetrics;
#endif
};

/**
 * @}
 */

} // namespace ot

#endif // THREAD_NETIF_HPP_
