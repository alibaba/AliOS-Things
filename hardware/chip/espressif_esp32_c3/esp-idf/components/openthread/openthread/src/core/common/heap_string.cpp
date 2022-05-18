/*
 *  Copyright (c) 2021, The OpenThread Authors.
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
 *   This file implements the `HeapString` (a heap allocated string).
 */

#include "heap_string.hpp"

#include "common/code_utils.hpp"
#include "common/instance.hpp"
#include "common/string.hpp"

namespace ot {

Error HeapString::Set(const char *aCString)
{
    Error  error = kErrorNone;
    size_t curSize;
    size_t newSize;

    VerifyOrExit(aCString != nullptr, Free());

    curSize = (mStringBuffer != nullptr) ? strlen(mStringBuffer) + 1 : 0;
    newSize = strlen(aCString) + 1;

    if (curSize != newSize)
    {
        char *newBuffer = static_cast<char *>(Instance::HeapCAlloc(sizeof(char), newSize));

        VerifyOrExit(newBuffer != nullptr, error = kErrorNoBufs);

        Instance::HeapFree(mStringBuffer);
        mStringBuffer = newBuffer;
    }

    memcpy(mStringBuffer, aCString, newSize);

exit:
    return error;
}

Error HeapString::Set(HeapString &&aString)
{
    VerifyOrExit(mStringBuffer != aString.mStringBuffer);

    Instance::HeapFree(mStringBuffer);
    mStringBuffer         = aString.mStringBuffer;
    aString.mStringBuffer = nullptr;

exit:
    return kErrorNone;
}

void HeapString::Free(void)
{
    Instance::HeapFree(mStringBuffer);
    mStringBuffer = nullptr;
}

bool HeapString::operator==(const char *aCString) const
{
    bool isEqual;

    VerifyOrExit((aCString != nullptr) && (mStringBuffer != nullptr), isEqual = (mStringBuffer == aCString));
    isEqual = (strcmp(mStringBuffer, aCString) == 0);

exit:
    return isEqual;
}

} // namespace ot
