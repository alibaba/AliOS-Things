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
 *   This file implements checksum calculation.
 */

#include "checksum.hpp"

#include "common/code_utils.hpp"
#include "common/message.hpp"
#include "net/icmp6.hpp"
#include "net/tcp6.hpp"
#include "net/udp6.hpp"

namespace ot {

void Checksum::AddUint8(uint8_t aUint8)
{
    uint16_t newValue = mValue;

    // BigEndian encoding: Even index is MSB and odd index is LSB.

    newValue += mAtOddIndex ? aUint8 : (static_cast<uint16_t>(aUint8) << 8);

    // Calculate one's complement sum.

    if (newValue < mValue)
    {
        newValue++;
    }

    mValue      = newValue;
    mAtOddIndex = !mAtOddIndex;
}

void Checksum::AddUint16(uint16_t aUint16)
{
    // BigEndian encoding
    AddUint8(static_cast<uint8_t>(aUint16 >> 8));
    AddUint8(static_cast<uint8_t>(aUint16 & 0xff));
}

void Checksum::AddData(const uint8_t *aBuffer, uint16_t aLength)
{
    for (uint16_t i = 0; i < aLength; i++)
    {
        AddUint8(aBuffer[i]);
    }
}

void Checksum::WriteToMessage(uint16_t aOffset, Message &aMessage) const
{
    uint16_t checksum = GetValue();

    if (checksum != 0xffff)
    {
        checksum = ~checksum;
    }

    checksum = Encoding::BigEndian::HostSwap16(checksum);

    aMessage.Write(aOffset, checksum);
}

void Checksum::Calculate(const Ip6::Address &aSource,
                         const Ip6::Address &aDestination,
                         uint8_t             aIpProto,
                         const Message &     aMessage)
{
    Message::Chunk chunk;
    uint16_t       length = aMessage.GetLength() - aMessage.GetOffset();

    // Pseudo-header for checksum calculation (RFC-2460).

    AddData(aSource.GetBytes(), sizeof(Ip6::Address));
    AddData(aDestination.GetBytes(), sizeof(Ip6::Address));
    AddUint16(length);
    AddUint16(static_cast<uint16_t>(aIpProto));

    // Add message content (from offset to the end) to checksum.

    aMessage.GetFirstChunk(aMessage.GetOffset(), length, chunk);

    while (chunk.GetLength() > 0)
    {
        AddData(chunk.GetBytes(), chunk.GetLength());
        aMessage.GetNextChunk(length, chunk);
    }
}

Error Checksum::VerifyMessageChecksum(const Message &aMessage, const Ip6::MessageInfo &aMessageInfo, uint8_t aIpProto)
{
    Checksum checksum;

    checksum.Calculate(aMessageInfo.GetPeerAddr(), aMessageInfo.GetSockAddr(), aIpProto, aMessage);

    return (checksum.GetValue() == kValidRxChecksum) ? kErrorNone : kErrorDrop;
}

void Checksum::UpdateMessageChecksum(Message &           aMessage,
                                     const Ip6::Address &aSource,
                                     const Ip6::Address &aDestination,
                                     uint8_t             aIpProto)
{
    uint16_t headerOffset;
    Checksum checksum;

    switch (aIpProto)
    {
    case Ip6::kProtoTcp:
        headerOffset = Ip6::Tcp::Header::kChecksumFieldOffset;
        break;

    case Ip6::kProtoUdp:
        headerOffset = Ip6::Udp::Header::kChecksumFieldOffset;
        break;

    case Ip6::kProtoIcmp6:
        headerOffset = Ip6::Icmp::Header::kChecksumFieldOffset;
        break;

    default:
        ExitNow();
    }

    checksum.Calculate(aSource, aDestination, aIpProto, aMessage);
    checksum.WriteToMessage(aMessage.GetOffset() + headerOffset, aMessage);

exit:
    return;
}

} // namespace ot
