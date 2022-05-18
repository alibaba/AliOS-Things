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
 *   This file implements the use of mbedTLS.
 */

#include "mbedtls.hpp"

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>
#include <mbedtls/entropy.h>
#include <mbedtls/platform.h>
#include <mbedtls/threading.h>

#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#include <mbedtls/pem.h>
#endif

#include "common/error.hpp"
#include "common/instance.hpp"

namespace ot {
namespace Crypto {

MbedTls::MbedTls(void)
{
#if OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS_MANAGEMENT
#ifdef MBEDTLS_DEBUG_C
    // mbedTLS's debug level is almost the same as OpenThread's
    mbedtls_debug_set_threshold(OPENTHREAD_CONFIG_LOG_LEVEL);
#endif
    mbedtls_platform_set_calloc_free(Instance::HeapCAlloc, Instance::HeapFree);
#endif // OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS_MANAGEMENT
}

Error MbedTls::MapError(int aMbedTlsError)
{
    Error error = kErrorNone;

    switch (aMbedTlsError)
    {
#if OPENTHREAD_CONFIG_ECDSA_ENABLE
    case MBEDTLS_ERR_ECP_BAD_INPUT_DATA:
    case MBEDTLS_ERR_MPI_BAD_INPUT_DATA:
    case MBEDTLS_ERR_MPI_INVALID_CHARACTER:
#endif
#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
    case MBEDTLS_ERR_PK_TYPE_MISMATCH:
    case MBEDTLS_ERR_PK_FILE_IO_ERROR:
    case MBEDTLS_ERR_PK_KEY_INVALID_VERSION:
    case MBEDTLS_ERR_PK_KEY_INVALID_FORMAT:
    case MBEDTLS_ERR_PK_UNKNOWN_PK_ALG:
    case MBEDTLS_ERR_PK_PASSWORD_REQUIRED:
    case MBEDTLS_ERR_PK_PASSWORD_MISMATCH:
    case MBEDTLS_ERR_PK_INVALID_PUBKEY:
    case MBEDTLS_ERR_PK_INVALID_ALG:
    case MBEDTLS_ERR_PK_UNKNOWN_NAMED_CURVE:
    case MBEDTLS_ERR_PK_BAD_INPUT_DATA:
    case MBEDTLS_ERR_X509_SIG_MISMATCH:
    case MBEDTLS_ERR_X509_BAD_INPUT_DATA:
    case MBEDTLS_ERR_X509_FILE_IO_ERROR:
    case MBEDTLS_ERR_X509_CERT_UNKNOWN_FORMAT:
    case MBEDTLS_ERR_X509_INVALID_VERSION:
    case MBEDTLS_ERR_X509_UNKNOWN_SIG_ALG:
    case MBEDTLS_ERR_X509_INVALID_SERIAL:
    case MBEDTLS_ERR_X509_UNKNOWN_OID:
    case MBEDTLS_ERR_X509_INVALID_FORMAT:
    case MBEDTLS_ERR_X509_INVALID_ALG:
    case MBEDTLS_ERR_X509_INVALID_NAME:
    case MBEDTLS_ERR_X509_INVALID_DATE:
    case MBEDTLS_ERR_X509_INVALID_SIGNATURE:
    case MBEDTLS_ERR_X509_INVALID_EXTENSIONS:
    case MBEDTLS_ERR_X509_UNKNOWN_VERSION:
#endif // MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
    case MBEDTLS_ERR_SSL_BAD_INPUT_DATA:
    case MBEDTLS_ERR_CTR_DRBG_REQUEST_TOO_BIG:
    case MBEDTLS_ERR_CTR_DRBG_INPUT_TOO_BIG:
        error = kErrorInvalidArgs;
        break;

#if OPENTHREAD_CONFIG_ECDSA_ENABLE
    case MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL:
    case MBEDTLS_ERR_MPI_BUFFER_TOO_SMALL:
    case MBEDTLS_ERR_MPI_ALLOC_FAILED:
#endif
#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
    case MBEDTLS_ERR_PEM_ALLOC_FAILED:
    case MBEDTLS_ERR_PK_ALLOC_FAILED:
    case MBEDTLS_ERR_X509_BUFFER_TOO_SMALL:
    case MBEDTLS_ERR_X509_ALLOC_FAILED:
#endif
    case MBEDTLS_ERR_SSL_ALLOC_FAILED:
    case MBEDTLS_ERR_SSL_WANT_WRITE:
    case MBEDTLS_ERR_ENTROPY_MAX_SOURCES:
        error = kErrorNoBufs;
        break;

#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
    case MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE:
    case MBEDTLS_ERR_PK_SIG_LEN_MISMATCH:
    case MBEDTLS_ERR_X509_FEATURE_UNAVAILABLE:
    case MBEDTLS_ERR_X509_CERT_VERIFY_FAILED:
#endif // MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
    case MBEDTLS_ERR_CTR_DRBG_ENTROPY_SOURCE_FAILED:
    case MBEDTLS_ERR_ENTROPY_SOURCE_FAILED:
    case MBEDTLS_ERR_ENTROPY_NO_SOURCES_DEFINED:
    case MBEDTLS_ERR_ENTROPY_NO_STRONG_SOURCE:
#if (MBEDTLS_VERSION_NUMBER < 0x03000000)
    case MBEDTLS_ERR_SSL_PEER_VERIFY_FAILED:
#endif
    case MBEDTLS_ERR_THREADING_BAD_INPUT_DATA:
    case MBEDTLS_ERR_THREADING_MUTEX_ERROR:
        error = kErrorSecurity;
        break;

#ifdef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
    case MBEDTLS_ERR_X509_FATAL_ERROR:
        error = kErrorFailed;
        break;
#endif
    case MBEDTLS_ERR_SSL_TIMEOUT:
    case MBEDTLS_ERR_SSL_WANT_READ:
        error = kErrorBusy;
        break;

#if OPENTHREAD_CONFIG_ECDSA_ENABLE
    case MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE:
        error = kErrorNotCapable;
        break;
#endif

    default:
        if (aMbedTlsError < 0)
        {
            error = kErrorFailed;
        }

        break;
    }

    return error;
}

} // namespace Crypto
} // namespace ot
