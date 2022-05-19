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
 *   This file implements the OpenThread Border Router API.
 */

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

#include <openthread/border_router.h>

#include "border_router/routing_manager.hpp"
#include "common/as_core_type.hpp"
#include "common/debug.hpp"

using namespace ot;

#if OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE
otError otBorderRoutingInit(otInstance *aInstance, uint32_t aInfraIfIndex, bool aInfraIfIsRunning)
{
    return AsCoreType(aInstance).Get<BorderRouter::RoutingManager>().Init(aInfraIfIndex, aInfraIfIsRunning);
}

otError otBorderRoutingSetEnabled(otInstance *aInstance, bool aEnabled)
{
    return AsCoreType(aInstance).Get<BorderRouter::RoutingManager>().SetEnabled(aEnabled);
}

otError otBorderRoutingGetOmrPrefix(otInstance *aInstance, otIp6Prefix *aPrefix)
{
    return AsCoreType(aInstance).Get<BorderRouter::RoutingManager>().GetOmrPrefix(AsCoreType(aPrefix));
}

otError otBorderRoutingGetOnLinkPrefix(otInstance *aInstance, otIp6Prefix *aPrefix)
{
    return AsCoreType(aInstance).Get<BorderRouter::RoutingManager>().GetOnLinkPrefix(AsCoreType(aPrefix));
}

#endif

otError otBorderRouterGetNetData(otInstance *aInstance, bool aStable, uint8_t *aData, uint8_t *aDataLength)
{
    return AsCoreType(aInstance).Get<NetworkData::Local>().CopyNetworkData(aStable, aData, *aDataLength);
}

otError otBorderRouterAddOnMeshPrefix(otInstance *aInstance, const otBorderRouterConfig *aConfig)
{
    Error error = kErrorNone;

    OT_ASSERT(aConfig != nullptr);

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    if (aConfig->mDp)
    {
        error = AsCoreType(aInstance).Get<BackboneRouter::Local>().SetDomainPrefix(AsCoreType(aConfig));
    }
    else
#endif
    {
        error = AsCoreType(aInstance).Get<NetworkData::Local>().AddOnMeshPrefix(AsCoreType(aConfig));
    }

    return error;
}

otError otBorderRouterRemoveOnMeshPrefix(otInstance *aInstance, const otIp6Prefix *aPrefix)
{
    Error error = kErrorNone;

    OT_ASSERT(aPrefix != nullptr);

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    error = AsCoreType(aInstance).Get<BackboneRouter::Local>().RemoveDomainPrefix(AsCoreType(aPrefix));

    if (error == kErrorNotFound)
#endif
    {
        error = AsCoreType(aInstance).Get<NetworkData::Local>().RemoveOnMeshPrefix(AsCoreType(aPrefix));
    }

    return error;
}

otError otBorderRouterGetNextOnMeshPrefix(otInstance *           aInstance,
                                          otNetworkDataIterator *aIterator,
                                          otBorderRouterConfig * aConfig)
{
    OT_ASSERT(aIterator != nullptr && aConfig != nullptr);

    return AsCoreType(aInstance).Get<NetworkData::Local>().GetNextOnMeshPrefix(*aIterator, AsCoreType(aConfig));
}

otError otBorderRouterAddRoute(otInstance *aInstance, const otExternalRouteConfig *aConfig)
{
    OT_ASSERT(aConfig != nullptr);

    return AsCoreType(aInstance).Get<NetworkData::Local>().AddHasRoutePrefix(AsCoreType(aConfig));
}

otError otBorderRouterRemoveRoute(otInstance *aInstance, const otIp6Prefix *aPrefix)
{
    OT_ASSERT(aPrefix != nullptr);

    return AsCoreType(aInstance).Get<NetworkData::Local>().RemoveHasRoutePrefix(AsCoreType(aPrefix));
}

otError otBorderRouterGetNextRoute(otInstance *           aInstance,
                                   otNetworkDataIterator *aIterator,
                                   otExternalRouteConfig *aConfig)
{
    OT_ASSERT(aIterator != nullptr && aConfig != nullptr);

    return AsCoreType(aInstance).Get<NetworkData::Local>().GetNextExternalRoute(*aIterator, AsCoreType(aConfig));
}

otError otBorderRouterRegister(otInstance *aInstance)
{
    AsCoreType(aInstance).Get<NetworkData::Notifier>().HandleServerDataUpdated();

    return kErrorNone;
}

#endif // OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
