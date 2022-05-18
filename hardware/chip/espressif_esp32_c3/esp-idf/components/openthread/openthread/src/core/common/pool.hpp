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
 *   This file includes definitions for a generic object pool.
 */

#ifndef POOL_HPP_
#define POOL_HPP_

#include "openthread-core-config.h"

#include "common/linked_list.hpp"
#include "common/non_copyable.hpp"

namespace ot {

class Instance;

/**
 * @addtogroup core-pool
 *
 * @brief
 *   This module includes definitions for OpenThread object pool.
 *
 * @{
 *
 */

/**
 * This template class represents an object pool.
 *
 * @tparam Type         The object type. Type should provide `GetNext() and `SetNext()` so that it can be added to a
 *                      linked list.
 * @tparam kPoolSize    Specifies the pool size (maximum number of objects in the pool).
 *
 */
template <class Type, uint16_t kPoolSize> class Pool : private NonCopyable
{
public:
    /**
     * This constructor initializes the pool.
     *
     */
    Pool(void)
        : mFreeList()
    {
        for (Type &entry : mPool)
        {
            mFreeList.Push(entry);
        }
    }

    /**
     * This constructor initializes the pool.
     *
     * This constructor version requires the `Type` class to provide method `void Init(Instance &)` to initialize
     * each `Type` entry object. This can be realized by the `Type` class inheriting from `InstaceLocatorInit()`.
     *
     * @param[in] aInstance   A reference to the OpenThread instance.
     *
     */
    explicit Pool(Instance &aInstance)
        : mFreeList()
    {
        for (Type &entry : mPool)
        {
            entry.Init(aInstance);
            mFreeList.Push(entry);
        }
    }

    /**
     * This method allocates a new object from the pool.
     *
     * @returns A pointer to the newly allocated object, or nullptr if all entries from the pool are already allocated.
     *
     */
    Type *Allocate(void) { return mFreeList.Pop(); }

    /**
     * This method frees a previously allocated object.
     *
     * The @p aEntry MUST be an entry from the pool previously allocated using `Allocate()` method and not yet freed.
     * An already freed entry MUST not be freed again.
     *
     * @param[in]  aEntry   The pool object entry to free.
     *
     */
    void Free(Type &aEntry) { mFreeList.Push(aEntry); }

    /**
     * This method frees all previously allocated objects.
     *
     */
    void FreeAll(void)
    {
        mFreeList.Clear();

        for (Type &entry : mPool)
        {
            mFreeList.Push(entry);
        }
    }

    /**
     * This method returns the pool size.
     *
     * @returns The pool size (maximum number of objects in the pool).
     *
     */
    uint16_t GetSize(void) const { return kPoolSize; }

    /**
     * This method indicates whether or not a given `Type` object is from the pool.
     *
     * @param[in]  aObject   A reference to a `Type` object.
     *
     * @retval TRUE if @p aObject is from the pool.
     * @retval FALSE if @p aObject is not from the pool.
     *
     */
    bool IsPoolEntry(const Type &aObject) const { return (&mPool[0] <= &aObject) && (&aObject < OT_ARRAY_END(mPool)); }

    /**
     * This method returns the associated index of a given entry from the pool.
     *
     * The @p aEntry MUST be from the pool, otherwise the behavior of this method is undefined.
     *
     * @param[in] aEntry  A reference to an entry from the pool.
     *
     * @returns The associated index of @p aEntry.
     *
     */
    uint16_t GetIndexOf(const Type &aEntry) const { return static_cast<uint16_t>(&aEntry - mPool); }

    /**
     * This method retrieves a pool entry at a given index.
     *
     * The @p aIndex MUST be from an earlier call to `GetIndexOf()`.
     *
     * @param[in] aIndex  An index.
     *
     * @returns A reference to entry at index @p aIndex.
     *
     */
    Type &GetEntryAt(uint16_t aIndex) { return mPool[aIndex]; }

    /**
     * This method retrieves a pool entry at a given index.
     *
     * The @p aIndex MUST be from an earlier call to `GetIndexOf()`.
     *
     * @param[in] aIndex  An index.
     *
     * @returns A reference to entry at index @p aIndex.
     *
     */
    const Type &GetEntryAt(uint16_t aIndex) const { return mPool[aIndex]; }

private:
    LinkedList<Type> mFreeList;
    Type             mPool[kPoolSize];
};

/**
 * @}
 *
 */

} // namespace ot

#endif // POOL_HPP_
