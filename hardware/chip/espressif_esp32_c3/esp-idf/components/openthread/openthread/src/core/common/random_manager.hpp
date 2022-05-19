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
 *  This file includes definitions for OpenThread random number generation manager class.
 */

#ifndef RANDOM_MANAGER_HPP_
#define RANDOM_MANAGER_HPP_

#include "openthread-core-config.h"

#include <stdint.h>

#if !OPENTHREAD_RADIO
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#endif

#include "common/error.hpp"
#include "common/non_copyable.hpp"

#if (!defined(MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES) && \
     (!defined(MBEDTLS_NO_PLATFORM_ENTROPY) || defined(MBEDTLS_HAVEGE_C) || defined(MBEDTLS_ENTROPY_HARDWARE_ALT)))
#define OT_MBEDTLS_STRONG_DEFAULT_ENTROPY_PRESENT
#endif

namespace ot {

/**
 * This class manages random number generator initialization/deinitialization.
 *
 */
class RandomManager : private NonCopyable
{
public:
    /**
     * This constructor initializes the object.
     *
     */
    RandomManager(void);

    /**
     * This destructor deinitializes the object.
     *
     */
    ~RandomManager(void);

    /**
     * This static method generates and returns a random value using a non-crypto Pseudo Random Number Generator.
     *
     * @returns    A random `uint32_t` value.
     *
     */
    static uint32_t NonCryptoGetUint32(void);

#if !OPENTHREAD_RADIO
    /**
     * This static method returns the initialized mbedtls_entropy_context.
     *
     * @returns  A pointer to initialized mbedtls_entropy_context.
     */
    static mbedtls_entropy_context *GetMbedTlsEntropyContext(void) { return sEntropy.GetContext(); }

    /**
     * This static method fills a given buffer with cryptographically secure random bytes.
     *
     * @param[out] aBuffer  A pointer to a buffer to fill with the random bytes.
     * @param[in]  aSize    Size of buffer (number of bytes to fill).
     *
     * @retval kErrorNone    Successfully filled buffer with random values.
     *
     */
    static Error CryptoFillBuffer(uint8_t *aBuffer, uint16_t aSize) { return sCtrDrbg.FillBuffer(aBuffer, aSize); }

    /**
     * This static method returns the initialized mbedtls_ctr_drbg_context.
     *
     * @returns  A pointer to the initialized mbedtls_ctr_drbg_context.
     *
     */
    static mbedtls_ctr_drbg_context *GetMbedTlsCtrDrbgContext(void) { return sCtrDrbg.GetContext(); }
#endif

private:
    class NonCryptoPrng // A non-crypto Pseudo Random Number Generator (PRNG)
    {
    public:
        void     Init(uint32_t aSeed);
        uint32_t GetNext(void);

    private:
        uint32_t mState;
    };

#if !OPENTHREAD_RADIO
    class Entropy
    {
    public:
        void Init(void);
        void Deinit(void);

        mbedtls_entropy_context *GetContext(void) { return &mEntropyContext; }

    private:
#ifndef OT_MBEDTLS_STRONG_DEFAULT_ENTROPY_PRESENT
        static int HandleMbedtlsEntropyPoll(void *aData, unsigned char *aOutput, size_t aInLen, size_t *aOutLen);
#endif // OT_MBEDTLS_STRONG_DEFAULT_ENTROPY_PRESENT

        static constexpr size_t kEntropyMinThreshold = 32;

        mbedtls_entropy_context mEntropyContext;
    };

    class CryptoCtrDrbg
    {
    public:
        void  Init(void);
        void  Deinit(void);
        Error FillBuffer(uint8_t *aBuffer, uint16_t aSize);

        mbedtls_ctr_drbg_context *GetContext(void) { return &mCtrDrbg; }

    private:
        mbedtls_ctr_drbg_context mCtrDrbg;
    };
#endif

    static uint16_t      sInitCount;
    static NonCryptoPrng sPrng;
#if !OPENTHREAD_RADIO
    static Entropy       sEntropy;
    static CryptoCtrDrbg sCtrDrbg;
#endif
};

} // namespace ot

#endif // RANDOM_MANAGER_HPP_
