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

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "mac/mac.hpp"
#include "mac/mac_frame.hpp"
#include "radio/radio.hpp"

#include "test_platform.h"
#include "test_util.h"

namespace ot {

bool CompareReversed(const uint8_t *aFirst, const uint8_t *aSecond, uint16_t aLength)
{
    bool matches = true;

    for (uint16_t i = 0; i < aLength; i++)
    {
        if (aFirst[i] != aSecond[aLength - 1 - i])
        {
            matches = false;
            break;
        }
    }

    return matches;
}

void TestMacAddress(void)
{
    const uint8_t           kExtAddr[OT_EXT_ADDRESS_SIZE] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0};
    const Mac::ShortAddress kShortAddr                    = 0x1234;

    ot::Instance *  instance;
    Mac::Address    addr;
    Mac::ExtAddress extAddr;
    uint8_t         buffer[OT_EXT_ADDRESS_SIZE];

    instance = testInitInstance();
    VerifyOrQuit(instance != nullptr, "nullptr instance\n");

    // Mac::ExtAddress

    extAddr.GenerateRandom();
    VerifyOrQuit(extAddr.IsLocal(), "Random Extended Address should have its Local bit set");
    VerifyOrQuit(!extAddr.IsGroup(), "Random Extended Address should not have its Group bit set");

    extAddr.CopyTo(buffer);
    VerifyOrQuit(memcmp(extAddr.m8, buffer, OT_EXT_ADDRESS_SIZE) == 0);

    extAddr.CopyTo(buffer, Mac::ExtAddress::kReverseByteOrder);
    VerifyOrQuit(CompareReversed(extAddr.m8, buffer, OT_EXT_ADDRESS_SIZE));

    extAddr.Set(kExtAddr);
    VerifyOrQuit(memcmp(extAddr.m8, kExtAddr, OT_EXT_ADDRESS_SIZE) == 0);

    extAddr.Set(kExtAddr, Mac::ExtAddress::kReverseByteOrder);
    VerifyOrQuit(CompareReversed(extAddr.m8, kExtAddr, OT_EXT_ADDRESS_SIZE));

    extAddr.SetLocal(true);
    VerifyOrQuit(extAddr.IsLocal());
    extAddr.SetLocal(false);
    VerifyOrQuit(!extAddr.IsLocal());
    extAddr.ToggleLocal();
    VerifyOrQuit(extAddr.IsLocal());
    extAddr.ToggleLocal();
    VerifyOrQuit(!extAddr.IsLocal());

    extAddr.SetGroup(true);
    VerifyOrQuit(extAddr.IsGroup());
    extAddr.SetGroup(false);
    VerifyOrQuit(!extAddr.IsGroup());
    extAddr.ToggleGroup();
    VerifyOrQuit(extAddr.IsGroup());
    extAddr.ToggleGroup();
    VerifyOrQuit(!extAddr.IsGroup());

    // Mac::Address

    VerifyOrQuit(addr.IsNone(), "Address constructor failed");
    VerifyOrQuit(addr.GetType() == Mac::Address::kTypeNone);

    addr.SetShort(kShortAddr);
    VerifyOrQuit(addr.GetType() == Mac::Address::kTypeShort);
    VerifyOrQuit(addr.IsShort(), "Address::SetShort() failed");
    VerifyOrQuit(!addr.IsExtended(), "Address::SetShort() failed");
    VerifyOrQuit(addr.GetShort() == kShortAddr);

    addr.SetExtended(extAddr);
    VerifyOrQuit(addr.GetType() == Mac::Address::kTypeExtended);
    VerifyOrQuit(!addr.IsShort(), "Address::SetExtended() failed");
    VerifyOrQuit(addr.IsExtended(), "Address::SetExtended() failed");
    VerifyOrQuit(addr.GetExtended() == extAddr);

    addr.SetExtended(extAddr.m8, Mac::ExtAddress::kReverseByteOrder);
    VerifyOrQuit(addr.GetType() == Mac::Address::kTypeExtended);
    VerifyOrQuit(!addr.IsShort(), "Address::SetExtended() failed");
    VerifyOrQuit(addr.IsExtended(), "Address::SetExtended() failed");
    VerifyOrQuit(CompareReversed(addr.GetExtended().m8, extAddr.m8, OT_EXT_ADDRESS_SIZE),
                 "Address::SetExtended() reverse byte order failed");

    addr.SetNone();
    VerifyOrQuit(addr.GetType() == Mac::Address::kTypeNone);
    VerifyOrQuit(addr.IsNone(), "Address:SetNone() failed");
    VerifyOrQuit(!addr.IsShort(), "Address::SetNone() failed");
    VerifyOrQuit(!addr.IsExtended(), "Address::SetNone() failed");

    VerifyOrQuit(!addr.IsBroadcast(), "Address:SetNone() failed");
    VerifyOrQuit(!addr.IsShortAddrInvalid());

    addr.SetExtended(extAddr);
    VerifyOrQuit(!addr.IsBroadcast());
    VerifyOrQuit(!addr.IsShortAddrInvalid());

    addr.SetShort(kShortAddr);
    VerifyOrQuit(!addr.IsBroadcast());
    VerifyOrQuit(!addr.IsShortAddrInvalid());

    addr.SetShort(Mac::kShortAddrBroadcast);
    VerifyOrQuit(addr.IsBroadcast());
    VerifyOrQuit(!addr.IsShortAddrInvalid());

    addr.SetShort(Mac::kShortAddrInvalid);
    VerifyOrQuit(!addr.IsBroadcast());
    VerifyOrQuit(addr.IsShortAddrInvalid());

    testFreeInstance(instance);
}

void CompareNetworkName(const Mac::NetworkName &aNetworkName, const char *aNameString)
{
    uint8_t len = static_cast<uint8_t>(strlen(aNameString));

    VerifyOrQuit(strcmp(aNetworkName.GetAsCString(), aNameString) == 0);

    VerifyOrQuit(aNetworkName.GetAsData().GetLength() == len);
    VerifyOrQuit(memcmp(aNetworkName.GetAsData().GetBuffer(), aNameString, len) == 0);
}

void TestMacNetworkName(void)
{
    const char kEmptyName[]   = "";
    const char kName1[]       = "network";
    const char kName2[]       = "network-name";
    const char kLongName[]    = "0123456789abcdef";
    const char kTooLongName[] = "0123456789abcdef0";

    char             buffer[sizeof(kTooLongName) + 2];
    uint8_t          len;
    Mac::NetworkName networkName;
    Mac::NetworkName networkName2;

    CompareNetworkName(networkName, kEmptyName);

    SuccessOrQuit(networkName.Set(Mac::NameData(kName1, sizeof(kName1))));
    CompareNetworkName(networkName, kName1);

    VerifyOrQuit(networkName.Set(Mac::NameData(kName1, sizeof(kName1))) == kErrorAlready, "failed to detect duplicate");
    CompareNetworkName(networkName, kName1);

    VerifyOrQuit(networkName.Set(Mac::NameData(kName1, sizeof(kName1) - 1)) == kErrorAlready,
                 "failed to detect duplicate");

    SuccessOrQuit(networkName.Set(Mac::NameData(kName2, sizeof(kName2))));
    CompareNetworkName(networkName, kName2);

    VerifyOrQuit(networkName.Set(Mac::NameData(kEmptyName, 0)) == kErrorInvalidArgs);

    SuccessOrQuit(networkName.Set(Mac::NameData(kLongName, sizeof(kLongName))));
    CompareNetworkName(networkName, kLongName);

    VerifyOrQuit(networkName.Set(Mac::NameData(kLongName, sizeof(kLongName) - 1)) == kErrorAlready,
                 "failed to detect duplicate");

    VerifyOrQuit(networkName.Set(kEmptyName) == kErrorInvalidArgs);

    SuccessOrQuit(networkName.Set(Mac::NameData(kName1, sizeof(kName1))));

    VerifyOrQuit(networkName.Set(Mac::NameData(kTooLongName, sizeof(kTooLongName))) == kErrorInvalidArgs,
                 "accepted an invalid (too long) name");

    CompareNetworkName(networkName, kName1);

    memset(buffer, 'a', sizeof(buffer));
    len = networkName.GetAsData().CopyTo(buffer, 1);
    VerifyOrQuit(len == 1, "NameData::CopyTo() failed");
    VerifyOrQuit(buffer[0] == kName1[0], "NameData::CopyTo() failed");
    VerifyOrQuit(buffer[1] == 'a', "NameData::CopyTo() failed");

    memset(buffer, 'a', sizeof(buffer));
    len = networkName.GetAsData().CopyTo(buffer, sizeof(kName1) - 1);
    VerifyOrQuit(len == sizeof(kName1) - 1, "NameData::CopyTo() failed");
    VerifyOrQuit(memcmp(buffer, kName1, sizeof(kName1) - 1) == 0, "NameData::CopyTo() failed");
    VerifyOrQuit(buffer[sizeof(kName1)] == 'a', "NameData::CopyTo() failed");

    memset(buffer, 'a', sizeof(buffer));
    len = networkName.GetAsData().CopyTo(buffer, sizeof(buffer));
    VerifyOrQuit(len == sizeof(kName1) - 1, "NameData::CopyTo() failed");
    VerifyOrQuit(memcmp(buffer, kName1, sizeof(kName1) - 1) == 0, "NameData::CopyTo() failed");
    VerifyOrQuit(buffer[sizeof(kName1)] == 0, "NameData::CopyTo() failed");

    SuccessOrQuit(networkName2.Set(Mac::NameData(kName1, sizeof(kName1))));
    VerifyOrQuit(networkName == networkName2);

    SuccessOrQuit(networkName2.Set(kName2));
    VerifyOrQuit(networkName != networkName2);
}

void TestMacHeader(void)
{
    static const struct
    {
        uint16_t fcf;
        uint8_t  secCtl;
        uint8_t  headerLength;
        uint8_t  footerLength;
    } tests[] = {
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrNone | Mac::Frame::kFcfSrcAddrNone, 0, 3, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrNone | Mac::Frame::kFcfSrcAddrShort, 0, 7, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrNone | Mac::Frame::kFcfSrcAddrExt, 0, 13, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrShort | Mac::Frame::kFcfSrcAddrNone, 0, 7, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrExt | Mac::Frame::kFcfSrcAddrNone, 0, 13, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrShort | Mac::Frame::kFcfSrcAddrShort, 0, 11, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrShort | Mac::Frame::kFcfSrcAddrExt, 0, 17, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrExt | Mac::Frame::kFcfSrcAddrShort, 0, 17, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrExt | Mac::Frame::kFcfSrcAddrExt, 0, 23, 2},

        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrShort | Mac::Frame::kFcfSrcAddrShort |
             Mac::Frame::kFcfPanidCompression,
         0, 9, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrShort | Mac::Frame::kFcfSrcAddrExt |
             Mac::Frame::kFcfPanidCompression,
         0, 15, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrExt | Mac::Frame::kFcfSrcAddrShort |
             Mac::Frame::kFcfPanidCompression,
         0, 15, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrExt | Mac::Frame::kFcfSrcAddrExt |
             Mac::Frame::kFcfPanidCompression,
         0, 21, 2},

        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrShort | Mac::Frame::kFcfSrcAddrShort |
             Mac::Frame::kFcfPanidCompression | Mac::Frame::kFcfSecurityEnabled,
         Mac::Frame::kSecMic32 | Mac::Frame::kKeyIdMode1, 15, 6},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrShort | Mac::Frame::kFcfSrcAddrShort |
             Mac::Frame::kFcfPanidCompression | Mac::Frame::kFcfSecurityEnabled,
         Mac::Frame::kSecMic32 | Mac::Frame::kKeyIdMode2, 19, 6},
    };

    for (const auto &test : tests)
    {
        uint8_t      psdu[OT_RADIO_FRAME_MAX_SIZE];
        Mac::TxFrame frame;

        frame.mPsdu      = psdu;
        frame.mLength    = 0;
        frame.mRadioType = 0;

        frame.InitMacHeader(test.fcf, test.secCtl);
        VerifyOrQuit(frame.GetHeaderLength() == test.headerLength);
        VerifyOrQuit(frame.GetFooterLength() == test.footerLength);
        VerifyOrQuit(frame.GetLength() == test.headerLength + test.footerLength);
    }
}

void VerifyChannelMaskContent(const Mac::ChannelMask &aMask, uint8_t *aChannels, uint8_t aLength)
{
    uint8_t index = 0;
    uint8_t channel;

    for (channel = Radio::kChannelMin; channel <= Radio::kChannelMax; channel++)
    {
        if (index < aLength)
        {
            if (channel == aChannels[index])
            {
                index++;
                VerifyOrQuit(aMask.ContainsChannel(channel));
            }
            else
            {
                VerifyOrQuit(!aMask.ContainsChannel(channel));
            }
        }
    }

    index   = 0;
    channel = Mac::ChannelMask::kChannelIteratorFirst;

    while (aMask.GetNextChannel(channel) == kErrorNone)
    {
        VerifyOrQuit(channel == aChannels[index++], "ChannelMask.GetNextChannel() failed");
    }

    VerifyOrQuit(index == aLength, "ChannelMask.GetNextChannel() failed");

    if (aLength == 1)
    {
        VerifyOrQuit(aMask.IsSingleChannel());
    }
    else
    {
        VerifyOrQuit(!aMask.IsSingleChannel());
    }

    VerifyOrQuit(aLength == aMask.GetNumberOfChannels());
}

void TestMacChannelMask(void)
{
    uint8_t allChannels[] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
    uint8_t channels1[]   = {11, 14, 15, 16, 17, 20, 21, 22, 24, 25};
    uint8_t channels2[]   = {14, 21, 26};
    uint8_t channels3[]   = {14, 21};
    uint8_t channles4[]   = {20};

    static const char kEmptyMaskString[]   = "{ }";
    static const char kAllChannelsString[] = "{ 11-26 }";
    static const char kChannels1String[]   = "{ 11, 14-17, 20-22, 24, 25 }";
    static const char kChannels2String[]   = "{ 14, 21, 26 }";
    static const char kChannels3String[]   = "{ 14, 21 }";
    static const char kChannels4String[]   = "{ 20 }";

    Mac::ChannelMask mask1;
    Mac::ChannelMask mask2(Radio::kSupportedChannels);

    printf("Testing Mac::ChannelMask\n");

    VerifyOrQuit(mask1.IsEmpty());
    printf("empty = %s\n", mask1.ToString().AsCString());
    VerifyOrQuit(strcmp(mask1.ToString().AsCString(), kEmptyMaskString) == 0);

    VerifyOrQuit(!mask2.IsEmpty());
    VerifyOrQuit(mask2.GetMask() == Radio::kSupportedChannels);
    printf("allChannels = %s\n", mask2.ToString().AsCString());
    VerifyOrQuit(strcmp(mask2.ToString().AsCString(), kAllChannelsString) == 0);

    mask1.SetMask(Radio::kSupportedChannels);
    VerifyOrQuit(!mask1.IsEmpty());
    VerifyOrQuit(mask1.GetMask() == Radio::kSupportedChannels);

    VerifyChannelMaskContent(mask1, allChannels, sizeof(allChannels));

    // Test ChannelMask::RemoveChannel()
    for (uint8_t index = 0; index < sizeof(allChannels) - 1; index++)
    {
        mask1.RemoveChannel(allChannels[index]);
        VerifyChannelMaskContent(mask1, &allChannels[index + 1], sizeof(allChannels) - 1 - index);
    }

    mask1.Clear();
    VerifyOrQuit(mask1.IsEmpty());
    VerifyChannelMaskContent(mask1, nullptr, 0);

    for (uint8_t channel : channels1)
    {
        mask1.AddChannel(channel);
    }

    printf("channels1 = %s\n", mask1.ToString().AsCString());
    VerifyOrQuit(strcmp(mask1.ToString().AsCString(), kChannels1String) == 0);

    VerifyOrQuit(!mask1.IsEmpty());
    VerifyChannelMaskContent(mask1, channels1, sizeof(channels1));

    mask2.Clear();

    for (uint8_t channel : channels2)
    {
        mask2.AddChannel(channel);
    }

    printf("channels2 = %s\n", mask2.ToString().AsCString());
    VerifyOrQuit(strcmp(mask2.ToString().AsCString(), kChannels2String) == 0);

    VerifyOrQuit(!mask2.IsEmpty());
    VerifyChannelMaskContent(mask2, channels2, sizeof(channels2));

    mask1.Intersect(mask2);
    VerifyChannelMaskContent(mask1, channels3, sizeof(channels3));
    printf("channels3 = %s\n", mask1.ToString().AsCString());
    VerifyOrQuit(strcmp(mask1.ToString().AsCString(), kChannels3String) == 0);

    mask2.Clear();
    mask2.AddChannel(channles4[0]);
    VerifyChannelMaskContent(mask2, channles4, sizeof(channles4));

    printf("channels4 = %s\n", mask2.ToString().AsCString());
    VerifyOrQuit(strcmp(mask2.ToString().AsCString(), kChannels4String) == 0);

    mask1.Clear();
    mask2.Clear();
    VerifyOrQuit(mask1 == mask2);

    mask1.SetMask(Radio::kSupportedChannels);
    mask2.SetMask(Radio::kSupportedChannels);
    VerifyOrQuit(mask1 == mask2);

    mask1.Clear();
    VerifyOrQuit(mask1 != mask2);
}

void TestMacFrameApi(void)
{
    uint8_t ack_psdu1[]     = {0x02, 0x10, 0x5e, 0xd2, 0x9b};
    uint8_t mac_cmd_psdu1[] = {0x6b, 0xdc, 0x85, 0xce, 0xfa, 0x47, 0x36, 0x07, 0xd9, 0x74, 0x45, 0x8d,
                               0xb2, 0x6e, 0x81, 0x25, 0xc9, 0xdb, 0xac, 0x2b, 0x0a, 0x0d, 0x00, 0x00,
                               0x00, 0x00, 0x01, 0x04, 0xaf, 0x14, 0xce, 0xaa, 0x5a, 0xe5};

    Mac::Frame frame;

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    uint8_t data_psdu1[]    = {0x29, 0xee, 0x53, 0xce, 0xfa, 0x01, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x6e, 0x16, 0x05,
                            0x00, 0x00, 0x00, 0x00, 0x0a, 0x6e, 0x16, 0x0d, 0x01, 0x00, 0x00, 0x00, 0x01};
    uint8_t mac_cmd_psdu2[] = {0x6b, 0xaa, 0x8d, 0xce, 0xfa, 0x00, 0x68, 0x01, 0x68, 0x0d,
                               0x08, 0x00, 0x00, 0x00, 0x01, 0x04, 0x0d, 0xed, 0x0b, 0x35,
                               0x0c, 0x80, 0x3f, 0x04, 0x4b, 0x88, 0x89, 0xd6, 0x59, 0xe1};
    uint8_t scf; // SecurityControlField
#endif

    // Imm-Ack, Sequence Number: 94
    //   Frame Control Field: 0x1002
    //     .... .... .... .010 = Frame Type: Ack (0x2)
    //     .... .... .... 0... = Security Enabled: False
    //     .... .... ...0 .... = Frame Pending: False
    //     .... .... ..0. .... = Acknowledge Request: False
    //     .... .... .0.. .... = PAN ID Compression: False
    //     .... ...0 .... .... = Sequence Number Suppression: False
    //     .... ..0. .... .... = Information Elements Present: False
    //     .... 00.. .... .... = Destination Addressing Mode: None (0x0)
    //     ..01 .... .... .... = Frame Version: IEEE Std 802.15.4-2006 (1)
    //     00.. .... .... .... = Source Addressing Mode: None (0x0)
    //   Sequence Number: 94
    //   FCS: 0x9bd2 (Correct)
    frame.mPsdu   = ack_psdu1;
    frame.mLength = sizeof(ack_psdu1);
    VerifyOrQuit(frame.GetType() == Mac::Frame::kFcfFrameAck);
    VerifyOrQuit(!frame.GetSecurityEnabled());
    VerifyOrQuit(!frame.GetFramePending());
    VerifyOrQuit(!frame.GetAckRequest());
    VerifyOrQuit(!frame.IsIePresent());
    VerifyOrQuit(!frame.IsDstPanIdPresent());
    VerifyOrQuit(!frame.IsDstAddrPresent());
    VerifyOrQuit(frame.GetVersion() == Mac::Frame::kFcfFrameVersion2006);
    VerifyOrQuit(!frame.IsSrcAddrPresent());
    VerifyOrQuit(frame.GetSequence() == 94);

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    // IEEE 802.15.4-2015 Data
    //   Sequence Number: 83
    //   Destination PAN: 0xface
    //   Destination: 16:6e:0a:00:00:00:00:01
    //   Extended Source: 16:6e:0a:00:00:00:00:05
    //   Auxiliary Security Header
    //     Security Control Field: 0x0d
    frame.mPsdu   = data_psdu1;
    frame.mLength = sizeof(data_psdu1);
    VerifyOrQuit(frame.IsVersion2015());
    VerifyOrQuit(frame.IsDstPanIdPresent());
    VerifyOrQuit(frame.IsDstAddrPresent());
    VerifyOrQuit(frame.IsSrcAddrPresent());
    SuccessOrQuit(frame.GetSecurityControlField(scf));
    VerifyOrQuit(scf == 0x0d);
    frame.SetSecurityControlField(0xff);
    SuccessOrQuit(frame.GetSecurityControlField(scf));
    VerifyOrQuit(scf == 0xff);
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

    // IEEE 802.15.4-2006 Mac Command
    //   Sequence Number: 133
    //   Command Identifier: Data Request (0x04)
    uint8_t commandId;
    frame.mPsdu   = mac_cmd_psdu1;
    frame.mLength = sizeof(mac_cmd_psdu1);
    VerifyOrQuit(frame.GetSequence() == 133);
    VerifyOrQuit(frame.GetVersion() == Mac::Frame::kFcfFrameVersion2006);
    VerifyOrQuit(frame.GetType() == Mac::Frame::kFcfFrameMacCmd);
    SuccessOrQuit(frame.GetCommandId(commandId));
    VerifyOrQuit(commandId == Mac::Frame::kMacCmdDataRequest);
    SuccessOrQuit(frame.SetCommandId(Mac::Frame::kMacCmdBeaconRequest));
    SuccessOrQuit(frame.GetCommandId(commandId));
    VerifyOrQuit(commandId == Mac::Frame::kMacCmdBeaconRequest);

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    // IEEE 802.15.4-2015 Mac Command
    //   Sequence Number: 141
    //   Header IEs
    //     CSL IE
    //     Header Termination 2 IE (Payload follows)
    //   Command Identifier: Data Request (0x04)
    frame.mPsdu   = mac_cmd_psdu2;
    frame.mLength = sizeof(mac_cmd_psdu2);
    VerifyOrQuit(frame.GetSequence() == 141);
    VerifyOrQuit(frame.IsVersion2015());
    VerifyOrQuit(frame.GetType() == Mac::Frame::kFcfFrameMacCmd);
    SuccessOrQuit(frame.GetCommandId(commandId));
    VerifyOrQuit(commandId == Mac::Frame::kMacCmdDataRequest);
    printf("commandId:%d\n", commandId);
    SuccessOrQuit(frame.SetCommandId(Mac::Frame::kMacCmdOrphanNotification));
    SuccessOrQuit(frame.GetCommandId(commandId));
    VerifyOrQuit(commandId == Mac::Frame::kMacCmdOrphanNotification);

#endif
}

void TestMacFrameAckGeneration(void)
{
    Mac::RxFrame receivedFrame;
    Mac::TxFrame ackFrame;
    uint8_t      ackFrameBuffer[100];

    ackFrame.mPsdu   = ackFrameBuffer;
    ackFrame.mLength = sizeof(ackFrameBuffer);

    // Received Frame 1
    // IEEE 802.15.4 Data
    //   Frame Control Field: 0xdc61
    //     .... .... .... .001 = Frame Type: Data (0x1)
    //     .... .... .... 0... = Security Enabled: False
    //     .... .... ...0 .... = Frame Pending: False
    //     .... .... ..1. .... = Acknowledge Request: True
    //     .... .... .1.. .... = PAN ID Compression: True
    //     .... ...0 .... .... = Sequence Number Suppression: False
    //     .... ..0. .... .... = Information Elements Present: False
    //     .... 11.. .... .... = Destination Addressing Mode: Long/64-bit (0x3)
    //     ..01 .... .... .... = Frame Version: IEEE Std 802.15.4-2006 (1)
    //     11.. .... .... .... = Source Addressing Mode: Long/64-bit (0x3)
    //  Sequence Number: 189
    //  Destination PAN: 0xface
    //  Destination: 16:6e:0a:00:00:00:00:01 (16:6e:0a:00:00:00:00:01)
    //  Extended Source: 16:6e:0a:00:00:00:00:02 (16:6e:0a:00:00:00:00:02)
    uint8_t data_psdu1[]  = {0x61, 0xdc, 0xbd, 0xce, 0xfa, 0x01, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x6e, 0x16, 0x02,
                            0x00, 0x00, 0x00, 0x00, 0x0a, 0x6e, 0x16, 0x7f, 0x33, 0xf0, 0x4d, 0x4c, 0x4d, 0x4c,
                            0x8b, 0xf0, 0x00, 0x15, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc2,
                            0x57, 0x9c, 0x31, 0xb3, 0x2a, 0xa1, 0x86, 0xba, 0x9a, 0xed, 0x5a, 0xb9, 0xa3, 0x59,
                            0x88, 0xeb, 0xbb, 0x0d, 0xc3, 0xed, 0xeb, 0x8a, 0x53, 0xa6, 0xed, 0xf7, 0xdd, 0x45,
                            0x6e, 0xf7, 0x9a, 0x17, 0xb4, 0xab, 0xc6, 0x75, 0x71, 0x46, 0x37, 0x93, 0x4a, 0x32,
                            0xb1, 0x21, 0x9f, 0x9d, 0xb3, 0x65, 0x27, 0xd5, 0xfc, 0x50, 0x16, 0x90, 0xd2, 0xd4};
    receivedFrame.mPsdu   = data_psdu1;
    receivedFrame.mLength = sizeof(data_psdu1);

    ackFrame.GenerateImmAck(receivedFrame, false);
    VerifyOrQuit(ackFrame.mLength == Mac::Frame::kImmAckLength);
    VerifyOrQuit(ackFrame.GetType() == Mac::Frame::kFcfFrameAck);
    VerifyOrQuit(!ackFrame.GetSecurityEnabled());
    VerifyOrQuit(!ackFrame.GetFramePending());

    VerifyOrQuit(!ackFrame.GetAckRequest());
    VerifyOrQuit(!ackFrame.IsIePresent());
    VerifyOrQuit(!ackFrame.IsDstPanIdPresent());
    VerifyOrQuit(!ackFrame.IsDstAddrPresent());
    VerifyOrQuit(!ackFrame.IsSrcAddrPresent());
    VerifyOrQuit(ackFrame.GetVersion() == Mac::Frame::kFcfFrameVersion2006);
    VerifyOrQuit(ackFrame.GetSequence() == 189);

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    // Received Frame 2
    // IEEE 802.15.4 Data
    //   Frame Control Field: 0xa869, Frame Type: Data, Security Enabled, Acknowledge Request, PAN ID Compression,
    //   Destination Addressing Mode: Short/16-bit, Frame Version: IEEE Std 802.15.4-2015, Source Addressing Mode:
    //   Short/16-bit
    //     .... .... .... .001 = Frame Type: Data (0x1)
    //     .... .... .... 1... = Security Enabled: True
    //     .... .... ...0 .... = Frame Pending: False
    //     .... .... ..1. .... = Acknowledge Request: True
    //     .... .... .1.. .... = PAN ID Compression: True
    //     .... ...0 .... .... = Sequence Number Suppression: False
    //     .... ..0. .... .... = Information Elements Present: False
    //     .... 10.. .... .... = Destination Addressing Mode: Short/16-bit (0x2)
    //     ..10 .... .... .... = Frame Version: IEEE Std 802.15.4-2015 (2)
    //     10.. .... .... .... = Source Addressing Mode: Short/16-bit (0x2)
    //   Sequence Number: 142
    //   Destination PAN: 0xface
    //   Destination: 0x2402
    //   Source: 0x2400
    //   [Extended Source: 16:6e:0a:00:00:00:00:01 (16:6e:0a:00:00:00:00:01)]
    //   [Origin: 2]
    //   Auxiliary Security Header
    //     Security Control Field: 0x0d, Security Level: Encryption with 32-bit Message Integrity Code, Key Identifier
    //     Mode: Indexed Key using the Default Key Source
    //       .... .101 = Security Level: Encryption with 32-bit Message Integrity Code (0x5)
    //       ...0 1... = Key Identifier Mode: Indexed Key using the Default Key Source (0x1)
    //       ..0. .... = Frame Counter Suppression: False
    //       .0.. .... = ASN in Nonce: False
    //       0... .... = Reserved: 0x0
    //     Frame Counter: 2
    //     Key Identifier Field
    //       Key Index: 0x01
    //   MIC: f94e5870
    //   [Key Number: 0]
    //   FCS: 0x8c40 (Correct)
    uint8_t data_psdu2[]  = {0x69, 0xa8, 0x8e, 0xce, 0xfa, 0x02, 0x24, 0x00, 0x24, 0x0d, 0x02,
                            0x00, 0x00, 0x00, 0x01, 0x6b, 0x64, 0x60, 0x08, 0x55, 0xb8, 0x10,
                            0x18, 0xc7, 0x40, 0x2e, 0xfb, 0xf3, 0xda, 0xf9, 0x4e, 0x58, 0x70};
    receivedFrame.mPsdu   = data_psdu2;
    receivedFrame.mLength = sizeof(data_psdu2);

    uint8_t     ie_data[6] = {0x04, 0x0d, 0x21, 0x0c, 0x35, 0x0c};
    Mac::CslIe *csl;

    IgnoreError(ackFrame.GenerateEnhAck(receivedFrame, false, ie_data, sizeof(ie_data)));
    csl = reinterpret_cast<Mac::CslIe *>(ackFrame.GetHeaderIe(Mac::CslIe::kHeaderIeId) + sizeof(Mac::HeaderIe));
    VerifyOrQuit(ackFrame.mLength == 23);
    VerifyOrQuit(ackFrame.GetType() == Mac::Frame::kFcfFrameAck);
    VerifyOrQuit(ackFrame.GetSecurityEnabled());
    VerifyOrQuit(ackFrame.IsIePresent());
    VerifyOrQuit(!ackFrame.IsDstPanIdPresent());
    VerifyOrQuit(ackFrame.IsDstAddrPresent());
    VerifyOrQuit(!ackFrame.IsSrcAddrPresent());
    VerifyOrQuit(ackFrame.GetVersion() == Mac::Frame::kFcfFrameVersion2015);
    VerifyOrQuit(ackFrame.GetSequence() == 142);
    VerifyOrQuit(csl->GetPeriod() == 3125 && csl->GetPhase() == 3105);

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    ackFrame.SetCslIe(123, 456);
    csl = reinterpret_cast<Mac::CslIe *>(ackFrame.GetHeaderIe(Mac::CslIe::kHeaderIeId) + sizeof(Mac::HeaderIe));
    VerifyOrQuit(csl->GetPeriod() == 123 && csl->GetPhase() == 456);
#endif
#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
}

} // namespace ot

int main(void)
{
    ot::TestMacAddress();
    ot::TestMacNetworkName();
    ot::TestMacHeader();
    ot::TestMacChannelMask();
    ot::TestMacFrameApi();
    ot::TestMacFrameAckGeneration();
    printf("All tests passed\n");
    return 0;
}
