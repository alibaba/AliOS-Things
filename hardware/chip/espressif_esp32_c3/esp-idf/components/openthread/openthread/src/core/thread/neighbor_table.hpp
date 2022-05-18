/*
 *  Copyright (c) 2016-2020, The OpenThread Authors.
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
 *   This file includes definitions for Thread neighbor table.
 */

#ifndef NEIGHBOR_TABLE_HPP_
#define NEIGHBOR_TABLE_HPP_

#include "openthread-core-config.h"

#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "thread/topology.hpp"

namespace ot {

/**
 * This class represents the Thread neighbor table.
 *
 */
class NeighborTable : public InstanceLocator, private NonCopyable
{
public:
    /**
     * This function pointer is called to notify that a child or router neighbor is being added to or removed from
     * neighbor table.
     *
     * Note that this callback in invoked while the neighbor/child table is being updated and always before the related
     * `Notifier` event.
     *
     */
    typedef otNeighborTableCallback Callback;

    /**
     * This type represents a neighbor table entry info (child or router) and is used as a parameter in the neighbor
     * table callback.
     *
     */
    typedef otNeighborTableEntryInfo EntryInfo;

    /**
     * This enumeration defines the constants used in `NeighborTable::Callback` to indicate whether a child or router
     * neighbor is being added or removed.
     *
     */
    enum Event : uint8_t
    {
        kChildAdded       = OT_NEIGHBOR_TABLE_EVENT_CHILD_ADDED,        ///< A child is being added.
        kChildRemoved     = OT_NEIGHBOR_TABLE_EVENT_CHILD_REMOVED,      ///< A child is being removed.
        kChildModeChanged = OT_NEIGHBOR_TABLE_EVENT_CHILD_MODE_CHANGED, ///< An existing child's mode changed.
        kRouterAdded      = OT_NEIGHBOR_TABLE_EVENT_ROUTER_ADDED,       ///< A router is being added.
        kRouterRemoved    = OT_NEIGHBOR_TABLE_EVENT_ROUTER_REMOVED,     ///< A router is being removed.
    };

    /**
     * This constructor initializes the `NeighborTable` instance.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit NeighborTable(Instance &aInstance);

    /**
     * This method searches among parent and parent candidate to find a `Neighbor` corresponding to a given short
     * address.
     *
     * @param[in]  aShortAddress  A short address.
     * @param[in]  aFilter        A neighbor state filter
     *
     * @returns A pointer to the `Neighbor` corresponding to @p aShortAddress, nullptr otherwise.
     *
     */
    Neighbor *FindParent(Mac::ShortAddress     aShortAddress,
                         Neighbor::StateFilter aFilter = Neighbor::kInStateValidOrRestoring);

    /**
     * This method searches in parent and parent candidate to find a `Neighbor` corresponding to a given MAC Extended
     * Address.
     *
     * @param[in]  aExtAddress   A MAC Extended Address.
     * @param[in]  aFilter       A neighbor state filter
     *
     * @returns A pointer to the `Neighbor` corresponding to @p aExtAddress, nullptr otherwise.
     *
     */
    Neighbor *FindParent(const Mac::ExtAddress &aExtAddress,
                         Neighbor::StateFilter  aFilter = Neighbor::kInStateValidOrRestoring);

    /**
     * This method searches among parent and parent candidate to find a `Neighbor` object corresponding to a given MAC
     * address.
     *
     * @param[in]  aMacAddress  A MAC address.
     * @param[in]  aFilter      A neighbor state filter
     *
     * @returns A pointer to the `Neighbor` corresponding to @p aMacAddress, nullptr otherwise.
     *
     */
    Neighbor *FindParent(const Mac::Address &  aMacAddress,
                         Neighbor::StateFilter aFilter = Neighbor::kInStateValidOrRestoring);

    /**
     * This method searches in the neighbor table to find a `Neighbor` corresponding to a given short address.
     *
     * @param[in]  aShortAddress  A short address.
     * @param[in]  aFilter        A neighbor state filter.
     *
     * @returns A pointer to the `Neighbor` corresponding to @p aShortAddress, nullptr otherwise.
     *
     */
    Neighbor *FindNeighbor(Mac::ShortAddress     aShortAddress,
                           Neighbor::StateFilter aFilter = Neighbor::kInStateValidOrRestoring);

    /**
     * This method searches in the neighbor table to find a `Neighbor` corresponding to a given MAC Extended Address.
     *
     * @param[in]  aExtAddress   A MAC Extended Address.
     * @param[in]  aFilter       A neighbor state filter.
     *
     * @returns A pointer to the `Neighbor` corresponding to @p aExtAddress, nullptr otherwise.
     *
     */
    Neighbor *FindNeighbor(const Mac::ExtAddress &aExtAddress,
                           Neighbor::StateFilter  aFilter = Neighbor::kInStateValidOrRestoring);

    /**
     * This method searches in the neighbor table to find a `Neighbor` object corresponding to a given MAC address.
     *
     * @param[in]  aMacAddress  A MAC address.
     * @param[in]  aFilter      A neighbor state filter.
     *
     * @returns A pointer to the `Neighbor` corresponding to @p aMacAddress, nullptr otherwise.
     *
     */
    Neighbor *FindNeighbor(const Mac::Address &  aMacAddress,
                           Neighbor::StateFilter aFilter = Neighbor::kInStateValidOrRestoring);

#if OPENTHREAD_FTD

    /**
     * This method searches in the neighbor table to find a `Neighbor` object corresponding to a given IPv6 address.
     *
     * @param[in]  aIp6Address  An IPv6 address.
     * @pram[in]   aFilter      A neighbor state filter.
     *
     * @returns A pointer to the `Neighbor` corresponding to @p aIp6Address, nullptr otherwise.
     *
     */
    Neighbor *FindNeighbor(const Ip6::Address &  aIp6Address,
                           Neighbor::StateFilter aFilter = Neighbor::kInStateValidOrRestoring);

    /**
     * This method searches in the neighbor table to find a `Neighbor` for which a one-way link is maintained (as in the
     * case of an FTD child with neighbor routers).
     *
     * @param[in]  aMacAddress  A MAC address.
     *
     * @returns A pointer to the Neighbor corresponding to @p aMacAddress, nullptr otherwise.
     *
     */
    Neighbor *FindRxOnlyNeighborRouter(const Mac::Address &aMacAddress);

#endif // OPENTHREAD_FTD

    /**
     * This method gets the next neighbor information. It is used to iterate through the entries of
     * the neighbor table.
     *
     * @param[inout]  aIterator  A reference to the iterator context. To get the first neighbor entry
                                 it should be set to OT_NEIGHBOR_INFO_ITERATOR_INIT.
     * @param[out]    aNeighInfo The neighbor information.
     *
     * @retval kErrorNone         Successfully found the next neighbor entry in table.
     * @retval kErrorNotFound     No subsequent neighbor entry exists in the table.
     *
     */
    Error GetNextNeighborInfo(otNeighborInfoIterator &aIterator, Neighbor::Info &aNeighInfo);

    /**
     * This method registers the "neighbor table changed" callback function.
     *
     * The provided callback (if non-nullptr) will be invoked when a child/router entry is being added/remove to/from
     * the neighbor table. Subsequent calls to this method will overwrite the previous callback.
     *
     * @param[in] aCallback    A pointer to callback handler function.
     *
     */
    void RegisterCallback(Callback aCallback) { mCallback = aCallback; }

    /**
     * This method signals a "neighbor table changed" event.
     *
     * This method invokes the `NeighborTable::Callback` and also signals the change through a related `Notifier` event.
     *
     * @param[in] aEvent     The event to emit (child/router added/removed, or child mode changed).
     * @param[in] aNeighbor  The neighbor that is being added/removed.
     *
     */
    void Signal(Event aEvent, const Neighbor &aNeighbor);

private:
    Neighbor *FindParent(const Neighbor::AddressMatcher &aMatcher);
    Neighbor *FindNeighbor(const Neighbor::AddressMatcher &aMatcher);
#if OPENTHREAD_FTD
    Neighbor *FindChildOrRouter(const Neighbor::AddressMatcher &aMatcher);
#endif

    Callback mCallback;
};

} // namespace ot

#endif // NEIGHBOR_TABLE_HPP_
