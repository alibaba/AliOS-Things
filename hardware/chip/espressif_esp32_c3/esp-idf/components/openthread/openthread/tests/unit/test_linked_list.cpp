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

#include <stdarg.h>
#include <string.h>

#include "test_platform.h"

#include <openthread/config.h>

#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/linked_list.hpp"

#include "test_util.h"

struct EntryBase
{
    EntryBase *mNext;
};

struct Entry : public EntryBase, ot::LinkedListEntry<Entry>
{
public:
    Entry(const char *aName, uint16_t aId)
        : mName(aName)
        , mId(aId)
    {
    }

    const char *GetName(void) const { return mName; }
    uint16_t    GetId(void) const { return mId; }
    bool        Matches(const char *aName) const { return strcmp(mName, aName) == 0; }
    bool        Matches(uint16_t aId) const { return mId == aId; }

private:
    const char *mName;
    uint16_t    mId;
};

// This function verifies the content of the linked list matches a given list of entries.
void VerifyLinkedListContent(const ot::LinkedList<Entry> *aList, ...)
{
    va_list      args;
    Entry *      argEntry;
    Entry *      argPrev = nullptr;
    const Entry *prev;
    uint16_t     unusedId = 100;

    va_start(args, aList);

    for (const Entry &entry : *aList)
    {
        argEntry = va_arg(args, Entry *);
        VerifyOrQuit(argEntry != nullptr, "List contains more entries than expected");
        VerifyOrQuit(argEntry == &entry, "List does not contain the same entry");
        VerifyOrQuit(aList->Contains(*argEntry));
        VerifyOrQuit(aList->ContainsMatching(argEntry->GetName()));
        VerifyOrQuit(aList->ContainsMatching(argEntry->GetId()));

        SuccessOrQuit(aList->Find(*argEntry, prev));
        VerifyOrQuit(prev == argPrev, "List::Find() returned prev entry is incorrect");

        VerifyOrQuit(aList->FindMatching(argEntry->GetName(), prev) == argEntry);
        VerifyOrQuit(prev == argPrev, "List::FindMatching() returned prev entry is incorrect");

        VerifyOrQuit(aList->FindMatching(argEntry->GetId(), prev) == argEntry);
        VerifyOrQuit(prev == argPrev, "List::FindMatching() returned prev entry is incorrect");

        argPrev = argEntry;
    }

    argEntry = va_arg(args, Entry *);
    VerifyOrQuit(argEntry == nullptr, "List contains less entries than expected");

    VerifyOrQuit(aList->GetTail() == argPrev);

    VerifyOrQuit(!aList->ContainsMatching("none"), "succeeded for a missing entry");
    VerifyOrQuit(!aList->ContainsMatching(unusedId), "succeeded for a missing entry");

    VerifyOrQuit(aList->FindMatching("none", prev) == nullptr, "succeeded for a missing entry");
    VerifyOrQuit(aList->FindMatching(unusedId, prev) == nullptr, "succeeded for a missing entry");
}

void TestLinkedList(void)
{
    Entry                 a("a", 1), b("b", 2), c("c", 3), d("d", 4), e("e", 5);
    Entry *               prev;
    ot::LinkedList<Entry> list;

    VerifyOrQuit(list.IsEmpty(), "failed after init");
    VerifyOrQuit(list.GetHead() == nullptr, "failed after init");
    VerifyOrQuit(list.Pop() == nullptr, "failed when empty");
    VerifyOrQuit(list.Find(a, prev) == ot::kErrorNotFound, "succeeded when empty");

    VerifyLinkedListContent(&list, nullptr);

    list.Push(a);
    VerifyOrQuit(!list.IsEmpty());
    VerifyLinkedListContent(&list, &a, nullptr);
    VerifyOrQuit(list.Find(b, prev) == ot::kErrorNotFound, "succeeded for a missing entry");

    SuccessOrQuit(list.Add(b));
    VerifyLinkedListContent(&list, &b, &a, nullptr);
    VerifyOrQuit(list.Find(c, prev) == ot::kErrorNotFound, "succeeded for a missing entry");

    list.Push(c);
    VerifyLinkedListContent(&list, &c, &b, &a, nullptr);

    SuccessOrQuit(list.Add(d));
    VerifyLinkedListContent(&list, &d, &c, &b, &a, nullptr);

    SuccessOrQuit(list.Add(e));
    VerifyLinkedListContent(&list, &e, &d, &c, &b, &a, nullptr);

    VerifyOrQuit(list.Add(a) == ot::kErrorAlready, "did not detect duplicate");
    VerifyOrQuit(list.Add(b) == ot::kErrorAlready, "did not detect duplicate");
    VerifyOrQuit(list.Add(d) == ot::kErrorAlready, "did not detect duplicate");
    VerifyOrQuit(list.Add(e) == ot::kErrorAlready, "did not detect duplicate");

    VerifyOrQuit(list.Pop() == &e);
    VerifyLinkedListContent(&list, &d, &c, &b, &a, nullptr);
    VerifyOrQuit(list.Find(e, prev) == ot::kErrorNotFound, "succeeded for a missing entry");

    VerifyOrQuit(list.FindMatching(d.GetName(), prev) == &d);
    VerifyOrQuit(prev == nullptr);
    VerifyOrQuit(list.FindMatching(c.GetId(), prev) == &c);
    VerifyOrQuit(prev == &d);
    VerifyOrQuit(list.FindMatching(b.GetName(), prev) == &b);
    VerifyOrQuit(prev == &c);
    VerifyOrQuit(list.FindMatching(a.GetId(), prev) == &a);
    VerifyOrQuit(prev == &b);
    VerifyOrQuit(list.FindMatching(e.GetId(), prev) == nullptr, "succeeded for a missing entry");
    VerifyOrQuit(list.FindMatching(e.GetName(), prev) == nullptr, "succeeded for a missing entry");

    list.SetHead(&e);
    VerifyLinkedListContent(&list, &e, &d, &c, &b, &a, nullptr);

    SuccessOrQuit(list.Remove(c));
    VerifyLinkedListContent(&list, &e, &d, &b, &a, nullptr);

    VerifyOrQuit(list.Remove(c) == ot::kErrorNotFound);
    VerifyLinkedListContent(&list, &e, &d, &b, &a, nullptr);
    VerifyOrQuit(list.Find(c, prev) == ot::kErrorNotFound, "succeeded for a missing entry");

    SuccessOrQuit(list.Remove(e));
    VerifyLinkedListContent(&list, &d, &b, &a, nullptr);
    VerifyOrQuit(list.Find(e, prev) == ot::kErrorNotFound, "succeeded for a missing entry");

    SuccessOrQuit(list.Remove(a));
    VerifyLinkedListContent(&list, &d, &b, nullptr);
    VerifyOrQuit(list.Find(a, prev) == ot::kErrorNotFound, "succeeded for a missing entry");

    list.Push(a);
    list.Push(c);
    list.Push(e);
    VerifyLinkedListContent(&list, &e, &c, &a, &d, &b, nullptr);

    VerifyOrQuit(list.PopAfter(&a) == &d);
    VerifyLinkedListContent(&list, &e, &c, &a, &b, nullptr);

    VerifyOrQuit(list.PopAfter(&b) == nullptr);
    VerifyLinkedListContent(&list, &e, &c, &a, &b, nullptr);

    VerifyOrQuit(list.PopAfter(&e) == &c);
    VerifyLinkedListContent(&list, &e, &a, &b, nullptr);

    list.PushAfter(c, b);
    VerifyLinkedListContent(&list, &e, &a, &b, &c, nullptr);

    list.PushAfter(d, a);
    VerifyLinkedListContent(&list, &e, &a, &d, &b, &c, nullptr);

    VerifyOrQuit(list.PopAfter(nullptr) == &e);
    VerifyLinkedListContent(&list, &a, &d, &b, &c, nullptr);

    VerifyOrQuit(list.PopAfter(nullptr) == &a);
    VerifyLinkedListContent(&list, &d, &b, &c, nullptr);

    list.Push(e);
    list.Push(a);
    VerifyLinkedListContent(&list, &a, &e, &d, &b, &c, nullptr);

    VerifyOrQuit(list.RemoveMatching(a.GetName()) == &a);
    VerifyLinkedListContent(&list, &e, &d, &b, &c, nullptr);

    VerifyOrQuit(list.RemoveMatching(c.GetId()) == &c);
    VerifyLinkedListContent(&list, &e, &d, &b, nullptr);

    VerifyOrQuit(list.RemoveMatching(c.GetId()) == nullptr, "succeeded for missing entry");
    VerifyOrQuit(list.RemoveMatching(a.GetName()) == nullptr, "succeeded for missing entry");

    VerifyOrQuit(list.RemoveMatching(d.GetId()) == &d);
    VerifyLinkedListContent(&list, &e, &b, nullptr);

    list.Clear();
    VerifyOrQuit(list.IsEmpty(), "failed after Clear()");
    VerifyOrQuit(list.PopAfter(nullptr) == nullptr);
    VerifyLinkedListContent(&list, nullptr);
    VerifyOrQuit(list.Find(a, prev) == ot::kErrorNotFound, "succeeded for a missing entry");
    VerifyOrQuit(list.FindMatching(b.GetName(), prev) == nullptr, "succeeded when empty");
    VerifyOrQuit(list.FindMatching(c.GetId(), prev) == nullptr, "succeeded when empty");
    VerifyOrQuit(list.RemoveMatching(a.GetName()) == nullptr, "succeeded when empty");
    VerifyOrQuit(list.Remove(a) == ot::kErrorNotFound, "succeeded when empty");
}

int main(void)
{
    TestLinkedList();
    printf("All tests passed\n");
    return 0;
}
