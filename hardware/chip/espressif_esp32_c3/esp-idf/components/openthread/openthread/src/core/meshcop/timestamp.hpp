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
 *   This file includes definitions for manipulating MeshCoP timestamps.
 *
 */

#ifndef MESHCOP_TIMESTAMP_HPP_
#define MESHCOP_TIMESTAMP_HPP_

#include "openthread-core-config.h"

#include <string.h>

#include <openthread/platform/toolchain.h>

#include "common/clearable.hpp"
#include "common/encoding.hpp"
#include "common/random.hpp"

namespace ot {
namespace MeshCoP {

using ot::Encoding::BigEndian::HostSwap16;
using ot::Encoding::BigEndian::HostSwap32;

/**
 * This class implements Timestamp generation and parsing.
 *
 */
OT_TOOL_PACKED_BEGIN
class Timestamp : public Clearable<Timestamp>
{
public:
    /**
     * This method compares this timestamp to another.
     *
     * @param[in]  aCompare  A reference to the timestamp to compare.
     *
     * @retval -1  if @p aCompare is less than this timestamp.
     * @retval  0  if @p aCompare is equal to this timestamp.
     * @retval  1  if @p aCompare is greater than this timestamp.
     *
     */
    int Compare(const Timestamp &aCompare) const;

    /**
     * This method returns the Seconds value.
     *
     * @returns The Seconds value.
     *
     */
    uint64_t GetSeconds(void) const
    {
        return (static_cast<uint64_t>(HostSwap16(mSeconds16)) << 32) + HostSwap32(mSeconds32);
    }

    /**
     * This method sets the Seconds value.
     *
     * @param[in]  aSeconds  The Seconds value.
     *
     */
    void SetSeconds(uint64_t aSeconds)
    {
        mSeconds16 = HostSwap16(static_cast<uint16_t>(aSeconds >> 32));
        mSeconds32 = HostSwap32(static_cast<uint32_t>(aSeconds & 0xffffffff));
    }

    /**
     * This method returns the Ticks value.
     *
     * @returns The Ticks value.
     *
     */
    uint16_t GetTicks(void) const { return HostSwap16(mTicks) >> kTicksOffset; }

    /**
     * This method sets the Ticks value.
     *
     * @param[in]  aTicks  The Ticks value.
     *
     */
    void SetTicks(uint16_t aTicks)
    {
        mTicks = HostSwap16((HostSwap16(mTicks) & ~kTicksMask) | ((aTicks << kTicksOffset) & kTicksMask));
    }

    /**
     * This method returns the Authoritative value.
     *
     * @returns The Authoritative value.
     *
     */
    bool GetAuthoritative(void) const { return (HostSwap16(mTicks) & kAuthoritativeMask) != 0; }

    /**
     * This method sets the Authoritative value.
     *
     * @param[in]  aAuthoritative  The Authoritative value.
     *
     */
    void SetAuthoritative(bool aAuthoritative)
    {
        mTicks = HostSwap16((HostSwap16(mTicks) & kTicksMask) |
                            ((aAuthoritative << kAuthoritativeOffset) & kAuthoritativeMask));
    }

    /**
     * This method increments the timestamp by a random number of ticks [0, 32767].
     *
     */
    void AdvanceRandomTicks(void);

private:
    static constexpr uint8_t  kTicksOffset         = 1;
    static constexpr uint16_t kTicksMask           = 0x7fff << kTicksOffset;
    static constexpr uint16_t kMaxRandomTicks      = 0x7fff;
    static constexpr uint8_t  kAuthoritativeOffset = 0;
    static constexpr uint16_t kAuthoritativeMask   = 1 << kAuthoritativeOffset;

    uint16_t mSeconds16; // bits 32-47
    uint32_t mSeconds32; // bits 0-31
    uint16_t mTicks;
} OT_TOOL_PACKED_END;

} // namespace MeshCoP
} // namespace ot

#endif // MESHCOP_TIMESTAMP_HPP_
