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
 *   This file includes macros for validating runtime conditions.
 */

#ifndef PLATFORM_UTILS_ENCODING_H
#define PLATFORM_UTILS_ENCODING_H

#include <stdint.h>

/**
 * Converts a 64-bit unsigned integer stored as a little-endian byte
 * array to uint64_t.
 *
 * Allows for the array to be byte-aligned, so that loading the
 * uint64_t value via this function avoids a processor exception due
 * to unaligned access.
 *
 * @param[in]  aSource  The byte array.
 *
 * @returns The 64-bit value as a uint64_t.
 *
 */
static inline uint64_t otEncodingReadUint64Le(const uint8_t *aSource)
{
    uint64_t value = 0;

    value |= (uint64_t)aSource[0];
    value |= ((uint64_t)aSource[1]) << 8;
    value |= ((uint64_t)aSource[2]) << 16;
    value |= ((uint64_t)aSource[3]) << 24;
    value |= ((uint64_t)aSource[4]) << 32;
    value |= ((uint64_t)aSource[5]) << 40;
    value |= ((uint64_t)aSource[6]) << 48;
    value |= ((uint64_t)aSource[7]) << 56;

    return value;
}

#endif // PLATFORM_UTILS_ENCODING_H
