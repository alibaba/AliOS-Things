/*
 *  Copyright (c) 2020, The OpenThread Authors.
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
 *   This file includes implementation for the RA-based routing management.
 *
 */

#include "border_router/routing_manager.hpp"

#if OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE

#include <string.h>

#include <openthread/platform/infra_if.h>

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/random.hpp"
#include "common/settings.hpp"
#include "net/ip6.hpp"
#include "thread/network_data_leader.hpp"
#include "thread/network_data_local.hpp"
#include "thread/network_data_notifier.hpp"

namespace ot {

namespace BorderRouter {

RoutingManager::RoutingManager(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mIsRunning(false)
    , mIsEnabled(false)
    , mInfraIfIsRunning(false)
    , mInfraIfIndex(0)
    , mIsAdvertisingLocalOnLinkPrefix(false)
    , mOnLinkPrefixDeprecateTimer(aInstance, HandleOnLinkPrefixDeprecateTimer)
    , mTimeRouterAdvMessageLastUpdate(TimerMilli::GetNow())
    , mDiscoveredPrefixInvalidTimer(aInstance, HandleDiscoveredPrefixInvalidTimer)
    , mDiscoveredPrefixStaleTimer(aInstance, HandleDiscoveredPrefixStaleTimer)
    , mRouterAdvertisementTimer(aInstance, HandleRouterAdvertisementTimer)
    , mRouterAdvertisementCount(0)
    , mVicariousRouterSolicitTimer(aInstance, HandleVicariousRouterSolicitTimer)
    , mRouterSolicitTimer(aInstance, HandleRouterSolicitTimer)
    , mRouterSolicitCount(0)
    , mRoutingPolicyTimer(aInstance, HandleRoutingPolicyTimer)
{
    mLocalOmrPrefix.Clear();

    mLocalOnLinkPrefix.Clear();
}

Error RoutingManager::Init(uint32_t aInfraIfIndex, bool aInfraIfIsRunning)
{
    Error error;

    VerifyOrExit(!IsInitialized(), error = kErrorInvalidState);
    VerifyOrExit(aInfraIfIndex > 0, error = kErrorInvalidArgs);

    SuccessOrExit(error = LoadOrGenerateRandomOmrPrefix());
    SuccessOrExit(error = LoadOrGenerateRandomOnLinkPrefix());

    mInfraIfIndex = aInfraIfIndex;

    // Initialize the infra interface status.
    SuccessOrExit(error = HandleInfraIfStateChanged(mInfraIfIndex, aInfraIfIsRunning));

exit:
    if (error != kErrorNone)
    {
        mInfraIfIndex = 0;
    }
    return error;
}

Error RoutingManager::SetEnabled(bool aEnabled)
{
    Error error = kErrorNone;

    VerifyOrExit(IsInitialized(), error = kErrorInvalidState);

    VerifyOrExit(aEnabled != mIsEnabled);

    mIsEnabled = aEnabled;
    EvaluateState();

exit:
    return error;
}

Error RoutingManager::GetOmrPrefix(Ip6::Prefix &aPrefix)
{
    Error error = kErrorNone;

    VerifyOrExit(IsInitialized(), error = kErrorInvalidState);
    aPrefix = mLocalOmrPrefix;

exit:
    return error;
}

Error RoutingManager::GetOnLinkPrefix(Ip6::Prefix &aPrefix)
{
    Error error = kErrorNone;

    VerifyOrExit(IsInitialized(), error = kErrorInvalidState);
    aPrefix = mLocalOnLinkPrefix;

exit:
    return error;
}

Error RoutingManager::LoadOrGenerateRandomOmrPrefix(void)
{
    Error error = kErrorNone;

    if (Get<Settings>().Read<Settings::OmrPrefix>(mLocalOmrPrefix) != kErrorNone || !IsValidOmrPrefix(mLocalOmrPrefix))
    {
        Ip6::NetworkPrefix randomOmrPrefix;

        otLogNoteBr("No valid OMR prefix found in settings, generating new one");

        error = randomOmrPrefix.GenerateRandomUla();
        if (error != kErrorNone)
        {
            otLogCritBr("Failed to generate random OMR prefix");
            ExitNow();
        }

        mLocalOmrPrefix.Set(randomOmrPrefix);
        IgnoreError(Get<Settings>().Save<Settings::OmrPrefix>(mLocalOmrPrefix));
    }

exit:
    return error;
}

Error RoutingManager::LoadOrGenerateRandomOnLinkPrefix(void)
{
    Error error = kErrorNone;

    if (Get<Settings>().Read<Settings::OnLinkPrefix>(mLocalOnLinkPrefix) != kErrorNone ||
        !mLocalOnLinkPrefix.IsUniqueLocal())
    {
        Ip6::NetworkPrefix randomOnLinkPrefix;

        otLogNoteBr("No valid on-link prefix found in settings, generating new one");

        error = randomOnLinkPrefix.GenerateRandomUla();
        if (error != kErrorNone)
        {
            otLogCritBr("Failed to generate random on-link prefix");
            ExitNow();
        }

        randomOnLinkPrefix.m8[6] = 0;
        randomOnLinkPrefix.m8[7] = 0;
        mLocalOnLinkPrefix.Set(randomOnLinkPrefix);

        IgnoreError(Get<Settings>().Save<Settings::OnLinkPrefix>(mLocalOnLinkPrefix));
    }

exit:
    return error;
}

void RoutingManager::EvaluateState(void)
{
    if (mIsEnabled && Get<Mle::MleRouter>().IsAttached() && mInfraIfIsRunning)
    {
        Start();
    }
    else
    {
        Stop();
    }
}

void RoutingManager::Start(void)
{
    if (!mIsRunning)
    {
        otLogInfoBr("Border Routing manager started");

        mIsRunning = true;
        StartRouterSolicitationDelay();
    }
}

void RoutingManager::Stop(void)
{
    VerifyOrExit(mIsRunning);

    UnpublishLocalOmrPrefix();

    if (mIsAdvertisingLocalOnLinkPrefix)
    {
        RemoveExternalRoute(mLocalOnLinkPrefix);

        // Start deprecating the local on-link prefix to send a PIO
        // with zero preferred lifetime in `SendRouterAdvertisement`.
        DeprecateOnLinkPrefix();
    }

    // Use empty OMR & on-link prefixes to invalidate possible advertised prefixes.
    SendRouterAdvertisement(OmrPrefixArray(), nullptr);

    mAdvertisedOmrPrefixes.Clear();
    mIsAdvertisingLocalOnLinkPrefix = false;
    mOnLinkPrefixDeprecateTimer.Stop();

    InvalidateAllDiscoveredPrefixes();
    mDiscoveredPrefixes.Clear();
    mDiscoveredPrefixInvalidTimer.Stop();
    mDiscoveredPrefixStaleTimer.Stop();

    mRouterAdvertisementTimer.Stop();
    mRouterAdvertisementCount = 0;

    mVicariousRouterSolicitTimer.Stop();
    mRouterSolicitTimer.Stop();
    mRouterSolicitCount = 0;

    mRoutingPolicyTimer.Stop();

    otLogInfoBr("Border Routing manager stopped");

    mIsRunning = false;

exit:
    return;
}

void RoutingManager::RecvIcmp6Message(uint32_t            aInfraIfIndex,
                                      const Ip6::Address &aSrcAddress,
                                      const uint8_t *     aBuffer,
                                      uint16_t            aBufferLength)
{
    Error                    error = kErrorNone;
    const Ip6::Icmp::Header *icmp6Header;

    VerifyOrExit(IsInitialized() && mIsRunning, error = kErrorDrop);
    VerifyOrExit(aInfraIfIndex == mInfraIfIndex, error = kErrorDrop);
    VerifyOrExit(aBuffer != nullptr && aBufferLength >= sizeof(*icmp6Header), error = kErrorParse);

    icmp6Header = reinterpret_cast<const Ip6::Icmp::Header *>(aBuffer);

    switch (icmp6Header->GetType())
    {
    case Ip6::Icmp::Header::kTypeRouterAdvert:
        HandleRouterAdvertisement(aSrcAddress, aBuffer, aBufferLength);
        break;
    case Ip6::Icmp::Header::kTypeRouterSolicit:
        HandleRouterSolicit(aSrcAddress, aBuffer, aBufferLength);
        break;
    default:
        break;
    }

exit:
    if (error != kErrorNone)
    {
        otLogDebgBr("Dropped ICMPv6 message: %s", ErrorToString(error));
    }
}

Error RoutingManager::HandleInfraIfStateChanged(uint32_t aInfraIfIndex, bool aIsRunning)
{
    Error error = kErrorNone;

    VerifyOrExit(IsInitialized(), error = kErrorInvalidState);
    VerifyOrExit(aInfraIfIndex == mInfraIfIndex, error = kErrorInvalidArgs);
    VerifyOrExit(aIsRunning != mInfraIfIsRunning);

    otLogInfoBr("Infra interface (%u) state changed: %sRUNNING -> %sRUNNING", aInfraIfIndex,
                (mInfraIfIsRunning ? "" : "NOT "), (aIsRunning ? "" : "NOT "));

    mInfraIfIsRunning = aIsRunning;
    EvaluateState();

exit:
    return error;
}

void RoutingManager::HandleNotifierEvents(Events aEvents)
{
    VerifyOrExit(IsInitialized() && IsEnabled());

    if (aEvents.Contains(kEventThreadRoleChanged))
    {
        EvaluateState();
    }

    if (mIsRunning && aEvents.Contains(kEventThreadNetdataChanged))
    {
        StartRoutingPolicyEvaluationDelay();
    }

exit:
    return;
}

void RoutingManager::EvaluateOmrPrefix(OmrPrefixArray &aNewOmrPrefixes)
{
    NetworkData::Iterator           iterator = NetworkData::kIteratorInit;
    NetworkData::OnMeshPrefixConfig onMeshPrefixConfig;
    Ip6::Prefix *                   smallestOmrPrefix       = nullptr;
    Ip6::Prefix *                   publishedLocalOmrPrefix = nullptr;

    OT_ASSERT(mIsRunning);

    while (Get<NetworkData::Leader>().GetNextOnMeshPrefix(iterator, onMeshPrefixConfig) == kErrorNone)
    {
        const Ip6::Prefix &prefix = onMeshPrefixConfig.GetPrefix();

        if (!IsValidOmrPrefix(onMeshPrefixConfig))
        {
            continue;
        }

        if (aNewOmrPrefixes.Contains(prefix))
        {
            // Ignore duplicate prefixes.
            continue;
        }

        if (aNewOmrPrefixes.PushBack(prefix) != kErrorNone)
        {
            otLogWarnBr("EvaluateOmrPrefix: Too many OMR prefixes, ignoring prefix %s", prefix.ToString().AsCString());
            continue;
        }

        if (smallestOmrPrefix == nullptr || (prefix < *smallestOmrPrefix))
        {
            smallestOmrPrefix = aNewOmrPrefixes.Back();
        }

        if (prefix == mLocalOmrPrefix)
        {
            publishedLocalOmrPrefix = aNewOmrPrefixes.Back();
        }
    }

    // Decide if we need to add or remove our local OMR prefix.

    if (aNewOmrPrefixes.IsEmpty())
    {
        otLogInfoBr("EvaluateOmrPrefix: No valid OMR prefixes found in Thread network");

        if (PublishLocalOmrPrefix() == kErrorNone)
        {
            IgnoreError(aNewOmrPrefixes.PushBack(mLocalOmrPrefix));
        }

        // The `aNewOmrPrefixes` remains empty if we fail to publish
        // the local OMR prefix.
    }
    else if (publishedLocalOmrPrefix != nullptr && smallestOmrPrefix != publishedLocalOmrPrefix)
    {
        otLogInfoBr("EvaluateOmrPrefix: There is already a smaller OMR prefix %s in the Thread network",
                    smallestOmrPrefix->ToString().AsCString());

        UnpublishLocalOmrPrefix();

        // Remove the local OMR prefix from the list by overwriting it
        // with the last element and then popping it from the list.

        *publishedLocalOmrPrefix = *aNewOmrPrefixes.Back();
        aNewOmrPrefixes.PopBack();
    }
}

Error RoutingManager::PublishLocalOmrPrefix(void)
{
    Error                           error = kErrorNone;
    NetworkData::OnMeshPrefixConfig omrPrefixConfig;

    OT_ASSERT(mIsRunning);

    omrPrefixConfig.Clear();
    omrPrefixConfig.mPrefix       = mLocalOmrPrefix;
    omrPrefixConfig.mStable       = true;
    omrPrefixConfig.mSlaac        = true;
    omrPrefixConfig.mPreferred    = true;
    omrPrefixConfig.mOnMesh       = true;
    omrPrefixConfig.mDefaultRoute = false;
    omrPrefixConfig.mPreference   = NetworkData::kRoutePreferenceMedium;

    error = Get<NetworkData::Local>().AddOnMeshPrefix(omrPrefixConfig);
    if (error != kErrorNone)
    {
        otLogWarnBr("Failed to publish local OMR prefix %s in Thread network: %s",
                    mLocalOmrPrefix.ToString().AsCString(), ErrorToString(error));
    }
    else
    {
        Get<NetworkData::Notifier>().HandleServerDataUpdated();
        otLogInfoBr("Published local OMR prefix %s in Thread network", mLocalOmrPrefix.ToString().AsCString());
    }

    return error;
}

void RoutingManager::UnpublishLocalOmrPrefix(void)
{
    Error error = kErrorNone;

    VerifyOrExit(mIsRunning);

    SuccessOrExit(error = Get<NetworkData::Local>().RemoveOnMeshPrefix(mLocalOmrPrefix));

    Get<NetworkData::Notifier>().HandleServerDataUpdated();
    otLogInfoBr("Unpublished local OMR prefix %s from Thread network", mLocalOmrPrefix.ToString().AsCString());

exit:
    if (error != kErrorNone)
    {
        otLogWarnBr("Failed to unpublish local OMR prefix %s from Thread network: %s",
                    mLocalOmrPrefix.ToString().AsCString(), ErrorToString(error));
    }
}

Error RoutingManager::AddExternalRoute(const Ip6::Prefix &aPrefix, RoutePreference aRoutePreference)
{
    Error                            error;
    NetworkData::ExternalRouteConfig routeConfig;

    OT_ASSERT(mIsRunning);

    routeConfig.Clear();
    routeConfig.SetPrefix(aPrefix);
    routeConfig.mStable     = true;
    routeConfig.mPreference = aRoutePreference;

    error = Get<NetworkData::Local>().AddHasRoutePrefix(routeConfig);
    if (error != kErrorNone)
    {
        otLogWarnBr("Failed to add external route %s: %s", aPrefix.ToString().AsCString(), ErrorToString(error));
    }
    else
    {
        Get<NetworkData::Notifier>().HandleServerDataUpdated();
        otLogInfoBr("Added external route %s", aPrefix.ToString().AsCString());
    }

    return error;
}

void RoutingManager::RemoveExternalRoute(const Ip6::Prefix &aPrefix)
{
    Error error = kErrorNone;

    VerifyOrExit(mIsRunning);

    SuccessOrExit(error = Get<NetworkData::Local>().RemoveHasRoutePrefix(aPrefix));

    Get<NetworkData::Notifier>().HandleServerDataUpdated();
    otLogInfoBr("Removed external route %s", aPrefix.ToString().AsCString());

exit:
    if (error != kErrorNone)
    {
        otLogWarnBr("Failed to remove external route %s: %s", aPrefix.ToString().AsCString(), ErrorToString(error));
    }
}

const Ip6::Prefix *RoutingManager::EvaluateOnLinkPrefix(void)
{
    const Ip6::Prefix *newOnLinkPrefix      = nullptr;
    const Ip6::Prefix *smallestOnLinkPrefix = nullptr;

    // We don't evaluate on-link prefix if we are doing Router Solicitation.
    VerifyOrExit(!mRouterSolicitTimer.IsRunning(),
                 newOnLinkPrefix = (mIsAdvertisingLocalOnLinkPrefix ? &mLocalOnLinkPrefix : nullptr));

    for (const ExternalPrefix &prefix : mDiscoveredPrefixes)
    {
        if (!prefix.mIsOnLinkPrefix || prefix.IsDeprecated())
        {
            continue;
        }

        if (smallestOnLinkPrefix == nullptr || (prefix.mPrefix < *smallestOnLinkPrefix))
        {
            smallestOnLinkPrefix = &prefix.mPrefix;
        }
    }

    // We start advertising our local on-link prefix if there is no existing one.
    if (smallestOnLinkPrefix == nullptr)
    {
        if (mIsAdvertisingLocalOnLinkPrefix ||
            (AddExternalRoute(mLocalOnLinkPrefix, NetworkData::kRoutePreferenceMedium) == kErrorNone))
        {
            newOnLinkPrefix = &mLocalOnLinkPrefix;
        }

        mOnLinkPrefixDeprecateTimer.Stop();
    }
    // When an application-specific on-link prefix is received and it is bigger than the
    // advertised prefix, we will not remove the advertised prefix. In this case, there
    // will be two on-link prefixes on the infra link. But all BRs will still converge to
    // the same smallest on-link prefix and the application-specific prefix is not used.
    else if (mIsAdvertisingLocalOnLinkPrefix)
    {
        if (mLocalOnLinkPrefix < *smallestOnLinkPrefix)
        {
            newOnLinkPrefix = &mLocalOnLinkPrefix;
        }
        else
        {
            otLogInfoBr("EvaluateOnLinkPrefix: There is already smaller on-link prefix %s on interface %u",
                        smallestOnLinkPrefix->ToString().AsCString(), mInfraIfIndex);
            DeprecateOnLinkPrefix();
        }
    }

exit:
    return newOnLinkPrefix;
}

void RoutingManager::HandleOnLinkPrefixDeprecateTimer(Timer &aTimer)
{
    aTimer.Get<RoutingManager>().HandleOnLinkPrefixDeprecateTimer();
}

void RoutingManager::HandleOnLinkPrefixDeprecateTimer(void)
{
    otLogInfoBr("Local on-link prefix %s expired", mLocalOnLinkPrefix.ToString().AsCString());
    RemoveExternalRoute(mLocalOnLinkPrefix);
}

void RoutingManager::DeprecateOnLinkPrefix(void)
{
    OT_ASSERT(mIsAdvertisingLocalOnLinkPrefix);

    otLogInfoBr("Deprecate local on-link prefix %s", mLocalOnLinkPrefix.ToString().AsCString());
    mOnLinkPrefixDeprecateTimer.StartAt(mTimeAdvertisedOnLinkPrefix,
                                        TimeMilli::SecToMsec(kDefaultOnLinkPrefixLifetime));
}

// This method evaluate the routing policy depends on prefix and route
// information on Thread Network and infra link. As a result, this
// method May send RA messages on infra link and publish/unpublish
// OMR prefix in the Thread network.
void RoutingManager::EvaluateRoutingPolicy(void)
{
    OT_ASSERT(mIsRunning);

    const Ip6::Prefix *newOnLinkPrefix = nullptr;
    OmrPrefixArray     newOmrPrefixes;

    otLogInfoBr("Evaluating routing policy");

    // 0. Evaluate on-link & OMR prefixes.
    newOnLinkPrefix = EvaluateOnLinkPrefix();
    EvaluateOmrPrefix(newOmrPrefixes);

    // 1. Send Router Advertisement message if necessary.
    SendRouterAdvertisement(newOmrPrefixes, newOnLinkPrefix);

    if (newOmrPrefixes.IsEmpty())
    {
        // This is the very exceptional case and happens only when we failed to publish
        // our local OMR prefix to the Thread network. We schedule the Router Advertisement
        // timer to re-evaluate our routing policy in the future.

        otLogWarnBr("No OMR prefix advertised! Start Router Advertisement timer for future evaluation");
    }

    // 2. Schedule Router Advertisement timer with random interval.
    {
        uint32_t nextSendTime;

        nextSendTime = Random::NonCrypto::GetUint32InRange(kMinRtrAdvInterval, kMaxRtrAdvInterval);

        if (mRouterAdvertisementCount <= kMaxInitRtrAdvertisements && nextSendTime > kMaxInitRtrAdvInterval)
        {
            nextSendTime = kMaxInitRtrAdvInterval;
        }

        otLogInfoBr("Router advertisement scheduled in %u seconds", nextSendTime);
        mRouterAdvertisementTimer.Start(Time::SecToMsec(nextSendTime));
    }

    // 3. Update advertised on-link & OMR prefixes information.
    mIsAdvertisingLocalOnLinkPrefix = (newOnLinkPrefix == &mLocalOnLinkPrefix);
    mAdvertisedOmrPrefixes          = newOmrPrefixes;
}

void RoutingManager::StartRoutingPolicyEvaluationDelay(void)
{
    OT_ASSERT(mIsRunning);

    uint32_t randomDelay;

    static_assert(kMaxRoutingPolicyDelay > 0, "invalid maximum routing policy evaluation delay");
    randomDelay = Random::NonCrypto::GetUint32InRange(0, Time::SecToMsec(kMaxRoutingPolicyDelay));

    otLogInfoBr("Start evaluating routing policy, scheduled in %u milliseconds", randomDelay);
    mRoutingPolicyTimer.Start(randomDelay);
}

// starts sending Router Solicitations in random delay
// between 0 and kMaxRtrSolicitationDelay.
void RoutingManager::StartRouterSolicitationDelay(void)
{
    uint32_t randomDelay;

    VerifyOrExit(!mRouterSolicitTimer.IsRunning() && mRouterSolicitCount == 0);

    mVicariousRouterSolicitTimer.Stop();

    static_assert(kMaxRtrSolicitationDelay > 0, "invalid maximum Router Solicitation delay");
    randomDelay = Random::NonCrypto::GetUint32InRange(0, Time::SecToMsec(kMaxRtrSolicitationDelay));

    otLogInfoBr("Start Router Solicitation, scheduled in %u milliseconds", randomDelay);
    mTimeRouterSolicitStart = TimerMilli::GetNow();
    mRouterSolicitTimer.Start(randomDelay);

exit:
    return;
}

Error RoutingManager::SendRouterSolicitation(void)
{
    Ip6::Address                    destAddress;
    RouterAdv::RouterSolicitMessage routerSolicit;

    OT_ASSERT(IsInitialized());

    destAddress.SetToLinkLocalAllRoutersMulticast();
    return otPlatInfraIfSendIcmp6Nd(mInfraIfIndex, &destAddress, reinterpret_cast<const uint8_t *>(&routerSolicit),
                                    sizeof(routerSolicit));
}

// This method sends Router Advertisement messages to advertise on-link prefix and route for OMR prefix.
// @param[in]  aNewOmrPrefixes   An array of the new OMR prefixes to be advertised.
//                               Empty array means we should stop advertising OMR prefixes.
// @param[in]  aOnLinkPrefix     A pointer to the new on-link prefix to be advertised.
//                               nullptr means we should stop advertising on-link prefix.
void RoutingManager::SendRouterAdvertisement(const OmrPrefixArray &aNewOmrPrefixes, const Ip6::Prefix *aNewOnLinkPrefix)
{
    uint8_t  buffer[kMaxRouterAdvMessageLength];
    uint16_t bufferLength = 0;

    static_assert(sizeof(mRouterAdvMessage) <= sizeof(buffer), "RA buffer too small");
    memcpy(buffer, &mRouterAdvMessage, sizeof(mRouterAdvMessage));
    bufferLength += sizeof(mRouterAdvMessage);

    if (aNewOnLinkPrefix != nullptr)
    {
        OT_ASSERT(aNewOnLinkPrefix == &mLocalOnLinkPrefix);

        RouterAdv::PrefixInfoOption pio;

        pio.SetOnLink(true);
        pio.SetAutoAddrConfig(true);
        pio.SetValidLifetime(kDefaultOnLinkPrefixLifetime);
        pio.SetPreferredLifetime(kDefaultOnLinkPrefixLifetime);
        pio.SetPrefix(*aNewOnLinkPrefix);

        OT_ASSERT(bufferLength + pio.GetSize() <= sizeof(buffer));
        memcpy(buffer + bufferLength, &pio, pio.GetSize());
        bufferLength += pio.GetSize();

        if (!mIsAdvertisingLocalOnLinkPrefix)
        {
            otLogInfoBr("Start advertising new on-link prefix %s on interface %u",
                        aNewOnLinkPrefix->ToString().AsCString(), mInfraIfIndex);
        }

        otLogInfoBr("Send on-link prefix %s in PIO (preferred lifetime = %u seconds, valid lifetime = %u seconds)",
                    aNewOnLinkPrefix->ToString().AsCString(), pio.GetPreferredLifetime(), pio.GetValidLifetime());

        mTimeAdvertisedOnLinkPrefix = TimerMilli::GetNow();
    }
    else if (mOnLinkPrefixDeprecateTimer.IsRunning())
    {
        RouterAdv::PrefixInfoOption pio;

        pio.SetOnLink(true);
        pio.SetAutoAddrConfig(true);
        pio.SetValidLifetime(TimeMilli::MsecToSec(mOnLinkPrefixDeprecateTimer.GetFireTime() - TimerMilli::GetNow()));

        // Set zero preferred lifetime to immediately deprecate the advertised on-link prefix.
        pio.SetPreferredLifetime(0);
        pio.SetPrefix(mLocalOnLinkPrefix);

        OT_ASSERT(bufferLength + pio.GetSize() <= sizeof(buffer));
        memcpy(buffer + bufferLength, &pio, pio.GetSize());
        bufferLength += pio.GetSize();

        otLogInfoBr("Send on-link prefix %s in PIO (preferred lifetime = %u seconds, valid lifetime = %u seconds)",
                    mLocalOnLinkPrefix.ToString().AsCString(), pio.GetPreferredLifetime(), pio.GetValidLifetime());
    }

    // Invalidate the advertised OMR prefixes if they are no longer in the new OMR prefix array.

    for (const Ip6::Prefix &advertisedOmrPrefix : mAdvertisedOmrPrefixes)
    {
        if (!aNewOmrPrefixes.Contains(advertisedOmrPrefix))
        {
            RouterAdv::RouteInfoOption rio;

            // Set zero route lifetime to immediately invalidate the advertised OMR prefix.
            rio.SetRouteLifetime(0);
            rio.SetPrefix(advertisedOmrPrefix);

            OT_ASSERT(bufferLength + rio.GetSize() <= sizeof(buffer));
            memcpy(buffer + bufferLength, &rio, rio.GetSize());
            bufferLength += rio.GetSize();

            otLogInfoBr("Stop advertising OMR prefix %s on interface %u", advertisedOmrPrefix.ToString().AsCString(),
                        mInfraIfIndex);
        }
    }

    for (const Ip6::Prefix &newOmrPrefix : aNewOmrPrefixes)
    {
        RouterAdv::RouteInfoOption rio;

        rio.SetRouteLifetime(kDefaultOmrPrefixLifetime);
        rio.SetPrefix(newOmrPrefix);

        OT_ASSERT(bufferLength + rio.GetSize() <= sizeof(buffer));
        memcpy(buffer + bufferLength, &rio, rio.GetSize());
        bufferLength += rio.GetSize();

        otLogInfoBr("Send OMR prefix %s in RIO (valid lifetime = %u seconds)", newOmrPrefix.ToString().AsCString(),
                    kDefaultOmrPrefixLifetime);
    }

    // Send the message only when there are options.
    if (bufferLength > sizeof(mRouterAdvMessage))
    {
        Error        error;
        Ip6::Address destAddress;

        ++mRouterAdvertisementCount;

        destAddress.SetToLinkLocalAllNodesMulticast();
        error = otPlatInfraIfSendIcmp6Nd(mInfraIfIndex, &destAddress, buffer, bufferLength);

        if (error == kErrorNone)
        {
            otLogInfoBr("Sent Router Advertisement on interface %u", mInfraIfIndex);
            otDumpDebgBr("[BR-CERT] direction=send | type=RA |", buffer, bufferLength);
        }
        else
        {
            otLogWarnBr("Failed to send Router Advertisement on interface %u: %s", mInfraIfIndex, ErrorToString(error));
        }
    }
}

bool RoutingManager::IsValidOmrPrefix(const NetworkData::OnMeshPrefixConfig &aOnMeshPrefixConfig)
{
    return IsValidOmrPrefix(aOnMeshPrefixConfig.GetPrefix()) && aOnMeshPrefixConfig.mSlaac && !aOnMeshPrefixConfig.mDp;
}

bool RoutingManager::IsValidOmrPrefix(const Ip6::Prefix &aOmrPrefix)
{
    // Accept ULA prefix with length of 64 bits and GUA prefix.
    return (aOmrPrefix.mLength == kOmrPrefixLength && aOmrPrefix.mPrefix.mFields.m8[0] == 0xfd) ||
           (aOmrPrefix.mLength >= 3 && (aOmrPrefix.GetBytes()[0] & 0xE0) == 0x20);
}

bool RoutingManager::IsValidOnLinkPrefix(const RouterAdv::PrefixInfoOption &aPio, bool aManagedAddrConfig)
{
    return IsValidOnLinkPrefix(aPio.GetPrefix()) && aPio.GetOnLink() &&
           (aPio.GetAutoAddrConfig() || aManagedAddrConfig);
}

bool RoutingManager::IsValidOnLinkPrefix(const Ip6::Prefix &aOnLinkPrefix)
{
    return !aOnLinkPrefix.IsLinkLocal() && !aOnLinkPrefix.IsMulticast();
}

void RoutingManager::HandleRouterAdvertisementTimer(Timer &aTimer)
{
    aTimer.Get<RoutingManager>().HandleRouterAdvertisementTimer();
}

void RoutingManager::HandleRouterAdvertisementTimer(void)
{
    otLogInfoBr("Router advertisement timer triggered");

    EvaluateRoutingPolicy();
}

void RoutingManager::HandleVicariousRouterSolicitTimer(Timer &aTimer)
{
    aTimer.Get<RoutingManager>().HandleVicariousRouterSolicitTimer();
}

void RoutingManager::HandleVicariousRouterSolicitTimer(void)
{
    otLogInfoBr("Vicarious router solicitation time out");

    for (const ExternalPrefix &prefix : mDiscoveredPrefixes)
    {
        if (prefix.mTimeLastUpdate <= mTimeVicariousRouterSolicitStart)
        {
            StartRouterSolicitationDelay();
            break;
        }
    }
}

void RoutingManager::HandleRouterSolicitTimer(Timer &aTimer)
{
    aTimer.Get<RoutingManager>().HandleRouterSolicitTimer();
}

void RoutingManager::HandleRouterSolicitTimer(void)
{
    otLogInfoBr("Router solicitation times out");

    if (mRouterSolicitCount < kMaxRtrSolicitations)
    {
        uint32_t nextSolicitationDelay;
        Error    error;

        error = SendRouterSolicitation();

        if (error == kErrorNone)
        {
            otLogDebgBr("Successfully sent %uth Router Solicitation", mRouterSolicitCount);
            ++mRouterSolicitCount;
            nextSolicitationDelay =
                (mRouterSolicitCount == kMaxRtrSolicitations) ? kMaxRtrSolicitationDelay : kRtrSolicitationInterval;
        }
        else
        {
            otLogCritBr("Failed to send %uth Router Solicitation: %s", mRouterSolicitCount, ErrorToString(error));

            // It's unexpected that RS will fail and we will retry sending RS messages in 60 seconds.
            // Notice that `mRouterSolicitCount` is not incremented for failed RS and thus we will
            // not start configuring on-link prefixes before `kMaxRtrSolicitations` successful RS
            // messages have been sent.
            nextSolicitationDelay = kRtrSolicitationRetryDelay;
            mRouterSolicitCount   = 0;
        }

        otLogDebgBr("Router solicitation timer scheduled in %u seconds", nextSolicitationDelay);
        mRouterSolicitTimer.Start(Time::SecToMsec(nextSolicitationDelay));
    }
    else
    {
        // Invalidate/deprecate all OMR/on-link prefixes that are not refreshed during Router Solicitation.
        for (ExternalPrefix &prefix : mDiscoveredPrefixes)
        {
            if (prefix.mTimeLastUpdate <= mTimeRouterSolicitStart)
            {
                if (prefix.mIsOnLinkPrefix)
                {
                    prefix.mPreferredLifetime = 0;
                }
                else
                {
                    InvalidateDiscoveredPrefixes(&prefix.mPrefix, prefix.mIsOnLinkPrefix);
                }
            }
        }

        // Invalidate the learned RA message if it is not refreshed during Router Solicitation.
        if (mTimeRouterAdvMessageLastUpdate <= mTimeRouterSolicitStart)
        {
            UpdateRouterAdvMessage(/* aRouterAdvMessage */ nullptr);
        }

        // Re-evaluate our routing policy and send Router Advertisement if necessary.
        EvaluateRoutingPolicy();
        mRouterSolicitCount = 0;
    }
}

void RoutingManager::HandleDiscoveredPrefixStaleTimer(Timer &aTimer)
{
    aTimer.Get<RoutingManager>().HandleDiscoveredPrefixStaleTimer();
}

void RoutingManager::HandleDiscoveredPrefixStaleTimer(void)
{
    otLogInfoBr("All on-link prefixes are stale");
    StartRouterSolicitationDelay();
}

void RoutingManager::HandleDiscoveredPrefixInvalidTimer(Timer &aTimer)
{
    aTimer.Get<RoutingManager>().HandleDiscoveredPrefixInvalidTimer();
}

void RoutingManager::HandleDiscoveredPrefixInvalidTimer(void)
{
    InvalidateDiscoveredPrefixes();
}

void RoutingManager::HandleRoutingPolicyTimer(Timer &aTimer)
{
    aTimer.Get<RoutingManager>().EvaluateRoutingPolicy();
}

void RoutingManager::HandleRouterSolicit(const Ip6::Address &aSrcAddress,
                                         const uint8_t *     aBuffer,
                                         uint16_t            aBufferLength)
{
    uint32_t randomDelay;

    OT_UNUSED_VARIABLE(aSrcAddress);
    OT_UNUSED_VARIABLE(aBuffer);
    OT_UNUSED_VARIABLE(aBufferLength);

    VerifyOrExit(!mRouterSolicitTimer.IsRunning());
    otLogInfoBr("Received Router Solicitation from %s on interface %u", aSrcAddress.ToString().AsCString(),
                mInfraIfIndex);

    if (!mVicariousRouterSolicitTimer.IsRunning())
    {
        mTimeVicariousRouterSolicitStart = TimerMilli::GetNow();
        mVicariousRouterSolicitTimer.Start(Time::SecToMsec(kVicariousSolicitationTime));
    }

    // Schedule Router Advertisements with random delay.
    randomDelay = Random::NonCrypto::GetUint32InRange(0, kMaxRaDelayTime);
    otLogInfoBr("Router Advertisement scheduled in %u milliseconds", randomDelay);
    mRouterAdvertisementTimer.Start(randomDelay);

exit:
    return;
}

uint32_t RoutingManager::ExternalPrefix::GetPrefixExpireDelay(uint32_t aValidLifetime)
{
    uint32_t delay;

    if (aValidLifetime * static_cast<uint64_t>(1000) > Timer::kMaxDelay)
    {
        delay = Timer::kMaxDelay;
    }
    else
    {
        delay = aValidLifetime * 1000;
    }

    return delay;
}

void RoutingManager::HandleRouterAdvertisement(const Ip6::Address &aSrcAddress,
                                               const uint8_t *     aBuffer,
                                               uint16_t            aBufferLength)
{
    OT_ASSERT(mIsRunning);
    OT_UNUSED_VARIABLE(aSrcAddress);

    using RouterAdv::Option;
    using RouterAdv::PrefixInfoOption;
    using RouterAdv::RouteInfoOption;
    using RouterAdv::RouterAdvMessage;

    bool                    needReevaluate = false;
    const uint8_t *         optionsBegin;
    uint16_t                optionsLength;
    const Option *          option;
    const RouterAdvMessage *routerAdvMessage;

    VerifyOrExit(aBufferLength >= sizeof(RouterAdvMessage));

    otLogInfoBr("Received Router Advertisement from %s on interface %u", aSrcAddress.ToString().AsCString(),
                mInfraIfIndex);
    otDumpDebgBr("[BR-CERT] direction=recv | type=RA |", aBuffer, aBufferLength);

    routerAdvMessage = reinterpret_cast<const RouterAdvMessage *>(aBuffer);
    optionsBegin     = aBuffer + sizeof(RouterAdvMessage);
    optionsLength    = aBufferLength - sizeof(RouterAdvMessage);

    option = nullptr;
    while ((option = Option::GetNextOption(option, optionsBegin, optionsLength)) != nullptr)
    {
        switch (option->GetType())
        {
        case Option::Type::kPrefixInfo:
        {
            const PrefixInfoOption *pio = static_cast<const PrefixInfoOption *>(option);

            if (pio->IsValid())
            {
                needReevaluate |= UpdateDiscoveredPrefixes(*pio, routerAdvMessage->GetManagedAddrConfig());
            }
        }
        break;

        case Option::Type::kRouteInfo:
        {
            const RouteInfoOption *rio = static_cast<const RouteInfoOption *>(option);

            if (rio->IsValid())
            {
                needReevaluate |= UpdateDiscoveredPrefixes(*rio);
            }
        }
        break;

        default:
            break;
        }
    }

    // Remember the header and parameters of RA messages which are
    // initiated from the infra interface.
    if (otPlatInfraIfHasAddress(mInfraIfIndex, &aSrcAddress))
    {
        needReevaluate |= UpdateRouterAdvMessage(routerAdvMessage);
    }

    if (needReevaluate)
    {
        StartRoutingPolicyEvaluationDelay();
    }

exit:
    return;
}

bool RoutingManager::UpdateDiscoveredPrefixes(const RouterAdv::PrefixInfoOption &aPio, bool aManagedAddrConfig)
{
    Ip6::Prefix     prefix         = aPio.GetPrefix();
    bool            needReevaluate = false;
    TimeMilli       staleTime;
    ExternalPrefix  onLinkPrefix;
    ExternalPrefix *existingPrefix = nullptr;

    if (!IsValidOnLinkPrefix(aPio, aManagedAddrConfig))
    {
        otLogInfoBr("Ignore invalid on-link prefix in PIO: %s", prefix.ToString().AsCString());
        ExitNow();
    }

    VerifyOrExit(!mIsAdvertisingLocalOnLinkPrefix || prefix != mLocalOnLinkPrefix);

    otLogInfoBr("Discovered on-link prefix (%s, %u seconds) from interface %u", prefix.ToString().AsCString(),
                aPio.GetValidLifetime(), mInfraIfIndex);

    onLinkPrefix.mIsOnLinkPrefix    = true;
    onLinkPrefix.mPrefix            = prefix;
    onLinkPrefix.mValidLifetime     = aPio.GetValidLifetime();
    onLinkPrefix.mPreferredLifetime = aPio.GetPreferredLifetime();
    onLinkPrefix.mTimeLastUpdate    = TimerMilli::GetNow();

    staleTime = TimerMilli::GetNow();

    for (ExternalPrefix &externalPrefix : mDiscoveredPrefixes)
    {
        if (externalPrefix == onLinkPrefix)
        {
            existingPrefix = &externalPrefix;
        }

        staleTime = OT_MAX(staleTime, externalPrefix.GetStaleTime());
    }

    if (existingPrefix == nullptr)
    {
        if (onLinkPrefix.mValidLifetime == 0)
        {
            ExitNow();
        }

        if (!mDiscoveredPrefixes.IsFull())
        {
            SuccessOrExit(AddExternalRoute(prefix, NetworkData::kRoutePreferenceMedium));
            existingPrefix  = mDiscoveredPrefixes.PushBack();
            *existingPrefix = onLinkPrefix;
            needReevaluate  = true;
        }
        else
        {
            otLogWarnBr("Discovered too many prefixes, ignore new on-link prefix %s", prefix.ToString().AsCString());
            ExitNow();
        }
    }
    else
    {
        constexpr uint32_t kTwoHoursInSeconds = 2 * 3600;

        // Per RFC 4862 section 5.5.3.e:
        // 1.  If the received Valid Lifetime is greater than 2 hours or
        //     greater than RemainingLifetime, set the valid lifetime of the
        //     corresponding address to the advertised Valid Lifetime.
        // 2.  If RemainingLifetime is less than or equal to 2 hours, ignore
        //     the Prefix Information option with regards to the valid
        //     lifetime, unless ...
        // 3.  Otherwise, reset the valid lifetime of the corresponding
        //     address to 2 hours.

        if (onLinkPrefix.mValidLifetime > kTwoHoursInSeconds ||
            onLinkPrefix.GetExpireTime() > existingPrefix->GetExpireTime())
        {
            existingPrefix->mValidLifetime = onLinkPrefix.mValidLifetime;
        }
        else if (existingPrefix->GetExpireTime() > TimerMilli::GetNow() + TimeMilli::SecToMsec(kTwoHoursInSeconds))
        {
            existingPrefix->mValidLifetime = kTwoHoursInSeconds;
        }

        existingPrefix->mPreferredLifetime = onLinkPrefix.mPreferredLifetime;
        existingPrefix->mTimeLastUpdate    = onLinkPrefix.mTimeLastUpdate;
        needReevaluate                     = (existingPrefix->mPreferredLifetime == 0);
    }

    mDiscoveredPrefixInvalidTimer.FireAtIfEarlier(existingPrefix->GetExpireTime());
    staleTime = OT_MAX(staleTime, existingPrefix->GetStaleTime());
    mDiscoveredPrefixStaleTimer.FireAtIfEarlier(staleTime);

exit:
    return needReevaluate;
}

bool RoutingManager::UpdateDiscoveredPrefixes(const RouterAdv::RouteInfoOption &aRio)
{
    Ip6::Prefix     prefix         = aRio.GetPrefix();
    bool            needReevaluate = false;
    ExternalPrefix  omrPrefix;
    ExternalPrefix *existingPrefix = nullptr;

    if (!IsValidOmrPrefix(prefix))
    {
        otLogInfoBr("Ignore invalid OMR prefix in RIO: %s", prefix.ToString().AsCString());
        ExitNow();
    }

    // Ignore OMR prefixes advertised by ourselves or in current Thread Network Data.
    // The `mAdvertisedOmrPrefixes` and the OMR prefix set in Network Data should eventually
    // be equal, but there is time that they are not synchronized immediately:
    // 1. Network Data could contain more OMR prefixes than `mAdvertisedOmrPrefixes` because
    //    we added random delay before Evaluating routing policy when Network Data is changed.
    // 2. `mAdvertisedOmrPrefixes` could contain more OMR prefixes than Network Data because
    //    it takes time to sync a new OMR prefix into Network Data (multicast loopback RA
    //    messages are usually faster than Thread Network Data propagation).
    // They are the reasons why we need both the checks.

    VerifyOrExit(!mAdvertisedOmrPrefixes.Contains(prefix));
    VerifyOrExit(!NetworkDataContainsOmrPrefix(prefix));

    otLogInfoBr("Discovered OMR prefix (%s, %u seconds) from interface %u", prefix.ToString().AsCString(),
                aRio.GetRouteLifetime(), mInfraIfIndex);

    if (aRio.GetRouteLifetime() == 0)
    {
        needReevaluate = InvalidateDiscoveredPrefixes(&prefix, /* aIsOnLinkPrefix */ false);
        ExitNow();
    }

    omrPrefix.mIsOnLinkPrefix  = false;
    omrPrefix.mPrefix          = prefix;
    omrPrefix.mValidLifetime   = aRio.GetRouteLifetime();
    omrPrefix.mRoutePreference = aRio.GetPreference();
    omrPrefix.mTimeLastUpdate  = TimerMilli::GetNow();

    for (ExternalPrefix &externalPrefix : mDiscoveredPrefixes)
    {
        if (externalPrefix == omrPrefix)
        {
            existingPrefix = &externalPrefix;
        }

        mDiscoveredPrefixStaleTimer.FireAtIfEarlier(externalPrefix.GetStaleTime());
    }

    if (existingPrefix == nullptr)
    {
        if (omrPrefix.mValidLifetime == 0)
        {
            ExitNow();
        }

        if (!mDiscoveredPrefixes.IsFull())
        {
            SuccessOrExit(AddExternalRoute(prefix, omrPrefix.mRoutePreference));
            existingPrefix = mDiscoveredPrefixes.PushBack();
            needReevaluate = true;
        }
        else
        {
            otLogWarnBr("Discovered too many prefixes, ignore new prefix %s", prefix.ToString().AsCString());
            ExitNow();
        }
    }

    *existingPrefix = omrPrefix;

    mDiscoveredPrefixInvalidTimer.FireAtIfEarlier(existingPrefix->GetExpireTime());
    mDiscoveredPrefixStaleTimer.FireAtIfEarlier(existingPrefix->GetStaleTime());

exit:
    return needReevaluate;
}

bool RoutingManager::InvalidateDiscoveredPrefixes(const Ip6::Prefix *aPrefix, bool aIsOnLinkPrefix)
{
    bool                didRemove                = false;
    TimeMilli           now                      = TimerMilli::GetNow();
    uint8_t             remainingOnLinkPrefixNum = 0;
    ExternalPrefixArray remainingPrefixes;

    mDiscoveredPrefixInvalidTimer.Stop();

    for (const ExternalPrefix &prefix : mDiscoveredPrefixes)
    {
        if ((aPrefix != nullptr && prefix.mPrefix == *aPrefix && prefix.mIsOnLinkPrefix == aIsOnLinkPrefix) ||
            (prefix.GetExpireTime() <= now))
        {
            RemoveExternalRoute(prefix.mPrefix);
            didRemove = true;
        }
        else
        {
            mDiscoveredPrefixInvalidTimer.FireAtIfEarlier(prefix.GetExpireTime());

            IgnoreError(remainingPrefixes.PushBack(prefix));

            if (prefix.mIsOnLinkPrefix)
            {
                ++remainingOnLinkPrefixNum;
            }
        }
    }

    mDiscoveredPrefixes = remainingPrefixes;

    if (remainingOnLinkPrefixNum == 0 && !mIsAdvertisingLocalOnLinkPrefix)
    {
        // There are no valid on-link prefixes on infra link now, start Router Solicitation
        // To discover more on-link prefixes or timeout to advertise my local on-link prefix.
        StartRouterSolicitationDelay();
    }

    return didRemove; // If anything was removed we need to reevaluate.
}

void RoutingManager::InvalidateAllDiscoveredPrefixes(void)
{
    for (ExternalPrefix &prefix : mDiscoveredPrefixes)
    {
        prefix.mValidLifetime = 0;
    }

    InvalidateDiscoveredPrefixes();

    OT_ASSERT(mDiscoveredPrefixes.IsEmpty());
}

bool RoutingManager::NetworkDataContainsOmrPrefix(const Ip6::Prefix &aPrefix) const
{
    NetworkData::Iterator           iterator = NetworkData::kIteratorInit;
    NetworkData::OnMeshPrefixConfig onMeshPrefixConfig;
    bool                            contain = false;

    while (Get<NetworkData::Leader>().GetNextOnMeshPrefix(iterator, onMeshPrefixConfig) == OT_ERROR_NONE)
    {
        if (IsValidOmrPrefix(onMeshPrefixConfig) && onMeshPrefixConfig.GetPrefix() == aPrefix)
        {
            contain = true;
            break;
        }
    }

    return contain;
}

// Update the `mRouterAdvMessage` with given Router Advertisement message.
// Returns a boolean which indicates whether there are changes of `mRouterAdvMessage`.
bool RoutingManager::UpdateRouterAdvMessage(const RouterAdv::RouterAdvMessage *aRouterAdvMessage)
{
    RouterAdv::RouterAdvMessage oldRouterAdvMessage;

    oldRouterAdvMessage = mRouterAdvMessage;

    mTimeRouterAdvMessageLastUpdate = TimerMilli::GetNow();
    if (aRouterAdvMessage == nullptr || aRouterAdvMessage->GetRouterLifetime() == 0)
    {
        mRouterAdvMessage.SetToDefault();
    }
    else
    {
        mRouterAdvMessage = *aRouterAdvMessage;
        mDiscoveredPrefixStaleTimer.FireAtIfEarlier(mTimeRouterAdvMessageLastUpdate +
                                                    Time::SecToMsec(kRtrAdvStaleTime));
    }

    return (mRouterAdvMessage != oldRouterAdvMessage);
}

} // namespace BorderRouter

} // namespace ot

#endif // OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE
