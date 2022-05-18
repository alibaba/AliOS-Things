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
 *   This file includes definitions for IPv6 addresses.
 */

#ifndef IP6_ADDRESS_HPP_
#define IP6_ADDRESS_HPP_

#include "openthread-core-config.h"

#include <stdint.h>

#include <openthread/ip6.h>

#include "common/clearable.hpp"
#include "common/encoding.hpp"
#include "common/equatable.hpp"
#include "common/string.hpp"
#include "mac/mac_types.hpp"
#include "net/ip4_address.hpp"

using ot::Encoding::BigEndian::HostSwap16;

namespace ot {
namespace Ip6 {

/**
 * @addtogroup core-ip6-ip6
 *
 * @{
 *
 */

/**
 * This class represents the Network Prefix of an IPv6 address (most significant 64 bits of the address).
 *
 */
OT_TOOL_PACKED_BEGIN
class NetworkPrefix : public otIp6NetworkPrefix, public Equatable<NetworkPrefix>, public Clearable<NetworkPrefix>
{
public:
    static constexpr uint8_t kSize   = OT_IP6_PREFIX_SIZE;            ///< Size in bytes.
    static constexpr uint8_t kLength = OT_IP6_PREFIX_SIZE * CHAR_BIT; ///< Length of Network Prefix in bits.

    /**
     * This method generates and sets the Network Prefix to a crypto-secure random Unique Local Address (ULA) based
     * on the pattern `fdxx:xxxx:xxxx:` (RFC 4193).
     *
     * @retval kErrorNone     Successfully generated a random ULA Network Prefix
     * @retval kErrorFailed   Failed to generate random ULA Network Prefix.
     *
     */
    Error GenerateRandomUla(void);

} OT_TOOL_PACKED_END;

/**
 * This class represents an IPv6 Prefix.
 *
 */
OT_TOOL_PACKED_BEGIN
class Prefix : public otIp6Prefix, public Clearable<Prefix>, public Unequatable<Prefix>
{
public:
    static constexpr uint8_t kMaxLength = OT_IP6_ADDRESS_SIZE * CHAR_BIT; ///< Max length of a prefix in bits.
    static constexpr uint8_t kMaxSize   = OT_IP6_ADDRESS_SIZE;            ///< Max (byte) size of a prefix.

    static constexpr uint16_t kInfoStringSize = OT_IP6_PREFIX_STRING_SIZE; ///< Info string size (`ToString()`).

    /**
     * This type defines the fixed-length `String` object returned from `ToString()`.
     *
     */
    typedef String<kInfoStringSize> InfoString;

    /**
     * This method gets the prefix as a pointer to a byte array.
     *
     * @returns A pointer to a byte array containing the Prefix.
     *
     */
    const uint8_t *GetBytes(void) const { return mPrefix.mFields.m8; }

    /**
     * This method gets the prefix length (in bits).
     *
     * @returns The prefix length (in bits).
     *
     */
    uint8_t GetLength(void) const { return mLength; }

    /**
     * This method returns the size (in bytes) of the prefix.
     *
     * @returns The size (in bytes) of the prefix array.
     *
     */
    uint8_t GetBytesSize(void) const { return SizeForLength(mLength); }

    /**
     * This method sets the prefix.
     *
     * @param[in] aPrefix  A pointer to buffer containing the prefix bytes.
     * @param[in] aLength  The length or prefix in bits.
     *
     */
    void Set(const uint8_t *aPrefix, uint8_t aLength);

    /**
     * This method sets the prefix from a given Network Prefix.
     *
     * @param[in] aNetworkPrefix    A Network Prefix.
     *
     */
    void Set(const NetworkPrefix &aNetworkPrefix) { Set(aNetworkPrefix.m8, NetworkPrefix::kLength); }

    /**
     * This method set the prefix length.
     *
     * @param[in] aLength  The new prefix length (in bits).
     *
     */
    void SetLength(uint8_t aLength) { mLength = aLength; }

    /**
     * This method indicates whether prefix length is valid (smaller or equal to max length).
     *
     * @retval TRUE   The prefix length is valid.
     * @retval FALSE  The prefix length is not valid.
     *
     */
    bool IsValid(void) const { return (mLength <= kMaxLength); }

    /**
     * This method indicates whether the prefix is a Link-Local prefix.
     *
     * @retval TRUE   The prefix is a Link-Local prefix.
     * @retval FALSE  The prefix is not a Link-Local prefix.
     *
     */
    bool IsLinkLocal(void) const
    {
        return mLength >= 10 && mPrefix.mFields.m8[0] == 0xfe && (mPrefix.mFields.m8[1] & 0xc0) == 0x80;
    }

    /**
     * This method indicates whether the prefix is a Multicast prefix.
     *
     * @retval TRUE   The prefix is a Multicast prefix.
     * @retval FALSE  The prefix is not a Multicast prefix.
     *
     */
    bool IsMulticast(void) const { return mLength >= 8 && mPrefix.mFields.m8[0] == 0xff; }

    /**
     * This method indicates whether the prefix is a Unique-Local prefix.
     *
     * @retval TRUE   The prefix is a Unique-Local prefix.
     * @retval FALSE  The prefix is not a Unique-Local prefix.
     *
     */
    bool IsUniqueLocal(void) const { return mLength >= 7 && (mPrefix.mFields.m8[0] & 0xfe) == 0xfc; }

    /**
     * This method indicates whether the prefix is equal to a given prefix.
     *
     * @param[in] aPrefixBytes   A pointer to buffer containing the prefix bytes to compare with.
     * @param[in] aPrefixLength  The length of prefix (in bits) specified by @p aPrefixBytes.
     *
     * @retval TRUE   If the prefix is equal to the specified prefix by @p aPrefixBytes and @p aPrefixLength.
     * @retval FALSE  If the prefix is not equal to the specified prefix by @p aPrefixBytes and @p aPrefixLength.
     *
     */
    bool IsEqual(const uint8_t *aPrefixBytes, uint8_t aPrefixLength) const;

    /**
     * This method indicates whether the prefix contains a sub-prefix.
     *
     * @param[in] aSubPrefix  A sub-prefix.
     *
     * @retval TRUE   The prefix contains the @p aSubPrefix
     * @retval FALSE  The prefix does not contains the @p aSubPrefix.
     *
     */
    bool ContainsPrefix(const Prefix &aSubPrefix) const
    {
        return (mLength >= aSubPrefix.mLength) &&
               (MatchLength(GetBytes(), aSubPrefix.GetBytes(), aSubPrefix.GetBytesSize()) >= aSubPrefix.GetLength());
    }

    /**
     * This method indicates whether the prefix contains a sub-prefix (given as a `NetworkPrefix`).
     *
     * @param[in] aSubPrefix  A sub-prefix (as a `NetworkPrefix`).
     *
     * @retval TRUE   The prefix contains the @p aSubPrefix
     * @retval FALSE  The prefix does not contains the @p aSubPrefix.
     *
     */
    bool ContainsPrefix(const NetworkPrefix &aSubPrefix) const
    {
        return (mLength >= NetworkPrefix::kLength) &&
               (MatchLength(GetBytes(), aSubPrefix.m8, NetworkPrefix::kSize) >= NetworkPrefix::kLength);
    }

    /**
     * This method overloads operator `==` to evaluate whether or not two prefixes are equal.
     *
     * @param[in]  aOther  The other prefix to compare with.
     *
     * @retval TRUE   If the two prefixes are equal.
     * @retval FALSE  If the two prefixes are not equal.
     *
     */
    bool operator==(const Prefix &aOther) const
    {
        return (mLength == aOther.mLength) &&
               (MatchLength(GetBytes(), aOther.GetBytes(), GetBytesSize()) >= GetLength());
    }

    /**
     * This method overloads operator `<` to compare two prefixes.
     *
     * A prefix with shorter length is considered smaller than the one with longer length. If the prefix lengths are
     * equal, then the prefix bytes are compared directly.
     *
     * @param[in] aOther  The other prefix to compare against.
     *
     * @retval TRUE   If the prefix is smaller than @p aOther.
     * @retval FALSE  If the prefix is not smaller than @p aOther.
     *
     */
    bool operator<(const Prefix &aOther) const;

    /**
     * This static method converts a prefix length (in bits) to size (number of bytes).
     *
     * @param[in] aLength   A prefix length (in bits).
     *
     * @returns The size (in bytes) of the prefix.
     *
     */
    static uint8_t SizeForLength(uint8_t aLength) { return BitVectorBytes(aLength); }

    /**
     * This static method returns the number of IPv6 prefix bits that match.
     *
     * @param[in]  aPrefixA     A pointer to a byte array containing a first prefix.
     * @param[in]  aPrefixB     A pointer to a byte array containing a second prefix.
     * @param[in]  aMaxSize     Number of bytes of the two prefixes.
     *
     * @returns The number of prefix bits that match.
     *
     */
    static uint8_t MatchLength(const uint8_t *aPrefixA, const uint8_t *aPrefixB, uint8_t aMaxSize);

    /**
     * This method indicates whether or not the prefix has a valid length for use as a NAT64 prefix.
     *
     * A NAT64 prefix must have one of the following lengths: 32, 40, 48, 56, 64, or 96 (per RFC 6502).
     *
     * @retval TRUE   If the prefix has a valid length for use as a NAT64 prefix.
     * @retval FALSE  If the prefix does not have a valid length for use as a NAT64 prefix.
     *
     */
    bool IsValidNat64(void) const;

    /**
     * This method converts the prefix to a string.
     *
     * The IPv6 prefix string is formatted as "%x:%x:%x:...[::]/plen".
     *
     * @returns An `InfoString` containing the string representation of the Prefix.
     *
     */
    InfoString ToString(void) const;

    /**
     * This method converts the prefix to a string.
     *
     * The IPv6 prefix string is formatted as "%x:%x:%x:...[::]/plen".
     *
     * If the resulting string does not fit in @p aBuffer (within its @p aSize characters), the string will be
     * truncated but the outputted string is always null-terminated.
     *
     * @param[out] aBuffer   A pointer to a char array to output the string (MUST NOT be nullptr).
     * @param[in]  aSize     The size of @p aBuffer (in bytes).
     *
     */
    void ToString(char *aBuffer, uint16_t aSize) const;

private:
    void ToString(StringWriter &aWriter) const;
} OT_TOOL_PACKED_END;

/**
 * This class represents the Interface Identifier of an IPv6 address.
 *
 */
OT_TOOL_PACKED_BEGIN
class InterfaceIdentifier : public otIp6InterfaceIdentifier,
                            public Equatable<InterfaceIdentifier>,
                            public Clearable<InterfaceIdentifier>
{
    friend class Address;

public:
    static constexpr uint8_t kSize = OT_IP6_IID_SIZE; ///< Size of an IPv6 Interface Identifier (in bytes).

    static constexpr uint16_t kInfoStringSize = 17; ///< Max chars for the info string (`ToString()`).

    /**
     * This type defines the fixed-length `String` object returned from `ToString()`.
     *
     */
    typedef String<kInfoStringSize> InfoString;

    /**
     * This method indicates whether or not the Interface Identifier is unspecified.
     *
     * @retval true  If the Interface Identifier is unspecified.
     * @retval false If the Interface Identifier is not unspecified.
     *
     */
    bool IsUnspecified(void) const;

    /**
     * This method indicates whether or not the Interface Identifier is reserved (RFC 5453).
     *
     * @retval true  If the Interface Identifier is reserved.
     * @retval false If the Interface Identifier is not reserved.
     *
     */
    bool IsReserved(void) const;

    /**
     * This method indicates whether or not the Interface Identifier is Subnet-Router Anycast (RFC 4291).
     *
     * @retval TRUE   If the Interface Identifier is a Subnet-Router Anycast address.
     * @retval FALSE  If the Interface Identifier is not a Subnet-Router Anycast address.
     *
     */
    bool IsSubnetRouterAnycast(void) const;

    /**
     * This method indicates whether or not the Interface Identifier is Reserved Subnet Anycast (RFC 2526).
     *
     * @retval TRUE   If the Interface Identifier is a Reserved Subnet Anycast address.
     * @retval FALSE  If the Interface Identifier is not a Reserved Subnet Anycast address.
     *
     */
    bool IsReservedSubnetAnycast(void) const;

    /**
     * This method generates and sets the Interface Identifier to a crypto-secure random byte sequence.
     *
     */
    void GenerateRandom(void);

    /**
     * This method gets the Interface Identifier as a pointer to a byte array.
     *
     * @returns A pointer to a byte array (of size `kSize`) containing the Interface Identifier.
     *
     */
    const uint8_t *GetBytes(void) const { return mFields.m8; }

    /**
     * This method sets the Interface Identifier from a given byte array.
     *
     * @param[in] aBuffer    Pointer to an array containing the Interface Identifier. `kSize` bytes from the buffer
     *                       are copied to form the Interface Identifier.
     *
     */
    void SetBytes(const uint8_t *aBuffer);

    /**
     * This method sets the Interface Identifier from a given IEEE 802.15.4 Extended Address.
     *
     * @param[in] aExtAddress  An Extended Address.
     *
     */
    void SetFromExtAddress(const Mac::ExtAddress &aExtAddress);

    /**
     * This method converts the Interface Identifier to an IEEE 802.15.4 Extended Address.
     *
     * @param[out]  aExtAddress  A reference to an Extended Address where the converted address is placed.
     *
     */
    void ConvertToExtAddress(Mac::ExtAddress &aExtAddress) const;

    /**
     * This method converts the Interface Identifier to an IEEE 802.15.4 MAC Address.
     *
     * @param[out]  aMacAddress  A reference to a MAC Address where the converted address is placed.
     *
     */
    void ConvertToMacAddress(Mac::Address &aMacAddress) const;

    /**
     * This method sets the Interface Identifier to Routing/Anycast Locator pattern `0000:00ff:fe00:xxxx` with a given
     * locator (RLOC16 or ALOC16) value.
     *
     * @param[in]  aLocator    RLOC16 or ALOC16.
     *
     */
    void SetToLocator(uint16_t aLocator);

    /**
     * This method indicates whether or not the Interface Identifier matches the locator pattern `0000:00ff:fe00:xxxx`.
     *
     * @retval TRUE   If the IID matches the locator pattern.
     * @retval FALSE  If the IID does not match the locator pattern.
     *
     */
    bool IsLocator(void) const;

    /**
     * This method indicates whether or not the Interface Identifier (IID) matches a Routing Locator (RLOC).
     *
     * In addition to checking that the IID matches the locator pattern (`0000:00ff:fe00:xxxx`), this method also
     * checks that the locator value is a valid RLOC16.
     *
     * @retval TRUE   If the IID matches a RLOC address.
     * @retval FALSE  If the IID does not match a RLOC address.
     *
     */
    bool IsRoutingLocator(void) const;

    /**
     * This method indicates whether or not the Interface Identifier (IID) matches an Anycast Locator (ALOC).
     *
     * In addition to checking that the IID matches the locator pattern (`0000:00ff:fe00:xxxx`), this method also
     * checks that the locator value is any valid ALOC16 (0xfc00 - 0xfcff).
     *
     * @retval TRUE   If the IID matches a ALOC address.
     * @retval FALSE  If the IID does not match a ALOC address.
     *
     */
    bool IsAnycastLocator(void) const;

    /**
     * This method indicates whether or not the Interface Identifier (IID) matches a Service Anycast Locator (ALOC).
     *
     * In addition to checking that the IID matches the locator pattern (`0000:00ff:fe00:xxxx`), this method also
     * checks that the locator value is a valid Service ALOC16 (0xfc10 – 0xfc2f).
     *
     * @retval TRUE   If the IID matches a ALOC address.
     * @retval FALSE  If the IID does not match a ALOC address.
     *
     */
    bool IsAnycastServiceLocator(void) const;

    /**
     * This method gets the Interface Identifier (IID) address locator fields.
     *
     * This method assumes the IID to match the locator pattern `0000:00ff:fe00:xxxx` (does not explicitly check this)
     * and returns the last `uint16` portion of the IID.
     *
     * @returns The RLOC16 or ALOC16.
     *
     */
    uint16_t GetLocator(void) const { return HostSwap16(mFields.m16[3]); }

    /**
     * This method sets the Interface Identifier (IID) address locator field.
     *
     * Unlike `SetToLocator()`, this method only changes the last 2 bytes of the IID and keeps the rest of the address
     * as before.
     *
     * @param[in]  aLocator   RLOC16 or ALOC16.
     *
     */
    void SetLocator(uint16_t aLocator) { mFields.m16[3] = HostSwap16(aLocator); }

    /**
     * This method converts an Interface Identifier to a string.
     *
     * @returns An `InfoString` containing the string representation of the Interface Identifier.
     *
     */
    InfoString ToString(void) const;

private:
    static constexpr uint8_t kAloc16Mask            = 0xfc; // The mask for ALOC16.
    static constexpr uint8_t kRloc16ReservedBitMask = 0x02; // The mask for the reserved bit of RLOC16.

} OT_TOOL_PACKED_END;

/**
 * This class implements an IPv6 address object.
 *
 */
OT_TOOL_PACKED_BEGIN
class Address : public otIp6Address, public Equatable<Address>, public Clearable<Address>
{
    friend class Prefix;

public:
    static constexpr uint8_t kAloc16Mask = InterfaceIdentifier::kAloc16Mask; ///< The mask for ALOC16.

    static constexpr uint8_t kSize = OT_IP6_ADDRESS_SIZE; ///< Size of an IPv6 Address (in bytes).

    static constexpr uint16_t kInfoStringSize = OT_IP6_ADDRESS_STRING_SIZE; ///< String Size for IPv6 address.

    // IPv6 Address Scopes
    static constexpr uint8_t kNodeLocalScope      = 0;  ///< Node-Local scope
    static constexpr uint8_t kInterfaceLocalScope = 1;  ///< Interface-Local scope
    static constexpr uint8_t kLinkLocalScope      = 2;  ///< Link-Local scope
    static constexpr uint8_t kRealmLocalScope     = 3;  ///< Realm-Local scope
    static constexpr uint8_t kAdminLocalScope     = 4;  ///< Admin-Local scope
    static constexpr uint8_t kSiteLocalScope      = 5;  ///< Site-Local scope
    static constexpr uint8_t kOrgLocalScope       = 8;  ///< Organization-Local scope
    static constexpr uint8_t kGlobalScope         = 14; ///< Global scope

    /**
     * This enumeration defines IPv6 address type filter.
     *
     */
    enum TypeFilter : uint8_t
    {
        kTypeAny,                           ///< Accept any IPv6 address (unicast or multicast).
        kTypeUnicast,                       ///< Accept unicast IPv6 addresses only.
        kTypeMulticast,                     ///< Accept multicast IPv6 addresses only.
        kTypeMulticastLargerThanRealmLocal, ///< Accept multicast IPv6 addresses with scope larger than Realm Local.
    };

    /**
     * This type defines the fixed-length `String` object returned from `ToString()`.
     *
     */
    typedef String<kInfoStringSize> InfoString;

    /**
     * This method gets the IPv6 address as a pointer to a byte array.
     *
     * @returns A pointer to a byte array containing the IPv6 address.
     *
     */
    const uint8_t *GetBytes(void) const { return mFields.m8; }

    /**
     * This method sets the IPv6 address from a given byte array.
     *
     * @param[in] aBuffer    Pointer to an array containing the IPv6 address. `kSize` bytes from the buffer
     *                       are copied to form the IPv6 address.
     *
     */
    void SetBytes(const uint8_t *aBuffer) { memcpy(mFields.m8, aBuffer, kSize); }

    /**
     * This method indicates whether or not the IPv6 address is the Unspecified Address.
     *
     * @retval TRUE   If the IPv6 address is the Unspecified Address.
     * @retval FALSE  If the IPv6 address is not the Unspecified Address.
     *
     */
    bool IsUnspecified(void) const;

    /**
     * This method indicates whether or not the IPv6 address is the Loopback Address.
     *
     * @retval TRUE   If the IPv6 address is the Loopback Address.
     * @retval FALSE  If the IPv6 address is not the Loopback Address.
     *
     */
    bool IsLoopback(void) const;

    /**
     * This method indicates whether or not the IPv6 address scope is Link-Local.
     *
     * @retval TRUE   If the IPv6 address scope is Link-Local.
     * @retval FALSE  If the IPv6 address scope is not Link-Local.
     *
     */
    bool IsLinkLocal(void) const;

    /**
     * This methods sets the IPv6 address to a Link-Local address with Interface Identifier generated from a given
     * MAC Extended Address.
     *
     * @param[in]  aExtAddress  A MAC Extended Address (used to generate the IID).
     *
     */
    void SetToLinkLocalAddress(const Mac::ExtAddress &aExtAddress);

    /**
     * This methods sets the IPv6 address to a Link-Local address with a given Interface Identifier.
     *
     * @param[in]  aIid   An Interface Identifier.
     *
     */
    void SetToLinkLocalAddress(const InterfaceIdentifier &aIid);

    /**
     * This method indicates whether or not the IPv6 address is multicast address.
     *
     * @retval TRUE   If the IPv6 address is a multicast address.
     * @retval FALSE  If the IPv6 address scope is not a multicast address.
     *
     */
    bool IsMulticast(void) const { return mFields.m8[0] == 0xff; }

    /**
     * This method indicates whether or not the IPv6 address is a link-local multicast address.
     *
     * @retval TRUE   If the IPv6 address is a link-local multicast address.
     * @retval FALSE  If the IPv6 address scope is not a link-local multicast address.
     *
     */
    bool IsLinkLocalMulticast(void) const;

    /**
     * This method indicates whether or not the IPv6 address is a link-local all nodes multicast address (ff02::01).
     *
     * @retval TRUE   If the IPv6 address is a link-local all nodes multicast address.
     * @retval FALSE  If the IPv6 address is not a link-local all nodes multicast address.
     *
     */
    bool IsLinkLocalAllNodesMulticast(void) const;

    /**
     * This method sets the IPv6 address to the link-local all nodes multicast address (ff02::01).
     *
     */
    void SetToLinkLocalAllNodesMulticast(void);

    /**
     * This method indicates whether or not the IPv6 address is a link-local all routers multicast address (ff02::02).
     *
     * @retval TRUE   If the IPv6 address is a link-local all routers multicast address.
     * @retval FALSE  If the IPv6 address is not a link-local all routers multicast address.
     *
     */
    bool IsLinkLocalAllRoutersMulticast(void) const;

    /**
     * This method sets the IPv6 address to the link-local all routers multicast address (ff02::02).
     *
     */
    void SetToLinkLocalAllRoutersMulticast(void);

    /**
     * This method indicates whether or not the IPv6 address is a realm-local multicast address.
     *
     * @retval TRUE   If the IPv6 address is a realm-local multicast address.
     * @retval FALSE  If the IPv6 address scope is not a realm-local multicast address.
     *
     */
    bool IsRealmLocalMulticast(void) const;

    /**
     * This method indicates whether or not the IPv6 address is a realm-local all nodes multicast address (ff03::01).
     *
     * @retval TRUE   If the IPv6 address is a realm-local all nodes multicast address.
     * @retval FALSE  If the IPv6 address is not a realm-local all nodes multicast address.
     *
     */
    bool IsRealmLocalAllNodesMulticast(void) const;

    /**
     * This method sets the IPv6 address to the realm-local all nodes multicast address (ff03::01)
     *
     */
    void SetToRealmLocalAllNodesMulticast(void);

    /**
     * This method indicates whether or not the IPv6 address is a realm-local all routers multicast address (ff03::02).
     *
     * @retval TRUE   If the IPv6 address is a realm-local all routers multicast address.
     * @retval FALSE  If the IPv6 address is not a realm-local all routers multicast address.
     *
     */
    bool IsRealmLocalAllRoutersMulticast(void) const;

    /**
     * This method sets the IPv6 address to the realm-local all routers multicast address (ff03::02).
     *
     */
    void SetToRealmLocalAllRoutersMulticast(void);

    /**
     * This method indicates whether or not the IPv6 address is a realm-local all MPL forwarders address (ff03::fc).
     *
     * @retval TRUE   If the IPv6 address is a realm-local all MPL forwarders address.
     * @retval FALSE  If the IPv6 address is not a realm-local all MPL forwarders address.
     *
     */
    bool IsRealmLocalAllMplForwarders(void) const;

    /**
     * This method sets the the IPv6 address to the realm-local all MPL forwarders address (ff03::fc).
     *
     */
    void SetToRealmLocalAllMplForwarders(void);

    /**
     * This method indicates whether or not the IPv6 address is multicast larger than realm local.
     *
     * @retval TRUE   If the IPv6 address is multicast larger than realm local.
     * @retval FALSE  If the IPv6 address is not multicast or the scope is not larger than realm local.
     *
     */
    bool IsMulticastLargerThanRealmLocal(void) const;

    /**
     * This method sets the IPv6 address to a Routing Locator (RLOC) IPv6 address with a given Network Prefix and
     * RLOC16 value.
     *
     * @param[in]  aNetworkPrefix    A Network Prefix.
     * @param[in]  aRloc16           A RLOC16 value.
     *
     */
    void SetToRoutingLocator(const NetworkPrefix &aNetworkPrefix, uint16_t aRloc16)
    {
        SetToLocator(aNetworkPrefix, aRloc16);
    }

    /**
     * This method sets the IPv6 address to a Anycast Locator (ALOC) IPv6 address with a given Network Prefix and
     * ALOC16 value.
     *
     * @param[in]  aNetworkPrefix    A Network Prefix.
     * @param[in]  aAloc16           A ALOC16 value.
     *
     */
    void SetToAnycastLocator(const NetworkPrefix &aNetworkPrefix, uint16_t aAloc16)
    {
        SetToLocator(aNetworkPrefix, aAloc16);
    }

    /**
     * This method returns the Network Prefix of the IPv6 address (most significant 64 bits of the address).
     *
     * @returns A reference to the Network Prefix.
     *
     */
    const NetworkPrefix &GetPrefix(void) const
    {
        return static_cast<const NetworkPrefix &>(mFields.mComponents.mNetworkPrefix);
    }

    /**
     * This method indicates whether the IPv6 address matches a given prefix.
     *
     * @param[in] aPrefix  An IPv6 prefix to match with.
     *
     * @retval TRUE   The IPv6 address matches the @p aPrefix.
     * @retval FALSE  The IPv6 address does not match the @p aPrefix.
     *
     */
    bool MatchesPrefix(const Prefix &aPrefix) const;

    /**
     * This method indicates whether the IPv6 address matches a given prefix.
     *
     * @param[in]  aPrefix         A buffer containing the prefix.
     * @param[in]  aPrefixLength   The prefix length (in bits).
     *
     * @retval TRUE   The IPv6 address matches the @p aPrefix.
     * @retval FALSE  The IPv6 address does not match the @p aPrefix.
     *
     */
    bool MatchesPrefix(const uint8_t *aPrefix, uint8_t aPrefixLength) const;

    /**
     * This method sets the IPv6 address prefix.
     *
     * This method only changes the first @p aPrefixLength bits of the address and keeps the rest of the bits in the
     * address as before.
     *
     * @param[in]  aPrefix         A buffer containing the prefix.
     * @param[in]  aPrefixLength   The prefix length (in bits).
     *
     */
    void SetPrefix(const uint8_t *aPrefix, uint8_t aPrefixLength) { SetPrefix(0, aPrefix, aPrefixLength); }

    /**
     * This method sets the IPv6 address prefix to the given Network Prefix.
     *
     * @param[in]  aNetworkPrefix   A Network Prefix.
     *
     */
    void SetPrefix(const NetworkPrefix &aNetworkPrefix);

    /**
     * This method sets the IPv6 address prefix.
     *
     * This method only changes the initial prefix length bits of the IPv6 address and keeps the rest of the bits in
     * the address as before.
     *
     * @param[in]  aPrefix         An IPv6 prefix.
     *
     */
    void SetPrefix(const Prefix &aPrefix);

    /**
     * This method sets the prefix content of the Prefix-Based Multicast Address.
     *
     * @param[in]  aPrefix         A buffer containing the prefix.
     * @param[in]  aPrefixLength   The prefix length (in bits).
     *
     */
    void SetMulticastNetworkPrefix(const uint8_t *aPrefix, uint8_t aPrefixLength);

    /**
     * This method sets the prefix content of Prefix-Based Multicast Address.
     *
     * @param[in]  aNetworkPrefix   A reference to a Network Prefix.
     *
     */
    void SetMulticastNetworkPrefix(const NetworkPrefix &aNetworkPrefix)
    {
        SetMulticastNetworkPrefix(aNetworkPrefix.m8, NetworkPrefix::kLength);
    }

    /**
     * This method sets the prefix content of Prefix-Based Multicast Address.
     *
     * @param[in]  aPrefix  An IPv6 Prefix.
     *
     */
    void SetMulticastNetworkPrefix(const Prefix &aPrefix)
    {
        SetMulticastNetworkPrefix(aPrefix.GetBytes(), aPrefix.GetLength());
    }

    /**
     * This method returns the Interface Identifier of the IPv6 address.
     *
     * @returns A reference to the Interface Identifier.
     *
     */
    const InterfaceIdentifier &GetIid(void) const
    {
        return static_cast<const InterfaceIdentifier &>(mFields.mComponents.mIid);
    }

    /**
     * This method returns the Interface Identifier of the IPv6 address.
     *
     * @returns A reference to the Interface Identifier.
     *
     */
    InterfaceIdentifier &GetIid(void) { return static_cast<InterfaceIdentifier &>(mFields.mComponents.mIid); }

    /**
     * This method sets the Interface Identifier.
     *
     * @param[in]  aIid  An Interface Identifier.
     *
     */
    void SetIid(const InterfaceIdentifier &aIid) { GetIid() = aIid; }

    /**
     * This method returns the IPv6 address scope.
     *
     * @returns The IPv6 address scope.
     *
     */
    uint8_t GetScope(void) const;

    /**
     * This method returns the number of IPv6 prefix bits that match.
     *
     * @param[in]  aOther  The IPv6 address to match against.
     *
     * @returns The number of IPv6 prefix bits that match.
     *
     */
    uint8_t PrefixMatch(const Address &aOther) const;

    /**
     * This method indicates whether address matches a given type filter.
     *
     * @param[in] aFilter   An address type filter.
     *
     * @retval TRUE   The address matches @p aFilter.
     * @retval FALSE  The address does not match @p aFilter.
     *
     */
    bool MatchesFilter(TypeFilter aFilter) const;

    /**
     * This method sets the IPv6 address by performing NAT64 address translation from a given IPv4 address as specified
     * in RFC 6052.
     *
     * The NAT64 @p aPrefix MUST have one of the following lengths: 32, 40, 48, 56, 64, or 96, otherwise the behavior
     * of this method is undefined.
     *
     * @param[in] aPrefix      The prefix to use for IPv4/IPv6 translation.
     * @param[in] aIp4Address  The IPv4 address to translate to IPv6.
     *
     */
    void SynthesizeFromIp4Address(const Prefix &aPrefix, const Ip4::Address &aIp4Address);

    /**
     * This method converts an IPv6 address string to binary.
     *
     * @param[in]  aString  A pointer to the null-terminated string.
     *
     * @retval kErrorNone          Successfully parsed the IPv6 address string.
     * @retval kErrorParse         Failed to parse the IPv6 address string.
     *
     */
    Error FromString(const char *aString);

    /**
     * This method converts the IPv6 address to a string.
     *
     * The IPv6 address string is formatted as 16 hex values separated by ':' (i.e., "%x:%x:%x:...:%x").
     *
     * @returns An `InfoString` representing the IPv6 address.
     *
     */
    InfoString ToString(void) const;

    /**
     * This method convert the IPv6 address to a C string.
     *
     * The IPv6 address string is formatted as 16 hex values separated by ':' (i.e., "%x:%x:%x:...:%x").
     *
     * If the resulting string does not fit in @p aBuffer (within its @p aSize characters), the string will be
     * truncated but the outputted string is always null-terminated.
     *
     * @param[out] aBuffer   A pointer to a char array to output the string (MUST NOT be nullptr).
     * @param[in]  aSize     The size of @p aBuffer (in bytes).
     *
     */
    void ToString(char *aBuffer, uint16_t aSize) const;

    /**
     * This method overloads operator `<` to compare two IPv6 addresses.
     *
     * @param[in] aOther  The other IPv6 address to compare with.
     *
     * @retval true   The IPv6 address is smaller than @p aOther.
     * @retval false  The IPv6 address is larger than or equal to @p aOther.
     *
     */
    bool operator<(const Address &aOther) const { return memcmp(mFields.m8, aOther.mFields.m8, sizeof(Address)) < 0; }

private:
    void SetPrefix(uint8_t aOffset, const uint8_t *aPrefix, uint8_t aPrefixLength);
    void SetToLocator(const NetworkPrefix &aNetworkPrefix, uint16_t aLocator);
    void ToString(StringWriter &aWriter) const;
    void AppendHexWords(StringWriter &aWriter, uint8_t aLength) const;

    static const Address &GetLinkLocalAllNodesMulticast(void);
    static const Address &GetLinkLocalAllRoutersMulticast(void);
    static const Address &GetRealmLocalAllNodesMulticast(void);
    static const Address &GetRealmLocalAllRoutersMulticast(void);
    static const Address &GetRealmLocalAllMplForwarders(void);

    static constexpr uint8_t kMulticastNetworkPrefixLengthOffset = 3; // Prefix-Based Multicast Address (RFC3306).
    static constexpr uint8_t kMulticastNetworkPrefixOffset       = 4; // Prefix-Based Multicast Address (RFC3306).
} OT_TOOL_PACKED_END;

/**
 * @}
 *
 */

} // namespace Ip6
} // namespace ot

#endif // IP6_ADDRESS_HPP_
