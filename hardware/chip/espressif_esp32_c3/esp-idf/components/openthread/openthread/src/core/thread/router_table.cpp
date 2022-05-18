/*
 *  Copyright (c) 2018, The OpenThread Authors.
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

#include "router_table.hpp"

#if OPENTHREAD_FTD

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"
#include "common/logging.hpp"
#include "common/timer.hpp"
#include "thread/mle.hpp"
#include "thread/mle_router.hpp"
#include "thread/network_data_leader.hpp"
#include "thread/thread_netif.hpp"

namespace ot {

RouterTable::Iterator::Iterator(Instance &aInstance)
    : InstanceLocator(aInstance)
    , ItemPtrIterator(Get<RouterTable>().GetFirstEntry())
{
}

void RouterTable::Iterator::Advance(void)
{
    mItem = Get<RouterTable>().GetNextEntry(mItem);
}

RouterTable::RouterTable(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mRouterIdSequenceLastUpdated(0)
    , mRouterIdSequence(Random::NonCrypto::GetUint8())
    , mActiveRouterCount(0)
{
    for (Router &router : mRouters)
    {
        router.Init(aInstance);
    }

    Clear();
}

const Router *RouterTable::GetFirstEntry(void) const
{
    const Router *router = &mRouters[0];
    VerifyOrExit(router->GetRloc16() != 0xffff, router = nullptr);

exit:
    return router;
}

const Router *RouterTable::GetNextEntry(const Router *aRouter) const
{
    VerifyOrExit(aRouter != nullptr);
    aRouter++;
    VerifyOrExit(aRouter < &mRouters[Mle::kMaxRouters], aRouter = nullptr);
    VerifyOrExit(aRouter->GetRloc16() != 0xffff, aRouter = nullptr);

exit:
    return aRouter;
}

void RouterTable::Clear(void)
{
    ClearNeighbors();
    mAllocatedRouterIds.Clear();
    memset(mRouterIdReuseDelay, 0, sizeof(mRouterIdReuseDelay));
    UpdateAllocation();
}

void RouterTable::ClearNeighbors(void)
{
    for (Router &router : mRouters)
    {
        if (router.IsStateValid())
        {
            Get<NeighborTable>().Signal(NeighborTable::kRouterRemoved, router);
        }

        router.SetState(Neighbor::kStateInvalid);
    }
}

bool RouterTable::IsAllocated(uint8_t aRouterId) const
{
    return mAllocatedRouterIds.Contains(aRouterId);
}

void RouterTable::UpdateAllocation(void)
{
    uint8_t indexMap[Mle::kMaxRouterId + 1];

    mActiveRouterCount = 0;

    // build index map
    for (uint8_t routerId = 0; routerId <= Mle::kMaxRouterId; routerId++)
    {
        if (IsAllocated(routerId) && mActiveRouterCount < Mle::kMaxRouters)
        {
            indexMap[routerId] = mActiveRouterCount++;
        }
        else
        {
            indexMap[routerId] = Mle::kInvalidRouterId;
        }
    }

    // shift entries forward
    for (int index = Mle::kMaxRouters - 2; index >= 0; index--)
    {
        uint8_t routerId = mRouters[index].GetRouterId();
        uint8_t newIndex;

        if (routerId > Mle::kMaxRouterId || indexMap[routerId] == Mle::kInvalidRouterId)
        {
            continue;
        }

        newIndex = indexMap[routerId];

        if (newIndex > index)
        {
            mRouters[newIndex] = mRouters[index];
        }
    }

    // shift entries backward
    for (uint8_t index = 1; index < Mle::kMaxRouters; index++)
    {
        uint8_t routerId = mRouters[index].GetRouterId();
        uint8_t newIndex;

        if (routerId > Mle::kMaxRouterId || indexMap[routerId] == Mle::kInvalidRouterId)
        {
            continue;
        }

        newIndex = indexMap[routerId];

        if (newIndex < index)
        {
            mRouters[newIndex] = mRouters[index];
        }
    }

    // fix replaced entries
    for (uint8_t routerId = 0; routerId <= Mle::kMaxRouterId; routerId++)
    {
        uint8_t index = indexMap[routerId];

        if (index != Mle::kInvalidRouterId)
        {
            Router &router = mRouters[index];

            if (router.GetRouterId() != routerId)
            {
                router.Clear();
                router.SetRloc16(Mle::Mle::Rloc16FromRouterId(routerId));
                router.SetNextHop(Mle::kInvalidRouterId);
            }
        }
    }

    // clear unused entries
    for (uint8_t index = mActiveRouterCount; index < Mle::kMaxRouters; index++)
    {
        Router &router = mRouters[index];
        router.Clear();
        router.SetRloc16(0xffff);
    }
}

Router *RouterTable::Allocate(void)
{
    Router *rval         = nullptr;
    uint8_t numAvailable = 0;
    uint8_t freeBit;

    // count available router ids
    for (uint8_t routerId = 0; routerId <= Mle::kMaxRouterId; routerId++)
    {
        if (!IsAllocated(routerId) && mRouterIdReuseDelay[routerId] == 0)
        {
            numAvailable++;
        }
    }

    VerifyOrExit(mActiveRouterCount < Mle::kMaxRouters && numAvailable > 0);

    // choose available router id at random
    freeBit = Random::NonCrypto::GetUint8InRange(0, numAvailable);

    // allocate router
    for (uint8_t routerId = 0; routerId <= Mle::kMaxRouterId; routerId++)
    {
        if (IsAllocated(routerId) || mRouterIdReuseDelay[routerId] > 0)
        {
            continue;
        }

        if (freeBit == 0)
        {
            rval = Allocate(routerId);
            OT_ASSERT(rval != nullptr);
            ExitNow();
        }

        freeBit--;
    }

exit:
    return rval;
}

Router *RouterTable::Allocate(uint8_t aRouterId)
{
    Router *rval = nullptr;

    VerifyOrExit(aRouterId <= Mle::kMaxRouterId && mActiveRouterCount < Mle::kMaxRouters && !IsAllocated(aRouterId) &&
                 mRouterIdReuseDelay[aRouterId] == 0);

    mAllocatedRouterIds.Add(aRouterId);
    UpdateAllocation();

    rval = GetRouter(aRouterId);
    rval->SetLastHeard(TimerMilli::GetNow());

    mRouterIdSequence++;
    mRouterIdSequenceLastUpdated = TimerMilli::GetNow();
    Get<Mle::MleRouter>().ResetAdvertiseInterval();

    otLogNoteMle("Allocate router id %d", aRouterId);

exit:
    return rval;
}

Error RouterTable::Release(uint8_t aRouterId)
{
    Error    error  = kErrorNone;
    uint16_t rloc16 = Mle::Mle::Rloc16FromRouterId(aRouterId);
    Router * router;

    OT_ASSERT(aRouterId <= Mle::kMaxRouterId);

    VerifyOrExit(Get<Mle::MleRouter>().IsLeader(), error = kErrorInvalidState);
    VerifyOrExit(IsAllocated(aRouterId), error = kErrorNotFound);

    router = GetNeighbor(rloc16);

    if (router != nullptr)
    {
        Get<NeighborTable>().Signal(NeighborTable::kRouterRemoved, *router);
    }

    mAllocatedRouterIds.Remove(aRouterId);
    UpdateAllocation();

    mRouterIdReuseDelay[aRouterId] = Mle::kRouterIdReuseDelay;

    for (router = GetFirstEntry(); router != nullptr; router = GetNextEntry(router))
    {
        if (router->GetNextHop() == rloc16)
        {
            router->SetNextHop(Mle::kInvalidRouterId);
            router->SetCost(0);
        }
    }

    mRouterIdSequence++;
    mRouterIdSequenceLastUpdated = TimerMilli::GetNow();

    Get<AddressResolver>().Remove(aRouterId);
    Get<NetworkData::Leader>().RemoveBorderRouter(rloc16, NetworkData::Leader::kMatchModeRouterId);
    Get<Mle::MleRouter>().ResetAdvertiseInterval();

    otLogNoteMle("Release router id %d", aRouterId);

exit:
    return error;
}

void RouterTable::RemoveRouterLink(Router &aRouter)
{
    aRouter.SetLinkQualityOut(0);
    aRouter.SetLastHeard(TimerMilli::GetNow());

    for (Router *cur = GetFirstEntry(); cur != nullptr; cur = GetNextEntry(cur))
    {
        if (cur->GetNextHop() == aRouter.GetRouterId())
        {
            cur->SetNextHop(Mle::kInvalidRouterId);
            cur->SetCost(0);

            if (GetLinkCost(*cur) >= Mle::kMaxRouteCost)
            {
                Get<Mle::MleRouter>().ResetAdvertiseInterval();
            }
        }
    }

    if (aRouter.GetNextHop() == Mle::kInvalidRouterId)
    {
        Get<Mle::MleRouter>().ResetAdvertiseInterval();

        // Clear all EID-to-RLOC entries associated with the router.
        Get<AddressResolver>().Remove(aRouter.GetRouterId());
    }
}

uint8_t RouterTable::GetActiveLinkCount(void) const
{
    uint8_t activeLinks = 0;

    for (const Router *router = GetFirstEntry(); router != nullptr; router = GetNextEntry(router))
    {
        if (router->IsStateValid())
        {
            activeLinks++;
        }
    }

    return activeLinks;
}

const Router *RouterTable::FindRouter(const Router::AddressMatcher &aMatcher) const
{
    const Router *router;

    for (router = GetFirstEntry(); router != nullptr; router = GetNextEntry(router))
    {
        if (router->Matches(aMatcher))
        {
            break;
        }
    }

    return router;
}

Router *RouterTable::GetNeighbor(uint16_t aRloc16)
{
    Router *router = nullptr;

    VerifyOrExit(aRloc16 != Get<Mle::MleRouter>().GetRloc16());
    router = FindRouter(Router::AddressMatcher(aRloc16, Router::kInStateValid));

exit:
    return router;
}

Router *RouterTable::GetNeighbor(const Mac::ExtAddress &aExtAddress)
{
    return FindRouter(Router::AddressMatcher(aExtAddress, Router::kInStateValid));
}

Router *RouterTable::GetNeighbor(const Mac::Address &aMacAddress)
{
    return FindRouter(Router::AddressMatcher(aMacAddress, Router::kInStateValid));
}

const Router *RouterTable::GetRouter(uint8_t aRouterId) const
{
    const Router *router = nullptr;
    uint16_t      rloc16;

    // Skip if invalid router id is passed.
    VerifyOrExit(aRouterId < Mle::kInvalidRouterId);

    rloc16 = Mle::Mle::Rloc16FromRouterId(aRouterId);
    router = FindRouter(Router::AddressMatcher(rloc16, Router::kInStateAny));

exit:
    return router;
}

Router *RouterTable::GetRouter(const Mac::ExtAddress &aExtAddress)
{
    return FindRouter(Router::AddressMatcher(aExtAddress, Router::kInStateAny));
}

Error RouterTable::GetRouterInfo(uint16_t aRouterId, Router::Info &aRouterInfo)
{
    Error   error = kErrorNone;
    Router *router;
    uint8_t routerId;

    if (aRouterId <= Mle::kMaxRouterId)
    {
        routerId = static_cast<uint8_t>(aRouterId);
    }
    else
    {
        VerifyOrExit(Mle::Mle::IsActiveRouter(aRouterId), error = kErrorInvalidArgs);
        routerId = Mle::Mle::RouterIdFromRloc16(aRouterId);
        VerifyOrExit(routerId <= Mle::kMaxRouterId, error = kErrorInvalidArgs);
    }

    router = GetRouter(routerId);
    VerifyOrExit(router != nullptr, error = kErrorNotFound);

    aRouterInfo.SetFrom(*router);

exit:
    return error;
}

Router *RouterTable::GetLeader(void)
{
    return GetRouter(Get<Mle::MleRouter>().GetLeaderId());
}

uint32_t RouterTable::GetLeaderAge(void) const
{
    return (mActiveRouterCount > 0) ? Time::MsecToSec(TimerMilli::GetNow() - mRouterIdSequenceLastUpdated) : 0xffffffff;
}

uint8_t RouterTable::GetNeighborCount(void) const
{
    uint8_t count = 0;

    for (const Router *router = GetFirstEntry(); router != nullptr; router = GetNextEntry(router))
    {
        if (router->IsStateValid())
        {
            count++;
        }
    }

    return count;
}

uint8_t RouterTable::GetLinkCost(Router &aRouter)
{
    uint8_t rval = Mle::kMaxRouteCost;

    VerifyOrExit(aRouter.GetRloc16() != Get<Mle::MleRouter>().GetRloc16() && aRouter.IsStateValid());

    rval = aRouter.GetLinkInfo().GetLinkQuality();

    if (rval > aRouter.GetLinkQualityOut())
    {
        rval = aRouter.GetLinkQualityOut();
    }

    rval = Mle::MleRouter::LinkQualityToCost(rval);

exit:
    return rval;
}

void RouterTable::UpdateRouterIdSet(uint8_t aRouterIdSequence, const Mle::RouterIdSet &aRouterIdSet)
{
    mRouterIdSequence            = aRouterIdSequence;
    mRouterIdSequenceLastUpdated = TimerMilli::GetNow();

    VerifyOrExit(mAllocatedRouterIds != aRouterIdSet);

    for (uint8_t routerId = 0; routerId <= Mle::kMaxRouterId; routerId++)
    {
        // If was allocated but removed in new Router Id Set
        if (IsAllocated(routerId) && !aRouterIdSet.Contains(routerId))
        {
            Router *router = GetRouter(routerId);

            OT_ASSERT(router != nullptr);
            router->SetNextHop(Mle::kInvalidRouterId);
            RemoveRouterLink(*router);

            mAllocatedRouterIds.Remove(routerId);
        }
    }

    mAllocatedRouterIds = aRouterIdSet;
    UpdateAllocation();
    Get<Mle::MleRouter>().ResetAdvertiseInterval();

exit:
    return;
}

void RouterTable::HandleTimeTick(void)
{
    Mle::MleRouter &mle = Get<Mle::MleRouter>();

    if (mle.IsLeader())
    {
        // update router id sequence
        if (GetLeaderAge() >= Mle::kRouterIdSequencePeriod)
        {
            mRouterIdSequence++;
            mRouterIdSequenceLastUpdated = TimerMilli::GetNow();
        }

        for (uint8_t routerId = 0; routerId <= Mle::kMaxRouterId; routerId++)
        {
            if (mRouterIdReuseDelay[routerId] > 0)
            {
                mRouterIdReuseDelay[routerId]--;
            }
        }
    }
}

} // namespace ot

#endif // OPENTHREAD_FTD
