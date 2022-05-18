/*
 *  Copyright (c) 2016, The OpenThread Authors.
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
 *   This file implements the OpenThread IPv6 API.
 */

#include "openthread-core-config.h"

#include <openthread/ip6.h>

#include "common/as_core_type.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#if OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE
#include "utils/slaac_address.hpp"
#endif

using namespace ot;

otError otIp6SetEnabled(otInstance *aInstance, bool aEnabled)
{
    Error     error    = kErrorNone;
    Instance &instance = AsCoreType(aInstance);

#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE
    VerifyOrExit(!instance.Get<Mac::LinkRaw>().IsEnabled(), error = kErrorInvalidState);
#endif

    if (aEnabled)
    {
        instance.Get<ThreadNetif>().Up();
    }
    else
    {
        instance.Get<ThreadNetif>().Down();
    }

#if OPENTHREAD_CONFIG_LINK_RAW_ENABLE
exit:
#endif
    return error;
}

bool otIp6IsEnabled(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<ThreadNetif>().IsUp();
}

const otNetifAddress *otIp6GetUnicastAddresses(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<ThreadNetif>().GetUnicastAddresses().GetHead();
}

otError otIp6AddUnicastAddress(otInstance *aInstance, const otNetifAddress *aAddress)
{
    return AsCoreType(aInstance).Get<ThreadNetif>().AddExternalUnicastAddress(AsCoreType(aAddress));
}

otError otIp6RemoveUnicastAddress(otInstance *aInstance, const otIp6Address *aAddress)
{
    return AsCoreType(aInstance).Get<ThreadNetif>().RemoveExternalUnicastAddress(AsCoreType(aAddress));
}

const otNetifMulticastAddress *otIp6GetMulticastAddresses(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<ThreadNetif>().GetMulticastAddresses().GetHead();
}

otError otIp6SubscribeMulticastAddress(otInstance *aInstance, const otIp6Address *aAddress)
{
    return AsCoreType(aInstance).Get<ThreadNetif>().SubscribeExternalMulticast(AsCoreType(aAddress));
}

otError otIp6UnsubscribeMulticastAddress(otInstance *aInstance, const otIp6Address *aAddress)
{
    return AsCoreType(aInstance).Get<ThreadNetif>().UnsubscribeExternalMulticast(AsCoreType(aAddress));
}

bool otIp6IsMulticastPromiscuousEnabled(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<ThreadNetif>().IsMulticastPromiscuousEnabled();
}

void otIp6SetMulticastPromiscuousEnabled(otInstance *aInstance, bool aEnabled)
{
    AsCoreType(aInstance).Get<ThreadNetif>().SetMulticastPromiscuous(aEnabled);
}

void otIp6SetReceiveCallback(otInstance *aInstance, otIp6ReceiveCallback aCallback, void *aCallbackContext)
{
    AsCoreType(aInstance).Get<Ip6::Ip6>().SetReceiveDatagramCallback(aCallback, aCallbackContext);
}

void otIp6SetAddressCallback(otInstance *aInstance, otIp6AddressCallback aCallback, void *aCallbackContext)
{
    AsCoreType(aInstance).Get<ThreadNetif>().SetAddressCallback(aCallback, aCallbackContext);
}

bool otIp6IsReceiveFilterEnabled(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Ip6::Ip6>().IsReceiveIp6FilterEnabled();
}

void otIp6SetReceiveFilterEnabled(otInstance *aInstance, bool aEnabled)
{
    AsCoreType(aInstance).Get<Ip6::Ip6>().SetReceiveIp6FilterEnabled(aEnabled);
}

otError otIp6Send(otInstance *aInstance, otMessage *aMessage)
{
    return AsCoreType(aInstance).Get<Ip6::Ip6>().SendRaw(AsCoreType(aMessage));
}

otMessage *otIp6NewMessage(otInstance *aInstance, const otMessageSettings *aSettings)
{
    return AsCoreType(aInstance).Get<Ip6::Ip6>().NewMessage(0, Message::Settings::From(aSettings));
}

otMessage *otIp6NewMessageFromBuffer(otInstance *             aInstance,
                                     const uint8_t *          aData,
                                     uint16_t                 aDataLength,
                                     const otMessageSettings *aSettings)
{
    return (aSettings != nullptr)
               ? AsCoreType(aInstance).Get<Ip6::Ip6>().NewMessage(aData, aDataLength, AsCoreType(aSettings))
               : AsCoreType(aInstance).Get<Ip6::Ip6>().NewMessage(aData, aDataLength);
}

otError otIp6AddUnsecurePort(otInstance *aInstance, uint16_t aPort)
{
    return AsCoreType(aInstance).Get<Ip6::Filter>().AddUnsecurePort(aPort);
}

otError otIp6RemoveUnsecurePort(otInstance *aInstance, uint16_t aPort)
{
    return AsCoreType(aInstance).Get<Ip6::Filter>().RemoveUnsecurePort(aPort);
}

void otIp6RemoveAllUnsecurePorts(otInstance *aInstance)
{
    AsCoreType(aInstance).Get<Ip6::Filter>().RemoveAllUnsecurePorts();
}

const uint16_t *otIp6GetUnsecurePorts(otInstance *aInstance, uint8_t *aNumEntries)
{
    return AsCoreType(aInstance).Get<Ip6::Filter>().GetUnsecurePorts(*aNumEntries);
}

bool otIp6IsAddressEqual(const otIp6Address *aFirst, const otIp6Address *aSecond)
{
    return AsCoreType(aFirst) == AsCoreType(aSecond);
}

bool otIp6ArePrefixesEqual(const otIp6Prefix *aFirst, const otIp6Prefix *aSecond)
{
    return AsCoreType(aFirst) == AsCoreType(aSecond);
}

otError otIp6AddressFromString(const char *aString, otIp6Address *aAddress)
{
    return AsCoreType(aAddress).FromString(aString);
}

void otIp6AddressToString(const otIp6Address *aAddress, char *aBuffer, uint16_t aSize)
{
    AsCoreType(aAddress).ToString(aBuffer, aSize);
}

void otIp6SockAddrToString(const otSockAddr *aSockAddr, char *aBuffer, uint16_t aSize)
{
    AsCoreType(aSockAddr).ToString(aBuffer, aSize);
}

void otIp6PrefixToString(const otIp6Prefix *aPrefix, char *aBuffer, uint16_t aSize)
{
    AsCoreType(aPrefix).ToString(aBuffer, aSize);
}

uint8_t otIp6PrefixMatch(const otIp6Address *aFirst, const otIp6Address *aSecond)
{
    OT_ASSERT(aFirst != nullptr && aSecond != nullptr);

    return AsCoreType(aFirst).PrefixMatch(AsCoreType(aSecond));
}

bool otIp6IsAddressUnspecified(const otIp6Address *aAddress)
{
    return AsCoreType(aAddress).IsUnspecified();
}

otError otIp6SelectSourceAddress(otInstance *aInstance, otMessageInfo *aMessageInfo)
{
    Error                             error = kErrorNone;
    const Ip6::Netif::UnicastAddress *netifAddr;

    netifAddr = AsCoreType(aInstance).Get<Ip6::Ip6>().SelectSourceAddress(AsCoreType(aMessageInfo));
    VerifyOrExit(netifAddr != nullptr, error = kErrorNotFound);
    aMessageInfo->mSockAddr = netifAddr->GetAddress();

exit:
    return error;
}

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_TMF_PROXY_MLR_ENABLE && OPENTHREAD_CONFIG_COMMISSIONER_ENABLE
otError otIp6RegisterMulticastListeners(otInstance *                            aInstance,
                                        const otIp6Address *                    aAddresses,
                                        uint8_t                                 aAddressNum,
                                        const uint32_t *                        aTimeout,
                                        otIp6RegisterMulticastListenersCallback aCallback,
                                        void *                                  aContext)
{
    return AsCoreType(aInstance).Get<MlrManager>().RegisterMulticastListeners(aAddresses, aAddressNum, aTimeout,
                                                                              aCallback, aContext);
}
#endif

#if OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE

bool otIp6IsSlaacEnabled(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Utils::Slaac>().IsEnabled();
}

void otIp6SetSlaacEnabled(otInstance *aInstance, bool aEnabled)
{
    Instance &instance = AsCoreType(aInstance);

    if (aEnabled)
    {
        instance.Get<Utils::Slaac>().Enable();
    }
    else
    {
        instance.Get<Utils::Slaac>().Disable();
    }
}

void otIp6SetSlaacPrefixFilter(otInstance *aInstance, otIp6SlaacPrefixFilter aFilter)
{
    AsCoreType(aInstance).Get<Utils::Slaac>().SetFilter(aFilter);
}

#endif // OPENTHREAD_CONFIG_IP6_SLAAC_ENABLE

#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE

otError otIp6SetMeshLocalIid(otInstance *aInstance, const otIp6InterfaceIdentifier *aIid)
{
    return AsCoreType(aInstance).Get<Mle::MleRouter>().SetMeshLocalIid(AsCoreType(aIid));
}

#endif

const char *otIp6ProtoToString(uint8_t aIpProto)
{
    return Ip6::Ip6::IpProtoToString(aIpProto);
}
