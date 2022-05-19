/*
 *    Copyright (c) 2016-2017, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 *    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file contains definitions a spinel interface to the OpenThread stack.
 */

#ifndef CHANGED_PROPS_SET_HPP_
#define CHANGED_PROPS_SET_HPP_

#include "openthread-core-config.h"

#include <stddef.h>

#include <openthread/error.h>

#include "lib/spinel/spinel.h"

namespace ot {
namespace Ncp {

/**
 * Defines a class to track a set of property/status changes that require update to host. The properties that can
 * be added to this set must support sending unsolicited updates. This class also provides mechanism for user
 * to block certain filterable properties disallowing the unsolicited update from them.
 *
 */
class ChangedPropsSet
{
public:
    /**
     * Defines an entry in the set/list.
     *
     */
    struct Entry
    {
        spinel_prop_key_t mPropKey;    ///< The spinel property key.
        spinel_status_t   mStatus;     ///< The spinel status (used only if prop key is `LAST_STATUS`).
        bool              mFilterable; ///< Indicates whether the entry can be filtered
    };

    /**
     * This constructor initializes the set.
     *
     */
    ChangedPropsSet(void)
        : mChangedSet(0)
        , mFilterSet(0)
    {
    }

    /**
     * This method clears the set.
     *
     */
    void Clear(void) { mChangedSet = 0; }

    /**
     * This method indicates if the set is empty or not.
     *
     * @returns TRUE if the set if empty, FALSE otherwise.
     *
     */
    bool IsEmpty(void) const { return (mChangedSet == 0); }

    /**
     * This method adds a property to the set. The property added must be in the list of supported properties
     * capable of sending unsolicited update, otherwise the input is ignored.
     *
     * Note that if the property is already in the set, adding it again does not change the set.
     *
     * @param[in] aPropKey    The spinel property key to be added to the set
     *
     */
    void AddProperty(spinel_prop_key_t aPropKey) { Add(aPropKey, SPINEL_STATUS_OK); }

    /**
     * This method adds a `LAST_STATUS` update to the set. The update must be in list of supported entries.
     *
     * @param[in] aStatus     The spinel status update to be added to set.
     *
     */
    void AddLastStatus(spinel_status_t aStatus) { Add(SPINEL_PROP_LAST_STATUS, aStatus); }

    /**
     * This method returns a pointer to array of entries of supported property/status updates. The list includes
     * all properties that can generate unsolicited update.
     *
     * @param[out]  aNumEntries  A reference to output the number of entries in the list.
     *
     * @returns A pointer to the supported entries array.
     *
     */
    const Entry *GetSupportedEntries(uint8_t &aNumEntries) const
    {
        aNumEntries = GetNumEntries();
        return &mSupportedProps[0];
    }

    /**
     * This method returns a pointer to the entry associated with a given index.
     *
     * @param[in] aIndex     The index to an entry.
     *
     * @returns A pointer to the entry associated with @p aIndex, or nullptr if the index is beyond end of array.
     *
     */
    const Entry *GetEntry(uint8_t aIndex) const
    {
        return (aIndex < GetNumEntries()) ? &mSupportedProps[aIndex] : nullptr;
    }

    /**
     * This method indicates if the entry associated with an index is in the set (i.e., it has been changed and
     * requires an unsolicited update).
     *
     * @param[in] aIndex     The index to an entry.
     *
     * @returns TRUE if the entry is in the set, FALSE otherwise.
     *
     */
    bool IsEntryChanged(uint8_t aIndex) const { return IsBitSet(mChangedSet, aIndex); }

    /**
     * This method removes an entry associated with an index in the set.
     *
     * Note that if the property/entry is not in the set, removing it simply does nothing.
     *
     * @param[in] aIndex               Index of entry to be removed.
     *
     */
    void RemoveEntry(uint8_t aIndex) { ClearBit(mChangedSet, aIndex); }

    /**
     * This method enables/disables filtering of a given property.
     *
     * @param[in] aPropKey             The property key to filter.
     * @param[in] aEnable              TRUE to enable filtering, FALSE to disable.
     *
     * @retval OT_ERROR_NONE           Filter state for given property updated successfully.
     * @retval OT_ERROR_INVALID_ARGS   The given property is not valid (i.e., not capable of unsolicited update).
     *
     */
    otError EnablePropertyFilter(spinel_prop_key_t aPropKey, bool aEnable);

    /**
     * This method determines whether filtering is enabled for an entry associated with an index.
     *
     * @param[in] aIndex               Index of entry to be checked.
     *
     * @returns TRUE if the filter is enabled for the given entry, FALSE otherwise.
     *
     */
    bool IsEntryFiltered(uint8_t aIndex) const { return IsBitSet(mFilterSet, aIndex); }

    /**
     * This method determines whether filtering is enabled for a given property key.
     *
     * @param[in] aPropKey             The property key to check.
     *
     * @returns TRUE if the filter is enabled for the given property, FALSE if the property is not filtered or if
     *          it is not filterable.
     *
     */
    bool IsPropertyFiltered(spinel_prop_key_t aPropKey) const;

    /**
     * This method clears the filter.
     *
     */
    void ClearFilter(void) { mFilterSet = 0; }

private:
    uint8_t GetNumEntries(void) const;
    void    Add(spinel_prop_key_t aPropKey, spinel_status_t aStatus);

    static void SetBit(uint64_t &aBitset, uint8_t aBitIndex) { aBitset |= (1ULL << aBitIndex); }
    static void ClearBit(uint64_t &aBitset, uint8_t aBitIndex) { aBitset &= ~(1ULL << aBitIndex); }
    static bool IsBitSet(uint64_t aBitset, uint8_t aBitIndex) { return (aBitset & (1ULL << aBitIndex)) != 0; }

    static const Entry mSupportedProps[];

    uint64_t mChangedSet;
    uint64_t mFilterSet;
};

} // namespace Ncp
} // namespace ot

#endif // CHANGED_PROPS_SET_HPP
