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
 *   This file includes definitions for a bit-vector.
 */

#ifndef BIT_VECTOR_HPP_
#define BIT_VECTOR_HPP_

#include "openthread-core-config.h"

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/encoding.hpp"
#include "common/equatable.hpp"

namespace ot {

/**
 * @addtogroup core-bit-vector
 *
 * @brief
 *   This module includes definitions for bit-vector.
 *
 * @{
 *
 */

/**
 * This class represents a bit-vector.
 *
 * @tparam N  Specifies the number of bits.
 *
 */
template <uint16_t N> class BitVector : public Equatable<BitVector<N>>, public Clearable<BitVector<N>>
{
public:
    /**
     * This method indicates whether a given index is included in the mask.
     *
     * @param[in] aIndex  The index.
     *
     * @retval TRUE   If the given index is set.
     * @retval FALSE  If the given index is clear.
     *
     */
    bool Get(uint16_t aIndex) const
    {
        OT_ASSERT(aIndex < N);
        return (mMask[aIndex / 8] & (0x80 >> (aIndex % 8))) != 0;
    }

    /**
     * This method sets the mask of a given index.
     *
     * @param[in] aIndex  The index.
     * @param[in] aValue  TRUE to set the mask, or FALSE to clear the mask.
     *
     */
    void Set(uint16_t aIndex, bool aValue)
    {
        OT_ASSERT(aIndex < N);

        if (aValue)
        {
            mMask[aIndex / 8] |= 0x80 >> (aIndex % 8);
        }
        else
        {
            mMask[aIndex / 8] &= ~(0x80 >> (aIndex % 8));
        };
    }

    /**
     * This method returns if any mask is set.
     *
     * @retval TRUE   If any index is set.
     * @retval FALSE  If all indexes are clear.
     *
     */
    bool HasAny(void) const
    {
        bool rval = false;

        for (uint8_t b : mMask)
        {
            if (b != 0)
            {
                ExitNow(rval = true);
            }
        }

    exit:
        return rval;
    }

private:
    uint8_t mMask[BitVectorBytes(N)];
};

/**
 * @}
 *
 */

} // namespace ot

#endif // BIT_VECTOR_HPP_
