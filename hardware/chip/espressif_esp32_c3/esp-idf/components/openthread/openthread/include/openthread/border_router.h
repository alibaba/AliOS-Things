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
 * @brief
 *  This file defines the OpenThread Border Router API.
 */

#ifndef OPENTHREAD_BORDER_ROUTER_H_
#define OPENTHREAD_BORDER_ROUTER_H_

#include <openthread/ip6.h>
#include <openthread/netdata.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-border-router
 *
 * @brief
 *  This module includes functions to manage local network data with the OpenThread Border Router.
 *
 * @{
 *
 */

/**
 * This method initializes the Border Routing Manager on given infrastructure interface.
 *
 * @note  This method MUST be called before any other otBorderRouting* APIs.
 *
 * @param[in]  aInstance          A pointer to an OpenThread instance.
 * @param[in]  aInfraIfIndex      The infrastructure interface index.
 * @param[in]  aInfraIfIsRunning  A boolean that indicates whether the infrastructure
 *                                interface is running.
 *
 * @retval  OT_ERROR_NONE           Successfully started the Border Routing Manager on given infrastructure.
 * @retval  OT_ERROR_INVALID_STATE  The Border Routing Manager has already been initialized.
 * @retval  OT_ERROR_INVALID_ARGS   The index of the infrastructure interface is not valid.
 * @retval  OT_ERROR_FAILED         Internal failure. Usually due to failure in generating random prefixes.
 *
 * @sa otPlatInfraIfStateChanged.
 *
 */
otError otBorderRoutingInit(otInstance *aInstance, uint32_t aInfraIfIndex, bool aInfraIfIsRunning);

/**
 * This method enables/disables the Border Routing Manager.
 *
 * @note  The Border Routing Manager is disabled by default.
 *
 * @param[in]  aInstance  A pointer to an OpenThread instance.
 * @param[in]  aEnabled   A boolean to enable/disable the routing manager.
 *
 * @retval  OT_ERROR_INVALID_STATE  The Border Routing Manager is not initialized yet.
 * @retval  OT_ERROR_NONE           Successfully enabled/disabled the Border Routing Manager.
 *
 */
otError otBorderRoutingSetEnabled(otInstance *aInstance, bool aEnabled);

/**
 * This method returns the off-mesh-routable (OMR) prefix.
 *
 * The randomly generated 64-bit prefix will be published
 * in the Thread network if there isn't already an OMR prefix.
 *
 * @param[in]   aInstance  A pointer to an OpenThread instance.
 * @param[out]  aPrefix    A pointer to where the prefix will be output to.
 *
 * @retval  OT_ERROR_INVALID_STATE  The Border Routing Manager is not initialized yet.
 * @retval  OT_ERROR_NONE           Successfully retrieved the OMR prefix.
 *
 */
otError otBorderRoutingGetOmrPrefix(otInstance *aInstance, otIp6Prefix *aPrefix);

/**
 * This method returns the on-link prefix for the adjacent infrastructure link.
 *
 * The randomly generated 64-bit prefix will be advertised
 * on the infrastructure link if there isn't already a usable
 * on-link prefix being advertised on the link.
 *
 * @param[in]   aInstance  A pointer to an OpenThread instance.
 * @param[out]  aPrefix    A pointer to where the prefix will be output to.
 *
 * @retval  OT_ERROR_INVALID_STATE  The Border Routing Manager is not initialized yet.
 * @retval  OT_ERROR_NONE           Successfully retrieved the on-link prefix.
 *
 */
otError otBorderRoutingGetOnLinkPrefix(otInstance *aInstance, otIp6Prefix *aPrefix);

/**
 * This method provides a full or stable copy of the local Thread Network Data.
 *
 * @param[in]     aInstance    A pointer to an OpenThread instance.
 * @param[in]     aStable      TRUE when copying the stable version, FALSE when copying the full version.
 * @param[out]    aData        A pointer to the data buffer.
 * @param[inout]  aDataLength  On entry, size of the data buffer pointed to by @p aData.
 *                             On exit, number of copied bytes.
 */
otError otBorderRouterGetNetData(otInstance *aInstance, bool aStable, uint8_t *aData, uint8_t *aDataLength);

/**
 * Add a border router configuration to the local network data.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 * @param[in]  aConfig   A pointer to the border router configuration.
 *
 * @retval OT_ERROR_NONE          Successfully added the configuration to the local network data.
 * @retval OT_ERROR_INVALID_ARGS  One or more configuration parameters were invalid.
 * @retval OT_ERROR_NO_BUFS       Not enough room is available to add the configuration to the local network data.
 *
 * @sa otBorderRouterRemoveOnMeshPrefix
 * @sa otBorderRouterRegister
 */
otError otBorderRouterAddOnMeshPrefix(otInstance *aInstance, const otBorderRouterConfig *aConfig);

/**
 * Remove a border router configuration from the local network data.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 * @param[in]  aPrefix   A pointer to an IPv6 prefix.
 *
 * @retval OT_ERROR_NONE       Successfully removed the configuration from the local network data.
 * @retval OT_ERROR_NOT_FOUND  Could not find the Border Router entry.
 *
 * @sa otBorderRouterAddOnMeshPrefix
 * @sa otBorderRouterRegister
 */
otError otBorderRouterRemoveOnMeshPrefix(otInstance *aInstance, const otIp6Prefix *aPrefix);

/**
 * This function gets the next On Mesh Prefix in the local Network Data.
 *
 * @param[in]     aInstance  A pointer to an OpenThread instance.
 * @param[inout]  aIterator  A pointer to the Network Data iterator context. To get the first on-mesh entry
                             it should be set to OT_NETWORK_DATA_ITERATOR_INIT.
 * @param[out]    aConfig    A pointer to the On Mesh Prefix information.
 *
 * @retval OT_ERROR_NONE       Successfully found the next On Mesh prefix.
 * @retval OT_ERROR_NOT_FOUND  No subsequent On Mesh prefix exists in the Thread Network Data.
 *
 */
otError otBorderRouterGetNextOnMeshPrefix(otInstance *           aInstance,
                                          otNetworkDataIterator *aIterator,
                                          otBorderRouterConfig * aConfig);

/**
 * Add an external route configuration to the local network data.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 * @param[in]  aConfig   A pointer to the external route configuration.
 *
 * @retval OT_ERROR_NONE          Successfully added the configuration to the local network data.
 * @retval OT_ERROR_INVALID_ARGS  One or more configuration parameters were invalid.
 * @retval OT_ERROR_NO_BUFS       Not enough room is available to add the configuration to the local network data.
 *
 * @sa otBorderRouterRemoveRoute
 * @sa otBorderRouterRegister
 */
otError otBorderRouterAddRoute(otInstance *aInstance, const otExternalRouteConfig *aConfig);

/**
 * Remove an external route configuration from the local network data.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 * @param[in]  aPrefix   A pointer to an IPv6 prefix.
 *
 * @retval OT_ERROR_NONE       Successfully removed the configuration from the local network data.
 * @retval OT_ERROR_NOT_FOUND  Could not find the Border Router entry.
 *
 * @sa otBorderRouterAddRoute
 * @sa otBorderRouterRegister
 */
otError otBorderRouterRemoveRoute(otInstance *aInstance, const otIp6Prefix *aPrefix);

/**
 * This function gets the next external route in the local Network Data.
 *
 * @param[in]     aInstance  A pointer to an OpenThread instance.
 * @param[inout]  aIterator  A pointer to the Network Data iterator context. To get the first external route entry
                             it should be set to OT_NETWORK_DATA_ITERATOR_INIT.
 * @param[out]    aConfig    A pointer to the External Route information.
 *
 * @retval OT_ERROR_NONE       Successfully found the next External Route.
 * @retval OT_ERROR_NOT_FOUND  No subsequent external route entry exists in the Thread Network Data.
 *
 */
otError otBorderRouterGetNextRoute(otInstance *           aInstance,
                                   otNetworkDataIterator *aIterator,
                                   otExternalRouteConfig *aConfig);

/**
 * Immediately register the local network data with the Leader.
 *
 * @param[in]  aInstance A pointer to an OpenThread instance.
 *
 * @retval OT_ERROR_NONE  Successfully queued a Server Data Request message for delivery.
 *
 * @sa otBorderRouterAddOnMeshPrefix
 * @sa otBorderRouterRemoveOnMeshPrefix
 * @sa otBorderRouterAddRoute
 * @sa otBorderRouterRemoveRoute
 */
otError otBorderRouterRegister(otInstance *aInstance);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_BORDER_ROUTER_H_
