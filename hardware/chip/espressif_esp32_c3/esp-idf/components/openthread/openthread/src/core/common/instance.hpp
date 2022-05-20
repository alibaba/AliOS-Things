/*
 *  Copyright (c) 2016-2017, The OpenThread Authors.
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
 *  This file defines OpenThread instance class.
 */

#ifndef INSTANCE_HPP_
#define INSTANCE_HPP_

#include "openthread-core-config.h"

#include <stdbool.h>
#include <stdint.h>

#include <openthread/heap.h>
#include <openthread/platform/logging.h>
#if OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE
#include <openthread/platform/memory.h>
#endif

#include "common/error.hpp"
#include "common/non_copyable.hpp"
#include "common/random_manager.hpp"
#include "common/tasklet.hpp"
#include "common/time_ticker.hpp"
#include "common/timer.hpp"
#include "common/uptime.hpp"
#include "diags/factory_diags.hpp"
#include "radio/radio.hpp"

#if OPENTHREAD_RADIO || OPENTHREAD_CONFIG_LINK_RAW_ENABLE
#include "common/message.hpp"
#include "mac/link_raw.hpp"
#endif
#if OPENTHREAD_FTD || OPENTHREAD_MTD
#include "common/code_utils.hpp"
#include "common/notifier.hpp"
#include "common/settings.hpp"
#include "crypto/mbedtls.hpp"
#include "meshcop/border_agent.hpp"
#if (OPENTHREAD_CONFIG_DATASET_UPDATER_ENABLE || OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE) && OPENTHREAD_FTD
#include "meshcop/dataset_updater.hpp"
#endif
#include "net/ip6.hpp"
#include "thread/announce_sender.hpp"
#include "thread/link_quality.hpp"
#include "thread/thread_netif.hpp"
#include "thread/tmf.hpp"
#include "utils/heap.hpp"
#if OPENTHREAD_CONFIG_PING_SENDER_ENABLE
#include "utils/ping_sender.hpp"
#endif
#if OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE && OPENTHREAD_FTD
#include "utils/channel_manager.hpp"
#endif
#if OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
#include "utils/channel_monitor.hpp"
#endif
#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
#include "utils/history_tracker.hpp"
#endif

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
#include "backbone_router/bbr_leader.hpp"

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
#include "backbone_router/bbr_local.hpp"
#endif

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
#include "thread/link_metrics.hpp"
#endif

#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

#if OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE
#include "border_router/routing_manager.hpp"
#endif

#endif // OPENTHREAD_FTD || OPENTHREAD_MTD
#if OPENTHREAD_ENABLE_VENDOR_EXTENSION
#include "common/extension.hpp"
#endif
#if OPENTHREAD_CONFIG_OTNS_ENABLE
#include "utils/otns.hpp"
#endif
/**
 * @addtogroup core-instance
 *
 * @brief
 *   This module includes definitions for OpenThread instance.
 *
 * @{
 *
 */

/**
 * This struct represents an opaque (and empty) type corresponding to an OpenThread instance object.
 *
 */
typedef struct otInstance
{
} otInstance;

namespace ot {

/**
 * This class represents an OpenThread instance.
 *
 * This class contains all the components used by OpenThread.
 *
 */
class Instance : public otInstance, private NonCopyable
{
public:
#if OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE
    /**
      * This static method initializes the OpenThread instance.
      *
      * This function must be called before any other calls on OpenThread instance.
      *
      * @param[in]    aBuffer      The buffer for OpenThread to use for allocating the Instance.
      * @param[inout] aBufferSize  On input, the size of `aBuffer`. On output, if not enough space for `Instance`, the
                                   number of bytes required for `Instance`.
      *
      * @returns  A pointer to the new OpenThread instance.
      *
      */
    static Instance *Init(void *aBuffer, size_t *aBufferSize);

#else // OPENTHREAD_CONFIG_MULTIPLE_INSTANCE_ENABLE

    /**
     * This static method initializes the single OpenThread instance.
     *
     * This method initializes OpenThread and prepares it for subsequent OpenThread API calls. This function must be
     * called before any other calls to OpenThread.
     *
     * @returns A reference to the single OpenThread instance.
     *
     */
    static Instance &InitSingle(void);

    /**
     * This static method returns a reference to the single OpenThread instance.
     *
     * @returns A reference to the single OpenThread instance.
     *
     */
    static Instance &Get(void);
#endif

    /**
     * This method indicates whether or not the instance is valid/initialized and not yet finalized.
     *
     * @returns TRUE if the instance is valid/initialized, FALSE otherwise.
     *
     */
    bool IsInitialized(void) const { return mIsInitialized; }

    /**
     * This method triggers a platform reset.
     *
     * The reset process ensures that all the OpenThread state/info (stored in volatile memory) is erased. Note that
     * this method does not erase any persistent state/info saved in non-volatile memory.
     *
     */
    void Reset(void);

#if OPENTHREAD_RADIO
    /**
     * This method resets the internal states of the radio.
     *
     */
    void ResetRadioStack(void);
#endif

    /**
     * This method returns the active log level.
     *
     * @returns The log level.
     *
     */
    static otLogLevel GetLogLevel(void)
#if OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE
    {
        return sLogLevel;
    }
#else
    {
        return static_cast<otLogLevel>(OPENTHREAD_CONFIG_LOG_LEVEL);
    }
#endif

#if OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE
    /**
     * This method sets the log level.
     *
     * @param[in] aLogLevel  A log level.
     *
     */
    static void SetLogLevel(otLogLevel aLogLevel)
    {
        OT_ASSERT(aLogLevel <= OT_LOG_LEVEL_DEBG && aLogLevel >= OT_LOG_LEVEL_NONE);
        sLogLevel = aLogLevel;
    }
#endif

    /**
     * This method finalizes the OpenThread instance.
     *
     * This method should be called when OpenThread instance is no longer in use.
     *
     */
    void Finalize(void);

#if OPENTHREAD_MTD || OPENTHREAD_FTD
    /**
     * This method deletes all the settings stored in non-volatile memory, and then triggers a platform reset.
     *
     */
    void FactoryReset(void);

    /**
     * This method erases all the OpenThread persistent info (network settings) stored in non-volatile memory.
     *
     * Erase is successful/allowed only if the device is in `disabled` state/role.
     *
     * @retval kErrorNone          All persistent info/state was erased successfully.
     * @retval kErrorInvalidState  Device is not in `disabled` state/role.
     *
     */
    Error ErasePersistentInfo(void);

#if OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE
    static void  HeapFree(void *aPointer) { otPlatFree(aPointer); }
    static void *HeapCAlloc(size_t aCount, size_t aSize) { return otPlatCAlloc(aCount, aSize); }
#else
    static void  HeapFree(void *aPointer) { sHeap.Free(aPointer); }
    static void *HeapCAlloc(size_t aCount, size_t aSize) { return sHeap.CAlloc(aCount, aSize); }

    /**
     * This method returns a reference to the Heap object.
     *
     * @returns A reference to the Heap object.
     *
     */
    Utils::Heap &GetHeap(void) { return sHeap; }
#endif // OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE

#if OPENTHREAD_CONFIG_COAP_API_ENABLE
    /**
     * This method returns a reference to application COAP object.
     *
     * @returns A reference to the application COAP object.
     *
     */
    Coap::Coap &GetApplicationCoap(void) { return mApplicationCoap; }
#endif

#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
    /**
     * This method returns a reference to application COAP Secure object.
     *
     * @returns A reference to the application COAP Secure object.
     *
     */
    Coap::CoapSecure &GetApplicationCoapSecure(void) { return mApplicationCoapSecure; }
#endif

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    /**
     * This method enables/disables the "DNS name compressions" mode.
     *
     * By default DNS name compression is enabled. When disabled, DNS names are appended as full and never compressed.
     * This is applicable to OpenThread's DNS and SRP client/server modules.
     *
     * This is intended for testing only and available under a `REFERENCE_DEVICE` config.
     *
     * @param[in] aEnabled   TRUE to enable the "DNS name compression" mode, FALSE to disable.
     *
     */
    static void SetDnsNameCompressionEnabled(bool aEnabled) { sDnsNameCompressionEnabled = aEnabled; }

    /**
     * This method indicates whether the "DNS name compression" mode is enabled or not.
     *
     * @returns TRUE if the "DNS name compressions" mode is enabled, FALSE otherwise.
     *
     */
    static bool IsDnsNameCompressionEnabled(void) { return sDnsNameCompressionEnabled; }
#endif

#endif // OPENTHREAD_MTD || OPENTHREAD_FTD

    /**
     * This template method returns a reference to a given `Type` object belonging to the OpenThread instance.
     *
     * For example, `Get<MeshForwarder>()` returns a reference to the `MeshForwarder` object of the instance.
     *
     * Note that any `Type` for which the `Get<Type>` is defined MUST be uniquely accessible from the OpenThread
     * `Instance` through the member variable property hierarchy.
     *
     * Specializations of the `Get<Type>()` method are defined in this file after the `Instance` class definition.
     *
     * @returns A reference to the `Type` object of the instance.
     *
     */
    template <typename Type> inline Type &Get(void);

private:
    Instance(void);
    void AfterInit(void);

    // Order of variables (their initialization in `Instance`)
    // is important.
    //
    // Tasklet and Timer Schedulers are first to ensure other
    // objects/classes can use them from their constructors.

    Tasklet::Scheduler    mTaskletScheduler;
    TimerMilli::Scheduler mTimerMilliScheduler;
#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    TimerMicro::Scheduler mTimerMicroScheduler;
#endif

#if OPENTHREAD_MTD || OPENTHREAD_FTD
    // RandomManager is initialized before other objects. Note that it
    // requires MbedTls which itself may use Heap.
#if !OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE
    static Utils::Heap sHeap;
#endif
    Crypto::MbedTls mMbedTls;
#endif // OPENTHREAD_MTD || OPENTHREAD_FTD

    RandomManager mRandomManager;

    // Radio is initialized before other member variables
    // (particularly, SubMac and Mac) to allow them to use its methods
    // from their constructor.
    Radio mRadio;
#if OPENTHREAD_CONFIG_UPTIME_ENABLE
    Uptime mUptime;
#endif

#if OPENTHREAD_MTD || OPENTHREAD_FTD
    // Notifier, TimeTicker, Settings, and MessagePool are initialized
    // before other member variables since other classes/objects from
    // their constructor may use them.
    Notifier       mNotifier;
    TimeTicker     mTimeTicker;
    Settings       mSettings;
    SettingsDriver mSettingsDriver;
    MessagePool    mMessagePool;

    Ip6::Ip6    mIp6;
    ThreadNetif mThreadNetif;

#if OPENTHREAD_CONFIG_COAP_API_ENABLE
    Coap::Coap mApplicationCoap;
#endif

#if OPENTHREAD_CONFIG_COAP_SECURE_API_ENABLE
    Coap::CoapSecure mApplicationCoapSecure;
#endif

#if OPENTHREAD_CONFIG_PING_SENDER_ENABLE
    Utils::PingSender mPingSender;
#endif

#if OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
    Utils::ChannelMonitor mChannelMonitor;
#endif

#if OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE && OPENTHREAD_FTD
    Utils::ChannelManager mChannelManager;
#endif

#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
    Utils::HistoryTracker mHistoryTracker;
#endif

#if (OPENTHREAD_CONFIG_DATASET_UPDATER_ENABLE || OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE) && OPENTHREAD_FTD
    MeshCoP::DatasetUpdater mDatasetUpdater;
#endif

#if OPENTHREAD_CONFIG_ANNOUNCE_SENDER_ENABLE
    AnnounceSender mAnnounceSender;
#endif

#if OPENTHREAD_CONFIG_OTNS_ENABLE
    Utils::Otns mOtns;
#endif

#if OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE
    BorderRouter::RoutingManager mRoutingManager;
#endif

#endif // OPENTHREAD_MTD || OPENTHREAD_FTD
#if OPENTHREAD_RADIO || OPENTHREAD_CONFIG_LINK_RAW_ENABLE
    Mac::LinkRaw mLinkRaw;
#endif // OPENTHREAD_RADIO || OPENTHREAD_CONFIG_LINK_RAW_ENABLE

#if OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE
    static otLogLevel sLogLevel;
#endif
#if OPENTHREAD_ENABLE_VENDOR_EXTENSION
    Extension::ExtensionBase &mExtension;
#endif
#if OPENTHREAD_CONFIG_DIAG_ENABLE
    FactoryDiags::Diags mDiags;
#endif
    bool mIsInitialized;

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE && (OPENTHREAD_FTD || OPENTHREAD_MTD)
    static bool sDnsNameCompressionEnabled;
#endif
};

// Specializations of the `Get<Type>()` method.

template <> inline Radio &Instance::Get(void)
{
    return mRadio;
}

template <> inline Radio::Callbacks &Instance::Get(void)
{
    return mRadio.mCallbacks;
}

#if OPENTHREAD_CONFIG_UPTIME_ENABLE
template <> inline Uptime &Instance::Get(void)
{
    return mUptime;
}
#endif

#if OPENTHREAD_MTD || OPENTHREAD_FTD
template <> inline Notifier &Instance::Get(void)
{
    return mNotifier;
}

template <> inline TimeTicker &Instance::Get(void)
{
    return mTimeTicker;
}

template <> inline Settings &Instance::Get(void)
{
    return mSettings;
}

template <> inline SettingsDriver &Instance::Get(void)
{
    return mSettingsDriver;
}

template <> inline MeshForwarder &Instance::Get(void)
{
    return mThreadNetif.mMeshForwarder;
}

#if OPENTHREAD_CONFIG_MULTI_RADIO
template <> inline RadioSelector &Instance::Get(void)
{
    return mThreadNetif.mRadioSelector;
}
#endif

template <> inline Mle::Mle &Instance::Get(void)
{
    return mThreadNetif.mMleRouter;
}

template <> inline Mle::MleRouter &Instance::Get(void)
{
    return mThreadNetif.mMleRouter;
}

template <> inline Mle::DiscoverScanner &Instance::Get(void)
{
    return mThreadNetif.mDiscoverScanner;
}

template <> inline NeighborTable &Instance::Get(void)
{
    return mThreadNetif.mMleRouter.mNeighborTable;
}

#if OPENTHREAD_FTD
template <> inline ChildTable &Instance::Get(void)
{
    return mThreadNetif.mMleRouter.mChildTable;
}

template <> inline RouterTable &Instance::Get(void)
{
    return mThreadNetif.mMleRouter.mRouterTable;
}
#endif

template <> inline Ip6::Netif &Instance::Get(void)
{
    return mThreadNetif;
}

template <> inline ThreadNetif &Instance::Get(void)
{
    return mThreadNetif;
}

template <> inline Ip6::Ip6 &Instance::Get(void)
{
    return mIp6;
}

template <> inline Mac::Mac &Instance::Get(void)
{
    return mThreadNetif.mMac;
}

template <> inline Mac::SubMac &Instance::Get(void)
{
    return mThreadNetif.mMac.mLinks.mSubMac;
}

#if OPENTHREAD_CONFIG_RADIO_LINK_TREL_ENABLE
template <> inline Trel::Link &Instance::Get(void)
{
    return mThreadNetif.mMac.mLinks.mTrel;
}

template <> inline Trel::Interface &Instance::Get(void)
{
    return mThreadNetif.mMac.mLinks.mTrel.mInterface;
}
#endif

#if OPENTHREAD_CONFIG_MAC_FILTER_ENABLE
template <> inline Mac::Filter &Instance::Get(void)
{
    return mThreadNetif.mMac.mFilter;
}
#endif

template <> inline Lowpan::Lowpan &Instance::Get(void)
{
    return mThreadNetif.mLowpan;
}

template <> inline KeyManager &Instance::Get(void)
{
    return mThreadNetif.mKeyManager;
}

template <> inline Ip6::Filter &Instance::Get(void)
{
    return mThreadNetif.mIp6Filter;
}

#if OPENTHREAD_FTD

template <> inline IndirectSender &Instance::Get(void)
{
    return mThreadNetif.mMeshForwarder.mIndirectSender;
}

template <> inline SourceMatchController &Instance::Get(void)
{
    return mThreadNetif.mMeshForwarder.mIndirectSender.mSourceMatchController;
}

template <> inline DataPollHandler &Instance::Get(void)
{
    return mThreadNetif.mMeshForwarder.mIndirectSender.mDataPollHandler;
}

#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
template <> inline CslTxScheduler &Instance::Get(void)
{
    return mThreadNetif.mMeshForwarder.mIndirectSender.mCslTxScheduler;
}
#endif

template <> inline AddressResolver &Instance::Get(void)
{
    return mThreadNetif.mAddressResolver;
}

template <> inline MeshCoP::Leader &Instance::Get(void)
{
    return mThreadNetif.mLeader;
}

template <> inline MeshCoP::JoinerRouter &Instance::Get(void)
{
    return mThreadNetif.mJoinerRouter;
}
#endif // OPENTHREAD_FTD

template <> inline AnnounceBeginServer &Instance::Get(void)
{
    return mThreadNetif.mAnnounceBegin;
}

template <> inline DataPollSender &Instance::Get(void)
{
    return mThreadNetif.mMeshForwarder.mDataPollSender;
}

template <> inline EnergyScanServer &Instance::Get(void)
{
    return mThreadNetif.mEnergyScan;
}

template <> inline PanIdQueryServer &Instance::Get(void)
{
    return mThreadNetif.mPanIdQuery;
}

#if OPENTHREAD_CONFIG_TMF_ANYCAST_LOCATOR_ENABLE
template <> inline AnycastLocator &Instance::Get(void)
{
    return mThreadNetif.mAnycastLocator;
}
#endif

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE || OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
template <> inline NetworkData::Local &Instance::Get(void)
{
    return mThreadNetif.mNetworkDataLocal;
}
#endif

template <> inline NetworkData::Leader &Instance::Get(void)
{
    return mThreadNetif.mNetworkDataLeader;
}

#if OPENTHREAD_FTD || OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE || OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
template <> inline NetworkData::Notifier &Instance::Get(void)
{
    return mThreadNetif.mNetworkDataNotifier;
}
#endif

#if OPENTHREAD_CONFIG_NETDATA_PUBLISHER_ENABLE
template <> inline NetworkData::Publisher &Instance::Get(void)
{
    return mThreadNetif.mNetworkDataPublisher;
}
#endif

template <> inline NetworkData::Service::Manager &Instance::Get(void)
{
    return mThreadNetif.mNetworkDataServiceManager;
}

#if OPENTHREAD_CONFIG_TCP_ENABLE
template <> inline Ip6::Tcp &Instance::Get(void)
{
    return mIp6.mTcp;
}
#endif

template <> inline Ip6::Udp &Instance::Get(void)
{
    return mIp6.mUdp;
}

template <> inline Ip6::Icmp &Instance::Get(void)
{
    return mIp6.mIcmp;
}

template <> inline Ip6::Mpl &Instance::Get(void)
{
    return mIp6.mMpl;
}

template <> inline Tmf::Agent &Instance::Get(void)
{
    return mThreadNetif.mTmfAgent;
}

#if OPENTHREAD_CONFIG_DTLS_ENABLE
template <> inline Coap::CoapSecure &Instance::Get(void)
{
    return mThreadNetif.mCoapSecure;
}
#endif

template <> inline MeshCoP::ActiveDataset &Instance::Get(void)
{
    return mThreadNetif.mActiveDataset;
}

template <> inline MeshCoP::PendingDataset &Instance::Get(void)
{
    return mThreadNetif.mPendingDataset;
}

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
template <> inline TimeSync &Instance::Get(void)
{
    return mThreadNetif.mTimeSync;
}
#endif

#if OPENTHREAD_CONFIG_COMMISSIONER_ENABLE && OPENTHREAD_FTD
template <> inline MeshCoP::Commissioner &Instance::Get(void)
{
    return mThreadNetif.mCommissioner;
}
#endif

#if OPENTHREAD_CONFIG_JOINER_ENABLE
template <> inline MeshCoP::Joiner &Instance::Get(void)
{
    return mThreadNetif.mJoiner;
}
#endif

#if OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
template <> inline Dns::Client &Instance::Get(void)
{
    return mThreadNetif.mDnsClient;
}
#endif

#if OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
template <> inline Srp::Client &Instance::Get(void)
{
    return mThreadNetif.mSrpClient;
}
#endif

#if OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_ENABLE
template <> inline Utils::SrpClientBuffers &Instance::Get(void)
{
    return mThreadNetif.mSrpClientBuffers;
}
#endif

#if OPENTHREAD_CONFIG_DNSSD_SERVER_ENABLE
template <> inline Dns::ServiceDiscovery::Server &Instance::Get(void)
{
    return mThreadNetif.mDnssdServer;
}
#endif

#if OPENTHREAD_FTD || OPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE
template <> inline NetworkDiagnostic::NetworkDiagnostic &Instance::Get(void)
{
    return mThreadNetif.mNetworkDiagnostic;
}
#endif

#if OPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE
template <> inline Dhcp6::Client &Instance::Get(void)
{
    return mThreadNetif.mDhcp6Client;
}
#endif

#if OPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE
template <> inline Dhcp6::Server &Instance::Get(void)
{
    return mThreadNetif.mDhcp6Server;
}
#endif

#if OPENTHREAD_CONFIG_NEIGHBOR_DISCOVERY_AGENT_ENABLE
template <> inline NeighborDiscovery::Agent &Instance::Get(void)
{
    return mThreadNetif.mNeighborDiscoveryAgent;
}
#endif

#if OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE
template <> inline Utils::Slaac &Instance::Get(void)
{
    return mThreadNetif.mSlaac;
}
#endif

#if OPENTHREAD_CONFIG_JAM_DETECTION_ENABLE
template <> inline Utils::JamDetector &Instance::Get(void)
{
    return mThreadNetif.mJamDetector;
}
#endif

#if OPENTHREAD_CONFIG_SNTP_CLIENT_ENABLE
template <> inline Sntp::Client &Instance::Get(void)
{
    return mThreadNetif.mSntpClient;
}
#endif

#if OPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE
#if OPENTHREAD_FTD
template <> inline Utils::ChildSupervisor &Instance::Get(void)
{
    return mThreadNetif.mChildSupervisor;
}
#endif
template <> inline Utils::SupervisionListener &Instance::Get(void)
{
    return mThreadNetif.mSupervisionListener;
}
#endif

#if OPENTHREAD_CONFIG_PING_SENDER_ENABLE
template <> inline Utils::PingSender &Instance::Get(void)
{
    return mPingSender;
}
#endif

#if OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
template <> inline Utils::ChannelMonitor &Instance::Get(void)
{
    return mChannelMonitor;
}
#endif

#if OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE && OPENTHREAD_FTD
template <> inline Utils::ChannelManager &Instance::Get(void)
{
    return mChannelManager;
}
#endif

#if OPENTHREAD_CONFIG_HISTORY_TRACKER_ENABLE
template <> inline Utils::HistoryTracker &Instance::Get(void)
{
    return mHistoryTracker;
}
#endif

#if (OPENTHREAD_CONFIG_DATASET_UPDATER_ENABLE || OPENTHREAD_CONFIG_CHANNEL_MANAGER_ENABLE) && OPENTHREAD_FTD
template <> inline MeshCoP::DatasetUpdater &Instance::Get(void)
{
    return mDatasetUpdater;
}
#endif

#if OPENTHREAD_CONFIG_BORDER_AGENT_ENABLE
template <> inline MeshCoP::BorderAgent &Instance::Get(void)
{
    return mThreadNetif.mBorderAgent;
}
#endif

#if OPENTHREAD_CONFIG_ANNOUNCE_SENDER_ENABLE
template <> inline AnnounceSender &Instance::Get(void)
{
    return mAnnounceSender;
}
#endif

template <> inline MessagePool &Instance::Get(void)
{
    return mMessagePool;
}

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

template <> inline BackboneRouter::Leader &Instance::Get(void)
{
    return mThreadNetif.mBackboneRouterLeader;
}

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
template <> inline BackboneRouter::Local &Instance::Get(void)
{
    return mThreadNetif.mBackboneRouterLocal;
}
template <> inline BackboneRouter::Manager &Instance::Get(void)
{
    return mThreadNetif.mBackboneRouterManager;
}

#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE
template <> inline BackboneRouter::MulticastListenersTable &Instance::Get(void)
{
    return mThreadNetif.mBackboneRouterManager.GetMulticastListenersTable();
}
#endif

#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE
template <> inline BackboneRouter::NdProxyTable &Instance::Get(void)
{
    return mThreadNetif.mBackboneRouterManager.GetNdProxyTable();
}
#endif

template <> inline BackboneRouter::BackboneTmfAgent &Instance::Get(void)
{
    return mThreadNetif.mBackboneRouterManager.GetBackboneTmfAgent();
}
#endif

#if OPENTHREAD_CONFIG_MLR_ENABLE || (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE)
template <> inline MlrManager &Instance::Get(void)
{
    return mThreadNetif.mMlrManager;
}
#endif

#if OPENTHREAD_CONFIG_DUA_ENABLE || (OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_DUA_ENABLE)
template <> inline DuaManager &Instance::Get(void)
{
    return mThreadNetif.mDuaManager;
}
#endif

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE || OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
template <> inline LinkMetrics::LinkMetrics &Instance::Get(void)
{
    return mThreadNetif.mLinkMetrics;
}
#endif

#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

#if OPENTHREAD_CONFIG_OTNS_ENABLE
template <> inline Utils::Otns &Instance::Get(void)
{
    return mOtns;
}
#endif

#if OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE
template <> inline BorderRouter::RoutingManager &Instance::Get(void)
{
    return mRoutingManager;
}
#endif

#if OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
template <> inline Srp::Server &Instance::Get(void)
{
    return mThreadNetif.mSrpServer;
}
#endif

#endif // OPENTHREAD_MTD || OPENTHREAD_FTD

#if OPENTHREAD_RADIO || OPENTHREAD_CONFIG_LINK_RAW_ENABLE
template <> inline Mac::LinkRaw &Instance::Get(void)
{
    return mLinkRaw;
}

#if OPENTHREAD_RADIO
template <> inline Mac::SubMac &Instance::Get(void)
{
    return mLinkRaw.mSubMac;
}
#endif

#endif // OPENTHREAD_RADIO || OPENTHREAD_CONFIG_LINK_RAW_ENABLE

template <> inline Tasklet::Scheduler &Instance::Get(void)
{
    return mTaskletScheduler;
}

template <> inline TimerMilli::Scheduler &Instance::Get(void)
{
    return mTimerMilliScheduler;
}

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
template <> inline TimerMicro::Scheduler &Instance::Get(void)
{
    return mTimerMicroScheduler;
}
#endif

#if OPENTHREAD_ENABLE_VENDOR_EXTENSION
template <> inline Extension::ExtensionBase &Instance::Get(void)
{
    return mExtension;
}
#endif

#if OPENTHREAD_CONFIG_DIAG_ENABLE
template <> inline FactoryDiags::Diags &Instance::Get(void)
{
    return mDiags;
}
#endif

/**
 * @}
 *
 */

} // namespace ot

#endif // INSTANCE_HPP_
