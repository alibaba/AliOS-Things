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
 *   This file includes definitions for using mbedTLS.
 */

#ifndef OT_MBEDTLS_HPP_
#define OT_MBEDTLS_HPP_

#include "openthread-core-config.h"

#include <mbedtls/version.h>

#include <openthread/instance.h>

#include "common/error.hpp"
#include "common/non_copyable.hpp"

/**
 * Keep forward-compatibility with Mbed TLS 3.0.
 *
 * Direct access to fields of structures declared in public headers is no longer
 * supported. In Mbed TLS 3, the layout of structures is not considered part of
 * the stable API, and minor versions (3.1, 3.2, etc.) may add, remove, rename,
 * reorder or change the type of structure fields.
 */
#if (MBEDTLS_VERSION_NUMBER < 0x03000000)
#ifndef MBEDTLS_PRIVATE
#define MBEDTLS_PRIVATE(member) member
#endif
#endif

namespace ot {
namespace Crypto {

/**
 * @addtogroup core-security
 *
 * @{
 *
 */

/**
 * This class implements mbedTLS memory.
 *
 */
class MbedTls : private NonCopyable
{
public:
    /**
     * This constructor initializes the object.
     *
     */
    MbedTls(void);

    /**
     * This method converts an mbed TLS error to OpenThread error.
     *
     * @param[in] aMbedTlsError  The mbed TLS error.
     *
     * @returns The mapped Error.
     *
     */
    static Error MapError(int aMbedTlsError);
};

/**
 * @}
 *
 */

} // namespace Crypto
} // namespace ot

#endif // OT_MBEDTLS_HPP_
