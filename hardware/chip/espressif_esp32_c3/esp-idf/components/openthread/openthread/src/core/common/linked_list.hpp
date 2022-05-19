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
 *   This file includes definitions for a generic singly linked list.
 */

#ifndef LINKED_LIST_HPP_
#define LINKED_LIST_HPP_

#include "openthread-core-config.h"

#include <stdio.h>

#include "common/const_cast.hpp"
#include "common/error.hpp"
#include "common/iterator_utils.hpp"

namespace ot {

/**
 * @addtogroup core-linked-list
 *
 * @brief
 *   This module includes definitions for OpenThread Singly Linked List.
 *
 * @{
 *
 */

/**
 * This template class represents a linked list entry.
 *
 * This class provides methods to `GetNext()` and `SetNext()` in the linked list entry.
 *
 * Users of this class should follow CRTP-style inheritance, i.e., the `Type` class itself should publicly inherit
 * from `LinkedListEntry<Type>`.
 *
 * The template type `Type` should contain a `mNext` member variable. The `mNext` should be of a type that can be
 * down-casted to `Type` itself.
 *
 */
template <class Type> class LinkedListEntry
{
public:
    /**
     * This method gets the next entry in the linked list.
     *
     * @returns A pointer to the next entry in the linked list or nullptr if at the end of the list.
     *
     */
    const Type *GetNext(void) const { return static_cast<const Type *>(static_cast<const Type *>(this)->mNext); }

    /**
     * This method gets the next entry in the linked list.
     *
     * @returns A pointer to the next entry in the linked list or nullptr if at the end of the list.
     *
     */
    Type *GetNext(void) { return static_cast<Type *>(static_cast<Type *>(this)->mNext); }

    /**
     * This method sets the next pointer on the entry.
     *
     * @param[in] aNext  A pointer to the next entry.
     *
     */
    void SetNext(Type *aNext) { static_cast<Type *>(this)->mNext = aNext; }
};

/**
 * This template class represents a singly linked list.
 *
 * The template type `Type` should provide `GetNext()` and `SetNext()` methods (which can be realized by `Type`
 * inheriting from `LinkedListEntry<Type>` class).
 *
 */
template <typename Type> class LinkedList
{
    class Iterator;
    class ConstIterator;

public:
    /**
     * This constructor initializes the linked list.
     *
     */
    LinkedList(void)
        : mHead(nullptr)
    {
    }

    /**
     * This method returns the entry at the head of the linked list
     *
     * @returns Pointer to the entry at the head of the linked list, or nullptr if the list is empty.
     *
     */
    Type *GetHead(void) { return mHead; }

    /**
     * This method returns the entry at the head of the linked list.
     *
     * @returns Pointer to the entry at the head of the linked list, or nullptr if the list is empty.
     *
     */
    const Type *GetHead(void) const { return mHead; }

    /**
     * This method sets the head of the linked list to a given entry.
     *
     * @param[in] aHead   A pointer to an entry to set as the head of the linked list.
     *
     */
    void SetHead(Type *aHead) { mHead = aHead; }

    /**
     * This method clears the linked list.
     *
     */
    void Clear(void) { mHead = nullptr; }

    /**
     * This method indicates whether the linked list is empty or not.
     *
     * @retval TRUE   If the linked list is empty.
     * @retval FALSE  If the linked list is not empty.
     *
     */
    bool IsEmpty(void) const { return (mHead == nullptr); }

    /**
     * This method pushes an entry at the head of the linked list.
     *
     * @param[in] aEntry   A reference to an entry to push at the head of linked list.
     *
     */
    void Push(Type &aEntry)
    {
        aEntry.SetNext(mHead);
        mHead = &aEntry;
    }

    /**
     * This method pushes an entry after a given previous existing entry in the linked list.
     *
     * @param[in] aEntry       A reference to an entry to push into the list.
     * @param[in] aPrevEntry   A reference to a previous entry (new entry @p aEntry will be pushed after this).
     *
     */
    void PushAfter(Type &aEntry, Type &aPrevEntry)
    {
        aEntry.SetNext(aPrevEntry.GetNext());
        aPrevEntry.SetNext(&aEntry);
    }

    /**
     * This method pops an entry from head of the linked list.
     *
     * @note This method does not change the popped entry itself, i.e., the popped entry next pointer stays as before.
     *
     * @returns The entry that was popped if the list is not empty, or nullptr if the list is empty.
     *
     */
    Type *Pop(void)
    {
        Type *entry = mHead;

        if (mHead != nullptr)
        {
            mHead = mHead->GetNext();
        }

        return entry;
    }

    /**
     * This method pops an entry after a given previous entry.
     *
     * @note This method does not change the popped entry itself, i.e., the popped entry next pointer stays as before.
     *
     * @param[in] aPrevEntry  A pointer to a previous entry. If it is not nullptr the entry after this will be popped,
     *                        otherwise (if it is nullptr) the entry at the head of the list is popped.
     *
     * @returns Pointer to the entry that was popped, or nullptr if there is no entry to pop.
     *
     */
    Type *PopAfter(Type *aPrevEntry)
    {
        Type *entry;

        if (aPrevEntry == nullptr)
        {
            entry = Pop();
        }
        else
        {
            entry = aPrevEntry->GetNext();

            if (entry != nullptr)
            {
                aPrevEntry->SetNext(entry->GetNext());
            }
        }

        return entry;
    }

    /**
     * This method indicates whether the linked list contains a given entry.
     *
     * @param[in] aEntry   A reference to an entry.
     *
     * @retval TRUE   The linked list contains @p aEntry.
     * @retval FALSE  The linked list does not contain @p aEntry.
     *
     */
    bool Contains(const Type &aEntry) const
    {
        const Type *prev;

        return Find(aEntry, prev) == kErrorNone;
    }

    /**
     * This template method indicates whether the linked list contains an entry matching a given entry indicator.
     *
     * The template type `Indicator` specifies the type of @p aIndicator object which is used to match against entries
     * in the list. To check that an entry matches the given indicator, the `Matches()` method is invoked on each
     * `Type` entry in the list. The `Matches()` method should be provided by `Type` class accordingly:
     *
     *     bool Type::Matches(const Indicator &aIndicator) const
     *
     * @param[in] aIndicator   An entry indicator to match against entries in the list.
     *
     * @retval TRUE   The linked list contains an entry matching @p aIndicator.
     * @retval FALSE  The linked list contains no entry matching @p aIndicator.
     *
     */
    template <typename Indicator> bool ContainsMatching(const Indicator &aIndicator) const
    {
        return FindMatching(aIndicator) != nullptr;
    }

    /**
     * This method adds an entry (at the head of the linked list) if it is not already in the list.
     *
     * @param[in] aEntry   A reference to an entry to add.
     *
     * @retval kErrorNone     The entry was successfully added at the head of the list.
     * @retval kErrorAlready  The entry is already in the list.
     *
     */
    Error Add(Type &aEntry)
    {
        Error error = kErrorNone;

        if (Contains(aEntry))
        {
            error = kErrorAlready;
        }
        else
        {
            Push(aEntry);
        }

        return error;
    }

    /**
     * This method removes an entry from the linked list.
     *
     * @note This method does not change the removed entry @p aEntry itself (it is `const`), i.e., the entry next
     * pointer of @p aEntry stays as before.
     *
     * @param[in] aEntry   A reference to an entry to remove.
     *
     * @retval kErrorNone      The entry was successfully removed from the list.
     * @retval kErrorNotFound  Could not find the entry in the list.
     *
     */
    Error Remove(const Type &aEntry)
    {
        Type *prev;
        Error error = Find(aEntry, prev);

        if (error == kErrorNone)
        {
            PopAfter(prev);
        }

        return error;
    }

    /**
     * This template method removes an entry matching a given entry indicator from the linked list.
     *
     * The template type `Indicator` specifies the type of @p aIndicator object which is used to match against entries
     * in the list. To check that an entry matches the given indicator, the `Matches()` method is invoked on each
     * `Type` entry in the list. The `Matches()` method should be provided by `Type` class accordingly:
     *
     *     bool Type::Matches(const Indicator &aIndicator) const
     *
     * @note This method does not change the removed entry itself (which is returned in case of success), i.e., the
     * entry next pointer stays as before.
     *
     *
     * @param[in] aIndicator   An entry indicator to match against entries in the list.
     *
     * @returns A pointer to the removed matching entry if one could be found, or nullptr if no matching entry is found.
     *
     */
    template <typename Indicator> Type *RemoveMatching(const Indicator &aIndicator)
    {
        Type *prev;
        Type *entry = FindMatching(aIndicator, prev);

        if (entry != nullptr)
        {
            PopAfter(prev);
        }

        return entry;
    }

    /**
     * This method searches within the linked list to find an entry and if found returns a pointer to previous entry.
     *
     * @param[in]  aEntry      A reference to an entry to find.
     * @param[out] aPrevEntry  A pointer to output the previous entry on success (when @p aEntry is found in the list).
     *                         @p aPrevEntry is set to nullptr if @p aEntry is the head of the list. Otherwise it is
     *                         updated to point to the previous entry before @p aEntry in the list.
     *
     * @retval kErrorNone      The entry was found in the list and @p aPrevEntry was updated successfully.
     * @retval kErrorNotFound  The entry was not found in the list.
     *
     */
    Error Find(const Type &aEntry, const Type *&aPrevEntry) const
    {
        Error error = kErrorNotFound;

        aPrevEntry = nullptr;

        for (const Type *entry = mHead; entry != nullptr; aPrevEntry = entry, entry = entry->GetNext())
        {
            if (entry == &aEntry)
            {
                error = kErrorNone;
                break;
            }
        }

        return error;
    }

    /**
     * This method searches within the linked list to find an entry and if found returns a pointer to previous entry.
     *
     * @param[in]  aEntry      A reference to an entry to find.
     * @param[out] aPrevEntry  A pointer to output the previous entry on success (when @p aEntry is found in the list).
     *                         @p aPrevEntry is set to nullptr if @p aEntry is the head of the list. Otherwise it is
     *                         updated to point to the previous entry before @p aEntry in the list.
     *
     * @retval kErrorNone      The entry was found in the list and @p aPrevEntry was updated successfully.
     * @retval kErrorNotFound  The entry was not found in the list.
     *
     */
    Error Find(const Type &aEntry, Type *&aPrevEntry)
    {
        return AsConst(this)->Find(aEntry, const_cast<const Type *&>(aPrevEntry));
    }

    /**
     * This template method searches within a given range of the linked list to find an entry matching a given
     * indicator.
     *
     * The template type `Indicator` specifies the type of @p aIndicator object which is used to match against entries
     * in the list. To check that an entry matches the given indicator, the `Matches()` method is invoked on each
     * `Type` entry in the list. The `Matches()` method should be provided by `Type` class accordingly:
     *
     *     bool Type::Matches(const Indicator &aIndicator) const
     *
     * @param[in]  aBegin      A pointer to the begin of the range.
     * @param[in]  aEnd        A pointer to the end of the range, or nullptr to search all entries after @p aBegin.
     * @param[in]  aIndicator  An indicator to match with entries in the list.
     * @param[out] aPrevEntry  A pointer to output the previous entry on success (when a match is found in the list).
     *                         @p aPrevEntry is set to nullptr if the matching entry is the head of the list. Otherwise
     *                         it is updated to point to the previous entry before the matching entry in the list.
     *
     * @returns A pointer to the matching entry if one is found, or nullptr if no matching entry was found.
     *
     */
    template <typename Indicator>
    const Type *FindMatching(const Type *     aBegin,
                             const Type *     aEnd,
                             const Indicator &aIndicator,
                             const Type *&    aPrevEntry) const
    {
        const Type *entry;

        aPrevEntry = nullptr;

        for (entry = aBegin; entry != aEnd; aPrevEntry = entry, entry = entry->GetNext())
        {
            if (entry->Matches(aIndicator))
            {
                break;
            }
        }

        return entry;
    }

    /**
     * This template method searches within a given range of the linked list to find an entry matching a given
     * indicator.
     *
     * The template type `Indicator` specifies the type of @p aIndicator object which is used to match against entries
     * in the list. To check that an entry matches the given indicator, the `Matches()` method is invoked on each
     * `Type` entry in the list. The `Matches()` method should be provided by `Type` class accordingly:
     *
     *     bool Type::Matches(const Indicator &aIndicator) const
     *
     * @param[in]  aBegin      A pointer to the begin of the range.
     * @param[in]  aEnd        A pointer to the end of the range, or nullptr to search all entries after @p aBegin.
     * @param[in]  aIndicator  An indicator to match with entries in the list.
     * @param[out] aPrevEntry  A pointer to output the previous entry on success (when a match is found in the list).
     *                         @p aPrevEntry is set to nullptr if the matching entry is the head of the list. Otherwise
     *                         it is updated to point to the previous entry before the matching entry in the list.
     *
     * @returns A pointer to the matching entry if one is found, or nullptr if no matching entry was found.
     *
     */
    template <typename Indicator>
    Type *FindMatching(const Type *aBegin, const Type *aEnd, const Indicator &aIndicator, Type *&aPrevEntry)
    {
        return AsNonConst(FindMatching(aBegin, aEnd, aIndicator, const_cast<const Type *&>(aPrevEntry)));
    }

    /**
     * This template method searches within the linked list to find an entry matching a given indicator.
     *
     * The template type `Indicator` specifies the type of @p aIndicator object which is used to match against entries
     * in the list. To check that an entry matches the given indicator, the `Matches()` method is invoked on each
     * `Type` entry in the list. The `Matches()` method should be provided by `Type` class accordingly:
     *
     *     bool Type::Matches(const Indicator &aIndicator) const
     *
     * @param[in]  aIndicator  An indicator to match with entries in the list.
     * @param[out] aPrevEntry  A pointer to output the previous entry on success (when a match is found in the list).
     *                         @p aPrevEntry is set to nullptr if the matching entry is the head of the list. Otherwise
     *                         it is updated to point to the previous entry before the matching entry in the list.
     *
     * @returns A pointer to the matching entry if one is found, or nullptr if no matching entry was found.
     *
     */
    template <typename Indicator> const Type *FindMatching(const Indicator &aIndicator, const Type *&aPrevEntry) const
    {
        return FindMatching(mHead, nullptr, aIndicator, aPrevEntry);
    }

    /**
     * This template method searches within the linked list to find an entry matching a given indicator, and if found
     * returns a pointer to its previous entry in the list.
     *
     * The template type `Indicator` specifies the type of @p aIndicator object which is used to match against entries
     * in the list. To check that an entry matches the given indicator, the `Matches()` method is invoked on each
     * `Type` entry in the list. The `Matches()` method should be provided by `Type` class accordingly:
     *
     *     bool Type::Matches(const Indicator &aIndicator) const
     *
     * @param[in]  aIndicator  An indicator to match with entries in the list.
     * @param[out] aPrevEntry  A pointer to output the previous entry on success (when a match is found in the list).
     *                         @p aPrevEntry is set to nullptr if the matching entry is the head of the list. Otherwise
     *                         it is updated to point to the previous entry before the matching entry in the list.
     *
     * @returns A pointer to the matching entry if one is found, or nullptr if no matching entry was found.
     *
     */
    template <typename Indicator> Type *FindMatching(const Indicator &aIndicator, Type *&aPrevEntry)
    {
        return AsNonConst(AsConst(this)->FindMatching(aIndicator, const_cast<const Type *&>(aPrevEntry)));
    }

    /**
     * This template method searches within the linked list to find an entry matching a given indicator.
     *
     * The template type `Indicator` specifies the type of @p aIndicator object which is used to match against entries
     * in the list. To check that an entry matches the given indicator, the `Matches()` method is invoked on each
     * `Type` entry in the list. The `Matches()` method should be provided by `Type` class accordingly:
     *
     *     bool Type::Matches(const Indicator &aIndicator) const
     *
     * @param[in]  aIndicator  An indicator to match with entries in the list.
     *
     * @returns A pointer to the matching entry if one is found, or nullptr if no matching entry was found.
     *
     */
    template <typename Indicator> const Type *FindMatching(const Indicator &aIndicator) const
    {
        const Type *prev;

        return FindMatching(aIndicator, prev);
    }

    /**
     * This template method searches within the linked list to find an entry matching a given indicator.
     *
     * The template type `Indicator` specifies the type of @p aIndicator object which is used to match against entries
     * in the list. To check that an entry matches the given indicator, the `Matches()` method is invoked on each
     * `Type` entry in the list. The `Matches()` method should be provided by `Type` class accordingly:
     *
     *     bool Type::Matches(const Indicator &aIndicator) const
     *
     * @param[in]  aIndicator  An indicator to match with entries in the list.
     *
     * @returns A pointer to the matching entry if one is found, or nullptr if no matching entry was found.
     *
     */
    template <typename Indicator> Type *FindMatching(const Indicator &aIndicator)
    {
        return AsNonConst(AsConst(this)->FindMatching(aIndicator));
    }

    /**
     * This method returns the tail of the linked list (i.e., the last entry in the list).
     *
     * @returns A pointer to the tail entry in the linked list or nullptr if the list is empty.
     *
     */
    const Type *GetTail(void) const
    {
        const Type *tail = mHead;

        if (tail != nullptr)
        {
            while (tail->GetNext() != nullptr)
            {
                tail = tail->GetNext();
            }
        }

        return tail;
    }

    /**
     * This method returns the tail of the linked list (i.e., the last entry in the list).
     *
     * @returns A pointer to the tail entry in the linked list or nullptr if the list is empty.
     *
     */
    Type *GetTail(void) { return AsNonConst(AsConst(this)->GetTail()); }

    // The following methods are intended to support range-based `for`
    // loop iteration over the linked-list entries and should not be
    // used directly.

    Iterator begin(void) { return Iterator(GetHead()); }
    Iterator end(void) { return Iterator(nullptr); }

    ConstIterator begin(void) const { return ConstIterator(GetHead()); }
    ConstIterator end(void) const { return ConstIterator(nullptr); }

private:
    class Iterator : public ItemPtrIterator<Type, Iterator>
    {
        friend class LinkedList;
        friend class ItemPtrIterator<Type, Iterator>;

        using ItemPtrIterator<Type, Iterator>::mItem;

        explicit Iterator(Type *aItem)
            : ItemPtrIterator<Type, Iterator>(aItem)
        {
        }

        void Advance(void) { mItem = mItem->GetNext(); }
    };

    class ConstIterator : public ItemPtrIterator<const Type, ConstIterator>
    {
        friend class LinkedList;
        friend class ItemPtrIterator<const Type, ConstIterator>;

        using ItemPtrIterator<const Type, ConstIterator>::mItem;

        explicit ConstIterator(const Type *aItem)
            : ItemPtrIterator<const Type, ConstIterator>(aItem)
        {
        }

        void Advance(void) { mItem = mItem->GetNext(); }
    };

    Type *mHead;
};

/**
 * @}
 *
 */

} // namespace ot

#endif // LINKED_LIST_HPP_
