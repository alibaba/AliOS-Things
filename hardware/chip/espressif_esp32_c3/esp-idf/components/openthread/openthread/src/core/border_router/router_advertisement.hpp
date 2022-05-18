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
 *   This file includes definitions for IPv6 Router Advertisement.
 *
 * See RFC 4861: Neighbor Discovery for IP version 6 (https://tools.ietf.org/html/rfc4861).
 *
 */

#ifndef ROUTER_ADVERTISEMENT_HPP_
#define ROUTER_ADVERTISEMENT_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE

#include <stdint.h>

#include <openthread/netdata.h>
#include <openthread/platform/toolchain.h>

#include "common/encoding.hpp"
#include "common/equatable.hpp"
#include "net/icmp6.hpp"
#include "net/ip6.hpp"
#include "thread/network_data_types.hpp"

using ot::Encoding::BigEndian::HostSwap16;
using ot::Encoding::BigEndian::HostSwap32;

namespace ot {

namespace BorderRouter {

namespace RouterAdv {

/**
 * This class represents the variable length options in Neighbor
 * Discovery messages.
 *
 * @sa PrefixInfoOption
 * @sa RouteInfoOption
 *
 */
OT_TOOL_PACKED_BEGIN
class Option
{
public:
    enum class Type : uint8_t
    {
        kPrefixInfo = 3,  ///< Prefix Information Option.
        kRouteInfo  = 24, ///< Route Information Option.
    };

    static constexpr uint8_t kLengthUnit = 8; ///< The unit of length in octets.

    /**
     * This constructor initializes the option with given type and length.
     *
     * @param[in]  aType    The type of this option.
     * @param[in]  aLength  The length of this option in unit of 8 octets.
     *
     */
    explicit Option(Type aType, uint8_t aLength = 0)
        : mType(aType)
        , mLength(aLength)
    {
    }

    /**
     * This method returns the type of this option.
     *
     * @returns  The option type.
     *
     */
    Type GetType(void) const { return mType; }

    /**
     * This method sets the size of the option (in bytes).
     *
     * Since the option must end on their natural 64-bits boundaries,
     * the actual length set to the option is padded to (aSize + 7) / 8 * 8.
     *
     * @param[in]  aSize  The size of the option in unit of 1 byte.
     *
     */
    void SetSize(uint16_t aSize) { mLength = static_cast<uint8_t>((aSize + kLengthUnit - 1) / kLengthUnit); }

    /**
     * This method returns the size of the option (in bytes).
     *
     * @returns  The size of the option in unit of 1 byte.
     *
     */
    uint16_t GetSize(void) const { return mLength * kLengthUnit; }

    /**
     * This method sets the length of the option (in unit of 8 bytes).
     *
     * @param[in]  aLength  The length of the option in unit of 8 bytes.
     *
     */
    void SetLength(uint8_t aLength) { mLength = aLength; }

    /**
     * This method returns the length of the option (in unit of 8 bytes).
     *
     * @returns  The length of the option in unit of 8 bytes.
     *
     */
    uint16_t GetLength(void) const { return mLength; }

    /**
     * This helper method returns a pointer to the next valid option in the buffer.
     *
     * @param[in]  aCurOption     The current option. Use nullptr to get the first option.
     * @param[in]  aBuffer        The buffer within which the options are held.
     * @param[in]  aBufferLength  The length of the buffer.
     *
     * @returns  A pointer to the next option if there are a valid one. Otherwise, nullptr.
     *
     */
    static const Option *GetNextOption(const Option *aCurOption, const uint8_t *aBuffer, uint16_t aBufferLength);

    /**
     * This method tells whether this option is valid.
     *
     * @return  A boolean that indicates whether this option is valid.
     *
     */
    bool IsValid(void) const { return mLength > 0; }

private:
    Type    mType;   // Type of the option.
    uint8_t mLength; // Length of the option in unit of 8 octets,
                     // including the `type` and `length` fields.
} OT_TOOL_PACKED_END;

/**
 * This class represents the Prefix Information Option.
 *
 * See section 4.6.2 of RFC 4861 for definition of this option.
 * https://tools.ietf.org/html/rfc4861#section-4.6.2
 *
 */
OT_TOOL_PACKED_BEGIN
class PrefixInfoOption : public Option
{
public:
    /**
     * This constructor initializes this option with zero prefix
     * length, valid lifetime and preferred lifetime.
     *
     */
    PrefixInfoOption(void);

    /**
     * This method returns the on-link flag.
     *
     * @returns  A boolean which indicates whether the on-link flag is set.
     *
     */
    bool GetOnLink(void) const { return (mReserved1 & kOnLinkFlagMask) != 0; }

    /**
     * This method sets the on-link (L) flag.
     *
     * @param[in]  aOnLink  A boolean indicates whether the prefix is on-link or off-link.
     *
     */
    void SetOnLink(bool aOnLink);

    /**
     * This method returns the autonomous address-configuration (A) flag.
     *
     * @returns  A boolean which indicates whether the A flag is set.
     *
     */
    bool GetAutoAddrConfig(void) const { return (mReserved1 & kAutoConfigFlagMask) != 0; }

    /**
     * This method sets the autonomous address-configuration (A) flag.
     *
     * @param[in]  aAutoAddrConfig  A boolean indicates whether this prefix can be used
     *                              for SLAAC.
     *
     */
    void SetAutoAddrConfig(bool aAutoAddrConfig);

    /**
     * This method set the valid lifetime of the prefix in seconds.
     *
     * @param[in]  aValidLifetime  The valid lifetime in seconds.
     *
     */
    void SetValidLifetime(uint32_t aValidLifetime) { mValidLifetime = HostSwap32(aValidLifetime); }

    /**
     * THis method returns the valid lifetime of the prefix in seconds.
     *
     * @returns  The valid lifetime in seconds.
     *
     */
    uint32_t GetValidLifetime(void) const { return HostSwap32(mValidLifetime); }

    /**
     * This method sets the preferred lifetime of the prefix in seconds.
     *
     * @param[in]  aPreferredLifetime  The preferred lifetime in seconds.
     *
     */
    void SetPreferredLifetime(uint32_t aPreferredLifetime) { mPreferredLifetime = HostSwap32(aPreferredLifetime); }

    /**
     * THis method returns the preferred lifetime of the prefix in seconds.
     *
     * @returns  The preferred lifetime in seconds.
     *
     */
    uint32_t GetPreferredLifetime(void) const { return HostSwap32(mPreferredLifetime); }

    /**
     * This method sets the prefix.
     *
     * @param[in]  aPrefix  The prefix contained in this option.
     *
     */
    void SetPrefix(const Ip6::Prefix &aPrefix);

    /**
     * This method returns the prefix in this option.
     *
     * @returns  The IPv6 prefix in this option.
     *
     */
    Ip6::Prefix GetPrefix(void) const;

    /**
     * This method tells whether this option is valid.
     *
     * @returns  A boolean indicates whether this option is valid.
     *
     */
    bool IsValid(void) const
    {
        return (GetSize() == sizeof(*this)) && (mPrefixLength <= OT_IP6_ADDRESS_SIZE * CHAR_BIT) &&
               (GetPreferredLifetime() <= GetValidLifetime());
    }

private:
    static constexpr uint8_t kAutoConfigFlagMask = 0x40; // Bit mask of the Automatic Address Configure flag.
    static constexpr uint8_t kOnLinkFlagMask     = 0x80; // Bit mask of the On-link flag.

    uint8_t      mPrefixLength;      // The prefix length in bits.
    uint8_t      mReserved1;         // The reserved field.
    uint32_t     mValidLifetime;     // The valid lifetime of the prefix.
    uint32_t     mPreferredLifetime; // The preferred lifetime of the prefix.
    uint32_t     mReserved2;         // The reserved field.
    Ip6::Address mPrefix;            // The prefix.
} OT_TOOL_PACKED_END;

static_assert(sizeof(PrefixInfoOption) == 32, "invalid PrefixInfoOption structure");

/**
 * This class represents the Route Information Option.
 *
 * See section 2.3 of RFC 4191 for definition of this option.
 * https://tools.ietf.org/html/rfc4191#section-2.3
 *
 */
OT_TOOL_PACKED_BEGIN
class RouteInfoOption : public Option
{
public:
    /**
     * This type represents a route preference.
     *
     */
    typedef NetworkData::RoutePreference RoutePreference;

    /**
     * This constructor initializes this option with zero prefix length.
     *
     */
    RouteInfoOption(void);

    /**
     * This method sets the route preference.
     *
     * @param[in]  aPreference  The route preference.
     *
     */
    void SetPreference(RoutePreference aPreference);

    /**
     * This method returns the route preference.
     *
     * @returns  The route preference.
     *
     */
    RoutePreference GetPreference(void) const;

    /**
     * This method sets the lifetime of the route in seconds.
     *
     * @param[in]  aLifetime  The lifetime of the route in seconds.
     *
     */
    void SetRouteLifetime(uint32_t aLifetime) { mRouteLifetime = HostSwap32(aLifetime); }

    /**
     * This method returns Route Lifetime in seconds.
     *
     * @returns  The Route Lifetime in seconds.
     *
     */
    uint32_t GetRouteLifetime(void) const { return HostSwap32(mRouteLifetime); }

    /**
     * This method sets the prefix.
     *
     * @param[in]  aPrefix  The prefix contained in this option.
     *
     */
    void SetPrefix(const Ip6::Prefix &aPrefix);

    /**
     * This method returns the prefix in this option.
     *
     * @returns  The IPv6 prefix in this option.
     *
     */
    Ip6::Prefix GetPrefix(void) const;

    /**
     * This method tells whether this option is valid.
     *
     * @returns  A boolean indicates whether this option is valid.
     *
     */
    bool IsValid(void) const;

private:
    static constexpr uint8_t kPreferenceOffset = 3;
    static constexpr uint8_t kPreferenceMask   = 3 << kPreferenceOffset;

    uint8_t      mPrefixLength;  // The prefix length in bits.
    uint8_t      mReserved;      // The reserved field.
    uint32_t     mRouteLifetime; // The lifetime in seconds.
    Ip6::Address mPrefix;        // The prefix.
} OT_TOOL_PACKED_END;

static_assert(sizeof(RouteInfoOption) == 24, "invalid RouteInfoOption structure");

/**
 * This class implements the Router Advertisement message.
 *
 * See section 4.2 of RFC 4861 for definition of this message.
 * https://tools.ietf.org/html/rfc4861#section-4.2
 *
 */
OT_TOOL_PACKED_BEGIN
class RouterAdvMessage : public Unequatable<RouterAdvMessage>
{
public:
    /**
     * This constructor initializes the Router Advertisement message with
     * zero router lifetime, reachable time and retransmission timer.
     *
     */
    RouterAdvMessage(void) { SetToDefault(); }

    /**
     * This method sets the RA message to default values.
     *
     */
    void SetToDefault(void);

    /**
     * This method sets the checksum value.
     *
     * @param[in]  aChecksum  The checksum value.
     *
     */
    void SetChecksum(uint16_t aChecksum) { mHeader.SetChecksum(aChecksum); }

    /**
     * This method sets the Router Lifetime in seconds.
     *
     * Zero Router Lifetime means we are not a default router.
     *
     * @param[in]  aRouterLifetime  The router lifetime in seconds.
     *
     */
    void SetRouterLifetime(uint16_t aRouterLifetime)
    {
        mHeader.mData.m16[kRouteLifetimeIdx] = HostSwap16(aRouterLifetime);
    }

    /**
     * This method returns the Router Lifetime.
     *
     * Zero Router Lifetime means we are not a default router.
     *
     * @returns  The router lifetime in seconds.
     *
     */
    uint16_t GetRouterLifetime(void) const { return HostSwap16(mHeader.mData.m16[kRouteLifetimeIdx]); }

    /**
     * This method returns the Managed Address Configuration ('m') flag.
     *
     * @returns  A boolean which indicates whether the 'm' flag is set.
     *
     */
    bool GetManagedAddrConfig(void) const { return (mHeader.mData.m8[kReservedIdx] & kManagedAddressConfigMask) != 0; }

    /**
     * This method overloads the assignment operator.
     *
     */
    const RouterAdvMessage &operator=(const RouterAdvMessage &aOther);

    /**
     * This method overloads operator `==` to evaluate whether or not
     * two instances of `RouterAdvMessage` are equal.
     *
     * @param[in]  aOther  The other `RouterAdvMessage` instance to compare with.
     *
     * @retval TRUE   If the two `RouterAdvMessage` instances are equal.
     * @retval FALSE  If the two `RouterAdvMessage` instances are not equal.
     *
     */
    bool operator==(const RouterAdvMessage &aOther) const;

private:
    // The index of Route Lifetime in ICMPv6 Header Data. In unit of 2 octets.
    static constexpr uint8_t kRouteLifetimeIdx = 1;

    // The index of Reserved byte in ICMPv6 Header Data. In unit of 1 octet.
    static constexpr uint8_t kReservedIdx = 1;

    // The bitmask of the Managed Address Configuration ('m') flag.
    static constexpr uint8_t kManagedAddressConfigMask = 0x80;

    Ip6::Icmp::Header mHeader;        // The common ICMPv6 header.
    uint32_t          mReachableTime; // The reachable time. In milliseconds.
    uint32_t          mRetransTimer;  // The retransmission timer. In milliseconds.
} OT_TOOL_PACKED_END;

static_assert(sizeof(RouterAdvMessage) == 16, "invalid RouterAdvMessage structure");

/**
 * This class implements the Router Solicitation message.
 *
 * See section 4.1 of RFC 4861 for definition of this message.
 * https://tools.ietf.org/html/rfc4861#section-4.1
 *
 */
OT_TOOL_PACKED_BEGIN
class RouterSolicitMessage
{
public:
    /**
     * This constructor initializes the Router Solicitation message.
     *
     */
    RouterSolicitMessage(void);

private:
    Ip6::Icmp::Header mHeader; // The common ICMPv6 header.
} OT_TOOL_PACKED_END;

static_assert(sizeof(RouterSolicitMessage) == 8, "invalid RouterSolicitMessage structure");

} // namespace RouterAdv

} // namespace BorderRouter

} // namespace ot

#endif // OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE

#endif // ROUTER_ADVERTISEMENT_HPP_
