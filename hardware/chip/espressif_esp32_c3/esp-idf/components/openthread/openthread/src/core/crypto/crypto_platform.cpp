/*
 *    Copyright (c) 2021, The OpenThread Authors.
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
 *   This file implements the Crypto platform callbacks into OpenThread and default/weak Crypto platform APIs.
 */

#include "openthread-core-config.h"

#include <mbedtls/aes.h>
#include <mbedtls/md.h>
#include <mbedtls/sha256.h>

#include <openthread/instance.h>
#include <openthread/platform/crypto.h>
#include <openthread/platform/time.h>

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/new.hpp"
#include "config/crypto.h"
#include "crypto/hmac_sha256.hpp"
#include "crypto/storage.hpp"

using namespace ot;
using namespace Crypto;

//---------------------------------------------------------------------------------------------------------------------
// Default/weak implementation of crypto platform APIs

OT_TOOL_WEAK otError otPlatCryptoInit(void)
{
    return kErrorNone;
}

// AES  Implementation
OT_TOOL_WEAK otError otPlatCryptoAesInit(otCryptoContext *aContext)
{
    Error                error = kErrorNone;
    mbedtls_aes_context *context;

    VerifyOrExit(aContext != nullptr, error = kErrorInvalidArgs);
    VerifyOrExit(aContext->mContextSize >= sizeof(mbedtls_aes_context), error = kErrorFailed);

    context = static_cast<mbedtls_aes_context *>(aContext->mContext);
    mbedtls_aes_init(context);

exit:
    return error;
}

OT_TOOL_WEAK otError otPlatCryptoAesSetKey(otCryptoContext *aContext, const otCryptoKey *aKey)
{
    Error                error = kErrorNone;
    mbedtls_aes_context *context;
    const LiteralKey     key(*static_cast<const Key *>(aKey));

    VerifyOrExit(aContext != nullptr, error = kErrorInvalidArgs);
    VerifyOrExit(aContext->mContextSize >= sizeof(mbedtls_aes_context), error = kErrorFailed);

    context = static_cast<mbedtls_aes_context *>(aContext->mContext);
    VerifyOrExit((mbedtls_aes_setkey_enc(context, key.GetBytes(), (key.GetLength() * CHAR_BIT)) == 0),
                 error = kErrorFailed);

exit:
    return error;
}

OT_TOOL_WEAK otError otPlatCryptoAesEncrypt(otCryptoContext *aContext, const uint8_t *aInput, uint8_t *aOutput)
{
    Error                error = kErrorNone;
    mbedtls_aes_context *context;

    VerifyOrExit(aContext != nullptr, error = kErrorInvalidArgs);
    VerifyOrExit(aContext->mContextSize >= sizeof(mbedtls_aes_context), error = kErrorFailed);

    context = static_cast<mbedtls_aes_context *>(aContext->mContext);
    VerifyOrExit((mbedtls_aes_crypt_ecb(context, MBEDTLS_AES_ENCRYPT, aInput, aOutput) == 0), error = kErrorFailed);

exit:
    return error;
}

OT_TOOL_WEAK otError otPlatCryptoAesFree(otCryptoContext *aContext)
{
    Error                error = kErrorNone;
    mbedtls_aes_context *context;

    VerifyOrExit(aContext != nullptr, error = kErrorInvalidArgs);
    VerifyOrExit(aContext->mContextSize >= sizeof(mbedtls_aes_context), error = kErrorFailed);

    context = static_cast<mbedtls_aes_context *>(aContext->mContext);
    mbedtls_aes_free(context);

exit:
    return error;
}

#if !OPENTHREAD_RADIO

// HMAC implementations
OT_TOOL_WEAK otError otPlatCryptoHmacSha256Init(otCryptoContext *aContext)
{
    Error                    error  = kErrorNone;
    const mbedtls_md_info_t *mdInfo = nullptr;
    mbedtls_md_context_t *   context;

    VerifyOrExit(aContext != nullptr, error = kErrorInvalidArgs);
    VerifyOrExit(aContext->mContextSize >= sizeof(mbedtls_md_context_t), error = kErrorFailed);

    context = static_cast<mbedtls_md_context_t *>(aContext->mContext);
    mbedtls_md_init(context);
    mdInfo = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    VerifyOrExit((mbedtls_md_setup(context, mdInfo, 1) == 0), error = kErrorFailed);

exit:
    return error;
}

OT_TOOL_WEAK otError otPlatCryptoHmacSha256Deinit(otCryptoContext *aContext)
{
    Error                 error = kErrorNone;
    mbedtls_md_context_t *context;

    VerifyOrExit(aContext != nullptr, error = kErrorInvalidArgs);
    VerifyOrExit(aContext->mContextSize >= sizeof(mbedtls_md_context_t), error = kErrorFailed);

    context = static_cast<mbedtls_md_context_t *>(aContext->mContext);
    mbedtls_md_free(context);

exit:
    return error;
}

OT_TOOL_WEAK otError otPlatCryptoHmacSha256Start(otCryptoContext *aContext, const otCryptoKey *aKey)
{
    Error                 error = kErrorNone;
    const LiteralKey      key(*static_cast<const Key *>(aKey));
    mbedtls_md_context_t *context;

    VerifyOrExit(aContext != nullptr, error = kErrorInvalidArgs);
    VerifyOrExit(aContext->mContextSize >= sizeof(mbedtls_md_context_t), error = kErrorFailed);

    context = static_cast<mbedtls_md_context_t *>(aContext->mContext);
    VerifyOrExit((mbedtls_md_hmac_starts(context, key.GetBytes(), key.GetLength()) == 0), error = kErrorFailed);

exit:
    return error;
}

OT_TOOL_WEAK otError otPlatCryptoHmacSha256Update(otCryptoContext *aContext, const void *aBuf, uint16_t aBufLength)
{
    Error                 error = kErrorNone;
    mbedtls_md_context_t *context;

    VerifyOrExit(aContext != nullptr, error = kErrorInvalidArgs);
    VerifyOrExit(aContext->mContextSize >= sizeof(mbedtls_md_context_t), error = kErrorFailed);

    context = static_cast<mbedtls_md_context_t *>(aContext->mContext);
    VerifyOrExit((mbedtls_md_hmac_update(context, reinterpret_cast<const uint8_t *>(aBuf), aBufLength) == 0),
                 error = kErrorFailed);

exit:
    return error;
}

OT_TOOL_WEAK otError otPlatCryptoHmacSha256Finish(otCryptoContext *aContext, uint8_t *aBuf, size_t aBufLength)
{
    OT_UNUSED_VARIABLE(aBufLength);

    Error                 error = kErrorNone;
    mbedtls_md_context_t *context;

    VerifyOrExit(aContext != nullptr, error = kErrorInvalidArgs);
    VerifyOrExit(aContext->mContextSize >= sizeof(mbedtls_md_context_t), error = kErrorFailed);

    context = static_cast<mbedtls_md_context_t *>(aContext->mContext);
    VerifyOrExit((mbedtls_md_hmac_finish(context, aBuf) == 0), error = kErrorFailed);

exit:
    return error;
}

otError otPlatCryptoHkdfInit(otCryptoContext *aContext)
{
    Error error = kErrorNone;

    VerifyOrExit(aContext != nullptr, error = kErrorInvalidArgs);
    VerifyOrExit(aContext->mContextSize >= sizeof(HmacSha256::Hash), error = kErrorFailed);

    new (aContext->mContext) HmacSha256::Hash();

exit:
    return error;
}

OT_TOOL_WEAK otError otPlatCryptoHkdfExpand(otCryptoContext *aContext,
                                            const uint8_t *  aInfo,
                                            uint16_t         aInfoLength,
                                            uint8_t *        aOutputKey,
                                            uint16_t         aOutputKeyLength)
{
    Error             error = kErrorNone;
    HmacSha256        hmac;
    HmacSha256::Hash  hash;
    uint8_t           iter = 0;
    uint16_t          copyLength;
    HmacSha256::Hash *prk;

    VerifyOrExit(aContext != nullptr, error = kErrorInvalidArgs);
    VerifyOrExit(aContext->mContextSize >= sizeof(HmacSha256::Hash), error = kErrorFailed);

    prk = static_cast<HmacSha256::Hash *>(aContext->mContext);

    // The aOutputKey is calculated as follows [RFC5889]:
    //
    //   N = ceil( aOutputKeyLength / HashSize)
    //   T = T(1) | T(2) | T(3) | ... | T(N)
    //   aOutputKey is first aOutputKeyLength of T
    //
    // Where:
    //   T(0) = empty string (zero length)
    //   T(1) = HMAC-Hash(PRK, T(0) | info | 0x01)
    //   T(2) = HMAC-Hash(PRK, T(1) | info | 0x02)
    //   T(3) = HMAC-Hash(PRK, T(2) | info | 0x03)
    //   ...

    while (aOutputKeyLength > 0)
    {
        Key cryptoKey;

        cryptoKey.Set(prk->GetBytes(), sizeof(HmacSha256::Hash));
        hmac.Start(cryptoKey);

        if (iter != 0)
        {
            hmac.Update(hash);
        }

        hmac.Update(aInfo, aInfoLength);

        iter++;
        hmac.Update(iter);
        hmac.Finish(hash);

        copyLength = (aOutputKeyLength > sizeof(hash)) ? sizeof(hash) : aOutputKeyLength;

        memcpy(aOutputKey, hash.GetBytes(), copyLength);
        aOutputKey += copyLength;
        aOutputKeyLength -= copyLength;
    }

exit:
    return error;
}

OT_TOOL_WEAK otError otPlatCryptoHkdfExtract(otCryptoContext *  aContext,
                                             const uint8_t *    aSalt,
                                             uint16_t           aSaltLength,
                                             const otCryptoKey *aInputKey)
{
    Error             error = kErrorNone;
    HmacSha256        hmac;
    Key               cryptoKey;
    HmacSha256::Hash *prk;
    const LiteralKey  inputKey(*static_cast<const Key *>(aInputKey));

    VerifyOrExit(aContext != nullptr, error = kErrorInvalidArgs);
    VerifyOrExit(aContext->mContextSize >= sizeof(HmacSha256::Hash), error = kErrorFailed);

    prk = static_cast<HmacSha256::Hash *>(aContext->mContext);

    cryptoKey.Set(aSalt, aSaltLength);
    // PRK is calculated as HMAC-Hash(aSalt, aInputKey)
    hmac.Start(cryptoKey);
    hmac.Update(inputKey.GetBytes(), inputKey.GetLength());
    hmac.Finish(*prk);

exit:
    return error;
}

otError otPlatCryptoHkdfDeinit(otCryptoContext *aContext)
{
    Error             error = kErrorNone;
    HmacSha256::Hash *prk;

    VerifyOrExit(aContext != nullptr, error = kErrorInvalidArgs);
    VerifyOrExit(aContext->mContextSize >= sizeof(HmacSha256::Hash), error = kErrorFailed);

    prk = static_cast<HmacSha256::Hash *>(aContext->mContext);
    prk->~Hash();
    aContext->mContext     = nullptr;
    aContext->mContextSize = 0;

exit:
    return error;
}

// SHA256 platform implementations
OT_TOOL_WEAK otError otPlatCryptoSha256Init(otCryptoContext *aContext)
{
    Error                   error = kErrorNone;
    mbedtls_sha256_context *context;

    VerifyOrExit(aContext != nullptr, error = kErrorInvalidArgs);

    context = static_cast<mbedtls_sha256_context *>(aContext->mContext);
    mbedtls_sha256_init(context);

exit:
    return error;
}

OT_TOOL_WEAK otError otPlatCryptoSha256Deinit(otCryptoContext *aContext)
{
    Error                   error = kErrorNone;
    mbedtls_sha256_context *context;

    VerifyOrExit(aContext != nullptr, error = kErrorInvalidArgs);
    VerifyOrExit(aContext->mContextSize >= sizeof(mbedtls_sha256_context), error = kErrorFailed);

    context = static_cast<mbedtls_sha256_context *>(aContext->mContext);
    mbedtls_sha256_free(context);
    aContext->mContext     = nullptr;
    aContext->mContextSize = 0;

exit:
    return error;
}

OT_TOOL_WEAK otError otPlatCryptoSha256Start(otCryptoContext *aContext)
{
    Error                   error = kErrorNone;
    mbedtls_sha256_context *context;

    VerifyOrExit(aContext != nullptr, error = kErrorInvalidArgs);
    VerifyOrExit(aContext->mContextSize >= sizeof(mbedtls_sha256_context), error = kErrorFailed);

    context = static_cast<mbedtls_sha256_context *>(aContext->mContext);

#if (MBEDTLS_VERSION_NUMBER >= 0x03000000)
    VerifyOrExit((mbedtls_sha256_starts(context, 0) == 0), error = kErrorFailed);
#else
    VerifyOrExit((mbedtls_sha256_starts_ret(context, 0) == 0), error = kErrorFailed);
#endif

exit:
    return error;
}

OT_TOOL_WEAK otError otPlatCryptoSha256Update(otCryptoContext *aContext, const void *aBuf, uint16_t aBufLength)
{
    Error                   error = kErrorNone;
    mbedtls_sha256_context *context;

    VerifyOrExit(aContext != nullptr, error = kErrorInvalidArgs);
    VerifyOrExit(aContext->mContextSize >= sizeof(mbedtls_sha256_context), error = kErrorFailed);

    context = static_cast<mbedtls_sha256_context *>(aContext->mContext);

#if (MBEDTLS_VERSION_NUMBER >= 0x03000000)
    VerifyOrExit((mbedtls_sha256_update(context, reinterpret_cast<const uint8_t *>(aBuf), aBufLength) == 0),
                 error = kErrorFailed);
#else
    VerifyOrExit((mbedtls_sha256_update_ret(context, reinterpret_cast<const uint8_t *>(aBuf), aBufLength) == 0),
                 error = kErrorFailed);
#endif

exit:
    return error;
}

OT_TOOL_WEAK otError otPlatCryptoSha256Finish(otCryptoContext *aContext, uint8_t *aHash, uint16_t aHashSize)
{
    OT_UNUSED_VARIABLE(aHashSize);

    Error                   error = kErrorNone;
    mbedtls_sha256_context *context;

    VerifyOrExit(aContext != nullptr, error = kErrorInvalidArgs);
    VerifyOrExit(aContext->mContextSize >= sizeof(mbedtls_sha256_context), error = kErrorFailed);

    context = static_cast<mbedtls_sha256_context *>(aContext->mContext);

#if (MBEDTLS_VERSION_NUMBER >= 0x03000000)
    VerifyOrExit((mbedtls_sha256_finish(context, aHash) == 0), error = kErrorFailed);
#else
    VerifyOrExit((mbedtls_sha256_finish_ret(context, aHash) == 0), error = kErrorFailed);
#endif

exit:
    return error;
}

#endif // #if !OPENTHREAD_RADIO
