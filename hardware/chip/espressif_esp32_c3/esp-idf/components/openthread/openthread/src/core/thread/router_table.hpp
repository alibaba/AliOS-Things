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

#ifndef ROUTER_TABLE_HPP_
#define ROUTER_TABLE_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_FTD

#include "common/const_cast.hpp"
#include "common/encoding.hpp"
#include "common/iterator_utils.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "mac/mac_types.hpp"
#include "thread/mle_types.hpp"
#include "thread/thread_tlvs.hpp"
#include "thread/topology.hpp"

namespace ot {

class RouterTable : public InstanceLocator, private NonCopyable
{
    friend class NeighborTable;
    class IteratorBuilder;

public:
    /**
     * This class represents an iterator for iterating through entries in the router table.
     *
     */
    class Iterator : public InstanceLocator, public ItemPtrIterator<Router, Iterator>
    {
        friend class ItemPtrIterator<Router, Iterator>;
        friend class IteratorBuilder;

    public:
        /**
         * This constructor initializes an `Iterator` instance to start from beginning of the router table.
         *
         * @param[in] aInstance  A reference to the OpenThread instance.
         *
         */
        explicit Iterator(Instance &aInstance);

    private:
        enum IteratorType : uint8_t
        {
            kEndIterator,
        };

        Iterator(Instance &aInstance, IteratorType)
            : InstanceLocator(aInstance)
        {
        }

        void Advance(void);
    };

    /**
     * Constructor.
     *
     * @param[in]  aInstance  A reference to the OpenThread instance.
     *
     */
    explicit RouterTable(Instance &aInstance);

    /**
     * This method clears the router table.
     *
     */
    void Clear(void);

    /**
     * This method removes all neighbor links to routers.
     *
     */
    void ClearNeighbors(void);

    /**
     * This method allocates a router with a random router id.
     *
     * @returns A pointer to the allocated router or nullptr if a router ID is not available.
     *
     */
    Router *Allocate(void);

    /**
     * This method allocates a router with a specified router id.
     *
     * @returns A pointer to the allocated router or nullptr if the router id could not be allocated.
     *
     */
    Router *Allocate(uint8_t aRouterId);

    /**
     * This method releases a router id.
     *
     * @param[in]  aRouterId  The router id.
     *
     * @retval kErrorNone          Successfully released the router id.
     * @retval kErrorInvalidState  The device is not currently operating as a leader.
     * @retval kErrorNotFound      The router id is not currently allocated.
     *
     */
    Error Release(uint8_t aRouterId);

    /**
     * This method removes a router link.
     *
     * @param[in]  aRouter  A reference to the router.
     *
     */
    void RemoveRouterLink(Router &aRouter);

    /**
     * This method returns the number of active routers in the Thread network.
     *
     * @returns The number of active routers in the Thread network.
     *
     */
    uint8_t GetActiveRouterCount(void) const { return mActiveRouterCount; }

    /**
     * This method returns the number of active links with neighboring routers.
     *
     * @returns The number of active links with neighboring routers.
     *
     */
    uint8_t GetActiveLinkCount(void) const;

    /**
     * This method returns the leader in the Thread network.
     *
     * @returns A pointer to the Leader in the Thread network.
     *
     */
    Router *GetLeader(void);

    /**
     * This method returns the time in seconds since the last Router ID Sequence update.
     *
     * @returns The time in seconds since the last Router ID Sequence update.
     *
     */
    uint32_t GetLeaderAge(void) const;

    /**
     * This method returns the link cost for a neighboring router.
     *
     * @param[in]  aRouter  A reference to the router.
     *
     * @returns The link cost.
     *
     */
    uint8_t GetLinkCost(Router &aRouter);

    /**
     * This method returns the neighbor for a given RLOC16.
     *
     * @param[in]  aRloc16  The RLOC16 value.
     *
     * @returns A pointer to the router or nullptr if the router could not be found.
     *
     */
    Router *GetNeighbor(uint16_t aRloc16);

    /**
     * This method returns the neighbor for a given IEEE Extended Address.
     *
     * @param[in]  aExtAddress  A reference to the IEEE Extended Address.
     *
     * @returns A pointer to the router or nullptr if the router could not be found.
     *
     */
    Router *GetNeighbor(const Mac::ExtAddress &aExtAddress);

    /**
     * This method returns the neighbor for a given MAC address.
     *
     * @param[in]  aMacAddress  A MAC address
     *
     * @returns A pointer to the router or nullptr if the router could not be found.
     *
     */
    Router *GetNeighbor(const Mac::Address &aMacAddress);

    /**
     * This method returns the router for a given router id.
     *
     * @param[in]  aRouterId  The router id.
     *
     * @returns A pointer to the router or nullptr if the router could not be found.
     *
     */
    Router *GetRouter(uint8_t aRouterId) { return AsNonConst(AsConst(this)->GetRouter(aRouterId)); }

    /**
     * This method returns the router for a given router id.
     *
     * @param[in]  aRouterId  The router id.
     *
     * @returns A pointer to the router or nullptr if the router could not be found.
     *
     */
    const Router *GetRouter(uint8_t aRouterId) const;

    /**
     * This method returns the router for a given IEEE Extended Address.
     *
     * @param[in]  aExtAddress  A reference to the IEEE Extended Address.
     *
     * @returns A pointer to the router or nullptr if the router could not be found.
     *
     */
    Router *GetRouter(const Mac::ExtAddress &aExtAddress);

    /**
     * This method returns if the router table contains a given `Neighbor` instance.
     *
     * @param[in]  aNeighbor  A reference to a `Neighbor`.
     *
     * @retval TRUE  if @p aNeighbor is a `Router` in the router table.
     * @retval FALSE if @p aNeighbor is not a `Router` in the router table
     *               (i.e. mParent, mParentCandidate, a `Child` of the child table).
     *
     */
    bool Contains(const Neighbor &aNeighbor) const
    {
        return mRouters <= &static_cast<const Router &>(aNeighbor) &&
               &static_cast<const Router &>(aNeighbor) < mRouters + Mle::kMaxRouters;
    }

    /**
     * This method retains diagnostic information for a given router.
     *
     * @param[in]   aRouterId    The router ID or RLOC16 for a given router.
     * @param[out]  aRouterInfo  The router information.
     *
     * @retval kErrorNone          Successfully retrieved the router info for given id.
     * @retval kErrorInvalidArgs   @p aRouterId is not a valid value for a router.
     * @retval kErrorNotFound      No router entry with the given id.
     *
     */
    Error GetRouterInfo(uint16_t aRouterId, Router::Info &aRouterInfo);

    /**
     * This method returns the Router ID Sequence.
     *
     * @returns The Router ID Sequence.
     *
     */
    uint8_t GetRouterIdSequence(void) const { return mRouterIdSequence; }

    /**
     * This method returns the local time when the Router ID Sequence was last updated.
     *
     * @returns The local time when the Router ID Sequence was last updated.
     *
     */
    TimeMilli GetRouterIdSequenceLastUpdated(void) const { return mRouterIdSequenceLastUpdated; }

    /**
     * This method returns the number of neighbor links.
     *
     * @returns The number of neighbor links.
     *
     */
    uint8_t GetNeighborCount(void) const;

    /**
     * This method indicates whether or not @p aRouterId is allocated.
     *
     * @retval TRUE if @p aRouterId is allocated.
     * @retval FALSE if @p aRouterId is not allocated.
     *
     */
    bool IsAllocated(uint8_t aRouterId) const;

    /**
     * This method updates the Router ID allocation.
     *
     * @param[in]  aRouterIdSequence  The Router Id Sequence.
     * @param[in]  aRouterIdSet       A reference to the Router Id Set.
     *
     */
    void UpdateRouterIdSet(uint8_t aRouterIdSequence, const Mle::RouterIdSet &aRouterIdSet);

    /**
     * This method gets the allocated Router ID set.
     *
     * @returns The allocated Router ID set.
     *
     */
    const Mle::RouterIdSet &GetRouterIdSet(void) const { return mAllocatedRouterIds; }

    /**
     * This method updates the router table and must be called with a one second period.
     *
     */
    void HandleTimeTick(void);

    /**
     * This method enables range-based `for` loop iteration over all Router entries in the Router table.
     *
     * This method should be used as follows:
     *
     *     for (Router &router : Get<RouterTable>().Iterate()) { ... }
     *
     * @returns An `IteratorBuilder` instance.
     *
     */
    IteratorBuilder Iterate(void) { return IteratorBuilder(GetInstance()); }

private:
    class IteratorBuilder : public InstanceLocator
    {
    public:
        explicit IteratorBuilder(Instance &aInstance)
            : InstanceLocator(aInstance)
        {
        }

        Iterator begin(void) { return Iterator(GetInstance()); }
        Iterator end(void) { return Iterator(GetInstance(), Iterator::kEndIterator); }
    };

    void          UpdateAllocation(void);
    const Router *GetFirstEntry(void) const;
    const Router *GetNextEntry(const Router *aRouter) const;
    Router *      GetFirstEntry(void) { return AsNonConst(AsConst(this)->GetFirstEntry()); }
    Router *      GetNextEntry(Router *aRouter) { return AsNonConst(AsConst(this)->GetNextEntry(aRouter)); }

    const Router *FindRouter(const Router::AddressMatcher &aMatcher) const;
    Router *      FindRouter(const Router::AddressMatcher &aMatcher)
    {
        return AsNonConst(AsConst(this)->FindRouter(aMatcher));
    }

    Router           mRouters[Mle::kMaxRouters];
    Mle::RouterIdSet mAllocatedRouterIds;
    uint8_t          mRouterIdReuseDelay[Mle::kMaxRouterId + 1];
    TimeMilli        mRouterIdSequenceLastUpdated;
    uint8_t          mRouterIdSequence;
    uint8_t          mActiveRouterCount;
};

} // namespace ot

#endif // OPENTHREAD_FTD

#endif // ROUTER_TABLE_HPP_
