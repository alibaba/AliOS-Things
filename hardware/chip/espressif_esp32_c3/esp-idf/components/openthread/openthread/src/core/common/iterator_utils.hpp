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
 *   This file includes definitions for a generic item-pointer iterator class.
 */

#ifndef ITERATOR_UTILS_HPP_
#define ITERATOR_UTILS_HPP_

namespace ot {

/**
 * @addtogroup core-iterator-utils
 *
 * @brief
 *   This module includes definitions for OpenThread generic item-pointer iterator class.
 *
 * @{
 *
 */

/**
 * This template class is used as a base class for those item-pointer iterators.
 *
 * These iterators have common methods and operators like `Advance()` and `++` and hold a pointer to the
 * object.
 *
 * Users of this class should follow CRTP-style inheritance, i.e., `IteratorType` class itself should publicly inherit
 * from `ItemPtrIterator<ItemType, IteratorType>`.
 *
 * @tparam  ItemType      The type of the object that the iterator points to.
 * @tparam  IteratorType  The Iterator class that inherits this class. The class MUST have a method `Advance()` which
 *                        moves the pointer to the next. `Advance()` SHALL NOT be called when `IsDone()` is `true` and
 *                        would set the pointer to `nullptr` when there's no more elements.
 *
 */
template <typename ItemType, typename IteratorType> class ItemPtrIterator
{
public:
    /**
     * This method indicates whether there are no more items to be accessed (iterator has reached the end).
     *
     * @retval TRUE   There are no more items to be accessed (iterator has reached the end).
     * @retval FALSE  The current item is valid.
     *
     */
    bool IsDone(void) const { return mItem == nullptr; }

    /**
     * This method overloads `++` operator (pre-increment) to advance the iterator.
     *
     * The iterator is moved to point to the next item using IteratorType's `Advance` method.
     * If there are no more items, the iterator becomes empty (i.e., `operator*` returns `nullptr` and `IsDone()`
     * returns `true`).
     *
     */
    void operator++(void) { static_cast<IteratorType *>(this)->Advance(); }

    /**
     * This method overloads `++` operator (post-increment) to advance the iterator.
     *
     * The iterator is moved to point to the next item using IteratorType's `Advance` method.
     * If there are no more items, the iterator becomes empty (i.e., `operator*` returns `nullptr` and `IsDone()`
     * returns `true`).
     *
     */
    void operator++(int) { static_cast<IteratorType *>(this)->Advance(); }

    /**
     * This method overloads the `*` dereference operator and gets a reference to then item to which the iterator is
     * currently pointing.
     *
     * This method MUST be used when the iterator is not empty/finished (i.e., `IsDone()` returns `false`).
     *
     * @returns A reference to the item currently pointed by the iterator.
     *
     */
    ItemType &operator*(void) { return *mItem; }

    /**
     * This method overloads the `->` dereference operator and gets a pointer to the item to which the iterator is
     * currently pointing.
     *
     * @returns A pointer to the item associated with the iterator, or `nullptr` if iterator is empty/done.
     *
     */
    ItemType *operator->(void) { return mItem; }

    /**
     * This method overloads operator `==` to evaluate whether or not two `Iterator` instances point to the same
     * item.
     *
     * @param[in]  aOther  The other `Iterator` to compare with.
     *
     * @retval TRUE   If the two `Iterator` objects point to the same item or both are done.
     * @retval FALSE  If the two `Iterator` objects do not point to the same item.
     *
     */
    bool operator==(const IteratorType &aOther) const { return mItem == aOther.mItem; }

    /**
     * This method overloads operator `!=` to evaluate whether or not two `Iterator` instances point to the same
     * child entry.
     *
     * @param[in]  aOther  The other `Iterator` to compare with.
     *
     * @retval TRUE   If the two `Iterator` objects do not point to the same item.
     * @retval FALSE  If the two `Iterator` objects point to the same item or both are done.
     *
     */
    bool operator!=(const IteratorType &aOther) const { return mItem != aOther.mItem; }

protected:
    /**
     * Default constructor
     *
     */
    ItemPtrIterator(void)
        : mItem(nullptr)
    {
    }

    /**
     * Constructor with an Item pointer.
     *
     */
    explicit ItemPtrIterator(ItemType *item)
        : mItem(item)
    {
    }

    ItemType *mItem;
};

/**
 * @}
 *
 */

} // namespace ot

#endif // ITERATOR_UTILS_HPP_
