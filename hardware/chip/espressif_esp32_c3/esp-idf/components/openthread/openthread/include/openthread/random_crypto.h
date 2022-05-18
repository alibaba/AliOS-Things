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
 *  This file defines the OpenThread cryptographic random number generator API.
 */

#ifndef OPENTHREAD_RANDOM_CRYPTO_H_
#define OPENTHREAD_RANDOM_CRYPTO_H_

#include <stdint.h>

#include <mbedtls/ctr_drbg.h>
#include <openthread/error.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-random-crypto
 *
 * @brief
 *   This module includes functions that generates cryptographic random numbers.
 *
 * @{
 *
 */

/**
 * This function fills a given buffer with cryptographically secure random bytes.
 *
 * @param[out] aBuffer  A pointer to a buffer to fill with the random bytes.
 * @param[in]  aSize    Size of buffer (number of bytes to fill).
 *
 */
otError otRandomCryptoFillBuffer(uint8_t *aBuffer, uint16_t aSize);

/**
 * This function returns initialized mbedtls_ctr_drbg_context.
 *
 * @returns  A pointer to initialized mbedtls_ctr_drbg_context.
 */
mbedtls_ctr_drbg_context *otRandomCryptoMbedTlsContextGet(void);

/**
 * @}
 *
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // OPENTHREAD_RANDOM_CRYPTO_H_
