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
 *   This file implements IPv4 address related functionality.
 */

#include "ip4_address.hpp"

#include "common/code_utils.hpp"
#include "common/numeric_limits.hpp"

namespace ot {
namespace Ip4 {

Error Address::FromString(const char *aString)
{
    constexpr char kSeperatorChar = '.';
    constexpr char kNullChar      = '\0';

    Error error = kErrorParse;

    for (uint8_t index = 0;; index++)
    {
        uint16_t value         = 0;
        uint8_t  hasFirstDigit = false;

        for (char digitChar = *aString;; ++aString, digitChar = *aString)
        {
            if ((digitChar < '0') || (digitChar > '9'))
            {
                break;
            }

            value = static_cast<uint16_t>((value * 10) + static_cast<uint8_t>(digitChar - '0'));
            VerifyOrExit(value <= NumericLimits<uint8_t>::kMax);
            hasFirstDigit = true;
        }

        VerifyOrExit(hasFirstDigit);

        mBytes[index] = static_cast<uint8_t>(value);

        if (index == sizeof(Address) - 1)
        {
            break;
        }

        VerifyOrExit(*aString == kSeperatorChar);
        aString++;
    }

    VerifyOrExit(*aString == kNullChar);
    error = kErrorNone;

exit:
    return error;
}

Address::InfoString Address::ToString(void) const
{
    InfoString string;

    string.Append("%d.%d.%d.%d", mBytes[0], mBytes[1], mBytes[2], mBytes[3]);

    return string;
}

} // namespace Ip4
} // namespace ot
