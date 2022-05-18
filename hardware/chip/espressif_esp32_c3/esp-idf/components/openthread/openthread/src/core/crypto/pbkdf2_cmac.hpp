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
 * @brief
 *  This file includes definitions for performing Password-Based Key Derivation Function 2 (PBKDF2) using CMAC.
 */

#ifndef PBKDF2_CMAC_HPP_
#define PBKDF2_CMAC_HPP_

#include "openthread-core-config.h"

#include <stdint.h>

namespace ot {
namespace Crypto {
namespace Pbkdf2 {

/**
 * @addtogroup core-security
 *
 * @{
 *
 */

constexpr uint16_t kMaxSaltLength = 30; ///< Max SALT length: salt prefix (6) + extended panid (8) + network name (16)

/**
 * This function performs PKCS#5 PBKDF2 using CMAC (AES-CMAC-PRF-128).
 *
 * @param[in]     aPassword          Password to use when generating key.
 * @param[in]     aPasswordLen       Length of password.
 * @param[in]     aSalt              Salt to use when generating key.
 * @param[in]     aSaltLen           Length of salt.
 * @param[in]     aIterationCounter  Iteration count.
 * @param[in]     aKeyLen            Length of generated key in bytes.
 * @param[out]    aKey               A pointer to the generated key.
 *
 */
void GenerateKey(const uint8_t *aPassword,
                 uint16_t       aPasswordLen,
                 const uint8_t *aSalt,
                 uint16_t       aSaltLen,
                 uint32_t       aIterationCounter,
                 uint16_t       aKeyLen,
                 uint8_t *      aKey);

/**
 * @}
 *
 */

} // namespace Pbkdf2
} // namespace Crypto
} // namespace ot

#endif // PBKDF2_CMAC_HPP_
