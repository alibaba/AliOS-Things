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
 *   This file implements the OpenThread Thread API (for both FTD and MTD).
 */

#include "openthread-core-config.h"

#if OPENTHREAD_FTD || OPENTHREAD_MTD

#include <openthread/thread.h>

#include "common/as_core_type.hpp"
#include "common/debug.hpp"
#include "common/locator_getters.hpp"

using namespace ot;

uint32_t otThreadGetChildTimeout(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mle::MleRouter>().GetTimeout();
}

void otThreadSetChildTimeout(otInstance *aInstance, uint32_t aTimeout)
{
    AsCoreType(aInstance).Get<Mle::MleRouter>().SetTimeout(aTimeout);
}

const otExtendedPanId *otThreadGetExtendedPanId(otInstance *aInstance)
{
    return &AsCoreType(aInstance).Get<Mac::Mac>().GetExtendedPanId();
}

otError otThreadSetExtendedPanId(otInstance *aInstance, const otExtendedPanId *aExtendedPanId)
{
    Error                     error    = kErrorNone;
    Instance &                instance = AsCoreType(aInstance);
    const Mac::ExtendedPanId &extPanId = AsCoreType(aExtendedPanId);
    Mle::MeshLocalPrefix      prefix;

    VerifyOrExit(instance.Get<Mle::MleRouter>().IsDisabled(), error = kErrorInvalidState);

    instance.Get<Mac::Mac>().SetExtendedPanId(extPanId);

    prefix.SetFromExtendedPanId(extPanId);
    instance.Get<Mle::MleRouter>().SetMeshLocalPrefix(prefix);

    instance.Get<MeshCoP::ActiveDataset>().Clear();
    instance.Get<MeshCoP::PendingDataset>().Clear();

exit:
    return error;
}

otError otThreadGetLeaderRloc(otInstance *aInstance, otIp6Address *aLeaderRloc)
{
    OT_ASSERT(aLeaderRloc != nullptr);

    return AsCoreType(aInstance).Get<Mle::MleRouter>().GetLeaderAddress(AsCoreType(aLeaderRloc));
}

otLinkModeConfig otThreadGetLinkMode(otInstance *aInstance)
{
    otLinkModeConfig config;

    AsCoreType(aInstance).Get<Mle::MleRouter>().GetDeviceMode().Get(config);

    return config;
}

otError otThreadSetLinkMode(otInstance *aInstance, otLinkModeConfig aConfig)
{
    return AsCoreType(aInstance).Get<Mle::MleRouter>().SetDeviceMode(Mle::DeviceMode(aConfig));
}

void otThreadGetNetworkKey(otInstance *aInstance, otNetworkKey *aNetworkKey)
{
    AsCoreType(aInstance).Get<KeyManager>().GetNetworkKey(AsCoreType(aNetworkKey));
}

#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
otNetworkKeyRef otThreadGetNetworkKeyRef(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<KeyManager>().GetNetworkKeyRef();
}
#endif

otError otThreadSetNetworkKey(otInstance *aInstance, const otNetworkKey *aKey)
{
    Error     error    = kErrorNone;
    Instance &instance = AsCoreType(aInstance);

    OT_ASSERT(aKey != nullptr);

    VerifyOrExit(instance.Get<Mle::MleRouter>().IsDisabled(), error = kErrorInvalidState);

    instance.Get<KeyManager>().SetNetworkKey(AsCoreType(aKey));

    instance.Get<MeshCoP::ActiveDataset>().Clear();
    instance.Get<MeshCoP::PendingDataset>().Clear();

exit:
    return error;
}

#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
otError otThreadSetNetworkKeyRef(otInstance *aInstance, otNetworkKeyRef aKeyRef)
{
    Error     error    = kErrorNone;
    Instance &instance = AsCoreType(aInstance);

    VerifyOrExit(aKeyRef != 0, error = kErrorInvalidArgs);

    VerifyOrExit(instance.Get<Mle::MleRouter>().IsDisabled(), error = kErrorInvalidState);

    instance.Get<KeyManager>().SetNetworkKeyRef((aKeyRef));
    instance.Get<MeshCoP::ActiveDataset>().Clear();
    instance.Get<MeshCoP::PendingDataset>().Clear();

exit:
    return error;
}
#endif

const otIp6Address *otThreadGetRloc(otInstance *aInstance)
{
    return &AsCoreType(aInstance).Get<Mle::MleRouter>().GetMeshLocal16();
}

const otIp6Address *otThreadGetMeshLocalEid(otInstance *aInstance)
{
    return &AsCoreType(aInstance).Get<Mle::MleRouter>().GetMeshLocal64();
}

const otMeshLocalPrefix *otThreadGetMeshLocalPrefix(otInstance *aInstance)
{
    return &AsCoreType(aInstance).Get<Mle::MleRouter>().GetMeshLocalPrefix();
}

otError otThreadSetMeshLocalPrefix(otInstance *aInstance, const otMeshLocalPrefix *aMeshLocalPrefix)
{
    Error error = kErrorNone;

    VerifyOrExit(AsCoreType(aInstance).Get<Mle::MleRouter>().IsDisabled(), error = kErrorInvalidState);

    AsCoreType(aInstance).Get<Mle::MleRouter>().SetMeshLocalPrefix(AsCoreType(aMeshLocalPrefix));
    AsCoreType(aInstance).Get<MeshCoP::ActiveDataset>().Clear();
    AsCoreType(aInstance).Get<MeshCoP::PendingDataset>().Clear();

exit:
    return error;
}

const otIp6Address *otThreadGetLinkLocalIp6Address(otInstance *aInstance)
{
    return &AsCoreType(aInstance).Get<Mle::MleRouter>().GetLinkLocalAddress();
}

const otIp6Address *otThreadGetLinkLocalAllThreadNodesMulticastAddress(otInstance *aInstance)
{
    return &AsCoreType(aInstance).Get<Mle::MleRouter>().GetLinkLocalAllThreadNodesAddress();
}

const otIp6Address *otThreadGetRealmLocalAllThreadNodesMulticastAddress(otInstance *aInstance)
{
    return &AsCoreType(aInstance).Get<Mle::MleRouter>().GetRealmLocalAllThreadNodesAddress();
}

otError otThreadGetServiceAloc(otInstance *aInstance, uint8_t aServiceId, otIp6Address *aServiceAloc)
{
    return AsCoreType(aInstance).Get<Mle::MleRouter>().GetServiceAloc(aServiceId, AsCoreType(aServiceAloc));
}

const char *otThreadGetNetworkName(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::Mac>().GetNetworkName().GetAsCString();
}

otError otThreadSetNetworkName(otInstance *aInstance, const char *aNetworkName)
{
    Error error = kErrorNone;

    VerifyOrExit(AsCoreType(aInstance).Get<Mle::MleRouter>().IsDisabled(), error = kErrorInvalidState);

    error = AsCoreType(aInstance).Get<Mac::Mac>().SetNetworkName(aNetworkName);
    AsCoreType(aInstance).Get<MeshCoP::ActiveDataset>().Clear();
    AsCoreType(aInstance).Get<MeshCoP::PendingDataset>().Clear();

exit:
    return error;
}

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
const char *otThreadGetDomainName(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mac::Mac>().GetDomainName().GetAsCString();
}

otError otThreadSetDomainName(otInstance *aInstance, const char *aDomainName)
{
    Error error = kErrorNone;

    VerifyOrExit(AsCoreType(aInstance).Get<Mle::MleRouter>().IsDisabled(), error = kErrorInvalidState);

    error = AsCoreType(aInstance).Get<Mac::Mac>().SetDomainName(aDomainName);

exit:
    return error;
}

#if OPENTHREAD_CONFIG_DUA_ENABLE
otError otThreadSetFixedDuaInterfaceIdentifier(otInstance *aInstance, const otIp6InterfaceIdentifier *aIid)
{
    Error error = kErrorNone;

    if (aIid)
    {
        error = AsCoreType(aInstance).Get<DuaManager>().SetFixedDuaInterfaceIdentifier(AsCoreType(aIid));
    }
    else
    {
        AsCoreType(aInstance).Get<DuaManager>().ClearFixedDuaInterfaceIdentifier();
    }

    return error;
}

const otIp6InterfaceIdentifier *otThreadGetFixedDuaInterfaceIdentifier(otInstance *aInstance)
{
    Instance &                      instance = AsCoreType(aInstance);
    const otIp6InterfaceIdentifier *iid      = nullptr;

    if (instance.Get<DuaManager>().IsFixedDuaInterfaceIdentifierSet())
    {
        iid = &instance.Get<DuaManager>().GetFixedDuaInterfaceIdentifier();
    }

    return iid;
}
#endif // OPENTHREAD_CONFIG_DUA_ENABLE

#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

uint32_t otThreadGetKeySequenceCounter(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<KeyManager>().GetCurrentKeySequence();
}

void otThreadSetKeySequenceCounter(otInstance *aInstance, uint32_t aKeySequenceCounter)
{
    AsCoreType(aInstance).Get<KeyManager>().SetCurrentKeySequence(aKeySequenceCounter);
}

uint32_t otThreadGetKeySwitchGuardTime(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<KeyManager>().GetKeySwitchGuardTime();
}

void otThreadSetKeySwitchGuardTime(otInstance *aInstance, uint32_t aKeySwitchGuardTime)
{
    AsCoreType(aInstance).Get<KeyManager>().SetKeySwitchGuardTime(aKeySwitchGuardTime);
}

otError otThreadBecomeDetached(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mle::MleRouter>().BecomeDetached();
}

otError otThreadBecomeChild(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mle::MleRouter>().BecomeChild(Mle::kAttachAny);
}

otError otThreadGetNextNeighborInfo(otInstance *aInstance, otNeighborInfoIterator *aIterator, otNeighborInfo *aInfo)
{
    OT_ASSERT((aInfo != nullptr) && (aIterator != nullptr));

    return AsCoreType(aInstance).Get<NeighborTable>().GetNextNeighborInfo(*aIterator, AsCoreType(aInfo));
}

otDeviceRole otThreadGetDeviceRole(otInstance *aInstance)
{
    return MapEnum(AsCoreType(aInstance).Get<Mle::MleRouter>().GetRole());
}

const char *otThreadDeviceRoleToString(otDeviceRole aRole)
{
    return Mle::Mle::RoleToString(MapEnum(aRole));
}

otError otThreadGetLeaderData(otInstance *aInstance, otLeaderData *aLeaderData)
{
    Error error = kErrorNone;

    OT_ASSERT(aLeaderData != nullptr);

    VerifyOrExit(AsCoreType(aInstance).Get<Mle::MleRouter>().IsAttached(), error = kErrorDetached);
    *aLeaderData = AsCoreType(aInstance).Get<Mle::MleRouter>().GetLeaderData();

exit:
    return error;
}

uint8_t otThreadGetLeaderRouterId(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mle::MleRouter>().GetLeaderId();
}

uint8_t otThreadGetLeaderWeight(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mle::MleRouter>().GetLeaderData().GetWeighting();
}

uint32_t otThreadGetPartitionId(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mle::MleRouter>().GetLeaderData().GetPartitionId();
}

uint16_t otThreadGetRloc16(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mle::MleRouter>().GetRloc16();
}

otError otThreadGetParentInfo(otInstance *aInstance, otRouterInfo *aParentInfo)
{
    Error   error = kErrorNone;
    Router *parent;

    OT_ASSERT(aParentInfo != nullptr);

    // Reference device needs get the original parent's info even after the node state changed.
#if !OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
    VerifyOrExit(AsCoreType(aInstance).Get<Mle::MleRouter>().IsChild(), error = kErrorInvalidState);
#endif

    parent = &AsCoreType(aInstance).Get<Mle::MleRouter>().GetParent();

    aParentInfo->mExtAddress     = parent->GetExtAddress();
    aParentInfo->mRloc16         = parent->GetRloc16();
    aParentInfo->mRouterId       = Mle::Mle::RouterIdFromRloc16(parent->GetRloc16());
    aParentInfo->mNextHop        = parent->GetNextHop();
    aParentInfo->mPathCost       = parent->GetCost();
    aParentInfo->mLinkQualityIn  = parent->GetLinkInfo().GetLinkQuality();
    aParentInfo->mLinkQualityOut = parent->GetLinkQualityOut();
    aParentInfo->mAge            = static_cast<uint8_t>(Time::MsecToSec(TimerMilli::GetNow() - parent->GetLastHeard()));
    aParentInfo->mAllocated      = true;
    aParentInfo->mLinkEstablished = parent->IsStateValid();

#if !OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
exit:
#endif
    return error;
}

otError otThreadGetParentAverageRssi(otInstance *aInstance, int8_t *aParentRssi)
{
    Error error = kErrorNone;

    OT_ASSERT(aParentRssi != nullptr);

    *aParentRssi = AsCoreType(aInstance).Get<Mle::MleRouter>().GetParent().GetLinkInfo().GetAverageRss();

    VerifyOrExit(*aParentRssi != OT_RADIO_RSSI_INVALID, error = kErrorFailed);

exit:
    return error;
}

otError otThreadGetParentLastRssi(otInstance *aInstance, int8_t *aLastRssi)
{
    Error error = kErrorNone;

    OT_ASSERT(aLastRssi != nullptr);

    *aLastRssi = AsCoreType(aInstance).Get<Mle::MleRouter>().GetParent().GetLinkInfo().GetLastRss();

    VerifyOrExit(*aLastRssi != OT_RADIO_RSSI_INVALID, error = kErrorFailed);

exit:
    return error;
}

otError otThreadSetEnabled(otInstance *aInstance, bool aEnabled)
{
    Error error = kErrorNone;

    if (aEnabled)
    {
        error = AsCoreType(aInstance).Get<Mle::MleRouter>().Start();
    }
    else
    {
        AsCoreType(aInstance).Get<Mle::MleRouter>().Stop();
    }

    return error;
}

uint16_t otThreadGetVersion(void)
{
    return OPENTHREAD_CONFIG_THREAD_VERSION;
}

bool otThreadIsSingleton(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mle::MleRouter>().IsSingleton();
}

otError otThreadDiscover(otInstance *             aInstance,
                         uint32_t                 aScanChannels,
                         uint16_t                 aPanId,
                         bool                     aJoiner,
                         bool                     aEnableEui64Filtering,
                         otHandleActiveScanResult aCallback,
                         void *                   aCallbackContext)
{
    return AsCoreType(aInstance).Get<Mle::DiscoverScanner>().Discover(
        Mac::ChannelMask(aScanChannels), aPanId, aJoiner, aEnableEui64Filtering,
        /* aFilterIndexes (use hash of factory EUI64) */ nullptr, aCallback, aCallbackContext);
}

otError otThreadSetJoinerAdvertisement(otInstance *   aInstance,
                                       uint32_t       aOui,
                                       const uint8_t *aAdvData,
                                       uint8_t        aAdvDataLength)
{
    return AsCoreType(aInstance).Get<Mle::DiscoverScanner>().SetJoinerAdvertisement(aOui, aAdvData, aAdvDataLength);
}

bool otThreadIsDiscoverInProgress(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<Mle::DiscoverScanner>().IsInProgress();
}

const otIpCounters *otThreadGetIp6Counters(otInstance *aInstance)
{
    return &AsCoreType(aInstance).Get<MeshForwarder>().GetCounters();
}

void otThreadResetIp6Counters(otInstance *aInstance)
{
    AsCoreType(aInstance).Get<MeshForwarder>().ResetCounters();
}

const otMleCounters *otThreadGetMleCounters(otInstance *aInstance)
{
    return &AsCoreType(aInstance).Get<Mle::MleRouter>().GetCounters();
}

void otThreadResetMleCounters(otInstance *aInstance)
{
    AsCoreType(aInstance).Get<Mle::MleRouter>().ResetCounters();
}

void otThreadRegisterParentResponseCallback(otInstance *                   aInstance,
                                            otThreadParentResponseCallback aCallback,
                                            void *                         aContext)
{
    AsCoreType(aInstance).Get<Mle::MleRouter>().RegisterParentResponseStatsCallback(aCallback, aContext);
}

#if OPENTHREAD_CONFIG_TMF_ANYCAST_LOCATOR_ENABLE
otError otThreadLocateAnycastDestination(otInstance *                   aInstance,
                                         const otIp6Address *           aAnycastAddress,
                                         otThreadAnycastLocatorCallback aCallback,
                                         void *                         aContext)
{
    return AsCoreType(aInstance).Get<AnycastLocator>().Locate(AsCoreType(aAnycastAddress), aCallback, aContext);
}

bool otThreadIsAnycastLocateInProgress(otInstance *aInstance)
{
    return AsCoreType(aInstance).Get<AnycastLocator>().IsInProgress();
}
#endif

#endif // OPENTHREAD_FTD || OPENTHREAD_MTD
