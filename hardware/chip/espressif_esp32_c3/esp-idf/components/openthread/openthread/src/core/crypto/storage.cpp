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
 *   This file includes implementation for Crypto Internal Trusted Storage (ITS) APIs.
 */

#include "crypto/storage.hpp"

#include "common/debug.hpp"

namespace ot {
namespace Crypto {

#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
Error Key::ExtractKey(uint8_t *aKeyBuffer, uint16_t &aKeyLength) const
{
    Error  error;
    size_t readKeyLength;

    OT_ASSERT(IsKeyRef());

    error = Crypto::Storage::ExportKey(GetKeyRef(), aKeyBuffer, aKeyLength, readKeyLength);

    OT_ASSERT(error == kErrorNone);
    VerifyOrExit(readKeyLength <= aKeyLength, error = kErrorNoBufs);

    aKeyLength = static_cast<uint16_t>(readKeyLength);

exit:
    return error;
}
#endif

LiteralKey::LiteralKey(const Key &aKey)
    : mKey(aKey.GetBytes())
    , mLength(aKey.GetLength())
{
#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
    if (aKey.IsKeyRef())
    {
        Error error;

        mKey    = mBuffer;
        mLength = sizeof(mBuffer);
        error   = aKey.ExtractKey(mBuffer, mLength);

        OT_ASSERT(error == kErrorNone);
        OT_UNUSED_VARIABLE(error);
    }
#endif
}

} // namespace Crypto
} // namespace ot
