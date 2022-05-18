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

/**
 * @file
 *   This file provides an implementation of OpenThread random number generation manager class.
 */

#include "random_manager.hpp"

#include <openthread/platform/entropy.h>

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/logging.hpp"
#include "common/random.hpp"
#include "crypto/mbedtls.hpp"

namespace ot {

uint16_t                     RandomManager::sInitCount = 0;
RandomManager::NonCryptoPrng RandomManager::sPrng;

#if !OPENTHREAD_RADIO
RandomManager::Entropy       RandomManager::sEntropy;
RandomManager::CryptoCtrDrbg RandomManager::sCtrDrbg;
#endif

RandomManager::RandomManager(void)
{
    uint32_t seed;
    Error    error;

    OT_UNUSED_VARIABLE(error);

    OT_ASSERT(sInitCount < 0xffff);

    VerifyOrExit(sInitCount == 0);

#if !OPENTHREAD_RADIO
    sEntropy.Init();
    sCtrDrbg.Init();

    error = Random::Crypto::FillBuffer(reinterpret_cast<uint8_t *>(&seed), sizeof(seed));
    OT_ASSERT(error == kErrorNone);
#else
    error = otPlatEntropyGet(reinterpret_cast<uint8_t *>(&seed), sizeof(seed));
    OT_ASSERT(error == kErrorNone);
#endif

    sPrng.Init(seed);

exit:
    sInitCount++;
}

RandomManager::~RandomManager(void)
{
    OT_ASSERT(sInitCount > 0);

    sInitCount--;
    VerifyOrExit(sInitCount == 0);

#if !OPENTHREAD_RADIO
    sCtrDrbg.Deinit();
    sEntropy.Deinit();
#endif

exit:
    return;
}

uint32_t RandomManager::NonCryptoGetUint32(void)
{
    OT_ASSERT(sInitCount > 0);

    return sPrng.GetNext();
}

//-------------------------------------------------------------------
// NonCryptoPrng

void RandomManager::NonCryptoPrng::Init(uint32_t aSeed)
{
    // The PRNG has a cycle of length 1 for the below two initial
    // seeds. For all other seed values the cycle is ~2^31 long.

    if ((aSeed == 0) || (aSeed == 0x7fffffff))
    {
        aSeed = 0x1;
    }

    mState = aSeed;
}

uint32_t RandomManager::NonCryptoPrng::GetNext(void)
{
    uint32_t mlcg, p, q;
    uint64_t tmpstate;

    tmpstate = static_cast<uint64_t>(33614) * static_cast<uint64_t>(mState);
    q        = tmpstate & 0xffffffff;
    q        = q >> 1;
    p        = tmpstate >> 32;
    mlcg     = p + q;

    if (mlcg & 0x80000000)
    {
        mlcg &= 0x7fffffff;
        mlcg++;
    }

    mState = mlcg;

    return mlcg;
}

#if !OPENTHREAD_RADIO

//-------------------------------------------------------------------
// Entropy

void RandomManager::Entropy::Init(void)
{
    mbedtls_entropy_init(&mEntropyContext);

#ifndef OT_MBEDTLS_STRONG_DEFAULT_ENTROPY_PRESENT
    mbedtls_entropy_add_source(&mEntropyContext, &RandomManager::Entropy::HandleMbedtlsEntropyPoll, nullptr,
                               kEntropyMinThreshold, MBEDTLS_ENTROPY_SOURCE_STRONG);
#endif // OT_MBEDTLS_STRONG_DEFAULT_ENTROPY_PRESENT
}

void RandomManager::Entropy::Deinit(void)
{
    mbedtls_entropy_free(&mEntropyContext);
}

#ifndef OT_MBEDTLS_STRONG_DEFAULT_ENTROPY_PRESENT

int RandomManager::Entropy::HandleMbedtlsEntropyPoll(void *         aData,
                                                     unsigned char *aOutput,
                                                     size_t         aInLen,
                                                     size_t *       aOutLen)
{
    int rval = MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;

    SuccessOrExit(otPlatEntropyGet(reinterpret_cast<uint8_t *>(aOutput), static_cast<uint16_t>(aInLen)));
    rval = 0;

    VerifyOrExit(aOutLen != nullptr);
    *aOutLen = aInLen;

exit:
    OT_UNUSED_VARIABLE(aData);
    return rval;
}

#endif // OT_MBEDTLS_STRONG_DEFAULT_ENTROPY_PRESENT

//-------------------------------------------------------------------
// CryptoCtrDrbg

void RandomManager::CryptoCtrDrbg::Init(void)
{
    int rval;

    mbedtls_ctr_drbg_init(&mCtrDrbg);

    rval =
        mbedtls_ctr_drbg_seed(&mCtrDrbg, mbedtls_entropy_func, RandomManager::GetMbedTlsEntropyContext(), nullptr, 0);

    if (rval != 0)
    {
        otLogCritMbedTls("Failed to seed the CTR DRBG");
    }

    OT_ASSERT(rval == 0);
}

void RandomManager::CryptoCtrDrbg::Deinit(void)
{
    mbedtls_ctr_drbg_free(&mCtrDrbg);
}

Error RandomManager::CryptoCtrDrbg::FillBuffer(uint8_t *aBuffer, uint16_t aSize)
{
    return ot::Crypto::MbedTls::MapError(
        mbedtls_ctr_drbg_random(&mCtrDrbg, static_cast<unsigned char *>(aBuffer), static_cast<size_t>(aSize)));
}

#endif // #if !OPENTHREAD_RADIO

} // namespace ot
