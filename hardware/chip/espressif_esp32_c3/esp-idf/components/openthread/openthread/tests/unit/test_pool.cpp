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

#include "test_platform.h"

#include <openthread/config.h>

#include "common/instance.hpp"
#include "common/pool.hpp"

#include "test_util.h"

struct EntryBase
{
    EntryBase *mNext;
};

struct Entry : public EntryBase, ot::LinkedListEntry<Entry>
{
public:
    Entry(void)
        : mInitWithInstance(false)
    {
    }

    void Init(ot::Instance &) { mInitWithInstance = true; }

    bool IsInitializedWithInstance(void) const { return mInitWithInstance; }

private:
    bool mInitWithInstance;
};

enum : uint16_t
{
    kPoolSize = 11,
};

typedef ot::Pool<Entry, kPoolSize> EntryPool;

static Entry sNonPoolEntry;

void VerifyEntry(EntryPool &aPool, const Entry &aEntry, bool aInitWithInstance)
{
    uint16_t         index;
    const EntryPool &constPool = const_cast<const EntryPool &>(aPool);

    VerifyOrQuit(aPool.IsPoolEntry(aEntry));
    VerifyOrQuit(!aPool.IsPoolEntry(sNonPoolEntry), "Pool::IsPoolEntry() succeeded for non-pool entry");

    index = aPool.GetIndexOf(aEntry);
    VerifyOrQuit(&aPool.GetEntryAt(index) == &aEntry);
    VerifyOrQuit(&constPool.GetEntryAt(index) == &aEntry);

    VerifyOrQuit(aEntry.IsInitializedWithInstance() == aInitWithInstance, "Pool did not correctly Init() entry");
}

void TestPool(EntryPool &aPool, bool aInitWithInstance)
{
    Entry *entries[kPoolSize];

    VerifyOrQuit(aPool.GetSize() == kPoolSize);

    for (Entry *&entry : entries)
    {
        entry = aPool.Allocate();
        VerifyOrQuit(entry != nullptr, "Pool::Allocate() failed");

        VerifyEntry(aPool, *entry, aInitWithInstance);
    }

    for (uint16_t numEntriesToFree = 1; numEntriesToFree <= kPoolSize; numEntriesToFree++)
    {
        VerifyOrQuit(aPool.Allocate() == nullptr, "Pool::Allocate() did not fail when all pool entries were allocated");

        for (uint16_t i = 0; i < numEntriesToFree; i++)
        {
            VerifyEntry(aPool, *entries[i], aInitWithInstance);
            aPool.Free(*entries[i]);
        }

        for (uint16_t i = 0; i < numEntriesToFree; i++)
        {
            entries[i] = aPool.Allocate();
            VerifyOrQuit(entries[i] != nullptr, "Pool::Allocate() failed");

            VerifyEntry(aPool, *entries[i], aInitWithInstance);
        }
    }

    VerifyOrQuit(aPool.Allocate() == nullptr, "Pool::Allocate() did not fail when all pool entries were allocated");
}

void TestPool(void)
{
    ot::Instance *instance = testInitInstance();
    EntryPool     pool1;
    EntryPool     pool2(*instance);

    TestPool(pool1, /* aInitWithInstance */ false);
    TestPool(pool2, /* aInitWithInstance */ true);

    testFreeInstance(instance);
}

int main(void)
{
    TestPool();
    printf("All tests passed\n");
    return 0;
}
