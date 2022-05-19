/*
 *  Copyright (c) 2016-2018, The OpenThread Authors.
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
 *   This file includes definitions for Thread child table.
 */

#ifndef CHILD_TABLE_HPP_
#define CHILD_TABLE_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_FTD

#include "common/const_cast.hpp"
#include "common/iterator_utils.hpp"
#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "thread/topology.hpp"

namespace ot {

/**
 * This class represents the Thread child table.
 *
 */
class ChildTable : public InstanceLocator, private NonCopyable
{
    friend class NeighborTable;
    class IteratorBuilder;

public:
    /**
     * This class represents an iterator for iterating through the child entries in the child table.
     *
     */
    class Iterator : public InstanceLocator, public ItemPtrIterator<Child, Iterator>
    {
        friend class ItemPtrIterator<Child, Iterator>;
        friend class IteratorBuilder;

    public:
        /**
         * This constructor initializes an `Iterator` instance.
         *
         * @param[in] aInstance  A reference to the OpenThread instance.
         * @param[in] aFilter    A child state filter.
         *
         */
        Iterator(Instance &aInstance, Child::StateFilter aFilter);

        /**
         * This method resets the iterator to start over.
         *
         */
        void Reset(void);

        /**
         * This method gets the `Child` entry to which the iterator is currently pointing.
         *
         * @returns A pointer to the `Child` entry, or `nullptr` if the iterator is done and/or empty.
         *
         */
        Child *GetChild(void) { return mItem; }

    private:
        explicit Iterator(Instance &aInstance)
            : InstanceLocator(aInstance)
            , mFilter(Child::StateFilter::kInStateValid)
        {
        }

        void Advance(void);

        Child::StateFilter mFilter;
    };

    /**
     * This constructor initializes a `ChildTable` instance.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     *
     */
    explicit ChildTable(Instance &aInstance);

    /**
     * This method clears the child table.
     *
     */
    void Clear(void);

    /**
     * This method returns the child table index for a given `Child` instance.
     *
     * @param[in]  aChild  A reference to a `Child`
     *
     * @returns The index corresponding to @p aChild.
     *
     */
    uint16_t GetChildIndex(const Child &aChild) const { return static_cast<uint16_t>(&aChild - mChildren); }

    /**
     * This method returns a pointer to a `Child` entry at a given index, or `nullptr` if the index is out of bounds,
     * i.e., index is larger or equal to maximum number of children allowed (@sa GetMaxChildrenAllowed()).
     *
     * @param[in]  aChildIndex  A child index.
     *
     * @returns A pointer to the `Child` corresponding to the given index, or `nullptr` if the index is out of bounds.
     *
     */
    Child *GetChildAtIndex(uint16_t aChildIndex);

    /**
     * This method gets a new/unused `Child` entry from the child table.
     *
     * @note The returned child entry will be cleared (`memset` to zero).
     *
     * @returns A pointer to a new `Child` entry, or `nullptr` if all `Child` entries are in use.
     *
     */
    Child *GetNewChild(void);

    /**
     * This method searches the child table for a `Child` with a given RLOC16 also matching a given state filter.
     *
     * @param[in]  aRloc16  A RLOC16 address.
     * @param[in]  aFilter  A child state filter.
     *
     * @returns  A pointer to the `Child` entry if one is found, or `nullptr` otherwise.
     *
     */
    Child *FindChild(uint16_t aRloc16, Child::StateFilter aFilter);

    /**
     * This method searches the child table for a `Child` with a given extended address also matching a given state
     * filter.
     *
     * @param[in]  aExtAddress A reference to an extended address.
     * @param[in]  aFilter     A child state filter.
     *
     * @returns  A pointer to the `Child` entry if one is found, or `nullptr` otherwise.
     *
     */
    Child *FindChild(const Mac::ExtAddress &aExtAddress, Child::StateFilter aFilter);

    /**
     * This method searches the child table for a `Child` with a given address also matching a given state filter.
     *
     * @param[in]  aMacAddress A reference to a MAC address.
     * @param[in]  aFilter     A child state filter.
     *
     * @returns  A pointer to the `Child` entry if one is found, or `nullptr` otherwise.
     *
     */
    Child *FindChild(const Mac::Address &aMacAddress, Child::StateFilter aFilter);

    /**
     * This method indicates whether the child table contains any child matching a given state filter.
     *
     * @param[in]  aFilter  A child state filter.
     *
     * @returns  TRUE if the table contains at least one child table matching the given filter, FALSE otherwise.
     *
     */
    bool HasChildren(Child::StateFilter aFilter) const;

    /**
     * This method returns the number of children in the child table matching a given state filter.
     *
     * @param[in]  aFilter  A child state filter.
     *
     * @returns Number of children matching the given state filer.
     *
     */
    uint16_t GetNumChildren(Child::StateFilter aFilter) const;

    /**
     * This method returns the maximum number of children that can be supported (build-time constant).
     *
     * @note Number of children allowed (from `GetMaxChildrenAllowed()`) can be less than maximum number of supported
     * children.
     *
     * @returns  The maximum number of children supported
     *
     */
    uint16_t GetMaxChildren(void) const { return kMaxChildren; }

    /**
     * This method get the maximum number of children allowed.
     *
     * @returns  The maximum number of children allowed.
     *
     */
    uint16_t GetMaxChildrenAllowed(void) const { return mMaxChildrenAllowed; }

    /**
     * This method sets the maximum number of children allowed.
     *
     * The number of children allowed must be at least one and at most same as maximum supported children (@sa
     * GetMaxChildren()). It can be changed only if the child table is empty.
     *
     * @param[in]  aMaxChildren  Maximum number of children allowed.
     *
     * @retval kErrorNone         The number of allowed children changed successfully.
     * @retval kErrorInvalidArgs  If @p aMaxChildren is not in the range [1, Max supported children].
     * @retval kErrorInvalidState The child table is not empty.
     *
     */
    Error SetMaxChildrenAllowed(uint16_t aMaxChildren);

    /**
     * This method enables range-based `for` loop iteration over all child entries in the child table matching a given
     * state filter.
     *
     * This method should be used as follows:
     *
     *     for (Child &child : aChildTable.Iterate(aFilter)) { ... }
     *
     * @param[in] aFilter  A child state filter.
     *
     * @returns An IteratorBuilder instance.
     *
     */
    IteratorBuilder Iterate(Child::StateFilter aFilter) { return IteratorBuilder(GetInstance(), aFilter); }

    /**
     * This method retains diagnostic information for an attached child by Child ID or RLOC16.
     *
     * @param[in]   aChildId    The Child ID or RLOC16 for an attached child.
     * @param[out]  aChildInfo  A reference to a `Child::Info` to populate with the child information.
     *
     */
    Error GetChildInfoById(uint16_t aChildId, Child::Info &aChildInfo);

    /**
     * This method retains diagnostic information for an attached child by the internal table index.
     *
     * @param[in]   aChildIndex  The table index.
     * @param[out]  aChildInfo   A reference to a `Child::Info` to populate with the child information.
     *
     */
    Error GetChildInfoByIndex(uint16_t aChildIndex, Child::Info &aChildInfo);

    /**
     * This method restores child table from non-volatile memory.
     *
     */
    void Restore(void);

    /**
     * This method removes a stored child information from non-volatile memory.
     *
     * @param[in]  aChildRloc16     A reference to the child to remove from non-volatile memory.
     *
     */
    void RemoveStoredChild(const Child &aChild);

    /**
     * This method store a child information into non-volatile memory.
     *
     * @param[in]  aChild          A reference to the child to store.
     *
     * @retval  kErrorNone     Successfully store child.
     * @retval  kErrorNoBufs   Insufficient available buffers to store child.
     *
     */
    Error StoreChild(const Child &aChild);

    /**
     * This method indicates whether the child table contains any sleepy child (in states valid or restoring) with a
     * given IPv6 address.
     *
     * @param[in]  aIp6Address  An IPv6 address.
     *
     * @retval TRUE   If the child table contains any sleepy child with @p aIp6Address.
     * @retval FALSE  If the child table does not contain any sleepy child with @p aIp6Address.
     *
     */
    bool HasSleepyChildWithAddress(const Ip6::Address &aIp6Address) const;

private:
    static constexpr uint16_t kMaxChildren = OPENTHREAD_CONFIG_MLE_MAX_CHILDREN;

    class IteratorBuilder : public InstanceLocator
    {
    public:
        IteratorBuilder(Instance &aInstance, Child::StateFilter aFilter)
            : InstanceLocator(aInstance)
            , mFilter(aFilter)
        {
        }

        Iterator begin(void) { return Iterator(GetInstance(), mFilter); }
        Iterator end(void) { return Iterator(GetInstance()); }

    private:
        Child::StateFilter mFilter;
    };

    Child *FindChild(const Child::AddressMatcher &aMatcher) { return AsNonConst(AsConst(this)->FindChild(aMatcher)); }

    const Child *FindChild(const Child::AddressMatcher &aMatcher) const;
    void         RefreshStoredChildren(void);

    uint16_t mMaxChildrenAllowed;
    Child    mChildren[kMaxChildren];
};

} // namespace ot

#endif // OPENTHREAD_FTD

#endif // CHILD_TABLE_HPP_
