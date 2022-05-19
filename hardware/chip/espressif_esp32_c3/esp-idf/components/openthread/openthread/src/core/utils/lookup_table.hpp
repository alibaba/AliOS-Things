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
 *   This file includes definitions for lookup table (binary search) functionality.
 */

#ifndef LOOKUP_TABLE_HPP_
#define LOOKUP_TABLE_HPP_

#include <stdint.h>

#include "common/error.hpp"

namespace ot {
namespace Utils {

/**
 * This class implements lookup table (using binary search) functionality.
 *
 */
class LookupTable
{
public:
    /**
     * This struct represents an example of a lookup table entry.
     *
     */
    struct Entry
    {
        /**
         * This constructor initializes the entry with a given name.
         *
         * @param[in] aName   The null-terminated name string with which to initialize the entry.
         *
         */
        constexpr explicit Entry(const char *aName)
            : mName(aName)
        {
        }

        const char *const mName;
    };

    /**
     * This template method indicates whether a given entry table array is sorted based on entry's name string and in
     * alphabetical order and contains not duplicate entries.
     *
     * This method is `constexpr` and is intended for use in `static_assert`s to verify that a `constexpr` lookup table
     * array is sorted.
     *
     * @tparam EntryType   The table entry type. The `EntryType` MUST provide `mName` member variable as a `const
     *                     char *`. For example, this can be realized by `EntryType` being a subclass of `Entry`.
     * @tparam kLength     The array length (number of entries in the array).
     *
     * @note In the common use of this method as `IsSorted(sTable)` where sTable` is a fixed size array, the template
     * types/parameters do not need to be explicitly specified and can be deduced from the passed-in argument.
     *
     * @param[in] aTable  A reference to an array of `kLength` entries on type `EntryType`
     *
     * @retval TRUE   If the entries in @p aTable are sorted (alphabetical order).
     * @retval FALSE  If the entries in @p aTable are not sorted.
     *
     */
    template <class EntryType, uint16_t kLength> static constexpr bool IsSorted(const EntryType (&aTable)[kLength])
    {
        return IsSorted(&aTable[0], kLength);
    }

    /**
     * This template method performs binary search in a given sorted table array to find an entry matching a given name.
     *
     * @note This method requires the array to be sorted, otherwise its behavior is undefined.
     *
     * @tparam EntryType   The table entry type. The `EntryType` MUST provide `mName` member variable as a `const
     *                     char *`. For example, this can be realized by `EntryType` being a subclass of `Entry`.
     * @tparam kLength     The array length (number of entries in the array).
     *
     * @note In the common use of this method as `Find(name, sTable)` where sTable` is a fixed size array, the template
     * types/parameters do not need to be explicitly specified and can be deduced from the passed-in argument.
     *
     * @param[in] aName   A name string to search for within the table.
     * @param[in] aTable  A reference to an array of `kLength` entries on type `EntryType`
     *
     * @returns A pointer to the entry in the table if a match is found, otherwise nullptr (no match in table).
     *
     */
    template <class EntryType, uint16_t kLength>
    static const EntryType *Find(const char *aName, const EntryType (&aTable)[kLength])
    {
        return reinterpret_cast<const EntryType *>(
            Find(aName, &aTable[0], kLength, sizeof(aTable[0]), GetName<EntryType>));
    }

private:
    typedef const char *(&NameGetter)(const void *aPointer);

    template <class EntryType> static const char *GetName(const void *aPointer)
    {
        return reinterpret_cast<const EntryType *>(aPointer)->mName;
    }

    template <class EntryType> static constexpr bool IsSorted(const EntryType *aTable, uint16_t aLength)
    {
        return (aLength <= 1) ? true
                              : AreInOrder(aTable[0].mName, aTable[1].mName) && IsSorted(aTable + 1, aLength - 1);
    }

    constexpr static bool AreInOrder(const char *aFirst, const char *aSecond)
    {
        return (*aFirst < *aSecond)
                   ? true
                   : ((*aFirst > *aSecond) || (*aFirst == '\0') ? false : AreInOrder(aFirst + 1, aSecond + 1));
    }

    static const void *Find(const char *aName,
                            const void *aTable,
                            uint16_t    aLength,
                            uint16_t    aTableEntrySize,
                            NameGetter  aNameGetter);
};

} // namespace Utils
} // namespace ot

#endif // LOOKUP_TABLE_HPP_
