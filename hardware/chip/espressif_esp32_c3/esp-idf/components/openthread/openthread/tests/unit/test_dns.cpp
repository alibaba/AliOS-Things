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

#include <string.h>

#include <openthread/config.h>

#include "test_platform.h"
#include "test_util.hpp"

#include "common/instance.hpp"
#include "net/dns_types.hpp"

namespace ot {

void TestDnsName(void)
{
    enum
    {
        kMaxSize       = 300,
        kMaxNameLength = Dns::Name::kMaxNameSize - 1,
    };

    struct TestName
    {
        const char *   mName;
        uint16_t       mEncodedLength;
        const uint8_t *mEncodedData;
        const char **  mLabels;
        const char *   mExpectedReadName;
    };

    Instance *   instance;
    MessagePool *messagePool;
    Message *    message;
    uint8_t      buffer[kMaxSize];
    uint16_t     len;
    uint16_t     offset;
    char         label[Dns::Name::kMaxLabelSize];
    uint8_t      labelLength;
    char         name[Dns::Name::kMaxNameSize];

    static const uint8_t kEncodedName1[] = {7, 'e', 'x', 'a', 'm', 'p', 'l', 'e', 3, 'c', 'o', 'm', 0};
    static const uint8_t kEncodedName2[] = {3, 'f', 'o', 'o', 1, 'a', 2, 'b', 'b', 3, 'e', 'd', 'u', 0};
    static const uint8_t kEncodedName3[] = {10, 'f', 'o', 'u', 'n', 'd', 'a', 't', 'i', 'o', 'n', 0};
    static const uint8_t kEncodedName4[] = {0};

    static const char *kLabels1[] = {"example", "com", nullptr};
    static const char *kLabels2[] = {"foo", "a", "bb", "edu", nullptr};
    static const char *kLabels3[] = {"foundation", nullptr};
    static const char *kLabels4[] = {nullptr};

    static const TestName kTestNames[] = {
        {"example.com", sizeof(kEncodedName1), kEncodedName1, kLabels1, "example.com."},
        {"example.com.", sizeof(kEncodedName1), kEncodedName1, kLabels1, "example.com."},
        {"foo.a.bb.edu", sizeof(kEncodedName2), kEncodedName2, kLabels2, "foo.a.bb.edu."},
        {"foo.a.bb.edu.", sizeof(kEncodedName2), kEncodedName2, kLabels2, "foo.a.bb.edu."},
        {"foundation", sizeof(kEncodedName3), kEncodedName3, kLabels3, "foundation."},
        {"foundation.", sizeof(kEncodedName3), kEncodedName3, kLabels3, "foundation."},
        {"", sizeof(kEncodedName4), kEncodedName4, kLabels4, "."},
        {".", sizeof(kEncodedName4), kEncodedName4, kLabels4, "."},
        {nullptr, sizeof(kEncodedName4), kEncodedName4, kLabels4, "."},
    };

    static const char *kMaxLengthNames[] = {
        "HereIsSomeoneHidden.MyHoldFromMeTaken.FromSelfHasMeDriven.MyLeadFromMeTaken."
        "HereIsSomeoneHidden.AsLifeSweeterThanLife.TakesMeToGardenOfSoul.MyFortFromMeTaken."
        "HereIsSomeoneHidden.LikeSugarInSugarCane.ASweetSugarTrader.MyShopFromMeTaken."
        "SorcererAndMagicia.",

        "HereIsSomeoneHidden.MyHoldFromMeTaken.FromSelfHasMeDriven.MyLeadFromMeTaken."
        "HereIsSomeoneHidden.AsLifeSweeterThanLife.TakesMeToGardenOfSoul.MyFortFromMeTaken."
        "HereIsSomeoneHidden.LikeSugarInSugarCane.ASweetSugarTrader.MyShopFromMeTaken."
        "SorcererAndMagicia",
    };

    static const char *kInvalidNames[] = {
        "foo..bar",
        "..",
        "a..",
        "..b",

        // Long label
        "a.an-invalid-very-long-label-string-with-more-than-sixty-four-characters.com",

        // Long name (more than 255 characters)
        "HereIsSomeoneHidden.MyHoldFromMeTaken.FromSelfHasMeDriven.MyLeadFromMeTaken."
        "HereIsSomeoneHidden.AsLifeSweeterThanLife.TakesMeToGardenOfSoul.MyFortFromMeTaken."
        "HereIsSomeoneHidden.LikeSugarInSugarCane.ASweetSugarTrader.MyShopFromMeTaken."
        "SorcererAndMagician.NoEyesCanEverSee.AnArtfulConjurer.MySenseFromMeTaken."
        "MyEyesWillNeverSee.BeautiesOfTheWholeWorld.BeholdWhoseVisionFine.MySightFromMeTaken"
        "PoemByRumiMolana",

        // Long name of 255 characters which ends with a dot
        "HereIsSomeoneHidden.MyHoldFromMeTaken.FromSelfHasMeDriven.MyLeadFromMeTaken."
        "HereIsSomeoneHidden.AsLifeSweeterThanLife.TakesMeToGardenOfSoul.MyFortFromMeTaken."
        "HereIsSomeoneHidden.LikeSugarInSugarCane.ASweetSugarTrader.MyShopFromMeTaken."
        "SorcererAndMagician.",

        // Long name of 254 characters which does not end with a dot
        "HereIsSomeoneHidden.MyHoldFromMeTaken.FromSelfHasMeDriven.MyLeadFromMeTaken."
        "HereIsSomeoneHidden.AsLifeSweeterThanLife.TakesMeToGardenOfSoul.MyFortFromMeTaken."
        "HereIsSomeoneHidden.LikeSugarInSugarCane.ASweetSugarTrader.MyShopFromMeTaken."
        "SorcererAndMagician",

    };

    static const char kBadLabel[] = "badlabel";
    static const char kBadName[]  = "bad.name";

    printf("================================================================\n");
    printf("TestDnsName()\n");

    instance = static_cast<Instance *>(testInitInstance());
    VerifyOrQuit(instance != nullptr, "Null OpenThread instance");

    messagePool = &instance->Get<MessagePool>();
    VerifyOrQuit((message = messagePool->New(Message::kTypeIp6, 0)) != nullptr, "Message::New failed");

    message->SetOffset(0);

    printf("----------------------------------------------------------------\n");
    printf("Verify domain name match:\n");

    {
        const char *subDomain;
        const char *domain;

        subDomain = "my-service._ipps._tcp.local.";
        domain    = "local.";
        VerifyOrQuit(Dns::Name::IsSubDomainOf(subDomain, domain));

        subDomain = "my-service._ipps._tcp.local";
        domain    = "local.";
        VerifyOrQuit(Dns::Name::IsSubDomainOf(subDomain, domain));

        subDomain = "my-service._ipps._tcp.local.";
        domain    = "local";
        VerifyOrQuit(Dns::Name::IsSubDomainOf(subDomain, domain));

        subDomain = "my-service._ipps._tcp.local";
        domain    = "local";
        VerifyOrQuit(Dns::Name::IsSubDomainOf(subDomain, domain));

        subDomain = "my-service._ipps._tcp.default.service.arpa.";
        domain    = "default.service.arpa.";
        VerifyOrQuit(Dns::Name::IsSubDomainOf(subDomain, domain));

        subDomain = "my-service._ipps._tcp.default.service.arpa.";
        domain    = "service.arpa.";
        VerifyOrQuit(Dns::Name::IsSubDomainOf(subDomain, domain));

        // Verify it doesn't match a portion of a label.
        subDomain = "my-service._ipps._tcp.default.service.arpa.";
        domain    = "vice.arpa.";
        VerifyOrQuit(!Dns::Name::IsSubDomainOf(subDomain, domain));
    }

    printf("----------------------------------------------------------------\n");
    printf("Append names, check encoded bytes, parse name and read labels:\n");

    for (const TestName &test : kTestNames)
    {
        IgnoreError(message->SetLength(0));

        SuccessOrQuit(Dns::Name::AppendName(test.mName, *message));

        len = message->GetLength();
        SuccessOrQuit(message->Read(0, buffer, len));

        DumpBuffer(test.mName, buffer, len);

        VerifyOrQuit(len == test.mEncodedLength, "Encoded length does not match expected value");
        VerifyOrQuit(memcmp(buffer, test.mEncodedData, len) == 0, "Encoded name data does not match expected data");

        // Parse and skip over the name
        offset = 0;
        SuccessOrQuit(Dns::Name::ParseName(*message, offset));
        VerifyOrQuit(offset == len, "Name::ParseName() returned incorrect offset");

        // Read labels one by one.
        offset = 0;

        for (uint8_t index = 0; test.mLabels[index] != nullptr; index++)
        {
            labelLength = sizeof(label);
            SuccessOrQuit(Dns::Name::ReadLabel(*message, offset, label, labelLength));

            printf("Label[%d] = \"%s\"\n", index, label);

            VerifyOrQuit(strcmp(label, test.mLabels[index]) == 0, "Name::ReadLabel() did not get expected label");
            VerifyOrQuit(labelLength == strlen(label), "Name::ReadLabel() returned incorrect label length");
        }

        labelLength = sizeof(label);
        VerifyOrQuit(Dns::Name::ReadLabel(*message, offset, label, labelLength) == kErrorNotFound,
                     "Name::ReadLabel() failed at end of the name");

        // Read entire name
        offset = 0;
        SuccessOrQuit(Dns::Name::ReadName(*message, offset, name, sizeof(name)));

        printf("Read name =\"%s\"\n", name);

        VerifyOrQuit(strcmp(name, test.mExpectedReadName) == 0, "Name::ReadName() did not get expected name");
        VerifyOrQuit(offset == len, "Name::ReadName() returned incorrect offset");

        // Read entire name with different name buffer sizes (just right and one byte off the expected size)
        offset = 0;
        SuccessOrQuit(
            Dns::Name::ReadName(*message, offset, name, static_cast<uint16_t>(strlen(test.mExpectedReadName) + 1)),
            "Name::ReadName() failed with exact name buffer size");
        offset = 0;
        VerifyOrQuit(Dns::Name::ReadName(*message, offset, name,
                                         static_cast<uint16_t>(strlen(test.mExpectedReadName))) == kErrorNoBufs,
                     "Name::ReadName() did not fail with too small name buffer size");

        // Compare labels one by one.
        offset = 0;

        for (uint8_t index = 0; test.mLabels[index] != nullptr; index++)
        {
            uint16_t startOffset = offset;

            SuccessOrQuit(Dns::Name::CompareLabel(*message, offset, test.mLabels[index]));
            VerifyOrQuit(offset != startOffset, "Name::CompareLabel() did not change offset");

            VerifyOrQuit(Dns::Name::CompareLabel(*message, startOffset, kBadLabel) == kErrorNotFound,
                         "Name::CompareLabel() did not fail with incorrect label");
        }

        // Compare the whole name.
        offset = 0;
        SuccessOrQuit(Dns::Name::CompareName(*message, offset, test.mExpectedReadName));
        VerifyOrQuit(offset == len, "Name::CompareName() returned incorrect offset");

        offset = 0;
        VerifyOrQuit(Dns::Name::CompareName(*message, offset, kBadName) == kErrorNotFound,
                     "Name::CompareName() did not fail with incorrect name");
        VerifyOrQuit(offset == len, "Name::CompareName() returned incorrect offset");

        // Remove the terminating '.' in expected name and verify
        // that it can still be used by `CompareName()`.
        offset = 0;
        strcpy(name, test.mExpectedReadName);
        name[strlen(name) - 1] = '\0';
        SuccessOrQuit(Dns::Name::CompareName(*message, offset, name));
        VerifyOrQuit(offset == len, "Name::CompareName() returned incorrect offset");

        if (strlen(name) >= 1)
        {
            name[strlen(name) - 1] = '\0';
            offset                 = 0;
            VerifyOrQuit(Dns::Name::CompareName(*message, offset, name) == kErrorNotFound,
                         "Name::CompareName() did not fail with invalid name");
            VerifyOrQuit(offset == len, "Name::CompareName() returned incorrect offset");
        }

        // Compare the name with itself read from message.
        offset = 0;
        SuccessOrQuit(Dns::Name::CompareName(*message, offset, *message, offset));
        VerifyOrQuit(offset == len, "Name::CompareName() returned incorrect offset");
    }

    printf("----------------------------------------------------------------\n");
    printf("Max length names:\n");

    for (const char *&maxLengthName : kMaxLengthNames)
    {
        if (maxLengthName[strlen(maxLengthName) - 1] == '.')
        {
            VerifyOrQuit(strlen(maxLengthName) == kMaxNameLength);
        }
        else
        {
            VerifyOrQuit(strlen(maxLengthName) == kMaxNameLength - 1);
        }

        IgnoreError(message->SetLength(0));

        printf("\"%s\"\n", maxLengthName);

        SuccessOrQuit(Dns::Name::AppendName(maxLengthName, *message));
    }

    printf("----------------------------------------------------------------\n");
    printf("Invalid names:\n");

    for (const char *&invalidName : kInvalidNames)
    {
        IgnoreError(message->SetLength(0));

        printf("\"%s\"\n", invalidName);

        VerifyOrQuit(Dns::Name::AppendName(invalidName, *message) == kErrorInvalidArgs);
    }

    printf("----------------------------------------------------------------\n");
    printf("Append as multiple labels and terminator instead of full name:\n");

    for (const TestName &test : kTestNames)
    {
        IgnoreError(message->SetLength(0));

        SuccessOrQuit(Dns::Name::AppendMultipleLabels(test.mName, *message));
        SuccessOrQuit(Dns::Name::AppendTerminator(*message));

        len = message->GetLength();
        SuccessOrQuit(message->Read(0, buffer, len));

        DumpBuffer(test.mName, buffer, len);

        VerifyOrQuit(len == test.mEncodedLength, "Encoded length does not match expected value");
        VerifyOrQuit(memcmp(buffer, test.mEncodedData, len) == 0, "Encoded name data does not match expected data");
    }

    printf("----------------------------------------------------------------\n");
    printf("Append labels one by one:\n");

    for (const TestName &test : kTestNames)
    {
        IgnoreError(message->SetLength(0));

        for (uint8_t index = 0; test.mLabels[index] != nullptr; index++)
        {
            SuccessOrQuit(Dns::Name::AppendLabel(test.mLabels[index], *message));
        }

        SuccessOrQuit(Dns::Name::AppendTerminator(*message));

        len = message->GetLength();
        SuccessOrQuit(message->Read(0, buffer, len));

        DumpBuffer(test.mName, buffer, len);

        VerifyOrQuit(len == test.mEncodedLength, "Encoded length does not match expected value");
        VerifyOrQuit(memcmp(buffer, test.mEncodedData, len) == 0, "Encoded name data does not match expected data");
    }

    message->Free();
    testFreeInstance(instance);
}

void TestDnsCompressedName(void)
{
    enum
    {
        kHeaderOffset   = 10,
        kGuardBlockSize = 20,
        kMaxBufferSize  = 100,
        kLabelSize      = 64,
        kNameSize       = 256,

        kName2EncodedSize = 4 + 2,  // encoded "FOO" + pointer label (2 bytes)
        kName3EncodedSize = 2,      // pointer label (2 bytes)
        kName4EncodedSize = 15 + 2, // encoded "Human.Readable" + pointer label (2 bytes).

    };

    const char kName[]          = "F.ISI.ARPA";
    const char kLabel1[]        = "FOO";
    const char kInstanceLabel[] = "Human.Readable";

    static const uint8_t kEncodedName[]    = {1, 'F', 3, 'I', 'S', 'I', 4, 'A', 'R', 'P', 'A', 0};
    static const uint8_t kIsiRelativeIndex = 2; // Index in kEncodedName to the start of "ISI.ARPA" portion.

    static const char *kName1Labels[] = {"F", "ISI", "ARPA"};
    static const char *kName2Labels[] = {"FOO", "F", "ISI", "ARPA"};
    static const char *kName3Labels[] = {"ISI", "ARPA"};
    static const char *kName4Labels[] = {"Human.Readable", "F", "ISI", "ARPA"};

    static const char kExpectedReadName1[] = "F.ISI.ARPA.";
    static const char kExpectedReadName2[] = "FOO.F.ISI.ARPA.";
    static const char kExpectedReadName3[] = "ISI.ARPA.";

    static const char kBadName[] = "bad.name";

    Instance *   instance;
    MessagePool *messagePool;
    Message *    message;
    Message *    message2;
    uint16_t     offset;
    uint16_t     name1Offset;
    uint16_t     name2Offset;
    uint16_t     name3Offset;
    uint16_t     name4Offset;
    uint8_t      buffer[kMaxBufferSize];
    char         label[kLabelSize];
    uint8_t      labelLength;
    char         name[kNameSize];
    Dns::Name    dnsName1;
    Dns::Name    dnsName2;
    Dns::Name    dnsName3;
    Dns::Name    dnsName4;

    printf("================================================================\n");
    printf("TestDnsCompressedName()\n");

    instance = static_cast<Instance *>(testInitInstance());
    VerifyOrQuit(instance != nullptr, "Null OpenThread instance");

    messagePool = &instance->Get<MessagePool>();
    VerifyOrQuit((message = messagePool->New(Message::kTypeIp6, 0)) != nullptr);

    // Append name1 "F.ISI.ARPA"

    for (uint8_t index = 0; index < kHeaderOffset + kGuardBlockSize; index++)
    {
        SuccessOrQuit(message->Append(index));
    }

    message->SetOffset(kHeaderOffset);

    name1Offset = message->GetLength();
    SuccessOrQuit(Dns::Name::AppendName(kName, *message));

    // Append name2 "FOO.F.ISI.ARPA" as a compressed name after some guard/extra bytes

    for (uint8_t index = 0; index < kGuardBlockSize; index++)
    {
        uint8_t value = 0xff;
        SuccessOrQuit(message->Append(value));
    }

    name2Offset = message->GetLength();

    SuccessOrQuit(Dns::Name::AppendLabel(kLabel1, *message));
    SuccessOrQuit(Dns::Name::AppendPointerLabel(name1Offset - kHeaderOffset, *message));

    // Append name3 "ISI.ARPA" as a compressed name after some guard/extra bytes

    for (uint8_t index = 0; index < kGuardBlockSize; index++)
    {
        uint8_t value = 0xaa;
        SuccessOrQuit(message->Append(value));
    }

    name3Offset = message->GetLength();
    SuccessOrQuit(Dns::Name::AppendPointerLabel(name1Offset + kIsiRelativeIndex - kHeaderOffset, *message));

    name4Offset = message->GetLength();
    SuccessOrQuit(Dns::Name::AppendLabel(kInstanceLabel, *message));
    SuccessOrQuit(Dns::Name::AppendPointerLabel(name1Offset - kHeaderOffset, *message));

    printf("----------------------------------------------------------------\n");
    printf("Read and parse the uncompressed name-1 \"F.ISI.ARPA\"\n");

    SuccessOrQuit(message->Read(name1Offset, buffer, sizeof(kEncodedName)));

    DumpBuffer(kName, buffer, sizeof(kEncodedName));
    VerifyOrQuit(memcmp(buffer, kEncodedName, sizeof(kEncodedName)) == 0,
                 "Encoded name data does not match expected data");

    offset = name1Offset;
    SuccessOrQuit(Dns::Name::ParseName(*message, offset));

    VerifyOrQuit(offset == name1Offset + sizeof(kEncodedName), "Name::ParseName() returned incorrect offset");

    offset = name1Offset;

    for (const char *nameLabel : kName1Labels)
    {
        labelLength = sizeof(label);
        SuccessOrQuit(Dns::Name::ReadLabel(*message, offset, label, labelLength));

        printf("label: \"%s\"\n", label);
        VerifyOrQuit(strcmp(label, nameLabel) == 0, "Name::ReadLabel() did not get expected label");
        VerifyOrQuit(labelLength == strlen(label), "Name::ReadLabel() returned incorrect label length");
    }

    labelLength = sizeof(label);
    VerifyOrQuit(Dns::Name::ReadLabel(*message, offset, label, labelLength) == kErrorNotFound,
                 "Name::ReadLabel() failed at end of the name");

    offset = name1Offset;
    SuccessOrQuit(Dns::Name::ReadName(*message, offset, name, sizeof(name)));
    printf("Read name =\"%s\"\n", name);
    VerifyOrQuit(strcmp(name, kExpectedReadName1) == 0, "Name::ReadName() did not return expected name");
    VerifyOrQuit(offset == name1Offset + sizeof(kEncodedName), "Name::ReadName() returned incorrect offset");

    offset = name1Offset;

    for (const char *nameLabel : kName1Labels)
    {
        SuccessOrQuit(Dns::Name::CompareLabel(*message, offset, nameLabel));
    }

    offset = name1Offset;
    SuccessOrQuit(Dns::Name::CompareName(*message, offset, kExpectedReadName1));
    VerifyOrQuit(offset == name1Offset + sizeof(kEncodedName), "Name::CompareName() returned incorrect offset");

    offset = name1Offset;
    VerifyOrQuit(Dns::Name::CompareName(*message, offset, kBadName) == kErrorNotFound,
                 "Name::CompareName() did not fail with incorrect name");
    VerifyOrQuit(offset == name1Offset + sizeof(kEncodedName), "Name::CompareName() returned incorrect offset");

    offset = name1Offset;
    SuccessOrQuit(Dns::Name::CompareName(*message, offset, *message, offset));
    VerifyOrQuit(offset == name1Offset + sizeof(kEncodedName), "Name::CompareName() returned incorrect offset");

    offset = name1Offset;
    VerifyOrQuit(Dns::Name::CompareName(*message, offset, *message, name2Offset) == kErrorNotFound,
                 "Name::CompareName() did not fail with mismatching name");
    VerifyOrQuit(offset == name1Offset + sizeof(kEncodedName), "Name::CompareName() returned incorrect offset");

    printf("----------------------------------------------------------------\n");
    printf("Read and parse compressed name-2 \"FOO.F.ISI.ARPA\"\n");

    SuccessOrQuit(message->Read(name2Offset, buffer, kName2EncodedSize));
    DumpBuffer("name2(compressed)", buffer, kName2EncodedSize);

    offset = name2Offset;
    SuccessOrQuit(Dns::Name::ParseName(*message, offset));
    VerifyOrQuit(offset == name2Offset + kName2EncodedSize, "Name::ParseName() returned incorrect offset");

    offset = name2Offset;

    for (const char *nameLabel : kName2Labels)
    {
        labelLength = sizeof(label);
        SuccessOrQuit(Dns::Name::ReadLabel(*message, offset, label, labelLength));

        printf("label: \"%s\"\n", label);
        VerifyOrQuit(strcmp(label, nameLabel) == 0, "Name::ReadLabel() did not get expected label");
        VerifyOrQuit(labelLength == strlen(label), "Name::ReadLabel() returned incorrect label length");
    }

    labelLength = sizeof(label);
    VerifyOrQuit(Dns::Name::ReadLabel(*message, offset, label, labelLength) == kErrorNotFound,
                 "Name::ReadLabel() failed at end of the name");

    offset = name2Offset;
    SuccessOrQuit(Dns::Name::ReadName(*message, offset, name, sizeof(name)));
    printf("Read name =\"%s\"\n", name);
    VerifyOrQuit(strcmp(name, kExpectedReadName2) == 0, "Name::ReadName() did not return expected name");
    VerifyOrQuit(offset == name2Offset + kName2EncodedSize, "Name::ReadName() returned incorrect offset");

    offset = name2Offset;

    for (const char *nameLabel : kName2Labels)
    {
        SuccessOrQuit(Dns::Name::CompareLabel(*message, offset, nameLabel));
    }

    offset = name2Offset;
    SuccessOrQuit(Dns::Name::CompareName(*message, offset, kExpectedReadName2));
    VerifyOrQuit(offset == name2Offset + kName2EncodedSize, "Name::CompareName() returned incorrect offset");

    offset = name2Offset;
    VerifyOrQuit(Dns::Name::CompareName(*message, offset, kBadName) == kErrorNotFound,
                 "Name::CompareName() did not fail with incorrect name");
    VerifyOrQuit(offset == name2Offset + kName2EncodedSize, "Name::CompareName() returned incorrect offset");

    offset = name2Offset;
    SuccessOrQuit(Dns::Name::CompareName(*message, offset, *message, offset), "Name::CompareName() with itself failed");
    VerifyOrQuit(offset == name2Offset + kName2EncodedSize, "Name::CompareName() returned incorrect offset");

    offset = name2Offset;
    VerifyOrQuit(Dns::Name::CompareName(*message, offset, *message, name3Offset) == kErrorNotFound,
                 "Name::CompareName() did not fail with mismatching name");
    VerifyOrQuit(offset == name2Offset + kName2EncodedSize, "Name::CompareName() returned incorrect offset");

    printf("----------------------------------------------------------------\n");
    printf("Read and parse compressed name-3 \"ISI.ARPA\"\n");

    SuccessOrQuit(message->Read(name3Offset, buffer, kName3EncodedSize));
    DumpBuffer("name2(compressed)", buffer, kName3EncodedSize);

    offset = name3Offset;
    SuccessOrQuit(Dns::Name::ParseName(*message, offset));
    VerifyOrQuit(offset == name3Offset + kName3EncodedSize, "Name::ParseName() returned incorrect offset");

    offset = name3Offset;

    for (const char *nameLabel : kName3Labels)
    {
        labelLength = sizeof(label);
        SuccessOrQuit(Dns::Name::ReadLabel(*message, offset, label, labelLength));

        printf("label: \"%s\"\n", label);
        VerifyOrQuit(strcmp(label, nameLabel) == 0, "Name::ReadLabel() did not get expected label");
        VerifyOrQuit(labelLength == strlen(label), "Name::ReadLabel() returned incorrect label length");
    }

    labelLength = sizeof(label);
    VerifyOrQuit(Dns::Name::ReadLabel(*message, offset, label, labelLength) == kErrorNotFound,
                 "Name::ReadLabel() failed at end of the name");

    offset = name3Offset;
    SuccessOrQuit(Dns::Name::ReadName(*message, offset, name, sizeof(name)));
    printf("Read name =\"%s\"\n", name);
    VerifyOrQuit(strcmp(name, kExpectedReadName3) == 0, "Name::ReadName() did not return expected name");
    VerifyOrQuit(offset == name3Offset + kName3EncodedSize, "Name::ReadName() returned incorrect offset");

    offset = name3Offset;

    for (const char *nameLabel : kName3Labels)
    {
        SuccessOrQuit(Dns::Name::CompareLabel(*message, offset, nameLabel));
    }

    offset = name3Offset;
    SuccessOrQuit(Dns::Name::CompareName(*message, offset, kExpectedReadName3));
    VerifyOrQuit(offset == name3Offset + kName3EncodedSize, "Name::CompareName() returned incorrect offset");

    offset = name3Offset;
    VerifyOrQuit(Dns::Name::CompareName(*message, offset, kBadName) == kErrorNotFound,
                 "Name::CompareName() did not fail with incorrect name");
    VerifyOrQuit(offset == name3Offset + kName3EncodedSize, "Name::CompareName() returned incorrect offset");

    offset = name3Offset;
    SuccessOrQuit(Dns::Name::CompareName(*message, offset, *message, offset), "Name::CompareName() with itself failed");
    VerifyOrQuit(offset == name3Offset + kName3EncodedSize, "Name::CompareName() returned incorrect offset");

    offset = name3Offset;
    VerifyOrQuit(Dns::Name::CompareName(*message, offset, *message, name4Offset) == kErrorNotFound,
                 "Name::CompareName() did not fail with mismatching name");
    VerifyOrQuit(offset == name3Offset + kName3EncodedSize, "Name::CompareName() returned incorrect offset");

    printf("----------------------------------------------------------------\n");
    printf("Read and parse the uncompressed name-4 \"Human\\.Readable.F.ISI.ARPA\"\n");

    SuccessOrQuit(message->Read(name4Offset, buffer, kName4EncodedSize));
    DumpBuffer("name4(compressed)", buffer, kName4EncodedSize);

    offset = name4Offset;
    SuccessOrQuit(Dns::Name::ParseName(*message, offset));
    VerifyOrQuit(offset == name4Offset + kName4EncodedSize, "Name::ParseName() returned incorrect offset");

    offset = name4Offset;

    for (const char *nameLabel : kName4Labels)
    {
        labelLength = sizeof(label);
        SuccessOrQuit(Dns::Name::ReadLabel(*message, offset, label, labelLength));

        printf("label: \"%s\"\n", label);
        VerifyOrQuit(strcmp(label, nameLabel) == 0, "Name::ReadLabel() did not get expected label");
        VerifyOrQuit(labelLength == strlen(label), "Name::ReadLabel() returned incorrect label length");
    }

    // `ReadName()` for name-4 should fails due to first label containing dot char.
    offset = name4Offset;
    VerifyOrQuit(Dns::Name::ReadName(*message, offset, name, sizeof(name)) == kErrorParse,
                 "Name::ReadName() did not fail with invalid label");

    offset = name4Offset;

    for (const char *nameLabel : kName4Labels)
    {
        SuccessOrQuit(Dns::Name::CompareLabel(*message, offset, nameLabel));
    }

    offset = name4Offset;
    SuccessOrQuit(Dns::Name::CompareName(*message, offset, *message, offset), "Name::CompareName() with itself failed");

    offset = name4Offset;
    VerifyOrQuit(Dns::Name::CompareName(*message, offset, *message, name1Offset) == kErrorNotFound,
                 "Name::CompareName() did not fail with mismatching name");

    printf("----------------------------------------------------------------\n");
    printf("Append names from one message to another\n");

    VerifyOrQuit((message2 = messagePool->New(Message::kTypeIp6, 0)) != nullptr);

    dnsName1.SetFromMessage(*message, name1Offset);
    dnsName2.SetFromMessage(*message, name2Offset);
    dnsName3.SetFromMessage(*message, name3Offset);
    dnsName4.SetFromMessage(*message, name4Offset);

    offset = 0;
    SuccessOrQuit(dnsName1.AppendTo(*message2));
    SuccessOrQuit(dnsName2.AppendTo(*message2));
    SuccessOrQuit(dnsName3.AppendTo(*message2));
    SuccessOrQuit(dnsName4.AppendTo(*message2));

    SuccessOrQuit(message2->Read(0, buffer, message2->GetLength()));
    DumpBuffer("message2", buffer, message2->GetLength());

    // Now compare the names one by one in `message2`. Note that
    // `CompareName()` will update `offset` on success.

    SuccessOrQuit(Dns::Name::CompareName(*message2, offset, dnsName1));
    SuccessOrQuit(Dns::Name::CompareName(*message2, offset, dnsName2));
    SuccessOrQuit(Dns::Name::CompareName(*message2, offset, dnsName3));
    SuccessOrQuit(Dns::Name::CompareName(*message2, offset, dnsName4));

    offset = 0;
    SuccessOrQuit(Dns::Name::ReadName(*message2, offset, name, sizeof(name)));
    printf("- Name1 after `AppendTo()`: \"%s\"\n", name);
    SuccessOrQuit(Dns::Name::ReadName(*message2, offset, name, sizeof(name)));
    printf("- Name2 after `AppendTo()`: \"%s\"\n", name);
    SuccessOrQuit(Dns::Name::ReadName(*message2, offset, name, sizeof(name)));
    printf("- Name3 after `AppendTo()`: \"%s\"\n", name);
    // `ReadName()` for name-4 will fail due to first label containing dot char.

    message->Free();
    message2->Free();
    testFreeInstance(instance);
}

void TestHeaderAndResourceRecords(void)
{
    enum
    {
        kHeaderOffset    = 0,
        kQuestionCount   = 1,
        kAnswerCount     = 2,
        kAdditionalCount = 5,
        kTtl             = 7200,
        kTxtTtl          = 7300,
        kSrvPort         = 1234,
        kSrvPriority     = 1,
        kSrvWeight       = 2,
        kMaxSize         = 600,
    };

    const char    kMessageString[]  = "DnsMessage";
    const char    kDomainName[]     = "example.com.";
    const char    kServiceLabels[]  = "_service._udp";
    const char    kServiceName[]    = "_service._udp.example.com.";
    const char    kInstance1Label[] = "inst1";
    const char    kInstance2Label[] = "instance2";
    const char    kInstance1Name[]  = "inst1._service._udp.example.com.";
    const char    kInstance2Name[]  = "instance2._service._udp.example.com.";
    const char    kHostName[]       = "host.example.com.";
    const uint8_t kTxtData[]        = {9, 'k', 'e', 'y', '=', 'v', 'a', 'l', 'u', 'e', 0};
    const char    kHostAddress[]    = "fd00::abcd:";

    const char *kInstanceLabels[] = {kInstance1Label, kInstance2Label};
    const char *kInstanceNames[]  = {kInstance1Name, kInstance2Name};

    Instance *          instance;
    MessagePool *       messagePool;
    Message *           message;
    Dns::Header         header;
    uint16_t            messageId;
    uint16_t            headerOffset;
    uint16_t            offset;
    uint16_t            numRecords;
    uint16_t            len;
    uint16_t            serviceNameOffset;
    uint16_t            hostNameOffset;
    uint16_t            answerSectionOffset;
    uint16_t            additionalSectionOffset;
    uint16_t            index;
    Dns::PtrRecord      ptrRecord;
    Dns::SrvRecord      srvRecord;
    Dns::TxtRecord      txtRecord;
    Dns::AaaaRecord     aaaaRecord;
    Dns::ResourceRecord record;
    Ip6::Address        hostAddress;

    char    label[Dns::Name::kMaxLabelSize];
    char    name[Dns::Name::kMaxNameSize];
    uint8_t buffer[kMaxSize];

    printf("================================================================\n");
    printf("TestHeaderAndResourceRecords()\n");

    instance = static_cast<Instance *>(testInitInstance());
    VerifyOrQuit(instance != nullptr, "Null OpenThread instance");

    messagePool = &instance->Get<MessagePool>();
    VerifyOrQuit((message = messagePool->New(Message::kTypeIp6, 0)) != nullptr);

    printf("----------------------------------------------------------------\n");
    printf("Preparing the message\n");

    SuccessOrQuit(message->Append(kMessageString));

    // Header

    headerOffset = message->GetLength();
    SuccessOrQuit(header.SetRandomMessageId());
    messageId = header.GetMessageId();
    header.SetType(Dns::Header::kTypeResponse);
    header.SetQuestionCount(kQuestionCount);
    header.SetAnswerCount(kAnswerCount);
    header.SetAdditionalRecordCount(kAdditionalCount);
    SuccessOrQuit(message->Append(header));
    message->SetOffset(headerOffset);

    // Question section

    serviceNameOffset = message->GetLength() - headerOffset;
    SuccessOrQuit(Dns::Name::AppendMultipleLabels(kServiceLabels, *message));
    SuccessOrQuit(Dns::Name::AppendName(kDomainName, *message));
    SuccessOrQuit(message->Append(Dns::Question(Dns::ResourceRecord::kTypePtr)));

    // Answer section

    answerSectionOffset = message->GetLength();

    for (const char *instanceLabel : kInstanceLabels)
    {
        SuccessOrQuit(Dns::Name::AppendPointerLabel(serviceNameOffset, *message));
        ptrRecord.Init();
        ptrRecord.SetTtl(kTtl);
        offset = message->GetLength();
        SuccessOrQuit(message->Append(ptrRecord));
        SuccessOrQuit(Dns::Name::AppendLabel(instanceLabel, *message));
        SuccessOrQuit(Dns::Name::AppendPointerLabel(serviceNameOffset, *message));
        ptrRecord.SetLength(message->GetLength() - offset - sizeof(Dns::ResourceRecord));
        message->Write(offset, ptrRecord);
    }

    // Additional section

    additionalSectionOffset = message->GetLength();

    for (const char *instanceName : kInstanceNames)
    {
        uint16_t instanceNameOffset = message->GetLength() - headerOffset;

        // SRV record
        SuccessOrQuit(Dns::Name::AppendName(instanceName, *message));
        srvRecord.Init();
        srvRecord.SetTtl(kTtl);
        srvRecord.SetPort(kSrvPort);
        srvRecord.SetWeight(kSrvWeight);
        srvRecord.SetPriority(kSrvPriority);
        offset = message->GetLength();
        SuccessOrQuit(message->Append(srvRecord));
        hostNameOffset = message->GetLength() - headerOffset;
        SuccessOrQuit(Dns::Name::AppendName(kHostName, *message));
        srvRecord.SetLength(message->GetLength() - offset - sizeof(Dns::ResourceRecord));
        message->Write(offset, srvRecord);

        // TXT record
        SuccessOrQuit(Dns::Name::AppendPointerLabel(instanceNameOffset, *message));
        txtRecord.Init();
        txtRecord.SetTtl(kTxtTtl);
        txtRecord.SetLength(sizeof(kTxtData));
        SuccessOrQuit(message->Append(txtRecord));
        SuccessOrQuit(message->Append(kTxtData));
    }

    SuccessOrQuit(hostAddress.FromString(kHostAddress));
    SuccessOrQuit(Dns::Name::AppendPointerLabel(hostNameOffset, *message));
    aaaaRecord.Init();
    aaaaRecord.SetTtl(kTtl);
    aaaaRecord.SetAddress(hostAddress);
    SuccessOrQuit(message->Append(aaaaRecord));

    // Dump the entire message

    VerifyOrQuit(message->GetLength() < kMaxSize, "Message is too long");
    SuccessOrQuit(message->Read(0, buffer, message->GetLength()));
    DumpBuffer("message", buffer, message->GetLength());

    printf("----------------------------------------------------------------\n");
    printf("Parse and verify the message\n");

    offset = 0;
    VerifyOrQuit(message->Compare(offset, kMessageString), "Message header does not match");
    offset += sizeof(kMessageString);

    // Header

    VerifyOrQuit(offset == headerOffset, "headerOffset is incorrect");
    SuccessOrQuit(message->Read(offset, header));
    offset += sizeof(header);

    VerifyOrQuit(header.GetMessageId() == messageId);
    VerifyOrQuit(header.GetType() == Dns::Header::kTypeResponse);
    VerifyOrQuit(header.GetQuestionCount() == kQuestionCount);
    VerifyOrQuit(header.GetAnswerCount() == kAnswerCount);
    VerifyOrQuit(header.GetAdditionalRecordCount() == kAdditionalCount);

    printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
    printf("Question Section\n");

    SuccessOrQuit(Dns::Name::CompareName(*message, offset, kServiceName), "Question name does not match");
    VerifyOrQuit(message->Compare(offset, Dns::Question(Dns::ResourceRecord::kTypePtr)));
    offset += sizeof(Dns::Question);

    printf("PTR for \"%s\"\n", kServiceName);

    printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
    printf("Answer Section\n");

    VerifyOrQuit(offset == answerSectionOffset, "answer section offset is incorrect");

    for (const char *instanceName : kInstanceNames)
    {
        SuccessOrQuit(Dns::Name::CompareName(*message, offset, kServiceName));
        SuccessOrQuit(Dns::ResourceRecord::ReadRecord(*message, offset, ptrRecord));
        VerifyOrQuit(ptrRecord.GetTtl() == kTtl, "Read PTR is incorrect");

        SuccessOrQuit(ptrRecord.ReadPtrName(*message, offset, name, sizeof(name)));
        VerifyOrQuit(strcmp(name, instanceName) == 0, "Inst1 name is incorrect");

        printf("    \"%s\" PTR %u %d \"%s\"\n", kServiceName, ptrRecord.GetTtl(), ptrRecord.GetLength(), name);
    }

    VerifyOrQuit(offset == additionalSectionOffset, "offset is incorrect after answer section parse");

    offset = answerSectionOffset;
    SuccessOrQuit(Dns::ResourceRecord::ParseRecords(*message, offset, kAnswerCount));
    VerifyOrQuit(offset == additionalSectionOffset, "offset is incorrect after answer section parse");

    printf("Use FindRecord() to find and iterate through all the records:\n");

    offset     = answerSectionOffset;
    numRecords = kAnswerCount;

    while (numRecords > 0)
    {
        uint16_t prevNumRecords = numRecords;

        SuccessOrQuit(Dns::ResourceRecord::FindRecord(*message, offset, numRecords, Dns::Name(kServiceName)));
        VerifyOrQuit(numRecords == prevNumRecords - 1, "Incorrect num records");
        SuccessOrQuit(Dns::ResourceRecord::ReadRecord(*message, offset, ptrRecord));
        VerifyOrQuit(ptrRecord.GetTtl() == kTtl, "Read PTR is incorrect");
        SuccessOrQuit(ptrRecord.ReadPtrName(*message, offset, label, sizeof(label), name, sizeof(name)));
        printf("    \"%s\" PTR %u %d inst:\"%s\" at \"%s\"\n", kServiceName, ptrRecord.GetTtl(), ptrRecord.GetLength(),
               label, name);
    }

    VerifyOrQuit(offset == additionalSectionOffset, "offset is incorrect after answer section parse");
    VerifyOrQuit(Dns::ResourceRecord::FindRecord(*message, offset, numRecords, Dns::Name(kServiceName)) ==
                     kErrorNotFound,
                 "FindRecord did not fail with no records");

    // Use `ReadRecord()` with a non-matching record type. Verify that it correct skips over the record.

    offset     = answerSectionOffset;
    numRecords = kAnswerCount;

    while (numRecords > 0)
    {
        SuccessOrQuit(Dns::ResourceRecord::FindRecord(*message, offset, numRecords, Dns::Name(kServiceName)));
        VerifyOrQuit(Dns::ResourceRecord::ReadRecord(*message, offset, srvRecord) == kErrorNotFound,
                     "ReadRecord() did not fail with non-matching type");
    }

    VerifyOrQuit(offset == additionalSectionOffset, "offset is incorrect after answer section parse");

    // Use `FindRecord` with a non-matching name. Verify that it correctly skips over all records.

    offset     = answerSectionOffset;
    numRecords = kAnswerCount;
    VerifyOrQuit(Dns::ResourceRecord::FindRecord(*message, offset, numRecords, Dns::Name(kInstance1Name)) ==
                     kErrorNotFound,
                 "FindRecord did not fail with non-matching name");
    VerifyOrQuit(numRecords == 0, "Incorrect num records");
    VerifyOrQuit(offset == additionalSectionOffset, "offset is incorrect after answer section parse");

    printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
    printf("Additional Section\n");

    for (const char *instanceName : kInstanceNames)
    {
        // SRV record
        SuccessOrQuit(Dns::Name::CompareName(*message, offset, instanceName));
        SuccessOrQuit(Dns::ResourceRecord::ReadRecord(*message, offset, srvRecord));
        VerifyOrQuit(srvRecord.GetTtl() == kTtl);
        VerifyOrQuit(srvRecord.GetPort() == kSrvPort);
        VerifyOrQuit(srvRecord.GetWeight() == kSrvWeight);
        VerifyOrQuit(srvRecord.GetPriority() == kSrvPriority);
        SuccessOrQuit(srvRecord.ReadTargetHostName(*message, offset, name, sizeof(name)));
        VerifyOrQuit(strcmp(name, kHostName) == 0);
        printf("    \"%s\" SRV %u %d %d %d %d \"%s\"\n", instanceName, srvRecord.GetTtl(), srvRecord.GetLength(),
               srvRecord.GetPort(), srvRecord.GetWeight(), srvRecord.GetPriority(), name);

        // TXT record
        SuccessOrQuit(Dns::Name::CompareName(*message, offset, instanceName));
        SuccessOrQuit(Dns::ResourceRecord::ReadRecord(*message, offset, txtRecord));
        VerifyOrQuit(txtRecord.GetTtl() == kTxtTtl);
        len = sizeof(buffer);
        SuccessOrQuit(txtRecord.ReadTxtData(*message, offset, buffer, len));
        VerifyOrQuit(len == sizeof(kTxtData));
        VerifyOrQuit(memcmp(buffer, kTxtData, len) == 0);
        printf("    \"%s\" TXT %u %d \"%s\"\n", instanceName, txtRecord.GetTtl(), txtRecord.GetLength(),
               reinterpret_cast<const char *>(buffer));
    }

    SuccessOrQuit(Dns::Name::CompareName(*message, offset, kHostName));
    SuccessOrQuit(Dns::ResourceRecord::ReadRecord(*message, offset, aaaaRecord));
    VerifyOrQuit(aaaaRecord.GetTtl() == kTtl);
    VerifyOrQuit(aaaaRecord.GetAddress() == hostAddress);
    printf("    \"%s\" AAAA %u %d \"%s\"\n", kHostName, aaaaRecord.GetTtl(), aaaaRecord.GetLength(),
           aaaaRecord.GetAddress().ToString().AsCString());

    VerifyOrQuit(offset == message->GetLength(), "offset is incorrect after additional section parse");

    // Use `ParseRecords()` to parse all records
    offset = additionalSectionOffset;
    SuccessOrQuit(Dns::ResourceRecord::ParseRecords(*message, offset, kAdditionalCount));
    VerifyOrQuit(offset == message->GetLength(), "offset is incorrect after additional section parse");

    printf("Use FindRecord() to search for specific name:\n");

    for (const char *instanceName : kInstanceNames)
    {
        offset     = additionalSectionOffset;
        numRecords = kAdditionalCount;

        SuccessOrQuit(Dns::ResourceRecord::FindRecord(*message, offset, numRecords, Dns::Name(instanceName)));
        SuccessOrQuit(Dns::ResourceRecord::ReadRecord(*message, offset, srvRecord));
        SuccessOrQuit(Dns::Name::ParseName(*message, offset));
        printf("    \"%s\" SRV %u %d %d %d %d\n", instanceName, srvRecord.GetTtl(), srvRecord.GetLength(),
               srvRecord.GetPort(), srvRecord.GetWeight(), srvRecord.GetPriority());

        SuccessOrQuit(Dns::ResourceRecord::FindRecord(*message, offset, numRecords, Dns::Name(instanceName)));
        SuccessOrQuit(Dns::ResourceRecord::ReadRecord(*message, offset, txtRecord));
        offset += txtRecord.GetLength();
        printf("    \"%s\" TXT %u %d\n", instanceName, txtRecord.GetTtl(), txtRecord.GetLength());

        VerifyOrQuit(Dns::ResourceRecord::FindRecord(*message, offset, numRecords, Dns::Name(instanceName)) ==
                         kErrorNotFound,
                     "FindRecord() did not fail with no more records");

        VerifyOrQuit(offset == message->GetLength(), "offset is incorrect after additional section parse");
    }

    offset     = additionalSectionOffset;
    numRecords = kAdditionalCount;
    SuccessOrQuit(Dns::ResourceRecord::FindRecord(*message, offset, numRecords, Dns::Name(kHostName)));

    SuccessOrQuit(Dns::ResourceRecord::ReadRecord(*message, offset, record));
    VerifyOrQuit(record.GetType() == Dns::ResourceRecord::kTypeAaaa);
    offset += record.GetLength();
    VerifyOrQuit(offset == message->GetLength(), "offset is incorrect after additional section parse");

    printf("Use FindRecord() to search for specific records:\n");
    printf(" Answer Section\n");

    for (index = 0; index < OT_ARRAY_LENGTH(kInstanceNames); index++)
    {
        offset = answerSectionOffset;
        SuccessOrQuit(
            Dns::ResourceRecord::FindRecord(*message, offset, kAnswerCount, index, Dns::Name(kServiceName), ptrRecord));

        printf("   index:%d -> \"%s\" PTR %u %d\n", index, kServiceName, ptrRecord.GetTtl(), ptrRecord.GetLength());
    }

    // Check `FindRecord()` failure with non-matching name, record type, or bad index.

    offset = answerSectionOffset;
    VerifyOrQuit(Dns::ResourceRecord::FindRecord(*message, offset, kAnswerCount, index, Dns::Name(kServiceName),
                                                 ptrRecord) == kErrorNotFound,
                 "FindRecord() did not fail with bad index");
    VerifyOrQuit(offset == answerSectionOffset, "FindRecord() changed offset on failure");

    offset = answerSectionOffset;
    VerifyOrQuit(Dns::ResourceRecord::FindRecord(*message, offset, kAnswerCount, index, Dns::Name(kInstance1Name),
                                                 ptrRecord) == kErrorNotFound,
                 "FindRecord() did not fail with bad index");
    VerifyOrQuit(offset == answerSectionOffset, "FindRecord() changed offset on failure");

    offset = answerSectionOffset;
    VerifyOrQuit(Dns::ResourceRecord::FindRecord(*message, offset, kAnswerCount, index, Dns::Name(kServiceName),
                                                 txtRecord) == kErrorNotFound,
                 "FindRecord() did not fail with bad index");
    VerifyOrQuit(offset == answerSectionOffset, "FindRecord() changed offset on failure");

    printf(" Additional Section\n");

    for (const char *instanceName : kInstanceNames)
    {
        // There is a single SRV and TXT entry for each instance
        offset = additionalSectionOffset;
        SuccessOrQuit(Dns::ResourceRecord::FindRecord(*message, offset, kAdditionalCount, /* aIndex */ 0,
                                                      Dns::Name(instanceName), srvRecord));
        printf("    \"%s\" SRV %u %d %d %d %d \n", instanceName, srvRecord.GetTtl(), srvRecord.GetLength(),
               srvRecord.GetPort(), srvRecord.GetWeight(), srvRecord.GetPriority());

        offset = additionalSectionOffset;
        SuccessOrQuit(Dns::ResourceRecord::FindRecord(*message, offset, kAdditionalCount, /* aIndex */ 0,
                                                      Dns::Name(instanceName), txtRecord));
        printf("    \"%s\" TXT %u %d\n", instanceName, txtRecord.GetTtl(), txtRecord.GetLength());

        offset = additionalSectionOffset;
        VerifyOrQuit(Dns::ResourceRecord::FindRecord(*message, offset, kAdditionalCount, /* aIndex */ 1,
                                                     Dns::Name(instanceName), srvRecord) == kErrorNotFound);

        offset = additionalSectionOffset;
        VerifyOrQuit(Dns::ResourceRecord::FindRecord(*message, offset, kAdditionalCount, /* aIndex */ 1,
                                                     Dns::Name(instanceName), txtRecord) == kErrorNotFound);
    }

    for (index = 0; index < kAdditionalCount; index++)
    {
        offset = additionalSectionOffset;
        // Find record with empty name (matching any) and any type.
        SuccessOrQuit(Dns::ResourceRecord::FindRecord(*message, offset, kAdditionalCount, index, Dns::Name(), record));
    }

    offset = additionalSectionOffset;
    VerifyOrQuit(Dns::ResourceRecord::FindRecord(*message, offset, kAdditionalCount, index, Dns::Name(), record) ==
                 kErrorNotFound);

    message->Free();
    testFreeInstance(instance);
}

void TestDnsTxtEntry(void)
{
    enum
    {
        kMaxTxtDataSize = 255,
    };

    struct EncodedTxtData
    {
        const uint8_t *mData;
        uint8_t        mLength;
    };

    const char    kKey1[]   = "key";
    const uint8_t kValue1[] = {'v', 'a', 'l', 'u', 'e'};

    const char    kKey2[]   = "E";
    const uint8_t kValue2[] = {'m', 'c', '^', '2'};

    const char    kKey3[]   = "space key";
    const uint8_t kValue3[] = {'=', 0, '='};

    const char    kKey4[]   = "123456789"; // Max recommended length key
    const uint8_t kValue4[] = {0};

    const char    kKey5[]   = "1234567890"; // Longer than recommended key
    const uint8_t kValue5[] = {'a'};

    const char kKey6[] = "boolKey";  // Should be encoded as "boolKey" (without `=`).
    const char kKey7[] = "emptyKey"; // Should be encoded as "emptyKey=".

    // Invalid key
    const char kShortKey[] = "";

    const uint8_t kEncodedTxt1[] = {9, 'k', 'e', 'y', '=', 'v', 'a', 'l', 'u', 'e'};
    const uint8_t kEncodedTxt2[] = {6, 'E', '=', 'm', 'c', '^', '2'};
    const uint8_t kEncodedTxt3[] = {13, 's', 'p', 'a', 'c', 'e', ' ', 'k', 'e', 'y', '=', '=', 0, '='};
    const uint8_t kEncodedTxt4[] = {11, '1', '2', '3', '4', '5', '6', '7', '8', '9', '=', 0};
    const uint8_t kEncodedTxt5[] = {12, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '=', 'a'};
    const uint8_t kEncodedTxt6[] = {7, 'b', 'o', 'o', 'l', 'K', 'e', 'y'};
    const uint8_t kEncodedTxt7[] = {9, 'e', 'm', 'p', 't', 'y', 'K', 'e', 'y', '='};

    const uint8_t kInvalidEncodedTxt1[] = {4, 'a', '=', 'b'}; // Incorrect length

    // Special encoded txt data with zero strings and string starting
    // with '=' (missing key) whcih should be skipped over silently.
    const uint8_t kSpecialEncodedTxt[] = {0, 0, 3, 'A', '=', 'B', 2, '=', 'C', 3, 'D', '=', 'E', 3, '=', '1', '2'};

    const Dns::TxtEntry kTxtEntries[] = {
        Dns::TxtEntry(kKey1, kValue1, sizeof(kValue1)),
        Dns::TxtEntry(kKey2, kValue2, sizeof(kValue2)),
        Dns::TxtEntry(kKey3, kValue3, sizeof(kValue3)),
        Dns::TxtEntry(kKey4, kValue4, sizeof(kValue4)),
        Dns::TxtEntry(kKey5, kValue5, sizeof(kValue5)),
        Dns::TxtEntry(kKey6, nullptr, 0),
        Dns::TxtEntry(kKey7, kValue1, 0),
    };

    const EncodedTxtData kEncodedTxtData[] = {
        {kEncodedTxt1, sizeof(kEncodedTxt1)}, {kEncodedTxt2, sizeof(kEncodedTxt2)},
        {kEncodedTxt3, sizeof(kEncodedTxt3)}, {kEncodedTxt4, sizeof(kEncodedTxt4)},
        {kEncodedTxt5, sizeof(kEncodedTxt5)}, {kEncodedTxt6, sizeof(kEncodedTxt6)},
        {kEncodedTxt7, sizeof(kEncodedTxt7)}};

    Instance *              instance;
    MessagePool *           messagePool;
    Message *               message;
    uint8_t                 txtData[kMaxTxtDataSize];
    uint16_t                txtDataLength;
    uint8_t                 index;
    Dns::TxtEntry           txtEntry;
    Dns::TxtEntry::Iterator iterator;

    printf("================================================================\n");
    printf("TestDnsTxtEntry()\n");

    instance = static_cast<Instance *>(testInitInstance());
    VerifyOrQuit(instance != nullptr);

    messagePool = &instance->Get<MessagePool>();
    VerifyOrQuit((message = messagePool->New(Message::kTypeIp6, 0)) != nullptr);

    SuccessOrQuit(Dns::TxtEntry::AppendEntries(kTxtEntries, OT_ARRAY_LENGTH(kTxtEntries), *message));

    txtDataLength = message->GetLength();
    VerifyOrQuit(txtDataLength < kMaxTxtDataSize, "TXT data is too long");

    SuccessOrQuit(message->Read(0, txtData, txtDataLength));
    DumpBuffer("txt data", txtData, txtDataLength);

    index = 0;
    for (const EncodedTxtData &encodedData : kEncodedTxtData)
    {
        VerifyOrQuit(memcmp(&txtData[index], encodedData.mData, encodedData.mLength) == 0);
        index += encodedData.mLength;
    }

    iterator.Init(txtData, txtDataLength);

    for (const Dns::TxtEntry &expectedTxtEntry : kTxtEntries)
    {
        uint8_t expectedKeyLength = static_cast<uint8_t>(strlen(expectedTxtEntry.mKey));

        SuccessOrQuit(iterator.GetNextEntry(txtEntry), "TxtEntry::GetNextEntry() failed");
        printf("key:\"%s\" valueLen:%d\n", txtEntry.mKey != nullptr ? txtEntry.mKey : "(null)", txtEntry.mValueLength);

        if (expectedKeyLength > Dns::TxtEntry::kMaxKeyLength)
        {
            // When the key is longer than recommended max key length,
            // the full encoded string is returned in `mValue` and
            // `mValueLength` and `mKey` should be set to  `nullptr`.

            VerifyOrQuit(txtEntry.mKey == nullptr, "TxtEntry key does not match expected value for long key");
            VerifyOrQuit(txtEntry.mValueLength == expectedKeyLength + expectedTxtEntry.mValueLength + sizeof(char),
                         "TxtEntry value length is incorrect for long key");
            VerifyOrQuit(memcmp(txtEntry.mValue, expectedTxtEntry.mKey, expectedKeyLength) == 0);
            VerifyOrQuit(txtEntry.mValue[expectedKeyLength] == static_cast<uint8_t>('='));
            VerifyOrQuit(memcmp(&txtEntry.mValue[expectedKeyLength + sizeof(uint8_t)], expectedTxtEntry.mValue,
                                expectedTxtEntry.mValueLength) == 0);
            continue;
        }

        VerifyOrQuit(strcmp(txtEntry.mKey, expectedTxtEntry.mKey) == 0);
        VerifyOrQuit(txtEntry.mValueLength == expectedTxtEntry.mValueLength);

        if (txtEntry.mValueLength != 0)
        {
            VerifyOrQuit(memcmp(txtEntry.mValue, expectedTxtEntry.mValue, txtEntry.mValueLength) == 0);
        }
        else
        {
            // Ensure both `txtEntry.mKey` and `expectedTxtEntry.mKey` are
            // null or both are non-null (for boolean or empty keys).
            VerifyOrQuit((txtEntry.mKey == nullptr) == (expectedTxtEntry.mKey == nullptr),
                         "TxtEntry value does not match expected value for bool or empty key");
        }
    }

    VerifyOrQuit(iterator.GetNextEntry(txtEntry) == kErrorNotFound, "GetNextEntry() returned unexpected entry");
    VerifyOrQuit(iterator.GetNextEntry(txtEntry) == kErrorNotFound, "GetNextEntry() succeeded after done");

    // Verify `AppendEntries()` correctly rejecting invalid key
    txtEntry.mValue       = kValue1;
    txtEntry.mValueLength = sizeof(kValue1);
    txtEntry.mKey         = kShortKey;
    VerifyOrQuit(Dns::TxtEntry::AppendEntries(&txtEntry, 1, *message) == kErrorInvalidArgs,
                 "AppendEntries() did not fail with invalid key");

    // Verify appending empty txt data

    SuccessOrQuit(message->SetLength(0));
    SuccessOrQuit(Dns::TxtEntry::AppendEntries(nullptr, 0, *message), "AppendEntries() failed with empty array");
    txtDataLength = message->GetLength();
    VerifyOrQuit(txtDataLength == sizeof(uint8_t), "Data length is incorrect with empty array");
    SuccessOrQuit(message->Read(0, txtData, txtDataLength));
    VerifyOrQuit(txtData[0] == 0, "Data is invalid with empty array");

    SuccessOrQuit(message->SetLength(0));
    txtEntry.mKey         = nullptr;
    txtEntry.mValue       = nullptr;
    txtEntry.mValueLength = 0;
    SuccessOrQuit(Dns::TxtEntry::AppendEntries(&txtEntry, 1, *message), "AppendEntries() failed with empty entry");
    txtDataLength = message->GetLength();
    VerifyOrQuit(txtDataLength == sizeof(uint8_t), "Data length is incorrect with empty entry");
    SuccessOrQuit(message->Read(0, txtData, txtDataLength), "Failed to read txt data from message");
    VerifyOrQuit(txtData[0] == 0, "Data is invalid with empty entry");

    // Verify `Iterator` behavior with invalid txt data.

    iterator.Init(kInvalidEncodedTxt1, sizeof(kInvalidEncodedTxt1));
    VerifyOrQuit(iterator.GetNextEntry(txtEntry) == kErrorParse, "GetNextEntry() did not fail with invalid data");

    // Verify `GetNextEntry()` correctly skipping over empty strings and
    // strings starting with '=' (missing key) in encoded txt.
    //
    // kSpecialEncodedTxt:
    // { 0, 3, 'A', '=', 'B', 2, '=', 'C', 3, 'D', '=', 'E', 3, '=', '1', '2' }

    iterator.Init(kSpecialEncodedTxt, sizeof(kSpecialEncodedTxt));

    // We should get "A=B` (or key="A", and value="B")
    SuccessOrQuit(iterator.GetNextEntry(txtEntry), "GetNextEntry() failed");
    VerifyOrQuit((txtEntry.mKey[0] == 'A') && (txtEntry.mKey[1] == '\0'), "GetNextEntry() got incorrect key");
    VerifyOrQuit((txtEntry.mValueLength == 1) && (txtEntry.mValue[0] == 'B'), "GetNextEntry() got incorrect value");

    // We should get "D=E` (or key="D", and value="E")
    SuccessOrQuit(iterator.GetNextEntry(txtEntry), "GetNextEntry() failed");
    VerifyOrQuit((txtEntry.mKey[0] == 'D') && (txtEntry.mKey[1] == '\0'), "GetNextEntry() got incorrect key");
    VerifyOrQuit((txtEntry.mValueLength == 1) && (txtEntry.mValue[0] == 'E'), "GetNextEntry() got incorrect value");

    VerifyOrQuit(iterator.GetNextEntry(txtEntry) == kErrorNotFound, "GetNextEntry() returned extra entry");

    message->Free();
    testFreeInstance(instance);
}

} // namespace ot

int main(void)
{
    ot::TestDnsName();
    ot::TestDnsCompressedName();
    ot::TestHeaderAndResourceRecords();
    ot::TestDnsTxtEntry();

    printf("All tests passed\n");
    return 0;
}
