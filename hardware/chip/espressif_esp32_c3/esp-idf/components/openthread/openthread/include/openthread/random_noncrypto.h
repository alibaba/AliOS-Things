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
 * @brief
 *  This file defines the OpenThread non cryptographic random number generator API.
 */

#ifndef OPENTHREAD_RANDOM_H_
#define OPENTHREAD_RANDOM_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-random-non-crypto
 *
 * @brief
 *   This module includes functions that generates non cryptographic random numbers.
 *
 * @{
 *
 */

/**
 * This function generates and returns a random `uint32_t` value.
 *
 * @returns    A random `uint32_t` value.
 *
 */
uint32_t otRandomNonCryptoGetUint32(void);

/**
 * This function generates and returns a random byte.
 *
 * @returns A random `uint8_t` value.
 *
 */
uint8_t otRandomNonCryptoGetUint8(void);

/**
 * This function generates and returns a random `uint16_t` value.
 *
 * @returns A random `uint16_t` value.
 *
 */
uint16_t otRandomNonCryptoGetUint16(void);

/**
 * This function generates and returns a random `uint8_t` value within a given range `[aMin, aMax)`.
 *
 * @param[in]  aMin  A minimum value (this value can be included in returned random result).
 * @param[in]  aMax  A maximum value (this value is excluded from returned random result).
 *
 * @returns    A random `uint8_t` value in the given range (i.e., aMin <= random value < aMax).
 */
uint8_t otRandomNonCryptoGetUint8InRange(uint8_t aMin, uint8_t aMax);

/**
 * This function generates and returns a random `uint16_t` value within a given range `[aMin, aMax)`.
 *
 * @note The returned random value can include the @p aMin value but excludes the @p aMax.
 *
 * @param[in]  aMin  A minimum value (this value can be included in returned random result).
 * @param[in]  aMax  A maximum value (this value is excluded from returned random result).
 *
 * @returns    A random `uint16_t` value in the given range (i.e., aMin <= random value < aMax).
 */
uint16_t otRandomNonCryptoGetUint16InRange(uint16_t aMin, uint16_t aMax);

/**
 * This function generates and returns a random `uint32_t` value within a given range `[aMin, aMax)`.
 *
 * @note The returned random value can include the @p aMin value but excludes the @p aMax.
 *
 * @param[in]  aMin  A minimum value (this value can be included in returned random result).
 * @param[in]  aMax  A maximum value (this value is excluded from returned random result).
 *
 * @returns    A random `uint32_t` value in the given range (i.e., aMin <= random value < aMax).
 *
 */
uint32_t otRandomNonCryptoGetUint32InRange(uint32_t aMin, uint32_t aMax);

/**
 * This function fills a given buffer with random bytes.
 *
 * @param[out] aBuffer  A pointer to a buffer to fill with the random bytes.
 * @param[in]  aSize    Size of buffer (number of bytes to fill).
 *
 */
void otRandomNonCryptoFillBuffer(uint8_t *aBuffer, uint16_t aSize);

/**
 * This function adds a random jitter within a given range to a given value.
 *
 * @param[in]  aValue     A value to which the random jitter is added.
 * @param[in]  aJitter    Maximum jitter. Random jitter is selected from the range `[-aJitter, aJitter]`.
 *
 * @returns    The given value with an added random jitter.
 *
 */
uint32_t otRandomNonCryptoAddJitter(uint32_t aValue, uint16_t aJitter);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_RANDOM_H_
