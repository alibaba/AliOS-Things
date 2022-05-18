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
 *  This file defines the OpenThread Backbone Router API (for Thread 1.2 FTD with
 *  `OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE`).
 */

#include "openthread-core-config.h"

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE

#include <openthread/backbone_router_ftd.h>

#include "common/as_core_type.hpp"
#include "common/locator_getters.hpp"

using namespace ot;

void otBackboneRouterSetEnabled(otInstance *aInstance, bool aEnabled)
{
    return AsCoreType(aInstance).Get<BackboneRouter::Local>().SetEnabled(aEnabled);
}

otBackboneRouterState otBackboneRouterGetState(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<BackboneRouter::Local>().GetState();
}

void otBackboneRouterGetConfig(otInstance *aInstance, otBackboneRouterConfig *aConfig)
{
    OT_ASSERT(aConfig != nullptr);

    AsCoreType(aInstance).Get<BackboneRouter::Local>().GetConfig(*aConfig);
}

otError otBackboneRouterSetConfig(otInstance *aInstance, const otBackboneRouterConfig *aConfig)
{
    OT_ASSERT(aConfig != nullptr);

    return AsCoreType(aInstance).Get<BackboneRouter::Local>().SetConfig(*aConfig);
}

otError otBackboneRouterRegister(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<BackboneRouter::Local>().AddService(true /* Force registration */);
}

uint8_t otBackboneRouterGetRegistrationJitter(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<BackboneRouter::Local>().GetRegistrationJitter();
}

void otBackboneRouterSetRegistrationJitter(otInstance *aInstance, uint8_t aJitter)
{
    return AsCoreType(aInstance).Get<BackboneRouter::Local>().SetRegistrationJitter(aJitter);
}

otError otBackboneRouterGetDomainPrefix(otInstance *aInstance, otBorderRouterConfig *aConfig)
{
    OT_ASSERT(aConfig != nullptr);

    return AsCoreType(aInstance).Get<BackboneRouter::Local>().GetDomainPrefix(AsCoreType(aConfig));
}

void otBackboneRouterSetDomainPrefixCallback(otInstance *                         aInstance,
                                             otBackboneRouterDomainPrefixCallback aCallback,
                                             void *                               aContext)
{
    return AsCoreType(aInstance).Get<BackboneRouter::Local>().SetDomainPrefixCallback(aCallback, aContext);
}

#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE
void otBackboneRouterSetNdProxyCallback(otInstance *                    aInstance,
                                        otBackboneRouterNdProxyCallback aCallback,
                                        void *                          aContext)
{
    AsCoreType(aInstance).Get<BackboneRouter::NdProxyTable>().SetCallback(aCallback, aContext);
}

otError otBackboneRouterGetNdProxyInfo(otInstance *                 aInstance,
                                       const otIp6Address *         aDua,
                                       otBackboneRouterNdProxyInfo *aNdProxyInfo)
{
    return AsCoreType(aInstance).Get<BackboneRouter::NdProxyTable>().GetInfo(
        reinterpret_cast<const Ip6::Address &>(*aDua), *aNdProxyInfo);
}
#endif // OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE

#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE
void otBackboneRouterSetMulticastListenerCallback(otInstance *                              aInstance,
                                                  otBackboneRouterMulticastListenerCallback aCallback,
                                                  void *                                    aContext)
{
    AsCoreType(aInstance).Get<BackboneRouter::MulticastListenersTable>().SetCallback(aCallback, aContext);
}

otError otBackboneRouterMulticastListenerGetNext(otInstance *                           aInstance,
                                                 otChildIp6AddressIterator *            aIterator,
                                                 otBackboneRouterMulticastListenerInfo *aListenerInfo)
{
    OT_ASSERT(aIterator != nullptr);
    OT_ASSERT(aListenerInfo != nullptr);

    return AsCoreType(aInstance).Get<BackboneRouter::MulticastListenersTable>().GetNext(*aIterator, *aListenerInfo);
}
#endif

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE
void otBackboneRouterConfigNextDuaRegistrationResponse(otInstance *                    aInstance,
                                                       const otIp6InterfaceIdentifier *aMlIid,
                                                       uint8_t                         aStatus)
{
    AsCoreType(aInstance).Get<BackboneRouter::Manager>().ConfigNextDuaRegistrationResponse(AsCoreTypePtr(aMlIid),
                                                                                           aStatus);
}
#endif

#if OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE
void otBackboneRouterConfigNextMulticastListenerRegistrationResponse(otInstance *aInstance, uint8_t aStatus)
{
    OT_ASSERT(aStatus <= ThreadStatusTlv::kMlrStatusMax);

    AsCoreType(aInstance).Get<BackboneRouter::Manager>().ConfigNextMulticastListenerRegistrationResponse(
        static_cast<ThreadStatusTlv::MlrStatus>(aStatus));
}

void otBackboneRouterMulticastListenerClear(otInstance *aInstance)
{
    AsCoreType(aInstance).Get<BackboneRouter::MulticastListenersTable>().Clear();
}

otError otBackboneRouterMulticastListenerAdd(otInstance *aInstance, const otIp6Address *aAddress, uint32_t aTimeout)
{
    OT_ASSERT(aAddress != nullptr);

    if (aTimeout == 0)
    {
        BackboneRouter::BackboneRouterConfig config;
        AsCoreType(aInstance).Get<BackboneRouter::Local>().GetConfig(config);
        aTimeout = config.mMlrTimeout;
    }

    aTimeout =
        aTimeout > static_cast<uint32_t>(Mle::kMlrTimeoutMax) ? static_cast<uint32_t>(Mle::kMlrTimeoutMax) : aTimeout;
    aTimeout = Time::SecToMsec(aTimeout);

    return AsCoreType(aInstance).Get<BackboneRouter::MulticastListenersTable>().Add(AsCoreType(aAddress),
                                                                                    TimerMilli::GetNow() + aTimeout);
}
#endif // OPENTHREAD_CONFIG_BACKBONE_ROUTER_MULTICAST_ROUTING_ENABLE
#endif // OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE

#endif // OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
