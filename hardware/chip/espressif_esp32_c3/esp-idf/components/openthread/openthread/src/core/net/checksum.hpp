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
 *   This file includes definitions for checksum calculation.
 */

#ifndef CHECKSUM_HPP_
#define CHECKSUM_HPP_

#include "openthread-core-config.h"

#include <stdint.h>

#include "common/message.hpp"
#include "net/ip6_address.hpp"
#include "net/ip6_headers.hpp"
#include "net/socket.hpp"

namespace ot {

/**
 * This class implements IP checksum calculation and verification.
 *
 */
class Checksum
{
    friend class ChecksumTester;

public:
    /**
     * This static method verifies the checksum in a given message (if UDP/ICMP6).
     *
     * @param[in] aMessage       The message to verify its checksum. The `aMessage.GetOffset()` should point to start
     *                           UDP/ICMP6 header.
     * @param[in] aMessageInfo   The message info associated with @p aMessage.
     * @param[in] aIpProto       The Internet Protocol value.
     *
     * @retval kErrorNone    The checksum is valid if UDP/ICMP6 protocol, or not a UDP/ICMP6 protocol.
     * @retval kErrorDrop    The check is not valid and message should be dropped.
     *
     */
    static Error VerifyMessageChecksum(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, uint8_t aIpProto);

    /**
     * This static method calculates and then updates the checksum in a given message (if UDP/ICMP6).
     *
     * @param[inout] aMessage   The message to update the checksum in. The `aMessage.GetOffset()` should point to start
     *                          of the UDP/ICMP6 header. On exit the checksum field in UDP/ICMP6 header in the message
     *                          is updated.
     * @param[in] aSource       The source address.
     * @param[in] aDestination  The destination address.
     * @param[in] aIpProto      The Internet Protocol value.
     *
     */
    static void UpdateMessageChecksum(Message &           aMessage,
                                      const Ip6::Address &aSource,
                                      const Ip6::Address &aDestination,
                                      uint8_t             aIpProto);

private:
    Checksum(void)
        : mValue(0)
        , mAtOddIndex(false)
    {
    }

    uint16_t GetValue(void) const { return mValue; }
    void     AddUint8(uint8_t aUint8);
    void     AddUint16(uint16_t aUint16);
    void     AddData(const uint8_t *aBuffer, uint16_t aLength);
    void     WriteToMessage(uint16_t aOffset, Message &aMessage) const;
    void     Calculate(const Ip6::Address &aSource,
                       const Ip6::Address &aDestination,
                       uint8_t             aIpProto,
                       const Message &     aMessage);

    static constexpr uint16_t kValidRxChecksum = 0xffff;

    uint16_t mValue;
    bool     mAtOddIndex;
};

} // namespace ot

#endif // CHECKSUM_HPP_
