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

/**
 * @file
 *   This file includes definitions for the RA-based routing management.
 *
 */

#ifndef ROUTING_MANAGER_HPP_
#define ROUTING_MANAGER_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE

#if !OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
#error "OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE is required for OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE."
#endif

#if !OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE
#error "OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE is required for OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE."
#endif

#include <openthread/netdata.h>
#include <openthread/platform/infra_if.h>

#include "border_router/router_advertisement.hpp"
#include "common/array.hpp"
#include "common/error.hpp"
#include "common/locator.hpp"
#include "common/notifier.hpp"
#include "common/timer.hpp"
#include "net/ip6.hpp"
#include "thread/network_data.hpp"

namespace ot {

namespace BorderRouter {

/**
 * This class implements bi-directional routing between Thread and Infrastructure networks.
 *
 * The Border Routing manager works on both Thread interface and infrastructure interface.
 * All ICMPv6 messages are sent/received on the infrastructure interface.
 *
 */
class RoutingManager : public InstanceLocator
{
    friend class ot::Notifier;

public:
    /**
     * This constructor initializes the routing manager.
     *
     * @param[in]  aInstance  A OpenThread instance.
     *
     */
    explicit RoutingManager(Instance &aInstance);

    /**
     * This method initializes the routing manager on given infrastructure interface.
     *
     * @param[in]  aInfraIfIndex      An infrastructure network interface index.
     * @param[in]  aInfraIfIsRunning  A boolean that indicates whether the infrastructure
     *                                interface is running.
     *
     * @retval  kErrorNone         Successfully started the routing manager.
     * @retval  kErrorInvalidArgs  The index of the infra interface is not valid.
     *
     */
    Error Init(uint32_t aInfraIfIndex, bool aInfraIfIsRunning);

    /**
     * This method enables/disables the Border Routing Manager.
     *
     * @note  The Border Routing Manager is enabled by default.
     *
     * @param[in]  aEnabled   A boolean to enable/disable the Border Routing Manager.
     *
     * @retval  kErrorInvalidState   The Border Routing Manager is not initialized yet.
     * @retval  kErrorNone           Successfully enabled/disabled the Border Routing Manager.
     *
     */
    Error SetEnabled(bool aEnabled);

    /**
     * This method returns the off-mesh-routable (OMR) prefix.
     *
     * The randomly generated 64-bit prefix will be published
     * in the Thread network if there isn't already an OMR prefix.
     *
     * @param[out]  aPrefix  A reference to where the prefix will be output to.
     *
     * @retval  kErrorInvalidState  The Border Routing Manager is not initialized yet.
     * @retval  kErrorNone          Successfully retrieved the OMR prefix.
     *
     */
    Error GetOmrPrefix(Ip6::Prefix &aPrefix);

    /**
     * This method returns the on-link prefix for the adjacent  infrastructure link.
     *
     * The randomly generated 64-bit prefix will be advertised
     * on the infrastructure link if there isn't already a usable
     * on-link prefix being advertised on the link.
     *
     * @param[out]  aPrefix  A reference to where the prefix will be output to.
     *
     * @retval  kErrorInvalidState  The Border Routing Manager is not initialized yet.
     * @retval  kErrorNone          Successfully retrieved the on-link prefix.
     *
     */
    Error GetOnLinkPrefix(Ip6::Prefix &aPrefix);

    /**
     * This method receives an ICMPv6 message on the infrastructure interface.
     *
     * Malformed or undesired messages are dropped silently.
     *
     * @param[in]  aInfraIfIndex  The infrastructure interface index.
     * @param[in]  aSrcAddress    The source address this message is sent from.
     * @param[in]  aBuffer        THe ICMPv6 message buffer.
     * @param[in]  aLength        The length of the ICMPv6 message buffer.
     *
     */
    void RecvIcmp6Message(uint32_t            aInfraIfIndex,
                          const Ip6::Address &aSrcAddress,
                          const uint8_t *     aBuffer,
                          uint16_t            aBufferLength);

    /**
     * This method handles infrastructure interface state changes.
     *
     * @param[in]  aInfraIfIndex  The index of the infrastructure interface.
     * @param[in]  aIsRunning     A boolean that indicates whether the infrastructure
     *                            interface is running.
     *
     * @retval  kErrorNone          Successfully updated the infra interface status.
     * @retval  kErrorInvalidState  The Routing Manager is not initialized.
     * @retval  kErrorInvalidArgs   The @p aInfraIfIndex doesn't match the infra interface
     *                              the Routing Manager is initialized with.
     *
     */
    Error HandleInfraIfStateChanged(uint32_t aInfraIfIndex, bool aIsRunning);

private:
    typedef NetworkData::RoutePreference RoutePreference;

    static constexpr uint16_t kMaxRouterAdvMessageLength = 256; // The maximum RA message length we can handle.

    // The maximum number of the OMR prefixes to advertise.
    static constexpr uint8_t kMaxOmrPrefixNum = OPENTHREAD_CONFIG_IP6_SLAAC_NUM_ADDRESSES;

    // The maximum number of prefixes to discover on the infra link.
    static constexpr uint8_t kMaxDiscoveredPrefixNum = 8;

    static constexpr uint8_t kOmrPrefixLength    = OT_IP6_PREFIX_BITSIZE; // The length of an OMR prefix. In bits.
    static constexpr uint8_t kOnLinkPrefixLength = OT_IP6_PREFIX_BITSIZE; // The length of an On-link prefix. In bits.

    // The maximum number of initial Router Advertisements.
    static constexpr uint32_t kMaxInitRtrAdvertisements = 3;

    // The maximum number of Router Solicitations before sending Router Advertisements.
    static constexpr uint32_t kMaxRtrSolicitations = 3;

    static constexpr uint32_t kDefaultOmrPrefixLifetime    = 1800; // The default OMR prefix valid lifetime. In sec.
    static constexpr uint32_t kDefaultOnLinkPrefixLifetime = 1800; // The default on-link prefix valid lifetime. In sec.
    static constexpr uint32_t kMaxRtrAdvInterval           = 600;  // Max Router Advertisement Interval. In sec.
    static constexpr uint32_t kMinRtrAdvInterval           = kMaxRtrAdvInterval / 3; // Min RA Interval. In sec.
    static constexpr uint32_t kMaxInitRtrAdvInterval       = 16;                     // Max Initial RA Interval. In sec.
    static constexpr uint32_t kMaxRaDelayTime              = 500; // Max delay of sending RA after rx RS. In msec.
    static constexpr uint32_t kRtrSolicitationInterval     = 4;   // Interval between RSs. In sec.
    static constexpr uint32_t kMaxRtrSolicitationDelay     = 1;   // Max delay for initial solicitation. In sec.
    static constexpr uint32_t kMaxRoutingPolicyDelay       = 1;   // Max delay for routing policy evaluation. In sec.
    static constexpr uint32_t kRtrSolicitationRetryDelay   = 60;  // The delay before retrying failed RS tx. In Sec.

    // The STALE_RA_TIME in seconds. The Routing Manager will consider the prefixes
    // and learned RA parameters STALE when they are not refreshed in STALE_RA_TIME
    // seconds. The Routing Manager will then start Router Solicitation to verify
    // that the STALE prefix is not being advertised anymore and remove the STALE
    // prefix.
    // The value is chosen in range of [`kMaxRtrAdvInterval` upper bound (1800s), `kDefaultOnLinkPrefixLifetime`].
    static constexpr uint32_t kRtrAdvStaleTime = 1800;

    // The VICARIOUS_SOLICIT_TIME in seconds. The Routing Manager will consider
    // the discovered prefixes invalid if they are not refreshed after receiving
    // a Router Solicitation message.
    // The value is equal to Router Solicitation timeout.
    static constexpr uint32_t kVicariousSolicitationTime =
        kRtrSolicitationInterval * (kMaxRtrSolicitations - 1) + kMaxRtrSolicitationDelay;

    static_assert(kMinRtrAdvInterval <= 3 * kMaxRtrAdvInterval / 4, "invalid RA intervals");
    static_assert(kDefaultOmrPrefixLifetime >= kMaxRtrAdvInterval, "invalid default OMR prefix lifetime");
    static_assert(kDefaultOnLinkPrefixLifetime >= kMaxRtrAdvInterval, "invalid default on-link prefix lifetime");
    static_assert(kRtrAdvStaleTime >= 1800 && kRtrAdvStaleTime <= kDefaultOnLinkPrefixLifetime,
                  "invalid RA STALE time");

    // This struct represents an external prefix which is
    // discovered on the infrastructure interface.
    struct ExternalPrefix : public Clearable<ExternalPrefix>, public Unequatable<ExternalPrefix>
    {
        Ip6::Prefix mPrefix;
        uint32_t    mValidLifetime;

        union
        {
            // Preferred Lifetime of on-link prefix, available
            // only when `mIsOnLinkPrefix` is TRUE.
            uint32_t mPreferredLifetime;

            // The preference of this route, available
            // only when `mIsOnLinkPrefix` is FALSE.
            RoutePreference mRoutePreference;
        };
        TimeMilli mTimeLastUpdate;
        bool      mIsOnLinkPrefix;

        bool operator==(const ExternalPrefix &aPrefix) const
        {
            return mPrefix == aPrefix.mPrefix && mIsOnLinkPrefix == aPrefix.mIsOnLinkPrefix;
        }

        bool IsDeprecated(void) const
        {
            OT_ASSERT(mIsOnLinkPrefix);

            return mTimeLastUpdate + TimeMilli::SecToMsec(mPreferredLifetime) <= TimerMilli::GetNow();
        }

        TimeMilli GetExpireTime(void) const { return mTimeLastUpdate + GetPrefixExpireDelay(mValidLifetime); }
        TimeMilli GetStaleTime(void) const
        {
            uint32_t delay = OT_MIN(kRtrAdvStaleTime, mIsOnLinkPrefix ? mPreferredLifetime : mValidLifetime);

            return mTimeLastUpdate + TimeMilli::SecToMsec(delay);
        }

        static uint32_t GetPrefixExpireDelay(uint32_t aValidLifetime);
    };

    typedef Array<Ip6::Prefix, kMaxOmrPrefixNum>           OmrPrefixArray;
    typedef Array<ExternalPrefix, kMaxDiscoveredPrefixNum> ExternalPrefixArray;

    void  EvaluateState(void);
    void  Start(void);
    void  Stop(void);
    void  HandleNotifierEvents(Events aEvents);
    bool  IsInitialized(void) const { return mInfraIfIndex != 0; }
    bool  IsEnabled(void) const { return mIsEnabled; }
    Error LoadOrGenerateRandomOmrPrefix(void);
    Error LoadOrGenerateRandomOnLinkPrefix(void);

    const Ip6::Prefix *EvaluateOnLinkPrefix(void);

    void  EvaluateRoutingPolicy(void);
    void  StartRoutingPolicyEvaluationDelay(void);
    void  EvaluateOmrPrefix(OmrPrefixArray &aNewOmrPrefixes);
    Error PublishLocalOmrPrefix(void);
    void  UnpublishLocalOmrPrefix(void);
    Error AddExternalRoute(const Ip6::Prefix &aPrefix, RoutePreference aRoutePreference);
    void  RemoveExternalRoute(const Ip6::Prefix &aPrefix);
    void  StartRouterSolicitationDelay(void);
    Error SendRouterSolicitation(void);
    void  SendRouterAdvertisement(const OmrPrefixArray &aNewOmrPrefixes, const Ip6::Prefix *aNewOnLinkPrefix);

    static void HandleRouterAdvertisementTimer(Timer &aTimer);
    void        HandleRouterAdvertisementTimer(void);
    static void HandleVicariousRouterSolicitTimer(Timer &aTimer);
    void        HandleVicariousRouterSolicitTimer(void);
    static void HandleRouterSolicitTimer(Timer &aTimer);
    void        HandleRouterSolicitTimer(void);
    static void HandleDiscoveredPrefixInvalidTimer(Timer &aTimer);
    void        HandleDiscoveredPrefixInvalidTimer(void);
    static void HandleDiscoveredPrefixStaleTimer(Timer &aTimer);
    void        HandleDiscoveredPrefixStaleTimer(void);
    static void HandleRoutingPolicyTimer(Timer &aTimer);
    void        HandleOnLinkPrefixDeprecateTimer(void);
    static void HandleOnLinkPrefixDeprecateTimer(Timer &aTimer);

    void DeprecateOnLinkPrefix(void);
    void HandleRouterSolicit(const Ip6::Address &aSrcAddress, const uint8_t *aBuffer, uint16_t aBufferLength);
    void HandleRouterAdvertisement(const Ip6::Address &aSrcAddress, const uint8_t *aBuffer, uint16_t aBufferLength);
    bool UpdateDiscoveredPrefixes(const RouterAdv::PrefixInfoOption &aPio, bool aManagedAddrConfig);
    bool UpdateDiscoveredPrefixes(const RouterAdv::RouteInfoOption &aRio);
    bool InvalidateDiscoveredPrefixes(const Ip6::Prefix *aPrefix = nullptr, bool aIsOnLinkPrefix = true);
    void InvalidateAllDiscoveredPrefixes(void);
    bool NetworkDataContainsOmrPrefix(const Ip6::Prefix &aPrefix) const;
    bool UpdateRouterAdvMessage(const RouterAdv::RouterAdvMessage *aRouterAdvMessage);

    static bool IsValidOmrPrefix(const NetworkData::OnMeshPrefixConfig &aOnMeshPrefixConfig);
    static bool IsValidOmrPrefix(const Ip6::Prefix &aOmrPrefix);
    static bool IsValidOnLinkPrefix(const RouterAdv::PrefixInfoOption &aPio, bool aManagedAddrConfig);
    static bool IsValidOnLinkPrefix(const Ip6::Prefix &aOnLinkPrefix);

    // Indicates whether the Routing Manager is running (started).
    bool mIsRunning;

    // Indicates whether the Routing manager is enabled. The Routing
    // Manager will be stopped if we are disabled.
    bool mIsEnabled;

    // Indicates whether the infra interface is running. The Routing
    // Manager will be stopped when the Infra interface is not running.
    bool mInfraIfIsRunning;

    // The index of the infra interface on which Router Advertisement
    // messages will be sent.
    uint32_t mInfraIfIndex;

    // The OMR prefix loaded from local persistent storage or randomly
    // generated if non is found in persistent storage.
    Ip6::Prefix mLocalOmrPrefix;

    // The advertised OMR prefixes. For a stable Thread network without
    // manually configured OMR prefixes, there should be a single OMR
    // prefix that is being advertised because each BRs will converge to
    // the smallest OMR prefix sorted by method IsPrefixSmallerThan. If
    // manually configured OMR prefixes exist, they will also be
    // advertised on infra link.
    OmrPrefixArray mAdvertisedOmrPrefixes;

    // The on-link prefix loaded from local persistent storage or
    // randomly generated if non is found in persistent storage.
    Ip6::Prefix mLocalOnLinkPrefix;

    bool mIsAdvertisingLocalOnLinkPrefix;

    // The last time when the on-link prefix is advertised with
    // non-zero preferred lifetime.
    TimeMilli  mTimeAdvertisedOnLinkPrefix;
    TimerMilli mOnLinkPrefixDeprecateTimer;

    // The array of prefixes discovered on the infra link. Those
    // prefixes consist of on-link prefix(es) and OMR prefixes
    // advertised by BRs in another Thread Network which is connected to
    // the same infra link.
    ExternalPrefixArray mDiscoveredPrefixes;

    // The RA header and parameters for the infra interface.
    // This value is initialized with `RouterAdvMessage::SetToDefault`
    // and updated with RA messages initiated from infra interface.
    RouterAdv::RouterAdvMessage mRouterAdvMessage;
    TimeMilli                   mTimeRouterAdvMessageLastUpdate;

    TimerMilli mDiscoveredPrefixInvalidTimer;
    TimerMilli mDiscoveredPrefixStaleTimer;

    TimerMilli mRouterAdvertisementTimer;
    uint32_t   mRouterAdvertisementCount;

    TimerMilli mVicariousRouterSolicitTimer;
    TimeMilli  mTimeVicariousRouterSolicitStart;
    TimerMilli mRouterSolicitTimer;
    TimeMilli  mTimeRouterSolicitStart;
    uint8_t    mRouterSolicitCount;

    TimerMilli mRoutingPolicyTimer;
};

} // namespace BorderRouter

} // namespace ot

#endif // OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE

#endif // ROUTING_MANAGER_HPP_
