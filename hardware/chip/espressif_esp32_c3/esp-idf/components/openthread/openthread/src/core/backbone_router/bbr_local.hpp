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
 *   This file includes definitions for local Backbone Router service.
 */

#ifndef BACKBONE_ROUTER_LOCAL_HPP_
#define BACKBONE_ROUTER_LOCAL_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE

#if (OPENTHREAD_CONFIG_THREAD_VERSION < OT_THREAD_VERSION_1_2)
#error "Thread 1.2 or higher version is required for OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE."
#endif

#if !OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
#error "OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE is required for OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE."
#endif

#if !OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE
#error "OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE is required for OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE."
#endif

#include <openthread/backbone_router.h>
#include <openthread/backbone_router_ftd.h>

#include "backbone_router/bbr_leader.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "net/netif.hpp"
#include "thread/network_data.hpp"

namespace ot {

namespace BackboneRouter {

/**
 * This class implements the definitions for local Backbone Router service.
 *
 */
class Local : public InstanceLocator, private NonCopyable
{
public:
    typedef otBackboneRouterState BackboneRouterState;

    /**
     * This constructor initializes the local Backbone Router.
     *
     * @param[in] aInstance  A reference to the OpenThread instance.
     *
     */
    explicit Local(Instance &aInstance);

    /**
     * This method enables/disables Backbone function.
     *
     * @param[in]  aEnable  TRUE to enable the backbone function, FALSE otherwise.
     *
     */
    void SetEnabled(bool aEnable);

    /**
     * This method retrieves the Backbone Router state.
     *
     *
     * @retval OT_BACKBONE_ROUTER_STATE_DISABLED   Backbone function is disabled.
     * @retval OT_BACKBONE_ROUTER_STATE_SECONDARY  Secondary Backbone Router.
     * @retval OT_BACKBONE_ROUTER_STATE_PRIMARY    Primary Backbone Router.
     *
     */
    BackboneRouterState GetState(void) const { return mState; }

    /**
     * This method resets the local Thread Network Data.
     *
     */
    void Reset(void);

    /**
     * This method gets local Backbone Router configuration.
     *
     * @param[out]  aConfig  The local Backbone Router configuration.
     *
     */
    void GetConfig(BackboneRouterConfig &aConfig) const;

    /**
     * This method sets local Backbone Router configuration.
     *
     * @param[in]  aConfig  The configuration to set.
     *
     * @retval kErrorNone         Successfully updated configuration.
     * @retval kErrorInvalidArgs  The configuration in @p aConfig is invalid.
     *
     */
    Error SetConfig(const BackboneRouterConfig &aConfig);

    /**
     * This method registers Backbone Router Dataset to Leader.
     *
     * @param[in]  aForce True to force registration regardless of current BackboneRouterState.
     *                    False to decide based on current BackboneRouterState.
     *
     *
     * @retval kErrorNone            Successfully added the Service entry.
     * @retval kErrorInvalidState    Not in the ready state to register.
     * @retval kErrorNoBufs          Insufficient space to add the Service entry.
     *
     */
    Error AddService(bool aForce = false);

    /**
     * This method indicates whether or not the Backbone Router is Primary.
     *
     * @retval  True  if the Backbone Router is Primary.
     * @retval  False if the Backbone Router is not Primary.
     *
     */
    bool IsPrimary(void) const { return mState == OT_BACKBONE_ROUTER_STATE_PRIMARY; }

    /**
     * This method indicates whether or not the Backbone Router is enabled.
     *
     * @retval  True  if the Backbone Router is enabled.
     * @retval  False if the Backbone Router is not enabled.
     *
     */
    bool IsEnabled(void) const { return mState != OT_BACKBONE_ROUTER_STATE_DISABLED; }

    /**
     * This method sets the Backbone Router registration jitter value.
     *
     * @param[in]  aRegistrationJitter the Backbone Router registration jitter value to set.
     *
     */
    void SetRegistrationJitter(uint8_t aRegistrationJitter) { mRegistrationJitter = aRegistrationJitter; }

    /**
     * This method returns the Backbone Router registration jitter value.
     *
     * @returns The Backbone Router registration jitter value.
     *
     */
    uint8_t GetRegistrationJitter(void) const { return mRegistrationJitter; }

    /**
     * This method notifies Primary Backbone Router status.
     *
     * @param[in]  aState   The state or state change of Primary Backbone Router.
     * @param[in]  aConfig  The Primary Backbone Router service.
     *
     */
    void HandleBackboneRouterPrimaryUpdate(Leader::State aState, const BackboneRouterConfig &aConfig);

    /**
     * This method gets the Domain Prefix configuration.
     *
     * @param[out]  aConfig  A reference to the Domain Prefix configuration.
     *
     * @retval kErrorNone      Successfully got the Domain Prefix configuration.
     * @retval kErrorNotFound  No Domain Prefix was configured.
     *
     */
    Error GetDomainPrefix(NetworkData::OnMeshPrefixConfig &aConfig);

    /**
     * This method removes the local Domain Prefix configuration.
     *
     * @param[in]  aPrefix A reference to the IPv6 Domain Prefix.
     *
     * @retval kErrorNone         Successfully removed the Domain Prefix.
     * @retval kErrorInvalidArgs  @p aPrefix is invalid.
     * @retval kErrorNotFound     No Domain Prefix was configured or @p aPrefix doesn't match.
     *
     */
    Error RemoveDomainPrefix(const Ip6::Prefix &aPrefix);

    /**
     * This method sets the local Domain Prefix configuration.
     *
     * @param[in]  aConfig A reference to the Domain Prefix configuration.
     *
     * @returns kErrorNone          Successfully set the local Domain Prefix.
     * @returns kErrorInvalidArgs   @p aConfig is invalid.
     *
     */
    Error SetDomainPrefix(const NetworkData::OnMeshPrefixConfig &aConfig);

    /**
     * This method returns a reference to the All Network Backbone Routers Multicast Address.
     *
     * @returns A reference to the All Network Backbone Routers Multicast Address.
     *
     */
    const Ip6::Address &GetAllNetworkBackboneRoutersAddress(void) const { return mAllNetworkBackboneRouters; }

    /**
     * This method returns a reference to the All Domain Backbone Routers Multicast Address.
     *
     * @returns A reference to the All Domain Backbone Routers Multicast Address.
     *
     */
    const Ip6::Address &GetAllDomainBackboneRoutersAddress(void) const { return mAllDomainBackboneRouters; }

    /**
     * This method applies the Mesh Local Prefix.
     *
     */
    void ApplyMeshLocalPrefix(void);

    /**
     * This method updates the subscription of All Domain Backbone Routers Multicast Address.
     *
     * @param[in]  aState  The Domain Prefix state or state change.
     *
     */
    void HandleDomainPrefixUpdate(Leader::DomainPrefixState aState);

    /**
     * This method sets the Domain Prefix callback.
     *
     * @param[in] aCallback  The callback function.
     * @param[in] aContext   A user context pointer.
     *
     */
    void SetDomainPrefixCallback(otBackboneRouterDomainPrefixCallback aCallback, void *aContext);

private:
    void SetState(BackboneRouterState aState);
    void RemoveService(void);
    void AddDomainPrefixToNetworkData(void);
    void RemoveDomainPrefixFromNetworkData(void);
#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO) && (OPENTHREAD_CONFIG_LOG_BBR == 1)
    void LogBackboneRouterService(const char *aAction, Error aError);
    void LogDomainPrefix(const char *aAction, Error aError);
#else
    void LogBackboneRouterService(const char *, Error) {}
    void LogDomainPrefix(const char *, Error) {}
#endif

    BackboneRouterState mState;
    uint32_t            mMlrTimeout;
    uint16_t            mReregistrationDelay;
    uint8_t             mSequenceNumber;
    uint8_t             mRegistrationJitter;

    // Indicates whether or not already add Backbone Router Service to local server data.
    // Used to check whether or not in restore stage after reset or whether to remove
    // Backbone Router service for Secondary Backbone Router if it was added by force.
    bool mIsServiceAdded;

    NetworkData::OnMeshPrefixConfig mDomainPrefixConfig;

    Ip6::Netif::UnicastAddress           mBackboneRouterPrimaryAloc;
    Ip6::Address                         mAllNetworkBackboneRouters;
    Ip6::Address                         mAllDomainBackboneRouters;
    otBackboneRouterDomainPrefixCallback mDomainPrefixCallback;
    void *                               mDomainPrefixCallbackContext;
};

} // namespace BackboneRouter

/**
 * @}
 */

} // namespace ot

#endif // OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE

#endif // BACKBONE_ROUTER_LOCAL_HPP_
