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

#ifndef CONFIG_CRYPTO_H_
#define CONFIG_CRYPTO_H_

/**
 * @def OPENTHREAD_CONFIG_CRYPTO_LIB
 *
 * Selects the crypto backend library for OpenThread.
 *
 * There are several options available
 * - @sa OPENTHREAD_CONFIG_CRYPTO_LIB_MBEDTLS
 * - @sa OPENTHREAD_CONFIG_CRYPTO_LIB_PSA
 * - @sa OPENTHREAD_CONFIG_CRYPTO_LIB_PLATFORM
 *
 */
#ifndef OPENTHREAD_CONFIG_CRYPTO_LIB
#define OPENTHREAD_CONFIG_CRYPTO_LIB OPENTHREAD_CONFIG_CRYPTO_LIB_MBEDTLS
#endif

/** Use mbedtls as crypto library */
#define OPENTHREAD_CONFIG_CRYPTO_LIB_MBEDTLS 0
/** Use ARM Platform Security Library as crypto library */
#define OPENTHREAD_CONFIG_CRYPTO_LIB_PSA 1
/** Use platform provided crypto library */
#define OPENTHREAD_CONFIG_CRYPTO_LIB_PLATFORM 2

#if OPENTHREAD_CONFIG_CRYPTO_LIB == OPENTHREAD_CONFIG_CRYPTO_LIB_PLATFORM

/**
 * @def OPENTHREAD_CONFIG_AES_CONTEXT_SIZE
 *
 * The size of the AES context byte array. Only applicable with OPENTHREAD_CONFIG_CRYPTO_LIB_PLATFORM.
 *
 */
#ifndef OPENTHREAD_CONFIG_AES_CONTEXT_SIZE
#error "OPENTHREAD_CONFIG_AES_CONTEXT_SIZE is missing"
#endif

/**
 * @def OPENTHREAD_CONFIG_HMAC_SHA256_CONTEXT_SIZE
 *
 * The size of the HMAC_SHA256 context byte array. Only applicable with OPENTHREAD_CONFIG_CRYPTO_LIB_PLATFORM.
 *
 */
#ifndef OPENTHREAD_CONFIG_HMAC_SHA256_CONTEXT_SIZE
#error "OPENTHREAD_CONFIG_HMAC_SHA256_CONTEXT_SIZE is missing"
#endif

/**
 * @def OPENTHREAD_CONFIG_HKDF_CONTEXT_SIZE
 *
 * The size of the HKDF context byte array. Only applicable with OPENTHREAD_CONFIG_CRYPTO_LIB_PLATFORM.
 *
 */
#ifndef OPENTHREAD_CONFIG_HKDF_CONTEXT_SIZE
#error "OPENTHREAD_CONFIG_HKDF_CONTEXT_SIZE is missing"
#endif

/**
 * @def OPENTHREAD_CONFIG_SHA256_CONTEXT_SIZE
 *
 * The size of the SHA256 context byte array. Only applicable with OPENTHREAD_CONFIG_CRYPTO_LIB_PLATFORM.
 *
 */
#ifndef OPENTHREAD_CONFIG_SHA256_CONTEXT_SIZE
#error "OPENTHREAD_CONFIG_SHA256_CONTEXT_SIZE is missing"
#endif

#endif // OPENTHREAD_CONFIG_CRYPTO_LIB == OPENTHREAD_CONFIG_CRYPTO_LIB_PLATFORM

#endif // CONFIG_CRYPTO_H_
