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
 *   This file includes definitions for performing HMAC SHA-256 computations.
 */

#ifndef HMAC_SHA256_HPP_
#define HMAC_SHA256_HPP_

#include "openthread-core-config.h"

#include <stdint.h>

#include <openthread/platform/crypto.h>

#include "common/code_utils.hpp"
#include "crypto/context_size.hpp"
#include "crypto/sha256.hpp"
#include "crypto/storage.hpp"

namespace ot {

class Message;

namespace Crypto {

/**
 * @addtogroup core-security
 *
 * @{
 *
 */

/**
 * This class implements HMAC SHA-256 computation.
 *
 */
class HmacSha256
{
public:
    /**
     * This type represents a HMAC SHA-256 hash.
     *
     */
    typedef Sha256::Hash Hash;

    /**
     * Constructor for `HmacSha256`.
     *
     */
    HmacSha256(void);

    /**
     * Destructor for `HmacSha256`.
     *
     */
    ~HmacSha256(void);

    /**
     * This method sets the key and starts the HMAC computation.
     *
     * @param[in]  aKey      The key to use.
     *
     */
    void Start(const Key &aKey);

    /**
     * This method inputs bytes into the HMAC computation.
     *
     * @param[in]  aBuf        A pointer to the input buffer.
     * @param[in]  aBufLength  The length of @p aBuf in bytes.
     *
     */
    void Update(const void *aBuf, uint16_t aBufLength);

    /**
     * This method inputs an object (treated as a sequence of bytes) into the HMAC computation.
     *
     * @tparam    ObjectType   The object type.
     *
     * @param[in] aObject      A reference to the object.
     *
     */
    template <typename ObjectType> void Update(const ObjectType &aObject)
    {
        static_assert(!TypeTraits::IsPointer<ObjectType>::kValue, "ObjectType must not be a pointer");
        return Update(&aObject, sizeof(ObjectType));
    }

    /**
     * This method inputs the bytes read from a given message into the HMAC computation.
     *
     * @param[in] aMessage    The message to read the data from.
     * @param[in] aOffset     The offset into @p aMessage to start to read.
     * @param[in] aLength     The number of bytes to read.
     *
     */
    void Update(const Message &aMessage, uint16_t aOffset, uint16_t aLength);

    /**
     * This method finalizes the hash computation.
     *
     * @param[out]  aHash  A reference to a `Hash` to output the calculated hash.
     *
     */
    void Finish(Hash &aHash);

private:
    otCryptoContext mContext;
    OT_DEFINE_ALIGNED_VAR(mContextStorage, kHmacSha256ContextSize, uint64_t);
};

/**
 * @}
 *
 */

} // namespace Crypto
} // namespace ot

#endif // HMAC_SHA256_HPP_
