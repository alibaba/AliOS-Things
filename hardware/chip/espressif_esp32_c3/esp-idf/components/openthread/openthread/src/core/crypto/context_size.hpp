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

#ifndef CRYPTO_CONTEXT_HPP_
#define CRYPTO_CONTEXT_HPP_

#include "openthread-core-config.h"
#include "openthread/crypto.h"

#if OPENTHREAD_CONFIG_CRYPTO_LIB == OPENTHREAD_CONFIG_CRYPTO_LIB_MBEDTLS
#include <mbedtls/aes.h>
#include <mbedtls/md.h>
#include <mbedtls/sha256.h>
#elif OPENTHREAD_CONFIG_CRYPTO_LIB == OPENTHREAD_CONFIG_CRYPTO_LIB_PSA
#include <psa/crypto.h>
#endif // OPENTHREAD_CONFIG_CRYPTO_LIB == OPENTHREAD_CONFIG_CRYPTO_LIB_MBEDTLS

namespace ot {
namespace Crypto {

#if OPENTHREAD_CONFIG_CRYPTO_LIB == OPENTHREAD_CONFIG_CRYPTO_LIB_MBEDTLS

constexpr uint16_t kAesContextSize        = sizeof(mbedtls_aes_context);
constexpr uint16_t kHmacSha256ContextSize = sizeof(mbedtls_md_context_t);
constexpr uint16_t kHkdfContextSize       = sizeof(otCryptoSha256Hash);
constexpr uint16_t kSha256ContextSize     = sizeof(mbedtls_sha256_context);

#elif OPENTHREAD_CONFIG_CRYPTO_LIB == OPENTHREAD_CONFIG_CRYPTO_LIB_PSA

constexpr uint16_t kAesContextSize        = sizeof(psa_key_id_t);
constexpr uint16_t kHmacSha256ContextSize = sizeof(psa_mac_operation_t);
constexpr uint16_t kHkdfContextSize       = sizeof(psa_key_derivation_operation_t);
constexpr uint16_t kSha256ContextSize     = sizeof(psa_hash_operation_t);

#else

constexpr uint16_t kAesContextSize        = OPENTHREAD_CONFIG_AES_CONTEXT_SIZE;
constexpr uint16_t kHmacSha256ContextSize = OPENTHREAD_CONFIG_HMAC_SHA256_CONTEXT_SIZE;
constexpr uint16_t kHkdfContextSize       = OPENTHREAD_CONFIG_HKDF_CONTEXT_SIZE;
constexpr uint16_t kSha256ContextSize     = OPENTHREAD_CONFIG_SHA256_CONTEXT_SIZE;

#endif // OPENTHREAD_CONFIG_CRYPTO_LIB == OPENTHREAD_CONFIG_CRYPTO_LIB_MBEDTLS

} // namespace Crypto
} // namespace ot

#endif // CRYPTO_CONTEXT_HPP_
