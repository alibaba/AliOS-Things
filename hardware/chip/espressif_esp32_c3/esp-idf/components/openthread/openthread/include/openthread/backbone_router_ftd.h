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
 * @brief
 *  This file defines the OpenThread Backbone Router API (for Thread 1.2 FTD with
 *  `OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE`).
 *
 */

#ifndef OPENTHREAD_BACKBONE_ROUTER_FTD_H_
#define OPENTHREAD_BACKBONE_ROUTER_FTD_H_

#include <openthread/backbone_router.h>
#include <openthread/ip6.h>
#include <openthread/netdata.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-backbone-router
 *
 * @{
 *
 */

/**
 * Represents the Backbone Router Status.
 *
 */
typedef enum
{
    OT_BACKBONE_ROUTER_STATE_DISABLED  = 0, ///< Backbone function is disabled.
    OT_BACKBONE_ROUTER_STATE_SECONDARY = 1, ///< Secondary Backbone Router.
    OT_BACKBONE_ROUTER_STATE_PRIMARY   = 2, ///< The Primary Backbone Router.
} otBackboneRouterState;

/**
 * This function enables or disables Backbone functionality.
 *
 * @param[in] aInstance A pointer to an OpenThread instance.
 * @param[in] aEnable   TRUE to enable Backbone functionality, FALSE otherwise.
 *
 * @sa otBackboneRouterGetState
 * @sa otBackboneRouterGetConfig
 * @sa otBackboneRouterSetConfig
 * @sa otBackboneRouterRegister
 *
 */
void otBackboneRouterSetEnabled(otInstance *aInstance, bool aEnable);

/**
 * This function gets the Backbone Router state.
 *
 * @param[in] aInstance       A pointer to an OpenThread instance.
 *
 * @retval OT_BACKBONE_ROUTER_STATE_DISABLED   Backbone functionality is disabled.
 * @retval OT_BACKBONE_ROUTER_STATE_SECONDARY  Secondary Backbone Router.
 * @retval OT_BACKBONE_ROUTER_STATE_PRIMARY    The Primary Backbone Router.
 *
 * @sa otBackboneRouterSetEnabled
 * @sa otBackboneRouterGetConfig
 * @sa otBackboneRouterSetConfig
 * @sa otBackboneRouterRegister
 *
 */
otBackboneRouterState otBackboneRouterGetState(otInstance *aInstance);

/**
 * This function gets the local Backbone Router configuration.
 *
 * @param[in]   aInstance            A pointer to an OpenThread instance.
 * @param[out]  aConfig              A pointer where to put local Backbone Router configuration.
 *
 *
 * @sa otBackboneRouterSetEnabled
 * @sa otBackboneRouterGetState
 * @sa otBackboneRouterSetConfig
 * @sa otBackboneRouterRegister
 *
 */
void otBackboneRouterGetConfig(otInstance *aInstance, otBackboneRouterConfig *aConfig);

/**
 * This function sets the local Backbone Router configuration.
 *
 * @param[in]  aInstance             A pointer to an OpenThread instance.
 * @param[in]  aConfig               A pointer to the Backbone Router configuration to take effect.
 *
 * @retval OT_ERROR_NONE          Successfully updated configuration.
 * @retval OT_ERROR_INVALID_ARGS  The configuration in @p aConfig is invalid.
 *
 * @sa otBackboneRouterSetEnabled
 * @sa otBackboneRouterGetState
 * @sa otBackboneRouterGetConfig
 * @sa otBackboneRouterRegister
 *
 */
otError otBackboneRouterSetConfig(otInstance *aInstance, const otBackboneRouterConfig *aConfig);

/**
 * This function explicitly registers local Backbone Router configuration.
 *
 * @param[in]  aInstance             A pointer to an OpenThread instance.
 *
 * @retval OT_ERROR_NO_BUFS           Insufficient space to add the Backbone Router service.
 * @retval OT_ERROR_NONE              Successfully queued a Server Data Request message for delivery.
 *
 * @sa otBackboneRouterSetEnabled
 * @sa otBackboneRouterGetState
 * @sa otBackboneRouterGetConfig
 * @sa otBackboneRouterSetConfig
 *
 */
otError otBackboneRouterRegister(otInstance *aInstance);

/**
 * This method returns the Backbone Router registration jitter value.
 *
 * @returns The Backbone Router registration jitter value.
 *
 * @sa otBackboneRouterSetRegistrationJitter
 *
 */
uint8_t otBackboneRouterGetRegistrationJitter(otInstance *aInstance);

/**
 * This method sets the Backbone Router registration jitter value.
 *
 * @param[in]  aJitter the Backbone Router registration jitter value to set.
 *
 * @sa otBackboneRouterGetRegistrationJitter
 *
 */
void otBackboneRouterSetRegistrationJitter(otInstance *aInstance, uint8_t aJitter);

/**
 * This method gets the local Domain Prefix configuration.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 * @param[out] aConfig   A pointer to the Domain Prefix configuration.
 *
 * @retval OT_ERROR_NONE       Successfully got the Domain Prefix configuration.
 * @retval OT_ERROR_NOT_FOUND  No Domain Prefix was configured.
 *
 */
otError otBackboneRouterGetDomainPrefix(otInstance *aInstance, otBorderRouterConfig *aConfig);

/**
 * This method configures response status for next DUA registration.
 *
 * Note: available only when `OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE` is enabled.
 *       Only used for test and certification.
 *
 * TODO: (DUA) support coap error code and corresponding process for certification purpose.
 *
 * @param[in] aInstance A pointer to an OpenThread instance.
 * @param[in] aMlIid    A pointer to the Mesh Local IID. If NULL, respond with @p aStatus for any
 *                      coming DUA.req, otherwise only respond the one with matching @p aMlIid.
 * @param[in] aStatus   The status to respond.
 *
 *
 */
void otBackboneRouterConfigNextDuaRegistrationResponse(otInstance *                    aInstance,
                                                       const otIp6InterfaceIdentifier *aMlIid,
                                                       uint8_t                         aStatus);

/**
 * This method configures response status for next Multicast Listener Registration.
 *
 * Note: available only when `OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE` is enabled.
 *       Only used for test and certification.
 *
 * @param[in] aInstance  A pointer to an OpenThread instance.
 * @param[in] aStatus    The status to respond.
 *
 */
void otBackboneRouterConfigNextMulticastListenerRegistrationResponse(otInstance *aInstance, uint8_t aStatus);

/**
 * Represents the Multicast Listener events.
 *
 */
typedef enum
{
    OT_BACKBONE_ROUTER_MULTICAST_LISTENER_ADDED   = 0, ///< Multicast Listener was added.
    OT_BACKBONE_ROUTER_MULTICAST_LISTENER_REMOVED = 1, ///< Multicast Listener was removed or expired.
} otBackboneRouterMulticastListenerEvent;

/**
 * This function pointer is called whenever the Multicast Listeners change.
 *
 * @param[in] aContext  The user context pointer.
 * @param[in] aEvent    The Multicast Listener event.
 * @param[in] aAddress  The IPv6 multicast address of the Multicast Listener.
 *
 */
typedef void (*otBackboneRouterMulticastListenerCallback)(void *                                 aContext,
                                                          otBackboneRouterMulticastListenerEvent aEvent,
                                                          const otIp6Address *                   aAddress);

/**
 * This method sets the Backbone Router Multicast Listener callback.
 *
 * @param[in] aInstance  A pointer to an OpenThread instance.
 * @param[in] aCallback  A pointer to the Multicast Listener callback.
 * @param[in] aContext   A user context pointer.
 *
 */
void otBackboneRouterSetMulticastListenerCallback(otInstance *                              aInstance,
                                                  otBackboneRouterMulticastListenerCallback aCallback,
                                                  void *                                    aContext);

/**
 * This method clears the Multicast Listeners.
 *
 * Note: available only when `OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE` is enabled.
 *       Only used for test and certification.
 *
 * @param[in] aInstance A pointer to an OpenThread instance.
 *
 * @sa otBackboneRouterMulticastListenerAdd
 * @sa otBackboneRouterMulticastListenerGetNext
 *
 */
void otBackboneRouterMulticastListenerClear(otInstance *aInstance);

/**
 * This method adds a Multicast Listener.
 *
 * Note: available only when `OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE` is enabled.
 *       Only used for test and certification.
 *
 * @param[in] aInstance  A pointer to an OpenThread instance.
 * @param[in] aAddress   The Multicast Listener address.
 * @param[in] aTimeout   The timeout (in seconds) of the Multicast Listener, or 0 to use the default MLR timeout.
 *
 * @retval OT_ERROR_NONE          If the Multicast Listener was successfully added.
 * @retval OT_ERROR_INVALID_ARGS  If the Multicast Listener address was invalid.
 * @retval OT_ERROR_NO_BUFS       No space available to save the Multicast Listener.
 *
 * @sa otBackboneRouterMulticastListenerClear
 * @sa otBackboneRouterMulticastListenerGetNext
 *
 */
otError otBackboneRouterMulticastListenerAdd(otInstance *aInstance, const otIp6Address *aAddress, uint32_t aTimeout);

#define OT_BACKBONE_ROUTER_MULTICAST_LISTENER_ITERATOR_INIT \
    0 ///< Initializer for otBackboneRouterMulticastListenerIterator

typedef uint16_t otBackboneRouterMulticastListenerIterator; ///< Used to iterate through Multicast Listeners.

/**
 * This structure represents a Backbone Router Multicast Listener info.
 *
 */
typedef struct otBackboneRouterMulticastListenerInfo
{
    otIp6Address mAddress; // Multicast Listener address.
    uint32_t     mTimeout; // Timeout (in seconds).
} otBackboneRouterMulticastListenerInfo;

/**
 * This function gets the next Multicast Listener info (using an iterator).
 *
 * @param[in]     aInstance    A pointer to an OpenThread instance.
 * @param[inout]  aIterator    A pointer to the iterator. On success the iterator will be updated to point to next
 *                             Multicast Listener. To get the first entry the iterator should be set to
 *                             OT_BACKBONE_ROUTER_MULTICAST_LISTENER_ITERATOR_INIT.
 * @param[out]    aListenerInfo  A pointer to an `otBackboneRouterMulticastListenerInfo` where information of next
 *                               Multicast Listener is placed (on success).
 *
 * @retval OT_ERROR_NONE       Successfully found the next Multicast Listener info (@p aListenerInfo was successfully
 *                             updated).
 * @retval OT_ERROR_NOT_FOUND  No subsequent Multicast Listener info was found.
 *
 * @sa otBackboneRouterMulticastListenerClear
 * @sa otBackboneRouterMulticastListenerAdd
 *
 */
otError otBackboneRouterMulticastListenerGetNext(otInstance *                               aInstance,
                                                 otBackboneRouterMulticastListenerIterator *aIterator,
                                                 otBackboneRouterMulticastListenerInfo *    aListenerInfo);

/**
 * Represents the ND Proxy events.
 *
 */
typedef enum
{
    OT_BACKBONE_ROUTER_NDPROXY_ADDED   = 0, ///< ND Proxy was added.
    OT_BACKBONE_ROUTER_NDPROXY_REMOVED = 1, ///< ND Proxy was removed.
    OT_BACKBONE_ROUTER_NDPROXY_RENEWED = 2, ///< ND Proxy was renewed.
    OT_BACKBONE_ROUTER_NDPROXY_CLEARED = 3, ///< All ND Proxies were cleared.
} otBackboneRouterNdProxyEvent;

/**
 * This function pointer is called whenever the Nd Proxy changed.
 *
 * @param[in] aContext  The user context pointer.
 * @param[in] aEvent    The ND Proxy event.
 * @param[in] aDua      The Domain Unicast Address of the ND Proxy, or `nullptr` if @p aEvent is
 *                      `OT_BACKBONE_ROUTER_NDPROXY_CLEARED`.
 *
 */
typedef void (*otBackboneRouterNdProxyCallback)(void *                       aContext,
                                                otBackboneRouterNdProxyEvent aEvent,
                                                const otIp6Address *         aDua);

/**
 * This method sets the Backbone Router ND Proxy callback.
 *
 * @param[in] aInstance  A pointer to an OpenThread instance.
 * @param[in] aCallback  A pointer to the ND Proxy callback.
 * @param[in] aContext   A user context pointer.
 *
 */
void otBackboneRouterSetNdProxyCallback(otInstance *                    aInstance,
                                        otBackboneRouterNdProxyCallback aCallback,
                                        void *                          aContext);

/**
 * Represents the Backbone Router ND Proxy info.
 *
 */
struct otBackboneRouterNdProxyInfo
{
    otIp6InterfaceIdentifier *mMeshLocalIid;             ///< Mesh-local IID
    uint32_t                  mTimeSinceLastTransaction; ///< Time since last transaction (Seconds)
    uint16_t                  mRloc16;                   ///< RLOC16
};

/**
 * This method gets the Backbone Router ND Proxy info.
 *
 * @param[in]   aInstance     A pointer to an OpenThread instance.
 * @param[in]   aDua          The Domain Unicast Address.
 * @param[out]  aNdProxyInfo  A pointer to the ND Proxy info.
 *
 * @retval OT_ERROR_NONE       Successfully got the ND Proxy info.
 * @retval OT_ERROR_NOT_FOUND  Failed to find the Domain Unicast Address in the ND Proxy table.
 *
 */
otError otBackboneRouterGetNdProxyInfo(otInstance *                 aInstance,
                                       const otIp6Address *         aDua,
                                       otBackboneRouterNdProxyInfo *aNdProxyInfo);

/**
 * Represents the Domain Prefix events.
 *
 */
typedef enum
{
    OT_BACKBONE_ROUTER_DOMAIN_PREFIX_ADDED   = 0, ///< Domain Prefix was added.
    OT_BACKBONE_ROUTER_DOMAIN_PREFIX_REMOVED = 1, ///< Domain Prefix was removed.
    OT_BACKBONE_ROUTER_DOMAIN_PREFIX_CHANGED = 2, ///< Domain Prefix was changed.
} otBackboneRouterDomainPrefixEvent;

/**
 * This function pointer is called whenever the Domain Prefix changed.
 *
 * @param[in] aContext       The user context pointer.
 * @param[in] aEvent         The Domain Prefix event.
 * @param[in] aDomainPrefix  The new Domain Prefix if added or changed, nullptr otherwise.
 *
 */
typedef void (*otBackboneRouterDomainPrefixCallback)(void *                            aContext,
                                                     otBackboneRouterDomainPrefixEvent aEvent,
                                                     const otIp6Prefix *               aDomainPrefix);
/**
 * This method sets the Backbone Router Domain Prefix callback.
 *
 * @param[in] aInstance  A pointer to an OpenThread instance.
 * @param[in] aCallback  A pointer to the Domain Prefix callback.
 * @param[in] aContext   A user context pointer.
 *
 */
void otBackboneRouterSetDomainPrefixCallback(otInstance *                         aInstance,
                                             otBackboneRouterDomainPrefixCallback aCallback,
                                             void *                               aContext);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_BACKBONE_ROUTER_FTD_H_
