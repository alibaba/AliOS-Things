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
 *  This file defines the DNS-SD server APIs.
 */

#ifndef OPENTHREAD_DNSSD_SERVER_H_
#define OPENTHREAD_DNSSD_SERVER_H_

#include <stdint.h>

#include <openthread/dns.h>
#include <openthread/error.h>
#include <openthread/ip6.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-dnssd-server
 *
 * @brief
 *   This module includes APIs for DNS-SD server.
 *
 * @{
 *
 */

/**
 * This function is called when a DNS-SD query subscribes one of:
 *      1. a service name.
 *      2. a service instance name.
 *      3. a host name.
 *
 * The DNS-SD query implementation is responsible for identifying what @p aFullName is.
 * If @p aFullName is a service name or service instance name, the DNS-SD query implementation should discover
 * corresponding service instance information and notify the DNS-SD server using
 * `otDnssdQueryHandleDiscoveredServiceInstance`.
 * If @p aFullName is a host name, the DNS-SD query implementation should
 * discover the host information and notify the DNS-SD server using `otDnssdQueryHandleDiscoveredHost`.
 *
 * @note There can be multiple subscription to the same name. DNS-SD query implementation should record the number of
 * active subscriptions and stop notifying when there is no active subscription for @p aFullName.
 *
 * @param[in] aContext      A pointer to the application-specific context.
 * @param[in] aFullName     The null-terminated full service name (e.g. "_ipps._tcp.default.service.arpa."),
 *                          or full service instance name (e.g. "OpenThread._ipps._tcp.default.service.arpa."),
 *                          or full host name (e.g. "ot-host.default.service.arpa.").
 *
 * @sa otDnssdQueryHandleDiscoveredServiceInstance
 * @sa otDnssdQueryHandleDiscoveredHost
 *
 */
typedef void (*otDnssdQuerySubscribeCallback)(void *aContext, const char *aFullName);

/**
 * This function is called when a DNS-SD query unsubscribes one of:
 *      1. a service name.
 *      2. a service instance name.
 *      3. a host name.
 *
 * The DNS-SD query implementation is responsible for identifying what @p aFullName is.
 *
 * @note There can be multiple subscription to the same name. DNS-SD query implementation should record the number of
 * active subscriptions and stop notifying when there is no active subscription for @p aFullName.
 *
 * @param[in] aContext      A pointer to the application-specific context.
 * @param[in] aFullName     The null-terminated full service name (e.g. "_ipps._tcp.default.service.arpa."), or
 *                          full service instance name (e.g. "OpenThread._ipps._tcp.default.service.arpa.").
 *
 */
typedef void (*otDnssdQueryUnsubscribeCallback)(void *aContext, const char *aFullName);

/**
 * This opaque type represents a DNS-SD query.
 *
 */
typedef void otDnssdQuery;

/**
 * This structure represents information of a discovered service instance for a DNS-SD query.
 *
 */
typedef struct otDnssdServiceInstanceInfo
{
    const char *        mFullName;   ///< Full instance name (e.g. "OpenThread._ipps._tcp.default.service.arpa.").
    const char *        mHostName;   ///< Host name (e.g. "ot-host.default.service.arpa.").
    uint8_t             mAddressNum; ///< Number of host IPv6 addresses.
    const otIp6Address *mAddresses;  ///< Host IPv6 addresses.
    uint16_t            mPort;       ///< Service port.
    uint16_t            mPriority;   ///< Service priority.
    uint16_t            mWeight;     ///< Service weight.
    uint16_t            mTxtLength;  ///< Service TXT RDATA length.
    const uint8_t *     mTxtData;    ///< Service TXT RDATA.
    uint32_t            mTtl;        ///< Service TTL (in seconds).
} otDnssdServiceInstanceInfo;

/**
 * This structure represents information of a discovered host for a DNS-SD query.
 *
 */
typedef struct otDnssdHostInfo
{
    uint8_t             mAddressNum; ///< Number of host IPv6 addresses.
    const otIp6Address *mAddresses;  ///< Host IPv6 addresses.
    uint32_t            mTtl;        ///< Service TTL (in seconds).
} otDnssdHostInfo;

/**
 * This enumeration specifies a DNS-SD query type.
 *
 */
typedef enum
{
    OT_DNSSD_QUERY_TYPE_NONE         = 0, ///< Service type unspecified.
    OT_DNSSD_QUERY_TYPE_BROWSE       = 1, ///< Service type browse service.
    OT_DNSSD_QUERY_TYPE_RESOLVE      = 2, ///< Service type resolve service instance.
    OT_DNSSD_QUERY_TYPE_RESOLVE_HOST = 3, ///< Service type resolve hostname.
} otDnssdQueryType;

/**
 * This function sets DNS-SD server query callbacks.
 *
 * The DNS-SD server calls @p aSubscribe to subscribe to a service or service instance to resolve a DNS-SD query and @p
 * aUnsubscribe to unsubscribe when the query is resolved or timeout.
 *
 * @note @p aSubscribe and @p aUnsubscribe must be both set or unset.
 *
 * @param[in] aInstance     The OpenThread instance structure.
 * @param[in] aSubscribe    A pointer to the callback function to subscribe a service or service instance.
 * @param[in] aUnsubscribe  A pointer to the callback function to unsubscribe a service or service instance.
 * @param[in] aContext      A pointer to the application-specific context.
 *
 */
void otDnssdQuerySetCallbacks(otInstance *                    aInstance,
                              otDnssdQuerySubscribeCallback   aSubscribe,
                              otDnssdQueryUnsubscribeCallback aUnsubscribe,
                              void *                          aContext);

/**
 * This function notifies a discovered service instance.
 *
 * The external query resolver (e.g. Discovery Proxy) should call this function to notify OpenThread core of the
 * subscribed services or service instances.
 *
 * @note @p aInstanceInfo must not contain unspecified or link-local or loop-back or multicast IP addresses.
 *
 * @param[in] aInstance         The OpenThread instance structure.
 * @param[in] aServiceFullName  The null-terminated full service name.
 * @param[in] aInstanceInfo     A pointer to the discovered service instance information.
 *
 */
void otDnssdQueryHandleDiscoveredServiceInstance(otInstance *                aInstance,
                                                 const char *                aServiceFullName,
                                                 otDnssdServiceInstanceInfo *aInstanceInfo);
/**
 * This function notifies a discovered host.
 *
 * The external query resolver (e.g. Discovery Proxy) should call this function to notify OpenThread core of the
 * subscribed hosts.
 *
 * @note @p aHostInfo must not contain unspecified or link-local or loop-back or multicast IP addresses.
 *
 * @param[in] aInstance         The OpenThread instance structure.
 * @param[in] aHostFullName     The null-terminated full host name.
 * @param[in] aHostInfo         A pointer to the discovered service instance information.
 *
 */
void otDnssdQueryHandleDiscoveredHost(otInstance *aInstance, const char *aHostFullName, otDnssdHostInfo *aHostInfo);

/**
 * This function acquires the next query in the DNS-SD server.
 *
 * @param[in] aInstance         The OpenThread instance structure.
 * @param[in] aQuery            The query pointer. Pass NULL to get the first query.
 *
 * @returns  A pointer to the query or NULL if no more queries.
 *
 */
const otDnssdQuery *otDnssdGetNextQuery(otInstance *aInstance, const otDnssdQuery *aQuery);

/**
 * This function acquires the DNS-SD query type and name for a specific query.
 *
 * @param[in]   aQuery            The query pointer acquired from `otDnssdGetNextQuery`.
 * @param[out]  aNameOutput       The name output buffer, which should be `OT_DNS_MAX_NAME_SIZE` bytes long.
 *
 * @returns The DNS-SD query type.
 *
 */
otDnssdQueryType otDnssdGetQueryTypeAndName(const otDnssdQuery *aQuery, char (*aNameOutput)[OT_DNS_MAX_NAME_SIZE]);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_DNSSD_SERVER_H_
