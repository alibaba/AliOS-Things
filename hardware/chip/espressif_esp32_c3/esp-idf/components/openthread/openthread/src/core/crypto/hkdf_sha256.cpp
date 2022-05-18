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
 *   This file implements HMAC-based Extract-and-Expand Key Derivation Function (HKDF) using SHA-256.
 */

#include "hkdf_sha256.hpp"

#include <string.h>

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/error.hpp"
#include "openthread/platform/crypto.h"

namespace ot {
namespace Crypto {

HkdfSha256::HkdfSha256(void)
{
    Error err = kErrorNone;

    mContext.mContext     = mContextStorage;
    mContext.mContextSize = sizeof(mContextStorage);
    err                   = otPlatCryptoHkdfInit(&mContext);

    OT_ASSERT(err == kErrorNone);
    OT_UNUSED_VARIABLE(err);
}

HkdfSha256::~HkdfSha256(void)
{
    Error err = otPlatCryptoHkdfDeinit(&mContext);

    OT_ASSERT(err == kErrorNone);
    OT_UNUSED_VARIABLE(err);
}

void HkdfSha256::Extract(const uint8_t *aSalt, uint16_t aSaltLength, const Key &aInputKey)
{
    Error err = otPlatCryptoHkdfExtract(&mContext, aSalt, aSaltLength, &aInputKey);
    OT_ASSERT(err == kErrorNone);
    OT_UNUSED_VARIABLE(err);
}

void HkdfSha256::Expand(const uint8_t *aInfo, uint16_t aInfoLength, uint8_t *aOutputKey, uint16_t aOutputKeyLength)
{
    Error err = otPlatCryptoHkdfExpand(&mContext, aInfo, aInfoLength, aOutputKey, aOutputKeyLength);
    OT_ASSERT(err == kErrorNone);
    OT_UNUSED_VARIABLE(err);
}

} // namespace Crypto
} // namespace ot
