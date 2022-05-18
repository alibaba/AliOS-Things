/*
 *  Copyright (c) 2016, The OpenThread Authors.
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
 *   This file implements PBKDF2 using AES-CMAC-PRF-128
 */

#include "pbkdf2_cmac.hpp"

#include <mbedtls/cmac.h>
#include <string.h>

#include "common/debug.hpp"

namespace ot {
namespace Crypto {
namespace Pbkdf2 {

#if OPENTHREAD_FTD

void GenerateKey(const uint8_t *aPassword,
                 uint16_t       aPasswordLen,
                 const uint8_t *aSalt,
                 uint16_t       aSaltLen,
                 uint32_t       aIterationCounter,
                 uint16_t       aKeyLen,
                 uint8_t *      aKey)
{
    const size_t kBlockSize = MBEDTLS_CIPHER_BLKSIZE_MAX;
    uint8_t      prfInput[kMaxSaltLength + 4]; // Salt || INT(), for U1 calculation
    long         prfOne[kBlockSize / sizeof(long)];
    long         prfTwo[kBlockSize / sizeof(long)];
    long         keyBlock[kBlockSize / sizeof(long)];
    uint32_t     blockCounter = 0;
    uint8_t *    key          = aKey;
    uint16_t     keyLen       = aKeyLen;
    uint16_t     useLen       = 0;

    memcpy(prfInput, aSalt, aSaltLen);
    OT_ASSERT(aIterationCounter % 2 == 0);
    aIterationCounter /= 2;

#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
    // limit iterations to avoid OSS-Fuzz timeouts
    aIterationCounter = 2;
#endif

    while (keyLen)
    {
        ++blockCounter;
        prfInput[aSaltLen + 0] = static_cast<uint8_t>(blockCounter >> 24);
        prfInput[aSaltLen + 1] = static_cast<uint8_t>(blockCounter >> 16);
        prfInput[aSaltLen + 2] = static_cast<uint8_t>(blockCounter >> 8);
        prfInput[aSaltLen + 3] = static_cast<uint8_t>(blockCounter);

        // Calculate U_1
        mbedtls_aes_cmac_prf_128(aPassword, aPasswordLen, prfInput, aSaltLen + 4,
                                 reinterpret_cast<uint8_t *>(keyBlock));

        // Calculate U_2
        mbedtls_aes_cmac_prf_128(aPassword, aPasswordLen, reinterpret_cast<const uint8_t *>(keyBlock), kBlockSize,
                                 reinterpret_cast<uint8_t *>(prfOne));

        for (uint32_t j = 0; j < kBlockSize / sizeof(long); ++j)
        {
            keyBlock[j] ^= prfOne[j];
        }

        for (uint32_t i = 1; i < aIterationCounter; ++i)
        {
            // Calculate U_{2 * i - 1}
            mbedtls_aes_cmac_prf_128(aPassword, aPasswordLen, reinterpret_cast<const uint8_t *>(prfOne), kBlockSize,
                                     reinterpret_cast<uint8_t *>(prfTwo));
            // Calculate U_{2 * i}
            mbedtls_aes_cmac_prf_128(aPassword, aPasswordLen, reinterpret_cast<const uint8_t *>(prfTwo), kBlockSize,
                                     reinterpret_cast<uint8_t *>(prfOne));

            for (uint32_t j = 0; j < kBlockSize / sizeof(long); ++j)
            {
                keyBlock[j] ^= prfOne[j] ^ prfTwo[j];
            }
        }

        useLen = (keyLen < kBlockSize) ? keyLen : kBlockSize;
        memcpy(key, keyBlock, useLen);
        key += useLen;
        keyLen -= useLen;
    }
}

#endif // OPENTHREAD_FTD

} // namespace Pbkdf2
} // namespace Crypto
} // namespace ot
