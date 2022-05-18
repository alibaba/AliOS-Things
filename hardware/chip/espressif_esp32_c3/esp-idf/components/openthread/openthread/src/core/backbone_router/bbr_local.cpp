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
 *   This file implements local Backbone Router service.
 */

#include "bbr_local.hpp"

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/random.hpp"
#include "thread/mle_types.hpp"
#include "thread/thread_netif.hpp"

namespace ot {

namespace BackboneRouter {

Local::Local(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mState(OT_BACKBONE_ROUTER_STATE_DISABLED)
    , mMlrTimeout(Mle::kMlrTimeoutDefault)
    , mReregistrationDelay(Mle::kRegistrationDelayDefault)
    , mSequenceNumber(Random::NonCrypto::GetUint8())
    , mRegistrationJitter(Mle::kBackboneRouterRegistrationJitter)
    , mIsServiceAdded(false)
    , mDomainPrefixCallback(nullptr)
    , mDomainPrefixCallbackContext(nullptr)
{
    mDomainPrefixConfig.GetPrefix().SetLength(0);

    // Primary Backbone Router Aloc
    mBackboneRouterPrimaryAloc.InitAsThreadOriginRealmLocalScope();
    mBackboneRouterPrimaryAloc.GetAddress().GetIid().SetToLocator(Mle::kAloc16BackboneRouterPrimary);

    // All Network Backbone Routers Multicast Address.
    mAllNetworkBackboneRouters.Clear();

    mAllNetworkBackboneRouters.mFields.m8[0]  = 0xff; // Multicast
    mAllNetworkBackboneRouters.mFields.m8[1]  = 0x32; // Flags = 3, Scope = 2
    mAllNetworkBackboneRouters.mFields.m8[15] = 3;    // Group ID = 3

    // All Domain Backbone Routers Multicast Address.
    mAllDomainBackboneRouters.Clear();

    mAllDomainBackboneRouters.mFields.m8[0]  = 0xff; // Multicast
    mAllDomainBackboneRouters.mFields.m8[1]  = 0x32; // Flags = 3, Scope = 2
    mAllDomainBackboneRouters.mFields.m8[15] = 3;    // Group ID = 3
}

void Local::SetEnabled(bool aEnable)
{
    VerifyOrExit(aEnable == (mState == OT_BACKBONE_ROUTER_STATE_DISABLED));

    if (aEnable)
    {
        SetState(OT_BACKBONE_ROUTER_STATE_SECONDARY);
        AddDomainPrefixToNetworkData();
        IgnoreError(AddService());
    }
    else
    {
        RemoveDomainPrefixFromNetworkData();
        RemoveService();
        SetState(OT_BACKBONE_ROUTER_STATE_DISABLED);
    }

exit:
    return;
}

void Local::Reset(void)
{
    VerifyOrExit(mState != OT_BACKBONE_ROUTER_STATE_DISABLED);

    RemoveService();

    if (mState == OT_BACKBONE_ROUTER_STATE_PRIMARY)
    {
        // Increase sequence number when changing from Primary to Secondary.
        mSequenceNumber++;
        Get<Notifier>().Signal(kEventThreadBackboneRouterLocalChanged);
        SetState(OT_BACKBONE_ROUTER_STATE_SECONDARY);
    }

exit:
    return;
}

void Local::GetConfig(BackboneRouterConfig &aConfig) const
{
    aConfig.mSequenceNumber      = mSequenceNumber;
    aConfig.mReregistrationDelay = mReregistrationDelay;
    aConfig.mMlrTimeout          = mMlrTimeout;
}

Error Local::SetConfig(const BackboneRouterConfig &aConfig)
{
    Error error  = kErrorNone;
    bool  update = false;

    VerifyOrExit(aConfig.mMlrTimeout >= Mle::kMlrTimeoutMin && aConfig.mMlrTimeout <= Mle::kMlrTimeoutMax,
                 error = kErrorInvalidArgs);
    // Validate configuration according to Thread 1.2.1 Specification 5.21.3.3:
    // "The Reregistration Delay in seconds MUST be lower than (0.5 * MLR Timeout). It MUST be at least 1."
    VerifyOrExit(aConfig.mReregistrationDelay >= 1, error = kErrorInvalidArgs);
    static_assert(sizeof(aConfig.mReregistrationDelay) < sizeof(aConfig.mMlrTimeout),
                  "the calculation below might overflow");
    VerifyOrExit(aConfig.mReregistrationDelay * 2 < aConfig.mMlrTimeout, error = kErrorInvalidArgs);

    if (aConfig.mReregistrationDelay != mReregistrationDelay)
    {
        mReregistrationDelay = aConfig.mReregistrationDelay;
        update               = true;
    }

    if (aConfig.mMlrTimeout != mMlrTimeout)
    {
        mMlrTimeout = aConfig.mMlrTimeout;
        update      = true;
    }

    if (aConfig.mSequenceNumber != mSequenceNumber)
    {
        mSequenceNumber = aConfig.mSequenceNumber;
        update          = true;
    }

    if (update)
    {
        Get<Notifier>().Signal(kEventThreadBackboneRouterLocalChanged);

        IgnoreError(AddService());
    }

exit:
    LogBackboneRouterService("Set", error);
    return error;
}

Error Local::AddService(bool aForce)
{
    Error                                            error = kErrorInvalidState;
    NetworkData::Service::BackboneRouter::ServerData serverData;

    VerifyOrExit(mState != OT_BACKBONE_ROUTER_STATE_DISABLED && Get<Mle::Mle>().IsAttached());

    VerifyOrExit(aForce /* if register by force */ ||
                 !Get<BackboneRouter::Leader>().HasPrimary() /* if no available Backbone Router service */ ||
                 Get<BackboneRouter::Leader>().GetServer16() == Get<Mle::MleRouter>().GetRloc16()
                 /* If the device itself should be BBR. */
    );

    serverData.SetSequenceNumber(mSequenceNumber);
    serverData.SetReregistrationDelay(mReregistrationDelay);
    serverData.SetMlrTimeout(mMlrTimeout);

    SuccessOrExit(error = Get<NetworkData::Service::Manager>().Add<NetworkData::Service::BackboneRouter>(serverData));
    Get<NetworkData::Notifier>().HandleServerDataUpdated();

    mIsServiceAdded = true;

exit:
    LogBackboneRouterService("Add", error);
    return error;
}

void Local::RemoveService(void)
{
    Error error;

    SuccessOrExit(error = Get<NetworkData::Service::Manager>().Remove<NetworkData::Service::BackboneRouter>());
    Get<NetworkData::Notifier>().HandleServerDataUpdated();
    mIsServiceAdded = false;

exit:
    LogBackboneRouterService("Remove", error);
}

void Local::SetState(BackboneRouterState aState)
{
    VerifyOrExit(mState != aState);

    if (mState == OT_BACKBONE_ROUTER_STATE_DISABLED)
    {
        // Update All Network Backbone Routers Multicast Address for both Secondary and Primary state.
        mAllNetworkBackboneRouters.SetMulticastNetworkPrefix(Get<Mle::MleRouter>().GetMeshLocalPrefix());
    }

    if (mState == OT_BACKBONE_ROUTER_STATE_PRIMARY)
    {
        Get<ThreadNetif>().RemoveUnicastAddress(mBackboneRouterPrimaryAloc);
    }
    else if (aState == OT_BACKBONE_ROUTER_STATE_PRIMARY)
    {
        // Add Primary Backbone Router Aloc for Primary Backbone Router.
        mBackboneRouterPrimaryAloc.GetAddress().SetPrefix(Get<Mle::MleRouter>().GetMeshLocalPrefix());
        Get<ThreadNetif>().AddUnicastAddress(mBackboneRouterPrimaryAloc);
    }

    mState = aState;

    Get<Notifier>().Signal(kEventThreadBackboneRouterStateChanged);

exit:
    return;
}

void Local::HandleBackboneRouterPrimaryUpdate(Leader::State aState, const BackboneRouterConfig &aConfig)
{
    OT_UNUSED_VARIABLE(aState);

    VerifyOrExit(mState != OT_BACKBONE_ROUTER_STATE_DISABLED && Get<Mle::MleRouter>().IsAttached());

    // Wait some jitter before trying to Register.
    if (aConfig.mServer16 == Mac::kShortAddrInvalid)
    {
        uint8_t delay = 1;

        if (!Get<Mle::MleRouter>().IsLeader())
        {
            delay += Random::NonCrypto::GetUint8InRange(0, mRegistrationJitter < 255 ? mRegistrationJitter + 1
                                                                                     : mRegistrationJitter);
        }

        // Here uses the timer resource in Mle.
        Get<Mle::MleRouter>().SetBackboneRouterRegistrationDelay(delay);
    }
    else if (aConfig.mServer16 != Get<Mle::MleRouter>().GetRloc16())
    {
        Reset();
    }
    else if (!mIsServiceAdded)
    {
        // Here original PBBR restores its Backbone Router Service from Thread Network,
        // Intentionally skips the state update as PBBR will refresh its service.
        mSequenceNumber      = aConfig.mSequenceNumber + 1;
        mReregistrationDelay = aConfig.mReregistrationDelay;
        mMlrTimeout          = aConfig.mMlrTimeout;
        Get<Notifier>().Signal(kEventThreadBackboneRouterLocalChanged);
        if (AddService(true /* Force registration to refresh and restore Primary state */) == kErrorNone)
        {
            Get<NetworkData::Notifier>().HandleServerDataUpdated();
        }
    }
    else
    {
        SetState(OT_BACKBONE_ROUTER_STATE_PRIMARY);
    }

exit:
    return;
}

Error Local::GetDomainPrefix(NetworkData::OnMeshPrefixConfig &aConfig)
{
    Error error = kErrorNone;

    VerifyOrExit(mDomainPrefixConfig.GetPrefix().GetLength() > 0, error = kErrorNotFound);

    aConfig = mDomainPrefixConfig;

exit:
    return error;
}

Error Local::RemoveDomainPrefix(const Ip6::Prefix &aPrefix)
{
    Error error = kErrorNone;

    VerifyOrExit(aPrefix.GetLength() > 0, error = kErrorInvalidArgs);
    VerifyOrExit(mDomainPrefixConfig.GetPrefix() == aPrefix, error = kErrorNotFound);

    if (IsEnabled())
    {
        RemoveDomainPrefixFromNetworkData();
    }

    mDomainPrefixConfig.GetPrefix().SetLength(0);

exit:
    return error;
}

Error Local::SetDomainPrefix(const NetworkData::OnMeshPrefixConfig &aConfig)
{
    Error error = kErrorNone;

    VerifyOrExit(aConfig.IsValid(GetInstance()), error = kErrorInvalidArgs);

    if (IsEnabled())
    {
        RemoveDomainPrefixFromNetworkData();
    }

    mDomainPrefixConfig = aConfig;
    LogDomainPrefix("Set", kErrorNone);

    if (IsEnabled())
    {
        AddDomainPrefixToNetworkData();
    }

exit:
    return error;
}

void Local::ApplyMeshLocalPrefix(void)
{
    VerifyOrExit(IsEnabled());

    Get<BackboneTmfAgent>().UnsubscribeMulticast(mAllNetworkBackboneRouters);
    mAllNetworkBackboneRouters.SetMulticastNetworkPrefix(Get<Mle::MleRouter>().GetMeshLocalPrefix());
    Get<BackboneTmfAgent>().SubscribeMulticast(mAllNetworkBackboneRouters);

    if (IsPrimary())
    {
        Get<ThreadNetif>().RemoveUnicastAddress(mBackboneRouterPrimaryAloc);
        mBackboneRouterPrimaryAloc.GetAddress().SetPrefix(Get<Mle::MleRouter>().GetMeshLocalPrefix());
        Get<ThreadNetif>().AddUnicastAddress(mBackboneRouterPrimaryAloc);
    }

exit:
    return;
}

void Local::HandleDomainPrefixUpdate(Leader::DomainPrefixState aState)
{
    if (!IsEnabled())
    {
        ExitNow();
    }

    if (aState == Leader::kDomainPrefixRemoved || aState == Leader::kDomainPrefixRefreshed)
    {
        Get<BackboneTmfAgent>().UnsubscribeMulticast(mAllDomainBackboneRouters);
    }

    if (aState == Leader::kDomainPrefixAdded || aState == Leader::kDomainPrefixRefreshed)
    {
        mAllDomainBackboneRouters.SetMulticastNetworkPrefix(*Get<Leader>().GetDomainPrefix());
        Get<BackboneTmfAgent>().SubscribeMulticast(mAllDomainBackboneRouters);
    }

    if (mDomainPrefixCallback != nullptr)
    {
        switch (aState)
        {
        case Leader::kDomainPrefixAdded:
            mDomainPrefixCallback(mDomainPrefixCallbackContext, OT_BACKBONE_ROUTER_DOMAIN_PREFIX_ADDED,
                                  Get<Leader>().GetDomainPrefix());
            break;
        case Leader::kDomainPrefixRemoved:
            mDomainPrefixCallback(mDomainPrefixCallbackContext, OT_BACKBONE_ROUTER_DOMAIN_PREFIX_REMOVED,
                                  Get<Leader>().GetDomainPrefix());
            break;
        case Leader::kDomainPrefixRefreshed:
            mDomainPrefixCallback(mDomainPrefixCallbackContext, OT_BACKBONE_ROUTER_DOMAIN_PREFIX_CHANGED,
                                  Get<Leader>().GetDomainPrefix());
            break;
        default:
            break;
        }
    }

exit:
    return;
}

void Local::RemoveDomainPrefixFromNetworkData(void)
{
    Error error = kErrorNotFound; // only used for logging.

    if (mDomainPrefixConfig.mPrefix.mLength > 0)
    {
        error = Get<NetworkData::Local>().RemoveOnMeshPrefix(mDomainPrefixConfig.GetPrefix());
    }

    LogDomainPrefix("Remove", error);
}

void Local::AddDomainPrefixToNetworkData(void)
{
    Error error = kErrorNotFound; // only used for logging.

    if (mDomainPrefixConfig.GetPrefix().GetLength() > 0)
    {
        error = Get<NetworkData::Local>().AddOnMeshPrefix(mDomainPrefixConfig);
    }

    LogDomainPrefix("Add", error);
}

#if (OPENTHREAD_CONFIG_LOG_LEVEL >= OT_LOG_LEVEL_INFO) && (OPENTHREAD_CONFIG_LOG_BBR == 1)
void Local::LogDomainPrefix(const char *aAction, Error aError)
{
    otLogInfoBbr("%s Domain Prefix: %s, %s", aAction, mDomainPrefixConfig.GetPrefix().ToString().AsCString(),
                 ErrorToString(aError));
}

void Local::LogBackboneRouterService(const char *aAction, Error aError)
{
    otLogInfoBbr("%s BBR Service: seqno (%d), delay (%ds), timeout (%ds), %s", aAction, mSequenceNumber,
                 mReregistrationDelay, mMlrTimeout, ErrorToString(aError));
}
#endif

void Local::SetDomainPrefixCallback(otBackboneRouterDomainPrefixCallback aCallback, void *aContext)
{
    mDomainPrefixCallback        = aCallback;
    mDomainPrefixCallbackContext = aContext;
}

} // namespace BackboneRouter

} // namespace ot

#endif // OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_ENABLE
