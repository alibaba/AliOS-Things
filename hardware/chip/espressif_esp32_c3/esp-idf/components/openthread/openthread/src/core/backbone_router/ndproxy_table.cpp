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
 *   This file implements Thread NdProxy Table management.
 */

#include "ndproxy_table.hpp"

#if OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE

#include "common/locator_getters.hpp"
#include "common/logging.hpp"

namespace ot {

namespace BackboneRouter {

void NdProxyTable::NdProxy::Init(const Ip6::InterfaceIdentifier &aAddressIid,
                                 const Ip6::InterfaceIdentifier &aMeshLocalIid,
                                 uint16_t                        aRloc16,
                                 uint32_t                        aTimeSinceLastTransaction)
{
    OT_ASSERT(!mValid);

    Clear();

    mValid        = true;
    mAddressIid   = aAddressIid;
    mMeshLocalIid = aMeshLocalIid;
    mDadFlag      = true;

    Update(aRloc16, aTimeSinceLastTransaction);
}

void NdProxyTable::NdProxy::Update(uint16_t aRloc16, uint32_t aTimeSinceLastTransaction)
{
    OT_ASSERT(mValid);

    mRloc16 = aRloc16;
    aTimeSinceLastTransaction =
        OT_MIN(aTimeSinceLastTransaction, static_cast<uint32_t>(Mle::kTimeSinceLastTransactionMax));
    mLastRegistrationTime = TimerMilli::GetNow() - TimeMilli::SecToMsec(aTimeSinceLastTransaction);
}

bool NdProxyTable::MatchesFilter(const NdProxy &aProxy, Filter aFilter)
{
    bool rval = false;

    switch (aFilter)
    {
    case kFilterInvalid:
        rval = !aProxy.mValid;
        break;
    case kFilterValid:
        rval = aProxy.mValid;
        break;
    case kFilterDadInProcess:
        rval = aProxy.mValid && aProxy.mDadFlag;
        break;
    }

    return rval;
}

NdProxyTable::Iterator::Iterator(Instance &aInstance, Filter aFilter)
    : InstanceLocator(aInstance)
    , mFilter(aFilter)
{
    NdProxyTable &table = GetInstance().Get<BackboneRouter::NdProxyTable>();

    mItem = &table.mProxies[0];

    if (!MatchesFilter(*mItem, mFilter))
    {
        Advance();
    }
}

NdProxyTable::Iterator::Iterator(Instance &aInstance, NdProxyTable::Iterator::IteratorType)
    : InstanceLocator(aInstance)
{
    NdProxyTable &table = GetInstance().Get<BackboneRouter::NdProxyTable>();
    mItem               = OT_ARRAY_END(table.mProxies);
}

void NdProxyTable::Iterator::Advance(void)
{
    NdProxyTable &table = GetInstance().Get<BackboneRouter::NdProxyTable>();

    do
    {
        mItem++;
    } while (mItem < OT_ARRAY_END(table.mProxies) && !MatchesFilter(*mItem, mFilter));
}

void NdProxyTable::Erase(NdProxy &aNdProxy)
{
    aNdProxy.mValid = false;
}

void NdProxyTable::HandleDomainPrefixUpdate(Leader::DomainPrefixState aState)
{
    if (aState == Leader::kDomainPrefixAdded || aState == Leader::kDomainPrefixRemoved ||
        aState == Leader::kDomainPrefixRefreshed)
    {
        Clear();
    }
}

void NdProxyTable::Clear(void)
{
    for (NdProxy &proxy : mProxies)
    {
        proxy.Clear();
    }

    if (mCallback != nullptr)
    {
        mCallback(mCallbackContext, OT_BACKBONE_ROUTER_NDPROXY_CLEARED, nullptr);
    }

    otLogNoteBbr("NdProxyTable::Clear!");
}

Error NdProxyTable::Register(const Ip6::InterfaceIdentifier &aAddressIid,
                             const Ip6::InterfaceIdentifier &aMeshLocalIid,
                             uint16_t                        aRloc16,
                             const uint32_t *                aTimeSinceLastTransaction)
{
    Error    error                    = kErrorNone;
    NdProxy *proxy                    = FindByAddressIid(aAddressIid);
    uint32_t timeSinceLastTransaction = aTimeSinceLastTransaction == nullptr ? 0 : *aTimeSinceLastTransaction;

    if (proxy != nullptr)
    {
        VerifyOrExit(proxy->mMeshLocalIid == aMeshLocalIid, error = kErrorDuplicated);

        proxy->Update(aRloc16, timeSinceLastTransaction);
        NotifyDuaRegistrationOnBackboneLink(*proxy, /* aIsRenew */ true);
        ExitNow();
    }

    proxy = FindByMeshLocalIid(aMeshLocalIid);
    if (proxy != nullptr)
    {
        TriggerCallback(OT_BACKBONE_ROUTER_NDPROXY_REMOVED, proxy->mAddressIid);
        Erase(*proxy);
    }
    else
    {
        proxy = FindInvalid();

        // TODO: evict stale DUA entries to have room for this new DUA.
        VerifyOrExit(proxy != nullptr, error = kErrorNoBufs);
    }

    proxy->Init(aAddressIid, aMeshLocalIid, aRloc16, timeSinceLastTransaction);
    mIsAnyDadInProcess = true;

exit:
    otLogInfoBbr("NdProxyTable::Register %s MLIID %s RLOC16 %04x LTT %u => %s", aAddressIid.ToString().AsCString(),
                 aMeshLocalIid.ToString().AsCString(), aRloc16, timeSinceLastTransaction, ErrorToString(error));
    return error;
}

NdProxyTable::NdProxy *NdProxyTable::FindByAddressIid(const Ip6::InterfaceIdentifier &aAddressIid)
{
    NdProxy *found = nullptr;

    for (NdProxy &proxy : Iterate(kFilterValid))
    {
        if (proxy.mAddressIid == aAddressIid)
        {
            ExitNow(found = &proxy);
        }
    }

exit:
    otLogDebgBbr("NdProxyTable::FindByAddressIid(%s) => %s", aAddressIid.ToString().AsCString(),
                 found ? found->mMeshLocalIid.ToString().AsCString() : "NOT_FOUND");
    return found;
}

NdProxyTable::NdProxy *NdProxyTable::FindByMeshLocalIid(const Ip6::InterfaceIdentifier &aMeshLocalIid)
{
    NdProxy *found = nullptr;

    for (NdProxy &proxy : Iterate(kFilterValid))
    {
        if (proxy.mMeshLocalIid == aMeshLocalIid)
        {
            ExitNow(found = &proxy);
        }
    }

exit:
    otLogDebgBbr("NdProxyTable::FindByMeshLocalIid(%s) => %s", aMeshLocalIid.ToString().AsCString(),
                 found ? found->mAddressIid.ToString().AsCString() : "NOT_FOUND");
    return found;
}

NdProxyTable::NdProxy *NdProxyTable::FindInvalid(void)
{
    NdProxy *found = nullptr;

    for (NdProxy &proxy : Iterate(kFilterInvalid))
    {
        ExitNow(found = &proxy);
    }

exit:
    otLogDebgBbr("NdProxyTable::FindInvalid() => %s", found ? "OK" : "NOT_FOUND");
    return found;
}

void NdProxyTable::HandleTimer(void)
{
    VerifyOrExit(mIsAnyDadInProcess);

    mIsAnyDadInProcess = false;

    for (NdProxy &proxy : Iterate(kFilterDadInProcess))
    {
        if (proxy.IsDadAttamptsComplete())
        {
            proxy.mDadFlag = false;
            NotifyDuaRegistrationOnBackboneLink(proxy, /* aIsRenew */ false);
        }
        else
        {
            mIsAnyDadInProcess = true;

            if (Get<BackboneRouter::Manager>().SendBackboneQuery(GetDua(proxy)) == kErrorNone)
            {
                proxy.IncreaseDadAttampts();
            }
        }
    }

exit:
    return;
}

void NdProxyTable::SetCallback(otBackboneRouterNdProxyCallback aCallback, void *aContext)
{
    mCallback        = aCallback;
    mCallbackContext = aContext;
}

void NdProxyTable::TriggerCallback(otBackboneRouterNdProxyEvent    aEvent,
                                   const Ip6::InterfaceIdentifier &aAddressIid) const
{
    Ip6::Address       dua;
    const Ip6::Prefix *prefix = Get<BackboneRouter::Leader>().GetDomainPrefix();

    VerifyOrExit(mCallback != nullptr);

    OT_ASSERT(prefix != nullptr);

    dua.SetPrefix(*prefix);
    dua.SetIid(aAddressIid);

    mCallback(mCallbackContext, aEvent, &dua);

exit:
    return;
}

void NdProxyTable::NotifyDadComplete(NdProxyTable::NdProxy &aNdProxy, bool aDuplicated)
{
    if (aDuplicated)
    {
        Erase(aNdProxy);
    }
    else
    {
        aNdProxy.mDadAttempts = Mle::kDuaDadRepeats;
    }
}

Ip6::Address NdProxyTable::GetDua(NdProxy &aNdProxy)
{
    Ip6::Address       dua;
    const Ip6::Prefix *domainPrefix = Get<BackboneRouter::Leader>().GetDomainPrefix();

    OT_ASSERT(domainPrefix != nullptr);

    dua.SetPrefix(*domainPrefix);
    dua.SetIid(aNdProxy.mAddressIid);

    return dua;
}

NdProxyTable::NdProxy *NdProxyTable::ResolveDua(const Ip6::Address &aDua)
{
    return Get<Leader>().IsDomainUnicast(aDua) ? FindByAddressIid(aDua.GetIid()) : nullptr;
}

void NdProxyTable::NotifyDuaRegistrationOnBackboneLink(NdProxyTable::NdProxy &aNdProxy, bool aIsRenew)
{
    if (!aNdProxy.mDadFlag)
    {
        TriggerCallback(aIsRenew ? OT_BACKBONE_ROUTER_NDPROXY_RENEWED : OT_BACKBONE_ROUTER_NDPROXY_ADDED,
                        aNdProxy.mAddressIid);

        IgnoreError(Get<BackboneRouter::Manager>().SendProactiveBackboneNotification(
            GetDua(aNdProxy), aNdProxy.GetMeshLocalIid(), aNdProxy.GetTimeSinceLastTransaction()));
    }
}

Error NdProxyTable::GetInfo(const Ip6::Address &aDua, otBackboneRouterNdProxyInfo &aNdProxyInfo)
{
    Error error = kErrorNotFound;

    VerifyOrExit(Get<Leader>().IsDomainUnicast(aDua), error = kErrorInvalidArgs);

    for (NdProxy &proxy : Iterate(kFilterValid))
    {
        if (proxy.mAddressIid == aDua.GetIid())
        {
            aNdProxyInfo.mMeshLocalIid             = &proxy.mMeshLocalIid;
            aNdProxyInfo.mTimeSinceLastTransaction = proxy.GetTimeSinceLastTransaction();
            aNdProxyInfo.mRloc16                   = proxy.mRloc16;

            ExitNow(error = kErrorNone);
        }
    }

exit:
    return error;
}

} // namespace BackboneRouter

} // namespace ot

#endif // OPENTHREAD_FTD && OPENTHREAD_CONFIG_BACKBONE_ROUTER_DUA_NDPROXYING_ENABLE
