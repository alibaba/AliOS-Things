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
 *   This file implements the OpenThread Network Data Publisher API.
 */

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_NETDATA_PUBLISHER_ENABLE

#include <openthread/netdata_publisher.h>

#include "common/as_core_type.hpp"
#include "common/locator_getters.hpp"

using namespace ot;

#if OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE

void otNetDataPublishDnsSrpServiceAnycast(otInstance *aInstance, uint8_t aSequenceNumber)
{
    AsCoreType(aInstance).Get<NetworkData::Publisher>().PublishDnsSrpServiceAnycast(aSequenceNumber);
}

void otNetDataPublishDnsSrpServiceUnicast(otInstance *aInstance, const otIp6Address *aAddress, uint16_t aPort)
{
    AsCoreType(aInstance).Get<NetworkData::Publisher>().PublishDnsSrpServiceUnicast(AsCoreType(aAddress), aPort);
}

void otNetDataPublishDnsSrpServiceUnicastMeshLocalEid(otInstance *aInstance, uint16_t aPort)
{
    AsCoreType(aInstance).Get<NetworkData::Publisher>().PublishDnsSrpServiceUnicast(aPort);
}

bool otNetDataIsDnsSrpServiceAdded(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<NetworkData::Publisher>().IsDnsSrpServiceAdded();
}

void otNetDataSetDnsSrpServicePublisherCallback(otInstance *                            aInstance,
                                                otNetDataDnsSrpServicePublisherCallback aCallback,
                                                void *                                  aContext)
{
    AsCoreType(aInstance).Get<NetworkData::Publisher>().SetDnsSrpServiceCallback(aCallback, aContext);
}

void otNetDataUnpublishDnsSrpService(otInstance *aInstance)
{
    AsCoreType(aInstance).Get<NetworkData::Publisher>().UnpublishDnsSrpService();
}

#endif // OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

otError otNetDataPublishOnMeshPrefix(otInstance *aInstance, const otBorderRouterConfig *aConfig)
{
    return AsCoreType(aInstance).Get<NetworkData::Publisher>().PublishOnMeshPrefix(AsCoreType(aConfig));
}

otError otNetDataPublishExternalRoute(otInstance *aInstance, const otExternalRouteConfig *aConfig)
{
    return AsCoreType(aInstance).Get<NetworkData::Publisher>().PublishExternalRoute(AsCoreType(aConfig));
}

bool otNetDataIsPrefixAdded(otInstance *aInstance, const otIp6Prefix *aPrefix)
{
    return AsCoreType(aInstance).Get<NetworkData::Publisher>().IsPrefixAdded(AsCoreType(aPrefix));
}

void otNetDataSetPrefixPublisherCallback(otInstance *                     aInstance,
                                         otNetDataPrefixPublisherCallback aCallback,
                                         void *                           aContext)
{
    return AsCoreType(aInstance).Get<NetworkData::Publisher>().SetPrefixCallback(aCallback, aContext);
}

otError otNetDataUnpublishPrefix(otInstance *aInstance, const otIp6Prefix *aPrefix)
{
    return AsCoreType(aInstance).Get<NetworkData::Publisher>().UnpublishPrefix(AsCoreType(aPrefix));
}

#endif // OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

#endif // OPENTHREAD_CONFIG_NETDATA_PUBLISHER_ENABLE
