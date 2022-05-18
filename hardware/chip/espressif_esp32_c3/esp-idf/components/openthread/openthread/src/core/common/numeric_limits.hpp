/*
 *  Copyright (c) 2020, The OpenThread Authors.
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
 *   This file includes definitions for numeric limits.
 */

#ifndef NUMERIC_LIMITS_HPP_
#define NUMERIC_LIMITS_HPP_

#include <stdint.h>

namespace ot {

/**
 * This class template provides a way to query properties of arithmetic types.
 *
 * There are no members if `Type` is not a supported arithmetic type.
 *
 */
template <typename Type> struct NumericLimits
{
};

// Specialization for different integral types.

template <> struct NumericLimits<int8_t>
{
    static constexpr int8_t kMin = INT8_MIN;
    static constexpr int8_t kMax = INT8_MAX;
};

template <> struct NumericLimits<int16_t>
{
    static constexpr int16_t kMin = INT16_MIN;
    static constexpr int16_t kMax = INT16_MAX;
};

template <> struct NumericLimits<int32_t>
{
    static constexpr int32_t kMin = INT32_MIN;
    static constexpr int32_t kMax = INT32_MAX;
};

template <> struct NumericLimits<int64_t>
{
    static constexpr int64_t kMin = INT64_MIN;
    static constexpr int64_t kMax = INT64_MAX;
};

template <> struct NumericLimits<uint8_t>
{
    static constexpr uint8_t kMin = 0;
    static constexpr uint8_t kMax = UINT8_MAX;
};

template <> struct NumericLimits<uint16_t>
{
    static constexpr uint16_t kMin = 0;
    static constexpr uint16_t kMax = UINT16_MAX;
};

template <> struct NumericLimits<uint32_t>
{
    static constexpr uint32_t kMin = 0;
    static constexpr uint32_t kMax = UINT32_MAX;
};

template <> struct NumericLimits<uint64_t>
{
    static constexpr uint64_t kMin = 0;
    static constexpr uint64_t kMax = UINT64_MAX;
};

} // namespace ot

#endif // NUMERIC_LIMITS_HPP_
