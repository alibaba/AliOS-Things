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
 *   This file includes definitions for CRC16 computations.
 */

#ifndef CRC16_HPP_
#define CRC16_HPP_

#include "openthread-core-config.h"

#include <stdint.h>

namespace ot {

/**
 * This class implements CRC16 computations.
 *
 */
class Crc16
{
public:
    enum Polynomial : uint16_t
    {
        kCcitt = 0x1021, ///< CRC16_CCITT
        kAnsi  = 0x8005, ///< CRC16-ANSI
    };

    /**
     * This constructor initializes the object.
     *
     * @param[in]  aPolynomial  The polynomial value.
     *
     */
    explicit Crc16(Polynomial aPolynomial);

    /**
     * This method initializes the CRC16 computation.
     *
     */
    void Init(void) { mCrc = 0; }

    /*c*
     * This method feeds a byte value into the CRC16 computation.
     *
     * @param[in]  aByte  The byte value.
     *
     */
    void Update(uint8_t aByte);

    /**
     * This method gets the current CRC16 value.
     *
     * @returns The current CRC16 value.
     *
     */
    uint16_t Get(void) const { return mCrc; }

private:
    uint16_t mPolynomial;
    uint16_t mCrc;
};

} // namespace ot

#endif // CRC16_HPP_
