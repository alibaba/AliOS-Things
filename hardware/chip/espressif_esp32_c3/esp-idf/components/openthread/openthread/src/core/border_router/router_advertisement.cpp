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
 *   This file includes implementations for ICMPv6 Router Advertisement.
 *
 */

#include "border_router/router_advertisement.hpp"

#if OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE

namespace ot {

namespace BorderRouter {

namespace RouterAdv {

const Option *Option::GetNextOption(const Option *aCurOption, const uint8_t *aBuffer, uint16_t aBufferLength)
{
    const uint8_t *nextOption = nullptr;
    const uint8_t *bufferEnd  = aBuffer + aBufferLength;

    VerifyOrExit(aBuffer != nullptr, nextOption = nullptr);

    if (aCurOption == nullptr)
    {
        nextOption = aBuffer;
    }
    else
    {
        nextOption = reinterpret_cast<const uint8_t *>(aCurOption) + aCurOption->GetSize();
    }

    VerifyOrExit(nextOption + sizeof(Option) <= bufferEnd, nextOption = nullptr);
    VerifyOrExit(reinterpret_cast<const Option *>(nextOption)->GetSize() > 0, nextOption = nullptr);
    VerifyOrExit(nextOption + reinterpret_cast<const Option *>(nextOption)->GetSize() <= bufferEnd,
                 nextOption = nullptr);

exit:
    return reinterpret_cast<const Option *>(nextOption);
}

PrefixInfoOption::PrefixInfoOption(void)
    : Option(Type::kPrefixInfo, sizeof(*this) / kLengthUnit)
    , mPrefixLength(0)
    , mReserved1(0)
    , mValidLifetime(0)
    , mPreferredLifetime(0)
    , mReserved2(0)
{
    OT_UNUSED_VARIABLE(mReserved2);

    mPrefix.Clear();
}

void PrefixInfoOption::SetOnLink(bool aOnLink)
{
    if (aOnLink)
    {
        mReserved1 |= kOnLinkFlagMask;
    }
    else
    {
        mReserved1 &= ~kOnLinkFlagMask;
    }
}

void PrefixInfoOption::SetAutoAddrConfig(bool aAutoAddrConfig)
{
    if (aAutoAddrConfig)
    {
        mReserved1 |= kAutoConfigFlagMask;
    }
    else
    {
        mReserved1 &= ~kAutoConfigFlagMask;
    }
}

void PrefixInfoOption::SetPrefix(const Ip6::Prefix &aPrefix)
{
    mPrefixLength = aPrefix.mLength;
    mPrefix       = static_cast<const Ip6::Address &>(aPrefix.mPrefix);
}

Ip6::Prefix PrefixInfoOption::GetPrefix(void) const
{
    Ip6::Prefix prefix;

    prefix.Set(mPrefix.GetBytes(), mPrefixLength);
    return prefix;
}

RouteInfoOption::RouteInfoOption(void)
    : Option(Type::kRouteInfo, 0)
    , mPrefixLength(0)
    , mReserved(0)
    , mRouteLifetime(0)
{
    OT_UNUSED_VARIABLE(mReserved);

    mPrefix.Clear();
}

void RouteInfoOption::SetPreference(RoutePreference aPreference)
{
    mReserved &= ~kPreferenceMask;
    mReserved |= (NetworkData::RoutePreferenceToValue(aPreference) << kPreferenceOffset) & kPreferenceMask;
}

RouteInfoOption::RoutePreference RouteInfoOption::GetPreference(void) const
{
    return NetworkData::RoutePreferenceFromValue((mReserved & kPreferenceMask) >> kPreferenceOffset);
}

void RouteInfoOption::SetPrefix(const Ip6::Prefix &aPrefix)
{
    // The total length (in bytes) of a Router Information Option
    // is: (8 bytes fixed option header) + (0, 8, or 16 bytes prefix).
    // Because the length of the option must be padded with 8 bytes,
    // the length of the prefix (in bits) must be padded with 64 bits.
    SetLength((aPrefix.mLength + kLengthUnit * CHAR_BIT - 1) / (kLengthUnit * CHAR_BIT) + 1);

    mPrefixLength = aPrefix.mLength;
    mPrefix       = static_cast<const Ip6::Address &>(aPrefix.mPrefix);
}

Ip6::Prefix RouteInfoOption::GetPrefix(void) const
{
    Ip6::Prefix prefix;

    prefix.Set(mPrefix.GetBytes(), mPrefixLength);
    return prefix;
}

bool RouteInfoOption::IsValid(void) const
{
    return (GetLength() == 1 || GetLength() == 2 || GetLength() == 3) &&
           (mPrefixLength <= OT_IP6_ADDRESS_SIZE * CHAR_BIT) && NetworkData::IsRoutePreferenceValid(GetPreference());
}

void RouterAdvMessage::SetToDefault(void)
{
    mHeader.Clear();
    mHeader.SetType(Ip6::Icmp::Header::kTypeRouterAdvert);
    mReachableTime = 0;
    mRetransTimer  = 0;
}

const RouterAdvMessage &RouterAdvMessage::operator=(const RouterAdvMessage &aOther)
{
    mHeader = aOther.mHeader;

    // Set zero value and let platform do the calculation.
    mHeader.SetChecksum(0);

    mReachableTime = aOther.mReachableTime;
    mRetransTimer  = aOther.mRetransTimer;

    return *this;
}

bool RouterAdvMessage::operator==(const RouterAdvMessage &aOther) const
{
    return memcmp(&mHeader.mData, &aOther.mHeader.mData, sizeof(mHeader.mData)) == 0 &&
           mReachableTime == aOther.mReachableTime && mRetransTimer == aOther.mRetransTimer;
}

RouterSolicitMessage::RouterSolicitMessage(void)
{
    mHeader.Clear();
    mHeader.SetType(Ip6::Icmp::Header::kTypeRouterSolicit);
}

} // namespace RouterAdv

} // namespace BorderRouter

} // namespace ot

#endif // OPENTHREAD_CONFIG_BORDER_ROUTING_ENABLE
