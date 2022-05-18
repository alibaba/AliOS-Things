/*
 *  Copyright (c) 2016-21, The OpenThread Authors.
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
 *   This file implements Thread Network Data related types and constants.
 */

#include "network_data_types.hpp"

#include "common/instance.hpp"
#include "thread/network_data_tlvs.hpp"

namespace ot {
namespace NetworkData {

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

static bool IsPrefixValid(Instance &aInstance, const Ip6::Prefix &aPrefix)
{
    // Check that prefix length is within the valid range and the prefix
    // does not overlap with the mesh-local prefix.

    return aPrefix.IsValid() && !aPrefix.ContainsPrefix(aInstance.Get<Mle::Mle>().GetMeshLocalPrefix());
}

bool OnMeshPrefixConfig::IsValid(Instance &aInstance) const
{
    bool isValid = false;

    if (mDhcp && mSlaac)
    {
        // A valid prefix MUST NOT allow both DHCPv6 and SLAAC for
        // address configuration.
        ExitNow();
    }

    if (mSlaac)
    {
        // An IPv6 address prefix used for stateless auto-configuration
        // [RFC4862] of an IEEE 802.15.4 interface MUST have a length of
        // 64 bits.
        VerifyOrExit(GetPrefix().GetLength() == Ip6::NetworkPrefix::kLength);
    }

    VerifyOrExit(IsRoutePreferenceValid(mPreference));
    VerifyOrExit(IsPrefixValid(aInstance, GetPrefix()));
    VerifyOrExit(GetPrefix().GetLength() > 0);

    isValid = true;

exit:
    return isValid;
}

uint16_t OnMeshPrefixConfig::ConvertToTlvFlags(void) const
{
    uint16_t flags = 0;

    if (mPreferred)
    {
        flags |= BorderRouterEntry::kPreferredFlag;
    }

    if (mSlaac)
    {
        flags |= BorderRouterEntry::kSlaacFlag;
    }

    if (mDhcp)
    {
        flags |= BorderRouterEntry::kDhcpFlag;
    }

    if (mConfigure)
    {
        flags |= BorderRouterEntry::kConfigureFlag;
    }

    if (mDefaultRoute)
    {
        flags |= BorderRouterEntry::kDefaultRouteFlag;
    }

    if (mOnMesh)
    {
        flags |= BorderRouterEntry::kOnMeshFlag;
    }

    if (mNdDns)
    {
        flags |= BorderRouterEntry::kNdDnsFlag;
    }

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
    if (mDp)
    {
        flags |= BorderRouterEntry::kDpFlag;
    }
#endif

    flags |= (static_cast<uint16_t>(RoutePreferenceToValue(mPreference)) << BorderRouterEntry::kPreferenceOffset);

    return flags;
}

#endif // OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

void OnMeshPrefixConfig::SetFrom(const PrefixTlv &        aPrefixTlv,
                                 const BorderRouterTlv &  aBorderRouterTlv,
                                 const BorderRouterEntry &aBorderRouterEntry)
{
    Clear();

    aPrefixTlv.CopyPrefixTo(GetPrefix());
    SetFromTlvFlags(aBorderRouterEntry.GetFlags());
    mRloc16 = aBorderRouterEntry.GetRloc();
    mStable = aBorderRouterTlv.IsStable();
}

void OnMeshPrefixConfig::SetFromTlvFlags(uint16_t aFlags)
{
    mPreferred    = ((aFlags & BorderRouterEntry::kPreferredFlag) != 0);
    mSlaac        = ((aFlags & BorderRouterEntry::kSlaacFlag) != 0);
    mDhcp         = ((aFlags & BorderRouterEntry::kDhcpFlag) != 0);
    mConfigure    = ((aFlags & BorderRouterEntry::kConfigureFlag) != 0);
    mDefaultRoute = ((aFlags & BorderRouterEntry::kDefaultRouteFlag) != 0);
    mOnMesh       = ((aFlags & BorderRouterEntry::kOnMeshFlag) != 0);
    mNdDns        = ((aFlags & BorderRouterEntry::kNdDnsFlag) != 0);
    mDp           = ((aFlags & BorderRouterEntry::kDpFlag) != 0);
    mPreference   = RoutePreferenceFromValue(static_cast<uint8_t>(aFlags >> BorderRouterEntry::kPreferenceOffset));
}

#if OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE
bool ExternalRouteConfig::IsValid(Instance &aInstance) const
{
    bool isValid = false;

    if (mNat64)
    {
        VerifyOrExit(GetPrefix().IsValidNat64());
    }

    VerifyOrExit(IsRoutePreferenceValid(mPreference));
    VerifyOrExit(IsPrefixValid(aInstance, GetPrefix()));

    isValid = true;

exit:
    return isValid;
}

uint8_t ExternalRouteConfig::ConvertToTlvFlags(void) const
{
    uint8_t flags = 0;

    if (mNat64)
    {
        flags |= HasRouteEntry::kNat64Flag;
    }

    flags |= (RoutePreferenceToValue(mPreference) << HasRouteEntry::kPreferenceOffset);

    return flags;
}

#endif // OPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE

void ExternalRouteConfig::SetFrom(Instance &           aInstance,
                                  const PrefixTlv &    aPrefixTlv,
                                  const HasRouteTlv &  aHasRouteTlv,
                                  const HasRouteEntry &aHasRouteEntry)
{
    Clear();

    aPrefixTlv.CopyPrefixTo(GetPrefix());
    SetFromTlvFlags(aHasRouteEntry.GetFlags());
    mStable              = aHasRouteTlv.IsStable();
    mRloc16              = aHasRouteEntry.GetRloc();
    mNextHopIsThisDevice = (aHasRouteEntry.GetRloc() == aInstance.Get<Mle::MleRouter>().GetRloc16());
}

void ExternalRouteConfig::SetFromTlvFlags(uint8_t aFlags)
{
    mNat64      = ((aFlags & HasRouteEntry::kNat64Flag) != 0);
    mPreference = RoutePreferenceFromValue(aFlags >> HasRouteEntry::kPreferenceOffset);
}

bool ServiceConfig::ServerConfig::operator==(const ServerConfig &aOther) const
{
    return (mStable == aOther.mStable) && (mServerDataLength == aOther.mServerDataLength) &&
           (memcmp(mServerData, aOther.mServerData, mServerDataLength) == 0);
}

void ServiceConfig::ServerConfig::SetFrom(const ServerTlv &aServerTlv)
{
    ServerData serverData;

    aServerTlv.GetServerData(serverData);
    mStable           = aServerTlv.IsStable();
    mRloc16           = aServerTlv.GetServer16();
    mServerDataLength = serverData.GetLength();
    serverData.CopyBytesTo(mServerData);
}

bool ServiceConfig::operator==(const ServiceConfig &aOther) const
{
    return (mEnterpriseNumber == aOther.mEnterpriseNumber) && (mServiceDataLength == aOther.mServiceDataLength) &&
           (memcmp(mServiceData, aOther.mServiceData, mServiceDataLength) == 0) &&
           (GetServerConfig() == aOther.GetServerConfig());
}

void ServiceConfig::SetFrom(const ServiceTlv &aServiceTlv, const ServerTlv &aServerTlv)
{
    ServiceData serviceData;

    Clear();

    aServiceTlv.GetServiceData(serviceData);
    mServiceId         = aServiceTlv.GetServiceId();
    mEnterpriseNumber  = aServiceTlv.GetEnterpriseNumber();
    mServiceDataLength = serviceData.GetLength();
    serviceData.CopyBytesTo(mServiceData);
    GetServerConfig().SetFrom(aServerTlv);
}

} // namespace NetworkData
} // namespace ot
