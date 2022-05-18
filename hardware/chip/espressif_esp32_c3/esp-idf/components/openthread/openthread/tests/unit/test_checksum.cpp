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

#include "common/encoding.hpp"
#include "common/instance.hpp"
#include "common/message.hpp"
#include "common/random.hpp"
#include "net/checksum.hpp"
#include "net/icmp6.hpp"
#include "net/udp6.hpp"

#include "test_platform.h"
#include "test_util.hpp"

namespace ot {

uint16_t CalculateChecksum(const void *aBuffer, uint16_t aLength)
{
    // Calculates checksum over a given buffer data. This implementation
    // is inspired by the algorithm from RFC-1071.

    uint32_t       sum   = 0;
    const uint8_t *bytes = reinterpret_cast<const uint8_t *>(aBuffer);

    while (aLength >= sizeof(uint16_t))
    {
        sum += Encoding::BigEndian::ReadUint16(bytes);
        bytes += sizeof(uint16_t);
        aLength -= sizeof(uint16_t);
    }

    if (aLength > 0)
    {
        sum += (static_cast<uint32_t>(bytes[0]) << 8);
    }

    // Fold 32-bit sum to 16 bits.

    while (sum >> 16)
    {
        sum = (sum & 0xffff) + (sum >> 16);
    }

    return static_cast<uint16_t>(sum & 0xffff);
}

uint16_t CalculateChecksum(const Ip6::Address &aSource,
                           const Ip6::Address &aDestination,
                           uint8_t             aIpProto,
                           const Message &     aMessage)
{
    // This method calculates the checksum over an IPv6 message.

    enum : uint16_t
    {
        kMaxPayload = 1024,
    };

    OT_TOOL_PACKED_BEGIN
    struct PseudoHeader
    {
        Ip6::Address mSource;
        Ip6::Address mDestination;
        uint32_t     mPayloadLength;
        uint32_t     mProtocol;
    } OT_TOOL_PACKED_END;

    OT_TOOL_PACKED_BEGIN
    struct ChecksumData
    {
        PseudoHeader mPseudoHeader;
        uint8_t      mPayload[kMaxPayload];
    } OT_TOOL_PACKED_END;

    ChecksumData data;
    uint16_t     payloadLength;

    payloadLength = aMessage.GetLength() - aMessage.GetOffset();

    data.mPseudoHeader.mSource        = aSource;
    data.mPseudoHeader.mDestination   = aDestination;
    data.mPseudoHeader.mProtocol      = Encoding::BigEndian::HostSwap32(aIpProto);
    data.mPseudoHeader.mPayloadLength = Encoding::BigEndian::HostSwap32(payloadLength);

    SuccessOrQuit(aMessage.Read(aMessage.GetOffset(), data.mPayload, payloadLength));

    return CalculateChecksum(&data, sizeof(PseudoHeader) + payloadLength);
}

void CorruptMessage(Message &aMessage)
{
    // Change a random bit in the message.

    uint16_t byteOffset;
    uint8_t  bitOffset;
    uint8_t  byte;

    byteOffset = Random::NonCrypto::GetUint16InRange(0, aMessage.GetLength());

    SuccessOrQuit(aMessage.Read(byteOffset, byte));

    bitOffset = Random::NonCrypto::GetUint8InRange(0, CHAR_BIT);

    byte ^= (1 << bitOffset);

    aMessage.Write(byteOffset, byte);
}

void TestUdpMessageChecksum(void)
{
    enum : uint16_t
    {
        kMinSize = sizeof(Ip6::Udp::Header),
        kMaxSize = kBufferSize * 3 + 24,
    };

    const char *kSourceAddress = "fd00:1122:3344:5566:7788:99aa:bbcc:ddee";
    const char *kDestAddress   = "fd01:2345:6789:abcd:ef01:2345:6789:abcd";

    Instance *instance = static_cast<Instance *>(testInitInstance());

    VerifyOrQuit(instance != nullptr);

    for (uint16_t size = kMinSize; size <= kMaxSize; size++)
    {
        Message *        message = instance->Get<Ip6::Ip6>().NewMessage(sizeof(Ip6::Udp::Header));
        Ip6::Udp::Header udpHeader;
        Ip6::MessageInfo messageInfo;

        VerifyOrQuit(message != nullptr, "Ip6::NewMesssage() failed");
        SuccessOrQuit(message->SetLength(size));

        // Write UDP header with a random payload.

        Random::NonCrypto::FillBuffer(reinterpret_cast<uint8_t *>(&udpHeader), sizeof(udpHeader));
        udpHeader.SetChecksum(0);
        message->Write(0, udpHeader);

        if (size > sizeof(udpHeader))
        {
            uint8_t  buffer[kMaxSize];
            uint16_t payloadSize = size - sizeof(udpHeader);

            Random::NonCrypto::FillBuffer(buffer, payloadSize);
            message->WriteBytes(sizeof(udpHeader), &buffer[0], payloadSize);
        }

        SuccessOrQuit(messageInfo.GetSockAddr().FromString(kSourceAddress));
        SuccessOrQuit(messageInfo.GetPeerAddr().FromString(kDestAddress));

        // Verify that the `Checksum::UpdateMessageChecksum` correctly
        // updates the checksum field in the UDP header on the message.

        Checksum::UpdateMessageChecksum(*message, messageInfo.GetSockAddr(), messageInfo.GetPeerAddr(), Ip6::kProtoUdp);

        SuccessOrQuit(message->Read(message->GetOffset(), udpHeader));
        VerifyOrQuit(udpHeader.GetChecksum() != 0);

        // Verify that the calculated UDP checksum is valid.

        VerifyOrQuit(CalculateChecksum(messageInfo.GetSockAddr(), messageInfo.GetPeerAddr(), Ip6::kProtoUdp,
                                       *message) == 0xffff);

        // Verify that `Checksum::VerifyMessageChecksum()` accepts the
        // message and its calculated checksum.

        SuccessOrQuit(Checksum::VerifyMessageChecksum(*message, messageInfo, Ip6::kProtoUdp));

        // Corrupt the message and verify that checksum is no longer accepted.

        CorruptMessage(*message);

        VerifyOrQuit(Checksum::VerifyMessageChecksum(*message, messageInfo, Ip6::kProtoUdp) != kErrorNone,
                     "Checksum passed on corrupted message");

        message->Free();
    }
}

void TestIcmp6MessageChecksum(void)
{
    enum : uint16_t
    {
        kMinSize = sizeof(Ip6::Icmp::Header),
        kMaxSize = kBufferSize * 3 + 24,
    };

    const char *kSourceAddress = "fd00:feef:dccd:baab:9889:7667:5444:3223";
    const char *kDestAddress   = "fd01:abab:beef:cafe:1234:5678:9abc:0";

    Instance *instance = static_cast<Instance *>(testInitInstance());

    VerifyOrQuit(instance != nullptr, "Null OpenThread instance\n");

    for (uint16_t size = kMinSize; size <= kMaxSize; size++)
    {
        Message *         message = instance->Get<Ip6::Ip6>().NewMessage(sizeof(Ip6::Icmp::Header));
        Ip6::Icmp::Header icmp6Header;
        Ip6::MessageInfo  messageInfo;

        VerifyOrQuit(message != nullptr, "Ip6::NewMesssage() failed");
        SuccessOrQuit(message->SetLength(size));

        // Write ICMP6 header with a random payload.

        Random::NonCrypto::FillBuffer(reinterpret_cast<uint8_t *>(&icmp6Header), sizeof(icmp6Header));
        icmp6Header.SetChecksum(0);
        message->Write(0, icmp6Header);

        if (size > sizeof(icmp6Header))
        {
            uint8_t  buffer[kMaxSize];
            uint16_t payloadSize = size - sizeof(icmp6Header);

            Random::NonCrypto::FillBuffer(buffer, payloadSize);
            message->WriteBytes(sizeof(icmp6Header), &buffer[0], payloadSize);
        }

        SuccessOrQuit(messageInfo.GetSockAddr().FromString(kSourceAddress));
        SuccessOrQuit(messageInfo.GetPeerAddr().FromString(kDestAddress));

        // Verify that the `Checksum::UpdateMessageChecksum` correctly
        // updates the checksum field in the ICMP6 header on the message.

        Checksum::UpdateMessageChecksum(*message, messageInfo.GetSockAddr(), messageInfo.GetPeerAddr(),
                                        Ip6::kProtoIcmp6);

        SuccessOrQuit(message->Read(message->GetOffset(), icmp6Header));
        VerifyOrQuit(icmp6Header.GetChecksum() != 0, "Failed to update checksum");

        // Verify that the calculated ICMP6 checksum is valid.

        VerifyOrQuit(CalculateChecksum(messageInfo.GetSockAddr(), messageInfo.GetPeerAddr(), Ip6::kProtoIcmp6,
                                       *message) == 0xffff);

        // Verify that `Checksum::VerifyMessageChecksum()` accepts the
        // message and its calculated checksum.

        SuccessOrQuit(Checksum::VerifyMessageChecksum(*message, messageInfo, Ip6::kProtoIcmp6));

        // Corrupt the message and verify that checksum is no longer accepted.

        CorruptMessage(*message);

        VerifyOrQuit(Checksum::VerifyMessageChecksum(*message, messageInfo, Ip6::kProtoIcmp6) != kErrorNone,
                     "Checksum passed on corrupted message");

        message->Free();
    }
}

class ChecksumTester
{
public:
    static void TestExampleVector(void)
    {
        // Example from RFC 1071
        const uint8_t  kTestVector[]       = {0x00, 0x01, 0xf2, 0x03, 0xf4, 0xf5, 0xf6, 0xf7};
        const uint16_t kTestVectorChecksum = 0xddf2;

        Checksum checksum;

        VerifyOrQuit(checksum.GetValue() == 0, "Incorrect initial checksum value");

        checksum.AddData(kTestVector, sizeof(kTestVector));
        VerifyOrQuit(checksum.GetValue() == kTestVectorChecksum);
        VerifyOrQuit(checksum.GetValue() == CalculateChecksum(kTestVector, sizeof(kTestVector)), );
    }
};

} // namespace ot

int main(void)
{
    ot::ChecksumTester::TestExampleVector();
    ot::TestUdpMessageChecksum();
    ot::TestIcmp6MessageChecksum();
    printf("All tests passed\n");
    return 0;
}
