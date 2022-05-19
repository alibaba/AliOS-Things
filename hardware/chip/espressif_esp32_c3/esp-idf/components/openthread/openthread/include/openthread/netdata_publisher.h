/*
 *  Copyright (c) 2021, The OpenThread Authors.
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
 *  This file defines the OpenThread Network Data Publisher API.
 */

#ifndef OPENTHREAD_NETDATA_PUBLISHER_H_
#define OPENTHREAD_NETDATA_PUBLISHER_H_

#include <openthread/netdata.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-thread-general
 *
 * @{
 *
 * The Network Data Publisher provides mechanisms to limit the number of similar Service and/or Prefix (on-mesh prefix
 * or external route) entries in the Thread Network Data by monitoring the Network Data and managing if or when to add
 * or remove entries.
 *
 * All the functions in this module require `OPENTHREAD_CONFIG_NETDATA_PUBLISHER_ENABLE` to be enabled.
 *
 */

/**
 * This enumeration represents the events reported from the Publisher callbacks.
 *
 */
typedef enum otNetDataPublisherEvent
{
    OT_NETDATA_PUBLISHER_EVENT_ENTRY_ADDED   = 0, ///< Published entry is added to the Thread Network Data.
    OT_NETDATA_PUBLISHER_EVENT_ENTRY_REMOVED = 1, ///< Published entry is removed from the Thread Network Data.
} otNetDataPublisherEvent;

/**
 * This function pointer type defines the callback used to notify when a "DNS/SRP Service" entry is added to or removed
 * from the Thread Network Data.
 *
 * On remove the callback is invoked independent of whether the entry is removed by `Publisher` (e.g., when there are
 * too many similar entries already present in the Network Data) or through an explicit call to unpublish the entry
 * (i.e., a call to `otNetDataUnpublishDnsSrpService()`).
 *
 * @param[in] aEvent     Indicates the event (whether the entry was added or removed).
 * @param[in] aContext   A pointer to application-specific context.
 *
 */
typedef void (*otNetDataDnsSrpServicePublisherCallback)(otNetDataPublisherEvent aEvent, void *aContext);

/**
 * This function pointer type defines the callback used to notify when a prefix (on-mesh or external route) entry is
 * added to or removed from the Thread Network Data.
 *
 * On remove the callback is invoked independent of whether the entry is removed by `Publisher` (e.g., when there are
 * too many similar entries already present in the Network Data) or through an explicit call to unpublish the entry.
 *
 * @param[in] aEvent     Indicates the event (whether the entry was added or removed).
 * @param[in] aPrefix    A pointer to the prefix entry.
 * @param[in] aContext   A pointer to application-specific context.
 *
 */
typedef void (*otNetDataPrefixPublisherCallback)(otNetDataPublisherEvent aEvent,
                                                 const otIp6Prefix *     aPrefix,
                                                 void *                  aContext);

/**
 * This function requests "DNS/SRP Service Anycast Address" to be published in the Thread Network Data.
 *
 * This function requires the feature `OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE` to be enabled.
 *
 * A call to this function will remove and replace any previous "DNS/SRP Service" entry that was being published (from
 * earlier call to any of `otNetDataPublishDnsSrpService{Type}()` functions).
 *
 * @param[in] aInstance        A pointer to an OpenThread instance.
 * @param[in] aSequenceNUmber  The sequence number of DNS/SRP Anycast Service.
 *
 */
void otNetDataPublishDnsSrpServiceAnycast(otInstance *aInstance, uint8_t aSequenceNUmber);

/**
 * This function requests "DNS/SRP Service Unicast Address" to be published in the Thread Network Data.
 *
 * This function requires the feature `OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE` to be enabled.
 *
 * A call to this function will remove and replace any previous "DNS/SRP Service" entry that was being published (from
 * earlier call to any of `otNetDataPublishDnsSrpService{Type}()` functions).
 *
 * This function publishes the "DNS/SRP Service Unicast Address" by including the address and port info in the Service
 * TLV data.
 *
 * @param[in] aInstance  A pointer to an OpenThread instance.
 * @param[in] aAddress   The DNS/SRP server address to publish (MUST NOT be NULL).
 * @param[in] aPort      The SRP server port number to publish.
 *
 */
void otNetDataPublishDnsSrpServiceUnicast(otInstance *aInstance, const otIp6Address *aAddress, uint16_t aPort);

/**
 * This function requests "DNS/SRP Service Unicast Address" to be published in the Thread Network Data.
 *
 * This function requires the feature `OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE` to be enabled.
 *
 * A call to this function will remove and replace any previous "DNS/SRP Service" entry that was being published (from
 * earlier call to any of `otNetDataPublishDnsSrpService{Type}()` functions).
 *
 * Unlike `otNetDataPublishDnsSrpServiceUnicast()` which requires the published address to be given and includes the
 * info in the Service TLV data, this function uses the device's mesh-local EID and includes the info in the Server TLV
 * data.
 *
 * @param[in] aInstance  A pointer to an OpenThread instance.
 * @param[in] aPort      The SRP server port number to publish.
 *
 */
void otNetDataPublishDnsSrpServiceUnicastMeshLocalEid(otInstance *aInstance, uint16_t aPort);

/**
 * This function indicates whether or not currently the "DNS/SRP Service" entry is added to the Thread Network Data.
 *
 * This function requires the feature `OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE` to be enabled.
 *
 * @param[in] aInstance  A pointer to an OpenThread instance.
 *
 * @retval TRUE    The published DNS/SRP Service entry is added to the Thread Network Data.
 * @retval FALSE   The entry is not added to Thread Network Data or there is no entry to publish.
 *
 */
bool otNetDataIsDnsSrpServiceAdded(otInstance *aInstance);

/**
 * This function sets a callback for notifying when a published "DNS/SRP Service" is actually added to or removed from
 * the Thread Network Data.
 *
 * A subsequent call to this function replaces any previously set callback function.
 *
 * This function requires the feature `OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE` to be enabled.
 *
 * @param[in] aInstance        A pointer to an OpenThread instance.
 * @param[in] aCallback        The callback function pointer (can be NULL if not needed).
 * @param[in] aContext         A pointer to application-specific context (used when @p aCallback is invoked).
 *
 */
void otNetDataSetDnsSrpServicePublisherCallback(otInstance *                            aInstance,
                                                otNetDataDnsSrpServicePublisherCallback aCallback,
                                                void *                                  aContext);

/**
 * This function unpublishes any previously added "DNS/SRP (Anycast or Unicast) Service" entry from the Thread Network
 * Data.
 *
 * This function requires the feature `OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE` to be enabled.
 *
 * @param[in] aInstance  A pointer to an OpenThread instance.
 *
 */
void otNetDataUnpublishDnsSrpService(otInstance *aInstance);

/**
 * This function requests an on-mesh prefix to be published in the Thread Network Data.
 *
 * This function requires the feature `OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE` to be enabled.
 *
 * Only stable entries can be published (i.e.,`aConfig.mStable` MUST be TRUE).
 *
 * @param[in] aInstance           A pointer to an OpenThread instance.
 * @param[in] aConfig             The on-mesh prefix config to publish (MUST NOT be NULL).
 *
 * @retval OT_ERROR_NONE          The on-mesh prefix is published successfully.
 * @retval OT_ERROR_INVALID_ARGS  The @p aConfig is not valid (bad prefix, invalid flag combinations, or not stable).
 * @retval OT_ERROR_ALREADY       An entry with the same prefix is already in the published list.
 * @retval OT_ERROR_NO_BUFS       Could not allocate an entry for the new request. Publisher supports a limited number
 *                                of entries (shared between on-mesh prefix and external route) determined by config
 *                                `OPENTHREAD_CONFIG_NETDATA_PUBLISHER_MAX_PREFIX_ENTRIES`.
 *
 *
 */
otError otNetDataPublishOnMeshPrefix(otInstance *aInstance, const otBorderRouterConfig *aConfig);

/**
 * This function requests an external route prefix to be published in the Thread Network Data.
 *
 * This function requires the feature `OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE` to be enabled.
 *
 * Only stable entries can be published (i.e.,`aConfig.mStable` MUST be TRUE).
 *
 * @param[in] aInstance           A pointer to an OpenThread instance.
 * @param[in] aConfig             The external route config to publish (MUST NOT be NULL).
 *
 * @retval OT_ERROR_NONE          The external route is published successfully.
 * @retval OT_ERROR_INVALID_ARGS  The @p aConfig is not valid (bad prefix, invalid flag combinations, or not stable).
 * @retval OT_ERROR_ALREADY       An entry with the same prefix is already in the published list.
 * @retval OT_ERROR_NO_BUFS       Could not allocate an entry for the new request. Publisher supports a limited number
 *                                of entries (shared between on-mesh prefix and external route) determined by config
 *                                `OPENTHREAD_CONFIG_NETDATA_PUBLISHER_MAX_PREFIX_ENTRIES`.
 */
otError otNetDataPublishExternalRoute(otInstance *aInstance, const otExternalRouteConfig *aConfig);

/**
 * This function indicates whether or not currently a published prefix entry (on-mesh or external route) is added to
 * the Thread Network Data.
 *
 * This function requires the feature `OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE` to be enabled.
 *
 * @param[in] aInstance  A pointer to an OpenThread instance.
 * @param[in] aPrefix    A pointer to the prefix (MUST NOT be NULL).
 *
 * @retval TRUE    The published prefix entry is added to the Thread Network Data.
 * @retval FALSE   The entry is not added to Thread Network Data or there is no entry to publish.
 *
 */
bool otNetDataIsPrefixAdded(otInstance *aInstance, const otIp6Prefix *aPrefix);

/**
 * This function sets a callback for notifying when a published prefix entry is actually added to or removed from
 * the Thread Network Data.
 *
 * A subsequent call to this function replaces any previously set callback function.
 *
 * This function requires the feature `OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE` to be enabled.
 *
 * @param[in] aInstance        A pointer to an OpenThread instance.
 * @param[in] aCallback        The callback function pointer (can be NULL if not needed).
 * @param[in] aContext         A pointer to application-specific context (used when @p aCallback is invoked).
 *
 */
void otNetDataSetPrefixPublisherCallback(otInstance *                     aInstance,
                                         otNetDataPrefixPublisherCallback aCallback,
                                         void *                           aContext);

/**
 * This function unpublishes a previously published prefix (on-mesh or external route).
 *
 * This function requires the feature `OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE` to be enabled.
 *
 * @param[in] aInstance          A pointer to an OpenThread instance.
 * @param[in] aPrefix            The prefix to unpublish (MUST NOT be NULL).
 *
 * @retval OT_ERROR_NONE         The prefix was unpublished successfully.
 * @retval OT_ERROR_NOT_FOUND    Could not find the prefix in the published list.
 *
 */
otError otNetDataUnpublishPrefix(otInstance *aInstance, const otIp6Prefix *aPrefix);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_NETDATA_PUBLISHER_H_
