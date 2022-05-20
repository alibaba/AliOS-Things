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
 *   This file implements IPv6 addresses.
 */

#include "ip6_address.hpp"

#include <stdio.h>

#include "common/code_utils.hpp"
#include "common/encoding.hpp"
#include "common/instance.hpp"
#include "common/numeric_limits.hpp"
#include "common/random.hpp"
#include "net/ip4_address.hpp"
#include "net/netif.hpp"

using ot::Encoding::BigEndian::HostSwap32;

namespace ot {
namespace Ip6 {

//---------------------------------------------------------------------------------------------------------------------
// NetworkPrefix methods

Error NetworkPrefix::GenerateRandomUla(void)
{
    m8[0] = 0xfd;

    return Random::Crypto::FillBuffer(&m8[1], kSize - 1);
}

//---------------------------------------------------------------------------------------------------------------------
// Prefix methods

void Prefix::Set(const uint8_t *aPrefix, uint8_t aLength)
{
    memcpy(mPrefix.mFields.m8, aPrefix, SizeForLength(aLength));
    mLength = aLength;
}

bool Prefix::IsEqual(const uint8_t *aPrefixBytes, uint8_t aPrefixLength) const
{
    return (mLength == aPrefixLength) && (MatchLength(GetBytes(), aPrefixBytes, GetBytesSize()) >= mLength);
}

bool Prefix::operator<(const Prefix &aOther) const
{
    bool    isSmaller;
    uint8_t matchedLength;

    VerifyOrExit(GetLength() == aOther.GetLength(), isSmaller = GetLength() < aOther.GetLength());

    matchedLength = MatchLength(GetBytes(), aOther.GetBytes(), GetBytesSize());

    VerifyOrExit(matchedLength < GetLength(), isSmaller = false);

    isSmaller = GetBytes()[matchedLength / CHAR_BIT] < aOther.GetBytes()[matchedLength / CHAR_BIT];

exit:
    return isSmaller;
}

uint8_t Prefix::MatchLength(const uint8_t *aPrefixA, const uint8_t *aPrefixB, uint8_t aMaxSize)
{
    uint8_t matchedLength = 0;

    OT_ASSERT(aMaxSize <= Address::kSize);

    for (uint8_t i = 0; i < aMaxSize; i++)
    {
        uint8_t diff = aPrefixA[i] ^ aPrefixB[i];

        if (diff == 0)
        {
            matchedLength += CHAR_BIT;
        }
        else
        {
            while ((diff & 0x80) == 0)
            {
                matchedLength++;
                diff <<= 1;
            }

            break;
        }
    }

    return matchedLength;
}

bool Prefix::IsValidNat64(void) const
{
    return (mLength == 32) || (mLength == 40) || (mLength == 48) || (mLength == 56) || (mLength == 64) ||
           (mLength == 96);
}

Prefix::InfoString Prefix::ToString(void) const
{
    InfoString string;

    ToString(string);

    return string;
}

void Prefix::ToString(char *aBuffer, uint16_t aSize) const
{
    StringWriter writer(aBuffer, aSize);

    ToString(writer);
}

void Prefix::ToString(StringWriter &aWriter) const
{
    uint8_t sizeInUint16 = (GetBytesSize() + sizeof(uint16_t) - 1) / sizeof(uint16_t);

    static_cast<const Address &>(mPrefix).AppendHexWords(aWriter, sizeInUint16);

    if (GetBytesSize() < Address::kSize - 1)
    {
        aWriter.Append("::");
    }

    aWriter.Append("/%d", mLength);
}

//---------------------------------------------------------------------------------------------------------------------
// InterfaceIdentifier methods

bool InterfaceIdentifier::IsUnspecified(void) const
{
    return (mFields.m32[0] == 0) && (mFields.m32[1] == 0);
}

bool InterfaceIdentifier::IsReserved(void) const
{
    return IsSubnetRouterAnycast() || IsReservedSubnetAnycast() || IsAnycastLocator();
}

bool InterfaceIdentifier::IsSubnetRouterAnycast(void) const
{
    return (mFields.m32[0] == 0) && (mFields.m32[1] == 0);
}

bool InterfaceIdentifier::IsReservedSubnetAnycast(void) const
{
    // Format of IID in a Reserved Subnet Anycast Address (RFC 2526)
    //
    // |      57 bits     |   7 bits   |
    // +------------------+------------+
    // | 1111110111...111 | anycast ID |
    // +------------------+------------+

    return (mFields.m32[0] == HostSwap32(0xfdffffff) && mFields.m16[2] == HostSwap16(0xffff) && mFields.m8[6] == 0xff &&
            mFields.m8[7] >= 0x80);
}

void InterfaceIdentifier::GenerateRandom(void)
{
    Error error;

    OT_UNUSED_VARIABLE(error);

    error = Random::Crypto::FillBuffer(mFields.m8, kSize);

    OT_ASSERT(error == kErrorNone);
}

void InterfaceIdentifier::SetBytes(const uint8_t *aBuffer)
{
    memcpy(mFields.m8, aBuffer, kSize);
}

void InterfaceIdentifier::SetFromExtAddress(const Mac::ExtAddress &aExtAddress)
{
    Mac::ExtAddress addr;

    addr = aExtAddress;
    addr.ToggleLocal();
    addr.CopyTo(mFields.m8);
}

void InterfaceIdentifier::ConvertToExtAddress(Mac::ExtAddress &aExtAddress) const
{
    aExtAddress.Set(mFields.m8);
    aExtAddress.ToggleLocal();
}

void InterfaceIdentifier::ConvertToMacAddress(Mac::Address &aMacAddress) const
{
    aMacAddress.SetExtended(mFields.m8);
    aMacAddress.GetExtended().ToggleLocal();
}

void InterfaceIdentifier::SetToLocator(uint16_t aLocator)
{
    // Locator IID pattern `0000:00ff:fe00:xxxx`
    mFields.m32[0] = HostSwap32(0x000000ff);
    mFields.m16[2] = HostSwap16(0xfe00);
    mFields.m16[3] = HostSwap16(aLocator);
}

bool InterfaceIdentifier::IsLocator(void) const
{
    // Locator IID pattern 0000:00ff:fe00:xxxx
    return (mFields.m32[0] == HostSwap32(0x000000ff) && mFields.m16[2] == HostSwap16(0xfe00));
}

bool InterfaceIdentifier::IsRoutingLocator(void) const
{
    return (IsLocator() && (mFields.m8[6] < kAloc16Mask) && ((mFields.m8[6] & kRloc16ReservedBitMask) == 0));
}

bool InterfaceIdentifier::IsAnycastLocator(void) const
{
    // Anycast locator range 0xfc00- 0xfcff (`kAloc16Mask` is 0xfc)
    return (IsLocator() && (mFields.m8[6] == kAloc16Mask));
}

bool InterfaceIdentifier::IsAnycastServiceLocator(void) const
{
    uint16_t locator = GetLocator();

    return (IsLocator() && (locator >= Mle::kAloc16ServiceStart) && (locator <= Mle::kAloc16ServiceEnd));
}

InterfaceIdentifier::InfoString InterfaceIdentifier::ToString(void) const
{
    InfoString string;

    string.AppendHexBytes(mFields.m8, kSize);

    return string;
}

//---------------------------------------------------------------------------------------------------------------------
// Address methods

bool Address::IsUnspecified(void) const
{
    return (mFields.m32[0] == 0 && mFields.m32[1] == 0 && mFields.m32[2] == 0 && mFields.m32[3] == 0);
}

bool Address::IsLoopback(void) const
{
    return (mFields.m32[0] == 0 && mFields.m32[1] == 0 && mFields.m32[2] == 0 && mFields.m32[3] == HostSwap32(1));
}

bool Address::IsLinkLocal(void) const
{
    return (mFields.m16[0] & HostSwap16(0xffc0)) == HostSwap16(0xfe80);
}

void Address::SetToLinkLocalAddress(const Mac::ExtAddress &aExtAddress)
{
    mFields.m32[0] = HostSwap32(0xfe800000);
    mFields.m32[1] = 0;
    GetIid().SetFromExtAddress(aExtAddress);
}

void Address::SetToLinkLocalAddress(const InterfaceIdentifier &aIid)
{
    mFields.m32[0] = HostSwap32(0xfe800000);
    mFields.m32[1] = 0;
    SetIid(aIid);
}

bool Address::IsLinkLocalMulticast(void) const
{
    return IsMulticast() && (GetScope() == kLinkLocalScope);
}

bool Address::IsLinkLocalAllNodesMulticast(void) const
{
    return (*this == GetLinkLocalAllNodesMulticast());
}

void Address::SetToLinkLocalAllNodesMulticast(void)
{
    *this = GetLinkLocalAllNodesMulticast();
}

bool Address::IsLinkLocalAllRoutersMulticast(void) const
{
    return (*this == GetLinkLocalAllRoutersMulticast());
}

void Address::SetToLinkLocalAllRoutersMulticast(void)
{
    *this = GetLinkLocalAllRoutersMulticast();
}

bool Address::IsRealmLocalMulticast(void) const
{
    return IsMulticast() && (GetScope() == kRealmLocalScope);
}

bool Address::IsMulticastLargerThanRealmLocal(void) const
{
    return IsMulticast() && (GetScope() > kRealmLocalScope);
}

bool Address::IsRealmLocalAllNodesMulticast(void) const
{
    return (*this == GetRealmLocalAllNodesMulticast());
}

void Address::SetToRealmLocalAllNodesMulticast(void)
{
    *this = GetRealmLocalAllNodesMulticast();
}

bool Address::IsRealmLocalAllRoutersMulticast(void) const
{
    return (*this == GetRealmLocalAllRoutersMulticast());
}

void Address::SetToRealmLocalAllRoutersMulticast(void)
{
    *this = GetRealmLocalAllRoutersMulticast();
}

bool Address::IsRealmLocalAllMplForwarders(void) const
{
    return (*this == GetRealmLocalAllMplForwarders());
}

void Address::SetToRealmLocalAllMplForwarders(void)
{
    *this = GetRealmLocalAllMplForwarders();
}

bool Address::MatchesPrefix(const Prefix &aPrefix) const
{
    return Prefix::MatchLength(mFields.m8, aPrefix.GetBytes(), aPrefix.GetBytesSize()) >= aPrefix.GetLength();
}

bool Address::MatchesPrefix(const uint8_t *aPrefix, uint8_t aPrefixLength) const
{
    return Prefix::MatchLength(mFields.m8, aPrefix, Prefix::SizeForLength(aPrefixLength)) >= aPrefixLength;
}

void Address::SetPrefix(const NetworkPrefix &aNetworkPrefix)
{
    mFields.mComponents.mNetworkPrefix = aNetworkPrefix;
}

void Address::SetPrefix(const Prefix &aPrefix)
{
    SetPrefix(0, aPrefix.GetBytes(), aPrefix.GetLength());
}

void Address::SetPrefix(uint8_t aOffset, const uint8_t *aPrefix, uint8_t aPrefixLength)
{
    uint8_t bytes     = aPrefixLength / CHAR_BIT;
    uint8_t extraBits = aPrefixLength % CHAR_BIT;

    OT_ASSERT(aPrefixLength <= (sizeof(Address) - aOffset) * CHAR_BIT);

    memcpy(mFields.m8 + aOffset, aPrefix, bytes);

    if (extraBits > 0)
    {
        uint8_t index = aOffset + bytes;
        uint8_t mask  = ((0x80 >> (extraBits - 1)) - 1);

        // `mask` has its higher (msb) `extraBits` bits as `0` and the remaining as `1`.
        // Example with `extraBits` = 3:
        // ((0x80 >> 2) - 1) = (0b0010_0000 - 1) = 0b0001_1111

        mFields.m8[index] &= mask;
        mFields.m8[index] |= (aPrefix[index] & ~mask);
    }
}

void Address::SetMulticastNetworkPrefix(const uint8_t *aPrefix, uint8_t aPrefixLength)
{
    SetPrefix(kMulticastNetworkPrefixOffset, aPrefix, aPrefixLength);
    mFields.m8[kMulticastNetworkPrefixLengthOffset] = aPrefixLength;
}

void Address::SetToLocator(const NetworkPrefix &aNetworkPrefix, uint16_t aLocator)
{
    SetPrefix(aNetworkPrefix);
    GetIid().SetToLocator(aLocator);
}

uint8_t Address::GetScope(void) const
{
    uint8_t rval;

    if (IsMulticast())
    {
        rval = mFields.m8[1] & 0xf;
    }
    else if (IsLinkLocal())
    {
        rval = kLinkLocalScope;
    }
    else if (IsLoopback())
    {
        rval = kNodeLocalScope;
    }
    else
    {
        rval = kGlobalScope;
    }

    return rval;
}

uint8_t Address::PrefixMatch(const Address &aOther) const
{
    return Prefix::MatchLength(mFields.m8, aOther.mFields.m8, sizeof(Address));
}

bool Address::MatchesFilter(TypeFilter aFilter) const
{
    bool matches = true;

    switch (aFilter)
    {
    case kTypeAny:
        break;

    case kTypeUnicast:
        matches = !IsUnspecified() && !IsMulticast();
        break;

    case kTypeMulticast:
        matches = IsMulticast();
        break;

    case kTypeMulticastLargerThanRealmLocal:
        matches = IsMulticastLargerThanRealmLocal();
        break;
    }

    return matches;
}

void Address::SynthesizeFromIp4Address(const Prefix &aPrefix, const Ip4::Address &aIp4Address)
{
    // The prefix length must be 32, 40, 48, 56, 64, 96. IPv4 bytes are added
    // after the prefix, skipping over the bits 64 to 71 (byte at `kSkipIndex`)
    // which must be set to zero. The suffix is set to zero (per RFC 6502).
    //
    //    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    //    |PL| 0-------------32--40--48--56--64--72--80--88--96--104---------|
    //    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    //    |32|     prefix    |v4(32)         | u | suffix                    |
    //    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    //    |40|     prefix        |v4(24)     | u |(8)| suffix                |
    //    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    //    |48|     prefix            |v4(16) | u | (16)  | suffix            |
    //    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    //    |56|     prefix                |(8)| u |  v4(24)   | suffix        |
    //    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    //    |64|     prefix                    | u |   v4(32)      | suffix    |
    //    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    //    |96|     prefix                                    |    v4(32)     |
    //    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+

    constexpr uint8_t kSkipIndex = 8;

    uint8_t ip6Index;

    OT_ASSERT(aPrefix.IsValidNat64());

    Clear();
    SetPrefix(aPrefix);

    ip6Index = aPrefix.GetLength() / CHAR_BIT;

    for (uint8_t i = 0; i < Ip4::Address::kSize; i++)
    {
        if (ip6Index == kSkipIndex)
        {
            ip6Index++;
        }

        mFields.m8[ip6Index++] = aIp4Address.GetBytes()[i];
    }
}

Error Address::FromString(const char *aString)
{
    constexpr uint8_t kInvalidIndex = 0xff;
    constexpr char    kColonChar    = ':';
    constexpr char    kDotChar      = '.';
    constexpr char    kNullChar     = '\0';

    Error   error      = kErrorParse;
    uint8_t index      = 0;
    uint8_t endIndex   = kSize / sizeof(uint16_t);
    uint8_t colonIndex = kInvalidIndex;
    bool    hasIp4     = false;

    // Check if the string starts with "::".

    if (*aString == kColonChar)
    {
        aString++;
        VerifyOrExit(*aString == kColonChar);
        aString++;
        colonIndex = index;
    }

    while (*aString != kNullChar)
    {
        const char *start = aString;
        uint32_t    value = 0;

        // Parse hex number

        while (true)
        {
            char    c = *aString;
            uint8_t digit;

            if (('A' <= c) && (c <= 'F'))
            {
                digit = static_cast<uint8_t>(c - 'A' + 10);
            }
            else if (('a' <= c) && (c <= 'f'))
            {
                digit = static_cast<uint8_t>(c - 'a' + 10);
            }
            else if (('0' <= c) && (c <= '9'))
            {
                digit = static_cast<uint8_t>(c - '0');
            }
            else
            {
                break;
            }

            aString++;
            value = (value << 4) + digit;

            VerifyOrExit(value <= NumericLimits<uint16_t>::kMax);
        }

        VerifyOrExit(aString != start);

        if (*aString == kDotChar)
        {
            // IPv6 address contains an embedded IPv4 address.
            aString = start;
            hasIp4  = true;
            endIndex -= Ip4::Address::kSize / sizeof(uint16_t);
            VerifyOrExit(index <= endIndex);
            break;
        }

        VerifyOrExit((*aString == kColonChar) || (*aString == kNullChar));

        VerifyOrExit(index < endIndex);
        mFields.m16[index++] = HostSwap16(static_cast<uint16_t>(value));

        if (*aString == kColonChar)
        {
            aString++;

            if (*aString == kColonChar)
            {
                VerifyOrExit(colonIndex == kInvalidIndex);
                colonIndex = index;
                aString++;
            }
        }
    }

    if (index < endIndex)
    {
        uint8_t wordsToCopy;

        VerifyOrExit(colonIndex != kInvalidIndex);

        wordsToCopy = index - colonIndex;

        memmove(&mFields.m16[endIndex - wordsToCopy], &mFields.m16[colonIndex], wordsToCopy * sizeof(uint16_t));
        memset(&mFields.m16[colonIndex], 0, (endIndex - index) * sizeof(uint16_t));
    }

    if (hasIp4)
    {
        Ip4::Address ip4Addr;

        SuccessOrExit(error = ip4Addr.FromString(aString));
        memcpy(OT_ARRAY_END(mFields.m8) - Ip4::Address::kSize, ip4Addr.GetBytes(), Ip4::Address::kSize);
    }

    error = kErrorNone;

exit:
    return error;
}

Address::InfoString Address::ToString(void) const
{
    InfoString string;

    ToString(string);

    return string;
}

void Address::ToString(char *aBuffer, uint16_t aSize) const
{
    StringWriter writer(aBuffer, aSize);
    ToString(writer);
}

void Address::ToString(StringWriter &aWriter) const
{
    AppendHexWords(aWriter, OT_ARRAY_LENGTH(mFields.m16));
}

void Address::AppendHexWords(StringWriter &aWriter, uint8_t aLength) const
{
    // Appends the first `aLength` elements in `mFields.m16[]` array
    // as hex words.

    for (uint8_t index = 0; index < aLength; index++)
    {
        if (index > 0)
        {
            aWriter.Append(":");
        }

        aWriter.Append("%x", HostSwap16(mFields.m16[index]));
    }
}

const Address &Address::GetLinkLocalAllNodesMulticast(void)
{
    return static_cast<const Address &>(Netif::kLinkLocalAllNodesMulticastAddress.mAddress);
}

const Address &Address::GetLinkLocalAllRoutersMulticast(void)
{
    return static_cast<const Address &>(Netif::kLinkLocalAllRoutersMulticastAddress.mAddress);
}

const Address &Address::GetRealmLocalAllNodesMulticast(void)
{
    return static_cast<const Address &>(Netif::kRealmLocalAllNodesMulticastAddress.mAddress);
}

const Address &Address::GetRealmLocalAllRoutersMulticast(void)
{
    return static_cast<const Address &>(Netif::kRealmLocalAllRoutersMulticastAddress.mAddress);
}

const Address &Address::GetRealmLocalAllMplForwarders(void)
{
    return static_cast<const Address &>(Netif::kRealmLocalAllMplForwardersMulticastAddress.mAddress);
}

} // namespace Ip6
} // namespace ot
