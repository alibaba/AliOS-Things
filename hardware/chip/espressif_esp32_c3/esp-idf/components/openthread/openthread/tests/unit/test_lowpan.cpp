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

#include "test_lowpan.hpp"

#include "test_platform.h"
#include "test_util.hpp"

using namespace ot;

namespace ot {

ot::Instance *  sInstance;
Ip6::Ip6 *      sIp6;
Lowpan::Lowpan *sLowpan;

void TestIphcVector::GetCompressedStream(uint8_t *aIphc, uint16_t &aIphcLength)
{
    memcpy(aIphc, mIphcHeader.mData, mIphcHeader.mLength);
    memcpy(aIphc + mIphcHeader.mLength, mPayload.mData, mPayload.mLength);

    aIphcLength = mIphcHeader.mLength + mPayload.mLength;
}

void TestIphcVector::GetUncompressedStream(uint8_t *aIp6, uint16_t &aIp6Length)
{
    aIp6Length = 0;

    memcpy(aIp6, reinterpret_cast<uint8_t *>(&mIpHeader), sizeof(mIpHeader));
    aIp6Length += sizeof(mIpHeader);

    if (mExtHeader.mLength)
    {
        memcpy(aIp6 + aIp6Length, mExtHeader.mData, mExtHeader.mLength);
        aIp6Length += mExtHeader.mLength;
    }

    if (mIpTunneledHeader.GetPayloadLength())
    {
        memcpy(aIp6 + aIp6Length, reinterpret_cast<uint8_t *>(&mIpTunneledHeader), sizeof(mIpTunneledHeader));
        aIp6Length += sizeof(mIpTunneledHeader);
    }

    if (mUdpHeader.GetLength())
    {
        memcpy(aIp6 + aIp6Length, reinterpret_cast<uint8_t *>(&mUdpHeader), sizeof(mUdpHeader));
        aIp6Length += sizeof(mUdpHeader);
    }

    memcpy(aIp6 + aIp6Length, mPayload.mData, mPayload.mLength);
    aIp6Length += mPayload.mLength;
}

void TestIphcVector::GetUncompressedStream(Message &aMessage)
{
    SuccessOrQuit(aMessage.Append(mIpHeader));

    if (mExtHeader.mLength)
    {
        SuccessOrQuit(aMessage.AppendBytes(mExtHeader.mData, mExtHeader.mLength));
    }

    if (mIpTunneledHeader.GetPayloadLength())
    {
        SuccessOrQuit(aMessage.Append(mIpTunneledHeader));
    }

    if (mUdpHeader.GetLength())
    {
        SuccessOrQuit(aMessage.Append(mUdpHeader));
    }

    SuccessOrQuit(aMessage.AppendBytes(mPayload.mData, mPayload.mLength));
}

/**
 * This function initializes Thread Interface.
 *
 */
static void Init(void)
{
    otMeshLocalPrefix meshLocalPrefix = {{0xfd, 0x00, 0xca, 0xfe, 0xfa, 0xce, 0x12, 0x34}};

    sInstance->Get<Mle::MleRouter>().SetMeshLocalPrefix(static_cast<Mle::MeshLocalPrefix &>(meshLocalPrefix));

    // Emulate global prefixes with contextes.
    uint8_t mockNetworkData[] = {
        0x0c, // MLE Network Data Type
        0x20, // MLE Network Data Length

        // Prefix 2001:2:0:1::/64
        0x03, 0x0e,                                                             // Prefix TLV
        0x00, 0x40, 0x20, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x07, 0x02, // 6LoWPAN Context ID TLV
        0x11, 0x40,                                                             // Context ID = 1, C = TRUE

        // Prefix 2001:2:0:2::/64
        0x03, 0x0e,                                                             // Prefix TLV
        0x00, 0x40, 0x20, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x07, 0x02, // 6LoWPAN Context ID TLV
        0x02, 0x40                                                              // Context ID = 2, C = FALSE
    };

    Message *message = sInstance->Get<MessagePool>().New(Message::kTypeIp6, 0);
    VerifyOrQuit(message != nullptr, "Ip6::NewMessage failed");

    SuccessOrQuit(message->AppendBytes(mockNetworkData, sizeof(mockNetworkData)));

    IgnoreError(sInstance->Get<NetworkData::Leader>().SetNetworkData(0, 0, true, *message, 0));
}

/**
 * This function performs compression or/and decompression based on the given test vector.
 *
 * @note Performing decompression and compression on the same LOWPAN_IPHC frame may give different result.
 *       This situation may occur when sender does not use the best possible compression,
 *       e.g. doesn't use LOWPAN_NHC for UDP - which is still valid.
 *
 * @param aVector     Test vector that has to be tested.
 * @param aCompress   Set to TRUE, if compression should be tested.
 * @param aDecompress Set to TRUE, if decomrpession should be tested.
 */
static void Test(TestIphcVector &aVector, bool aCompress, bool aDecompress)
{
    Message *message = nullptr;
    uint8_t  result[512];
    uint8_t  iphc[512];
    uint8_t  ip6[512];
    uint16_t iphcLength;
    uint16_t ip6Length;

    aVector.GetCompressedStream(iphc, iphcLength);
    aVector.GetUncompressedStream(ip6, ip6Length);

    printf("\n=== Test name: %s ===\n\n", aVector.mTestName);

    printf("Expected error -------------- %s\n", aVector.mError ? "yes" : "no");
    printf("UDP present ----------------- %s\n", aVector.mUdpHeader.GetLength() ? "yes" : "no");
    printf("Extension Headers present --- %s\n", aVector.mExtHeader.mLength ? "yes" : "no");
    printf("IP-in-IP present ------------ %s\n", aVector.mIpTunneledHeader.GetPayloadLength() ? "yes" : "no");
    printf("LOWPAN_IPHC length ---------- %d\n", aVector.mIphcHeader.mLength);
    printf("IPv6 uncompressed offset ---- %d\n\n", aVector.mPayloadOffset);

    DumpBuffer("Expected IPv6 uncompressed packet", ip6, ip6Length);
    DumpBuffer("Expected LOWPAN_IPHC compressed frame", iphc, iphcLength);

    if (aCompress)
    {
        Lowpan::BufferWriter buffer(result, 127);

        VerifyOrQuit((message = sInstance->Get<MessagePool>().New(Message::kTypeIp6, 0)) != nullptr);

        aVector.GetUncompressedStream(*message);

        VerifyOrQuit(sLowpan->Compress(*message, aVector.mMacSource, aVector.mMacDestination, buffer) ==
                     aVector.mError);

        if (aVector.mError == kErrorNone)
        {
            uint8_t compressBytes = static_cast<uint8_t>(buffer.GetWritePointer() - result);

            // Append payload to the LOWPAN_IPHC.
            message->ReadBytes(message->GetOffset(), result + compressBytes,
                               message->GetLength() - message->GetOffset());

            DumpBuffer("Resulted LOWPAN_IPHC compressed frame", result,
                       compressBytes + message->GetLength() - message->GetOffset());

            VerifyOrQuit(compressBytes == aVector.mIphcHeader.mLength, "Lowpan::Compress failed");
            VerifyOrQuit(message->GetOffset() == aVector.mPayloadOffset, "Lowpan::Compress failed");
            VerifyOrQuit(memcmp(iphc, result, iphcLength) == 0, "Lowpan::Compress failed");
        }

        message->Free();
        message = nullptr;
    }

    if (aDecompress)
    {
        VerifyOrQuit((message = sInstance->Get<MessagePool>().New(Message::kTypeIp6, 0)) != nullptr);

        int decompressedBytes =
            sLowpan->Decompress(*message, aVector.mMacSource, aVector.mMacDestination, iphc, iphcLength, 0);

        message->ReadBytes(0, result, message->GetLength());

        if (aVector.mError == kErrorNone)
        {
            // Append payload to the IPv6 Packet.
            memcpy(result + message->GetLength(), iphc + decompressedBytes,
                   iphcLength - static_cast<uint16_t>(decompressedBytes));

            DumpBuffer("Resulted IPv6 uncompressed packet", result,
                       message->GetLength() + iphcLength - static_cast<uint16_t>(decompressedBytes));

            VerifyOrQuit(decompressedBytes == aVector.mIphcHeader.mLength, "Lowpan::Decompress failed");
            VerifyOrQuit(message->GetOffset() == aVector.mPayloadOffset, "Lowpan::Decompress failed");
            VerifyOrQuit(message->GetOffset() == message->GetLength(), "Lowpan::Decompress failed");
            VerifyOrQuit(memcmp(ip6, result, ip6Length) == 0, "Lowpan::Decompress failed");
        }
        else
        {
            VerifyOrQuit(decompressedBytes < 0, "Lowpan::Decompress failed");
        }

        message->Free();
        message = nullptr;
    }

    printf("PASS\n\n");
}

/***************************************************************************************************
 * @section Test constants.
 **************************************************************************************************/
static const uint8_t sTestMacSourceDefaultLong[]      = {0x00, 0x00, 0x5e, 0xef, 0x10, 0x22, 0x11, 0x00};
static const uint8_t sTestMacDestinationDefaultLong[] = {0x00, 0x00, 0x5e, 0xef, 0x10, 0xaa, 0xbb, 0xcc};

static uint16_t sTestMacSourceDefaultShort      = 0x0000;
static uint16_t sTestMacDestinationDefaultShort = 0xc003;
static uint16_t sTestMacDestinationBroadcast    = 0xffff;

static const uint8_t sTestPayloadDefault[] = {0x80, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};

/***************************************************************************************************
 * @section Test cases.
 **************************************************************************************************/

static void TestFullyCompressableLongAddresses(void)
{
    TestIphcVector testVector("Fully compressable IPv6 addresses using long MAC addresses");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64, "fe80::200:5eef:1022:1100",
                           "fe80::200:5eef:10aa:bbcc");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x33, 0x3a};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestFullyCompressableShortAddresses(void)
{
    TestIphcVector testVector("Fully compressable IPv6 addresses using short MAC addresses");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationDefaultShort);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64, "fe80::ff:fe00:0000",
                           "fe80::ff:fe00:c003");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x33, 0x3a};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestFullyCompressableShortLongAddresses(void)
{
    TestIphcVector testVector("Fully compressable IPv6 addresses using short and long MAC addresses");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64, "fe80::ff:fe00:0000",
                           "fe80::200:5eef:10aa:bbcc");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x33, 0x3a};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestFullyCompressableLongShortAddresses(void)
{
    TestIphcVector testVector("Fully compressable IPv6 addresses using long and short MAC addresses");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultShort);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64, "fe80::200:5eef:1022:1100",
                           "fe80::ff:fe00:c003");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x33, 0x3a};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestSourceUnspecifiedAddress(void)
{
    TestIphcVector testVector("Unspecified source IPv6 address");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultShort);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64, "::", "fe80::ff:fe00:c003");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x43, 0x3a};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestSource128bitDestination128bitAddresses(void)
{
    TestIphcVector testVector("IPv6 addresses inline");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultShort);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64,
                           "2001:2:0:3:aaaa:bbbb:cccc:dddd", "2001:2:0:4::");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x00, 0x3a, 0x20, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0xaa,
                      0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0x20, 0x01, 0x00, 0x02, 0x00,
                      0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestSource64bitDestination64bitLongAddresses(void)
{
    TestIphcVector testVector("IPv6 addresses 64-bit using long MAC addresses");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64, "fe80::200:5eef:1022:1101",
                           "fe80::200:5eef:10aa:bbcd");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x11, 0x3a, 0x02, 0x00, 0x5e, 0xef, 0x10, 0x22, 0x11,
                      0x01, 0x02, 0x00, 0x5e, 0xef, 0x10, 0xaa, 0xbb, 0xcd};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestSource64bitDestination64bitShortAddresses(void)
{
    TestIphcVector testVector("IPv6 addresses 64-bit using short MAC addresses");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationDefaultShort);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64, "fe80::200:5eef:1022:1101",
                           "fe80::200:5eef:10aa:bbcd");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x11, 0x3a, 0x02, 0x00, 0x5e, 0xef, 0x10, 0x22, 0x11,
                      0x01, 0x02, 0x00, 0x5e, 0xef, 0x10, 0xaa, 0xbb, 0xcd};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestSource16bitDestination16bitAddresses(void)
{
    TestIphcVector testVector("IPv6 addresses 16-bit using short MAC addresses");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationDefaultShort);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64, "fe80::ff:fe00:0001",
                           "fe80::ff:fe00:c004");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x22, 0x3a, 0x00, 0x01, 0xc0, 0x04};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestSourceCompressedDestination16bitAddresses(void)
{
    TestIphcVector testVector("Fully compressable IPv6 source and destination 16-bit using long MAC addresses");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64, "fe80::200:5eef:1022:1100",
                           "fe80::ff:fe00:beaf");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x32, 0x3a, 0xbe, 0xaf};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestSourceCompressedDestination128bitAddresses(void)
{
    TestIphcVector testVector("Fully compressable IPv6 source and destination inline using long MAC addresses");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64, "fe80::200:5eef:1022:1100",
                           "2001:2:0:4::");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x30, 0x3a, 0x20, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00,
                      0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestMulticast128bitAddress(void)
{
    TestIphcVector testVector("Multicast address inline");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationBroadcast);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64, "fe80::200:5eef:1022:1100",
                           "ff05::100:0030:0001");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x38, 0x3a, 0xff, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x30, 0x00, 0x01};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestMulticast48bitAddress(void)
{
    TestIphcVector testVector("Multicast address 48-bit");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationBroadcast);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64, "fe80::200:5eef:1022:1100",
                           "ff05::1:0030:0001");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x39, 0x3a, 0x05, 0x01, 0x00, 0x30, 0x00, 0x01};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestMulticast32bitAddress(void)
{
    TestIphcVector testVector("Multicast address 32-bit");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationBroadcast);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64, "fe80::200:5eef:1022:1100",
                           "ff03::fc");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x1a, 0x3a, 0x02, 0x00, 0x5e, 0xef, 0x10, 0x22, 0x11, 0x00, 0x03, 0x00, 0x00, 0xfc};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestMulticast8bitAddress(void)
{
    TestIphcVector testVector("Multicast address 8-bit");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationBroadcast);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64, "fe80::200:5eef:1022:1100",
                           "ff02::2");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x3b, 0x3a, 0x02};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestStatefulSource64bitDestination64bitContext0(void)
{
    TestIphcVector testVector("Stateful compression source and destination addresses 64-bit, context 0");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationDefaultShort);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64,
                           "fd00:cafe:face:1234:abcd:ef01:2345:6789", "fd00:cafe:face:1234:c31d:a702:0d41:beef");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x55, 0x3a, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67,
                      0x89, 0xc3, 0x1d, 0xa7, 0x02, 0x0d, 0x41, 0xbe, 0xef};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestStatefulSource64bitDestination64bitContext0IfContextInLine(void)
{
    TestIphcVector testVector("Stateful compression source and destination addresses 64-bit, context 0 inline");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationDefaultShort);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64,
                           "fd00:cafe:face:1234:abcd:ef01:2345:6789", "fd00:cafe:face:1234:c31d:a702:0d41:beef");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0xd5, 0x00, 0x3a, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45,
                      0x67, 0x89, 0xc3, 0x1d, 0xa7, 0x02, 0x0d, 0x41, 0xbe, 0xef};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform decompression test only.
    Test(testVector, false, true);
}

static void TestStatefulSource16bitDestination16bitContext0(void)
{
    TestIphcVector testVector("Stateful compression source and destination addresses 16-bit, context 0");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationDefaultShort);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64,
                           "fd00:cafe:face:1234::ff:fe00:fffc", "fd00:cafe:face:1234::ff:fe00:fffe");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x66, 0x3a, 0xff, 0xfc, 0xff, 0xfe};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestStatefulCompressableLongAddressesContext0(void)
{
    TestIphcVector testVector("Stateful compression compressable long addresses, context 0");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64,
                           "fd00:cafe:face:1234:0200:5eef:1022:1100", "fd00:cafe:face:1234:0200:5eef:10aa:bbcc");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x77, 0x3a};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestStatefulCompressableShortAddressesContext0(void)
{
    TestIphcVector testVector("Stateful compression compressable short addresses, context 0");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationDefaultShort);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64,
                           "fd00:cafe:face:1234::ff:fe00:0000", "fd00:cafe:face:1234::ff:fe00:c003");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x77, 0x3a};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestStatefulCompressableLongShortAddressesContext0(void)
{
    TestIphcVector testVector("Stateful compression compressable long and short addresses, context 0");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultShort);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64,
                           "fd00:cafe:face:1234:0200:5eef:1022:1100", "fd00:cafe:face:1234::ff:fe00:c003");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x77, 0x3a};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestStatefulSource64bitDestination128bitContext1(void)
{
    TestIphcVector testVector("Stateful compression source addresses 64-bit and destination inline, context 1");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationDefaultShort);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64,
                           "2001:2:0:1:abcd:ef01:2345:6789", "2001:2:0:3:c31d:a702:0d41:beef");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0xd0, 0x10, 0x3a, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0x20, 0x01,
                      0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0xc3, 0x1d, 0xa7, 0x02, 0x0d, 0x41, 0xbe, 0xef};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestStatefulSource64bitDestination64bitContext1(void)
{
    TestIphcVector testVector("Stateful compression source and destination addresses 64-bit, context 1");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationDefaultShort);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64,
                           "2001:2:0:1:abcd:ef01:2345:6789", "2001:2:0:1:c31d:a702:0d41:beef");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0xd5, 0x11, 0x3a, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45,
                      0x67, 0x89, 0xc3, 0x1d, 0xa7, 0x02, 0x0d, 0x41, 0xbe, 0xef};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestStatefulSourceDestinationInlineContext2CIDFalse(void)
{
    TestIphcVector testVector("Stateful compression source and destination addresses inline, context 2 (C=FALSE)");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationDefaultShort);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64,
                           "2001:2:0:2:abcd:ef01:2345:6789", "2001:2:0:2:c31d:a702:0d41:beef");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x00, 0x3a, 0x20, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0xab,
                      0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0x20, 0x01, 0x00, 0x02, 0x00,
                      0x00, 0x00, 0x02, 0xc3, 0x1d, 0xa7, 0x02, 0x0d, 0x41, 0xbe, 0xef};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression test only.
    Test(testVector, true, false);
}

static void TestStatefulMulticastDestination48bitContext0(void)
{
    TestIphcVector testVector("Stateful compression multicast address, context 0");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultShort);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64,
                           "fd00:cafe:face:1234:0200:5eef:1022:1100", "ff33:0040:fd00:cafe:face:1234:0000:0001");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x7c, 0x3a, 0x33, 0x00, 0x00, 0x00, 0x00, 0x01};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform decompression tests.
    Test(testVector, true, true);
}

static void TestTrafficClassFlowLabel3Bytes(void)
{
    TestIphcVector testVector("Traffic Class and Flow Label 3 bytes");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x6011ac59, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64, "fe80::200:5eef:1022:1100",
                           "fe80::200:5eef:10aa:bbcc");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x6a, 0x33, 0x41, 0xac, 0x59, 0x3a};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestTrafficClassFlowLabel1Byte(void)
{
    TestIphcVector testVector("Traffic Class and Flow Label 1 byte");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60d00000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64, "fe80::200:5eef:1022:1100",
                           "fe80::200:5eef:10aa:bbcc");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x72, 0x33, 0x43, 0x3a};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestTrafficClassFlowLabel1ByteEcnOnly(void)
{
    TestIphcVector testVector("Traffic Class and Flow Label 1 byte with ecn only");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60100000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64, "fe80::200:5eef:1022:1100",
                           "fe80::200:5eef:10aa:bbcc");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x72, 0x33, 0x40, 0x3a};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestTrafficClassFlowLabelInline(void)
{
    TestIphcVector testVector("Traffic Class and Flow Label inline");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x6ea12345, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 64, "fe80::200:5eef:1022:1100",
                           "fe80::200:5eef:10aa:bbcc");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x62, 0x33, 0xBA, 0x01, 0x23, 0x45, 0x3a};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestHopLimit1(void)
{
    TestIphcVector testVector("Hop Limit 1");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationDefaultShort);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 1, "fe80::ff:fe00:0000",
                           "fe80::ff:fe00:c003");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x79, 0x33, 0x3a};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestHopLimit255(void)
{
    TestIphcVector testVector("Hop Limit 255");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationDefaultShort);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 255, "fe80::ff:fe00:0000",
                           "fe80::ff:fe00:c003");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7B, 0x33, 0x3a};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestHopLimitInline(void)
{
    TestIphcVector testVector("Hop Limit Inline");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationDefaultShort);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 63, "fe80::ff:fe00:0000",
                           "fe80::ff:fe00:c003");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x78, 0x33, 0x3a, 0x3f};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestUdpSourceDestinationInline(void)
{
    TestIphcVector testVector("UDP source and destination inline");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault) + 8, Ip6::kProtoUdp, 64, "fe80::200:5eef:1022:1100",
                           "fe80::200:5eef:10aa:bbcc");

    // Setup UDP header.
    testVector.SetUDPHeader(5683, 5684, sizeof(sTestPayloadDefault) + 8, 0xbeef);

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7e, 0x33, 0xf0, 0x16, 0x33, 0x16, 0x34, 0xbe, 0xef};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(48);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestUdpSourceInlineDestination8bit(void)
{
    TestIphcVector testVector("UDP source inline destination 8-bit");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault) + 8, Ip6::kProtoUdp, 64, "fe80::200:5eef:1022:1100",
                           "fe80::200:5eef:10aa:bbcc");

    // Setup UDP header.
    testVector.SetUDPHeader(5683, 61441, sizeof(sTestPayloadDefault) + 8, 0xbeef);

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7e, 0x33, 0xf1, 0x16, 0x33, 0x01, 0xbe, 0xef};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(48);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestUdpSource8bitDestinationInline(void)
{
    TestIphcVector testVector("UDP source 8-bit destination 8-bit");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault) + 8, Ip6::kProtoUdp, 64, "fe80::200:5eef:1022:1100",
                           "fe80::200:5eef:10aa:bbcc");

    // Setup UDP header.
    testVector.SetUDPHeader(61695, 5683, sizeof(sTestPayloadDefault) + 8, 0xbeef);

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7e, 0x33, 0xf2, 0xff, 0x16, 0x33, 0xbe, 0xef};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(48);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestUdpFullyCompressed(void)
{
    TestIphcVector testVector("UDP fully compressed");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault) + 8, Ip6::kProtoUdp, 64, "fe80::200:5eef:1022:1100",
                           "fe80::200:5eef:10aa:bbcc");

    // Setup UDP header.
    testVector.SetUDPHeader(61616, 61631, sizeof(sTestPayloadDefault) + 8, 0xface);

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7e, 0x33, 0xf3, 0x0f, 0xfa, 0xce};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(48);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestUdpFullyCompressedMulticast(void)
{
    TestIphcVector testVector("UDP fully compressed with IP multicast");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault) + 8, Ip6::kProtoUdp, 64, "fe80::200:5eef:1022:1100",
                           "ff02::1");

    // Setup UDP header.
    testVector.SetUDPHeader(61616, 61631, sizeof(sTestPayloadDefault) + 8, 0xface);

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7e, 0x3b, 0x01, 0xf3, 0x0f, 0xfa, 0xce};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(48);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestUdpWithoutNhc(void)
{
    TestIphcVector testVector("UDP without LOWPAN_NHC compression");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationDefaultShort);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoUdp, 64, "fe80::ff:fe00:0000",
                           "fe80::ff:fe00:c003");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x33, 0x11};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(40);
    testVector.SetError(kErrorNone);

    // Perform only decompression test.
    Test(testVector, false, true);
}

static void TestExtensionHeaderHopByHopNoPadding(void)
{
    TestIphcVector testVector("Extension Header - Hop-by-Hop with no padding");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationBroadcast);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault) + 8, Ip6::kProtoHopOpts, 64,
                           "fd00:cafe:face:1234::ff:fe00:0000", "ff03::1");

    // Setup extension header.
    uint8_t extHeader[] = {0x3a, 0x00, 0x6d, 0x04, 0x60, 0x11, 0x00, 0x0c};
    testVector.SetExtHeader(extHeader, sizeof(extHeader));

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7e, 0x7a, 0x03, 0x00, 0x00, 0x01, 0xe0, 0x3a, 0x06, 0x6d, 0x04, 0x60, 0x11, 0x00, 0x0c};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(48);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestExtensionHeaderHopByHopPad1(void)
{
    TestIphcVector testVector("Extension Header - Hop-by-Hop with Pad1");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationBroadcast);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault) + 8, Ip6::kProtoHopOpts, 64,
                           "fd00:cafe:face:1234::ff:fe00:0000", "ff03::1");

    // Setup extension header.
    uint8_t extHeader[] = {0x3a, 0x00, 0x6d, 0x03, 0x60, 0x11, 0x00, 0x00};
    testVector.SetExtHeader(extHeader, sizeof(extHeader));

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7e, 0x7a, 0x03, 0x00, 0x00, 0x01, 0xe0, 0x3a, 0x05, 0x6d, 0x03, 0x60, 0x11, 0x00};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(48);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestExtensionHeaderHopByHopPadN2(void)
{
    TestIphcVector testVector("Extension Header - Hop-by-Hop with PadN2");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationBroadcast);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault) + 8, Ip6::kProtoHopOpts, 64,
                           "fd00:cafe:face:1234::ff:fe00:0000", "ff03::1");

    // Setup extension header.
    uint8_t extHeader[] = {0x3a, 0x00, 0x6d, 0x02, 0x60, 0x11, 0x01, 0x00};
    testVector.SetExtHeader(extHeader, sizeof(extHeader));

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7e, 0x7a, 0x03, 0x00, 0x00, 0x01, 0xe0, 0x3a, 0x04, 0x6d, 0x02, 0x60, 0x11};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(48);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestExtensionHeaderHopByHopPadN3(void)
{
    TestIphcVector testVector("Extension Header - Hop-by-Hop with PadN3");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationBroadcast);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault) + 8, Ip6::kProtoHopOpts, 64,
                           "fd00:cafe:face:1234::ff:fe00:0000", "ff03::1");

    // Setup extension header.
    uint8_t extHeader[] = {0x3a, 0x00, 0x6d, 0x01, 0x60, 0x01, 0x01, 0x00};
    testVector.SetExtHeader(extHeader, sizeof(extHeader));

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7e, 0x7a, 0x03, 0x00, 0x00, 0x01, 0xe0, 0x3a, 0x03, 0x6d, 0x01, 0x60};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(48);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestExtensionHeaderHopByHopPadN4(void)
{
    TestIphcVector testVector("Extension Header - Hop-by-Hop with PadN4");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationBroadcast);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault) + 8, Ip6::kProtoHopOpts, 64,
                           "fd00:cafe:face:1234::ff:fe00:0000", "ff03::1");

    // Setup extension header.
    uint8_t extHeader[] = {0x3a, 0x00, 0x6d, 0x00, 0x01, 0x02, 0x00, 0x00};
    testVector.SetExtHeader(extHeader, sizeof(extHeader));

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7e, 0x7a, 0x03, 0x00, 0x00, 0x01, 0xe0, 0x3a, 0x02, 0x6d, 0x00};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(48);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestExtensionHeaderHopByHopPadN5(void)
{
    TestIphcVector testVector("Extension Header - Hop-by-Hop with PadN5");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationBroadcast);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault) + 16, Ip6::kProtoHopOpts, 64,
                           "fd00:cafe:face:1234::ff:fe00:0000", "ff03::1");

    // Setup extension header.
    uint8_t extHeader[] = {0x3a, 0x01, 0x6d, 0x07, 0x01, 0x02, 0x01, 0x00,
                           0x00, 0x00, 0x33, 0x01, 0x03, 0x00, 0x00, 0x00};
    testVector.SetExtHeader(extHeader, sizeof(extHeader));

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7e, 0x7a, 0x03, 0x00, 0x00, 0x01, 0xe0, 0x3a, 0x09,
                      0x6d, 0x07, 0x01, 0x02, 0x01, 0x00, 0x00, 0x00, 0x33};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(56);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestExtensionHeaderHopByHopPadN6(void)
{
    TestIphcVector testVector("Extension Header - Hop-by-Hop with PadN6");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationBroadcast);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault) + 24, Ip6::kProtoHopOpts, 64,
                           "fd00:cafe:face:1234::ff:fe00:0000", "ff03::1");

    // Setup extension header.
    uint8_t extHeader[] = {0x3a, 0x02, 0x6d, 0x0e, 0x01, 0x02, 0x01, 0x00, 0x00, 0x00, 0x33, 0x01,
                           0x03, 0x00, 0x00, 0x00, 0x11, 0x00, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00};
    testVector.SetExtHeader(extHeader, sizeof(extHeader));

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7e, 0x7a, 0x03, 0x00, 0x00, 0x01, 0xe0, 0x3a, 0x10, 0x6d, 0x0e, 0x01, 0x02,
                      0x01, 0x00, 0x00, 0x00, 0x33, 0x01, 0x03, 0x00, 0x00, 0x00, 0x11, 0x00};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(64);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestExtensionHeaderHopByHopPadN7(void)
{
    TestIphcVector testVector("Extension Header - Hop-by-Hop with PadN7");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationBroadcast);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault) + 24, Ip6::kProtoHopOpts, 64,
                           "fd00:cafe:face:1234::ff:fe00:0000", "ff03::1");

    // Setup extension header.
    uint8_t extHeader[] = {0x3a, 0x02, 0x6d, 0x0d, 0x01, 0x02, 0x01, 0x00, 0x00, 0x00, 0x33, 0x01,
                           0x03, 0x00, 0x00, 0x00, 0x11, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00};
    testVector.SetExtHeader(extHeader, sizeof(extHeader));

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7e, 0x7a, 0x03, 0x00, 0x00, 0x01, 0xe0, 0x3a, 0x0f, 0x6d, 0x0d, 0x01,
                      0x02, 0x01, 0x00, 0x00, 0x00, 0x33, 0x01, 0x03, 0x00, 0x00, 0x00, 0x11};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(64);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestExtensionHeaderHopByHopPadN2UdpFullyCompressed(void)
{
    TestIphcVector testVector("Extension Header - Hop-by-Hop with PadN2 and UDP");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationBroadcast);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault) + 16, Ip6::kProtoHopOpts, 64,
                           "fd00:cafe:face:1234::ff:fe00:0000", "ff03::1");

    // Setup extension header.
    uint8_t extHeader[] = {0x11, 0x00, 0x6d, 0x02, 0x60, 0x11, 0x01, 0x00};
    testVector.SetExtHeader(extHeader, sizeof(extHeader));

    // Setup UDP header.
    testVector.SetUDPHeader(61616, 61631, sizeof(sTestPayloadDefault) + 8, 0xface);

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7e, 0x7a, 0x03, 0x00, 0x00, 0x01, 0xe1, 0x04, 0x6d, 0x02, 0x60, 0x11, 0xf3, 0x0f, 0xfa, 0xce};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(56);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestIpInIpHopByHopPadN2UdpSourceDestinationInline(void)
{
    TestIphcVector testVector("IP-in-IP with Hop-by-Hop with PadN2 and UDP");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultShort);
    testVector.SetMacDestination(sTestMacDestinationBroadcast);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault) + 56, Ip6::kProtoHopOpts, 64,
                           "fd00:cafe:face:1234::ff:fe00:0000", "ff03::fc");

    // Setup extension header.
    uint8_t extHeader[] = {0x29, 0x00, 0x6d, 0x02, 0x00, 0x11, 0x01, 0x00};
    testVector.SetExtHeader(extHeader, sizeof(extHeader));

    // Setup IPv6 tunneled header.
    testVector.SetIpTunneledHeader(0x60000000, sizeof(sTestPayloadDefault) + 8, Ip6::kProtoUdp, 64,
                                   "fd00:cafe:face:1234::ff:fe00:0000", "ff05::1");

    // Setup UDP header.
    testVector.SetUDPHeader(5683, 5684, sizeof(sTestPayloadDefault) + 8, 0xbeef);

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7e, 0x7a, 0x03, 0x00, 0x00, 0xfc, 0xe1, 0x04, 0x6d, 0x02, 0x00, 0x11, 0xee,
                      0x7e, 0x7a, 0x05, 0x00, 0x00, 0x01, 0xf0, 0x16, 0x33, 0x16, 0x34, 0xbe, 0xef};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(96);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestIpInIpWithoutExtensionHeader(void)
{
    TestIphcVector testVector("IP-in-IP without extension header");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Setup IPv6 header.
    testVector.SetIpHeader(0x60000000, sizeof(sTestPayloadDefault) + 40, Ip6::kProtoIp6, 64, "fe80::200:5eef:1022:1100",
                           "ff03::1");

    // Setup IPv6 tunneled header.
    testVector.SetIpTunneledHeader(0x60000000, sizeof(sTestPayloadDefault), Ip6::kProtoIcmp6, 1,
                                   "fe80::200:5eef:1022:1100", "fe80::200:5eef:10aa:bbcc");

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7e, 0x3a, 0x03, 0x00, 0x00, 0x01, 0xee, 0x79, 0x33, 0x3a};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetPayload(sTestPayloadDefault, sizeof(sTestPayloadDefault));
    testVector.SetPayloadOffset(80);
    testVector.SetError(kErrorNone);

    // Perform compression and decompression tests.
    Test(testVector, true, true);
}

static void TestErrorNoIphcDispatch(void)
{
    TestIphcVector testVector("Invalid dispatch");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x40, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetError(kErrorParse);

    // Perform decompression test.
    Test(testVector, false, true);
}

static void TestErrorTruncatedIphc(void)
{
    TestIphcVector testVector("Truncated LOWPAN_IPHC");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x00};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetError(kErrorParse);

    // Perform decompression test.
    Test(testVector, false, true);
}

static void TestErrorReservedValueDestination0100(void)
{
    TestIphcVector testVector("Reserved value of M-DAC-DAM - 0100");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x34, 0x3A};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetError(kErrorParse);

    // Perform decompression test.
    Test(testVector, false, true);
}

static void TestErrorReservedValueDestination1101(void)
{
    TestIphcVector testVector("Reserved value of M-DAC-DAM - 1101");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x3D, 0x3A};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetError(kErrorParse);

    // Perform decompression test.
    Test(testVector, false, true);
}

static void TestErrorReservedValueDestination1110(void)
{
    TestIphcVector testVector("Reserved value of M-DAC-DAM - 1110");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x3E, 0x3A};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetError(kErrorParse);

    // Perform decompression test.
    Test(testVector, false, true);
}

static void TestErrorReservedValueDestination1111(void)
{
    TestIphcVector testVector("Reserved value of M-DAC-DAM - 1111");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7a, 0x3F, 0x3A};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetError(kErrorParse);

    // Perform decompression test.
    Test(testVector, false, true);
}

static void TestErrorUnknownNhc(void)
{
    TestIphcVector testVector("Unknown value of LOWPAN_NHC ID");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7e, 0x33, 0x00};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetError(kErrorParse);

    // Perform decompression test.
    Test(testVector, false, true);
}

static void TestErrorReservedNhc5(void)
{
    TestIphcVector testVector("Reserved value of LOWPAN_NHC EID - 0x05");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7e, 0x33, 0xea};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetError(kErrorParse);

    // Perform decompression test.
    Test(testVector, false, true);
}

static void TestErrorReservedNhc6(void)
{
    TestIphcVector testVector("Reserved value of LOWPAN_NHC EID - 0x06");

    // Setup MAC addresses.
    testVector.SetMacSource(sTestMacSourceDefaultLong);
    testVector.SetMacDestination(sTestMacDestinationDefaultLong);

    // Set LOWPAN_IPHC header.
    uint8_t iphc[] = {0x7e, 0x33, 0xec};
    testVector.SetIphcHeader(iphc, sizeof(iphc));

    // Set payload and error.
    testVector.SetError(kErrorParse);

    // Perform decompression test.
    Test(testVector, false, true);
}

/***************************************************************************************************
 * @section Main test.
 **************************************************************************************************/

void TestLowpanIphc(void)
{
    sInstance = testInitInstance();

    VerifyOrQuit(sInstance != nullptr, "nullptr instance");

    sIp6    = &sInstance->Get<Ip6::Ip6>();
    sLowpan = &sInstance->Get<Lowpan::Lowpan>();

    Init();

    // Stateless unicast addresses compression / decompression tests.
    TestFullyCompressableLongAddresses();
    TestFullyCompressableShortAddresses();
    TestFullyCompressableShortLongAddresses();
    TestFullyCompressableLongShortAddresses();
    TestSourceUnspecifiedAddress();
    TestSource128bitDestination128bitAddresses();
    TestSource64bitDestination64bitLongAddresses();
    TestSource64bitDestination64bitShortAddresses();
    TestSource16bitDestination16bitAddresses();
    TestSourceCompressedDestination16bitAddresses();
    TestSourceCompressedDestination128bitAddresses();

    // Stateless multicast addresses compression / decompression tests.
    TestMulticast128bitAddress();
    TestMulticast48bitAddress();
    TestMulticast32bitAddress();
    TestMulticast8bitAddress();

    // Stateful unicast addresses compression / decompression tests.
    TestStatefulSource64bitDestination64bitContext0();
    TestStatefulSource64bitDestination64bitContext0IfContextInLine();
    TestStatefulSource16bitDestination16bitContext0();
    TestStatefulCompressableLongAddressesContext0();
    TestStatefulCompressableShortAddressesContext0();
    TestStatefulCompressableLongShortAddressesContext0();
    TestStatefulSource64bitDestination128bitContext1();
    TestStatefulSource64bitDestination64bitContext1();
    TestStatefulSourceDestinationInlineContext2CIDFalse();

    // Stateful multicast addresses compression / decompression tests.
    TestStatefulMulticastDestination48bitContext0();

    // Traffic Class and Flow Label compression / decompression tests.
    TestTrafficClassFlowLabel3Bytes();
    TestTrafficClassFlowLabel1Byte();
    TestTrafficClassFlowLabel1ByteEcnOnly();
    TestTrafficClassFlowLabelInline();

    // Hop Limit compression / decompression tests.
    TestHopLimit1();
    TestHopLimit255();
    TestHopLimitInline();

    // UDP compression / decompression tests.
    TestUdpSourceDestinationInline();
    TestUdpSourceInlineDestination8bit();
    TestUdpSource8bitDestinationInline();
    TestUdpFullyCompressed();
    TestUdpFullyCompressedMulticast();
    TestUdpWithoutNhc();

    // Extension Headers compression / decompression tests.
    TestExtensionHeaderHopByHopNoPadding();
    TestExtensionHeaderHopByHopPad1();
    TestExtensionHeaderHopByHopPadN2();
    TestExtensionHeaderHopByHopPadN3();
    TestExtensionHeaderHopByHopPadN4();
    TestExtensionHeaderHopByHopPadN5();
    TestExtensionHeaderHopByHopPadN6();
    TestExtensionHeaderHopByHopPadN7();
    TestExtensionHeaderHopByHopPadN2UdpFullyCompressed();

    // IP-in-IP compression / decompression tests.
    TestIpInIpHopByHopPadN2UdpSourceDestinationInline();
    TestIpInIpWithoutExtensionHeader();

    // Invalid frame to be decompressed.
    TestErrorNoIphcDispatch();
    TestErrorTruncatedIphc();
    TestErrorReservedValueDestination0100();
    TestErrorReservedValueDestination1101();
    TestErrorReservedValueDestination1110();
    TestErrorReservedValueDestination1111();
    TestErrorUnknownNhc();
    TestErrorReservedNhc5();
    TestErrorReservedNhc6();

    testFreeInstance(sInstance);
}

void TestLowpanMeshHeader(void)
{
    enum
    {
        kMaxFrameSize = 127,
        kSourceAddr   = 0x100,
        kDestAddr     = 0x200,
    };

    const uint8_t kMeshHeader1[] = {0xb1, 0x01, 0x00, 0x02, 0x00};       // src:0x100, dest:0x200, hop:0x1
    const uint8_t kMeshHeader2[] = {0xbf, 0x20, 0x01, 0x00, 0x02, 0x00}; // src:0x100, dest:0x200, hop:0x20
    const uint8_t kMeshHeader3[] = {0xbf, 0x01, 0x01, 0x00, 0x02, 0x00}; // src:0x100, dest:0x200, hop:0x1 (deepHops)

    uint8_t            frame[kMaxFrameSize];
    uint16_t           length;
    uint16_t           headerLength;
    Lowpan::MeshHeader meshHeader;

    meshHeader.Init(kSourceAddr, kDestAddr, 1);
    VerifyOrQuit(meshHeader.GetSource() == kSourceAddr, "failed after Init()");
    VerifyOrQuit(meshHeader.GetDestination() == kDestAddr, "failed after Init()");
    VerifyOrQuit(meshHeader.GetHopsLeft() == 1, "failed after Init()");

    length = meshHeader.WriteTo(frame);
    VerifyOrQuit(length == meshHeader.GetHeaderLength());
    VerifyOrQuit(length == sizeof(kMeshHeader1), "MeshHeader::WriteTo() returned length is incorrect");
    VerifyOrQuit(memcmp(frame, kMeshHeader1, length) == 0, "MeshHeader::WriteTo() failed");

    memset(&meshHeader, 0, sizeof(meshHeader));
    VerifyOrQuit(Lowpan::MeshHeader::IsMeshHeader(frame, length));
    SuccessOrQuit(meshHeader.ParseFrom(frame, length, headerLength));
    VerifyOrQuit(headerLength == length, "MeshHeader::ParseFrom() returned length is incorrect");
    VerifyOrQuit(meshHeader.GetSource() == kSourceAddr, "failed after ParseFrom()");
    VerifyOrQuit(meshHeader.GetDestination() == kDestAddr, "failed after ParseFrom()");
    VerifyOrQuit(meshHeader.GetHopsLeft() == 1, "failed after ParseFrom()");

    VerifyOrQuit(meshHeader.ParseFrom(frame, length - 1, headerLength) == kErrorParse,
                 "MeshHeader::ParseFrom() did not fail with incorrect length");

    //- - - - - - - - - - - - - - - - - - - - - - - - - -

    meshHeader.Init(kSourceAddr, kDestAddr, 0x20);
    VerifyOrQuit(meshHeader.GetSource() == kSourceAddr, "failed after Init()");
    VerifyOrQuit(meshHeader.GetDestination() == kDestAddr, "failed after Init()");
    VerifyOrQuit(meshHeader.GetHopsLeft() == 0x20, "failed after Init()");

    length = meshHeader.WriteTo(frame);
    VerifyOrQuit(length == sizeof(kMeshHeader2), "MeshHeader::WriteTo() returned length is incorrect");
    VerifyOrQuit(length == meshHeader.GetHeaderLength());
    VerifyOrQuit(memcmp(frame, kMeshHeader2, length) == 0, "MeshHeader::WriteTo() failed");

    memset(&meshHeader, 0, sizeof(meshHeader));
    VerifyOrQuit(Lowpan::MeshHeader::IsMeshHeader(frame, length));
    SuccessOrQuit(meshHeader.ParseFrom(frame, length, headerLength));
    VerifyOrQuit(headerLength == length, "MeshHeader::ParseFrom() returned length is incorrect");
    VerifyOrQuit(meshHeader.GetSource() == kSourceAddr, "failed after ParseFrom()");
    VerifyOrQuit(meshHeader.GetDestination() == kDestAddr, "failed after ParseFrom()");
    VerifyOrQuit(meshHeader.GetHopsLeft() == 0x20, "failed after ParseFrom()");

    VerifyOrQuit(meshHeader.ParseFrom(frame, length - 1, headerLength) == kErrorParse,
                 "MeshHeader::ParseFrom() did not fail with incorrect length");

    //- - - - - - - - - - - - - - - - - - - - - - - - - -

    SuccessOrQuit(meshHeader.ParseFrom(kMeshHeader3, sizeof(kMeshHeader3), headerLength));
    VerifyOrQuit(headerLength == sizeof(kMeshHeader3), "MeshHeader::ParseFrom() returned length is incorrect");
    VerifyOrQuit(meshHeader.GetSource() == kSourceAddr, "failed after ParseFrom()");
    VerifyOrQuit(meshHeader.GetDestination() == kDestAddr, "failed after ParseFrom()");
    VerifyOrQuit(meshHeader.GetHopsLeft() == 1, "failed after ParseFrom()");

    VerifyOrQuit(meshHeader.WriteTo(frame) == sizeof(kMeshHeader1));

    VerifyOrQuit(meshHeader.ParseFrom(kMeshHeader3, sizeof(kMeshHeader3) - 1, headerLength) == kErrorParse,
                 "MeshHeader::ParseFrom() did not fail with incorrect length");
}

void TestLowpanFragmentHeader(void)
{
    enum
    {
        kMaxFrameSize = 127,
        kSize         = 0x7ef,
        kTag          = 0x1234,
        kOffset       = (100 * 8),
    };

    const uint8_t kFragHeader1[] = {0xc7, 0xef, 0x12, 0x34};       // size:0x7ef, tag:0x1234, offset:0 (first frag)
    const uint8_t kFragHeader2[] = {0xe7, 0xef, 0x12, 0x34, 0x64}; // size:0x7ef, tag:0x1234, offset:100 (next frag)
    const uint8_t kFragHeader3[] = {0xe7, 0xef, 0x12, 0x34, 0x00}; // size:0x7ef, tag:0x1234, offset:0 (next frag)

    const uint8_t kInvalidFragHeader1[] = {0xe8, 0xef, 0x12, 0x34, 0x64};
    const uint8_t kInvalidFragHeader2[] = {0xd0, 0xef, 0x12, 0x34, 0x64};
    const uint8_t kInvalidFragHeader3[] = {0x90, 0xef, 0x12, 0x34, 0x64};

    uint8_t                frame[kMaxFrameSize];
    uint16_t               length;
    uint16_t               headerLength;
    Lowpan::FragmentHeader fragHeader;

    fragHeader.InitFirstFragment(kSize, kTag);
    VerifyOrQuit(fragHeader.GetDatagramSize() == kSize, "failed after Init");
    VerifyOrQuit(fragHeader.GetDatagramTag() == kTag, "failed after Init()");
    VerifyOrQuit(fragHeader.GetDatagramOffset() == 0, "failed after Init()");

    length = fragHeader.WriteTo(frame);
    VerifyOrQuit(length == Lowpan::FragmentHeader::kFirstFragmentHeaderSize,
                 "FragmentHeader::WriteTo() returned length is incorrect");
    VerifyOrQuit(length == sizeof(kFragHeader1), "FragmentHeader::WriteTo() returned length is incorrect");
    VerifyOrQuit(memcmp(frame, kFragHeader1, length) == 0, "FragmentHeader::WriteTo() failed");

    memset(&fragHeader, 0, sizeof(fragHeader));
    VerifyOrQuit(Lowpan::FragmentHeader::IsFragmentHeader(frame, length));
    SuccessOrQuit(fragHeader.ParseFrom(frame, length, headerLength));
    VerifyOrQuit(headerLength == length, "FragmentHeader::ParseFrom() returned length is incorrect");
    VerifyOrQuit(fragHeader.GetDatagramSize() == kSize, "failed after ParseFrom()");
    VerifyOrQuit(fragHeader.GetDatagramTag() == kTag, "failed after ParseFrom()");
    VerifyOrQuit(fragHeader.GetDatagramOffset() == 0, "failed after ParseFrom()");

    VerifyOrQuit(fragHeader.ParseFrom(frame, length - 1, headerLength) == kErrorParse,
                 "FragmentHeader::ParseFrom() did not fail with incorrect length");

    //- - - - - - - - - - - - - - - - - - - - - - - - - -

    fragHeader.Init(kSize, kTag, kOffset);
    VerifyOrQuit(fragHeader.GetDatagramSize() == kSize, "failed after Init");
    VerifyOrQuit(fragHeader.GetDatagramTag() == kTag, "failed after Init()");
    VerifyOrQuit(fragHeader.GetDatagramOffset() == kOffset, "failed after Init()");

    // Check the truncation of offset (to be multiple of 8).
    fragHeader.Init(kSize, kTag, kOffset + 1);
    VerifyOrQuit(fragHeader.GetDatagramOffset() == kOffset, "FragmentHeader::GetDatagramOffset() did not truncate");
    fragHeader.Init(kSize, kTag, kOffset + 7);
    VerifyOrQuit(fragHeader.GetDatagramOffset() == kOffset, "FragmentHeader::GetDatagramOffset() did not truncate");

    length = fragHeader.WriteTo(frame);
    VerifyOrQuit(length == Lowpan::FragmentHeader::kSubsequentFragmentHeaderSize,
                 "FragmentHeader::WriteTo() returned length is incorrect");
    VerifyOrQuit(length == sizeof(kFragHeader2), "FragmentHeader::WriteTo() returned length is incorrect");
    VerifyOrQuit(memcmp(frame, kFragHeader2, length) == 0, "FragmentHeader::WriteTo() failed");

    memset(&fragHeader, 0, sizeof(fragHeader));
    VerifyOrQuit(Lowpan::FragmentHeader::IsFragmentHeader(frame, length));
    SuccessOrQuit(fragHeader.ParseFrom(frame, length, headerLength));
    VerifyOrQuit(headerLength == length, "FragmentHeader::ParseFrom() returned length is incorrect");
    VerifyOrQuit(fragHeader.GetDatagramSize() == kSize, "failed after ParseFrom()");
    VerifyOrQuit(fragHeader.GetDatagramTag() == kTag, "failed after ParseFrom()");
    VerifyOrQuit(fragHeader.GetDatagramOffset() == kOffset, "failed after ParseFrom()");

    VerifyOrQuit(fragHeader.ParseFrom(frame, length - 1, headerLength) == kErrorParse,
                 "FragmentHeader::ParseFrom() did not fail with incorrect length");

    //- - - - - - - - - - - - - - - - - - - - - - - - - -

    length = sizeof(kFragHeader3);
    memcpy(frame, kFragHeader3, length);
    SuccessOrQuit(fragHeader.ParseFrom(frame, length, headerLength));
    VerifyOrQuit(headerLength == length, "FragmentHeader::ParseFrom() returned length is incorrect");
    VerifyOrQuit(fragHeader.GetDatagramSize() == kSize, "failed after ParseFrom()");
    VerifyOrQuit(fragHeader.GetDatagramTag() == kTag, "failed after ParseFrom()");
    VerifyOrQuit(fragHeader.GetDatagramOffset() == 0, "failed after ParseFrom()");

    VerifyOrQuit(fragHeader.ParseFrom(frame, length - 1, headerLength) == kErrorParse,
                 "FragmentHeader::ParseFrom() did not fail with incorrect length");

    //- - - - - - - - - - - - - - - - - - - - - - - - - -

    length = sizeof(kInvalidFragHeader1);
    memcpy(frame, kInvalidFragHeader1, length);
    VerifyOrQuit(!Lowpan::FragmentHeader::IsFragmentHeader(frame, length),
                 "IsFragmentHeader() did not detect invalid header");
    VerifyOrQuit(fragHeader.ParseFrom(frame, length, headerLength) != kErrorNone,
                 "FragmentHeader::ParseFrom() did not fail with invalid header");

    length = sizeof(kInvalidFragHeader2);
    memcpy(frame, kInvalidFragHeader2, length);
    VerifyOrQuit(!Lowpan::FragmentHeader::IsFragmentHeader(frame, length),
                 "IsFragmentHeader() did not detect invalid header");
    VerifyOrQuit(fragHeader.ParseFrom(frame, length, headerLength) != kErrorNone,
                 "FragmentHeader::ParseFrom() did not fail with invalid header");

    length = sizeof(kInvalidFragHeader3);
    memcpy(frame, kInvalidFragHeader3, length);
    VerifyOrQuit(!Lowpan::FragmentHeader::IsFragmentHeader(frame, length),
                 "IsFragmentHeader() did not detect invalid header");
    VerifyOrQuit(fragHeader.ParseFrom(frame, length, headerLength) != kErrorNone,
                 "FragmentHeader::ParseFrom() did not fail with invalid header");
}

} // namespace ot

int main(void)
{
    TestLowpanIphc();
    TestLowpanMeshHeader();
    TestLowpanFragmentHeader();

    printf("All tests passed\n");
    return 0;
}
