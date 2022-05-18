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

#include <string.h>

#include "test_platform.h"

#include <openthread/config.h>

#include "common/instance.hpp"
#include "utils/lookup_table.hpp"

#include "test_util.h"

typedef ot::Utils::LookupTable::Entry Entry;

struct TableEntryBase
{
    constexpr explicit TableEntryBase(uint8_t aValue)
        : mValue(aValue)
    {
    }

    uint8_t mValue;
};

struct TableEntry : public TableEntryBase, public Entry
{
    constexpr TableEntry(const char *aName, uint8_t aValue)
        : TableEntryBase(aValue)
        , Entry(aName)
        , mUint16(0xabba)
        , mUint8(aValue)
    {
    }

    uint16_t mUint16;
    uint8_t  mUint8;
};

void TestLookupTable(void)
{
    enum : uint16_t
    {
        kMaxNameSize = 30,
    };

    constexpr TableEntry kTable[] = {
        {"arkham city", 9}, {"arkham knight", 7}, {"bloodborne", 10}, {"god of war", 10},       {"horizon", 9},
        {"infamous", 7},    {"last guardian", 7}, {"last of us", 11}, {"last of us part 2", 8}, {"mass effect", 8},
        {"sekiro", 10},     {"tomb raider", 9},   {"uncharted", 9},
    };

    constexpr Entry kUnsortedTable[]       = {Entry("z"), Entry("a"), Entry("b")};
    constexpr Entry kDuplicateEntryTable[] = {Entry("duplicate"), Entry("duplicate")};

    static_assert(ot::Utils::LookupTable::IsSorted(kTable), "LookupTable::IsSorted() failed");
    static_assert(!ot::Utils::LookupTable::IsSorted(kUnsortedTable), "failed for unsorted table");
    static_assert(!ot::Utils::LookupTable::IsSorted(kDuplicateEntryTable), "failed for table with duplicate entries");

    for (const TableEntry &tableEntry : kTable)
    {
        const TableEntry *entry;
        char              name[kMaxNameSize];

        strcpy(name, tableEntry.mName);

        entry = ot::Utils::LookupTable::Find(name, kTable);
        VerifyOrQuit(entry == &tableEntry, "LookupTable::Find() failed");

        name[strlen(name) - 1] = '\0';
        entry                  = ot::Utils::LookupTable::Find(name, kTable);
        VerifyOrQuit(entry == nullptr, "LookupTable::Find() failed with non-matching name");
    }

    VerifyOrQuit(ot::Utils::LookupTable::Find("dragon age", kTable) == nullptr, "failed with non-exiting match");
}

int main(void)
{
    TestLookupTable();
    printf("All tests passed\n");
    return 0;
}
