/*
 *  Copyright (c) 2017, The OpenThread Authors.
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
 *   This file implements the OpenThread DNSv6 API.
 */

#include "openthread-core-config.h"

#include <openthread/dns_client.h>

#include "common/as_core_type.hpp"

using namespace ot;

void otDnsInitTxtEntryIterator(otDnsTxtEntryIterator *aIterator, const uint8_t *aTxtData, uint16_t aTxtDataLength)
{
    AsCoreType(aIterator).Init(aTxtData, aTxtDataLength);
}

otError otDnsGetNextTxtEntry(otDnsTxtEntryIterator *aIterator, otDnsTxtEntry *aEntry)
{
    return AsCoreType(aIterator).GetNextEntry(AsCoreType(aEntry));
}

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
void otDnsSetNameCompressionEnabled(bool aEnabled)
{
    Instance::SetDnsNameCompressionEnabled(aEnabled);
}

bool otDnsIsNameCompressionEnabled(void)
{
    return Instance::IsDnsNameCompressionEnabled();
}
#endif

#if OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE

const otDnsQueryConfig *otDnsClientGetDefaultConfig(otInstance *aInstance)
{
    return &AsCoreType(aInstance).Get<Dns::Client>().GetDefaultConfig();
}

void otDnsClientSetDefaultConfig(otInstance *aInstance, const otDnsQueryConfig *aConfig)
{
    if (aConfig != nullptr)
    {
        AsCoreType(aInstance).Get<Dns::Client>().SetDefaultConfig(AsCoreType(aConfig));
    }
    else
    {
        AsCoreType(aInstance).Get<Dns::Client>().ResetDefaultConfig();
    }
}

otError otDnsClientResolveAddress(otInstance *            aInstance,
                                  const char *            aHostName,
                                  otDnsAddressCallback    aCallback,
                                  void *                  aContext,
                                  const otDnsQueryConfig *aConfig)
{
    return AsCoreType(aInstance).Get<Dns::Client>().ResolveAddress(aHostName, aCallback, aContext,
                                                                   AsCoreTypePtr(aConfig));
}

otError otDnsAddressResponseGetHostName(const otDnsAddressResponse *aResponse,
                                        char *                      aNameBuffer,
                                        uint16_t                    aNameBufferSize)
{
    return AsCoreType(aResponse).GetHostName(aNameBuffer, aNameBufferSize);
}

otError otDnsAddressResponseGetAddress(const otDnsAddressResponse *aResponse,
                                       uint16_t                    aIndex,
                                       otIp6Address *              aAddress,
                                       uint32_t *                  aTtl)
{
    uint32_t ttl;

    return AsCoreType(aResponse).GetAddress(aIndex, AsCoreType(aAddress), (aTtl != nullptr) ? *aTtl : ttl);
}

#if OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

otError otDnsClientBrowse(otInstance *            aInstance,
                          const char *            aServiceName,
                          otDnsBrowseCallback     aCallback,
                          void *                  aContext,
                          const otDnsQueryConfig *aConfig)
{
    return AsCoreType(aInstance).Get<Dns::Client>().Browse(aServiceName, aCallback, aContext, AsCoreTypePtr(aConfig));
}

otError otDnsBrowseResponseGetServiceName(const otDnsBrowseResponse *aResponse,
                                          char *                     aNameBuffer,
                                          uint16_t                   aNameBufferSize)
{
    return AsCoreType(aResponse).GetServiceName(aNameBuffer, aNameBufferSize);
}

otError otDnsBrowseResponseGetServiceInstance(const otDnsBrowseResponse *aResponse,
                                              uint16_t                   aIndex,
                                              char *                     aLabelBuffer,
                                              uint8_t                    aLabelBufferSize)
{
    return AsCoreType(aResponse).GetServiceInstance(aIndex, aLabelBuffer, aLabelBufferSize);
}

otError otDnsBrowseResponseGetServiceInfo(const otDnsBrowseResponse *aResponse,
                                          const char *               aInstanceLabel,
                                          otDnsServiceInfo *         aServiceInfo)
{
    return AsCoreType(aResponse).GetServiceInfo(aInstanceLabel, AsCoreType(aServiceInfo));
}

otError otDnsBrowseResponseGetHostAddress(const otDnsBrowseResponse *aResponse,
                                          const char *               aHostName,
                                          uint16_t                   aIndex,
                                          otIp6Address *             aAddress,
                                          uint32_t *                 aTtl)
{
    uint32_t ttl;

    return AsCoreType(aResponse).GetHostAddress(aHostName, aIndex, AsCoreType(aAddress), aTtl != nullptr ? *aTtl : ttl);
}

otError otDnsClientResolveService(otInstance *            aInstance,
                                  const char *            aInstanceLabel,
                                  const char *            aServiceName,
                                  otDnsServiceCallback    aCallback,
                                  void *                  aContext,
                                  const otDnsQueryConfig *aConfig)
{
    return AsCoreType(aInstance).Get<Dns::Client>().ResolveService(aInstanceLabel, aServiceName, aCallback, aContext,
                                                                   AsCoreTypePtr(aConfig));
}

otError otDnsServiceResponseGetServiceName(const otDnsServiceResponse *aResponse,
                                           char *                      aLabelBuffer,
                                           uint8_t                     aLabelBufferSize,
                                           char *                      aNameBuffer,
                                           uint16_t                    aNameBufferSize)
{
    return AsCoreType(aResponse).GetServiceName(aLabelBuffer, aLabelBufferSize, aNameBuffer, aNameBufferSize);
}

otError otDnsServiceResponseGetServiceInfo(const otDnsServiceResponse *aResponse, otDnsServiceInfo *aServiceInfo)
{
    return AsCoreType(aResponse).GetServiceInfo(AsCoreType(aServiceInfo));
}

otError otDnsServiceResponseGetHostAddress(const otDnsServiceResponse *aResponse,
                                           const char *                aHostName,
                                           uint16_t                    aIndex,
                                           otIp6Address *              aAddress,
                                           uint32_t *                  aTtl)
{
    uint32_t ttl;

    return AsCoreType(aResponse).GetHostAddress(aHostName, aIndex, AsCoreType(aAddress),
                                                (aTtl != nullptr) ? *aTtl : ttl);
}

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_SERVICE_DISCOVERY_ENABLE

#endif // OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
