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
 *   This file includes definitions for performing HMAC-based Extract-and-Expand Key Derivation Function (HKDF) using
 *   SHA-256.
 */

#ifndef HKDF_SHA256_HPP_
#define HKDF_SHA256_HPP_

#include "openthread-core-config.h"

#include "common/code_utils.hpp"
#include "crypto/context_size.hpp"
#include "crypto/hmac_sha256.hpp"
#include "openthread/platform/crypto.h"

namespace ot {
namespace Crypto {

/**
 * @addtogroup core-security
 *
 * @{
 *
 */

/**
 * This class implements HMAC-based Extract-and-Expand Key Derivation Function (HKDF) [RFC5869] using SHA-256.
 *
 */
class HkdfSha256
{
public:
    /**
     * Constructor to initialize the context.
     *
     */
    HkdfSha256(void);

    /**
     * Destructor to free the context.
     *
     */
    ~HkdfSha256(void);

    /**
     * This method performs the HKDF Extract step.
     *
     * In the Extract step getting an input key extracts from it a pseudo-random key.
     *
     * @param[in] aSalt             A pointer to buffer containing salt.
     * @param[in] aSaltLength       The salt length (in bytes).
     * @param[in] aInputKey         The input key.
     *
     */
    void Extract(const uint8_t *aSalt, uint16_t aSaltLength, const Key &aInputKey);

    /**
     * This method performs the HKDF Expand step.
     *
     * The method should be used after a previous `Extract` call, otherwise its behavior is undefined. In the Expand
     * stage an output key of a given length is derived from the pseudo-random key of Extract stage.
     *
     * @param[in]  aInfo             A pointer to buffer containing info sequence.
     * @param[in]  aInfoLength       The info length (in bytes).
     * @param[out] aOutputKey        Buffer to place the output key (must contain at least @p aOutputKeyLength bytes).
     * @param[in]  aOutputKeyLength  The output key length.
     *
     */
    void Expand(const uint8_t *aInfo, uint16_t aInfoLength, uint8_t *aOutputKey, uint16_t aOutputKeyLength);

private:
    otCryptoContext mContext;
    OT_DEFINE_ALIGNED_VAR(mContextStorage, kHkdfContextSize, uint64_t);
};

/**
 * @}
 *
 */

} // namespace Crypto
} // namespace ot

#endif // HKDF_SHA256_HPP_
