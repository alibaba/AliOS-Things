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

/**
 * @file
 *   This file includes definitions for IPv4 addresses.
 */

#ifndef IP4_ADDRESS_HPP_
#define IP4_ADDRESS_HPP_

#include "openthread-core-config.h"

#include "common/clearable.hpp"
#include "common/equatable.hpp"
#include "common/error.hpp"
#include "common/string.hpp"

namespace ot {
namespace Ip4 {

/**
 * This class represents an IPv4 address.
 *
 */
OT_TOOL_PACKED_BEGIN
class Address : public Equatable<Address>, public Clearable<Address>
{
public:
    static constexpr uint16_t kSize              = 4;  ///< Size of an IPv4 Address (in bytes).
    static constexpr uint16_t kAddressStringSize = 17; ///< String size used by `ToString()`.

    /**
     * This type defines the fixed-length `String` object returned from `ToString()`.
     *
     */
    typedef String<kAddressStringSize> InfoString;

    /**
     * This method gets the IPv4 address as a pointer to a byte array.
     *
     * @returns A pointer to a byte array containing the IPv4 address.
     *
     */
    const uint8_t *GetBytes(void) const { return mBytes; }

    /**
     * This method sets the IPv4 address from a given byte array.
     *
     * @param[in] aBuffer    Pointer to an array containing the IPv4 address. `kSize` bytes from the buffer
     *                       are copied to form the IPv4 address.
     *
     */
    void SetBytes(const uint8_t *aBuffer) { memcpy(mBytes, aBuffer, kSize); }

    /**
     * This method parses an IPv4 address string.
     *
     * The string MUST follow the quad-dotted notation of four decimal values (ranging from 0 to 255 each). For
     * example, "127.0.0.1"
     *
     * @param[in]  aString        A pointer to the null-terminated string.
     *
     * @retval kErrorNone         Successfully parsed the IPv4 address string.
     * @retval kErrorParse        Failed to parse the IPv4 address string.
     *
     */
    Error FromString(const char *aString);

    /**
     * This method converts the IPv4 address to a string.
     *
     * The string format uses quad-dotted notation of four bytes in the address (e.g., "127.0.0.1").
     *
     * @returns An `InfoString` representing the IPv4 address.
     *
     */
    InfoString ToString(void) const;

private:
    uint8_t mBytes[kSize];
} OT_TOOL_PACKED_END;

} // namespace Ip4
} // namespace ot

#endif // IP4_ADDRESS_HPP_
